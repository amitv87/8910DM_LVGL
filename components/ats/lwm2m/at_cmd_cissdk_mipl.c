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

#ifdef CONFIG_AT_CISSDK_MIPL_SUPPORT
#include "at_command.h"
#include "at_engine.h"
#include "stdio.h"
#include "cfw.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "unistd.h"
#include "cis_internals.h"
#include <cis_def.h>
#include <cis_api.h>
#include <cis_if_sys.h>
//#include <cis_log.h>
#include <cis_list.h>
#include "at_response.h"
#include "ctype.h"
#include <float.h>

typedef enum
{
    EVETN_IND = 0,
    SAMPLE_CALLBACK_READ,
    SAMPLE_CALLBACK_WRITE,
    SAMPLE_CALLBACK_EXECUTE,
    SAMPLE_CALLBACK_OBSERVE,
    SAMPLE_CALLBACK_SETPARAMS,
    SAMPLE_CALLBACK_DISCOVER,
    // TODO:SAMPLE_CALLBACK_RFACK,
    // TODO:SAMPLE_CALLBACK_NOTIFY,
} at_callback_type_t;

struct st_observe_info
{
    struct st_observe_info *next;
    cis_listid_t mid;
    cis_uri_t uri;
    cis_observe_attr_t params;
};

struct st_callback_info
{
    struct st_callback_info *next;
    cis_listid_t mid;
    at_callback_type_t flag;
    cis_uri_t uri;

    union {
        struct
        {
            cis_data_t *value;
            cis_attrcount_t count;
        } asWrite;

        struct
        {
            uint8_t *buffer;
            uint32_t length;
        } asExec;

        struct
        {
            bool flag;
        } asObserve;

        struct
        {
            cis_observe_attr_t params;
        } asObserveParam;
    } param;
};

typedef enum _RESPONSE_TYPE_T
{
    TYPE_OBSERVER = 0x10000000,
    TYPE_DISCOVER = 0x01000000,
    TYPE_READ = 0x00100000,
    TYPE_WRITE = 0x11000000,
    TYPE_EXECUTE = 0x01100000,
    TYPE_PARAM = 0x11100000,
} RESPONSE_TYPE_T;

static bool gRegState = false;
static bool gWaitCLose = false;
atCmdEngine_t *gEngine = NULL;
static cis_oid_t g_objectid = URI_INVALID;
static uint32_t g_ResponseMsgId = 0;
static uint32_t g_ResponseMagicID = 0;
static void *g_context = NULL;

