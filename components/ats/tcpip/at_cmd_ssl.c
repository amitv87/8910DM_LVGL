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

#include "stdio.h"
#include "cfw.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"
#include "at_cfw.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_cmd_tcpip.h"
#include "at_cfg.h"
#include "sockets.h"
#include "at_cfg.h"
#include "mbedtls_sockets.h"

#define AT_TCPIP_ADDRLEN 20

#define ERR_SUCCESS 0

static TLSSOCK_HANDLE s_sslSocket;
static int g_sslstate = 0;
static int s_sslSendSize = 0;
static int ssl_socketconn = CFW_TCPIP_IPPROTO_TCP;

atCmdEngine_t *useEngine = NULL;

static char *ca_crt = "-----BEGIN CERTIFICATE-----\n\
MIICvzCCAiigAwIBAgIJAOEH30RV037pMA0GCSqGSIb3DQEBCwUAMHYxCzAJBgNV\n\
BAYTAkNOMRAwDgYDVQQIDAd0aWFuamluMQswCQYDVQQHDAJUSjEMMAoGA1UECgwD\n\
UkRBMQwwCgYDVQQLDANSREExDDAKBgNVBAMMA1JEQTEeMBwGCSqGSIb3DQEJARYP\n\
Y2FAcmRhbWljcm8uY29tMCAXDTE4MDkxOTA5MDYxM1oYDzIxMTgwODI2MDkwNjEz\n\
WjB2MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHdGlhbmppbjELMAkGA1UEBwwCVEox\n\
DDAKBgNVBAoMA1JEQTEMMAoGA1UECwwDUkRBMQwwCgYDVQQDDANSREExHjAcBgkq\n\
hkiG9w0BCQEWD2NhQHJkYW1pY3JvLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAw\n\
gYkCgYEAviKxja+DM/W7JCwf3QK8mYmlF3a4YABgMEwAlW4S/ulZ50HGBBzEMHj7\n\
Q0GANnTGSJFaSpbOkjA8TJ5fly/mLffz1GYcgkU2lCiiDckh1WXJOwZsivAogoML\n\
0LmLTN7xHXzV2lN10U+fDM+v3AD1rEJlDVoZ5z4d7SwsSj7qW8kCAwEAAaNTMFEw\n\
HQYDVR0OBBYEFG3UUGWA/6v3YQTZB+CDey0JJ4nXMB8GA1UdIwQYMBaAFG3UUGWA\n\
/6v3YQTZB+CDey0JJ4nXMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQAD\n\
gYEAGyeuyAqK8RgCSwoDJGj358cEURpC0FT/KuH+43XrkUHCJCMhH2zcaeOr6eBG\n\
ws0bYutiSRLnxMb5LP1tha+xRBiruykIuNCWoK3z751rr1xQsjzkl0EV41UjIGoF\n\
pPH8E14NwjLr9vPA68ZkfQmBI8H08yTRsqUJOLYLP/mLwIY=\n\
-----END CERTIFICATE-----";

