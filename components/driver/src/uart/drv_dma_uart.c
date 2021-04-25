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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_log.h"
#include "osi_api.h"
#include "osi_fifo.h"
#include "drv_uart.h"
#include "drv_names.h"
#include "drv_axidma.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "drv_config.h"
#include <stdlib.h>
#include <string.h>

#include "quec_proj_config.h"


// UART baud rate calculation
// --------------------------
// * When cfg_clk_uart.b.uart_sel_pll == 0, UART input clock is 26MHz.
// * When cfg_clk_uart.b.uart_sel_pll == 1, UART input clock is half of
//   CPUPLL divided by cfg_clk_uart.

#define FORCE_RX_FIFO_MODE false
#define FORCE_TX_FIFO_MODE true

#define UART_FAST_INPUT_CLK (CONFIG_DEFAULT_CPUPLL_FREQ / 2)
#define UART_BAUD_BASE_FAST_CLK (96000000)
#define UART_BAUD_BASE_SLOW_CLK (26000000)
#define UART_RX_TIMEOUT_CNT (4 * 10)
#define UART_TX_DELAY (0)
#define UART_TXFIFO_SIZE (128)
#define UART_RXFIFO_SIZE (128)
#define UART_TX_TRIG_FIFO_MODE (0)
#define UART_RX_TRIG_FIFO_MODE (64)
#define UART_TX_TRIG_DMA_MODE (1)
#define UART_RX_TRIG_DMA_MODE (112)

// The source IDs should match hardware
#define AD_SOURCE_UART1RX 0
#define AD_SOURCE_UART1TX 1
#define AD_SOURCE_UART2RX 2
#define AD_SOURCE_UART2TX 3
#define AD_SOURCE_UART3RX 4
#define AD_SOURCE_UART3TX 5

struct drvUart
{
    uint32_t name;
    bool opened;
    bool auto_baud;
    HWP_ARM_UART_T *hwp;
    bool use_fast_base_clock;
    volatile uint32_t *reg_clk_uart;
    uint8_t rx_dma_source_id;
    uint8_t tx_dma_source_id;
    uint32_t irqn;
    uint32_t irq_priority;

    bool tx_use_dma;
    bool rx_use_dma;
    bool tx_complete_needed;
    drvAxidmaCh_t *rx_dma_ch;
    drvAxidmaCh_t *tx_dma_ch;
    uint32_t rx_dma_size;
    uint32_t tx_dma_size;
    uint8_t *rx_dma_buf;
    uint8_t *tx_dma_buf;

    drvUartCfg_t cfg;
    uint8_t isr_data[UART_TXFIFO_SIZE];
    osiFifo_t rx_fifo;
    osiFifo_t tx_fifo;
    osiPmSource_t *pm_source;
    osiSemaphore_t *send_sema;
    osiSemaphore_t *recv_sema;
    osiSemaphore_t *recv_avail_sema;

    volatile bool send_timeout;
    osiTimer_t *send_timer;

    int sleep_timeout;
    osiTimer_t *sleep_timer;
    void *alloc_buf;
};

static void drvUartISR(void *ctx);
static void _uartStartTxDma(drvUart_t *d, const void *data, size_t size);
static bool _uartStartRxDma(drvUart_t *d);
static bool _setBaud(drvUart_t *d, uint32_t baud);

static void _startSleepTimer(drvUart_t *d)
{
    if (d->sleep_timeout >= 0)
        osiTimerStart(d->sleep_timer, d->sleep_timeout);
}

static unsigned _calcDivider24(unsigned input, unsigned output)
{
    unsigned delta = -1U;
    unsigned rnum = 0, rdenom = 0, rout = 0;
    for (unsigned num = 0x3ff; num > 0; num--)
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

    OSI_LOGD(0, "DIV24 input/%u output/%u num/%u denom/%u out/%u",
             input, output, rnum, rdenom, rout);
    return (rdenom << 10) | rnum;
}

static unsigned _calcUartDiv(unsigned input, unsigned output)
{
    unsigned delta = -1U;
    unsigned rs = 1, rd = 1, rout = 0;
    for (unsigned s = 16; s >= 6; s--)
    {
        unsigned d = (input + (s * output) / 2) / (s * output);
        if (d <= 1 || d >= (1 << 16))
            continue;

        unsigned out = input / (s * d);
        unsigned diff = (out > output) ? out - output : output - out;
        if (diff < delta)
        {
            delta = diff;
            rs = s;
            rd = d;
            rout = out;
        }
        if (diff == 0)
            break;
    }

    OSI_LOGD(0, "DIVUART input/%u output/%u const/%u div/%u out/%u",
             input, output, rs, rd, rout);
    return ((rs - 1) << 16) | (rd - 1);
}

static unsigned _uartInput(drvUart_t *d)
{
    REG_SYS_CTRL_CFG_CLK_UART_T clk_uart = {*d->reg_clk_uart};
    if (clk_uart.b.uart_sel_pll != 0)
    {
        unsigned num = clk_uart.b.uart_divider & 0x3ff;
        unsigned denom = (clk_uart.b.uart_divider >> 10) & 0x3fff;
        return (((uint64_t)UART_FAST_INPUT_CLK * num) / denom);
    }

    return UART_BAUD_BASE_SLOW_CLK;
}

