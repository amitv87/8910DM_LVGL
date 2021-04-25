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

#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "osi_log.h"
#include "atr_config.h"
#include "stdio.h"

#ifdef CONFIG_AT_DM_LWM2M_SUPPORT

#include <cis_api.h>
#include <cis_internals.h>
#include <cis_list.h>

#include "at_cmd_dm_lwm2m.h"

#define DM_CIS_URI_EQUAL(oriUri, desUri) (((oriUri)->flag == (desUri)->flag) && ((oriUri)->objectId == (desUri)->objectId) && ((oriUri)->instanceId == (desUri)->instanceId) && ((oriUri)->resourceId == (desUri)->resourceId))

typedef enum
{
    DM_EVETN_IND = 0,
    DM_SAMPLE_CALLBACK_READ,
    DM_SAMPLE_CALLBACK_WRITE,
    DM_SAMPLE_CALLBACK_EXECUTE,
    DM_SAMPLE_CALLBACK_OBSERVE,
    DM_SAMPLE_CALLBACK_SETPARAMS,
    DM_SAMPLE_CALLBACK_DISCOVER,
} at_dm_callback_type_t;

struct dm_st_callback_info
{
    struct dm_st_callback_info *next;
    cis_listid_t mid;
    at_dm_callback_type_t flag;
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

struct dm_st_observe_info
{
    struct dm_st_observe_info *next;
    cis_listid_t mid;
    cis_uri_t uri;
    cis_observe_attr_t params;
};

osiSemaphore_t *g_dm_pumpSema;

static void *g_dm_context = NULL;

static bool g_dm_shutdown = false;

osiThread_t *g_dm_hThread;

//struct atCmdEngine *g_dm_engine = NULL;

static char dm_adapter_config_hex[] = {
    0x13, 0x00, 0x5f,
    0xf1, 0x00, 0x03,
    0xf2, 0x00, 0x48, 0x04, 0x00, 0x11, 0x00, 0x00, 0x05, 0x43,
    0x4d, 0x49, 0x4f, 0x54, 0x00, 0x04, 0x73, 0x64, 0x6b, 0x55, 0x00, 0x04, 0x73, 0x64, 0x6b, 0x50,
    0x00, 0x10, 0x31, 0x31, 0x37, 0x2e, 0x31, 0x36, 0x31, 0x2e, 0x32, 0x2e, 0x37, 0x3a, 0x35, 0x36, 0x38, 0x33, /*length 16 117.161.2.7*/
    0x00, 0x1a, 0x41, 0x75, 0x74, 0x68, 0x43, 0x6f, 0x64, 0x65, 0x3a, 0x3b, 0x55, 0x73,
    0x65, 0x72, 0x64, 0x61, 0x74, 0x61, 0x3a, 0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0x3b,
    0xf3, 0x00,
    0x11, 0xe4, 0x00, 0xc8, 0x00, 0x09, 0x73, 0x79, 0x73, 0x55, 0x44, 0x3a, 0x31, 0x32, 0x33};

static char dm_commercial_config_hex[] = {
    0x13, 0x00, 0x60,
    0xf1, 0x00, 0x03,
    0xf2, 0x00, 0x49, 0x04, 0x00, 0x11, 0x00, 0x00, 0x05, 0x43,
    0x4d, 0x49, 0x4f, 0x54, 0x00, 0x04, 0x73, 0x64, 0x6b, 0x55, 0x00, 0x04, 0x73, 0x64, 0x6b, 0x50,
    0x00, 0x11, 0x31, 0x31, 0x37, 0x2e, 0x31, 0x36, 0x31, 0x2e, 0x32, 0x2e, 0x34, 0x31, 0x3a, 0x35, 0x36, 0x38, 0x33, /*length 17 117.161.2.41*/
    0x00, 0x1a, 0x41, 0x75, 0x74, 0x68, 0x43, 0x6f, 0x64, 0x65, 0x3a, 0x3b, 0x55, 0x73,
    0x65, 0x72, 0x64, 0x61, 0x74, 0x61, 0x3a, 0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0x3b,
    0xf3, 0x00,
    0x11, 0xe4, 0x00, 0xc8, 0x00, 0x09, 0x73, 0x79, 0x73, 0x55, 0x44, 0x3a, 0x31, 0x32, 0x33};

static bool isAdapterPlatform = true;

static void dm_event_ind_process(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    st_context_t *context = (st_context_t *)pEvent->param1;
    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)pEvent->param2;

