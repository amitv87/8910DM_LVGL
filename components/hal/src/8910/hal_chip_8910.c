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

#include "osi_api.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "hal_iomux.h"
#include "cmsis_core.h"
#include "osi_log.h"

#include "quec_proj_config.h"

static uint32_t gClock26MUseMap = 0;
static uint8_t gCameraClkFlag = 0;

uint32_t halGetChipId(void)
{
    REG_SYS_CTRL_CFG_CHIP_PROD_ID_T prod_id = {hwp_sysCtrl->cfg_chip_prod_id};
    if (prod_id.b.prod_id != 0x8910)
        osiPanic();

    return HAL_CHIP_ID_8910;
}

uint32_t halGetMetalId(void)
{
    REG_SYS_CTRL_CFG_CHIP_PROD_ID_T prod_id = {hwp_sysCtrl->cfg_chip_prod_id};
    switch (prod_id.b.metal_id)
    {
    case 0:
    default:
        return HAL_METAL_ID_U01;
    }
}

void halChangeSysClk(uint32_t freq, bool extram_access)
{
    ;
}

void halClock26MRequest(clock26MUser_t user)
{
    if ((gClock26MUseMap & user) == user)
        return;

    uint32_t sc = osiEnterCritical();

    if (gClock26MUseMap == 0)
        hwp_rfReg->ana_ctrl_reg32 = 1;

    gClock26MUseMap |= user;

    REG_CP_SYSREG_RF_ANA_26M_CTRL_T ana_26m_ctrl = {};
    if (user == CLK_26M_USER_AUX1)
    {
        hwp_rfReg->ana_ctrl_reg27 |= (1 << 15);
    }
    else
    {
        if (user == CLK_26M_USER_ADC)
            ana_26m_ctrl.b.enable_clk_26m_audio = 1;
        else if (user == CLK_26M_USER_WCN)
            ana_26m_ctrl.b.enable_clk_26m_wcn = 1;
        else if (user == CLK_26M_USER_AUDIO)
        {
            ana_26m_ctrl.b.enable_clk_26m_audio = 1;
            ana_26m_ctrl.b.enable_clk_26m_aud2ad = 1;
        }

        hwp_sysreg->rf_ana_26m_ctrl_set = ana_26m_ctrl.v;
    }

    osiExitCritical(sc);
}

void halClock26MRelease(clock26MUser_t user)
{
    if ((gClock26MUseMap & user) == 0)
        return;

    uint32_t sc = osiEnterCritical();

    gClock26MUseMap &= ~user;
    REG_CP_SYSREG_RF_ANA_26M_CTRL_T ana_26m_ctrl = {};

    if (user == CLK_26M_USER_AUX1)
    {
        hwp_rfReg->ana_ctrl_reg27 &= ~(1 << 15);
    }
    else
    {
        if (user == CLK_26M_USER_WCN)
            ana_26m_ctrl.b.enable_clk_26m_wcn = 1;
        else if (user == CLK_26M_USER_ADC && (gClock26MUseMap & (CLK_26M_USER_AUDIO | CLK_26M_USER_WCN)) == 0)
            ana_26m_ctrl.b.enable_clk_26m_audio = 1;
        else if (user == CLK_26M_USER_AUDIO)
        {
            if ((gClock26MUseMap & (CLK_26M_USER_ADC | CLK_26M_USER_WCN)) == 0)
                ana_26m_ctrl.b.enable_clk_26m_audio = 1;
            ana_26m_ctrl.b.enable_clk_26m_aud2ad = 1;
        }
        if ((gClock26MUseMap & (CLK_26M_USER_AUDIO | CLK_26M_USER_WCN | CLK_26M_USER_ADC)) == 0)
        {
            ana_26m_ctrl.b.enable_clk_26m_audio = 1;
            ana_26m_ctrl.b.enable_clk_26m_aud2ad = 1;
        }
    }

    hwp_sysreg->rf_ana_26m_ctrl_clr = ana_26m_ctrl.v;

    if (gClock26MUseMap == 0)
        hwp_rfReg->ana_ctrl_reg32 = 0;

    osiExitCritical(sc);
}

