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
//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "drv_config.h"
#ifdef CONFIG_HEADSET_DETECT_SUPPORT
#include "drv_pmic_codec.h"
#include "drv_gpio.h"
#include "hal_adi_bus.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include <string.h>
#include "drv_adc.h"
#include "drv_efuse_pmic.h"
#include "audio_device.h"
#include "drv_adc_efuse.h"
#include <stdlib.h>

#define SCI_GetTickCount() (uint32_t) osiUpTime()
#define SCI_Sleep osiThreadSleep

T_HEADPHONE_STATUS g_HeadPhoneStatus = {0, UNKNOWN_MIC_TYPE};

bool g_ap_status = false;

static HEADSET_INFO_T s_headset_cfg =
    {
        2650, // 2.65v large scale 3.233v
        150,  //0.15V
        900,  //0.9V
        50,   //0.05V
        100,  // 0.1v
        0,
        0,
        0,
        0,
        0,
        0,
        939836,
};

bool g_ExtCapChargeFinished = false;

HEADSET_INFO_T_PTR SPRD_CODEC_HEADSET_GetInfo(void)
{
    HEADSET_INFO_T_PTR cfg_ptr = NULL;

    cfg_ptr = &s_headset_cfg;
    return cfg_ptr;
}

// ana_pmu0
void _sprd_codec_ap_headmic_sleep_mode_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (en)
        sc2720m_ana_pmu0.b.rg_aud_hmic_sleep_en = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_hmic_sleep_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_headmic_bias_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (en)
        sc2720m_ana_pmu0.b.rg_aud_hmic_bias_en = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_hmic_bias_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_LDO_vb_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (en)
        sc2720m_ana_pmu0.b.rg_aud_vb_en = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_vb_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_bg_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (en)
        sc2720m_ana_pmu0.b.rg_aud_bg_en = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_bg_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_LDO_vb_sleep_mode_power_down(uint32_t down)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (down)
        sc2720m_ana_pmu0.b.rg_aud_vb_hdmc_sp_pd = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_vb_hdmc_sp_pd = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_vbg_sel(uint32_t voltage_sel)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (voltage_sel)
        sc2720m_ana_pmu0.b.rg_aud_vbg_sel = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_vbg_sel = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_vbg_temp_tune(uint32_t option)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    sc2720m_ana_pmu0.b.rg_aud_vbg_temp_tune = option;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_mic_bias_power_down(uint32_t down)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (down)
        sc2720m_ana_pmu0.b.rg_aud_micbias_plgb = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_micbias_plgb = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

void _sprd_codec_ap_aud_bias_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    if (en)
        sc2720m_ana_pmu0.b.rg_aud_bias_en = 1;
    else
        sc2720m_ana_pmu0.b.rg_aud_bias_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
}

// ana_clk0
void _sprd_codec_ap_ana_clk_digtial_clk_6p5m_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    if (en)
        sc2720m_ana_clk0.b.rg_aud_dig_clk_6p5m_en = 1;
    else
        sc2720m_ana_clk0.b.rg_aud_dig_clk_6p5m_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

void _sprd_codec_ap_ana_clk_ana_core_clk_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    if (en)
        sc2720m_ana_clk0.b.rg_aud_ana_clk_en = 1;
    else
        sc2720m_ana_clk0.b.rg_aud_ana_clk_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

void _sprd_codec_ap_ana_clk_DRV_clk_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    if (en)
        sc2720m_ana_clk0.b.rg_drv_clk_en = 1;
    else
        sc2720m_ana_clk0.b.rg_drv_clk_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

void _sprd_codec_ap_ana_clk_DAC_clk_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    if (en)
        sc2720m_ana_clk0.b.rg_aud_da_clk_en = 1;
    else
        sc2720m_ana_clk0.b.rg_aud_da_clk_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

void _sprd_codec_ap_ana_clk_ADC_clk_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    if (en)
        sc2720m_ana_clk0.b.rg_aud_ad_clk_en = 1;
    else
        sc2720m_ana_clk0.b.rg_aud_ad_clk_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

void _sprd_codec_ap_ana_clk_ADC_clk_rst()
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK0_T sc2720m_ana_clk0;
    sc2720m_ana_clk0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk0);
    sc2720m_ana_clk0.b.rg_aud_ad_clk_rst = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
    sc2720m_ana_clk0.b.rg_aud_ad_clk_rst = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk0, sc2720m_ana_clk0.v);
}

// ana_cdc1
void _sprd_codec_ap_adc_VCMI_sel(uint32_t sel)
{
    REG_RDA2720M_AUD_CODEC_ANA_CDC1_T sc2720m_ana_cdc1;
    sc2720m_ana_cdc1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc1);
    sc2720m_ana_cdc1.b.rg_aud_advcmi_int_sel = sel;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc1, sc2720m_ana_cdc1.v);
}

void _sprd_codec_ap_DAC_dc_offset_trim(uint32_t offset_trim)
{
    REG_RDA2720M_AUD_CODEC_ANA_CDC1_T sc2720m_ana_cdc1;
    sc2720m_ana_cdc1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc1);
    sc2720m_ana_cdc1.b.rg_aud_dalr_os_d = offset_trim;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc1, sc2720m_ana_cdc1.v);
}

// ana_cdc2
void _sprd_codec_ap_hp_buf_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CDC2_T sc2720m_ana_cdc2;
    if (!en)
    {
        sc2720m_ana_cdc2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc2);
        if ((sc2720m_ana_cdc2.b.rg_aud_rcv_en) ||
            (sc2720m_ana_cdc2.b.rg_aud_hpl_en) ||
            (sc2720m_ana_cdc2.b.rg_aud_hpr_en))
        {
            OSI_LOGI(0, "_sprd_codec_ap_hp_buf_en close buf fail cdc2 val 0x%x", sc2720m_ana_cdc2.v);
            return;
        }
    }
    if (en)
        sc2720m_ana_cdc2.b.rg_aud_hpbuf_en = 1;
    else
        sc2720m_ana_cdc2.b.rg_aud_hpbuf_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc2, sc2720m_ana_cdc2.v);
}

