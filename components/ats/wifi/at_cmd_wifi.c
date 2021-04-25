/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "osi_api.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "drv_wifi.h"
#include "ats_config.h"

#include <stdlib.h>
#include <stdio.h>

#define WIFI_SCAN_MAX_TIME (5000)
#define WIFI_SCAN_DEFAULT_TIME (600)
#define WIFI_SCAN_MAX_AP_CNT (300)
#define WIFI_SCAN_DEFAULT_AP_CNT (100)
#define WIFI_SCAN_DEFAULT_ROUND (1)
#define WIFI_SCAN_MAX_ROUND (10)

static drvWifi_t *gDrvWifi = NULL;

void atCmdHandleWifiOpen(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (gDrvWifi != NULL)
        {
            OSI_LOGI(0, "wifi already open");
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            return;
        }

        gDrvWifi = drvWifiOpen();
        if (gDrvWifi == NULL)
        {
            OSI_LOGE(0, "wifi open create wifi context fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void atCmdHandleWifiClose(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        if (gDrvWifi == NULL)
        {
            OSI_LOGI(0, "wifi already close");
            atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            return;
        }

        drvWifiClose(gDrvWifi);
        gDrvWifi = NULL;
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

static int prvWifiApInfoCompare(const void *ctx1, const void *ctx2)
{
    const wifiApInfo_t *w1 = (const wifiApInfo_t *)ctx1;
    const wifiApInfo_t *w2 = (const wifiApInfo_t *)ctx2;
    return (w1->rssival - w2->rssival);
}

void atCmdHandleWifiScan(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_READ)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char resp[40];
        snprintf(resp, 40, "+WIFISCAN: <0-13>, <timeout>, <max_ap>");
        atCmdRespInfoText(cmd->engine, resp);
        RETURN_OK(cmd->engine);
    }

    drvWifi_t *d = gDrvWifi;
    if (d == NULL)
    {
        OSI_LOGE(0, "wifi scan open device first");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    uint8_t ch = 0;
    uint32_t timeout = WIFI_SCAN_DEFAULT_TIME;
    uint32_t maxap = WIFI_SCAN_DEFAULT_AP_CNT;
    uint32_t round = WIFI_SCAN_DEFAULT_ROUND;
    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        ch = atParamDefUintInRange(cmd->params[0], 0, 0, 13, &paramok);
        if (!paramok)
            goto param_end;

        timeout = atParamDefUintInRange(cmd->params[1], WIFI_SCAN_DEFAULT_TIME, 120, WIFI_SCAN_MAX_TIME, &paramok);
        if (!paramok)
            goto param_end;

        maxap = atParamDefUintInRange(cmd->params[2], WIFI_SCAN_DEFAULT_AP_CNT, 1, WIFI_SCAN_MAX_AP_CNT, &paramok);
        if (!paramok)
            goto param_end;

        if (ch != 0 && cmd->param_count > 3)
        {
            paramok = false;
            goto param_end;
        }

        round = atParamDefIntInRange(cmd->params[3], WIFI_SCAN_DEFAULT_ROUND, 1, WIFI_SCAN_MAX_ROUND, &paramok);
    param_end:
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    OSI_LOGI(0, "wifi scan channel %u/%u/%u(/%u)", ch, timeout, maxap, round);
    wifiApInfo_t *aps = (wifiApInfo_t *)calloc(maxap, sizeof(wifiApInfo_t));
    if (aps == NULL)
    {
        OSI_LOGE(0, "wifi sacn fail allocate memory %u", maxap * sizeof(wifiApInfo_t));
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
    }

    wifiScanRequest_t req = {
        .aps = aps,
        .max = maxap,
        .found = 0,
        .maxtimeout = timeout,
    };

    bool r = true;
    if (ch != 0)
    {
        r = drvWifiScanChannel(d, &req, ch);
    }
    else
    {
        for (unsigned n = 0; n < round; ++n)
        {
            for (ch = 1; ch <= WIFI_CHANNEL_MAX; ++ch)
            {
                r = drvWifiScanChannel(d, &req, ch);
                if (!r)
                {
                    break;
                }
            }
        }
    }

    if (r)
    {
        qsort(&req.aps[0], req.found, sizeof(wifiApInfo_t), prvWifiApInfoCompare);
        char resp[64];
        for (uint32_t i = 0; i < req.found; ++i)
        {
            wifiApInfo_t *w = &req.aps[i];
            snprintf(resp, 64, "%x%lx, %i, %u", w->bssid_high, w->bssid_low, w->rssival, w->channel);
            atCmdRespInfoText(cmd->engine, resp);
        }
    }

    free(aps);
    if (!r)
    {
        OSI_LOGE(0, "WIFI Scan %u start fail", ch);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        RETURN_OK(cmd->engine);
    }
}

#ifdef CONFIG_AT_WIFI_SENSITIVITY_TEST_SUPPORT

typedef struct
{
    uint32_t count;
    bool running;
} wifiSenTest_t;

static wifiSenTest_t gWifiSenTest = {};

static void _wifiScanIsrCB(const wifiApInfo_t *info, void *ctx)
{
    (*(uint32_t *)ctx) += 1;
}

void atCmdHandleWifiSensitivityTest(atCommand_t *cmd)
{
    const unsigned resp_size = 64;
    char resp[resp_size];
    if (cmd->type == AT_CMD_TEST)
    {
        // mode == 0, scan channel and report every <interval> milliseconds, report <1-100> times
        // mode == 1, start scan a channel, or stop scan and report beacon received count
        atCmdRespInfoText(cmd->engine, "+WIFISENTEST: 0, <1-13>, <interval>, <1-100>");
        atCmdRespInfoText(cmd->engine, "+WIFISENTEST: 1, <0-1>, <1-13>");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        drvWifi_t *d = gDrvWifi;
        if (d == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        uint8_t mode, start = 0;
        uint8_t ch = 0;
        unsigned interval = 1;
        unsigned round = 1;
        bool paramok = true;
        uint32_t pcnt = 0;
        do
        {
            mode = atParamUintInRange(cmd->params[pcnt++], 0, 1, &paramok);
            if (!paramok)
                break;

            if (mode == 1)
            {
                start = atParamUintInRange(cmd->params[pcnt++], 0, 1, &paramok);
                if (!paramok)
                    break;

                if (start == 0)
                    break;
            }

            ch = atParamUintInRange(cmd->params[pcnt++], 1, 13, &paramok);
            if (!paramok)
                break;

            if (mode == 0)
            {
                interval = atParamUint(cmd->params[pcnt++], &paramok);
                if (!paramok)
                    break;

                round = atParamUintInRange(cmd->params[pcnt++], 1, 100, &paramok);
            }
        } while (0);

        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 0)
        {
            uint32_t count = 0, total = 0;
            bool r = drvWifiScanAsyncStart(d, ch, _wifiScanIsrCB, &count);
            if (!r)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            for (unsigned i = 0; i < round; ++i)
            {
                osiThreadSleep(interval);
                uint32_t critical = osiEnterCritical();
                uint32_t tmp = count;
                total += count;
                count = 0;
                osiExitCritical(critical);
                snprintf(resp, resp_size, "+WIFISENTEST: %u -> %lu (%u)", mode, tmp, i);
                atCmdRespInfoText(cmd->engine, resp);
            }
            drvWifiScanAsyncStop(d);
            snprintf(resp, resp_size, "+WIFISENTEST: %u -> %lu (total)", mode, total);
            atCmdRespInfoText(cmd->engine, resp);
            RETURN_OK(cmd->engine);
        }
        else
        {
            wifiSenTest_t *ctx = &gWifiSenTest;
            if (start)
            {
                if (ctx->running)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                ctx->running = true;
                ctx->count = 0;
                bool r = drvWifiScanAsyncStart(d, ch, _wifiScanIsrCB, &ctx->count);
                if (!r)
                {
                    ctx->running = false;
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                RETURN_OK(cmd->engine);
            }
            else
            {
                if (!ctx->running)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                snprintf(resp, resp_size, "+WIFISENTEST: %u -> %lu", mode, ctx->count);
                drvWifiScanAsyncStop(d);
                ctx->count = 0;
                ctx->running = false;
                atCmdRespInfoText(cmd->engine, resp);
                RETURN_OK(cmd->engine);
            }
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#endif