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

#include "osi_log.h"
#include "osi_api.h"
#include "osi_fifo.h"
#include "drv_uart.h"
#include "drv_names.h"
#include "drv_ifc.h"
#include "hwregs.h"
#include "hal_iomux.h"
#include <stdlib.h>
#include <string.h>

#define UART_TX_DMA_SIZE (256)

#ifdef CONFIG_SOC_8909
#define UART_HAS_AUTO_FAIL_INTR
#undef UART_SLEEP_NEED_CHANGE_DIVIDER
#define UART_COUNT (3)
#undef USE_STATIC_TX_BUF
#elif defined(CONFIG_SOC_8955)
#undef UART_HAS_AUTO_FAIL_INTR
#define UART_SLEEP_NEED_CHANGE_DIVIDER
#define UART_COUNT (2)
#define UART_DIV_UPDATE (1 << 24)
#define hwp_uart1 hwp_uart
#define SYS_IRQ_UART1 SYS_IRQ_UART
#define USE_STATIC_TX_BUF
static uint8_t gUart1TxDmaBuf[UART_TX_DMA_SIZE] OSI_SECTION_SRAM_UC_UNINIT;
static uint8_t gUart2TxDmaBuf[UART_TX_DMA_SIZE] OSI_SECTION_SRAM_UC_UNINIT;
#endif

#define UART_DIV_NUM(n) ((n)&0x3ff)
#define UART_DIV_DENOM(n) (((n)&0x3fff) << 10)

#define UART_RX_POLL_PERIOD (15)      // ms
#define UART_AUTO_BAUD_POLL_TIME (10) // ms

enum
{
    UART_ST_CLOSED,
    UART_ST_BAUD_LOCKING,
    UART_ST_OPENED,
    UART_ST_SLEEP
};

typedef struct
{
    uint32_t name;
    HWP_UART_T *hwp;
    volatile uint32_t *reg_cfg_clk_uart;
    uint8_t irqn;
    unsigned iomux_rx;
    unsigned iomux_tx;
    unsigned iomux_rts;
    unsigned iomix_cts;
} drvUartHWDesc_t;

struct drvUart
{
    const drvUartHWDesc_t *hw_desc;
    drvIfcChannel_t rx_ifc;
    drvIfcChannel_t tx_ifc;
    drvUartCfg_t cfg;
    uint32_t divider;
    void *alloc_buf;
    uint8_t state;

    void *rx_dma_buf; // MUST be uncachable
    void *tx_dma_buf; // MUST be uncachable
    uint32_t rx_dma_size;
    uint32_t tx_dma_size;
    osiFifo_t rx_fifo;
    osiFifo_t tx_fifo;

    osiPmSource_t *pm_source;
    osiSemaphore_t *send_sema;
    osiSemaphore_t *recv_sema;
    osiSemaphore_t *tx_space_avail_sema;

    int sleep_timeout;
    osiTimer_t *sleep_timer;
    osiTimer_t *rx_poll_timer;
    osiTimer_t *auto_baud_timer;
};

#if defined(CONFIG_SOC_8909)
static const drvUartHWDesc_t gUartHWDesc[] = {
    {DRV_NAME_UART1, hwp_uart1, &hwp_sysCtrl->cfg_clk_uart[0], SYS_IRQ_UART1,
     HAL_IOMUX_FUN_UART1_RXD,
     HAL_IOMUX_FUN_UART1_TXD,
     HAL_IOMUX_FUN_UART1_RTS,
     HAL_IOMUX_FUN_UART1_CTS},
    {DRV_NAME_UART2, hwp_uart2, &hwp_sysCtrl->cfg_clk_uart[1], SYS_IRQ_UART2,
     HAL_IOMUX_FUN_UART2_RXD,
     HAL_IOMUX_FUN_UART2_TXD,
     HAL_IOMUX_FUN_UART2_RTS,
     HAL_IOMUX_FUN_UART2_CTS},
    {DRV_NAME_UART3, hwp_uart3, &hwp_sysCtrl->cfg_clk_uart[2], SYS_IRQ_UART3,
     HAL_IOMUX_FUN_UART3_RXD,
     HAL_IOMUX_FUN_UART3_TXD,
     HAL_IOMUX_FUN_UART3_RTS,
     HAL_IOMUX_FUN_UART3_CTS},
};
#elif defined(CONFIG_SOC_8955)
static const drvUartHWDesc_t gUartHWDesc[] = {
    {DRV_NAME_UART1, hwp_uart, &hwp_sysCtrl->cfg_clk_uart[1], SYS_IRQ_UART,
     HAL_IOMUX_FUN_UART1_RXD,
     HAL_IOMUX_FUN_UART1_TXD,
     HAL_IOMUX_FUN_UART1_RTS,
     HAL_IOMUX_FUN_UART1_CTS},
    {DRV_NAME_UART2, hwp_uart2, &hwp_sysCtrl->cfg_clk_uart[2], SYS_IRQ_UART2,
     HAL_IOMUX_FUN_UART2_RXD,
     HAL_IOMUX_FUN_UART2_TXD,
     HAL_IOMUX_FUN_UART2_RTS,
     HAL_IOMUX_FUN_UART2_CTS},
};
#endif

