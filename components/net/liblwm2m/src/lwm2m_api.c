#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "lwm2m_api.h"
#include "lwm2mclient.h"
#include "liblwm2m.h"
#include "commandline.h"
#include "expat.h"
#include "internals.h"

lwm2m_config_t *lwm2m_configs[MAX_REF_NUM];
// static uint8_t gVersion = 0;

static void lwm2m_clientd(void *param);

static bool isRefValid(uint8_t ref)
{
    //return ref >= 0 && ref < MAX_REF_NUM && lwm2m_configs[ref] != NULL;
    return ref < MAX_REF_NUM && lwm2m_configs[ref] != NULL; // for converity cid 39612
}

static bool isRefRegistered(uint8_t ref)
{
    return isRefValid(ref) && lwm2m_configs[ref]->isregisted;
}

#define EVID_CASE(nEvId) \
    case nEvId:          \
        return ((uint8_t *)#nEvId)

uint8_t *lwm2m_eventName(uint8_t nEvId)
{
    switch (nEvId)
    {
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_START);
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_SUCCESS);
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_FAILED);
        EVID_CASE(LWM2M_EVENT_CONNECT_SUCCESS);
        EVID_CASE(LWM2M_EVENT_CONNECT_FAILED);
        EVID_CASE(LWM2M_EVENT_REG_SUCCESS);
        EVID_CASE(LWM2M_EVENT_REG_FAILED);
        EVID_CASE(LWM2M_EVENT_REG_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_LIFETIME_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_STATUS_HALT);
        EVID_CASE(LWM2M_EVENT_UPDATE_SUCCESS);
        EVID_CASE(LWM2M_EVENT_UPDATE_FAILED);
        EVID_CASE(LWM2M_EVENT_UPDATE_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_RESPONSE_FAILED);
        EVID_CASE(LWM2M_EVENT_NOTIFY_FAILED);
    default:
        fprintf(stderr, "Unknown Event name:%d", nEvId);
        return (uint8_t *)"Unknown Event name";
    }
}

int lwm2m_is_pending_regcmd(uint8_t ref)
{
    if (!isRefValid(ref))
        return LWM2M_RET_ERROR;
    return lwm2m_configs[ref]->pending_regcmd;
}

void lwm2mPostEvent(uint8_t ref, uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2)
{
    osiEvent_t *ev = malloc(sizeof(osiEvent_t));
    osiEvent_t *lwm2m_ev = malloc(sizeof(osiEvent_t));
    lwm2m_ev->param1 = nResult;
    lwm2m_ev->param2 = nParam1;
    lwm2m_ev->param3 = nParam2;

    ev->id = nEventId;
    ev->param1 = (uint32_t)lwm2m_configs[ref]->userparam;
    ev->param2 = ref;
    ev->param3 = (uint32_t)lwm2m_ev;
    osiThreadCallback((osiThread_t *)lwm2m_configs[ref]->event_handle, (osiCallback_t)lwm2m_configs[ref]->callback, ev);
    int ret = -1;
    if (nEventId == EVETN_IND && lwm2m_configs[ref]->pending_regcmd)
    {
        if (nResult == LWM2M_EVENT_REG_FAILED || nResult == LWM2M_EVENT_STATUS_HALT || nResult == LWM2M_EVENT_REG_TIMEOUT)
            ret = 0;
        else if (nResult == LWM2M_EVENT_REG_SUCCESS)
            ret = 1;
    }
    if (ret != -1)
    {
        lwm2m_configs[ref]->pending_regcmd = 0;
        lwm2mPostEvent(ref, SERVER_REGCMD_RESULT_IND, ret, 0, 0);
    }
}

typedef struct
{
    uint8_t item2;
    uint8_t ishostkey;
    uint8_t isuserdatakey;
    uint8_t host[50];
    uint8_t cmdline[100];
} att_value_t;