static unsigned _calcBaud(drvUart_t *d)
{
    unsigned uart_input = _uartInput(d);

    REG_ARM_UART_UART_BAUD_T uart_baud = {d->hwp->uart_baud};
    return uart_input / ((uart_baud.b.baud_const + 1) * (uart_baud.b.baud_div + 1));
}

static unsigned _calcAutoBaud(drvUart_t *d)
{
    REG_ARM_UART_UART_AT_STATUS_T status = {d->hwp->uart_at_status};
    if (!status.b.auto_baud_locked)
        return 0;

    unsigned uart_input = _uartInput(d);
    REG_ARM_UART_UART_CONF_T uart_conf = {d->hwp->uart_conf};

    return uart_input / (((uart_conf.b.at_div_mode ? 0xf : 0x3) + 1) * (status.b.at_baud_div + 1));
}

// Check whether auto baud is locked. It may be called in UART ISR or
// polling timer. State may be changed if locked.
static bool _uartAutoBaudCheck(drvUart_t *d)
{
    bool locked = false;
    uint32_t sc = osiEnterCritical();

    REG_ARM_UART_UART_AT_STATUS_T status = {d->hwp->uart_at_status};
    REG_ARM_UART_UART_CONF_T config = {d->hwp->uart_conf};

    if (status.b.character_miscompare)
    {
        config.b.at_enable = 0;
        d->hwp->uart_conf = config.v;
        osiDelayUS(20);
        config.b.at_enable = 1;
        d->hwp->uart_conf = config.v;
        locked = false;
    }
    else if (status.b.auto_baud_locked)
    {
        OSI_LOGI(0, "DRV %4c baud locked, status=0x%x", d->name, status.v);
        d->auto_baud = false;
        d->cfg.baud = _calcAutoBaud(d);

        osiFifoReset(&d->rx_fifo);
        locked = true;
    }

    osiExitCritical(sc);
    return locked;
}

static void _uartTxDmaIsr(drvAxidmaIrqEvent_t devt, void *p)
{
    drvUart_t *d = (drvUart_t *)p;
    OSI_LOGD(0, "DRV %4c TX DMA event 0x%x", d->name, devt);

    if (d->tx_dma_ch == NULL)
    {
        OSI_LOGE(0, "DRV %4c TX DMA interrupt without channel", d->name);
        return;
    }

    if (devt & AD_EVT_FINISH)
    {
        if (drvAxidmaChBusy(d->tx_dma_ch))
        {
            OSI_LOGE(0, "DRV %4c TX DMA finished but busy", d->name);
        }
        else if (!osiFifoIsEmpty(&d->tx_fifo))
        {
            int size = osiFifoGet(&d->tx_fifo, d->tx_dma_buf, d->tx_dma_size);
            _uartStartTxDma(d, d->tx_dma_buf, size);
        }
        else
        {
            if (d->tx_complete_needed)
            {
                uint32_t event = d->cfg.event_mask & DRV_UART_EVENT_TX_COMPLETE;
                d->tx_complete_needed = false;
                if (event && d->cfg.event_cb)
                    d->cfg.event_cb(d->cfg.event_cb_ctx, event);
            }
        }
    }
}

static void _uartRxDmaIsr(drvAxidmaIrqEvent_t devt, void *p_)
{
    uint32_t event = 0;
    drvUart_t *d = (drvUart_t *)p_;
    uint32_t sc = osiEnterCritical();
    osiIrqDisable(d->irqn);

    OSI_LOGD(0, "DRV %4c RX DMA event 0x%x", d->name, devt);

    if (devt & (AD_EVT_FINISH | AD_EVT_PART_FINISH))
    {
        unsigned count = drvAxidmaChStop(d->rx_dma_ch);
        uint32_t recv = d->rx_dma_size - count;
        if (recv != 0)
        {
            event |= DRV_UART_EVENT_RX_ARRIVED;
            osiDCacheInvalidate(d->rx_dma_buf, d->rx_dma_size);
            int trans = osiFifoPut(&d->rx_fifo, d->rx_dma_buf, recv);
            if ((uint32_t)trans != recv)
            {
                event |= DRV_UART_EVENT_RX_OVERFLOW;
            }
        }
        if (osiFifoSpace(&d->rx_fifo) >= UART_RXFIFO_SIZE)
        {
            _uartStartRxDma(d);
        }
        else
        {
            OSI_LOGI(0, "DRV %4c stop Rx Dma in DMA ISR", d->name);
        }
        _startSleepTimer(d);
    }

    osiIrqEnable(d->irqn);
    osiExitCritical(sc);

    if (event & DRV_UART_EVENT_RX_ARRIVED)
        osiSemaphoreRelease(d->recv_avail_sema);

    event &= d->cfg.event_mask;
    if (event != 0 && d->cfg.event_cb != NULL)
        d->cfg.event_cb(d->cfg.event_cb_ctx, event);
}

