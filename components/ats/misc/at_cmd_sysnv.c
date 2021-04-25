/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "osi_log.h"
#include "osi_sysnv.h"
#include "at_command.h"
#include "at_response.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lwip/opt.h"

enum
{
    SYSNV_READ = 0,
    SYSNV_SET = 1,
    SYSNV_CLEAR = 2
};

enum
{
    SYSNV_TYPE_BOOL = 0,
    SYSNV_TYPE_INT = 1,
};

typedef struct
{
    void *value;
    int type;
    int min;
    int max;
    const char *name;
    const char *hint;
} sysnvItem_t;

#define ITEM_INT_VAL(pval) (*((int *)pval))
#define ITEM_BOOL_VAL(pval) (*((bool *)pval))

static const char kSysnvBoolHint[] = "0.disable; 1.enable";
static const char kSysnvUsbModeHint[] = "0.Charger only; 1.RDA (8910 ROM) serial; "
                                        "2.Eight serials; 3.RNDIS and serials; "
                                        "4.SPRD U2S Diag; 5.ECM and serials; "
                                        "6.ECM ACM[0-4], USL[2,4]; "
                                        "7.RNDIS only; 8.ECM only";
static const char kSysnvDiagDeviceHint[] = "1.diag device uart; 2.diag device usb serial";
#if IP_NAT
static const char kSysnvNatCfgHint[] = "0 disable; 16bit config nat cid; 101 =0000000100100101 sim 0 cid(1,3,6 enable)(2,4,5,7,8 disable) sim1 cid(1 enable)(2,3,4,5,6,7,8 disable)";
#endif

static const sysnvItem_t gAtSysnvMap[] = {
    {&gSysnvTraceEnabled, SYSNV_TYPE_BOOL, 0, 1, "ap_trace_en", kSysnvBoolHint},
    {&gSysnvDeepSleepEnabled, SYSNV_TYPE_BOOL, 0, 1, "deep_sleep_en", kSysnvBoolHint},
    {&gSysnvPsmSleepEnabled, SYSNV_TYPE_BOOL, 0, 1, "psm_sleep_en", kSysnvBoolHint},
    {&gSysnvUsbWorkMode, SYSNV_TYPE_INT, 0, 8, "usbmode", kSysnvUsbModeHint},
    {&gSysnvUsbDetMode, SYSNV_TYPE_INT, 0, 1, "usb_detect_mode", "0.Charger Detect; 1.Always On"},
    {&gSysnvDiagDevice, SYSNV_TYPE_INT, 1, 2, "diag_device", kSysnvDiagDeviceHint},
    {&gSysnvSimHotPlug, SYSNV_TYPE_BOOL, 0, 1, "sim_hotplug", kSysnvBoolHint},
    {&gSysnvSimVoltTrigMode, SYSNV_TYPE_BOOL, 0, 1, "sim_hotplug_vol_trigmode", kSysnvBoolHint},
#if IP_NAT
    {&gSysnvNATCfg, SYSNV_TYPE_INT, 0, 65535, "nat_cfg", kSysnvNatCfgHint},
#endif
    {&gSysnvFstraceMask, SYSNV_TYPE_INT, 0, 65535, "fstrace",
     "[0]: aplog, [1] profile, [2] bsdump, [3] cplog, [4] zsplog, [5] moslog"},
    {&gSysnvPoffChargeEnable, SYSNV_TYPE_BOOL, 0, 1, "poff_charge_en", kSysnvBoolHint},
};

static const sysnvItem_t *prvNameGetItem(const char *name)
{
    for (unsigned i = 0; i < OSI_ARRAY_SIZE(gAtSysnvMap); ++i)
    {
        const sysnvItem_t *item = &gAtSysnvMap[i];
        if (strcmp(item->name, name) == 0)
            return item;
    }
    return NULL;
}

void atCmdHandleSYSNV(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        bool paramok = true;
        uint8_t type = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (type == SYSNV_CLEAR)
        {
            if (cmd->param_count != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            osiSysnvClear();
            RETURN_OK(cmd->engine);
        }

        const char *name = atParamStr(cmd->params[1], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        const sysnvItem_t *item = prvNameGetItem(name);
        if (item == NULL)
        {
            OSI_LOGXE(OSI_LOGPAR_S, 0, "at set sysnv %s not available", name);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);
        }

        if (type == SYSNV_READ)
        {
            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            int val = 0;
            if (item->type == SYSNV_TYPE_BOOL)
                val = ITEM_BOOL_VAL(item->value) ? 1 : 0;
            else if (item->type == SYSNV_TYPE_INT)
                val = ITEM_INT_VAL(item->value);
            else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            char resp[64];
            snprintf(resp, 64, "+SYSNV: \"%s\",%d", item->name, val);
            atCmdRespInfoText(cmd->engine, resp);
            RETURN_OK(cmd->engine);
        }
        else
        {
            if (item->type == SYSNV_TYPE_BOOL || item->type == SYSNV_TYPE_INT)
            {
                int val = atParamIntInRange(cmd->params[2], item->min, item->max, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);

                if (item->type == SYSNV_TYPE_BOOL)
                    ITEM_BOOL_VAL(item->value) = !!val;
                else if (item->type == SYSNV_TYPE_INT)
                    ITEM_INT_VAL(item->value) = val;

                if (!osiSysnvSave())
                {
                    OSI_LOGE(0, "AT+SYSNV fail to save");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                RETURN_OK(cmd->engine);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char *resp = (char *)malloc(2048);
        for (unsigned i = 0; i < OSI_ARRAY_SIZE(gAtSysnvMap); ++i)
        {
            const sysnvItem_t *item = &gAtSysnvMap[i];
            snprintf(resp, 2048, "+SYSNV: \"%s\", \"%s\"", item->name, item->hint);
            atCmdRespInfoText(cmd->engine, resp);
        }
        free(resp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