static void elementStart(void *data, const char *el, const char **attr)
{
    att_value_t *value = (att_value_t *)data;
    fprintf(stderr, "el =%s,%s,%s", el, attr[0], attr[1]);
    if (el != NULL && attr[0] != NULL && attr[1] != NULL)
    {
        if (strcmp(el, "item") == 0 && strcmp(attr[0], "id") == 0 && strcmp(attr[1], "2") == 0)
            value->item2 = 2;
        else if (value->item2 && strcmp(el, "data") == 0 && strcmp(attr[0], "name") == 0)
        {
            if (strcmp(attr[1], "Host") == 0)
                value->ishostkey = 1;
            else if (strcmp(attr[1], "Userdata") == 0)
                value->isuserdatakey = 1;
        }
    }
}

static void attValues(void *data, const XML_Char *s, int len)
{
    att_value_t *value = (att_value_t *)data;
    if (value->ishostkey && len < 50)
    {
        memcpy(value->host, s, len);
        value->ishostkey = 0;
        value->item2--;
        fprintf(stderr, "host =%s", value->host);
    }
    else if (value->isuserdatakey && len < 100)
    {
        memcpy(value->cmdline, s, len);
        value->isuserdatakey = 0;
        value->item2--;
        fprintf(stderr, "cmdline =%s", value->cmdline);
    }
}

int8_t lwm2m_new_config_xml(void *file_config, uint16_t size)
{
    if (netif_default == NULL)
        return -1;
    XML_Parser parser = XML_ParserCreate(NULL);
    if (parser == NULL)
    {
        return -1;
    }
    char *port = NULL;
    uint8_t cmdline[256] = {0};
    att_value_t value = {0};
    XML_SetUserData(parser, &value);
    /* Tell expat to use functions start() and end() each times it encounters
     * the start or end of an element. */
    XML_SetStartElementHandler(parser, elementStart);
    XML_SetCharacterDataHandler(parser, attValues);
    if (XML_Parse(parser, file_config, size, XML_TRUE) == XML_STATUS_ERROR)
    {
        return -1;
    }
    XML_ParserFree(parser);
    if (strlen((char *)value.host) != 0)
    {
        port = strstr((char *)value.host, ":");
        if (port != NULL)
        {
            value.host[port - (char *)value.host] = 0;
            port++;
        }
        fprintf(stderr, "host =%s,port=%s", value.host, port);
    }
    if (strlen((char *)value.cmdline) != 0)
    {
        fprintf(stderr, "cmdline =%s", value.cmdline);
    }
    if (port)
        snprintf((char *)cmdline, 255, "%s -h %s -p %s", value.cmdline, value.host, port);
    else
        snprintf((char *)cmdline, 255, "%s -h %s", value.cmdline, value.host);
    return lwm2m_new_config(cmdline);
}

int8_t lwm2m_new_config(const uint8_t *cmdline)
{
    uint8_t ref = 0xff;
    for (int i = 0; i < MAX_REF_NUM; i++)
    {
        lwm2m_config_t *lwm2m_config = lwm2m_configs[i];
        if (lwm2m_config != NULL)
        {
            if (strcmp((const char *)cmdline, (const char *)lwm2m_config->cmdline) == 0)
            {
                if (lwm2m_config->isregisted)
                    return -1;
                else
                    ref = i;
                break;
            }
        }
        else if (ref == 0xff)
            ref = i;
    }
    if (ref == 0xff)
        return -1;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    if (lwm2m_config == NULL)
    {
        uint8_t **argv = lwm2m_malloc(20 * 4);
        uint8_t argc = 1;
        char delim[] = " ";
        char *token;
        static char *lasts;

        lwm2m_config = lwm2m_malloc(sizeof(lwm2m_config_t));
        strcpy((char *)lwm2m_config->cmdline, (const char *)cmdline);
        for (token = strtok_r((char *)cmdline, delim, &lasts); token != NULL; token = strtok_r(NULL, delim, &lasts))
        {
            argv[argc] = lwm2m_malloc(strlen(token) + 1);
            strcpy((char *)argv[argc], token);
            argc++;
        }
        lwm2m_config->observer = false;
        lwm2m_config->argv = argv;
        lwm2m_config->argc = argc;
        lwm2m_config->isregisted = 0;
    }
    else
    {
        if (lwm2m_config->isregisted)
            return -1;
    }
    lwm2m_configs[ref] = lwm2m_config;
    return ref;
}