// ana_cdc3
void _sprd_codec_ap_DAC_dc_offset_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_CDC3_T sc2720m_ana_cdc3;
    sc2720m_ana_cdc3.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc3);
    if (en)
        sc2720m_ana_cdc3.b.rg_aud_dalr_os_en = 1;
    else
        sc2720m_ana_cdc3.b.rg_aud_dalr_os_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc3, sc2720m_ana_cdc3.v);
}

// clk_en
void _sprd_codec_ap_aud_hid_en(uint32_t en)
{
    REG_RDA2720M_AUD_CLK_EN_T sc2720m_clk_en;
    sc2720m_clk_en.v = halAdiBusRead(&hwp_rda2720mAud->clk_en);
    if (en)
        sc2720m_clk_en.b.clk_aud_hid_en = 1;
    else
        sc2720m_clk_en.b.clk_aud_hid_en = 0;
    halAdiBusWrite(&hwp_rda2720mAud->clk_en, sc2720m_clk_en.v);
}

void _sprd_codec_ap_aud_6p5m_en(uint32_t en)
{
    REG_RDA2720M_AUD_CLK_EN_T sc2720m_clk_en;
    sc2720m_clk_en.v = halAdiBusRead(&hwp_rda2720mAud->clk_en);
    if (en)
        sc2720m_clk_en.b.clk_aud_6p5m_en = 1;
    else
        sc2720m_clk_en.b.clk_aud_6p5m_en = 0;
    halAdiBusWrite(&hwp_rda2720mAud->clk_en, sc2720m_clk_en.v);
}

void _sprd_codec_ap_aud_1k_en(uint32_t en)
{
    REG_RDA2720M_AUD_CLK_EN_T sc2720m_clk_en;
    sc2720m_clk_en.v = halAdiBusRead(&hwp_rda2720mAud->clk_en);
    if (en)
        sc2720m_clk_en.b.clk_aud_1k_en = 1;
    else
        sc2720m_clk_en.b.clk_aud_1k_en = 0;
    halAdiBusWrite(&hwp_rda2720mAud->clk_en, sc2720m_clk_en.v);
}

void _sprd_codec_ap_aud_32k_en(uint32_t en)
{
    REG_RDA2720M_AUD_CLK_EN_T sc2720m_clk_en;
    sc2720m_clk_en.v = halAdiBusRead(&hwp_rda2720mAud->clk_en);
    if (en)
        sc2720m_clk_en.b.clk_aud_32k_en = 1;
    else
        sc2720m_clk_en.b.clk_aud_32k_en = 0;
    halAdiBusWrite(&hwp_rda2720mAud->clk_en, sc2720m_clk_en.v);
}

// ana_hdt0
static void _sprd_codec_set_auxadc_scale(uint32_t is_big_scale)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (is_big_scale)
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2ad_scale = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2ad_scale = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

static void _sprd_codec_hedet_vref_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_vref_en = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_vref_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

static void _sprd_codec_headmic_detect_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_micdet_en = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_micdet_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

void _sprd_codec_headset_lint_low_det_filter_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_ldet_l_filter = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_ldet_l_filter = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

static void _sprd_codec_headmic_adc_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_buf_en = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_buf_en = 0;
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2i_en = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2i_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

static void _sprd_codec_headset_button_detect_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (en)
        sc2720m_ana_hdt0.b.rg_aud_hedet_bdet_en = 1;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_bdet_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

static void _sprd_codec_set_auxadc_to_headmic(uint32_t is_headmic)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    if (is_headmic)
        sc2720m_ana_hdt0.b.rg_aud_hedet_mux2adc_sel = HEDET_MUX2ADC_SEL_HEADMIC_IN_DETECT;
    else
        sc2720m_ana_hdt0.b.rg_aud_hedet_mux2adc_sel = HEDET_MUX2ADC_SEL_HEADSET_L_INT;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
}

uint32_t _sprd_codec_ap_headmic_adcget(uint32_t is_big_scale)
{
    uint32_t adc_Value = 0;
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    sc2720m_ana_hdt0.b.rg_aud_hedet_buf_chop = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);

    _sprd_codec_set_auxadc_scale(is_big_scale);
    adc_Value = drvAdcGetHMicChannelVolt(ADC_SCALE_1V250, ((is_big_scale) ? HEADMICIN_DET_SCALE1 : HEADMICIN_DET_SCALE0));
    sc2720m_ana_hdt0.b.rg_aud_hedet_buf_chop = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);

    _sprd_codec_set_auxadc_scale(is_big_scale);
    adc_Value = drvAdcGetHMicChannelVolt(ADC_SCALE_1V250, ((is_big_scale) ? HEADMICIN_DET_SCALE1 : HEADMICIN_DET_SCALE0)) + adc_Value;
    OSI_LOGI(0, "adc_value is %d\n", adc_Value / 2);
    return adc_Value / 2;
}

// ana_hdt2
static void _sprd_codec_headset_plugout_det_en(uint32_t en)
{
    //enable audio headset plug out detect
    REG_RDA2720M_AUD_CODEC_ANA_HDT2_T sc2720m_ana_hdt2;
    sc2720m_ana_hdt2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt2);
    if (en)
        sc2720m_ana_hdt2.b.rg_aud_plgpd_en = 1;
    else
        sc2720m_ana_hdt2.b.rg_aud_plgpd_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt2, sc2720m_ana_hdt2.v);
}

static void _sprd_codec_hpl_det_delay_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_HDT2_T sc2720m_ana_hdt2;
    sc2720m_ana_hdt2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt2);
    if (en)
        sc2720m_ana_hdt2.b.rg_aud_hpl_en_d2hdt_en = 1;
    else
        sc2720m_ana_hdt2.b.rg_aud_hpl_en_d2hdt_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt2, sc2720m_ana_hdt2.v);
}

// ana_dcl0
void _sprd_codec_ap_dcl_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    if (en)
        sc2720m_ana_dcl0.b.rg_aud_dcl_en = 1;
    else
        sc2720m_ana_dcl0.b.rg_aud_dcl_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
}

void _sprd_codec_ap_dcl_rst()
{
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    sc2720m_ana_dcl0.b.rg_aud_dcl_rst = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
    sc2720m_ana_dcl0.b.rg_aud_dcl_rst = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
}

