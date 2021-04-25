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
#include "ats_config.h"

#ifdef CONFIG_AT_OCEANCONNECT_SUPPORT

#include "osi_log.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "at_cfg.h"
#include "lwm2m_api.h"
#include "assert.h"
#include "stdio.h"
#include "cfw.h"
#include "at_cfg.h"
#include "unistd.h"
#include "ctype.h"
#include "nvm.h"

static int8_t gOceanConnectRef = -1;
static int8_t gRegStatus = 0;
static uint8_t nnmi_status = 0;
static uint32_t droped_count = 0;
atCmdEngine_t *gOcAtEngine = NULL;
typedef struct
{
    uint8_t *msg;
    uint32_t len;
} bufferd_msg_t;

static bufferd_msg_t bufferd_msg = {0};

#define DEBUG_CDP_EVENT 0

static void cdp_server_ind_process(osiEvent_t *pEvent)
{
    atCmdEngine_t *engine = (atCmdEngine_t *)pEvent->param1;
    uint8_t ref = (uint8_t)pEvent->param2;
    osiEvent_t *lwm2m_ev = (osiEvent_t *)pEvent->param3;
    bool engine_valid = false;

    uint32_t result = lwm2m_ev->param1;
    uint32_t result_param1 = lwm2m_ev->param2;
    uint32_t result_param2 = lwm2m_ev->param3;

    engine_valid = atCmdEngineIsValid(engine);
    OSI_LOGI(0, "cdp_server_ind_process pEvent->nEventId=%d,gOceanConnectRef=%d engine %d",
             pEvent->id, gOceanConnectRef, engine_valid);

    switch (pEvent->id)
    {
        //+LWM2MEVENT: <ref>, <evtid><CR>
    case EVETN_IND:
    {
#if DEBUG_CDP_EVENT
        char refstr[50] = {0};
        sprintf(refstr, "+LWM2MEVENT:%d, %s", ref, lwm2m_eventName(result));
        atCmdRespUrcText(engine, refstr);
#endif

#if 0
        if(result == LWM2M_EVENT_UPDATE_SUCCESS && gATCurrentRAIMode == 1)
        {
            uint8_t nSim;
            uint8_t nCID;
            CFW_GPRS_DATA *pGprsData = NULL;
            lwm2m_config_t *lwm2m_config = lwm2m_get_config(gOceanConnectRef);

            at_ModuleGetSimCid(&nSim, &nCID, lwm2m_config->nDLCI);
            if ((pGprsData = malloc(1 + sizeof(CFW_GPRS_DATA))) == NULL)
            {
                break;
            }

            memset(pGprsData, 0, sizeof(CFW_GPRS_DATA));
            pGprsData->pData[0]=0x01;
            pGprsData->nDataLength = 1;
            CFW_SetPsDataAttribute(&pGprsData->attribute, 0x01, 0, 0);
            uint32_t nAppSN = 0x00; //Just for compiling issue
            CFW_GprsSendData(nCID, pGprsData, nAppSN,nSim);
            free(pGprsData);
	    pGprsData = NULL;
        }
#endif
    }
    break;
    case SERVER_REGCMD_RESULT_IND:
    {
        if (gRegStatus == 0)
        {
            if (result)
            {
                gRegStatus = 1;
                if (engine_valid)
                    atCmdRespOK(engine);
            }
            else
            {
                gRegStatus = -1;
                if (engine_valid)
                    atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
    }
    break;
    case SERVER_QUIT_IND:
    {
        if (engine_valid)
        {
            if (result == 1) //quit cmd ok
                atCmdRespOK(engine);
            else if (lwm2m_is_pending_regcmd(ref))
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
        }
        gOceanConnectRef = -1;
        gOcAtEngine = NULL;
        lwm2m_free_task(ref);
        lwm2m_free_config(ref);
    }
    break;
    //+MIPLREAD: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, [<count>] <CR>
    case OBJ_READ_IND:
    {
        read_param_t *param = (read_param_t *)result_param1;
        if (param->objid == 19 && param->insid == 0 && param->resid == 0)
        {
            lwm2m_read_rsp(19, 0, 0, lwm2m_opaque, NULL, 0, true, true, ref);
        }
        else
            lwm2m_read_rsp(19, 0, 0, lwm2m_opaque, NULL, 0, true, false, ref);
#if DEBUG_CDP_EVENT
        {
            uint32_t msgid = result;
            char refstr[100] = {0};
            sprintf(refstr, "+MIPLREAD:%d,%ld,%d,%d,%d,%d", ref, msgid, param->objid, (short)param->insid, (short)param->resid, param->count);
            atCmdRespUrcText(engine, refstr);
        }
#endif
    }
    break;
    //+LWM2MWRITE: <ref>, <objectid>, <instanceid>, <resourceid>, <value>, <index><CR>
    case OBJ_WRITE_IND:
    {
        write_param_t *param = (write_param_t *)result_param1;
        uint32_t count = result_param2;

        if (param->objid == 19 && param->insid == 1 && param->resid == 0 && count == 1)
        {
            char refstr[550] = {0};
            if (nnmi_status == 1)
            {
                sprintf(refstr, "+NNMI: %d,", param->len);
                for (int i = 0; i < param->len; i++)
                {
                    char tmp[5] = {0};
                    sprintf(tmp, "%02x", param->data[i]);
                    strcat(refstr, tmp);
                }
            }
            else if (nnmi_status == 2)
                sprintf(refstr, "+NNMI");
            if (nnmi_status != 0)
                atCmdRespUrcText(engine, refstr);
            if (nnmi_status != 1)
            {
                if (bufferd_msg.msg != NULL)
                {
                    droped_count += bufferd_msg.len;
                    free(bufferd_msg.msg);
                    memset(&bufferd_msg, 0, sizeof(bufferd_msg));
                }
                bufferd_msg.msg = malloc(param->len);
                bufferd_msg.len = param->len;
                memcpy(bufferd_msg.msg, param->data, param->len);
            }
            lwm2m_write_rsp(1, ref);
        }
        else
        {
            OSI_LOGI(0, "OBJ_WRITE_IND Failed! objid=%d, insid=%d, resid=%d, count=%d", param->objid, param->insid, param->resid, count);
        }
    }
    break;
    case OBJ_EXE_IND:
    {
        // do server exe
        lwm2m_exec_rsp(1, ref);
    }
    break;
    //+MIPLOBSERVE: <ref>, <msgid>, <flag>,<objectid>, [<instanceid>], [<resourceid>] <CR>
    case OBJ_OBSERVE_IND:
    {
        uint32_t msgid = result;
        uint16_t objid = result_param1 >> 16;
        int16_t insid = result_param1;
        int16_t resid = result_param2 >> 16;
        uint8_t isAddObserve = result_param2;
        char refstr[100] = {0};
        if (insid == -1)
            sprintf(refstr, "+MIPLOBSERVE:%d,%lu,%d,%d,-1,-1", ref, msgid, isAddObserve, objid);
        else
        {
            sprintf(refstr, "+MIPLOBSERVE:%d,%lu,%d,%d,%d,%d", ref, msgid, isAddObserve, objid, insid, resid);
            if (objid == 19 && insid == 0) //&& resid == 0
            {
                lwm2m_set_observer(ref, isAddObserve);
            }
        }

        atCmdRespUrcText(engine, refstr);
    }
    break;
    case OBJ_OPERATE_RSP_IND:
    {
#if DEBUG_CDP_EVENT
        char refstr[100] = {0};
        sprintf(refstr, "+OBJ_OPERATE_RSP_IND:%d, %ld, %d", ref, result_param1, (int)result);
        atCmdRespUrcText(engine, refstr);
#endif
        if ((int)result_param1 == -1 && engine_valid)
        {
            if ((int)result != -1)
                atCmdRespOK(engine);
            else
            {
                //gOceanConnectRef = -1;
                atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL);
            }
        }
    }
    break;
    case FOTA_DOWNLOADING_IND:
    {
        char refstr[100] = {0};
        int download_seze = result_param1;
        if (download_seze == 0)
        {
            sprintf(refstr, "FOTA download start");
        }
        else
        {
            sprintf(refstr, "FOTA downloading ... %d B", download_seze);
        }
        atCmdRespUrcText(engine, refstr);
    }
    break;
    case FOTA_DOWNLOAD_FAILED_IND:
    {
        char refstr[100] = {0};
        sprintf(refstr, "FOTA download failed");
        atCmdRespUrcText(engine, refstr);
    }
    break;
    case FOTA_DOWNLOAD_SUCCESS_IND:
    {
        char refstr[100] = {0};
        sprintf(refstr, "FOTA download success");
        atCmdRespUrcText(engine, refstr);
    }
    break;
    case FOTA_PACKAGE_CHECK_IND:
    {
        char refstr[100] = {0};
        sprintf(refstr, "FOTA package check %s", result_param1 == 1 ? "success" : "failed");
        atCmdRespUrcText(engine, refstr);
    }
    break;
    case FOTA_UPGRADE_OK_IND:
    {
        char refstr[100] = {0};
        sprintf(refstr, "FOTA update success. Ready to reboot");
        atCmdRespUrcText(engine, refstr);
    }
    break;
    default:
    {
#if DEBUG_CDP_EVENT
        char refstr[50] = {0};
        sprintf(refstr, "+UNPROCESS EVENT:%ld", pEvent->id);
        atCmdRespUrcText(engine, refstr);
#endif
    }
    break;
    }
    OSI_LOGI(0, "cdp_server_ind_process exit pEvent->nEventId=%d,gOceanConnectRef=%d", pEvent->id, gOceanConnectRef);
    free(lwm2m_ev);
    lwm2m_ev = NULL;

    free(pEvent);
}

//AT+NCDPOPEN=<ip_addr>[,<port>][,<psk>]
void AT_CmdFunc_NCDPOPEN(atCommand_t *pParam)
{
    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint16_t uPort = 5683;
        const char *psk = NULL;
        const char *ipaddress = atParamStr(pParam->params[0], &paramok);

        if (gOcAtEngine != NULL && atCmdEngineIsValid(gOcAtEngine) && gOcAtEngine != pParam->engine)
        {
            OSI_LOGI(0, "AT+NCDPOPEN atEngine is not NULL 0x%x", gOcAtEngine);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        gOcAtEngine = pParam->engine;

        if (pParam->param_count > 1)
            uPort = atParamUintInRange(pParam->params[1], 0, 65535, &paramok);
        if (pParam->param_count > 2)
            psk = atParamStr(pParam->params[2], &paramok);

        if (netif_default == NULL)
        {
            OSI_LOGI(0, "AT+NCDPOPEN netif_default=NULL");
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_NO_NET_SERVICE));
        }

        if (gOceanConnectRef != -1)
        {
            OSI_LOGI(0x1000448e, "AT+NCDPOPEN gOceanConnectRef=%d", gOceanConnectRef);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        if (!paramok || pParam->param_count > 3)
        {
            OSI_LOGI(0, "AT+NCDPOPEN param error");
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (uPort == 0)
            uPort = 5638;

        //uint8_t imeisize = 0;
        uint8_t imei[16] = {0};
        char cmdline[200] = {0};

        //CFW_EmodGetIMEI(imei, &imeisize, atCmdGetSim(pParam->engine));
        uint8_t sim = (netif_default->sim_cid & 0xf0) >> 4;
        OSI_LOGI(0, "AT+NCDPOPEN sim:%d", sim);
        int nImeiLen = nvmReadImei(sim, (nvmImei_t *)imei);
        if (nImeiLen == -1)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        if (uPort == 5684)
        {
            if (psk == NULL)
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            else
                snprintf(cmdline, sizeof(cmdline) - 1, "-n %s -h %s -p %d -4 -t 60 -i %s -s %s", imei, ipaddress, uPort, imei, psk);
        }
        else
            sprintf(cmdline, "-n %s -h %s -p %d -4 -t 60", imei, ipaddress, uPort);
        int8_t ref = -1;
        ref = lwm2m_new_config((uint8_t *)cmdline);
        if (ref >= 0)
        {
            OSI_LOGI(0, "lwm2m_new_config success");
            //        lwm2mcfg_set_handler_ext(ref, atEngineGetThreadId(), (osiCallback_t)cdp_server_ind_process, pParam->engine, pParam->nDLCI);
            lwm2mcfg_set_handler_ext(ref, atEngineGetThreadId(), (osiCallback_t)cdp_server_ind_process, pParam->engine, 0);
            //uint8_t refstr[3] = {0};
            //sprintf(refstr, "Config :%d", ref);
            //atCmdRespInfoText(pParam->engine, refstr);
            lwm2m_ret_t ret = lwm2m_add_obj(19, 2, (uint8_t *)"11", 6, ref);

            if (ret != LWM2M_RET_OK || lwm2m_register(ref, 0, 60) != LWM2M_RET_OK)
            {
                OSI_LOGI(0, "AT+NCDPOPEN failed");
                lwm2m_free_config(ref);
                gOceanConnectRef = -1;
                gOcAtEngine = NULL;
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
            }
            else
            {
                gRegStatus = 0;
                gOceanConnectRef = ref;
                AT_SetAsyncTimerMux(pParam->engine, 60 + 120);
            }
        }
        else
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespOK(pParam->engine);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+NCDPCLOSE
void AT_CmdFunc_NCDPCLOSE(atCommand_t *pParam)
{
    if (pParam->type == AT_CMD_EXE)
    {
        if (pParam->param_count > 0)
        {
            OSI_LOGI(0, "AT+NCDPCLOSE pParam->param_count=%d", pParam->param_count);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (gOceanConnectRef == -1)
        {
            OSI_LOGI(0, "AT+NCDPCLOSE gOceanConnectRef=%d", gOceanConnectRef);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        if (gOcAtEngine != NULL && atCmdEngineIsValid(gOcAtEngine) && gOcAtEngine != pParam->engine)
        {
            OSI_LOGI(0, "AT+NCDPCLOSE at Engine invalid 0x%x", gOcAtEngine);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        gOcAtEngine = pParam->engine;

        if (lwm2m_unregister(gOceanConnectRef) == LWM2M_RET_OK)
            AT_SetAsyncTimerMux(pParam->engine, 60 + 120);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+NMGS=<length>,<data>
void AT_CmdFunc_NMGS(atCommand_t *pParam)
{
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (pParam->param_count != 2)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        uint32_t length = atParamUintInRange(pParam->params[0], 1, 512, &paramok);
        if (!paramok)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        OSI_LOGI(0, "AT_CmdFunc_NMGS param type = %d,gOceanConnectRef=%d\n", pParam->params[1]->type, gOceanConnectRef);

        const char *data = atParamHexRawText(pParam->params[1], length, &paramok);
        if (!paramok)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (gOcAtEngine != NULL && atCmdEngineIsValid(gOcAtEngine) && gOcAtEngine != pParam->engine)
        {
            OSI_LOGI(0, "AT+NMGS at Engine invalid 0x%x", gOcAtEngine);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        gOcAtEngine = pParam->engine;

        if (lwm2m_get_observer(gOceanConnectRef) == LWM2M_RET_OK && lwm2m_notify(19, 0, 0, (uint8_t *)data, length, lwm2m_opaque, gOceanConnectRef) == LWM2M_RET_OK)
            AT_SetAsyncTimerMux(pParam->engine, 60);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+NMGR
void AT_CmdFunc_NMGR(atCommand_t *pParam)
{
    if (pParam->type == AT_CMD_EXE)
    {
        char refstr[550] = {0};

        if (gOcAtEngine != NULL && atCmdEngineIsValid(gOcAtEngine) && gOcAtEngine != pParam->engine)
        {
            OSI_LOGI(0, "AT+NMGR at Engine invalid 0x%x", gOcAtEngine);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        gOcAtEngine = pParam->engine;

        if (bufferd_msg.msg != NULL)
        {
            sprintf(refstr, "%lu,", bufferd_msg.len);
            for (int i = 0; i < bufferd_msg.len; i++)
            {
                char tmp[5] = {0};
                sprintf(tmp, "%02x", bufferd_msg.msg[i]);
                strcat(refstr, tmp);
            }
            free(bufferd_msg.msg);
            memset(&bufferd_msg, 0, sizeof(bufferd_msg));
            atCmdRespInfoText(pParam->engine, refstr);
        }
        atCmdRespOK(pParam->engine);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+NNMI=<status>
void AT_CmdFunc_NNMI(atCommand_t *pParam)
{
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        uint8_t status = atParamUintInRange(pParam->params[0], 0, 2, &paramok);

        if (!paramok || pParam->param_count != 1)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (gOcAtEngine != NULL && atCmdEngineIsValid(gOcAtEngine) && gOcAtEngine != pParam->engine)
        {
            OSI_LOGI(0, "AT+NMGS at Engine invalid 0x%x", gOcAtEngine);
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        gOcAtEngine = pParam->engine;

        nnmi_status = status;
        atCmdRespOK(pParam->engine);
    }
    else if (pParam->type == AT_CMD_READ)
    {
        char refstr[10] = {0};
        sprintf(refstr, "+NNMI:%d", nnmi_status);
        atCmdRespInfoText(pParam->engine, refstr);
        atCmdRespOK(pParam->engine);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

int8_t get_Oceanconnect_ref(void)
{
    return gOceanConnectRef;
}

#endif
