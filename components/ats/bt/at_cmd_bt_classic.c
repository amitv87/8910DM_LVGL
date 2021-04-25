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
#if 0
#ifdef CONFIG_AT_BT_CLASSIC_SUPPORT

#include "osi_api.h"
#include "osi_log.h"

#include "cfw.h"
#include "cfw_errorcode.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"

#include "at_cmd_btmsg.h"

#include "bt_abs.h"
#include "bt_config.h"

#define ATC_MAX_BT_NAME_STRING_LEN 164
#define ATC_MAX_BT_ADDRESS_STRING_LEN 20
#define ATC_BT_NAME_ASCII 0           //the bt name encode in ascii
#define ATC_BT_NAME_UCS2 1            //the bt name encode in ucs2
#define ATC_BT_TX_TIMEOUT (30 * 1000) //input timeout

typedef enum AT_BT_PROFILE
{
    NONE = 0,
    SPP,
} AT_BT_PROFILE_E;

typedef enum AT_BT_STATUS
{
    AT_BT_ST_IDLE = 0,
    AT_BT_ST_INQ,
    AT_BT_ST_PAIR,
    AT_BT_ST_CONNECT,
    AT_BT_ST_DISCONNECT,
} AT_BT_STATUS_E;

typedef struct BT_TXBUFFER
{
    uint8_t *buffer;
    uint16_t len;
    uint16_t request;
    uint16_t sended;
    uint32_t type;
    osiTimer_t *tmrID;
    AT_BT_PROFILE_E profile;
} BT_TXBUFFER_T;

typedef struct AT_BT_App_Ctx
{
    bool addrPinAval;
    BT_ADDRESS addrPin;
    AT_BT_STATUS_E status;
    BT_TXBUFFER_T txbuf;
    BT_AT_SPP_DATA SppData;
} AT_BT_App_Ctx_t;

static AT_BT_App_Ctx_t *gAtBtCtx = NULL;
static atCmdEngine_t *BtCmdEngine = NULL;

#define AT_BT_TIMEOUT 120

static void AT_BTTimeoutHandler(atCommand_t *cmd)
{
    OSI_LOGI(0, "BT# BT command timeout");
    atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT);
}

#ifdef BT_SPP_SUPPORT
static void BT_data_mode_stop(atCmdEngine_t *engine);
static void BT_data_mode_start(atCmdEngine_t *engine);
#endif

/*****************************************************************************/
//  Description :convert the bt name double byte ascii 0030 to ascii 30
//  Global resource dependence : rsp
//  Author:       yongxia.zhang
//  Note:
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

static bool AppConvertHexToBin( // false: There is invalid char
    uint8_t *hex_ptr,           // in: the hexadecimal format string
    uint16_t length,            // in: the length of hexadecimal string
    uint8_t *bin_ptr            // out: pointer to the binary format string
)
{
    uint8_t *dest_ptr = bin_ptr;
    int32 i = 0;
    uint8_t ch = 0;

    OSI_ASSERT(NULL != hex_ptr, "NULL == hex_ptr"); /*assert verified*/
    OSI_ASSERT(NULL != bin_ptr, "NULL == bin_ptr"); /*assert verified*/

    for (i = 0; i < length; i += 2)
    {
        // the bit 8,7,6,5
        ch = hex_ptr[i];

        // digital 0 - 9
        if (ch >= '0' && ch <= '9')
        {
            *dest_ptr = (uint8_t)((ch - '0') << 4);
        }

        // a - f
        else if (ch >= 'a' && ch <= 'f')
        {
            *dest_ptr = (uint8_t)((ch - 'a' + 10) << 4);
        }

        // A - F
        else if (ch >= 'A' && ch <= 'F')
        {
            *dest_ptr = (uint8_t)((ch - 'A' + 10) << 4);
        }
        else
        {
            return false;
        }

        // the bit 1,2,3,4
        ch = hex_ptr[i + 1];

        // digtial 0 - 9
        if (ch >= '0' && ch <= '9')
        {
            *dest_ptr |= (uint8_t)(ch - '0');
        }

        // a - f
        else if (ch >= 'a' && ch <= 'f')
        {
            *dest_ptr |= (uint8_t)(ch - 'a' + 10);
        }

        // A - F
        else if (ch >= 'A' && ch <= 'F')
        {
            *dest_ptr |= (uint8_t)(ch - 'A' + 10);
        }
        else
        {
            return false;
        }

        dest_ptr++;
    }
    return true;
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

    memset(name_str_ptr, 0, name_str_arr_len);

    if (name[0] < 0x0080)
    {
        //if the name is encode in 2 type ascii
        AppConvertWstrToStr(name, name_str_ptr);
    }
    else
    {
        memcpy(name_str_ptr, (void *)name, name_arr_len * 2);
    }
}

