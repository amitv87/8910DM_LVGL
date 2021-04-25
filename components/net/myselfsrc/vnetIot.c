#include "cJSON.h"
#include "netutils.h"
#include "mupnp/util/string.h"
#include "http_api.h"
#include "sockets.h"
#include "at_cfg.h"
#include <time.h>
#include "mbedtls/base64.h"
#include "at_engine.h"
#include "osi_log.h"
#include "at_response.h"
#include "stdio.h"
#include "cfw.h"
#include "http_download.h"
#include "http_lunch_api.h"
#include "at_command.h"
#include "vfs.h"
#include "nvm.h"
#include "osi_api.h"

#ifdef CONFIG_SOC_8910

#define UEICCID_LEN 20
#define REG_HTTP_URL "http://zzhc.vnet.cn:9999/"
#define REG_HTTP_CONTENT "application/encrypted-json"
#define NV_SELF_REG "/nvm/dm_self_register.nv"
#define SOFTWARE_VER_NUM "BC60_1268D_V1.0"
#define RETRY_TIMER  (60*60*1000)

osiTimer_t *ghttpregtimer = NULL;
osiThread_t *dxhttpThread = NULL;

static void do_dxregProcess(uint8_t nCid, uint8_t nSimId);

extern char *vnetregdata;

extern osiThread_t *netGetTaskID();

typedef struct {
    uint8_t simId;
    uint8_t nCid;
    uint8_t resultCode;
    uint8_t retryCount;
    uint8_t resultDesc[12];
}reg_ctrl_t;

reg_ctrl_t *reg_ctrl;
nHttp_info *at_nHttp_reg = NULL;
bool is_dxreg_inited = false;

static bool isResultOk(reg_ctrl_t *reg_ctrl)
{
    return reg_ctrl->resultCode == 0 && strcmp((char *)reg_ctrl->resultDesc,"Success") == 0;
}

static void parseJson(char *pMsg, reg_ctrl_t *reg_ctrl)
{
    if (NULL == pMsg)
    {
        OSI_LOGI(0, "parseJson: pMsg is NULL!!!\n");
        return;
    }
    cJSON *pJson = cJSON_Parse(pMsg);
    if (NULL == pJson)
    {
        OSI_LOGI(0, "parseJson: pJson is NULL!!!\n");
        return;
    }
    char *regdatajs = cJSON_PrintUnformatted(pJson);
    OSI_LOGXI(OSI_LOGPAR_S, 0, "parseJson: %s\n", regdatajs);	

    cJSON *pSub = cJSON_GetObjectItem(pJson, "resultCode");
    if (NULL != pSub)
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0, "resultCode: %s\n", pSub->valuestring);
        reg_ctrl->resultCode = atoi(pSub->valuestring);
    }

    pSub = cJSON_GetObjectItem(pJson, "resultDesc");
    if (NULL != pSub)
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0, "resultDesc: %s\n", pSub->valuestring);
        strncpy((char *)reg_ctrl->resultDesc,pSub->valuestring,10);
    }
     free(regdatajs);
}

