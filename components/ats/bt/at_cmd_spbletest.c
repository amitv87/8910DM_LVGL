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

#ifdef CONFIG_AT_SPBLETEST_SUPPORT

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
#include "bt_abs.h"

static char g_bletest_rsp_str[128] = {0};

#ifndef WIN32
static bool bletest_bt_rx_data_flag = false;
static uint32_t rx_data_show_pkt_cnt = 0;
static uint32_t rx_data_show_pkt_err_cnt = 0;
static uint32_t rx_data_show_bit_cnt = 0;
static uint32_t rx_data_show_bit_err_cnt = 0;
static uint8_t rx_data_show_rssi = 0;
static BT_STATUS rx_data_show_status = BT_SUCCESS;
#endif

static void BT_SPBLETEST_GetRXDataCallback(const BT_NONSIG_DATA *data)
{
#ifndef WIN32
    rx_data_show_rssi = data->rssi;
    rx_data_show_status = data->status;
    rx_data_show_pkt_cnt = data->pkt_cnt;
    rx_data_show_pkt_err_cnt = data->pkt_err_cnt;
    rx_data_show_bit_cnt = data->bit_cnt;
    rx_data_show_bit_err_cnt = data->bit_err_cnt;

    bletest_bt_rx_data_flag = TRUE;
#endif
}

