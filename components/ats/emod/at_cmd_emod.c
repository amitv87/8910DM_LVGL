/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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
#include "cfw_config.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "osi_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "at_command.h"
#include "atr_config.h"

// TODO: CLAC ignore should use AT_CON_NOT_CLAC constraints
static const char *gClacIgnoreCmds[] = {
    "+CADTF",
    "+CPBW",
    "+CGMI",
    "^PMSTART",
    "+CAIET",
    "+CANXP",
    "+CAWTF",
    "+CCFCU",
    "+CPBF",
    "+SETLOCK",
    "^TRACECTRL",
    "+USBMODE",
    "+CAVQE",
    "+CCWA",
    "+UPDATE",
    "+RFCALIB",
    "^SWJTAG",
    "+CALIBINFO",
    "^HEAPINFO",
    "+CALIBINFO",
    "+DLST",
    "+CR",
    "+CRES",
    "E",
    "+CIPDPDP",
    "+PINGSTOP",
    "+SET7SRESET",
    "+CGSN",
    "+TSOST",
    "+CMER",
    "+LBTEST",
    "+MIPLCLEARSTATE",
    "+CMOD",
    "+SPENGMD",
    "+CSDF",
    "+CPLS",
    "+CLIP",
    "+CACM",
    "O",
    "+TSOCR",
    "+CGEQMIN",
    "+TSOSTF",
    "+CDIS",
    "+CACCP",
    "+ECSQ",
    "+CSAS",
    "+QGPCLASS",
    "+CLIR",
    "+CPBS",
    "^PMSTOP",
    "^TIMEOUTABORT",
    "+TSOCL",
    "+CAPRE",
    "+CGEQREQ",
    "+CAPOST",
    "^FORCEDNLD",
    "+TPING",
    "+TSORF",
    "^SSIT",
    "+CAVCT",
    "+CPBR",
    "&Y",
    "&D",
    "+NVPC",
    "+EMMCDDRSIZE",
    "+CGSEND",
    "+PCMPLAY",
    "^PSMEN",
    "+NVGV",
    "^UPTIME",
    "+CGMR",
    "+RRTMPARAM",
};

static bool prvClacForceIgnore(const atCmdDesc_t *desc)
{
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gClacIgnoreCmds); n++)
    {
        if (strcmp(gClacIgnoreCmds[n], desc->name) == 0)
            return true;
    }
    return false;
}

static int prvDescCmpByName(const void *a, const void *b)
{
    const atCmdDesc_t *pa = *(const atCmdDesc_t **)a;
    const atCmdDesc_t *pb = *(const atCmdDesc_t **)b;
    if (pa == NULL)
        return -1;
    if (pb == NULL)
        return 1;
    return strcmp(pa->name, pb->name);
}