static char *client_cert = "Certificate:\n\
    Data:\n\
        Version: 3 (0x2)\n\
        Serial Number: 36 (0x24)\n\
    Signature Algorithm: sha256WithRSAEncryption\n\
        Issuer: C=CN, ST=tianjin, L=TJ, O=RDA, OU=RDA, CN=RDA/emailAddress=ca@rdamicro.com\n\
        Validity\n\
            Not Before: Sep 19 00:00:00 2018 GMT\n\
            Not After : Sep 19 00:00:00 2027 GMT\n\
        Subject: C=CN, ST=tianjin, O=RDA, OU=RDA, CN=RDA/emailAddress=client@rdamicro.com\n\
        Subject Public Key Info:\n\
            Public Key Algorithm: rsaEncryption\n\
                Public-Key: (1024 bit)\n\
                Modulus:\n\
                    00:d6:ba:00:f0:aa:5b:22:ff:42:0f:9c:86:53:be:\n\
                    4f:5c:86:c4:53:15:f3:f0:22:36:a2:1c:0f:97:4b:\n\
                    5c:35:28:cc:0b:e7:08:c8:87:7a:3d:f9:a5:5a:12:\n\
                    44:7f:c4:8b:11:ce:94:fb:78:a7:8c:48:d7:75:0e:\n\
                    8a:98:53:cf:87:1f:9b:d9:b5:f4:1e:22:e7:64:43:\n\
                    da:04:86:2d:62:25:93:75:bf:cc:2a:89:07:db:69:\n\
                    1c:6b:7f:e4:e1:42:44:63:48:c9:ac:ab:f5:f2:8c:\n\
                    f1:c4:03:3c:b0:2e:c1:61:a7:44:12:76:98:3e:d9:\n\
                    3d:97:9c:ee:46:bf:8a:99:4f\n\
                Exponent: 65537 (0x10001)\n\
        X509v3 extensions:\n\
            X509v3 Basic Constraints:\n\
                CA:FALSE\n\
            Netscape Comment:\n\
                OpenSSL Generated Certificate\n\
            X509v3 Subject Key Identifier:\n\
                A4:DF:A9:EC:82:49:AF:00:1C:98:E6:57:21:AF:CE:D5:24:35:0F:26\n\
            X509v3 Authority Key Identifier:\n\
                keyid:6D:D4:50:65:80:FF:AB:F7:61:04:D9:07:E0:83:7B:2D:09:27:89:D7\n\
    \n\
    Signature Algorithm: sha256WithRSAEncryption\n\
         7e:ea:bb:db:93:f2:5e:5d:0d:d8:7e:16:ed:78:32:11:56:61:\n\
         65:ab:f2:b8:63:bb:19:25:a8:64:27:33:49:a3:90:5c:59:75:\n\
         e5:0e:1e:ae:65:57:40:ac:5b:97:98:8a:5e:2f:0a:a8:81:63:\n\
         20:0b:eb:a1:79:20:f0:aa:09:20:c4:0c:b6:0e:a3:84:ec:29:\n\
         3b:1d:8e:97:26:53:e5:a3:b8:83:3e:0c:11:34:ed:13:13:3e:\n\
         24:a1:25:54:bd:3e:5d:3a:ff:85:7e:62:da:a8:14:5b:d5:16:\n\
         f7:60:aa:49:9f:77:97:8d:d4:54:59:74:50:c4:0b:bc:ce:9f:\n\
         ec:69\n\
-----BEGIN CERTIFICATE-----\n\
MIIC1DCCAj2gAwIBAgIBJDANBgkqhkiG9w0BAQsFADB2MQswCQYDVQQGEwJDTjEQ\n\
MA4GA1UECAwHdGlhbmppbjELMAkGA1UEBwwCVEoxDDAKBgNVBAoMA1JEQTEMMAoG\n\
A1UECwwDUkRBMQwwCgYDVQQDDANSREExHjAcBgkqhkiG9w0BCQEWD2NhQHJkYW1p\n\
Y3JvLmNvbTAeFw0xODA5MTkwMDAwMDBaFw0yNzA5MTkwMDAwMDBaMG0xCzAJBgNV\n\
BAYTAkNOMRAwDgYDVQQIDAd0aWFuamluMQwwCgYDVQQKDANSREExDDAKBgNVBAsM\n\
A1JEQTEMMAoGA1UEAwwDUkRBMSIwIAYJKoZIhvcNAQkBFhNjbGllbnRAcmRhbWlj\n\
cm8uY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDWugDwqlsi/0IPnIZT\n\
vk9chsRTFfPwIjaiHA+XS1w1KMwL5wjIh3o9+aVaEkR/xIsRzpT7eKeMSNd1DoqY\n\
U8+HH5vZtfQeIudkQ9oEhi1iJZN1v8wqiQfbaRxrf+ThQkRjSMmsq/XyjPHEAzyw\n\
LsFhp0QSdpg+2T2XnO5Gv4qZTwIDAQABo3sweTAJBgNVHRMEAjAAMCwGCWCGSAGG\n\
+EIBDQQfFh1PcGVuU1NMIEdlbmVyYXRlZCBDZXJ0aWZpY2F0ZTAdBgNVHQ4EFgQU\n\
pN+p7IJJrwAcmOZXIa/O1SQ1DyYwHwYDVR0jBBgwFoAUbdRQZYD/q/dhBNkH4IN7\n\
LQknidcwDQYJKoZIhvcNAQELBQADgYEAfuq725PyXl0N2H4W7XgyEVZhZavyuGO7\n\
GSWoZCczSaOQXFl15Q4ermVXQKxbl5iKXi8KqIFjIAvroXkg8KoJIMQMtg6jhOwp\n\
Ox2OlyZT5aO4gz4METTtExM+JKElVL0+XTr/hX5i2qgUW9UW92CqSZ93l43UVFl0\n\
UMQLvM6f7Gk=\n\
-----END CERTIFICATE-----";