void _sprd_codec_ap_depop_module_auto_rst()
{
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    sc2720m_ana_dcl0.b.rg_aud_dpop_auto_rst = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
    sc2720m_ana_dcl0.b.rg_aud_dpop_auto_rst = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
}

void _sprd_codec_ap_drv_delay(uint32_t delay)
{
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    sc2720m_ana_dcl0.b.rg_aud_drv_soft_t = delay;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
}

void _sprd_codec_ap_drv_soft_start_en(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    if (en)
        sc2720m_ana_dcl0.b.rg_aud_drv_soft_en = 1;
    else
        sc2720m_ana_dcl0.b.rg_aud_drv_soft_en = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl0, sc2720m_ana_dcl0.v);
}

// ana_clk1
void _sprd_codec_ap_ana_clk_AUD_CLK_PN_sel(uint32_t clk)
{
    REG_RDA2720M_AUD_CODEC_ANA_CLK1_T sc2720m_ana_clk1;
    sc2720m_ana_clk1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_clk1);
    sc2720m_ana_clk1.b.rg_aud_clk_pn_sel = clk;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_clk1, sc2720m_ana_clk1.v);
}

static void _sprd_codec_dc_calibration_start()
{
    REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    sc2720m_ana_sts2.b.rg_caldc_start = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
    sc2720m_ana_sts2.b.rg_caldc_start = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
}

static void _sprd_codec_ap_depop_charge_start(uint32_t en)
{
    REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    if (en)
        sc2720m_ana_sts2.b.rg_depop_chg_start = 1;
    else
        sc2720m_ana_sts2.b.rg_depop_chg_start = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
}
/*
static void _sprd_codec_headset_adc_cal_from_efuse(void)
{
    uint8_t delta[4] = {0};
    uint32_t block0_bit7 = 128;
    uint32_t test[2] = {0};
    //unsigned int adie_chip_id;
    OSI_LOGXI(OSI_LOGPAR_SI, 0, "[%s][%d]to get calibration data from efuse ...\n", __FUNCTION__, __LINE__);
    if (g_SC2720_ADC_CAL_HEADSET.cal_type != SC2720_HEADSET_AUXADC_CAL_NO)
    {
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "[%s][%d]efuse A,B,E has been calculated already!\n", __FUNCTION__, __LINE__);
        return;
    }

    uint32_t result;
    //uint32_t read_data;

    drvEfusePmicOpen();
    result = drvEfusePmicRead(20, &test[0]); //EFUSE_PHY_BLK_ID_20
    if (!result)
        OSI_LOGI(0, "block[%d] = 0x%08x", 20, test[0]);
    else
        OSI_LOGI(0, "block[%d], error  result = 0x%08x", 20, result);

    result = drvEfusePmicRead(22, &test[1]); //EFUSE_PHY_BLK_ID_22
    if (!result)
        OSI_LOGI(0, "block[%d] = 0x%08x", 22, test[1]);
    else
        OSI_LOGI(0, "block[%d], error  result = 0x%08x", 22, result);

    delta[0] = test[0] & 0xFF;
    delta[1] = (test[0] & 0xFF00) >> 8;
    delta[2] = test[1] & 0xFF;
    delta[3] = (test[1] & 0xFF00) >> 8;
    OSI_LOGI(0, "test[0] 0x%x %d, test[1] 0x%x %d\n", test[0], test[0], test[1], test[1]);
    OSI_LOGI(0, "d[0] %#x %d d[1] %#x %d d[2] %#x %d d[3] %#x %d\n",
             delta[0], delta[0], delta[1], delta[1],
             delta[2], delta[2], delta[3], delta[3]);

    result = drvEfusePmicRead(0, &block0_bit7); //EFUSE_PHY_BLK_ID_0
    if (!result)
        OSI_LOGI(0, "block[%d] = 0x%08x", 0, block0_bit7);
    else
        OSI_LOGI(0, "block[%d], error  result = 0x%08x", 0, result);

    //block0_bit7 = sprd_pmic_efuse_read_bits(0, SC2720_BITCOUNT);

    OSI_LOGI(0, "block_7 0x%08x\n", block0_bit7);
    //SCI_TRACE_LOW("block_7 0x%08x\n", block0_bit7);
    if (!(block0_bit7 & (1 << SC2720_PROTECT_BIT)))
    {

        OSI_LOGI(0, "block 0 bit 7 set 1 no efuse data\n");
        return;
    }
    drvEfusePmicClose();

    g_SC2720_ADC_CAL_HEADSET.cal_type = SC2720_HEADSET_AUXADC_CAL_DO;
    OSI_LOGI(0, "block 0 bit 7 set 0 have efuse data\n");
    g_SC2720_ADC_CAL_HEADSET.A = (delta[0] - 128 + 80) * 4;
    g_SC2720_ADC_CAL_HEADSET.B = (delta[1] - 128 + 833) * 4;
    g_SC2720_ADC_CAL_HEADSET.E1 = delta[2] * 2 + 2500;
    g_SC2720_ADC_CAL_HEADSET.E2 = delta[3] * 4 + 1300;
    OSI_LOGI(0, "A %d, B %d E1 %d E2 %d\n",
             g_SC2720_ADC_CAL_HEADSET.A, g_SC2720_ADC_CAL_HEADSET.B,
             g_SC2720_ADC_CAL_HEADSET.E1, g_SC2720_ADC_CAL_HEADSET.E2);
}
*/
static void _sprd_codec_ap_fast_charge()
{
    int cnt = 0;
    //unsigned int mask, val;

    REG_RDA2720M_AUD_CODEC_ANA_CDC2_T sc2720m_ana_cdc2;
    sc2720m_ana_cdc2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc2);
    sc2720m_ana_cdc2.b.rg_aud_das_en = 1;
    sc2720m_ana_cdc2.b.rg_aud_pa_en = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc2, sc2720m_ana_cdc2.v);

    REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    sc2720m_ana_sts2.b.rg_caldc_eno = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);

    REG_RDA2720M_AUD_CODEC_ANA_STS0_T sc2720m_ana_sts0;
    sc2720m_ana_sts0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts0);
    sc2720m_ana_sts0.b.rg_dccali_rdaci_adj = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts0, sc2720m_ana_sts0.v);

    SCI_Sleep(1); /* Wait for 1mS */
    sc2720m_ana_sts0.b.rg_dccali_rdaci_adj = 0;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts0, sc2720m_ana_sts0.v);

    sc2720m_ana_sts0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts0);
    OSI_LOGI(0, "start charge ANA_STS0: %x,time%d!", sc2720m_ana_sts0.v, SCI_GetTickCount());

    OSI_LOGI(0, "_sprd_codec_ap_fast_charge:Start waiting for fast charging.\n");
    /* Wait at least 50mS before DC-CAL. */
    while (++cnt <= 12)
        SCI_Sleep(5);

    OSI_LOGI(0, "_sprd_codec_ap_fast_charge: fast charging completed. (%d ms)\n,time%d",
             (cnt - 1) * 5, SCI_GetTickCount());
}

