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
#include "hwregs.h"
#include "drv_spi.h"
#include "drv_ifc.h"
#include "drv_names.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "osi_log.h"
#include "osi_api.h"
#include "hal_iomux.h"
#include "osi_compiler.h"

typedef struct
{
    HWP_SPI_T *hwp;
    uint32_t irqn;
    uint32_t prio;
} spiMasterHw_t;

#if defined(CONFIG_SOC_8910)
static spiMasterHw_t spi_masters[] = {
    {.hwp = hwp_spi1, .irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_SPI_1), .prio = SYS_IRQ_PRIO_SPI_1},
    {.hwp = hwp_spi2, .irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_SPI_2), .prio = SYS_IRQ_PRIO_SPI_2},
};
#elif defined(CONFIG_SOC_8909)
static spiMasterHw_t spi_masters[] = {
    {.hwp = hwp_spi1, .irqn = SYS_IRQ_SPI1, .prio = SYS_IRQ_PRIO_SPI1},
    {.hwp = hwp_spi2, .irqn = SYS_IRQ_SPI2, .prio = SYS_IRQ_PRIO_SPI2},
};
#elif defined(CONFIG_SOC_8955)
static spiMasterHw_t spi_masters[] = {
    {.hwp = hwp_spi1, .irqn = SYS_IRQ_SPI1, .prio = SYS_IRQ_PRIO_SPI1},
    {.hwp = hwp_spi2, .irqn = SYS_IRQ_SPI2, .prio = SYS_IRQ_PRIO_SPI2}},
}
;
#endif

#define FIFOSIZE 16
#define DummyData 0xff
#define SPI_MASTER_COUNT OSI_ARRAY_SIZE(spi_masters)
#define IS_SPI_ALL_PERIPH(PERIPH) (((PERIPH) == DRV_NAME_SPI1) || ((PERIPH) == DRV_NAME_SPI2) || ((PERIPH) == DRV_NAME_SPI3))
#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_CPOL_LOW) || ((CPOL) == SPI_CPOL_HIGH))
#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_CPHA_1Edge) || ((CPHA) == SPI_CPHA_2Edge))
#define IS_SPI_CS(CS) (((CS) == SPI_I2C_CS0) || ((CS) == SPI_I2C_CS1) || ((CS) == SPI_I2C_CS2))
#define IS_SPI_CSPOL(CS_POL) (((CS_POL) == SPI_CS_ACTIVE_HIGH) || ((CS_POL) == SPI_CS_ACTIVE_LOW))
#define IS_SPI_DI(DI) (((DI) == SPI_DI_0) || ((DI) == SPI_DI_1) || ((DI) == SPI_DI_2))

typedef enum
{
    SPI_HALF_CLK_DELAY_0,
    SPI_HALF_CLK_DELAY_1,
    SPI_HALF_CLK_DELAY_2,
    SPI_HALF_CLK_DELAY_3,
} drvSpiDelay;

struct drvSpiMaster
{
    HWP_SPI_T *hwp;
    uint32_t irqn;
    uint32_t prio;
    drvSpiConfig_t config;
    HWP_SPI_T hwp_default;
    drvIfcChannel_t rx_ifc;
    drvIfcChannel_t tx_ifc;
    void (*callback)(drvSpiIrq cause);
    osiMutex_t *access_lock;
    osiPmSource_t *pm_source;
};

static drvSpiMaster_t gSpiMaster[SPI_MASTER_COUNT] = {};

static void _spiSuspend(void *ctx, osiSuspendMode_t mode)
{
}

static void _spiResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
}
static const osiPmSourceOps_t _spiPmOps = {
    .suspend = _spiSuspend,
    .resume = _spiResume,
};

static void _setclock(drvSpiMaster_t *d)
{
    uint32_t new_clk_scale;
    REG_SPI_CFG_T cfg;
    cfg.v = hwp_spi1->cfg;
    new_clk_scale = CONFIG_DEFAULT_SYSAHB_FREQ / (2 * d->config.baud) - 1;
    cfg.b.clock_divider = new_clk_scale;
    hwp_spi1->cfg = cfg.v;
}