// start sleep timer is auto sleep is enabled
static void _startSleepTimer(drvUart_t *d)
{
    if (d->sleep_timeout >= 0)
        osiTimerStartRelaxed(d->sleep_timer, d->sleep_timeout, OSI_DELAY_MAX);
}

static void _startRxDma(drvUart_t *d)
{
    drvIfcStart(&d->rx_ifc, d->rx_dma_buf, d->rx_dma_size);
    osiTimerStartRelaxed(d->rx_poll_timer, UART_RX_POLL_PERIOD, OSI_WAIT_FOREVER);
}

// calculate the divider. "input" is the source clock frequency,
// "output" is the requested baud rate.
static unsigned _calcDivider(unsigned input, unsigned output)
{
    unsigned divider = UART_DIV_NUM(1) | UART_DIV_DENOM(1);

    if (output == 0) // auto baud
        return divider;

    unsigned delta = -1U;
    unsigned rnum = 0, rdenom = 0, rout = 0;
    for (unsigned num = 0; num < 0x400; num++)
    {
        uint64_t up = (uint64_t)input * num;
        unsigned denom = (up + output / 2) / output;
        if (denom == 0 || denom >= 0x4000)
            continue;

        unsigned out = up / denom;
        unsigned diff = (out > output) ? out - output : output - out;
        if (diff < delta)
        {
            delta = diff;
            rnum = num;
            rdenom = denom;
            rout = out;
        }
        if (diff == 0)
            break;
    }

    divider = UART_DIV_NUM(rnum) | UART_DIV_DENOM(rdenom);
    OSI_LOGD(0, "DIV24 input/%u output/%u num/%u denom/%u out/%u",
             input, output, rnum, rdenom, rout);
    return divider;
}

// set registers for baud rate
static void _setBaud(drvUart_t *d)
{
#if defined(CONFIG_SOC_8909)
    // all of the three UART will use 6.5MHz clock
    // NOTE: 0 is slow, 1 is fast
    REG_SYS_CTRL_SEL_CLOCK_T sel_clock = {hwp_sysCtrl->sel_clock};
    sel_clock.b.sel_clk_uart1 = 0;
    sel_clock.b.sel_clk_uart2 = 0;
    sel_clock.b.sel_clk_uart3 = 0;
    hwp_sysCtrl->sel_clock = sel_clock.v;

    d->divider = _calcDivider(6500000 / 4, d->cfg.baud);
    *(d->hw_desc->reg_cfg_clk_uart) = d->divider;
#elif defined(CONFIG_SOC_8955)
    d->divider = _calcDivider(52000000 / 4, d->cfg.baud);
    *(d->hw_desc->reg_cfg_clk_uart) = d->divider | UART_DIV_UPDATE;
#endif
}