OSI_UNUSED static int OutputTime(char *rsp, struct tm *tm)
{
    static const char *fmt1 = "20%02d-%02d-%02d,%02d:%02d:%02d";
    static const char *fmt2 = "20%04d-%02d-%02d,%02d:%02d:%02d";
    if (gAtSetting.csdf_auxmode == 1)
    {
        return sprintf(rsp, fmt1, (tm->tm_year + 1900) % 100, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    else
    {
        return sprintf(rsp, fmt2, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}

void httpreg_timeout(void *param)
{
    OSI_LOGI(0, "do_dxreg httpreg_timeout_reg_ctrl->retryCount: %d\n", reg_ctrl->retryCount);
    do_dxregProcess(reg_ctrl->nCid,  reg_ctrl->simId);
}

static char *genRegData(uint8_t simid)
{
    unsigned char *buffer;
    size_t buffer_len;
    size_t len;
    size_t out_len;
    cJSON *pJsonRoot = NULL;
    pJsonRoot = cJSON_CreateObject();
    if (NULL == pJsonRoot)
        return NULL;
    uint8_t ret = 0;
    CFW_NW_STATUS_INFO sStatus;
    char rsp[10];
    uint8_t imei[16] = {0};
    uint8_t imsi[16] = {0};
    uint8_t simiccid[21] = {0};
    uint8_t imsisize = 16;
    uint8_t iccid_len = 20;
    int nImeiLen = nvmReadImei(simid, (nvmImei_t *)imei);
    if (nImeiLen == -1)
         OSI_LOGI(0, "nvmReadImei read error...");
    OSI_LOGXI(OSI_LOGPAR_S, 0, "NvmReadImeigetSimImei:%s", imei);

    getSimIccid(simid,simiccid,&iccid_len);

    getSimImsi(simid,imsi,&imsisize);
    time_t lt = osiEpochSecond() ;
    struct tm ltm;
    gmtime_r(&lt, &ltm);
    char ts[48];
    OutputTime(ts, &ltm);
    ret = CFW_GprsGetstatus(&sStatus, simid);
    if (ret != 0)
    {
        OSI_LOGI(0, "genRegData#CFW_GprsGetstatus error CELLID...");
        return NULL;
    }
    sprintf(rsp, "%02X%02X", sStatus.nCellId[0], sStatus.nCellId[1]);
    cJSON_AddStringToObject(pJsonRoot, "REGVER", "5.0");
    cJSON_AddStringToObject(pJsonRoot, "MEID", "");
    cJSON_AddStringToObject(pJsonRoot, "MODEL", "BJS-BC60");
    cJSON_AddStringToObject(pJsonRoot, "SWVER", SOFTWARE_VER_NUM);    //软件版本号
    cJSON_AddStringToObject(pJsonRoot, "IMEI1", (char *)imei);
    cJSON_AddStringToObject(pJsonRoot, "SIM1CDMAIMSI", "");
    cJSON_AddStringToObject(pJsonRoot, "SIM1ICCID", (const char *)simiccid);
    cJSON_AddStringToObject(pJsonRoot, "SIM1LTEIMSI", (const char *)imsi);
    cJSON_AddStringToObject(pJsonRoot, "SIM1CELLID", (const char *)rsp);
    cJSON_AddStringToObject(pJsonRoot, "REGDATE", (const char *)ts);

    char *regdatajs = cJSON_PrintUnformatted(pJsonRoot);

    if (regdatajs == NULL)
    {
        OSI_LOGI(0, "JsonString error...");
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    else
        OSI_LOGXI(OSI_LOGPAR_S, 0, "genRegData :%s\n", regdatajs);
    out_len = strlen(regdatajs);

    mbedtls_base64_encode( NULL, 0, &buffer_len, (const unsigned char *)regdatajs, out_len);
    buffer = (unsigned char *)malloc(buffer_len);

    if (buffer != NULL && mbedtls_base64_encode(buffer, buffer_len, &len, (const unsigned char *)regdatajs, out_len) != 0)
    {
        OSI_LOGI(0, "base64 error...");
    }
    free(regdatajs);
    cJSON_Delete(pJsonRoot);
    return (char *)buffer;
}
static void dx_registercb(void *param)
{
    nHttp_info *nHttp_inforeg = (nHttp_info *)param;
    OSI_LOGI(0, "dx_registercb Entering at_nHttp_reg->CID= %d", at_nHttp_reg->CID);
    if (Http_postnreg(nHttp_inforeg) == false)
    {
        if (Term_Http(at_nHttp_reg) != true)
        {
            OSI_LOGI(0, "dx_registercb Term fail, please try again later\n");
            at_nHttp_reg = NULL;
            is_dxreg_inited = false;
            goto threadexit;
        }
	 at_nHttp_reg = NULL;
        if(reg_ctrl->retryCount < 10)
        {
            reg_ctrl->retryCount++;
            OSI_LOGI(0, "dx_registercb error Http_postnreg error reg_ctrl->retryCount=%d", reg_ctrl->retryCount );
            ghttpregtimer = osiTimerCreate(netGetTaskID(), httpreg_timeout, NULL);
            if (ghttpregtimer == NULL)
            {
                OSI_LOGI(0, "HTTPreg# dx_registercb HTTPregTimmer create timer failed result error");
                is_dxreg_inited = false;
                goto threadexit;
            }
            osiTimerStart(ghttpregtimer , RETRY_TIMER);
            goto threadexit;
        }
        else
        {
             OSI_LOGI(0, "dx_registercb response operation errorreg_ctrl->retryCount = %d", reg_ctrl->retryCount );
             if(NULL != vnetregdata)
             {
                free(vnetregdata);
                vnetregdata = NULL;
             }
             is_dxreg_inited = false;
             goto tmdethexit;
        }
    }
    else
    {
         if (Term_Http(at_nHttp_reg) != true)
        {
            OSI_LOGI(0, "dx_registercb Term fail, please try again later\n");
            at_nHttp_reg = NULL;
            is_dxreg_inited = false;
            goto threadexit;
        }
         at_nHttp_reg = NULL;
        // response operation succuss
       //char *data = NULL;    //data 做一个全局变量下行下去得到返回的数据
       OSI_LOGI(0, "dx_registercb postnreg success");
       parseJson(vnetregdata, reg_ctrl);
       if(NULL != vnetregdata)
       {
           free(vnetregdata);
           vnetregdata = NULL;
       }
       if (isResultOk(reg_ctrl))
       {
           uint8_t simiccid[UEICCID_LEN+1] = {0};
           uint8_t iccid_len = UEICCID_LEN;
           if (ghttpregtimer != NULL)
           {
               osiTimerStop(ghttpregtimer);
               osiTimerDelete(ghttpregtimer);
               ghttpregtimer = NULL;
           }
           OSI_LOGI(0, "dx_registercb response operation succuss data right");
           getSimIccid(reg_ctrl->simId,simiccid,&iccid_len);
           vfs_file_write(NV_SELF_REG, simiccid, 20);
           is_dxreg_inited = false;
           //NV_SetUEIccid(simiccid,iccid_len,reg_ctrl->simId);
           goto threadexit;
       }
       else if (reg_ctrl->retryCount < 10)
       {
           reg_ctrl->retryCount++;
           OSI_LOGI(0, "dx_registercb response operation succuss data error");
           ghttpregtimer = osiTimerCreate(netGetTaskID(), httpreg_timeout, NULL);
           if (ghttpregtimer == NULL)
           {
               OSI_LOGI(0, "HTTPreg# dx_registercb HTTPregTimmer create timer failed");
               is_dxreg_inited = false;
               goto threadexit;
           }
           osiTimerStart(ghttpregtimer , RETRY_TIMER);
           goto threadexit;
           //COS_StartFunctionTimer(60*60*1000,do_dxregProcess,reg_ctrl->simId);
       }
       else
       {
            OSI_LOGI(0, "dx_registercb httppostreg success reg_ctrl->retryCount = %d", reg_ctrl->retryCount );
            if(NULL != vnetregdata)
            {
               free(vnetregdata);
               vnetregdata = NULL;
            }
            is_dxreg_inited = false;
            goto tmdethexit;
       }

    }
tmdethexit:
    if (ghttpregtimer != NULL)
    {
        //osiTimerStop(ghttpregtimer);
        osiTimerDelete(ghttpregtimer);
        ghttpregtimer = NULL;
    }
    if(dxhttpThread  != NULL)
    {
        dxhttpThread = NULL;
        OSI_LOGI(0, "dx_registercb will exit...");
        osiThreadExit();
    }
    return;

threadexit:
    if(dxhttpThread  != NULL)
    {
        dxhttpThread = NULL;
        OSI_LOGI(0, "dx_registercb will exit...");
        osiThreadExit();
    }
    return;
}

static void do_dxregProcess(uint8_t nCid, uint8_t nSimId)
{
    char *regdata = NULL;

    if (netif_default == NULL)
        goto retry;
    if(at_nHttp_reg != NULL)
    {
        OSI_LOGI(0, "do_dxregProcess#at_nHttp_reg is not null first term ");
        Term_Http(at_nHttp_reg);
    }
    at_nHttp_reg = Init_Http();
    if (at_nHttp_reg == NULL)
    {
        OSI_LOGI(0, "Init fail, please try again later\n");
        goto retry;
    }
    regdata = genRegData(nSimId);
    if (regdata == NULL)
    {
        OSI_LOGI(0, "regdata error...");
        goto retry;
    }
    at_nHttp_reg->CID = nCid;
    OSI_LOGI(0, "do_dxregProcessat_nHttp_reg->CID=%d", at_nHttp_reg->CID);
    strcpy(at_nHttp_reg->url,  REG_HTTP_URL);
    strcpy(at_nHttp_reg->CONTENT_TYPE,  REG_HTTP_CONTENT );
    strcpy(at_nHttp_reg->user_data,  regdata );

    free(regdata);
    (at_nHttp_reg->cg_http_api)->nSIM = nSimId;
    (at_nHttp_reg->cg_http_api)->nCID = nCid;
    dxhttpThread = osiThreadCreate("dx_register", dx_registercb, at_nHttp_reg, OSI_PRIORITY_NORMAL,4096, 0);
    if(dxhttpThread == NULL)
    {
        OSI_LOGI(0, "httppost create async osiThreadCreate fail");
        goto retry;
    }
    reg_ctrl->nCid = nCid;
    reg_ctrl->simId = nSimId;
    return;
retry:
    if (reg_ctrl->retryCount < 10)
    {
        reg_ctrl->retryCount++;
        OSI_LOGI(0, "do_dxregProcess error");
        ghttpregtimer = osiTimerCreate(netGetTaskID(), httpreg_timeout, NULL);
        if (ghttpregtimer == NULL)
        {
            OSI_LOGI(0, "do_dxregProcess# do_dxregProcessTimmer create timer failed");
            if(dxhttpThread  != NULL)
            {
                dxhttpThread = NULL;
                OSI_LOGI(0, "dx_registercb will exit...");
                osiThreadExit();
            }
            is_dxreg_inited = false;
            return;
        }
        osiTimerStart(ghttpregtimer , RETRY_TIMER);
        if(dxhttpThread  != NULL)
        {
            dxhttpThread = NULL;
            OSI_LOGI(0, "dx_registercb will exit...");
            osiThreadExit();
        }
    }
    else
    {
         if (ghttpregtimer != NULL)
         {
             //osiTimerStop(ghttpregtimer);
             osiTimerDelete(ghttpregtimer);
             ghttpregtimer = NULL;
         }
         is_dxreg_inited = false;

         if(dxhttpThread  != NULL)
         {
             dxhttpThread = NULL;
             OSI_LOGI(0, "dx_registercb will exit...");
             osiThreadExit();
         }
         return;
    }
}

bool sul_ZeroMemory8(void  *pBuf, uint32_t count)
{
    char *xs = pBuf;
    while (count--)
        *xs++ = 0x0;
    return 1;
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE //not suitable for quectel
int vnet4gSelfRegister(uint8_t nCid, uint8_t nSimId)
{
    uint8_t tmpMode = 0;
    uint8_t operatorId[6];
    uint8_t *pOperName = NULL;
    uint8_t *pOperShortName = NULL;
    uint8_t ueiccid[UEICCID_LEN + 1] = {0};
    uint8_t  nRet = 0;
    if (ghttpregtimer != NULL)         //上次未做完需要停掉
    {
        OSI_LOGI(0, "vnet4gSelfRegister#register  stop ghttpregtimer");
        osiTimerStop(ghttpregtimer);
        osiTimerDelete(ghttpregtimer);
        ghttpregtimer = NULL;
        is_dxreg_inited = false;
    }
    if(is_dxreg_inited )
    {
        OSI_LOGI(0, "vnet4gSelfRegister#register is processing do not repeat exit");
        return 0;
    }
    is_dxreg_inited = true;
    sul_ZeroMemory8(operatorId, 6);
    nRet = CFW_NwGetCurrentOperator(operatorId, &tmpMode, nSimId);
    if (nRet != 0)
    {
        is_dxreg_inited = false;
        return -1;
    }
    OSI_LOGXI(OSI_LOGPAR_IS, 0, "vnet4gSelfRegister#CFW_NwGetCurrentOperator operatorId=%d %s", tmpMode, operatorId);
    nRet =  CFW_CfgNwGetOperatorName(operatorId, &pOperName, &pOperShortName);
    if (nRet != 0)
    {
        is_dxreg_inited = false;
        return -1;
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0, "vnet4gSelfRegister#CFW_CfgNwGetOperatorName pOperName=%s", pOperName);
    if ((0 == strcmp("ChinaTelecom", (char*)pOperName))||(0 == strcmp("CHN-CT", (char*)pOperName)))
    {
        uint8_t simiccid[21] = {0};
        uint8_t iccid_len = UEICCID_LEN;
        if (!getSimIccid(nSimId,simiccid,&iccid_len))
        {
            is_dxreg_inited = false;
            return -1;
        }
        int fd = vfs_open(NV_SELF_REG, 2);
        if (fd < 0)
        {
            OSI_LOGI(0, "vnet4gSelfRegister#This file is not exit vfs_open failed fd =%d", fd);
            vfs_file_write(NV_SELF_REG, "0", 1);
        }
        else
        {
            OSI_LOGI(0, "vnet4gSelfRegister#This file is  exit vfs_open success fd =%d", fd);
            struct stat st = {};
            vfs_fstat(fd, &st);
            int file_size = st.st_size;
            if(file_size == 1 || file_size == 20)
            {
                OSI_LOGI(0, "vnet4gSelfRegister#This file size is right =%d", file_size);
                vfs_read(fd, ueiccid, 20);
                OSI_LOGXI(OSI_LOGPAR_S, 0, "vnet4gSelfRegister#This file ueiccid =%s", ueiccid);
                vfs_close(fd);
            }
            else
            {
                OSI_LOGI(0, "vnet4gSelfRegister#This file size is error =%d", file_size);
                vfs_unlink(NV_SELF_REG);
                vfs_file_write(NV_SELF_REG, "0", 1);
            }

        }
        // NV_GetUEIccid(ueiccid,iccid_len,simId);    //读flag   比较iccid
        if (memcmp(ueiccid, simiccid,iccid_len) == 0)
        {
             is_dxreg_inited = false;
             OSI_LOGI(0, "vnet4gSelfRegister# ueiccid same as simiccid register success");
             return 0;
        }
        else
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "vnet4gSelfRegister#simiccid = %s", simiccid);
            reg_ctrl = (reg_ctrl_t *)malloc(sizeof(reg_ctrl));
            if(NULL == reg_ctrl)
            {
                OSI_LOGI(0, "reg_ctrl malloc error");
                is_dxreg_inited = false;
                return -1;
            }
            reg_ctrl->retryCount = 0;
            do_dxregProcess(nCid, nSimId);
        }
        return 1;
    }
    else
    {
        OSI_LOGI(0, "vnet4gSelfRegister# dvnet4gSelfRegister operator is not vnet4g");
        is_dxreg_inited = false;
        return 0;
    }
}
#endif


#endif