void atCmdHandleCLAC(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        unsigned count = atCommandDescCount();
        const atCmdDesc_t **descs = (const atCmdDesc_t **)calloc(count, sizeof(descs[0]));
        if (descs == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        for (unsigned n = 0; n < count; n++)
        {
            const atCmdDesc_t *desc = atCommandDescByIndex(n);
            if (desc == NULL)
                continue;
            if (desc->constrains & AT_CON_NOT_CLAC)
                continue;
            if (prvClacForceIgnore(desc))
                continue;
            descs[n] = desc;
        }

        qsort(descs, count, sizeof(descs[0]), prvDescCmpByName);

        char rsp[128];
        for (unsigned n = 0; n < count; n++)
        {
            const atCmdDesc_t *desc = descs[n];
            if (desc == NULL)
                continue;

            sprintf(rsp, "AT%s", desc->name);
            atCmdRespInfoText(cmd->engine, rsp);
        }

        free(descs);
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#ifdef CONFIG_SOC_8910
uint8_t gTueInfoFlag = 0;
uint8_t gTueInfoSimID = 0;
osiTimer_t *tTueInfoTimer = NULL;
#define AT_TUE_INFO_MS 1000 * 1

static void _tueInfoPrintData(atCommand_t *cmd)
{
    CFW_TUE_STATUS_INFO ueStatsRpt;
    char rsp[250] = {
        0x00,
    };
    uint8_t nSim = gTueInfoSimID;

    CFW_GetStatusInfo(&ueStatsRpt, nSim);
    OSI_LOGI(0, ("TUEINFO:dlEarfcn:%d,pcid:%d,rsrp:%d,rsrq:%d,\r\n"), ueStatsRpt.dlEarfcn, ueStatsRpt.pcid, ueStatsRpt.rsrp, ueStatsRpt.rsrq);

    sprintf(rsp, "TUEINFO:DLEARFCN,%lu", ueStatsRpt.dlEarfcn);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:PCID,%u", ueStatsRpt.pcid);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:RSRP,%d", ueStatsRpt.rsrp);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:RSRQ,%d", ueStatsRpt.rsrq);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:SINR,%u", ueStatsRpt.sinr);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:MCL,%u", ueStatsRpt.mcl);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:ULMCS,%u", ueStatsRpt.ulMcs);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:DLMCS,%u", ueStatsRpt.dlMcs);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:MPDCCHREPNUM,%u", ueStatsRpt.mpdcchRepNum);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:PUSCHREPNUM,%u", ueStatsRpt.puschRepNum);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:PDSCHREPNUM,%u", ueStatsRpt.pdschRepNum);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:ULINITIALBLER,%u", ueStatsRpt.ulInitialBler);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:DLINITIALBLER,%u", ueStatsRpt.dlInitialBler);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:ULRBNUM,%lu", ueStatsRpt.ulRbNum);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:DLRBNUM,%lu", ueStatsRpt.dlRbNum);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:ULRLCRATE,%lu", ueStatsRpt.ulRlcRate);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:DLRLCRATE,%lu", ueStatsRpt.dlRlcRate);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:ULTBS,%lu", ueStatsRpt.ulTbs);
    atCmdRespSimUrcText(nSim, rsp);
    sprintf(rsp, "TUEINFO:DLTBS,%lu", ueStatsRpt.dlTbs);
    atCmdRespSimUrcText(nSim, rsp);

    return;
}
static void _tueInfoExePrintData(atCommand_t *cmd)
{
    CFW_TUE_STATUS_INFO ueStatsRpt;
    char rsp[250] = {
        0x00,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);

    CFW_GetStatusInfo(&ueStatsRpt, nSim);
    OSI_LOGI(0, ("TUEINFO:dlEarfcn:%d,pcid:%d,rsrp:%d,rsrq:%d,\r\n"), ueStatsRpt.dlEarfcn, ueStatsRpt.pcid, ueStatsRpt.rsrp, ueStatsRpt.rsrq);

    sprintf(rsp, "TUEINFO:DLEARFCN,%lu", ueStatsRpt.dlEarfcn);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:PCID,%u", ueStatsRpt.pcid);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:RSRP,%d", ueStatsRpt.rsrp);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:RSRQ,%d", ueStatsRpt.rsrq);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:SINR,%u", ueStatsRpt.sinr);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:MCL,%u", ueStatsRpt.mcl);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:ULMCS,%u", ueStatsRpt.ulMcs);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:DLMCS,%u", ueStatsRpt.dlMcs);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:MPDCCHREPNUM,%u", ueStatsRpt.mpdcchRepNum);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:PUSCHREPNUM,%u", ueStatsRpt.puschRepNum);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:PDSCHREPNUM,%u", ueStatsRpt.pdschRepNum);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:ULINITIALBLER,%u", ueStatsRpt.ulInitialBler);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:DLINITIALBLER,%u", ueStatsRpt.dlInitialBler);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:ULRBNUM,%lu", ueStatsRpt.ulRbNum);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:DLRBNUM,%lu", ueStatsRpt.dlRbNum);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:ULRLCRATE,%lu", ueStatsRpt.ulRlcRate);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:DLRLCRATE,%lu", ueStatsRpt.dlRlcRate);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:ULTBS,%lu", ueStatsRpt.ulTbs);
    atCmdRespInfoText(cmd->engine, rsp);
    sprintf(rsp, "TUEINFO:DLTBS,%lu", ueStatsRpt.dlTbs);
    atCmdRespInfoText(cmd->engine, rsp);

    return;
}

