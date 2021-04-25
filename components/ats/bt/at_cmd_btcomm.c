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
#include "ats_config.h"

#ifdef CONFIG_AT_BTCOMM_SUPPORT

#include "osi_api.h"
#include "osi_log.h"

#include "cfw.h"
#include "cfw_errorcode.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"

#include "at_cmd_btmsg.h"

#include "bt_abs.h"
#include "bt_config.h"

#define AT_BTCOMM_TIMEOUT 120
#define AT_BT_NAME_MAX_SIZE 31
#define AT_BT_ADDR_MAX_SIZE 17
#define AT_BT_PROFILE_MAX_SIZE 12
#define AT_BT_INQURY_TYPE_MAX_SIZE 8
#define ATC_MAX_BT_SEND_DATA_LEN 128

typedef enum AT_BTCOMM_STATUS
{
    AT_BTCOMM_ST_IDLE = 0,
    AT_BTCOMM_ST_INQ,
    AT_BTCOMM_ST_INQ_CANCEL,
} AT_BTCOMM_STATUS_E;

typedef struct AT_BTCOMM_App_Ctx
{
    AT_BTCOMM_STATUS_E status;
    BT_AT_SPP_DATA SppData;
} AT_BTCOMM_App_Ctx_t;

static atCmdEngine_t *BtCommCmdEngine = NULL;
static AT_BTCOMM_App_Ctx_t *gAtBtCommCtx = NULL;

static bool AppCheckBTNameValid(uint8_t ch)
{
    if (ch >= '0' && ch <= '9') // digital 0 - 9
    {
        return true;
    }
    else if (ch >= 'a' && ch <= 'z') // a - z
    {
        return true;
    }
    else if (ch >= 'A' && ch <= 'Z') // A - Z
    {
        return true;
    }
    else if (ch == '_') // _
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************/
//  Description :convert the bt name double byte ascii 0030 to ascii 30
/*****************************************************************************/
static uint8_t *AppConvertWstrToStr(
    const uint16_t *src,
    uint8_t *dst)
{
    uint8_t *return_str = dst;

    if (src == NULL || NULL == dst)
    {
        return return_str;
    }

    while (!(*src & 0xFF00) && '\0' != (*dst++ = *src++))
    {
        ;
    }

    return return_str;
}

/*****************************************************************************/
//  Description :convert the bt name ascii 30 to double byte ascii 0030
/*****************************************************************************/
static uint16_t *AppConvertStrToWstr(
    const uint8_t *src,
    uint16_t *dst)
{
    uint16_t *return_wstr = dst;

    if (src == NULL || NULL == dst)
    {
        return return_wstr;
    }

    while ('\0' != (*dst++ = *src++)) /* copy string */
    {
        ;
    }

    return return_wstr;
}

/*****************************************************************************/
//  Description :convert the bt address to atc output string
//  For example :the bt address 0x112233445566,convert to atc output string 11:22:33:44:55:66
//  Global resource dependence : rsp
//  Author:       yongxia.zhang
//  Note:
/*****************************************************************************/
static void AppConvertBtAddrtoString(uint8_t *addr, uint8_t *str_ptr)
{
    sprintf((char *)str_ptr, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", addr[0], ":", addr[1], ":", addr[2], ":", addr[3], ":", addr[4], ":", addr[5]);
}

static void AppConvertBtNametoString(uint16_t *name, uint8_t name_arr_len, uint8_t *name_str_ptr, uint8_t name_str_arr_len)
{
    OSI_LOGXI(OSI_LOGPAR_M, 0x10005236, "Dump : %*s", name_arr_len * 2, (void *)name);

    if (name[0] < 0x0080)
    {
        //if the name is encode in 2 type ascii
        AppConvertWstrToStr(name, name_str_ptr);
    }
}

static void AppConvertBtNametoString_Ex(uint16_t *name, uint8_t name_arr_len, uint8_t *name_str_ptr, uint8_t name_str_arr_len)
{
    uint8_t i = 0;
    uint8_t j = 0;

    OSI_LOGXI(OSI_LOGPAR_M, 0x10005236, "Dump : %*s", name_arr_len * 2, (void *)name);

    while (name[j] != 0 && j < name_arr_len && i < name_str_arr_len)
    {
        if (name[j] < 0x0080)
        {
            name_str_ptr[i++] = name[j];
        }
        else
        {
            name_str_ptr[i++] = *((uint8_t *)(name + j) + 1);
            name_str_ptr[i++] = *((uint8_t *)(name + j));
        }

        j++;
    }

    OSI_LOGXI(OSI_LOGPAR_M, 0x10005236, "Dump : %*s", name_str_arr_len, (void *)name_str_ptr);
}

/*****************************************************************************/
//  convert to double byte ascii;
/*****************************************************************************/
static bool AppConvertStringtoBtName(uint8_t *str, uint8_t str_len, uint16_t *name, uint8_t name_arr_count)
{
    uint32_t i = 0;

    OSI_LOGXI(OSI_LOGPAR_M, 0x10005236, "Dump : %*s", str_len, (void *)str);

    for (i = 0; i < str_len; i++)
    {
        if (!AppCheckBTNameValid(str[i]))
            return false;
    }

    AppConvertStrToWstr(str, name);

    return true;
}

static bool AppConvertStringtoBtAddr(uint8_t *str_ptr, uint8_t length, uint8_t *addr)
{
    uint8_t i = 0, j = 0;
    uint8_t hex_value = 0;

    if ((NULL == str_ptr) || (length != 17))
    {
        return false;
    }

    for (i = 0; i < length; ++i)
    {
        hex_value = 0;
        if (str_ptr[i] >= '0' && str_ptr[i] <= '9')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - '0');
        }
        else if (str_ptr[i] >= 'a' && str_ptr[i] <= 'f')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - 'a') + 0x0a;
        }
        else if (str_ptr[i] >= 'A' && str_ptr[i] <= 'F')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - 'A') + 0x0a;
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#: invalid param, not numeric, return position:%d", i);
            return false;
        }

        i++;
        if (str_ptr[i] >= '0' && str_ptr[i] <= '9')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - '0');
        }
        else if (str_ptr[i] >= 'a' && str_ptr[i] <= 'f')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - 'a') + 0x0a;
        }
        else if (str_ptr[i] >= 'A' && str_ptr[i] <= 'F')
        {
            hex_value = hex_value * 16 + (int32)(str_ptr[i] - 'A') + 0x0a;
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#: invalid param, not numeric, return position:%d", i);
            return false;
        }

        addr[j++] = hex_value;
        if (j == 6)
            break;

        i++;
        if (str_ptr[i] != ':')
        {
            OSI_LOGI(0, "BTCOMM#: invalid param, not numeric, return position:%d", i);
            return false;
        }
    }

    OSI_LOGI(0, "BTCOMM#: get the bt addr: 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    return true;
}