static void _spiEnable(drvSpiMaster_t *d, bool en)
{
    REG_SPI_CTRL_T spi_ctrl;
    spi_ctrl.v = d->hwp->ctrl;
    if (en)
    {
        spi_ctrl.b.enable = 1;
    }
    else
    {
        spi_ctrl.b.enable = 0;
    }
    d->hwp->ctrl = spi_ctrl.v;
}

static void _setCsSet(drvSpiMaster_t *d, drvSpiCsSel cs)
{
    REG_SPI_CTRL_T spi_ctrl;
    spi_ctrl.v = d->hwp->ctrl;
    spi_ctrl.b.cs_sel = cs;
    d->hwp->ctrl = spi_ctrl.v;
}

static void _setConfig(drvSpiMaster_t *d, drvSpiConfig_t cfg)
{
    d->config.inputEn = cfg.inputEn;
    d->config.oe_delay = cfg.oe_delay;
    d->config.name = cfg.name;
    d->config.framesize = cfg.framesize;
    d->config.baud = cfg.baud;
    d->config.cs_polarity0 = cfg.cs_polarity0;
    d->config.cs_polarity1 = cfg.cs_polarity1;
    d->config.cpol = cfg.cpol;
    d->config.cpha = cfg.cpha;
    d->config.input_sel = cfg.input_sel;
    d->config.transmode = cfg.transmode;
}

static drvSpiMaster_t *_drvSpiInitGet(drvSpiConfig_t cfg)
{
    uint8_t devnum;
    switch (cfg.name)
    {
    case DRV_NAME_SPI1:
        devnum = 0;
        break;
    case DRV_NAME_SPI2:
        devnum = 1;
        break;
    default:
        return NULL;
    }
    if (devnum >= SPI_MASTER_COUNT)
    {
        OSI_LOGE(0, "input device error, %d", devnum);
        return NULL;
    }
    uint32_t sc = osiEnterCritical();
    drvSpiMaster_t *d = &gSpiMaster[devnum];
    d->hwp = spi_masters[devnum].hwp;
    d->irqn = spi_masters[devnum].irqn;
    d->prio = spi_masters[devnum].prio;
    d->access_lock = osiMutexCreate();
    d->hwp_default.ctrl = d->hwp->ctrl;
    d->hwp_default.cfg = d->hwp->cfg;
    d->hwp_default.pattern = d->hwp->pattern;
    d->hwp_default.stream = d->hwp->stream;
    d->hwp_default.pin_control = d->hwp->pin_control;
    d->hwp_default.irq = d->hwp->irq;
    _setConfig(d, cfg);
    drvIfcChannelInit(&d->rx_ifc, d->config.name, DRV_IFC_RX);
    drvIfcChannelInit(&d->tx_ifc, d->config.name, DRV_IFC_TX);
    drvIfcRequestChannel(&d->tx_ifc);
    drvIfcRequestChannel(&d->rx_ifc);
    d->pm_source = osiPmSourceCreate(d->config.name, &_spiPmOps, d);
    OSI_ASSERT(d->access_lock && d->pm_source, "spi init fail");
    osiExitCritical(sc);
    return d;
}

static void _drvSpiFlushFifos(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    spiStatus.b.fifo_flush = 1;
    d->hwp->status = spiStatus.v;
}

static void _drvClearRxDmaDone(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    spiStatus.b.rx_dma_done = 1;
    d->hwp->status = spiStatus.v;
}

static void _drvClearTxDmaDone(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    spiStatus.b.tx_dma_done = 1;
    d->hwp->status = spiStatus.v;
}

void drvSpiEnableStreamMode(drvSpiMaster_t *d, uint8_t stream_bit, bool on)
{
    REG_SPI_STREAM_T stream;
    stream.v = d->hwp->stream;
    if (on)
    {
        stream.b.tx_stream_bit = stream_bit;
        stream.b.tx_stream_mode = 1;
        stream.b.tx_stream_stop_with_rx_dma_done = 1;
    }
    else
    {
        stream.b.tx_stream_bit = 0;
        stream.b.tx_stream_mode = 0;
        stream.b.tx_stream_stop_with_rx_dma_done = 0;
    }
    d->hwp->stream = stream.v;
}

