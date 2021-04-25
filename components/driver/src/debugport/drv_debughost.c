/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_debug_port.h"
#include "drv_debug_port_imp.h"
#include "drv_config.h"
#include "drv_ifc.h"
#include "hwregs.h"
#include "hal_hw_map.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_byte_buf.h"
#include "osi_fifo.h"
#include "osi_trace.h"
#include "osi_sysnv.h"
#include <string.h>
#include <assert.h>
#include <errno.h>

/**
 * \brief debughost as debug port
 *
 * Debughost is a special uart with hardware support to handle read
 * write host command (flow id 0xff). This driver will just use the
 * debuguart features.
 *
 * The default baud rate of debughost is 921600. Though it is possible
 * to change baud rate, it will be complex to change baud rate at PC
 * tool when talking to ROM and application. So, use fixed baud now.
 *
 * \paragraph tx blocking
 *
 * Debughost will use software flow control (without software flow
 * control, hardware read write command can't work), maybe it is a
 * reason that TX will be blocked.
 *
 * So, at any time to wait tx, timeout is always needed.
 *
 * \paragraph rx ifc
 *
 * It is a question to use rx ifc or not use rx ifc
 * - When rx ifc is not used, the fifo is 16 bytes, 1.7ms. It is enough
 *   for 8910 to avoid underflow, but it is not enough for 8955.
 * - When rx ifc is used, ifc channel will be always occupied. In most
 *   cases, ifc channel is tight, and may cause others peripherals can't
 *   get ifc channel.
 *
 * By default, rx ifc won't be used.
 *
 * \paragraph low power and sleep
 *
 * TX transfer will prevent system sleep. It will increase power
 * consumption when trace is enabled. And it is *common sense* to turn off
 * trace at power consumption measurement.
 *
 * The timer to pill trace must be relaxed. Otherwise, it will seriously
 * impact power consumption.
 */

// Timeout to wait tx done for host command. This timeout is to handle
// the case that debughost TX is blocked, and avoid infinite wait. It
// should be large enough for normal case.
#define HOST_CMD_TX_TIMEOUT (500)

// Timeout for TX fifo write. This timeout is to handle the case that
// debughost TX is blocked, and avoid infinite wait. The timeout value
// is the time there are no TX room.
#define TX_FIFO_WRITE_TIMEOUT_US (200)

// Timer interval to check new trace
#define TRACE_TX_POLL_INTERVAL (20)

#define OUTPUT_AT_TXDONE (1)
#define OUTPUT_AT_TIMEOUT (2)
#define OUTPUT_AT_BLUE_SCREEN (3)

#ifdef CONFIG_SOC_8910
#define IRQN_DEBUG_UART HAL_SYSIRQ_NUM(SYS_IRQ_ID_DEBUG_UART)
#else
#define IRQN_DEBUG_UART SYS_IRQ_DEBUG_UART
#endif

// MIPS should use OSI_KSEG1
#ifdef CONFIG_CPU_MIPS
#define DHOST_DCACHE_INV(address, size)
#define DHOST_DCACHE_CLEAN(address, size)
#else
#define DHOST_DCACHE_INV(address, size) osiDCacheInvalidate(address, size)
#define DHOST_DCACHE_CLEAN(address, size) osiDCacheClean(address, size)
#endif

typedef struct
{
    drvDebugPort_t port;
    bool blue_screen_mode;
    uint8_t rx_buf[CONFIG_DEBUGHOST_RX_BUF_SIZE];
    osiFifo_t rx_fifo;
    osiCallback_t rx_cb;
    void *rx_cb_ctx;
    drvIfcChannel_t tx_ifc;
    osiBuffer_t trace_buf;
    osiTimer_t *tx_timer;
    osiMutex_t *lock;
    osiPmSource_t *pm_source;
    unsigned ctrl_val;
    unsigned irq_mask_val;
    unsigned triggers_val;
} drvDhostPort_t;

static drvDhostPort_t gDhostPort;

/**
 * Read from fifo, return read bytes.
 */