static bool AppConvertStringtoUint32(uint8_t *str_ptr, uint8_t length, uint32_t *num_ptr)
{
    uint8_t i;
    uint32_t numeric_value = 0;

    if ((NULL == str_ptr) || (length < 1) || (length > 8))
    {
        return false;
    }

    for (i = 0; i < length; ++i)
    {
        if (str_ptr[i] >= '0' && str_ptr[i] <= '9')
        {
            numeric_value = numeric_value * 16 + (int32)(str_ptr[i] - '0');
        }
        else if (str_ptr[i] >= 'a' && str_ptr[i] <= 'f')
        {
            numeric_value = numeric_value * 16 + (int32)(str_ptr[i] - 'a') + 0x0a;
        }
        else if (str_ptr[i] >= 'A' && str_ptr[i] <= 'F')
        {
            numeric_value = numeric_value * 16 + (int32)(str_ptr[i] - 'A') + 0x0a;
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#:ConvertStringtoUint32, invalid param, not numeric, return position:%d", i);
            return false;
        }
    }

    OSI_LOGI(0, "BTCOMM#:ConvertStringtoUint32,get the number: 0x%08x", numeric_value);

    *num_ptr = numeric_value;

    return true;
}

static void AT_BTCommTimeoutHandler(atCommand_t *cmd)
{
    OSI_LOGI(0, "BTCOMM# BTComm command timeout");
    atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT);
}

static void AT_BtComm_Complete(atCmdEngine_t *engine, bool ret, char *str)
{
    if (ret)
    {
        if (str != NULL)
            atCmdRespOKText(engine, str);
        else
            atCmdRespOKText(engine, "+BTCOMM:OK");
    }
    else
    {
        if (str != NULL)
            atCmdRespErrorText(engine, str);
        else
            atCmdRespErrorText(engine, "+BTCOMM:ERR=error");
    }

    atCmdWrite(engine, "\r\n", 2);
}

static void AT_BtSpp_Complete(atCmdEngine_t *engine, bool ret, char *str)
{
    if (ret)
    {
        if (str != NULL)
            atCmdRespOKText(engine, str);
        else
            atCmdRespOKText(engine, "+BTSPP:OK");
    }
    else
    {
        if (str != NULL)
            atCmdRespErrorText(engine, str);
        else
            atCmdRespErrorText(engine, "+BTSPP:ERR=error");
    }

    atCmdWrite(engine, "\r\n", 2);
}

