/*********************************************************
 *
 * File Name
 *        at_cmd_alic.c
 * Author
 *
 * Date
 *         2020/03/18
 * Descriptions:
 *        Communicate with the aliyun Iot Platform Through MQTT Protocol
 *
 *********************************************************/
#include "ats_config.h"

#ifdef CONFIG_AT_ALIC_SUPPORT
#include "at_command.h"
#include "stdio.h"
#include "cfw.h"
#include "sockets.h"
#include "at_engine.h"
#include "at_response.h"
#include "osi_log.h"
#include "netutils.h"
//#include "../../net/ali_c_sdk/v3.0.1/src/mqtt/mqtt_api.h"
#include "mqtt_api.h"
//#include "../../net/ali_c_sdk/v3.0.1/src/dev_sign/dev_sign_api.h"
#include "dev_sign_api.h"
#include "at_cmd_alic.h"
#include "infra_defs.h"
#include "dynreg_api.h"
#include "mqtt_wrapper.h"
#include "vfs.h"
//#include "infra_compat.h"

#define AT_StrCpy(des, src) strcpy((char *)des, (const char *)src)
#define AT_StrLen(str) strlen((const char *)str)

uint8_t authmode = 0;
static void *s_alic_client;
static uint8_t s_status = AT_ALIC_INIT;
atCmdEngine_t *gAlicEngine = NULL;
iotx_sign_mqtt_t s_default_sign;
static uint8_t region = IOTX_HTTP_REGION_SHANGHAI;
iotx_mqtt_param_t s_alic_param;
static char s_alic_msg_buffer[AT_ALIC_INCOMING_MSG_LEN] = {0};
iotx_dev_meta_info_t meta;
bool is_alic_timeout = false;

#define NV_ALIC_DNAME "/nvm/dm_alic_dname.nv"
#define NV_ALIC_DSECRET "/nvm/dm_alic_dsecret.nv"

osiThread_t *alicThread = NULL;

static bool alic_shutdown = false;
static char curTopicBuf[AT_ALIC_TOPIC_LEN];
static char CurBroadcastTopic[AT_ALIC_TOPIC_LEN]; //broadcast topic format /broadcast/device name/{anything}
enum ALIC_STATE
{
    ALIC_SUB = 1,
    ALIC_PUB,
    ALIC_UNSUB,
    ALIC_NULL,
};

enum ALIC_STATE alic_state = ALIC_NULL;

const char *iotx_alic_ca_crt =
    {

        "-----BEGIN CERTIFICATE-----\r\n"
        "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n"
        "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
        "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n"
        "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
        "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n"
        "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n"
        "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n"
        "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n"
        "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n"
        "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n"
        "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n"
        "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n"
        "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n"
        "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n"
        "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n"
        "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n"
        "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n"
        "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n"
        "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n"
        "-----END CERTIFICATE-----"};

//static void at_alic_start_recv_loop(uint32_t delay);
static void _timeoutalicTimeout(atCommand_t *pParam)
{
    OSI_LOGI(0, "ALICSUB/UNSUB/PUB request timeout!");
    is_alic_timeout = true;
    atCmdRespInfoText(pParam->engine, "ALICTIMEOUT: timeout");
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
}
static int at_alic_auth(char *pkey, char *dname, char *dsecret)
{
    int rest = 0;
    OSI_LOGXI(OSI_LOGPAR_SSS, 0, "at_alic_auth pkey:%s dname:%s, dsecret:%s", pkey, dname, dsecret);
    /* Device AUTH */
    strncpy(meta.product_key, pkey, strlen(pkey));
    strncpy(meta.device_name, dname, strlen(dname));
    strncpy(meta.device_secret, dsecret, strlen(dsecret));
#if 1
    rest = IOT_Sign_MQTT(region, &meta, &s_default_sign);
    if (rest < 0)
    {
        OSI_LOGI(0, "AUTH request failed!");
        return -1;
    }
    // OSI_LOGXI(OSI_LOGPAR_SII, 0, "at_alic_auth psecret:%s  s_status: %d regision: %d", s_status, region);
#endif
    memset(curTopicBuf, 0, AT_ALIC_TOPIC_LEN);
    sprintf(curTopicBuf, "/%s/%s", meta.product_key, meta.device_name);
    memset(CurBroadcastTopic, 0, AT_ALIC_TOPIC_LEN);
    sprintf(CurBroadcastTopic, "/broadcast/%s/", meta.product_key);
    OSI_LOGXI(OSI_LOGPAR_SS, 0, "at_alic_auth curTopicBuf:%s CurBroadcastTopic:%s", curTopicBuf, CurBroadcastTopic);
    s_status = AT_ALIC_AUTH;
    return 0;
}