static inline bool _setHWRegs(drvUart_t *d)
{
    switch (d->name)
    {
    case DRV_NAME_UART1:
        d->hwp = hwp_uart1;
        d->rx_dma_source_id = AD_SOURCE_UART1RX;
        d->tx_dma_source_id = AD_SOURCE_UART1TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_1);
        d->irq_priority = SYS_IRQ_PRIO_UART_1;
        d->reg_clk_uart = &hwp_sysCtrl->cfg_clk_uart[1];
        d->use_fast_base_clock = false;
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_1);
        break;

    case DRV_NAME_UART2:
        d->hwp = hwp_uart2;
        d->rx_dma_source_id = AD_SOURCE_UART2RX;
        d->tx_dma_source_id = AD_SOURCE_UART2TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_2);
        d->irq_priority = SYS_IRQ_PRIO_UART_2;
        d->reg_clk_uart = &hwp_sysCtrl->cfg_clk_uart[2];
        d->use_fast_base_clock = true;
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_2);
        break;

    case DRV_NAME_UART3:
        d->hwp = hwp_uart3;
        d->rx_dma_source_id = AD_SOURCE_UART3RX;
        d->tx_dma_source_id = AD_SOURCE_UART3TX;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_UART_3);
        d->irq_priority = SYS_IRQ_PRIO_UART_3;
        d->reg_clk_uart = &hwp_sysCtrl->cfg_clk_uart[3];
        d->use_fast_base_clock = true;
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_UART_3);
        break;

    default:
        OSI_LOGE(0, "DRV UART: invalid device name %4c", d->name);
        return false;
    }

    if (d->tx_use_dma)
    {
        drvAxidmaChSetDmamap(d->tx_dma_ch, d->tx_dma_source_id, d->tx_dma_source_id);
        drvAxidmaChRegisterIsr(d->tx_dma_ch, _uartTxDmaIsr, d);
    }

    if (d->rx_use_dma)
    {
        drvAxidmaChSetDmamap(d->rx_dma_ch, d->rx_dma_source_id, d->rx_dma_source_id);
        drvAxidmaChRegisterIsr(d->rx_dma_ch, _uartRxDmaIsr, d);
    }

    return true;
}

static bool _setBaud(drvUart_t *d, uint32_t baud)
{
    uint32_t set_baud = baud;
    if (baud == 0)
    {
        // uart use default buad to communicate
        baud = CONFIG_UART_AUTOMODE_DEFAULT_BAUD;
        OSI_LOGI(0, "%4c auto baud mode, tx default baud %d", d->name, baud);
    }

    REG_SYS_CTRL_CFG_CLK_UART_T clk_uart;
    if (d->use_fast_base_clock)
    {
        unsigned uart_baud = _calcUartDiv(UART_BAUD_BASE_FAST_CLK, baud);
        unsigned div24 = _calcDivider24(UART_FAST_INPUT_CLK, UART_BAUD_BASE_FAST_CLK);

        REG_FIELD_CHANGE3(*(d->reg_clk_uart), clk_uart,
                          uart_divider, div24,
                          uart_divider_update, 1,
                          uart_sel_pll, 1);
        d->hwp->uart_baud = uart_baud;
    }
    else
    {
        REG_FIELD_CHANGE1(*(d->reg_clk_uart), clk_uart,
                          uart_sel_pll, 0);
        d->hwp->uart_baud = _calcUartDiv(UART_BAUD_BASE_SLOW_CLK, baud);
    }

    OSI_LOGI(0, "%4c set baud %d, real %d", d->name, set_baud, _calcBaud(d));
    return true;
}

static inline void _setTxInt(drvUart_t *d, bool enabled)
{
    REG_ARM_UART_UART_CONF_T uart_conf = {d->hwp->uart_conf};
    uart_conf.b.tx_ie = enabled ? 1 : 0;
    d->hwp->uart_conf = uart_conf.v;
}

static inline size_t _txSpace(drvUart_t *d)
{
    REG_ARM_UART_UART_TXFIFO_STAT_T uart_txfifo_stat = {d->hwp->uart_txfifo_stat};
    uint32_t tx_fifo_cnt = uart_txfifo_stat.b.tx_fifo_cnt;
    return tx_fifo_cnt < UART_TXFIFO_SIZE
               ? UART_TXFIFO_SIZE - tx_fifo_cnt
               : 0;
}

static inline size_t _rxBytes(drvUart_t *d)
{
    REG_ARM_UART_UART_RXFIFO_STAT_T uart_rxfifo_stat = {d->hwp->uart_rxfifo_stat};
    return uart_rxfifo_stat.b.rx_fifo_cnt;
}

static bool _uartStartRxDma(drvUart_t *d)
{
    osiDCacheInvalidate(d->rx_dma_buf, d->rx_dma_size);
    drvAxidmaCfg_t cfg = {};
    cfg.src_addr = (uint32_t)&d->hwp->uart_rx;
    cfg.dst_addr = (uint32_t)d->rx_dma_buf;
    cfg.data_size = d->rx_dma_size;
    cfg.part_trans_size = UART_RX_TRIG_DMA_MODE;
    cfg.data_type = AD_DATA_8BIT;
    cfg.src_addr_fix = 1;
    cfg.sync_irq = 1;
    cfg.mask = (AD_EVT_FINISH | AD_EVT_PART_FINISH);

    return drvAxidmaChStart(d->rx_dma_ch, &cfg);
}

