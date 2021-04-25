#if 0

#include "coap_async_api.h"

#include "cJSON.h"
#include "string.h"
#include "sockets.h"
#include "netutils.h"
#include "mbedtls/base64.h"

typedef void (*coap_selfreg_data_cb_t)(unsigned int uti, char *code, int len);
typedef struct
{
    coap_status_t status;
    int simId;
    unsigned char resultCode;
    unsigned char retryCount;
    char resultDesc[12];
    coap_selfreg_data_cb_t cb;
    unsigned int uti;
} reg_ctrl_t;

static reg_ctrl_t gRegCtrl;
osiTimer_t *g_pstRetryTimer = NULL;

static char *genRegData(int simid);
static void parseJson(char *pMsg, reg_ctrl_t *reg_ctrl);
static bool isResultOk(reg_ctrl_t *reg_ctrl);
static void coap_user_cb(coap_client_t *client, coap_method_t method, bool result);
static void coap_incoming_data_cb(coap_client_t *client, const unsigned char *data, short unsigned int len, unsigned char num);
static void do_regProcess(void *param);
static int unicomIotSelfRegister(void *param);
static void doSelfReg(void *param);

static char *genRegData(int simid)
{
    char *buffer;
    size_t buffer_len;
    size_t len;
    size_t out_len;
    cJSON *pJsonRoot = NULL;
    pJsonRoot = cJSON_CreateObject();
    if (NULL == pJsonRoot)
        return NULL;

    char imei[16] = {0};
    char imsi[16] = {0};
    //char simiccid[20/*UEICCID_LEN*/+1] = {0};
    unsigned char imeisize = 16;
    unsigned char imsisize = 16;
    //unsigned char iccid_len = 20/*UEICCID_LEN*/;
    //getSimIccid(simid,simiccid,&iccid_len);
    getSimImei(simid, (unsigned char *)imei, &imeisize);
    getSimImsi(simid, (unsigned char *)imsi, &imsisize);

    char *regver = "1";
    cJSON_AddStringToObject(pJsonRoot, "REGVER", regver);
    cJSON_AddStringToObject(pJsonRoot, "MEID", /*"353114008096366"*/ imei);
    cJSON_AddStringToObject(pJsonRoot, "MODELSMS", "RDA-2017");
    cJSON_AddStringToObject(pJsonRoot, "SWVER", "build20200108");
    cJSON_AddStringToObject(pJsonRoot, "SIM1ICCID", "89860318250222720244" /*simiccid*/);
    cJSON_AddStringToObject(pJsonRoot, "SIM1LTEIMSI", /*"460110032920462"*/ imsi);
    char *regdatajs = cJSON_PrintUnformatted(pJsonRoot);

    if (regdatajs == NULL)
    {
        sys_arch_printf("JsonString error...");
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    else
        sys_arch_printf("genRegData :%s\n", regdatajs);

    out_len = strlen(regdatajs);

    mbedtls_base64_encode(NULL, 0, &buffer_len, (unsigned char *)regdatajs, out_len);
    buffer = (char *)malloc(buffer_len);

    if (buffer != NULL && mbedtls_base64_encode((unsigned char *)buffer, buffer_len, &len, (unsigned char *)regdatajs, out_len) != 0)
    {
        sys_arch_printf("base64 error...");
    }
    free(regdatajs);
    cJSON_Delete(pJsonRoot);
    return buffer;
}

static void parseJson(char *pMsg, reg_ctrl_t *reg_ctrl)
{
    if (NULL == pMsg)
    {
        sys_arch_printf("parseJson: pMsg is NULL!!!\n");
        return;
    }
    cJSON *pJson = cJSON_Parse(pMsg);
    if (NULL == pJson)
    {
        sys_arch_printf("parseJson: pJson is NULL!!!\n");
        return;
    }
    char *regdatajs = cJSON_PrintUnformatted(pJson);
    sys_arch_printf("parseJson: %s\n", regdatajs);

    cJSON *pSub = cJSON_GetObjectItem(pJson, "resultCode");
    if (NULL != pSub)
    {
        sys_arch_printf("resultCode: %s\n", pSub->valuestring);
        reg_ctrl->resultCode = atoi(pSub->valuestring);
    }

    pSub = cJSON_GetObjectItem(pJson, "resultDesc");
    if (NULL != pSub)
    {
        sys_arch_printf("resultDesc: %s\n", pSub->valuestring);
        strncpy(reg_ctrl->resultDesc, pSub->valuestring, 10);
    }

    if (gRegCtrl.cb != NULL)
    {
        sys_arch_printf("notify to uplower\n");
        gRegCtrl.cb(gRegCtrl.uti, regdatajs, strlen(regdatajs));
    }
    free(regdatajs);
}

static bool isResultOk(reg_ctrl_t *reg_ctrl)
{
    return reg_ctrl->resultCode == 0 && strcmp(reg_ctrl->resultDesc, "Sucess") == 0;
}

static void coap_user_cb(coap_client_t *client, coap_method_t method, bool result)
{
    sys_arch_printf("coap_user_cb (%d):%d", method, result);
    reg_ctrl_t *reg_ctrl = (reg_ctrl_t *)coap_client_getUserData(client);
    if (result == true && isResultOk(reg_ctrl))
    {
        //unsigned char simiccid[20/*UEICCID_LEN*/] = {0};
        //unsigned char iccid_len = 20/*UEICCID_LEN*/;
        //getSimIccid(reg_ctrl->simId,simiccid,&iccid_len);
        //NV_SetUEIccid(simiccid,iccid_len,reg_ctrl->simId);
        gRegCtrl.status = coap_client_getStatus(client);
        if (NULL != g_pstRetryTimer)
        {
            osiTimerStop(g_pstRetryTimer);
            osiTimerDelete(g_pstRetryTimer);
            g_pstRetryTimer = NULL;
        }
        gRegCtrl.retryCount = 0;
    }
    else if (reg_ctrl->retryCount < 3)
    {
        reg_ctrl->retryCount++;
        osiTimerStart(g_pstRetryTimer, 60 * 60 * 1000);
    }
    else
    {
        if (NULL != g_pstRetryTimer)
        {
            osiTimerStop(g_pstRetryTimer);
            osiTimerDelete(g_pstRetryTimer);
            g_pstRetryTimer = NULL;
        }
        gRegCtrl.retryCount = 0;
    }
    coap_client_free(client);
}

static void coap_incoming_data_cb(coap_client_t *client, const unsigned char *data, short unsigned int len, unsigned char num)
{
    sys_arch_printf("incoming_data(%d:%d): %s", num, len, data);
    reg_ctrl_t *reg_ctrl = (reg_ctrl_t *)coap_client_getUserData(client);
    parseJson((char *)data, reg_ctrl);
}

static void do_regProcess(void *param)
{
    char *regdata = NULL;
    int simid = (int)param;

    if (netif_default == NULL)
        goto retry;

    coap_client_t *client = coap_client_new(osiThreadCurrent(), coap_user_cb, &gRegCtrl);
    if (client == NULL)
        goto retry;

    gRegCtrl.simId = simid;
    regdata = genRegData(simid);
    if (NULL == regdata)
    {
        sys_arch_printf("regdata error...");
        coap_client_free(client);
        goto retry;
    }
    gRegCtrl.status = coap_client_getStatus(client);
    coap_client_setParams(client, ((unsigned char *)("-t application/json")));
    coap_client_setParams(client, ((unsigned char *)("-p 5683")));
    coap_client_setIndataCbk(client, ((coap_incoming_data_cb_t)coap_incoming_data_cb));
    coap_client_setPayload(client, (unsigned char *)regdata, strlen(regdata));
    coap_async_post(client, ((unsigned char *)("coap://114.255.193.236:5683/")), NULL, true);
    gRegCtrl.status = coap_client_getStatus(client);
    free(regdata);
    return;

retry:
    if (gRegCtrl.retryCount < 3)
    {
        gRegCtrl.retryCount++;
        osiTimerStart(g_pstRetryTimer, 60 * 60 * 1000);
    }
    else
    {
        if (NULL != g_pstRetryTimer)
        {
            osiTimerStop(g_pstRetryTimer);
            osiTimerDelete(g_pstRetryTimer);
            g_pstRetryTimer = NULL;
        }
        gRegCtrl.retryCount = 0;
    }
    return;
}

static int unicomIotSelfRegister(void *param)
{
    unsigned char ueiccid[20 /*UEICCID_LEN*/] = {0};  //in NV
    unsigned char simiccid[20 /*UEICCID_LEN*/] = {0}; //in sim card
    unsigned char iccid_len = 20 /*UEICCID_LEN*/;
    //if (!getSimIccid(simId, simiccid, &iccid_len))
    //    return -1;
    //NV_GetUEIccid(ueiccid, iccid_len, (int)param);
    if (0 == memcmp(ueiccid, simiccid, iccid_len))
        return 0;
    else
    {
        do_regProcess(param);
        if (gRegCtrl.status >= COAP_STATUS_READY)
            return 0;
        else
            return -1;
    }
}

static void doSelfReg(void *param)
{
    unicomIotSelfRegister(param);
}

void startUnicomIotSelfRegOnce(unsigned char flag, unsigned int uti, coap_selfreg_data_cb_t cb)
{
    //static bool iscalled;
    gRegCtrl.cb = cb;
    gRegCtrl.uti = uti;

    g_pstRetryTimer = osiTimerCreate(osiThreadCurrent(), doSelfReg, ((void *)0));

    //if (!iscalled && flag)
    //{
    //    iscalled = true;
    doSelfReg((void *)0);
    //}
}

#endif
