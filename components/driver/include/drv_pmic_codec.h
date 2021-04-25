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

#ifndef _DRV_PMIC_CODEC_H_
#define _DRV_PMIC_CODEC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UNKNOWN_MIC_TYPE,
    HEADSET_NO_MIC,
    HEADSET_4POLE_NORMAL,
    HEADSET_4POLE_NOT_NORMAL,
    HEADSET_APPLE,
    HEADSET_TYPE_ERR = -1,
} SC2720_HEADSET_TYPE_T;

typedef struct HeadPhoneSt
{
    uint32_t Connected;
    SC2720_HEADSET_TYPE_T MicType;
} T_HEADPHONE_STATUS;

typedef struct
{
    uint32_t headset_stable_max_value;           // 2.65v
    uint32_t headset_one_half_adc_gnd;           //0.15V
    uint32_t headset_adc_threshold_3pole_detect; //0.9V
    uint32_t headset_half_adc_gnd;               //0.05V
    uint32_t headset_adc_gnd;                    // 0.1v
    uint32_t headset_key_send_min_vol;
    uint32_t headset_key_send_max_vol;
    uint32_t headset_key_volup_min_vol;
    uint32_t headset_key_volup_max_vol;
    uint32_t headset_key_voldown_min_vol;
    uint32_t headset_key_voldown_max_vol;
    uint32_t coefficient;
} HEADSET_INFO_T, *HEADSET_INFO_T_PTR;

/**
 * @brief headset insert depop and other related oper
 *
 * @heaset_type headset type,
 * @return
 */

void __sprd_codec_headset_related_oper(uint32_t heaset_type);

/**
 * @brief detect headset type.
 *
 * @return headset type
 */

uint32_t __sprd_codec_headset_type_detect(void);
/**
 * @brief set headset connection status.
 *
 * @status headset insert state,
 * @return
 */

void __sprd_codec_set_headset_connection_status(uint32_t status);
/**
 * @brief return the headset insert state.
 *
 * @return
 *      true insert.
 *      false no insert.
 */

bool drvPmicAudHeadsetInsertConfirm(void);
/**
 * @brief return the headbutton press state.
 *
 * @return
 *      true press.
 *      false no press.
 */

bool drvPmicAudHeadButtonPressConfirm(void);
/**
 * @brief return the volt for the hmic channel in mV.
 *
 * @param is_big_scale set the scale of the hmic channel,true use ADC_SCALE_1V250,
 * true use ADC_SCALE_1V250,,rg_aud_hedet_v2ad_scale 0
 * false use ADC_SCALE_1V250,rg_aud_hedet_v2ad_scale 1
 * @return
 *      -1 failure
 *      the volt vlaue.
 */

uint32_t drvPmicAudGetHmicAdcValue(bool is_big_scale);

/**
 * @brief headset detect poweron init.
 */

void drvPmicAudHeadsetDectRegInit(void);

#ifdef __cplusplus
}
#endif
#endif
