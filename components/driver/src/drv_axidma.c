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

#include "drv_axidma.h"
#include "drv_names.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdint.h>
#include <hwregs.h>
#include <assert.h>

#define AXIDMA_APCH_COUNT (6) // [0:5] for ap, [6:10] for cp
#define AXIDMA_ALLCH_COUNT (12)
#define REG_WORDS_PER_CH (0x40 / 4)
#define CHANNEL_REG(reg, n) (*(&(reg) + (n)*REG_WORDS_PER_CH))

typedef REG_ARM_AXIDMA_AXIDMA_C0_CONF_T axidmaHwChConf_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_MAP_T axidmaHwChMap_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_COUNT_T axidmaHwChCount_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_COUNTP_T axidmaHwChCountP_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_STATUS_T axidmaHwChStatus_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_SGCONF_T axidmaHwChSgConf_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_SET_T axidmaHwChSet_t;
typedef REG_ARM_AXIDMA_AXIDMA_C0_CLR_T axidmaHwChClr_t;

typedef volatile struct
{
    axidmaHwChConf_t conf;
    axidmaHwChMap_t map;
    REG32 saddr;
    REG32 daddr;
    axidmaHwChCount_t count;
    axidmaHwChCountP_t countp;
    axidmaHwChStatus_t status;
    REG32 sgaddr;
    axidmaHwChSgConf_t sgconf;
    axidmaHwChSet_t set;
    axidmaHwChClr_t clr;
    REG32 Reserved_0000006C[5];
} axidmaHwCh_t;

static_assert(sizeof(axidmaHwCh_t) == REG_WORDS_PER_CH * 4, "axidmaHwCh_t size doesn't match");

struct drv_axidma_channel
{
    axidmaHwCh_t *hw;
    uint8_t id;
    uint8_t req_id;
    uint8_t ack_id;
    uint8_t occupied : 1;
    drvAxidmaIsr_t isr;
    void *param;
};

typedef struct drv_axidma_context
{
    drvAxidmaCh_t axidma_channels[AXIDMA_APCH_COUNT];
    uint32_t channel_count;

    // for suspend resume
    osiPmSource_t *pm_source;
    uint32_t ch_irq_distr;
    uint32_t ch_maps[AXIDMA_ALLCH_COUNT];
} drvAxidmaCtx_t;

static drvAxidmaCtx_t gAxidmaCtx = {};

static void _drvAxidmaIsr(void *ctx_)
{
    uint32_t cc = osiEnterCritical();
    drvAxidmaCtx_t *ctx = (drvAxidmaCtx_t *)ctx_;
    REG_ARM_AXIDMA_AXIDMA_IRQ_STAT_T irq = {hwp_dma->axidma_irq_stat};

    OSI_LOGD(0, "AXI DMA ISR/%08x", irq.v);

    for (uint32_t i = 0; i < ctx->channel_count; ++i)
    {
        if (irq.v & (1 << i))
        {
            drvAxidmaCh_t *channel = &ctx->axidma_channels[i];
            axidmaHwChStatus_t status = channel->hw->status;
            channel->hw->status.v = status.v;

            drvAxidmaIrqEvent_t event = 0;

            if (status.b.countp_finish == 1)
                event |= AD_EVT_PART_FINISH;

            if (status.b.count_finish == 1)
                event |= AD_EVT_FINISH;

            if (status.b.run == 0)
                event |= AD_EVT_STOP;

            if (channel->isr)
                channel->isr(event, channel->param);
        }
    }

    osiExitCritical(cc);
}

static void _drvAxidmaChannelInit(drvAxidmaCtx_t *ctx)
{
    axidmaHwCh_t *const hw_channels = (axidmaHwCh_t *)(&hwp_dma->axidma_c0_conf);
    ctx->channel_count = OSI_ARRAY_SIZE(ctx->axidma_channels);
    for (uint32_t i = 0; i < ctx->channel_count; ++i)
    {
        drvAxidmaCh_t *ch = &ctx->axidma_channels[i];
        ch->hw = &hw_channels[i];
        ch->id = i;
        ch->occupied = 0;
        ch->isr = NULL;
        ch->param = NULL;
    }

    axidmaHwChMap_t map = {};
    map.b.req_source = 0x1f;
    map.b.ack_map = 0x1f;
    for (int n = 0; n < AXIDMA_ALLCH_COUNT; n++)
        CHANNEL_REG(hwp_dma->axidma_c0_map, n) = map.v;
}

static void _drvAxidmaHwInit(drvAxidmaCtx_t *ctx)
{
    // 0: send IRQ to 0x1B
    // 1: send IRQ to 0x1C (conf[14]==0) or 0x1D (conf[14]==1)
    // AP will use IRQ 0x1C
    hwp_dma->axidma_ch_irq_distr |= 0x3f;

    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_AXIDMA_1_SECURITY), _drvAxidmaIsr, ctx);
    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_AXIDMA_1_SECURITY), SYS_IRQ_PRIO_AXIDMA_1_SECURITY);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_AXIDMA_1_SECURITY));
}

static void _drvArmdmaChPrioSet(uint8_t prio_set)
{
    hwp_dma->axidma_conf |= (prio_set << 2) & 0x4;
}

drvAxidmaCh_t *drvAxidmaChAllocate()
{
    drvAxidmaCh_t *ch = NULL;
    drvAxidmaCtx_t *ctx = &gAxidmaCtx;
    uint32_t cc = osiEnterCritical();
    for (uint32_t i = 0; i < OSI_ARRAY_SIZE(ctx->axidma_channels); ++i)
    {
        if (ctx->axidma_channels[i].occupied == 0)
        {
            ch = &ctx->axidma_channels[i];
            ch->occupied = 1;
            break;
        }
    }

    osiExitCritical(cc);
    return ch;
}

