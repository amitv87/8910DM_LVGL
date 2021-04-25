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
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "cfw.h"
#include "cfw_event.h"
#include "cfw_chset.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "tb.h"
#include "osi_api.h"
#include "osi_log.h"
#include "atr_config.h"
#include "tb_config.h"
#include "nvm.h"
#include "at_cfg.h"
#include "time.h"
#include "sockets.h"
#include "at_cfw.h"
#include "ml.h"
#include "audio_device.h"
#include "fupdate.h"
#include "drv_rtc.h"
#include "drv_gpio.h"
#include "netmain.h"
#include "ppp_interface.h"

#ifdef CONFIG_TB_API_SUPPORT
#include "tb_internal.h"
//variable
tb_wan_network_info_cb g_tb_wan_network_info_cb = NULL;
tb_data_callback_fun g_tb_data_callback_fun = NULL;
tb_sim_callback_fun g_tb_sim_callback_fun = NULL;
tb_sms_callback_fun g_tb_sms_callback_fun = NULL;
tb_sms_received_contents_cb g_tb_sms_received_contents_cb = NULL;
tb_voice_callback_fun g_tb_voice_callback_fun = NULL;

tb_data_connect_profile g_tb_data_connect_profile[2] = {
    0,
};
uint8_t g_tb_sim_status = 0xff;

tb_data_dial_status g_tb_data_dial_status[2] = {TB_DIAL_STATUS_DISCONNECTED, TB_DIAL_STATUS_DISCONNECTED};

char g_tb_imsi_valid = 0;
char g_tb_imsi[16] = {
    0,
};

char g_tb_own_num_valid = 0;
char g_tb_own_num[41] = {
    0,
};
static uint8_t g_SMS_Unread_Msg = 0;
static uint8_t gConnectingFlag = 0;

OSI_WEAK const char *AT_GMI_ID = GMI_ID;
OSI_WEAK const char *AT_GMM_ID = GMM_ID;
OSI_WEAK const char *AT_GMR_ID = GMR_ID;

#define URI_MAX_NUMBER 21
#define SIP_STR "sip:"

uint8_t g_cfw_exit_flag = 0xFF;

//API
static uint16_t tb_RequestUTI(osiEventCallback_t cb, void *cb_ctx)
{
    return cfwRequestUTIEx(cb, cb_ctx, atEngineGetThreadId(), true);
}
//NW module
int tb_wan_reg_callback(tb_wan_network_info_cb wan_network_info_func)
{
    OSI_LOGI(0, "tb_wan_reg_callback, wan_network_info_func is 0x%x", wan_network_info_func);
    g_tb_wan_network_info_cb = wan_network_info_func;
    return TB_SUCCESS;
}

int tb_wan_get_network_type(char *type, int size)
{
    uint8_t nSim = 0;
    uint8_t nRat = CFW_NWGetStackRat(nSim);

    CFW_NW_STATUS_INFO NwStatus;
    CFW_NW_STATUS_INFO GprsStatus;

    char *pNetworkType = NULL;

    OSI_LOGI(0, "tb_wan_get_network_type, nRat is %d", nRat);

    CFW_NwGetStatus(&NwStatus, nSim);
    CFW_GprsGetstatus(&GprsStatus, nSim);

    if (nRat == 2)
    { //GSM
        OSI_LOGI(0, "tb_wan_get_network_type, NwStatus.nStatus is %d", NwStatus.nStatus);
        if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
        {
            pNetworkType = TB_NETWORK_TYPE_2G;
        }
        else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED)
        {
            pNetworkType = TB_NETWORK_TYPE_LIMITED;
        }
        else
        {
            pNetworkType = TB_NETWORK_TYPE_NO_SERVICE;
        }
    }
    else if (nRat == 4)
    { //LTE
        OSI_LOGI(0, "tb_wan_get_network_type, GprsStatus.nStatus is %d", GprsStatus.nStatus);
        if (GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
        {
            pNetworkType = TB_NETWORK_TYPE_4G;
        }
        else if (GprsStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED)
        {
            pNetworkType = TB_NETWORK_TYPE_LIMITED;
        }
        else
        {
            pNetworkType = TB_NETWORK_TYPE_NO_SERVICE;
        }
    }
    else
    {
        OSI_LOGI(0, "tb_wan_get_network_type Unkonwn rat");
    }

    if (pNetworkType != NULL)
    {
        strncpy(type, pNetworkType, size);
        return TB_SUCCESS;
    }
    else
    {
        return TB_FAILURE;
    }
}

int tb_wan_get_network_info(tb_wan_network_info_s *network_info)
{
    uint8_t nSim = 0;
    uint8_t mode;
    uint8_t OperatorId[6];
    uint8_t nIMSI[15] = {
        0,
    };
    uint8_t nTempOperatorId[16] = {
        0x0f,
    };
    uint8_t nHomeOperatorId[6] = {
        0x0f,
    };
    uint8_t nIMSILen = 0;
    uint8_t nMNCLen = 0;
    unsigned char *oper_name = NULL;
    unsigned char *oper_short_name = NULL;

    uint8_t nRat;
    char *sNetType = NULL;

    CFW_NW_STATUS_INFO NwStatus;
    CFW_NW_STATUS_INFO GprsStatus;

    CFW_NET_INFO_T *pNetinfo = (CFW_NET_INFO_T *)malloc(sizeof(CFW_NET_INFO_T));
    if (pNetinfo == NULL)
    {
        return TB_FAILURE;
    }
    memset(pNetinfo, 0, sizeof(CFW_NET_INFO_T));
    if (0 != CFW_GetNetinfo(pNetinfo, nSim))
    {
        free(pNetinfo);
        return TB_FAILURE;
    }

    if (network_info == NULL)
    {
        free(pNetinfo);
        return TB_FAILURE;
    }

    OSI_LOGI(0, "tb_wan_get_network_info");

    CFW_NwGetCurrentOperator(OperatorId, &mode, nSim);
    CFW_CfgNwGetOperatorName(OperatorId, &oper_name, &oper_short_name);

    OSI_LOGI(0, "OperatorId:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", OperatorId[0], OperatorId[1], OperatorId[2], OperatorId[3], OperatorId[4], OperatorId[5]);

    if (oper_name != NULL)
    {
        strncpy(network_info->provider_long_name, (const char *)oper_name, TB_PROVIDER_LONG_NAME_SIZE);
    }
    if (oper_short_name != NULL)
    {
        strncpy(network_info->provider_short_name, (const char *)oper_short_name, TB_PROVIDER_SHORT_NAME_SIZE);
    }

    nRat = CFW_NWGetStackRat(nSim);
    OSI_LOGI(0, "nRat:%d", nRat);
    if (nRat == 2)
    {
        sNetType = TB_NET_TYPE_GSM;
    }
    else if (nRat == 4)
    {
        sNetType = TB_NET_TYPE_LTE;
    }

    if (sNetType != NULL)
    {
        strncpy(network_info->net_type, sNetType, TB_NETWORK_TYPE_SIZE);
    }

    CFW_NwGetStatus(&NwStatus, nSim);
    CFW_GprsGetstatus(&GprsStatus, nSim);

    CFW_CfgGetIMSI(nIMSI, nSim);
    tb_IMSItoHomePlmn(nIMSI, nTempOperatorId, &nIMSILen);
    CFW_GetMNCLen(&nMNCLen, nSim);
    memset(nHomeOperatorId, 0x0f, 6);
    memcpy(nHomeOperatorId, nTempOperatorId, 3 + nMNCLen);
    OSI_LOGI(0, "NwStatus.nStatus:%d", NwStatus.nStatus);
    if ((NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING) && (GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING))
    {
        network_info->domain_state = TB_WAN_DOMAIN_STATE_CS_PS;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {

        network_info->domain_state = TB_WAN_DOMAIN_STATE_CS_ONLY;
    }
    else if (GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {
        network_info->domain_state = TB_WAN_DOMAIN_STATE_PS_ONLY;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || GprsStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED)
    {
        network_info->domain_state = TB_WAN_DOMIAN_STATE_UNKOWN;
    }

    if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME)
    {
        network_info->roam_state = TB_WAN_ROAM_STATUS_HOME;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {
        if (memcmp((const void *)OperatorId, (const void *)nHomeOperatorId, 3) == 0)
        {
            network_info->roam_state = TB_WAN_ROAM_STATUS_INTERNAL;
        }
        else
        {
            network_info->roam_state = TB_WAN_ROAM_STATUS_INTERNATIONAL;
        }
    }
    else
    {
        network_info->roam_state = TB_WAN_ROAM_STATUS_UNKOWN;
    }

    memcpy(network_info->mcc, OperatorId, TB_MCC_MNC_SIZE);
    OSI_LOGI(0, "mcc:0x%x,0x%x,0x%x", network_info->mcc[0], network_info->mcc[1], network_info->mcc[2]);
    memcpy(network_info->mnc, &(OperatorId[3]), TB_MCC_MNC_SIZE);
    OSI_LOGI(0, "mnc:0x%x,0x%x,0x%x", network_info->mnc[0], network_info->mnc[1], network_info->mnc[2]);

    if (nRat == 2)
    {
        OSI_LOGI(0, "lac_code:%d,%d", NwStatus.nAreaCode[3], NwStatus.nAreaCode[4]);
        memcpy(network_info->lac_code, &NwStatus.nAreaCode[3], TB_LAC_CODE_SIZE);
        OSI_LOGI(0, "cell_id:%d,%d,%d,%d", NwStatus.nCellId[0], NwStatus.nCellId[1], NwStatus.nCellId[2], NwStatus.nCellId[3]);
        memcpy(network_info->cell_id, NwStatus.nCellId, TB_CELL_ID_SIZE);
        // TODO: band info??
    }
    else if (nRat == 4)
    {
        OSI_LOGI(0, "lac_code:%d,%d", GprsStatus.nAreaCode[0], GprsStatus.nAreaCode[1]);
        memcpy(network_info->lac_code, &GprsStatus.nAreaCode[0], TB_LAC_CODE_SIZE);
        OSI_LOGI(0, "cell_id:%d,%d,%d,%d", GprsStatus.nCellId[0], GprsStatus.nCellId[1], GprsStatus.nCellId[2], GprsStatus.nCellId[3]);
        memcpy(network_info->cell_id, GprsStatus.nCellId, TB_CELL_ID_SIZE);
        OSI_LOGI(0, "bandInfo:0x%x", pNetinfo->nwCapabilityLte.lteScell.bandInfo);
        network_info->net_band[0] = (pNetinfo->nwCapabilityLte.lteScell.bandInfo >> 8);
        network_info->net_band[1] = (pNetinfo->nwCapabilityLte.lteScell.bandInfo & 0xFF);
    }
    free(pNetinfo);
    OSI_LOGI(0, "tb_wan_get_network_info, end");
    return TB_SUCCESS;
}

int tb_wan_get_signal_info(tb_wan_signal_info_s *signal_info)
{
    uint8_t nSim = 0;
    uint8_t nRat = CFW_NWGetStackRat(nSim);
    CFW_NW_QUAL_INFO QualInfo = {0};
    OSI_LOGI(0, "tb_wan_get_signal_info");
    if (signal_info == NULL)
    {
        OSI_LOGI(0, "tb_wan_get_signal_info,signal_info NULL");
        return TB_FAILURE;
    }

    if (CFW_NwGetQualReport(&QualInfo, nSim) != 0)
    {
        OSI_LOGI(0, "tb_wan_get_signal_info,signal_info get Qual Report failed");
        return TB_FAILURE;
    }

    if (nRat == 2)
    {
        OSI_LOGI(0, "tb_wan_get_signal_info,nRssi:%d", QualInfo.nRssi);
        signal_info->signal_bar = tb_get_signal_bar_gsm(QualInfo.nRssi);
        signal_info->rssi = QualInfo.nRssi;
    }
    else if (nRat == 4)
    {
        OSI_LOGI(0, "tb_wan_get_signal_info,iRsrp:%d", QualInfo.iRsrp);
        signal_info->signal_bar = tb_get_signal_bar_lte(QualInfo.iRsrp);
        signal_info->rsrp = QualInfo.iRsrp;
        signal_info->rsrq = QualInfo.iRsrq;
    }
    else
    {
        OSI_LOGI(0, "tb_wan_get_signal_info,unkown Rat:%d", nRat);
        return TB_FAILURE;
    }
    OSI_LOGI(0, "tb_wan_get_signal_info, end");
    return TB_SUCCESS;
}

int tb_data_reg_callback(tb_data_callback_fun fun)
{
    OSI_LOGI(0, "tb_data_reg_callback");
    g_tb_data_callback_fun = fun;
    return TB_SUCCESS;
}

int tb_data_wan_disconnect(tb_data_profile_id cid)
{
    uint32_t result = 0;
    uint8_t nSim = 0;

    OSI_LOGI(0, "tb_data_wan_disconnect, cid: %d", cid);

    uint16_t uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Gprs_Act_Rsp, NULL);

    result = CFW_GprsAct(0, (uint8_t)cid, uti, nSim);
    if (result != 0)
    {
        OSI_LOGI(0, "tb_data_wan_disconnect, Act failed: 0x%x", result);
        return TB_FAILURE;
    }

    g_tb_data_dial_status[cid - 1] = TB_DIAL_STATUS_DISCONNECTING;

    if (g_tb_data_callback_fun != NULL)
    {
        g_tb_data_callback_fun(cid, TB_DIAL_STATUS_DISCONNECTING);
    }

    return TB_SUCCESS;
}

int tb_data_get_dial_status(tb_data_profile_id cid, tb_data_dial_status *istatus)
{
    //uint8_t nState = 0;
    //uint8_t nSim = 0;
    OSI_LOGI(0, "tb_data_get_dial_status, cid: %d", cid);
    if (cid != TB_PROFILE_ID_PUBLIC && cid != TB_PROFILE_ID_PRIVATE)
    {
        return TB_FAILURE;
    }
    if (istatus == NULL)
    {
        return TB_FAILURE;
    }

    *istatus = g_tb_data_dial_status[cid - 1];

    return TB_SUCCESS;
}

int tb_data_set_connect_parameter(tb_data_profile_id cid, tb_data_connect_profile para)
{
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont = {
        0,
    };
    uint8_t nSim = 0;

    OSI_LOGI(0, "tb_data_set_connect_parameter, cid: %d", cid);

    if (cid != TB_PROFILE_ID_PUBLIC && cid != TB_PROFILE_ID_PRIVATE)
    {
        return TB_FAILURE;
    }

    sPdpCont.nApnSize = strlen((const char *)para.apn);
    if (sPdpCont.nApnSize > THE_APN_MAX_LEN)
    {
        sPdpCont.nApnSize = THE_APN_MAX_LEN;
    }
    strncpy((char *)sPdpCont.pApn, (const char *)para.apn, THE_APN_MAX_LEN);

    sPdpCont.nApnUserSize = strlen((const char *)para.username);
    if (sPdpCont.nApnUserSize > THE_APN_USER_MAX_LEN)
    {
        sPdpCont.nApnUserSize = THE_APN_USER_MAX_LEN;
    }
    strncpy((char *)sPdpCont.pApnUser, (const char *)para.username, THE_APN_USER_MAX_LEN);

    sPdpCont.nApnPwdSize = strlen((const char *)para.password);
    if (sPdpCont.nApnPwdSize > THE_APN_PWD_MAX_LEN)
    {
        sPdpCont.nApnPwdSize = THE_APN_PWD_MAX_LEN;
    }
    strncpy((char *)sPdpCont.pApnPwd, (const char *)para.password, THE_APN_PWD_MAX_LEN);

    OSI_LOGI(0, "tb_data_set_connect_parameter, para.pdp_type: %d", para.pdp_type);
    switch (para.pdp_type)
    {
    case TB_PDP_TYPE_IPV4:
        sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
        break;
    case TB_PDP_TYPE_IPV6:
        sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IPV6;
        break;
    case TB_PDP_TYPE_IPV4V6:
        sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IPV4V6;
        break;
    default:
        OSI_LOGI(0, "tb_data_set_connect_parameter, unknown type");
        break;
    }

    sPdpCont.nAuthProt = para.auth_mode;

    if (CFW_GprsSetPdpCxtV2(cid, &sPdpCont, nSim) != 0)
    {
        return TB_FAILURE;
    }
    else
    {
        memcpy(&(g_tb_data_connect_profile[cid - 1]), &para, sizeof(tb_data_connect_profile));
        return TB_SUCCESS;
    }
}

int tb_data_wan_connect(tb_data_profile_id cid)
{
    uint32_t result = 0;
    uint8_t nSim = 0;
    OSI_LOGI(0, "tb_data_wan_connect, cid: %d", cid);

    if (cid != TB_PROFILE_ID_PUBLIC && cid != TB_PROFILE_ID_PRIVATE)
    {
        return TB_FAILURE;
    }

    uint16_t uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Gprs_Act_Rsp, NULL);

    result = CFW_GprsAct(1, (uint8_t)cid, uti, nSim);
    if (result != 0)
    {
        OSI_LOGI(0, "tb_data_wan_connect, Act failed: 0x%x", result);
        return TB_FAILURE;
    }

    g_tb_data_dial_status[cid - 1] = TB_DIAL_STATUS_CONNECTING;

    if (g_tb_data_callback_fun != NULL)
    {
        g_tb_data_callback_fun(cid, TB_DIAL_STATUS_CONNECTING);
    }

    return TB_SUCCESS;
}

