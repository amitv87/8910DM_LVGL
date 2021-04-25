#include "coap_async_api.h"
#include "async_worker.h"
#include "lwipopts.h"
#include "osi_log.h"
#include "osi_api.h"

struct coap_client_t
{
    coap_status_t status;
    bool autoFree;
    uint8_t *cmdline;
    uint8_t *payload;
    uint32_t payload_len;
    void *user_task;
    coap_user_cb_t user_cb;
    void *user_data;
    coap_incoming_data_cb_t data_cb;
};

#define CMDLINE_NUM (16)
static AWORKER_RC coapAworkerRequestHandler(AWORKER_REQ *req)
{
    AWORKER_RC rc = AWRC_FAIL;
    bool paramok = true;
    int nResult = -1;

    OSI_LOGXI(OSI_LOGPAR_SII, 0, "func:%s line:%d event:%d", __func__, __LINE__, (int)req->event);
    coap_client_t *client = (coap_client_t *)req->param;
    char **argv = NULL;
    uint8_t argc = 0;
    bool has_payload = false;
    char delim[] = " ";
    char *token;
    static char *lasts;

    const uint8_t *cmdline = client->cmdline;
    argv = malloc(CMDLINE_NUM * 4);
    memset(argv, 0, CMDLINE_NUM * 4);
    argv[argc++] = "coap_client";
    for (token = strtok_r((char *)cmdline, delim, &lasts); token != NULL; token = strtok_r(NULL, delim, &lasts))
    {
        argv[argc] = malloc(strlen(token) + 1);
        strcpy(argv[argc], token);
        argc++;

        if (argc >= CMDLINE_NUM)
        {
            rc = AWRC_FAIL;
            goto error;
        }
    }

    has_payload = aworker_param_getu32(req, 0, &paramok);
    if (has_payload && (client->payload == NULL))
    {
        rc = AWRC_FAIL;
        goto error;
    }
    if (client->payload)
    {
        coap_client_set_payload(NULL, client->payload_len, 1);
        coap_client_set_payload(client->payload, client->payload_len, 1);
    }
    if (client->data_cb)
        coap_client_set_datahander(client->data_cb, client);

    client->status = COAP_STATUS_PROCEING;
    nResult = coap_client_main(argc, argv);

error:
    for (int i = 1; i < argc; i++)
    {
        if (NULL != argv[i])
        {
            free(argv[i]);
            argv[i] = NULL;
        }
    }
    if (NULL != argv)
    {
        free(argv);
        argv = NULL;
    }

    if (nResult != -1)
    {
        rc = AWRC_SUCCESS;
    }
    return rc;
}

static void coapAworkerCallback(int result, uint32_t event, void *param)
{
    //OSI_LOGXI(OSI_LOGPAR_SII, 0x100075c4, "func:%s line:%d event:%d", __func__, __LINE__, (int)1);
    coap_client_t *client = (coap_client_t *)param;
    OSI_LOGI(0, "coapAworkerCallback result:%d event:%d\n", (int)result, (int)event);
    client->status = COAP_STATUS_DONE;
    if (client->user_cb != NULL)
        client->user_cb(client, (coap_method_t)event, result == AWRC_SUCCESS);
}

static AWORKER_REQ *createAworkerReq(uint32_t event, void *param, uint32_t bufflen, void *handle)
{
    return aworker_create_request((void *)handle,
                                  coapAworkerRequestHandler, coapAworkerCallback, event, param, bufflen);
}

coap_client_t *coap_client_new(void *user_task, coap_user_cb_t user_cb, void *user_data)
{
    coap_client_t *client = malloc(sizeof(coap_client_t));

    if (client != NULL)
    {
        memset(client, 0, sizeof(coap_client_t));
        client->status = COAP_STATUS_INIT;
        client->user_task = user_task;
        client->user_cb = user_cb;
        client->user_data = user_data;
    }
    return client;
}

bool coap_client_free(coap_client_t *client)
{
    if ((NULL != client) && (client->status != COAP_STATUS_READY))
    {
        free(client->cmdline);
        free(client->payload);
        client->cmdline = NULL;
        client->payload = NULL;
        client->status = COAP_STATUS_NONE;
        free(client);
        return true;
    }
    return false;
}

void *coap_client_getUserData(coap_client_t *client)
{
    if (NULL != client)
    {
        return client->user_data;
    }

    return NULL;
}

coap_status_t coap_client_getStatus(coap_client_t *client)
{
    if (NULL != client)
    {
        return client->status;
    }

    return COAP_STATUS_NONE;
}

bool coap_client_cleanParams(coap_client_t *client)
{
    if ((NULL != client) && (client->status != COAP_STATUS_READY))
    {
        free(client->cmdline);
        free(client->payload);
        client->cmdline = NULL;
        client->payload = NULL;
        client->status = COAP_STATUS_INIT;
        return true;
    }
    return false;
}