lwm2m_ret_t lwm2m_get_observer(uint8_t ref)
{
    if (!isRefValid(ref))
        return LWM2M_RET_ERROR;

    return lwm2m_configs[ref]->observer == true ? LWM2M_RET_OK : LWM2M_RET_ERROR;
}

lwm2m_ret_t lwm2m_set_observer(uint8_t ref, bool observer)
{
    if (!isRefValid(ref))
        return LWM2M_RET_ERROR;

    lwm2m_configs[ref]->observer = observer;
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_free_task(uint8_t ref)
{
    if (lwm2m_configs[ref])
    {
        if (((void *)(lwm2m_configs[ref]->this)) != NULL)
        {
            close(lwm2m_configs[ref]->ipc_socket);
            close(lwm2m_configs[ref]->ipc_data_socket);
            //COS_StopTask((TASK_HANDLE *)lwm2m_configs[ref]->this);
            //COS_DeleteTask(lwm2m_configs[ref]->this);
        }
    }
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_free_config(uint8_t ref)
{
    if (!isRefValid(ref) || isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    if (lwm2m_configs[ref])
    {
        obj_arry_t *objects = lwm2m_configs[ref]->objects;
        int i = 0;
        for (; i < MAX_INIT_OBJECTS; i++)
        {
            if (objects[i].objId != 0)
            {
                lwm2m_free(objects[i].bitmap);
            }
        }

        for (i = 1; i < lwm2m_configs[ref]->argc; i++)
        {
            lwm2m_free(lwm2m_configs[ref]->argv[i]);
            lwm2m_configs[ref]->argv[i] = NULL;
        }
        lwm2m_free(lwm2m_configs[ref]->argv);
        memset(lwm2m_configs[ref]->cmdline, 0, 256);
        lwm2m_free(lwm2m_configs[ref]);
        lwm2m_configs[ref] = NULL;
    }
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2mcfg_set_handler(uint8_t ref, void *taskHandle, osiCallback_t callback, void *userparam)
{
    if (!isRefValid(ref) || isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    lwm2m_config->event_handle = (uint32_t)taskHandle;
    lwm2m_config->callback = callback;
    lwm2m_config->userparam = userparam;

    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2mcfg_set_handler_ext(uint8_t ref, void *taskHandle, osiCallback_t callback, void *userparam, uint8_t nDLCI)
{
    if (!isRefValid(ref) || isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    if (lwm2m_config == NULL)
    {
        return LWM2M_RET_ERROR;
    }

    lwm2m_config->event_handle = (uint32_t)taskHandle;
    lwm2m_config->callback = callback;
    lwm2m_config->userparam = userparam;
    lwm2m_config->nDLCI = nDLCI;

    return LWM2M_RET_OK;
}

lwm2m_config_t *lwm2m_get_config(uint8_t configIndex)
{
    return lwm2m_configs[configIndex];
}

uint8_t lwm2m_get_ipcSocket(uint8_t configIndex)
{
    return lwm2m_configs[configIndex]->ipc_socket;
}

uint8_t lwm2m_get_ipcDataSocket(uint8_t configIndex)
{
    return lwm2m_configs[configIndex]->ipc_data_socket;
}

void lwm2m_parse_buffer(char *buffer, void *value, uint8_t count, void *delim)
{
    char *token;
    int i = 0;
    static char *lasts;
    fprintf(stdout, "lwm2m_parse_buffer buffer:%s,count=%d", buffer, count);
    if (delim == NULL)
        delim = " ";
    for (token = strtok_r((char *)buffer, delim, &lasts); token != NULL && i < count; token = strtok_r(NULL, delim, &lasts))
    {
        ((int *)value)[i++] = (int)token;
    }
}

lwm2m_ret_t lwm2m_excute_cmd(uint8_t *data, uint32_t data_len, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    if (NULL == lwm2m_config)
    {
        return LWM2M_RET_ERROR;
    }

    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = 1234 + ref;
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    sendto(lwm2m_config->ipc_socket, data, data_len, 0, (const struct sockaddr *)&to4, sizeof(to4));
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_excute_data_cmd(uint8_t *data, uint32_t data_len, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    if (NULL == lwm2m_config || netif_default == NULL)
    {
        return LWM2M_RET_ERROR;
    }

    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = 1235 + ref;
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    sendto(lwm2m_config->ipc_data_socket, data, data_len, 0, (const struct sockaddr *)&to4, sizeof(to4));
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_is_registed(uint8_t ref)
{
    return isRefRegistered(ref);
}

obj_arry_t *lwm2m_get_init_objs(uint8_t ref)
{
    if (!isRefValid(ref))
        return NULL;
    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    return lwm2m_config->objects;
}

lwm2m_ret_t lwm2m_add_obj(uint16_t objId, uint32_t instCount, uint8_t *bitmap, uint16_t resCount, uint8_t ref)
{
    if (!isRefValid(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    obj_arry_t *objects = lwm2m_config->objects;
    int newIndex = -1;
    int i = 0;

    for (; i < MAX_INIT_OBJECTS; i++)
    {
        if (objects[i].objId == objId)
        {
            return LWM2M_RET_ERROR;
        }
        else if (objects[i].objId == 0 && newIndex == -1)
        {
            newIndex = i;
        }
    }

    if (newIndex != -1 && newIndex < MAX_INIT_OBJECTS)
    {
        objects[newIndex].objId = objId;
        objects[newIndex].bitmap = (uint8_t *)(lwm2m_strdup((char *)bitmap));
        objects[newIndex].instCount = instCount;
        objects[newIndex].resCount = resCount;
    }
    else
    {
        return LWM2M_RET_ERROR;
    }

    if (isRefRegistered(ref))
    {
        uint8_t cmd[550] = {0};
        if (instCount > 500)
            return LWM2M_RET_ERROR;
        sprintf((char *)cmd, "add %d %d %s %d", objId, (unsigned int)instCount, bitmap, resCount);
        return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
    }
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_del_obj(uint16_t objId, uint8_t ref)
{
    if (!isRefValid(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    obj_arry_t *objects = lwm2m_config->objects;
    int i = 0;
    bool isObjExist = false;
    for (; i < MAX_INIT_OBJECTS; i++)
    {
        if (objects[i].objId == objId)
        {
            objects[i].objId = 0;
            objects[i].resCount = 0;
            lwm2m_free(objects[i].bitmap);
            objects[i].bitmap = NULL;
            objects[i].instCount = 0;
            isObjExist = true;

            ipso_obj_t *obj_type_table = lwm2m_config->obj_type_table;
            int j = 0;
            for (; j < MAX_DYNAMIC_OBJECTS; j++)
            {
                if (obj_type_table[i].objId == objId)
                {
                    memset(&obj_type_table[i], 0, sizeof(ipso_obj_t));
                    break;
                }
            }
        }
    }

    if (!isObjExist)
    {
        return LWM2M_RET_ERROR;
    }

    if (isRefRegistered(ref))
    {
        uint8_t cmd[10] = {0};
        sprintf((char *)cmd, "rm %d", objId);
        return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
    }
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_read_rsp(uint16_t objId, uint16_t instId, uint16_t resId, lwm2m_tpye_t type,
                           const uint8_t *value, uint32_t value_length, bool isEnd, uint8_t result, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[50] = {0};
    lwm2m_ret_t ret;
    snprintf((char *)cmd, 50, "read %d %d %d %d %d %d %d", objId, instId, resId, isEnd, result, type, (unsigned int)value_length);
    uint8_t *data = lwm2m_malloc(strlen((char *)cmd) + 1 + value_length + 1);
    memset(data, 0, strlen((char *)cmd) + 1 + value_length + 1);
    memcpy(data, cmd, strlen((char *)cmd));
    memcpy(data + strlen((char *)cmd) + 1, value, value_length);
    ret = lwm2m_excute_cmd(data, strlen((char *)cmd) + 1 + value_length + 1, ref);
    lwm2m_free(data);
    return ret;
}

lwm2m_ret_t lwm2m_write_rsp(bool result, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[20] = {0};
    sprintf((char *)cmd, "write %d", result);
    return lwm2m_excute_data_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_exec_rsp(bool result, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[20] = {0};
    sprintf((char *)cmd, "execute %d", result);
    return lwm2m_excute_data_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_discover_rsp(uint8_t result, const uint8_t *value, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    if (strlen((char *)value) > 200)
        return LWM2M_RET_ERROR;

    uint8_t cmd[256] = {0};
    snprintf((char *)cmd, 255, "discover %d %s", result, value);
    return lwm2m_excute_data_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_start_fota_download(const uint8_t *uri, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    if (strlen((char *)uri) > 200)
        return LWM2M_RET_ERROR;

    uint8_t cmd[256] = {0};
    snprintf((char *)cmd, 255, "download_fota %s", uri);
    LOG_ARG("lwm2m_start_fota_download %s", uri);
    return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_notify(uint16_t objId, uint16_t instId, uint16_t resId, const uint8_t *value,
                         uint32_t value_length, lwm2m_tpye_t type, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[50] = {0};
    lwm2m_ret_t ret;

    if (value == NULL)
        value_length = 0;

    snprintf((char *)cmd, 50, "change /%d/%d/%d %d %d", objId, instId, resId, type, (unsigned int)value_length);

    uint8_t *data = lwm2m_malloc(strlen((char *)cmd) + 1 + value_length + 1);
    memset(data, 0, strlen((char *)cmd) + 1 + value_length + 1);
    memcpy(data, cmd, strlen((char *)cmd));
    memcpy(data + strlen((char *)cmd) + 1, value, value_length);
    ret = lwm2m_excute_cmd(data, strlen((char *)cmd) + 1 + value_length + 1, ref);
    lwm2m_free(data);
    return ret;
}

lwm2m_ret_t lwm2m_notify_fota_state(uint32_t state, uint32_t resulte, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[50] = {0};
    lwm2m_ret_t ret;
    snprintf((char *)cmd, 50, "notify_fota %ld %ld", state, resulte);
    ret = lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
    return ret;
}

lwm2m_ret_t lwm2m_update(uint32_t lifetime, bool withObject, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    uint8_t cmd[50] = {0};
    sprintf((char *)cmd, "update %d %d", (unsigned int)lifetime, withObject);
    return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_set_parameter(uint16_t objId, uint16_t instId, uint16_t resId,
                                uint8_t *parameter, uint8_t ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    if (strlen((char *)parameter) > 200)
        return LWM2M_RET_ERROR;

    uint8_t cmd[256] = {0};
    snprintf((char *)cmd, 255, "param %d %d %d %s", objId, instId, resId, parameter);
    return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
}

lwm2m_ret_t lwm2m_register(int ref, uint32_t lifetime, uint32_t timeout)
{
    if (!isRefValid(ref) || isRefRegistered(ref))
        return LWM2M_RET_ERROR;

    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    lwm2m_config->pending_regcmd = 1;
    lwm2m_config->lifetime = lifetime;
    lwm2m_config->timeout = timeout;
    //zhangyi del for portint 20180710
    //lwm2m_config->this = COS_CreateTask(lwm2m_clientd,(void *)ref,NULL,2048,231,0,0,"lwm2m_clientd");
    lwm2m_config->this = (uint32_t)osiThreadCreate("lwm2m_clientd", lwm2m_clientd, (void *)ref, OSI_PRIORITY_NORMAL, 2048 * 4, 32);

    return LWM2M_RET_OK;
}
lwm2m_ret_t QuitHandshake();

lwm2m_ret_t lwm2m_unregister(int ref)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    lwm2m_configs[ref]->isquit = 1;
    QuitHandshake();
    return lwm2m_excute_cmd((uint8_t *)"quit", 4, ref);
}

lwm2m_ret_t lwm2m_set_dynamic_ipso(int ref)
{
    if (!isRefValid(ref) || isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    lwm2m_config->use_dynamic_ipso = 1;
    return LWM2M_RET_OK;
}

lwm2m_ret_t lwm2m_set_send_flag(int ref, int flag)
{
    if (!isRefValid(ref) || !isRefRegistered(ref))
        return LWM2M_RET_ERROR;
    uint8_t cmd[15] = {0};
    sprintf((char *)cmd, "setflag %d", flag);
    return lwm2m_excute_cmd(cmd, strlen((char *)cmd), ref);
}

bool lwm2m_is_dynamic_ipso(int ref)
{
    if (!isRefValid(ref))
        return false;
    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    return lwm2m_config->use_dynamic_ipso;
}

#define MALLOC_BLK 16
static void lwm2m_clientd(void *param)
{
    uint8_t ref = (uint8_t)(uint32_t)param;
    lwm2m_config_t *lwm2m_config = lwm2m_configs[ref];
    int ret = -1;
    if (lwm2m_config == NULL || netif_default == NULL)
        goto EXIT;

    lwm2m_config->ipc_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lwm2m_config->ipc_socket < 0)
        goto EXIT;
    struct sockaddr_in to4 = {0};
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = 1234 + ref;
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    if (bind(lwm2m_config->ipc_socket, (const struct sockaddr *)&to4, sizeof(to4)) < 0)
    {
        close(lwm2m_config->ipc_socket);
        lwm2m_config->ipc_socket = -1;
        goto EXIT;
    }

    lwm2m_config->ipc_data_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lwm2m_config->ipc_data_socket < 0)
        goto EXIT;
    to4.sin_len = sizeof(to4);
    to4.sin_family = AF_INET;
    to4.sin_port = 1235 + ref;
    to4.sin_addr.s_addr = PP_HTONL(INADDR_LOOPBACK);
    if (bind(lwm2m_config->ipc_data_socket, (const struct sockaddr *)&to4, sizeof(to4)) < 0)
    {
        close(lwm2m_config->ipc_socket);
        lwm2m_config->ipc_socket = -1;
        close(lwm2m_config->ipc_data_socket);
        lwm2m_config->ipc_data_socket = -1;
        goto EXIT;
    }
    int begin = lwm2m_config->argc;

    lwm2m_config->argv[0] = ((uint8_t *)"lwm2mclient_main");
    lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
    strcpy((char *)lwm2m_config->argv[lwm2m_config->argc++], "-f");
    lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
    snprintf(((char *)(lwm2m_config->argv[lwm2m_config->argc++])), MALLOC_BLK, "%d", ref);
    lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
    strcpy((char *)lwm2m_config->argv[lwm2m_config->argc++], "-k");
    lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
    snprintf((char *)lwm2m_config->argv[lwm2m_config->argc++], MALLOC_BLK, "%d", (unsigned int)lwm2m_config->ipc_socket);

    if (lwm2m_config->lifetime > 0)
    {
        lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
        strcpy((char *)lwm2m_config->argv[lwm2m_config->argc++], "-t");
        lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
        snprintf((char *)lwm2m_config->argv[lwm2m_config->argc++], MALLOC_BLK, "%d", (unsigned int)lwm2m_config->lifetime);
    }

    if (lwm2m_config->timeout > 0)
    {
        lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
        strcpy((char *)lwm2m_config->argv[lwm2m_config->argc++], "-o");
        lwm2m_config->argv[lwm2m_config->argc] = lwm2m_malloc(MALLOC_BLK);
        snprintf((char *)lwm2m_config->argv[lwm2m_config->argc++], MALLOC_BLK, "%d", (unsigned int)lwm2m_config->timeout);
    }
    lwm2m_config->isregisted = 1;
    ret = lwm2mclient_main(lwm2m_config->argc, (char **)lwm2m_config->argv);
    lwm2m_config->isregisted = 0;

    for (int i = begin; i < lwm2m_config->argc; i++)
    {
        lwm2m_free(lwm2m_config->argv[i]);
        lwm2m_config->argv[i] = NULL;
    }
    lwm2m_config->argc = begin;

    if (lwm2m_is_dynamic_ipso(ref))
    {
        ipso_obj_t *obj_type_table = lwm2m_config->obj_type_table;
        memset(obj_type_table, 0, sizeof(ipso_obj_t) * MAX_DYNAMIC_OBJECTS);
    }

EXIT:
    lwm2mPostEvent(ref, SERVER_QUIT_IND, ret, 0, 0);
    osiThreadExit();
}