static int prvDhostFifoRead(void *data, unsigned size)
{
    REG_DEBUG_UART_STATUS_T status = {hwp_debugUart->status};
    int bytes = status.b.rx_fifo_level;
    if (bytes > size)
        bytes = size;

    uint8_t *data8 = (uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        *data8++ = hwp_debugUart->rxtx_buffer;
    return bytes;
}

/**
 * Write to fifo, return written bytes.
 */
static int prvDhostFifoWrite(const void *data, unsigned size)
{
    REG_DEBUG_UART_STATUS_T status = {hwp_debugUart->status};
    uint32_t tx_fifo_level = status.b.tx_fifo_level;
    if (tx_fifo_level >= DEBUG_UART_TX_FIFO_SIZE)
        return 0;

    int bytes = DEBUG_UART_TX_FIFO_SIZE - tx_fifo_level;
    if (bytes > size)
        bytes = size;

    const uint8_t *data8 = (const uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        hwp_debugUart->rxtx_buffer = *data8++;
    return bytes;
}

/**
 * Loop to write all to fifo, return false on timeout
 */
static bool prvDhostFifoWriteAll(const void *data, unsigned size)
{
    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);

    while (size > 0)
    {
        if (osiElapsedTimeUS(&timer) > TX_FIFO_WRITE_TIMEOUT_US)
            return false;

        int wsize = prvDhostFifoWrite(data, size);
        if (wsize > 0)
        {
            size -= wsize;
            data = (const char *)data + wsize;
            osiElapsedTimerStart(&timer);
        }
    }
    return true;
}

/**
 * Start the tx timer
 */
static void prvDhostStartTxTimer(drvDhostPort_t *d)
{
    osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Trace output, various modes
 */
static void prvDhostTraceOutput(drvDhostPort_t *d, unsigned whence)
{
    if (!d->port.mode.trace_enable)
        return;

    if (whence == OUTPUT_AT_TXDONE)
    {
        if (d->trace_buf.size != 0)
            osiTraceBufHandled();

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            prvDhostStartTxTimer(d);
            return;
        }

        DHOST_DCACHE_CLEAN((void *)d->trace_buf.ptr, d->trace_buf.size);
        drvIfcStart(&d->tx_ifc, (void *)d->trace_buf.ptr, d->trace_buf.size);
        osiTimerStop(d->tx_timer);
    }
    else if (whence == OUTPUT_AT_TIMEOUT)
    {
        if (d->trace_buf.size != 0) // in transfer
            return;

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            prvDhostStartTxTimer(d);
            return;
        }

        DHOST_DCACHE_CLEAN((void *)d->trace_buf.ptr, d->trace_buf.size);
        drvIfcStart(&d->tx_ifc, (void *)d->trace_buf.ptr, d->trace_buf.size);
    }
    else if (whence == OUTPUT_AT_BLUE_SCREEN)
    {
        osiBuffer_t buf = osiTraceBufFetch();
        if (buf.size != 0)
        {
            prvDhostFifoWriteAll((void *)buf.ptr, buf.size);
            osiTraceBufHandled();
        }
    }
}

/**
 * Handle rx interrupts.
 */
static void prvDhostRxISR(drvDhostPort_t *d)
{
    int bytes = 0;

    uint8_t buf[DEBUG_UART_RX_FIFO_SIZE];
    bytes = prvDhostFifoRead(buf, DEBUG_UART_RX_FIFO_SIZE);
    if (bytes > 0)
        osiFifoPut(&d->rx_fifo, buf, bytes);

    if (bytes > 0)
    {
        OSI_LOGD(0, "debughost: get %d bytes", bytes);
        d->rx_cb(d->rx_cb_ctx);
    }
}

/**
 * Debuguart ISR
 */
static void prvDhostISR(void *param)
{
    drvDhostPort_t *d = (drvDhostPort_t *)param;
    REG_DEBUG_UART_IRQ_CAUSE_T cause = {hwp_debugUart->irq_cause};
    hwp_debugUart->irq_cause = cause.v; // clear tx_dma_done, rx_dma_done, rx_dma_timeout

    if (cause.b.tx_dma_done)
        prvDhostTraceOutput(d, OUTPUT_AT_TXDONE);

    if (cause.b.rx_dma_done || cause.b.rx_dma_timeout ||
        cause.b.rx_data_available || cause.b.rx_timeout)
        prvDhostRxISR(d);
}

/**
 * Polling trace in timer ISR
 */
static void prvDhostTraceTimeout(void *param)
{
    drvDhostPort_t *d = (drvDhostPort_t *)param;
    prvDhostTraceOutput(d, OUTPUT_AT_TIMEOUT);
}

/**
 * Send packet inside mutex.
 */