static void _tueInfoTimerCB(void *ctx)
{
    //atCommand_t *cmd = (atCommand_t *)ctx;

    _tueInfoPrintData(NULL);
    osiTimerStart(tTueInfoTimer, AT_TUE_INFO_MS);
    return;
}
void atCmdHandleTUEINFO(atCommand_t *cmd)
{
    char rspInfo[100] = {
        0x00,
    };

    OSI_LOGI(0, "Enter atCmdHandleTUEINFO!");

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        bool paramok = true;
        uint8_t nReportFlag = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);

        if (!paramok || cmd->param_count != 1)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));
        if (0 == nReportFlag)
        {
            if (tTueInfoTimer != NULL)
                osiTimerDelete(tTueInfoTimer);
            tTueInfoTimer = NULL;
        }
        else
        {
            gTueInfoSimID = atCmdGetSim(cmd->engine);
            tTueInfoTimer = osiTimerCreate(atEngineGetThreadId(), _tueInfoTimerCB, (void *)NULL);
            osiTimerStart(tTueInfoTimer, AT_TUE_INFO_MS);
        }
        gTueInfoFlag = nReportFlag;

        sprintf(rspInfo, "TUEINFO:%d", gTueInfoFlag);
        atCmdRespInfoText(cmd->engine, rspInfo);
        atCmdRespOK(cmd->engine);
    }
    break;

    case AT_CMD_EXE:
        _tueInfoExePrintData(cmd);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_READ:
        sprintf(rspInfo, "TUEINFO:%d", gTueInfoFlag);
        atCmdRespInfoText(cmd->engine, rspInfo);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "TUEINFO:(0,1)");
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
    OSI_LOGI(0, "atCmdHandleTUEINFO! end");
}
#ifdef CONFIG_AT_SSIM_SUPPORT

uint8_t gScanRssiFlag = 0;
osiTimer_t *tScanRssiTimer = NULL;
#define AT_SCAN_RSSI_MS 1000 * 1
#define GET_RSSIDATA_LEN_ONETIME 400