static void cis_event_ind_process(void *param)
{
    OSI_LOGI(0x0, "enter cis_event_ind_process");
    osiEvent_t *pEvent = (osiEvent_t *)param;
    st_context_t *context = (st_context_t *)pEvent->param1;
    atCmdEngine_t *engine = gEngine;
    struct st_callback_info *newNode = (struct st_callback_info *)pEvent->param2;
    switch (pEvent->id)
    {
    //+MIPLEVENT: <ref>, <evtid><CR>
    case EVETN_IND:
    {
        switch (pEvent->param2)
        {
        case CIS_EVENT_RESPONSE_SUCCESS:
        {
            OSI_LOGI(0x0, "cis_on_event response ok mid:%ld\n", (uint32_t)pEvent->param3);
            atCmdRespOK(gEngine);
        }
        break;
        case CIS_EVENT_RESPONSE_FAILED:
        {
            OSI_LOGI(0x0, "cis_on_event response failed mid:%ld\n", (uint32_t)pEvent->param3);
            if (g_ResponseMsgId != (uint32_t)pEvent->param3)
            {
                g_ResponseMsgId = (uint32_t)pEvent->param3;
                atCmdRespCmeError(gEngine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                free(pEvent);
                return;
            }
        }
        break;
        case CIS_EVENT_NOTIFY_SUCCESS:
        {
            OSI_LOGI(0x0, "cis_on_event notify ok mid:%ld\n", (uint32_t)pEvent->param3);
            atCmdRespOK(gEngine);
        }
        break;
        case CIS_EVENT_NOTIFY_FAILED:
        {
            OSI_LOGI(0x0, "cis_on_event notify failed mid:%ld\n", (uint32_t)pEvent->param3);
            atCmdRespCmeError(gEngine, ERR_AT_CME_EXE_FAIL);
        }
        break;
        case CIS_EVENT_UPDATE_NEED:
            OSI_LOGI(0x0, "cis_on_event need to update,reserve time:%lds\n", (uint32_t)pEvent->param3);
            cis_update_reg(g_context, LIFETIME_INVALID, false);
            break;
        case CIS_EVENT_REG_SUCCESS:
            gRegState = true;
            break;
        case CIS_EVENT_UNREG_DONE:
            gRegState = false;
            if (gWaitCLose)
            {
                atCmdRespOK(gEngine);
                gWaitCLose = false;
            }
        default:
            break;
        }

        char refstr[50] = {0};
        sprintf(refstr, "+MIPLEVENT:%d,%ld", 0, pEvent->param2);
        atCmdRespUrcText(engine, refstr);
    }
    break;
    //+MIPLREAD: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, [<count>] <CR>
    case SAMPLE_CALLBACK_READ:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        st_object_t *obj = (st_object_t *)cis_findObject((void *)context, uri.objectId);
        uint32_t res_count = 0;
        if (obj != NULL)
            res_count = obj->attributeCount + obj->actionCount;
        if (1 /*uri.instanceId >= 0 && uri.resourceId > 0*/)
        {
            sprintf(refstr, "+MIPLREAD:%d,%ld,%d,%d,%d", 0, mid, uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1,
                    CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1);
        }
        else
        {
            sprintf(refstr, "+MIPLREAD:%d,%ld,%d,%d,%d,%ld", 0, mid, uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1,
                    CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1, res_count);
        }
        g_ResponseMagicID = (mid | TYPE_READ);
        atCmdRespUrcText(engine, refstr);
        free(newNode);
    }
    break;
    //+MIPLWRITE: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, <valuetype>, <len>, <value>, <index><CR>
    case SAMPLE_CALLBACK_WRITE:
    {
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        cis_data_t *value = newNode->param.asWrite.value;
        int16_t count = (int16_t)newNode->param.asWrite.count;
        while (--count >= 0)
        {
            char *refstr = NULL;
            refstr = (char *)malloc(value[count].asBuffer.length * 2 + 100);
            if (NULL == refstr)
            {
                memset(value[count].asBuffer.buffer, 0, value[count].asBuffer.length);
                free(value[count].asBuffer.buffer);
                value[count].asBuffer.buffer = NULL;
                break;
            }
            memset(refstr, 0, value[count].asBuffer.length * 2 + 100);

            char *hexStr = NULL;
            hexStr = (char *)malloc(value[count].asBuffer.length * 2 + 2);
            if (NULL == hexStr)
            {
                memset(value[count].asBuffer.buffer, 0, value[count].asBuffer.length);
                free(value[count].asBuffer.buffer);
                value[count].asBuffer.buffer = NULL;
                memset(refstr, 0, value[count].asBuffer.length * 2 + 100);
                free(refstr);
                refstr = NULL;
                break;
            }
            memset(hexStr, 0, value[count].asBuffer.length * 2 + 2);

            for (int i = 0; i < value[count].asBuffer.length; i++)
            {
                sprintf(hexStr + 2 * i, "%02X", value[count].asBuffer.buffer[i]);
            }

            sprintf(refstr, "+MIPLWRITE:%d,%ld,%d,%d,%d,%d,%ld,%s,0,%d", 0, mid,
                    uri.objectId, uri.instanceId, value[count].id, value[count].type,
                    value[count].asBuffer.length, hexStr, count);
            g_ResponseMagicID = (mid | TYPE_WRITE);
            atCmdRespUrcText(engine, refstr);
            memset(hexStr, 0, value[count].asBuffer.length * 2 + 2);
            free(hexStr);
            hexStr = NULL;
            memset(refstr, 0, value[count].asBuffer.length * 2 + 100);
            free(refstr);
            refstr = NULL;
            memset(value[count].asBuffer.buffer, 0, value[count].asBuffer.length);
            free(value[count].asBuffer.buffer);
            value[count].asBuffer.buffer = NULL;
        }
        free(newNode);
    }
    break;
    //+MIPLEXECUTE: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>[, <len>, <arguments>]<CR>
    case SAMPLE_CALLBACK_EXECUTE:
    {
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        uint8_t *buffer = newNode->param.asExec.buffer;
        uint32_t length = newNode->param.asExec.length;

        char *refstr = NULL;
        refstr = (char *)malloc(length + 100);
        if (refstr == NULL)
        {
            memset(newNode->param.asExec.buffer, 0, newNode->param.asExec.length);
            free(newNode->param.asExec.buffer);
            free(newNode);
            return;
        }
        g_ResponseMagicID = (mid | TYPE_EXECUTE);
        sprintf(refstr, "+MIPLEXECUTE:%d,%ld,%d,%d,%d,%ld,\"%s\"", 0, mid, uri.objectId, uri.instanceId, uri.resourceId, length, buffer);
        atCmdRespUrcText(engine, refstr);
        memset(newNode->param.asExec.buffer, 0, newNode->param.asExec.length);
        free(newNode->param.asExec.buffer);
        memset(refstr, 0, length + 100);
        free(refstr);
        free(newNode);
    }
    break;
    //+MIPLOBSERVE: <ref>, <msgid>, <flag>,<objectid>, [<instanceid>], [<resourceid>] <CR>
    case SAMPLE_CALLBACK_OBSERVE:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        g_ResponseMagicID = (mid | TYPE_OBSERVER);
        sprintf(refstr, "+MIPLOBSERVE:%d,%ld,%d,%d,%d,%d", 0, mid, newNode->param.asObserve.flag,
                uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1, CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1);
        atCmdRespUrcText(engine, refstr);
        free(newNode);
    }
    break;
    //+MIPLPARAMETER: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, <parameter>, <len><CR>
    case SAMPLE_CALLBACK_SETPARAMS:
    {
        char refstr[200] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        g_ResponseMagicID = (mid | TYPE_PARAM);
        cis_observe_attr_t parameters = newNode->param.asObserveParam.params;
        char tmp[100] = {0};
        sprintf(tmp, "pmin=%ld; pmax=%ld; gt=%f; lt=%f; stp=%f", parameters.minPeriod,
                parameters.maxPeriod, parameters.greaterThan, parameters.lessThan, parameters.step);
        sprintf(refstr, "+MIPLPARAMETER:%d,%ld,%d,%d,%d,%d,%s", 0, mid, uri.objectId, uri.instanceId, uri.resourceId,
                strlen(tmp), tmp);
        atCmdRespUrcText(engine, refstr);
        free(newNode);
    }
    break;
    //+MIPLDISCOVER:<ref>, <msgid>, <objectid><CR>
    case SAMPLE_CALLBACK_DISCOVER:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        sprintf(refstr, "+MIPLDISCOVER:%d,%ld,%d", 0, mid, uri.objectId);
        g_ResponseMagicID = (mid | TYPE_DISCOVER);
        g_objectid = uri.objectId;
        atCmdRespUrcText(engine, refstr);
        free(newNode);
    }
    break;

    default:
        break;
    }
    free(pEvent);
}

static bool g_shutdown = false;

cis_version_t ver;
osiThread_t *hThread = NULL;
osiSemaphore_t *pumpSema = NULL;

static uint8_t g_authCode[16] = {0};

#define CIS_URI_EQUAL(oriUri, desUri) (((oriUri)->flag == (desUri)->flag) && ((oriUri)->objectId == (desUri)->objectId) && ((oriUri)->instanceId == (desUri)->instanceId) && ((oriUri)->resourceId == (desUri)->resourceId))

