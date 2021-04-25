#include "ats_config.h"
#ifdef CONFIG_AT_SPBLE_SUPPORT
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
#include "at_cmd_btmsg.h"
#include "bt_config.h"
#include "bt_abs.h"

#define ATC_BT_NAME_ASCII 0            //the bt name encode in ascii
#define ATC_BT_NAME_UCS2 1             //the bt name encode in ucs2
#define ATC_MAX_BT_NAME_STRING_LEN 164 //BT_DEVICE_NAME_SIZE = 41

//----------------------------------------------------------
#define TRACE_HEAD "SPBLE"
static atCmdEngine_t *g_atCmdEngine = NULL;

static bool AppCheckBTNameValid(unsigned char ch)
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

static void AppConvertBinToHex(
    uint8_t *bin_ptr, // in: the binary format string
    uint16_t length,  // in: the length of binary string
    uint8_t *hex_ptr  // out: pointer to the hexadecimal format string
)
{
    uint8_t semi_octet = 0;
    int32 i = 0;

    OSI_ASSERT(NULL != bin_ptr, "NULL == bin_ptr"); /*assert verified*/
    OSI_ASSERT(NULL != hex_ptr, "NULL == hex_ptr"); /*assert verified*/

    for (i = 0; i < length; i++)
    {
        // get the high 4 bits
        semi_octet = (uint8_t)((bin_ptr[i] & 0xF0) >> 4);
        if (semi_octet <= 9) //semi_octet >= 0
        {
            *hex_ptr = (uint8_t)(semi_octet + '0');
        }
        else
        {
            *hex_ptr = (uint8_t)(semi_octet + 'A' - 10);
        }

        hex_ptr++;

        // get the low 4 bits
        semi_octet = (uint8_t)(bin_ptr[i] & 0x0f);
        if (semi_octet <= 9) // semi_octet >= 0
        {
            *hex_ptr = (uint8_t)(semi_octet + '0');
        }
        else
        {
            *hex_ptr = (uint8_t)(semi_octet + 'A' - 10);
        }
        hex_ptr++;
    }
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
//  Description :convert the bt name to string
//              bt name is encoded in UCS2,if it is double byte ascii,convert to ascii,else convert to string
//              for example: 1) UCS2 004D004F0043004F0052,convert to MOCOR
//                           2) UCS2 5F204E09,convert to string 5F204E09
//  Global resource dependence : rsp
//  Author:       yongxia.zhang
//  Note:
/*****************************************************************************/
static void AppConvertBtNametoString(uint16_t *name, uint8_t name_arr_len, uint8_t *name_str_ptr, uint8_t name_str_arr_len)
{
    uint8_t i = 0, j = 0;
    uint8_t temp_name[200];

    memset(name_str_ptr, 0, name_str_arr_len);

    if (name[0] < 0x0080)
    {
        //if the name is encode in 2 type ascii
        AppConvertWstrToStr(name, name_str_ptr);
    }
    else
    {
        //if the name is encode in UCS2
        for (i = 0; i < name_arr_len; i++)
        {
            if (name[i] != '\0')
            {
                j++;
            }
            else
            {
                break;
            }
        }

        if (j == 0)
        {
            name_str_ptr[0] = '\0';
        }
        else
        {
            for (i = 0; i < j; i++)
            {
                temp_name[2 * i] = name[i] >> 8;
                temp_name[2 * i + 1] = name[i] & 0xff;
            }
            AppConvertBinToHex(temp_name, j * 2, name_str_ptr);
        }
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

static unsigned int StringsToUnsignedInt(char *_src)
{
    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    unsigned int val = strtoul(_src, &endptr, 10);
    if (*endptr != '\0' || errno != 0)
        return -1;
    return val;
}

static int AddrStringsToU8Int(char *_src, char *_des, int _length)
{
    int i = 0;
    int j = 0;
    char Tmp[18] = {0};
    int Length = 0;
    char TraceBuf[128] = {
        0,
    };

    for (i = 0, j = 0; i < _length; i++)
    {
        if (':' != _src[i])
        {
            Tmp[j] = _src[i];
            j++;
        }
    }

    Length = strlen(Tmp);

    for (i = 0; i < Length; i++)
    {
        if (Tmp[i] >= '0' && Tmp[i] <= '9')
            Tmp[i] = Tmp[i] - '0';
        else if (Tmp[i] >= 'A' && Tmp[i] <= 'F')
            Tmp[i] = 10 + (Tmp[i] - 'A');
        else if (Tmp[i] >= 'a' && Tmp[i] <= 'f')
            Tmp[i] = 10 + (Tmp[i] - 'a');
        else
            return -1;
    }

    for (i = 0, j = 0; i < Length; i += 2, j++)
    {
        _des[j] = ((16 * (Tmp[i])) + (Tmp[i + 1]));
    }

    TraceBuf[0] = '\0';
    sprintf(TraceBuf, "%s %s(%d):_src:%s _des[0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]", TRACE_HEAD, __func__, __LINE__, _src, _des[0], _des[1], _des[2], _des[3], _des[4], _des[5]);
    OSI_LOGI(0, TraceBuf);

    return 0;
}

static int AddrU8IntToStrings(char *_src, char *_des)
{
    sprintf(_des, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", _src[0], ":", _src[1], ":", _src[2], ":", _src[3], ":", _src[4], ":", _src[5]);
    return 0;
}

static char *del_char(char *s, int len, char c)
{
    int i = 0, j = 0;
    for (i = 0; i < len; i++)
    {
        if (s[i] != c)
        {
            s[j++] = s[i];
        }
    }
    s[j] = '\0';
    return (s);
}

#define TIMEOUT_NOT_RESPONSE (120 * 1000)
static void _AppBleNotResponseHandler(atCommand_t *cmd)
{
    AT_CMD_RETURN(atCmdRespCmeError(cmd->engine, ERR_AT_CME_NETWORK_TIMOUT));
}

static void _AppBleRecvTpDataCb(short _length, unsigned char *_data)
{
    if (_length > 255)
    {
        atCmdRespInfoText(g_atCmdEngine, "Ble receive tp data,but data length is too long.");
    }
    else
    {
        atCmdWrite(g_atCmdEngine, _data, _length);
    }

    return;
}

void _AppBleMsgHandler(void *param)
{
    char TraceBuf[64] = {
        0,
    };
    osiEvent_t *pEvent = (osiEvent_t *)param;
    if (NULL == pEvent)
    {
        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): param is NULL", TRACE_HEAD, __func__, __LINE__);
        OSI_LOGI(0, TraceBuf);
        return;
    }

    TraceBuf[0] = '\0';
    sprintf(TraceBuf, "%s %s(%d): APP event id = 0x%lx", TRACE_HEAD, __func__, __LINE__, pEvent->id);
    OSI_LOGI(0, TraceBuf);

    switch (pEvent->id)
    {
    //COMM
    case APP_BLE_SET_PUBLIC_ADDR:
    case APP_BLE_SET_RANDOM_ADDR:
    case APP_BLE_ADD_WHITE_LIST:
    case APP_BLE_REMOVE_WHITE_LIST:
    case APP_BLE_CLEAR_WHITE_LIST:
    case APP_BLE_DISCONNECT:
    case APP_BLE_UPDATA_CONNECT:
    {
        if (0 == ((int)pEvent->param1))
        {
            atCmdRespOKText(g_atCmdEngine, "+BLECOMM:OK");
        }
        else
        {
            atCmdRespErrorText(g_atCmdEngine, "+BLECOMM:ERR=error");
        }
        break;
    }
    case APP_BLE_CONNECT:
    {
        if (0 == ((int)pEvent->param1))
        {
            atCmdRespOKText(g_atCmdEngine, "+BLECOMM:OK");
            BLE_RegisterWriteUartCallback(_AppBleRecvTpDataCb);
        }
        else
        {
            atCmdRespErrorText(g_atCmdEngine, "+BLECOMM:ERR=error");
        }
        break;
    }
    //ADV
    case APP_BLE_SET_ADV_PARA:
    case APP_BLE_SET_ADV_DATA:
    case APP_BLE_SET_ADV_SCAN_RSP:
    {
        if (0 == ((int)pEvent->param1))
        {
            atCmdRespOKText(g_atCmdEngine, "+BLEADV:OK");
        }
        else
        {
            atCmdRespErrorText(g_atCmdEngine, "+BLEADV:ERR=error");
        }
        break;
    }
    case APP_BLE_SET_ADV_ENABLE:
    {
        if (0 == ((int)pEvent->param1))
        {
            atCmdRespOKText(g_atCmdEngine, "+BLEADV:OK");
            BLE_RegisterWriteUartCallback(_AppBleRecvTpDataCb);
        }
        else
        {
            atCmdRespErrorText(g_atCmdEngine, "+BLEADV:ERR=error");
        }
        break;
    }
    //SCAN
    case APP_BLE_SET_SCAN_PARA:
    case APP_BLE_SET_SCAN_ENABLE:
    case APP_BLE_SET_SCAN_DISENABLE:
    {
        if (0 == ((int)pEvent->param1))
        {
            atCmdRespOKText(g_atCmdEngine, "+BLESCAN:OK");
        }
        else
        {
            atCmdRespErrorText(g_atCmdEngine, "+BLESCAN:ERR=error");
        }
        break;
    }
    case APP_BLE_SET_SCAN_REPORT:
    {
        st_scan_report_info *pstScanRspInfo = NULL;
        pstScanRspInfo = (st_scan_report_info *)pEvent->param2;
        if (NULL != pstScanRspInfo)
        {
            char cScanRspInfo[128] = {'\0'};
            sprintf(cScanRspInfo, "%s:%d,%02x:%02x:%02x:%02x:%02x:%02x", pstScanRspInfo->name, pstScanRspInfo->addr_type, pstScanRspInfo->bdAddress.addr[0], pstScanRspInfo->bdAddress.addr[1], pstScanRspInfo->bdAddress.addr[2], pstScanRspInfo->bdAddress.addr[3], pstScanRspInfo->bdAddress.addr[4], pstScanRspInfo->bdAddress.addr[5]);
            atCmdRespInfoText(g_atCmdEngine, cScanRspInfo);
        }
        else
        {
            atCmdRespInfoText(g_atCmdEngine, "+BLESCAN:Test");
        }
        break;
    }
    default:
        break;
    }

    if (NULL != (void *)pEvent->param2)
    {
        free((void *)pEvent->param2);
    }

    if (NULL != pEvent)
    {
        free(pEvent);
    }

    return;
}

//AT+BLECOMM
void AT_SPBLE_CmdFunc_COMM(atCommand_t *pParam)
{
    char TraceBuf[128] = {
        0,
    };
    char RespBuf[128] = {
        0,
    };
    int Index = 0;
    //#define CMD_MAX_NUM (16)
    const char *CmdStr[17] =
        {
            "GETVERSION",
            "SETPUBLICADDR",
            "PUBLICADDR?",
            "SETRANDOMADDR",
            "RANDOMADDR?",
            "ADDWHITELIST",
            "REMOVEWHITELIST",
            "CLEANWHITELIST",
            "WHITELIST?",
            "SETNAME",
            "NAME?",
            "CONNECT",
            "DISCONNECT",
            "CONNECTION?",
            "CONNECTIONLIST?",
            "UPDATECONN",
            "SENDDATA"};

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        g_atCmdEngine = pParam->engine;
        pParam->params[0]->value[pParam->params[0]->length] = '\0';
        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
        OSI_LOGI(0, TraceBuf);

        for (Index = 0; Index < 16; Index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
            {
                break;
            }
        }

        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
        OSI_LOGI(0, TraceBuf);

        switch (Index)
        {
        case 0: //"GETVERSION"
        {
            const char *Version = NULL;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // call bt api for get version.
            Version = BT_BLE_GetBTVersionInfo();
            //       Version = "W0.0.0";

            sprintf(RespBuf, "+BLECOMM:OK=%s", Version);
            atCmdRespOKText(pParam->engine, RespBuf);

            return;
            break;
        }
        case 1: //"SETPUBLICADDR"
        {
            char *PublicAddr = NULL;
            char Addr[6] = {0};

            if (BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (pParam->params[1]->length > 17 || pParam->params[1]->length < 0)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. public addr. error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            PublicAddr = (char *)pParam->params[1]->value;
            AddrStringsToU8Int(PublicAddr, Addr, pParam->params[1]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):public addr is [%x %x %x %x %x %x].", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api set public addr
            BLE_SetPublicAddr((unsigned char *)Addr);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 2: //"PUBLICADDR?"
        {
            char Addr[6] = {0x01, 0x20, 0x00, 0x6E, 0x05, 0xC6};
            char PublicAddr[18] = "00:00:00:00:00:00";
            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // call bt api get public addr
            BLE_GetPublicAddr((unsigned char *)Addr);

            AddrU8IntToStrings(Addr, PublicAddr);
            sprintf(RespBuf, "+BLECOMM:PUBLICADDR=%s", PublicAddr);
            atCmdRespOKText(pParam->engine, RespBuf);

            return;
            break;
        }
        case 3: //"SETRANDOMADDR"
        {
            char *RandomAddr = NULL;
            char Addr[6] = {0};

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            RandomAddr = (char *)pParam->params[1]->value;
            AddrStringsToU8Int(RandomAddr, Addr, pParam->params[1]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):random addr is [%x %x %x %x %x %x].", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api set random addr
            BLE_SetRandomAddr((unsigned char *)Addr);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 4: //"RANDOMADDR?"
        {
            char Addr[6] = {0x01, 0x20, 0x00, 0x6E, 0x05, 0xC6};
            char RandomAddr[18] = "00:00:00:00:00:00";

            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // call bt api get random addr
            BLE_GetRandomAddr((unsigned char *)Addr);

            AddrU8IntToStrings(Addr, RandomAddr);
            sprintf(RespBuf, "+BLECOMM:RANDOMADDR=%s", RandomAddr);
            atCmdRespOKText(pParam->engine, RespBuf);

            return;
            break;
        }
        case 5: //"ADDWHITELIST"
        {
            char *WhiteListAddr = NULL;
            char Addr[6] = {0};
            int Type = -1;
            st_white_list_info white_list_info;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (3 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            Type = (int)StringsToUnsignedInt(pParam->params[1]->value); //	(pParam->params[1]->value[0] - '0');
            if (0 != Type && 1 != Type)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[2]->value[pParam->params[2]->length] = '\0';
            WhiteListAddr = (char *)pParam->params[2]->value;
            AddrStringsToU8Int(WhiteListAddr, Addr, pParam->params[2]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):white list addr is [%x %x %x %x %x %x], type is %d.", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5], Type);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api add white list
            white_list_info.addr_type = Type;
            memcpy(white_list_info.addr, Addr, BT_ADDRESS_SIZE);
            BLE_AddWhiteList(white_list_info);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 6: //"REMOVEWHITELIST"
        {
            char *WhiteListAddr = NULL;
            char Addr[6] = {0};

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            WhiteListAddr = (char *)pParam->params[1]->value;
            AddrStringsToU8Int(WhiteListAddr, Addr, pParam->params[1]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):white list addr is [%x %x %x %x %x %x].", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api remove white list
            BLE_RemoveWhiteList((unsigned char *)Addr);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 7: //"CLEANWHITELIST"
        {
            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api move white list
            BLE_ClearWhiteList();
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 8: //"WHITELIST?"
        {
            unsigned char WhiteCount = 0;
            //char WhiteList[128] = {0,};
            st_white_list_info stWhiteList = {0};

            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // call bt api get all white list.
            // If white list is empty, return NONE.
            WhiteCount = BLE_QuiryWhiteCount();
            if (0 == WhiteCount)
            {
                atCmdRespOKText(pParam->engine, "+BLECOMM:WHITELIST=NONE");
            }
            else
            {
                unsigned char i = 0;
                for (i = 0; i < WhiteCount; i++)
                {
                    BLE_QuiryWhiteListInfo(i, &stWhiteList);
                    sprintf(RespBuf, "+BLECOMM:WHITELIST=%d,%x:%x:%x:%x:%x:%x", stWhiteList.addr_type, stWhiteList.addr[0], stWhiteList.addr[1], stWhiteList.addr[2], stWhiteList.addr[3], stWhiteList.addr[4], stWhiteList.addr[5]);
                    atCmdRespInfoText(pParam->engine, RespBuf);
                }
                atCmdRespFinish(pParam->engine);
            }

            return;
            break;
        }
        case 9: //"SETNAME"
        {
            unsigned char i = 0;
            char *Name = NULL;
            uint8_t name_type = ATC_BT_NAME_ASCII;
            uint16_t local_name[BT_DEVICE_NAME_SIZE] = {0};

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (pParam->params[1]->length > 29)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[1] length error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            Name = (char *)pParam->params[1]->value;
            for (i = 0; i < strlen(Name); i++)
            {
                if (false == AppCheckBTNameValid(Name[i]))
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s set name valid.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);
                    atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                    return;
                }
            }
            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s name is %s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Name);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // call bt api set name.
            AppConvertStringtoBtName(name_type, (uint8_t *)Name, strlen(Name), local_name, BT_DEVICE_NAME_SIZE);
            BLE_SetLocalName(local_name);

            atCmdRespOKText(pParam->engine, "+BLECOMM:OK");

            return;
            break;
        }
        case 10: //"NAME?"
        {
            uint16_t device_name[BT_DEVICE_NAME_SIZE] = {0};
            uint8_t name_string[BT_DEVICE_NAME_SIZE] = {0};

            if (1 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            // TODO:
            // call bt api get name.
            BLE_GetLocalName(device_name);
            AppConvertBtNametoString(device_name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));
            sprintf(RespBuf, "+BLECOMM:NAME=%s", name_string);
            atCmdRespOKText(pParam->engine, RespBuf);

            return;
            break;
        }
        case 11: //"CONNECT"
        {
            char *BlueAddr = NULL;
            char Addr[6] = {0};
            int Type = -1;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (3 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            Type = (int)StringsToUnsignedInt(pParam->params[1]->value); //(pParam->params[1]->value[0] - '0');
            if (0 != Type && 1 != Type)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[1] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[2]->value[pParam->params[2]->length] = '\0';
            BlueAddr = (char *)pParam->params[2]->value;
            AddrStringsToU8Int(BlueAddr, Addr, pParam->params[2]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):connect addr is [%x %x %x %x %x %x], type is %d.", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5], Type);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api connect
            if (BT_IN_PROGRESS != BLE_Connect(Type, (unsigned char *)Addr))
            {
                atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);
            }
            else
            {
                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error, exist one connection");
            }

            return;
            break;
        }
        case 12: //"DISCONNECT"
        {
            char *BlueAddr = NULL;
            char Addr[6] = {0};

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            BlueAddr = (char *)pParam->params[1]->value;
            AddrStringsToU8Int(BlueAddr, Addr, pParam->params[1]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):disconnect addr is [%x %x %x %x %x %x].", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api disconnect blueteeth
            BLE_Disconnect((unsigned char *)Addr);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 13: //"CONNECTION?"
        {
            int State = -1;
            char *BlueAddr = NULL;
            char Addr[6] = {0};

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            BlueAddr = (char *)pParam->params[1]->value;
            AddrStringsToU8Int(BlueAddr, Addr, pParam->params[1]->length);

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d):state addr is [%x %x %x %x %x %x].", TRACE_HEAD, __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // call bt api get state.
            State = (int)BLE_GetConnectState((unsigned char *)Addr);
            switch (State)
            {
            case 0:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_NONE");
                break;
            case 1:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_ACTIVE");
                break;
            case 2:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_CLOSED");
                break;
            case 3:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_PENDING");
                break;
            case 4:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_RELEASEPENDING");
                break;
            case 5:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_PENDINGHOLDALLACLS");
                break;
            case 6:
                atCmdRespOKText(pParam->engine, "+BLECOMM:STATE=CONN_REQPENDING");
                break;
            default:
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s connect state error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);
                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
            }

            return;
            break;
        }
#if 0
			case 14: //"CONNECTIONLIST?"
			{
				char ConnectList[128] = {0,};
				int Handle = -1;
				if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLECOMM:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api get connect list. 

				sprintf(RespBuf, "+BLECOMM:CONNECTIONLIST=%d,%s", Handle, ConnectList);
	            atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);

				return ;
				break;
			}
#endif
        case 15: //"UPDATECONN"
        {
            int Handle = -1;
            int MaxInterval = -1;
            int MinInterval = -1;
            int Latency = -1;
            int Timeout = -1;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (6 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            Handle = StringsToUnsignedInt(pParam->params[1]->value);
            MinInterval = StringsToUnsignedInt(pParam->params[2]->value);
            MaxInterval = StringsToUnsignedInt(pParam->params[3]->value);
            Latency = StringsToUnsignedInt(pParam->params[4]->value);
            Timeout = StringsToUnsignedInt(pParam->params[5]->value);
            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s handle=%d Interval[%d,%d] Latency=%d Timeout=%d", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Handle, MinInterval, MaxInterval, Latency, Timeout);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api update connect
            BLE_UpdateConnect((unsigned short)Handle, (unsigned short)MinInterval, (unsigned short)MaxInterval, (unsigned short)Latency, (unsigned short)Timeout);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 16: //"SENDDATA"
        {
            char *DataBuf = NULL;
            int DataLength = 0;
            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            DataLength = pParam->params[1]->length;
            if (DataLength > 244)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[1]. length error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            pParam->params[1]->value[pParam->params[1]->length] = '\0';
            DataBuf = (char *)pParam->params[1]->value;
            atCmdWrite(pParam->engine, DataBuf, DataLength);

            // call bt api send data.
            BLE_SendTpData((short)DataLength, (unsigned char *)DataBuf);
            atCmdRespOKText(pParam->engine, "+BLECOMM:OK");

            return;
            break;
        }
        default:
        {
            OSI_LOGI(0, "SPBLE:COMM Index error.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        }
        break;
    }
    default:
    {
        OSI_LOGI(0, "SPBLE:COMM pParam->type error.");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    }
}
#if 0
//AT+BLESMP
void AT_SPBLE_CmdFunc_SMP(atCommand_t *pParam)
{
	char TraceBuf[64] = {0,};
	char RespBuf[32] = {0,};
    int Index = 0;
	//#define CMD_MAX_NUM (7)
	const char *CmdStr[7] =
        {
            "SETMODE",
            "MODE?",
            "SETPASSKEY",
            "PASSKEY?",
            "PAIRLIST",
            "REMOVEPAIR",
            "CLEANPAIR"
        };

	switch (pParam->type)
	{
		case AT_CMD_SET:
		{
            pParam->params[0]->value[pParam->params[0]->length] = '\0';
			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
			OSI_LOGI(0, TraceBuf);
	
			for (Index = 0; Index < 7; Index++)
        	{
	            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
	            {
	                break;
	            }
        	}

			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
	        OSI_LOGI(0, TraceBuf);

	        switch (Index)
	        {
	        case 0: //"SETMODE"
	        {
				int Mode = -1;

				if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				// TODO:
				// call bt api for set mode. 

	            atCmdRespInfoText(pParam->engine, "+BLESMP:OK");
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
	        }
			case 1: //"MODE?"
			{
				int Mode[18] = -1;
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api for get mode

				sprintf(RespBuf, "+BLESMP:MODE=%d", Mode);
				atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}
			case 2: //"SETPASSKEY"
			{
				int i = 0;
				char PassKey[7] = "000000";
	            if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				if (6 != pParam->params[1]->length)
				{
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param[1] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
				}
				
				pParam->params[1]->value[pParam->params[1]->length] = '\0';
				strcpy(PassKey, (char *)pParam->params[1]->value);
				for (i = 0; i < 6; i++)
				{
					if (PassKey[i] <= '0' || PassKey[i] >= '9')
					{
						TraceBuf[0] = '\0';
						sprintf(TraceBuf, "%s %s(%d): %s param[1] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
			        	OSI_LOGI(0, TraceBuf);

						atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
						atCmdRespFinish(pParam->engine);
			        	return ;						
					}
				}
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s passkey is %s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], PassKey);
				OSI_LOGI(0, TraceBuf);

				// TODO:
				// call bt api for set pass key

				atCmdRespInfoText(pParam->engine, "+BLESMP:OK");
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			case 3: //"PASSKEY?"
			{
				char PassKey[7] = {0,};
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api for get passkey

				sprintf(RespBuf, "+BLESMP:MODE=%s", PassKey);
				atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}
			case 4: //"PAIRLIST"
			{
				char PairList[18] = {0,};
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				// TODO:
				// call bt api get public addr
				
				if (0 == strlen(PairList))
				{
					strcpy(PairList, "NONE");
				}
				sprintf(RespBuf, "+BLESMP:PAIRLIST=%s", PairList);
	            atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			case 5: //"REMOVEPAIR"
			{
				char Addr[18] = "00:00:00:00:00:00";
				if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				pParam->params[1]->value[pParam->params[1]->length] = '\0';
				strcpy(Addr, (char *)pParam->params[1]->value);
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s addr is %s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Addr);
				OSI_LOGI(0, TraceBuf);
				
				// TODO:
				// call bt api add white list

	            atCmdRespInfoText(pParam->engine, " +BLESMP:OK");
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			case 6: //"CLEANPAIR"
			{
				if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLESMP:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s clean pair.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
				OSI_LOGI(0, TraceBuf);
				
				// TODO:
				// call bt api remove white list

	            atCmdRespInfoText(pParam->engine, " +BLESMP:OK");
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			default:
				
			}
		break;
		}
		default:
    	{
        	OSI_LOGI(0, "SPBLE: pParam->type error.");
        	RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    	}
	}
}
#endif
#if 0
//AT+BLEGATTSRV
void AT_SPBLE_CmdFunc_GATTSRV(atCommand_t *pParam)
{
	char TraceBuf[64] = {0,};
	char RespBuf[32] = {0,};
    int Index = 0;
	//#define CMD_MAX_NUM (6)
	const char *CmdStr[6] =
        {
            "ADDSERVICE",
            "SERVICE?",
            "SETNOTIFYINTERVAL",
            "NOTIFYINTERVAL?",
            "SETINDICATIONINTERVAL",
            "INDICATION?"
        };

	switch (pParam->type)
	{
		case AT_CMD_SET:
		{
            pParam->params[0]->value[pParam->params[0]->length] = '\0';
			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
			OSI_LOGI(0, TraceBuf);
	
			for (Index = 0; Index < 6; Index++)
        	{
	            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
	            {
	                break;
	            }
        	}

			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
	        OSI_LOGI(0, TraceBuf);

	        switch (Index)
	        {
	        case 0: //"ADDSERVICE"
	        {
				//int ParamCount = 0;
				//int i = 0;
	
				if (pParam->param_count < 2)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				// TODO:
				// call bt api for add service profile. 

	            atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:OK");
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
	        }
			case 1: //"SERVICE?"
			{
				int Profile = 0;
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api for get service profile.

				sprintf(RespBuf, "+BLEGATTSRV:SERVICE=%d", Profile);
				atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}
			case 2: //"SETNOTIFYINTERVAL"
			{
				int Interval = 0;
	            if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				Interval = pParam->params[1]->value - '0';		
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s interval is %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Interval);
				OSI_LOGI(0, TraceBuf);

				// TODO:
				// call bt api for set notify interval.

				atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:OK");
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			case 3: //"NOTIFYINTERVAL?"
			{
				int Interval = 0;
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api for get notify interval.

				sprintf(RespBuf, "+BLEGATTSRV:NOTIFYINTERVAL=%d", Interval);
				atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}
			case 4: //"SETINDICATIONINTERVAL"
			{
				int Interval = 0;
	            if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				Interval = pParam->params[1]->value - '0';		
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s interval is %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Interval);
				OSI_LOGI(0, TraceBuf);

				// TODO:
				// call bt api for set notify interval.

				atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:OK");
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}
			case 5: //"INDICATION?"
			{
				int Interval = 0;
	            if (1 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }
				
				// TODO:
				// call bt api for get notify interval.

				sprintf(RespBuf, "+BLEGATTSRV:INDICATIONINTERVAL=%d", Interval);
				atCmdRespInfoText(pParam->engine, RespBuf);
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}			
			default:
				
			}
		break;
		}
		default:
    	{
        	OSI_LOGI(0, "SPBLE: pParam->type error.");
        	RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    	}
	}
}
#endif
//AT+BLEADV
void AT_SPBLE_CmdFunc_ADV(atCommand_t *pParam)
{
    char TraceBuf[128] = {
        0,
    };
    //char RespBuf[32] = {0,};
    int Index = 0;
    //#define CMD_MAX_NUM (4)
    const char *CmdStr[4] =
        {
            "SETADVPARAM",
            "SETADVDATA",
            "SETADVENABLE",
            "SETSCANRSPDATA"};

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        g_atCmdEngine = pParam->engine;
        pParam->params[0]->value[pParam->params[0]->length] = '\0';
        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
        OSI_LOGI(0, TraceBuf);

        for (Index = 0; Index < 4; Index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
            {
                break;
            }
        }

        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
        OSI_LOGI(0, TraceBuf);

        switch (Index)
        {
        case 0: //"SETADVPARAM"
        {
            int AdvMin = -1;
            int AdvMax = -1;
            int AdvType = -1;
            int OwnAddrType = -1;
            int DirectAddrType = -1; //can default
            char *DirectAddr = NULL; //can default
            //char Addr[6] = {0};      //can default
            bdaddr_t stAddr = {0};
            int AdvChannMap = -1;
            int AdvFilter = -1;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (7 != pParam->param_count && 9 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            AdvMin = StringsToUnsignedInt(pParam->params[1]->value);
            if (AdvMin < 0x20 || AdvMin > 0x4000)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[AdvMin] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }
            else
            {
                //AdvMin = AdvMin * 625 / 1000;
            }

            AdvMax = StringsToUnsignedInt(pParam->params[2]->value);
            if (AdvMax < 0x20 || AdvMax > 0x4000)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[AdvMax] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }
            else
            {
                //    AdvMax = AdvMax * 625 / 1000;
            }

            if (AdvMin > AdvMax)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[AdvMin, AdvMax] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            AdvType = StringsToUnsignedInt(pParam->params[3]->value);
            switch (AdvType)
            {
            case 0:
            case 1:
            case 4:
                break;
            case 2:
            case 3:
            {
                if (AdvMin < 0xA0)
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvMin, AdvMax, AdvType] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }
                break;
            }
            default:
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[AdvType] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }
            }

            OwnAddrType = StringsToUnsignedInt(pParam->params[4]->value);
            if (OwnAddrType < 0 || OwnAddrType > 3)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[OwnAddrType] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            if (7 == pParam->param_count)
            {
                AdvChannMap = StringsToUnsignedInt(pParam->params[5]->value);
                if (0 == (AdvChannMap & 0x00000007))
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvChannMap] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }
                AdvFilter = StringsToUnsignedInt(pParam->params[6]->value);
                if (AdvFilter < 0 || AdvFilter > 3)
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvFilter] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }
            }
            else if (9 == pParam->param_count)
            {
                DirectAddrType = StringsToUnsignedInt(pParam->params[5]->value);
                if (DirectAddrType < 0 || DirectAddrType > 1)
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvFilter] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }

                DirectAddr = (char *)pParam->params[6]->value;
                AddrStringsToU8Int(DirectAddr, (char *)stAddr.bytes, pParam->params[6]->length);

                AdvChannMap = StringsToUnsignedInt(pParam->params[7]->value);
                if (0 == (AdvChannMap & 0x00000007))
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvChannMap] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }

                AdvFilter = StringsToUnsignedInt(pParam->params[8]->value);
                if (AdvFilter < 0 || AdvFilter > 3)
                {
                    TraceBuf[0] = '\0';
                    sprintf(TraceBuf, "%s %s(%d): %s param[AdvFilter] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                    OSI_LOGI(0, TraceBuf);

                    atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                    return;
                }
            }

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s count=%d [%d,%d,%d,%d,%d,%d,%d]", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], pParam->param_count, AdvMin, AdvMax, AdvType, OwnAddrType, AdvChannMap, AdvFilter, DirectAddrType);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for add set adv param.
            BLE_SetAdvPara((unsigned short)AdvMin, (unsigned short)AdvMax, (unsigned char)AdvType, (unsigned char)OwnAddrType, (unsigned char)DirectAddrType, stAddr, (unsigned char)AdvChannMap, (unsigned char)AdvFilter);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);
            return;
            break;
        }
        case 1: //"SETADVDATA"
        {
            int Leng = -1;
            char *Data = NULL;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (3 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            Leng = StringsToUnsignedInt(pParam->params[1]->value);
            Data = (char *)pParam->params[2]->value;
            if ((Leng * 2) != pParam->params[2]->length)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Leng, Data] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }
            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s Leng=%d, Data:%s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Leng, Data);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for set adv data.
            BLE_SetAdvData(Leng, Data);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 2: //"SETADVENABLE"
        {
            int AdvEnable = 0;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            AdvEnable = StringsToUnsignedInt(pParam->params[1]->value);
            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s adv enable is %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], AdvEnable);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for set adv enable.
            BLE_SetAdvEnable((unsigned char)AdvEnable);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 3: //"SETSCANRSPDATA"
        {
            int Leng = -1;
            char *ScanResponseData = NULL;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (3 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }

            Leng = StringsToUnsignedInt(pParam->params[1]->value);
            ScanResponseData = (char *)pParam->params[2]->value;
            if ((Leng * 2) != pParam->params[2]->length)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Leng, Data] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLEADV:ERR=error");
                return;
            }
            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s Leng=%d, ScanResponseData:%s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Leng, ScanResponseData);
            OSI_LOGI(0, TraceBuf);

            //String to unsigned long
            unsigned char adv_data[31] = {0};
            unsigned char i = 0, index = 0;
            char *adv_data_tmp = del_char(ScanResponseData, strlen(ScanResponseData), ' ');
            char hex_data[2] = {0};
            for (i = 0; i < strlen(adv_data_tmp);)
            {
                memcpy(hex_data, &adv_data_tmp[i], 2);
                adv_data[index] = strtoul(hex_data, NULL, 16) & 0xFF;
                //printf("\nlength:%d,adv_data[%d]:%x\n",strlen(adv_data_tmp),index,adv_data[index]);
                index++;
                i += 2;
            }

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for set adv data.
            BLE_SetScanRsp(adv_data, Leng);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        default:
        {
            OSI_LOGI(0, "SPBLE:ADV Index error.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        }
        break;
    }
    default:
    {
        OSI_LOGI(0, "SPBLE: ADV pParam->type error.");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    }
}
#if 0
//AT+BLEEXTADV
void AT_SPBLE_CmdFunc_EXTADV(atCommand_t *pParam)
{
	char TraceBuf[64] = {0,};
	char RespBuf[32] = {0,};
    int Index = 0;
	//#define CMD_MAX_NUM (3)
	const char *CmdStr[3] =
        {
            "SETADVPARAM",
            "SETADVENABLE",
            "SETSYNCADVENABLE"
        };

	switch (pParam->type)
	{
		case AT_CMD_SET:
		{
            pParam->params[0]->value[pParam->params[0]->length] = '\0';
			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
			OSI_LOGI(0, TraceBuf);
	
			for (Index = 0; Index < 3; Index++)
        	{
	            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
	            {
	                break;
	            }
        	}

			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
	        OSI_LOGI(0, TraceBuf);

	        switch (Index)
	        {
	        case 0: //"SETADVPARAM"
	        {
	
				if (9 != pParam->param_count && 11 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEEXTADV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				// TODO:
				// call bt api for add set extadv param. 
				// set bt callback for response.
	            
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
	        }
			case 1: //"SETADVENABLE"
			{
				int ExtAdvEnable = 0;
	            if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEEXTADV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				ExtAdvEnable = pParam->params[1]->value[0] - '0';
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s ext adv enable is %s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], ExtAdvEnable);
				OSI_LOGI(0, TraceBuf);
				
				// TODO:
				// call bt api for get service profile.
				// set bt callback for response.

				atCmdRespFinish(pParam->engine);
				return ;
	            break;
			}
			case 2: //"SETSYNCADVENABLE"
			{
				int SyncAdvEnable = 0;
	            if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEEXTADV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				SyncAdvEnable = pParam->params[1]->value[0] - '0';		
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s sync adv enable is %s.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], SyncAdvEnable);
				OSI_LOGI(0, TraceBuf);

				// TODO:
				// call bt api for set scan response data.
				// set bt callback for response.
				
				atCmdRespFinish(pParam->engine);
				return ;
				break;
			}					
			default:
				
			}
		break;
		}
		default:
    	{
        	OSI_LOGI(0, "SPBLE: pParam->type error.");
        	RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    	}
	}
}
#endif