int tb_data_get_connect_parameter(tb_data_profile_id cid, tb_data_connect_profile *para)
{
    OSI_LOGI(0, "tb_data_get_connect_parameter, cid: %d", cid);

    if (cid != TB_PROFILE_ID_PUBLIC && cid != TB_PROFILE_ID_PRIVATE)
    {
        return TB_FAILURE;
    }

    if (para == NULL)
    {
        return TB_FAILURE;
    }

    memcpy((void *)para, &(g_tb_data_connect_profile[cid - 1]), sizeof(tb_data_connect_profile));
    return TB_SUCCESS;
}

int tb_data_get_statistics(tb_data_profile_id cid, tb_data_statistics *statistics)
{

    if (cid != TB_PROFILE_ID_PUBLIC && cid != TB_PROFILE_ID_PRIVATE)
        return TB_FAILURE;

    if (statistics == NULL)
        return TB_FAILURE;

    OSI_LOGI(0, "tb_data_get_statistics, cid: %d", cid);
    //if (g_tb_data_dial_status[cid - 1] == TB_DIAL_STATUS_CONNECTED)
    {
        uint8_t nSim = 0;
        uint32_t size = 0;
        uint32_t rx_size = 0;
        uint32_t tx_size = 0;

        CFW_get_Netif_dataCountBySimCid(nSim, cid, UPLOAD, LWIP_DATA, &size);
        tx_size += size;
        CFW_get_Netif_dataCountBySimCid(nSim, cid, UPLOAD, PPP_DATA, &size);
        tx_size += size;
        CFW_get_Netif_dataCountBySimCid(nSim, cid, UPLOAD, RNDIS_DATA, &size);
        tx_size += size;

        CFW_get_Netif_dataCountBySimCid(nSim, cid, DOWNLOAD, LWIP_DATA, &size);
        rx_size += size;
        CFW_get_Netif_dataCountBySimCid(nSim, cid, DOWNLOAD, PPP_DATA, &size);
        rx_size += size;
        CFW_get_Netif_dataCountBySimCid(nSim, cid, DOWNLOAD, RNDIS_DATA, &size);
        rx_size += size;

        statistics->realtime_rx_bytes = rx_size;
        statistics->realtime_tx_bytes = tx_size;
    }

    return TB_SUCCESS;
}

