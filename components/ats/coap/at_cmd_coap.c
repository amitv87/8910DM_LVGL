#include "ats_config.h"

#ifdef CONFIG_AT_COAP_SUPPORT

#include "stdio.h"
#include "cfw.h"
#include "at_cmd_coap.h"
#include "at_cfg.h"
#include "sockets.h"
#include "unistd.h"
#include "coap_async_api.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"

COAP_DATA_T at_coap_data;
coap_client_t *at_coap_client;
static bool g_bCoapDoing = false;
static osiMutex_t *g_pstMutex = NULL;

#define DATA_LEN_INV (0)
#define DATA_LEN_MIN (1)
#define DATA_LEN_MAX (65535)
#define DATA_TIMEOUT_MIN (1)
#define DATA_TIMEOUT_DEF (90)
#define DATA_TIMEOUT_MAX (120)
#define DATA_MANUL_LEN_MAX (1024)
#define AT_URL_LENG_MAX (60)
#define AT_CMDLINE_LENG_MAX (128)

typedef struct
{
    uint32_t uMuxIndex;
    uint32_t pos;
    uint32_t size;
    uint8_t *data;
    uint32_t timeout;
} coapAsyncCtx_t;

static void coapProcessReturnData(coap_client_t *client, const unsigned char *data, size_t len, int num)
{
    OSI_LOGXI(OSI_LOGPAR_IIS, 0x10003f06, "incoming_data(%d:%d): %s", num, len, data);
    if (NULL != at_coap_data.gBuff[num])
        return;
    at_coap_data.gLength = at_coap_data.gLength + len;
    at_coap_data.gBuff[num] = malloc(len + 1);
    memset(at_coap_data.gBuff[num], 0, len + 1);
    memcpy(at_coap_data.gBuff[num], data, len);
    at_coap_data.gNum = num;
    at_coap_data.gLen[num] = len;

    return;
}

static void coapParseResult(atCmdEngine_t *engine)
{
    char OutStr[COAP_MESSAGE_MAX_SIZE] = {0};
    bool result = true;

    for (int i = 0; i <= at_coap_data.gNum; i++)
    {
        if (0 == strncmp("error ", (const char *)at_coap_data.gBuff[i], 6))
        {
            result = false;
        }
    }
    if (result)
    {
        OutStr[0] = '\0';
        sprintf(OutStr, "+COAP(%d):", at_coap_data.gLength);
        atCmdRespInfoText(engine, OutStr);
    }

    for (int i = 0; i <= at_coap_data.gNum; i++)
    {
        char *data = (char *)at_coap_data.gBuff[i];
        OSI_LOGI(0, data);
        atCmdRespInfoNText(engine, data, strlen(data));
    }

    return;
}

static void coap_data_free()
{
    for (int i = 0; i <= at_coap_data.gNum; i++)
    {
        free(at_coap_data.gBuff[i]);
        at_coap_data.gBuff[i] = NULL;
        at_coap_data.gLen[i] = 0;
    }
    at_coap_data.gLength = 0;
    at_coap_data.gNum = 0;

    return;
}

static void coapUserCallback(coap_client_t *client, coap_method_t method, bool result)
{
    OSI_LOGI(0x10003f07, "coapUserCallback result:%d event:%d\n", result, method);
    atCmdEngine_t *engine = (atCmdEngine_t *)coap_client_getUserData(client);
    switch (method)
    {
    case COAP_GET:
    case COAP_PUT:
    case COAP_POST:
    case COAP_DELETE:
        if (result > 0 && at_coap_data.gLength > 0)
            coapParseResult(engine);
        break;

    default:
        OSI_LOGI(0x10003f08, "at_coap_worker_callback unhandled event:%d\n", method);
        break;
    }
    if (result > 0)
    {
        atCmdRespOK(engine);
    }
    else
    {
        atCmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL); //bugfix for 988974 bu SUN Wei
    }

    coap_data_free();
    coap_client_free(client);
    at_coap_client = NULL;

    osiMutexLock(g_pstMutex);
    g_bCoapDoing = false;
    osiMutexUnlock(g_pstMutex);

    return;
}

