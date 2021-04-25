#include "ats_config.h"
#ifdef CONFIG_AT_PAHO_MQTT_SUPPORT
#include "stdio.h"
#include "cfw.h"
#include "at_cfg.h"
#include "sockets.h"
#include "unistd.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"
#include "at_command.h"
#include "at_engine.h"
#include "MQTTAsync.h"

#define MQTT_DEBUG_HEAD "PahoMqtt "

#define MQTT_HOSTNAME_LEN_MAX (256)
#define MQTT_PORT_DEF (1885)
#define MQTT_PORT_MIN (1)
#define MQTT_PORT_MAX (65535)
#define MQTT_CLIENTID_LEN_MAX (256)
#define MQTT_KEEPLIVE_DEF (60)
#define MQTT_KEEPLIVE_MIN (10)
#define MQTT_KEEPLIVE_MAX (3600)
#define MQTT_CLEANSESSION_DEF (1)
#define MQTT_CLEANSESSION_MIN (0)
#define MQTT_CLEANSESSION_MAX (1)
#define MQTT_USERNAME_LEN_MAX (256)
#define MQTT_PASSWORD_LEN_MAX (256)
#define MQTT_TOPIC_LEN_MAX (64)
#define MQTT_MESSAGE_LEN_MAX (256)
#define MQTT_QOS_DEF (0)
#define MQTT_QOS_MIN (0)
#define MQTT_QOS_MAX (2)
#define MQTT_DUP_DEF (0)
#define MQTT_DUP_MIN (0)
#define MQTT_DUP_MAX (1)
#define MQTT_RETAIN_DEF (0)
#define MQTT_RETAIN_MIN (0)
#define MQTT_RETAIN_MAX (1)
#define MQTT_SUB_FLAG_DEF (0)
#define MQTT_UNSUB_FLAG (0)
#define MQTT_SUB_FLAG (1)

#define MQTT_EVENT_BASE (0xAC)
#define MQTT_CONN_RSP_SUCC (MQTT_EVENT_BASE + 0)
#define MQTT_CONN_RSP_FAIL (MQTT_EVENT_BASE + 1)
#define MQTT_CONN_RSP_LOST (MQTT_EVENT_BASE + 2)
#define MQTT_PUB_RSP_SUCC (MQTT_EVENT_BASE + 3)
#define MQTT_PUB_RSP_FAIL (MQTT_EVENT_BASE + 4)
#define MQTT_SUB_RSP_SUCC (MQTT_EVENT_BASE + 5)
#define MQTT_SUB_RSP_FAIL (MQTT_EVENT_BASE + 6)
#define MQTT_UNSUB_RSP_SUCC (MQTT_EVENT_BASE + 7)
#define MQTT_UNSUB_RSP_FAIL (MQTT_EVENT_BASE + 8)
#define MQTT_DISCONN_RSP_SUCC (MQTT_EVENT_BASE + 9)
#define MQTT_DISCONN_RSP_FAIL (MQTT_EVENT_BASE + 10)

typedef enum
{
    DISCONNECTED = 0,
    CONNECTED = 1,
    CONNECTING = 2
} EMqttConnectState;

struct MqttOpts
{
    /* MQTT client state */
    EMqttConnectState m_eState;

    /* MQTT options */
    int m_eMQTTVersion;
    char *m_pcTopic;
    char *m_pcClientid;
    int m_iQos;
    char *m_pcUsername;
    char *m_pcPassword;
    char *m_pcHost;
    char *m_pcPort;
    int m_iCleanSession;
    char *m_pcConnection;
    int m_iKeepalive;
    int m_iRetained;

    /* message options */
    char *m_pcMessage;

    /* TLS options */
    int m_iInsecure;
    char *m_pcCapath;
    char *m_pcCert;
    char *m_pcCafile;
    char *m_pcKey;
    char *m_pcKeypass;
    char *m_pcCiphers;
};
static struct MqttOpts g_stMqttOpts = {0};
static MQTTAsync g_stMqttClient = NULL;
static atCmdEngine_t *g_atCmdEngine = NULL;

