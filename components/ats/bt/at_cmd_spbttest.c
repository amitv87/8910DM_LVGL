#include "ats_config.h"
#ifdef CONFIG_AT_SPBTTEST_SUPPORT
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
#include "bt_abs.h"

//----------------------------------------------------------
//#define SPBTTEST
#define SPBTTEST_MAX_CMD_NUM (16)

#ifndef WIN32
static uint32 rx_data_show_pkt_cnt = 0;
static uint32 rx_data_show_pkt_err_cnt = 0;
static uint32 rx_data_show_bit_cnt = 0;
static uint32 rx_data_show_bit_err_cnt = 0;
static uint8 rx_data_show_rssi = 0;
static BT_STATUS rx_data_show_status = BT_SUCCESS;
BOOLEAN spbttest_bt_rx_data_flag = FALSE;
#endif

char g_rsp_str[512] = {
    0,
};
static void BT_SPBTTEST_GetRXDataCallback(const BT_NONSIG_DATA *data)
{
#ifndef WIN32
    rx_data_show_rssi = data->rssi;
    rx_data_show_status = data->status;
    rx_data_show_pkt_cnt = data->pkt_cnt;
    rx_data_show_pkt_err_cnt = data->pkt_err_cnt;
    rx_data_show_bit_cnt = data->bit_cnt;
    rx_data_show_bit_err_cnt = data->bit_err_cnt;

    spbttest_bt_rx_data_flag = TRUE;
#endif
}

//AT_CMD_FUNC(ATC_ProcessSPBTTEST)
void AT_SPBT_CmdFunc_TEST(atCommand_t *pParam)
{
#ifndef WIN32

#if defined(BLE_INCLUDED) || defined(BT_BLE_SUPPORT)
    OSI_LOGI(0, "SPBTTEST: Not support.");
    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
#else
    unsigned int type_oper = 0;
    const char *s_bttest_str[SPBTTEST_MAX_CMD_NUM] =
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
            "RXDATA",
            "BTPTS",
            "MAC"};
    int bttest_index;
    bool quiry_flag = false;
#ifdef BT_NONSIG_SUPPORT
    BT_NONSIG_PARAM spbttest_bt_non_param = {0};