static bool _startConfig(drvUart_t *d)
{
    osiIrqDisable(d->irqn);

    REG_ARM_UART_UART_CONF_T uart_conf = {};
    uart_conf.b.check = (d->cfg.parity == DRV_UART_NO_PARITY) ? 0 : 1;
    uart_conf.b.parity = (d->cfg.parity == DRV_UART_EVEN_PARITY) ? 0 : 1;
    uart_conf.b.stop_bit = (d->cfg.stop_bits == DRV_UART_STOP_BITS_1) ? 0 : 1;
    uart_conf.b.at_enable = d->cfg.baud ? 0 : 1;
    // if at_enable, check should be closed, and resumed when baud locked
    if (uart_conf.b.at_enable)
        uart_conf.b.check = 0;
    uart_conf.b.st_check = 1;
    uart_conf.b.rx_ie = d->rx_use_dma ? 0 : 1;
    uart_conf.b.tx_ie = 0;
    uart_conf.b.tout_ie = 1;
    uart_conf.b.hdlc = 0;
    uart_conf.b.frm_stp = 1; // ??
    uart_conf.b.trail = 0;
    uart_conf.b.txrst = 1;
    uart_conf.b.rxrst = 1;
    uart_conf.b.at_parity_en = uart_conf.b.check;
    uart_conf.b.at_parity_sel = uart_conf.b.parity;
    uart_conf.b.at_verify_2byte = 1;
    uart_conf.b.at_div_mode = 0;

    uart_conf.b.hwfc = (d->cfg.rts_enable || d->cfg.cts_enable) ? 1 : 0;
    if (d->cfg.rts_enable)
    {
        uart_conf.b.tout_hwfc = 0;
        uart_conf.b.rx_trig_hwfc = 1;
    }
    d->hwp->uart_conf = uart_conf.v;

    REG_ARM_UART_UART_DELAY_T uart_delay = {};
    uart_delay.b.toutcnt = UART_RX_TIMEOUT_CNT;
    if (d->name == DRV_NAME_UART2)
    {
        uart_delay.b.two_tx_delay = 1;
    }
    else
    {
        uart_delay.b.two_tx_delay = UART_TX_DELAY;
    }
    d->hwp->uart_delay = uart_delay.v;

    REG_ARM_UART_UART_RXTRIG_T uart_rxtrig = {};
    uart_rxtrig.b.rx_trig = d->rx_use_dma ? UART_RX_TRIG_DMA_MODE : UART_RX_TRIG_FIFO_MODE;
    d->hwp->uart_rxtrig = uart_rxtrig.v;

    REG_ARM_UART_UART_TXTRIG_T uart_txtrig = {};
    uart_txtrig.b.tx_trig = d->tx_use_dma ? UART_TX_TRIG_DMA_MODE : UART_TX_TRIG_FIFO_MODE;
    d->hwp->uart_txtrig = uart_txtrig.v;

    for (;;)
    {
        uart_conf.v = d->hwp->uart_conf;
        if (uart_conf.b.txrst == 0 && uart_conf.b.rxrst == 0)
            break;
    }

    d->hwp->uart_status = d->hwp->uart_status;
    if (d->rx_use_dma)
    {
        // when at_enable, use fifo mode temporary to drop the first untrusted package
        if (uart_conf.b.at_enable)
        {
            uart_rxtrig.b.rx_trig = UART_RX_TRIG_FIFO_MODE;
            d->hwp->uart_rxtrig = uart_rxtrig.v;
        }
        else if ((d->cfg.rx_buf_size < UART_RXFIFO_SIZE) || !_uartStartRxDma(d))
        {
            uart_rxtrig.b.rx_trig = UART_RX_TRIG_FIFO_MODE;
            d->hwp->uart_rxtrig = uart_rxtrig.v;
            d->rx_use_dma = false;
        }
    }

    // when auto baud locked ,at_enable set 0 at status will be clean
    REG_ARM_UART_UART_AT_STATUS_T status = {d->hwp->uart_at_status};
    d->auto_baud = (d->cfg.baud == 0 && !status.b.auto_baud_locked);

    osiIrqSetHandler(d->irqn, drvUartISR, d);
    osiIrqSetPriority(d->irqn, d->irq_priority);
    osiIrqEnable(d->irqn);

    return true;
}

static int _sendFifoMode(drvUart_t *d, const uint8_t *data, size_t size)
{
    uint32_t critical = osiEnterCritical();

    int send = 0;
    if (osiFifoIsEmpty(&d->tx_fifo))
    {
        // send to HW fifo only when there are no pending data in SW fifo
        size_t tx_space = _txSpace(d);
        if (tx_space > size)
            tx_space = size;

        for (size_t n = 0; n < tx_space; n++)
            d->hwp->uart_tx = *data++;

        size -= tx_space;
        send += tx_space;

        // enabled tx interrupt
        _setTxInt(d, true);
    }

    // put remaining in SW fifo, and SW fifo will check zero size
    send += osiFifoPut(&d->tx_fifo, data, size);
    d->tx_complete_needed = true;

    osiExitCritical(critical);
    return send;
}