void drvAxidmaChRelease(drvAxidmaCh_t *ch)
{
    if (ch == NULL)
        return;

    drvAxidmaChStop(ch);
    uint32_t cc = osiEnterCritical();
    ch->isr = NULL;
    ch->param = NULL;
    ch->occupied = 0;
    osiExitCritical(cc);
}

bool drvAxidmaChStart(drvAxidmaCh_t *ch, const drvAxidmaCfg_t *cfg)
{
    if (ch == NULL || ch->occupied != 1)
        return false;

    if (cfg == NULL || cfg->data_size == 0 || cfg->part_trans_size == 0)
        return false;

    // keep bit14 as 0
    axidmaHwChConf_t conf = {};
    conf.b.data_type = cfg->data_type;
    conf.b.syn_irq = cfg->sync_irq;
    conf.b.saddr_fix = cfg->src_addr_fix;
    conf.b.daddr_fix = cfg->dst_addr_fix;
    conf.b.force_trans = cfg->force_trans;
    conf.b.req_sel = cfg->req_sel_level;
    conf.b.start = 1;
    if (AD_EVT_FINISH & cfg->mask)
        conf.b.irq_f = 1;

    if (AD_EVT_PART_FINISH & cfg->mask)
        conf.b.irq_t = 1;

    uint32_t sc = osiEnterCritical();
    _drvArmdmaChPrioSet(1);
    ch->hw->saddr = cfg->src_addr;
    ch->hw->daddr = cfg->dst_addr;
    ch->hw->count.v = cfg->data_size;
    ch->hw->countp.v = cfg->part_trans_size;
    ch->hw->conf.v = conf.v;
    osiExitCritical(sc);

    return true;
}

unsigned drvAxidmaChStop(drvAxidmaCh_t *ch)
{
    if (ch == NULL)
        return 0;

    axidmaHwChConf_t conf;
    conf.v = ch->hw->conf.v;
    conf.b.start = 0;
    conf.b.irq_t = 0;
    conf.b.irq_f = 0;
    ch->hw->conf.v = conf.v;
    OSI_LOOP_WAIT(ch->hw->status.b.run == 0);

    unsigned count = ch->hw->count.v;
    ch->hw->countp.v = 0;
    ch->hw->count.v = 0;
    ch->hw->status.v = ch->hw->status.v;

    if (hwp_dma->axidma_arm_req_stat & (1 << ch->req_id))
    {
        uint8_t dummy;
        conf.b.start = 1;
        ch->hw->saddr = (uint32_t)&dummy;
        ch->hw->daddr = (uint32_t)&dummy;
        ch->hw->count.v = 1;
        ch->hw->countp.v = 1;
        ch->hw->conf.v = conf.v;

        OSI_LOOP_WAIT(ch->hw->status.b.count_finish_sta == 1);

        conf.b.start = 0;
        ch->hw->conf.v = conf.v;
        OSI_LOOP_WAIT(ch->hw->status.b.run == 0);
    }
    return count;
}

bool drvAxidmaChBusy(drvAxidmaCh_t *ch)
{
    if (!ch || ch->occupied != 1)
        return true;
    return ch->hw->status.b.run == 1;
}

void drvAxidmaChSetDmamap(drvAxidmaCh_t *ch, uint8_t req_source, uint8_t ack_map)
{
    if (!ch || ch->occupied != 1)
        return;

    ch->req_id = req_source;
    ch->ack_id = ack_map;

    axidmaHwChMap_t map = {};
    map.b.req_source = req_source;
    map.b.ack_map = ack_map;
    ch->hw->map.v = map.v;
}

uint32_t drvAxidmaChCount(drvAxidmaCh_t *ch)
{
    return ch ? ch->hw->count.b.count : 0;
}

void drvAxidmaStopAll(void)
{
    axidmaHwCh_t *const hw_channels = (axidmaHwCh_t *)(&hwp_dma->axidma_c0_conf);
    for (uint32_t i = 0; i < AXIDMA_APCH_COUNT; ++i)
        hw_channels[i].conf.b.start = 0;
}

void drvAxidmaChRegisterIsr(drvAxidmaCh_t *ch, drvAxidmaIsr_t isr, void *param)
{
    if (!ch || ch->occupied != 1)
        return;

    uint32_t sc = osiEnterCritical();
    ch->isr = isr;
    ch->param = param;
    osiExitCritical(sc);
}

static void _axidmaSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvAxidmaCtx_t *p = &gAxidmaCtx;
    p->ch_irq_distr = hwp_dma->axidma_ch_irq_distr;
    for (int n = 0; n < AXIDMA_ALLCH_COUNT; n++)
        p->ch_maps[n] = CHANNEL_REG(hwp_dma->axidma_c0_map, n);
}

void drvAxidmaResume(osiSuspendMode_t mode, uint32_t source)
{
    if (source & OSI_RESUME_ABORT)
        return;

    drvAxidmaCtx_t *p = &gAxidmaCtx;
    hwp_dma->axidma_ch_irq_distr = p->ch_irq_distr;
    for (int n = 0; n < AXIDMA_ALLCH_COUNT; n++)
        CHANNEL_REG(hwp_dma->axidma_c0_map, n) = p->ch_maps[n];
}

static const osiPmSourceOps_t _axidmaPmOps = {
    .suspend = _axidmaSuspend,
};

void drvAxidmaInit(void)
{
    drvAxidmaCtx_t *ctx = &gAxidmaCtx;
    _drvAxidmaChannelInit(ctx);
    _drvAxidmaHwInit(ctx);

    ctx->pm_source = osiPmSourceCreate(DRV_NAME_AXI_DMA, &_axidmaPmOps, NULL);
}