#endif
    static unsigned char spbttest_bt_address[6] = {0};
    static unsigned char spbttest_bt_tx_ch = 0;
    static unsigned char spbttest_bt_tx_pattern = 1;
    static unsigned char spbttest_bt_tx_pkttype = 1;
    static unsigned short spbttest_bt_tx_pktlen = 0;
    static unsigned char spbttest_bt_tx_pwr_type = 0; // 0---gain value; 1---power level
    static unsigned char spbttest_bt_tx_pwr_value = 0;
    static unsigned char spbttest_bt_tx_mode = 0;
    static unsigned char spbttest_bt_tx_status = 0;
    static unsigned char spbttest_bt_rx_ch = 0;
    static unsigned char spbttest_bt_rx_pattern = 0;
    static unsigned char spbttest_bt_rx_pktype = 1;
    static unsigned char spbttest_bt_rx_gain_mode = 0;
    static unsigned char spbttest_bt_rx_gain_value = 0;
    static unsigned char spbttest_bt_rx_status = 0;
    //    static unsigned char spbttest_bt_create_flag = 0;
    bool spbttest_bt_status = false;

    switch (pParam->type)
    {
    case AT_CMD_SET:
        if ('?' == pParam->params[0]->value[pParam->params[0]->length - 1])
        {
            quiry_flag = true;
            pParam->params[0]->value[pParam->params[0]->length - 1] = '\0';
        }
        else
        {
            pParam->params[0]->value[pParam->params[0]->length] = '\0';
        }

        for (bttest_index = 0; bttest_index < SPBTTEST_MAX_CMD_NUM; bttest_index++)
        {
            if (!strcmp((char *)pParam->params[0]->value, s_bttest_str[bttest_index]))
            {
                break;
            }
        }

        OSI_LOGI(0, "SPBTTEST: param index is %d, quiry flag is %d", bttest_index, quiry_flag);

        switch (bttest_index)
        {
        case 0: //"TESTMODE"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get testmode");
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", BT_GetTestMode()); //lint !e718  !e746
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                /* feng.liu
                if (osiGetBootMode() == OSI_BOOTMODE_CALIB)
                {
                    if (0 == spbttest_bt_create_flag)
                    {
                        BT_Init(NULL); //create the BT thread
                        spbttest_bt_create_flag = 1;
                        osiThreadSleep(400);
                    }
                }
                */
                type_oper = (unsigned int)(pParam->params[1]->value[0] - '0');

                if (0 == type_oper) // quit EUT mode
                {
                    OSI_LOGI(0, "SPBTTEST: quit EUT");
                    if (BT_TESTMODE_SIG == BT_GetTestMode())
                    {
                        BT_LeaveEutMode();
                        //MNPHONE_SetDisablePowerSweep(false);
                    }
                    else
                    {
                        BT_Stop();
                        osiThreadSleep(300); //Wait BT Task running to stop the BT
                    }
#ifdef BT_NONSIG_SUPPORT
                    memset(&spbttest_bt_non_param, 0, sizeof(BT_NONSIG_PARAM));
#endif
                    memset(spbttest_bt_address, 0, 6);
                    spbttest_bt_tx_ch = 0;
                    spbttest_bt_tx_pattern = 1;
                    spbttest_bt_tx_pkttype = 1;
                    spbttest_bt_tx_pktlen = 0;
                    spbttest_bt_tx_pwr_type = 0;
                    spbttest_bt_tx_pwr_value = 0;
                    spbttest_bt_tx_mode = 0;
                    spbttest_bt_tx_status = 0;
                    spbttest_bt_rx_ch = 0;
                    spbttest_bt_rx_pattern = 0;
                    spbttest_bt_rx_pktype = 1;
                    spbttest_bt_rx_gain_mode = 0;
                    spbttest_bt_rx_gain_value = 0;
                    spbttest_bt_rx_status = 0;
                    BT_SetTestMode(BT_TESTMODE_NONE); //lint !e718  !e746
                }
                else if (1 == type_oper) // enter EUT mode
                {
                    OSI_LOGI(0, "SPBTTEST: enter EUT");
                    //MNPHONE_SetDisablePowerSweep(true);
                    if (BT_ERROR == BT_EnterEutMode())
                    {
                        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL); //init not complete, after waiting a miniate,retry
                    }
                }
                else if (2 == type_oper) // enter nonSignal mode
                {
                    OSI_LOGI(0, "SPBTTEST: start Non Sig Test");

                    BT_SetTestMode(BT_TESTMODE_NONSIG);

                    if (BT_PENDING == BT_Start())
                    {
                        osiThreadSleep(700); //wait BT init
                    }
                    else
                    {
                        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_FAIL); //init not complete, after waiting a miniate,retry
                    }
                }
                else
                {
                    OSI_LOGI(0, "SPBTTEST: Param[1] error.");
                    RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                }
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }

            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 1: //"TESTADDRESS"
        {
            if (quiry_flag)
            {
                sprintf((char *)g_rsp_str, "%s%s%s%x%x%x%x%x%x", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_address[0], spbttest_bt_address[1], spbttest_bt_address[2], spbttest_bt_address[3], spbttest_bt_address[4], spbttest_bt_address[5]);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                unsigned char tmp_addr[12] = {0};
                int i = 0;
                int j = 0;
                //here call the low layer API to set TESTADDRESS
                char outstring[64] = {
                    0,
                };
                outstring[0] = '\0';
                sprintf((char *)outstring, "testing bt addr: %s, len=%d", ((char *)pParam->params[1]->value), pParam->params[1]->length);
                OSI_LOGI(0, outstring);
                for (i = 1; i < pParam->params[1]->length - 1; i++)
                {
                    if (pParam->params[1]->value[i] != ':')
                    {
                        if (pParam->params[1]->value[i] >= 'A' && pParam->params[1]->value[i] <= 'F')
                        {
                            tmp_addr[j] = (pParam->params[1]->value[i] - '0' - 7);
                        }
                        else
                        {
                            tmp_addr[j] = (pParam->params[1]->value[i] - '0');
                        }
                        OSI_LOGI(0, "tmp_addr=%x", tmp_addr[j]);
                        ++j;
                    }
                }

                for (j = 0; j < 6; j++)
                {
                    spbttest_bt_address[j] = (tmp_addr[j * 2]) << 4 | (tmp_addr[j * 2 + 1]);
                    OSI_LOGI(0, "set bt addr: %x", spbttest_bt_address[j]);
                }

                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }

            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 2: //"TXCH"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TXCH");
                //here call the low layer API to get TXCH
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_ch);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                if (pParam->params[1]->length == 1) // valid type_oper range [0~93]
                {
                    type_oper = pParam->params[1]->value[0] - '0';
                }
                else if (pParam->params[1]->length == 2)
                {
                    type_oper = pParam->params[1]->value[0] - '0';
                    type_oper = type_oper * 10 + (pParam->params[1]->value[1] - '0');
                }
                spbttest_bt_tx_ch = (unsigned char)type_oper;
                //here call the low layer API to set TXCH, type_oper is the input x
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 3: //"TXPATTERN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TXPATTERN");
                //here call the low layer API to get TXPATTERN
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_pattern);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                type_oper = pParam->params[1]->value[0] - '0';

                //here call the low layer API to set TXPATTERN, type_oper is the input x, range [0~3]
                //wuding: according to the NPI BT AT cmd doc
                //00000000---1
                //11111111---2
                //10101010---3
                //PRBS9     ---4
                //11110000---9
                spbttest_bt_tx_pattern = (unsigned char)type_oper;
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 4: //"TXPKTTYPE"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TXPKTTYPE");
                //here call the low layer API to get TXPKTTYPE
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_pkttype);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                //type_oper = pParam->params[1]->value[0] - '0';
                unsigned char i = 0;
                for (type_oper = 0, i = 0; i < pParam->params[1]->length; i++)
                {
                    type_oper = type_oper * 10 + (pParam->params[1]->value[i] - '0');
                }

                //wuding: according to the NPI BT AT cmd doc
                //0x00---NULL,             0x01---POLL,          0x02---FHS,            0x03---DM1,            0x04---DH1,
                //0x05---HV1,              0x06---HV2,           0x07---HV3,            0x08---DV,               0x09---AUX1,
                //0x0A---DM3,             0x0B---DH3,           0x0C---EV4,            0x0D---EV5,             0x0E---DM5,
                //0x0F---DH5,              0x10---ID,             0x11---INVALID,      0x12 ---,                  0x13 ---,
                //0x14---EDR_2DH1,   0x15---EDR_EV3,    0x16---EDR_2EV3,   0x17---EDR_3EV3,    0x18---EDR_3DH1,
                //0x19---EDR_AUX1,   0x1A---EDR_2DH3,  0x1B---EDR_3DH3,  0x1C---EDR_2EV5,   0x1D---EDR_3EV5,
                //0x1E---EDR_2DH5,   0x1F---EDR_3DH5
                spbttest_bt_tx_pkttype = (unsigned char)type_oper;

                //here call the low layer API to set TXPKTTYPE, type_oper is the input x, range [0~5]
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 5: //"TXPKTLEN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TXPKTLEN");
                //here call the low layer API to get TXPKTLEN
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_pktlen);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                int i = 0;
                for (type_oper = 0, i = 0; i < pParam->params[1]->length; i++)
                {
                    type_oper = type_oper * 10 + (pParam->params[1]->value[i] - '0');
                }
                //here call the low layer API to set TXPKTLEN, type_oper is the input x
                spbttest_bt_tx_pktlen = (unsigned short)type_oper;
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 6: //"TXPWR"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TXPWR X1,X2");
                //here call the low layer API to get TXPWR
                //wuding: the code coming from 7701, it looks strange
                //sprintf((char *)g_rsp_str, "%s%s%s%d%s%d","+SPBTTEST:",s_bttest_str[bttest_index],"=", 1,",",58);
                sprintf((char *)g_rsp_str, "%s%s%s%d,%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_pwr_type, spbttest_bt_tx_pwr_value);
            }
            else if ((pParam->param_count > 2) && (0 != pParam->params[1]->length) && (0 != pParam->params[2]->length))
            {
                int x1 = 0, x2 = 0, i;
                x1 = pParam->params[1]->value[0] - '0';

                for (x2 = 0, i = 0; i < pParam->params[2]->length; i++)
                {
                    x2 = x2 * 10 + (pParam->params[2]->value[i] - '0');
                }

                //wuding: NPI AT cmd AT+SPBTTEST=TXPWR,x1,x2
                //x1: 0---gain value;  1---power level
                //x2: value
                spbttest_bt_tx_pwr_type = x1;
                spbttest_bt_tx_pwr_value = x2;

                //here call the low layer API to set TXPWR, x1 & x2 are the input
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 7: //"TX"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get TX status");
                //here call the low layer API to get TX status
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_tx_status);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                int pktcnt = 0, i;
                type_oper = pParam->params[1]->value[0] - '0';

                if (0 == type_oper)
                {
                    spbttest_bt_tx_status = 0;
                    //here call the low layer API to stop TX
                    OSI_LOGI(0, "SPBTTEST: stop TX");
#ifdef BT_NONSIG_SUPPORT
                    spbttest_bt_status = BT_SetNonSigTxTestMode(false, &spbttest_bt_non_param);
#endif
                }
                else if (1 == type_oper)
                {
                    spbttest_bt_tx_status = 1;

                    if ((pParam->param_count > 2) && (0 != pParam->params[2]->length))
                    {
                        spbttest_bt_tx_mode = pParam->params[2]->value[0] - '0';
#ifdef BT_NONSIG_SUPPORT
                        spbttest_bt_non_param.channel = spbttest_bt_tx_ch;
                        spbttest_bt_non_param.pattern = spbttest_bt_tx_pattern;
                        spbttest_bt_non_param.pac.pac_type = spbttest_bt_tx_pkttype;
                        spbttest_bt_non_param.pac.pac_len = spbttest_bt_tx_pktlen;
                        spbttest_bt_non_param.item.power.power_type = spbttest_bt_tx_pwr_type;
                        spbttest_bt_non_param.item.power.power_value = spbttest_bt_tx_pwr_value;
#endif
                        if (0 == spbttest_bt_tx_mode)
                        {
                            //here call the low layer API to Sart TX for continues
                            OSI_LOGI(0, "SPBTTEST: Sart TX for continues");
#ifdef BT_NONSIG_SUPPORT
                            spbttest_bt_non_param.pac.pac_cnt = 0;
#endif
                        }
                        else
                        {
                            if ((pParam->param_count > 3) && (0 != pParam->params[3]->length))
                            {
                                for (pktcnt = 0, i = 0; i < pParam->params[3]->length; i++)
                                {
                                    pktcnt = pktcnt * 10 + (pParam->params[3]->value[i] - '0');
                                }
                            }
                            else
                            {
                                pktcnt = 0; //continues
                            }

                            //here call the low layer API to Sart TX for single, pktcnt is input for pktcnt
                            OSI_LOGI(0, "SPBTTEST: Sart TX for single");
#ifdef BT_NONSIG_SUPPORT
                            spbttest_bt_non_param.pac.pac_cnt = pktcnt;
#endif
                        }
#ifdef BT_NONSIG_SUPPORT
                        spbttest_bt_status = BT_SetNonSigTxTestMode(true, &spbttest_bt_non_param);
#endif
                    }
                    else
                    {
                        OSI_LOGI(0, "SPBTTEST: Param[2] error.");
                        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                }

                osiThreadSleep(200);

                if (true == spbttest_bt_status) //lint !e774
                {
                    sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
                }
                else
                {
                    sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:ERR");
                }
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 8: //"RXCH"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get RXCH");
                //here call the low layer API to get RXCH
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_rx_ch);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                if (pParam->params[1]->length == 1) // valid type_oper range [0~93]
                {
                    type_oper = pParam->params[1]->value[0] - '0';
                }
                else if (pParam->params[1]->length == 2)
                {
                    type_oper = pParam->params[1]->value[0] - '0';
                    type_oper = type_oper * 10 + (pParam->params[1]->value[1] - '0');
                }
                spbttest_bt_rx_ch = (unsigned char)type_oper;
                //here call the low layer API to set RXCH, type_oper is the input x
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 9: //"RXPATTERN"
        {
            //wuding: there is no RXPATTERN in BT No-Signal!
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get RXPATTERN");
                //here call the low layer API to get RXPATTERN
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_rx_pattern);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                type_oper = pParam->params[1]->value[0] - '0';
                //wuding: NPI AT cmd
                //0000---0
                //1111---1
                //1010---2
                //random---3
                //receiver  ---7
                //11110000---9
                spbttest_bt_rx_pattern = (unsigned char)type_oper;
                //here call the low layer API to set RXPATTERN, type_oper is the input x, range [0~3]
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 10: //"RXPKTTYPE"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get RXPKTTYPE");
                //here call the low layer API to get RXPKTTYPE
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_rx_pktype);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                unsigned char i = 0;
                for (type_oper = 0, i = 0; i < pParam->params[1]->length; i++)
                {
                    type_oper = type_oper * 10 + (pParam->params[1]->value[i] - '0');
                }

                //wuding: the same as TX PKT type
                spbttest_bt_rx_pktype = (unsigned char)type_oper;

                //here call the low layer API to set RXPKTTYPE, type_oper is the input x, range [0~5]
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 11: //"RXGAIN"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get RXGAIN");
                //here call the low layer API to get RXGAIN Mode and x
                if (0 == spbttest_bt_rx_gain_mode)
                {
                    sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", 0);
                }
                else
                {
                    sprintf((char *)g_rsp_str, "%s%s%s%d,%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", 1, spbttest_bt_rx_gain_value);
                }
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                int mode = 0, x = 0, i;
                mode = pParam->params[1]->value[0] - '0';

                spbttest_bt_rx_gain_mode = mode; //0 Auto; 1 Fix

                if (0 == mode)
                {
                    //here call the low layer API to Set RX Gain Auto mode
                    OSI_LOGI(0, "SPBTTEST: Set RX Gain Auto mode");
                }
                else if (1 == mode)
                {
                    if ((pParam->param_count > 2) && (0 != pParam->params[2]->length))
                    {
                        for (x = 0, i = 0; i < pParam->params[2]->length; i++)
                        {
                            x = x * 10 + (pParam->params[2]->value[i] - '0');
                        }
                        //here call the low layer API to Set RX Gain Fix mode, x is input for x
                        //AT+SPBTTEST=RXGAIN,mode,[x]
                        //x: gain parameter , only available when mode is 1
                        if ((0 == x) || (1 == x) || (2 == x) || (3 == x) || (4 == x) || (5 == x)) //double check before send it to controller
                        {
                            spbttest_bt_rx_gain_value = x;
                        }
                        else
                        {
                            spbttest_bt_rx_gain_value = 0;
                        }
                    }
                    else
                    {
                        OSI_LOGI(0, "SPBTTEST: Param[2] error.");
                        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                }

                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
        case 12: //"RX"
        {
            if (quiry_flag)
            {
                OSI_LOGI(0, "SPBTTEST: get RX");
                //here call the low layer API to get RX
                sprintf((char *)g_rsp_str, "%s%s%s%d", "+SPBTTEST:", s_bttest_str[bttest_index], "=", spbttest_bt_rx_status);
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                bool enable = false;
                type_oper = pParam->params[1]->value[0] - '0';

                if (type_oper)
                {
                    spbttest_bt_rx_status = 1;
                    enable = true;
#ifdef BT_NONSIG_SUPPORT
                    spbttest_bt_non_param.pattern = spbttest_bt_rx_pattern;
                    spbttest_bt_non_param.channel = spbttest_bt_rx_ch;
                    if (1 == spbttest_bt_rx_gain_mode) //Fix Mode
                    {
                        spbttest_bt_non_param.item.rx_gain = spbttest_bt_rx_gain_value;
                    }
                    else //auto mode
                    {
                        spbttest_bt_non_param.item.rx_gain = 0;
                    }
                    spbttest_bt_non_param.pac.pac_type = spbttest_bt_rx_pktype;
#endif
                }
                else
                {
                    spbttest_bt_rx_status = 0;
                    enable = false;
                }
#ifdef BT_NONSIG_SUPPORT
                spbttest_bt_status = BT_SetNonSigRxTestMode(enable, &spbttest_bt_non_param, (BT_ADDRESS *)spbttest_bt_address);
#endif
                //here call the low layer API to set RX, type_oper is the input x, range [0~1]
                if (spbttest_bt_status) //lint !e774
                {
                    sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
                }
                else
                {
                    sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:ERR");
                }
            }
            atCmdRespInfoText(pParam->engine, g_rsp_str); // return ok response for AT cmd first
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            //for rx test end and start next tx test
            //BT_RX_STATUS((unsigned char)type_oper);
            break;
        }
        case 13: //"RXDATA"
        {
            if (quiry_flag)
            {
                unsigned int res[5] = {0};
                unsigned char loop_times = 0;

                OSI_LOGI(0, "SPBTTEST: get RXDATA");

#ifdef BT_NONSIG_SUPPORT
                //here call the low layer API to get RXDATA, such as error_bits,total_bits,error_packets,total_packets,rssi
                spbttest_bt_rx_data_flag = false;
                spbttest_bt_status = BT_GetNonSigRxData((BT_NONSIGCALLBACK)BT_SPBTTEST_GetRXDataCallback);

                if (spbttest_bt_status)
                {
                    while (!spbttest_bt_rx_data_flag)
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

                    if (spbttest_bt_rx_data_flag && (BT_SUCCESS == rx_data_show_status))
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
                        OSI_LOGI(0, "rxdata, null. %d %d", spbttest_bt_rx_data_flag, rx_data_show_status);
                    }

                    sprintf((char *)g_rsp_str, "%s%s%s%d,%d,%d,%d,0x%x", "+SPBTTEST:", s_bttest_str[bttest_index], "=",
                            res[0] /*error_bits  990*/, res[1] /*total_bits 1600000*/, res[2] /*error_packets 100*/, res[3] /*total_packets 1024*/, res[4] /*rssi*/);
                }
                else
#endif
                {
                    sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:ERR");
                }
            } //lint !e529
            else
            {
                OSI_LOGI(0, "SPBTTEST: Param error.");
                RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }

            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
#if 0 //zhangyi del at 20200326
        case 14:
        {
            unsigned char oper = pParam->params[1]->value[0] - '0';
            switch (oper)
            {
            case 0:
                pbap_pullphonebook_req();
                break;
            case 1:
                pbap_pullvcardentry_req();
                break;
            case 2:
                pts_mode_enter();
                break;
            case 3:
                pts_mode_exit();
                break;
            default:
                break;
            }
            sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }
#endif
        case 15: //"MAC"
        {
            BT_ADDRESS addr;
            uint8_t addr_str[20] = {0};

            if (quiry_flag)
            {
                BT_GetBdAddr(&addr);
                sprintf((char *)addr_str, "%02x%02x%02x%02x%02x%02x", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
                sprintf((char *)g_rsp_str, "+SPBTTEST:MAC=%s", (char *)addr_str);
                atCmdRespInfoText(pParam->engine, g_rsp_str);
                AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            }
            else if ((pParam->param_count > 1) && (0 != pParam->params[1]->length))
            {
                unsigned char tmp_addr[12] = {0};
                int i = 0;
                int j = 0;

                char outstring[64] = {
                    0,
                };
                outstring[0] = '\0';
                sprintf((char *)outstring, "bt addr: %s, len=%d", ((char *)pParam->params[1]->value), pParam->params[1]->length);
                OSI_LOGI(0, outstring);
                for (i = 0; i < pParam->params[1]->length; i++)
                {
                    if (pParam->params[1]->value[i] >= 'A' && pParam->params[1]->value[i] <= 'F')
                    {
                        tmp_addr[j] = (pParam->params[1]->value[i] - 55);
                    }
                    else if (pParam->params[1]->value[i] >= 'a' && pParam->params[1]->value[i] <= 'f')
                    {
                        tmp_addr[j] = (pParam->params[1]->value[i] - 87);
                    }
                    else if (pParam->params[1]->value[i] >= '0' && pParam->params[1]->value[i] <= '9')
                    {
                        tmp_addr[j] = (pParam->params[1]->value[i] - '0');
                    }
                    else
                    {
                        OSI_LOGI(0, "SPBTTEST: Param[1] error.");
                        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    }
                    OSI_LOGI(0, "tmp_addr=%x", tmp_addr[j]);
                    ++j;
                }

                for (j = 0; j < 6; j++)
                {
                    addr.addr[j] = (tmp_addr[j * 2]) << 4 | (tmp_addr[j * 2 + 1]);
                    OSI_LOGI(0, "set bt addr: %x", addr.addr[j]);
                }

                BT_SetBdAddr(&addr);
                sprintf((char *)g_rsp_str, "%s", "+SPBTTEST:OK");
            }

            atCmdRespInfoText(pParam->engine, g_rsp_str);
            AT_CMD_RETURN(atCmdRespOK(pParam->engine));
            break;
        }

        default: //Error input para
        {
            OSI_LOGI(0, "SPBTTEST: Param[0] error.");
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        }

        break;

    default:
    {
        OSI_LOGI(0, "SPBTTEST: pParam->type error.");
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
    }
    }
#endif
#endif
    return;
}

