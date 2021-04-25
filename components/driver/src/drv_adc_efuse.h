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

#ifndef _DRV_ADC_EFUSE_H_
#define _DRV_ADC_EFUSE_H_

struct rda_adc_cal
{
    uint16_t p0_vol; //4.2V
    uint16_t p0_adc;
    uint16_t p1_vol; //3.6V
    uint16_t p1_adc;
    uint16_t cal_type;
};

struct adc_efuse_para
{
    int small_adc_p0;
    int small_adc_p1;
    int small_vol_p0;
    int small_vol_p1;
    int small_cal_efuse_blk;
    int small_cal_efuse_bit;
    int scale01_adc_p0;
    int scale01_adc_p1;
    int scale01_vol_p0;
    int scale01_vol_p1;
    int scale01_cal_efuse_blk;
    int scale01_cal_efuse_bit;
    int scale02_adc_p0;
    int scale02_adc_p1;
    int scale02_vol_p0;
    int scale02_vol_p1;
    int scale02_cal_efuse_blk;
    int scale02_cal_efuse_bit;
    int big_adc_p0;
    int big_adc_p1;
    int big_vol_p0;
    int big_vol_p1;
    int big_cal_efuse_blk;
    int big_cal_efuse_bit;
    int headmic_adc_p0;
    int headmic_adc_p1;
    int headmic_vol_p0;
    int headmic_vol_p1;
    int headmic_cal_efuse_blk;
    int headmic_cal_efuse_bit;
    int blk_width;
    int adc_data_off;
};
//AUDIO_HEADMIC
enum adc_channel_20
{
    HEADMICIN_DET_SCALE1 = 0,
    HEADMICIN_L_INT_SCALE1 = 1,
    HP_L_SCALE1 = 2,
    HP_R_SCALE1 = 3,
    VDD_VB_SCALE1 = 4,
    VDD_PA_SCALE1 = 5,
    MICBIAS_SCALE1 = 6,
    HEADMIC_BIAS_SCALE1 = 7,
    // set rg_aud_hedet_v2ad_scale=0
    HEADMICIN_DET_SCALE0 = 8,
    HEADMICIN_L_INT_SCALE0 = 9,
    HP_L_SCALE0 = 10,
    HP_R_SCALE0 = 11,
    VDD_VB_SCALE0 = 12,
    VDD_PA_SCALE0 = 13,
    MICBIAS_SCALE0 = 14,
    HEADMIC_BIAS_SCALE0 = 15,
};
//DCDC_CALOUT
enum adc_channel_13
{
    DCDC_VCORE = 0,
    DCDC_VGEN = 1,
    DCDC_VPA = 2,
};

#endif /*__RDA_ADC_H__*/