/*****************************************************************************/
//  Description :convert the string to bt name
//              bt name is encoded in UCS2.
//              if the atc input bt name is ascii,convert to double byte ascii(UCS2);
//              if the atc input bt name is UCS2 string,convert to UCS2
//              for example: 1) atc input bt name is MOCOR,convert to UCS2 004D004F0043004F0052
//                           2) atc input bt name is string 5F204E09,convert to UCS2 5F204E09
//  Global resource dependence : rsp
//  Author:       yongxia.zhang
//  Note:
/*****************************************************************************/
static void AppConvertStringtoBtName(uint8_t type, uint8_t *str, uint8_t str_len, uint16_t *name, uint8_t name_arr_count)
{
    uint8_t temp_name[200] = {0};

    uint8_t i = 0, j = 0;

    memset(name, 0, name_arr_count * 2);

    if (type == ATC_BT_NAME_ASCII)
    {
        //transfer uint8_t ascii to ucs2
        AppConvertStrToWstr(str, name);
    }
    else
    {
        //transfer hex string to bin
        AppConvertHexToBin(str, str_len, temp_name);

        //copy uint8_t to uint16_t
        for (i = 0; i < (name_arr_count - 1); i++)
        {
            if (i > (sizeof(temp_name) - 2))
                break;

            if ((temp_name[i] == 0x00) &&
                (temp_name[i + 1] == 0x00))
            {
                break;
            }
            name[j] = temp_name[i++];
            name[j] = (name[j] << 8) | temp_name[i];
            j++;
        }

        name[j] = '\0';
    }
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
            OSI_LOGI(0, "BT#:ConvertStringtoUint32, invalid param, not numeric, return position:%d", i);
            return false;
        }
    }

    OSI_LOGI(0, "BT#:ConvertStringtoUint32,get the number: 0x%08x", numeric_value);

    *num_ptr = numeric_value;

    return true;
}

bool AppConvertStringtoBtAddr(uint8_t *str_ptr, uint8_t length, uint8_t *addr)
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
            OSI_LOGI(0, "BT#:ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
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
            OSI_LOGI(0, "BT#:ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return false;
        }

        addr[j++] = hex_value;
        if (j == 6)
            break;

        i++;
        if (str_ptr[i] != ':')
        {
            OSI_LOGI(0, "BT#:ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return false;
        }
    }

    OSI_LOGI(0, "BT#:ConvertStringtoUint32,get the bt addr: 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    return true;
}

#ifdef BT_SPP_SUPPORT

static void BT_txbufferinit(BT_TXBUFFER_T *tx, uint32_t request)
{
    tx->len = 0;
    tx->sended = 0;
    tx->request = request;
    tx->type = NONE;

    if (tx->buffer != NULL)
    {
        free(tx->buffer);
        tx->buffer = NULL;
    }

    if (tx->tmrID != NULL)
    {
        osiTimerStop(tx->tmrID);
        osiTimerDelete(tx->tmrID);
        tx->tmrID = NULL;
    }
}

static int32_t BT_txbufpush(BT_TXBUFFER_T *tx, uint8_t *data, uint16_t size)
{
    if (size + tx->len > tx->request)
        size = tx->request - tx->len;

    memcpy(tx->buffer + tx->len, data, size);
    tx->len += size;
    return tx->request - tx->len;
}

static void BT_send_txbuf_finish(BT_TXBUFFER_T *tx, bool ret)
{
    BT_txbufferinit(tx, 0);

    if (ret)
    {
        atCmdRespOK(BtCmdEngine);
    }
    else
    {
        atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
    }
}

static void BT_send_timeout(void *tx)
{
    BT_data_mode_stop(BtCmdEngine);
    BT_send_txbuf_finish((BT_TXBUFFER_T *)tx, false);
}

static void BT_send_txbuf(BT_TXBUFFER_T *tx)
{
    uint32 size = 0;

    OSI_LOGI(0, "BT# triger BT_send_txbuf");

    if (tx->profile == SPP)
    {
        size = BT_SppWriteData(tx->buffer + tx->sended, tx->request - tx->sended);
    }
    else
    {
        BT_send_txbuf_finish(tx, false);
        return;
    }

    tx->sended += size;

    if (tx->sended == tx->request)
    {
        BT_send_txbuf_finish(tx, true);
    }
    else
    {
        if (tx->tmrID == NULL)
            tx->tmrID = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)BT_send_txbuf, (void *)tx);

        if (tx->tmrID != NULL)
            osiTimerStart(tx->tmrID, 100);
        else
            BT_send_txbuf_finish(tx, false);
    }
}

static int BT_UartRecv(void *param, uint8_t *data, unsigned length)
{
    OSI_LOGI(0, "BT# Bt_UartRecv = %d", length);

    BT_TXBUFFER_T *tx = &(gAtBtCtx->txbuf);

    int32_t left = BT_txbufpush(tx, data, length);
    if (left == 0)
    {
        if (tx->tmrID != NULL)
        {
            osiTimerStop(tx->tmrID);
            osiTimerDelete(tx->tmrID);
            tx->tmrID = NULL;
        }

        BT_data_mode_stop(BtCmdEngine);
        atCmdWrite(BtCmdEngine, tx->buffer, tx->len);
        BT_send_txbuf(tx);
    }
    else
    {
        if (tx->tmrID != NULL)
        {
            osiTimerStop(tx->tmrID);
            osiTimerStart(tx->tmrID, ATC_BT_TX_TIMEOUT);
        }
    }

    return length;
}

static void BT_data_mode_stop(atCmdEngine_t *engine)
{
    atCmdSetLineMode(engine);
}