static bool prvDhostSendPacketLocked(drvDhostPort_t *d, const void *data, unsigned size)
{
    if (!d->port.mode.trace_enable)
        return prvDhostFifoWriteAll(data, size);

    if (!osiTraceBufPutPacket(data, size))
        return false;

    unsigned critical = osiEnterCritical();
    prvDhostTraceOutput(d, OUTPUT_AT_TIMEOUT);
    osiExitCritical(critical);

    return OSI_LOOP_WAIT_POST_TIMEOUT_US(osiTraceBufPacketFinished(data),
                                         HOST_CMD_TX_TIMEOUT * 1000,
                                         osiThreadSleepUS(2000));
}

/**
 * Send out host command result packet, in host command thread.
 */
static bool prvDhostSendPacket(drvDebugPort_t *p, const void *packet, unsigned packet_len)
{
    drvDhostPort_t *d = OSI_CONTAINER_OF(p, drvDhostPort_t, port);

    if (d->blue_screen_mode)
        return prvDhostFifoWriteAll(packet, packet_len);

    osiMutexLock(d->lock);
    bool ok = prvDhostSendPacketLocked(d, packet, packet_len);
    osiMutexUnlock(d->lock);
    return ok;
}

/**
 * Read, called in host command thread
 */
static int prvDhostRead(drvDebugPort_t *p, void *data, unsigned size)
{
    drvDhostPort_t *d = OSI_CONTAINER_OF(p, drvDhostPort_t, port);
    return osiFifoGet(&d->rx_fifo, data, size);
}

/**
 * Dummy rx callback, do nothing
 */
static void prvDummyRxCallback(void *param) {}

/**
 * Set rx callback
 */
static void prvDhostSetRxCallback(drvDebugPort_t *p, osiCallback_t cb, void *param)
{
    drvDhostPort_t *d = OSI_CONTAINER_OF(p, drvDhostPort_t, port);
    d->rx_cb = (cb == NULL) ? prvDummyRxCallback : cb;
    d->rx_cb_ctx = param;
}

/**
 * Debughost enter blue screen mode
 */
static void prvDhostBsEnter(void *param)
{
    drvDhostPort_t *d = (drvDhostPort_t *)param;

    d->blue_screen_mode = true;
    d->rx_cb = prvDummyRxCallback;
    if (d->port.mode.trace_enable)
    {
        OSI_LOOP_WAIT_TIMEOUT_US(!drvIfcIsRunning(&d->tx_ifc), HOST_CMD_TX_TIMEOUT * 1000);
        drvIfcStop(&d->tx_ifc);

        if (d->trace_buf.size != 0)
            osiTraceBufHandled();
    }
    osiDebugEvent(GDB_EVENT);
}

/**
 * Debughost polling in blue screen mode
 */
static void prvDhostBsPoll(void *param)
{
    drvDhostPort_t *d = (drvDhostPort_t *)param;

    char read_buf[DEBUG_UART_RX_FIFO_SIZE];
    int rbytes = prvDhostFifoRead(read_buf, DEBUG_UART_RX_FIFO_SIZE);
    if (rbytes > 0)
        osiFifoPut(&d->rx_fifo, read_buf, rbytes);

    prvDhostTraceOutput(d, OUTPUT_AT_BLUE_SCREEN);
}

/**
 * Set trace enable or disable
 */
static void prvDhostSetTraceEnable(drvDebugPort_t *p, bool enable)
{
    drvDhostPort_t *d = OSI_CONTAINER_OF(p, drvDhostPort_t, port);
    if (enable)
    {
        uint32_t critical = osiEnterCritical();
        prvDhostTraceOutput(d, OUTPUT_AT_TIMEOUT);
        osiExitCritical(critical);
    }
}

/**
 * Debughost suspend callback.
 */
static void prvDhostSuspend(void *param, osiSuspendMode_t mode)
{
#ifdef CONFIG_SOC_8811
    drvDhostPort_t *d = &gDhostPort;

    REG_DEBUG_UART_CTRL_T ctrl = {hwp_debugUart->ctrl};
    ctrl.b.dma_mode = 0;
    hwp_debugUart->ctrl = ctrl.v;

    drvIfcReleaseChannel(&d->tx_ifc);
#endif
}

/**
 * Debughost resume callback.
 */
static void prvDhostResume(void *param, osiSuspendMode_t mode, uint32_t source)
{
#ifdef CONFIG_SOC_8811
    drvDhostPort_t *d = &gDhostPort;

    drvDhostRestoreConfig();
    drvIfcRequestChannel(&d->tx_ifc);
#endif
}