static void _AppBtCommMsgHandler(void *param)
{
    char rsp_str[512] = {0};

    osiEvent_t *pEvent = (osiEvent_t *)param;
    if (pEvent == NULL)
        return;

    OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: id=0x%x, param1=0x%x, param2=0x%x", pEvent->id, pEvent->param1, pEvent->param2);

    BT_STATUS status = (BT_STATUS)(pEvent->param1);
    void *pData = (void *)(pEvent->param2);

    if (BtCommCmdEngine == NULL)
    {
        OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: BtCommCmdEngine is invalid");
        goto EXIT;
    }

    switch (pEvent->id)
    {
    case APP_BT_ME_ON_CNF:
    {
        if (BT_SUCCESS == status)
        {
            if (gAtBtCommCtx != NULL)
            {
                OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: gAtBtCommCtx is not NULL");

                if (gAtBtCommCtx->SppData.buf != NULL)
                {
                    free(gAtBtCommCtx->SppData.buf);
                    gAtBtCommCtx->SppData.buf = NULL;
                    gAtBtCommCtx->SppData.dataLen = 0;
                }

                free(gAtBtCommCtx);
                gAtBtCommCtx = NULL;
            }

            gAtBtCommCtx = (AT_BTCOMM_App_Ctx_t *)malloc(sizeof(AT_BTCOMM_App_Ctx_t));
            if (gAtBtCommCtx == NULL)
            {
                OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: gAtBtCommCtx malloc failed");
                AppRegisterBtMsgCB(APP_BT_MSG, NULL);
                AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
                BtCommCmdEngine = NULL;
                BT_Stop();
                break;
            }
            memset(gAtBtCommCtx, 0, sizeof(AT_BTCOMM_App_Ctx_t));
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: BT Start failed");
            AppRegisterBtMsgCB(APP_BT_MSG, NULL);
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
            BtCommCmdEngine = NULL;
        }
    }
    break;
    case APP_BT_ME_OFF_CNF:
    {
        if (BT_SUCCESS == status)
        {
            AppRegisterBtMsgCB(APP_BT_MSG, NULL);
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
            if (gAtBtCommCtx != NULL)
            {
                if (gAtBtCommCtx->SppData.buf != NULL)
                {
                    free(gAtBtCommCtx->SppData.buf);
                    gAtBtCommCtx->SppData.buf = NULL;
                    gAtBtCommCtx->SppData.dataLen = 0;
                }
                free(gAtBtCommCtx);
                gAtBtCommCtx = NULL;
            }
            BtCommCmdEngine = NULL;
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: BT Stop failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_VISIBILE_CNF:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: VISIBILE CNF failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_HIDDEN_CNF:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: HIDDEN CNF failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_SET_LOCAL_NAME_RES:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: Set local name failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_SET_LOCAL_ADDR_RES:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: Set local addr failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_INQ_DEV_NAME:
    {
        if (BT_SUCCESS == status)
        {
            BT_DEVICE_INFO *pdev_info = NULL;
            uint8 addr_string[AT_BT_ADDR_MAX_SIZE + 1] = {0};

            if (NULL == pData)
            {
                OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: get pdata failed");
                break;
            }
            pdev_info = (BT_DEVICE_INFO *)pData;
            AppConvertBtAddrtoString(pdev_info->addr.addr, addr_string);
            //+INQ:<address>,<name>,<cod>
#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT
            sprintf((char *)rsp_str, "%s%s%s%s%s%08x", "+INQ:", addr_string, ",", (char *)pdev_info->name, ",", pdev_info->dev_class);
#else
            sprintf((char *)rsp_str, "%s%s%s%s%s%08lx", "+INQ:", addr_string, ",", (char *)pdev_info->name, ",", pdev_info->dev_class);
#endif
            atCmdRespInfoText(BtCommCmdEngine, (char *)rsp_str);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: INQ device name failed");
        }
    }
    break;
    case APP_BT_INQ_COMP_CNF:
    {
        if (gAtBtCommCtx->status == AT_BTCOMM_ST_INQ ||
            gAtBtCommCtx->status == AT_BTCOMM_ST_INQ_CANCEL)
        {
            if (BT_SUCCESS == status)
            {
                atCmdRespInfoText(BtCommCmdEngine, (char *)"+INQE");
            }
            else
            {
                OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: INQ comp cnf failed");
                atCmdRespInfoText(BtCommCmdEngine, (char *)"+INQ:ERR");
            }

            gAtBtCommCtx->status = AT_BTCOMM_ST_IDLE;
        }
    }
    break;
    case APP_BT_INQUIRY_CANCEL:
    {
        if (gAtBtCommCtx->status == AT_BTCOMM_ST_INQ_CANCEL)
        {
            if (BT_SUCCESS == status)
            {
                atCmdRespInfoText(BtCommCmdEngine, (char *)"+INQE");
                gAtBtCommCtx->status = AT_BTCOMM_ST_IDLE;
            }
            else
            {
                OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: INQ cancel failed");
                atCmdRespInfoText(BtCommCmdEngine, (char *)"+INQ:ERR");
                gAtBtCommCtx->status = AT_BTCOMM_ST_INQ;
            }
        }
    }
    break;
    case APP_BT_DEV_PAIR_COMPLETE:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: Dev Pair failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_DELETE_DEVICE_RES:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: Dev Pair failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_SPP_CONNECT_IND:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: bt spp connect failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_SPP_DISCONNECT_IND:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtComm_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: bt spp disconnect failed");
            AT_BtComm_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    case APP_BT_SPP_DATA_RECIEVE_IND:
    {
        if (NULL == pData)
        {
            OSI_LOGI(0, "BTSPP#_AppBtCommMsgHandler: get pdata failed");
            break;
        }

        BT_AT_SPP_DATA *SppData = (BT_AT_SPP_DATA *)pData;

#if 0
        uint8_t *buf = NULL;
        uint32_t offset = 0;

        offset = gAtBtCommCtx->SppData.dataLen;
        gAtBtCommCtx->SppData.dataLen += SppData->dataLen;
        buf = malloc(gAtBtCommCtx->SppData.dataLen);
        if (buf != NULL)
        {
            memset(buf, 0, gAtBtCommCtx->SppData.dataLen);
            if (offset > 0)
            {
                memcpy(buf, gAtBtCommCtx->SppData.buf, offset);
                free(gAtBtCommCtx->SppData.buf);
                gAtBtCommCtx->SppData.buf = NULL;
            }
            memcpy(buf + offset, SppData->buf, SppData->dataLen);
            gAtBtCommCtx->SppData.buf = buf;

            sprintf((char *)rsp_str, "%s, %d", "+BTSPP:RECV", SppData->dataLen);
        }
        else
        {
            OSI_LOGI(0, "BTSPP#_AppBtCommMsgHandler: malloc spp data failed");
            gAtBtCommCtx->SppData.dataLen -= SppData->dataLen;
            sprintf((char *)rsp_str, "%s", "+BTSPP:ERR");
        }
        atCmdRespInfoText(BtCommCmdEngine, (char *)rsp_str);
#else
        uint32_t i = 0;
        uint8_t *datahex = NULL;
        datahex = malloc(SppData->dataLen * 5 + 32);
        if (datahex == NULL)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: malloc hex string failed");
            atCmdRespInfoText(BtCommCmdEngine, (char *)"+BTSPP:RECV,ERR");
        }
        else
        {
            memset(datahex, 0, (SppData->dataLen * 5 + 32));
            sprintf((char *)datahex, "+BTSPP:RECV,%d,", SppData->dataLen);
            for (i = 0; i < SppData->dataLen; i++)
                sprintf((char *)datahex + strlen((char *)datahex), "0x%02X ", SppData->buf[i]);
            atCmdRespInfoText(BtCommCmdEngine, (char *)datahex);
            free(datahex);
        }
#endif
        BT_FreeMenSppData((BT_AT_SPP_DATA *)pData);
        pData = NULL;
    }
    break;
    case APP_BT_SPP_DATA_SEND_IND:
    {
        if (BT_SUCCESS == status)
        {
            AT_BtSpp_Complete(BtCommCmdEngine, true, NULL);
        }
        else
        {
            OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: bt spp send failed");
            AT_BtSpp_Complete(BtCommCmdEngine, false, NULL);
        }
    }
    break;
    default:
        OSI_LOGI(0, "BTCOMM#_AppBtCommMsgHandler: unkown msgid=0x%x", pEvent->id);
        break;
    }

EXIT:
    if (NULL != pData)
    {
        free(pData);
    }

    if (NULL != pEvent)
    {
        free(pEvent);
    }

    return;
}

void AT_BTApp_CmdFunc_BtComm(atCommand_t *pParam)
{
    bool paramok = true;
    uint32_t mode = 0;
    bool status = false;
    BT_ADDRESS addr;
    uint32_t device_number = 0;
    uint8_t index = 0;
    BT_DEVICE_INFO dev = {0};
    uint8_t InqType[AT_BT_INQURY_TYPE_MAX_SIZE + 1] = {0};
    uint8_t profile[AT_BT_PROFILE_MAX_SIZE + 1] = {0};
    uint8_t bt_addr[AT_BT_ADDR_MAX_SIZE + 1] = {0};
    uint8_t local_name[AT_BT_NAME_MAX_SIZE + 1] = {0};
    uint8_t bond_name[BT_DEVICE_NAME_SIZE * 2 + 1] = {0};
    uint16_t bt_name[AT_BT_NAME_MAX_SIZE + 1] = {0};
    BT_SCAN_E ScanMode = BT_NONE_ENABLE;
    uint32_t service_type = BT_SERVICE_ALL;
    BT_STATUS ret_val = BT_ERROR;
    char rsp[256] = {0};
    const char *s_bttest_str[] =
        {
            "ENABLE",
            "DISABLE",
            "STATE?",
            "SCANMODE",
            "SCANMODE?",
            "LOCALNAME",
            "LOCALNAME?",
            "LOCALADDR",
            "LOCALADDR?",
            "CONNECT",
            "DISCONNECT",
            "CONNECTION?",
            "INQURY",
            "BOND",
            "BOND?",
            "REMOVEBOND",
        };

    int btcomm_index;
    uint32_t intParam1, intParam2;

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        pParam->params[0]->value[pParam->params[0]->length] = '\0';

        for (btcomm_index = 0; btcomm_index < (sizeof(s_bttest_str) / sizeof(s_bttest_str[0])); btcomm_index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, s_bttest_str[btcomm_index]))
            {
                break;
            }
        }

        OSI_LOGI(0, "BTCOMM#: param index is %d", btcomm_index);

        switch (btcomm_index)
        {
        case 0: //"ENABLE"
        {
            if (pParam->param_count != 1)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                if (!AppRegisterBtMsgCB(APP_BT_MSG, _AppBtCommMsgHandler))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: register msg callback failed", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                ret_val = BT_Start();
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_Start pending", s_bttest_str[btcomm_index]);
                    BtCommCmdEngine = pParam->engine;
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_Start() failed", s_bttest_str[btcomm_index]);
                    AppRegisterBtMsgCB(APP_BT_MSG, NULL);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: start bt fail as bt is already on!", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 1: //"DISABLE"
        {
            if (pParam->param_count != 1)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (BT_GetState())
            {
                ret_val = BT_Stop();
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_Stop pending", s_bttest_str[btcomm_index]);
                    atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_Stop() failed", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Stop bt fail as bt is already off!", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 2: // STATE?
        {
            if (pParam->param_count != 1)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            sprintf((char *)rsp, "+BTCOMM:STATE=%s", (BT_GetState() ? "enable" : "disable"));
            AT_BtComm_Complete(pParam->engine, true, rsp);
        }
        break;
        case 3: // SCANMODE
        {
            if (pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            intParam1 = atParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: inquiry scan is invalid", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            intParam2 = atParamUintInRange(pParam->params[2], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: page scan is invalid", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is not start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (intParam1 == 0 && intParam2 == 0)
                ScanMode = BT_NONE_ENABLE;
            else if (intParam1 == 1 && intParam2 == 0)
                ScanMode = BT_INQUIRY_ENABLE;
            else if (intParam1 == 0 && intParam2 == 1)
                ScanMode = BT_PAGE_ENABLE;
            else if (intParam1 == 1 && intParam2 == 1)
                ScanMode = BT_PAGE_AND_INQUIRY_ENABLE;
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SII, 0, "BTCOMM#%s: inquiry=%d, page=%d", s_bttest_str[btcomm_index], intParam1, intParam2);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }
            OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_SCAN_E=%d", s_bttest_str[btcomm_index], ScanMode);

            ret_val = BT_SetVisibilityUni(ScanMode);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_SetVisibilityUni pending", s_bttest_str[btcomm_index]);
                atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
            }
            else if (BT_SUCCESS == ret_val)
            {
                AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_SetVisibilityUni() failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 4: // SCANMODE?
        {
            if (pParam->param_count != 1)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            ScanMode = BT_GetVisibilityUni();
            if (ScanMode == BT_NONE_ENABLE)
            {
                AT_BtComm_Complete(pParam->engine, true, "+BTCOMM:SCANMODE=0,0");
            }
            else if (ScanMode == BT_INQUIRY_ENABLE)
            {
                AT_BtComm_Complete(pParam->engine, true, "+BTCOMM:SCANMODE=1,0");
            }
            else if (ScanMode == BT_PAGE_ENABLE)
            {
                AT_BtComm_Complete(pParam->engine, true, "+BTCOMM:SCANMODE=0,1");
            }
            else if (ScanMode == BT_PAGE_AND_INQUIRY_ENABLE)
            {
                AT_BtComm_Complete(pParam->engine, true, "+BTCOMM:SCANMODE=1,1");
            }
            else
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 5: // LOCALNAME
        {
            if (pParam->param_count != 2)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if ((pParam->params[1]->length == 0) || (pParam->params[1]->length > AT_BT_NAME_MAX_SIZE))
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[1]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)local_name, pParam->params[1]->value, pParam->params[1]->length);

            if (!AppConvertStringtoBtName((uint8_t *)local_name, strlen((char *)local_name), bt_name, AT_BT_NAME_MAX_SIZE))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Convert String to BtName failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            ret_val = BT_SetLocalName(bt_name);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_SetLocalName pending", s_bttest_str[btcomm_index]);
                atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
            }
            else if (BT_SUCCESS == ret_val)
            {
                AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_SetLocalName() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 6: // LOCALNAME?
        {
            ret_val = BT_GetLocalName(bt_name);
            if (BT_SUCCESS == ret_val)
            {
                AppConvertBtNametoString(bt_name, AT_BT_NAME_MAX_SIZE, local_name, AT_BT_NAME_MAX_SIZE);
                sprintf((char *)rsp, "+BTCOMM:LOCALNAME=%s", (char *)local_name);
                AT_BtComm_Complete(pParam->engine, true, rsp);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_GetLocalName() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 7: // LOCALADDR
        {
            if (pParam->param_count != 2)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (pParam->params[1]->length != AT_BT_ADDR_MAX_SIZE)
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[1]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)bt_addr, pParam->params[1]->value, pParam->params[1]->length);

            if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            ret_val = BT_SetBdAddr(&addr);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_SetBdAddr pending", s_bttest_str[btcomm_index]);
                atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
            }
            else if (BT_SUCCESS == ret_val)
            {
                AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_SetBdAddr() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 8: // LOCALADDR?
        {
            BT_GetBdAddr(&addr);
            AppConvertBtAddrtoString(addr.addr, bt_addr);
            sprintf((char *)rsp, "+BTCOMM:LOCALADDR=%s", (char *)bt_addr);
            AT_BtComm_Complete(pParam->engine, true, rsp);
        }
        break;
        case 9: // CONNECT
        {
            if (pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if ((pParam->params[1]->length == 0) || (pParam->params[1]->length > AT_BT_PROFILE_MAX_SIZE))
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[1]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)profile, pParam->params[1]->value, pParam->params[1]->length);

            if (pParam->params[2]->length != AT_BT_ADDR_MAX_SIZE)
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[2]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)bt_addr, pParam->params[2]->value, pParam->params[2]->length);

            if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is not start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (strcmp((char *)profile, "SPP") == 0)
            {
#ifdef BT_SPP_SUPPORT
                if (!BT_SppConnectStatus())
                {
                    ret_val = BT_SppConnect(&addr, SPP_PORT_SPEED_115200);
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: SPP status is incorrect", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }
#else
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: SPP is not support", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
#endif
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SS, 0, "BTCOMM#%s: %s is not support", s_bttest_str[btcomm_index], (char *)profile);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (ret_val == BT_PENDING)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT Connect pending", s_bttest_str[btcomm_index]);
                atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
            }
            else if (ret_val == BT_SUCCESS)
            {
                AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT Connect() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 10: // DISCONNECT
        {
            if (pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if ((pParam->params[1]->length == 0) || (pParam->params[1]->length > AT_BT_PROFILE_MAX_SIZE))
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[1]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)profile, pParam->params[1]->value, pParam->params[1]->length);

            if (pParam->params[2]->length != AT_BT_ADDR_MAX_SIZE)
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[2]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)bt_addr, pParam->params[2]->value, pParam->params[2]->length);

            if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is not start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (strcmp((char *)profile, "SPP") == 0)
            {
#ifdef BT_SPP_SUPPORT
                if (BT_SppConnectStatus())
                {
                    ret_val = BT_SppDisconnect();
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: SPP status is incorrect", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }
#else
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: SPP is not support", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
#endif
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SS, 0, "BTCOMM#%s: %s is not support", s_bttest_str[btcomm_index], (char *)profile);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (ret_val == BT_PENDING)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT Disonnect pending", s_bttest_str[btcomm_index]);
                atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
            }
            else if (ret_val == BT_SUCCESS)
            {
                AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT Disonnect() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        case 11: // CONNECTION?
        {
            if (pParam->param_count != 2)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (pParam->params[1]->length != AT_BT_ADDR_MAX_SIZE)
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[1]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)bt_addr, pParam->params[1]->value, pParam->params[1]->length);

            if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is not start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            status = BT_SppConnectStatusUni(&addr);
            sprintf((char *)rsp, "+BTCOMM:CONNECTION=%s,%s", (status ? "connection" : "disconnected"), (char *)bt_addr);
            AT_BtComm_Complete(pParam->engine, true, rsp);
        }
        break;
        case 12: // INQURY
        {
            if (pParam->param_count != 2 && pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            mode = atParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: mode error", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (pParam->param_count == 3)
            {
                if ((pParam->params[2]->length == 0) || (pParam->params[2]->length > AT_BT_INQURY_TYPE_MAX_SIZE))
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[2]->length);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                memcpy((void *)InqType, pParam->params[2]->value, pParam->params[2]->length);

                if (!AppConvertStringtoUint32((uint8_t *)InqType, strlen((char *)InqType), &service_type))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to uint32 failed", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }
            }

            if (mode == 1)
            {
                if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                OSI_LOGI(0, "BTCOMM#: service_type = 0x%x", service_type);

                ret_val = BT_SearchDevice(service_type);
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_SearchDevice pending", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, true, "+INQS");
                    gAtBtCommCtx->status = AT_BTCOMM_ST_INQ;
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_SearchDevice ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
            else
            {
                if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_INQ)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is not inqury", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                ret_val = BT_CancelSearch();
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_CancelSearch pending", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, true, NULL);
                    gAtBtCommCtx->status = AT_BTCOMM_ST_INQ_CANCEL;
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_CancelSearch ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
        }
        break;
        case 13: // BOND
        {
            if (pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            mode = atParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: mode error", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (pParam->params[2]->length != AT_BT_ADDR_MAX_SIZE)
            {
                OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[2]->length);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            memcpy((void *)bt_addr, pParam->params[2]->value, pParam->params[2]->length);

            if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (mode == 1)
            {
                ret_val = BT_PairDevice(&addr);
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_PairDevice pending", s_bttest_str[btcomm_index]);
                    atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_PairDevice ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
            else
            {
                ret_val = BT_CancelPair(&addr);
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT_CancelPair pending", s_bttest_str[btcomm_index]);
                    atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_CancelPair ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
        }
        break;
        case 14: // BOND?
        {
            if (pParam->param_count != 1)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);
            OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: device_number = %d", s_bttest_str[btcomm_index], device_number);

            for (index = 0; index < device_number; index++)
            {
                if (BT_GetPairedDeviceInfo(BT_SERVICE_ALL, index, &dev) == BT_SUCCESS)
                {
                    //+LISTBOND:11:22:33:44:55:66,devName1,00000408
                    memset(bond_name, 0, sizeof(bond_name));
                    AppConvertBtAddrtoString(dev.addr.addr, bt_addr);
                    AppConvertBtNametoString_Ex(dev.name, BT_DEVICE_NAME_SIZE, bond_name, sizeof(bond_name));
                    sprintf((char *)rsp, "%s%s%s%s%s%08x", "+LISTBOND:", (char *)bt_addr, ",", (char *)bond_name, ",", (unsigned int)dev.dev_class);
                    atCmdRespInfoText(pParam->engine, (char *)rsp);
                }
            }

            AT_BtComm_Complete(pParam->engine, true, NULL);
        }
        break;
        case 15: // REMOVEBOND
        {
            if (pParam->param_count != 2 && pParam->param_count != 3)
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            mode = atParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: mode error", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (!BT_GetState())
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: Bt is not start", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: BT is busy", s_bttest_str[btcomm_index]);
                AT_BtComm_Complete(pParam->engine, false, NULL);
                break;
            }

            if (mode == 0) //clear all device
            {
                device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);

                for (index = 0; index < device_number; index++)
                {
                    ret_val = BT_GetPairedDeviceInfo(BT_SERVICE_ALL, 0, &dev);
                    if (ret_val != BT_SUCCESS)
                    {
                        OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_GetPairedDeviceInfo() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                        AT_BtComm_Complete(pParam->engine, false, NULL);
                        break;
                    }
                    ret_val = BT_RemovePairedDevice(&(dev.addr));
                    if (ret_val != BT_SUCCESS)
                    {
                        OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_RemovePairedDevice() failed, ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                        AT_BtComm_Complete(pParam->engine, false, NULL);
                        break;
                    }
                }

                if (index == device_number)
                    AT_BtComm_Complete(pParam->engine, true, NULL);
            }
            else if (mode == 1 && pParam->param_count == 3) //clear specific address device
            {
                if (pParam->params[2]->length != AT_BT_ADDR_MAX_SIZE)
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: length error %d", s_bttest_str[btcomm_index], pParam->params[2]->length);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                memcpy((void *)bt_addr, pParam->params[2]->value, pParam->params[2]->length);

                if (!AppConvertStringtoBtAddr((uint8_t *)bt_addr, strlen((char *)bt_addr), addr.addr))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTCOMM#%s: convert string to BtAddr failed", s_bttest_str[btcomm_index]);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                    break;
                }

                ret_val = BT_RemovePairedDevice(&addr);
                if (BT_SUCCESS == ret_val)
                {
                    AT_BtComm_Complete(pParam->engine, true, NULL);
                }
                else
                {
                    OSI_LOGXI(OSI_LOGPAR_SI, 0, "BTCOMM#%s: BT_RemovePairedDevice() ret_val = %d", s_bttest_str[btcomm_index], ret_val);
                    AT_BtComm_Complete(pParam->engine, false, NULL);
                }
            }
            else
            {
                AT_BtComm_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        default:
        {
            OSI_LOGI(0, "BTCOMM#: parser failed");
            AT_BtComm_Complete(pParam->engine, false, NULL);
        }
        break;
        }
    }
    break;
    default:
    {
        OSI_LOGI(0, "BTCOMM#: pParam type failed");
        AT_BtComm_Complete(pParam->engine, false, NULL);
    }
    break;
    }
    return;
}