static void MqttOptsInit();
static void MqttOptsUninit();
static int _MessageArrivedCb(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
static void _ConnectionLostCb(void *context, char *cause);
static void onConnect(void *context, MQTTAsync_successData *response);
static void onConnectFailure(void *context, MQTTAsync_failureData *response);
static int MqttConnect(MQTTAsync client);
static void onSubscribe(void *context, MQTTAsync_successData *response);
static void onSubscribeFailure(void *context, MQTTAsync_failureData *response);
static int MqttSubscribe(MQTTAsync client);
static void onUnsubscribe(void *context, MQTTAsync_successData *response);
static void onUnsubscribeFailure(void *context, MQTTAsync_failureData *response);
static int MqttUnsubscribe(MQTTAsync client);
static void onPublish(void *context, MQTTAsync_successData *response);
static void onPublishFailure(void *context, MQTTAsync_failureData *response);
static int MqttPublish(MQTTAsync client, int datalen, char *data);
static void onDisconnect(void *context, MQTTAsync_successData *response);
static void onDisconnectFailure(void *context, MQTTAsync_failureData *response);
static int MqttDisconnect(MQTTAsync client);

//Mqtt Options init & uninit
static void MqttOptsInit()
{
    g_stMqttOpts.m_eState = DISCONNECTED;
    g_stMqttOpts.m_eMQTTVersion = MQTTVERSION_DEFAULT;
    g_stMqttOpts.m_pcTopic = NULL;
    g_stMqttOpts.m_pcClientid = NULL;
    g_stMqttOpts.m_iQos = MQTT_QOS_DEF;
    g_stMqttOpts.m_pcUsername = NULL;
    g_stMqttOpts.m_pcPassword = NULL;
    g_stMqttOpts.m_pcHost = NULL;
    g_stMqttOpts.m_pcPort = NULL;
    g_stMqttOpts.m_iCleanSession = MQTT_CLEANSESSION_DEF;
    g_stMqttOpts.m_pcConnection = 0;
    g_stMqttOpts.m_iKeepalive = MQTT_KEEPLIVE_DEF;
    g_stMqttOpts.m_iRetained = MQTT_RETAIN_DEF;
    g_stMqttOpts.m_pcMessage = NULL;
    g_stMqttOpts.m_iInsecure = 0;
    g_stMqttOpts.m_pcCapath = NULL;
    g_stMqttOpts.m_pcCert = NULL;
    g_stMqttOpts.m_pcCafile = NULL;
    g_stMqttOpts.m_pcKey = NULL;
    g_stMqttOpts.m_pcKeypass = NULL;
    g_stMqttOpts.m_pcCiphers = NULL;

    g_atCmdEngine = NULL;

    return;
}

static void MqttOptsUninit()
{
    g_stMqttOpts.m_eState = DISCONNECTED;
    g_stMqttOpts.m_eMQTTVersion = MQTTVERSION_DEFAULT;
    g_stMqttOpts.m_pcTopic = NULL;
    g_stMqttOpts.m_pcClientid = NULL;
    g_stMqttOpts.m_iQos = MQTT_QOS_DEF;
    g_stMqttOpts.m_pcUsername = NULL;
    g_stMqttOpts.m_pcPassword = NULL;
    g_stMqttOpts.m_pcHost = NULL;
    g_stMqttOpts.m_pcPort = NULL;
    g_stMqttOpts.m_iCleanSession = MQTT_CLEANSESSION_DEF;
    g_stMqttOpts.m_pcConnection = 0;
    g_stMqttOpts.m_iKeepalive = MQTT_KEEPLIVE_DEF;
    g_stMqttOpts.m_iRetained = MQTT_RETAIN_DEF;
    g_stMqttOpts.m_pcMessage = NULL;
    g_stMqttOpts.m_iInsecure = 0;
    g_stMqttOpts.m_pcCapath = NULL;
    g_stMqttOpts.m_pcCert = NULL;
    g_stMqttOpts.m_pcCafile = NULL;
    g_stMqttOpts.m_pcKey = NULL;
    g_stMqttOpts.m_pcKeypass = NULL;
    g_stMqttOpts.m_pcCiphers = NULL;

    g_atCmdEngine = NULL;

    return;
}

static void _AtMqttPostEventCb(void *param)
{
    char cOutstr[64] = {0};
    osiEvent_t *pEvent = (osiEvent_t *)param;
    if (pEvent == NULL)
        return;
    int rc = (int)pEvent->param1;
    OSI_LOGI(0, "AT_MQTT_AsyncEventProcess() line[%d]:pEvent->nEventId=%d, rc=%d", __LINE__, pEvent->id, rc);
    switch (pEvent->id)
    {
    case MQTT_CONN_RSP_SUCC:
    {
        if (g_atCmdEngine != NULL)
            atCmdRespOK(g_atCmdEngine);
        g_stMqttOpts.m_eState = CONNECTED;
        break;
    }
    case MQTT_CONN_RSP_FAIL:
    {
        MQTTAsync_destroy(&g_stMqttClient);
        if (g_atCmdEngine != NULL)
            atCmdRespCmeError(g_atCmdEngine, ERR_AT_CME_EXE_FAIL);
        MqttOptsUninit();
        break;
    }
    case MQTT_CONN_RSP_LOST:
    {
        cOutstr[0] = '\0';
        sprintf(cOutstr, "%s", "+MQTTDISCONNECTED:256,connection lost.");
        cOutstr[strlen(cOutstr)] = '\0';
        if (g_atCmdEngine != NULL)
            atCmdRespInfoText(g_atCmdEngine, cOutstr);
        MQTTAsync_destroy(&g_stMqttClient);
        MqttOptsUninit();
        break;
    }
    case MQTT_DISCONN_RSP_SUCC:
    {
        MQTTAsync_destroy(&g_stMqttClient);
        //atCmdRespOK(g_atCmdEngine);
        MqttOptsUninit();
        break;
    }
    case MQTT_DISCONN_RSP_FAIL:
    {
        cOutstr[0] = '\0';
        sprintf(cOutstr, "%s", "+MQTTDISCONNECTED:disconnect fail.");
        cOutstr[strlen(cOutstr)] = '\0';
        if (g_atCmdEngine != NULL)
            atCmdRespInfoText(g_atCmdEngine, cOutstr);
        break;
    }
    default:
        break;
    }

    if (NULL != pEvent)
    {
        free(pEvent);
    }

    return;
}

static void AtMqttPostEvent(uint32_t nEventId, uint32_t response)
{
    osiEvent_t *ev = malloc(sizeof(osiEvent_t));
    ev->id = nEventId;
    ev->param1 = response;

    osiThreadCallback(atEngineGetThreadId(), _AtMqttPostEventCb, (void *)ev);
    return;
}

//Message Callback
static int _MessageArrivedCb(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    char *tmpmess = NULL;
    char *outstr = NULL;
    int length = 0;

    if (NULL != g_atCmdEngine)
    {
        tmpmess = (char *)malloc(message->payloadlen + 1);
        tmpmess[0] = '\0';
        strncpy(tmpmess, (char *)message->payload, message->payloadlen);
        tmpmess[message->payloadlen] = '\0';

        length = topicLen + message->payloadlen + 48;
        outstr = (char *)malloc(length);
        outstr[0] = '\0';
        sprintf(outstr, "%s:%s,%d,%s", "+MQTTPUBLISH", topicName, message->payloadlen, tmpmess);
        outstr[((strlen(outstr) < (length - 1)) ? strlen(outstr) : (length - 1))] = '\0';
        atCmdRespInfoText(g_atCmdEngine, outstr);
    }

    if (NULL != message)
    {
        MQTTAsync_freeMessage(&message);
    }

    if (NULL != topicName)
    {
        MQTTAsync_free(topicName);
    }

    if (NULL != tmpmess)
    {
        free(tmpmess);
        tmpmess = NULL;
    }
    if (NULL != outstr)
    {
        free(outstr);
        outstr = NULL;
    }

    return 1;
}

//Connection Lost Callback
static void _ConnectionLostCb(void *context, char *cause)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Connection lost!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
    AtMqttPostEvent(MQTT_CONN_RSP_LOST, 0x0);

    return;
}