void _sprd_codec_ap_dc_calibration()
{
    //int ret = 0;
    //unsigned int mask, val;
    int cnt = 20;
    //unsigned int adie_chip_id;
    //1\POWER READY
    _sprd_codec_ap_bg_en(true);
    _sprd_codec_ap_aud_bias_en(true);
    // 2\cal related ready
    REG_RDA2720M_AUD_CODEC_ANA_CDC2_T sc2720m_ana_cdc2;
    sc2720m_ana_cdc2.v = 0x0000;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc2, sc2720m_ana_cdc2.v);
    _sprd_codec_ap_DAC_dc_offset_en(0);
    REG_RDA2720M_AUD_CODEC_ANA_CDC3_T sc2720m_ana_cdc3;
    sc2720m_ana_cdc3.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_cdc3);
    sc2720m_ana_cdc3.b.rg_aud_sdalhpl = 1;
    sc2720m_ana_cdc3.b.rg_aud_sdarhpr = 1;
    sc2720m_ana_cdc3.b.rg_aud_sdalrcv = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc3, sc2720m_ana_cdc3.v);
    REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;
    sc2720m_ana_sts2.v = 0x0000;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
#if 0
	SPRD_CODEC_AP_REG_BF_SET_VAL(&aud_codec_ctrl_ptr->ana_sts3,
		DEPOP_BIAS_SEL_2P5UA,DEPOP_BIAS_SEL, \
		DEPOP_BIAS_SEL_MASK \
		);
	_sprd_codec_ap_en_op( 0,&aud_codec_ctrl_ptr->ana_dcl0, DPOP_AUTO_RST);
#endif
    cnt = 10;
    //3\fast charge to normal mode
    _sprd_codec_ap_fast_charge();
    SCI_Sleep(5);
    // 4\cal ready
    _sprd_codec_ap_hp_buf_en(1);
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    sc2720m_ana_sts2.b.rg_caldc_eno = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
    sc2720m_ana_sts2.b.rg_caldc_en = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);

    REG_RDA2720M_AUD_CODEC_ANA_DCL4_T sc2720m_ana_dcl4;
    sc2720m_ana_dcl4.v = 0xFFFF;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl4, sc2720m_ana_dcl4.v);

    REG_RDA2720M_AUD_CODEC_ANA_DCL6_T sc2720m_ana_dcl6;
    sc2720m_ana_dcl6.v = 0x4CD8;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl6, sc2720m_ana_dcl6.v);

    REG_RDA2720M_AUD_CODEC_ANA_DCL7_T sc2720m_ana_dcl7;
    sc2720m_ana_dcl7.v = 0x2E6C;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_dcl7, sc2720m_ana_dcl7.v);

    REG_RDA2720M_AUD_CODEC_ANA_STS0_T sc2720m_ana_sts0;
    sc2720m_ana_sts0.v = 0xA820;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts0, sc2720m_ana_sts0.v);

    SCI_Sleep(2);
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    OSI_LOGI(0, "start dc cal ANA_STS2: %x,time%d!", sc2720m_ana_sts2.v, SCI_GetTickCount());
    _sprd_codec_dc_calibration_start();

    sc2720m_ana_sts2.v = 0xE000;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);

    /* Waiting for DCCAL process finish.
	 * Twice read for anti-glitch.
	 */
    cnt = 20;
    do
    {
        sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
        OSI_LOGI(0, "1st ANA_STS2: %x\n", sc2720m_ana_sts2.v);

        if ((sc2720m_ana_sts2.b.rg_aud_dccal_sts) && (sc2720m_ana_sts2.b.rg_aud_hp_dpop_dvld))
        {
            SCI_Sleep(5);
            sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
            OSI_LOGI(0, "1st ANA_STS2: %x\n", sc2720m_ana_sts2.v);
            if ((sc2720m_ana_sts2.b.rg_aud_dccal_sts) && (sc2720m_ana_sts2.b.rg_aud_hp_dpop_dvld))
                break;
        }
        SCI_Sleep(15);
    } while (--cnt);
    if (!cnt)
    {
        //ret = -1;
        OSI_LOGE(0, "%s, waiting for DCCAL finish timeout!\n", __FUNCTION__);
    }
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);

    OSI_LOGI(0, "end dc cal ANA_STS2: %x,time%d!", sc2720m_ana_sts2.v, SCI_GetTickCount());
    OSI_LOGI(0, "_sprd_codec_ap_dc_calibration:DC-CAL takes about %dms. cnt: %d\n",
             (20 - cnt) * 15 + (cnt ? 5 : 0), cnt);

    sc2720m_ana_cdc2.v = 0x0000;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_cdc2, sc2720m_ana_cdc2.v);
    OSI_LOGI(0, "__sprd_codec_ap_dc_calibration done.");
}