static char *client_key = "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXgIBAAKBgQDWugDwqlsi/0IPnIZTvk9chsRTFfPwIjaiHA+XS1w1KMwL5wjI\n\
h3o9+aVaEkR/xIsRzpT7eKeMSNd1DoqYU8+HH5vZtfQeIudkQ9oEhi1iJZN1v8wq\n\
iQfbaRxrf+ThQkRjSMmsq/XyjPHEAzywLsFhp0QSdpg+2T2XnO5Gv4qZTwIDAQAB\n\
AoGBAIFmGgbuQnm2pdLOmsyAlUbHGCyRwC1oENBkZKjiCzEl4sERe2OM8QfEF/dN\n\
puXwRXZ3raRVs3KAuwaZur/NUfOa3kwGljnb859z5HcsWxb85ts+4TFP8AT2izjj\n\
XGYHBZZhVhD7Fr4rXOaKbzBprjXTgSgtJ6JZaAtgrWhD7kUxAkEA+bNwBKaTXgBK\n\
egN7NRfrL2bTFWKxY+Jse4NTflDaFDtUVrEqjpZlUCyGO5gPLkY4Zb7llgu28+E+\n\
FZQMdh5VRQJBANwks93pDiUmmiyL4f+DP4Bvtc1fgK7NYEMey42/46LlNC9s7YBA\n\
XXozLTdXtUJLliRy1VRK763OtocZ3kkfC4MCQHdNFCgkriQrX2oMX8FuPB/ZsOB2\n\
1uoyNEKO7EVdu9QOxKzm2L5nfOBhZYDzlc02H5v9KRQXZMIAy/jjU1DcDUECQQCQ\n\
0Bf95zwl1iHfIdl1wnm4XEPkqGk3E+S54n0Wbt8oncvZUrddAXo8U4Pv/uM+jAhl\n\
S6DnhS/rTqqlbq/Zu/FPAkEAjl3zMwClAVUEcqwDzLV6Pw4vaOxDtO+cx+XL24K8\n\
zIN9d8BuUWotkY5D/cnuDenQPMERtFJwRjEtiJKqqE3evg==\n\
-----END RSA PRIVATE KEY-----";

static const int ciphersuite_preference[] =
    {
        MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA,
        MBEDTLS_TLS_RSA_WITH_RC4_128_SHA,
        MBEDTLS_TLS_RSA_WITH_RC4_128_MD5,
        MBEDTLS_TLS_RSA_WITH_NULL_SHA};

