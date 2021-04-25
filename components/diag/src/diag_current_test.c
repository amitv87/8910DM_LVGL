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
#define POST_NAME OSI_MAKE_TAG('P', 'O', 'S', 'T')

#include "diag_config.h"
#include "diag.h"
#include "osi_api.h"
#include "osi_log.h"
#include "drv_uart.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#include "cmddef.h"
#include <stdlib.h>
#include <string.h>

typedef struct diagCurrentTestContext
{
    uint8_t subtype;
} diagCurrentTestContext_t;

static diagCurrentTestContext_t gDiagCurrTestCtx;
static osiPmSource_t *gCalibPostPmSource = NULL;

static bool _handleCurrentTest(const diagMsgHead_t *cmd, void *ctx)
{
    // no response for these commands
    switch (cmd->subtype)
    {
    case CURRENT_TEST_STOP:
        switch (gDiagCurrTestCtx.subtype)
        {
        case CURRENT_TEST_TX_MAX_PEAK_VALUE:
            break;
        case CURRENT_TEST_RX_MIN_PEAK_VALUE:
            break;
        case CURRENT_TEST_CHARGING:
        case CURRENT_TEST_LED_ON:
        case CURRENT_TEST_VIBRATOR_ON:
            break;
        default:
            break;
        }
        break;

    case CURRENT_TEST_RF_CLOSED: // Not support now.
        break;

    case CURRENT_TEST_DEEP_SLEEP:
        // don't send any response.
        if (osiGetBootMode() == OSI_BOOTMODE_CALIB_POST)
            osiPmWakeUnlock(gCalibPostPmSource);
        else
        {
            OSI_LOGE(0, "unsupport sleep mode");
            while (1)
                ;
        }
        break;

    case CURRENT_TEST_LED_ON:
        break;

    case CURRENT_TEST_VIBRATOR_ON:
        break;

    case CURRENT_TEST_RX_MIN_PEAK_VALUE:
        break;

    case CURRENT_TEST_TX_MAX_PEAK_VALUE:
        break;

    case CURRENT_TEST_CHARGING:
        break;

    case CURRENT_TEST_DEEP_GET_SLEEP_FLAG:
        break;

    case CURRENT_TEST_DEEP_SLEEP_FLAG_ENABLE:
        break;

    case CURRENT_TEST_DEEP_SLEEP_FLAG_DISABLE:
        break;

    case CURRENT_TEST_UART_ENABLESLEEP:
        break;

    case CURRENT_TEST_POWER_OFF:
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
        break;

    default:
        break;
    }

    gDiagCurrTestCtx.subtype = cmd->subtype;
    return true;
}

void diagCurrentTestInit(void)
{
    diagRegisterCmdHandle(DIAG_CURRENT_TEST_F, _handleCurrentTest, NULL);
}

void diagCalibPostInit(void)
{
    gCalibPostPmSource = osiPmSourceCreate(POST_NAME, NULL, NULL);
    osiPmWakeLock(gCalibPostPmSource);
}