static void at_alic_urc_at_cb(void *param)
{
    char *msg = (char *)param;
    atCmdRespUrcText(gAlicEngine, msg);
}

static void at_alic_ok_cb(void *param)
{
    OSI_LOGI(0, "at_alic_ok_cb response operation succuss");
    alic_state = ALIC_NULL;
    if (is_alic_timeout)
    {
        OSI_LOGI(0, "at_alic_ok_cb alic timeout had response");
        return;
    }
    RETURN_OK(gAlicEngine);
}

static void at_alic_error_cb(void *param)
{

    OSI_LOGI(0, " at_alic_error_cb response operation error");
    alic_state = ALIC_NULL;
    if (is_alic_timeout)
    {
        OSI_LOGI(0, "at_alic_ok_cb alic timeout had response");
        return;
    }
    RETURN_CME_ERR(gAlicEngine, ERR_AT_CME_EXE_FAIL);
}

static void at_alic_post_urc(char *msg)
{
    osiThreadCallback(atEngineGetThreadId(), at_alic_urc_at_cb, msg);
}

static void at_alic_post_ok(void *param)
{
    osiThreadCallback(atEngineGetThreadId(), at_alic_ok_cb, param);
}

static void at_alic_post_error(void *param)
{
    osiThreadCallback(atEngineGetThreadId(), at_alic_error_cb, param);
}

static void at_alic_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    char topic[AT_ALIC_TOPIC_LEN + 1] = {0};
    strncpy(topic, ptopic_info->ptopic, ptopic_info->topic_len);
    sprintf(s_alic_msg_buffer, "+ALICPUBLISH: %d, %s, %d, %s", ptopic_info->packet_id, topic,
            strlen(ptopic_info->payload), (char *)ptopic_info->payload);
    at_alic_post_urc(s_alic_msg_buffer);
}