//Connect
static void onConnect(void *context, MQTTAsync_successData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Connection success!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
    AtMqttPostEvent(MQTT_CONN_RSP_SUCC, 0x0);

    return;
}

static void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Connection fail!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
    AtMqttPostEvent(MQTT_CONN_RSP_FAIL, 0x0);

    return;
}

static int MqttConnect(MQTTAsync client)
{
    MQTTAsync_connectOptions ConnOpts = MQTTAsync_connectOptions_initializer;
    int rc = 0;

    ConnOpts.onSuccess = onConnect;
    ConnOpts.onFailure = onConnectFailure;
    ConnOpts.cleansession = g_stMqttOpts.m_iCleanSession;

    ConnOpts.keepAliveInterval = g_stMqttOpts.m_iKeepalive;
    ConnOpts.username = g_stMqttOpts.m_pcUsername;
    ConnOpts.password = g_stMqttOpts.m_pcPassword;
    ConnOpts.MQTTVersion = g_stMqttOpts.m_eMQTTVersion;
    ConnOpts.context = client;
    ConnOpts.automaticReconnect = 0;

    rc = MQTTAsync_connect(client, &ConnOpts);

    return rc;
}

void AT_GPRS_CmdFunc_MQTTCONN(atCommand_t *pParam)
{
    bool bParamRet = true;
    char cDebugBuf[350] = {'\0'};

    char *pcHost = NULL;
    int iPort = MQTT_PORT_DEF;
    char cPort[6] = {'\0'};
    char cUrl[MQTT_HOSTNAME_LEN_MAX + 6] = {'\0'};
    char *pcClientId = NULL;
    int iKeepLive = MQTT_KEEPLIVE_DEF;
    int iCleanSession = MQTT_CLEANSESSION_DEF;
    char *pcUserName = NULL;
    char *pcPassword = NULL;

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (NULL == netif_default)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Net is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (CONNECTING == g_stMqttOpts.m_eState || CONNECTED == g_stMqttOpts.m_eState)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        MqttOptsInit();
        g_stMqttOpts.m_eState = CONNECTING;

        if ((5 != pParam->param_count) && (7 != pParam->param_count))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param. count error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        pcHost = (char *)atParamStr(pParam->params[0], &bParamRet);
        if ((false == bParamRet) || (strlen(pcHost) > MQTT_HOSTNAME_LEN_MAX - 1))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[0] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }
        iPort = atParamUintInRange(pParam->params[1], MQTT_PORT_MIN, MQTT_PORT_MAX, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[1] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }
        cPort[0] = '\0';
        sprintf(cPort, "%d", iPort);
        pcClientId = (char *)atParamStr(pParam->params[2], &bParamRet);
        if ((false == bParamRet) || (strlen(pcClientId) > MQTT_CLIENTID_LEN_MAX - 1))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[2] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        iKeepLive = atParamUintInRange(pParam->params[3], MQTT_KEEPLIVE_MIN, MQTT_KEEPLIVE_MAX, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[3] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        iCleanSession = atParamUintInRange(pParam->params[4], MQTT_CLEANSESSION_MIN, MQTT_CLEANSESSION_MAX, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[4] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        if (7 == pParam->param_count)
        {
            pcUserName = (char *)atParamStr(pParam->params[5], &bParamRet);
            if ((false == bParamRet) || (strlen(pcUserName) > MQTT_HOSTNAME_LEN_MAX - 1))
            {
                cDebugBuf[0] = '\0';
                sprintf(cDebugBuf, "%s%s(%d):Param[5] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
                OSI_LOGI(0, cDebugBuf);
                goto ParamInvalid;
            }

            pcPassword = (char *)atParamStr(pParam->params[6], &bParamRet);
            if ((false == bParamRet) || (strlen(pcPassword) > MQTT_PASSWORD_LEN_MAX - 1))
            {
                cDebugBuf[0] = '\0';
                sprintf(cDebugBuf, "%s%s(%d):Param[6] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
                OSI_LOGI(0, cDebugBuf);
                goto ParamInvalid;
            }
        }

        g_stMqttOpts.m_eMQTTVersion = MQTTVERSION_3_1_1;
        g_stMqttOpts.m_pcHost = pcHost;
        g_stMqttOpts.m_pcPort = cPort;
        g_stMqttOpts.m_iCleanSession = iCleanSession;
        g_stMqttOpts.m_pcUsername = pcUserName;
        g_stMqttOpts.m_pcPassword = pcPassword;
        g_stMqttOpts.m_pcClientid = pcClientId;
        g_stMqttOpts.m_iKeepalive = iKeepLive;
        sprintf(cUrl, "%s:%s", g_stMqttOpts.m_pcHost, g_stMqttOpts.m_pcPort);

        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s%s(%d):Url=%s.", MQTT_DEBUG_HEAD, __func__, __LINE__, cUrl);
        OSI_LOGI(0, cDebugBuf);
        if (NULL != g_stMqttOpts.m_pcUsername && NULL != g_stMqttOpts.m_pcPassword)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):UserName=%s, Password=%s.", MQTT_DEBUG_HEAD, __func__, __LINE__, g_stMqttOpts.m_pcUsername, g_stMqttOpts.m_pcPassword);
            OSI_LOGI(0, cDebugBuf);
        }

        MQTTAsync_init();
        MQTTAsync_create(&g_stMqttClient, cUrl, g_stMqttOpts.m_pcClientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTAsync_setCallbacks(g_stMqttClient, g_stMqttClient, _ConnectionLostCb, _MessageArrivedCb, NULL);
        MqttConnect(g_stMqttClient);

        goto Success;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s%s(%d):Not support command.", MQTT_DEBUG_HEAD, __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        goto NotSurropt;
    }
    }

Success:
    g_atCmdEngine = pParam->engine;
    return;
ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
Fail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
NotSurropt:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

//Subscribe & Unsubscribe
static void onSubscribe(void *context, MQTTAsync_successData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Subscribe success!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Subscribe fail!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static int MqttSubscribe(MQTTAsync client)
{
    MQTTAsync_responseOptions SubRespOpts = MQTTAsync_responseOptions_initializer;
    int rc;

    SubRespOpts.onSuccess = onSubscribe;
    SubRespOpts.onFailure = onSubscribeFailure;
    SubRespOpts.context = client;
    rc = MQTTAsync_subscribe(client, g_stMqttOpts.m_pcTopic, g_stMqttOpts.m_iQos, &SubRespOpts);

    return rc;
}

static void onUnsubscribe(void *context, MQTTAsync_successData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Unsubscribe success!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static void onUnsubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Unsubscribe fail!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static int MqttUnsubscribe(MQTTAsync client)
{
    MQTTAsync_responseOptions UnsubRespOpts = MQTTAsync_responseOptions_initializer;
    int rc;

    UnsubRespOpts.onSuccess = onUnsubscribe;
    UnsubRespOpts.onFailure = onUnsubscribeFailure;
    UnsubRespOpts.context = client;
    rc = MQTTAsync_unsubscribe(client, g_stMqttOpts.m_pcTopic, &UnsubRespOpts);

    return rc;
}

void AT_GPRS_CmdFunc_MQTT_SUB_UNSUB(atCommand_t *pParam)
{
    bool bParamRet = true;
    char cDebugBuf[256] = {'\0'};

    char *pcTopic = NULL;
    unsigned char ucSubFlag = MQTT_SUB_FLAG_DEF;
    int iQos = MQTT_QOS_DEF;

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (NULL == netif_default)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Net is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (CONNECTING == g_stMqttOpts.m_eState || DISCONNECTED == g_stMqttOpts.m_eState)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (pParam->engine != g_atCmdEngine)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Don't support multiple instance.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (1 != MQTTAsync_isConnected(g_stMqttClient))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (pParam->param_count < 2 || pParam->param_count > 3)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param. count error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        pcTopic = (char *)atParamStr(pParam->params[0], &bParamRet);
        if ((false == bParamRet) || (strlen(pcTopic) > MQTT_TOPIC_LEN_MAX - 1))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[0] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        ucSubFlag = atParamUintInRange(pParam->params[1], MQTT_UNSUB_FLAG, MQTT_SUB_FLAG, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[1] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        if (MQTT_UNSUB_FLAG == ucSubFlag && 2 == pParam->param_count)
        {
            g_stMqttOpts.m_pcTopic = pcTopic;
            MqttUnsubscribe(g_stMqttClient);
        }
        else if (MQTT_SUB_FLAG == ucSubFlag && 3 == pParam->param_count)
        {
            iQos = atParamUintInRange(pParam->params[2], MQTT_QOS_MIN, MQTT_QOS_MAX, &bParamRet);
            if (false == bParamRet)
            {
                cDebugBuf[0] = '\0';
                sprintf(cDebugBuf, "%s%s(%d):Param[2] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
                OSI_LOGI(0, cDebugBuf);
                goto ParamInvalid;
            }
            g_stMqttOpts.m_pcTopic = pcTopic;
            g_stMqttOpts.m_iQos = iQos;
            MqttSubscribe(g_stMqttClient);
        }
        else
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param. count error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        goto Success;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s%s(%d):Not support command.", MQTT_DEBUG_HEAD, __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        goto NotSurropt;
    }
    }

Success:
    RETURN_OK(pParam->engine);
ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
Fail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
NotSurropt:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

//Publish
static void onPublish(void *context, MQTTAsync_successData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Publish success!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static void onPublishFailure(void *context, MQTTAsync_failureData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Publish fail!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);

    return;
}

static int MqttPublish(MQTTAsync client, int datalen, char *data)
{
    MQTTAsync_responseOptions PubRespOpts = MQTTAsync_responseOptions_initializer;
    int rc;

    PubRespOpts.onSuccess = onPublish;
    PubRespOpts.onFailure = onPublishFailure;
    PubRespOpts.context = client;
    rc = MQTTAsync_send(client, g_stMqttOpts.m_pcTopic, datalen, data, g_stMqttOpts.m_iQos, g_stMqttOpts.m_iRetained, &PubRespOpts);

    return rc;
}

void AT_GPRS_CmdFunc_MQTTPUB(atCommand_t *pParam)
{
    bool bParamRet = true;
    char cDebugBuf[256] = {'\0'};

    char *pcTopic = NULL;
    char *pcMessage = NULL;
    int iQos = MQTT_QOS_DEF;
    int iDup = MQTT_DUP_DEF;
    int iRetain = MQTT_RETAIN_DEF;

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        if (NULL == netif_default)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Net is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (CONNECTING == g_stMqttOpts.m_eState || DISCONNECTED == g_stMqttOpts.m_eState)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (pParam->engine != g_atCmdEngine)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Don't support multiple instance.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (1 != MQTTAsync_isConnected(g_stMqttClient))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (5 != pParam->param_count)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param. count error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        pcTopic = (char *)atParamStr(pParam->params[0], &bParamRet);
        if ((false == bParamRet) || (strlen(pcTopic) > MQTT_TOPIC_LEN_MAX - 1))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[0] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        pcMessage = (char *)atParamStr(pParam->params[1], &bParamRet);
        if ((false == bParamRet) || (strlen(pcMessage) > MQTT_MESSAGE_LEN_MAX - 1))
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[1] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        iQos = atParamUintInRange(pParam->params[2], MQTT_QOS_MIN, MQTT_QOS_MAX, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[2] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        iDup = atParamUintInRange(pParam->params[3], MQTT_DUP_MIN, MQTT_DUP_MAX, &bParamRet);
        if (false == bParamRet)
        {
            iDup = iDup;
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[3] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        iRetain = atParamUintInRange(pParam->params[4], MQTT_RETAIN_MIN, MQTT_RETAIN_MAX, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Param[4] error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto ParamInvalid;
        }

        g_stMqttOpts.m_pcTopic = pcTopic;
        g_stMqttOpts.m_pcMessage = pcMessage;
        g_stMqttOpts.m_iQos = iQos;
        g_stMqttOpts.m_iRetained = iRetain;

        MqttPublish(g_stMqttClient, strlen(g_stMqttOpts.m_pcMessage), g_stMqttOpts.m_pcMessage);
        goto Success;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s%s(%d):Not support command.", MQTT_DEBUG_HEAD, __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        goto NotSurropt;
    }
    }

Success:
    RETURN_OK(pParam->engine);
ParamInvalid:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
Fail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
NotSurropt:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

//Disconnect
static void onDisconnect(void *context, MQTTAsync_successData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Disconnection success!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
    AtMqttPostEvent(MQTT_DISCONN_RSP_SUCC, 0x0);

    return;
}

static void onDisconnectFailure(void *context, MQTTAsync_failureData *response)
{
    char cOutstr[64] = {0};

    cOutstr[0] = '\0';
    sprintf(cOutstr, "%s%s(%d):Disconnection fail!", MQTT_DEBUG_HEAD, __func__, __LINE__);
    OSI_LOGI(0, cOutstr);
    AtMqttPostEvent(MQTT_DISCONN_RSP_FAIL, 0x0);

    return;
}

static int MqttDisconnect(MQTTAsync client)
{
    MQTTAsync_disconnectOptions DisconncOpts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    DisconncOpts.onSuccess = onDisconnect;
    DisconncOpts.onFailure = onDisconnectFailure;
    DisconncOpts.context = client;
    rc = MQTTAsync_disconnect(client, &DisconncOpts);

    return rc;
}

void AT_GPRS_CmdFunc_MQTTDISCONN(atCommand_t *pParam)
{
    char cDebugBuf[256] = {'\0'};

    switch (pParam->type)
    {
    case AT_CMD_EXE:
    {
        if (NULL == netif_default)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Net is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (CONNECTING == g_stMqttOpts.m_eState || DISCONNECTED == g_stMqttOpts.m_eState)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Mqtt connection state is error.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        if (pParam->engine != g_atCmdEngine)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s%s(%d):Don't support multiple instance.", MQTT_DEBUG_HEAD, __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            goto Fail;
        }

        MqttDisconnect(g_stMqttClient);
        goto Success;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s%s(%d):Not support command.", MQTT_DEBUG_HEAD, __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        goto NotSurropt;
    }
    }

Success:
    RETURN_OK(pParam->engine);
Fail:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
NotSurropt:
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
}

#endif