int tb_data_get_ipv4_address(tb_data_profile_id cid, tb_data_ipv4_addr *address)
{
    uint8_t nSim = 0;
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
    uint32_t ret = 0;
    OSI_LOGI(0, "tb_data_get_ipv4_address, cid: %d", cid);
    if (address == NULL)
    {
        OSI_LOGI(0, "Null address");
        return TB_FAILURE;
    }

    if (g_tb_data_dial_status[cid - 1] != TB_DIAL_STATUS_CONNECTED)
    {
        memset(address, 0, sizeof(tb_data_ipv4_addr));
        sprintf(address->ip_addr, "0.0.0.0");
        sprintf(address->pref_dns_addr, "0.0.0.0");
        sprintf(address->standy_dns_addr, "0.0.0.0");
        return TB_SUCCESS;
    }
    ret = CFW_GprsGetPdpCxtV2(cid, &sPdpCont, nSim);
    if (ret != 0)
    {
        OSI_LOGI(0, "get address fail, ret: 0x%x", ret);
        return TB_FAILURE;
    }

    memset(address, 0, sizeof(tb_data_ipv4_addr));
    OSI_LOGI(0, "sPdpCont.nIpType is :%d", sPdpCont.nIpType);
    OSI_LOGI(0, "sPdpCont.nPdpAddrSize is :%d", sPdpCont.nPdpAddrSize);
    if (sPdpCont.nPdpAddrSize == 0)
    {
        OSI_LOGI(0, "nPdpAddrSize zero");
        sprintf(address->ip_addr, "0.0.0.0");
        //return TB_FAILURE;
    }
    else
    {
        sprintf(address->ip_addr, "%d.%d.%d.%d", sPdpCont.pPdpAddr[0], sPdpCont.pPdpAddr[1], sPdpCont.pPdpAddr[2], sPdpCont.pPdpAddr[3]);
    }
    if (sPdpCont.nPdpDnsSize == 0)
    {
        OSI_LOGI(0, "nPdpDnsSize zero");
        sprintf(address->pref_dns_addr, "0.0.0.0");
        sprintf(address->standy_dns_addr, "0.0.0.0");
        //return TB_FAILURE;
    }
    else
    {
        sprintf(address->pref_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
        sprintf(address->standy_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);
    }

    return TB_SUCCESS;
}

int tb_data_get_ip_address(tb_data_profile_id cid, tb_data_ip_addr *address)
{
    uint8_t nSim = 0;
    CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
    uint32_t ret = 0;
    OSI_LOGI(0, "tb_data_get_ip_address, cid: %d", cid);
    if (address == NULL)
    {
        OSI_LOGI(0, "Null address");
        return TB_FAILURE;
    }

    if (g_tb_data_dial_status[cid - 1] != TB_DIAL_STATUS_CONNECTED)
    {
        OSI_LOGI(0, "Not Connected");
        memset(address, 0, sizeof(tb_data_ip_addr));
        address->nPdpType = TB_DATA_PDP_TYPE_UNUSED;
        return TB_SUCCESS;
    }
    ret = CFW_GprsGetPdpCxtV2(cid, &sPdpCont, nSim);
    if (ret != 0)
    {
        OSI_LOGI(0, "get address fail, ret: 0x%x", ret);
        return TB_FAILURE;
    }

    memset(address, 0, sizeof(tb_data_ip_addr));
    OSI_LOGI(0, "sPdpCont.nIpType is :%d", sPdpCont.nIpType);
    OSI_LOGI(0, "sPdpCont.nPdpAddrSize is :%d", sPdpCont.nPdpAddrSize);
    OSI_LOGI(0, "sPdpCont.nPdpType is :%d", sPdpCont.nPdpType);
    switch (sPdpCont.nPdpType)
    {
    case CFW_GPRS_PDP_TYPE_IP:
    {
        address->nPdpType = TB_PDP_TYPE_IPV4;
        if (sPdpCont.nPdpAddrSize == 0)
        {
            OSI_LOGI(0, "nPdpAddrSize zero");
            sprintf(address->ip_addr, "0.0.0.0");
        }
        else
        {
            sprintf(address->ip_addr, "%d.%d.%d.%d", sPdpCont.pPdpAddr[0], sPdpCont.pPdpAddr[1], sPdpCont.pPdpAddr[2], sPdpCont.pPdpAddr[3]);
        }
        if (sPdpCont.nPdpDnsSize == 0)
        {
            OSI_LOGI(0, "nPdpDnsSize zero");
            sprintf(address->pref_dns_addr, "0.0.0.0");
            sprintf(address->standy_dns_addr, "0.0.0.0");
        }
        else
        {
            sprintf(address->pref_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
            sprintf(address->standy_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);
        }
    }
    break;

    case CFW_GPRS_PDP_TYPE_IPV6:
    {
        ip6_addr_t ip6 = {0};
        address->nPdpType = TB_PDP_TYPE_IPV6;
        if (sPdpCont.nPdpAddrSize == 0)
        {

            OSI_LOGI(0, "nPdpAddrSize zero");
            IP6_ADDR(&ip6, 0, 0, 0, 0);
            sprintf(address->ipv6_addr, ip6addr_ntoa(&ip6));
        }
        else
        {
            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[4], sPdpCont.pPdpAddr[5], sPdpCont.pPdpAddr[6], sPdpCont.pPdpAddr[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[8], sPdpCont.pPdpAddr[9], sPdpCont.pPdpAddr[10], sPdpCont.pPdpAddr[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[12], sPdpCont.pPdpAddr[13], sPdpCont.pPdpAddr[14], sPdpCont.pPdpAddr[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[16], sPdpCont.pPdpAddr[17], sPdpCont.pPdpAddr[18], sPdpCont.pPdpAddr[19]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->ipv6_addr, ip6addr_ntoa(&ip6));
        }
        if (sPdpCont.nPdpDnsSize == 0)
        {
            OSI_LOGI(0, "nPdpDnsSize zero");
            IP6_ADDR(&ip6, 0, 0, 0, 0);
            sprintf(address->pref_v6_dns_addr, ip6addr_ntoa(&ip6));
            sprintf(address->standy_v6_dns_addr, ip6addr_ntoa(&ip6));
        }
        else
        {
            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->pref_v6_dns_addr, ip6addr_ntoa(&ip6));

            addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
            addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
            addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
            addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->standy_v6_dns_addr, ip6addr_ntoa(&ip6));
        }
    }
    break;

    case CFW_GPRS_PDP_TYPE_IPV4V6:
    {
        ip6_addr_t ip6 = {0};
        address->nPdpType = TB_PDP_TYPE_IPV4V6;
        if (sPdpCont.nPdpAddrSize == 0)
        {

            OSI_LOGI(0, "nPdpAddrSize zero");
            sprintf(address->ip_addr, "0.0.0.0");
            IP6_ADDR(&ip6, 0, 0, 0, 0);
            sprintf(address->ipv6_addr, ip6addr_ntoa(&ip6));
        }
        else
        {
            sprintf(address->ip_addr, "%d.%d.%d.%d", sPdpCont.pPdpAddr[0], sPdpCont.pPdpAddr[1], sPdpCont.pPdpAddr[2], sPdpCont.pPdpAddr[3]);

            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[4], sPdpCont.pPdpAddr[5], sPdpCont.pPdpAddr[6], sPdpCont.pPdpAddr[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[8], sPdpCont.pPdpAddr[9], sPdpCont.pPdpAddr[10], sPdpCont.pPdpAddr[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[12], sPdpCont.pPdpAddr[13], sPdpCont.pPdpAddr[14], sPdpCont.pPdpAddr[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpAddr[16], sPdpCont.pPdpAddr[17], sPdpCont.pPdpAddr[18], sPdpCont.pPdpAddr[19]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->ipv6_addr, ip6addr_ntoa(&ip6));
        }
        if (sPdpCont.nPdpDnsSize == 0)
        {
            OSI_LOGI(0, "nPdpDnsSize zero");
            sprintf(address->pref_dns_addr, "0.0.0.0");
            sprintf(address->standy_dns_addr, "0.0.0.0");
            IP6_ADDR(&ip6, 0, 0, 0, 0);
            sprintf(address->pref_v6_dns_addr, ip6addr_ntoa(&ip6));
            sprintf(address->standy_v6_dns_addr, ip6addr_ntoa(&ip6));
        }
        else
        {
            sprintf(address->pref_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[0], sPdpCont.pPdpDns[1], sPdpCont.pPdpDns[2], sPdpCont.pPdpDns[3]);
            sprintf(address->standy_dns_addr, "%d.%d.%d.%d", sPdpCont.pPdpDns[21], sPdpCont.pPdpDns[22], sPdpCont.pPdpDns[23], sPdpCont.pPdpDns[24]);

            uint32_t addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[4], sPdpCont.pPdpDns[5], sPdpCont.pPdpDns[6], sPdpCont.pPdpDns[7]));
            uint32_t addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[8], sPdpCont.pPdpDns[9], sPdpCont.pPdpDns[10], sPdpCont.pPdpDns[11]));
            uint32_t addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[12], sPdpCont.pPdpDns[13], sPdpCont.pPdpDns[14], sPdpCont.pPdpDns[15]));
            uint32_t addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[16], sPdpCont.pPdpDns[17], sPdpCont.pPdpDns[18], sPdpCont.pPdpDns[19]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->pref_v6_dns_addr, ip6addr_ntoa(&ip6));

            addr0 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[25], sPdpCont.pPdpDns[26], sPdpCont.pPdpDns[27], sPdpCont.pPdpDns[28]));
            addr1 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[29], sPdpCont.pPdpDns[30], sPdpCont.pPdpDns[31], sPdpCont.pPdpDns[32]));
            addr2 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[33], sPdpCont.pPdpDns[34], sPdpCont.pPdpDns[35], sPdpCont.pPdpDns[36]));
            addr3 = PP_HTONL(LWIP_MAKEU32(sPdpCont.pPdpDns[37], sPdpCont.pPdpDns[38], sPdpCont.pPdpDns[39], sPdpCont.pPdpDns[40]));
            IP6_ADDR(&ip6, addr0, addr1, addr2, addr3);
            sprintf(address->standy_v6_dns_addr, ip6addr_ntoa(&ip6));
        }
    }
    break;

    default:
    {
        OSI_LOGI(0, "Unknown type");
        memset(address, 0, sizeof(tb_data_ip_addr));
        address->nPdpType = TB_DATA_PDP_TYPE_UNUSED;
    }
    break;
    }

    return TB_SUCCESS;
}

//SIM module
int tb_sim_reg_callback(tb_sim_callback_fun fun)
{
    OSI_LOGI(0, "tb_sim_reg_callback");
    g_tb_sim_callback_fun = fun;
    return TB_SUCCESS;
}

int tb_sim_get_sim_state(tb_sim_status *istate)
{
    OSI_LOGI(0, "tb_sim_get_sim_state, g_tb_sim_status: 0x%x", g_tb_sim_status);

    if (istate == NULL)
    {
        return TB_FAILURE;
    }

    if (g_tb_sim_status == 0xff)
    {
        return TB_FAILURE;
    }

    *istate = g_tb_sim_status;

    return TB_SUCCESS;
}

void tb_IMSItoASC(uint8_t *InPut, uint8_t *OutPut, uint8_t *OutLen)
{
    uint8_t i;

    if (InPut[0] == 0)
    {
        *OutLen = 0;
        return;
    }
    OutPut[0] = ((InPut[1] & 0xF0) >> 4) + 0x30;
    for (i = 2; i < InPut[0] + 1 && 2 * (i - 1) < *OutLen; i++)
    {
        OutPut[2 * (i - 1) - 1] = (InPut[i] & 0x0F) + 0x30;
        OutPut[2 * (i - 1)] = ((InPut[i] & 0xF0) >> 4) + 0x30;
    }
    OutPut[2 * i - 1] = 0x00;
    *OutLen = 2 * i - 1;
}

int tb_sim_get_imsi(char *imsi, int size)
{
    OSI_LOGI(0, "tb_sim_get_imsi, g_tb_imsi_valid is %d", g_tb_imsi_valid);
    if (g_tb_imsi_valid == 0)
        return TB_FAILURE;

    strncpy(imsi, (const char *)g_tb_imsi, size);

    return TB_SUCCESS;
}

int tb_sim_get_imei(char *imei, int size)
{
    uint8_t nSim = 0;
    uint8_t nImei[16] = {
        0,
    };
    int nImeiLen = nvmReadImei(nSim, (nvmImei_t *)nImei);

    if (nImeiLen == -1)
    {
        OSI_LOGI(0, "tb_sim_get_imei, nv fail");
        return TB_FAILURE;
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0, "imei:%s", (const char *)nImei);
    strncpy(imei, (const char *)nImei, size);
    return TB_SUCCESS;
}

int tb_sim_get_phone_num(char *pBuf, int size)
{
    OSI_LOGI(0, "tb_sim_get_phone_num");
    if (g_tb_own_num_valid == 0)
    {
        OSI_LOGI(0, "not valid");
        return TB_FAILURE;
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0, "phone num is: %s", g_tb_own_num);
    strncpy(pBuf, (const char *)g_tb_own_num, size);
    return TB_SUCCESS;
}

//SMS module
int tb_sms_reg_callback(tb_sms_callback_fun fun)
{
    OSI_LOGI(0, "tb_sms_reg_callback");
    g_tb_sms_callback_fun = fun;
    return TB_SUCCESS;
}

int tb_sms_get_contents_cb(tb_sms_received_contents_cb fun)
{
    OSI_LOGI(0, "tb_sms_reg_callback");
    g_tb_sms_received_contents_cb = fun;
    return TB_SUCCESS;
}

static uint8_t tb_getPduDcs(uint8_t *data)
{
    uint8_t *tem = &data[data[0] + 1];
    return (tem[1 + (uint8_t)(tem[1] / 2 + tem[1] % 2 + 2) + 1]);
}

static bool tb_sms_IsValidPhoneNumber(const char *s, uint8_t size, bool *bIsInternational)
{
    if (s == NULL || size == 0)
        return false;

    *bIsInternational = false;
    const char *end = s + size;
    if (*s == '+')
    {
        *bIsInternational = true;
        s++;
    }

    while (s < end)
    {
        char c = *s++;
        if (!(c == '#' || c == '*' || (c >= '0' && c <= '9')))
            return false;
    }
    return true;
}