void halCameraClockRequest(cameraUser_t user, cameraClk_t Mclk)
{
    if (gCameraClkFlag != 0)
    {
        OSI_LOGI(0, "WARNING: Camera Clk Request fail, flag: %d", gCameraClkFlag);
        return;
    }
    uint32_t sc = osiEnterCritical();

    REG_SYS_CTRL_CFG_CLK_CAMERA_OUT_T cfg_clk_camera_out;
    if (user == CLK_CAMA_USER_CAMERA)
    {
        uint32_t newClkScale;
        cfg_clk_camera_out.v = hwp_sysCtrl->cfg_clk_camera_out;
        cfg_clk_camera_out.b.clk_camera_out_en = SYS_CTRL_CLK_CAMERA_OUT_EN_V_ENABLE;
        hwp_sysCtrl->cfg_clk_camera_out = cfg_clk_camera_out.v;

        cfg_clk_camera_out.v &= SYS_CTRL_CLK_CAMERA_OUT_EN_V_ENABLE;
        cfg_clk_camera_out.b.clk_camera_div_src_sel = SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_V_156_MHZ;
        newClkScale = CONFIG_DEFAULT_CPUPLL_FREQ / 1000000 / Mclk;
        cfg_clk_camera_out.b.clk_camera_out_div = newClkScale & 0x7FF;
        cfg_clk_camera_out.b.clk_camera_div_update = SYS_CTRL_CLK_CAMERA_DIV_UPDATE_V_ENABLE;
        hwp_sysCtrl->cfg_clk_camera_out = cfg_clk_camera_out.v;
    }
    else if (user == CLK_CAMA_USER_AUDIO && Mclk == CAMA_CLK_OUT_FREQ_26M)
    {
        // set camera clock 26M for audio codec
        halIomuxSetFunction(HAL_IOMUX_FUN_CAMERA_REF_CLK_PAD_CAMERA_REF_CLK);

        REG_SYS_CTRL_CLK_OTHERS_ENABLE_T clk_others_enable;
        clk_others_enable.v = hwp_sysCtrl->clk_others_enable;
        clk_others_enable.b.enable_oc_id_cam = SYS_CTRL_MODE_OC_ID_CAM_V_MANUAL;
        hwp_sysCtrl->clk_others_enable = clk_others_enable.v;

        cfg_clk_camera_out.v = hwp_sysCtrl->cfg_clk_camera_out;
        cfg_clk_camera_out.b.clk_camera_out_en = SYS_CTRL_CLK_CAMERA_OUT_EN_V_ENABLE;
        cfg_clk_camera_out.b.clk_camera_div_src_sel = SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_V_26_MHZ;
        hwp_sysCtrl->cfg_clk_camera_out = cfg_clk_camera_out.v;
        osiDelayUS(40);

        REG_ANALOG_REG_PAD_CAMERA_CFG_T pad_camera_cfg;
        pad_camera_cfg.v = hwp_analogReg->pad_camera_cfg;
        pad_camera_cfg.b.pad_camera_ref_clk_wpus = 0;
        pad_camera_cfg.b.pad_camera_ref_clk_se = 0;
        pad_camera_cfg.b.pad_camera_ref_clk_ie = 1;
        pad_camera_cfg.b.pad_camera_ref_clk_drv = 0;
        hwp_analogReg->pad_camera_cfg = pad_camera_cfg.v;
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
	else if(user == CLK_CAMA_USER_AUDIO)
	{
		uint32_t newClkScale;
        // set camera clock 26M for audio codec
        halIomuxSetFunction(HAL_IOMUX_FUN_CAMERA_REF_CLK_PAD_CAMERA_REF_CLK);

        REG_SYS_CTRL_CLK_OTHERS_ENABLE_T clk_others_enable;
        clk_others_enable.v = hwp_sysCtrl->clk_others_enable;
        clk_others_enable.b.enable_oc_id_cam = SYS_CTRL_MODE_OC_ID_CAM_V_MANUAL;
        hwp_sysCtrl->clk_others_enable = clk_others_enable.v;

        cfg_clk_camera_out.v = hwp_sysCtrl->cfg_clk_camera_out;
        cfg_clk_camera_out.b.clk_camera_out_en = SYS_CTRL_CLK_CAMERA_OUT_EN_V_ENABLE;
        cfg_clk_camera_out.b.clk_camera_div_src_sel = SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_V_156_MHZ;
        newClkScale = CONFIG_DEFAULT_CPUPLL_FREQ / 1000000 / Mclk;
        cfg_clk_camera_out.b.clk_camera_out_div = newClkScale & 0x7FF;
        cfg_clk_camera_out.b.clk_camera_div_update = SYS_CTRL_CLK_CAMERA_DIV_UPDATE_V_ENABLE;
        hwp_sysCtrl->cfg_clk_camera_out = cfg_clk_camera_out.v;

        osiDelayUS(40);

        REG_ANALOG_REG_PAD_CAMERA_CFG_T pad_camera_cfg;
        pad_camera_cfg.v = hwp_analogReg->pad_camera_cfg;
        pad_camera_cfg.b.pad_camera_ref_clk_wpus = 0;
        pad_camera_cfg.b.pad_camera_ref_clk_se = 0;
        pad_camera_cfg.b.pad_camera_ref_clk_ie = 1;
        pad_camera_cfg.b.pad_camera_ref_clk_drv = 0;
        hwp_analogReg->pad_camera_cfg = pad_camera_cfg.v;
    }
#endif
    else
    {
        osiExitCritical(sc);
        OSI_LOGI(0, "Camera Clk Request not success, user: %d, clk: %d", user, Mclk);
        return;
    }
    gCameraClkFlag = user;
    osiExitCritical(sc);
}

void halCameraClockRelease(cameraUser_t user)
{
    if (gCameraClkFlag == 0)
        return;
    uint32_t sc = osiEnterCritical();

    REG_SYS_CTRL_CFG_CLK_CAMERA_OUT_T cfg_clk_camera_out;
    if (user == CLK_CAMA_USER_CAMERA || user == CLK_CAMA_USER_AUDIO)
    {
        cfg_clk_camera_out.v = 0;
        cfg_clk_camera_out.b.clk_camera_out_en = 0;
        hwp_sysCtrl->cfg_clk_camera_out = cfg_clk_camera_out.v;
    }
    else
    {
        osiExitCritical(sc);
        OSI_LOGI(0, "Camera Clk Release not success, user: %d", user);
        return;
    }
    gCameraClkFlag = 0;
    osiExitCritical(sc);
}

void osiDCacheClean(const void *address, size_t size)
{
    __DMB();
    uintptr_t p = (uintptr_t)address;
    uintptr_t end = p + size;
    p &= ~(CONFIG_CACHE_LINE_SIZE - 1);
    for (; p < end; p += CONFIG_CACHE_LINE_SIZE)
        __set_DCCMVAC(p);
    __DSB();
}

void osiDCacheInvalidate(const void *address, size_t size)
{
    uintptr_t p = (uintptr_t)address;
    uintptr_t end = p + size;
    p &= ~(CONFIG_CACHE_LINE_SIZE - 1);
    for (; p < end; p += CONFIG_CACHE_LINE_SIZE)
        __set_DCIMVAC(p);
    __DSB();
}

void osiDCacheCleanInvalidate(const void *address, size_t size)
{
    __DMB();
    uintptr_t p = (uintptr_t)address;
    uintptr_t end = p + size;
    p &= ~(CONFIG_CACHE_LINE_SIZE - 1);
    for (; p < end; p += CONFIG_CACHE_LINE_SIZE)
        __set_DCCIMVAC(p);
    __DSB();
}

void osiICacheInvalidate(const void *address, size_t size)
{
    L1C_InvalidateICacheAll();
}

void osiICacheSync(const void *address, size_t size)
{
    osiDCacheClean(address, size);
    __ISB();
}

void osiDCacheCleanAll(void)
{
    L1C_CleanDCacheAll();
    __DSB();
}

void osiDCacheInvalidateAll(void)
{
    L1C_InvalidateDCacheAll();
    __DSB();
}

void osiDCacheCleanInvalidateAll(void)
{
    L1C_CleanInvalidateDCacheAll();
    __DSB();
}

void osiICacheInvalidateAll(void)
{
    L1C_InvalidateICacheAll();
}

void osiICacheSyncAll(void)
{
    L1C_CleanDCacheAll();
    __DSB();
    __ISB();
}

uint32_t OSI_SECTION_SRAM_TEXT osiIrqSave(void)
{
    uint32_t flags;
    asm volatile(
        " mrs   %0, cpsr\n"
        " cpsid i\n"
        " dsb\n"
        " isb\n"
        : "=r"(flags)
        :
        : "memory", "cc");
    return flags;
}

void OSI_SECTION_SRAM_TEXT osiIrqRestore(uint32_t flags)
{
    asm volatile(
        " msr   cpsr_c, %0\n"
        " dsb\n"
        " isb\n"
        :
        : "r"(flags)
        : "memory", "cc");
}

OSI_DECL_STRONG_ALIAS(osiIrqSave, uint32_t osiEnterCritical(void));
OSI_DECL_STRONG_ALIAS(osiIrqSave, uint32_t uxPortEnterCritical(void));
OSI_DECL_STRONG_ALIAS(osiIrqSave, uint32_t uxPortSetInterruptMaskFromISR(void));
OSI_DECL_STRONG_ALIAS(osiIrqSave, uint32_t vPortDisableInterrupts(void));
OSI_DECL_STRONG_ALIAS(osiIrqRestore, void osiExitCritical(uint32_t flags));
OSI_DECL_STRONG_ALIAS(osiIrqRestore, void vPortExitCritical(uint32_t flags));
OSI_DECL_STRONG_ALIAS(osiIrqRestore, void vPortClearInterruptMaskFromISR(uint32_t flags));