//---------------------SPBQBTEST----------------------------
#define CHAR_BACKSPACE 8
#define AT_CMD_LINE_BUFF_LEN 1024 * 5
typedef struct
{
    uint8_t *buff;     // buffer, shared by all mode
    uint32_t buffLen;  // existed buffer data byte count
    uint32_t buffSize; // buffer size
    osiMutex_t *mutex;
} BYPASS_BUFFER_T;

static BYPASS_BUFFER_T *g_bypass_buf = NULL;
static atCmdEngine_t *g_pstCmdEngine = NULL;

#if 0 //if you debug, open it. 
static void ATModeSwitchHandler(atDispatch_t *th, atModeSwitchCause_t cause)
{
    atCmdEngine_t *engine = atDispatchGetCmdEngine(th);
    switch (cause)
    {
    case AT_MODE_SWITCH_DATA_ESCAPE:
        if (engine != NULL)
            atCmdRespInfoText(engine, "DATA_ESCAPE(+++) OK");
        break;
    case AT_MODE_SWITCH_DATA_RESUME:
        if (engine != NULL)
            atCmdRespIntermCode(engine, CMD_RC_CONNECT);
        break;
    case AT_MODE_SWITCH_DATA_START:
        if (engine != NULL)
            atCmdRespInfoText(engine, "DATA_START OK");
        break;
    default:
        break;
    }

	return ;
}
#endif

