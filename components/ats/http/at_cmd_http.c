/*********************************************************
 *
 * File Name
 *        at_http_api.c
 * Author
 *         shaoboan
 * Date
 *         2017/03/18
 * Descriptions:
 *        Commands specific to MTs supporting the Packet Domain.
 *
 *********************************************************/
#include "ats_config.h"

#ifdef CONFIG_AT_HTTP_SUPPORT
#include "at_command.h"
#include "stdio.h"
#include "cfw.h"
#include "at_cmd_http.h"
#include "sockets.h"
#include "http_api.h"
#include "at_engine.h"
#include "http_download.h"
#include "http_lunch_api.h"
#include "async_worker.h"
#include "at_response.h"
#include "osi_log.h"
#include "netutils.h"

#define AT_MAX_PPP_ID 8
#define AT_TCPIPACTPHASE_GPRS 0
#define AT_StrLen(str) strlen((const char *)str)
#define AT_StrCpy(des, src) strcpy((char *)des, (const char *)src)

#define HTTP_URL_SIZE 255
uint8_t outString[1500] = {
    0x00,
};
nHttp_info *at_nHttp_info;
Http_info *at_Http_info;
long real_len = 0;
bool is_http1_inited = false;
bool is_http2_inited = false;
bool is_aworker_runing = false;
bool is_ready_to_action = false;

atCmdEngine_t *gHttpEngine = NULL;

static AWORKER_REQ *s_httpx_aworker_req;
//static uint8_t s_dlci1 = 0;