static void tb_smsNumber2Str(const uint8_t *bcd, unsigned size, unsigned type, char *s)
{
    if (type == 0xD0)
    {
        cfwDecode7Bit(bcd, s, size);
    }
    else
    {
        if (type == CFW_TELNUMBER_TYPE_INTERNATIONAL)
            *s++ = '+';
        cfwBcdToDialString(bcd, size, s);
    }
}

int tb_sms_send_sms(tb_sms_msg_type *msg)
{
    uint8_t nSim = 0;
    uint8_t uti = 0;
    uint8_t *send_data = NULL;
    uint32_t ret;
    int i = 0;
    int send_size = 0;
    bool bUnicode = false;
    bool bIsInternational = false;
    CFW_DIALNUMBER_V2 destNum;
    CFW_SMS_PARAMETER sInfo;

    CFW_CfgGetDefaultSmsParam(&sInfo, nSim);

    OSI_LOGI(0, "tb_sms_send_sms");
    //destination address
    if (!tb_sms_IsValidPhoneNumber((const char *)msg->dest_num, strlen((const char *)msg->dest_num), &bIsInternational))
    {
        OSI_LOGI(0, "tb_sms_send_sms num invalid!!!");
        return TB_FAILURE;
    }

    if (bIsInternational)
    {
        destNum.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    }
    else
    {
        destNum.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
    }

    destNum.nDialNumberSize = cfwDialStringToBcd((const void *)msg->dest_num, strlen((const char *)msg->dest_num), destNum.pDialNumber);

    //data
    OSI_LOGI(0, "tb_sms_send_sms msg->msg_len = %d \n", msg->msg_len);

    send_data = mlConvertStr((const char *)msg->msg_content, msg->msg_len, ML_UTF8, ML_UTF16BE, &send_size);
    if (send_data == NULL)
    {
        return TB_FAILURE;
    }

    if ((send_size == msg->msg_len) && (0 == memcmp(send_data, msg->msg_content, send_size)))
    {
        strncpy((char *)send_data, (const char *)msg->msg_content, msg->msg_len);
    }

    for (i = 0; i < send_size; i += 2)
    {
        if (0x0 != send_data[i])
        {
            bUnicode = true;
            break;
        }
    }
    OSI_LOGI(0, "tb_sms_send_sms bUnicode = %d \n", bUnicode);

    if (bUnicode)
    {
        sInfo.dcs = 8;
    }
    else
    {
        send_data = mlConvertStr((const char *)msg->msg_content, msg->msg_len, ML_UTF8, ML_GSM, &send_size);
        if (send_data == NULL)
        {
            return TB_FAILURE;
        }

        sInfo.dcs = 0;
    }
    CFW_CfgSetDefaultSmsParam(&sInfo, nSim);

    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_sms_send_sms send %s", send_data);
    OSI_LOGI(0, "tb_sms_send_sms send_size = %d \n", send_size);

    if (send_size > 420)
    {
        free(send_data);
        return TB_FAILURE;
    }

    uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Sms_Send_Rsp, NULL);
    ret = CFW_SmsSendMessage_V2(&destNum, send_data,
                                send_size, uti, nSim);
    if (ret != 0)
    {
        free(send_data);
        return TB_FAILURE;
    }
    free(send_data);
    return TB_SUCCESS;
}

int tb_sms_get_sms_withid(int id)
{
    uint32_t ret;
    uint8_t nSim = 0;
    uint8_t uti = 0;

    OSI_LOGI(0, "tb_sms_get_sms_withid id = %d", id);
    uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Sms_Read_Rsp, NULL);
    ret = CFW_SmsReadMessage(CFW_SMS_STORAGE_ME, 1, id, uti, nSim);
    if (ret != 0)
    {
        return TB_FAILURE;
    }
    return TB_FAILURE;
}

int tb_sms_delete_sms(int MessageIndex)
{
    uint32_t ret;
    uint8_t nSim = 0;
    uint8_t uti = 0;

    uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Sms_Delete_Rsp, NULL);
    ret = CFW_SmsDeleteMessage(MessageIndex, CFW_SMS_STORED_STATUS_STORED_ALL, CFW_SMS_STORAGE_ME, 1, uti, nSim);
    if (ret != 0)
    {
        return TB_FAILURE;
    }
    return TB_SUCCESS;
}

//VOICE module
int tb_voice_reg_callback(tb_voice_callback_fun fun)
{
    OSI_LOGI(0, "tb_voice_reg_callback");
    g_tb_voice_callback_fun = fun;
    return TB_SUCCESS;
}

static bool tb_voice_GetUriCallNumber(
    const char *uri_ptr,            //[IN]
    char uriNum[URI_MAX_NUMBER + 1] //[OUT]
)
{
    const char *pURINumStart = NULL;
    const char *pURINumEnd = NULL;
    const char *pURIHostStart = NULL;
    const char *pURINumSemic = NULL; //semicolon
    int urinum_len;

    if (uri_ptr == NULL || uriNum == NULL)
    {
        return false;
    }

    pURINumStart = strstr(uri_ptr, SIP_STR);
    if (pURINumStart != NULL)
    {
        pURINumStart += strlen(SIP_STR);
    }
    else
    {
        return false;
    }

    //get host string start pointer
    pURIHostStart = strstr(uri_ptr, "@");
    pURINumSemic = strstr(uri_ptr, ";");
    if (NULL != pURINumSemic)
    {
        if (NULL == pURIHostStart)
        {
            pURINumEnd = pURINumSemic;
        }
        else if (0 > (pURINumSemic - pURIHostStart))
        {
            pURINumEnd = pURINumSemic;
        }
        else
        {
            pURINumEnd = pURIHostStart;
        }
    }
    else if (NULL != pURIHostStart)
    {
        pURINumEnd = pURIHostStart;
    }
    else
    {
        return false;
    }

    if (pURINumEnd != pURINumStart)
    {
        urinum_len = pURINumEnd - pURINumStart;
        if (urinum_len > URI_MAX_NUMBER)
        {
            urinum_len = URI_MAX_NUMBER;
        }

        strncpy(
            uriNum,
            pURINumStart,
            urinum_len);
    }

    return false;
}

static bool tb_voice_isEmcNum(uint8_t *bcd, uint8_t len)
{
    static uint8_t emcNum[][2] = {
        {0x11, 0xF2},
        {0x19, 0xF1},
    };

    if (len != 2)
        return false;

    for (size_t n = 0; n < OSI_ARRAY_SIZE(emcNum); n++)
    {
        if (memcmp(bcd, emcNum[n], len) == 0)
            return true;
    }
    return false;
}

int tb_voice_dial_call(char *call_number)
{
    uint8_t nSim = 0;
    uint8_t uti = 0;
    uint32_t ret = 0;
    int bcd_len;
    int call_number_len = strlen(call_number);
    CFW_DIALNUMBER_V2 dial_num = {
        0,
    };

    //dial number
    if ('+' == call_number[0])
    {
        bcd_len = cfwDialStringToBcd(call_number + 1, call_number_len - 1, dial_num.pDialNumber);
    }
    else if ((('0' == call_number[0]) && ('0' == call_number[1]) && (!('0' == call_number[2] && call_number_len == 3))))
    {
        bcd_len = cfwDialStringToBcd(call_number + 2, call_number_len - 2, dial_num.pDialNumber);
    }
    else
    {
        bcd_len = cfwDialStringToBcd(call_number, call_number_len, dial_num.pDialNumber);
    }
    if (bcd_len < 0 || bcd_len > 50)
    {
        return TB_FAILURE;
    }
    dial_num.nDialNumberSize = bcd_len;

    //number type
    if (('+' == call_number[0]) || (('0' == call_number[0]) && ('0' == call_number[1])))
    {
        dial_num.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    }
    else
    {
        dial_num.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
    }

    //emergency or narmal call
    CFW_SIM_STATUS sim_status = CFW_GetSimStatus(nSim);
    bool is_emc = (sim_status == CFW_SIM_NORMAL) && tb_voice_isEmcNum(dial_num.pDialNumber, dial_num.nDialNumberSize);
    if (is_emc)
    {
        ret = CFW_CcEmcDial(dial_num.pDialNumber, dial_num.nDialNumberSize, nSim);
    }
    else
    {
        CFW_GetFreeUTI(0, &uti);
        ret = CFW_CcInitiateSpeechCall_V2(&dial_num, uti, nSim);
    }
    if (0 != ret)
    {
        return TB_FAILURE;
    }

    return TB_SUCCESS;
}

int tb_voice_answer_call()
{
    uint8_t nSim = 0;
    uint32_t ret = 0;

    ret = CFW_CcAcceptSpeechCallEx(nSim);
    if (0 != ret)
    {
        return TB_FAILURE;
    }
    return TB_SUCCESS;
}

int tb_voice_hungup_call()
{
    int i = 0;
    uint8_t nSim = 0;
    uint8_t cnt = 0;
    uint32_t ret = 0;

    CFW_CC_CURRENT_CALL_INFO call_info[AT_CC_MAX_NUM];

    ret = CFW_CcGetCurrentCall(call_info, &cnt, nSim);
    OSI_LOGI(0, "tb_voice_hungup_call cnt = %d, ret = 0x%x", cnt, ret);
    for (i = 0; i < cnt; i++)
    {
        OSI_LOGI(0, "tb_voice_hungup_call status = %d", call_info[i].status);

        if (CFW_CM_STATUS_WAITING != call_info[i].status)
        {
            ret = CFW_CcReleaseCallX(call_info[i].idx, nSim);
            if (0 != ret)
            {
                return TB_FAILURE;
            }
        }
    }
    return TB_SUCCESS;
}

int tb_voice_switch_audio_channel(int audio_channel)
{
    int audch = 0;

    if (0 == audio_channel)
    {
        audch = 2;
    }
    else if (1 == audio_channel)
    {
        audch = 1;
    }
    else
    {
        return TB_FAILURE;
    }
    audevSetOutput((audevOutput_t)audch);
    return TB_SUCCESS;
}

int tb_device_get_modemserialnumber(char *msn, int size)
{
    uint8_t nSim = 0;
    uint8_t sn[25] = {
        0,
    };

    int len = nvmReadSN(nSim, &sn, 24);
    if (len == -1)
    {
        return TB_FAILURE;
    }

    strncpy(msn, (const char *)sn, size);

    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_get_modemserialnumber msn %s", msn);

    return TB_SUCCESS;
}

int tb_device_get_hwversion(char *hdver, int size)
{
    uint8_t *basebandVersion = CFW_EmodGetBaseBandVersionV1();

    if (basebandVersion == NULL)
    {
        return TB_FAILURE;
    }

    strncpy(hdver, (const char *)basebandVersion, size);

    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_get_hwversion basebandVersion %s", basebandVersion);

    return TB_SUCCESS;
}

int tb_device_getversion(char *ver, int size)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_getversion GMI %s", AT_GMI_ID);
    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_getversion GMM %s", AT_GMM_ID);
    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_getversion GMR %s", AT_GMR_ID);
    strncpy(ver, (const char *)AT_GMI_ID, size - 1);
    if (size - strlen(AT_GMI_ID) > 2)
    {
        strcat(ver, " ");
    }

    if (size - strlen(ver) > strlen(AT_GMM_ID) + 1)
    {
        strcat(ver, (const char *)AT_GMM_ID);
    }

    if (size - strlen(ver) > 2)
    {
        strcat(ver, " ");
    }

    if (size - strlen(ver) > strlen(AT_GMR_ID) + 1)
    {
        strcat(ver, (const char *)AT_GMR_ID);
    }
    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_getversion ver %s", ver);
    return TB_SUCCESS;
}

