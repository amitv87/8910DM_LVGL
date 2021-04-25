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

#ifndef _AT_CMD_AUDIO_
#define _AT_CMD_AUDIO_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#ifdef CONFIG_SOC_8910
// =============================================================================
//  TYPES
// =============================================================================
typedef struct
{
    int32_t id;
    uint32_t freq;
    int32_t clockMode;
} HAL_I2C_CFG_T;

typedef struct
{
    uint32_t regAddr;
    uint16_t val;
    uint16_t delay;
} HAL_CODEC_REG_T;

typedef enum
{
    HAL_AIF_RX_DELAY_ALIGN,
    HAL_AIF_RX_DELAY_1,
    HAL_AIF_RX_DELAY_2,
    HAL_AIF_RX_DELAY_3,
    HAL_AIF_RX_DELAY_QTY
} HAL_AIF_RX_DELAY_T;

typedef enum
{
    HAL_AIF_RX_MODE_STEREO_STEREO,
    HAL_AIF_RX_MODE_STEREO_MONO_FROM_L,
    HAL_AIF_RX_MODE_QTY
} HAL_AIF_RX_MODE_T;

typedef enum
{
    HAL_SERIAL_MODE_I2S,
    HAL_SERIAL_MODE_VOICE,
    HAL_SERIAL_MODE_DAI,
    HAL_SERIAL_MODE_QTY
} HAL_SERIAL_MODE_T;

typedef enum
{
    HAL_AIF_TX_DELAY_ALIGN,
    HAL_AIF_TX_DELAY_1,
    HAL_AIF_TX_DELAY_QTY
} HAL_AIF_TX_DELAY_T;

typedef enum
{
    HAL_AIF_TX_MODE_STEREO_STEREO,
    HAL_AIF_TX_MODE_MONO_STEREO_CHAN_L,
    HAL_AIF_TX_MODE_MONO_STEREO_DUPLI,
    HAL_AIF_TX_MODE_QTY
} HAL_AIF_TX_MODE_T;

typedef struct
{
    HAL_SERIAL_MODE_T mode;
    bool aifIsMaster;
    bool lsbFirst;
    bool polarity;
    HAL_AIF_RX_DELAY_T rxDelay;
    HAL_AIF_TX_DELAY_T txDelay;
    HAL_AIF_RX_MODE_T rxMode;
    HAL_AIF_TX_MODE_T txMode;
    uint32_t fs;
    uint32_t bckLrckRatio;
    bool invertBck;
    bool outputHalfCycleDelay;
    bool inputHalfCycleDelay;
    bool enableBckOutGating;
} HAL_AIF_SERIAL_CFG_T;

typedef struct
{
    uint32_t codecAddr;
    uint16_t codecIsMaster;
    uint16_t dataFormat;
    uint16_t codecCurrentuser;
    uint16_t codecTestmode;
    uint16_t initFlag;
    uint16_t externalCodec;
    uint16_t reserved[24];
    HAL_I2C_CFG_T i2cCfg;
    HAL_AIF_SERIAL_CFG_T i2sAifcfg;
    HAL_CODEC_REG_T initRegCfg[100];
    HAL_CODEC_REG_T closeRegCfg[50];
    HAL_CODEC_REG_T sampleRegCfg[10][12];
    HAL_CODEC_REG_T inpathRegCfg[6][20];
    HAL_CODEC_REG_T outpathRegCfg[4][20];
} HAL_CODEC_CFG_T;
#endif

void atAudioInit(void);

OSI_EXTERN_C_END
#endif
