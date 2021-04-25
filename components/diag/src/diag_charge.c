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

#include "diag_config.h"
#include "diag.h"
#include "osi_api.h"
#include "osi_log.h"
#include "drv_uart.h"
#include "drv_adc.h"
#include "drv_charger.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
#include "cmddef.h"
#include <stdlib.h>
#include <string.h>

#define DIAG_CMD_PARAM(cmd, type) const type *param = (const type *)diagCmdData(cmd)

enum
{
    CHARGER_OPERATION = 0,
};

typedef struct
{
    uint16_t operation;
    uint16_t len;
    uint8_t data[0];
} chargeCtrl_t;

typedef struct
{
    uint16_t status;
    uint16_t length;
} diagToolsCnf_t;

static bool _handleCharge(const diagMsgHead_t *cmd, void *ctx)
{
    switch (cmd->subtype)
    {
    case CHARGER_OPERATION:
    {
        DIAG_CMD_PARAM(cmd, chargeCtrl_t);
        if (param->operation == 0x10)
        {
            diagToolsCnf_t cnf = {};
            if (param->len == 4)
            {
                uint32_t *data = (uint32_t *)param->data;
                // charge on
                if (*data == 1)
                {
                    drvChargeEnable();
                    // status 0:success others: fail, len = 0
                    cnf.status = 0;
                    cnf.length = 0;
                }
            }
            else
            {
                cnf.status = 1;
                cnf.length = 0;
            }
            diagOutputPacket2(cmd, &cnf, sizeof(diagToolsCnf_t));
        }
        break;
    }
    default:
        diagBadCommand(cmd);
        break;
    }
    return true;
}

static bool _handleBattVoltage(const diagMsgHead_t *cmd, void *ctx)
{
    switch (cmd->subtype)
    {
    case BATT_VOLTAGE_F:
    {
        uint32_t volt = (uint32_t)drvAdcGetChannelVolt(ADC_CHANNEL_VBATSENSE, ADC_SCALE_5V000);
        volt |= 0x1000000; //set bit24 = 1, if bit24 = 0 tools will *10

        diagOutputPacket2(cmd, &volt, sizeof(uint32_t));
        break;
    }
    default:
        diagBadCommand(cmd);
        break;
    }
    return true;
}

void diagChargeInit(void)
{
    diagRegisterCmdHandle(DIAG_AP_F, _handleCharge, NULL);
    diagRegisterCmdHandle(DIAG_POWER_SUPPLY_F, _handleBattVoltage, NULL);
}