static coap_client_t *AT_getCoapClient(atCmdEngine_t *engine)
{
    if (NULL == at_coap_client)
    {
        at_coap_client = coap_client_new(osiThreadCurrent(), coapUserCallback, engine);
        return at_coap_client;
    }
    else if ((NULL != at_coap_client) && (engine == ((atCmdEngine_t *)coap_client_getUserData(at_coap_client))))
    {
        return at_coap_client;
    }
    else
    {
        return NULL;
    }
}

static void coapAsyncCtxDestroy(atCommand_t *cmd)
{
    coapAsyncCtx_t *pstAsync = (coapAsyncCtx_t *)cmd->async_ctx;
    if (NULL == pstAsync)
    {
        return;
    }
    if (NULL != pstAsync->data)
    {
        free(pstAsync->data);
    }
    free(pstAsync);
    cmd->async_ctx = NULL;
    return;
}

static void _coapDataAutoSendTimeoutCB(atCommand_t *param)
{
    //char *cOutStr = NULL;
    coap_client_t *client = NULL;
    atCommand_t *cmd = (atCommand_t *)param;
    char aucBuffer[40] = {0};

    coapAsyncCtx_t *pstAsync = (coapAsyncCtx_t *)cmd->async_ctx;
    if (NULL == pstAsync)
    {
        goto Exit;
    }

    if (0 == pstAsync->pos)
    {
        snprintf(aucBuffer, sizeof(aucBuffer), "input data timeout");
        atCmdSetLineMode(cmd->engine);
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        AT_CMD_RETURN(atCmdRespOKText(cmd->engine, aucBuffer));
    }

    client = AT_getCoapClient(cmd->engine);
    if (NULL != client)
    {
        coap_client_setPayload(client, pstAsync->data, pstAsync->size);
    }
    atCmdSetLineMode(cmd->engine);

    //cOutStr = coap_client_getPayload(client);
    //atCmdRespInfoNText(cmd->engine, ">>>>>>>>>>", strlen(">>>>>>>>>>"));
    //atCmdRespInfoNText(cmd->engine, cOutStr, strlen(cOutStr));
    //atCmdRespInfoNText(cmd->engine, "<<<<<<<<<<", strlen("<<<<<<<<<<"));
    //cOutStr = NULL;
Exit:
    osiMutexLock(g_pstMutex);
    g_bCoapDoing = false;
    osiMutexUnlock(g_pstMutex);
    AT_CMD_RETURN(atCmdRespOK(cmd->engine));
}

static int _coapDataBypassCB(void *param, const void *data, size_t size)
{
    //char * cOutStr = NULL;
    coap_client_t *client = NULL;
    atCommand_t *cmd = (atCommand_t *)param;
    coapAsyncCtx_t *pstAsync = (coapAsyncCtx_t *)cmd->async_ctx;

    OSI_LOGI(0, "AT CIPSEND size/%d pos/%d data_size/%d", pstAsync->size, pstAsync->pos, size);

    size_t len = pstAsync->size - pstAsync->pos;
    if (len > size)
    {
        len = size;
    }
    memcpy(pstAsync->data + pstAsync->pos, data, len);
    pstAsync->pos += len;

    atChannelSetting_t *chsetting = atCmdChannelSetting(cmd->engine);
    if (chsetting->ate)
    {
        atCmdWrite(cmd->engine, data, len);
    }
    if (pstAsync->pos >= pstAsync->size)
    {
        client = AT_getCoapClient(cmd->engine);
        if (NULL != client)
        {
            coap_client_setPayload(client, pstAsync->data, pstAsync->size);
        }
        //cOutStr = coap_client_getPayload(client);
        //atCmdRespInfoNText(cmd->engine, ">>>>>>>>>>", strlen(">>>>>>>>>>"));
        //atCmdRespInfoNText(cmd->engine, cOutStr, strlen(cOutStr));
        //atCmdRespInfoNText(cmd->engine, "<<<<<<<<<<", strlen("<<<<<<<<<<"));
        //cOutStr = NULL;
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        atCmdRespOK(cmd->engine);
    }
    return len;
}

