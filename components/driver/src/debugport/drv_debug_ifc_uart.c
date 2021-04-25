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
#include "drv_names.h"
#include "drv_ifc.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "osi_trace.h"
#include "osi_fifo.h"
#include <stdlib.h>

#include "uart/drv_ifc_uart.h"

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

#define RX_DMA_SIZE (256)
#define RX_FIFO_BUF_SIZE (512)
#define TRACE_TX_POLL_INTERVAL (20)
#define TRACE_PACKET_TX_TIMEOUT (500)
#define UART_BAUD (921600)

#define OUTPUT_AT_TXDONE (1)
#define OUTPUT_AT_TIMEOUT (2)
#define OUTPUT_AT_BLUE_SCREEN (3)

typedef struct
{
    drvDebugPort_t port;
    HWP_UART_T *hwp;
    osiMutex_t *lock;
    bool blue_screen_mode;
    bool uart_is_lp;
    unsigned uart_divider;
    unsigned uart_lp_divider;
    osiCallback_t rx_cb;
    void *rx_cb_ctx;
    drvIfcChannel_t rx_ifc;
    osiFifo_t rx_fifo;
    void *rx_dma_buf;
    drvIfcChannel_t tx_ifc;
    osiBuffer_t trace_buf;
    osiTimer_t *tx_timer;
    osiPmSource_t *pm_source;
} drvDebugUartPort_t;

/**
 * Start tx timer
 */
static void prvUartStartTxTimer(drvDebugUartPort_t *d)
{
    osiTimerStartRelaxed(d->tx_timer, TRACE_TX_POLL_INTERVAL, OSI_DELAY_MAX);
}

/**
 * Start tx dma
 */
static void prvUartStartTxDma(drvDebugUartPort_t *d, const void *data, unsigned size)
{
    osiDCacheClean(data, size);
    drvIfcStart(&d->tx_ifc, data, size);
    osiPmWakeLock(d->pm_source);
}

/**
 * Start rx dma
 */
static void prvUartStartRxDma(drvDebugUartPort_t *d)
{
    osiDCacheInvalidate(d->rx_dma_buf, RX_DMA_SIZE);
    drvIfcStart(&d->rx_ifc, d->rx_dma_buf, RX_DMA_SIZE);
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
        if (d->trace_buf.size != 0)
            osiTraceBufHandled();

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size != 0)
        {
            prvUartStartTxDma(d, (void *)d->trace_buf.ptr, d->trace_buf.size);
            osiTimerStop(d->tx_timer);
        }
        else
        {
            osiPmWakeUnlock(d->pm_source);
            prvUartStartTxTimer(d);
        }
    }
    else if (whence == OUTPUT_AT_TIMEOUT)
    {
        if (d->trace_buf.size != 0) // in transfer
            return;

        d->trace_buf = osiTraceBufFetch();
        if (d->trace_buf.size != 0)
        {
            prvUartStartTxDma(d, (void *)d->trace_buf.ptr, d->trace_buf.size);
            osiTimerStop(d->tx_timer);
        }
        else
        {
            osiPmWakeUnlock(d->pm_source); // must be created
            prvUartStartTxTimer(d);
        }
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
 * UART isr
 */
static void prvUartISR(void *param)
{
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)param;

    REG_UART_IRQ_CAUSE_T irq_cause = {d->hwp->irq_cause};
    d->hwp->irq_cause = irq_cause.v; // write to clear

    if (irq_cause.b.rx_dma_done || irq_cause.b.rx_dma_timeout)
    {
        drvIfcFlush(&d->rx_ifc);
        int bytes = RX_DMA_SIZE - drvIfcGetTC(&d->rx_ifc);
        if (bytes > 0)
        {
            osiDCacheInvalidate(d->rx_dma_buf, bytes);
            osiFifoPut(&d->rx_fifo, d->rx_dma_buf, bytes);
            prvUartStartRxDma(d);
            d->rx_cb(d->rx_cb_ctx);
        }
        else
        {
            drvIfcClearFlush(&d->rx_ifc);
        }
    }

    if (irq_cause.b.tx_dma_done)
        prvUartTraceOutput(d, OUTPUT_AT_TXDONE);
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
 * Send out host/diag command result packet, in host/diag command thread.
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
static void prvDuartSetRxCallback(drvDebugPort_t *p, osiCallback_t cb, void *param)
{
    drvDebugUartPort_t *d = OSI_CONTAINER_OF(p, drvDebugUartPort_t, port);
    d->rx_cb = (cb == NULL) ? prvDummyRxCallback : cb;
    d->rx_cb_ctx = param;
}

/**
 * Set trace enable or disable
 */
static void prvDuartSetTraceEnable(drvDebugPort_t *p, bool enable)
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
static int prvDuartRead(drvDebugPort_t *p, void *data, unsigned size)
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

    drvIfcFlush(&d->rx_ifc);
    int bytes = RX_DMA_SIZE - drvIfcGetTC(&d->rx_ifc);
    if (bytes > 0)
    {
        osiDCacheCleanInvalidate(d->rx_dma_buf, bytes);
        osiFifoPut(&d->rx_fifo, d->rx_dma_buf, bytes);
    }
    drvIfcStop(&d->rx_ifc);

    drvIfcWaitDone(&d->tx_ifc);
    if (d->trace_buf.size != 0)
        osiTraceBufHandled();
    d->trace_buf.size = 0;

    if (d->port.mode.trace_enable)
    {
#ifdef CONFIG_KERNEL_HOST_TRACE
        prvUartSendPacket(&d->port, gBlueScreenEventData, GDB_EVENT_DATA_SIZE);
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
        uint8_t buf[UART_RX_FIFO_SIZE];
        int rbytes = prvUartFifoRead(d->hwp, buf, UART_RX_FIFO_SIZE);
        if (rbytes > 0)
            osiFifoPut(&d->rx_fifo, buf, rbytes);
    }

    prvUartTraceOutput(d, OUTPUT_AT_BLUE_SCREEN);
}