static const osiPmSourceOps_t gDhostPmOps = {
    .suspend = prvDhostSuspend,
    .resume = prvDhostResume,
};

static const drvDebugPortOps_t gDhostPortOps = {
    .send_packet = prvDhostSendPacket,
    .set_rx_callback = prvDhostSetRxCallback,
    .set_trace_enable = prvDhostSetTraceEnable,
    .read = prvDhostRead,
};

/**
 * Initialize debughost
 */
drvDebugPort_t *drvDhostCreate(drvDebugPortMode_t mode)
{
    hwp_debugUart->irq_mask = 0;

    REG_DEBUG_UART_TRIGGERS_T triggers = {};
    triggers.b.rx_trigger = 7;
    triggers.b.tx_trigger = 12;
    triggers.b.afc_level = 10;
    hwp_debugUart->triggers = triggers.v;

    REG_DEBUG_UART_CTRL_T ctrl = {};
    ctrl.b.enable = 1;
    ctrl.b.data_bits = DEBUG_UART_DATA_BITS_V_8_BITS;
    ctrl.b.parity_enable = DEBUG_UART_PARITY_ENABLE_V_NO;
    ctrl.b.tx_stop_bits = DEBUG_UART_TX_STOP_BITS_V_1_BIT;
    ctrl.b.tx_break_control = 0;
    ctrl.b.rx_fifo_reset = 1;
    ctrl.b.tx_fifo_reset = 1;
    ctrl.b.dma_mode = 1;
    ctrl.b.swrx_flow_ctrl = 1;
    ctrl.b.swtx_flow_ctrl = 1;
    ctrl.b.backslash_en = 1;
    ctrl.b.tx_finish_n_wait = 0;
    ctrl.b.divisor_mode = 0;
    ctrl.b.irda_enable = 0;
    ctrl.b.rx_rts = 1;
    ctrl.b.auto_flow_control = 1;
    ctrl.b.loop_back_mode = 0;
    ctrl.b.rx_lock_err = 0;
    ctrl.b.hst_txd_oen = DEBUG_UART_HST_TXD_OEN_V_ENABLE;
    ctrl.b.rx_break_length = 11;
    hwp_debugUart->ctrl = ctrl.v;

    drvDhostPort_t *d = &gDhostPort;

    d->port.ops = &gDhostPortOps;
    d->port.name = DRV_NAME_DEBUGUART;
    d->port.mode = mode;
    d->rx_cb = prvDummyRxCallback;
    d->lock = osiMutexCreate();
    d->pm_source = osiPmSourceCreate(DRV_NAME_DEBUGUART, &gDhostPmOps, d);

    osiFifoInit(&d->rx_fifo, d->rx_buf, CONFIG_DEBUGHOST_RX_BUF_SIZE);

    d->tx_timer = osiTimerCreate(NULL, prvDhostTraceTimeout, d);
    if (d->port.mode.trace_enable)
    {
        drvIfcChannelInit(&d->tx_ifc, DRV_NAME_DEBUGUART, DRV_IFC_TX);
        drvIfcRequestChannel(&d->tx_ifc);
    }

    REG_DEBUG_UART_IRQ_MASK_T irq_mask = {
        .b.tx_dma_done = d->port.mode.trace_enable ? 1 : 0,
        .b.rx_data_available = 1,
        .b.rx_timeout = 1,
    };
    hwp_debugUart->irq_mask = irq_mask.v;

    d->ctrl_val = ctrl.v;
    d->irq_mask_val = irq_mask.v;
    d->triggers_val = triggers.v;

    osiIrqSetHandler(IRQN_DEBUG_UART, prvDhostISR, d);
    osiIrqSetPriority(IRQN_DEBUG_UART, SYS_IRQ_PRIO_DEBUG_UART);
    osiIrqEnable(IRQN_DEBUG_UART);

    if (d->port.mode.trace_enable)
        osiTraceSetDebugPort(&d->port);

    osiRegisterBlueScreenHandler(prvDhostBsEnter, prvDhostBsPoll, d);

    prvDhostStartTxTimer(d);
    drvDebugPortRegister(&d->port);
    return &d->port;
}

/**
 * Restore debughost configuration
 */
void drvDhostRestoreConfig(void)
{
    drvDhostPort_t *d = &gDhostPort;
    hwp_debugUart->ctrl = d->ctrl_val;
    hwp_debugUart->irq_mask = d->irq_mask_val;
    hwp_debugUart->triggers = d->triggers_val;
}