static bool _drvWaitTxFinish(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    if (!(spiStatus.b.active_status) && (spiStatus.b.tx_space == FIFOSIZE))
    {
        return true;
    }
    return false;
}

static bool _drvSpiWaitTxDone(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    if ((spiStatus.b.tx_dma_done) &&
        (drvIfcGetTC(&d->tx_ifc) == 0) &&
        (spiStatus.b.tx_space == FIFOSIZE) &&
        !(spiStatus.b.active_status))
    {
        _drvClearTxDmaDone(d);
        return true;
    }
    return false;
}
static bool _drvSpiWaitRxDone(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    if ((spiStatus.b.rx_dma_done) &&
        (drvIfcGetTC(&d->rx_ifc) == 0))
    {
        _drvClearRxDmaDone(d);
        return true;
    }
    return false;
}

uint8_t drvSpiGetTxFree(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    return spiStatus.b.tx_space;
}

uint8_t drvSpiGetRxLevel(drvSpiMaster_t *d)
{
    REG_SPI_STATUS_T spiStatus;
    spiStatus.v = d->hwp->status;
    return spiStatus.b.rx_level;
}

void drvSpiPinControl(drvSpiMaster_t *d, drvSpiPinCtrl pinctrl, drvSpiPinState pinstate)
{
    REG_SPI_PIN_CONTROL_T pin_ctrl;
    pin_ctrl.v = d->hwp->pin_control;
    switch (pinctrl)
    {
    case SPI_CLK_CTRL:
        pin_ctrl.b.clk_ctrl = pinstate;
        break;
    case SPI_DO_CTRL:
        pin_ctrl.b.do_ctrl = pinstate;
        break;
    case SPI_CS0_CTRL:
        pin_ctrl.b.cs0_ctrl = pinstate;
        break;
    case SPI_CS1_CTRL:
        pin_ctrl.b.cs1_ctrl = pinstate;
        break;
    case SPI_CS2_CTRL:
        pin_ctrl.b.cs2_ctrl = pinstate;
        break;
    default:
        break;
    }
    d->hwp->pin_control = pin_ctrl.v;
}

static uint8_t _drvSpiSend(drvSpiMaster_t *d, uint8_t *sendaddr, uint32_t len)
{
    uint8_t i;
    if (len > 0 && sendaddr != NULL)
    {
        uint8_t txfree = drvSpiGetTxFree(d);
        if (txfree > len)
            txfree = len;
        for (i = 0; i < txfree; i++)
        {
            d->hwp->rxtx_buffer = *(sendaddr + i);
        }

        return txfree;
    }
    return 0;
}

static void _drvSpiIrqHandle(void *ctx)
{
    drvSpiMaster_t *d = (drvSpiMaster_t *)ctx;
    REG_SPI_STATUS_T status;
    drvSpiIrq cause = {
        0,
    };
    status.v = d->hwp->status;
    if (status.b.cause_rx_ovf_irq)
        cause.rxOvf = 1;
    if (status.b.cause_tx_th_irq)
        cause.txTh = 1;
    if (status.b.cause_tx_dma_irq)
        cause.txDmaDone = 1;
    if (status.b.cause_rx_th_irq)
        cause.rxTh = 1;
    if (status.b.cause_rx_dma_irq)
        cause.rxDmaDone = 1;
    d->hwp->status = status.v;
    if (d->callback)
        d->callback(cause);
}