// Check whether auto baud is locked. It may be called in UART ISR or
// polling timer. State may be changed if locked.
static bool _checkBaudLocked(drvUart_t *d)
{
    uint32_t critical = osiEnterCritical();

    REG_UART_CTRL_T ctrl;
    REG_UART_STATUS_T status = {d->hw_desc->hwp->status};

    if (status.b.auto_baud_locked)
    {
        d->state = UART_ST_OPENED;

        // The 2 sync characters won't be send to UART FIFO by hardware
        osiFifoReset(&d->rx_fifo);
        if (d->cfg.auto_baud_lc)
            osiFifoPut(&d->rx_fifo, "at", 2);
        else
            osiFifoPut(&d->rx_fifo, "AT", 2);

        REG_UART_IRQ_MASK_T irq_mask = {d->hw_desc->hwp->irq_mask};
#ifdef UART_HAS_AUTO_FAIL_INTR
        irq_mask.b.auto_fail = 0;
#endif
        irq_mask.b.rx_data_available = 0;
        irq_mask.b.rx_timeout = 0;
        irq_mask.b.rx_line_err = 0;
        irq_mask.b.rx_dma_done = 1;
        irq_mask.b.rx_dma_timeout = 1;
        d->hw_desc->hwp->irq_mask = irq_mask.v;

        // Auto baud shall be done in FIFO mode. After lock, it is needed
        // to change to DMA mode.
        REG_FIELD_CHANGE1(d->hw_desc->hwp->ctrl, ctrl, dma_mode, 1);
        _startRxDma(d);
        osiExitCritical(critical);
        return true;
    }

    if (status.b.character_miscompare)
    {
        // It is needed to restart manually.
        ctrl.v = d->hw_desc->hwp->ctrl;
        ctrl.b.enable = 0;
        d->hw_desc->hwp->ctrl = ctrl.v;

        osiDelayUS(20);
        ctrl.b.enable = 1;
        d->hw_desc->hwp->ctrl = ctrl.v;
    }

#ifndef UART_HAS_AUTO_FAIL_INTR
    osiTimerStartRelaxed(d->auto_baud_timer, UART_AUTO_BAUD_POLL_TIME, OSI_WAIT_FOREVER);
#endif

    osiExitCritical(critical);
    return false;
}

#ifndef UART_HAS_AUTO_FAIL_INTR
static void _autoBaudPoll(void *param)
{
    drvUart_t *d = (drvUart_t *)param;

    if (d->state != UART_ST_BAUD_LOCKING)
        return;

    _checkBaudLocked(d);
}
#endif

static void _rxDmaPoll(void *param)
{
    drvUart_t *d = (drvUart_t *)param;

    if (d->state != UART_ST_OPENED)
        return;

    osiIrqDisable(d->hw_desc->irqn);
    uint32_t evt = 0;
    int bytes = d->rx_dma_size - drvIfcGetTC(&d->rx_ifc);
    if (bytes > 0)
    {
        drvIfcFlush(&d->rx_ifc);
        int rbytes = osiFifoPut(&d->rx_fifo, d->rx_dma_buf, bytes);
        if (rbytes < bytes)
            evt |= DRV_UART_EVENT_RX_OVERFLOW;

        _startRxDma(d);
        evt |= DRV_UART_EVENT_RX_ARRIVED;
    }
    osiIrqEnable(d->hw_desc->irqn);

    if (evt != 0)
        _startSleepTimer(d);

    evt &= d->cfg.event_mask;
    if (evt != 0 && d->cfg.event_cb != 0)
        d->cfg.event_cb(d->cfg.event_cb_ctx, evt);
}

static void _uartISR(void *ctx)
{
    drvUart_t *d = (drvUart_t *)ctx;
    uint32_t evt = 0;

    // clear interrupts
    REG_UART_IRQ_CAUSE_T cause = {d->hw_desc->hwp->irq_cause};
    d->hw_desc->hwp->irq_cause = cause.v;

    OSI_LOGD(0, "DRV %4c IRQ, status=0x%08x, state=%d", d->hw_desc->name, cause.v, d->state);

    if (d->state == UART_ST_OPENED)
    {
        if (cause.b.rx_dma_done || cause.b.rx_dma_timeout)
        {
            drvIfcFlush(&d->rx_ifc);
            int bytes = d->rx_dma_size - drvIfcGetTC(&d->rx_ifc);
            if (bytes > 0)
            {
                int rbytes = osiFifoPut(&d->rx_fifo, d->rx_dma_buf, bytes);
                if (rbytes < bytes)
                    evt |= DRV_UART_EVENT_RX_OVERFLOW;

                _startRxDma(d);
                evt |= DRV_UART_EVENT_RX_ARRIVED;
            }
            else
            {
                drvIfcClearFlush(&d->rx_ifc);
            }
        }

        if (cause.b.tx_dma_done)
        {
            int bytes = osiFifoGet(&d->tx_fifo, d->tx_dma_buf, d->tx_dma_size);
            if (bytes > 0)
            {
                drvIfcStart(&d->tx_ifc, d->tx_dma_buf, bytes);

                // in case someone is waiting on TX space
                osiSemaphoreRelease(d->tx_space_avail_sema);
            }
            else
            {
                drvIfcReleaseChannel(&d->tx_ifc);
                evt |= DRV_UART_EVENT_TX_COMPLETE;
            }
        }
    }

    if (d->state == UART_ST_BAUD_LOCKING)
    {
        if (_checkBaudLocked(d))
            evt |= DRV_UART_EVENT_RX_ARRIVED;
    }

    if (d->state == UART_ST_SLEEP)
    {
        // The interrupts shall only wakeup system. The processing after
        // wakeup will be done at wakeup hook.
    }

    if (evt != 0)
        _startSleepTimer(d);

    evt &= d->cfg.event_mask;
    if (evt != 0 && d->cfg.event_cb != 0)
        d->cfg.event_cb(d->cfg.event_cb_ctx, evt);
}

