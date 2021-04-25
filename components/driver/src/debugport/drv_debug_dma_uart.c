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

#include "drv_debug_port.h"
#include "drv_debug_port_imp.h"
#include "drv_axidma.h"
#include "drv_names.h"
#include "hwregs.h"
#include "osi_trace.h"
#include "osi_fifo.h"
#include <stdlib.h>

/**
 * \brief uart as debugging
 *
 * The driver of debug uart is a simplified uart driver. The configuration
 * is fixed, no parameters and no configurations.
 *
 * System interrupt response is fast enough, and fifo mode rx is enough.
 * RX data will be put to a software fifo before processed (host or diag).
 * The processing is in a low priority thread, and there exists risk of
 * software fifo overflow, but it will only happen at extreme high loading.
 *
 * TX will use dma when trace is enabled. TX fifo mode will introduce too
 * many interrupts. When trace is not enabled, tx will use fifo mode.
 *
 * There is a sleep-loop wait to send packet through trace buffer. It
 * looks dirty, but doesn't hurt.
 *
 * \paragraph low power and sleep
 *
 * It is assumed that memory bandwidth is enough for rx/tx even bus clock
 * is switched to 26MHz. So, the activity won't set clock constraint.
 *
 * TX transfer will prevent system sleep. It will increase power
 * consumption when trace is enabled. And it is *common sense* to turn off
 * trace at power consumption measurement.
 *
 * The timer to pill trace must be relaxed. Otherwise, it will seriously
 * impact power consumption.
 */
#include "osi_log.h"

#define UART_TXFIFO_SIZE (128)
#define UART_RXFIFO_SIZE (128)
#ifdef CONFIG_SOC_8910
#define RXFIFO_BUF_SIZE (8 * 1024)
#else
#define RXFIFO_BUF_SIZE (256)
#endif
#define TRACE_TX_POLL_INTERVAL (20)
#define TRACE_PACKET_TX_TIMEOUT (500)

#define OUTPUT_AT_TXDONE (1)
#define OUTPUT_AT_TIMEOUT (2)
#define OUTPUT_AT_BLUE_SCREEN (3)

// The source IDs should match hardware
#define AD_SOURCE_UART1RX 0
#define AD_SOURCE_UART1TX 1
#define AD_SOURCE_UART2RX 2
#define AD_SOURCE_UART2TX 3
#define AD_SOURCE_UART3RX 4
#define AD_SOURCE_UART3TX 5

typedef struct
{
    drvDebugPort_t port;
    uintptr_t hwp_cfg_clk_uart;
    HWP_ARM_UART_T *hwp;
    osiMutex_t *lock;
    osiPmSource_t *pm_source;
    bool blue_screen_mode;
    uint8_t tx_dma_source_id;
    uint32_t irqn;
    uint32_t irq_priority;
    drvAxidmaCh_t *tx_dma_ch;
    osiBuffer_t trace_buf;
    osiTimer_t *tx_timer;
    osiCallback_t rx_cb;
    void *rx_cb_ctx;
    osiFifo_t rx_fifo;
} drvDebugUartPort_t;

/**
 * Read from fifo, return read bytes.
 */