static uint8_t _drvSpiRead(drvSpiMaster_t *d, uint8_t *sendaddr, uint8_t *readaddr, uint32_t len)
{
    uint8_t i;
    if (len > 0 && readaddr != NULL)
    {
        uint8_t rxlevel = FIFOSIZE - drvSpiGetRxLevel(d);
        if (rxlevel > len)
            rxlevel = len;
        for (i = 0; i < rxlevel; i++)
        {
            d->hwp->rxtx_buffer = *(sendaddr + i);
        }
        while (!_drvWaitTxFinish(d))
            ;
        for (i = 0; i < rxlevel; i++)
        {
            *(readaddr + i) = d->hwp->rxtx_buffer;
        }
        return rxlevel;
    }
    return 0;
}

bool drvSpiWrite(drvSpiMaster_t *d, drvSpiCsSel cs, uint8_t *sendaddr, uint32_t len)
{
    OSI_ASSERT(IS_SPI_CS(cs), "SPI CS ERROR");
    if (d != NULL && sendaddr != NULL && len > 0)
    {
        osiMutexLock(d->access_lock);
        _drvSpiFlushFifos(d);
        _setCsSet(d, cs);
        if (d->config.transmode == SPI_DIRECT_POLLING || d->config.transmode == SPI_DIRECT_IRQ)
        {
            uint8_t sendlen;
            while (len)
            {
                sendlen = _drvSpiSend(d, sendaddr, len);
                sendaddr += sendlen;
                len -= sendlen;
            }
            while (!_drvWaitTxFinish(d))
                ;
            osiMutexLock(d->access_lock);
            return true;
        }
        else if (d->config.transmode == SPI_DMA_POLLING || d->config.transmode == SPI_DMA_IRQ)
        {
            osiDCacheClean(sendaddr, len);
            drvIfcFlush(&d->tx_ifc);
            drvIfcStart(&d->tx_ifc, sendaddr, len);
            while (!_drvSpiWaitTxDone(d))
                ;
            osiMutexUnlock(d->access_lock);
            return true;
        }
        else
        {
            osiMutexUnlock(d->access_lock);
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool drvSpiRead(drvSpiMaster_t *d, drvSpiCsSel cs, uint8_t *sendaddr, uint8_t *readaddr, uint32_t len)
{
    OSI_ASSERT(IS_SPI_CS(cs), "SPI CS ERROR");
    if (d != NULL && readaddr != NULL && len > 0)
    {
        osiMutexLock(d->access_lock);
        _drvSpiFlushFifos(d);
        _setCsSet(d, cs);
        if (d->config.transmode == SPI_DIRECT_POLLING || d->config.transmode == SPI_DIRECT_IRQ)
        {
            uint8_t readlen;
            while (len)
            {
                readlen = _drvSpiRead(d, sendaddr, readaddr, len);
                readaddr += readlen;
                sendaddr += readlen;
                len -= readlen;
            }
            osiMutexLock(d->access_lock);
            return true;
        }
        else if (d->config.transmode == SPI_DMA_POLLING || d->config.transmode == SPI_DMA_IRQ)
        {
            osiDCacheClean(sendaddr, len);
            osiDCacheClean(readaddr, len);
            drvIfcFlush(&d->tx_ifc);
            drvIfcFlush(&d->rx_ifc);
            drvIfcStart(&d->rx_ifc, readaddr, len);
            drvIfcStart(&d->tx_ifc, sendaddr, len);
            while (!_drvSpiWaitRxDone(d))
                ;
            osiDCacheInvalidate(readaddr, len);
            osiMutexUnlock(d->access_lock);
            return true;
        }
        else
        {
            osiMutexUnlock(d->access_lock);
            return false;
        }
    }
    else
    {
        return false;
    }
}

drvSpiMaster_t *drvSpiMasterAcquire(drvSpiConfig_t cfg)
{
    REG_SPI_CTRL_T spi_ctrl;
    REG_SPI_CFG_T spi_cfg;

    OSI_ASSERT(IS_SPI_ALL_PERIPH(cfg.name), "SPI NAME ERROR");
    OSI_ASSERT(IS_SPI_CPOL(cfg.cpol), "SPI CPOL ERROR");
    OSI_ASSERT(IS_SPI_CPHA(cfg.cpha), "SPI CPHA ERROR");
    OSI_ASSERT(IS_SPI_CSPOL(cfg.cs_polarity0), "SPI CS POL ERROR");
    OSI_ASSERT(IS_SPI_CSPOL(cfg.cs_polarity1), "SPI CS POL ERROR");
    OSI_ASSERT(IS_SPI_DI(cfg.input_sel), "SPI INPUT SEL ERROR");

    drvSpiMaster_t *d = _drvSpiInitGet(cfg);
    if (d != NULL)
    {
        _setclock(d);
        _spiEnable(d, true);
        spi_ctrl.v = d->hwp->ctrl;
        spi_ctrl.b.input_mode = d->config.inputEn;
        spi_ctrl.b.clock_polarity = d->config.cpol;
        spi_ctrl.b.frame_size = d->config.framesize - 1;
        spi_ctrl.b.oe_delay = d->config.oe_delay;
        if (d->config.cpha == SPI_CPHA_1Edge)
        {
            spi_ctrl.b.do_delay = SPI_HALF_CLK_DELAY_1;
            spi_ctrl.b.clock_delay = SPI_HALF_CLK_DELAY_2;
            spi_ctrl.b.di_delay = SPI_HALF_CLK_DELAY_2;
        }
        else
        {
            spi_ctrl.b.do_delay = SPI_HALF_CLK_DELAY_2;
            spi_ctrl.b.clock_delay = SPI_HALF_CLK_DELAY_2;
            spi_ctrl.b.di_delay = SPI_HALF_CLK_DELAY_3;
        }
        spi_ctrl.b.cs_delay = SPI_HALF_CLK_DELAY_3;
        spi_ctrl.b.cs_pulse = SPI_HALF_CLK_DELAY_0;
        spi_ctrl.b.input_sel = d->config.input_sel;

        spi_cfg.v = d->hwp->cfg;
        spi_cfg.b.cs_polarity_0 = d->config.cs_polarity0;
        spi_cfg.b.cs_polarity_1 = d->config.cs_polarity1;
        spi_cfg.b.clock_limiter = 0;

        d->hwp->ctrl = spi_ctrl.v;
        d->hwp->cfg = spi_cfg.v;
        return d;
    }
    return NULL;
}

void drvSpiSetIrq(drvSpiMaster_t *d, drvSpiIrq *mask, callback callfunc)
{
    REG_SPI_IRQ_T irq;
    irq.b.mask_rx_ovf_irq = mask->rxOvf;
    irq.b.mask_tx_th_irq = mask->txTh;
    irq.b.mask_tx_dma_irq = mask->txDmaDone;
    irq.b.mask_rx_th_irq = mask->rxTh;
    irq.b.mask_rx_dma_irq = mask->rxDmaDone;
    irq.b.tx_threshold = mask->Tx_Rthreshold;
    irq.b.rx_threshold = mask->Rx_Tthreshold;
    d->hwp->irq = irq.v;
    d->callback = callfunc;
    osiIrqDisable(d->irqn);
    osiIrqSetHandler(d->irqn, _drvSpiIrqHandle, d);
    osiIrqSetPriority(d->irqn, d->prio);
    osiIrqEnable(d->irqn);
}

void drvSpiClrIrq(drvSpiMaster_t *d)
{
    REG_SPI_IRQ_T irq;
    irq.v = 0;
    d->hwp->irq = irq.v;
    d->callback = NULL;
    osiIrqDisable(d->irqn);
}

void drvSpiMasterRelease(drvSpiMaster_t *d)
{
    d->irqn = 0;
    d->hwp->ctrl = d->hwp_default.ctrl;
    d->hwp->cfg = d->hwp_default.cfg;
    d->hwp->pattern = d->hwp_default.pattern;
    d->hwp->stream = d->hwp_default.stream;
    d->hwp->pin_control = d->hwp_default.pin_control;
    d->hwp->irq = d->hwp_default.irq;
    osiMutexDelete(d->access_lock);
    drvIfcReleaseChannel(&d->rx_ifc);
    drvIfcReleaseChannel(&d->tx_ifc);
    osiPmSourceDelete(d->pm_source);
}