static int _sendDmaMode(drvUart_t *d, const void *data, size_t size)
{
    int send = osiFifoPut(&d->tx_fifo, data, size);

    uint32_t sc = osiEnterCritical();

    if (!drvIfcIsRunning(&d->tx_ifc))
    {
        drvIfcWaitChannel(&d->tx_ifc);

        int bytes = osiFifoGet(&d->tx_fifo, d->tx_dma_buf, d->tx_dma_size);

        REG_UART_IRQ_MASK_T irq_mask;
        REG_FIELD_CHANGE1(d->hw_desc->hwp->irq_mask, irq_mask, tx_dma_done, 1);

        // TX DMA buffer must be uncachable
        drvIfcStart(&d->tx_ifc, d->tx_dma_buf, bytes);
    }

    osiExitCritical(sc);
    return send;
}

int drvUartSend(drvUart_t *d, const void *data, size_t size)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0 || d->state != UART_ST_OPENED)
        return 0;

    osiSemaphoreAcquire(d->send_sema);
    int len = _sendDmaMode(d, data, size);
    osiSemaphoreRelease(d->send_sema);

    OSI_LOGD(0, "DRV %4c send %d, write %d", d->hw_desc->name, size, len);
    return len;
}

int drvUartReceive(drvUart_t *d, void *data, size_t size)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0 || d->state != UART_ST_OPENED)
        return 0;

    osiSemaphoreAcquire(d->recv_sema);
    int len = osiFifoGet(&d->rx_fifo, data, size);
    osiSemaphoreRelease(d->recv_sema);

    OSI_LOGD(0, "DRV %4c receive %d, got %d", d->hw_desc->name, size, len);
    return len;
}

int drvUartSendAll(drvUart_t *d, const void *data, size_t size, uint32_t timeout_ms)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0 || d->state != UART_ST_OPENED)
        return 0;

    size_t total = size;
    osiSemaphoreAcquire(d->recv_sema);
    while (size > 0)
    {
        int len = _sendDmaMode(d, data, size);
        if (len > 0)
        {
            size -= len;
            data = (const char *)data + len;
        }
        else
        {
            if (!osiSemaphoreTryAcquire(d->tx_space_avail_sema, timeout_ms))
                break;
        }
    }

    osiSemaphoreRelease(d->recv_sema);
    return total - size;
}

bool drvUartWaitTxFinish(drvUart_t *d, uint32_t timeout)
{
    if (d == NULL)
        return false;

    unsigned start = osiUpTime();
    while (osiUpTime() - start < timeout)
    {
        uint32_t sc = osiEnterCritical();
        bool done = osiFifoIsEmpty(&d->tx_fifo) && !drvIfcIsRunning(&d->tx_ifc);
        osiExitCritical(sc);

        if (done)
            return true;
        osiThreadSleep(5);
    }

    return false;
}

void drvUartSetAutoSleep(drvUart_t *d, int timeout)
{
    d->sleep_timeout = timeout;
    _startSleepTimer(d);
}

static void _destroy(drvUart_t *d)
{
    osiSemaphoreDelete(d->recv_sema);
    osiSemaphoreDelete(d->send_sema);
    free(d->alloc_buf);
    free(d);
}

static void _sleepTimeout(void *param)
{
    drvUart_t *d = (drvUart_t *)param;
    osiPmWakeUnlock(d->pm_source);
}

static void _uartResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    // TODO
}

static const osiPmSourceOps_t _uartPmOps = {
    .resume = _uartResume,
};

static inline unsigned _rxDmaSize(uint32_t baud)
{
    if (baud == 0)
        baud = 921600;
    if (baud > 921600)
        baud = 921600;

    uint32_t size = ((baud + 115200 / 2) / 115200) * 256;
    if (size < 256)
        size = 256;
    return size;
}