static cis_data_t *prv_dataDup(const cis_data_t *value, cis_attrcount_t attrcount)
{
    cis_data_t *newData;
    newData = (cis_data_t *)malloc(attrcount * sizeof(cis_data_t));
    if (newData == NULL)
    {
        return NULL;
    }
    cis_attrcount_t index;
    for (index = 0; index < attrcount; index++)
    {
        newData[index].id = value[index].id;
        newData[index].type = value[index].type;
        newData[index].asBuffer.length = value[index].asBuffer.length;
        newData[index].asBuffer.buffer = (uint8_t *)malloc(value[index].asBuffer.length + 1);
        memset(newData[index].asBuffer.buffer, 0, value[index].asBuffer.length + 1);
        memcpy(newData[index].asBuffer.buffer, value[index].asBuffer.buffer, value[index].asBuffer.length);
        memcpy(&newData[index].value.asInteger, &value[index].value.asInteger, sizeof(newData[index].value));
    }
    return newData;
}

static void cis_PostEvent(void *context, uint32_t event_id, void *param1, void *param2)
{
    osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
    ev->id = event_id;
    ev->param1 = (uint32_t)context;
    ev->param2 = (uint32_t)param1;
    ev->param3 = (uint32_t)param2;
    osiThreadCallback(atEngineGetThreadId(), (osiCallback_t)cis_event_ind_process, ev);
}