static int _recvFifoMode(drvUart_t *d, uint8_t *data, size_t size)
{
    osiIrqDisable(d->irqn);

    int recv = osiFifoGet(&d->rx_fifo, data, size);
    if (recv < size)
    {
        // when there are no enough data in SW fifo, check HW fifo
        size_t rx_bytes = _rxBytes(d);
        if (rx_bytes > 0)
        {
            data += recv;
            size -= recv;
            if (rx_bytes > size)
                rx_bytes = size;

            for (size_t n = 0; n < rx_bytes; n++)
                *data++ = d->hwp->uart_rx;
            recv += rx_bytes;
        }
    }

    osiIrqEnable(d->irqn);
    return recv;
}

static void drvUartISR(void *ctx)
{
    drvUart_t *d = (drvUart_t *)ctx;
    uint32_t evt = 0;

    // clear interrupts
    REG_ARM_UART_UART_STATUS_T status = {d->hwp->uart_status};
    d->hwp->uart_status = status.v; // write to clear
    OSI_LOGD(0, "DRV %4c IRQ, status=0x%08x", d->name, status.v);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP    
    //add by sum.li 2021/2/25  uart haved data,not allow to sleep 
    extern void ql_data_lock_not_allow_sleep(void);//Defined in quec_power_mg.c
    ql_data_lock_not_allow_sleep();
#endif
    
    if (d->auto_baud)
    {
        bool locked = _uartAutoBaudCheck(d);
        if (locked)
            evt |= DRV_UART_EVENT_RX_ARRIVED;
    }

    REG_ARM_UART_UART_AT_STATUS_T at_status = {d->hwp->uart_at_status};

    if (status.b.timeout_int || status.b.rx_int)
    {
        if (at_status.b.auto_baud_locked == 0)
        {
            evt |= DRV_UART_EVENT_RX_ARRIVED;
            if (d->rx_use_dma && drvAxidmaChBusy(d->rx_dma_ch))
            {
                unsigned count = drvAxidmaChStop(d->rx_dma_ch);
                uint32_t dma_recv = d->rx_dma_size - count;
                if (dma_recv != 0)
                {
                    osiDCacheInvalidate(d->rx_dma_buf, d->rx_dma_size);
                    int dma_trans = osiFifoPut(&d->rx_fifo, d->rx_dma_buf, dma_recv);
                    if ((uint32_t)dma_trans != dma_recv)
                    {
                        evt |= DRV_UART_EVENT_RX_OVERFLOW;
                    }
                }
            }

            size_t rx_remained = osiFifoSpace(&d->rx_fifo);
            size_t rx_bytes = _rxBytes(d);
            if (rx_remained > 0)
            {
                if (rx_bytes > rx_remained)
                {
                    OSI_LOGD(0, "DRV %4c RX revc %d  trans %d", d->name, rx_bytes, rx_remained);
                    rx_bytes = rx_remained;
                }

                for (size_t n = 0; n < rx_bytes; n++)
                {
                    d->isr_data[n] = d->hwp->uart_rx;
                }

                int trans = osiFifoPut(&d->rx_fifo, d->isr_data, rx_bytes);
                if (trans != rx_bytes)
                {
                    evt |= DRV_UART_EVENT_RX_OVERFLOW;
                }
            }
            else
            {
                if (rx_bytes > UART_RXFIFO_SIZE - 1)
                {
                    evt |= DRV_UART_EVENT_RX_OVERFLOW;
                }

            }
        }
        else
        {
            // when auto baud locked, change to fixed baud
            _setBaud(d, d->cfg.baud);
            osiFifoReset(&d->rx_fifo);

            REG_ARM_UART_UART_CONF_T config = {d->hwp->uart_conf};
            config.b.rxrst = 1;
            config.b.check = (d->cfg.parity == DRV_UART_NO_PARITY) ? 0 : 1;
            config.b.at_parity_en = config.b.check;
            config.b.at_enable = 0;
            d->hwp->uart_conf = config.v;
            REG_WAIT_FIELD_EQZ(config, d->hwp->uart_conf, rxrst);

            if (d->rx_use_dma)
            {
                REG_ARM_UART_UART_RXTRIG_T uart_rxtrig = {};
                uart_rxtrig.b.rx_trig = UART_RX_TRIG_DMA_MODE;
                d->hwp->uart_rxtrig = uart_rxtrig.v;
            }
        }
        if (d->rx_use_dma)
        {
            if (osiFifoSpace(&d->rx_fifo) >= UART_RXFIFO_SIZE)
                _uartStartRxDma(d);
            _startSleepTimer(d);
        }
    }

    size_t tx_space = _txSpace(d);
    size_t tx_remained = osiFifoBytes(&d->tx_fifo);
    if (tx_space == UART_TXFIFO_SIZE && tx_remained == 0)
    {
        _setTxInt(d, false);
        if (d->tx_complete_needed)
            evt |= DRV_UART_EVENT_TX_COMPLETE;
        d->tx_complete_needed = false;
    }
    else if (tx_space > 0)
    {
        _setTxInt(d, true);
        size_t len = osiFifoGet(&d->tx_fifo, d->isr_data, tx_space);
        for (unsigned n = 0; n < len; n++)
            d->hwp->uart_tx = d->isr_data[n];
    }

    if (evt & DRV_UART_EVENT_RX_ARRIVED)
        osiSemaphoreRelease(d->recv_avail_sema);

    if (evt != 0 && evt != DRV_UART_EVENT_TX_COMPLETE)
        _startSleepTimer(d);

    evt &= d->cfg.event_mask;
    if (evt != 0 && d->cfg.event_cb != 0)
        d->cfg.event_cb(d->cfg.event_cb_ctx, evt);
}