static uint16_t tb_ConvertICCID(uint8_t *pInput, uint16_t nInputLen, uint8_t *pOutput)
{
    uint16_t i;
    uint8_t *pBuffer = pOutput;
    for (i = 0; i < nInputLen; i++)
    {
        uint8_t high4bits = pInput[i] >> 4;
        uint8_t low4bits = pInput[i] & 0x0F;

        if (low4bits < 0x0A)
            *pOutput++ = low4bits + '0'; // 0 - 0x09
        else                             // 0x0A - 0x0F
            *pOutput++ = low4bits - 0x0A + 'A';

        if (high4bits < 0x0A)
            *pOutput++ = high4bits + '0'; // 0 - 9
        else                              // 0x0A - 0x0F
            *pOutput++ = high4bits - 0x0A + 'A';
    }
    OSI_LOGI(0, "shane: return size = %d", pOutput - pBuffer);
    return pOutput - pBuffer;
}

int tb_device_get_iccid(char *iccid, int size)
{
    uint8_t nSim = 0;
    uint8_t *pICCID = CFW_GetICCID(nSim);

    if (pICCID != NULL)
    {
        uint8_t ICCID[21] = {0};
        OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", 10, pICCID);
        OSI_LOGI(0, "shane: at_hex2ascii");
        tb_ConvertICCID(pICCID, 10, ICCID);
        OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", 20, ICCID);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "pICCID1 =%s\n", ICCID);
        strncpy(iccid, (const char *)ICCID, size);
    }
    else
    {
        return TB_FAILURE;
    }

    OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_device_get_iccid iccid %s", iccid);

    return TB_SUCCESS;
}
int tb_device_set_rtc_timer(unsigned long seconds, tb_timer_callbck pCallback, tb_timer_p *pTimer)
{
    tb_timer_p timer;
    if (pTimer == NULL || pCallback == NULL)
    {
        OSI_LOGI(0, "tb_device_set_rtc_timer NULL parameter");
        return TB_FAILURE;
    }
    timer = osiTimerCreate(NULL, pCallback, NULL);
    osiTimerSetCallback(timer, OSI_TIMER_IN_ISR, pCallback, timer);
    osiTimerStart(timer, seconds * 1000);
    *pTimer = timer;
    OSI_LOGI(0, "tb_device_set_rtc_timer, pTimer: 0x%x", pTimer);
    return TB_SUCCESS;
}

int tb_device_cancel_timer(tb_timer_p pTimer)
{
    if (pTimer == NULL)
    {
        OSI_LOGI(0, "tb_device_cancel_timer NULL parameter");
        return TB_FAILURE;
    }
    OSI_LOGI(0, "tb_device_cancel_timer, pTimer: 0x%x", pTimer);
    osiTimerStop(pTimer);
    osiTimerDelete(pTimer);
    return TB_SUCCESS;
}

int tb_device_shutdown_system(void)
{
    OSI_LOGI(0, "tb_device_shutdown_system");
    g_cfw_exit_flag = 0;
    CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF);
    return TB_SUCCESS;
}

int tb_device_reboot_system(void)
{
    OSI_LOGI(0, "tb_device_reboot_system");
    //osiShutdown(OSI_SHUTDOWN_RESET);
    g_cfw_exit_flag = 1;
    CFW_ShellControl(CFW_CONTROL_CMD_POWER_OFF);
    return TB_SUCCESS;
}

int tb_device_modem_wakeup_mcu(uint32_t gpio_num)
{
    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_OUTPUT,
        .out_level = false,
    };
    drvGpio_t *d = drvGpioOpen(gpio_num, &cfg, NULL, NULL);
    if (d == NULL)
        return TB_FAILURE;

    for (uint8_t i = 1; i <= 6; ++i)
    {
        drvGpioWrite(d, (i % 2));
        osiThreadSleep(50);
    }

    drvGpioClose(d);
    return TB_SUCCESS;
}

int tb_device_enable_sleep(bool enable)
{
    static osiPmSource_t *gTbPmLock = NULL;
    if (gTbPmLock == NULL)
    {
        const uint32_t name = OSI_MAKE_TAG('T', 'B', 'D', 'V');
        gTbPmLock = osiPmSourceCreate(name, NULL, NULL);
        if (gTbPmLock == NULL)
            return TB_FAILURE;
    }
    if (enable)
        osiPmWakeUnlock(gTbPmLock);
    else
        osiPmWakeLock(gTbPmLock);
    return TB_SUCCESS;
}

int tb_device_get_update_result(void)
{
    fupdateStatus_t status = fupdateGetStatus();
    if (status == FUPDATE_STATUS_FINISHED)
    {
        char *old_version = NULL;
        char *new_version = NULL;
        if (fupdateGetVersion(&old_version, &new_version))
        {
            OSI_LOGXI(OSI_LOGPAR_SS, 0, "FUPDATE: %s -> %s",
                      old_version, new_version);
            free(old_version);
            free(new_version);
        }
        fupdateInvalidate(true);
        return TB_SUCCESS;
    }
    return TB_FAILURE;
}

int tb_device_start_update_firmware(void)
{
    if (!fupdateSetReady(NULL))
    {
        OSI_LOGE(0, "Fota Error: not ready");
        return TB_FAILURE;
    }
    osiShutdown(OSI_SHUTDOWN_RESET);
    return TB_SUCCESS;
}

int tb_device_query_hu_ipaddr(char *hu_ipaddr, int ipaddr_buf_len)
{
    char temp[20] = {
        0,
    };
    struct netif *netinfo = NULL;

    OSI_LOGE(0, "tb_device_query_hu_ipaddr");
    if (hu_ipaddr == NULL)
    {
        OSI_LOGE(0, "NULL hu_ipaddr");
        return TB_FAILURE;
    }
    NETIF_FOREACH(netinfo)
    {
        OSI_LOGE(0, "netinfo->link_mode:%d", netinfo->link_mode);
        if (netinfo->link_mode == NETIF_LINK_MODE_NETCARD)
        {
            sprintf(temp, "%s", ipaddr_ntoa(&(netinfo->ip_addr)));
            break;
        }
    }

    if (strlen(temp))
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0, "hu_ipaddr:%s", (const char *)temp);
        strncpy(hu_ipaddr, temp, ipaddr_buf_len);
    }
    else
    {
        strncpy(hu_ipaddr, "0.0.0.0", ipaddr_buf_len);
        return TB_FAILURE;
    }

    return TB_SUCCESS;
}
//CFW Eveent handle