void AT_COAP_CmdFunc_DATA(atCommand_t *pParam)
{
    char OutStr[32] = {0};
    unsigned int uiLength = DATA_LEN_INV;
    unsigned char ucTime = DATA_TIMEOUT_DEF;
    bool bParamOk = true;

    if (NULL == g_pstMutex)
    {
        g_pstMutex = osiMutexCreate();
    }

    osiMutexLock(g_pstMutex);
    if (true == g_bCoapDoing)
    {
        osiMutexUnlock(g_pstMutex);
        goto ExeFail;
    }
    else
    {
        g_bCoapDoing = true;
    }
    osiMutexUnlock(g_pstMutex);

    OSI_LOGI(0, "AT+COAPDATA: pParam->type = %d", pParam->type);
    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        switch (pParam->param_count)
        {
        case 1:
            uiLength = atParamUintInRange(pParam->params[0], DATA_LEN_MIN, DATA_LEN_MAX, &bParamOk);
            if (!bParamOk)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            ucTime = DATA_TIMEOUT_DEF;
            break;
        case 2:
            uiLength = atParamUintInRange(pParam->params[0], DATA_LEN_MIN, DATA_LEN_MAX, &bParamOk);
            if (!bParamOk)
            {

                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            ucTime = atParamUintInRange(pParam->params[1], 1, 120, &bParamOk);
            if (!bParamOk)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            //ucTime += 5;
            break;
        default:
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        coapAsyncCtx_t *pstAsync = (coapAsyncCtx_t *)malloc(sizeof(*pstAsync));
        if (NULL == pstAsync)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto NOMemory;
        }

        pParam->async_ctx = pstAsync;
        pParam->async_ctx_destroy = coapAsyncCtxDestroy;
        pstAsync->size = uiLength;
        pstAsync->pos = 0;
        pstAsync->data = (uint8_t *)malloc(uiLength);
        pstAsync->timeout = ucTime;
        if (NULL == pstAsync->data)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto NOMemory;
        }

        atCmdRespOutputPrompt(pParam->engine);
        atCmdSetTimeoutHandler(pParam->engine, ucTime * 1000, _coapDataAutoSendTimeoutCB);
        atCmdSetBypassMode(pParam->engine, _coapDataBypassCB, pParam);
        goto End;
        break;
    }
    case AT_CMD_TEST:
    {
        OutStr[0] = '\0';
        sprintf(OutStr, "+COAPDATA:<1-%d>[,[1-%d]]", DATA_LEN_MAX, DATA_TIMEOUT_MAX);
        //sprintf(OutStr, "+COAPDATA:[1-%d]", DATA_LEN_MAX);
        atCmdRespInfoText(pParam->engine, OutStr);
        atCmdRespOK(pParam->engine);
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        goto End;
        break;
    }
    default:
    {
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
    }

ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

NOMemory:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);

ExeFail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

End:
    return;
}

void AT_COAP_CmdFunc_GET(atCommand_t *pParam)
{
    if (NULL == g_pstMutex)
    {
        g_pstMutex = osiMutexCreate();
    }

    osiMutexLock(g_pstMutex);
    if (true == g_bCoapDoing)
    {
        osiMutexUnlock(g_pstMutex);
        goto ExeFail;
    }
    else
    {
        g_bCoapDoing = true;
    }
    osiMutexUnlock(g_pstMutex);

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint8_t def_seconds = DATA_TIMEOUT_DEF;

        if (pParam->param_count < 2 || pParam->param_count > 3) //bugfix 989024 by SUN Wei
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        const char *url = atParamStr(pParam->params[0], &paramok);
        if (strlen(url) > AT_URL_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
        const char *cmdline = atParamStr(pParam->params[1], &paramok);
        if (pParam->param_count > 2)
        {
            uint8_t wait_seconds = atParamIntInRange(pParam->params[2], DATA_TIMEOUT_MIN, DATA_TIMEOUT_MAX, &paramok);
            if (!paramok)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            if (wait_seconds != 0)
                def_seconds = wait_seconds;
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f0d, "AT_COAP_CmdFunc_GET cmdline =%s", cmdline);
        if (!paramok)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        uint8_t timeout_param[10];
        sprintf((char *)timeout_param, "-B %d", def_seconds);

        coap_client_t *client = AT_getCoapClient(pParam->engine);
        if (client != NULL)
        {
            coap_client_setParams(client, timeout_param);
            coap_client_setParams(client, (uint8_t *)cmdline);
            coap_client_setIndataCbk(client, (coap_incoming_data_cb_t)coapProcessReturnData);
            paramok = coap_async_get(client, (uint8_t *)url, (uint8_t *)cmdline);
        }
        else
            paramok = false;

        if (paramok)
        {
            AT_SetAsyncTimerMux(pParam->engine, def_seconds + 5);
            goto End;
        }
        else
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
    }
    else
    {
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        goto NotSupport;
    }

ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

ExeFail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

NotSupport:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);