void AT_BTApp_CmdFunc_BtSpp(atCommand_t *pParam)
{
    uint32_t i = 0;
    uint8_t type = 0; // ascii default
    uint32_t datalen = 0;
    uint8_t *datahex = NULL;
    uint8_t data[ATC_MAX_BT_SEND_DATA_LEN] = {0};
    bool paramok = true;
    const char *s_bttest_str[] =
        {
            "SEND",
            "RECV",
        };

    int btcomm_index;

    if (!BT_GetState())
    {
        OSI_LOGI(0, "BTSPP#: Bt is not start");
        AT_BtSpp_Complete(pParam->engine, false, NULL);
        return;
    }

    if (BT_SppConnectStatus() != true)
    {
        OSI_LOGI(0, "BTSPP#: SPP is not connected");
        AT_BtSpp_Complete(pParam->engine, false, NULL);
        return;
    }

    if (gAtBtCommCtx == NULL || gAtBtCommCtx->status != AT_BTCOMM_ST_IDLE)
    {
        OSI_LOGI(0, "BTSPP#: BT is busy");
        AT_BtSpp_Complete(pParam->engine, false, NULL);
        return;
    }

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        pParam->params[0]->value[pParam->params[0]->length] = '\0';

        for (btcomm_index = 0; btcomm_index < (sizeof(s_bttest_str) / sizeof(s_bttest_str[0])); btcomm_index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, s_bttest_str[btcomm_index]))
            {
                break;
            }
        }

        OSI_LOGI(0, "BTSPP#: param index is %d", btcomm_index);

        switch (btcomm_index)
        {
        case 0: //"SEND"
        {
            if (pParam->param_count != 4)
            {
                AT_BtSpp_Complete(pParam->engine, false, NULL);
                break;
            }

            type = atParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: type error", s_bttest_str[btcomm_index]);
                AT_BtSpp_Complete(pParam->engine, false, NULL);
                break;
            }

            datalen = atParamUintInRange(pParam->params[2], 0, ATC_MAX_BT_SEND_DATA_LEN, &paramok);
            if (!paramok)
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: datalen error", s_bttest_str[btcomm_index]);
                AT_BtSpp_Complete(pParam->engine, false, NULL);
                break;
            }

            if (type == 0)
            {
                if (datalen != pParam->params[3]->length)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: datalen error", s_bttest_str[btcomm_index]);
                    AT_BtSpp_Complete(pParam->engine, false, NULL);
                    break;
                }

                memcpy((void *)data, pParam->params[3]->value, pParam->params[3]->length);

                BT_SppWriteData(data, datalen);
            }
            else
            {
                if (datalen != (pParam->params[3]->length / 2))
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: datalen error", s_bttest_str[btcomm_index]);
                    AT_BtSpp_Complete(pParam->engine, false, NULL);
                    break;
                }

                datahex = (uint8_t *)atParamHexRawText(pParam->params[3], datalen, &paramok);
                if (!paramok)
                {
                    OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: get hex data failed", s_bttest_str[btcomm_index]);
                    AT_BtSpp_Complete(pParam->engine, false, NULL);
                    break;
                }

                BT_SppWriteData(datahex, datalen);
            }

            atCmdSetTimeoutHandler(pParam->engine, AT_BTCOMM_TIMEOUT * 1000, AT_BTCommTimeoutHandler);
        }
        break;
        case 1: //"RECV"
        {
            if (gAtBtCommCtx->SppData.dataLen != 0 && gAtBtCommCtx->SppData.buf != NULL)
            {
                atCmdWrite(pParam->engine, (const void *)"> ", 2);

                if (type == 0)
                {
                    atCmdWrite(pParam->engine, gAtBtCommCtx->SppData.buf, gAtBtCommCtx->SppData.dataLen);
                }
                else
                {
                    datahex = malloc(gAtBtCommCtx->SppData.dataLen * 2 + 2);
                    if (datahex == NULL)
                    {
                        OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: malloc hex string failed", s_bttest_str[btcomm_index]);
                        AT_BtSpp_Complete(pParam->engine, false, NULL);
                        break;
                    }

                    memset(datahex, 0, gAtBtCommCtx->SppData.dataLen * 2 + 2);
                    for (i = 0; i < gAtBtCommCtx->SppData.dataLen; i++)
                        sprintf((char *)datahex + 2 * i, "%02X", gAtBtCommCtx->SppData.buf[i]);
                    atCmdRespInfoText(pParam->engine, (char *)datahex);
                    free(datahex);
                }

                free(gAtBtCommCtx->SppData.buf);
                gAtBtCommCtx->SppData.buf = NULL;
                gAtBtCommCtx->SppData.dataLen = 0;
                AT_BtSpp_Complete(pParam->engine, true, NULL);
            }
            else
            {
                OSI_LOGXI(OSI_LOGPAR_S, 0, "BTSPP#%s: no data to read", s_bttest_str[btcomm_index]);
                AT_BtSpp_Complete(pParam->engine, false, NULL);
            }
        }
        break;
        default:
        {
            OSI_LOGI(0, "BTSPP#: parser failed");
            AT_BtSpp_Complete(pParam->engine, false, NULL);
        }
        break;
        }
    }
    break;
    default:
    {
        OSI_LOGI(0, "BTSPP#: pParam type failed");
        AT_BtSpp_Complete(pParam->engine, false, NULL);
    }
    break;
    }
}

#endif