static void _sprd_codec_ap_depop_charge()
{
    //int val;
    //int ret = 0;
    int cnt = 20;
    REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;

    /* Check the status of DC-CAL*/
    do
    {
        sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
        OSI_LOGI(0, "ANA_STS2: %x,time%d!", sc2720m_ana_sts2.v, SCI_GetTickCount());

        if (sc2720m_ana_sts2.b.rg_aud_hp_dpop_dvld)
            break;
        SCI_Sleep(5);
    } while (--cnt);

    if (!cnt)
    {
        OSI_LOGI(0, "%s, waiting for DCCAL finish timeout!\n", __FUNCTION__);
        return;
    }
    OSI_LOGI(0, "_sprd_codec_ap_depop_charg:Check DC-CAL status wait cnt: %d\n", cnt);

    sc2720m_ana_sts2.b.rg_depop_chg_en = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
    sc2720m_ana_sts2.b.rg_aud_plugin = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
    sc2720m_ana_sts2.b.rg_depop_chg_en = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
    SCI_Sleep(2);

    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    OSI_LOGI(0, "Check charging status, start charge ANA_STS2: %x\n", sc2720m_ana_sts2.v);

    /* Start charging */
    sc2720m_ana_sts2.b.rg_depop_chg_start = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);

    /* Waiting for charging finish. */
    cnt = 30;
    do
    {
        sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
        OSI_LOGI(0, "Check charging status, ANA_STS2: %x\n", sc2720m_ana_sts2.v);
        if ((sc2720m_ana_sts2.b.rg_depop_chg_sts) && (sc2720m_ana_sts2.b.rg_aud_rcv_dpop_dvld))
            break;
        SCI_Sleep(20);
    } while (--cnt);
    if (!cnt)
    {
        //ret = -1;
        OSI_LOGE(0, "%s, waiting for charging finish timeout!\n", __FUNCTION__);
    }
    sc2720m_ana_sts2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts2);
    OSI_LOGI(0, "end depop charge ANA_STS2: %x,time%d!", sc2720m_ana_sts2.v, SCI_GetTickCount());
    OSI_LOGI(0, "_sprd_codec_ap_depop_charg:Charging ext cap takes about %dms. cnt: %d\n",
             (30 - cnt) * 20, cnt);
}

void _sprd_codec_ap_hp_depop_start(uint32_t en)
{
    OSI_LOGI(0, "_sprd_codec_ap_hp_depop_start %d", en);
    if (en)
    {
        _sprd_codec_ap_dc_calibration();
        _sprd_codec_ap_depop_charge();
        g_ExtCapChargeFinished = true;
    }
    else
    {
        REG_RDA2720M_AUD_CODEC_ANA_STS2_T sc2720m_ana_sts2;
        sc2720m_ana_sts2.b.rg_caldc_start = 0;
        halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts2, sc2720m_ana_sts2.v);
        _sprd_codec_ap_depop_charge_start(en);
        g_ExtCapChargeFinished = false;
    }
}

void __sprd_codec_ap_hp_depop_en(uint32_t en)
{
    OSI_LOGI(0, "__sprd_codec_ap_hp_depop_en %d", en);

#define CP_DO_DEPOP
#ifdef CP_DO_DEPOP
    audHeadsetdepop_en(en, g_HeadPhoneStatus.MicType);
#else
    if (en)
    {
        _sprd_codec_ap_ana_clk_digtial_clk_6p5m_en(1);

        _sprd_codec_ap_ana_clk_ana_core_clk_en(1);
        _sprd_codec_ap_ana_clk_ADC_clk_rst();
        _sprd_codec_ap_ana_clk_ADC_clk_en(1);

        //_sprd_codec_ap_ana_clk_ADC_clk_sel(1);
        _sprd_codec_ap_ana_clk_DAC_clk_en(1);
        //_sprd_codec_ap_ana_clk_DAC_clk_sel(1);

        _sprd_codec_ap_ana_clk_DRV_clk_en(1);

        _sprd_codec_ap_dcl_en(1);
        //_sprd_codec_ap_drv_soft_start_en(1);
        _sprd_codec_ap_dcl_rst();
        _sprd_codec_ap_depop_module_auto_rst();     //must be rst. or error
        _sprd_codec_ap_ana_clk_AUD_CLK_PN_sel(0xD); //fixe me
        // adie_audio_digtial part
        _sprd_codec_ap_aud_6p5m_en(1);
        _sprd_codec_ap_aud_1k_en(1);
        //_sprd_codec_ap_aud_hid_en(1);
        _sprd_codec_ap_aud_32k_en(1);
        //_sprd_codec_ap_LDO_vb_en(TRUE);
        _sprd_codec_ap_LDO_vb_sleep_mode_power_down(1);
        _sprd_codec_ap_bg_en(true);
        _sprd_codec_ap_aud_bias_en(true);
        _sprd_codec_ap_adc_VCMI_sel(0);
        _sprd_codec_ap_DAC_dc_offset_trim(0x2);
        _sprd_codec_ap_hp_depop_start(true);
        _sprd_codec_ap_DAC_dc_offset_en(en);
    }
    else
    {
        _sprd_codec_ap_hp_depop_start(false);
        //if (!g_ap_status)
        if (!audIsCodecOpen())
        {
            // adie_audio_analog part
            _sprd_codec_ap_ana_clk_digtial_clk_6p5m_en(0);
            _sprd_codec_ap_ana_clk_ana_core_clk_en(0);
            _sprd_codec_ap_ana_clk_ADC_clk_rst();
            _sprd_codec_ap_ana_clk_ADC_clk_en(0);
            //_sprd_codec_ap_ana_clk_ADC_clk_sel(0);
            _sprd_codec_ap_ana_clk_DAC_clk_en(0);
            //_sprd_codec_ap_ana_clk_DAC_clk_sel(0);

            _sprd_codec_ap_ana_clk_DRV_clk_en(0);
            _sprd_codec_ap_dcl_en(0); //for speaker
            //_sprd_codec_ap_drv_soft_start_en(0);
            // adie_audio_digtial part
            _sprd_codec_ap_aud_6p5m_en(0);
            _sprd_codec_ap_aud_1k_en(0);
            //_sprd_codec_ap_aud_hid_en(0);
            _sprd_codec_ap_aud_32k_en(0);
            _sprd_codec_ap_bg_en(false);
            _sprd_codec_ap_aud_bias_en(false);

            //_sprd_codec_ap_adc_VCMI_sel(0);
            //_sprd_codec_ap_LDO_vb_sleep_mode_power_down(FALSE);//sleep
            //_sprd_codec_ap_en_op(0, &aud_codec_ctrl_ptr->ana_sts2, CALDC_START);
        }
    }
#endif
}