static bool _allocBuf(drvUart_t *d)
{
    if (d->alloc_buf != NULL)
        free(d->alloc_buf);

    d->alloc_buf = NULL;

    uint32_t rx_dma_size = _rxDmaSize(d->cfg.baud);
    uint32_t alloc_size = d->cfg.rx_buf_size + d->cfg.tx_buf_size +
                          CONFIG_CACHE_LINE_SIZE + rx_dma_size;
#ifndef USE_STATIC_TX_BUF
    alloc_size += UART_TX_DMA_SIZE;
#endif
    uintptr_t alloc_buf = (uintptr_t)calloc(1, alloc_size);
    if (alloc_buf == (uintptr_t)NULL)
        return false;

    d->alloc_buf = (void *)alloc_buf;
    d->rx_dma_size = rx_dma_size;
    d->tx_dma_size = UART_TX_DMA_SIZE;
    void *rx_buf = (void *)OSI_PTR_INCR_POST(alloc_buf, d->cfg.rx_buf_size);
    void *tx_buf = (void *)OSI_PTR_INCR_POST(alloc_buf, d->cfg.tx_buf_size);
    alloc_buf = OSI_ALIGN_UP(alloc_buf, CONFIG_CACHE_LINE_SIZE);
    d->rx_dma_buf = (void *)OSI_KSEG1(OSI_PTR_INCR_POST(alloc_buf, rx_dma_size));
#ifndef USE_STATIC_TX_BUF
    d->tx_dma_buf = (void *)OSI_KSEG1(OSI_PTR_INCR_POST(alloc_buf, UART_TX_DMA_SIZE));
#endif
    osiFifoInit(&d->rx_fifo, rx_buf, d->cfg.rx_buf_size);
    osiFifoInit(&d->tx_fifo, tx_buf, d->cfg.tx_buf_size);
    return true;
}

drvUart_t *drvUartCreate(uint32_t name, drvUartCfg_t *cfg)
{
    OSI_LOGD(0, "DRV %4c create", name);

    const drvUartHWDesc_t *hw_desc = NULL;
    for (int n = 0; n < OSI_ARRAY_SIZE(gUartHWDesc); n++)
    {
        if (name == gUartHWDesc[n].name)
        {
            hw_desc = &gUartHWDesc[n];
            break;
        }
    }

    if (hw_desc == NULL)
    {
        OSI_LOGE(0, "DRV UART: invalid device name %4c", name);
        return NULL;
    }

    drvUart_t *d = (drvUart_t *)calloc(1, sizeof(drvUart_t));
    if (d == NULL)
        return NULL;

    d->hw_desc = hw_desc;
    d->cfg = *cfg;
    if (!drvIfcChannelInit(&d->rx_ifc, name, DRV_IFC_RX) ||
        !drvIfcChannelInit(&d->tx_ifc, name, DRV_IFC_TX) ||
        !_allocBuf(d))
    {
        free(d);
        return NULL;
    }

    d->state = UART_ST_CLOSED;
    d->pm_source = osiPmSourceCreate(name, &_uartPmOps, d);
    d->send_sema = osiSemaphoreCreate(1, 1);
    d->recv_sema = osiSemaphoreCreate(1, 1);
    d->tx_space_avail_sema = osiSemaphoreCreate(1, 0);
    d->rx_poll_timer = osiTimerCreate(NULL, _rxDmaPoll, d);
    d->sleep_timeout = -1;
    d->sleep_timer = osiTimerCreate(NULL, _sleepTimeout, d);
#ifndef UART_HAS_AUTO_FAIL_INTR
    d->auto_baud_timer = osiTimerCreate(NULL, _autoBaudPoll, d);
#endif
#ifdef USE_STATIC_TX_BUF
    d->tx_dma_size = UART_TX_DMA_SIZE;
    d->tx_dma_buf = (d->hw_desc->name == DRV_NAME_UART1) ? gUart1TxDmaBuf : gUart2TxDmaBuf;
#endif
    return d;
}