    switch (pEvent->id)
    {
    //+MIPLEVENT: <ref>, <evtid><CR>
    case DM_EVETN_IND:
    {
        char refstr[50] = {0};
        sprintf(refstr, "%s", STR_EVENT_CODE(pEvent->param2));
        atCmdRespDefUrcText(refstr);
    }
    break;
    //+MIPLREAD: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, [<count>] <CR>
    case DM_SAMPLE_CALLBACK_READ:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        st_object_t *obj = cis_findObject(context, uri.objectId);
        uint32_t res_count = 0;
        if (obj != NULL)
            res_count = obj->attributeCount + obj->actionCount;
        if (1 /*uri.instanceId >= 0 && uri.resourceId > 0*/)
        {
            sprintf(refstr, "+MIPLREAD:%d,%ld,%d,%d,%d", 0, mid, (uint16_t)uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1,
                    CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1);
        }
        else
        {
            sprintf(refstr, "+MIPLREAD:%d,%ld,%d,%d,%d,%ld", 0, mid, (uint16_t)uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1,
                    CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1, res_count);
        }
        atCmdRespDefUrcText(refstr);
        free(newNode);
    }
    break;
    //+MIPLWRITE: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, <valuetype>, <len>, <value>, <index><CR>
    case DM_SAMPLE_CALLBACK_WRITE:
    {
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        cis_data_t *value = newNode->param.asWrite.value;
        int16_t count = (int16_t)newNode->param.asWrite.count;
        while (--count >= 0)
        {
            char *refstr = NULL;
            refstr = malloc(value[count].asBuffer.length * 2 + 100);
            if (NULL == refstr)
            {
                memset(value[count].asBuffer.buffer, 0, value[count].asBuffer.length);
                free(value[count].asBuffer.buffer);
                value[count].asBuffer.buffer = NULL;
                break;
            }
            memset(refstr, 0, value[count].asBuffer.length * 2 + 100);

            char *hexStr = NULL;
            hexStr = malloc(value[count].asBuffer.length * 2 + 2);
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
                snprintf(hexStr + 2 * i, sizeof(hexStr), "%02X", value[count].asBuffer.buffer[i]);
            }

            sprintf(refstr, "+MIPLWRITE:%d,%ld,%d,%d,%d,%d,%ld,%s,0,%d", 0, mid,
                    uri.objectId, uri.instanceId, value[count].id, value[count].type,
                    value[count].asBuffer.length, hexStr, count);

            atCmdRespDefUrcText(refstr);
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
    case DM_SAMPLE_CALLBACK_EXECUTE:
    {
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        uint8_t *buffer = newNode->param.asExec.buffer;
        uint32_t length = newNode->param.asExec.length;

        char *refstr = NULL;
        refstr = malloc(length + 100);
        if (refstr == NULL)
        {
            memset(newNode->param.asExec.buffer, 0, newNode->param.asExec.length);
            free(newNode->param.asExec.buffer);
            free(newNode);
            break;
        }

        sprintf(refstr, "+MIPLEXECUTE:%d,%ld,%d,%d,%d,%ld,\"%s\"", 0, mid, uri.objectId, uri.instanceId, uri.resourceId, length, buffer);
        atCmdRespDefUrcText(refstr);
        memset(newNode->param.asExec.buffer, 0, newNode->param.asExec.length);
        free(newNode->param.asExec.buffer);
        memset(refstr, 0, length + 100);
        free(refstr);
        free(newNode);
    }
    break;
    //+MIPLOBSERVE: <ref>, <msgid>, <flag>,<objectid>, [<instanceid>], [<resourceid>] <CR>
    case DM_SAMPLE_CALLBACK_OBSERVE:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        sprintf(refstr, "+MIPLOBSERVE:%d,%ld,%d,%d,%d,%d", 0, mid, newNode->param.asObserve.flag,
                uri.objectId, CIS_URI_IS_SET_INSTANCE(&uri) ? uri.instanceId : -1, CIS_URI_IS_SET_RESOURCE(&uri) ? uri.resourceId : -1);
        atCmdRespDefUrcText(refstr);
        free(newNode);
    }
    break;
    //+MIPLPARAMETER: <ref>, <msgid>, <objectid>, <instanceid>, <resourceid>, <parameter>, <len><CR>
    case DM_SAMPLE_CALLBACK_SETPARAMS:
    {
        char refstr[200] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        cis_observe_attr_t parameters = newNode->param.asObserveParam.params;
        char tmp[100] = {0};
        sprintf(tmp, "pmin=%ld; pmax=%ld; gt=%f; lt=%f; stp=%f", parameters.minPeriod,
                parameters.maxPeriod, parameters.greaterThan, parameters.lessThan, parameters.step);
        sprintf(refstr, "+MIPLPARAMETER:%d,%ld,%d,%d,%d,%d,%s", 0, mid, uri.objectId, uri.instanceId, uri.resourceId,
                strlen(tmp), tmp);
        atCmdRespDefUrcText(refstr);
        free(newNode);
    }
    break;
    //+MIPLDISCOVER:<ref>, <msgid>, <objectid><CR>
    case DM_SAMPLE_CALLBACK_DISCOVER:
    {
        char refstr[100] = {0};
        cis_uri_t uri = newNode->uri;
        cis_mid_t mid = newNode->mid;
        sprintf(refstr, "+MIPLDISCOVER:%d,%ld,%d", 0, mid, uri.objectId);
        atCmdRespDefUrcText(refstr);
        free(newNode);
    }
    break;
    default:
        break;
    }
    free(pEvent);
}