void _sprd_codec_headset_detect_reg_init(void)
{
    //enable audio module power
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    module_en0.v = halAdiBusRead(&hwp_rda2720mGlobal->module_en0);
    module_en0.b.aud_en = 1;
    halAdiBusWrite(&hwp_rda2720mGlobal->module_en0, module_en0.v);

    _sprd_codec_ap_aud_hid_en(1);

    /* default ANA_HDT0=0a02*/
    /* must set in initial phase */
    _sprd_codec_hedet_vref_en(1);
    _sprd_codec_set_auxadc_scale(1);
    /* headset_reg_set_bits(ANA_HDT0, BIT(HEDET_BUF_CHOP)); */
    {
        REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
        sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2i_sel = 2; //1u
        halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
        OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt0: 0x%08x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt0.v);
    }

    REG_RDA2720M_AUD_CODEC_ANA_HDT1_T sc2720m_ana_hdt1;
    sc2720m_ana_hdt1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt1);
    /* default ANA_HDT1 = 0xce05 */
    /* bit15-13 = 110 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_micdet_ref_sel = HEDET_MICDET_REF_SEL_2P6;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit12-11 = 01 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_micdet_hys_sel = HEDET_MICDET_HYS_SEL_20MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit10-8 = 110 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_refl_sel = HEDET_LDET_REFL_SEL_300MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit7-6 = 0 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_refh_sel = HEDET_LDET_REFH_SEL_1P7;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit5-4 = 0 must set in initial phase */
    sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_pu_pd = HEDET_LDET_PU_PD_PU;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit3-2 = 01 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_l_hys_sel = HEDET_LDET_L_HYS_SEL_20MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    /* bit1-0 = 01 */
    sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_h_hys_sel = HEDET_LDET_H_HYS_SEL_20MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);

    sc2720m_ana_hdt1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt1);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt1: 0x%08x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt1.v);

    REG_RDA2720M_AUD_CODEC_ANA_HDT2_T sc2720m_ana_hdt2;
    sc2720m_ana_hdt2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt2);
    /* default ANA_HDT2 = 0x0534*/
    sc2720m_ana_hdt2.b.rg_aud_hedet_bdet_hys_sel = HEDET_BDET_HYS_SEL_20MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt2, sc2720m_ana_hdt2.v);

    /* bit10-7 = 1010 bdet_ref_v=0.85v*/
    sc2720m_ana_hdt2.b.rg_aud_hedet_bdet_ref_sel = HEDET_BDET_REF_SEL_850MV;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt2, sc2720m_ana_hdt2.v);

    _sprd_codec_headset_plugout_det_en(1);
    _sprd_codec_hpl_det_delay_en(1);
    sc2720m_ana_hdt2.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt2);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt2: 0x%08x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt2.v);

    /*ANA_DCL0 = 0x46
    headset_reg_write(ANA_DCL0, 0x46, 0xFFFF);
    headset_reg_read(ANA_DCL0, &val);
    pr_info("%s: ANA_DCL0: %#x\n", __func__, val);*/
    _sprd_codec_ap_drv_delay(1);
    _sprd_codec_ap_drv_soft_start_en(1);
    _sprd_codec_ap_dcl_en(1);
    REG_RDA2720M_AUD_CODEC_ANA_DCL0_T sc2720m_ana_dcl0;
    sc2720m_ana_dcl0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_dcl0);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_dcl0: 0x%08x\n", __FUNCTION__, __LINE__, sc2720m_ana_dcl0.v);

    /*ANA_PMU0 init config*/
    _sprd_codec_ap_vbg_sel(1);             // 1: BG=1.5V 0:1.55v
    _sprd_codec_ap_mic_bias_power_down(0); // 0:power up
    _sprd_codec_ap_vbg_temp_tune(VBG_TEMP_TUNE_TC_REDUCE);

    _sprd_codec_ap_LDO_vb_sleep_mode_power_down(0);
    _sprd_codec_ap_LDO_vb_en(1);

    REG_RDA2720M_AUD_CODEC_ANA_PMU0_T sc2720m_ana_pmu0;
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    sc2720m_ana_pmu0.b.rg_hmic_comp_mode1_en = 1;
    halAdiBusWrite(&hwp_rda2720mAudCodec->ana_pmu0, sc2720m_ana_pmu0.v);
    sc2720m_ana_pmu0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_pmu0);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_pmu0: 0x%08x\n", __FUNCTION__, __LINE__, sc2720m_ana_pmu0.v);
}

uint32_t _sprd_codec_head_mic_insert_sts()
{
    REG_RDA2720M_AUD_CODEC_ANA_STS7_T sc2720m_ana_sts7;
    sc2720m_ana_sts7.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts7);
    return sc2720m_ana_sts7.b.rg_aud_head_insert;
}

