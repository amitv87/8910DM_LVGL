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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('P', 'M', 'I', 'C')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_pmic_intr.h"
#include "drv_gpio.h"
#include "hal_adi_bus.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include <string.h>

typedef struct
{
    signed char bit;
    drvPmicIntrCB_t cb;
    void *cb_ctx;
} drvPmicEicConfig_t;

typedef struct
{
    signed char bit;
    drvPmicIntrCB_t cb;
    void *cb_ctx;
} drvPmicIntrConfig_t;

typedef struct
{
    drvGpio_t *gpio;

    drvPmicIntrConfig_t intr[DRV_PMIC_INTR_COUNT];
    drvPmicEicConfig_t eic[DRV_PMIC_EIC_COUNT];
} drvPmicIntrContext_t;

static drvPmicIntrContext_t gDrvPmicIntrCtx;

static void _pmicISR(void *ctx)
{
    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;

    REG_RDA2720M_INT_INT_MASK_STATUS_T status = {};
    status.v = halAdiBusRead(&hwp_rda2720mInt->int_mask_status);

    OSI_LOGD(0, "PMIC intr 0x%08x", status.v);

    uint32_t disable_mask = status.v;
    for (unsigned n = 0; n < DRV_PMIC_INTR_COUNT; n++)
    {
        drvPmicIntrConfig_t *intr = &p->intr[n];
        if (intr->cb == NULL)
            continue;

        unsigned mask = 1 << intr->bit;
        if ((status.v & mask) == 0)
            continue;

        disable_mask &= ~mask;
        intr->cb(intr->cb_ctx);
    }

    // disable non-registered interrupts
    if (disable_mask != 0)
    {
        halAdiBusBatchChange(
            &hwp_rda2720mInt->int_en, disable_mask, 0,
            HAL_ADI_BUS_CHANGE_END);
    }
}