static int tb_get_signal_bar_gsm(uint8_t nCsq)
{
    if (nCsq >= TB_GSM_SIGNAL_BAR_5_VALUE)
    {
        return 5;
    }
    else if (nCsq >= TB_GSM_SIGNAL_BAR_4_VALUE)
    {
        return 4;
    }
    else if (nCsq >= TB_GSM_SIGNAL_BAR_3_VALUE)
    {
        return 3;
    }
    else if (nCsq >= TB_GSM_SIGNAL_BAR_2_VALUE)
    {
        return 2;
    }
    else if (nCsq >= TB_GSM_SIGNAL_BAR_1_VALUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int tb_get_signal_bar_lte(uint8_t nRscp)
{
    if (nRscp >= TB_LTE_SIGNAL_BAR_5_VALUE)
    {
        return 5;
    }
    else if (nRscp >= TB_LTE_SIGNAL_BAR_4_VALUE)
    {
        return 4;
    }
    else if (nRscp >= TB_LTE_SIGNAL_BAR_3_VALUE)
    {
        return 3;
    }
    else if (nRscp >= TB_LTE_SIGNAL_BAR_2_VALUE)
    {
        return 2;
    }
    else if (nRscp >= TB_LTE_SIGNAL_BAR_1_VALUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int tb_handle_nw_signal_quality_ind(const CFW_EVENT *cfw_event)
{
    uint8_t nSim = cfw_event->nFlag;
    uint8_t nRat = CFW_NWGetStackRat(nSim);
    int nSignalBar = 0xFF;
    char *sSignalBar = NULL;

    OSI_LOGI(0, "tb_handle_nw_signal_quality_ind nRat:%d, nParam1:%d", nRat, cfw_event->nParam1);
    if (nRat == 2)
    { //GSM
        uint8_t nCsq = (uint8_t)cfw_event->nParam1;

        nSignalBar = tb_get_signal_bar_gsm(nCsq);
    }
    else if (nRat == 4)
    { //LTE
        uint8_t nRscp = (uint8_t)cfw_event->nParam1;
        nSignalBar = tb_get_signal_bar_lte(nRscp);
    }
    else
    {
        OSI_LOGI(0, "tb_handle_nw_signal_quality_ind Unkonwn rat");
    }
    switch (nSignalBar)
    {
    case 5:
        sSignalBar = TB_SIGNAL_BAR_5;
        break;
    case 4:
        sSignalBar = TB_SIGNAL_BAR_4;
        break;
    case 3:
        sSignalBar = TB_SIGNAL_BAR_3;
        break;
    case 2:
        sSignalBar = TB_SIGNAL_BAR_2;
        break;
    case 1:
        sSignalBar = TB_SIGNAL_BAR_1;
        break;
    case 0:
        sSignalBar = TB_SIGNAL_BAR_0;
        break;
    default:
        break;
    }
    if (g_tb_wan_network_info_cb != NULL && sSignalBar != NULL)
    {

        g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_SIG_BAR, (const char *)sSignalBar);
    }
    return TB_SUCCESS;
}

static void tb_IMSItoHomePlmn(uint8_t *InPut, uint8_t *OutPut, uint8_t *OutLen)
{
    uint8_t i;
    OutPut[0] = ((InPut[1] & 0xF0) >> 4);
    for (i = 2; i < InPut[0] + 1; i++)
    {
        OutPut[2 * (i - 1) - 1] = (InPut[i] & 0x0F);
        OutPut[2 * (i - 1)] = ((InPut[i] & 0xF0) >> 4);
    }
    OutPut[2 * InPut[0] - 1] = 0x00;
    *OutLen = 2 * InPut[0] - 1;

    return;
}

static int tb_handle_nw_status_ind(const CFW_EVENT *cfw_event)
{
    uint8_t mode;
    uint8_t OperatorId[6];
    uint8_t nIMSI[15] = {
        0,
    };
    uint8_t nTempOperatorId[16] = {
        0x0f,
    };
    uint8_t nHomeOperatorId[6] = {
        0x0f,
    };
    uint8_t nIMSILen = 0;
    uint8_t nSim = cfw_event->nFlag;
    uint8_t nMNCLen = 0;
    CFW_NW_STATUS_INFO NwStatus;
    CFW_NW_STATUS_INFO GprsStatus;
    char *sRoamingStatus = NULL;

    unsigned char *oper_name = NULL;
    unsigned char *oper_short_name = NULL;

    uint8_t nRat = 0;
    char *sNetType = NULL;

    char *sCsPsStatus = NULL;

    uint8_t i;
    uint8_t bCellIdValid = 0;
    char sCellId[9] = {
        0,
    };
    char sTempCellId[3] = {
        0,
    };

    OSI_LOGI(0, "tb_handle_nw_status_ind");

    CFW_NwGetCurrentOperator(OperatorId, &mode, nSim);

    CFW_CfgGetIMSI(nIMSI, nSim);
    tb_IMSItoHomePlmn(nIMSI, nTempOperatorId, &nIMSILen);
    CFW_GetMNCLen(&nMNCLen, nSim);
    memset(nHomeOperatorId, 0x0f, 6);
    memcpy(nHomeOperatorId, nTempOperatorId, 3 + nMNCLen);

    CFW_NwGetStatus(&NwStatus, nSim);
    CFW_GprsGetstatus(&GprsStatus, nSim);

    OSI_LOGI(0, "NwStatus.nStatus is %d", NwStatus.nStatus);
    if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME)
    {
        sRoamingStatus = TB_ROAMING_STATUS_HOME;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {
        if (memcmp((const void *)OperatorId, (const void *)nHomeOperatorId, 3) == 0)
        {
            sRoamingStatus = TB_ROAMING_STATUS_INTERNAL;
        }
        else
        {
            sRoamingStatus = TB_ROAMING_STATUS_INTERNATIONAL;
        }
    }

    CFW_CfgNwGetOperatorName(OperatorId, &oper_name, &oper_short_name);

    nRat = CFW_NWGetStackRat(nSim);
    if (nRat == 2)
    {
        sNetType = TB_NET_TYPE_GSM;
    }
    else if (nRat == 4)
    {
        sNetType = TB_NET_TYPE_LTE;
    }

    if ((NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING) && (GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING))
    {

        sCsPsStatus = TB_CS_PS_STAUS_CS_PS;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || NwStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {

        sCsPsStatus = TB_CS_PS_STAUS_CS_ONLY;
    }
    else if (GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_HOME || GprsStatus.nStatus == CFW_NW_STATUS_REGISTERED_ROAMING)
    {
        sCsPsStatus = TB_CS_PS_STAUS_PS_ONLY;
    }
    else if (NwStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED || GprsStatus.nStatus == CFW_NW_STATUS_REGISTRATION_DENIED)
    {
        sCsPsStatus = TB_CS_PS_STAUS_CAMPED;
    }

    for (i = 0; i < 4; i++)
    {
        if (NwStatus.nCellId[i] != 0)
        {
            bCellIdValid = 1;
        }
        if (bCellIdValid == 1)
        {
            memset(sTempCellId, 0, sizeof(sTempCellId));
            sprintf(sTempCellId, "%02x", NwStatus.nCellId[i]);
            strcat(sCellId, sTempCellId);
        }
    }

    if (g_tb_wan_network_info_cb != NULL)
    {
        if (sRoamingStatus)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_nw_status_ind, sRoamingStatus is %s", sRoamingStatus);
            g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_ROAMING_STATUS, (const char *)sRoamingStatus);
        }

        if (oper_short_name != NULL)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_nw_status_ind, oper_short_name is %s", oper_short_name);
            g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_NET_PROVIDER, (const char *)oper_short_name);
        }

        if (sNetType != NULL)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_nw_status_ind, sNetType is %s", sNetType);
            g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_NET_TYPE, (const char *)sNetType);
        }

        if (sCsPsStatus)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_nw_status_ind, sCsPsStatus is %s", sCsPsStatus);
            g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_CS_PS_STATUS, (const char *)sCsPsStatus);
        }

        if (bCellIdValid)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_nw_status_ind, oper_short_name is %s", oper_short_name);
            g_tb_wan_network_info_cb(TB_WAN_NOTIFY_TYPE_CELL_ID_INFO, (const char *)sCellId);
        }
    }
    return TB_SUCCESS;
}
static void tb_handle_Gprs_Act_Rsp(void *ctx, const osiEvent_t *event)
{
    //uint8_t nSim = cfw_event->nFlag;
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    OSI_LOGI(0, "tb_handle_Gprs_Act_Rsp: nType: %d, nParam1:%d", cfw_event->nType, cfw_event->nParam1);
    if (cfw_event->nParam1 != TB_PROFILE_ID_PUBLIC && cfw_event->nParam1 != TB_PROFILE_ID_PRIVATE)
    {
        return;
    }

    if (cfw_event->nType == CFW_GPRS_ACTIVED)
    {

        osiEvent_t tcpip_event = *(osiEvent_t *)cfw_event;
        tcpip_event.id = EV_TCPIP_CFW_GPRS_ACT;
        CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
        tcpip_cfw_event->nUTI = 0;
        osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);
        g_tb_data_dial_status[cfw_event->nParam1 - 1] = TB_DIAL_STATUS_CONNECTED;

        if (g_tb_data_callback_fun != NULL)
        {
            g_tb_data_callback_fun(cfw_event->nParam1, TB_DIAL_STATUS_CONNECTED);
        }
    }
    else if (cfw_event->nType == CFW_GPRS_DEACTIVED)
    {
        uint8_t nCid, nSimId;
        //Delete ppp Session first
        OSI_LOGI(0, "ppp session will be deleted!!! GprsActRsp");
        nCid = cfw_event->nParam1;
        nSimId = cfw_event->nFlag;
        pppSessionDeleteByNetifDestoryed(nSimId, nCid);

        osiEvent_t tcpip_event = *(osiEvent_t *)cfw_event;
        tcpip_event.id = EV_TCPIP_CFW_GPRS_DEACT;
        CFW_EVENT *tcpip_cfw_event = (CFW_EVENT *)&tcpip_event;
        tcpip_cfw_event->nUTI = 0;
        osiEventSend(netGetTaskID(), (const osiEvent_t *)&tcpip_event);

        g_tb_data_dial_status[cfw_event->nParam1 - 1] = TB_DIAL_STATUS_DISCONNECTED;

        if (g_tb_data_callback_fun != NULL)
        {
            g_tb_data_callback_fun(cfw_event->nParam1, TB_DIAL_STATUS_DISCONNECTED);
        }
    }
}

static int tb_handle_Gprs_Active_Ind(const CFW_EVENT *cfw_event)
{
    //uint8_t nSim = cfw_event->nFlag;
    OSI_LOGI(0, "tb_handle_Gprs_Active_Ind: nType: %d, nParam1:%d", cfw_event->nType, cfw_event->nParam1);
    if (cfw_event->nParam1 != TB_PROFILE_ID_PUBLIC && cfw_event->nParam1 != TB_PROFILE_ID_PRIVATE)
    {
        return TB_SUCCESS;
    }

    if (g_tb_data_callback_fun != NULL)
    {
        g_tb_data_callback_fun(cfw_event->nParam1, TB_DIAL_STATUS_CONNECTED);
    }

    return TB_SUCCESS;
}

static int tb_handle_Gprs_DeActive_Ind(const CFW_EVENT *cfw_event)
{
    //uint8_t nSim = cfw_event->nFlag;
    OSI_LOGI(0, "tb_handle_Gprs_DeActive_Ind: nType: %d, nParam1:%d", cfw_event->nType, cfw_event->nParam1);
    if (cfw_event->nParam1 != TB_PROFILE_ID_PUBLIC && cfw_event->nParam1 != TB_PROFILE_ID_PRIVATE)
    {
        return TB_SUCCESS;
    }

    g_tb_data_dial_status[cfw_event->nParam1 - 1] = TB_DIAL_STATUS_DISCONNECTED;

    if (g_tb_data_callback_fun != NULL)
    {
        g_tb_data_callback_fun(cfw_event->nParam1, TB_DIAL_STATUS_DISCONNECTED);
    }

    return TB_SUCCESS;
}

static int tb_handle_Init_ind(const CFW_EVENT *cfw_event)
{
    OSI_LOGI(0, "tb_handle_Init_ind: nType: %d, nParam1: 0x%x", cfw_event->nType, cfw_event->nParam1);
    switch (cfw_event->nType)
    {
    case CFW_INIT_STATUS_NO_SIM:
        g_tb_sim_status = TB_SIM_STATUS_UNDETECTED;
        if (g_tb_sim_callback_fun)
        {
            g_tb_sim_callback_fun(TB_SIM_STATUS_UNDETECTED);
        }
        break;

    case CFW_INIT_STATUS_SIM_DROP:
        g_tb_sim_status = TB_SIM_STATUS_UNDETECTED;
        if (g_tb_sim_callback_fun)
        {
            g_tb_sim_callback_fun(TB_SIM_STATUS_UNDETECTED);
        }
        break;

    case CFW_INIT_STATUS_SIM:
        if (cfw_event->nParam1 & CFW_INIT_SIM_PIN1_READY)
        {
            uint8_t nSim = 0;
            uint16_t uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Get_Provider_Id_Rsp, NULL);
            g_tb_imsi_valid = 0;
            CFW_SimGetProviderId(uti, nSim);

            g_tb_sim_status = TB_SIM_STATUS_READY;
            if (g_tb_sim_callback_fun)
            {
                g_tb_sim_callback_fun(TB_SIM_STATUS_READY);
            }
        }
        else if (cfw_event->nParam1 & CFW_INIT_SIM_WAIT_PIN1)
        {
            g_tb_sim_status = TB_SIM_STATUS_WAIT_PIN;
            if (g_tb_sim_callback_fun)
            {
                g_tb_sim_callback_fun(TB_SIM_STATUS_WAIT_PIN);
            }
        }
        else if (cfw_event->nParam1 & CFW_INIT_SIM_WAIT_PUK1)
        {
            g_tb_sim_status = TB_SIM_STATUS_WAIT_PUK;
            if (g_tb_sim_callback_fun)
            {
                g_tb_sim_callback_fun(TB_SIM_STATUS_WAIT_PUK);
            }
        }
        else if (cfw_event->nParam1 & CFW_INIT_SIM_CARD_BLOCK)
        {
            g_tb_sim_status = TB_SIM_STATUS_UNDETECTED;
            if (g_tb_sim_callback_fun)
            {
                g_tb_sim_callback_fun(TB_SIM_STATUS_UNDETECTED);
            }
        }
        break;
    case CFW_INIT_STATUS_PBK:
    {
        uint8_t nSim = 0;
        uint16_t uti = tb_RequestUTI((osiEventCallback_t)tb_handle_Get_PBK_Entry_Rsp, NULL);
        CFW_SimGetPbkEntry(CFW_PBK_ON, 0x01, uti, nSim);
    }
    break;
    default:
        break;
    }

    return TB_SUCCESS;
}

static int tb_handle_Srv_Init_ind(const CFW_EVENT *cfw_event)
{
    uint8_t nSim = 0;

    OSI_LOGI(0, "tb_handle_SRV_Init_ind: nType: %d, nParam1: 0x%x", cfw_event->nType, cfw_event->nParam1);

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
    {
        return TB_FAILURE;
    }

    switch (cfw_event->nType)
    {
    case 0x10:
    {
        CFW_SMS_STORAGE_INFO sms_stor_info = {0};
        uint8_t nMeFull[CFW_SIM_COUNT] = {0, 0};
        uint8_t uOption = 0;
        uint8_t uNewSmsStorage = 0;

        CFW_CfgGetNewSmsOption(&uOption, &uNewSmsStorage, nSim);
        CFW_CfgSetNewSmsOption(uOption, CFW_SMS_STORAGE_ME, nSim);
        CFW_CfgSetSmsFormat(1, nSim);

        if (0 != CFW_CfgGetSmsStorageInfo(&sms_stor_info, CFW_SMS_STORAGE_ME, nSim))
        {
            return ERR_AT_CMS_MEM_FAIL;
        }

        if ((sms_stor_info.totalSlot == sms_stor_info.usedSlot) && (sms_stor_info.usedSlot != 0))
        {
            nMeFull[nSim] = 1;
        }

        if ((NULL != g_tb_sms_callback_fun) && (0 != sms_stor_info.totalSlot))
        {
            g_tb_sms_callback_fun(TB_SMS_NV_TOTAL_SMS, sms_stor_info.totalSlot);
        }

        if ((NULL != g_tb_sms_callback_fun) && (0 != sms_stor_info.unReadRecords))
        {
            g_SMS_Unread_Msg += sms_stor_info.unReadRecords;
            g_tb_sms_callback_fun(TB_SIM_NV_UNREAD_SMS, sms_stor_info.unReadRecords);
        }

        if ((NULL != g_tb_sms_callback_fun) && (0 != nMeFull[nSim]))
        {
            g_tb_sms_callback_fun(TB_SMS_FULL_FLAG, nMeFull[nSim]);
        }
    }
    break;

    case 0x11:
    {
        return TB_FAILURE;
    }
    break;

    default:
        break;
    }

    return TB_SUCCESS;
}

