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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('A', 'T', 'S', '4')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_log.h"
#include "at_cfw.h"
#include "cfw_errorcode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include "mal_api.h"

#define SIM_TYPE_TSIM 0x00
#define SIM_TYPE_VSIM 0x01
#define SIM_TYPE_SSIM 0x02
#define TESTING_CODE_4VSIM
//================================================
//Customer should modify the following code..
#ifdef TESTING_CODE_4VSIM // Testing code

uint8_t sim_work_type = SIM_TYPE_TSIM;
uint16_t setVsimOrUsim(uint8_t type)
{
    sim_work_type = type;
    OSI_LOGI(0, "call customer setVsimOrUsim, set SIM to %s", type == 2 ? "true SIM" : "v SIM");
    return 0;
}

uint16_t VSimReset(uint8_t *ATRData, uint8_t *ATRSize)
{
    const char atr[] = {0x00, 0x11, 0x22, 0x33, 0x44,
                        0x55, 0x66, 0x77, 0x88, 0x99,
                        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    *ATRSize = sizeof(atr);
    memcpy(ATRData, atr, *ATRSize);
    OSI_LOGI(0, "call customer VSimReset(%d)!", *ATRSize);
    return 0;
}

int processAPDUCommand(uint8_t *TxData, uint16_t TxSize, uint8_t *RxData, uint16_t *RxSize, uint8_t slot)
{
    const char rx[] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB,
                       0xAA, 0x99, 0x88, 0x77, 0x66,
                       0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
    *RxSize = sizeof(rx);
    memcpy(RxData, rx, *RxSize);
    OSI_LOGI(0, "call customer processAPDUCommand(%d)!", *RxSize);
    return 0x9000;
}

uint16_t simcardStatus(void)
{
    OSI_LOGI(0, "call customer simcardStatus");
    return sim_work_type;
}

#endif
//================================================

typedef int (*VSIM_COMMAND)(uint8_t *apdu_req, uint16_t apdu_req_len, uint8_t *apdu_rsp, uint16_t apdu_rsp_len, uint8_t slot);
uint16_t (*pFunSimVsimReset)(uint8_t *, uint8_t *, uint8_t) = NULL;
VSIM_COMMAND pFunSimVsimProcessApdu[CONFIG_NUMBER_OF_SIM];
uint8_t (*pFunSimGetCardMode)(uint8_t) = NULL;

unsigned int setVsimHandler(void *getSimType, void *vsimReset, void *vsimProcessApdu, uint8_t nSimID)
{
    if ((getSimType == NULL) || (vsimReset == NULL) || (vsimProcessApdu == NULL))
        return ~0;

    pFunSimGetCardMode = getSimType;
    pFunSimVsimReset = vsimReset;
    pFunSimVsimProcessApdu[nSimID] = vsimProcessApdu;
    return 0;
}

//=============================================================================
//CP invoke function begin
uint16_t sim_GetSimCardModeCB(uint8_t nSimID)
{
    OSI_LOGI(0, "call CFW_GetSimCardMode!");
    uint16_t mode = 0;
    if (pFunSimGetCardMode != NULL)
    {
        mode = pFunSimGetCardMode(nSimID);
        if ((mode == SIM_TYPE_VSIM) || (mode == SIM_TYPE_SSIM))
        {
            if ((pFunSimVsimReset == NULL) || (pFunSimVsimProcessApdu == NULL))
                return ~0;
        }
        mode = SIM_TYPE_VSIM; //Currently the protocol stack thinks vsim and ssim are the same operation.
        OSI_LOGI(0, "call User's function simcardStatus(%d)", mode);
    }
    else
    {
        mode = 0;
        OSI_LOGI(0, "default mode, user's interface didnot set, true sim");
    }
    return mode;
}

uint16_t sim_VsimResetCB(uint8_t *ATRData, uint8_t *ATRSize, uint8_t nSimID)
{
    OSI_LOGI(0, "call CFW_VsimReset!");
    uint16_t retval = ~0;
    if (pFunSimVsimReset != NULL)
        retval = pFunSimVsimReset(ATRData, ATRSize, nSimID);
    else
    {
        *ATRSize = 0;
    }
    return retval;
}

uint16_t sim_VsimProcessApduCB(uint8_t *TxData, uint16_t TxSize, uint8_t *RxData, uint16_t *RxSize, uint8_t nSimID)
{
    OSI_LOGI(0, "call CFW_VsimProcessAPDU!");

    OSI_LOGI(0, "TxData = %X, TxSize = %d, RxData = %X, RxSize = %d", TxData, TxSize, RxData, RxSize);
    for (int i = 0; i < TxSize; i++)
        OSI_LOGI(0, "TxData[%d] = %X", i, TxData[i]);
    uint16_t sw = 0;
    if (pFunSimVsimProcessApdu[nSimID])
        sw = pFunSimVsimProcessApdu[nSimID](TxData, TxSize, RxData, *RxSize, nSimID);
    OSI_LOGI(0, "RxData = %X, RxSize = %d, sw = 0x%X", RxData, *RxSize, sw);
    return sw;
}
//CP invoke function end
//=============================================================================

int vsimInit(int SimId, VSIM_COMMAND pfnCommand, int mode)
{
    pFunSimVsimProcessApdu[SimId] = pfnCommand;
    return ERR_SUCCESS;
}
int vsim_exit(int SimId)
{
    pFunSimVsimProcessApdu[SimId] = NULL;
    return 0;
}
int vsimQueryVirtual(int SimId)
{
    return gAtSetting.virtual_sim_card[SimId];
}
int vsimSetVirtual(int SimId, int mode)
{
    gAtSetting.virtual_sim_card[SimId] = mode;
    atCfgAutoSave();
    return 0;
}
uint16_t vsimReset(uint8_t *ATRData, uint8_t *ATRSize)
{
    return 0;
}
uint32_t vsimGetEO(uint8_t SimId, osiEventCallback_t *pVsimGetEORspCB, osiCallback_t *pVsimGetEORspTimeoutCB)
{
    return 0;
}

//================================================
//Let's use sim card 0 as vsim slot,and customer should kindly modify it.
#define VSIM_SIM_SLOT_ID 0

void atVsimInit()
{
    setVsimOrUsim(SIM_TYPE_VSIM);
    OSI_LOGI(0, "atVsimInit: Start");
    vsimInit(VSIM_SIM_SLOT_ID, (VSIM_COMMAND)processAPDUCommand, 0);
}

typedef struct
{
    unsigned errcode;
} atVsimCtx_t;

/*********************************************************
** Function: prvVsimResetFinish
** Description: The processing result of simVsimReset.
*********************************************************/
static void prvVsimResetFinish(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atVsimCtx_t *ctx = (atVsimCtx_t *)cmd->async_ctx;
    OSI_LOGI(0, "prvVsimResetFinish: errcode=%d", ctx->errcode);
    if (ctx->errcode == 0)
        RETURN_OK(cmd->engine);
    else
        RETURN_CME_ERR(cmd->engine, ctx->errcode);
}

/*********************************************************
** Function: prvVsimExeReset
** Description: Func is used to Call the interface simVsimReset.
*********************************************************/
static void prvVsimExeReset(void *param)
{
    atCommand_t *cmd = (atCommand_t *)param;
    atVsimCtx_t *ctx = (atVsimCtx_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    OSI_LOGI(0, "prvVsimExeReset: nSim=%d", nSim);
    ctx->errcode = simVsimReset(nSim);
    atEngineCallback(prvVsimResetFinish, cmd);
}

/********************************************************
** Function: atCmdHandleCVSIMRESET
** Description: AT Command reset vsim
** Used: AT^CVSIMRESET      AT^CVSIMRESET=?
********************************************************/
void atCmdHandleCVSIMRESET(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_EXE)
    {
        atVsimCtx_t *ctx = (atVsimCtx_t *)calloc(1, sizeof(*ctx));
        if (ctx == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
        cmd->async_ctx = ctx;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        OSI_LOGI(0, "Come in atCmdHandleCVSIMRESET Fun Exe Command");
        atCmdWorkerCall(cmd->engine, prvVsimExeReset, cmd);
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[10];
        sprintf(rsp, "+CRESET");
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}