End:
    return;
}

void AT_COAP_CmdFunc_PUT(atCommand_t *pParam)
{
    if (NULL == g_pstMutex)
    {
        g_pstMutex = osiMutexCreate();
    }

    osiMutexLock(g_pstMutex);
    if (true == g_bCoapDoing)
    {
        osiMutexUnlock(g_pstMutex);
        goto ExeFail;
    }
    else
    {
        g_bCoapDoing = true;
    }
    osiMutexUnlock(g_pstMutex);

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint8_t def_seconds = DATA_TIMEOUT_DEF;
        bool has_payload = true;

        if (pParam->param_count < 2 || pParam->param_count > 4) //bugfix 989062 by SUN Wei
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        const char *url = atParamStr(pParam->params[0], &paramok);
        if (strlen(url) > AT_URL_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
        const char *cmdline = atParamStr(pParam->params[1], &paramok);
        if (strlen(cmdline) > AT_CMDLINE_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        if (pParam->param_count > 2)
        {
            if (!atParamIsEmpty(pParam->params[2]))
            {
                uint8_t wait_seconds = atParamIntInRange(pParam->params[2], 1, DATA_TIMEOUT_MAX, &paramok);
                if (wait_seconds != 0)
                    def_seconds = wait_seconds;
            }
        }

        if (pParam->param_count > 3)
        {
            if (!atParamIsEmpty(pParam->params[3]))
                has_payload = ((atParamUintInRange(pParam->params[3], 0, 1, &paramok) == 1) ? true : false);
        }

        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f0e, "AT_COAP_CmdFunc_PUT cmdline =%s", cmdline);
        if (!paramok)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        uint8_t timeout_param[10];
        sprintf((char *)timeout_param, "-B %d", def_seconds);

        coap_client_t *client = AT_getCoapClient(pParam->engine);
        if (client != NULL)
        {
            coap_client_setParams(client, timeout_param);
            coap_client_setParams(client, (uint8_t *)cmdline);
            coap_client_setIndataCbk(client, (coap_incoming_data_cb_t)coapProcessReturnData);
            paramok = coap_async_put(client, (uint8_t *)url, (uint8_t *)cmdline, has_payload);
        }
        else
            paramok = false;

        if (paramok)
        {
            AT_SetAsyncTimerMux(pParam->engine, def_seconds + 5);
            goto End;
        }
        else
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
    }
    else
    {
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        goto NotSupport;
    }

ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

ExeFail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

NotSupport:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);

End:
    return;
}

void AT_COAP_CmdFunc_POST(atCommand_t *pParam)
{
    bool bParamOk = true;
    bool bPayloadFlag = true;
    unsigned char ucTimeout = DATA_TIMEOUT_DEF;
    char sTimeout[10] = {0};
    const char *pcUrl = NULL;
    const char *pcCmdline = NULL;
    coap_client_t *pstClient = NULL;

    if (NULL == g_pstMutex)
    {
        g_pstMutex = osiMutexCreate();
    }

    osiMutexLock(g_pstMutex);
    if (true == g_bCoapDoing)
    {
        osiMutexUnlock(g_pstMutex);
        goto ExeFail;
    }
    else
    {
        g_bCoapDoing = true;
    }
    osiMutexUnlock(g_pstMutex);

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        pcUrl = atParamStr(pParam->params[0], &bParamOk);
        if (strlen(pcUrl) > AT_URL_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
        pcCmdline = atParamStr(pParam->params[1], &bParamOk);
        if (strlen(pcCmdline) > AT_CMDLINE_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        switch (pParam->param_count)
        {
        case 2:
        {
            break;
        }
        case 3:
        {
            if (false == atParamIsEmpty(pParam->params[2]))
            {
                ucTimeout = atParamUintInRange(pParam->params[2], DATA_TIMEOUT_MIN, DATA_TIMEOUT_MAX, &bParamOk);
            }
            if (false == bParamOk)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            break;
        }
        case 4:
        {
            if (false == atParamIsEmpty(pParam->params[2]))
            {
                ucTimeout = atParamUintInRange(pParam->params[2], 1, DATA_TIMEOUT_MAX, &bParamOk);
            }
            if (false == bParamOk)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            if (false == atParamIsEmpty(pParam->params[3]))
            {
                bPayloadFlag = atParamUintInRange(pParam->params[3], 0, 1, &bParamOk);
            }
            if (false == bParamOk)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            break;
        }
        default:
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        sprintf(sTimeout, "-B %d", ucTimeout);

        pstClient = AT_getCoapClient(pParam->engine);
        if (NULL != pstClient)
        {
            coap_client_setParams(pstClient, (uint8_t *)sTimeout);
            coap_client_setParams(pstClient, (uint8_t *)pcCmdline);
            coap_client_setIndataCbk(pstClient, (coap_incoming_data_cb_t)coapProcessReturnData);
            bParamOk = coap_async_post(pstClient, (uint8_t *)pcUrl, (uint8_t *)pcCmdline, bPayloadFlag);
        }
        else
        {
            bParamOk = false;
        }

        if (true == bParamOk)
        {
            goto End;
        }
        else
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ExeFail;
        }

        break;
    }
    default:
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        goto NotSupport;
    }

ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

ExeFail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

NotSupport:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);