bool coap_client_setParams(coap_client_t *client, uint8_t *cmdline_param)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        uint8_t *cmdline = NULL;
        if (cmdline_param == NULL)
            return true;
        unsigned int cmdline_length = strlen((const char *)cmdline_param);
        if (client->cmdline != NULL)
        {
            cmdline_length += strlen((const char *)client->cmdline);
        }
        cmdline = malloc(cmdline_length + 2);
        memset(cmdline, 0, cmdline_length + 2);
        if (client->cmdline != NULL)
            sprintf((char *)cmdline, "%s %s", client->cmdline, cmdline_param);
        else
            strcpy((char *)cmdline, (const char *)cmdline_param);
        if (client->cmdline)
            free(client->cmdline);
        client->cmdline = cmdline;
        OSI_LOGXI(OSI_LOGPAR_SISS, 0x100075c6, "func:%s line:%d setParams param:%s,cmdline:%s", __func__, __LINE__, cmdline_param, client->cmdline);
        return true;
    }
    return false;
}

bool coap_client_setIndataCbk(coap_client_t *client, coap_incoming_data_cb_t data_cb)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        client->data_cb = data_cb;
        return true;
    }
    return false;
}

bool coap_client_setPayload(coap_client_t *client, uint8_t *data, uint32_t data_len)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT || client->status == COAP_STATUS_READY))
    {
        if (NULL != client->payload)
        {
            free(client->payload);
        }
        client->payload = malloc(data_len + 1);
        client->payload_len = data_len;
        memcpy(client->payload, data, data_len);
        client->payload[data_len] = 0;
        OSI_LOGXI(OSI_LOGPAR_S, 0x100075c7, "coap_client_setPayload setPayload=%s", client->payload);

        return true;
    }
    return false;
}

char *coap_client_getPayload(coap_client_t *client)
{
    if ((NULL != client) && (NULL != client->payload))
    {
        return (char *)client->payload;
    }

    return NULL;
}

bool coap_async_get(coap_client_t *client, uint8_t *url, uint8_t *cmdline)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        bool paramok = true;
        uint8_t getCmd[] = "-m get";
        //coap_client_setParams(client, cmdline);
        coap_client_setParams(client, getCmd);
        coap_client_setParams(client, url);
        client->status = COAP_STATUS_READY;
        AWORKER_REQ *areq = createAworkerReq(COAP_GET, client, 0, client->user_task);
        if (areq == NULL)
        {
            OSI_LOGI(0, "coap_async_get fail, create coap work queue");
            paramok = false;
        }
        aworker_param_putu32(areq, 0, &paramok);
        aworker_post_req_delay(areq, 0, &paramok);
        return paramok;
    }
    return false;
}

bool coap_async_put(coap_client_t *client, uint8_t *url, uint8_t *cmdline, bool has_payload)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        bool paramok = true;
        uint8_t putCmd[] = "-m put";
        //coap_client_setParams(client, cmdline);
        coap_client_setParams(client, putCmd);
        coap_client_setParams(client, url);
        client->status = COAP_STATUS_READY;
        AWORKER_REQ *areq = createAworkerReq(COAP_PUT, client, 0, client->user_task);
        if (areq == NULL)
        {
            OSI_LOGI(0, "coap_async_put fail, create coap work queue");
            paramok = false;
        }
        aworker_param_putu32(areq, has_payload, &paramok);
        aworker_post_req_delay(areq, 0, &paramok);
        return paramok;
    }
    return false;
}

bool coap_async_post(coap_client_t *client, uint8_t *url, uint8_t *cmdline, bool has_payload)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        bool paramok = true;
        uint8_t postCmd[] = "-m post";
        //coap_client_setParams(client, cmdline);
        coap_client_setParams(client, postCmd);
        coap_client_setParams(client, url);
        client->status = COAP_STATUS_READY;
        AWORKER_REQ *areq = createAworkerReq(COAP_POST, client, 0, client->user_task);
        if (areq == NULL)
        {
            OSI_LOGI(0, "coap_async_post fail, create coap work queue");
            paramok = false;
        }
        aworker_param_putu32(areq, has_payload, &paramok);
        aworker_post_req_delay(areq, 0, &paramok);
        return paramok;
    }
    return false;
}

bool coap_async_delete(coap_client_t *client, uint8_t *url, uint8_t *cmdline)
{
    if ((NULL != client) && (client->status == COAP_STATUS_INIT))
    {
        bool paramok = true;
        uint8_t delCmd[] = "-m delete";
        //coap_client_setParams(client, cmdline);
        coap_client_setParams(client, delCmd);
        coap_client_setParams(client, url);
        client->status = COAP_STATUS_READY;
        AWORKER_REQ *areq = createAworkerReq(COAP_DELETE, client, 0, client->user_task);
        if (areq == NULL)
        {
            OSI_LOGI(0, "coap_async_delete fail, create coap work queue");
            paramok = false;
        }
        aworker_post_req_delay(areq, 0, &paramok);
        return paramok;
    }
    return false;
}