static void ssl_rsp(void *param)
{
    osiEvent_t *pEvent = (osiEvent_t *)param;
    char uaRspStr[60] = {
        0,
    };
    OSI_LOGI(0, "enterring ssl_rsp");
    OSI_LOGI(0x0, "ssl_rsp Got : 0x%x,0x%x,0x%x,0x%x",
             pEvent->id, pEvent->param1, pEvent->param2, pEvent->param3);
    atCmdEngine_t *engine = (atCmdEngine_t *)pEvent->param3;
    TLSSOCK_HANDLE sslSocket = pEvent->param2;

    switch (pEvent->id)
    {
    case EV_CFW_TLSSOCK_CONNECT_EVENT_IND:
    {
        int32_t errorCode = pEvent->param1;
        if (errorCode == 0)
        {
            MbedtlsSocket_SetReportFlag(sslSocket, 1);
            g_sslstate = 1;
            strcpy(uaRspStr, "SSL CONNECT OK");
            atCmdRespInfoText(engine, uaRspStr);
            atCmdRespOK(engine);
        }
        else if (errorCode < -1)
        {
            OSI_LOGI(0, "SSL HANDSHAKE ERROR :-%lx", -errorCode);
            sprintf(uaRspStr, "SSL CONNECT FAIL");
            atCmdRespErrorText(engine, uaRspStr);
            useEngine = NULL;
            mbedtlsSocket_Free(sslSocket);
        }
        else
        {
            strcpy(uaRspStr, "SSL CONNECT FAIL");
            atCmdRespErrorText(engine, uaRspStr);
            useEngine = NULL;
            mbedtlsSocket_Free(sslSocket);
        }
    }
    break;
    case EV_CFW_TLSSOCK_CLOSE_EVENT_IND:
    {

        strcpy(uaRspStr, "SSL CLOSED");
        g_sslstate = 0;
        mbedtlsSocket_Free(sslSocket);
        atCmdRespUrcText(engine, uaRspStr);
    }
    break;
    case EV_CFW_TLSSOCK_READ_EVENT_IND:
    {
        char *data = NULL;
        data = malloc(DATA_OUTPUT_RINGBUF_SIZE + 1);
        if (data == NULL)
            break;
        memset(data, 0, DATA_OUTPUT_RINGBUF_SIZE + 1);

        uint32_t readlen = 0;
        if (g_sslstate == 0)
        {
            OSI_LOGI(0x0, "ssl_rsp EV_CFW_TLSSOCK_READ_EVENT_IND g_sslstate == 0");
            free(data);
            break;
        }
        readlen = MbedtlsSocket_Recv(sslSocket, (uint8_t *)data, DATA_OUTPUT_RINGBUF_SIZE);
        if (readlen <= 0)
        {
            memset(data, 0, DATA_OUTPUT_RINGBUF_SIZE + 1);
            free(data);
            data = NULL;
            break;
        }

        do
        {
            atCmdRespUrcNText(engine, data, readlen);
            memset(data, 0, DATA_OUTPUT_RINGBUF_SIZE + 1);
            readlen = MbedtlsSocket_Recv(sslSocket, (uint8_t *)data, DATA_OUTPUT_RINGBUF_SIZE);
        } while (readlen != 0);

        memset(data, 0, DATA_OUTPUT_RINGBUF_SIZE + 1);
        free(data);
        data = NULL;
    }
    break;
    case EV_CFW_TLSSOCK_DATA_SENT_EVENT_IND:
    {
        if (s_sslSendSize > 0)
        {
            s_sslSendSize = 0;
            strcpy(uaRspStr, "SSL SEND OK");
            atCmdRespUrcText(engine, uaRspStr);
            //atCmdRespOK(engine);
        }
    }
    break;
    default:
        OSI_LOGI(0x0, "ssl_rsp unexpect asynchrous event/response %d", pEvent->id);
        break;
    }
    free(param);
}

extern int32_t AT_SSL_Connect(uint8_t nDLCI, uint8_t nMuxIndex, uint8_t *ipaddress);
static uint8_t getActivedPdp(struct atCmdEngine *engine)
{
    uint8_t nSim = atCmdGetSim(engine);
    ;
    uint8_t m_uAttState = CFW_GPRS_DETACHED;
    uint8_t uPDPState = 0;

    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS || 0 == m_uAttState)
    {
        OSI_LOGI(0x10003f56, "useExistPdp, not attached\n");
        goto error;
    }
    uint8_t i = 1;
    for (i = 1; i < 8; i++)
    {
        if (CFW_GetGprsActState(i, &uPDPState, nSim) != ERR_SUCCESS || uPDPState == CFW_GPRS_DEACTIVED)
        {
            OSI_LOGI(0x10003f57, "useExistPdp, not actived\n");
            continue;
        }
        break;
    }
    if (i >= 8)
        goto error;
    return i;