static bool _ScanRssiPrintData(uint8_t sim)
{
    bool bRet = true;
    uint16_t nFreqNums = 0;
    int16_t rssi[GET_RSSIDATA_LEN_ONETIME] = {
        0,
    };
    char rsp[250] = {
        0x00,
    };
    OSI_LOGI(0, "_ScanRssiPrintData start!");

    bRet = CFW_GetScanRssiInfo(0, 0, &nFreqNums, rssi, sim);
    if (!bRet)
    {
        OSI_LOGI(0, "_ScanRssiPrintData Cann't get data!");
        return bRet;
    }
    OSI_SXDUMP(0, "rssi:", rssi, 40);
    if (nFreqNums <= GET_RSSIDATA_LEN_ONETIME)
    {
        uint8_t i = 0;
        uint8_t index = 0;
        sprintf(rsp, "SCANRSSI:");

        atCmdRespSimUrcText(sim, rsp);
        for (i = 0; i < nFreqNums / 10; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }
        if (nFreqNums % 10 > 0)
        {
            uint8_t j = 0;
            for (j = 0; j < (nFreqNums - i * 10); j++)
            {
                sprintf(rsp, "%d\n", rssi[i * 10 + j]);
            }
            atCmdRespSimUrcText(sim, rsp);
        }

        return bRet;
    }
    else if (nFreqNums <= GET_RSSIDATA_LEN_ONETIME * 2)
    {
        uint8_t index = 0;
        uint16_t nSecondDataLen = 0;
        uint8_t i = 0;
        sprintf(rsp, "SCANRSSI:");
        atCmdRespSimUrcText(sim, rsp);
        //get the first block 400 data
        for (i = 0; i < 40; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }

        //get the second  block some data,
        CFW_GetScanRssiInfo(0, 1, &nFreqNums, rssi, sim);
        nSecondDataLen = nFreqNums - GET_RSSIDATA_LEN_ONETIME;
        for (uint8_t i = 0; i < nSecondDataLen / 10; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }
        if (nSecondDataLen % 10 > 0)
        {
            uint8_t j = 0;
            for (j = 0; j < (nSecondDataLen - i * 10); j++)
            {
                if ((i * 10 + j) < GET_RSSIDATA_LEN_ONETIME)
                    sprintf(rsp, "%d\n", rssi[i * 10 + j]);
            }
            atCmdRespSimUrcText(sim, rsp);
        }

        return bRet;
    }
    else if (nFreqNums <= 1000)
    {
        uint8_t index = 0;
        uint16_t nThirdDataLen = 0;
        uint8_t i = 0;
        sprintf(rsp, "SCANRSSI:");
        atCmdRespSimUrcText(sim, rsp);
        //get the first block 400 data
        for (i = 0; i < 40; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }
        //get the second block 400 data,
        CFW_GetScanRssiInfo(0, 1, &nFreqNums, rssi, sim);
        for (i = 0; i < 40; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }

        //get the third block some data
        CFW_GetScanRssiInfo(0, 2, &nFreqNums, rssi, sim);
        nThirdDataLen = nFreqNums - GET_RSSIDATA_LEN_ONETIME * 2;
        for (uint8_t i = 0; i < nThirdDataLen / 10; i++)
        {
            index = i * 10;
            sprintf(rsp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", rssi[index], rssi[index + 1], rssi[index + 2], rssi[index + 3], rssi[index + 4], rssi[index + 5], rssi[index + 6], rssi[index + 7], rssi[index + 8], rssi[index + 9]);
            atCmdRespSimUrcText(sim, rsp);
        }
        if (nThirdDataLen % 10 > 0)
        {
            uint8_t j = 0;
            for (j = 0; j < (nThirdDataLen - i * 10); j++)
            {
                if ((i * 10 + j) < GET_RSSIDATA_LEN_ONETIME)
                    sprintf(rsp, "%d\n", rssi[i * 10 + j]);
            }
            atCmdRespSimUrcText(sim, rsp);
        }
    }
    else
    {
        OSI_LOGI(0, "_ScanRssiPrintData data len ERROR!");
    }

    return bRet;
}
static void _ScanRssiTimerCB(void *ctx)
{
    char rsp[10] = {
        0,
    };
    OSI_LOGI(0, "Enter _ScanRssiTimerCB!");
    uint8_t sim = 0; // TODO

    if (!_ScanRssiPrintData(sim))
    {
        sprintf(rsp, "ERROR");
        atCmdRespSimUrcText(sim, rsp);
        return;
    }

    OSI_LOGI(0, "_ScanRssiTimerCB Restart timer!");
    osiTimerStart(tScanRssiTimer, AT_SCAN_RSSI_MS);
    return;
}

void atCmdHandleSCANRSSI(atCommand_t *cmd)
{
    char rspInfo[100] = {
        0x00,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);
    OSI_LOGI(0, "Enter atCmdHandleSCANRSSI!");

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        bool paramok = true;
        uint8_t band = 0;
        uint8_t nReportFlag = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
        if (cmd->param_count > 1)
            band = atParamDefUintInRange(cmd->params[1], 0, 0, 255, &paramok);

        if (!paramok || cmd->param_count > 2)
            AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

        gScanRssiFlag = nReportFlag;
        if (2 == nReportFlag)
        {
            if (tScanRssiTimer != NULL)
                osiTimerDelete(tScanRssiTimer);
            tScanRssiTimer = NULL;
        }
        else if (1 == nReportFlag)
        {
            tScanRssiTimer = osiTimerCreate(atEngineGetThreadId(), _ScanRssiTimerCB, (void *)NULL);
            osiTimerStart(tScanRssiTimer, AT_TUE_INFO_MS);
        }
        else //nReportFlag is 0
        {
            uint16_t pFreqsNums = 0;
            int16_t rssi[400] = {
                0,
            };

            OSI_LOGI(0, "SCANRSSI:band:%d", band);
            CFW_GetScanRssiInfo(band, 0xff, &pFreqsNums, rssi, nSim);
        }

        sprintf(rspInfo, "SCANRSSI:%d", gScanRssiFlag);
        atCmdRespInfoText(cmd->engine, rspInfo);
        atCmdRespOK(cmd->engine);
    }
    break;

    case AT_CMD_READ:
        sprintf(rspInfo, "SCANRSSI:%d", gScanRssiFlag);
        atCmdRespInfoText(cmd->engine, rspInfo);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
        atCmdRespInfoText(cmd->engine, "SCANRSSI:(0,1)");
        atCmdRespOK(cmd->engine);
        break;

    default:
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
    OSI_LOGI(0, "atCmdHandleSCANRSSI! end");
}
#endif //CONFIG_AT_SSIM_SUPPORT

#endif