/**
 * PM resume, restore uart configuration
 */
static void prvDuartPmResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    // TODO
}

/**
 * PM suspend, nothing to be done
 */
static void prvDuartPmSuspend(void *ctx, osiSuspendMode_t mode)
{
    // TODO
}

static const drvDebugPortOps_t gDuartPortOps = {
    .send_packet = prvUartSendPacket,
    .set_rx_callback = prvDuartSetRxCallback,
    .set_trace_enable = prvDuartSetTraceEnable,
    .read = prvDuartRead,
};

static const osiPmSourceOps_t gUartPmOps = {
    .suspend = prvDuartPmSuspend,
    .resume = prvDuartPmResume,
};

/**
 * Create debug port on uart
 */
drvDebugPort_t *drvDebugUartPortCreate(unsigned name, drvDebugPortMode_t mode)
{
    if (mode.bs_only || !mode.cmd_enable)
        return NULL; // not support

    HWP_UART_T *hwp = prvUartGetHwp(name);
    if (hwp == NULL)
        return NULL;

    unsigned mem_size = sizeof(drvDebugUartPort_t) + RX_DMA_SIZE + RX_FIFO_BUF_SIZE + CONFIG_CACHE_LINE_SIZE;
    uintptr_t mem = (uintptr_t)calloc(1, mem_size);
    drvDebugUartPort_t *d = (drvDebugUartPort_t *)OSI_PTR_INCR_POST(mem, sizeof(drvDebugUartPort_t));
    mem = OSI_ALIGN_UP(mem, CONFIG_CACHE_LINE_SIZE);
    d->rx_dma_buf = (void *)OSI_PTR_INCR_POST(mem, RX_DMA_SIZE);
    void *rxfifo_mem = (void *)OSI_PTR_INCR_POST(mem, RX_FIFO_BUF_SIZE);
    osiFifoInit(&d->rx_fifo, rxfifo_mem, RX_FIFO_BUF_SIZE);

    d->port.ops = &gDuartPortOps;
    d->port.name = name;
    d->port.mode = mode;
    d->hwp = hwp;
    d->rx_cb = prvDummyRxCallback;
    d->lock = osiMutexCreate();
    d->uart_divider = halCalcDivider24(52000000 / 4, UART_BAUD);
    d->uart_lp_divider = halCalcDivider24(26000000 / 4, UART_BAUD);
    d->uart_is_lp = prvUartIsLp(name);
    if (d->uart_is_lp)
    {
        d->uart_divider |= PMUC_SEL_CLK_UART1(2);
        d->uart_lp_divider |= PMUC_SEL_CLK_UART1(3);
    }

    unsigned irqn = prvUartGetIrqn(name);
    unsigned irq_priority = prvUartGetIrqPriority(name);

    prvUartSetBaudDivider(name, d->uart_divider);

    REG_UART_CTRL_T ctrl = {
        .b.enable = 0,
        .b.data_bits = 1,
        .b.tx_stop_bits = 0,
        .b.parity_enable = 0,
        .b.parity_select = 0,
        .b.divisor_mode = 1, // divisor 4,
        .b.dma_mode = 1,
        .b.rx_break_length = 13,
    };
    REG_UART_TRIGGERS_T triggers = {
        .b.rx_trigger = UART_RX_FIFO_SIZE / 2,
        .b.tx_trigger = UART_TX_FIFO_SIZE / 2,
    };
    REG_UART_IRQ_MASK_T irq_mask = {
        .b.rx_dma_done = 1,
        .b.rx_dma_timeout = 1,
        .b.tx_dma_done = 1,
    };
    REG_UART_CMD_SET_T cmd_set = {
        .b.rx_fifo_reset = 1,
        .b.tx_fifo_reset = 1,
    };

    d->hwp->irq_mask = 0;
    d->hwp->triggers = triggers.v;
    d->hwp->ctrl = ctrl.v;

    d->hwp->cmd_set = cmd_set.v;
    d->hwp->cmd_set = cmd_set.v;

    ctrl.b.enable = UART_ENABLE_V_ENABLE;
    d->hwp->ctrl = ctrl.v;

    d->hwp->irq_mask = irq_mask.v;

    d->pm_source = osiPmSourceCreate(name, &gUartPmOps, d);
    drvIfcChannelInit(&d->rx_ifc, name, DRV_IFC_RX);
    drvIfcRequestChannel(&d->rx_ifc);

    if (d->port.mode.trace_enable)
    {
        drvIfcChannelInit(&d->tx_ifc, name, DRV_IFC_TX);
        drvIfcRequestChannel(&d->tx_ifc);
        d->tx_timer = osiTimerCreate(NULL, prvUartTraceTimeout, d);
        osiTraceSetDebugPort((drvDebugPort_t *)d);
    }

    if (d->port.mode.bs_enable)
        osiRegisterBlueScreenHandler(prvUartBsEnter, prvUartBsPoll, d);

    osiIrqSetHandler(irqn, prvUartISR, d);
    osiIrqSetPriority(irqn, irq_priority);
    osiIrqEnable(irqn);

    prvUartStartRxDma(d);
    prvUartStartTxTimer(d);
    drvDebugPortRegister(&d->port);
    return &d->port;
}