static void tb_handle_Get_Provider_Id_Rsp(void *ctx, const osiEvent_t *event)
{
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    OSI_LOGI(0, "tb_handle_Get_Provider_Id_Rsp: nType: %d, nParam2: 0x%x", cfw_event->nType, cfw_event->nParam2);

    if (cfw_event->nType == 0)
    {
        uint8_t nLen = cfw_event->nParam2;
        g_tb_imsi_valid = 1;
        memset(g_tb_imsi, 0, sizeof(g_tb_imsi));
        if (nLen >= sizeof(g_tb_imsi))
        {
            nLen = sizeof(g_tb_imsi) - 1;
        }
        strncpy(g_tb_imsi, (const char *)cfw_event->nParam1, nLen);

        OSI_LOGXI(OSI_LOGPAR_S, 0, "imsi:%s", (const char *)g_tb_imsi);
    }
}

static void tb_handle_Get_PBK_Entry_Rsp(void *ctx, const osiEvent_t *event)
{
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    OSI_LOGI(0, "tb_handle_Get_PBK_Entry_Rsp,  nType is %d, nParam2 is 0x%x", cfw_event->nType, cfw_event->nParam2);
    if (cfw_event->nType == 0 && (cfw_event->nParam2 >> 24) == CFW_PBK_ON)
    {
        CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)cfw_event->nParam1;

        if (pGetPBKEntry != NULL)
        {
            uint8_t i;
            uint8_t nNumLen;
            uint8_t nOct = 0;

            nNumLen = pGetPBKEntry->nNumberSize;
            if (nNumLen > 20)
            {
                nNumLen = 20;
            }
            OSI_LOGI(0, "nType is %d", pGetPBKEntry->nType);

            pGetPBKEntry->pNumber = (uint8_t *)pGetPBKEntry + (uint32_t)sizeof(CFW_SIM_PBK_ENTRY_INFO);

            memset(g_tb_own_num, 0, sizeof(g_tb_own_num));
            if (pGetPBKEntry->nType == CFW_TELNUMBER_TYPE_INTERNATIONAL)
            {
                strcpy(g_tb_own_num, "+");
                nOct = 1;
            }
            for (i = 0; i < nNumLen; i++)
            {
                if ((pGetPBKEntry->pNumber[i] & 0x0f) == 0xf)
                {
                    break;
                }
                sprintf(&g_tb_own_num[2 * i + nOct], "%d", pGetPBKEntry->pNumber[i] & 0x0f);
                if (((pGetPBKEntry->pNumber[i] & 0x0f0) >> 4) == 0xf)
                {
                    break;
                }
                sprintf(&g_tb_own_num[2 * i + 1 + nOct], "%d", (pGetPBKEntry->pNumber[i] & 0x0f0) >> 4);
            }

            g_tb_own_num_valid = 1;

            OSI_LOGXI(OSI_LOGPAR_S, 0, "my phone number:%s", (const char *)g_tb_own_num);

            for (i = 0; i < nNumLen; i++)
            {
                OSI_LOGI(0, "pNumber[%d] 0x%x", i, pGetPBKEntry->pNumber[i]);
            }
        }
        else
        {
            OSI_LOGI(0, "tb_handle_Get_PBK_Entry_Rsp,  NULL record");
        }
    }
    return;
}

static void tb_handle_Sms_Send_Rsp(void *ctx, const osiEvent_t *event)
{
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    uint8_t nType = cfw_event->nType;

    OSI_LOGI(0, "tb_handle_Sms_Send_Rsp,  nType is 0x%x", cfw_event->nType);

    if ((NULL != g_tb_sms_callback_fun) && (0 == nType))
    {
        OSI_LOGI(0, "tb_handle_Sms_Send_Rsp send sms sucess!\n");
        g_tb_sms_callback_fun(TB_SMS_SEND_RESULT, 1);
    }
    else if ((NULL != g_tb_sms_callback_fun) && (0xf0 == nType))
    {
        OSI_LOGI(0, "tb_handle_Sms_Send_Rsp send sms failed!");
        g_tb_sms_callback_fun(TB_SMS_SEND_RESULT, 0);
    }
    return;
}

static void tb_handle_Sms_Delete_Rsp(void *ctx, const osiEvent_t *event)
{
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    uint8_t nType = cfw_event->nType;

    OSI_LOGI(0, "tb_handle_Sms_Delete_Rsp,  nType is 0x%x", cfw_event->nType);

    if ((NULL != g_tb_sms_callback_fun) && (0 == nType))

    {
        OSI_LOGI(0, "tb_handle_Sms_Delete_Rsp delete sms sucess!\n");
        g_tb_sms_callback_fun(TB_SMS_DELETE_RESULT, 3);
    }
    else if ((NULL != g_tb_sms_callback_fun) && (0xf0 == nType))
    {
        OSI_LOGI(0, "tb_handle_Sms_Delete_Rsp delete sms failed!");
        g_tb_sms_callback_fun(TB_SMS_DELETE_RESULT, 2);
    }
    return;
}

static void tb_handle_Sms_Read_Rsp(void *sys_ctx, const osiEvent_t *event)
{
    CFW_EVENT *cfw_event = (CFW_EVENT *)event;
    tb_sms_message *ctx = (tb_sms_message *)malloc(sizeof(tb_sms_message));
    if (NULL == ctx)
    {
        free(ctx);
        return;
    }
    memset(ctx, 0, sizeof(tb_sms_message));

    /* check event type */
    if (0 != cfw_event->nType)
    {
        free(ctx);
        return;
    }

    CFW_SMS_NODE_EX *pNode = (CFW_SMS_NODE_EX *)cfw_event->nParam1;

    OSI_LOGI(0, "tb_handle_Sms_Read_Rsp pNode->node.nStatus = %d", pNode->node.nStatus);
    if ((NULL != g_tb_sms_callback_fun) && (CFW_SMS_STORED_STATUS_UNREAD == pNode->node.nStatus))
    {
        if (g_SMS_Unread_Msg > 0)
        {
            g_SMS_Unread_Msg--;
            g_tb_sms_callback_fun(TB_SIM_NV_UNREAD_SMS, g_SMS_Unread_Msg);
        }
    }

    /* just for debug */
    OSI_LOGI(0, "tb_handle_Sms_Read_Rsp pNode->nType = %d", pNode->node.nType);

    if (pNode->node.nType == 1) // text mode,show param, state = unread/read msg
    {
        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *pType1Data =
            (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *)&pNode->info;

        if (pType1Data->dcs == 2)
        {
            int i = 0;
            int dest_len = 0;
            char oaNumber[TB_WMS_ADDRESS_LEN_MAX + 1];

            char *dest_data = mlConvertStr(pType1Data->data, pType1Data->length,
                                           ML_UTF16BE, ML_UTF8, &dest_len);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 dest_len = %d", dest_len);

            for (i = 0; i < dest_len; i++)
            {
                OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 dest_len = 0x%x", dest_data[i]);
            }

            tb_smsNumber2Str(pType1Data->oa, pType1Data->oa_size, pType1Data->tooa, oaNumber);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_Sms_Read_Rsp dcs2 oaNumber %s", oaNumber);

            if (dest_data != NULL)
            {
                strncpy(ctx->content, dest_data, dest_len);
                ctx->content_len = dest_len;
            }
            strncpy(ctx->phone, oaNumber, strlen(oaNumber));
            ctx->date.year = pType1Data->scts.uYear;
            ctx->date.month = pType1Data->scts.uMonth;
            ctx->date.day = pType1Data->scts.uDay;
            ctx->date.hour = pType1Data->scts.uHour;
            ctx->date.minute = pType1Data->scts.uMinute;
            ctx->date.second = pType1Data->scts.uSecond;
            ctx->date.izone = pType1Data->scts.iZone;
            ctx->total = pNode->node.nConcatNextIndex & 0xff;
            ctx->current_index = (pNode->node.nConcatNextIndex & 0xff00) >> 8;
            ctx->index = pNode->node.nConcatCurrentIndex;
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 ctx->toal = %d", ctx->total);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 ctx->current_index= %d", ctx->current_index);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 ctx->index = %d", ctx->index);
            if (NULL != g_tb_sms_received_contents_cb)
            {
                g_tb_sms_received_contents_cb(ctx);
            }
        }
        else if (pType1Data->dcs == 0)
        {
            int i = 0;
            int dest_len = 0;
            char oaNumber[TB_WMS_ADDRESS_LEN_MAX + 1];

            char *dest_data = mlConvertStr(pType1Data->data, pType1Data->length,
                                           ML_GSM, ML_UTF8, &dest_len);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dest_len = %d", dest_len);

            for (i = 0; i < dest_len; i++)
            {
                OSI_LOGI(0, "tb_handle_Sms_Read_Rsp dcs2 dest_len = 0x%x", dest_data[i]);
            }

            tb_smsNumber2Str(pType1Data->oa, pType1Data->oa_size, pType1Data->tooa, oaNumber);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "tb_handle_Sms_Read_Rsp oaNumber %s", oaNumber);

            if (dest_data != NULL)
            {
                strncpy(ctx->content, dest_data, dest_len);
                ctx->content_len = dest_len;
            }
            strncpy(ctx->phone, oaNumber, strlen(oaNumber));
            ctx->date.year = pType1Data->scts.uYear;
            ctx->date.month = pType1Data->scts.uMonth;
            ctx->date.day = pType1Data->scts.uDay;
            ctx->date.hour = pType1Data->scts.uHour;
            ctx->date.minute = pType1Data->scts.uMinute;
            ctx->date.second = pType1Data->scts.uSecond;
            ctx->date.izone = pType1Data->scts.iZone;
            ctx->total = pNode->node.nConcatNextIndex & 0xff;
            ctx->current_index = (pNode->node.nConcatNextIndex & 0xff00) >> 8;
            ctx->index = pNode->node.nConcatCurrentIndex;
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp ctx->toal = %d", ctx->total);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp ctx->current_Index= %d", ctx->current_index);
            OSI_LOGI(0, "tb_handle_Sms_Read_Rsp ctx->index = %d", ctx->index);
            if (NULL != g_tb_sms_received_contents_cb)
            {
                g_tb_sms_received_contents_cb(ctx);
            }
        }
    }
    else
    {
        free(ctx);
        return;
    }
    free(ctx);
    return;
}