//AT+BLESCAN
void AT_SPBLE_CmdFunc_SCAN(atCommand_t *pParam)
{
    char TraceBuf[128] = {
        0,
    };
    //char RespBuf[32] = {0,};
    int Index = 0;
    //#define CMD_MAX_NUM (2)
    const char *CmdStr[2] =
        {
            "SETSCANPARAM",
            "SETSCANENABLE"};

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        g_atCmdEngine = pParam->engine;
        pParam->params[0]->value[pParam->params[0]->length] = '\0';
        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
        OSI_LOGI(0, TraceBuf);

        for (Index = 0; Index < 2; Index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
            {
                break;
            }
        }

        TraceBuf[0] = '\0';
        sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
        OSI_LOGI(0, TraceBuf);

        switch (Index)
        {
        case 0: //"SETSCANPARM"
        {
            int Type = 0;
            int Interval = 0;
            int Window = 0;
            int Filter = 0;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (5 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }

            Type = StringsToUnsignedInt(pParam->params[1]->value);
            if (0 != Type && 1 != Type)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Type] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }

            Interval = StringsToUnsignedInt(pParam->params[2]->value);
            if (Interval < 0x4 || Interval > 0x4000)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Interval] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }
            else
            {
                Interval = Interval * 625 / 1000;
            }

            Window = StringsToUnsignedInt(pParam->params[3]->value);
            if (Window < 0x4 || Window > 0x4000)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Window] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }
            else
            {
                Window = Window * 625 / 1000;
            }

            Filter = StringsToUnsignedInt(pParam->params[4]->value);
            if (Filter < 0 || Filter > 3)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[Filter] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s scan param. is %d %d %d %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Type, Interval, Window, Filter);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for set scan param..
            BLE_SetScanPara((unsigned int)Type, (unsigned short)Interval, (unsigned short)Window, (unsigned int)Filter);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            return;
            break;
        }
        case 1: //"SETSCANENABLE"
        {
            int ScanEnable = 0;

            if (!BT_GetState())
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s Bt is not start.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLECOMM:ERR=error");
                return;
            }

            if (2 != pParam->param_count)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }

            ScanEnable = StringsToUnsignedInt(pParam->params[1]->value);
            if (0 != ScanEnable && 1 != ScanEnable)
            {
                TraceBuf[0] = '\0';
                sprintf(TraceBuf, "%s %s(%d): %s param[ScanEnable] error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
                OSI_LOGI(0, TraceBuf);

                atCmdRespErrorText(pParam->engine, "+BLESCAN:ERR=error");
                return;
            }

            TraceBuf[0] = '\0';
            sprintf(TraceBuf, "%s %s(%d): %s scan enable is %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], ScanEnable);
            OSI_LOGI(0, TraceBuf);

            // TODO:
            // set bt callback for response.
            AppRegisterBtMsgCB(APP_BLE_MSG, _AppBleMsgHandler);
            // call bt api for set scan enable or unenable.
            BLE_SetScanEnable((unsigned char)ScanEnable);
            atCmdSetTimeoutHandler(pParam->engine, TIMEOUT_NOT_RESPONSE, _AppBleNotResponseHandler);

            //atCmdRespOKText(pParam->engine, "+BLESCAN:OK");
            return;
            break;
        }
        default:
        {
            OSI_LOGI(0, "SPBLE:SCAN Index error.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        }
        break;
    }
    default:
    {
        OSI_LOGI(0, "SPBLE:SCAN pParam->type error.");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    }
}
#if 0
//AT+BLEGATTCLI
void AT_SPBLE_CmdFunc_GATTCLI(atCommand_t *pParam)
{
	char TraceBuf[64] = {0,};
	char RespBuf[32] = {0,};
    int Index = 0;
	//#define CMD_MAX_NUM (1)
	const char *CmdStr[1] =
        {
            "TESTPROFILE"
        };

	switch (pParam->type)
	{
		case AT_CMD_SET:
		{
            pParam->params[0]->value[pParam->params[0]->length] = '\0';
			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): %s", TRACE_HEAD, __func__, __LINE__, (char *)pParam->params[0]->value);
			OSI_LOGI(0, TraceBuf);
	
			for (Index = 0; Index < 1; Index++)
        	{
	            if (!strcmp((char *)pParam->params[0]->value, CmdStr[Index]))
	            {
	                break;
	            }
        	}

			TraceBuf[0] = '\0';
			sprintf(TraceBuf, "%s %s(%d): Index = %d", TRACE_HEAD, __func__, __LINE__, Index);
	        OSI_LOGI(0, TraceBuf);

	        switch (Index)
	        {
	        case 0: //"TESTPROFILE"
	        {
				int Number = 0;
				if (2 != pParam->param_count)
	            {
					TraceBuf[0] = '\0';
					sprintf(TraceBuf, "%s %s(%d): %s param. count error.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index]);
		        	OSI_LOGI(0, TraceBuf);

					atCmdRespInfoText(pParam->engine, "+BLEGATTSRV:ERR=error");
					atCmdRespFinish(pParam->engine);
		        	return ;
	            }

				Number = pParam->params[1]->value[0] - '0';
				TraceBuf[0] = '\0';
				sprintf(TraceBuf, "%s %s(%d): %s profile numberis %d.", TRACE_HEAD, __func__, __LINE__, CmdStr[Index], Number);
				OSI_LOGI(0, TraceBuf);
				
				// TODO:
				// call bt api for add set extadv param. 
				// set bt callback for response.
	            
				atCmdRespFinish(pParam->engine);
				return ;
	            break;
	        }										
			default:
				
			}
		break;
		}
		default:
    	{
        	OSI_LOGI(0, "SPBLE: pParam->type error.");
        	RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    	}
	}
}
#endif
#endif