typedef enum at_httpx_event
{
    AT_HTTPX_EV_GET = 1,
    AT_HTTPX_EV_PUT,
    AT_HTTPX_EV_HEAD,
    AT_HTTPX_EV_AUTHOR,
    AT_HTTPX_EV_OPTIONS,
    AT_HTTPX_EV_TRACE,
    AT_HTTPX_EV_DELETE,
    AT_HTTPX_EV_POST,
    AT_HTTPX_EV_DOWNLOAD,
    AT_HTTPX_EV_GETN,
    AT_HTTPX_EV_POSTN,
    AT_HTTPX_EV_DELETEN,
    AT_HTTPX_EV_PARA,
    AT_HTTPX_EV_HEADN,
    AT_HTTPX_EV_ONENET_DELETE,
    AT_HTTPX_EV_ONENET_PUT,
    AT_HTTPX_EV_DATA,
    AT_HTTPX_EV_READ,
} AT_HTTPX_EVT;
static AWORKER_RC at_httpx_worker_handler(AWORKER_REQ *req)
{
    uint32_t event = req->event;
    AWORKER_RC rc = AWRC_SUCCESS;
    bool paramret = true;
    uint16_t index = 0;
    mUpnpHttpResponse *response = NULL;
    s_httpx_aworker_req = req;
    Http_info *Http_info1;
    nHttp_info *nHttp_info1;
    OSI_LOGI(0x10004702, "at_httpx_worker_handler event:%d", event);
    switch (event)
    {
    case AT_HTTPX_EV_GET:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004703, "AT_HTTPX_EV_GET get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_get(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_HEAD:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004704, "AT_HTTPX_EV_HEAD get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_head(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            mupnp_http_response_clear(response);

            mupnp_http_response_delete(response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_AUTHOR:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004705, "AT_HTTPX_EV_AUTHOR get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_authorization(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            mupnp_http_response_clear(response);

            mupnp_http_response_delete(response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_OPTIONS:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004706, "AT_HTTPX_EV_OPTIONS get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_options(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            mupnp_http_response_clear(response);

            mupnp_http_response_delete(response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_TRACE:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004707, "AT_HTTPX_EV_TRACE get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_trace(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            mupnp_http_response_clear(response);

            mupnp_http_response_delete(response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_DELETE:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004708, "AT_HTTPX_EV_PUT get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((Http_delete(Http_info1)) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_PUT:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004708, "AT_HTTPX_EV_PUT get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((Http_put(Http_info1)) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_POST:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004709, "AT_HTTPX_EV_POST get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Http_info1 = (Http_info *)ADDR;
        if ((response = Http_post(Http_info1)) == NULL)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            http_response_print((mUpnpHttpPacket *)response);

            mupnp_http_response_clear(response);

            mupnp_http_response_delete(response);

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_PARA:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470a, "AT_HTTPX_EV_PARA get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if (false == Http_para(nHttp_info1, nHttp_info1->tag, nHttp_info1->value))
        {
            rc = AWRC_FAIL;
            Http_WriteUart("Error Tag, please use correct Tag\r\n", 36);
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_HEADN:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470b, "AT_HTTPX_EV_HEADN get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if (false == Http_headn(nHttp_info1))
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_ONENET_DELETE:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470c, "AT_HTTPX_EV_ONENET_DELETE get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if (false == Http_onenet_delete(nHttp_info1))
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_ONENET_PUT:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470d, "AT_HTTPX_EV_ONENET_PUT get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if (false == Http_onenet_put(nHttp_info1))
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_READ:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470e, "AT_HTTPX_EV_READ get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if (false == Http_read(nHttp_info1, nHttp_info1->offset, nHttp_info1->length))
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    case AT_HTTPX_EV_DATA:
    {

        uint32_t ADDR;
        uint32_t ADDR1;
        uint32_t Len;
        char *data;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470f, "AT_HTTPX_EV_DATA get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        ADDR1 = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470f, "AT_HTTPX_EV_DATA get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        Len = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x1000470f, "AT_HTTPX_EV_DATA get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        data = (char *)ADDR1;
        at_nHttp_info->content_length = Len;
        if (false == http_setUserdata(nHttp_info1, data, Len))
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        if (data != NULL)
        {
            free(data);
            data = NULL;
        }
        break;
    }

    case AT_HTTPX_EV_DOWNLOAD:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004710, "AT_HTTPX_EV_DOWNLOAD get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;

        if (Http_getn_break(nHttp_info1) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }

        break;
    }
    case AT_HTTPX_EV_GETN:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004710, "AT_HTTPX_EV_DOWNLOAD get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;

        if (Http_getn_break(nHttp_info1) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }

        break;
    }
    case AT_HTTPX_EV_POSTN:
    {

        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004710, "AT_HTTPX_EV_DOWNLOAD get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;

        if (Http_postn(nHttp_info1) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }

        break;
    }
    case AT_HTTPX_EV_DELETEN:
    {
        uint32_t ADDR;
        ADDR = aworker_param_getu32(req, index++, (bool *)&paramret);
        if (!paramret)
        {
            OSI_LOGI(0x10004711, "AT_HTTPX_EV_DELETEN get param fail from async worker");
            rc = AWRC_FAIL;
            break;
        }
        nHttp_info1 = (nHttp_info *)ADDR;
        if ((Http_deleten(nHttp_info1)) == false)
        {
            rc = AWRC_FAIL;
            break;
        }
        else
        {

            rc = AWRC_SUCCESS;
        }
        break;
    }
    default:
        OSI_LOGI(0x10004712, "at_httpx_worker_handler unhandled event:%d", event);
        break;
    }
    if (rc != AWRC_PROCESSING)
    {
        // if return value was not AWRC_PROCESSING,
        // the request will be deleted by aworker, and become invalid
        s_httpx_aworker_req = NULL;
    }
    return rc;
}

static void at_httpx_worker_callback(int result, uint32_t event, void *param)
{

    OSI_LOGI(0x10004462, "at_mqttx_worker_callback result:%d event:%d", result, event);
    switch (event)
    {
    case AT_HTTPX_EV_GET:
        break;
    case AT_HTTPX_EV_PUT:
        break;
    case AT_HTTPX_EV_POST:
        break;
    case AT_HTTPX_EV_HEAD:
        break;
    case AT_HTTPX_EV_AUTHOR:
        break;
    case AT_HTTPX_EV_OPTIONS:
        break;
    case AT_HTTPX_EV_TRACE:
        break;
    case AT_HTTPX_EV_DOWNLOAD:
        break;
    case AT_HTTPX_EV_DELETE:
        break;
    case AT_HTTPX_EV_PARA:
        break;
    case AT_HTTPX_EV_HEADN:
        break;
    case AT_HTTPX_EV_ONENET_DELETE:
        break;
    case AT_HTTPX_EV_ONENET_PUT:
        break;
    case AT_HTTPX_EV_DELETEN:
        break;
    case AT_HTTPX_EV_GETN:
        break;
    case AT_HTTPX_EV_POSTN:
        break;
    case AT_HTTPX_EV_READ:
        break;
    case AT_HTTPX_EV_DATA:
        break;
    default:
        OSI_LOGI(0x10004463, "at_mqttx_worker_callback unhandled event:%d", event);
        break;
    }
    if (result == 0)
    {
        // response operation succuss
        OSI_LOGI(0x10004713, "response operation succuss:%d", event);
        is_aworker_runing = false;
    }
    else
    {
        // response operation fail
        OSI_LOGI(0x10004714, "response operation fail:%d", event);
        is_aworker_runing = false;
    }
}

static AWORKER_REQ *at_httpx_create_async_req(uint32_t event, void *param, uint32_t bufflen)
{
    AWORKER_REQ *areq = aworker_create_request(atEngineGetThreadId(),
                                               at_httpx_worker_handler, at_httpx_worker_callback, event, param, bufflen);
    return areq;
}

void Http_WriteUart(const char *data, unsigned length)
{
    if (data != NULL)
    {
        atCmdWrite(gHttpEngine, data, (uint32_t)length);
        atCmdWrite(gHttpEngine, "\r\n", AT_StrLen("\r\n"));
    }
    else
    {
        atCmdWrite(gHttpEngine, "data is null", AT_StrLen("data is null"));
    }
}

void Http_WriteUartEx(atCmdEngine_t *engine, const char *data, unsigned length)
{
    if (engine == NULL)
        return;
    if (data != NULL)
    {
        atCmdWrite(engine, data, (uint32_t)length);
        atCmdWrite(engine, "\r\n", AT_StrLen("\r\n"));
    }
    else
    {
        atCmdWrite(engine, "data is null", AT_StrLen("data is null"));
    }
}

void SetHttpEngine(atCmdEngine_t *Engine)
{

    gHttpEngine = Engine;
}

void AT_CmdFunc_HTTPHEAD(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[60] = {
        0x00,
    };
    AWORKER_REQ *areq;
    bool paramRet = true;

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http2_inited)
        {
            Http_WriteUart("Please INITHTTP first.", strlen("Please INITHTTP first."));
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
        if (is_aworker_runing)
        {
            Http_WriteUart("Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (!is_ready_to_action)
        {
            Http_WriteUart("Http is not ready to action,please TERMHTTP then INITHTTP.", strlen("Http is not ready to action,please TERMHTTP then INITHTTP."));
            RETURN_OK(pParam->engine);
        }
        is_ready_to_action = false;
        if (pParam->param_count != 1)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        strcpy(at_Http_info->url, atParamStr(pParam->params[0], &iResult));
        if (!iResult)
        {
            OSI_LOGI(0x1000472c, "HTTPHEAD HEAD URL error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        OSI_LOGXI(OSI_LOGPAR_S, 0x1000472d, "HTTPHEAD HEAD URL : %s", at_Http_info->url);

        areq = at_httpx_create_async_req(AT_HTTPX_EV_HEAD, NULL, 0);

        if (areq == NULL)
        {
            OSI_LOGI(0x1000472e, "AT+HTTPHEAD create async request fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        aworker_param_putu32(areq, (uint32_t)at_Http_info, &paramRet);
        is_aworker_runing = true;

        if (!aworker_post_req_delay(areq, 0, &paramRet))
        {
            OSI_LOGI(0x1000472f, "AT+HTTPHEAD put param or send async request fail");
            is_aworker_runing = false;
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            RETURN_OK(pParam->engine);
        }

        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPHEAD:<url>");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x10004730, "HTTPHEAD ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void process_HTTPDLD_data(const unsigned char *data, size_t len)
{
    char OutStr[20];
    sprintf(OutStr, "+HTTPDLD: %d\r\n", len);
    Http_WriteUart(OutStr, AT_StrLen(OutStr));
    Http_WriteUart((char *)data, len);
}

/**
 * @brief This function sets the ca.crt,client.crt and client_private_key to be used for https for secure requests.
 * @This func have one parameter: 0: ca.crt  1: client.crt  2: clientprivate.key
 * @return      No Return value
 */

//static uint32_t crtFlag;
#if 0
void AT_CmdFunc_HTTPSSETCRT(atCommand_t *pParam)
{
    bool iResult = true;
    uint8_t PromptMsg[8] = {0};
    uint8_t tmpString[60] = {
        0x00,
    };
    bool result;
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004745, "AT+SETCACRT: pParam is NULL");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    gHttpEngine = pParam->engine;

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http1_inited)
        {

            OSI_LOGI(0x10004746, "please HTTPINIT first");
            AT_CMD_RETURN(at_CmdRespErrorText(pParam->engine, "+ERROR<please HTTPINIT first>"));
        }

        if (!pParam->iExDataLen)
        {
            if (pParam->pPara == NULL)
            {
                OSI_LOGI(0x10004745, "AT+SETCACRT: pParam is NULL");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (pParam->param_count == 0)
            {
                OSI_LOGI(0x10004747, "AT+SETCACRT: pPara number is 0");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get crtFlag
            crtFlag = at_ParamUintInRange(pParam->params[0], 0, 2, &iResult);
            OSI_LOGI(0x10004748, "crtFlag = %d ", crtFlag);
            if (!iResult)
            {
                OSI_LOGI(0x10004749, "get crtFlag error\n");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            PromptMsg[0] = AT_GC_CfgGetS3Value();
            PromptMsg[1] = AT_GC_CfgGetS4Value();
            AT_StrCat(PromptMsg, "> ");
            OSI_LOGI(0x1000474a, " auto sending timer:0 seconds");
            at_CmdRespInfoText(pParam->engine, (uint8_t *)PromptMsg);
            AT_CMD_ENGINE_T *th = at_CmdGetByChannel(pParam->nDLCI);
            at_CmdSetPromptMode(th);
            return;
        }
        else
        {
            if (pParam->pExData[pParam->iExDataLen - 1] == 27)
            {
                OSI_LOGI(0x10003f0b, "end with esc, cancel send");
                RETURN_OK(pParam->engine);
            }
            else if (pParam->pExData[pParam->iExDataLen - 1] != 26)
            {
                OSI_LOGI(0x10003f0c, "not end with ctl+z, err happen");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CMS_INVALID_LEN);
            }
            OSI_LOGI(0x1000474b, "pParam->iExDataLen: %d", pParam->iExDataLen);
#if defined(MUPNP_USE_OPENSSL)
            result = Https_saveCrttoFile(pParam->pExData, pParam->iExDataLen - 1, crtFlag);
            OSI_LOGI(0x1000474c, "MUPNP_USE_OPENSSL");
#else
            result = false;
            OSI_LOGI(0x100052cb, "result = false");
#endif
            if (!result)
            {
                OSI_LOGI(0x1000474e, "Https_saveCrttoFile error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CMS_MEM_FAIL);
            }
            at_CmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
            return;
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+SETCACRT=<crtFlag>,crtFlag:0,1,2");
        at_CmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000474f, "AT_CmdFunc_HTTPSSETCRT ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
#endif
void AT_CmdFunc_HTTPPARA(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[60] = {
        0x00,
    };
    AWORKER_REQ *areq;
    bool paramRet = true;
    char *pstr = NULL;

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http1_inited)
        {
            OSI_LOGI(0x10004746, "please HTTPINIT first");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
        }
        if (is_aworker_runing)
        {
            Http_WriteUart("Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (pParam->param_count != 2)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        pstr = (char *)atParamStr(pParam->params[0], &iResult);
        if (!iResult || strlen(pstr) >= SIZE_HTTP_USR)
        {
            OSI_LOGI(0x10004751, "TAG error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        strncpy(at_nHttp_info->tag, pstr, SIZE_HTTP_USR);
        at_nHttp_info->tag[SIZE_HTTP_USR - 1] = '\0';

        pstr = (char *)atParamStr(pParam->params[1], &iResult);
        if (!iResult || strlen(pstr) >= SIZE_HTTP_USR)
        {
            OSI_LOGI(0x10004752, "VALUE error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        strncpy(at_nHttp_info->value, pstr, SIZE_HTTP_USR);
        at_nHttp_info->value[SIZE_HTTP_USR - 1] = '\0';

        OSI_LOGXI(OSI_LOGPAR_S, 0x10004753, "VALUE IS : %s", at_nHttp_info->value);

        areq = at_httpx_create_async_req(AT_HTTPX_EV_PARA, NULL, 0);

        if (areq == NULL)
        {
            OSI_LOGI(0x10004754, "AT+HTTPPARA create async request fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
        is_aworker_runing = true;
        if (!aworker_post_req_delay(areq, 0, &paramRet))
        {
            OSI_LOGI(0x10004755, "AT+HTTPPARA send async request fail");
            is_aworker_runing = false;
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            RETURN_OK(pParam->engine);
        }

        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPPARA:<tag>,<value>");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x10004756, "+HTTPPARA ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
void AT_CmdFunc_HTTPACTION(atCommand_t *pParam)
{
    bool iResult = true;
    //uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    uint8_t nSim = atCmdGetSim(pParam->engine);
    uint8_t METHOD;
    AWORKER_REQ *areq;
    bool paramRet = true;
    char tmpString[60] = {
        0x00,
    };
    struct netif *netif = NULL;

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http1_inited)
        {
            Http_WriteUart("Please HTTPINIT first.", strlen("Please HTTPINIT first."));
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
        }
        if (is_aworker_runing)
        {
            Http_WriteUart("Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (!is_ready_to_action)
        {
            Http_WriteUart("Http is not ready to action,please HTTPTERM then HTTPINIT.", strlen("Http is not ready to action,please HTTPTERM then HTTPINIT."));
            RETURN_OK(pParam->engine);
        }
        is_ready_to_action = false;

        if (pParam->param_count != 1)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        METHOD = atParamUintInRange(pParam->params[0], 0, 3, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0x10004757, "HTTPACTION GET METHOD error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        OSI_LOGI(0x10004758, "HTTPACTION begin get netif :");

        OSI_LOGI(0x10004759, "HTTPACTION cid %d :", at_nHttp_info->CID);

        OSI_LOGI(0x1000475a, "HTTPACTION SIM %d :", nSim);

        if (at_nHttp_info->CID < 1 || at_nHttp_info->CID > 7)
        {
            OSI_LOGI(0, "HTTPACTION CID error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_PARA_CID_INVALID);
        }

        netif = getGprsNetIf(nSim, at_nHttp_info->CID);

        if (netif == NULL)
        {
            OSI_LOGI(0x1000475b, "HTTPACTION GET NETIF error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_NETIF_NULL);
        }

        sprintf(tmpString, "%c%c%d", netif->name[0], netif->name[1], netif->num);

        AT_StrCpy(at_nHttp_info->device.ifr_name, tmpString);

        OSI_LOGI(0x1000475c, "HTTPACTION begin get device :");

        OSI_LOGI(0x1000475d, "HTTPACTION METHOD %d :", METHOD);

        switch (METHOD)
        {
        case 0:
            at_nHttp_info->method_code = 0;

            OSI_LOGXI(OSI_LOGPAR_S, 0x1000475e, "HTTPGET  URL %s", at_nHttp_info->url);

            areq = at_httpx_create_async_req(AT_HTTPX_EV_GETN, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x1000475f, "AT+HTTPGETN create async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;
            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x10004760, "AT+HTTPGETN send async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }
            break;

        case 1:
            at_nHttp_info->method_code = 1;

            areq = at_httpx_create_async_req(AT_HTTPX_EV_POSTN, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x10004761, "AT+HTTPPOSTN create async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;
            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x10004762, "AT+HTTPPOSTN put param or send async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }

            break;

        case 2:
            at_nHttp_info->method_code = 2;

            areq = at_httpx_create_async_req(AT_HTTPX_EV_HEADN, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x10004763, "AT+HTTPHEADN create async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;

            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x10004764, "AT+HTTPHEADN put param or send async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }
            break;

        case 3:
            at_nHttp_info->method_code = 3;

            areq = at_httpx_create_async_req(AT_HTTPX_EV_DELETEN, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x10004765, "AT+HTTPDELETENcreate async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;

            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x10004766, "AT+HTTPDELETENsend put param or async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }

            break;
#if 0
        case 4:
            at_nHttp_info->method_code = 4;

            areq = at_httpx_create_async_req(AT_HTTPX_EV_ONENET_DELETE, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x10004767, "AT+HTTPONENETDELETE create async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;

            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x10004768, "AT+HTTPONENETDELETE put param or send async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }

            break;
        case 5:
            at_nHttp_info->method_code = 5;

            areq = at_httpx_create_async_req(AT_HTTPX_EV_ONENET_PUT, NULL, 0);

            if (areq == NULL)
            {
                OSI_LOGI(0x10004769, "AT+HTTPONENETPUT create async request fail");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }

            aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
            is_aworker_runing = true;
            if (!aworker_post_req_delay(areq, 0, &paramRet))
            {
                OSI_LOGI(0x1000476a, "AT+HTTPONENETPUT put param or send async request fail");
                is_aworker_runing = false;
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                RETURN_OK(pParam->engine);
            }

            break;
#endif
        default:
            OSI_LOGI(0x100052cc, "HTTPACTION METHOD error, ONLY SUPPORT 0~3");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPACTION:(0~3)");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000476c, "HTTPACTION ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
void AT_CmdFunc_HTTPSTATUS(atCommand_t *pParam)
{
    char *method_name = NULL;
    int status;
    char tmpString[60] = {
        0x00,
    };

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    if (!is_http1_inited)
    {
        OSI_LOGI(0x10004746, "please HTTPINIT first");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
    }

    switch (pParam->type)
    {
    case AT_CMD_EXE:

        if (at_nHttp_info->method_code == 0)
        {

            method_name = "GET";
            status = 1;
            sprintf(tmpString, "%s %d %ld %ld", method_name, status, at_nHttp_info->data_finish, at_nHttp_info->data_remain);
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
        else if (at_nHttp_info->method_code == 1)
        {

            method_name = "POST";
            status = 2;
            sprintf(tmpString, "%s %d %ld %ld", method_name, status, at_nHttp_info->data_finish, at_nHttp_info->data_remain);
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
        else if (at_nHttp_info->method_code == 2)
        {

            method_name = "HEAD";
            status = 1;
            sprintf(tmpString, "%s %d %ld %ld", method_name, status, at_nHttp_info->data_finish, at_nHttp_info->data_remain);
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
        else if (at_nHttp_info->method_code == 3)
        {

            method_name = "DELETE";
            status = 1;
            sprintf(tmpString, "%s %d %ld %ld", method_name, status, at_nHttp_info->data_finish, at_nHttp_info->data_remain);
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
        else
        {
            OSI_LOGI(0, "please HTTPACTION first");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_ACTION_NEED);
        }

        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPSTATUS");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000476d, "HTTPSTATUS ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
void AT_CmdFunc_HTTPREAD(atCommand_t *pParam)
{
    bool iResult = true;
    long offset;
    long length;
    char tmpString[60] = {
        0x00,
    };
    AWORKER_REQ *areq;
    bool paramRet = true;

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http1_inited)
        {

            OSI_LOGI(0x10004746, "please HTTPINIT first");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
        }
        if (is_aworker_runing)
        {
            Http_WriteUart("Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (pParam->param_count != 2)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (at_nHttp_info->method_code == 2)
        {
            OSI_LOGI(0, "HTTPREAD error, HEAD method has no content!");
            Http_WriteUart("HEAD method has no content!\n", strlen("HEAD method has no content!\n"));
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        at_nHttp_info->offset = atParamUintInRange(pParam->params[0], 0, 319488, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0x1000476f, "HTTPREAD OFFSET error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        at_nHttp_info->length = atParamUintInRange(pParam->params[1], 1, 319488, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0x10004770, "HTTPREAD LENGTH URL error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        offset = at_nHttp_info->offset;
        length = at_nHttp_info->length;
        OSI_LOGI(0, "HTTPREAD offset %d length %d", offset, length);
        OSI_LOGI(0x10004772, "HTTPREAD at_nHttp_info->content_length %d", at_nHttp_info->content_length);
        if (offset >= at_nHttp_info->content_length)
        {
            OSI_LOGI(0x10004773, "HTTPREAD  error, ADDR is too large!");
            Http_WriteUart("HTTPREAD  error, ADDR is too large!\n", strlen("HTTPREAD  error, ADDR is too large!\n"));
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        areq = at_httpx_create_async_req(AT_HTTPX_EV_READ, NULL, 0);

        if (areq == NULL)
        {
            OSI_LOGI(0x10004775, "AT+HTTPREAD create async request fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }

        aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
        is_aworker_runing = true;

        if (!aworker_post_req_delay(areq, 0, &paramRet))
        {
            OSI_LOGI(0x10004776, "AT+HTTPREAD send async request fail");
            is_aworker_runing = false;
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            RETURN_OK(pParam->engine);
        }

        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPREAD:<start_address>,<byte_size>");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x10004777, "HTTPREAD ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

#define HTTP_DATA_MAX_SIZE (10 * 1024)
static uint32_t crtFlag;
typedef struct
{
    uint32_t uMuxIndex;
    uint32_t pos;
    uint32_t size;
    uint8_t *data;
} httpAsyncCtx_t;

static void httpAsyncCtxDestroy(atCommand_t *cmd)
{
    httpAsyncCtx_t *async = (httpAsyncCtx_t *)cmd->async_ctx;
    if (async == NULL)
        return;
    if (async->data != NULL)
        free(async->data);
    free(async);
    cmd->async_ctx = NULL;
}
static void _httpSetcrtPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    bool paramRet = true;
    httpAsyncCtx_t *async = (httpAsyncCtx_t *)cmd->async_ctx;
    OSI_LOGI(0x0, "AT HTTPSSETCRT end_mode:%d", end_mode);
    OSI_LOGI(0x0, "AT HTTPSSETCRT send size/%d", size);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
#if defined(MUPNP_USE_OPENSSL)
    paramRet = Https_saveCrttoFile((char *)async->data, size - 1, crtFlag);
    OSI_LOGI(0x1000474c, "MUPNP_USE_OPENSSL");
#else
    paramRet = false;
    OSI_LOGI(0x100052cb, "result = false");
#endif
    if (!paramRet)
    {
        OSI_LOGI(0x1000474e, "Https_saveCrttoFile error");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_MEMORY_FAILURE);
    }
    RETURN_OK(cmd->engine);
}

/**
 * @brief This function sets the ca.crt,client.crt and client_private_key to be used for https for secure requests.
 * @This func have one parameter: 0: ca.crt  1: client.crt  2: clientprivate.key
 * @return      No Return value
 */

void AT_CmdFunc_HTTPSSETCRT(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[60] = {
        0x00,
    };
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004745, "AT+SETCACRT: pParam is NULL");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if (!is_http1_inited)
        {

            OSI_LOGI(0x10004746, "please HTTPINIT first");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
        }

        if (pParam->params == NULL)
        {
            OSI_LOGI(0x100052cd, "AT+HTTPSSETCRT: pParam is NULL");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (pParam->param_count != 1)
        {
            OSI_LOGI(0x100052ce, "AT+HTTPSSETCRT: pPara number error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        // get crtFlag
        crtFlag = atParamUintInRange(pParam->params[0], 0, 2, &iResult);
        OSI_LOGI(0x10004748, "crtFlag = %d ", crtFlag);
        if (!iResult)
        {
            OSI_LOGI(0x10004749, "get crtFlag error\n");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        httpAsyncCtx_t *async = (httpAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        memset(async, 0, sizeof(*async));
        pParam->async_ctx = async;
        async->data = (uint8_t *)malloc(HTTP_DATA_MAX_SIZE + 1);
        if (NULL == async->data)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(async->data, 0, HTTP_DATA_MAX_SIZE + 1);
        pParam->async_ctx_destroy = httpAsyncCtxDestroy;
        atCmdRespOutputPrompt(pParam->engine);
        atCmdSetPromptMode(pParam->engine, _httpSetcrtPromptCB, pParam, async->data, HTTP_DATA_MAX_SIZE);
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPSSETCRT:(0~2)");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000474f, "AT_CmdFunc_HTTPSSETCRT ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

static void _httpDataPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    AWORKER_REQ *areq;
    bool paramRet = true;
    httpAsyncCtx_t *async = (httpAsyncCtx_t *)cmd->async_ctx;
    OSI_LOGI(0x0, "AT HTTPDATA end_mode:%d", end_mode);
    OSI_LOGI(0x0, "AT HTTPDATA send size/%d", size);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

    areq = at_httpx_create_async_req(AT_HTTPX_EV_DATA, NULL, 0);
    if (areq == NULL)
    {
        OSI_LOGI(0x1000477a, "AT+HTTPDATA create async request fail");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    uint8_t *asyncdata = (uint8_t *)malloc(HTTP_DATA_MAX_SIZE + 1);
    if (NULL == asyncdata)
    {
        aworker_dismiss_request(areq);
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);
    }
    memset(asyncdata, 0, HTTP_DATA_MAX_SIZE + 1);
    memcpy(asyncdata, async->data, HTTP_DATA_MAX_SIZE);
    aworker_param_putu32(areq, (uint32_t)at_nHttp_info, &paramRet);
    aworker_param_putu32(areq, (uint32_t)asyncdata, &paramRet);
    aworker_param_putu32(areq, (size - 1), &paramRet);
    is_aworker_runing = true;
    if (!aworker_post_req_delay(areq, 0, &paramRet))
    {
        OSI_LOGI(0x1000477b, "AT+HTTPDATA put param or send async request fail");
        is_aworker_runing = false;
        free(asyncdata);
        asyncdata = NULL;
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }
    else
    {
        RETURN_OK(cmd->engine);
    }
}

void AT_CmdFunc_HTTPDATA(atCommand_t *pParam)
{
    char tmpString[60] = {
        0x00,
    };
    if (NULL == pParam)
    {
        OSI_LOGI(0x10004745, "AT+SETCACRT: pParam is NULL");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_EXE:

        if (!is_http1_inited)
        {

            OSI_LOGI(0x10004746, "please HTTPINIT first");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_HTTP_INIT_NEED);
        }
        if (is_aworker_runing)
        {
            Http_WriteUart("Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (pParam->param_count != 0)
        {
            OSI_LOGI(0x10004779, "AT+HTTPDATA: pPara number is error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        httpAsyncCtx_t *async = (httpAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        memset(async, 0, sizeof(*async));
        pParam->async_ctx = async;
        async->data = (uint8_t *)malloc(HTTP_DATA_MAX_SIZE + 1);
        if (NULL == async->data)
        {
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        }
        memset(async->data, 0, HTTP_DATA_MAX_SIZE + 1);
        pParam->async_ctx_destroy = httpAsyncCtxDestroy;
        atCmdRespOutputPrompt(pParam->engine);
        atCmdSetPromptMode(pParam->engine, _httpDataPromptCB, pParam, async->data, HTTP_DATA_MAX_SIZE);
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPDATA");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x1000477c, "AT_CmdFunc_HTTPDATA ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
void AT_CmdFunc_HTTPINIT(atCommand_t *pParam)
{
    char tmpString[60] = {
        0x00,
    };

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    switch (pParam->type)
    {
    case AT_CMD_EXE:
        if (is_http2_inited)
        {
            Http_WriteUartEx(pParam->engine, "Please TERMHTTP first.", strlen("Please TERMHTTP first."));
            RETURN_OK(pParam->engine);
        }
        if (is_aworker_runing)
        {
            Http_WriteUartEx(pParam->engine, "Aworker is running please wait.", strlen("Aworker is running please wait."));
            RETURN_OK(pParam->engine);
        }
        if (!is_http1_inited)
        {
            at_nHttp_info = Init_Http();
            if (at_nHttp_info == NULL)
            {
                atCmdRespInfoText(pParam->engine, "Init fail, please try again later\n");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            //at_ModuleGetSimCid(&((at_nHttp_info->cg_http_api)->nSIM), &((at_nHttp_info->cg_http_api)->nCID), pParam->nDLCI);
            //at_ModuleGetSimCid(&((at_nHttp_info->cg_http_api)->nSIM), &((at_nHttp_info->cg_http_api)->nCID),0);
            (at_nHttp_info->cg_http_api)->nSIM = 0;
            (at_nHttp_info->cg_http_api)->nCID = 1;

            is_http1_inited = true;
            is_ready_to_action = true;
            gHttpEngine = pParam->engine;
            RETURN_OK(pParam->engine);
        }
        else
        {
            atCmdRespInfoText(pParam->engine, "HTTP is already INIT, please exe AT+HTTPTERM first\n");
            RETURN_OK(pParam->engine);
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPINIT");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x100040ca, "HTTPINIT ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
void AT_CmdFunc_HTTPTERM(atCommand_t *pParam)
{
    char tmpString[60] = {
        0x00,
    };

    if (NULL == pParam)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (gHttpEngine != NULL && gHttpEngine != pParam->engine)
    {
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }

    switch (pParam->type)
    {
    case AT_CMD_EXE:
        if (is_aworker_runing)
        {

            atCmdRespInfoText(pParam->engine, "Aworker is running, please wait!\n");
            RETURN_OK(pParam->engine);
        }
        if (is_http1_inited)
        {

            real_len = 0;
            if (Term_Http(at_nHttp_info) != true)
            {

                atCmdRespInfoText(pParam->engine, "Term fail, please try again later\n");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }

            is_http1_inited = false;
            gHttpEngine = NULL;

            RETURN_OK(pParam->engine);
        }
        else
        {

            atCmdRespInfoText(pParam->engine, "HTTP is already TERM, please exe AT+HTTPINIT first\n");
            RETURN_OK(pParam->engine);
        }
        break;
    case AT_CMD_TEST:
        AT_StrCpy(tmpString, "+HTTPTERM");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
        break;
    default:
        OSI_LOGI(0x100040cb, "HTTPTERM ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

#endif //AT_HTTP_SUPPORT