static void BT_data_mode_start(atCmdEngine_t *engine)
{
    BT_TXBUFFER_T *tx = &(gAtBtCtx->txbuf);
    atCmdRespOutputPrompt(engine);
    atCmdSetBypassMode(engine, (atCmdBypassCB_t)BT_UartRecv, NULL);

    // create serial input timer
    if (tx->tmrID == NULL)
        tx->tmrID = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)BT_send_timeout, (void *)tx);

    if (tx->tmrID != NULL)
        osiTimerStart(tx->tmrID, ATC_BT_TX_TIMEOUT);
    else
        BT_send_txbuf_finish(tx, false);
}

#endif

static void _AppBtMsgHandler(void *param)
{
    char rsp_str[512] = {0};

    osiEvent_t *pEvent = (osiEvent_t *)param;
    if (pEvent == NULL)
        return;

    OSI_LOGI(0, "BT#_AppBtMsgHandler: id=0x%x, param1=0x%x, param2=0x%x", pEvent->id, pEvent->param1, pEvent->param2);

    BT_STATUS status = (BT_STATUS)(pEvent->param1);
    void *pData = (void *)(pEvent->param2);

    if (BtCmdEngine == NULL)
    {
        OSI_LOGI(0, "BT#_AppBtMsgHandler: BtCmdEngine is invalid");
        goto EXIT;
    }

    switch (pEvent->id)
    {
    case APP_BT_ME_ON_CNF:
    {
        if (BT_SUCCESS == status)
        {
            if (gAtBtCtx != NULL)
            {
                OSI_LOGI(0, "BT#_AppBtMsgHandler: gAtBtCtx is not NULL");

                if (gAtBtCtx->SppData.buf != NULL)
                {
                    free(gAtBtCtx->SppData.buf);
                    gAtBtCtx->SppData.buf = NULL;
                    gAtBtCtx->SppData.dataLen = 0;
                }

                free(gAtBtCtx);
                gAtBtCtx = NULL;
            }

            gAtBtCtx = (AT_BT_App_Ctx_t *)malloc(sizeof(AT_BT_App_Ctx_t));
            if (gAtBtCtx == NULL)
            {
                OSI_LOGI(0, "BT#_AppBtMsgHandler: gAtBtCtx malloc failed");
                BT_Stop();
                AppRegisterBtMsgCB(APP_BT_MSG, NULL);
                atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
                BtCmdEngine = NULL;
                break;
            }

            memset(gAtBtCtx, 0, sizeof(AT_BT_App_Ctx_t));
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: BT Start failed");
            AppRegisterBtMsgCB(APP_BT_MSG, NULL);
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
            BtCmdEngine = NULL;
        }
    }
    break;
    case APP_BT_ME_OFF_CNF:
    {
        if (BT_SUCCESS == status)
        {
            AppRegisterBtMsgCB(APP_BT_MSG, NULL);
            atCmdRespOK(BtCmdEngine);
            if (gAtBtCtx != NULL)
            {
                if (gAtBtCtx->SppData.buf != NULL)
                {
                    free(gAtBtCtx->SppData.buf);
                    gAtBtCtx->SppData.buf = NULL;
                    gAtBtCtx->SppData.dataLen = 0;
                }
                free(gAtBtCtx);
                gAtBtCtx = NULL;
            }
            BtCmdEngine = NULL;
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: BT Stop failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_VISIBILE_CNF:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: VISIBILE CNF failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_HIDDEN_CNF:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: HIDDEN CNF failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_SET_LOCAL_NAME_RES:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: Set local name failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_SET_LOCAL_ADDR_RES:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: Set local addr failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_INQ_DEV_NAME:
    {
        if (BT_SUCCESS == status)
        {
            BT_DEVICE_INFO *pdev_info = NULL;
            uint8 addr_string[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
            uint8 name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};

            if (NULL == pData)
            {
                OSI_LOGI(0, "BT#_AppBtMsgHandler: get pdata failed");
                break;
            }
            pdev_info = (BT_DEVICE_INFO *)pData;
            AppConvertBtAddrtoString(pdev_info->addr.addr, addr_string);
            AppConvertBtNametoString(pdev_info->name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));

            //+INQ:<address>,<name>,<cod>
#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT
            sprintf((char *)rsp_str, "%s%s%s%s%s%08x", "+INQ:", addr_string, ",", name_string, ",", pdev_info->dev_class);
#else
            sprintf((char *)rsp_str, "%s%s%s%s%s%08lx", "+INQ:", addr_string, ",", name_string, ",", pdev_info->dev_class);
#endif
            atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: INQ device name failed");
        }
    }
    break;
    case APP_BT_INQ_COMP_CNF:
    {
        if (BT_SUCCESS == status)
        {
            sprintf((char *)rsp_str, "%s", "+INQE");
            atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: INQ comp cnf failed");
        }

        gAtBtCtx->status = AT_BT_ST_IDLE;
    }
    break;
    case APP_BT_INQUIRY_CANCEL:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
            gAtBtCtx->status = AT_BT_ST_IDLE;
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: INQ cancel failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_DEV_PAIR_COMPLETE:
    {
        if (BT_SUCCESS == status)
        {
            sprintf((char *)rsp_str, "%s", "+PAIRCOMPLETE:0");
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: Dev Pair failed");
            sprintf((char *)rsp_str, "%s", "+PAIRCOMPLETE:1");
        }
        gAtBtCtx->status = AT_BT_ST_IDLE;
        gAtBtCtx->addrPinAval = false;
        atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
    }
    break;
    case APP_BT_DELETE_DEVICE_RES:
    {
        if (BT_SUCCESS == status)
        {
            atCmdRespOK(BtCmdEngine);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: Delete Pair failed");
            atCmdRespCmeError(BtCmdEngine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case APP_BT_DEV_PIN_REQ:
    {
        if (BT_SUCCESS == status)
        {
            BT_DEVICE_INFO *pdev_info = NULL;
            uint8 addr_string[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
            uint8 name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};

            if (NULL == pData)
            {
                OSI_LOGI(0, "BT#_AppBtMsgHandler: get pdata failed");
                break;
            }

            pdev_info = (BT_DEVICE_INFO *)pData;

            if (gAtBtCtx != NULL)
            {
                gAtBtCtx->addrPinAval = true;
                gAtBtCtx->addrPin = pdev_info->addr;
            }

            AppConvertBtAddrtoString(pdev_info->addr.addr, addr_string);
            AppConvertBtNametoString(pdev_info->name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));

            //+PASSKEYREQ:<address>,<name>
            sprintf((char *)rsp_str, "%s%s%s%s", "+PASSKEYREQ:", addr_string, ",", name_string);
            atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        }
    }
    break;
    case APP_BT_SSP_NUM_IND:
    {
        BT_DEVICE_INFO_EXT *pdev_info = NULL;
        uint8 addr_string[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};

        if (NULL == pData)
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: get pdata failed");
            break;
        }

        pdev_info = (BT_DEVICE_INFO_EXT *)pData;

        if (gAtBtCtx != NULL)
        {
            gAtBtCtx->addrPinAval = true;
            gAtBtCtx->addrPin = pdev_info->addr;
        }

        AppConvertBtAddrtoString(pdev_info->addr.addr, addr_string);

        //+SSPNUMIND:<ssp numeric key>,<address>
        if (pdev_info->ssp_numric_key[0] == 0xff)
        {
            sprintf((char *)rsp_str, "%s%s%s%s", "+SSPNUMIND:", "FFFFFF", ",", addr_string);
            atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        }
        else
        {
            sprintf((char *)rsp_str, "%s%s%s%s", "+SSPNUMIND:", pdev_info->ssp_numric_key, ",", addr_string);
            atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        }
    }
    break;
    case APP_BT_SPP_CONNECT_IND:
    {
        sprintf((char *)rsp_str, "%s: %d", "+CONNECTED", (BT_SUCCESS == status ? 0 : 1));
        atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        gAtBtCtx->status = AT_BT_ST_IDLE;
    }
    break;
    case APP_BT_SPP_DISCONNECT_IND:
    {
        if (gAtBtCtx->status == AT_BT_ST_DISCONNECT)
        {
            sprintf((char *)rsp_str, "%s: %d", "+DISCONNECTED", (BT_SUCCESS == status ? 0 : 1));
        }
        else
        {
            sprintf((char *)rsp_str, "%s", "+DISCONNECTED");
        }

        atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
        gAtBtCtx->status = AT_BT_ST_IDLE;
    }
    break;
    case APP_BT_SPP_DATA_RECIEVE_IND:
    {
        if (NULL == pData)
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: get pdata failed");
            break;
        }

        uint8_t *buf = NULL;
        uint32_t offset = 0;
        BT_AT_SPP_DATA *SppData = (BT_AT_SPP_DATA *)pData;

        offset = gAtBtCtx->SppData.dataLen;
        gAtBtCtx->SppData.dataLen += SppData->dataLen;
        buf = malloc(gAtBtCtx->SppData.dataLen);
        if (buf != NULL)
        {
            memset(buf, 0, gAtBtCtx->SppData.dataLen);
            if (offset > 0)
            {
                memcpy(buf, gAtBtCtx->SppData.buf, offset);
                free(gAtBtCtx->SppData.buf);
                gAtBtCtx->SppData.buf = NULL;
            }
            memcpy(buf + offset, SppData->buf, SppData->dataLen);
            gAtBtCtx->SppData.buf = buf;

            sprintf((char *)rsp_str, "%s, %d", "+DATARECV:SPP", SppData->dataLen);
        }
        else
        {
            OSI_LOGI(0, "BT#_AppBtMsgHandler: malloc spp data failed");
            gAtBtCtx->SppData.dataLen -= SppData->dataLen;
            sprintf((char *)rsp_str, "%s", "+DATARECV:ERR");
        }
        BT_FreeMenSppData((BT_AT_SPP_DATA *)pData);
        pData = NULL;
        atCmdRespInfoText(BtCmdEngine, (char *)rsp_str);
    }
    break;
    default:
        OSI_LOGI(0, "BT#_AppBtMsgHandler: unkown msgid=0x%x", pEvent->id);
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

void AT_BTApp_CmdFunc_ONOFF(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t onoff = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "BT#ONOFF: onoff:%d", onoff);

        if (onoff == 0)
        {
            if (BT_GetState())
            {
                if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
                {
                    OSI_LOGI(0, "BT#ONOFF: BT is busy");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }

                ret_val = BT_Stop();
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGI(0, "BT#ONOFF: BT_Stop() pending");
                    atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
                    break;
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
                OSI_LOGI(0, "BT#ONOFF: Stop bt fail as bt is already off!");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else if (onoff == 1)
        {
            if (!BT_GetState())
            {
                if (!AppRegisterBtMsgCB(APP_BT_MSG, _AppBtMsgHandler))
                {
                    OSI_LOGI(0, "BT#ONOFF: register msg callback failed");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }

                ret_val = BT_Start();
                if (BT_PENDING == ret_val)
                {
                    OSI_LOGI(0, "BT#ONOFF: BT_Start() pending");
                    BtCmdEngine = cmd->engine;
                    break;
                }
                else
                {
                    AppRegisterBtMsgCB(APP_BT_MSG, NULL);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }
            else
            {
                OSI_LOGI(0, "BT#ONOFF: start bt fail as bt is already on!");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    break;
    case AT_CMD_READ:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "+BTONOFF:%d", BT_GetState());
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTONOFF=<state>");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_VISIBLE(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t visible = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "BT#VISIBLE: visible = %d", visible);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#VISIBLE: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
        {
            OSI_LOGI(0, "BT#VISIBLE: BT is busy");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (0 == visible) // set hidden
        {
            ret_val = BT_SetVisibility(BT_PAGE_ENABLE); //set bt hidden

            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#VISIBLE: BT_SetVisibility() pendind");
                atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
                break;
            }
            else
            {
                OSI_LOGI(0, "BT#VISIBLE: BT_SetVisibility() failed, ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else if (1 == visible) // set visible
        {
            ret_val = BT_SetVisibility(BT_INQUIRY_ENABLE | BT_PAGE_ENABLE); //set bt visible
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#VISIBLE: BT_SetVisibility() pendind");
                atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
                break;
            }
            else
            {
                OSI_LOGI(0, "BT#VISIBLE: BT_SetVisibility() failed, ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_READ:
    {
        char rsp[128] = {0};
        uint32_t visible_info = 0;
        uint32_t is_visible = 0;

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#VISIBLE: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        visible_info = BT_GetVisibility();

        OSI_LOGI(0, "BT#VISIBLE: visible status = %d", visible_info);

        if ((BT_INQUIRY_ENABLE | BT_PAGE_ENABLE) == visible_info)
        {
            is_visible = 1; // Only when device is inquiry and page enable
        }
        else
        {
            is_visible = 0;
        }

        sprintf((char *)rsp, "+BTVISIBLE:%ld", is_visible);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTVISIBLE=<visible>");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_NAME(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        uint8_t name_type = ATC_BT_NAME_ASCII;
        uint16_t local_name[BT_DEVICE_NAME_SIZE] = {0};

        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        char *name = (char *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || (strlen(name) == 0) || (strlen(name) > ATC_MAX_BT_NAME_STRING_LEN))
        {
            OSI_LOGI(0, "BT#NAME: get bt name error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count == 2)
        {
            name_type = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                OSI_LOGI(0, "BT#NAME: get bt name type error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#NAME: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
        {
            OSI_LOGI(0, "BT#NAME: BT is busy");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        AppConvertStringtoBtName(name_type, (uint8_t *)name, strlen(name), local_name, BT_DEVICE_NAME_SIZE);
        ret_val = BT_SetLocalName(local_name);
        if (BT_PENDING == ret_val)
        {
            OSI_LOGI(0, "BT#NAME: BT_SetLocalName() pending");
            atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
            break;
        }
        else if (BT_SUCCESS == ret_val)
        {
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "BT#NAME: BT_SetLocalName() failed, ret_val = %d", ret_val);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case AT_CMD_READ:
    {
        char rsp[200] = {0};
        uint16_t device_name[BT_DEVICE_NAME_SIZE] = {0};
        uint8_t name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#NAME: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        OSI_LOGI(0, "BT#NAME: get bt name");

        ret_val = BT_GetLocalName(device_name);
        if (BT_SUCCESS == ret_val)
        {
            AppConvertBtNametoString(device_name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));
            sprintf((char *)rsp, "+BTNAME:%s", (char *)name_string);
            atCmdRespInfoText(cmd->engine, rsp);
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "BT#NAME: BT_GetLocalName() failed, ret_val = %d", ret_val);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTNAME=<name>[,<name_type>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

// AT+BTADDR=<addr>
void AT_BTApp_CmdFunc_ADDR(atCommand_t *cmd)
{
    char rsp[128] = {0};
    BT_ADDRESS addr;
    BT_STATUS ret_val;
    bool paramok = true;
    uint8_t addr_str[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        char *address = (char *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || (strlen(address) == 0))
        {
            OSI_LOGI(0, "BT#PAIR: get bt address error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (!AppConvertStringtoBtAddr((uint8_t *)address, strlen(address), addr.addr))
        {
            OSI_LOGI(0, "BT#PAIR: convert string to BtAddr failed");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (BT_GetState())
        {
            OSI_LOGI(0, "BT#ADDR: Bt is start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        ret_val = BT_SetBdAddr(&addr);
        if (BT_PENDING == ret_val)
        {
            OSI_LOGI(0, "BT#ADDR: BT_SetBdAddr() pending");
            atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
            break;
        }
        else if (BT_SUCCESS == ret_val)
        {
            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "BT#ADDR: BT_SetBdAddr() failed, ret_val = %d", ret_val);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case AT_CMD_READ:
    {
        if (BT_GetState())
        {
            OSI_LOGI(0, "BT#ADDR: Bt is start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        BT_GetBdAddr(&addr);
        AppConvertBtAddrtoString(addr.addr, addr_str);
        sprintf((char *)rsp, "+BTADDR:%s", (char *)addr_str);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        sprintf((char *)rsp, "AT+BTADDR=<addr>");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_INQ(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;
    uint32_t service_type = BT_SERVICE_ALL;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#INQ: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (mode == 1)
        {
            if (cmd->param_count != 1)
            {
                char *type = (char *)atParamStr(cmd->params[1], &paramok);
                if (!paramok || (strlen(type) == 0))
                {
                    OSI_LOGI(0, "BT#INQ: get bt type error");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                if (!AppConvertStringtoUint32((uint8_t *)type, strlen(type), &service_type))
                {
                    OSI_LOGI(0, "BT#INQ: convert string to uint32 failed");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }

            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
            {
                OSI_LOGI(0, "BT#INQ: BT is busy");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            ret_val = BT_SearchDevice(service_type);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#INQ: BT_SearchDevice() pending");
                char rsp[128] = {0};
                atCmdRespOK(cmd->engine);
                sprintf((char *)rsp, "%s", "+INQS");
                atCmdRespInfoText(cmd->engine, (char *)rsp);
                gAtBtCtx->status = AT_BT_ST_INQ;
            }
            else
            {
                OSI_LOGI(0, "BT#INQ: BT_SearchDevice ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else if (mode == 0)
        {
            if (cmd->param_count != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_INQ)
            {
                OSI_LOGI(0, "BT#INQ: BT didn't inq before");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            ret_val = BT_CancelSearch();
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#INQ: BT_CancelSearch() pending");
                atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
                break;
            }
            else if (BT_SUCCESS == ret_val)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#INQ: BT_CancelSearch ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_READ:
    {
        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#INQ: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

#if defined(CONFIG_BLUEU_BT_ENABLE)
        uint32 i = 0;
        uint32 state = BT_GetSearchDevice_state();
        if (state == BT_INQ_IDLE)
        {
            uint32 cnt = BT_GetSearchDeviceCount();
            for (i = 0; i < cnt; i++)
            {
                UINT8 addr[6] = {0};
                UINT8 name[32] = {0};
                char rsp[512] = {0};

                if (BT_GetSearchDeviceInfo(addr, name, i))
                {
                    uint8_t addr_str[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
                    sprintf((char *)addr_str, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", addr[0], ":", addr[1], ":", addr[2], ":", addr[3], ":", addr[4], ":", addr[5]);
                    sprintf((char *)rsp, "%s%s%s%s%s%s", "+BT Name:", "=", name, "+BT ADDR:", "=", addr_str);
                    atCmdRespInfoText(cmd->engine, (char *)rsp);
                }
            }

            RETURN_OK(cmd->engine);
        }
        else
        {
            OSI_LOGI(0, "BT#INQ: BT_GetSearchDevice_state() failed, ret_val = %d", state);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
#else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
#endif
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTINQ=<mode>[,<type>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_PAIR(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#PAIR: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (mode == 1)
        {
            BT_ADDRESS addr = {0};

            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *address = (char *)atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(address) == 0))
            {
                OSI_LOGI(0, "BT#PAIR: get bt address error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (!AppConvertStringtoBtAddr((uint8_t *)address, strlen(address), addr.addr))
            {
                OSI_LOGI(0, "BT#PAIR: convert string to BtAddr failed");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
            {
                OSI_LOGI(0, "BT#PAIR: BT is busy");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            ret_val = BT_PairDevice(&addr);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#PAIR: BT_PairDevice() pending");
                gAtBtCtx->status = AT_BT_ST_PAIR;
                RETURN_OK(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#PAIR: BT_PairDevice ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else if (mode == 0)
        {
            BT_ADDRESS addr = {0};

            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *address = (char *)atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(address) == 0))
            {
                OSI_LOGI(0, "BT#PAIR: get bt address error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (!AppConvertStringtoBtAddr((uint8_t *)address, strlen(address), addr.addr))
            {
                OSI_LOGI(0, "BT#PAIR: AppConvertStringtoBtAddr error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
            {
                OSI_LOGI(0, "BT#PAIR: BT is busy");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            ret_val = BT_CancelPair(&addr);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#PAIR: BT_CancelPair() pending");
                atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
                break;
            }
            else if (BT_SUCCESS == ret_val)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#PAIR: BT_CancelPair ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTPAIR=<mode>,<address>");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_PIN(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#PIN: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtBtCtx == NULL || !gAtBtCtx->addrPinAval)
        {
            OSI_LOGI(0, "BT#PIN: gAtBtCtx is invalid");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        char *pin = (char *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || (strlen(pin) == 0) || (strlen(pin) > 16))
        {
            OSI_LOGI(0, "BT#PIN: get bt pin error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        ret_val = BT_InputPairPin(&(gAtBtCtx->addrPin), (uint8_t *)pin, strlen(pin));
        if (BT_SUCCESS == ret_val)
        {
            RETURN_OK(cmd->engine);
        }
        else if (BT_PENDING == ret_val)
        {
            OSI_LOGI(0, "BT#PIN: BT_InputPairPin() pending");
            atCmdSetTimeoutHandler(cmd->engine, AT_BT_TIMEOUT * 1000, AT_BTTimeoutHandler);
            break;
        }
        else
        {
            OSI_LOGI(0, "BT#PIN: BT_InputPairPin ret_val = %d", ret_val);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTPIN=<pin>");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_LISTPD(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
    {
        char rsp[256] = {0};
        int device_number = 0;
        uint8_t index = 0;
        uint8_t addr_string[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
        uint8_t name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};
        BT_DEVICE_INFO dev = {0};

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#LISTPD: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
        {
            OSI_LOGI(0, "BT#LISTPD: BT is busy");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);
        OSI_LOGI(0, "BT#LISTPD: device_number = %d", device_number);

        for (index = 0; index < device_number; index++)
        {
            if (BT_GetPairedDeviceInfo(BT_SERVICE_ALL, index, &dev) == BT_SUCCESS)
            {
                //+LISTPD:11:22:33:44:55:66,devName1,00000408
                AppConvertBtAddrtoString(dev.addr.addr, addr_string);
                AppConvertBtNametoString(dev.name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));
                sprintf((char *)rsp, "%s%s%s%s%s%08x", "+LISTPD:", addr_string, ",", name_string, ",", (unsigned int)dev.dev_class);
                atCmdRespInfoText(cmd->engine, (char *)rsp);
            }
        }

        //+LISTPDE
        sprintf((char *)rsp, "+LISTPDE");
        atCmdRespInfoText(cmd->engine, (char *)rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTLISTPD");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_REMOVEPD(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "BT#REMOVEPD: mode = %d", mode);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#LISTPD: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
        {
            OSI_LOGI(0, "BT#LISTPD: BT is busy");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (mode == 0) //clear all device
        {
            int device_number = 0;
            uint8_t index = 0;
            BT_DEVICE_INFO dev = {0};

            device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);

            for (index = 0; index < device_number; index++)
            {
                ret_val = BT_GetPairedDeviceInfo(BT_SERVICE_ALL, 0, &dev);
                if (ret_val != BT_SUCCESS)
                {
                    OSI_LOGI(0, "BT#LISTPD: BT_GetPairedDeviceInfo() failed, ret_val = %d", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
                ret_val = BT_RemovePairedDevice(&(dev.addr));
                if (ret_val != BT_SUCCESS)
                {
                    OSI_LOGI(0, "BT#LISTPD: BT_RemovePairedDevice() failed, ret_val = %d", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
            }

            RETURN_OK(cmd->engine);
        }
        else if (mode == 1) //clear specific address device
        {
            BT_ADDRESS addr = {0};

            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *address = (char *)atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(address) == 0))
            {
                OSI_LOGI(0, "BT#REMOVEPD: get bt address error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (!AppConvertStringtoBtAddr((uint8_t *)address, strlen(address), addr.addr))
            {
                OSI_LOGI(0, "BT#REMOVEPD: convert string to BtAddr failed");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            ret_val = BT_RemovePairedDevice(&addr);
            if (BT_PENDING == ret_val)
            {
                OSI_LOGI(0, "BT#REMOVEPD: BT_RemovePairedDevice() pending");
                break;
            }
            else if (BT_SUCCESS == ret_val)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#REMOVEPD: BT_RemovePairedDevice() ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            break;
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTREMOVEPD=<mode>[,<address>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

void AT_BTApp_CmdFunc_CONNECT(atCommand_t *cmd)
{
    bool paramok = true;
    BT_STATUS ret_val = BT_SUCCESS;

    char rsp[256] = {0};
    char addressStr[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
    char *address = NULL;
    BT_ADDRESS addr = {0};

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {

        if (cmd->param_count != 2 && cmd->param_count != 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "BT#CONNECT: mode = %d", mode);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#CONNECT: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (mode == 1 || mode == 0)
        {
            if (((mode == 0) && (cmd->param_count != 2)) ||
                ((mode == 1) && (cmd->param_count != 3)))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *profile = (char *)atParamStr(cmd->params[1], &paramok);
            if (!paramok || (strlen(profile) == 0))
            {
                OSI_LOGI(0, "BT#CONNECT: get bt profile error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (cmd->param_count == 3)
            {
                address = (char *)atParamStr(cmd->params[2], &paramok);
                if (!paramok || (strlen(address) == 0))
                {
                    OSI_LOGI(0, "BT#CONNECT: get bt address error");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                if (!AppConvertStringtoBtAddr((uint8_t *)address, strlen(address), addr.addr))
                {
                    OSI_LOGI(0, "BT#CONNECT: convert string to BtAddr failed");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                strncpy(addressStr, address, ATC_MAX_BT_ADDRESS_STRING_LEN);
            }

            if (strcmp((char *)profile, "SPP") == 0)
            {
#ifdef BT_SPP_SUPPORT
                if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
                {
                    OSI_LOGI(0, "BT#CONNECT: BT is busy");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }

                if ((mode == 1) && (!BT_SppConnectStatus()))
                {
                    ret_val = BT_SppConnect(&addr, SPP_PORT_SPEED_115200);
                }
                else if ((mode == 0) && BT_SppConnectStatus())
                {
                    ret_val = BT_SppDisconnect();
                }
                else
                {
                    OSI_LOGI(0, "BT#CONNECT: SPP status is incorrect");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                }
#else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
#endif
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if ((ret_val == BT_PENDING) || (ret_val == BT_SUCCESS))
            {
                atCmdRespOK(cmd->engine);

                if (mode == 1)
                {
                    //+CONNECTING>>11:22:33:44:55:66
                    sprintf((char *)rsp, "%s%s", "+CONNECTING>> ", addressStr);
                    atCmdRespInfoText(cmd->engine, (char *)rsp);
                }

                if (ret_val == BT_SUCCESS)
                {
                    if (mode == 1)
                        sprintf((char *)rsp, "+CONNNECTED:0");
                    else
                        sprintf((char *)rsp, "+DISCONNNECTED:0");

                    atCmdRespInfoText(cmd->engine, (char *)rsp);
                }
                else
                {
                    gAtBtCtx->status = (mode == 1 ? AT_BT_ST_CONNECT : AT_BT_ST_DISCONNECT);
                }
            }
            else
            {
                OSI_LOGI(0, "BT#CONNECT: BT Connect()/Disconnect() failed, ret_val = %d", ret_val);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        char rsp[128] = {0};
        sprintf((char *)rsp, "AT+BTCONNECT=<mode>,<profile>[,<address>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

// AT+BTSEND = <profile>,<length>[,<type>]
void AT_BTApp_CmdFunc_SEND(atCommand_t *cmd)
{
    bool paramok = true;
    char rsp[256] = {0};
    uint32_t len = 0;
    uint8_t type = 0; // ascii default
    char *profile = NULL;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 2 && cmd->param_count != 3)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        profile = (char *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || (strlen(profile) == 0))
        {
            OSI_LOGI(0, "BT#SEND: get bt profile error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        len = atParamUintInRange(cmd->params[1], 1, 1024, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "BT#SEND: %d bytes to send", len);

        if (cmd->param_count == 3)
        {
            type = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // Now only support 0 (ascii)
        if (type != 0)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#SEND: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (strcmp((char *)profile, "SPP") == 0)
        {
#ifdef BT_SPP_SUPPORT
            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
            {
                OSI_LOGI(0, "BT#SEND: BT is busy");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            if (BT_SppConnectStatus() != true)
            {
                OSI_LOGI(0, "BT#SEND: SPP is not connected");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            gAtBtCtx->txbuf.sended = 0;
            gAtBtCtx->txbuf.profile = SPP;
            gAtBtCtx->txbuf.type = type;
            gAtBtCtx->txbuf.request = len;
            gAtBtCtx->txbuf.buffer = malloc(len);
            if (gAtBtCtx->txbuf.buffer != NULL)
            {
                BT_data_mode_start(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#SEND: Send buffer malloc failed");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
#endif
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    break;
    case AT_CMD_TEST:
    {
        sprintf((char *)rsp, "AT+BTSEND=<profile>,<length>[,<type>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

// AT+BTREAD = <profile> [,<type>]
void AT_BTApp_CmdFunc_READ(atCommand_t *cmd)
{
    uint32_t i = 0;
    bool paramok = true;
    char rsp[256] = {0};
    uint8_t type = 0;
    char *profile = NULL;
    char *format = NULL;

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1 && cmd->param_count != 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        profile = (char *)atParamStr(cmd->params[0], &paramok);
        if (!paramok || (strlen(profile) == 0))
        {
            OSI_LOGI(0, "BT#READ: get bt profile error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (cmd->param_count == 2)
        {
            type = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (!BT_GetState())
        {
            OSI_LOGI(0, "BT#READ: Bt is not start");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (strcmp((char *)profile, "SPP") == 0)
        {
#ifdef BT_SPP_SUPPORT
            if (gAtBtCtx == NULL || gAtBtCtx->status != AT_BT_ST_IDLE)
            {
                OSI_LOGI(0, "BT#READ: BT is busy");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            if (BT_SppConnectStatus() != true)
            {
                OSI_LOGI(0, "BT#READ: SPP is not connected");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            if (gAtBtCtx->SppData.dataLen != 0 && gAtBtCtx->SppData.buf != NULL)
            {
                atCmdWrite(cmd->engine, (const void *)"> ", 2);

                if (type == 0)
                {
                    atCmdWrite(cmd->engine, gAtBtCtx->SppData.buf, gAtBtCtx->SppData.dataLen);
                }
                else
                {
                    format = malloc(gAtBtCtx->SppData.dataLen * 2 + 2);
                    if (format == NULL)
                    {
                        OSI_LOGI(0, "BT#READ: malloc hex string failed");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }

                    memset(format, 0, gAtBtCtx->SppData.dataLen * 2 + 2);
                    for (i = 0; i < gAtBtCtx->SppData.dataLen; i++)
                        sprintf(format + 2 * i, "%02X", gAtBtCtx->SppData.buf[i]);
                    atCmdRespInfoText(cmd->engine, (char *)format);
                    free(format);
                }

                free(gAtBtCtx->SppData.buf);
                gAtBtCtx->SppData.buf = NULL;
                gAtBtCtx->SppData.dataLen = 0;
                RETURN_OK(cmd->engine);
            }
            else
            {
                OSI_LOGI(0, "BT#READ: no data to read");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_NOT_SURPORT);
#endif
        }
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }
    case AT_CMD_TEST:
    {
        sprintf((char *)rsp, "AT+BTREAD = <profile> [,<type>]");
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}
#endif