bool drvUartOpen(drvUart_t *d)
{
    OSI_LOGD(0, "DRV %4c open", d->hw_desc->name);

    if (d->alloc_buf == NULL)
        return false;

    osiIrqDisable(d->hw_desc->irqn);

    halIomuxSetFunction(d->hw_desc->iomux_rx);
    halIomuxSetFunction(d->hw_desc->iomux_tx);

    _setBaud(d);

    bool auto_baud = (d->cfg.baud == 0);
    uint32_t parity_select;
    if (d->cfg.parity == DRV_UART_ODD_PARITY)
        parity_select = 0;
    else if (d->cfg.parity == DRV_UART_EVEN_PARITY)
        parity_select = 1;
    else if (d->cfg.parity == DRV_UART_SPACE_PARITY)
        parity_select = 2;
    else if (d->cfg.parity == DRV_UART_MARK_PARITY)
        parity_select = 3;
    else
        parity_select = 0;

    REG_UART_CTRL_T ctrl = {
        .b = {
            .enable = 1,
            .data_bits = (d->cfg.data_bits == DRV_UART_DATA_BITS_7) ? 0 : 1,
            .tx_stop_bits = (d->cfg.stop_bits == DRV_UART_STOP_BITS_1) ? 0 : 1,
            .parity_enable = (d->cfg.parity == DRV_UART_NO_PARITY) ? 0 : 1,
            .parity_select = parity_select,
            .auto_enable = (d->cfg.baud == 0) ? 1 : 0,
            .sync_char_sel = d->cfg.auto_baud_lc ? 1 : 0,
            .divisor_mode = 0,
            .irda_enable = 0,
            .dma_mode = auto_baud ? 0 : 1,
            .auto_flow_control = (d->cfg.cts_enable || d->cfg.rts_enable) ? 1 : 0,
            .loop_back_mode = 0,
            .rx_lock_err = 0,
            .rx_break_length = 13,
        }};
    REG_UART_TRIGGERS_T triggers = {
        .b = {
            .rx_trigger = 0,
            .tx_trigger = 0,
            .afc_level = UART_RX_FIFO_SIZE / 2,
        }};

    d->hw_desc->hwp->irq_mask = 0;
    d->hw_desc->hwp->triggers = triggers.v;
    d->hw_desc->hwp->ctrl = ctrl.v;

    REG_UART_IRQ_MASK_T irq_mask = {};
    drvIfcWaitChannel(&d->rx_ifc);
    if (auto_baud)
    {
        d->state = UART_ST_BAUD_LOCKING;

#ifdef UART_HAS_AUTO_FAIL_INTR
        irq_mask.b.auto_fail = 1;
#else
        osiTimerStartRelaxed(d->auto_baud_timer, UART_AUTO_BAUD_POLL_TIME, OSI_WAIT_FOREVER);
#endif
        irq_mask.b.rx_data_available = 1;
        irq_mask.b.rx_timeout = 1;
        irq_mask.b.rx_line_err = 1;
    }
    else
    {
        d->state = UART_ST_OPENED;
        irq_mask.b.rx_dma_done = 1;
        irq_mask.b.rx_dma_timeout = 1;
        irq_mask.b.tx_dma_done = 1;
        _startRxDma(d);
    }

    d->hw_desc->hwp->irq_mask = irq_mask.v;
    osiIrqSetHandler(d->hw_desc->irqn, _uartISR, d);
    osiIrqEnable(d->hw_desc->irqn);
    return true;
}

void drvUartClose(drvUart_t *d)
{
    OSI_LOGD(0, "DRV %4c close", d->hw_desc->name);

    osiIrqDisable(d->hw_desc->irqn);
    osiIrqSetHandler(d->hw_desc->irqn, NULL, NULL);
    d->state = UART_ST_CLOSED;
}

void drvUartDestroy(drvUart_t *d)
{
    if (d == NULL)
        return;

    drvUartClose(d);
    _destroy(d);
}

const drvUartCfg_t *drvUartConfig(drvUart_t *d)
{
    if (d == NULL)
        return NULL;
    return &d->cfg;
}

bool drvUartReconfig(drvUart_t *d, drvUartCfg_t *cfg)
{
    if (d == NULL || d->state != UART_ST_CLOSED)
        return false;

    if (cfg == NULL)
        return true;

    bool realloc_needed = (d->cfg.tx_buf_size != cfg->tx_buf_size ||
                           d->cfg.rx_buf_size != cfg->rx_buf_size ||
                           d->cfg.baud != cfg->baud);

    d->cfg = *cfg;
    if (realloc_needed)
        return _allocBuf(d);
    return true;
}

int drvUartReadAvail(drvUart_t *d)
{
    if (d == NULL)
        return -1;

    if (d->alloc_buf == NULL)
        return 0;

    return osiFifoBytes(&d->rx_fifo);
}

int drvUartWriteAvail(drvUart_t *d)
{
    if (d == NULL)
        return -1;

    return osiFifoSpace(&d->tx_fifo);
}
