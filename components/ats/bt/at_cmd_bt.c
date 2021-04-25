/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "connectivity_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#if 0 
//#ifdef CONFIG_BLUEU_BT_ENABLE
#include "bt_config.h"
#include "ats_config.h"
#include "at_cmd_bt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt_cfg.h"
#include "sci_types.h"
#include "connectivity_config.h"
#include "bt_abs.h"
//#include <assert.h>

typedef struct
{
    uint8 param_len;
    uint8 param[ATC_SPBBTTEST_MAX_LEN];
} ATC_SPBTTEST_PARAM_T;

static struct atCmdEngine *engine_spp = NULL;

void SPPatCmdRespInfoText(const char *text)
{
    if (engine_spp)
    {
        atCmdRespInfoText(engine_spp, (char *)text);
    }
}
BOOLEAN ConvertStringtoUint32(uint8 *str_ptr, uint8 length, uint32 *num_ptr)
{
    uint8 i;
    uint32 numeric_value = 0;

    if ((NULL == str_ptr) || (length < 1) || (length > 8))
    {
        return FALSE;
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
            SCI_TRACE_LOW("ATC: ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return FALSE;
        }
    }

    SCI_TRACE_LOW("ATC: ConvertStringtoUint32,get the number: 0x%8x", numeric_value);

    *num_ptr = numeric_value;

    return TRUE;
}

BOOLEAN ConvertStringtoBtAddr(uint8 *str_ptr, uint8 length, uint8 *addr)
{
    uint8 i = 0, j = 0;
    uint8 hex_value = 0;

    if ((NULL == str_ptr) || (length != 17))
    {
        return FALSE;
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
            SCI_TRACE_LOW("ATC: ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return FALSE;
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
            SCI_TRACE_LOW("ATC: ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return FALSE;
        }

        addr[j++] = hex_value;
        if (j == 6)
            break;

        i++;
        if (str_ptr[i] != ':')
        {
            SCI_TRACE_LOW("ATC: ConvertStringtoUint32,invalid param, not numeric, return position:%d", i);
            return FALSE;
        }
    }

    SCI_TRACE_LOW("ATC: ConvertStringtoUint32,get the bt addr: 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    return TRUE;
}
void ConvertBtAddrtoString(uint8 *addr, uint8 *str_ptr)
{
    sprintf((char *)str_ptr, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", addr[0], ":", addr[1], ":", addr[2], ":", addr[3], ":", addr[4], ":", addr[5]);
}

BOOLEAN ConvertHexToBin( // FALSE: There is invalid char
    uint8 *hex_ptr,      // in: the hexadecimal format string
    uint16 length,       // in: the length of hexadecimal string
    uint8 *bin_ptr       // out: pointer to the binary format string
)
{
    uint8 *dest_ptr = bin_ptr;
    int32 i = 0;
    uint8 ch = 0;

    OSI_ASSERT(PNULL != hex_ptr, "PNULL == hex_ptr"); /*assert verified*/
    OSI_ASSERT(PNULL != bin_ptr, "PNULL == bin_ptr"); /*assert verified*/

    for (i = 0; i < length; i += 2)
    {
        // the bit 8,7,6,5
        ch = hex_ptr[i];
        // digital 0 - 9
        if (ch >= '0' && ch <= '9')
        {
            *dest_ptr = (uint8)((ch - '0') << 4);
        }
        // a - f
        else if (ch >= 'a' && ch <= 'f')
        {
            *dest_ptr = (uint8)((ch - 'a' + 10) << 4);
        }
        // A - F
        else if (ch >= 'A' && ch <= 'F')
        {
            *dest_ptr = (uint8)((ch - 'A' + 10) << 4);
        }
        else
        {
            return FALSE;
        }

        // the bit 1,2,3,4
        ch = hex_ptr[i + 1];
        // digtial 0 - 9
        if (ch >= '0' && ch <= '9')
        {
            *dest_ptr |= (uint8)(ch - '0');
        }
        // a - f
        else if (ch >= 'a' && ch <= 'f')
        {
            *dest_ptr |= (uint8)(ch - 'a' + 10);
        }
        // A - F
        else if (ch >= 'A' && ch <= 'F')
        {
            *dest_ptr |= (uint8)(ch - 'A' + 10);
        }
        else
        {
            return FALSE;
        }

        dest_ptr++;
    }
    return TRUE;
}

void ConvertBinToHex(
    uint8 *bin_ptr, // in: the binary format string
    uint16 length,  // in: the length of binary string
    uint8 *hex_ptr  // out: pointer to the hexadecimal format string
)
{
    uint8 semi_octet = 0;
    int32 i = 0;

    OSI_ASSERT(PNULL != bin_ptr, "PNULL == bin_ptr"); /*assert verified*/
    OSI_ASSERT(PNULL != hex_ptr, "PNULL == hex_ptr"); /*assert verified*/

    for (i = 0; i < length; i++)
    {
        // get the high 4 bits
        semi_octet = (uint8)((bin_ptr[i] & 0xF0) >> 4);
        if (semi_octet <= 9) //semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            *hex_ptr = (uint8)(semi_octet + 'A' - 10);
        }

        hex_ptr++;

        // get the low 4 bits
        semi_octet = (uint8)(bin_ptr[i] & 0x0f);
        if (semi_octet <= 9) // semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            *hex_ptr = (uint8)(semi_octet + 'A' - 10);
        }
        hex_ptr++;
    }
}

uint8 *ConvertWstrToStr(
    const uint16 *src,
    uint8 *dst)
{
    uint8 *return_str = dst;

    if (src == PNULL || PNULL == dst)
    {
        return return_str;
    }

    while (!(*src & 0xFF00) && '\0' != (*dst++ = *src++))
    {
        ;
    }

    return return_str;
}

uint16 *ConvertStrToWstr(
    const uint8 *src,
    uint16 *dst)
{
    uint16 *return_wstr = dst;

    if (src == PNULL || PNULL == dst)
    {
        return return_wstr;
    }

    while ('\0' != (*dst++ = *src++)) /* copy string */
    {
        ;
    }

    return return_wstr;
}

void ConvertBtNametoString(uint16 *name, uint8 name_arr_len, uint8 *name_str_ptr, uint8 name_str_arr_len)
{
    uint8 i = 0, j = 0;
    uint8 temp_name[200];

    SCI_MEMSET(name_str_ptr, 0, name_str_arr_len);

    if (name[0] < 0x0080)
    {
        //if the name is encode in 2 type ascii
        ConvertWstrToStr(name, name_str_ptr);
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
            ConvertBinToHex(temp_name, j * 2, name_str_ptr);
        }
    }
}

