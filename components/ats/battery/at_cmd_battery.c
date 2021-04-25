#include "at_cfw.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "ats_config.h"
#include "drv_charger.h"
#include <stdio.h>

uint16_t gPrePmInfo = 0xffff;

void atCmdHandleCBC(atCommand_t *cmd)
{
    char pOutStr[200] = {
        0x00,
    };

    switch (cmd->type)
    {
    case AT_CMD_TEST:
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        sprintf((char *)pOutStr, "+CBC: (0-5),(0-100)");
#else
        sprintf((char *)pOutStr, "+CBC: (0-2),(0-100),<voltage>");
#endif
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;

    case AT_CMD_EXE:
    {
        uint8_t nBcs = 0;
        uint8_t nBcl = 0;
        CFW_EMOD_BATTERY_INFO pBatInfo;

        drvChargerGetInfo(&nBcs, &nBcl);
        pBatInfo.nChargeInfo.nBcs = nBcs;
        pBatInfo.nChargeInfo.nBcl = nBcl;
#ifndef CONFIG_QUEC_PROJECT_FEATURE
        sprintf((char *)pOutStr, "+CBC: %u,%u", pBatInfo.nChargeInfo.nBcs, pBatInfo.nChargeInfo.nBcl);
#else
        sprintf((char *)pOutStr, "+CBC: %u,%u,%u", pBatInfo.nChargeInfo.nBcs, pBatInfo.nChargeInfo.nBcl, drvChargerGetVbatRT());
#endif
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
        break;
    }

    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
void atNotcieClientPmInfo(const osiEvent_t *event)
{
    char pOutStr[200] = {
        0x00,
    };

    uint8_t nBcs = 0;
    uint8_t nBcl = 0;
    uint16_t info;

    drvChargerGetInfo(&nBcs, &nBcl);
    info = nBcs << 8 | nBcl;
    if (gPrePmInfo == info)
    {
        return;
    }
    gPrePmInfo = info;
    if (0 == nBcs)
        sprintf((char *)pOutStr, "+CIEV: No Charger, capacity %u", nBcl);
    else if (2 == nBcs)
        sprintf((char *)pOutStr, "+CIEV: Charging, capacity %u", nBcl);
    else if (3 == nBcs)
    {
        if (nBcl != 100)
            sprintf((char *)pOutStr, "+CIEV: Charging, capacity %u", nBcl);
        else
            sprintf((char *)pOutStr, "+CIEV: Charge Full, capacity %u", nBcl);
    }

    atCmdRespDefUrcText(pOutStr);
}

void atSendNoticeEvent(void)
{
    osiEvent_t pm_event = {.id = EV_PM_BC_IND};
    osiEventSend(atEngineGetThreadId(), &pm_event);
}

void atPmInit(void)
{
    atEventRegister(EV_PM_BC_IND, atNotcieClientPmInfo);
    drvChargerSetNoticeCB(atSendNoticeEvent);
}
