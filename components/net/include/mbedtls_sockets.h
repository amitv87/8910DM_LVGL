
#include <cfw.h>
#include "sockets.h"
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/netdb.h"

//#include "netif.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SSL

#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"
#include "mbedtls/debug.h"

#define DATA_OUTPUT_RINGBUF_SIZE 1024

#define TLSSOCK_INVALID_HANDLE 0 //-1

#define TLSSOCK_HANDLE uint32_t

#define MBEDSOCK_TASK_PRIORITY OSI_PRIORITY_NORMAL //(COS_MMI_TASKS_PRIORITY_BASE + 2)

#define TLSSOCK_ERR_SOCK_CONNECT_FAIL -1
#define TLSSOCK_ERR_SSL_CERTIFICATE_VERIFY_FAIL -2

#define IP_ADDR uint32_t
typedef osiThread_t *HANDLE;
#define COS_CALLBACK_FUNC_T osiCallback_t
#define FALSE 0
#define TRUE 1
#define COS_CALLBACK_FUNC_T osiCallback_t

typedef enum
{
    TLSSOCK_STATE_CREATED,
    TLSSOCK_STATE_CONNECTING,
    TLSSOCK_STATE_CONNECTED,
} TLSSOCK_STATE_NUM;

typedef enum
{
    EV_CFW_TLSSOCK_CREATE_REQ = 0x1000,
    EV_CFW_TLSSOCK_CONNECT_REQ,
    EV_CFW_TLSSOCK_FREE_REQ,
    EV_CFW_TLSSOCK_INTERNAL_READ_REQ
} CFW_TLSSOCK_REQ;

typedef enum
{
    EV_CFW_TLSSOCK_CONNECT_EVENT_IND,
    EV_CFW_TLSSOCK_CLOSE_EVENT_IND,
    EV_CFW_TLSSOCK_DATA_SENT_EVENT_IND,
    EV_CFW_TLSSOCK_READ_EVENT_IND,
} SOCK_TYPE;

typedef enum
{
    TLSSOCK_CFG_TYPE_VERSION,
    TLSSOCK_CFG_TYPE_AUTHMODE,
    TLSSOCK_CFG_TYPE_CIPHER_SUITE,
    TLSSOCK_CFG_TYPE_CA,
    TLSSOCK_CFG_TYPE_CLI_CERT,
    TLSSOCK_CFG_TYPE_CLI_KEY,
    TLSSOCK_CFG_TYPE_IGNORE_RTC_TIME,
    TLSSOCK_CFG_TYPE_HTTPS,
    TLSSOCK_CFG_TYPE_HTTPS_CTXI,
    TLSSOCK_CFG_TYPE_SMTPS_TYPE,
    TLSSOCK_CFG_TYPE_SMTPS_CTXI,
} TLSSOCK_CFG_TYPE_E;

#define TLSSOCK_AUTH_MODE_NONE 0
#define TLSSOCK_AUTH_MODE_SERVER_OPTIONAL 1
#define TLSSOCK_AUTH_MODE_SERVER_REQUIRED 2

#define TLSSOCK_INVALID_CID 0xFF
struct data_buf_t
{
    uint32_t put;
    uint32_t get;
    uint8_t buf[DATA_OUTPUT_RINGBUF_SIZE];
};

typedef struct _TLSSOCK_CRT_T
{
    char *crt;
    int32_t len;
} TLSSOCK_CRT_T;

typedef struct _TLSSOCK_CONTEXT
{
    int32_t is_used;
    TLSSOCK_STATE_NUM state;
    char *ip;
    char *port;
    uint8_t nCid;
    uint8_t nSimid;
    uint16_t fport;
    uint32_t fip;
    uint32_t rx_data_len;
    int32_t report_flag;
    HANDLE notify_task;
    COS_CALLBACK_FUNC_T notify_callback;
    void *funcParam;
    mbedtls_ssl_context ssl;
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    int32_t sock_id;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    struct data_buf_t rx_buf;
} TLSSOCK_CONTEXT;

void mbedtlsSocket_Init(void);

TLSSOCK_HANDLE mbedtlsSocket_Create(HANDLE notify_task, COS_CALLBACK_FUNC_T func, void *funcParam);
int32_t MbedtlsSocket_SetReportFlag(TLSSOCK_HANDLE handle, uint8_t value);
int32_t mbedtlsSocket_Connect(TLSSOCK_HANDLE handle, IP_ADDR ip, uint16_t port);
int32_t mbedtlsSocket_Send(TLSSOCK_HANDLE handle, uint8_t *buf, uint32_t len);
int32_t MbedtlsSocket_Recv(TLSSOCK_HANDLE handle, uint8_t *buf, uint32_t len);
void mbedtlsSocket_Free(TLSSOCK_HANDLE handle);
int32_t mbedtlsSocket_Cfg(TLSSOCK_HANDLE handle, TLSSOCK_CFG_TYPE_E type, uint32_t param);
int32_t mbedtlsSocket_SetSimCid(TLSSOCK_HANDLE handle, uint8_t nSimid, uint8_t nCid);

#endif