//////////////////////////////////////////////////////////////////////////
cis_coapret_t cis_onRead(void *context, cis_uri_t *uri, cis_mid_t mid)
{
    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_READ;
    newNode->mid = mid;
    newNode->uri = *uri;

    OSI_LOGI(0x0, "cis_onRead:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);

    cis_PostEvent(context, SAMPLE_CALLBACK_READ, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t cis_onDiscover(void *context, cis_uri_t *uri, cis_mid_t mid)
{

    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_DISCOVER;
    newNode->mid = mid;
    newNode->uri = *uri;

    OSI_LOGI(0x0, "cis_onDiscover:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);

    cis_PostEvent(context, SAMPLE_CALLBACK_DISCOVER, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t cis_onWrite(void *context, cis_uri_t *uri, const cis_data_t *value, cis_attrcount_t attrcount, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0x0, "cis_onWrite:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }
    else
    {
        OSI_LOGI(0x0, "cis_onWrite:(%d/%d)\n", uri->objectId, uri->instanceId);
    }

    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_WRITE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asWrite.count = attrcount;
    newNode->param.asWrite.value = prv_dataDup(value, attrcount);

    cis_PostEvent(context, SAMPLE_CALLBACK_WRITE, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t cis_onExec(void *context, cis_uri_t *uri, const uint8_t *value, uint32_t length, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0x0, "cis_onExec:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }
    else
    {
        return CIS_CALLBACK_METHOD_NOT_ALLOWED;
    }

    if (!CIS_URI_IS_SET_INSTANCE(uri))
    {
        return CIS_CALLBACK_BAD_REQUEST;
    }

    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_EXECUTE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asExec.buffer = (uint8_t *)malloc(length + 1);
    memset(newNode->param.asExec.buffer, 0, length + 1);
    newNode->param.asExec.length = length;
    memcpy(newNode->param.asExec.buffer, value, length);

    cis_PostEvent(context, SAMPLE_CALLBACK_EXECUTE, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t cis_onObserve(void *context, cis_uri_t *uri, bool flag, cis_mid_t mid)
{
    OSI_LOGI(0x0, "cis_onObserve mid:%ld uri:(%d/%d/%d)\n", mid, uri->objectId, uri->instanceId, uri->resourceId);

    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_OBSERVE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asObserve.flag = flag;

    if (newNode->param.asObserve.flag)
    {

        OSI_LOGI(0, "cis_on_observe set: %d/%d/%d",
                 uri->objectId,
                 CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : -1,
                 CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : -1);
    }
    else
    {
        OSI_LOGI(0, "cis_on_observe cancel: %d/%d/%d\n",
                 uri->objectId,
                 CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : -1,
                 CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : -1);
    }
    cis_PostEvent(context, SAMPLE_CALLBACK_OBSERVE, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t cis_onParams(void *context, cis_uri_t *uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0x0, "cis_on_params:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }

    if (!CIS_URI_IS_SET_INSTANCE(uri))
    {
        return CIS_CALLBACK_BAD_REQUEST;
    }

    struct st_callback_info *newNode = (struct st_callback_info *)malloc(sizeof(struct st_callback_info));
    newNode->next = NULL;
    newNode->flag = SAMPLE_CALLBACK_SETPARAMS;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asObserveParam.params = parameters;
    cis_PostEvent(context, SAMPLE_CALLBACK_SETPARAMS, newNode, NULL);
    return CIS_CALLBACK_CONFORM;
}

void cis_onEvent(void *context, cis_evt_t eid, void *param)
{
    OSI_LOGI(0x0, "cis_on_event(%ld):%s\n", eid, STR_EVENT_CODE(eid));

    cis_PostEvent(context, EVETN_IND, (void *)eid, param);
}

static void lwm2m_clientd(void *param)
{
    while (!g_shutdown)
    {
        /*pump function*/
        cis_pump(g_context);
        osiThreadSleepRelaxed(100, OSI_DELAY_MAX);
    }
    hThread = NULL;
    OSI_LOGI(0x0, "lwm2m_clientd will exit...");
    osiThreadExit();
}

static void cis_CommandNotResponse(atCommand_t *cmd)
{
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT));
}

void AT_CmdFunc_MIPLCREATE(atCommand_t *pParam)
{
    if (AT_CMD_SET == pParam->type || AT_CMD_EXE == pParam->type)
    {
        bool paramok = true;
        static uint8_t *databuff = NULL;
        static uint32_t offset = 0;
        uint32_t currentsize = 0;
        uint32_t totalsize = 0;
        uint8_t index = 0;
        uint8_t flags = 0;
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (AT_CMD_SET == pParam->type && pParam->param_count == 5)
        {
            currentsize = atParamUint(pParam->params[3], &paramok);
            if (!paramok)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }
            totalsize = atParamUintInRange(pParam->params[0], 0, 10240, &paramok);
            if (!paramok)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }
            const char *config = atParamHexRawText(pParam->params[1], currentsize, &paramok);
            if (!paramok)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }
            index = atParamUintInRange(pParam->params[2], 0, 255, &paramok);
            if (!paramok)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }
            flags = atParamUintInRange(pParam->params[4], 0, 2, &paramok);
            if (!paramok)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }

            uint32_t config_length = pParam->params[1]->length;

            if ((totalsize < currentsize) ||
                (totalsize < config_length) ||
                (config_length != currentsize) ||
                (index == 0 && flags != 0) ||
                (index != 0 && flags == 0) ||
                (totalsize < (currentsize + offset)))
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto CLEAN;
            }

            if ((flags == 1) || ((flags == 0)))
            {
                if (databuff == NULL)
                {
                    databuff = malloc(totalsize);
                    memset(databuff, 0x00, totalsize);
                }
                memcpy(databuff + offset, config, config_length);
                offset += config_length;

                if (index != 0)
                {
                    gEngine = pParam->engine;
                    AT_CMD_RETURN(atCmdRespOK(pParam->engine));
                }
            }
            else
            {
                if (databuff == NULL)
                {
                    AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                else
                {
                    memcpy(databuff + offset, config, config_length);
                    offset += config_length;
                    gEngine = pParam->engine;
                    AT_CMD_RETURN(atCmdRespOK(pParam->engine));
                }
            }
        }
        else if (AT_CMD_EXE == pParam->type)
        {
            const uint8_t config[] = {0x13, 0x00, 0x43,
                                      0xf1, 0x00, 0x03,
                                      0xf2, 0x00, 0x35,
                                      0x04, 0x00 /*mtu*/, 0x11 /*Link & bind type*/, 0x80 /*BS DTLS ENABLED*/,
                                      0x00, 0x05 /*apn length*/, 0x43, 0x4d, 0x49, 0x4f, 0x54 /*apn: CMIOT*/,
                                      0x00, 0x00 /*username length*/, /*username*/
                                      0x00, 0x00 /*password length*/, /*password*/
                                      0x00, 0x12 /*host length*/, 0x31, 0x38, 0x33, 0x2e, 0x32, 0x33, 0x30, 0x2e, 0x34, 0x30, 0x2e, 0x33, 0x39, 0x3a, 0x35, 0x36, 0x38, 0x33 /*host: 183.230.40.39:5683*/,
                                      0x00, 0x0d /*userdata length*/, 0x50, 0x53, 0x4b, 0x3a, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3b /*userdata: PSK:12345678;*/,
                                      0xf3, 0x00, 0x08,
                                      0xe4 /*log config*/, 0x00, 0xc8 /*LogBufferSize: 200*/,
                                      0x00, 0x00 /*userdata length*/ /*userdata*/};

            currentsize = sizeof(config);
            totalsize = sizeof(config);
            index = 0;
            flags = 0;

            if (databuff == NULL)
            {
                databuff = malloc(totalsize);
                memset(databuff, 0, totalsize);
            }
            memcpy(databuff + offset, config, currentsize);
            offset += currentsize;
        }
        else
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        OSI_LOGI(0x0, "mipl config totalsize: %ld", totalsize);

        sys_arch_dump(databuff, offset);

        if (cis_init(&g_context, databuff, offset, NULL) != CIS_RET_OK)
        {
            OSI_LOGI(0x0, "cis entry init failed.\n");
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            goto CLEAN;
        }

        cis_version(&ver);
        OSI_LOGI(0x0, "CIS SDK Version:%u.%u.%u\n", ver.major, ver.minor, ver.micro);

        g_shutdown = false;

        pumpSema = osiSemaphoreCreate(1, 0);
        hThread = osiThreadCreate("cis_lwm2m",
                                  lwm2m_clientd,
                                  NULL,
                                  232,
                                  2048 * 8,
                                  32);
        atCmdRespInfoText(pParam->engine, "+MIPLCREATE:0");
        gEngine = pParam->engine;
        atCmdRespOK(pParam->engine);

    CLEAN:
        if (databuff != NULL)
        {
            memset(databuff, 0, offset);
            free(databuff);
            databuff = NULL;
            offset = 0;
        }
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        atCmdRespOK(pParam->engine);
    }
    else
    {
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

//AT+MIPLDELETE=<ref><CR>
void AT_CmdFunc_MIPLDELETE(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (atCmdEngineIsValid(gEngine) && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 1)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema != NULL)
            osiSemaphoreRelease(pumpSema);
        else
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        if (cis_isRegister(g_context))
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        if (hThread != NULL)
        {
            g_shutdown = true;
            while (hThread != NULL)
                osiThreadSleep(100);
        }

        if (cis_deinit(&g_context) == CIS_RET_OK)
        {
            atCmdRespOK(pParam->engine);
            osiSemaphoreDelete(pumpSema);
            pumpSema = NULL;
            gEngine = NULL;

            if (g_authCode != NULL)
            {
                memset(g_authCode, 0x00, 16);
            }
        }
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLOPEN=<ref>,<lifetime>[,<timeout>] <CR>
void AT_CmdFunc_MIPLOPEN(atCommand_t *pParam)
{
    bool paramok = true;
    unsigned char ucRef = 0;

    if (gEngine != NULL && gEngine != pParam->engine)
    {
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        return;
    }

    if (gRegState)
    {
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        return;
    }

    if (pParam->type == AT_CMD_SET)
    {
        if (pParam->param_count != 2 && pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t lifetime = atParamUintInRange(pParam->params[1], LIFETIME_LIMIT_MIN, LIFETIME_LIMIT_MAX, &paramok);
        if (true != paramok)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        uint32_t timeout = 60;
        if (pParam->param_count == 3)
        {
            timeout = atParamUintInRange(pParam->params[2], 0, 60, &paramok);
            if (true != paramok)
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        OSI_LOGI(0x0, "mipl open timeout: %ld", timeout);

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        cis_callback_t callback;
        callback.onRead = cis_onRead;
        callback.onWrite = cis_onWrite;
        callback.onExec = cis_onExec;
        callback.onObserve = cis_onObserve;
        callback.onSetParams = cis_onParams;
        callback.onEvent = cis_onEvent;
        callback.onDiscover = cis_onDiscover;

        if (cis_register(g_context, lifetime, &callback) == CIS_RET_OK)
        {
            atCmdRespOK(pParam->engine);
        }
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLCLOSE=<ref><CR>
void AT_CmdFunc_MIPLCLOSE(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;
    if (gEngine != NULL && gEngine != pParam->engine)
    {
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        return;
    }

    if (pParam->type == AT_CMD_SET)
    {
        if (pParam->param_count != 1)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        if (cis_unregister(g_context) == CIS_RET_OK)
        {
            gWaitCLose = true;
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        }
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLADDOBJ=<ref>, <objectid>, <instancecount>, <instancebitmap>, <attributecount>£¬<actioncount><CR>
void AT_CmdFunc_MIPLADDOBJ(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 6)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        bool paramok = true;
        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        uint16_t objid = atParamUintInRange(pParam->params[1], 1, URI_MAX_ID, &paramok);
        uint32_t instCounts = atParamUint(pParam->params[2], &paramok);
        const char *instBitmap = atParamStr(pParam->params[3], &paramok);
        uint8_t attcount = atParamUintInRange(pParam->params[4], 0, 255, &paramok);
        uint8_t actcount = atParamUintInRange(pParam->params[5], 0, 255, &paramok);
        if (!paramok || attcount + actcount == 0 || instCounts != strlen(instBitmap))
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        cis_inst_bitmap_t bitmap;
        cis_res_count_t rescount;
        cis_instcount_t instCount, instBytes;
        const char *instAsciiPtr;
        uint8_t *instPtr;
        cis_oid_t oid;
        int16_t i;
        oid = objid;
        instCount = (uint16_t)strlen(instBitmap);
        instBytes = (instCount - 1) / 8 + 1;
        instAsciiPtr = instBitmap;

        instPtr = (uint8_t *)malloc(instBytes);
        memset(instPtr, 0, instBytes);

        for (i = 0; i < instCount; i++)
        {
            cis_instcount_t instBytePos = i / 8;
            cis_instcount_t instByteOffset = 7 - (i % 8);

            if (instAsciiPtr[i] != '1' && instAsciiPtr[i] != '0')
            {
                memset(instPtr, 0, instBytes);
                free(instPtr);
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }

            if (instAsciiPtr[i] == '1')
            {
                instPtr[instBytePos] += 0x01 << instByteOffset;
            }
        }

        bitmap.instanceCount = instCount;
        bitmap.instanceBitmap = instPtr;
        bitmap.instanceBytes = instBytes;

        rescount.attrCount = attcount;
        rescount.actCount = actcount;

        if (cis_addobject(g_context, oid, &bitmap, &rescount) == CIS_RET_OK)
            atCmdRespOK(pParam->engine);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);

        memset(instPtr, 0, instBytes);
        free(instPtr);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLDELOBJ=<ref>, <objectid><CR>
void AT_CmdFunc_MIPLDELOBJ(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pParam->param_count != 2)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        bool paramok = true;
        uint16_t objid = atParamUintInRange(pParam->params[1], 1, URI_MAX_ID, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (cis_delobject(g_context, objid) == CIS_RET_OK)
            atCmdRespOK(pParam->engine);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLNOTIFY=<ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, <valuetype>, <len>, <value>, <index>,<flag>[,ackid]<CR>
void AT_CmdFunc_MIPLNOTIFY(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;
    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 10 && pParam->param_count != 11)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        static char *savedValue = NULL;
        static uint32_t savedLength = 0;
        const char *valuestr = NULL;
        int64_t value = 0;
        double dvalue = 0;
        bool bvalue = false;
        bool isAckid = false;
        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint16_t objid = atParamUintInRange(pParam->params[2], 1, URI_MAX_ID, &paramok);
        uint16_t instid = atParamUintInRange(pParam->params[3], 0, URI_MAX_ID, &paramok);
        uint16_t resid = atParamUintInRange(pParam->params[4], 0, URI_MAX_ID, &paramok);
        cis_datatype_t valueType = atParamUintInRange(pParam->params[5], 1, 5, &paramok);
        uint16_t len = atParamUintInRange(pParam->params[6], valueType == cis_data_type_bool ? 1 : 0,
                                          valueType == cis_data_type_bool ? 1 : 65535, &paramok);
        cis_uri_t uri;
        cis_uri_make(objid, instid, resid, &uri);
        if (cis_check_object_instance_id(g_context, &uri) == CIS_RET_ERROR)
        {
            if (savedValue != NULL)
            {
                free(savedValue);
                savedValue = NULL;
                savedLength = 0;
            }
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        // integer optional len 2 4 8 byte
        // float optional len 4 8 byte
        if (!paramok || (valueType == cis_data_type_integer && (len != 2 && len != 4 && len != 8)) || (valueType == cis_data_type_float && (len != 4 && len != 8)))
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        uint32_t actual_length = pParam->params[7]->length;
        if (valueType == cis_data_type_string)
        {
            valuestr = atParamStr(pParam->params[7], &paramok);
            actual_length = strlen(valuestr);
        }
        else if (valueType == cis_data_type_float)
        {
            dvalue = atParamDouble(pParam->params[7], &paramok);
            if (paramok && len == 4 && ((dvalue < 0.0 - (double)FLT_MAX) || (dvalue > (double)FLT_MAX)))
                paramok = false;
        }
        else if (valueType == cis_data_type_opaque)
        {
            valuestr = atParamHexRawText(pParam->params[7], len, &paramok);
            if (paramok == false || valuestr == NULL)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            actual_length = strlen(valuestr);
        }
        else if (valueType == cis_data_type_bool)
        {
            bvalue = atParamUintInRange(pParam->params[7], 0, 1, &paramok);
        }
        else
        {
            if (len == 2)
                value = atParamIntInRange(pParam->params[7], INT16_MIN, INT16_MAX, &paramok);
            else if (len == 4)
                value = atParamInt(pParam->params[7], &paramok);
            else if (len == 8)
                value = atParamInt64(pParam->params[7], &paramok);
        }

        if (!paramok || ((actual_length != len) && ((valueType != cis_data_type_integer) && (valueType != cis_data_type_float))))
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        uint8_t index = atParamUintInRange(pParam->params[8], 0, 255, &paramok);
        if (paramok == false)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        uint8_t flag = atParamUintInRange(pParam->params[9], 0, 2, &paramok);
        if (paramok == false)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        if (pParam->param_count == 11)
        {
            isAckid = true;
        }

        if (valuestr != NULL)
        {
            if (flag == 1)
            {
                if (savedValue != NULL)
                {
                    free(savedValue);
                    savedValue = NULL;
                    savedLength = 0;
                }

                savedValue = malloc(len + 1);
                memset(savedValue, 0, len + 1);
                memcpy(savedValue, valuestr, len);
                savedLength = len;

                AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            }
            else if (savedValue != NULL)
            {
                char *tmp = (char *)malloc(savedLength + len + 1);
                memset(tmp, 0, savedLength + len + 1);
                memcpy(tmp, savedValue, savedLength);
                memcpy(tmp + savedLength, valuestr, len);
                free(savedValue);
                savedValue = tmp;
                savedLength += len;
                if (flag == 2)
                    AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            }
            else if (flag == 0)
            {
                savedValue = malloc(len + 1);
                memset(savedValue, 0, len + 1);
                memcpy(savedValue, valuestr, len);
                savedLength = len;
            }
            else
            {
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
            }
        }
        else
        {
            if (savedValue != NULL)
            {
                free(savedValue);
                savedValue = NULL;
                savedLength = 0;
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                goto clean;
            }
        }

        st_data_t tmpdata;
        if (valueType == cis_data_type_string)
            data_encode_string(savedValue, &tmpdata);
        else if (valueType == cis_data_type_float)
            data_encode_float(dvalue, &tmpdata);
        else if (valueType == cis_data_type_opaque)
            data_encode_opaque((uint8_t *)savedValue, savedLength, &tmpdata);
        else if (valueType == cis_data_type_bool)
            data_encode_bool(bvalue, &tmpdata);
        else
            data_encode_int(value, &tmpdata);
        cis_coapret_t result = CIS_NOTIFY_CONTENT;
        if (index != 0)
            result = CIS_NOTIFY_CONTINUE;

        osiSemaphoreRelease(pumpSema);

        if (cis_notify(g_context, &uri, (cis_data_t *)&tmpdata, msgid, result, isAckid) == CIS_RET_OK)
        {
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        }
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        if ((valueType == cis_data_type_opaque || valueType == cis_data_type_string) && tmpdata.asBuffer.buffer != NULL)
        {
            free(tmpdata.asBuffer.buffer);
        }
    clean:
        if (savedValue)
            free(savedValue);
        savedValue = NULL;
        savedLength = 0;
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLREADRSP=<ref>, <msgid>, <result>, <objectid>, <instanceid>, <resourceid>, <valuetype>, <len>, <value>, <index>, <flag><CR>
void AT_CmdFunc_MIPLREADRSP(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;
    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3 && pParam->param_count != 11)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));

        static char *savedValue = NULL;
        static uint32_t savedLength = 0;
        const char *valuestr = NULL;
        int64_t value = 0;
        double dvalue = 0;
        bool bvalue = false;
        cis_coapret_t cisresult = CIS_RESPONSE_READ;
        cis_ret_t ret = CIS_RET_OK;

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        if ((msgid | TYPE_READ) != g_ResponseMagicID)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        if (result != 1 && pParam->param_count > 3)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }
        else if (result == 1 && pParam->param_count == 3)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }
        else if (result == 11)
            cisresult = CIS_RESPONSE_BAD_REQUEST;
        else if (result == 12)
            cisresult = CIS_RESPONSE_UNAUTHORIZED;
        else if (result == 13)
            cisresult = CIS_RESPONSE_NOT_FOUND;
        else if (result == 14)
            cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
        else if (result == 15)
            cisresult = CIS_RESPONSE_NOT_ACCEPTABLE;
        else if (result != 1)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto clean;
        }

        g_ResponseMsgId = 0;

        if (cisresult == CIS_RESPONSE_READ)
        {
            uint16_t objid = atParamUintInRange(pParam->params[3], 1, URI_MAX_ID, &paramok);
            uint16_t instid = atParamUintInRange(pParam->params[4], 0, URI_MAX_ID, &paramok);
            uint16_t resid = atParamUintInRange(pParam->params[5], 0, URI_MAX_ID, &paramok);
            cis_datatype_t valueType = atParamUintInRange(pParam->params[6], 1, 5, &paramok);
            uint16_t len = atParamUintInRange(pParam->params[7], valueType == cis_data_type_bool ? 1 : 0,
                                              valueType == cis_data_type_bool ? 1 : 65535, &paramok);

            cis_uri_t uri;
            cis_uri_make(objid, instid, resid, &uri);
            if (cis_check_object_instance_id(g_context, &uri) == CIS_RET_ERROR)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            // integer optional len 2 4 8 byte
            // float optional len 4 8 byte
            if (!paramok || (valueType == cis_data_type_integer && (len != 2 && len != 4 && len != 8)) || (valueType == cis_data_type_float && (len != 4 && len != 8)))
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            uint32_t actual_length = pParam->params[8]->length;
            if (valueType == cis_data_type_string)
            {
                valuestr = atParamStr(pParam->params[8], &paramok);
                actual_length = strlen(valuestr);
            }
            else if (valueType == cis_data_type_float)
            {
                dvalue = atParamDouble(pParam->params[8], &paramok);
                if (paramok && len == 4 && ((dvalue < 0.0 - (double)FLT_MAX) || (dvalue > (double)FLT_MAX)))
                    paramok = false;
            }
            else if (valueType == cis_data_type_opaque)
            {
                valuestr = atParamHexRawText(pParam->params[8], len, &paramok);
                if (paramok == false || valuestr == NULL)
                {
                    atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    goto clean;
                }

                actual_length = strlen(valuestr);
            }
            else if (valueType == cis_data_type_bool)
                bvalue = atParamUintInRange(pParam->params[8], 0, 1, &paramok);
            else
            {
                if (len == 2)
                    value = atParamIntInRange(pParam->params[8], INT16_MIN, INT16_MAX, &paramok);
                else if (len == 4)
                    value = atParamInt(pParam->params[8], &paramok);
                else if (len == 8)
                    value = atParamInt64(pParam->params[8], &paramok);
            }

            if (!paramok || ((actual_length != len) && ((valueType != cis_data_type_integer) && (valueType != cis_data_type_float))))
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            uint32_t index = atParamUintInRange(pParam->params[9], 0, 255, &paramok);
            if (paramok == false)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            int flag = atParamUintInRange(pParam->params[10], 0, 2, &paramok);
            if (paramok == false)
            {
                atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto clean;
            }

            if (valueType == cis_data_type_string || valueType == cis_data_type_opaque)
            {
                if (valuestr != NULL)
                {
                    if (flag == 1)
                    {
                        if (savedValue != NULL)
                        {
                            free(savedValue);
                            savedValue = NULL;
                            savedLength = 0;
                        }

                        savedValue = malloc(len + 1);
                        memset(savedValue, 0, len + 1);
                        memcpy(savedValue, valuestr, len);
                        savedLength = len;
                        AT_CMD_RETURN(atCmdRespOK(pParam->engine));
                    }
                    else if (savedValue != NULL)
                    {
                        char *tmp = (char *)malloc(savedLength + len + 1);
                        memset(tmp, 0, savedLength + len + 1);
                        memcpy(tmp, savedValue, savedLength);
                        memcpy(tmp + savedLength, valuestr, len);
                        free(savedValue);
                        savedValue = tmp;
                        savedLength += len;
                        if (flag == 2)
                            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
                    }
                    else if (flag == 0)
                    {
                        savedValue = malloc(len + 1);
                        memset(savedValue, 0, len + 1);
                        memcpy(savedValue, valuestr, len);
                        savedLength = len;
                    }
                    else
                    {
                        AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                    }
                }
                else
                {
                    atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    goto clean;
                }
            }

            st_data_t tmpdata;
            if (valueType == cis_data_type_string)
                data_encode_string((const char *)savedValue, &tmpdata);
            else if (valueType == cis_data_type_float)
                data_encode_float(dvalue, &tmpdata);
            else if (valueType == cis_data_type_opaque)
                data_encode_opaque((uint8_t *)savedValue, savedLength, &tmpdata);
            else if (valueType == cis_data_type_bool)
                data_encode_bool(bvalue, &tmpdata);
            else
                data_encode_int(value, &tmpdata);

            if (index != 0)
                cisresult = CIS_RESPONSE_CONTINUE;
            else
                cisresult = CIS_RESPONSE_READ;

            ret = cis_response(g_context, &uri, (cis_data_t *)&tmpdata, msgid, cisresult);
            if ((valueType == cis_data_type_opaque || valueType == cis_data_type_string) && tmpdata.asBuffer.buffer != NULL)
            {
                free(tmpdata.asBuffer.buffer);
            }
        }
        else
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        osiSemaphoreRelease(pumpSema);
        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    clean:
        if (savedValue)
            free(savedValue);
        savedValue = NULL;
        savedLength = 0;
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLWRITERSP=<ref>, <msgid>, <result><CR>
void AT_CmdFunc_MIPLWRITERSP(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;
    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if ((msgid | TYPE_WRITE) != g_ResponseMagicID)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        cis_ret_t ret = CIS_RET_OK;
        cis_coapret_t cisresult = CIS_RESPONSE_WRITE;
        if (result == 2)
        {
            ret = cis_response(g_context, NULL, NULL, msgid, CIS_RESPONSE_WRITE);
        }
        else
        {
            if (result == 11)
                cisresult = CIS_RESPONSE_BAD_REQUEST;
            else if (result == 12)
                cisresult = CIS_RESPONSE_UNAUTHORIZED;
            else if (result == 13)
                cisresult = CIS_RESPONSE_NOT_FOUND;
            else if (result == 14)
                cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        }

        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLEXECUTERSP=<ref>, <msgid>, <result><CR>
void AT_CmdFunc_MIPLEXECUTERSP(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if ((msgid | TYPE_EXECUTE) != g_ResponseMagicID)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        g_ResponseMsgId = 0;
        cis_ret_t ret = CIS_RET_OK;
        cis_coapret_t cisresult = CIS_RESPONSE_EXECUTE;
        if (result == 2)
        {
            ret = cis_response(g_context, NULL, NULL, msgid, CIS_RESPONSE_EXECUTE);
        }
        else
        {
            if (result == 11)
                cisresult = CIS_RESPONSE_BAD_REQUEST;
            else if (result == 12)
                cisresult = CIS_RESPONSE_UNAUTHORIZED;
            else if (result == 13)
                cisresult = CIS_RESPONSE_NOT_FOUND;
            else if (result == 14)
                cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        }

        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLOBSERVERSP=<ref>, <msgid>, <result><CR>
void AT_CmdFunc_MIPLOBSERVERSP(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if ((msgid | TYPE_OBSERVER) != g_ResponseMagicID)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        g_ResponseMsgId = 0;
        cis_ret_t ret = CIS_RET_OK;
        cis_coapret_t cisresult = CIS_RESPONSE_OBSERVE;
        if (result == 1)
        {
            ret = cis_response(g_context, NULL, NULL, msgid, CIS_RESPONSE_OBSERVE);
        }
        else
        {
            if (result == 11)
                cisresult = CIS_RESPONSE_BAD_REQUEST;
            else if (result == 12)
                cisresult = CIS_RESPONSE_UNAUTHORIZED;
            else if (result == 13)
                cisresult = CIS_RESPONSE_NOT_FOUND;
            else if (result == 14)
                cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
            else if (result == 15)
                cisresult = CIS_RESPONSE_NOT_ACCEPTABLE;
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        }

        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

static bool prv_isdigit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    else
        return false;
}
int cis_atoi(const char *s)
{
    int i = 0;
    int j = 0;
    int length = strlen(s);
    OSI_LOGI(0, "cis_atoi :strlen(s)=%d\n", strlen(s));
    while (j < length)
    {
        if (!prv_isdigit((char)*s))
        {
            OSI_LOGI(0, "cis_atoi error:not digit\n");
            return -1;
        }
        i = i * 10 + (*s++) - '0';
        j++;
    }
    OSI_LOGI(0, "cis_atoi total :i=%d\n", i);
    return i;
}

//AT+MIPLDISCOVERRSP=<ref>,<msgid>,<result>,<length>,<valuestring><CR>
void AT_CmdFunc_MIPLDISCOVERRSP(atCommand_t *pParam)
{
    unsigned char ucRef = 0;
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 5)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        char *valuestr = (char *)atParamStr(pParam->params[4], &paramok);
        uint32_t actual_length = strlen(valuestr);
        uint32_t length = atParamUint(pParam->params[3], &paramok);
        if (!paramok || actual_length != length)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if ((msgid | TYPE_DISCOVER) != g_ResponseMagicID)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        //check the resource count
        st_object_t *obj = NULL;
        if (g_context != NULL && g_objectid != URI_INVALID)
        {
            obj = (st_object_t *)cis_findObject((void *)g_context, g_objectid);
            if (obj == NULL)
            {
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
            }
            else
            {
                uint32_t res_count = obj->attributeCount + obj->actionCount;
                uint32_t res_index = 0;
                char *token;
                static char *lasts;
                char *delim = ";";
                char *checkstr = malloc(actual_length + 1);
                if (checkstr == NULL)
                    AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                memset(checkstr, 0, actual_length + 1);
                strcpy(checkstr, valuestr);
                for (token = strtok_r(checkstr, delim, &lasts); token != NULL; token = strtok_r(NULL, delim, &lasts), res_index++)
                    ;
                OSI_LOGI(0, "cissys_logwrite:# mipldiscover res_count:%d, res_index:%d", res_count, res_index);
                free(checkstr);
                if (res_index != res_count)
                    AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }
        else
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        g_ResponseMsgId = 0;
        cis_ret_t ret = CIS_RET_OK;
        cis_coapret_t cisresult = CIS_RESPONSE_DISCOVER;
        if (result == 1)
        {
            char *token;
            static char *lasts;
            char *delim = ";";
            for (token = strtok_r(valuestr, delim, &lasts); token != NULL; token = strtok_r(NULL, delim, &lasts))
            {
                int id = cis_atoi(token);
                if (id < 0)
                    AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                uint16_t resId = (uint16_t)id;
                cis_uri_t uri;
                uri.objectId = URI_INVALID;
                uri.instanceId = URI_INVALID;
                uri.resourceId = resId;
                cis_uri_update(&uri);
                ret = cis_response(g_context, &uri, NULL, msgid, CIS_RESPONSE_CONTINUE);
            }
            if (ret == CIS_RET_OK)
                ret = cis_response(g_context, NULL, NULL, msgid, CIS_RESPONSE_DISCOVER);
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        else
        {
            if (result == 11)
                cisresult = CIS_RESPONSE_BAD_REQUEST;
            else if (result == 12)
                cisresult = CIS_RESPONSE_UNAUTHORIZED;
            else if (result == 13)
                cisresult = CIS_RESPONSE_NOT_FOUND;
            else if (result == 14)
                cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
            else if (result == 15)
                cisresult = CIS_RESPONSE_NOT_ACCEPTABLE;
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        }
        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLPARAMETERRSP=<ref>, <msgid>, <result><CR>
void AT_CmdFunc_MIPLPARAMETERRSP(atCommand_t *pParam)
{
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        unsigned char ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint32_t msgid = atParamUint(pParam->params[1], &paramok);
        uint8_t result = atParamUintInRange(pParam->params[2], 0, 100, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if ((msgid | TYPE_PARAM) != g_ResponseMagicID)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        cis_ret_t ret = CIS_RET_OK;
        cis_coapret_t cisresult = CIS_RESPONSE_OBSERVE_PARAMS;
        if (result == 2)
        {
            ret = cis_response(g_context, NULL, NULL, msgid, CIS_RESPONSE_OBSERVE_PARAMS);
        }
        else
        {
            if (result == 11)
                cisresult = CIS_RESPONSE_BAD_REQUEST;
            else if (result == 12)
                cisresult = CIS_RESPONSE_UNAUTHORIZED;
            else if (result == 13)
                cisresult = CIS_RESPONSE_NOT_FOUND;
            else if (result == 14)
                cisresult = CIS_RESPONSE_METHOD_NOT_ALLOWED;
            else
                AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            ret = cis_response(g_context, NULL, NULL, msgid, cisresult);
        }

        if (ret == CIS_RET_OK)
            atCmdSetTimeoutHandler(pParam->engine, 60 * 1000, cis_CommandNotResponse);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLUPDATE=<ref>,<lifetime>,<withObjectFlag><CR>
void AT_CmdFunc_MIPLUPDATE(atCommand_t *pParam)
{
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 3)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        unsigned char ucRef = atParamUint(pParam->params[0], &paramok);
        if (true != paramok || 0 != ucRef)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        uint32_t lifetime = atParamUint(pParam->params[1], &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint8_t updateobj = atParamUintInRange(pParam->params[2], 0, 1, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (!(lifetime == 0 || (lifetime >= LIFETIME_LIMIT_MIN && lifetime <= LIFETIME_LIMIT_MAX)))
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (pumpSema == NULL)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        osiSemaphoreRelease(pumpSema);

        if (cis_update_reg(g_context, lifetime, updateobj) == CIS_RET_OK)
            atCmdRespOK(pParam->engine);
        else
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

//AT+MIPLVER? <CR>
void AT_CmdFunc_MIPLVER(atCommand_t *pParam)
{
    if (pParam->type == AT_CMD_READ)
    {
        cis_version(&ver);

        char aucBuffer[32] = {0};
        sprintf(aucBuffer, "+MIPLVER: %u.%u.%u", ver.major, ver.minor, ver.micro);
        atCmdRespInfoText(pParam->engine, aucBuffer);
        atCmdRespOK(pParam->engine);
    }
    else
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
}

void AT_CmdFunc_MIPLCLEARSTATE(atCommand_t *pParam)
{
    bool paramok = true;

    if (pParam->type == AT_CMD_SET)
    {
        if (gEngine != NULL && gEngine != pParam->engine)
        {
            atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        if (pParam->param_count != 1)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        uint8_t fwFlag = atParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (cis_clearFwState(g_context, fwFlag) != CIS_RET_OK)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        else
        {
            atCmdRespOK(pParam->engine);
        }
    }
    else
    {
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

#endif