SC2720_HEADSET_TYPE_T __sprd_codec_ap_headmic_type_detect(void)
{
    uint32_t adc_mic_average = 0;
    uint32_t adc_left_average = 0;
    uint32_t retry_times = 10;
    //int val;
    REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;

    HEADSET_INFO_T_PTR cfg_ptr = NULL;

    cfg_ptr = SPRD_CODEC_HEADSET_GetInfo();
    OSI_LOGXD(OSI_LOGPAR_SI, 0, "[%s][%d]\n", __FUNCTION__, __LINE__);
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt0: %x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt0.v);
    _sprd_codec_headmic_adc_en(1);
    SCI_Sleep(10);

    _sprd_codec_ap_aud_hid_en(1);

    while (retry_times > 0)
    {
        OSI_LOGXI(OSI_LOGPAR_SI, 0, "[%s][%d]entry\n", __FUNCTION__, __LINE__);
        /* set large scale */
        _sprd_codec_set_auxadc_scale(1);
        _sprd_codec_set_auxadc_to_headmic(1);
        SCI_Sleep(10);
        adc_mic_average = _sprd_codec_ap_headmic_adcget(1);
        if ((adc_mic_average > cfg_ptr->headset_stable_max_value) || (adc_mic_average == -1)) // 2.65v
        {
            retry_times--;
            SCI_Sleep(100);
            OSI_LOGI(0, "headset: retry time is %d adc_mic_average %d\n",
                     retry_times, adc_mic_average);
            if (0 == retry_times)
                return HEADSET_TYPE_ERR;
        }
        else
        {
            OSI_LOGD(0, "headset: retry time is %d adc_mic_average=%d<headset_stable_max_value ",
                     retry_times, adc_mic_average);
            break;
        }
    }
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt0: %x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt0.v);

    OSI_LOGD(0, "headset: now get adc value  in little scale\n");
    /* change to little scale */
    _sprd_codec_set_auxadc_scale(0);
    /* get adc value of left */
    _sprd_codec_set_auxadc_to_headmic(0);
    SCI_Sleep(10);
    sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
    OSI_LOGXD(OSI_LOGPAR_SII, 0, "[%s][%d]ana_hdt0: %x\n", __FUNCTION__, __LINE__, sc2720m_ana_hdt0.v);

    adc_left_average = _sprd_codec_ap_headmic_adcget(0);
    OSI_LOGI(0, "headset: adc_left_average = %d\n", adc_left_average);

    /* Get adc value of headmic in. */
    _sprd_codec_set_auxadc_to_headmic(1);
    adc_mic_average = _sprd_codec_ap_headmic_adcget(0);
    OSI_LOGI(0, "headset:little scale adc_mic_average1 = %d\n", adc_mic_average);
#if 0
    val = SPRD_CODEC_AP_REG_GET(&aud_codec_ctrl_ptr->ana_hdt0);
	SCI_TRACE_LOW("__sprd_codec_ap_headmic_type_detect %d: ana_hdt0: %x\n", __LINE__, val);
        val = SPRD_CODEC_AP_REG_GET(&aud_codec_ctrl_ptr->ana_hdt1);
	SCI_TRACE_LOW("__sprd_codec_ap_headmic_type_detect %d: ana_hdt1: %x\n", __LINE__, val);
        val = SPRD_CODEC_AP_REG_GET(&aud_codec_ctrl_ptr->ana_hdt2);
	SCI_TRACE_LOW("__sprd_codec_ap_headmic_type_detect %d: ana_hdt2: %x\n", __LINE__, val);
        val = SPRD_CODEC_AP_REG_GET(&aud_codec_ctrl_ptr->ana_pmu0);
	SCI_TRACE_LOW("__sprd_codec_ap_headmic_type_detect %d: ana_pmu0: %x\n", __LINE__, val);
#endif
    if (adc_left_average < cfg_ptr->headset_one_half_adc_gnd) // 0.15v
    {
        if (adc_mic_average < cfg_ptr->headset_adc_threshold_3pole_detect) // 0.9v
        {
            OSI_LOGI(0, "headset: HEADSET_NO_MIC\n");
            return HEADSET_NO_MIC;
        }
        if (_sprd_codec_head_mic_insert_sts() == 0)
        {
            OSI_LOGI(0, "headset: 4pole HEADSET  mic abnormal \n");
            return HEADSET_NO_MIC;
        }
        if (adc_left_average < cfg_ptr->headset_half_adc_gnd) // 0.05v
        {
            /* bit10-8 change from 110 to 001 HDT1*/
            REG_RDA2720M_AUD_CODEC_ANA_HDT1_T sc2720m_ana_hdt1;
            sc2720m_ana_hdt1.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt1);
            sc2720m_ana_hdt1.b.rg_aud_hedet_ldet_refl_sel = HEDET_LDET_REFL_SEL_50MV;
            halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt1, sc2720m_ana_hdt1.v);
        }
        OSI_LOGI(0, "headset: HEADSET_4POLE_NORMAL\n");
        return HEADSET_4POLE_NORMAL;
    }
    else if (abs(adc_mic_average - adc_left_average) < cfg_ptr->headset_adc_gnd) // 0.1v
    {
        OSI_LOGI(0, "headset: HEADSET_4POLE_NOT_NORMAL\n");
        return HEADSET_4POLE_NOT_NORMAL;
    }
    else
    {
        OSI_LOGI(0, "headset: HEADSET_TYPE_ERR\n");
        return HEADSET_TYPE_ERR;
    }
    OSI_LOGI(0, "headset: HEADSET_TYPE_ERR\n");
    return HEADSET_TYPE_ERR;
}

void __sprd_codec_set_headset_connection_status(uint32_t status)
{
    OSI_LOGXI(OSI_LOGPAR_SII, 0, "[%s][%d]status: %x\n", __FUNCTION__, __LINE__, status);
    if (status)
    {
        _sprd_codec_headset_lint_low_det_filter_en(1);
        _sprd_codec_headset_plugout_det_en(1);
        _sprd_codec_headset_detect_reg_init();
        _sprd_codec_headmic_detect_en(1);
        //_sprd_codec_ap_vbg_sel(1);
        //_sprd_codec_ap_LDO_vb_sleep_mode_power_down(0);
        //_sprd_codec_ap_LDO_vb_en(1);
        // _sprd_codec_ap_bg_en(1);
        _sprd_codec_ap_aud_bias_en(1);
        //_sprd_codec_ap_mic_bias_en(1);
        _sprd_codec_ap_headmic_bias_en(1);
        _sprd_codec_ap_headmic_sleep_mode_en(1);
        //_sprd_codec_ap_vbg_temp_tune(VBG_TEMP_TUNE_TC_REDUCE);
        //if (g_SC2720_ADC_CAL_HEADSET.cal_type == SC2720_HEADSET_AUXADC_CAL_NO)
        //{
        //    _sprd_codec_headset_adc_cal_from_efuse();
        //}
    }
    else
    {
        __sprd_codec_ap_hp_depop_en(0);
        REG_RDA2720M_AUD_CODEC_ANA_STS0_T sc2720m_ana_sts0;
        sc2720m_ana_sts0.v = 0x0000;
        halAdiBusWrite(&hwp_rda2720mAudCodec->ana_sts0, sc2720m_ana_sts0.v);
        _sprd_codec_headset_lint_low_det_filter_en(0);
        _sprd_codec_ap_headmic_bias_en(0);
        _sprd_codec_ap_headmic_sleep_mode_en(0);
        _sprd_codec_ap_hp_buf_en(0);
        _sprd_codec_headmic_adc_en(0);
        _sprd_codec_headset_button_detect_en(0);
        _sprd_codec_headmic_detect_en(0);
        _sprd_codec_headset_plugout_det_en(0);

        REG_RDA2720M_AUD_CODEC_ANA_HDT0_T sc2720m_ana_hdt0;
        sc2720m_ana_hdt0.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_hdt0);
        sc2720m_ana_hdt0.b.rg_aud_hedet_v2ad_scale = 1;
        sc2720m_ana_hdt0.b.rg_aud_hedet_buf_chop = 0;
        halAdiBusWrite(&hwp_rda2720mAudCodec->ana_hdt0, sc2720m_ana_hdt0.v);
    }
    g_HeadPhoneStatus.Connected = status;
    g_HeadPhoneStatus.MicType = UNKNOWN_MIC_TYPE;
}