End:
    return;
}

void AT_COAP_CmdFunc_DELETE(atCommand_t *pParam)
{
    if (NULL == g_pstMutex)
    {
        g_pstMutex = osiMutexCreate();
    }

    osiMutexLock(g_pstMutex);
    if (true == g_bCoapDoing)
    {
        osiMutexUnlock(g_pstMutex);
        goto ExeFail;
    }
    else
    {
        g_bCoapDoing = true;
    }
    osiMutexUnlock(g_pstMutex);

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint8_t def_seconds = DATA_TIMEOUT_DEF;
        const char *url = atParamStr(pParam->params[0], &paramok);
        if (strlen(url) > AT_URL_LENG_MAX)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
        const char *cmdline = atParamStr(pParam->params[1], &paramok);

        if (pParam->param_count < 2 || pParam->param_count > 3)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        if (pParam->param_count > 2)
        {
            uint8_t wait_seconds = atParamIntInRange(pParam->params[2], DATA_TIMEOUT_MIN, DATA_TIMEOUT_MAX, &paramok);
            if (!paramok)
            {
                osiMutexLock(g_pstMutex);
                g_bCoapDoing = false;
                osiMutexUnlock(g_pstMutex);
                goto ParamInvalid;
            }
            if (wait_seconds != 0)
                def_seconds = wait_seconds;
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x10003f0d, "AT_COAP_CmdFunc_GET cmdline =%s", cmdline);
        if (!paramok)
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }

        uint8_t timeout_param[10];
        sprintf((char *)timeout_param, "-B %d", def_seconds);

        coap_client_t *client = AT_getCoapClient(pParam->engine);
        if (client != NULL)
        {
            coap_client_setParams(client, timeout_param);
            coap_client_setParams(client, (uint8_t *)cmdline);
            coap_client_setIndataCbk(client, (coap_incoming_data_cb_t)coapProcessReturnData);
            paramok = coap_async_delete(client, (uint8_t *)url, (uint8_t *)cmdline);
        }
        else
            paramok = false;

        if (paramok)
        {
            AT_SetAsyncTimerMux(pParam->engine, def_seconds + 5);
            goto End;
        }
        else
        {
            osiMutexLock(g_pstMutex);
            g_bCoapDoing = false;
            osiMutexUnlock(g_pstMutex);
            goto ParamInvalid;
        }
    }
    else
    {
        osiMutexLock(g_pstMutex);
        g_bCoapDoing = false;
        osiMutexUnlock(g_pstMutex);
        goto NotSupport;
    }

ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);

ExeFail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);

NotSupport:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);

End:
    return;
}

void AT_COAP_CmdFunc_DATAREG(atCommand_t *pParam)
{
    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        bool dontReset = 0;

        if (1 != pParam->param_count)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        dontReset = atParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
#if 0 //don't support
        if (!dontReset)
            NV_SetUEIccid(NULL,20,(AT_SIM_ID(pParam->nDLCI)));
        if (vnetIotSelfRegister(AT_SIM_ID(pParam->nDLCI))<0)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        else
            RETURN_OK(pParam->engine);
#else
        if (!dontReset)
            RETURN_OK(pParam->engine);
        else
            RETURN_OK(pParam->engine);
#endif
    }
    else
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    return;
}
#endif