BYPASS_BUFFER_T *at_DataBufCreate()
{
    BYPASS_BUFFER_T *bypass_buff = (BYPASS_BUFFER_T *)malloc(sizeof(*bypass_buff));
    if (bypass_buff == NULL)
        return NULL;
    bypass_buff->buff = (uint8_t *)malloc(AT_CMD_LINE_BUFF_LEN);
    if (bypass_buff->buff == NULL)
    {
        free(bypass_buff);
        return NULL;
    }
    bypass_buff->buffLen = 0;
    bypass_buff->buffSize = AT_CMD_LINE_BUFF_LEN;
    bypass_buff->mutex = osiMutexCreate();
    return bypass_buff;
}

void at_DataBufDestroy(BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
        return;
    osiMutexDelete(bypass_buff->mutex);
    free(bypass_buff->buff);
    free(bypass_buff);
}

//Data: UART --> BT
extern void SET_RE_FLAG(int flag);
extern int GET_RE_FLAG();
static int _RecvBDataformUARTCb(void *param, const void *data, size_t length)
{

    OSI_LOGI(0x10003f2d, "bypassDataRecv,length=%d ", length);
    if (g_bypass_buf == NULL)
        g_bypass_buf = at_DataBufCreate();
    if (g_bypass_buf == NULL)
        return 0;
    osiMutexLock(g_bypass_buf->mutex);
    uint8_t *data_u8 = (uint8_t *)data;
    size_t length_input = length;
    while (length > 0)
    {
        uint8_t c = *data_u8++;
        length--;

        // Remove previous byte for BACKSPACE
#if 0
        if (c == CHAR_BACKSPACE)
        {
            if (g_bypass_buf->buffLen > 0)
                --g_bypass_buf->buffLen;
            continue;
        }
#endif
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (g_bypass_buf->buffLen >= g_bypass_buf->buffSize)
        {
            OSI_LOGI(0, "bypassDataRecv bypass mode overflow, drop all");
            g_bypass_buf->buffLen = 0;
        }
        g_bypass_buf->buff[g_bypass_buf->buffLen++] = c;
    }
    osiMutexUnlock(g_bypass_buf->mutex);
    SET_RE_FLAG(1);
    UartDrv_Tx(g_bypass_buf->buff, length_input);
    g_bypass_buf->buffLen = 0;

    return length_input;
}