static void _uartStartTxDma(drvUart_t *d, const void *data, size_t size)
{
    osiDCacheClean(data, size);
    drvAxidmaCfg_t cfg = {};
    cfg.src_addr = (const uint32_t)data;
    cfg.dst_addr = (uint32_t)&d->hwp->uart_tx;
    cfg.data_size = size;
    cfg.data_type = AD_DATA_8BIT;
    cfg.part_trans_size = 1;
    cfg.dst_addr_fix = 1;
    cfg.force_trans = 1;
    cfg.mask = AD_EVT_FINISH;
    OSI_LOGD(0, "TX DMA %p, %d", data, size);
    drvAxidmaChStart(d->tx_dma_ch, &cfg);
}

static int _sendDmaMode(drvUart_t *d, const void *data, size_t size)
{
    uint32_t sc = osiEnterCritical();

    int send = 0;
    if (!drvAxidmaChBusy(d->tx_dma_ch) && osiFifoIsEmpty(&d->tx_fifo))
    {
        send += (size > d->tx_dma_size) ? d->tx_dma_size : size;
        memcpy(d->tx_dma_buf, data, send);
        _uartStartTxDma(d, d->tx_dma_buf, send);

        data = (char *)data + send;
        size -= send;
    }

    send += osiFifoPut(&d->tx_fifo, data, size);
    d->tx_complete_needed = true;

    osiExitCritical(sc);
    return send;
}

static inline int _recvDmaMode(drvUart_t *d, uint8_t *data, size_t size)
{
    uint32_t sc = osiEnterCritical();
    osiIrqDisable(d->irqn);

    int recv = osiFifoGet(&d->rx_fifo, data, size);

    if (!drvAxidmaChBusy(d->rx_dma_ch))
    {
        int rx_bytes = _rxBytes(d);
        if (rx_bytes > 0 && recv < size)
        {
            data += recv;
            size -= recv;
            if (rx_bytes > size)
                rx_bytes = size;
            for (size_t n = 0; n < rx_bytes; n++)
            {
                *data++ = d->hwp->uart_rx;
            }
            recv += rx_bytes;
        }

        size_t space = osiFifoSpace(&d->rx_fifo);
        if (space > UART_RXFIFO_SIZE)
        {
            _uartStartRxDma(d);
        }
    }

    osiIrqEnable(d->irqn);
    osiExitCritical(sc);

    return recv;
}

int drvUartSend(drvUart_t *d, const void *data, size_t size)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0)
        return 0;
    osiSemaphoreAcquire(d->send_sema);
    int len = d->tx_use_dma
                  ? _sendDmaMode(d, data, size)
                  : _sendFifoMode(d, data, size);
    osiSemaphoreRelease(d->send_sema);

    OSI_LOGD(0, "DRV %4c send %d, write %d", d->name, size, len);
    return len;
}

int drvUartReceive(drvUart_t *d, void *data, size_t size)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0)
        return 0;
    osiSemaphoreAcquire(d->recv_sema);
    int len = d->rx_use_dma
                  ? _recvDmaMode(d, data, size)
                  : _recvFifoMode(d, data, size);
    osiSemaphoreRelease(d->recv_sema);

    OSI_LOGD(0, "DRV %4c receive %d, got %d", d->name, size, len);
    return len;
}

static void _sendTimeout(void *param)
{
    drvUart_t *d = (drvUart_t *)param;
    d->send_timeout = true;
}

int drvUartSendAll(drvUart_t *d, const void *data, size_t size, uint32_t timeout_ms)
{
    if (d == NULL)
        return -1;
    if (data == NULL || size == 0)
        return 0;

    size_t total = size;
    osiSemaphoreAcquire(d->send_sema);

    d->send_timeout = false;
    osiTimerStart(d->send_timer, timeout_ms);
    while (size > 0)
    {
        int len = d->tx_use_dma
                      ? _sendDmaMode(d, data, size)
                      : _sendFifoMode(d, data, size);

        if (len > 0)
        {
            osiTimerStart(d->send_timer, timeout_ms);
            size -= len;
            data = (const char *)data + len;
        }
        else
        {
            if (d->send_timeout)
                break;

            osiThreadSleep(10);
        }
    }
    osiTimerStop(d->send_timer);
    osiSemaphoreRelease(d->send_sema);

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
        bool done = osiFifoIsEmpty(&d->tx_fifo);
        if (done)
        {
            done = d->tx_use_dma
                       ? !drvAxidmaChBusy(d->tx_dma_ch)
                       : (_txSpace(d) == UART_TXFIFO_SIZE);
        }
        osiExitCritical(sc);
        if (done)
            return true;
        osiThreadSleep(5);
    }
    return false;
}