void ConvertStringtoBtName(uint8 type, uint8 *str, uint8 str_len, uint16 *name, uint8 name_arr_count)
{
    uint8 temp_name[200] = {0};
    uint8 i = 0, j = 0;

    SCI_MEMSET(name, 0, name_arr_count * 2);

    if (type == ATC_BT_NAME_ASCII)
    {
        //transfer uint8 ascii to ucs2
        ConvertStrToWstr(str, name);
    }
    else
    {
        //transfer hex string to bin
        ConvertHexToBin(str, str_len, temp_name);

        //copy uint8 to uint16
        for (i = 0; i < (name_arr_count - 1); i++)
        {
            if (i > (sizeof(temp_name) - 2))
                break;

            if ((temp_name[i] == 0x00) &&
                (temp_name[i + 1] == 0x00)) //lint !e661
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

#endif

void atCmdHandleSPBTCTRL(atCommand_t *cmd)
{
#if 0 
//#ifdef CONFIG_BLUEU_BT_ENABLE

    char g_rsp_str[1024] = {0};
    uint32 type_oper;
    ATC_SPBTTEST_PARAM_T param[9] = {0};

    const char *s_btctrl_str[38] =
        {
            "ONOFF", "VISIBLE", "NAME", "ADDR", "INQ", "INQC", "PAIR", "PAIRC", "PIN", "LISTPD", "REMOVEPD", "CONNECT", "ADV", "ADVDATA", "CONPARA", "LINKNUM", "SLVAUTH", "DEBUG", "TEST", "HFU", "A2DPSNK", "AVRCPCT", "HID", "SPP",
            "GATSREG", "GATSS", "GATSC", "GATSD", "GATSST", "GATSIND", "GATSRSP", "GATSDISC", "HOGP", "GATTSETNOTIFY", "SCAN", "SCANC", "LEPAIR", "LEPAIRC"};
    int bttest_index;
    BOOLEAN quiry_flag = FALSE;
    BT_STATUS ret_val = BT_SUCCESS;
    uint8 i;

    OSI_LOGI(0, "ATC: SPBTCTRL,enter atCmdHandleSPBTCTRL");

    for (i = 0; i < cmd->param_count; i++)
    {
        param[i].param_len = cmd->params[i]->length;
        if (param[i].param_len > ATC_SPBBTTEST_MAX_LEN)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        strncpy((char *)param[i].param, cmd->params[i]->value, param[i].param_len);
    }

    switch (cmd->type)
    {
    case AT_CMD_SET:

        if ('?' == param[0].param[param[0].param_len - 1])
        {
            quiry_flag = TRUE;
        }

        for (bttest_index = 0; bttest_index < 38; bttest_index++)
        {
            if (quiry_flag)
            {
                if (!strncmp((char *)param[0].param, s_btctrl_str[bttest_index], param[0].param_len - 1))
                {
                    break;
                }
            }
            else
            {
                if (!strncmp((char *)param[0].param, s_btctrl_str[bttest_index], param[0].param_len))
                {
                    break;
                }
            }
        }

        OSI_LOGI(0, "ATC: SPBTCTRL, bttest_index =%d,quiry_flag=%d", bttest_index, quiry_flag);
        switch (bttest_index)
        {

        case 0: //"ONOFF"
        {
            engine_spp = cmd->engine;
            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: SPBTCTRL,get bt state");
                //+SPBTCTRL:ONOFF=<state>
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", BT_GetState());
                atCmdRespInfoText(cmd->engine, g_rsp_str);
                ;
                break; //OK
            }
            else if (param[1].param_len != 0)
            {
                type_oper = param[1].param[0] - '0';

                if (0 == type_oper) // bt stop
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,stop bt");
                    if (BT_GetState())
                    {
                        ret_val = BT_Stop();
                        if (BT_PENDING == ret_val)
                        {
                            break;
                        }
                        else
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                        }
                    }
                    else
                    {
                        sprintf((char *)g_rsp_str, "stop bt fail,bt is already off!");
                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else if (1 == type_oper) // bt start
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,start bt");
                    if (!BT_GetState())
                    {
                        ret_val = BT_Start();
                        if (BT_PENDING == ret_val)
                        {
                            break;
                        }
                        else
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                        }
                    }
                    else
                    {
                        sprintf((char *)g_rsp_str, "start bt fail,bt is already on!");
                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,ONOFF param error,param=%d", type_oper);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            break;
        }

        case 1: //"VISIBLE"
        {
            if (quiry_flag)
            {
                uint32 visible_info = 0;
                BOOLEAN is_visible = FALSE;

                if (BT_GetState())
                {
                    visible_info = BT_GetVisibility();
                    SCI_TRACE_LOW("ATC: SPBTCTRL,visible status is = %d", visible_info);

                    if ((BT_INQUIRY_ENABLE | BT_PAGE_ENABLE) == visible_info)
                    {
                        // Only when device is inquiry and page enable
                        is_visible = TRUE;
                    }
                    else
                    {
                        is_visible = FALSE;
                    }
                }
                else
                {
                    is_visible = FALSE;
                }

                //+SPBTCTRL:VISIBLE=<visible>
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", is_visible);
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else if (param[1].param_len != 0)
            {
                type_oper = param[1].param[0] - '0';

                if (0 == type_oper) // set hidden
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,set hidden");
                    ret_val = BT_SetVisibility(BT_PAGE_ENABLE); //set blue tooth hidden
                    if (BT_PENDING == ret_val)
                    {
                        break;
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else if (1 == type_oper) // set visible
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,set visible");
                    ret_val = BT_SetVisibility(BT_INQUIRY_ENABLE | BT_PAGE_ENABLE); //set blue tooth visible
                    if (BT_PENDING == ret_val)
                    {
                        break;
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    SCI_TRACE_LOW("ATC: SPBTCTRL,VISIBLE param error,param=%d", type_oper);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            break;
        } //end VISIBLE

        case 2: //"NAME"
        {
            if (quiry_flag)
            {
                uint16 device_name[BT_DEVICE_NAME_SIZE] = {0};
                uint8 name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};

                SCI_TRACE_LOW("ATC: SPBTCTRL,get bt name");
                BT_GetLocalName(device_name);
                ConvertBtNametoString(device_name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));
                //+SPBTCTRL:NAME=<name>
                sprintf((char *)g_rsp_str, "%s%s%s%s", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", name_string);
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else if (param[1].param_len != 0)
            {
                uint8 name_type = ATC_BT_NAME_ASCII;
                uint16 local_name[BT_DEVICE_NAME_SIZE] = {0};

                if (param[2].param_len != 0)
                {
                    if (param[2].param[0] == '1')
                    {
                        name_type = ATC_BT_NAME_UCS2;
                    }
                }

                ConvertStringtoBtName(name_type, param[1].param, param[1].param_len, local_name, BT_DEVICE_NAME_SIZE);
                ret_val = BT_SetLocalName(local_name);
                if (BT_SUCCESS == ret_val)
                {
                    break; //OK
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call BT_SetLocalName ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            break;
        }

        case 3: //"ADDR"
        {
            if (quiry_flag)
            {
                BT_ADDRESS addr;
                uint8 addr_str[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
                BT_GetBdAddr(&addr);
                ConvertBtAddrtoString(addr.addr, addr_str);

                //+SPBTCTRL:ADDR=<address>
                sprintf((char *)g_rsp_str, "%s%s%s%s", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", addr_str);
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            break;
        } //END ADDR

        case 4: //"INQ"
        {
            if (quiry_flag)
            {
#if defined(CONFIG_BLUEU_BT_ENABLE)
                if (BT_GetSearchDevice_state() == BT_INQ_IDLE)
                {
                    uint32 cnt = BT_GetSearchDeviceCount();
                    for (uint32 i = 0; i < cnt; i++)
                    {
                        UINT8 addr[6] = {0};
                        UINT8 name[32] = {0};
                        memset(g_rsp_str, 0, 1024);

                        if (BT_GetSearchDeviceInfo(addr, name, i))
                        {
                            uint8 addr_str[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
                            sprintf((char *)addr_str, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", addr[0], ":", addr[1], ":", addr[2], ":", addr[3], ":", addr[4], ":", addr[5]);
                            sprintf((char *)g_rsp_str, "%s%s%s%s%s%s", "+BT Name:", "=", name, "+BT ADDR:", "=", addr_str);
                            atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                        }
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
#else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
            }
            else
            {
                uint32 service_type = BT_SERVICE_ALL;

                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoUint32(param[1].param, param[1].param_len, &service_type))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }

                ret_val = BT_SearchDevice(service_type);
                if (BT_PENDING == ret_val)
                {
                    //OK
                    //+INQS
                    sprintf((char *)g_rsp_str, "%s", "OK");
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                    sprintf((char *)g_rsp_str, "%s", "+INQS");
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                    break;
                }
                else
                {
                    //ERROR
                    SCI_TRACE_LOW("ATC: call BT_SearchDevice ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        }

        case 5: //"INQC"
        {
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                ret_val = BT_CancelSearch();

                if (BT_PENDING == ret_val)
                {
                    break;
                }
                else if (BT_SUCCESS == ret_val)
                {
                    break; //OK
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call BT_CancelSearch ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        } //END INQC

        case 6: //"PAIR"
        {
            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: call PAIR ERROR quiry_flag=%d.", quiry_flag);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                BT_ADDRESS addr = {0};

                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call PAIR ERROR param[1].param_len== 0.");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                ret_val = BT_PairDevice(&addr);

                if (BT_PENDING == ret_val)
                {
                    break; //OK
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call BT_PairDevice ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        }

        case 7: //"PAIRC"
        {
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                BT_ADDRESS addr = {0};

                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                ret_val = BT_CancelPair(&addr);

                if ((BT_PENDING == ret_val) || (BT_SUCCESS == ret_val))
                {
                    break; //OK
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call BT_CancelPair ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        } //END PAIRC

        case 8: //"PIN"
        {
            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: call PIN ERROR quiry_flag=%d.", quiry_flag);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                BT_ADDRESS addr = {0};
                //AT+SPBTCTRL=PIN,<addr>,<pin>
                //get addr
                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        SCI_TRACE_LOW("ATC: call PIN ConvertStringtoBtAddr ERROR.");
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call PIN param[1].param_len==0 ERROR");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                //get pin
                if (param[2].param_len != 0)
                {
                    ret_val = BT_InputPairPin(&addr, param[2].param, param[2].param_len);

                    if (BT_PENDING == ret_val)
                    {
                        break;
                    }
                    else
                    {
                        SCI_TRACE_LOW("ATC: call BT_InputPairPin ret_val = %d.", ret_val);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        } //END PIN

        case 9: //"LISTPD"
        {
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                int device_number = 0;
                uint8 index = 0;
                uint8 addr_string[ATC_MAX_BT_ADDRESS_STRING_LEN] = {0};
                uint8 name_string[ATC_MAX_BT_NAME_STRING_LEN] = {0};
                BT_DEVICE_INFO dev = {0};

                device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);

                for (index = 1; index <= device_number; index++)
                {
                    BT_GetPairedDeviceInfo(BT_SERVICE_ALL, index, &dev);
                    //+LISTPD:11:22:33:44:55:66,devName1,00000408
                    ConvertBtAddrtoString(dev.addr.addr, addr_string);
                    ConvertBtNametoString(dev.name, BT_DEVICE_NAME_SIZE, name_string, sizeof(name_string));
                    sprintf((char *)g_rsp_str, "%s%s%s%s%s%08x", "+LISTPD:", addr_string, ",", name_string, ",", (unsigned int)dev.dev_class);
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                //+LISTPDE
                sprintf((char *)g_rsp_str, "+LISTPDE");
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }

            break;
        }

        case 10: //"REMOVEPD"
        {
            //AT+SPBTCTRL=REMOVEPD,<cmd_type>,<address>
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    if (param[1].param[0] == '0') //clear one paired device record
                    {
                        if (param[2].param_len != 0) //address
                        {
                            BT_ADDRESS addr = {0};

                            ConvertStringtoBtAddr(param[2].param, param[2].param_len, addr.addr);
                            ret_val = BT_RemovePairedDevice(&addr);

                            if (BT_PENDING != ret_val)
                            {
                                SCI_TRACE_LOW("ATC: call BT_RemovePairedDevice ret_val = %d.", ret_val);
                                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                            }

                            break; //OK
                        }
                        else
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                        }
                    }
                    else if (param[1].param[0] == '1') //clear all the paired device records
                    {
                        int device_number = 0;
                        uint8 index = 0;
                        BT_DEVICE_INFO dev = {0};

                        device_number = BT_GetPairedDeviceCount(BT_SERVICE_ALL);

                        for (index = 1; index <= device_number; index++)
                        {
                            BT_GetPairedDeviceInfo(BT_SERVICE_ALL, index, &dev);
                            BT_RemovePairedDevice(&(dev.addr));
                        }

                        break; //OK
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        }

        case 11: //"CONNECT"
        {
            //AT+SPBTCTRL=CONNECT,<address>,<profile>
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    BT_ADDRESS addr = {0};
                    uint8 connect_flag = 0;

                    //get bt address
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }

                    //profile name
                    if (param[2].param_len != 0)
                    {
                        if (!strcmp((char *)param[2].param, "SPP"))
                        {
#ifdef BT_SPP_SUPPORT
                            if (param[3].param_len != 0)
                            {
                                connect_flag = param[3].param[0] - '0';

                                if (connect_flag)
                                {
                                    ret_val = BT_SppConnect(&addr, SPP_PORT_SPEED_115200);
                                }
                                else
                                {
                                    ret_val = BT_SppDisconnect();
                                }

                                if ((ret_val == BT_PENDING) || (ret_val == BT_SUCCESS))
                                {
                                    //OK
                                    //+CONNECTING>>11:22:33:44:55:66
                                    sprintf((char *)g_rsp_str, "%s", "OK");
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    sprintf((char *)g_rsp_str, "%s%s", "+CONNECTING>>", param[1].param);
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    if (ret_val == BT_SUCCESS)
                                    {
                                        if (connect_flag)
                                        {
                                            sprintf((char *)g_rsp_str, "+CONNNECTED");
                                        }
                                        else
                                        {
                                            sprintf((char *)g_rsp_str, "+DISCONNECTED");
                                        }
                                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                                    }
                                    break;
                                }
                                else
                                {
                                    SCI_TRACE_LOW("ATC: call BT_SppConnect ret_val = %d.", ret_val);
                                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                                }
                            }
#else
                            SCI_TRACE_LOW("ATC: call BT_SppConnect ERR_OPERATION_NOT_ALLOWED.");
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
                        }
                        else if (!strcmp((char *)param[2].param, "HID"))
                        {
#ifdef BT_HID_SUPPORT
                            if (param[3].param_len != 0)
                            {
                                connect_flag = param[3].param[0] - '0';

                                if (connect_flag)
                                {
                                    /**/
                                    if ((0x40 == addr.addr[0]) && (0x45 == addr.addr[1]) && (0xDA == addr.addr[2]))
                                    {
                                        SCI_TRACE_LOW("ATC: YWD change the addr");
                                        addr.addr[0] = addr.addr[5];
                                        addr.addr[1] = addr.addr[4];
                                        addr.addr[2] = addr.addr[3];
                                        addr.addr[3] = 0xDA;
                                        addr.addr[4] = 0x45;
                                        addr.addr[5] = 0x40;
                                    }
                                    /**/
                                    ret_val = HID_Connect(addr);
                                }
                                else
                                {
                                    ret_val = HID_Disconnect();
                                }

                                SCI_TRACE_LOW("BTYWD AT HID=0x%x", ret_val);

                                if ((ret_val == BT_PENDING) || (ret_val == BT_SUCCESS))
                                {
                                    //OK
                                    //+CONNECTING>>11:22:33:44:55:66
                                    sprintf((char *)g_rsp_str, "%s", "OK");
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    sprintf((char *)g_rsp_str, "%s%s", "+CONNECTING>>", param[1].param);
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    if (ret_val == BT_SUCCESS)
                                    {
                                        if (connect_flag)
                                        {
                                            sprintf((char *)g_rsp_str, "+CONNECTED");
                                        }
                                        else
                                        {
                                            sprintf((char *)g_rsp_str, "+DISCONNECTED");
                                        }
                                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                                    }
                                    break;
                                }
                                else
                                {
                                    SCI_TRACE_LOW("ATC: BT HID ret_val = %d.", ret_val);
                                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                                }
                            }
#else
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
                        }
                        else if (!strcmp((char *)param[2].param, "HFU"))
                        {
#ifdef BT_HFU_SUPPORT
                            if (param[3].param_len != 0)
                            {
                                connect_flag = param[3].param[0] - '0';

                                if (connect_flag)
                                {
                                    ret_val = BT_HfuConnect(&addr);
                                }
                                else
                                {
                                    ret_val = BT_HfuDisconnect();
                                }

                                if ((ret_val == BT_PENDING) || (ret_val == BT_SUCCESS))
                                {
                                    //OK
                                    //+CONNECTING>>11:22:33:44:55:66
                                    sprintf((char *)g_rsp_str, "%s", "OK");
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    sprintf((char *)g_rsp_str, "%s%s", "+CONNECTING>>", param[1].param);
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    if (ret_val == BT_SUCCESS)
                                    {
                                        if (connect_flag)
                                        {
                                            sprintf((char *)g_rsp_str, "+CONNNECTED");
                                        }
                                        else
                                        {
                                            sprintf((char *)g_rsp_str, "+DISCONNECTED");
                                        }
                                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                                    }
                                    break;
                                }
                                else
                                {
                                    SCI_TRACE_LOW("ATC: BT HFU ret_val = %d.", ret_val);
                                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                                }
                            }
#else
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
                        }
                        else if (!strcmp((char *)param[2].param, "A2DPSNK"))
                        {
#ifdef BT_AV_SNK_SUPPORT
                            if (param[3].param_len != 0)
                            {
                                connect_flag = param[3].param[0] - '0';

                                if (connect_flag)
                                {
                                    ret_val = A2DP_Sink_Connect(addr);
                                }
                                else
                                {
                                    ret_val = A2DP_Sink_Disconnect();
                                }

                                if ((ret_val == BT_PENDING) || (ret_val == BT_SUCCESS))
                                {
                                    //OK
                                    //+CONNECTING>>11:22:33:44:55:66
                                    sprintf((char *)g_rsp_str, "%s", "OK");
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    sprintf((char *)g_rsp_str, "%s%s", "+CONNECTING>>", param[1].param);
                                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);

                                    if (ret_val == BT_SUCCESS)
                                    {
                                        if (connect_flag)
                                        {
                                            sprintf((char *)g_rsp_str, "+CONNNECTED");
                                        }
                                        else
                                        {
                                            sprintf((char *)g_rsp_str, "+DISCONNECTED");
                                        }
                                        atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                                    }
                                    break;
                                }
                                else
                                {
                                    SCI_TRACE_LOW("ATC: BT A2DP SNK ret_val = %d.", ret_val);
                                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                                }
                            }
#else
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
                        }
                        else
                        {
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                        }
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        }
        case 12: //ADV
        {

#ifdef CONFIG_BLUEU_BT_ENABLE
            char *msg = NULL;
            UINT8 ble_adv_state = 2;

            SCI_TRACE_LOW("BLE ATC YWD %d %d", param[1].param_len, param[2].param_len);

            if (quiry_flag) //²éÑ¯adv ²ÎÊý
            {
                ble_adv_state = bt_handle_get_adv_enable_state();
                switch (ble_adv_state)
                {
                case 0:
                    msg = "+BTCOMM:CONNECTION=0\n";
                    break;
                case 1:
                    msg = "+BTCOMM:CONNECTION=1\n";
                    break;
                default:
                    msg = "+BTCOMM:ERR=error\n";
                    break;
                }
                atCmdRespInfoText(cmd->engine, (char *)msg);
                break;
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    if (1 == (param[1].param[0] - '0'))
                    {

                        bt_handle_adv_enable(TRUE);
                    }
                    else //Disable ADV
                    {
                        bt_handle_adv_enable(FALSE);
                    }
                }

                break;
            }
            break;

#else
#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)

            BOOLEAN adv_flag = FALSE;
            BOOLEAN adv_interval_flag = FALSE;
            BOOLEAN adv_type_flag = FALSE;
            BOOLEAN adv_own_addr_type_flag = FALSE;
            BOOLEAN adv_dir_addr_type_flag = FALSE;
            BOOLEAN adv_dir_addr_flag = FALSE;
            BOOLEAN adv_channel_flag = FALSE;
            BOOLEAN adv_policy_flag = FALSE;
            tBLE_ADV_TYPE adv_type = 0;
            uint16 interval = 0;
            tBLE_ADDR_TYPE local_addr_type = 0;
            tBLE_ADDR_TYPE peer_addr_type = 0;
            bdaddr_t peer_addr = {0};
            tBLE_ADV_CHNL_MAP channel_map = 0;
            uint8 filter_policy = 0;
            //BLE_STATUS status = BLE_SUCCESS;

            SCI_TRACE_LOW("BT ATC YWD %d %d", param[1].param_len, param[2].param_len);
            if (!BT_GetState())
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }

            if (quiry_flag)
            {
                adv_flag = ble_adv_is_enable();
                if (adv_flag)
                {
                    ble_get_adv_param(&adv_type, &interval, &local_addr_type, &peer_addr_type, &peer_addr);
                    channel_map = ble_get_gap_adv_channel_map(); //lint !e718  !e746
                    filter_policy = ble_get_adv_filter_policy(); //lint !e718  !e746

                    sprintf((char *)g_rsp_str, "%s%s%s%d%s%d%s%d%s%d%s%d%s%x%s%x%s%x%s%x%s%x%s%x%s%d%s%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=",
                            adv_flag, ",", interval, ",", adv_type, ",", local_addr_type, ",", peer_addr_type, ",",
                            peer_addr.address[0], ":", peer_addr.address[1], ":", peer_addr.address[2], ":", peer_addr.address[3], ":", peer_addr.address[4], ":", peer_addr.address[5], ",",
                            channel_map, ",", filter_policy);
                }
                else
                {
                    sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", adv_flag);
                }
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break;
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    if (1 == (param[1].param[0] - '0'))
                    {
                        adv_flag = TRUE;
                        if (param[2].param_len != 0)
                        {
                            adv_interval_flag = TRUE;
                        }
                        if (param[3].param_len != 0)
                        {
                            adv_type_flag = TRUE;
                        }
                        if (param[4].param_len != 0)
                        {
                            adv_own_addr_type_flag = TRUE;
                        }
                        if (param[5].param_len != 0)
                        {
                            adv_dir_addr_type_flag = TRUE;
                        }
                        if (param[6].param_len != 0)
                        {
                            adv_dir_addr_flag = TRUE;
                        }
                        if (param[7].param_len != 0)
                        {
                            adv_channel_flag = TRUE;
                        }
                        if (param[8].param_len != 0)
                        {
                            adv_policy_flag = TRUE;
                        }
                    }
                }

                if (adv_flag)
                {
                    if (adv_interval_flag && adv_type_flag && adv_own_addr_type_flag && adv_dir_addr_type_flag && adv_dir_addr_flag)
                    {
                        //all should be TRUE here
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }

                    /*lint -save -e527 -e774 */
                    if (adv_interval_flag)
                    {
                        interval = atoi((char *)param[2].param);
                    }

                    if (adv_type_flag)
                    {
                        adv_type = atoi((char *)param[3].param);
                    }

                    if (adv_own_addr_type_flag)
                    {
                        local_addr_type = atoi((char *)param[4].param);
                    }

                    if (adv_dir_addr_type_flag)
                    {
                        peer_addr_type = atoi((char *)param[5].param);
                    }

                    if (adv_dir_addr_flag)
                    {
                        uint8 tmp_addr[12] = {0};
                        int i = 0;
                        int j = 0;

                        for (i = 0; i < param[6].param_len; i++)
                        {
                            if (param[6].param[i] != ':')
                            {
                                if (param[6].param[i] >= 'A' && param[6].param[i] <= 'F')
                                {
                                    tmp_addr[j] = (param[6].param[i] - '0' - 7);
                                }
                                else
                                {
                                    tmp_addr[j] = (param[6].param[i] - '0');
                                }

                                //SCI_TRACE_LOW("tmp_addr=%x", tmp_addr[j]);
                                ++j;
                            }
                        }

                        for (j = 0; j < 6; j++)
                        {
                            //sprintf(tmp_addr_2[j], "%x", (tmp_addr[j*2]<<4 | tmp_addr[j*2+1]));
                            peer_addr.address[j] = (tmp_addr[j * 2]) << 4 | (tmp_addr[j * 2 + 1]);
                        }
                    }

                    status = ble_set_adv_param(adv_type, interval, local_addr_type, peer_addr_type, &peer_addr);
                    if (BLE_SUCCESS == status) //some parameters are invalid
                    {
                        if (adv_channel_flag)
                        {
                            channel_map = atoi((char *)param[7].param);
                            ble_set_gap_adv_channel_map(channel_map); //lint !e718  !e746
                        }

                        if (adv_policy_flag)
                        {
                            filter_policy = atoi((char *)param[8].param);
                            ble_set_adv_filter_policy(filter_policy); //lint !e718  !e746
                        }

                        ble_adv_enable(FALSE); //Disable adv before new parameters running

                        ble_adv_enable(TRUE); //Enable ADV with new parameters
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else //Disable ADV
                {
                    ble_adv_enable(FALSE);
                }

                break;
            }
            break;
#else
            SCI_TRACE_LOW("ATC: SPBTCTRL ADV ERR_OPERATION_NOT_ALLOWED.");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
#endif
        }

        case 13: //ADVDATA
        {
#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)

            BOOLEAN adv_data_flag = FALSE;
            uint8 data_buf[40] = {0};
            uint8 data_len = 0;

            if (!BT_GetState())
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }

            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    adv_data_flag = TRUE;
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                if (adv_data_flag) //lint !e774
                {
                    //copy data
                    data_len = param[1].param_len;
                    if (param[1].param_len >= 40)
                    {
                        data_len = 40;
                    }
                    SCI_MEMCPY(data_buf, param[1].param, data_len);
                    SCI_TRACE_LOW("ATC: ADV data_len=%d, data_buf %s", data_len, data_buf);
                    //ble_set_adv_manu_data(data_buf, data_len); //lint !e718  !e746
                    bt_handle_adv_data(data_len, data_buf);
                }

                break;
            }
            break;
#else
            SCI_TRACE_LOW("ATC: SPBTCTRL ADVDATA ERR_OPERATION_NOT_ALLOWED.");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }

        case 14: //CONPARA  AT+SPBTCTRL=CONPARA,uint16,uint16,uint16,uint16
        {
#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)

            BOOLEAN param1_flag = FALSE;
            uint16 param1 = 0;
            BOOLEAN param2_flag = FALSE;
            uint16 param2 = 0;
            BOOLEAN param3_flag = FALSE;
            uint16 param3 = 0;
            BOOLEAN param4_flag = FALSE;
            uint16 param4 = 0;
            conn_param_req_t current_para = {0};

            if (!BT_GetState())
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }

            if (quiry_flag)
            {
                ble_get_conn_param(&current_para); //lint !e718  !e746
                //+SPBTCTRL:CONPARA=<%d>,<%d>,<%d>,<%d>,
                sprintf((char *)g_rsp_str, "%s%s%s%d,%d,%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", current_para.conn_interval, current_para.conn_latency, current_para.timeout);
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    param1_flag = TRUE;
                }

                if (param[2].param_len != 0)
                {
                    param2_flag = TRUE;
                }

                if (param[3].param_len != 0)
                {
                    param3_flag = TRUE;
                }

                if (param[4].param_len != 0)
                {
                    param4_flag = TRUE;
                }

                if (param1_flag)
                {
                    param1 = (uint16)atoi((char *)param[1].param);
                    SCI_TRACE_LOW("ATC: CONPARA param1=%d", param1);
                }

                if (param2_flag)
                {
                    param2 = (uint16)atoi((char *)param[2].param);
                    SCI_TRACE_LOW("ATC: CONPARA param2=%d", param2);
                }

                if (param3_flag)
                {
                    param3 = (uint16)atoi((char *)param[3].param);
                    SCI_TRACE_LOW("ATC: CONPARA param3=%d", param3);
                }

                if (param4_flag)
                {
                    param4 = (uint16)atoi((char *)param[4].param);
                    SCI_TRACE_LOW("ATC: CONPARA param4=%d", param4);
                }

                status = S_ATC_FAIL;
                if (param1_flag && param2_flag && param3_flag && param4_flag)
                {
                    if (ble_l2cap_conn_param_update_req(param1, param2, param3, param4))
                    {
                        status = S_ATC_SUCCESS;
                    }
                }
                return status;
            }
            break;
#else
            SCI_TRACE_LOW("ATC: SPBTCTRL CONPARA ERR_OPERATION_NOT_ALLOWED.");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }

        case 15: //LINKNUM
        {
#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)

            uint8 link_num = 0;
            BOOLEAN ret = FALSE;

            if (quiry_flag)
            {
                //+SPBTCTRL:LINKNUM=<%d>
                //sprintf((char *)g_rsp_str, "%s%s%s%d","+SPBTCTRL:",s_btctrl_str[bttest_index],"=", link_num);
                //atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                //break;//OK
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    link_num = (uint16)atoi((char *)param[1].param);
                    ret = ble_set_link_num(link_num); //lint !e718  !e746
                }

                if (ret)
                {
                    break;
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
#else
            SCI_TRACE_LOW("ATC: SPBTCTRL LINKNUM ERR_OPERATION_NOT_ALLOWED.");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }

        case 16: //SLVAUTH
        {
#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)

            if (quiry_flag)
            {
                //atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                //break;//OK
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                ble_slave_send_authen_req(); //lint !e718  !e746
                break;
            }

            break;
#else
            SCI_TRACE_LOW("ATC: SPBTCTRL SLVAUTH ERR_OPERATION_NOT_ALLOWED.");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }

        case 17: //DEBUG
        {

            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    if (1 == (param[1].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[1]:%d", param[1].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                if (param[2].param_len != 0)
                {
                    if (1 == (param[2].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[2]:%d", param[2].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                if (param[3].param_len != 0)
                {
                    if (1 == (param[3].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[3]:%d", param[3].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                if (param[4].param_len != 0)
                {
                    if (1 == (param[4].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[4]:%d", param[4].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                if (param[5].param_len != 0)
                {
                    if (1 == (param[5].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[5]:%d", param[5].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                if (param[6].param_len != 0)
                {
                    if (1 == (param[6].param[0] - '0'))
                    {
                    }
                    sprintf((char *)g_rsp_str, "param[6]:%d", param[6].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }

                break;
            }
            break;
        }

        case 18: //TEST
        {
#ifdef CHIP_VER_EX_SL6210C
            const char *s_bt_simu_str[BT_SIMU_MAX] = {"NONE", "KPD", "SPEAKER"};
            BT_SIMU_MODE_E simu_mode = BT_SIMU_NONE;
            BOOLEAN flag = FALSE;
            if (quiry_flag)
            {
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTCTRL:", s_btctrl_str[bttest_index], "=", BT_ATC_GetTestMode(&simu_mode));
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    if (1 == (param[1].param[0] - '0'))
                    {
                        flag = TRUE;
                        if (param[2].param_len != 0)
                        {
                            for (bttest_index = 0; bttest_index < BT_SIMU_MAX; bttest_index++)
                            {
                                if (!strcmp((char *)param[2].param, s_bt_simu_str[bttest_index]))
                                {
                                    break;
                                }
                            }
                            if (bttest_index < BT_SIMU_MAX)
                            {
                                simu_mode = (BT_SIMU_MODE_E)bttest_index;
                            }
                        }
                        else
                        {
                            simu_mode = BT_SIMU_KPD; //KPD as default, to compitile
                        }
                    }

                    BT_ATC_SetTestMode(flag, simu_mode); //We should set the simu mode before the BT ON

                    sprintf((char *)g_rsp_str, "TEST:%d", param[1].param[0] - '0');
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }
                break;
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
            break;
        }

        case 19: //"HFU"
        {
#ifdef CHIP_VER_EX_SL6210C
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
#ifdef BT_HFU_SUPPORT
                ATC_SPBTCTRL_HFU_PROCESS(atc_config_ptr, &param[1]);
                break;
#else
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }
        break;

        case 20: //"A2DPSNK"
        {
#ifdef CHIP_VER_EX_SL6210C
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    type_oper = param[1].param[0] - '0';
                }

                switch (type_oper)
                {
                case 0:
                {
                }
                break;

                case 1:
                {
                }
                break;

                default:
                    break;
                }
                break;
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }
        break;

        case 21: //"AVRCP CT"
        {
#if defined(CHIP_VER_EX_SL6210C) && defined(BT_AV_CT_SUPPORT)
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    AvrcpCtPanelOperation Op_Avrcp = {0};
                    BOOLEAN isAutoRelease = TRUE;
                    uint32 avrcpct_ctrl_time = 0;
                    /* op  0x0041	volume_up --- VOLUP
                             *	   0x0042	volume_down --- VOLDOWN
                             *	   0x0043	mute --- MUTE
                             *	   0x0044	play --- PLAY
                             *	   0x0045	stop
                             *	   0x0046	pause
                             *	   0x0047	record 
                             *	   0x0048	rewind 
                             *	   0x0049	fast forward 
                             *	   0x004a	eject 
                             *	   0x004b	forward 
                             *	   0x004c	backward
                             */
                    if (('V' == param[1].param[0]) && ('O' == param[1].param[1]) && ('L' == param[1].param[2]) && ('U' == param[1].param[3]) && ('P' == param[1].param[4]))
                    {
                        type_oper = 0x41;
                    }
                    else if (('V' == param[1].param[0]) && ('O' == param[1].param[1]) && ('L' == param[1].param[2]) && ('D' == param[1].param[3]) && ('O' == param[1].param[4]) && ('W' == param[1].param[5]) && ('N' == param[1].param[6]))
                    {
                        type_oper = 0x42;
                    }
                    else if (('M' == param[1].param[0]) && ('U' == param[1].param[1]) && ('T' == param[1].param[2]) && ('E' == param[1].param[3]))
                    {
                        type_oper = 0x43;
                    }
                    else if (('P' == param[1].param[0]) && ('L' == param[1].param[1]) && ('A' == param[1].param[2]) && ('Y' == param[1].param[3]))
                    {
                        type_oper = 0x44;
                    }
                    else if (('S' == param[1].param[0]) && ('T' == param[1].param[1]) && ('O' == param[1].param[2]) && ('P' == param[1].param[3]))
                    {
                        type_oper = 0x45;
                    }
                    else if (('P' == param[1].param[0]) && ('A' == param[1].param[1]) && ('U' == param[1].param[2]) && ('S' == param[1].param[3]) && ('E' == param[1].param[4]))
                    {
                        type_oper = 0x46;
                    }
                    else if (('R' == param[1].param[0]) && ('E' == param[1].param[1]) && ('C' == param[1].param[2]) && ('O' == param[1].param[3]) && ('R' == param[1].param[4]) && ('D' == param[1].param[5]))
                    {
                        type_oper = 0x47;
                    }
                    else if (('R' == param[1].param[0]) && ('E' == param[1].param[1]) && ('W' == param[1].param[2]) && ('I' == param[1].param[3]) && ('N' == param[1].param[4]) && ('D' == param[1].param[5]))
                    {
                        type_oper = 0x48;
                    }
                    else if (('F' == param[1].param[0]) && ('A' == param[1].param[1]) && ('S' == param[1].param[2]) && ('T' == param[1].param[3]) && ('F' == param[1].param[4]) && ('W' == param[1].param[5]) && ('D' == param[1].param[6]))
                    {
                        type_oper = 0x49;
                    }
                    else if (('E' == param[1].param[0]) && ('J' == param[1].param[1]) && ('E' == param[1].param[2]) && ('C' == param[1].param[3]) && ('T' == param[1].param[4]))
                    {
                        type_oper = 0x4A;
                    }
                    else if (('F' == param[1].param[0]) && ('O' == param[1].param[1]) && ('R' == param[1].param[2]) && ('W' == param[1].param[3]) && ('A' == param[1].param[4]) && ('R' == param[1].param[5]) && ('D' == param[1].param[6]))
                    {
                        type_oper = 0x4B;
                    }
                    else if (('B' == param[1].param[0]) && ('A' == param[1].param[1]) && ('C' == param[1].param[2]) && ('K' == param[1].param[3]) && ('W' == param[1].param[4]) && ('A' == param[1].param[5]) && ('R' == param[1].param[6]) && ('D' == param[1].param[7]))
                    {
                        type_oper = 0x4C;
                    }

                    Op_Avrcp.Op = (uint16)type_oper;
                    if ((0x49 == type_oper) || (0x48 == type_oper))
                    {
                        Op_Avrcp.press = TRUE;
                        ConvertStringtoDecimal(param[2].param, strlen(param[2].param), &avrcpct_ctrl_time);
                        SCI_TRACE_LOW("yiheng.zhou_avrcpct_ctrl_time:%d", avrcpct_ctrl_time);
                        AVRCP_CT_FASTFWD_OR_REWIND_PanelKey(Op_Avrcp, avrcpct_ctrl_time); //push fastfwd/rewind
                    }
                    else
                    {
                        Op_Avrcp.press = TRUE;
                        isAutoRelease = TRUE;
                        AVRCP_CT_SetPanelKey(Op_Avrcp, isAutoRelease);
                    }
                }

                break;
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }
        break;

        case 22: //"HID"
        {
#if defined(CHIP_VER_EX_SL6210C) && defined(BT_HID_SUPPORT)
            uint32 key_val = 0;

            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                //SCI_TRACE_LOW("BTYWD HID Key %d . 0x%x 0x%x 0x%x ... %d",param[1].param_len,param[1].param[0],param[1].param[1],param[1].param[2],param[2].param_len);
                if (('K' == param[1].param[0]) && ('E' == param[1].param[1]) && ('Y' == param[1].param[2]))
                {
                    type_oper = 0;
                }

                switch (type_oper)
                {
                case 0: /*input Key*/
                {
                    if (param[2].param_len != 0)
                    {
                        //if ((param[3].param_len != 0))
                        {
                            if (ConvertStringtoDecimal(param[2].param, param[2].param_len, &key_val))
                            {
                                //SCI_TRACE_LOW("BTYWD HID Key 222222 key value=%d",key_val);
                                BT_HIDSendBTKey(KEY_PRESSED, key_val, TRUE);
                                SCI_SLEEP(10);
                                BT_HIDSendBTKey(KEY_RELEASED, key_val, TRUE);
                            }
                            else
                            {
                                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                            }
                        }
                    }
                }
                break;

                default:
                    break;
                }
                break;
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }
        break;

        case 23: //"SPP"
        {
#ifdef BT_SPP_SUPPORT

            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                if (param[1].param_len != 0)
                {
                    BT_SppWriteData(param[1].param, param[1].param_len);
                }
                else
                {
                    sprintf((char *)g_rsp_str, "Start AT UART echo recieved spp data");
                    atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                }
            }
#else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
#endif
        }
        break;

        case 24: //GATSREG   Register/Deregister a GATT Server
        {
            uint8 op = 0;
            uint8 gserv_id[32] = {0};
            uint8 gserv_id_len = 0;
            uint32 server_handle = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {

                //AT+SPBTCTRL=GATSREG?
                //+SPBTCTRL:GATSREG=<server_handle>,<server_uuid>,<result>
                //OK

                //BT function
                app_read_register_gatt_server_id();

                break;
            }
            else
            {
                //register server
                //AT+SPBTCTRL=GATSREG,1,<server_uuid>
                //+SPBTCTRL:GATSREG=1,<server_handle>,<server_uuid>,<result>
                //OK

                //degister Server
                //AT+SPBTCTRL=GATSREG,0,<server_handle>
                //+SPBTCTRL:GATSREG=0,<server_handle>,<server_uuid>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    op = param[1].param[0] - '0';

                    if ((op != 0) && (op != 1))
                    {
                        SCI_TRACE_LOW("ATC: AT+SPBTCTRL=GATSREG, op=%d param error", op);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }

                //BT function
                if (1 == op)
                {
                    if (param[2].param_len != 0)
                    {
                        if (param[2].param_len >= 32)
                        {
                            param[2].param_len = 32;
                        }
                        //string "ABCD" to 0xABCD
                        ConvertHexToBin(param[2].param, param[2].param_len, gserv_id);
                        gserv_id_len = param[2].param_len / 2;
                    }

                    result = app_register_gatt_server(gserv_id, gserv_id_len);
                }
                else
                {
                    if (param[2].param_len != 0)
                    {
                        server_handle = atoi((char *)param[2].param);
                    }

                    result = app_deregister_gatt_server(server_handle);
                }

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 25: //GATSS    Add/Remove a Service
        {
            uint8 op = 0;
            uint8 service_uuid[32] = {0};
            uint8 service_uuid_len = 0;
            uint16 handle_num = 0;
            uint16 service_handle = 0;
            uint8 is_primary = 0;
            uint32 server_handle = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {

                //AT+SPBTCTRL=GATSS?
                //+SPBTCTRL:GATSS=<server_handle>,<service_uuid>,<service_handle>,<service_handle_number>,<result>
                //OK

                //BT function
                app_read_user_service();
                break;
            }
            else
            {
                //Add a service
                //AT+SPBTCTRL=GATSS,1,<server_handle>,<service_uuid>,<service_handle_number>,<is_primary>
                //+SPBTCTRL:GATSS=1,<server_handle>,<service_uuid>,<service_handle>,<service_handle_number>,<result>
                //OK

                //remove a service
                //AT+SPBTCTRL=GATSS,0,<service_handle>
                //+SPBTCTRL:GATSS=0,<server_handle>,<service_uuid>,<service_handle>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    op = param[1].param[0] - '0';

                    if ((op != 0) && (op != 1))
                    {
                        SCI_TRACE_LOW("ATC: AT+SPBTCTRL=GATSS, op=%d paamer error", op);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                if (1 == op)
                {
                    if (param[2].param_len != 0)
                    {
                        server_handle = atoi((char *)param[2].param);
                    }

                    if (param[3].param_len != 0)
                    {
                        if (param[3].param_len >= 32)
                        {
                            param[3].param_len = 32;
                        }
                        ConvertHexToBin(param[3].param, param[3].param_len, service_uuid);
                        service_uuid_len = param[3].param_len / 2;
                    }

                    if (param[4].param_len != 0)
                    {
                        handle_num = atoi((char *)param[4].param);
                    }

                    if (param[5].param_len != 0)
                    {
                        is_primary = atoi((char *)param[5].param);
                    }

                    //BT function
                    result = app_add_user_service(server_handle, service_uuid, service_uuid_len, handle_num, is_primary);
                }
                else
                {

                    if (param[2].param_len != 0)
                    {
                        service_handle = atoi((char *)param[2].param);
                    }

                    //BT function
                    result = app_remove_user_service(service_handle);
                }

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 26: //GATSC   Add a Characteristic to an Existing Service
        {
            uint16 service_handle = 0;
            uint8 char_uuid[32] = {0};
            uint8 char_uuid_len = 0;
            uint32 property = 0;
            uint8 permission = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s  not support read command", s_btctrl_str[bttest_index]);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            else
            {

                //AT+SPBTCTRL=GATSC,<service_handle>,<char_uuid>,<property>,<permission>
                //+SPBTCTRL:GATSC=1,<server_handle>,<service_handle>,<char_uuid>,<char_handle>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    service_handle = atoi((char *)param[1].param);
                }

                if (param[2].param_len != 0)
                {
                    if (param[2].param_len >= 32)
                    {
                        param[2].param_len = 32;
                    }
                    ConvertHexToBin(param[2].param, param[2].param_len, char_uuid);
                    char_uuid_len = param[2].param_len / 2;
                }

                if (param[3].param_len != 0)
                {
                    property = atoi((char *)param[3].param);
                }

                if (param[4].param_len != 0)
                {
                    permission = atoi((char *)param[4].param);
                }

                //BT function
                result = app_add_user_character(service_handle, char_uuid, char_uuid_len, property, permission);

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 27: //GATSD  AT+SPBTCTRL=GATSD  Add a Descriptor to an Existing Service
        {
            uint16 service_handle = 0;
            uint8 desc_uuid[32] = {0};
            uint8 desc_uuid_len = 0;
            uint8 permission = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s  not support read command", s_btctrl_str[bttest_index]);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            else
            {

                //AT+SPBTCTRL=GATSD,<service_handle>,<desc_uuid>,<permission>
                //+SPBTCTRL:GATSC=1,<server_handle>,<service_handle>,<desc_uuid>,<desc_handle>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    service_handle = atoi((char *)param[1].param);
                }

                if (param[2].param_len != 0)
                {
                    if (param[2].param_len >= 32)
                    {
                        param[2].param_len = 32;
                    }
                    ConvertHexToBin(param[2].param, param[2].param_len, desc_uuid);
                    desc_uuid_len = param[2].param_len / 2;
                }

                if (param[3].param_len != 0)
                {
                    permission = atoi((char *)param[3].param);
                }

                //BT function
                result = app_add_user_discriptor(service_handle, desc_uuid, desc_uuid_len, permission);

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 28: //GATSST  Start/Stop a Service
        {
            uint8 op = 0;
            uint16 service_handle = 0;
            uint8 transport = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s  not support read command", s_btctrl_str[bttest_index]);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            else
            {
                //start a service
                //AT+SPBTCTRL=GATSST,1,<service_handle>,<transport>
                //+SPBTCTRL:GATSST=1,<server_handle>,<service_uuid>,<service_handle>,<result>
                //OK

                //stop a service
                //AT+SPBTCTRL=GATSST,0,<service_handle>
                //+SPBTCTRL:GATSST=0,<server_handle>,<service_uuid>,<service_handle>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    op = param[1].param[0] - '0';

                    if ((op != 0) && (op != 1))
                    {
                        SCI_TRACE_LOW("ATC: AT+SPBTCTRL=GATSS, op=%d paamer error", op);
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                if (param[2].param_len != 0)
                {
                    service_handle = atoi((char *)param[2].param);
                }

                if (param[3].param_len != 0)
                {
                    transport = atoi((char *)param[3].param);
                }

                //BT function
                if (1 == op)
                {
                    result = app_start_user_service(service_handle, transport);
                }
                else
                {
                    result = app_stop_user_service(service_handle);
                }

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 29: //GATSIND    Send an Indication or Notification to a Client
        {

            uint32 conn_id = 0;
            uint16 attr_handle = 0;
            uint8 need_confirm = 0;
            BOOLEAN result = FALSE;
            uint8 ind_value[32] = {0};
            uint8 ind_value_len = 0;

            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s  not support read command", s_btctrl_str[bttest_index]);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            else
            {

                //AT+SPBTCTRL=GATSIND,<conn_id>,<attr_handle>,<need_confirm>,<ind_value>
                //+SPBTCTRL:GATSIND=<attr_handle>,<conn_id>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    conn_id = atoi((char *)param[1].param);
                }

                if (param[2].param_len != 0)
                {
                    attr_handle = atoi((char *)param[2].param);
                }

                if (param[3].param_len != 0)
                {
                    need_confirm = atoi((char *)param[3].param);
                }

                if (param[4].param_len != 0)
                {
                    if (param[4].param_len >= 32)
                    {
                        param[4].param_len = 32;
                    }
                    ConvertHexToBin(param[4].param, param[4].param_len, ind_value);
                    ind_value_len = param[4].param_len / 2;
                }

                //BT function
                result = app_send_indi_or_noti(conn_id, attr_handle, need_confirm, ind_value, ind_value_len);

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 30: //GATSRSP    Send a Response to a Clientâ€™s Read or Write Operation
        {

            uint32 conn_id = 0;
            uint32 trans_id = 0;
            uint16 attr_handle = 0;
            uint8 rsp_value[32] = {0};
            uint8 rsp_value_len = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s  not support read command", s_btctrl_str[bttest_index]);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            }
            else
            {
                //AT+SPBTCTRL=GATSRSP,<conn_id>,<trans_id>,<attr_handle>,<rsp_value>
                //+SPBTCTRL:GATSRSP=<conn_id>,<att_handle>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    conn_id = atoi((char *)param[1].param);
                }

                if (param[2].param_len != 0)
                {
                    trans_id = atoi((char *)param[2].param);
                }

                if (param[3].param_len != 0)
                {
                    attr_handle = atoi((char *)param[3].param);
                }

                if (param[4].param_len != 0)
                {
                    if (param[4].param_len >= 32)
                    {
                        param[4].param_len = 32;
                    }
                    ConvertHexToBin(param[4].param, param[4].param_len, rsp_value);
                    rsp_value_len = param[4].param_len / 2;
                }

                //BT function
                result = app_send_response(conn_id, trans_id, attr_handle, rsp_value, rsp_value_len);

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 31: //GATSDISC    Disconnect Server Actively
        {
            uint32 conn_id = 0;
            //uint16 attr_handle = 0;
            BOOLEAN result = FALSE;

            if (quiry_flag)
            {
                //AT+SPBTCTRL=GATSDISC?
                //+SPBTCTRL:GATSDISC=<conn_id>,<remote_addr>,<result>
                //OK

                //BT function
                app_read_user_conn_id();
                break;
            }
            else
            {

                //AT+SPBTCTRL=GATSDISC,<conn_id>
                //+SPBTCTRL:GATSDISC=<conn_id>,<result>
                //OK

                if (param[1].param_len != 0)
                {
                    conn_id = atoi((char *)param[1].param);
                }

                //BT function
                result = app_disconnect_gatt_server(conn_id);

                if (TRUE == result)
                {
                    break;
                }
                else
                {
                    SCI_TRACE_LOW("ATC: AT+SPBTCTRL=%s, result=%d", s_btctrl_str[bttest_index], result);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            break;
        }

        case 32: //Hogp
        {
#ifdef BT_BLE_HOGP_SUPPORT
            ATC_SPBTCTRL_HOGP_PROCESS(atc_config_ptr, &param[1]);
            break;
#endif
        }
        case 33: //"GATTSETNOTIFY"
        {
            if (quiry_flag)
            {
                uint8 name_string[20] = {0};

                SCI_TRACE_LOW("ATC: SPBTCTRL,get bt gatt notify");
                ret_val = BT_GetGattNotify(name_string, 20);
                sprintf((char *)g_rsp_str, "%s%s%s%s", "+SPBTCTRL:", "notify", "=", name_string);
                atCmdRespInfoText(cmd->engine, (char *)g_rsp_str);
                break; //OK
            }
            else if (param[1].param_len != 0)
            {
                ret_val = BT_SetGattNotify(param[1].param, param[1].param_len);
                if (BT_SUCCESS == ret_val)
                {
                    break; //OK
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call BT_gattsetnotify ret_val = %d.", ret_val);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            break;
        }
        case 34:
            OSI_LOGI(0, "ATC: SPBTCTRL,enter app_le_scan_enable");
            //app_le_scan_enable();
            break;
        case 35:
            OSI_LOGI(0, "ATC: SPBTCTRL,enter app_le_scan_disable");
            mgr_le_scan_stop();
            break;
        case 36: //"LEPAIR"
        {
            if (quiry_flag)
            {
                SCI_TRACE_LOW("ATC: call PAIR ERROR quiry_flag=%d.", quiry_flag);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                BT_ADDRESS addr = {0};
                //uint8 addr_type = 0;

                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                    if (param[2].param_len != 0)
                    {
                        //addr_type = param[2].param[0] - '0';
                        //att_client_connect(*(bdaddr_t*)&addr.addr[0], 30,addr_type);
                        //GATT_Tester_Connect(*(bdaddr_t *)&addr.addr[0], addr_type);
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    SCI_TRACE_LOW("ATC: call PAIR ERROR param[1].param_len== 0.");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }
            }

            break;
        }

        case 37: //"LEPAIRC"
        {
            if (quiry_flag)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                BT_ADDRESS addr = {0};

                if (param[1].param_len != 0)
                {
                    if (!ConvertStringtoBtAddr(param[1].param, param[1].param_len, addr.addr))
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                    }
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
                }

                //ret_val = mgr_le_release_acl_fromaddr(*(bdaddr_t*)&addr.addr[0]);
                // gatt_tester_disconnect(*(bdaddr_t *)&addr.addr[0]);
            }

            break;
        } //END PAIRC

        default: //Error input para
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        } //end  switch (bttest_index)

        break;

    case AT_CMD_READ:
    case AT_CMD_TEST:
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }

    atCmdRespOK(cmd->engine);
    return;

#else
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);

#endif
}