static int prvUartFifoRead(HWP_ARM_UART_T *hwp, void *data, unsigned size)
{
    REG_ARM_UART_UART_RXFIFO_STAT_T uart_rxfifo_stat = {hwp->uart_rxfifo_stat};
    int bytes = uart_rxfifo_stat.b.rx_fifo_cnt;
    if (bytes == UART_RXFIFO_SIZE)
        OSI_LOGI(0, "debug uart Rx overflow, size %d",bytes);
    if (bytes > size)
        bytes = size;

    uint8_t *data8 = (uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        *data8++ = hwp->uart_rx;
    return bytes;
}

/**
 * Write to fifo, return written bytes.
 */
static int prvUartFifoWrite(HWP_ARM_UART_T *hwp, const void *data, unsigned size)
{
    REG_ARM_UART_UART_TXFIFO_STAT_T uart_txfifo_stat = {hwp->uart_txfifo_stat};
    uint32_t tx_fifo_cnt = uart_txfifo_stat.b.tx_fifo_cnt;
    int bytes = tx_fifo_cnt < UART_TXFIFO_SIZE
                    ? UART_TXFIFO_SIZE - tx_fifo_cnt
                    : 0;
    if (bytes > size)
        bytes = size;

    const uint8_t *data8 = (const uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        hwp->uart_tx = *data8++;
    return bytes;
}

/**
 * Loop to write all to fifo, always return true
 */
static bool prvUartFifoWriteAll(HWP_ARM_UART_T *hwp, const void *packet, unsigned packet_len)
{
    while (packet_len > 0)
    {
        int bytes = prvUartFifoWrite(hwp, packet, packet_len);
        if (bytes > 0)
        {
            packet = (const char *)packet + bytes;
            packet_len -= bytes;
        }
    }
    return true;
}

/**
 * Start the tx timer
 */
static void prvUartStartTxTimer(drvDebugUartPort_t *d)
{
    osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Start tx dma
 */
static void prvUartStartTxDma(drvDebugUartPort_t *d, const void *data, size_t size)
{
    osiDCacheClean(data, size);
    drvAxidmaCfg_t cfg = {};
    cfg.src_addr = (const uint32_t)data;
    cfg.dst_addr = (uint32_t)&d->hwp->uart_tx;
    cfg.data_size = size;
    cfg.data_type = AD_DATA_8BIT;
    cfg.part_trans_size = 16; //?
    cfg.dst_addr_fix = 1;
    cfg.sync_irq = 1;
    cfg.mask = AD_EVT_FINISH;
    drvAxidmaChStart(d->tx_dma_ch, &cfg);

    osiPmWakeLock(d->pm_source); // must be created
}

/**
 * Trace output, various modes
 */
static void prvUartTraceOutput(drvDebugUartPort_t *d, unsigned whence)
{
    if (!d->port.mode.trace_enable)
        return;

    if (whence == OUTPUT_AT_TXDONE)
    {
        if (d->tx_dma_ch == NULL)
            return;

        if (d->trace_buf.size != 0)
            osiTraceBufHandled();

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            osiPmWakeUnlock(d->pm_source); // must be created
            prvUartStartTxTimer(d);
            return;
        }

        prvUartStartTxDma(d, (void *)d->trace_buf.ptr, d->trace_buf.size);
        osiTimerStop(d->tx_timer);
    }
    else if (whence == OUTPUT_AT_TIMEOUT)
    {
        if (d->tx_dma_ch == NULL)
            return;

        if (d->trace_buf.size != 0) // in transfer
            return;

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size == 0)
        {
            prvUartStartTxTimer(d);
            return;
        }

        prvUartStartTxDma(d, (void *)d->trace_buf.ptr, d->trace_buf.size);
    }
    else if (whence == OUTPUT_AT_BLUE_SCREEN)
    {
        osiBuffer_t buf = osiTraceBufFetch();
        if (buf.size != 0)
        {
            prvUartFifoWriteAll(d->hwp, (void *)buf.ptr, buf.size);
            osiTraceBufHandled();
        }
    }
}

/**
 * TX dma isr, called in dma interrupt.
 */
static void prvUartTxDmaISR(drvAxidmaIrqEvent_t devt, void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;

    if (!(devt & AD_EVT_FINISH))
        return;

    prvUartTraceOutput(d, OUTPUT_AT_TXDONE);
}

/**
 * UART rx isr, called in uart interrupt
 */
static void prvUartRxISR(drvDebugUartPort_t *d)
{
    uint8_t buf[UART_RXFIFO_SIZE];
    int rbytes = prvUartFifoRead(d->hwp, buf, UART_RXFIFO_SIZE);
    if (rbytes > 0)
    {
        osiFifoPut(&d->rx_fifo, buf, rbytes);
        d->rx_cb(d->rx_cb_ctx);
    }
}

/**
 * UART isr
 */
static void prvUartISR(void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;

    REG_ARM_UART_UART_STATUS_T status = {d->hwp->uart_status};
    d->hwp->uart_status = status.v; // write to clear

    if (status.b.timeout_int || status.b.rx_int)
        prvUartRxISR(d);
}

/**
 * Polling trace in timer ISR
 */
static void prvUartTraceTimeout(void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;
    prvUartTraceOutput(d, OUTPUT_AT_TIMEOUT);
}

/**
 * Initialize uart, blue screen only shall be considered
 */
static void prvUartInit(drvDebugUartPort_t *d)
{
    // not to use pll
    REG_SYS_CTRL_CFG_CLK_UART_T cfg_clk_uart = {OSI_REG32_READ(d->hwp_cfg_clk_uart)};
    cfg_clk_uart.b.uart_sel_pll = 0;
    OSI_REG32_WRITE(d->hwp_cfg_clk_uart, cfg_clk_uart.v);

    REG_ARM_UART_UART_CONF_T uart_conf = {
        .b.check = 0,    // DRV_UART_NO_PARITY
        .b.parity = 0,   // DRV_UART_EVEN_PARITY
        .b.stop_bit = 0, // DRV_UART_STOP_BITS_1
        .b.st_check = 1,
        .b.rx_ie = d->blue_screen_mode ? 0 : 1,
        .b.tx_ie = 0,
        .b.tout_ie = d->blue_screen_mode ? 0 : 1,
        .b.hwfc = 0,
        .b.rx_trig_hwfc = 0,
        .b.tout_hwfc = 0,
        .b.hdlc = 0,
        .b.frm_stp = 1, // ??
        .b.trail = 0,
        .b.txrst = 1,
        .b.rxrst = 1,
        .b.at_parity_en = 0,
        .b.at_parity_sel = 0,
        .b.at_verify_2byte = 0,
        .b.at_div_mode = 0,
        .b.at_enable = 0,
    };
    d->hwp->uart_conf = uart_conf.v;

    REG_ARM_UART_UART_DELAY_T uart_delay = {
        .b.toutcnt = 40,
        .b.two_tx_delay = 0,
    };
    d->hwp->uart_delay = uart_delay.v;

    REG_ARM_UART_UART_RXTRIG_T uart_rxtrig = {.b.rx_trig = 32};
    d->hwp->uart_rxtrig = uart_rxtrig.v;

    REG_ARM_UART_UART_TXTRIG_T uart_txtrig = {.b.tx_trig = 0};
    d->hwp->uart_txtrig = uart_txtrig.v;

    d->hwp->uart_baud = 0x60003; // for 921600 baud rate

    REG_WAIT_FIELD_EQZ(uart_conf, d->hwp->uart_conf, txrst);
    REG_WAIT_FIELD_EQZ(uart_conf, d->hwp->uart_conf, rxrst);
}

/**
 * Send packet inside mutex.
 */
static bool prvUartSendPacketLocked(drvDebugUartPort_t *d, const void *data, unsigned size)
{
    if (!d->port.mode.trace_enable)
        return prvUartFifoWriteAll(d->hwp, data, size);

    if (!osiTraceBufPutPacket(data, size))
        return false;

    unsigned critical = osiEnterCritical();
    prvUartTraceOutput(d, OUTPUT_AT_TIMEOUT);
    osiExitCritical(critical);

    return OSI_LOOP_WAIT_POST_TIMEOUT_US(osiTraceBufPacketFinished(data),
                                         TRACE_PACKET_TX_TIMEOUT * 1000,
                                         osiThreadSleepUS(2000));
}

/**
 * Send out host/diag command result packet.
 */
static bool prvUartSendPacket(drvDebugPort_t *p, const void *data, unsigned size)
{
    drvDebugUartPort_t *d = OSI_CONTAINER_OF(p, drvDebugUartPort_t, port);

    if (d->blue_screen_mode)
        return prvUartFifoWriteAll(d->hwp, data, size);

    osiMutexLock(d->lock);
    bool ok = prvUartSendPacketLocked(d, data, size);
    osiMutexUnlock(d->lock);
    return ok;
}

/**
 * Dummy rx callback, do nothing
 */
static void prvDummyRxCallback(void *param) {}

/**
 * Set rx callback
 */
static void prvUartSetRxCallback(drvDebugPort_t *p, osiCallback_t cb, void *param)
{
    drvDebugUartPort_t *d = OSI_CONTAINER_OF(p, drvDebugUartPort_t, port);
    d->rx_cb = (cb == NULL) ? prvDummyRxCallback : cb;
    d->rx_cb_ctx = param;
}

/**
 * Set trace enable or disable
 */
static void prvUartSetTraceEnable(drvDebugPort_t *p, bool enable)
{
    drvDebugUartPort_t *d = OSI_CONTAINER_OF(p, drvDebugUartPort_t, port);
    if (enable)
    {
        uint32_t critical = osiEnterCritical();
        prvUartTraceOutput(d, OUTPUT_AT_TIMEOUT);
        osiExitCritical(critical);
    }
}

/**
 * Read, called in host/diag command thread
 */
static int prvUartRead(drvDebugPort_t *p, void *data, unsigned size)
{
    drvDebugUartPort_t *d = OSI_CONTAINER_OF(p, drvDebugUartPort_t, port);
    return osiFifoGet(&d->rx_fifo, data, size);
}

/**
 * Enter blue screen mode
 */
static void prvUartBsEnter(void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;

    d->blue_screen_mode = true;
    d->rx_cb = prvDummyRxCallback;

    // When TX DMA is enabled, wait TX DMA done
    if (d->tx_dma_ch != NULL)
        OSI_POLL_WAIT(!drvAxidmaChBusy(d->tx_dma_ch));

    if (d->port.mode.bs_only)
    {
        // HACK: For blue screen only, this uart may be used for other
        // purpose in normal mode. It is possible that the DMA is
        // running, and the DMA channel is unknown. So, wait a while
        // and stop all.
        osiDelayUS(200 * 1000);
        drvAxidmaStopAll();
        prvUartInit(d);
    }

    if (d->port.mode.trace_enable)
    {
#ifdef CONFIG_KERNEL_HOST_TRACE
        prvUartFifoWriteAll(d->hwp, gBlueScreenEventData, GDB_EVENT_DATA_SIZE);
#endif
    }
}

/**
 * Polling in blue screen mode
 */
static void prvUartBsPoll(void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;

    if (d->port.mode.cmd_enable)
    {
        uint8_t buf[UART_RXFIFO_SIZE];
        int rbytes = prvUartFifoRead(d->hwp, buf, UART_RXFIFO_SIZE);
        if (rbytes > 0)
            osiFifoPut(&d->rx_fifo, buf, rbytes);
    }

    prvUartTraceOutput(d, OUTPUT_AT_BLUE_SCREEN);
}

/**
 * PM resume, restore uart configuration
 */
static void prvUartPmResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    if (source & OSI_RESUME_ABORT)
        return;

    drvDebugUartPort_t *d = (drvDebugUartPort_t *)ctx;
    prvUartInit(d);
}