bool drvUartWaitRxAvail(drvUart_t *d, unsigned timeout)
{
    if (d == NULL || !d->opened)
        return false;

    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);
    for (;;)
    {
        if (osiFifoBytes(&d->rx_fifo) > 0)
            return true;

        if (timeout == OSI_WAIT_FOREVER)
        {
            osiSemaphoreAcquire(d->recv_avail_sema);
        }
        else
        {
            int remained = timeout - osiElapsedTime(&timer);
            if (remained <= 0)
                return false;

            osiSemaphoreTryAcquire(d->recv_avail_sema, remained);
        }
    }
}

void drvUartSetAutoSleep(drvUart_t *d, int timeout)
{
    osiPmWakeLock(d->pm_source);
    d->sleep_timeout = timeout;
    if (d->sleep_timeout >= 0)
        osiTimerStart(d->sleep_timer, d->sleep_timeout);
    else
        osiTimerStop(d->sleep_timer);
}

static void _destroy(drvUart_t *d)
{
    if (d->tx_dma_ch)
        drvAxidmaChRelease(d->tx_dma_ch);
    if (d->rx_dma_ch)
        drvAxidmaChRelease(d->rx_dma_ch);
    osiSemaphoreDelete(d->recv_sema);
    osiSemaphoreDelete(d->send_sema);
    osiTimerDelete(d->send_timer);
    osiTimerDelete(d->sleep_timer);
    osiPmSourceDelete(d->pm_source);
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
    drvUart_t *d = (drvUart_t *)ctx;
    uint32_t critical = osiEnterCritical();
    if (!d->opened)
    {
        osiExitCritical(critical);
        return;
    }

    if (mode == OSI_SUSPEND_PM1 && d->name == DRV_NAME_UART1 && d->rx_use_dma)
    {
        _uartStartRxDma(d);
    }

    if ((source & OSI_RESUME_ABORT))
    {
        osiExitCritical(critical);
        return;
    }

    // UART1 in AON_DOMAIN, not power done during pm1
    if (mode == OSI_SUSPEND_PM2 || d->name != DRV_NAME_UART1)
    {
        _setHWRegs(d);
        _setBaud(d, d->cfg.baud);
        _startConfig(d);
    }
    osiExitCritical(critical);

    bool wake_by_uart1 = (source & (HAL_RESUME_SRC_UART1 | HAL_RESUME_SRC_UART1_RXD)) != 0;
    if (d->name == DRV_NAME_UART1 && wake_by_uart1)
    {
        osiPmWakeLock(d->pm_source);
        _startSleepTimer(d);
    }
}

static void _uartSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvUart_t *d = (drvUart_t *)ctx;
    uint32_t critical = osiEnterCritical();
    if (!d->opened)
    {
        osiExitCritical(critical);
        return;
    }

    if (mode == OSI_SUSPEND_PM1 && d->name == DRV_NAME_UART1 && d->rx_use_dma)
    {
        unsigned count = drvAxidmaChStop(d->rx_dma_ch);
        uint32_t recv = d->rx_dma_size - count;
        if (recv != 0)
            OSI_LOGE(0, "DRV %4c suspend with dma data %u", d->name, recv);
    }
    osiExitCritical(critical);
}

static const osiPmSourceOps_t _uartPmOps = {
    .suspend = _uartSuspend,
    .resume = _uartResume,
};