error:
    return TLSSOCK_INVALID_CID;
}

void AT_TCPIP_CmdFunc_SSLSTART(atCommand_t *pParam)
{

    OSI_LOGI(0, "AT+SSLSTART ... ... ");

    if (AT_CMD_SET == pParam->type)
    {
        bool paramok = true;
        uint32_t uPort = 0;
        char aucBuffer[40];
        uint32_t nAddr = 0;
        int32_t iResult = 0;
        uint8_t nSim = atCmdGetSim(pParam->engine);
        char ipaddress[AT_TCPIP_ADDRLEN] = {0};
        ip_addr_t nIpAddr;

        //const char *mode = atParamStr(pParam->params[0], &paramok);
        char *host = (char *)atParamStr(pParam->params[0], &paramok);
        uPort = atParamUintInRange(pParam->params[1], 0, 65535, &paramok);

        if (!paramok)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        if (useEngine != NULL && useEngine != pParam->engine)
        {
            OSI_LOGI(0x0, "useEngine  %x, cmd->engine %x", useEngine, pParam->engine);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        useEngine = pParam->engine;
        uint8_t nCid = getActivedPdp(pParam->engine);
        if (nCid == TLSSOCK_INVALID_CID)
        {
            OSI_LOGI(0, "CONNECT FAIL PDP error");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        if (g_sslstate == 1)
        {
            OSI_LOGI(0, "state error,g_sslstate=%d", g_sslstate);
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        iResult = CFW_GethostbynameEX(host, &nIpAddr, nCid, nSim, NULL, NULL);
        if (iResult == RESOLV_QUERY_INVALID)
        {
            OSI_LOGI(0, "SSL connect gethost by name error");
            sprintf(aucBuffer, "CONNECT FAIL NDS error");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }
        else if (iResult == RESOLV_COMPLETE)
        {
            OSI_LOGI(0, "AT_SSL_Connect(), CFW_GethostbynameEX() RESOLV_COMPLETE\n");

            char *pcIpAddr = ipaddr_ntoa(&nIpAddr);
            strncpy(ipaddress, pcIpAddr, AT_TCPIP_ADDRLEN);
        }

        nAddr = CFW_TcpipInetAddr(ipaddress);
        if (IPADDR_NONE == nAddr)
        {
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        /*if (strcasecmp(mode, "SSL") != 0)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }*/

        mbedtlsSocket_Init();
        s_sslSocket = mbedtlsSocket_Create((HANDLE)osiThreadCurrent(), ssl_rsp, pParam->engine);
        if (s_sslSocket == TLSSOCK_INVALID_HANDLE)
        {
            sprintf(aucBuffer, "CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }

        iResult = mbedtlsSocket_Cfg(s_sslSocket, TLSSOCK_CFG_TYPE_AUTHMODE, TLSSOCK_AUTH_MODE_SERVER_OPTIONAL);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "CONFIG FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }
        TLSSOCK_CRT_T ca_cert;
        ca_cert.crt = ca_crt;
        ca_cert.len = strlen(ca_crt) + 1;
        OSI_LOGI(0, "ca_cert.len=%d", ca_cert.len);
        iResult = mbedtlsSocket_Cfg(s_sslSocket, TLSSOCK_CFG_TYPE_CA, (uint32_t)&ca_cert);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }

        TLSSOCK_CRT_T cli_cert;
        cli_cert.crt = client_cert;
        cli_cert.len = strlen(client_cert) + 1;
        OSI_LOGI(0, "cli_cert.len=%d", cli_cert.len);
        iResult = mbedtlsSocket_Cfg(s_sslSocket, TLSSOCK_CFG_TYPE_CLI_CERT, (uint32_t)&cli_cert);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "SSL CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }
        TLSSOCK_CRT_T cli_key;
        cli_key.crt = client_key;
        cli_key.len = strlen(client_key) + 1;
        OSI_LOGI(0, "cli_key.len=%d", cli_key.len);
        iResult = mbedtlsSocket_Cfg(s_sslSocket, TLSSOCK_CFG_TYPE_CLI_KEY, (uint32_t)&cli_key);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "SSL CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }

        iResult = mbedtlsSocket_Cfg(s_sslSocket, TLSSOCK_CFG_TYPE_CIPHER_SUITE, (uint32_t)ciphersuite_preference);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "SSL CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }

        mbedtlsSocket_SetSimCid(s_sslSocket, nSim, nCid);
        iResult = mbedtlsSocket_Connect(s_sslSocket, nAddr, uPort);
        if (iResult != 0)
        {
            sprintf(aucBuffer, "SSL CONNECT FAIL");
            useEngine = NULL;
            AT_CMD_RETURN(atCmdRespErrorText(pParam->engine, aucBuffer));
        }
        AT_SetAsyncTimerMux(pParam->engine, 120);
    }
    else if (AT_CMD_TEST == pParam->type)
    {
        char *pRstStr = malloc(85);
        if (NULL == pRstStr)
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_NO_MEMORY));

        memset(pRstStr, 0, 85);
        strcpy(pRstStr, "+SSLSTART:(\"(0-255).(0-255).(0-255).(0-255)\"), (0-65535)");

        atCmdRespInfoText(pParam->engine, pRstStr);
        atCmdRespOK(pParam->engine);
        free(pRstStr);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_SSLSTART(), command type not allowed");
        AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

#define TCPIP_DATA_MAX_SIZE (10 * 1024)

typedef struct _tcpipAsyncCtx_t
{
    uint32_t uMuxIndex;
    uint32_t pos;
    uint32_t size;
    uint8_t *data;
} tcpipAsyncCtx_t;

static void tcpipAsyncCtxDestroy(atCommand_t *cmd)
{
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;
    if (async == NULL)
        return;
    if (async->data != NULL)
        free(async->data);
    free(async);
    cmd->async_ctx = NULL;
}

static void _timeoutabortTimeout(atCommand_t *cmd)
{
    OSI_LOGI(0, "_timeoutabortTimeout");
    AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, "COMMAND NOT RESPONSE"));
}

static void _tcpipSend(atCommand_t *cmd, uint8_t *send_data, uint32_t uLength)
{
    //stAT_Tcpip_Paras *tcpipParas = &(g_uCipContexts.nTcpipParas[uMuxIndex]);
    int32_t iResult = 0;

    if (ssl_socketconn != CFW_TCPIP_IPPROTO_UDP)
    {
        iResult = mbedtlsSocket_Send(s_sslSocket, (uint8_t *)send_data, (uint16_t)uLength);
        char aucBuffer[40] = {0};
        if (iResult == SOCKET_ERROR)
        {
            sprintf(aucBuffer, "SEND FAIL");
            AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
        }
    }
    else
    {
#if 0
        CFW_TCPIP_SOCKET_ADDR nDestAddr;
        nDestAddr.sin_family = CFW_TCPIP_AF_INET;
        nDestAddr.sin_port = htons(tcpipParas->uPort);
        nDestAddr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
        iResult = CFW_TcpipInetAddr((const char *)tcpipParas->uaIPAddress);
        if (SOCKET_ERROR == iResult)
        {
            OSI_LOGI(0x10003f7c, "AT_TCPIP_CmdFunc_CIPSEND CFW_TcpipInetAddr() failed\n");
        }
        else
        {
            nDestAddr.sin_addr.s_addr = iResult;
            OSI_LOGI(0x10003f7d, "dest_addr.sin_addr.s_addr send 0x%x \n", nDestAddr.sin_addr.s_addr);
            iResult = CFW_TcpipSocketSendto(tcpipParas->uSocket, (uint8_t *)send_data, (uint16_t_t)uLength, 0, &nDestAddr, nDestAddr.sin_len);
            if (iResult == SOCKET_ERROR)
            {

                OSI_LOGI(0x10003f7e, "UDP send socket data error");
            }
        }
#endif
    }

    atCmdSetTimeoutHandler(cmd->engine, 120000, _timeoutabortTimeout);

    char aucBuffer[40] = {0};
    if (iResult != SOCKET_ERROR)
    {
        s_sslSendSize = iResult;
        AT_CMD_RETURN(atCmdRespOKText(cmd->engine, ""));
    }
    else
    {
        sprintf(aucBuffer, "SEND FAIL");

        AT_CMD_RETURN(atCmdRespErrorText(cmd->engine, aucBuffer));
    }
}

static void _tcpipDataPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)cmd->async_ctx;
    OSI_LOGI(0, "AT CIPSEND111 send size/%d", size);
    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_PARAM_INVALID));

    OSI_LOGI(0, "AT CIPSEND send size/%d", size);
    if (end_mode == AT_PROMPT_END_CTRL_Z && 1 == size)
        AT_CMD_RETURN(atCmdRespCmsError(cmd->engine, ERR_AT_CMS_INVALID_PARA));
    _tcpipSend(cmd, async->data, size - 1);
}