void __sprd_codec_headset_type_related_oper(SC2720_HEADSET_TYPE_T headset_type)
{
    g_HeadPhoneStatus.MicType = headset_type;

    _sprd_codec_headmic_adc_en(0);
    _sprd_codec_headset_lint_low_det_filter_en(1);
    if (audIsCodecOpen())
    {
        _sprd_codec_ap_LDO_vb_sleep_mode_power_down(1);
    }
    else
    {
        _sprd_codec_ap_bg_en(0);
        _sprd_codec_ap_LDO_vb_sleep_mode_power_down(0);
    }
    switch (headset_type)
    {
    case HEADSET_TYPE_ERR:
        _sprd_codec_ap_headmic_bias_en(0);
        _sprd_codec_ap_headmic_sleep_mode_en(0);
        //_sprd_codec_ap_vbg_sel(0);
        //_sprd_codec_ap_vbg_temp_tune(VBG_TEMP_TUNE_NORMAL);
        _sprd_codec_headmic_detect_en(0);
        _sprd_codec_hpl_det_delay_en(0);
        OSI_LOGI(0, "headset_type = %d (HEADSET_TYPE_ERR)\n", headset_type);
        break;
    case HEADSET_4POLE_NOT_NORMAL:
        _sprd_codec_ap_headmic_bias_en(0);
        _sprd_codec_ap_headmic_sleep_mode_en(0);
        // _sprd_codec_ap_vbg_sel(0);
        //_sprd_codec_ap_vbg_temp_tune(VBG_TEMP_TUNE_NORMAL);
        _sprd_codec_headmic_detect_en(0);
        _sprd_codec_hpl_det_delay_en(0);
        OSI_LOGI(0, "headset_type = %d (HEADSET_4POLE_NOT_NORMAL)\n", headset_type);
        break;
    case HEADSET_4POLE_NORMAL:
        _sprd_codec_headset_button_detect_en(1);
        _sprd_codec_headmic_detect_en(1);
        _sprd_codec_ap_headmic_sleep_mode_en(1);
        OSI_LOGI(0, "headset_type = %d (HEADSET_4POLE_NORMAL)\n", headset_type);
        break;
    case HEADSET_NO_MIC:
        _sprd_codec_ap_headmic_bias_en(0);
        _sprd_codec_ap_headmic_sleep_mode_en(0);
        //_sprd_codec_ap_vbg_sel(0);
        // _sprd_codec_ap_vbg_temp_tune(VBG_TEMP_TUNE_NORMAL);
        _sprd_codec_headmic_detect_en(0);
        _sprd_codec_hpl_det_delay_en(0);
        OSI_LOGI(0, "headset_type = %d (HEADSET_NO_MIC)\n", headset_type);
        break;
    case HEADSET_APPLE:
        OSI_LOGI(0, "headset_type = %d (HEADSET_APPLE)\n", headset_type);
        break;
    default:
        OSI_LOGI(0, "headset_type = %d (HEADSET_UNKNOWN)\n", headset_type);
        break;
    }
}

uint32_t __sprd_codec_headset_type_detect(void)
{
    uint32_t heaset_type = 0;
    heaset_type = __sprd_codec_ap_headmic_type_detect();
    return heaset_type;
}

void __sprd_codec_headset_related_oper(uint32_t heaset_type)
{
    g_HeadPhoneStatus.MicType = heaset_type;
    __sprd_codec_ap_hp_depop_en(true);
    __sprd_codec_headset_type_related_oper(heaset_type);
    return;
}

bool drvPmicAudHeadsetInsertConfirm(void)
{
    REG_RDA2720M_AUD_CODEC_ANA_STS7_T sc2720m_ana_sts7;
    sc2720m_ana_sts7.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts7);
    return sc2720m_ana_sts7.b.rg_aud_head_insert_all;
}

bool drvPmicAudHeadButtonPressConfirm(void)
{
    REG_RDA2720M_AUD_CODEC_ANA_STS7_T sc2720m_ana_sts7;
    sc2720m_ana_sts7.v = halAdiBusRead(&hwp_rda2720mAudCodec->ana_sts7);
    return sc2720m_ana_sts7.b.rg_aud_head_button;
}

uint32_t drvPmicAudGetHmicAdcValue(bool is_big_scale)
{
    uint32_t adc_mic_average = 0;
    _sprd_codec_headmic_adc_en(1);
    SCI_Sleep(10);
    _sprd_codec_ap_aud_hid_en(1);
    if (is_big_scale)
    {
        _sprd_codec_set_auxadc_scale(1);
        _sprd_codec_set_auxadc_to_headmic(1);
        SCI_Sleep(10);
        adc_mic_average = _sprd_codec_ap_headmic_adcget(1);
    }
    else
    {
        _sprd_codec_set_auxadc_scale(0);
        _sprd_codec_set_auxadc_to_headmic(1);
        SCI_Sleep(10);
        adc_mic_average = _sprd_codec_ap_headmic_adcget(0);
    }
    return adc_mic_average;
}

void drvPmicAudHeadsetDectRegInit(void)
{
    _sprd_codec_headset_detect_reg_init();
    _sprd_codec_ap_LDO_vb_sleep_mode_power_down(1);
    _sprd_codec_ap_LDO_vb_en(0);
}
#endif