static void dm_PostEvent(void *context, at_dm_callback_type_t event_id, void *param)
{
    osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
    ev->id = event_id;
    ev->param1 = (uint32_t)context;
    ev->param2 = (uint32_t)param;
    osiThreadCallback(atEngineGetThreadId(), (osiCallback_t)dm_event_ind_process, ev);
}

static cis_data_t *dm_dataDup(const cis_data_t *value, cis_attrcount_t attrcount)
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

//////////////////////////////////////////////////////////////////////////
cis_coapret_t dm_onRead(void *context, cis_uri_t *uri, cis_mid_t mid)
{
    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_READ;
    newNode->mid = mid;
    newNode->uri = *uri;

    OSI_LOGI(0, "dm_onRead:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);

    dm_PostEvent(context, DM_SAMPLE_CALLBACK_READ, newNode);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t dm_onDiscover(void *context, cis_uri_t *uri, cis_mid_t mid)
{

    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_DISCOVER;
    newNode->mid = mid;
    newNode->uri = *uri;

    OSI_LOGI(0, "dm_onDiscover:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);

    dm_PostEvent(context, DM_SAMPLE_CALLBACK_DISCOVER, newNode);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t dm_onWrite(void *context, cis_uri_t *uri, const cis_data_t *value, cis_attrcount_t attrcount, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0, "dm_onWrite:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }
    else
    {
        OSI_LOGI(0, "dm_onWrite:(%d/%d)\n", uri->objectId, uri->instanceId);
    }

    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_WRITE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asWrite.count = attrcount;
    newNode->param.asWrite.value = dm_dataDup(value, attrcount);

    dm_PostEvent(context, DM_SAMPLE_CALLBACK_WRITE, newNode);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t dm_onExec(void *context, cis_uri_t *uri, const uint8_t *value, uint32_t length, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0, "dm_onExec:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }
    else
    {
        return CIS_CALLBACK_METHOD_NOT_ALLOWED;
    }

    if (!CIS_URI_IS_SET_INSTANCE(uri))
    {
        return CIS_CALLBACK_BAD_REQUEST;
    }

    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_EXECUTE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asExec.buffer = (uint8_t *)malloc(length + 1);
    memset(newNode->param.asExec.buffer, 0, length + 1);
    newNode->param.asExec.length = length;
    memcpy(newNode->param.asExec.buffer, value, length);

    dm_PostEvent(context, DM_SAMPLE_CALLBACK_EXECUTE, newNode);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t dm_onObserve(void *context, cis_uri_t *uri, bool flag, cis_mid_t mid)
{
    OSI_LOGI(0, "dm_onObserve mid:%d uri:(%d/%d/%d)\n", mid, uri->objectId, uri->instanceId, uri->resourceId);

    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_OBSERVE;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asObserve.flag = flag;

    dm_PostEvent(context, DM_SAMPLE_CALLBACK_OBSERVE, newNode);
    return CIS_CALLBACK_CONFORM;
}

cis_coapret_t dm_onParams(void *context, cis_uri_t *uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
    if (CIS_URI_IS_SET_RESOURCE(uri))
    {
        OSI_LOGI(0, "cis_on_params:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
    }

    if (!CIS_URI_IS_SET_INSTANCE(uri))
    {
        return CIS_CALLBACK_BAD_REQUEST;
    }

    struct dm_st_callback_info *newNode = (struct dm_st_callback_info *)malloc(sizeof(struct dm_st_callback_info));
    newNode->next = NULL;
    newNode->flag = DM_SAMPLE_CALLBACK_SETPARAMS;
    newNode->mid = mid;
    newNode->uri = *uri;
    newNode->param.asObserveParam.params = parameters;
    dm_PostEvent(context, DM_SAMPLE_CALLBACK_SETPARAMS, newNode);
    return CIS_CALLBACK_CONFORM;
}

void dm_onEvent(void *context, cis_evt_t eid, void *param)
{
    OSI_LOGI(0, "dm_on_event(%d):%s\n", eid, STR_EVENT_CODE(eid));
    switch (eid)
    {
    case CIS_EVENT_RESPONSE_SUCCESS:
    {
        OSI_LOGI(0, "dm_on_event response ok mid:%d\n", (int32_t)param);
        //atCmdRespOK(g_dm_engine);
    }
    break;
    case CIS_EVENT_RESPONSE_FAILED:
    {
        OSI_LOGE(0, "dm_on_event response failed mid:%d\n", (int32_t)param);
        //atCmdRespCmeError(g_dm_engine, ERR_AT_CME_EXE_FAIL);
    }
    break;
    case CIS_EVENT_NOTIFY_SUCCESS:
    {
        OSI_LOGI(0, "dm_on_event notify ok mid:%d\n", (int32_t)param);
        //atCmdRespOK(g_dm_engine);
    }
    break;
    case CIS_EVENT_NOTIFY_FAILED:
    {
        OSI_LOGE(0, "dm_on_event notify failed mid:%d\n", (int32_t)param);
        //atCmdRespCmeError(g_dm_engine, ERR_AT_CME_EXE_FAIL);
    }
    break;
    case CIS_EVENT_UPDATE_NEED:
        OSI_LOGI(0, "dm_on_event need to update,reserve time:%ds\n", (int32_t)param);
        cis_update_reg(g_dm_context, LIFETIME_INVALID, false);
        break;
    default:
        break;
    }
    dm_PostEvent(context, DM_EVETN_IND, (void *)eid);
}

static void dm_clientd(void *param)
{
    while (!g_dm_shutdown)
    {
        /*pump function*/
        cis_pump(g_dm_context);
        osiThreadSleep(100);
    }
    osiThreadExit();
}

void dm_register()
{
    OSI_LOGI(0, "dm_register ");
    if (!(gAtSetting.self_register & 0X07))
    {
        OSI_LOGE(0, "dm_register DM doesn't register gAtSetting.self_register = %d", gAtSetting.self_register);
        return;
    }
    OSI_LOGI(0, "dm_register gAtSetting.self_register = %d", gAtSetting.self_register);

    Options dm_config = {0};
    memcpy(dm_config.szCMEI_IMEI, (char *)"CMEI_IMEI", strlen((char *)"CMEI_IMEI"));
    memcpy(dm_config.szIMSI, (char *)"IMSI", strlen((char *)"IMSI"));
    memcpy(dm_config.szDMv, (char *)"v2.0", strlen((char *)"v3.0"));
    memcpy(dm_config.szAppKey, (char *)"M100000058", strlen((char *)"M100000058"));
    memcpy(dm_config.szPwd, (char *)"s2n7TgPkg3IT2oo51T630F3f6U5XJ6vj", strlen((char *)"s2n7TgPkg3IT2oo51T630F3f6U5XJ6vj"));

    if (isAdapterPlatform)
    {
        if (cis_init(&g_dm_context, dm_adapter_config_hex, sizeof(dm_adapter_config_hex), &dm_config) != CIS_RET_OK)
        {
            OSI_LOGE(0, "cis entry init failed.\n");
            return;
        }
    }
    else
    {
        if (cis_init(&g_dm_context, dm_commercial_config_hex, sizeof(dm_commercial_config_hex), &dm_config) != CIS_RET_OK)
        {
            OSI_LOGE(0, "cis entry init failed.\n");
            return;
        }
    }

    g_dm_shutdown = false;

    g_dm_pumpSema = osiSemaphoreCreate(1, 0);
    g_dm_hThread = osiThreadCreate("dm_lwm2m",
                                   dm_clientd,
                                   NULL,
                                   232,
                                   2048 * 8,
                                   32);

    cis_callback_t callback;
    callback.onRead = dm_onRead;
    callback.onWrite = dm_onWrite;
    callback.onExec = dm_onExec;
    callback.onObserve = dm_onObserve;
    callback.onSetParams = dm_onParams;
    callback.onEvent = dm_onEvent;
    callback.onDiscover = dm_onDiscover;

    osiSemaphoreRelease(g_dm_pumpSema);

    if (cis_register(g_dm_context, 60, &callback) == CIS_RET_OK)
    {
        OSI_LOGI(0, "register success.\n");
    }
    else
    {
        OSI_LOGE(0, "register fail.\n");

        osiSemaphoreDelete(g_dm_pumpSema);
        cis_deinit(&g_dm_context);
    }
}

void atCmdHandleSELFREGISTER(atCommand_t *cmd)
{
    OSI_LOGI(0, "atCmdHandleSELFREGISTER");
    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;

        uint8_t operator= atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count == 1)
        {
            char repStr[32] = {0};
            sprintf(repStr, "%s: %d", cmd->desc->name, (gAtSetting.self_register >> operator) & 0x01);
            atCmdRespInfoText(cmd->engine, repStr);
        }
        else
        {
            uint8_t flag = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            switch (operator)
            {
            case OPERATOR_CMCC:
                gAtSetting.self_register &= 0X0; //0x06
                gAtSetting.self_register |= flag;
                break;
            case OPERATOR_UNICOM:
                gAtSetting.self_register &= 0X0; //0x05
                gAtSetting.self_register |= (flag << 1);
                break;
            case OPERATOR_CTCC:
                gAtSetting.self_register &= 0X0; //0x03
                gAtSetting.self_register |= (flag << 2);
                break;
            default:
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        OSI_LOGI(0, "atCmdHandleSELFREGISTER gAtSetting.self_register =%d", gAtSetting.self_register);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char repStr[32] = {0};
        sprintf(repStr, "%s: (0-2),(0-1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, repStr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

void atCmdHandleSETDMADPPLATFORM(atCommand_t *cmd)
{
    OSI_LOGI(0, "atCmdHandleSELFREGISTER");
    if (AT_CMD_SET == cmd->type)
    {
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;

        bool flag = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        isAdapterPlatform = flag;
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char repStr[32] = {0};
        sprintf(repStr, "+SETDMADPPLATFORM: %d", isAdapterPlatform);
        atCmdRespInfoText(cmd->engine, repStr);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char repStr[32] = {0};
        sprintf(repStr, "+SETDMADPPLATFORM: (0-1)");
        atCmdRespInfoText(cmd->engine, repStr);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmeError(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#else
void dm_register()
{
}
#endif