void AT_SPBLE_CmdFunc_TEST(atCommand_t *cmd)
{
    uint32_t type_oper = 0;

    const char *s_bletest_str[14] =
        {
            "TESTMODE",
            "TESTADDRESS",
            "TXCH",
            "TXPATTERN",
            "TXPKTTYPE",
            "TXPKTLEN",
            "TXPWR",
            "TX",
            "RXCH",
            "RXPATTERN",
            "RXPKTTYPE",
            "RXGAIN",
            "RX",
            "RXDATA"};

    int bletest_index;
    bool quiry_flag = false;

#ifdef BT_NONSIG_SUPPORT
    BT_NONSIG_PARAM spbletest_bt_non_param = {0};
#endif

    static uint8_t spbletest_bt_address[6] = {0};
    static uint8_t spbletest_bt_tx_ch = 0;
    static uint8_t spbletest_bt_tx_pattern = 1;
    static uint8_t spbletest_bt_tx_pkttype = 1;
    static uint16_t spbletest_bt_tx_pktlen = 0;
    static uint8_t spbletest_bt_tx_pwr_type = 0; // 0---gain value; 1---power level
    static uint8_t spbletest_bt_tx_pwr_value = 0;
    static uint8_t spbletest_bt_tx_mode = 0;
    static uint8_t spbletest_bt_tx_status = 0;
    static uint8_t spbletest_bt_rx_ch = 0;
    static uint8_t spbletest_bt_rx_pattern = 0;
    static uint8_t spbletest_bt_rx_pktype = 1;
    static uint8_t spbletest_bt_rx_gain_mode = 0;
    static uint8_t spbletest_bt_rx_gain_value = 0;
    static uint8_t spbletest_bt_rx_status = 0;
    //static uint8_t spbletest_bt_create_flag = 0;
    bool spbletest_bt_status = false;

    bool paramok = true;
    memset(g_bletest_rsp_str, 0, sizeof(g_bletest_rsp_str));

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        char *bt_cmd_str = (char *)atParamRawText(cmd->params[0], &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if ('?' == bt_cmd_str[cmd->params[0]->length - 1])
        {
            quiry_flag = true;
            bt_cmd_str[cmd->params[0]->length - 1] = '\0';
        }
        else
        {
            bt_cmd_str[cmd->params[0]->length] = '\0';
        }

        for (bletest_index = 0; bletest_index < 14; bletest_index++)
        {
            if (!strcmp(bt_cmd_str, s_bletest_str[bletest_index]))
            {
                break;
            }
        }

        OSI_LOGI(0, "ATC: SPBLETEST, bttest_index=%d, quiry_flag=%d", bletest_index, quiry_flag);

        switch (bletest_index)
        {
        case 0: //"TESTMODE"
        {
            if (quiry_flag)
            {
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", BT_GetTestMode());
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }

            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            type_oper = atParamUintInRange(cmd->params[1], 0, 2, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

#if 0
                if (CALIBRATION_MODE == POWER_GetResetMode())
                {
                    if (0 == spbletest_bt_create_flag)
                    {
                        BT_Init(NULL);
                        spbletest_bt_create_flag = 1;
                        osiThreadSleep(400);
                    }
                }
#endif

            if (0 == type_oper) // quit EUT mode
            {
                OSI_LOGI(0, "SPBLETEST: quit TEST");

                UART_SetControllerBqbMode(FALSE);
                BT_Stop();

#ifdef BT_NONSIG_SUPPORT
                memset(&spbletest_bt_non_param, 0, sizeof(BT_NONSIG_PARAM));
#endif
                memset(spbletest_bt_address, 0, 6);
                spbletest_bt_tx_ch = 0;
                spbletest_bt_tx_pattern = 1;
                spbletest_bt_tx_pkttype = 1;
                spbletest_bt_tx_pktlen = 0;
                spbletest_bt_tx_pwr_type = 0;
                spbletest_bt_tx_pwr_value = 0;
                spbletest_bt_tx_mode = 0;
                spbletest_bt_tx_status = 0;
                spbletest_bt_rx_ch = 0;
                spbletest_bt_rx_pattern = 0;
                spbletest_bt_rx_pktype = 1;
                spbletest_bt_rx_gain_mode = 0;
                spbletest_bt_rx_gain_value = 0;
                spbletest_bt_rx_status = 0;

                BT_SetTestMode(BT_TESTMODE_NONE);
            }
            else if (1 == type_oper) // enter EUT mode
            {
                OSI_LOGI(0, "SPBLETEST: enter EUT");
                BT_SetTestMode(BT_TESTMODE_SIG);
                if (BT_PENDING == BT_Start())
                {
                    osiThreadSleep(1000); //wait BT init
                    bt_handle_adv_enable(FALSE);
                    osiThreadSleep(100);
                    UART_SetControllerBqbMode(TRUE);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL); //init not complete, after waiting a miniate,retry
                }
            }
            else if (2 == type_oper) // enter nonSignal mode
            {
                OSI_LOGI(0, "SPBLETEST: start Non Sig Test");
                BT_SetTestMode(BT_TESTMODE_NONSIG);
                if (BT_PENDING == BT_Start())
                {
                    osiThreadSleep(1000); //wait BT init
                    bt_handle_adv_enable(FALSE);
                    osiThreadSleep(100);
                }
                else
                {
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL); //init not complete, after waiting a miniate,retry
                }
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
            atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        break;

        case 1: //"TESTADDRESS"
        {
            if (quiry_flag)
            {
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#if 0            
                //wuding: the code coming from 7701, it looks strange
                uint8_t addr[12] = {0};
                uint8_t i = 0;
                OSI_LOGI(0, "SPBTTEST: get TESTADDRESS");     
                //here call the low layer API to get TESTADDRESS
                //BT_TEST_Get_BdAddr(addr);
                for(i=0; i<6;i++)
                    OSI_LOGI(0, "get bt addr:%x", addr[i]);
                sprintf((char *)g_rsp_str, "%s%s%s%s","+SPBTTEST:",s_bttest_str[bttest_index],"=", "11:22:33:44:55:66");
#endif
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%x%x%x%x%x%x", "+SPBTTEST:", s_bletest_str[bletest_index], "=", spbletest_bt_address[0], spbletest_bt_address[1], spbletest_bt_address[2], spbletest_bt_address[3], spbletest_bt_address[4], spbletest_bt_address[5]);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }

            if (cmd->param_count != 2)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            char *bt_addr_str = (char *)atParamStr(cmd->params[1], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            uint8_t tmp_addr[12] = {0};
            int i = 0;
            int j = 0;

            //here call the low layer API to set TESTADDRESS
            OSI_LOGI(0, "testing bt addr: %s", bt_addr_str);
            for (i = 0; i < strlen(bt_addr_str); i++)
            {
                if (bt_addr_str[i] != ':')
                {
                    if (bt_addr_str[i] >= 'A' && bt_addr_str[i] <= 'F')
                    {
                        tmp_addr[j] = (bt_addr_str[i] - '0' - 7);
                    }
                    else if (bt_addr_str[i] >= 'a' && bt_addr_str[i] <= 'f')
                    {
                        tmp_addr[j] = (bt_addr_str[i] - '0' - 39);
                    }
                    else if (bt_addr_str[i] >= '0' && bt_addr_str[i] <= '9')
                    {
                        tmp_addr[j] = (bt_addr_str[i] - '0');
                    }
                    else
                    {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    OSI_LOGI(0, "tmp_addr=%x", tmp_addr[j]);
                    ++j;
                }
            }

            for (j = 0; j < 6; j++)
            {
                spbletest_bt_address[j] = (tmp_addr[j * 2]) << 4 | (tmp_addr[j * 2 + 1]);
                OSI_LOGI(0, "set bt addr: %x", spbletest_bt_address[j]);
            }

            //BT_TEST_Set_BdAddr(tmp_addr_2);
            sprintf((char *)g_bletest_rsp_str, "%s", "+SPBTTEST:OK");
            atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(cmd->engine));
        }
        break;

        case 2: //"TXCH"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TXCH");
                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TXCH
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_ch);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t channel = atParamUintInRange(cmd->params[1], 0, 39, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                spbletest_bt_tx_ch = (uint8_t)channel;

                //here call the low layer API to set TXCH, type_oper is the input x
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 3: //"TXPATTERN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TXPATTERN");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TXPATTERN
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_pattern);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t pattern = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to set TXPATTERN, type_oper is the input x, range [0~7]
                //wuding: according to the NPI BT AT cmd doc
                //PN9        ---0
                //11110000   ---1
                //1010       ---2
                //PN15       ---3
                //1111       ---4
                //0000       ---5
                //00001111   ---6
                //0101       ---7

                spbletest_bt_tx_pattern = (uint8_t)pattern;
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 4: //"TXPKTTYPE"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TXPKTTYPE");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TXPKTTYPE
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_pkttype);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t TxPktType = atParamUintInRange(cmd->params[1], 0, 3, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //wuding: according to the NPI BT AT cmd doc
                //0x00---RF_PHY_Test_Ref
                //0x01---ADV_NONCONN_IND
                //0x02---ADV_IND
                //0x03---DATA,

                spbletest_bt_tx_pkttype = (uint8_t)TxPktType;

                //here call the low layer API to set TXPKTTYPE, type_oper is the input x, range [0~3]
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 5: //"TXPKTLEN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TXPKTLEN");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TXPKTLEN
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_pktlen);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t TxPktLen = atParamUintInRange(cmd->params[1], 0, 37, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to set TXPKTLEN, type_oper is the input x
                spbletest_bt_tx_pktlen = (uint16_t)TxPktLen;
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 6: //"TXPWR"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TXPWR X1,X2");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TXPWR
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d,%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_pwr_type, spbletest_bt_tx_pwr_value);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 3)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t TxPwrType = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t TxPwrValue = atParamUint(cmd->params[2], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //wuding: NPI AT cmd AT+SPBLETEST=TXPWR,x1,x2
                //x1: 0---gain value;  1---power level
                //x2: value
                spbletest_bt_tx_pwr_type = TxPwrType;
                spbletest_bt_tx_pwr_value = TxPwrValue;

                //here call the low layer API to set TXPWR, x1 & x2 are the input
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 7: //"TX"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get TX status");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get TX status
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_tx_status);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count < 2 || cmd->param_count > 4)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t TxStatus = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                if (TxStatus == 0) //AT+SPBLETEST=TX, 0
                {
#if 0 //feng.liu adapt sima test tool
                    if (cmd->param_count != 2)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#endif

                    spbletest_bt_tx_status = 0;

                    //here call the low layer API to stop TX
                    OSI_LOGI(0, "SPBLETEST: stop TX");
#ifdef BT_NONSIG_SUPPORT
                    spbletest_bt_status = BT_BLE_SetNonSigTxTestMode(false, &spbletest_bt_non_param);
#endif
                }
                else
                {
                    spbletest_bt_tx_status = 1;

                    if (cmd->param_count < 3 || cmd->param_count > 4)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    uint8_t TxMode = atParamUintInRange(cmd->params[2], 0, 1, &paramok);
                    if (!paramok)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    spbletest_bt_tx_mode = TxMode;
#ifdef BT_NONSIG_SUPPORT
                    spbletest_bt_non_param.channel = spbletest_bt_tx_ch;
                    spbletest_bt_non_param.pattern = spbletest_bt_tx_pattern;
                    spbletest_bt_non_param.pac.pac_type = spbletest_bt_tx_pkttype;
                    spbletest_bt_non_param.pac.pac_len = spbletest_bt_tx_pktlen;
                    spbletest_bt_non_param.item.power.power_type = spbletest_bt_tx_pwr_type;
                    spbletest_bt_non_param.item.power.power_value = spbletest_bt_tx_pwr_value;
#endif
                    if (spbletest_bt_tx_mode == 0) //AT+SPBLETEST=TX, 1, 0
                    {
                        //here call the low layer API to Sart TX for continues
                        OSI_LOGI(0, "SPBLETEST: Sart TX for continues");

                        if (cmd->param_count != 3)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#ifdef BT_NONSIG_SUPPORT
                        spbletest_bt_non_param.pac.pac_cnt = 0;
#endif
                    }
                    else //AT+SPBLETEST=TX, 1, 1, 3000
                    {
                        if (cmd->param_count != 4)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                        uint8_t TxPktCnt = atParamUint(cmd->params[3], &paramok);
                        if (!paramok)
                            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                        //here call the low layer API to Sart TX for single, pktcnt is input for pktcnt
                        OSI_LOGI(0, "SPBLETEST: Sart TX for single, TxPktCnt = %d", TxPktCnt);
#ifdef BT_NONSIG_SUPPORT
                        spbletest_bt_non_param.pac.pac_cnt = TxPktCnt;
#endif
                    }

#ifdef BT_NONSIG_SUPPORT
                    spbletest_bt_status = BT_BLE_SetNonSigTxTestMode(true, &spbletest_bt_non_param);
#endif
                }

                if (true == spbletest_bt_status)
                {
                    sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                }
                else
                {
                    sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:ERR");
                }

                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 8: //"RXCH"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RXCH");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get RXCH
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_rx_ch);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t channel = atParamUintInRange(cmd->params[1], 0, 39, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                spbletest_bt_rx_ch = (uint8_t)channel;

                //here call the low layer API to set TXCH, type_oper is the input x
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 9: //"RXPATTERN"
        {
            //wuding: there is no RXPATTERN in BT No-Signal!
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RXPATTERN");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get RXPATTERN
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_rx_pattern);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t RxPattern = atParamUintInRange(cmd->params[1], 0, 7, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //wuding: NPI AT cmd
                //PN9        ---0
                //11110000   ---1
                //1010       ---2
                //PN15       ---3
                //1111       ---4
                //0000       ---5
                //00001111   ---6
                //0101       ---7

                spbletest_bt_rx_pattern = (uint8_t)RxPattern;

                //here call the low layer API to set RXPATTERN, type_oper is the input x, range [0~7]
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 10: //"RXPKTTYPE"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RXPKTTYPE");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get RXPKTTYPE
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_rx_pktype);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t RxPktType = atParamUintInRange(cmd->params[1], 0, 3, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //rang: 0~3
                //0x00: RF_PHY_Test_Ref
                //0x01: ADV_NONCONN_IND
                //0x02: ADV_IND  0x03: DATA

                spbletest_bt_rx_pktype = (uint8_t)RxPktType;

                //here call the low layer API to set RXPKTTYPE, type_oper is the input x, range [0~5]
                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 11: //"RXGAIN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RXGAIN");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get RXGAIN Mode and x
                if (0 == spbletest_bt_rx_gain_mode)
                {
                    sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", 0);
                }
                else
                {
                    sprintf((char *)g_bletest_rsp_str, "%s%s%s%d,%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", 1, spbletest_bt_rx_gain_value);
                }
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                if (cmd->param_count < 2 || cmd->param_count > 3)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t RxGainMode = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                spbletest_bt_rx_gain_mode = RxGainMode; //0 Auto, 1 Fix

                if (0 == RxGainMode)
                {
                    OSI_LOGI(0, "SPBLETEST: Set RX Gain Auto mode");

                    if (cmd->param_count != 2)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    //here call the low layer API to Set RX Gain Auto mode
                }
                else if (1 == RxGainMode)
                {
                    if (cmd->param_count != 3)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    uint8_t RxGainValue = atParamUint(cmd->params[2], &paramok);
                    if (!paramok)
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                    //here call the low layer API to Set RX Gain Fix mode, x is input for x
                    //AT+SPBLETEST=RXGAIN,mode,[x]
                    //x: gain parameter , only available when mode is 1
                    if ((0 == RxGainValue) || (1 == RxGainValue) || (2 == RxGainValue) || (3 == RxGainValue) || (4 == RxGainValue) || (5 == RxGainValue)) //double check before send it to controller
                    {
                        spbletest_bt_rx_gain_value = RxGainValue;
                    }
                    else
                    {
                        spbletest_bt_rx_gain_value = 0;
                    }
                }

                sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 12: //"RX"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RX");

                if (cmd->param_count != 1)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                //here call the low layer API to get RX
                sprintf((char *)g_bletest_rsp_str, "%s%s%s%d", "+SPBLETEST:", s_bletest_str[bletest_index], "=", spbletest_bt_rx_status);
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                bool enable = false;

                if (cmd->param_count != 2)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                uint8_t Rx = atParamUintInRange(cmd->params[1], 0, 1, &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                if (Rx == 1)
                {
                    spbletest_bt_rx_status = 1;
                    enable = true;
#ifdef BT_NONSIG_SUPPORT
                    spbletest_bt_non_param.pattern = spbletest_bt_rx_pattern;
                    spbletest_bt_non_param.channel = spbletest_bt_rx_ch;
                    if (1 == spbletest_bt_rx_gain_mode) //Fix Mode
                    {
                        spbletest_bt_non_param.item.rx_gain = spbletest_bt_rx_gain_value;
                    }
                    else //auto mode
                    {
                        spbletest_bt_non_param.item.rx_gain = 0;
                    }
                    spbletest_bt_non_param.pac.pac_type = spbletest_bt_rx_pktype;
#endif
                }
                else
                {
                    spbletest_bt_rx_status = 0;
                    enable = false;
                }

                OSI_LOGI(0, "SPBLETEST: get RX, enable = %d", enable);

#ifdef BT_NONSIG_SUPPORT
                spbletest_bt_status = BT_BLE_SetNonSigRxTestMode(enable, &spbletest_bt_non_param, (BT_ADDRESS *)spbletest_bt_address);
#endif
                //here call the low layer API to set RX, type_oper is the input x, range [0~1]
                if (spbletest_bt_status)
                {
                    sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:OK");
                }
                else
                {
                    sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:ERR");
                }
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
        }
        break;

        case 13: //"RXDATA"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBLETEST: get RXDATA");

#ifdef BT_NONSIG_SUPPORT
                uint32_t res[5] = {0};
                uint8_t loop_times = 0;

                //here call the low layer API to get RXDATA, such as error_bits, total_bits, error_packets, total_packets, rssi
                bletest_bt_rx_data_flag = false;
                spbletest_bt_status = BT_BLE_GetNonSigRxData((BT_NONSIGCALLBACK)BT_SPBLETEST_GetRXDataCallback);

                if (spbletest_bt_status)
                {
                    while (!bletest_bt_rx_data_flag)
                    {
                        loop_times++;
                        if (loop_times < 12) //wait 600ms
                        {
                            osiThreadSleep(50);
                        }
                        else
                        {
                            break; //quit the loop
                        }
                    }

                    if (bletest_bt_rx_data_flag && (BT_SUCCESS == rx_data_show_status))
                    {
                        res[0] = rx_data_show_bit_err_cnt;
                        res[1] = rx_data_show_bit_cnt;
                        res[2] = rx_data_show_pkt_err_cnt;
                        res[3] = rx_data_show_pkt_cnt;
                        res[4] = rx_data_show_rssi;
                        OSI_LOGI(0, "rxdata, %d, %d, %d, %d", res[0], res[1], res[2], res[3], res[4]);
                    }
                    else
                    {
                        OSI_LOGI(0, "rxdata, null. %d %d", bletest_bt_rx_data_flag, rx_data_show_status);
                    }

                    sprintf((char *)g_bletest_rsp_str, "%s%s%s%u,%u,%u,%u,0x%x", "+SPBLETEST:", s_bletest_str[bletest_index], "=",
                            (unsigned int)res[0] /*error_bits  990*/, (unsigned int)res[1] /*total_bits 1600000*/, (unsigned int)res[2] /*error_packets 100*/, (unsigned int)res[3] /*total_packets 1024*/, (unsigned int)res[4] /*rssi*/);
                }
                else
#endif
                {
                    sprintf((char *)g_bletest_rsp_str, "%s", "+SPBLETEST:ERR");
                }
                atCmdRespInfoText(cmd->engine, g_bletest_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(cmd->engine));
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        break;

        default:
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }
    }
    break;

    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
        break;
    }
}

#endif