static void writeInfoNText(atCmdEngine_t *engine, const char *text, unsigned length)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    if (atDispatchIsDataMode(dispatch) && atDispatchGetDataEngine(dispatch) != NULL)
    {
        atDataWrite(atDispatchGetDataEngine(dispatch), text, length);
    }
    else
    {
        atCmdRespInfoNText(engine, text, length);
    }
}

//Data: BT --> UART
void _RecvBDataformBTCb(char *buf, unsigned int length)
{
    //ToDo:
    if (NULL != g_pstCmdEngine)
    {
        writeInfoNText(g_pstCmdEngine, buf, length);
    }
    return;
}
extern void (*BT_UART_FUCTION)(char *, unsigned int);
//void (*BT_UART_FUCTION)(char *, unsigned int) = NULL;
void BtTOUARTSetCallback(void (*RecvBDataTOUART)(char *, unsigned int))
{
    BT_UART_FUCTION = RecvBDataTOUART;
}

static void start_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    atDispatchSetModeSwitchHandler(dispatch, NULL /*ATModeSwitchHandler*/);
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, dispatch);
    atDataEngine_t *dataEngine = atDispatchGetDataEngine(dispatch);
    //set callback, from UART to BT
    atDataSetBypassMode(dataEngine, _RecvBDataformUARTCb, (void *)engine);
    //set callback, from BT to UART
    //ToDo:
    BtTOUARTSetCallback(_RecvBDataformBTCb);
    UART_SetControllerBqbMode(TRUE);
}

static void stop_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    if (atDispatchIsDataMode(dispatch))
    {
        atDispatchSetModeSwitchHandler(dispatch, NULL /*ATModeSwitchHandler*/);
        atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, dispatch);
    }
    at_DataBufDestroy(g_bypass_buf);
    g_bypass_buf = NULL;
    BtTOUARTSetCallback(NULL);
    UART_SetControllerBqbMode(FALSE);
}

void AT_SPBQB_CmdFunc_TEST(atCommand_t *pParam)
{
    int iFlag = -1;
    bool bParamRet = true;
    char cDebugBuf[64] = {'\0'};

    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        iFlag = atParamUintInRange(pParam->params[0], 0, 1, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s(%d):Param[0] error.", __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        if (1 == iFlag)
        {
            g_pstCmdEngine = pParam->engine;
            start_transparent_mode(pParam->engine);
        }
        else if (0 == iFlag)
        {
            stop_transparent_mode(pParam->engine);
            g_pstCmdEngine = NULL;
            RETURN_OK(pParam->engine);
        }

        break;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s(%d):exe not support.", __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    }
}

#endif
