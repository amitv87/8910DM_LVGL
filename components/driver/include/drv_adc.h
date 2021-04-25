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

#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

#define ADC_CHANNEL_INVALID -1

enum adc_aux_scale
{
    ADC_SCALE_1V250 = 0,
    ADC_SCALE_2V444 = 1,
    ADC_SCALE_3V233 = 2,
    ADC_SCALE_5V000 = 3,
    ADC_SCALE_MAX = 3,
};

enum adc_aux_channel
{
    ADC_CHANNEL_BAT_DET = 0,
    ADC_CHANNEL_1 = 1,
    ADC_CHANNEL_2 = 2,
    ADC_CHANNEL_3 = 3,
    ADC_CHANNEL_4 = 4,
    ADC_CHANNEL_VBATSENSE = 5,
    ADC_CHANNEL_6 = 6, //no use
    ADC_CHANNEL_TYPEC_CC1 = 7,
    ADC_CHANNEL_THM = 8,
    ADC_CHANNEL_TYPEC_CC2 = 9,
    ADC_CHANNEL_10 = 10, //no use
    ADC_CHANNEL_11 = 11, //no use
    ADC_CHANNEL_12 = 12, //no use
    ADC_CHANNEL_DCDC_CALOUT = 13,
    ADC_CHANNEL_VCHGSEN = 14,
    ADC_CHANNEL_VCHG_BG = 15,
    ADC_CHANNEL_PROG2ADC = 16,
    ADC_CHANNEL_17 = 17,
    ADC_CHANNEL_18 = 18,
    ADC_CHANNEL_SDAVDD = 19,
    ADC_CHANNEL_HEADMIC = 20,
    ADC_CHANNEL_LDO_CALOUT0 = 21,
    ADC_CHANNEL_LDO_CALOUT1 = 22,
    ADC_CHANNEL_LDO_CALOUT2 = 23,
    ADC_CHANNEL_24 = 24, //no use
    ADC_CHANNEL_25 = 25, //no use
    ADC_CHANNEL_26 = 26, //no use
    ADC_CHANNEL_27 = 27, //no use
    ADC_CHANNEL_28 = 28, //no use
    ADC_CHANNEL_SELF_OFFSET_MEASURE = 29,
    ADC_CHANNEL_DP = 30,
    ADC_CHANNEL_DM = 31,
    ADC_CHANNEL_MAX = 31,
};

/**
 * @brief initialize ADC driver
 * it should be called after ADI bus is initialized.
 */

void drvAdcInit(void);

/**
 * @brief return the volt for the given channel in mV.
 *
 * @param channel  set the channel to measue
 * @param scale   set the scale of the channel
 * @return
 *      -1 failure
 *      the volt vlaue.
 */

int32_t drvAdcGetChannelVolt(uint32_t channel, int32_t scale);

/**
 * @brief return the raw adc value for the given channel.
 *
 * @param channel  set the channel to measue
 * @param scale   set the scale of the channel
 * @return
 *      -1 failure
 *      the raw vlaue.
 */

int32_t drvAdcGetRawValue(uint32_t channel, int32_t scale);
/**
 * @brief return the volt for the hmic channel in mV.
 *
 * @param scale  	set the scale of the hmic channel,default use ADC_SCALE_1V250
 * @param con_mode  set the scale ratio of the channel,match with rg_aud_hedet_v2ad_scale
 * @return
 *      -1 failure
 *      the volt vlaue.
 */

int32_t drvAdcGetHMicChannelVolt(int32_t scale, int32_t con_mode);

#endif /*__RDA_ADC_H__*/