drvUart_t *drvUartCreate(uint32_t name, drvUartCfg_t *cfg)
{
    drvAxidmaCh_t *rx_dma_ch = FORCE_RX_FIFO_MODE ? NULL : drvAxidmaChAllocate();
    drvAxidmaCh_t *tx_dma_ch = FORCE_TX_FIFO_MODE ? NULL : drvAxidmaChAllocate();
    uint32_t rx_dma_size = (rx_dma_ch == NULL) ? 0 : (2 * UART_RXFIFO_SIZE);
    uint32_t tx_dma_size = (tx_dma_ch == NULL) ? 0 : OSI_ALIGN_UP(cfg->tx_buf_size / 2 + 4, CONFIG_CACHE_LINE_SIZE);
    uint32_t alloc_buf_size = cfg->rx_buf_size + cfg->tx_buf_size +
                              CONFIG_CACHE_LINE_SIZE + rx_dma_size + tx_dma_size;

    drvUart_t *d = (drvUart_t *)calloc(1, sizeof(drvUart_t));
    if (d == NULL)
        return NULL;

    uintptr_t pextra = (uintptr_t)malloc(alloc_buf_size);
    if (pextra == 0)
    {
        free(d);
        return NULL;
    }

    d->alloc_buf = (void *)pextra;
    uintptr_t rx_buf = OSI_PTR_INCR_POST(pextra, cfg->rx_buf_size);
    uintptr_t tx_buf = OSI_PTR_INCR_POST(pextra, cfg->tx_buf_size);
    pextra = OSI_ALIGN_UP(pextra, CONFIG_CACHE_LINE_SIZE);
    uintptr_t rx_dma_buf = OSI_PTR_INCR_POST(pextra, rx_dma_size);
    uintptr_t tx_dma_buf = OSI_PTR_INCR_POST(pextra, tx_dma_size);

    d->name = name;
    d->cfg = *cfg;
    osiFifoInit(&d->rx_fifo, (void *)rx_buf, cfg->rx_buf_size);
    osiFifoInit(&d->tx_fifo, (void *)tx_buf, cfg->tx_buf_size);
    d->tx_complete_needed = false;
    d->tx_use_dma = false;
    d->rx_use_dma = false;
    d->tx_dma_ch = tx_dma_ch;
    d->tx_dma_size = tx_dma_size;
    d->tx_dma_buf = (uint8_t *)tx_dma_buf;
    d->tx_use_dma = (tx_dma_ch != NULL);

    d->rx_dma_ch = rx_dma_ch;
    d->rx_dma_size = rx_dma_size;
    d->rx_dma_buf = (uint8_t *)rx_dma_buf;
    d->rx_use_dma = (rx_dma_ch != NULL);

    d->send_sema = osiSemaphoreCreate(1, 1);
    d->recv_sema = osiSemaphoreCreate(1, 1);
    d->recv_avail_sema = osiSemaphoreCreate(1, 0);
    d->send_timer = osiTimerCreate(NULL, _sendTimeout, d);
    d->opened = false;
    d->pm_source = osiPmSourceCreate(name, &_uartPmOps, d);
    d->sleep_timeout = -1;
    d->sleep_timer = osiTimerCreate(NULL, _sleepTimeout, d);
    return d;
}

bool drvUartOpen(drvUart_t *d)
{
    if (!_setHWRegs(d))
        goto failed;
    if (!_setBaud(d, d->cfg.baud))
        goto failed;
    if (!_startConfig(d))
        goto failed;

    d->opened = true;
    osiPmWakeLock(d->pm_source);
    return true;

failed:
    drvUartClose(d);
    return false;
}

void drvUartClose(drvUart_t *d)
{
    if (d->tx_use_dma)
        drvAxidmaChStop(d->tx_dma_ch);

    if (d->rx_use_dma)
        drvAxidmaChStop(d->rx_dma_ch);

    osiIrqDisable(d->irqn);
    osiIrqSetHandler(d->irqn, NULL, NULL);
    d->opened = false;

    // for auto baud
    REG_ARM_UART_UART_CONF_T uart_conf = {d->hwp->uart_conf};
    uart_conf.b.at_enable = 0;
    d->hwp->uart_conf = uart_conf.v;

    osiPmWakeUnlock(d->pm_source);
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
    if (d == NULL || d->opened)
        return false;

    if (cfg == NULL)
        return true;

    if (d->cfg.tx_buf_size != cfg->tx_buf_size ||
        d->cfg.rx_buf_size != cfg->rx_buf_size)
    {
        uint32_t rx_dma_size = d->rx_dma_size;
        uint32_t tx_dma_size = d->tx_use_dma ? OSI_ALIGN_UP(cfg->tx_buf_size / 2 + 4, CONFIG_CACHE_LINE_SIZE) : 0;
        uint32_t alloc_size = cfg->tx_buf_size + cfg->rx_buf_size +
                              CONFIG_CACHE_LINE_SIZE + tx_dma_size + rx_dma_size;

        uintptr_t pextra = (uintptr_t)malloc(alloc_size);
        if (pextra == 0)
            return false;

        free(d->alloc_buf);
        d->alloc_buf = (void *)pextra;

        uintptr_t rx_buf = OSI_PTR_INCR_POST(pextra, cfg->rx_buf_size);
        uintptr_t tx_buf = OSI_PTR_INCR_POST(pextra, cfg->tx_buf_size);
        pextra = OSI_ALIGN_UP(pextra, CONFIG_CACHE_LINE_SIZE);
        uintptr_t rx_dma_buf = OSI_PTR_INCR_POST(pextra, rx_dma_size);
        uintptr_t tx_dma_buf = OSI_PTR_INCR_POST(pextra, tx_dma_size);
        osiFifoInit(&d->rx_fifo, (void *)rx_buf, cfg->rx_buf_size);
        osiFifoInit(&d->tx_fifo, (void *)tx_buf, cfg->tx_buf_size);
        d->tx_dma_size = tx_dma_size;
        d->tx_dma_buf = (uint8_t *)tx_dma_buf;
        d->rx_dma_size = rx_dma_size;
        d->rx_dma_buf = (uint8_t *)rx_dma_buf;
    }

    d->cfg = *cfg;
    return true;
}

int drvUartReadAvail(drvUart_t *d)
{
    if (d == NULL)
        return -1;

    if (!d->opened)
        return 0;

    return osiFifoBytes(&d->rx_fifo);
}

int drvUartWriteAvail(drvUart_t *d)
{
    if (d == NULL)
        return -1;

    if (!d->opened)
        return 0;

    return osiFifoSpace(&d->tx_fifo);
}