static int tb_handle_New_Sms_Ind(const CFW_EVENT *pCfwEvent)
{
    int nSim = pCfwEvent->nFlag;
    CFW_NEW_SMS_NODE_EX *pNewMsgNode = (CFW_NEW_SMS_NODE_EX *)pCfwEvent->nParam1;

    OSI_LOGI(0, "tb_handle_New_Sms_Ind received new sms\n");

    if (NULL != g_tb_sms_callback_fun)
    {
        g_tb_sms_callback_fun(TB_SMS_RECEIVED, 1);
    }

    /* get new message node and check it */
    if (NULL == pNewMsgNode || pCfwEvent->nType != 0)
    {
        CFW_SmsMtSmsPPAckReq(nSim);
        return TB_SUCCESS;
    }

    OSI_LOGI(0, "tb_handle_New_Sms_Ind: type = %d\n", pNewMsgNode->node.nType);
    /* check the msg node type */
    switch (pNewMsgNode->node.nType)
    {
    case 0x20:
    {
        CFW_SMS_PDU_INFO_V2 *pPduNodeInfo = NULL;
        pPduNodeInfo = (CFW_SMS_PDU_INFO_V2 *)&pNewMsgNode->info;

        uint8_t *pPduData = NULL;
        pPduData = pPduNodeInfo->pData;

        uint8_t dcs = tb_getPduDcs(pPduData);
        uint8_t messageClass = 4;

        if ((dcs & 0x10) == 0x10)
        {
            messageClass = dcs & 0x03;
        }
        OSI_LOGI(0, "tb_handle_New_Sms_Ind: dcs = %d, messageClass = %d\n", dcs, messageClass);

        if ((NULL != g_tb_sms_callback_fun) && (1 == messageClass || 4 == messageClass))
        {
            g_SMS_Unread_Msg++;
            g_tb_sms_callback_fun(TB_SIM_NV_UNREAD_SMS, g_SMS_Unread_Msg);
            g_tb_sms_callback_fun(TB_SMS_RECEIVED_WITH_ID, pNewMsgNode->node.nConcatCurrentIndex);
        }
        break;
    }
    case 0x21:
    case 0x22:
    {
        CFW_SMS_TXT_HRD_IND_V2 *pTextWithHead = (CFW_SMS_TXT_HRD_IND_V2 *)&pNewMsgNode->info;
        uint8_t dcs = pTextWithHead->real_dcs;
        uint8_t messageClass = 4;

        if ((dcs & 0x10) == 0x10)
        {
            messageClass = dcs & 0x03;
        }
        OSI_LOGI(0, "tb_handle_New_Sms_Ind: dcs = %d, messageClass = %d\n", dcs, messageClass);

        if ((NULL != g_tb_sms_callback_fun) && (1 == messageClass || 4 == messageClass))
        {
            g_SMS_Unread_Msg++;
            g_tb_sms_callback_fun(TB_SIM_NV_UNREAD_SMS, g_SMS_Unread_Msg);
            g_tb_sms_callback_fun(TB_SMS_RECEIVED_WITH_ID, pNewMsgNode->node.nConcatCurrentIndex);
        }
        break;
    }
    default:
        break;
    }
    OSI_LOGI(0, "tb_handle_New_Sms_Ind received new sms id = %d\n\n", pNewMsgNode->node.nConcatCurrentIndex);
    CFW_SmsMtSmsPPAckReq(nSim);
    return TB_SUCCESS;
}

static int tb_handle_CC_Initiate_Speech_Call_Rsp(const CFW_EVENT *cfw_event)
{
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;

    if ((0 == cfw_event->nType) || (1 == cfw_event->nType))
    {
        notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_CONNECTED;
        notify_call_info.dial_result = 1;
    }
    else
    {
        notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_FAILED;
        notify_call_info.dial_result = 0;
    }
    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }

    return TB_SUCCESS;
}

static int tb_handle_CC_Accept_Call_Rsp(const CFW_EVENT *cfw_event)
{
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;

    if (0x0f == cfw_event->nType)
    {
        notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_CONNECTED;
        notify_call_info.connect_result = 1;
    }
    else
    {
        notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_FAILED;
        notify_call_info.connect_result = 0;
    }
    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }

    return TB_SUCCESS;
}

static int tb_handle_CC_HungUp_Call_Rsp(const CFW_EVENT *cfw_event)
{
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;
    notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_ENDED;
    notify_call_info.hungup_result = 1;

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }

    return TB_SUCCESS;
}

static int tb_handle_CC_Alerting_Call_Ind(const CFW_EVENT *cfw_event)
{
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;

    notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_CONNECTING;

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }

    return TB_SUCCESS;
}

static int tb_handle_CC_Speech_Call_Ind(const CFW_EVENT *pCfwEvent)
{
    int num_len = 0;
    char num_str[TEL_NUMBER_MAX_LEN + 2];

    CFW_SPEECH_CALL_IND *call_info = (CFW_SPEECH_CALL_IND *)pCfwEvent->nParam1;
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_INCOMING;
    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;
    notify_call_info.remote_party_name_valid = 0;

    if (call_info->TelNumber.nSize)
    {
        num_len = cfwBcdToDialString(call_info->TelNumber.nTelNumber, call_info->TelNumber.nSize, (char *)num_str);
        strncpy(notify_call_info.number, num_str, num_len);
    }
    else
    {
        tb_voice_GetUriCallNumber((const char *)call_info->calling_uri, num_str);
        strncpy(notify_call_info.number, num_str, strlen(num_str));
    }

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }
    return TB_SUCCESS;
}

static int tb_handle_CC_Release_Call_Ind(const CFW_EVENT *pCfwEvent)
{
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_ENDED;
    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;
    notify_call_info.hungup_result = 1;

    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }
    return TB_SUCCESS;
}

static int tb_handle_CC_Error_Call_Ind(const CFW_EVENT *pCfwEvent)
{
    int num_len = 0;
    char num_str[TEL_NUMBER_MAX_LEN + 2];

    CFW_SPEECH_CALL_IND *call_info = (CFW_SPEECH_CALL_IND *)pCfwEvent->nParam1;
    tb_voice_notify_call_state_number_info notify_call_info = {
        0,
    };

    notify_call_info.call_state = TB_VOICE_NOTIFY_CALL_ENDED;
    notify_call_info.call_type = TB_VOICE_CALL_TYPE_VOICE;
    notify_call_info.remote_party_name_valid = 0;

    if (call_info->TelNumber.nSize)
    {
        num_len = cfwBcdToDialString(call_info->TelNumber.nTelNumber, call_info->TelNumber.nSize, (char *)num_str);
        strncpy(notify_call_info.number, num_str, num_len);
    }
    else
    {
        tb_voice_GetUriCallNumber((const char *)call_info->calling_uri, num_str);
        strncpy(notify_call_info.number, num_str, strlen(num_str));
    }
    if (NULL != g_tb_voice_callback_fun)
    {
        g_tb_voice_callback_fun(TB_VOICE_REPORT_CALL_INFO, &notify_call_info);
    }
    return TB_SUCCESS;
}

static int tb_handle_Cfw_Exit_Ind(const CFW_EVENT *cfw_event)
{
    OSI_LOGI(0, "tb_handle_Cfw_Exit_Ind: nType: %d,g_cfw_exit_flag:%d", cfw_event->nType, g_cfw_exit_flag);

    if (g_cfw_exit_flag == 0)
    {
        OSI_LOGI(0, "Shut Down");
        g_cfw_exit_flag = 0xFF;
        drvRtcUpdateTime();
        atCfgAutoSave();
        osiThreadSleep(100);
        osiShutdown(OSI_SHUTDOWN_POWER_OFF);
    }
    else if (g_cfw_exit_flag == 1)
    {
        OSI_LOGI(0, "Reboot");
        g_cfw_exit_flag = 0xFF;
        drvRtcUpdateTime();
        atCfgAutoSave();
        osiThreadSleep(100);
        osiShutdown(OSI_SHUTDOWN_RESET);
    }

    return TB_SUCCESS;
}

int tb_handle_cfw_event(const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    switch (cfw_event->nEventId)
    {
    case EV_CFW_NW_SIGNAL_QUALITY_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_NW_SIGNAL_QUALITY_IND");
        tb_handle_nw_signal_quality_ind(cfw_event);
        break;
    case EV_CFW_NW_REG_STATUS_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_NW_REG_STATUS_IND");
        tb_handle_nw_status_ind(cfw_event);
        break;
    case EV_CFW_GPRS_STATUS_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_GPRS_STATUS_IND");
        tb_handle_nw_status_ind(cfw_event);
        break;
    case EV_CFW_GPRS_CXT_ACTIVE_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_GPRS_CXT_ACTIVE_IND");
        tb_handle_Gprs_Active_Ind(cfw_event);
        break;
    case EV_CFW_GPRS_CXT_DEACTIVE_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_GPRS_CXT_DEACTIVE_IND");
        tb_handle_Gprs_DeActive_Ind(cfw_event);
        break;
    case EV_CFW_INIT_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_INIT_IND");
        tb_handle_Init_ind(cfw_event);
        break;
    case EV_CFW_SRV_STATUS_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_SRV_STATUS_IND");
        tb_handle_Srv_Init_ind(cfw_event);
        break;
    case EV_CFW_NEW_SMS_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_NEW_SMS_IND");
        tb_handle_New_Sms_Ind(cfw_event);
        break;
    case EV_CFW_CC_SPEECH_CALL_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_SPEECH_CALL_IND");
        tb_handle_CC_Speech_Call_Ind(cfw_event);
        break;
    case EV_CFW_CC_ERROR_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_ERROR_IND");
        tb_handle_CC_Error_Call_Ind(cfw_event);
        break;
    case EV_CFW_CC_RELEASE_CALL_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_RELEASE_CALL_IND");
        tb_handle_CC_Release_Call_Ind(cfw_event);
        break;
    case EV_CFW_CC_CALL_TI_ASSIGNED_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_CALL_TI_ASSIGNED_IND");
        if (1 == gConnectingFlag)
        {
            gConnectingFlag = 0;
        }
        break;
    case EV_CFW_CC_ALERT_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_ALERT_IND");
        if (0 == gConnectingFlag)
        {
            gConnectingFlag = 1;
            tb_handle_CC_Alerting_Call_Ind(cfw_event);
        }
        else
        {
            gConnectingFlag = 0;
        }
        break;
    case EV_CFW_CC_AUDIOON_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_AUDIOON_IND");
        if (0 == gConnectingFlag)
        {
            gConnectingFlag = 1;
            tb_handle_CC_Alerting_Call_Ind(cfw_event);
        }
        else
        {
            gConnectingFlag = 0;
        }
        break;
    case EV_CFW_CC_INITIATE_SPEECH_CALL_RSP:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_INITIATE_SPEECH_CALL_RSP");
        tb_handle_CC_Initiate_Speech_Call_Rsp(cfw_event);
        break;
    case EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP");
        tb_handle_CC_Accept_Call_Rsp(cfw_event);
        break;
    case EV_CFW_CC_RELEASE_CALL_RSP:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_CC_RELEASE_CALL_RSP");
        tb_handle_CC_HungUp_Call_Rsp(cfw_event);
        break;
    case EV_CFW_EXIT_IND:
        OSI_LOGI(0, "tb_handle_cfw_event: EV_CFW_EXIT_IND");
        tb_handle_Cfw_Exit_Ind(cfw_event);
        break;
    default:
        break;
    }
    return TB_SUCCESS;
}

#endif