void *IOT_ALIC_Construct_Api(iotx_mqtt_param_t *pInitParams)
{
    void *pclient;
    int ret = 0;
    /* post reply doesn't need */
    //int post_reply_need = 1;
    //IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    pclient = wrapper_mqtt_init(pInitParams);

    if (pclient == NULL)
    {
        OSI_LOGI(0, "IOT_ALIC_Construct_Api wrapper_mqtt_init error");
        return NULL;
    }

    ret = wrapper_mqtt_connect(pclient);
    if (ret != 0)
    {
        if (ret != -44)
        {
            OSI_LOGI(0, "IOT_ALIC_Construct_Apiwrapper_mqtt_connect failed");
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }

    //iotx_mqtt_report_funcs(pclient);
    /* Mqtt Connect Callback */
#if 0
    void *callback;
    callback = iotx_event_callback(ITE_MQTT_CONNECT_SUCC);
    if (callback) {
        ((int (*)(void))callback)();
    	}
#endif
    return pclient;
}

static int at_alic_publish(char *topic, char *payload, uint8_t dup, uint8_t qos, uint8_t retain)
{
    int ret;
    iotx_mqtt_topic_info_t topic_msg;
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    OSI_LOGXI(OSI_LOGPAR_SIII, 0, "at_alic_publish() entry topic:%s dup:%d qos:%d retain:%d", topic, dup, qos, retain);
    topic_msg.qos = qos;
    topic_msg.retain = retain;
    topic_msg.dup = dup;
    topic_msg.payload = (void *)payload;
    topic_msg.payload_len = strlen(payload);

    ret = IOT_MQTT_Publish(s_alic_client, topic, &topic_msg);
    if (ret < 0)
    {
        OSI_LOGI(0, "at_alic_publish() fail - ret: %d", ret);
    }
    return ret;
}
#if 0
static int at_alic_default_publish(char *payload, uint8_t dup, uint8_t qos, uint8_t retain)
{
    char topic[AT_ALIC_TOPIC_LEN + 1] = {0};
    at_alic_default_topic("update", topic, sizeof(topic));
    return at_alic_publish(topic, payload, dup, qos, retain);
}
#endif
static void at_alic_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uint32_t packet_id = (uint32_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    bool resp = true;
    int rc = 0;
    OSI_LOGI(0, "at_alic_event_handle msg->event_type =%d\n", msg->event_type);
    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_UNDEF:
        OSI_LOGI(0, "undefined event occur.");
        resp = false;
        break;
    case IOTX_MQTT_EVENT_DISCONNECT:
        OSI_LOGI(0, "MQTT disconnect.");
        resp = false;
        break;

    case IOTX_MQTT_EVENT_RECONNECT:
        OSI_LOGI(0, "MQTT reconnect.");
        resp = false;
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
        OSI_LOGI(0, "subscribe success, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_SUB)
        {
            OSI_LOGI(0, "MQTT subscribe ack duplicate.");
            resp = false;
        }
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
        OSI_LOGI(0, "subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_SUB)
        {
            OSI_LOGI(0, "MQTT subscribe wait ack timeout duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        OSI_LOGI(0, "subscribe nack, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_SUB)
        {
            OSI_LOGI(0, "MQTT subscribe nack duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
        OSI_LOGI(0, "unsubscribe success, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_UNSUB)
        {
            OSI_LOGI(0, "MQTT unsubscribe  ack duplicate.");
            resp = false;
        }
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
        OSI_LOGI(0, "unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_UNSUB)
        {
            OSI_LOGI(0, "MQTT unsubscribe  ack duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
        OSI_LOGI(0, "unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_UNSUB)
        {
            OSI_LOGI(0, "MQTT unsubscribe  Nack duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
        OSI_LOGI(0, "publish success, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_PUB)
        {
            OSI_LOGI(0, "MQTT publish  ack duplicate.");
            resp = false;
        }
        break;

    case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
        OSI_LOGI(0, "publish timeout, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_PUB)
        {
            OSI_LOGI(0, "MQTT publish timeout duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_PUBLISH_NACK:
        OSI_LOGI(0, "publish nack, packet-id=%u", (unsigned int)packet_id);
        if (alic_state != ALIC_PUB)
        {
            OSI_LOGI(0, "MQTT publish nack duplicate.");
            resp = false;
        }
        rc = -1;
        break;

    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        OSI_LOGI(0, "topic message arrived but without any related handle: topic_len:%d,ptopic:%p,payload_len:%d,payload:%p",
                 topic_info->topic_len,
                 topic_info->ptopic,
                 topic_info->payload_len,
                 topic_info->payload);
        resp = false;
        at_alic_message_arrive(pcontext, pclient, msg);
        break;
    case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
        OSI_LOGI(0, "OVERFLOW event occur.");
        resp = false;
        break;
    default:
        OSI_LOGI(0, "Should NOT arrive here.");
        resp = false;
        break;
    }
    if (resp)
    {
        if (rc == 0)
        {
            // response operation succuss
            at_alic_post_ok(NULL);
        }
        else
        {
            // response operation fail
            at_alic_post_error(NULL);
        }
    }
}

static void at_alic_set_conn_param(uint16_t keepalive, uint8_t cleanversion)
{
    memset(&s_alic_param, 0x0, sizeof(s_alic_param));

    s_alic_param.port = s_default_sign.port;
    s_alic_param.host = s_default_sign.hostname;
    s_alic_param.client_id = s_default_sign.clientid;
    s_alic_param.username = s_default_sign.username;
    s_alic_param.password = s_default_sign.password;
    //s_alic_param.pub_key = s_default_sign.pub_key;

    s_alic_param.request_timeout_ms = 2000;
    s_alic_param.clean_session = cleanversion;
    s_alic_param.keepalive_interval_ms = keepalive * 1000;

    s_alic_param.read_buf_size = AT_ALIC_RX_BUFFER_SIZE;
    s_alic_param.write_buf_size = AT_ALIC_RX_BUFFER_SIZE;
    s_alic_param.pub_key = iotx_alic_ca_crt;

    s_alic_param.handle_event.h_fp = at_alic_event_handle;
    s_alic_param.handle_event.pcontext = NULL;
}

static int at_alic_subscribe(char *topic, uint8_t qos)
{
    OSI_LOGI(0, "aat_alic_subscribe qos= %d", qos);
    int ret = IOT_MQTT_Subscribe(s_alic_client, topic, qos, at_alic_message_arrive, NULL);
    return ret;
}
static int at_alic_connect_server(uint16_t keepalive, uint8_t cleanversion)
{
    at_alic_set_conn_param(keepalive, cleanversion);
    s_alic_client = IOT_ALIC_Construct_Api(&s_alic_param); //IOT_MQTT_Constru
    if (s_alic_client == NULL)
    {
        OSI_LOGI(0, "at_alic_connect_server IOT_MQTT_Construct() fail");
        return -1;
    }
    s_status = AT_ALIC_CONNECTED;
    return 0;
}

static void alic_clientd(void *param)
{
    while (!alic_shutdown)
    {
        /*pump function*/
        if (s_alic_client == NULL)
        {
            OSI_LOGI(0, "alic_clientd invalid client");
            return;
        }
        IOT_MQTT_Yield(s_alic_client, 100);
        osiThreadSleepRelaxed(100, OSI_DELAY_MAX);
    }
    alicThread = NULL;
    OSI_LOGI(0, "alic_clientd connectend thread will exit...");
    osiThreadExit();
}
static int dev_sign_ver(char *product_key, char *product_secret, char *device_name)
{
    int rest = 0;
    int fd = 0;
    char devname[IOTX_DEVICE_NAME_LEN + 1] = {0};
    strncpy(meta.product_key, product_key, strlen(product_key));
    strncpy(meta.product_secret, product_secret, strlen(product_secret));
    strncpy(meta.device_name, device_name, strlen(device_name));

    fd = vfs_open(NV_ALIC_DNAME, 2);
    if (fd < 0)
    {
        OSI_LOGI(0, "alic_auth#This file is not exit vfs_open failed fd =%d", fd);
        rest = IOT_Dynamic_Register(region, &meta);
        if (rest < 0)
        {
            OSI_LOGI(0, "alic_auth#IOT_Dynamic_Register error");
            return -1;
        }
        else
        {
            OSI_LOGXI(OSI_LOGPAR_SSS, 0, "AT+ALICAUTH  PK:%s dn:%s DS:%s", meta.product_key, meta.device_name, meta.device_secret);
            rest = vfs_file_write(NV_ALIC_DNAME, meta.device_name, strlen(meta.device_name));
            if (rest < 0)
            {
                OSI_LOGI(0, "alic_auth#vfs_file_write error");
                return -1;
            }
            else
            {
                rest = vfs_file_write(NV_ALIC_DSECRET, meta.device_secret, strlen(meta.device_secret));
                if (rest < 0)
                {
                    OSI_LOGI(0, "alic_auth#vfs_file_write error");
                    return -1;
                }
                else
                {
                    OSI_LOGI(0, "alic_auth#meta.device_secret vfs_file_write success");
                    return 0;
                }
            }
        }
    }
    else
    {

        OSI_LOGI(0, "alic_auth#This file is  exit vfs_open success fd =%d", fd);
        struct stat st = {};
        vfs_fstat(fd, &st);
        int file_size = st.st_size;
        if (file_size <= 0)
            return -1;
        vfs_read(fd, devname, IOTX_DEVICE_NAME_LEN);
        vfs_close(fd);
        if (0 == strncmp(devname, device_name, AT_StrLen(device_name)))
        {
            fd = vfs_open(NV_ALIC_DSECRET, 2);
            if (fd < 0)
            {
                OSI_LOGI(0, "alic_auth#NV_ALIC_DSECRET vfs_open error fd =%d", fd);
                return -1;
            }
            else
            {
                struct stat stt = {};
                vfs_fstat(fd, &stt);
                int file_size = stt.st_size;
                if (file_size <= 0)
                {
                    OSI_LOGI(0, "alic_auth#NV_ALIC_DSECRET file size error");
                    return -1;
                }
                else
                {
                    vfs_read(fd, meta.device_secret, IOTX_DEVICE_SECRET_LEN);
                    vfs_close(fd);
                    return 0;
                }
            }
        }
        else
        {
            vfs_unlink(NV_ALIC_DNAME);
            vfs_unlink(NV_ALIC_DSECRET);
            rest = IOT_Dynamic_Register(region, &meta);
            if (rest < 0)
            {
                OSI_LOGI(0, "alic_auth#IOT_Dynamic_Register error");
                return -1;
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SSS, 0, "AT+ALICAUTH  PK:%s dn:%s DS:%s", meta.product_key, meta.device_name, meta.device_secret);
                rest = vfs_file_write(NV_ALIC_DNAME, meta.device_name, strlen(meta.device_name));
                if (rest < 0)
                {
                    OSI_LOGI(0, "alic_auth#vfs_file_write error");
                    return -1;
                }
                else
                {
                    rest = vfs_file_write(NV_ALIC_DSECRET, meta.device_secret, strlen(meta.device_secret));
                    if (rest < 0)
                    {
                        OSI_LOGI(0, "alic_auth#vfs_file_write error");
                        return -1;
                    }
                    else
                    {
                        OSI_LOGI(0, "alic_auth#meta.device_secret vfs_file_write success");
                        return 0;
                    }
                }
            }
        }
    }
}

void AT_ALIC_CmdFunc_AUTH(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[100] = {
        0x00,
    };
    uint8_t mode = 0;
    // iotx_http_region_types_t sregion = IOTX_HTTP_REGION_SHANGHAI;
    //char *rspstr;
    char *pkey;
    char *psecret;
    char *dname;
    char *dsecret;
    int rest = 0;
    if (NULL == pParam)
    {
        OSI_LOGI(0, "AT+ALICAUTH: pParam is NULL");
        return;
    }
    if (AT_CMD_SET == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT+ALICAUTH net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if ((s_status != AT_ALIC_INIT) && (s_status != AT_ALIC_DISCONNECTED) && (s_status != AT_ALIC_AUTH))
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_AUTH before AUTH, CONNECTION must be disconnect");
            //atCmdRespInfoText(pParam->engine, "ALIC is already Connected, please exe AT+ALICDISCONN first\n");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (pParam->param_count != 4)
        {
            OSI_LOGI(0, "AT+ALICAUTH put param count error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        mode = atParamUintInRange(pParam->params[0], 0, 1, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT+ALICAUTH: AuthMode pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        authmode = mode; //mode 0 just one secret  mode 1 type a secret
#if 0
        sregion = (iotx_http_region_types_t)atParamUintInRange(pParam->params[1], 0, 6, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT+ALICAUTH: AuthMode pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        region = sregion;
        OSI_LOGI(0, "AT+ALICAUTH: region: %d %d", sregion, region);
#endif
        if (authmode)
        {
            // get product key
            pkey = (char *)atParamStr(pParam->params[1], &iResult);
            if (!iResult || AT_StrLen(pkey) > IOTX_PRODUCT_KEY_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get pkey param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get product secret
            psecret = (char *)atParamStr(pParam->params[2], &iResult);
            if (!iResult || AT_StrLen(psecret) > IOTX_PRODUCT_SECRET_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get psecret param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get device name
            dname = (char *)atParamStr(pParam->params[3], &iResult);
            if (!iResult || AT_StrLen(dname) > IOTX_DEVICE_NAME_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get dname param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
            //iotx_dev_meta_info_t meta;

            rest = dev_sign_ver(pkey, psecret, dname);
            if (rest < 0)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to Register");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                dsecret = malloc(IOTX_DEVICE_SECRET_LEN + 1);
                memset(dsecret, 0, IOTX_DEVICE_SECRET_LEN + 1);
                strncpy(dsecret, meta.device_secret, IOTX_DEVICE_SECRET_LEN);
            }
            //add data memory
            //#endif
        }
        else
        {
            // get product key
            pkey = (char *)atParamStr(pParam->params[1], &iResult);
            if (!iResult || AT_StrLen(pkey) > IOTX_PRODUCT_KEY_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get pkey param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get device name
            dname = (char *)atParamStr(pParam->params[2], &iResult);
            if (!iResult || AT_StrLen(dname) > IOTX_DEVICE_NAME_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get dname param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            // get device secret
            dsecret = (char *)atParamStr(pParam->params[3], &iResult);
            if (!iResult || AT_StrLen(dsecret) > IOTX_DEVICE_SECRET_LEN)
            {
                OSI_LOGI(0, "AT+ALICAUTH fail to get dsecret param error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
        }
        if (at_alic_auth(pkey, dname, dsecret) != 0)
        {
            OSI_LOGI(0, "AT+ALICAUTH at_alic_auth fail ");
            if (authmode)
                free(dsecret);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        OSI_LOGI(0, "AT+ALICAUTH at_alic_auth succuss");
        if (authmode)
            free(dsecret);
        RETURN_OK(pParam->engine);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        if (authmode == 0)
        {
            AT_StrCpy(tmpString, "+ALICAUTH: <Auth Mode>,<Product Key>,<Device Name>,<Device Secret>");
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
        else
        {
            AT_StrCpy(tmpString, "+ALICAUTH: <Auth Mode>,<Product Key>,<Product Secret>,<Device Name>");
            atCmdRespInfoText(pParam->engine, tmpString);
            RETURN_OK(pParam->engine);
        }
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_AUTH ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_ALIC_CmdFunc_CONN(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[100] = {
        0x00,
    };
    uint16_t keepAlive;
    uint8_t cleanssion;
    if (NULL == pParam)
    {
        OSI_LOGI(0, "AT+ALICCONN: pParam is NULL");
        return;
    }

    if (AT_CMD_SET == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (s_status == AT_ALIC_INIT || s_status == AT_ALIC_CONNECTED)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN before connect, authenticate must be done");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (pParam->param_count != 2)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN param  count ERROR");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        keepAlive = atParamUintInRange(pParam->params[0], AT_ALIC_KEEPALIVE_MIN, AT_ALIC_KEEPALIVE_MAX, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN: keepAlive pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        cleanssion = atParamUintInRange(pParam->params[1], 0, 1, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN: cleanssion pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (s_default_sign.port == 0) //TLS port=443 else port=1883
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN NO auther register");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (at_alic_connect_server(keepAlive, cleanssion) < 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN at_alic_connect_server fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        gAlicEngine = pParam->engine;
        alic_shutdown = false;
        is_alic_timeout = false;
        alicThread = osiThreadCreate("alic", alic_clientd, NULL, OSI_PRIORITY_NORMAL, 2048 * 8, 0);
        if (alicThread == NULL)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN alicThread fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        RETURN_OK(pParam->engine);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        AT_StrCpy(tmpString, "+ALICCONN: <Keep Alive>,<Clean Session>");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_CONN ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_ALIC_CmdFunc_SUB(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[100] = {
        0x00,
    };
    char *topic;
    uint32_t qos;

    if (NULL == pParam)
    {
        OSI_LOGI(0, "AT+ALICSUB: pParam is NULL");
        return;
    }

    if (AT_CMD_SET == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (s_status != AT_ALIC_CONNECTED)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong status:%d", s_status);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (pParam->param_count != 2)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong param_count:%d", pParam->param_count);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        topic = (char *)atParamStr(pParam->params[0], &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong topic");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (AT_StrLen(topic) > AT_ALIC_TOPIC_LEN)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong topic LENGTH");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (strstr(topic, curTopicBuf) == NULL && strncmp(topic, CurBroadcastTopic, AT_StrLen(CurBroadcastTopic)) != 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong topic format");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        qos = atParamUintInRange(pParam->params[1], 0, 1, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB Wrong qos");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        is_alic_timeout = false;
        if (at_alic_subscribe(topic, qos) < 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB at_alic_subscribe fail ");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        alic_state = ALIC_SUB;
        atCmdSetTimeoutHandler(pParam->engine, 60000, _timeoutalicTimeout);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        AT_StrCpy(tmpString, "+ALICSUB: <Topic>, <QoS>(0,1)");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_SUB ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_ALIC_CmdFunc_UNSUB(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[100] = {
        0x00,
    };
    char *topic;
    if (NULL == pParam)
    {
        OSI_LOGI(0, "AAT_ALIC_CmdFunc_UNSUB: pParam is NULL");
        return;
    }

    if (AT_CMD_SET == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (s_status != AT_ALIC_CONNECTED)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB Wrong status:%d", s_status);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (pParam->param_count != 1)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB Wrong param_count:%d", pParam->param_count);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        topic = (char *)atParamStr(pParam->params[0], &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB Wrong topic");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (AT_StrLen(topic) > AT_ALIC_TOPIC_LEN)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUBWrong topic LENGTH");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (strstr(topic, curTopicBuf) == NULL && strncmp(topic, CurBroadcastTopic, AT_StrLen(CurBroadcastTopic)) != 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB Wrong topic format");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        is_alic_timeout = false;
        if (IOT_MQTT_Unsubscribe(s_alic_client, topic) < 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB IOT_MQTT_Unsubscribe fail");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        alic_state = ALIC_UNSUB;
        atCmdSetTimeoutHandler(pParam->engine, 60000, _timeoutalicTimeout);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        AT_StrCpy(tmpString, "+ALICUNSUB: <Topic>");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_UNSUB ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_ALIC_CmdFunc_PUB(atCommand_t *pParam)
{
    bool iResult = true;
    char tmpString[100] = {
        0x00,
    };
    char *topic = NULL;
    uint8_t qos;
    uint8_t retain = 0;
    uint8_t duplicate = 0;
    char *msg;
    uint8_t index = 0;
    if (NULL == pParam)
    {
        OSI_LOGI(0, "AT+ALICPUB: pParam is NULL");
        return;
    }

    if (AT_CMD_SET == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (s_status != AT_ALIC_CONNECTED)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB before connect, authenticate must be done");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if ((pParam->param_count < 3) || (pParam->param_count > 5))
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB param  count ERROR");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }

        topic = (char *)atParamStr(pParam->params[index++], &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB Wrong topic");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (AT_StrLen(topic) > AT_ALIC_TOPIC_LEN)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB Wrong topic LENGTH");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (strstr(topic, curTopicBuf) == NULL || strstr(topic, "#") != NULL || strstr(topic, "+") != NULL)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB Wrong topic format");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        qos = atParamUintInRange(pParam->params[index++], 0, 1, &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB: qos pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        msg = (char *)atParamStr(pParam->params[index++], &iResult);
        if (!iResult)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB: cleanssion pParam error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (AT_StrLen(msg) > AT_ALIC_OUT_MSG_LEN)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB Wrong MESSAGE LENGTH");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (pParam->param_count >= 4)
        {
            duplicate = atParamDefUintInRange(pParam->params[index++], 0, 0, 1, &iResult);
            if (!iResult)
            {
                OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB: duplicate pParam error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            retain = atParamDefUintInRange(pParam->params[index++], 0, 0, 1, &iResult);
            if (!iResult)
            {
                OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB: retain pParam error");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        is_alic_timeout = false;
        if (at_alic_publish(topic, msg, duplicate, qos, retain) < 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB at_alic_publish");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (!qos)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB at_alic_publish no ack");
            RETURN_OK(pParam->engine);
        }
        alic_state = ALIC_PUB;
        atCmdSetTimeoutHandler(pParam->engine, 60000, _timeoutalicTimeout);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        AT_StrCpy(tmpString, "+ALICPUB: <Topic>,<QoS>,<Message>,[duplicate],[retain]");
        atCmdRespInfoText(pParam->engine, tmpString);
        RETURN_OK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_PUB ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void AT_ALIC_CmdFunc_DISCONN(atCommand_t *pParam)
{
    bool iResult = true;
    int retparam = 0;
    if (NULL == pParam)
    {
        OSI_LOGI(0, "AT+ALICDISCONN: pParam is NULL");
        return;
    }

    if (AT_CMD_EXE == pParam->type)
    {
        if (NULL == netif_default)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_DISCONN net error");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (s_status != AT_ALIC_CONNECTED)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_DISCONN Wrong status:%d", s_status);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (alicThread != NULL)
        {
            alic_shutdown = true;
            while (alicThread != NULL)
                osiThreadSleep(100);
        }
        retparam = IOT_MQTT_Destroy(&s_alic_client);
        if (retparam < 0)
        {
            OSI_LOGI(0, "AT_ALIC_CmdFunc_DISCONN  IOT_MQTT_Destroy iResult:%d", iResult);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        s_alic_client = NULL;
        s_status = AT_ALIC_DISCONNECTED;
        OSI_LOGI(0, "AT_ALIC_CmdFunc_DISCONN  IOT_MQTT_Destroy success");
        RETURN_OK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT_ALIC_CmdFunc_DISCONN ERROR");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

#endif