static void _pmicEicISR(void *ctx)
{
    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    uint32_t mis = halAdiBusRead(&hwp_rda2720mEic->eicmis);

    // disable and clear interrupt
    REG_RDA2720M_INT_INT_EN_T int_en;
    halAdiBusBatchChange(
        &hwp_rda2720mEic->eicie, mis, 0,
        &hwp_rda2720mEic->eicic, mis, mis,
        &hwp_rda2720mInt->int_en,
        REG_FIELD_MASKVAL1(int_en, eic_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);

    for (int n = 0; n < DRV_PMIC_EIC_COUNT; n++)
    {
        drvPmicEicConfig_t *ec = &p->eic[n];
        if ((mis & (1 << ec->bit)) == 0)
            continue;

        if (ec->cb != NULL)
            ec->cb(ec->cb_ctx);
    }
}

void drvPmicIntrInit(void)
{
    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    memset(p, 0, sizeof(*p));

    p->intr[DRV_PMIC_INTR_EIC].cb = _pmicEicISR;
    p->intr[DRV_PMIC_INTR_EIC].cb_ctx = NULL;

    for (int n = 0; n < DRV_PMIC_INTR_COUNT; n++)
        p->intr[n].bit = n;
    for (int n = 0; n < DRV_PMIC_EIC_COUNT; n++)
        p->eic[n].bit = n;

    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_GLOBAL_RTC_CLK_EN0_T rtc_clk_en0;
    REG_RDA2720M_INT_INT_EN_T int_en;
    halAdiBusBatchChange(
        // disable all interrupts
        &hwp_rda2720mInt->int_en, HAL_ADI_BUS_OVERWITE(0),
        // enable EIC
        &hwp_rda2720mGlobal->module_en0,
        REG_FIELD_MASKVAL1(module_en0, eic_en, 1),
        // enable EIC RTC clock
        &hwp_rda2720mGlobal->rtc_clk_en0,
        REG_FIELD_MASKVAL1(rtc_clk_en0, rtc_eic_en, 1),
        // disable all EIC interrupts
        &hwp_rda2720mEic->eicie, HAL_ADI_BUS_OVERWITE(0),
        // enable EIC interrupt
        &hwp_rda2720mInt->int_en,
        REG_FIELD_MASKVAL1(int_en, eic_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);

    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_INPUT,
        .intr_enabled = true,
        .intr_level = true,
        .rising = true,
        .falling = false,
        .debounce = false,
    };

    p->gpio = drvGpioOpen(CONFIG_PMIC_INTR_GPIO, &cfg, _pmicISR, NULL);
    if (p->gpio == NULL)
        osiPanic();
}

void drvPmicIntrEnable(unsigned intr, drvPmicIntrCB_t cb, void *ctx)
{
    if (intr >= DRV_PMIC_INTR_COUNT)
        return;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    int bit = p->intr[intr].bit;
    if (bit < 0)
        return;

    p->intr[intr].cb = cb;
    p->intr[intr].cb_ctx = ctx;
    halAdiBusBatchChange(
        &hwp_rda2720mInt->int_en, (1 << bit), (1 << bit),
        HAL_ADI_BUS_CHANGE_END);
}

void drvPmicIntrDisable(unsigned intr)
{
    if (intr >= DRV_PMIC_INTR_COUNT)
        return;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    int bit = p->intr[intr].bit;
    if (bit < 0)
        return;

    p->intr[intr].cb = NULL;
    p->intr[intr].cb_ctx = NULL;
    halAdiBusBatchChange(
        &hwp_rda2720mInt->int_en, (1 << bit), 0,
        HAL_ADI_BUS_CHANGE_END);
}

void drvPmicEicSetCB(unsigned eic, drvPmicIntrCB_t cb, void *ctx)
{
    if (eic >= DRV_PMIC_EIC_COUNT)
        return;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    drvPmicEicConfig_t *ec = &p->eic[eic];
    ec->cb = cb;
    ec->cb_ctx = ctx;
}

void drvPmicEicTrigger(unsigned eic, unsigned debounce, bool level)
{
    OSI_LOGD(0, "PMIC EIC trigger eic/%d dbnc/%d level/%d", eic, debounce, level);
    if (eic >= DRV_PMIC_EIC_COUNT)
        return;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    drvPmicEicConfig_t *ec = &p->eic[eic];
    if (ec->bit < 0)
        return;

    unsigned mask = 1 << ec->bit;
    REG_RDA2720M_EIC_EIC0CTRL_T eic0ctrl;
    halAdiBusBatchChange(
        // disable interrupt
        &hwp_rda2720mEic->eicie, mask, 0,
        // set polaruty
        &hwp_rda2720mEic->eiciev, mask, (level ? mask : 0),
        // set debounce
        &hwp_rda2720mEic->eic0ctrl + ec->bit,
        REG_FIELD_MASKVAL2(eic0ctrl, dbnc_en, 1, dbnc_cnt, debounce),
        // enable interrupt
        &hwp_rda2720mEic->eicie, mask, mask,
        // start trigger
        &hwp_rda2720mEic->eictrig, mask, mask,
        HAL_ADI_BUS_CHANGE_END);
}

bool drvPmicEicGetLevel(unsigned eic)
{
    if (eic >= DRV_PMIC_EIC_COUNT)
        return false;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    drvPmicEicConfig_t *ec = &p->eic[eic];
    if (ec->bit < 0)
        return false;

    unsigned mask = 1 << ec->bit;
    halAdiBusWrite(&hwp_rda2720mEic->eicdmsk, mask);
    unsigned data = halAdiBusRead(&hwp_rda2720mEic->eicdata);
    return (data & mask) != 0;
}

void drvPmicEicDisable(unsigned eic)
{
    if (eic >= DRV_PMIC_EIC_COUNT)
        return;

    drvPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    drvPmicEicConfig_t *ec = &p->eic[eic];
    if (ec->bit < 0)
        return;

    unsigned mask = 1 << ec->bit;
    halAdiBusBatchChange(
        &hwp_rda2720mEic->eicie, mask, 0,
        HAL_ADI_BUS_CHANGE_END);
}