void AT_TCPIP_CmdFunc_SSLSEND(atCommand_t *pParam) //atCommand_t *cmd
{
    OSI_LOGI(0x0, "AT+SSLSEND: enter");

    if (AT_CMD_EXE == pParam->type)
    {
        if (pParam->param_count != 0)
        {
            OSI_LOGI(0, "AT+SSLSEND: pPara number is error");
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        if (useEngine != pParam->engine)
        {
            OSI_LOGI(0x0, "useEngine  %x, cmd->engine %x", useEngine, pParam->engine);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (g_sslstate != 1)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        tcpipAsyncCtx_t *async = (tcpipAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        memset(async, 0, sizeof(*async));
        pParam->async_ctx = async;
        pParam->async_ctx_destroy = tcpipAsyncCtxDestroy;
        async->data = (uint8_t *)malloc(TCPIP_DATA_MAX_SIZE);
        if (async->data == NULL)
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_NO_MEMORY);
        async->uMuxIndex = 0;
        atCmdRespOutputPrompt(pParam->engine);
        atCmdSetPromptMode(pParam->engine, _tcpipDataPromptCB, pParam, async->data, TCPIP_DATA_MAX_SIZE);
    }
    else
    {
        OSI_LOGI(0, "AT_TCPIP_CmdFunc_SSLSEND(), command type not allowed");
        AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}
void AT_TCPIP_CmdFunc_SSLCLOSE(atCommand_t *pParam)
{
    OSI_LOGI(0, "SSLCLOSE processing\n");

    if (pParam->type == AT_CMD_EXE)
    {
        if (useEngine != NULL && atCmdEngineIsValid(useEngine) && useEngine != pParam->engine)
        {
            OSI_LOGI(0x0, "useEngine  %x, cmd->engine %x", useEngine, pParam->engine);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (g_sslstate == 0)
        {
            AT_CMD_RETURN(atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }
        mbedtlsSocket_Free(s_sslSocket);
        g_sslstate = 0;
        char aucBuffer[40] = {0};
        strcpy(aucBuffer, "SSL CLOSE OK");
        useEngine = NULL;
        atCmdRespOKText(pParam->engine, aucBuffer);
    }
    else if (pParam->type == AT_CMD_TEST)
    {
        atCmdRespOK(pParam->engine);
    }
    else
    {
        OSI_LOGI(0, "AT+SSLCLOSE: command not supported");
        atCmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