/**
 * PM suspend, nothing to be done
 */
static void prvUartPmSuspend(void *ctx, osiSuspendMode_t mode)
{
}

static const drvDebugPortOps_t gDuartPortOps = {
    .send_packet = prvUartSendPacket,
    .set_rx_callback = prvUartSetRxCallback,
    .set_trace_enable = prvUartSetTraceEnable,
    .read = prvUartRead,
};

static const osiPmSourceOps_t gDuartPmOps = {
    .suspend = prvUartPmSuspend,
    .resume = prvUartPmResume,
};

/**
 * Create debug port on uart
 */
drvDebugPort_t *drvDebugUartPortCreate(unsigned name, drvDebugPortMode_t mode)
{
    if (mode.bs_only && mode.trace_enable)
        return NULL; // BS only can't support trace

    unsigned mem_size = sizeof(drvDebugUartPort_t) + RXFIFO_BUF_SIZE;
    uintptr_t mem = (uintptr_t)calloc(1, mem_size);
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)OSI_PTR_INCR_POST(mem, sizeof(drvDebugUartPort_t));
    void *rxfifo_mem = (void *)OSI_PTR_INCR_POST(mem, RXFIFO_BUF_SIZE);

    d->port.ops = &gDuartPortOps;
    d->port.name = name;
    d->port.mode = mode;
    d->rx_cb = prvDummyRxCallback;
    d->lock = osiMutexCreate();
    osiFifoInit(&d->rx_fifo, rxfifo_mem, RXFIFO_BUF_SIZE);

    switch (name)
    {
    case DRV_NAME_UART1:
        d->hwp = hwp_uart1;
        d->tx_dma_source_id = AD_SOURCE_UART1TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_1);
        d->irq_priority = SYS_IRQ_PRIO_UART_1;
        d->hwp_cfg_clk_uart = (uintptr_t)&hwp_sysCtrl->cfg_clk_uart[1];
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_1);
        break;
    case DRV_NAME_UART2:
        d->hwp = hwp_uart2;
        d->tx_dma_source_id = AD_SOURCE_UART2TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_2);
        d->irq_priority = SYS_IRQ_PRIO_UART_2;
        d->hwp_cfg_clk_uart = (uintptr_t)&hwp_sysCtrl->cfg_clk_uart[2];
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_2);
        break;
    case DRV_NAME_UART3:
        d->hwp = hwp_uart3;
        d->tx_dma_source_id = AD_SOURCE_UART3TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_3);
        d->irq_priority = SYS_IRQ_PRIO_UART_3;
        d->hwp_cfg_clk_uart = (uintptr_t)&hwp_sysCtrl->cfg_clk_uart[3];
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_3);
        break;
    }

    if (d->hwp == NULL)
        goto failed;

    if (!d->port.mode.bs_only)
    {
        prvUartInit(d);

        if (d->port.mode.trace_enable)
        {
            d->tx_dma_ch = drvAxidmaChAllocate();
            drvAxidmaChSetDmamap(d->tx_dma_ch, d->tx_dma_source_id, d->tx_dma_source_id);
            drvAxidmaChRegisterIsr(d->tx_dma_ch, prvUartTxDmaISR, d);
        }

        osiIrqSetHandler(d->irqn, prvUartISR, d);
        osiIrqSetPriority(d->irqn, d->irq_priority);
        osiIrqEnable(d->irqn);
    }

    if (d->port.mode.trace_enable)
    {
        d->pm_source = osiPmSourceCreate(name, &gDuartPmOps, d);
        d->tx_timer = osiTimerCreate(NULL, prvUartTraceTimeout, d);
        osiTraceSetDebugPort((drvDebugPort_t *)d);
    }

    if (d->port.mode.bs_enable)
        osiRegisterBlueScreenHandler(prvUartBsEnter, prvUartBsPoll, d);

    prvUartStartTxTimer(d);
    drvDebugPortRegister(&d->port);
    return &d->port;

failed:
    free(d);
    return NULL;
}
