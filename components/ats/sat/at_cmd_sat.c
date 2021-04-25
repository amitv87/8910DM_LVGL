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

#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "osi_sysnv.h"
#include "at_cfw.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include "ml.h"
#include "mal_api.h"
#include "srv_sim_detect.h"
#include "drv_md_ipc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include "sockets.h"
#include "time.h"

#include "nvm.h"
//#include "pal_imei.h"
uint8_t at_ascii2hex(uint8_t *pInput, uint8_t *pOutput);

#define CFW_SAT_IND_PDU 0
#define CFW_SAT_IND_TEXT 1
#define CFW_SAT_IND_UCS2 2

bool gATSATComQualifier[NUMBER_OF_SIM] = {
    0,
};
bool gATSATSendSMSFlag[NUMBER_OF_SIM] = {false, false};
bool gATSATLocalInfoFlag[NUMBER_OF_SIM] = {false, false};
bool gATSATSendUSSDFlag[NUMBER_OF_SIM] = {false, false};
bool gATSATSetupCallFlag[NUMBER_OF_SIM] = {false, false};
bool gATSATSetupCallProcessingFlag[NUMBER_OF_SIM] = {false, false};

uint8_t AllowedInstance = INSTANCE;
static uint8_t g_ActiveSatFlag = 0;

#ifdef AT_STSF
//Start For STSF command
uint8_t STK_Mode = 0;
uint8_t STK_ProfileBuffer[12] = {0};
uint8_t STK_TimeOut = 1;
uint8_t STK_AutoResp = 0;
//End For STSF command
#endif
//uint8_t Write_esim_mask = 0;

uint8_t AT_SS_GetUSSDNum();
uint8_t AT_SS_GetUSSDState();
uint8_t CFW_GetCurrentCmdNum(CFW_SIM_ID nSimID);
uint32_t sms_tool_CheckDcs(uint8_t *pDcs);
void _ResetSimTimer(uint8_t nTimeInterval, uint8_t nTimeUnit, CFW_SIM_ID nSimID);
void CFW_SetSATIndFormate(bool bMode);
uint32_t ML_Unicode2LocalLanguageBigEnding(const uint8_t *in, uint32_t in_len,
                                           uint8_t **out, uint32_t *out_len, uint8_t nCharset[12]);

uint32_t CFW_SimInit(uint8_t nLevel, CFW_SIM_ID nSimID);
//uint16_t SUL_hex2ascii(uint8_t *pInput, uint16_t nInputLen, uint8_t *pOutput);
bool charset_gsm2ascii(uint8_t *src, uint16_t slen, uint8_t *dst, uint16_t *dlen);
#if 0
uint32_t AT_SAT_SpecialCMDSendSMS(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    uint8_t nAlphaLen = 0;
    uint8_t nAddrLen = 0;
    uint8_t nAddrType = 0;
    uint8_t nTPDULen = 0;

    uint8_t *nAlphaStr = NULL;
    uint8_t *nAddrStr = NULL;
    uint8_t *nTPDUStr = NULL;

    CFW_SAT_SMS_RSP *pSmsResp = NULL;
    uint16_t nMemsize = SIZEOF(CFW_SAT_SMS_RSP);
    uint16_t nLength = 0x00;
    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;

    OSI_LOGI(0x10004586, "AT_SAT_SpecialCMDSendSMS gATSATSendSMSFlag %d\n", gATSATSendSMSFlag[nSimID]);

    gATSATSendSMSFlag[nSimID] = true;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    uint8_t nCmdQual = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;

        nLength = pOffset[nIndex++];
        nAlphaLen = nLength;
        nAlphaStr = pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x06) || (pOffset[nIndex] == 0x86))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++] - 1;
        nAddrLen = nLength;
        nAddrType = pOffset[nIndex++];
        nAddrStr = pOffset + nIndex;
    }
    else
        nLength = 0;
    nIndex = nIndex + nLength;
    nMemsize += nLength;

    if ((pOffset[nIndex] == 0x0B) || (pOffset[nIndex] == 0x8B))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
        nTPDULen = nLength;
        nTPDUStr = (uint8_t *)pOffset + nIndex;
    }
    else
    {
        OSI_LOGI(0x10004587, "Error Invalid data Not 0x0B or 0x8B But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
    }
    nMemsize += nLength;

    pSmsResp = (CFW_SAT_SMS_RSP *)malloc(nMemsize);
    if (pSmsResp == NULL)
    {
        OSI_LOGI(0x081009a5, "Error Malloc failed\n");
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsResp, nMemsize);

    pSmsResp->nAlphaLen = nAlphaLen;
    pSmsResp->pAlphaStr = (uint8_t *)pSmsResp + SIZEOF(CFW_SAT_SMS_RSP);
    SUL_MemCopy8(pSmsResp->pAlphaStr, nAlphaStr, nAlphaLen);

    pSmsResp->nAddrLen = nAddrLen;
    pSmsResp->nAddrType = nAddrType;
    pSmsResp->pAddrStr = pSmsResp->pAlphaStr + nAlphaLen;
    if (nAddrLen != 0)
    {
        SUL_MemCopy8(pSmsResp->pAddrStr, nAddrStr, nAddrLen);
    }

    pSmsResp->nPDULen = nTPDULen;
    pSmsResp->pPDUStr = pSmsResp->pAddrStr + nAddrLen;

    SUL_MemCopy8(pSmsResp->pPDUStr, nTPDUStr, nTPDULen);
    // check the DCS and tpdu, if dcs is 7 bits and all the tpdu bit7 is 0, it means the DCS is not equal to the data,
    // we need to transfer the 8 bit data to 7 bit.

    uint8_t uDCSOffsize = 0;
    uint8_t uVPLen = 0;
    uint8_t uVPF = 0;
    uint8_t UdataOff = 0;

    uint8_t buff7[140] = {
        0,
    };
    uint8_t buff7len = 0;

    // MTI+MR+DALEN/2+DATYPE+PID + DCS
    uDCSOffsize = 2 + (pSmsResp->pPDUStr[2] + 1) / 2 + 1 + 2;

    // bit3,4 of MTI is vpf
    uVPF = (pSmsResp->pPDUStr[0] >> 3) & 3;
    if (!uVPF) // NOT present
        uVPLen = 0;
    else if (uVPF == 2) // relative format
        uVPLen = 1;
    else
        uVPLen = 7;

    OSI_LOGI(0x10004588, "uDCSOffsize=%d,pPDUPacketTmp[uDCSOffsize]=0x%02x, uVPLen=%d pSmsResp->pPDUStr[uDCSOffsize-1] %d\n",
             uDCSOffsize, pSmsResp->pPDUStr[uDCSOffsize], uVPLen, pSmsResp->pPDUStr[uDCSOffsize - 1]);
    // dcs offsize + udl+1-->userdata
    // cause VPF is 0, it means vp field is not present, so when test, we don't count this, othersize VP will take 1~7 octs.
    UdataOff = uDCSOffsize + 2 + uVPLen;

    //SEND SHORT MESSAGE;
    //bit 1:                  0 = packing not required
    //                          1 = SMS packing by the ME required
    if (0x00 == nCmdQual)
    {
        //packing not required
    }
    else if (!pSmsResp->pPDUStr[uDCSOffsize]) // check if match the condition 1, DCS is 0
    {
        uint8_t uLoop = 0;

        // here need to check whether all the bit7 of userdata is 0,
        for (uLoop = 0; uLoop < pSmsResp->pPDUStr[UdataOff - 1]; uLoop++)
        {
            if ((pSmsResp->pPDUStr[UdataOff + uLoop] >> 7) & 1)
                break;
        }

        OSI_LOGI(0x08100db1, "uLoop=%d, pSmsResp->pPDUStr[UdataOff-1]=%d\n", uLoop,
                 pSmsResp->pPDUStr[UdataOff - 1]);

        // check if match the condition 2: all the bit 7 of user data is 0.
        if (uLoop && (uLoop == pSmsResp->pPDUStr[UdataOff - 1]))
        {
            buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);

            SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

            OSI_LOGI(0x08100db2, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);

            OSI_LOGI(0x08100db7, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

            pSmsResp->nPDULen += (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);

            OSI_LOGI(0x08100db4, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);
        }
    }
    else if (0xF2 == pSmsResp->pPDUStr[uDCSOffsize])
    {
        // change DCS to 0, 7  bit decode
        pSmsResp->pPDUStr[uDCSOffsize] = 0;
        uint8_t uLoop = 0;
        OSI_LOGI(0x10004589, "New pSmsResp->pPDUStr[UdataOff - 1]= %d\n", pSmsResp->pPDUStr[UdataOff - 1]);
        // here need to check whether all the bit7 of userdata is 0,
        for (uLoop = 0; uLoop < pSmsResp->pPDUStr[UdataOff - 1]; uLoop++)
        {
            if ((pSmsResp->pPDUStr[UdataOff + uLoop] >> 7) & 1)
                break;
        }
        if (uLoop && (uLoop == pSmsResp->pPDUStr[UdataOff - 1]))
        {
            // encode user data to 7 bit data
            buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
            SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

            OSI_LOGI(0x08100db2, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);
            OSI_LOGI(0x08100db7, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

            pSmsResp->nPDULen += (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);

            OSI_LOGI(0x08100db4, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);
        }
    }
    else if ((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0F)) && (0x00 == pSmsResp->pPDUStr[uDCSOffsize - 1]))
    {
        OSI_LOGI(0x1000458a, "((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0F)) && (0x00 == pSmsResp->pPDUStr[uDCSOffsize-1]))\n");
        // change DCS to 0, 7  bit decode
        pSmsResp->pPDUStr[uDCSOffsize] = 0;
        // encode user data to 7 bit data
        buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
        SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

        OSI_LOGI(0x08100db2, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);
        OSI_LOGI(0x08100db7, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

        pSmsResp->nPDULen += (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);
        OSI_LOGI(0x08100db4, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);
    }

    uint32_t result = 0;
    uint8_t *pPDUPacket = NULL;
    uint8_t *pPDUPacketTmp = NULL;
    uint16_t nPDUPacket = 0;
    uint16_t nPDUPacketTmp = 0;
    uint8_t pAdd[12] = {
        0x00,
    };

    OSI_LOGI(0x1000458b, "AT_SAT_SpecialCMDSendSMS nAddrLen = %d", pSmsResp->nAddrLen);
    if (pSmsResp->nAddrLen == 0) //如果没有传地址上来，就取得短信息中心号码
    {
        CFW_SMS_PARAMETER smsInfo;
        memset(&smsInfo, 0, sizeof(CFW_SMS_PARAMETER));
        CFW_CfgGetSmsParam(&smsInfo, 0, nSimID);

        pSmsResp->nAddrLen = smsInfo.nNumber[0];
        memcpy(pAdd, &(smsInfo.nNumber[1]), smsInfo.nNumber[0]);
        nPDUPacketTmp = pSmsResp->nPDULen + pSmsResp->nAddrLen + 1;

        pPDUPacket = (uint8_t *)malloc(nPDUPacketTmp);
        if (pPDUPacket == NULL)
        {
            free(pSmsResp);
            pSmsResp = NULL;
            return ERR_NO_MORE_MEMORY;
        }

        memset(pPDUPacket, 0xFF, nPDUPacketTmp);
        pPDUPacketTmp = pPDUPacket;
        *(pPDUPacketTmp++) = pSmsResp->nAddrLen;
    }
    else //如果传上来地址，在前面加91 代表加号
    {
        nPDUPacketTmp = pSmsResp->nPDULen + pSmsResp->nAddrLen + 2; //tpdu length ，加2是因为增加了2位: 地址长度和 91
        pPDUPacket = (uint8_t *)malloc(nPDUPacketTmp);
        if (pPDUPacket == NULL)
        {
            free(pSmsResp);
            pSmsResp = NULL;
            return ERR_NO_MORE_MEMORY;
        }
        memset(pPDUPacket, 0xFF, nPDUPacketTmp);
        memcpy(pAdd, pSmsResp->pAddrStr, pSmsResp->nAddrLen);
        pPDUPacketTmp = pPDUPacket;
        *(pPDUPacketTmp++) = pSmsResp->nAddrLen + 1; //地址的长度，加一是因为多了一个91
        OSI_LOGI(0x1000458c, "AT_SAT_SpecialCMDSendSMS pSmsResp->nAddrType = %d", pSmsResp->nAddrType);
        *(pPDUPacketTmp++) = pSmsResp->nAddrType; //0x91;   // "+"
    }

    nPDUPacket = nPDUPacketTmp;
    if (pSmsResp->nAddrLen != 0)
    {
        memcpy(pPDUPacketTmp, pAdd, pSmsResp->nAddrLen);
        pPDUPacketTmp += pSmsResp->nAddrLen;
    }
    if (pSmsResp->nPDULen != 0)
    {
        memcpy(pPDUPacketTmp, pSmsResp->pPDUStr, pSmsResp->nPDULen);
    }

    uint8_t nFormat = 0x00;
    uint8_t Uti = 0;

    CFW_CfgGetSmsFormat(&nFormat, nSimID);
    CFW_CfgSetSmsFormat(0, nSimID);
    //TODO ... AT_GetFreeUTI(CFW_SS_SRV_ID, &Uti);
    //result = CFW_SmsSendMessage(NULL, pPDUPacket, nPDUPacket, Uti, nSimID);
    CFW_CfgSetSmsFormat(nFormat, nSimID);

    OSI_LOGI(0x1000458b, "AT_SAT_SpecialCMDSendSMS nAddrLen = %d", pSmsResp->nAddrLen);
    OSI_LOGI(0x1000458d, "AT_SAT_SpecialCMDSendSMS nPDULen = %d", pSmsResp->nPDULen);
    if (result == ERR_SUCCESS)
    {
        OSI_LOGI(0x1000458e, "AT_SAT_SpecialCMDSendSMS Success");
    }
    else
    {
        OSI_LOGI(0x1000458f, "AT_SAT_SpecialCMDSendSMS CFW_SmsSendMessage error!!!! = %x", result);
    }

    free(pPDUPacket);
    pPDUPacket = NULL;
    if (pSmsResp != NULL)
    {
        free(pSmsResp);
        pSmsResp = NULL;
    }
    return ERR_SUCCESS;
}

uint32_t gSATCurrentCmdStamp[CFW_SIM_COUNT] = {
    0x00,
};

uint32_t AT_SAT_SpecialCMDProvideLocalInfo(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{

    uint8_t *pOffset = pFetchData;
    uint8_t nIndex = 0x00;
    uint8_t nUTI = 0x00;
    uint32_t nRet = ERR_SUCCESS;
    nUTI = AT_ASYN_GET_DLCI(nSimID);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    uint8_t nComQualifier = pOffset[nIndex + 5];
    //uint8_t nCmdNum         = pOffset[nIndex + 3];
    gATSATComQualifier[nSimID] = nComQualifier;
    nIndex = nIndex + 10;
    OSI_LOGI(0x10004590, "AT_SAT_SpecialCMDProvideLocalInfo nComQualifier %d\n", nComQualifier);

    /*
    -   PROVIDE LOCAL INFORMATION
    '00' = Location Information (MCC, MNC, LAC and Cell Identity)
    '01' = IMEI of the ME
    '02' = Network Measurement results
    '03' = Date, time and time zone
    '04' = Language setting
    '05' = Timing Advance
    '06' to 'FF' = Reserved
    */
    if (0x00 == nComQualifier)
    {
        CFW_TSM_FUNCTION_SELECT SelecFUN;
        SelecFUN.nNeighborCell = false;
        SelecFUN.nServingCell = true;
        gATSATLocalInfoFlag[nSimID] = true;
        nRet = CFW_EmodOutfieldTestStart(&SelecFUN, nUTI, nSimID);
        if (nRet == ERR_SUCCESS)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0x090002aa, " Func: %s PROVIDE LOCAL INFO ERR_SUCCESS", __FUNCTION__);
        }
        else
        {
            OSI_LOGXI(OSI_LOGPAR_SI, 0x10004591, " Func: %s PROVIDE LOCAL INFO ERROR!!! 0x%x", __FUNCTION__, nRet);
        }
        return ERR_CONTINUE;
    }
    else if (0x01 == nComQualifier)
    {
        uint8_t pImei[16] = {
            0x00,
        };
        uint8_t nImeiLen = 0x00;
        uint8_t pResponseData[26] = {0x81, 0x03, 0x01, 0x26, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00, 0x94, 0x08, /*0x10,0x72,0x84,0x00,0x53,0x56,0x68,0xF8*/};
        gATSATLocalInfoFlag[nSimID] = true;
        pResponseData[2] = CFW_GetCurrentCmdNum(nSimID);
        CFW_EmodGetIMEIBCD(pImei, &nImeiLen, nSimID);

        if (nImeiLen > 8)
        {
            nImeiLen = 8;
        }
        SUL_MemCopy8(&pResponseData[14], pImei, nImeiLen);

        nRet = CFW_SatResponse(0xFF, 0x00, 0x0, pResponseData, 22, nUTI, nSimID);
        OSI_LOGI(0x10004592, "CFW_SatResponse nRet:0x%x!\n", nRet);
        return ERR_CONTINUE;
    }
    else if ((0x02 == nComQualifier) || (0x03 == nComQualifier))
    {
    }
    else if (0x05 == nComQualifier)
    {
        CFW_TSM_FUNCTION_SELECT SelecFUN;
        SelecFUN.nNeighborCell = false;
        SelecFUN.nServingCell = true;

        gATSATLocalInfoFlag[nSimID] = true;

        nRet = CFW_EmodOutfieldTestStart(&SelecFUN, nUTI, nSimID);
        if (nRet == ERR_SUCCESS)
        {
            OSI_LOGXI(OSI_LOGPAR_S, 0x090002aa, " Func: %s PROVIDE LOCAL INFO ERR_SUCCESS", __FUNCTION__);
        }
        else
        {
            OSI_LOGXI(OSI_LOGPAR_SI, 0x10004591, " Func: %s PROVIDE LOCAL INFO ERROR!!! 0x%x", __FUNCTION__, nRet);
        }
        return ERR_CONTINUE;
    }
    else
    {
        return ERR_NOT_SUPPORT;
    }
    return ERR_SUCCESS;
}

extern bool gSatFreshComm[];

uint32_t AT_SAT_SpecialCMDRefresh(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{

    OSI_LOGXI(OSI_LOGPAR_S, 0x07090005, "%s\n", __func__);

    uint8_t nIndex = 0x00;
    uint8_t nLength = 0x00;
    uint8_t *pOffset = pFetchData;
    uint8_t nNumOfFiles = 0x00;
    uint8_t *pFiles = NULL;

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    uint8_t nComQualifier = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    /*
    '00' =SIM Initialization and Full File Change Notification;
    '01' = File Change Notification;
    '02' = SIM Initialization and File Change Notification;
    '03' = SIM Initialization;
    '04' = SIM Reset;
    '05' to 'FF' = reserved values.
    */
    OSI_LOGI(0x10004593, "nComQualifier %d\n", nComQualifier);
    if (0x00 == nComQualifier)
    {
        OSI_LOGI(0x10004594, "nComQualifier 0x00 re attachment NW\n");
        //TODO
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);

        uint8_t nUTI = 0x00;
        CFW_GetFreeUTI(0, &nUTI);

        uint32_t nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x01;
        OSI_LOGI(0x08100dc8, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
    }
    else if (0x01 == nComQualifier)
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {
            if (pOffset[++nIndex] == 0x81)
                nIndex++;
            nLength = pOffset[nIndex++];
            nNumOfFiles = pOffset[nIndex++];

            pFiles = (uint8_t *)CSW_SIM_MALLOC(nLength);
            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);
            uint8_t nUTI = 0x00;
            CFW_GetFreeUTI(0, &nUTI);

            uint32_t nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x11;
            OSI_LOGI(0x08100dc8, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
            return ERR_SUCCESS;
        }
        else
        {
            return ERR_SUCCESS;
        }
    }
    else if (0x02 == nComQualifier)
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {
            if (pOffset[++nIndex] == 0x81)
                nIndex++;

            nLength = pOffset[nIndex++];
            nNumOfFiles = pOffset[nIndex++];

            pFiles = (uint8_t *)CSW_SIM_MALLOC(nLength);
            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);

            uint8_t nUTI = 0x00;
            CFW_GetFreeUTI(0, &nUTI);
            uint32_t nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x21;
            OSI_LOGI(0x08100dc8, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
            return ERR_SUCCESS;
        }
    }
    else if (0x03 == nComQualifier)
    {
        //Add by Lixp at 20130721
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);
        uint8_t nUTI = 0x00;
        CFW_GetFreeUTI(0, &nUTI);
        //uint32_t nRet = CFW_SetComm(CFW_DISABLE_COMM,1,nUTI,nSimID);
        uint32_t nRet = CFW_SimReset(nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x31;
        OSI_LOGI(0x10004595, "CFW_SimReset  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
        return ERR_SUCCESS;
    }
    else if (0x04 == nComQualifier)
    {
        OSI_LOGI(0x08100dcc, "CFW_SimInit[%d] %d\n", nSimID);
        gSatFreshComm[nSimID] = 0x41;
        CFW_SimInit(0, nSimID);
        return ERR_SUCCESS;
    }
    CFW_SatResponse(0x01, 0x00, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSimID), nSimID);
    return ERR_SUCCESS;
}

uint32_t AT_SAT_SpecialCMDDisplayTxt(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    uint8_t nCodeSch = 0;
    uint8_t *pTextStr = NULL;
    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;
    uint16_t nLength = 0x00;

    //pOffset += 2;
    OSI_LOGI(0x10004596, "AT_SAT_SpecialCMDDisplayTxt\n");

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x0D) || (pOffset[nIndex] == 0x8D))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++] - 1;
        nCodeSch = pOffset[nIndex++];
        pTextStr = pOffset + nIndex;
    }
    else
    {
        OSI_LOGI(0x10004597, "Error Invalid data Not 0x0D or 0x8D But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
        ;
    }
    uint8_t nCSWDCS = nCodeSch;
    sms_tool_CheckDcs(&nCSWDCS);
    OSI_LOGI(0x10004598, "AT_SAT_SpecialCMDDisplayTxt nLength %d nCodeSch %d nCSWDCS %d\n", nLength, nCodeSch, nCSWDCS);
    if (0x00 == nCSWDCS)
    {
        uint8_t *p = (uint8_t *)AT_MALLOC(2 * nLength);
        SUL_ZeroMemory8(p, 2 * nLength);

        SUL_Decode7Bit(pTextStr, p, nLength);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004599, "AT_SAT_SpecialCMDDisplayTxt p %s\n", p);
        AT_FREE(p);
    }
    else
    {
        OSI_LOGXI(OSI_LOGPAR_S, 0x1000459a, "AT_SAT_SpecialCMDDisplayTxt pTextStr %s\n", pTextStr);
    }
    return ERR_SUCCESS;
}

uint32_t AT_SAT_SpecialCMDPollInterval(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{

    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;
    uint8_t nTimeUnit = 0x00;
    uint8_t nTimeInterval = 0x00;

    OSI_LOGXI(OSI_LOGPAR_S, 0x07090005, "%s\n", __func__);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x04) || (pOffset[nIndex] == 0x84))
    {
        //nLen          = pOffset[nIndex + 1];
        nTimeUnit = pOffset[nIndex + 2];
        nTimeInterval = pOffset[nIndex + 3];
    }
    else
    {
        OSI_LOGI(0x1000459b, "Invald tag pOffset[nIndex] 0x%x", pOffset[nIndex]);
        return ERR_SUCCESS;
    }
    OSI_LOGI(0x1000459c, "Success nTimeInterval %d nTimeUnit %d", nTimeInterval, nTimeUnit);
    _ResetSimTimer(nTimeInterval, nTimeUnit, nSimID);
    return ERR_SUCCESS;
}

extern uint32_t gSATTimerValue[][CFW_SIM_COUNT];
extern uint8_t _GetTimerIdentifier(uint8_t *p);
extern uint32_t _GetTimerValue(uint8_t *p);
extern uint32_t sxs_ExistTimer(u32 Id);

uint32_t AT_SAT_SpecialCMDTimerManagement(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;
    uint8_t nTimerID = 0x00;
    uint8_t nHour = 0x00;
    uint8_t nMinute = 0x00;
    uint8_t nSecond = 0x00;
    uint32_t nTimerValue = 0x00;
    uint32_t nRet = ERR_SUCCESS;

    OSI_LOGXI(OSI_LOGPAR_S, 0x07090005, "%s\n", __func__);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    uint8_t nComQualifier = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x24) || (pOffset[nIndex] == 0xA4))
    {
        nTimerID = _GetTimerIdentifier(&pOffset[nIndex]);
        nIndex += 3;
    }
    if ((pOffset[nIndex] == 0x25) || (pOffset[nIndex] == 0xA5))
    {
        nTimerValue = _GetTimerValue(&pOffset[nIndex]);
        nHour = nTimerValue >> 16;
        nMinute = nTimerValue >> 8;
        nSecond = nTimerValue & 0xFF;
    }
    if (nTimerID)
    {
        gSATTimerValue[nTimerID - 1][nSimID] = nTimerValue;
    }
    bool bRet = false;
    /*
    -   TIMER MANAGEMENT
    bits 1 to 2
            00 = start
            01 = deactivate
            10 = get current value
            11 = RFU
    bits 3 to 8 RFU
    */
    OSI_LOGI(0x10004593, "nComQualifier %d\n", nComQualifier);
    if (0x00 == nComQualifier)
    {
        bRet = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1, COS_TIMER_MODE_SINGLE, (nHour * 60 * 60 + nMinute * 60 + nSecond) * 1000 MILLI_SECOND);
        OSI_LOGI(0x08100de8, "COS_SetTimerEX	  %d\n", bRet);
        OSI_LOGI(0x08100de9, "nTimerID	  %d\n", nTimerID);
        OSI_LOGI(0x08100dea, "Second	  %d\n", (nHour * 60 * 60 + nMinute * 60 + nSecond));
    }
    else if (0x01 == nComQualifier)
    {
        bRet = COS_KillTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1);
        OSI_LOGI(0x08100deb, "COS_KillTimerEX	  %d\n", bRet);
        OSI_LOGI(0x08100de9, "nTimerID	  %d\n", nTimerID);
    }

    uint8_t pResponseDataQual1[250] = {0x81, 0x03, 0x01, 0x27, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00, 0x04, 0x01,
                                       0x01, 0xa5, 0x03, 0x00, 0x00, (uint8_t)sxs_ExistTimer(HVY_TIMER_IN + PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1) / 16384};
    uint8_t pResponseDataQual0[250] = {0x81, 0x03, 0x01, 0x27, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
    pResponseDataQual1[2] = CFW_GetCurrentCmdNum(nSimID);
    pResponseDataQual0[2] = CFW_GetCurrentCmdNum(nSimID);

    if (nComQualifier == 1)
        nRet = CFW_SatResponse(0xFF, 0x00, 0x0, pResponseDataQual1, 20, AT_ASYN_GET_DLCI(nSimID), nSimID);
    else if (nComQualifier == 0)
        nRet = CFW_SatResponse(0xFF, 0x00, 0x0, pResponseDataQual0, 12, AT_ASYN_GET_DLCI(nSimID), nSimID);
    OSI_LOGI(0x10004592, "CFW_SatResponse nRet:0x%x!\n", nRet);

    return ERR_CONTINUE;
}

uint32_t AT_SAT_SpecialCMDSendUSSD(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    OSI_LOGXI(OSI_LOGPAR_S, 0x07090005, "%s\n", __func__);

    uint8_t nUSSDLen = 0;

    uint8_t *nUSSDStr = NULL;
    uint8_t *pUSSDPacket = NULL;
    uint8_t nUSSDdcs = 0;

    uint8_t UTI = AT_ASYN_GET_DLCI(nSimID);
    uint32_t nRet = 0;

    uint16_t nMemsize = SIZEOF(CFW_SAT_SMS_RSP);
    uint16_t nLength = 0x00;
    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;

    OSI_LOGI(0x1000459d, "gATSATSendUSSDFlag %d\n", gATSATSendUSSDFlag[nSimID]);

    gATSATSendUSSDFlag[nSimID] = true;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x0A) || (pOffset[nIndex] == 0x8A))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;

        nLength = pOffset[nIndex++];
        nUSSDdcs = pOffset[nIndex++];

        nUSSDLen = nLength;
        nUSSDStr = (uint8_t *)pOffset + nIndex;
    }
    else
    {
        OSI_LOGI(0x1000459e, "Error Invalid data Not 0x0A or 0x8A But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
    }
    nMemsize += nLength;
    pUSSDPacket = (uint8_t *)AT_MALLOC(nUSSDLen - 1); //-1 is DCS
    if (pUSSDPacket == NULL)
    {
        return ERR_NO_MORE_MEMORY;
    }
    memset(pUSSDPacket, 0xFF, nUSSDLen - 1);
    memcpy(pUSSDPacket, nUSSDStr, nUSSDLen - 1);

    OSI_LOGI(0x1000459f, " nAddrLen = %d", nUSSDLen);
    AT_TC_MEMBLOCK(g_sw_AT_SAT, pUSSDPacket, nUSSDLen - 1, 16);

    if (AT_SS_GetUSSDState())
        nRet = CFW_SsSendUSSD(pUSSDPacket, nUSSDLen - 1, 3, nUSSDdcs, AT_SS_GetUSSDNum(), nSimID);
    else
        nRet = CFW_SsSendUSSD(pUSSDPacket, nUSSDLen - 1, 3, nUSSDdcs, UTI, nSimID);

    if (nRet == ERR_SUCCESS)
    {
        OSI_LOGI(0x100045a0, " Success");
    }
    else
    {
        OSI_LOGI(0x100045a1, " CFW_SsSendUSSD error!!!! = %x", nRet);
    }

    AT_FREE(pUSSDPacket);
    pUSSDPacket = NULL;
    return ERR_SUCCESS;
}

uint32_t AT_SAT_SpecialCMDCallSetup(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    OSI_LOGI(0x100045a2, "AT_SAT_SpecialCMDCallSetup AT_SAT_SpecialCMDCallSetup %d\n", gATSATSetupCallFlag[nSimID]);
    uint8_t nAlphaConfirmLen = 0;
    uint8_t *pAlphaConfirmStr = NULL;

    uint8_t nAlphaSetupLen = 0;
    uint8_t *pAlphaSetupStr = NULL;

    uint8_t nAddrLen = 0;
    uint8_t nAddrType = 0;
    uint8_t *pAddrStr = NULL;

    uint8_t nSubAddrLen = 0;
    uint8_t *pSubAddrStr = NULL;

    uint8_t nCapabilityCfgLen = 0;
    uint8_t *pCapabilityCfgStr = NULL;

    uint8_t nTuint = 0;
    uint8_t nTinterval = 0;

    CFW_SAT_CALL_RSP *pCallResp = NULL;
    uint16_t nMemsize = SIZEOF(CFW_SAT_CALL_RSP);
    uint16_t nLength = 0x00;
    uint32_t nIndex = 0x00;
    uint8_t *pOffset = pFetchData;

    gATSATSetupCallFlag[nSimID] = true;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
        nAlphaConfirmLen = nLength;
        pAlphaConfirmStr = pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x06) || (pOffset[nIndex] == 0x86))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++] - 1;
        nAddrLen = nLength;
        nAddrType = pOffset[nIndex++];
        pAddrStr = pOffset + nIndex;
    }
    else
    {
        OSI_LOGI(0x100045a3, "Error Invalid data Not 0x06 or 0x86 But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
    }
    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x07) || (pOffset[nIndex] == 0x87))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
        nCapabilityCfgLen = nLength;
        pCapabilityCfgStr = (uint8_t *)pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x08) || (pOffset[nIndex] == 0x88)) // SubAddress
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
        nSubAddrLen = nLength;
        pSubAddrStr = (uint8_t *)pOffset + nIndex;
    }

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x04) || (pOffset[nIndex] == 0x84)) // Duration
    {
        nTuint = pOffset[nIndex + 2];
        nTinterval = pOffset[nIndex + 3];
        nLength = 4;
    }
    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x1E) || (pOffset[nIndex] == 0x9E))
        nIndex = nIndex + 4;

    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength = pOffset[nIndex++];
        nAlphaSetupLen = nLength;
        pAlphaSetupStr = pOffset + nIndex;
    }
    nMemsize += nLength;
    pCallResp = (CFW_SAT_CALL_RSP *)AT_MALLOC(nMemsize); //4//46
    if (pCallResp == NULL)
    {
        OSI_LOGI(0x081009a5, "Error Malloc failed\n");
        return ERR_NO_MORE_MEMORY;
    }
    pCallResp->nAlphaConfirmLen = nAlphaConfirmLen;
    pCallResp->pAlphaConfirmStr = (uint8_t *)pCallResp + SIZEOF(CFW_SAT_CALL_RSP);
    SUL_MemCopy8(pCallResp->pAlphaConfirmStr, pAlphaConfirmStr, nAlphaConfirmLen);

    pCallResp->nAddrLen = nAddrLen;
    pCallResp->nAddrType = nAddrType;
    pCallResp->pAddrStr = pCallResp->pAlphaConfirmStr + nAlphaConfirmLen;
    SUL_MemCopy8(pCallResp->pAddrStr, pAddrStr, nAddrLen);

    pCallResp->nCapabilityCfgLen = nCapabilityCfgLen;
    pCallResp->pCapabilityCfgStr = pCallResp->pAddrStr + nAddrLen;
    SUL_MemCopy8(pCallResp->pCapabilityCfgStr, pCapabilityCfgStr, nCapabilityCfgLen);

    pCallResp->nSubAddrLen = nSubAddrLen;
    pCallResp->pSubAddrStr = pCallResp->pCapabilityCfgStr + nCapabilityCfgLen;
    SUL_MemCopy8(pCallResp->pSubAddrStr, pSubAddrStr, nSubAddrLen);

    pCallResp->nTuint = nTuint;
    pCallResp->nTinterval = nTinterval;

    pCallResp->nAlphaSetupLen = nAlphaSetupLen;
    pCallResp->pAlphaSetupStr = pCallResp->pSubAddrStr + nSubAddrLen;
    SUL_MemCopy8(pCallResp->pAlphaSetupStr, pAlphaSetupStr, nAlphaSetupLen);

    CFW_DIALNUMBER nDialNumber;
    uint8_t nCCIndex = 0x00;
    nDialNumber.pDialNumber = AT_MALLOC(pCallResp->nAddrLen);
    SUL_MemSet8(nDialNumber.pDialNumber, 0x00, pCallResp->nAddrLen);
    SUL_MemCopy8(nDialNumber.pDialNumber, pCallResp->pAddrStr, pCallResp->nAddrLen);
    nDialNumber.nDialNumberSize = pCallResp->nAddrLen;
    nDialNumber.nType = pCallResp->nAddrType;

    if (pCallResp != NULL)
    {
        AT_FREE(pCallResp);
        pCallResp = NULL;
    }
    CFW_CcInitiateSpeechCallEx(&nDialNumber, &nCCIndex, AT_ASYN_GET_DLCI(nSimID), nSimID);
    return ERR_SUCCESS;
}

uint8_t *gSATSpecialCMDData[CFW_SIM_COUNT] = {
    NULL,
};
uint8_t gSATSpecialCMDDataLen[CFW_SIM_COUNT] = {
    0x00,
};

uint32_t AT_SAT_SpecialCMDProcess(uint8_t *pFetchData, uint32_t nFetchDatalen, CFW_SIM_ID nSimID)
{
    if (pFetchData == NULL)
    {
        return 0x00;
    }

    AT_TC_MEMBLOCK(g_sw_AT_SAT, pFetchData, nFetchDatalen, 16);

    uint32_t nIndex = 0x00;
    uint32_t nLength = 0x00;
    uint32_t nRet = 0x00;
    uint8_t *pOffset = pFetchData;

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nLength = pOffset[nIndex] + nIndex; // Update the length

    OSI_LOGXI(OSI_LOGPAR_SI, 0x100045a4, "%s DataLen[%d]\n\n", __func__, nLength);
    uint8_t nSAT_CmdType = pOffset[nIndex + 4];
    OSI_LOGI(0x100045a5, "Fetch data nSAT_CmdType 0x%x\n\n", nSAT_CmdType);

    switch (nSAT_CmdType)
    {
    //SAT PROVIDE LOCAL INFORMATION
    case 0x26:
        nRet = AT_SAT_SpecialCMDProvideLocalInfo(pFetchData, nFetchDatalen, nSimID);
        break;
    //SAT SMS
    case 0x13:
        AT_SAT_SpecialCMDSendSMS(pFetchData, nFetchDatalen, nSimID);
        break;
#if 0
    //SAT USSD
    case 0x12:
        AT_SAT_SpecialCMDSendUSSD(pFetchData, nFetchDatalen, nSimID);
        break;

    case 0x05:
    {
        uint8_t nRspData[] = {0x81, 0x03, 0x02, 0x05, 0x00, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
        CFW_SatResponse(0xFF, 0x00, 0, nRspData, 0x0b, AT_ASYN_GET_DLCI(nSimID), nSimID);
        nRet = ERR_CONTINUE;
    }
    break;
    case 0x03:
    {
        uint8_t nRspData[] = {0x81, 0x03, 0x02, 0x03, 0x00, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
        CFW_SatResponse(0xFF, 0x00, 0, nRspData, 0x0b, AT_ASYN_GET_DLCI(nSimID), nSimID);
        nRet = ERR_CONTINUE;
    }
#endif
        break;
    //SAT CALL
    case 0x10:
    {
        if (gSATSpecialCMDData[nSimID])
        {
            AT_FREE(gSATSpecialCMDData[nSimID]);
            gSATSpecialCMDData[nSimID] = NULL;
        }

        gSATSpecialCMDData[nSimID] = (uint8_t *)AT_MALLOC(nFetchDatalen);
        SUL_ZeroMemory8(gSATSpecialCMDData[nSimID], nFetchDatalen);
        SUL_MemCopy8(gSATSpecialCMDData[nSimID], pFetchData, nFetchDatalen);
        gSATSpecialCMDDataLen[nSimID] = nFetchDatalen;
    }
        OSI_LOGI(0x100045a6, "AT_SAT_SpecialCMDCallSetup no process\n\n");
        //AT_SAT_SpecialCMDCallSetup(pFetchData, nFetchDatalen,nSimID);
        break;
    case 0x01:
    {
        AT_SAT_SpecialCMDRefresh(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x21:
    {
        AT_SAT_SpecialCMDDisplayTxt(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x3:
    {
        AT_SAT_SpecialCMDPollInterval(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x27:
    {
        nRet = AT_SAT_SpecialCMDTimerManagement(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    default:
        break;
    }
    if (ERR_CONTINUE == nRet)
        return 0x00;
    else
        return nSAT_CmdType;
}

uint8_t gDestAsciiData[CFW_SIM_COUNT][512] = {
    {
        0x00,
    },
};
uint8_t gDestAsciiDataSetUpMenu[CFW_SIM_COUNT][512] = {
    {
        0x00,
    },
};


uint8_t _GetStarSharp(uint8_t n)
{
    if (0x0a == n)
        return '*';
    else if (0x0b == n)
        return '#';
    else if (0x0c == n)
        return 'p';
    else if (0x0d == n)
        return 'w';
    else if (0x0e == n)
        return 'w';
    else if (0x0f == n)
        return 'w';
    else
        return (n + '0');
}

uint8_t *_GetSSString(uint8_t *pString, uint32_t nLen)
{
    uint8_t *p = (uint8_t *)malloc(2 * nLen + 2);
    if (p == NULL)
        return 0;
    SUL_ZeroMemory8(p, 2 * nLen + 2);

    uint32_t i = 0;
    uint32_t j = 0;
    for (; i < nLen; i++)
    {
        p[j] = _GetStarSharp(pString[i] & 0xF);
        p[j + 1] = _GetStarSharp((pString[i] >> 4) & 0xF);
        j = j + 2;
    }

    if (nLen)
    {
        if (p[2 * nLen - 1] == 'w')
            p[2 * nLen - 1] = 0x00;
    }

    OSI_LOGXI(OSI_LOGPAR_IS, 0x100045a7, "_GetUSSDString strlen %d %s\n", strlen(p), p);
    return p;
}

#define _IsAddressTag(nTag) ((nTag == 0x06) || (nTag == 0x86))
#define _IsSSStringTag(nTag) ((nTag == 0x09) || (nTag == 0x89))
#define _IsUSSDStringTag(nTag) ((nTag == 0x0A) || (nTag == 0x8A))

/*
SS string
Byte(s) Description Length
1   SS string tag   1
2 to (Y 1)+2    Length (X)  Y
(Y 1)+3     TON and NPI 1
(Y 1)+4 to (Y 1)+X+2    SS or USSD string   X - 1

*/
uint32_t _ProcessCallControlSS(uint8_t *pData, CFW_SIM_ID nSim)
{
    uint32_t nRet = ERR_SUCCESS;
    uint8_t *w = _GetSSString(&(pData[5]), pData[3] - 1);

    gATSATSendUSSDFlag[nSim] = true;
    nRet = CFW_SsSendUSSD(w, strlen(w), 131, 0, AT_ASYN_GET_DLCI(nSim), nSim);

    AT_FREE(w);
    w = NULL;

    OSI_LOGI(0x100045a8, "_ProcessCallControlSS nRet 0x%x", nRet);

    if (ERR_SUCCESS != nRet)
    {
        CFW_SatResponse(0x11, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSendUSSDFlag[nSim] = false;
    }

    return nRet;
}

/*
USSD string
Byte(s) Description Length
1   USSD string tag 1
2 to (Y-1)+2    Length (X)  Y
(Y-1)+3 Data coding scheme  1
(Y-1)+4 to (Y-1)+X+2    USSD string X-1
*/
uint32_t _ProcessCallControlUSSD(uint8_t *pData, CFW_SIM_ID nSim)
{
    uint32_t nRet = ERR_SUCCESS;
    //uint8_t*w     = _GetUSSDString( &( pData[5] ), pData[3] - 1 );

    gATSATSendUSSDFlag[nSim] = true;
    nRet = CFW_SsSendUSSD(&(pData[5]), pData[3], 3, pData[4], AT_ASYN_GET_DLCI(nSim), nSim);
    OSI_LOGI(0x100045a9, "_ProcessCallControlUSSD nRet 0x%x", nRet);
    if (ERR_SUCCESS != nRet)
    {
        CFW_SatResponse(0x12, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSendUSSDFlag[nSim] = false;
    }
    return nRet;
}

/*
Address
Byte(s) Description Length
1   Address tag 1
2 to (Y 1)+2    Length (X)  Y
(Y 1)+3 TON and NPI 1
(Y 1)+4 to (Y 1)+X+2    Dialling number string  X 1
*/
uint32_t _ProcessCallControlAdress(uint8_t *pData, CFW_SIM_ID nSim)
{
    uint32_t nRet = ERR_SUCCESS;
    gATSATSetupCallFlag[nSim] = true;

    CFW_DIALNUMBER nDialNumber;
    uint8_t nCCIndex = 0x00;

    nDialNumber.pDialNumber = AT_MALLOC(pData[3]);
    SUL_MemSet8(nDialNumber.pDialNumber, 0x00, pData[3]);
    SUL_MemCopy8(nDialNumber.pDialNumber, &(pData[5]), pData[3] - 1);
    nDialNumber.nDialNumberSize = pData[3] - 1;
    nDialNumber.nType = pData[4];

    nRet = CFW_CcInitiateSpeechCallEx(&nDialNumber, &nCCIndex, AT_ASYN_GET_DLCI(nSim), nSim);
    OSI_LOGI(0x100045aa, "_ProcessCallControlAdress nRet 0x%x", nRet);
    if (ERR_SUCCESS != nRet)
    {
        CFW_SatResponse(0x10, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSetupCallFlag[nSim] = false;
    }
    return nRet;
}

uint8_t _GetCallControlStringTag(uint8_t *pData)
{
    return (pData[2]);
}
#endif

#ifdef AT_STSF
//Start For STSF command
#define SIM_SAT_REFRESH_COM 0x01
#define SIM_SAT_SEND_SS_COM 0x11
#define SIM_SAT_SEND_USSD_COM 0x12
#define SIM_SAT_SEND_SMS_COM 0x13
#define SIM_SAT_PLAY_TONE_COM 0x20
//End For STSF command
#endif

#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT 4
#endif
#ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT - 1))
#endif

#define SIM_SAT_REFRESH_COM 0x01
#define SIM_SAT_MORE_TIME_COM 0x02
#define SIM_SAT_POLL_INTERVAL_COM 0x03
#define SIM_SAT_POLLING_OFF_COM 0x04
#define SIM_SAT_SETUP_EVENT_LIST_COM 0x05
#define SIM_SAT_CALL_SETUP_COM 0x10
#define SIM_SAT_SEND_USSD_COM 0x12
#define SIM_SAT_SEND_SMS_COM 0x13
#define SIM_SAT_PLAY_TONE_COM 0x20
#define SIM_SAT_DISPLAY_TEXT_COM 0x21
#define SIM_SAT_GET_INKEY_COM 0x22
#define SIM_SAT_GET_INPUT_COM 0x23
#define SIM_SAT_SELECT_ITEM_COM 0x24
#define SIM_SAT_SETUP_MENU_COM 0x25
#define SIM_SAT_PROVIDE_LOCAL_INFO_COM 0x26
#define SIM_SAT_TIMER_MANAGEMENT_COM 0x27
#define SIM_SAT_SETUP_IDLE_MODE_COM 0x28
#define SIM_SAT_OPEN_CHANNEL_COM 0x40
#define SIM_SAT_CLOSE_CHANNEL_COM 0x41
#define SIM_SAT_RECEIVE_DATA_COM 0x42
#define SIM_SAT_SEND_DATA_COM 0x43
#define SIM_SAT_GET_CHANNEL_STATUS_COM 0x44
#define SIM_SAT_EVENT_DOWNLOAD 0xD6
#define EVENT_DATA_AVAILABLE 0x09
#define EVENT_CHANNEL_STATUS 0x0A

extern bool gSimStage3Finish[CFW_SIM_END];
extern bool g_IMSI_INIT[CFW_SIM_COUNT];

extern uint32_t CFW_SimClose(uint16_t nUTI, CFW_SIM_ID nSimID);
void callback_gprs_active(atCommand_t *cmd, const osiEvent_t *event);
uint8_t callback_gprs_attach(atCommand_t *cmd, const osiEvent_t *event);
static void callback_nw_deregister(atCommand_t *cmd, const osiEvent_t *event);
static void callback_stk_response(atCommand_t *cmd, const osiEvent_t *event);

extern void CFW_IotICCIDRefresh(void);

#define BIP_MAX_BUFFER_SIZE 1500
#define BIP_RETRY_NUM 3
static uint8_t bip_retry_num = 0;
static uint8_t bip_cid = 0;
static uint8_t bip_protocol = IPPROTO_TCP;
static uint8_t bip_type = SOCK_STREAM;
static SOCKET bip_socket = 0;
static uint8_t bip_addrtype = 0;
static uint8_t bip_qualifier = 0;
static uint8_t bip_bearer_type = 0;
static uint16_t bip_buffer_size = BIP_MAX_BUFFER_SIZE;
static uint8_t bip_channel_staus[2] = {0};
#define BIP_IPv6 0x57
#define BIP_IPv4 0x21
union DestAddr {
    CFW_TCPIP_SOCKET_ADDR addr4;
    CFW_TCPIP_SOCKET_ADDR6 addr6;
    uint8_t type;
};

static union DestAddr bip_addr = {0};

enum bip_sm_status
{
    BIP_IDLE,
    BIP_ATTACHED,
    BIP_ACTIVED,
    BIP_SOCKET,
    BIP_CONNECTED
};
static uint8_t bip_status = BIP_IDLE; // status variable of state machine

static uint8_t bip_command = 0;
struct BIP_SEND_DATA
{
    uint8_t *pData;
    uint16_t nLength;
};
static struct BIP_SEND_DATA bip_sdata = {0, 0};

struct receive_buffer
{
    struct receive_buffer *next;
    uint16_t size;
    uint16_t offset;
    uint8_t buffer[0];
};
struct BIP_RECEVIE_DATA
{
    uint8_t buffer[238]; //used to send to uicc
    uint16_t total_len;
    struct receive_buffer *first;
};
static struct BIP_RECEVIE_DATA bip_rdata = {{0}, 0, 0};

struct BIP_USER
{
    uint8_t *user;
    uint8_t length;
};
static struct BIP_USER bip_user = {0, 0};

struct BIP_APN
{
    uint8_t *apn;
    uint8_t length;
};
static struct BIP_APN bip_apn = {0, 0};

struct BIP_PASSWD
{
    uint8_t *passwd;
    uint8_t length;
};
static struct BIP_PASSWD bip_passwd = {0, 0};
static CFW_GPRS_QOS bip_qos = {0};
static uint8_t bip_pdp_type = 0;
static uint8_t bip_oc_tr_status = 0;
void at_bip_clean_receive_buffer(void)
{
    struct receive_buffer *p = bip_rdata.first;
    while (p != NULL)
    {
        bip_rdata.first = p->next;
        free(p);
        p = bip_rdata.first;
    }
}

void at_bip_response(uint8_t nError, CFW_SIM_ID nSimID)
{
    uint16_t uti = cfwRequestNoWaitUTI();
    if (bip_command == SIM_SAT_OPEN_CHANNEL_COM)
    {
        struct CFW_SAT_CHANNEL channel_info;
        channel_info.nBearerType = bip_bearer_type;
        channel_info.nBufferSize = bip_buffer_size;
        if ((bip_status >= BIP_SOCKET) && (bip_type == CFW_TCPIP_SOCK_STREAM))
            channel_info.nStatus = (0x80 + bip_cid) << 0x08;
        else if ((bip_status >= BIP_ACTIVED) && (bip_type == CFW_TCPIP_SOCK_DGRAM))
            channel_info.nStatus = (0x80 + bip_cid) << 0x08;
        else
            channel_info.nStatus = 0x0005;
        if (channel_info.nBearerType == 0x02)
        {
            OSI_LOGI(0, ">nPrecedence = %d", bip_qos.nPrecedence);
            OSI_LOGI(0, ">nDelay = %d", bip_qos.nDelay);
            OSI_LOGI(0, ">nReliability = %d", bip_qos.nReliability);
            OSI_LOGI(0, ">nPeak = %d", bip_qos.nPeak);
            OSI_LOGI(0, ">nMean = %d", bip_qos.nMean);

            channel_info.Qos.nPrecedence = bip_qos.nPrecedence;
            channel_info.Qos.nDelay = bip_qos.nDelay;
            channel_info.Qos.nReliability = bip_qos.nReliability;
            channel_info.Qos.nPeak = bip_qos.nPeak;
            channel_info.Qos.nMean = bip_qos.nMean;
            channel_info.nPacketType = bip_pdp_type; //IP(Internal protocol type);
        }
        channel_info.nPacketType = 0x02; //IP(Internal protocol type);
        OSI_LOGI(0, "bip_status = %d, bip_type = %d", bip_status, bip_type);
        OSI_LOGI(0, "channel_info.nStatus = %d", channel_info.nStatus);
        OSI_LOGI(0, "channel_info.nBufferSize = %X", channel_info.nBufferSize);
        if (nError == 0)
            nError = bip_oc_tr_status;
        CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, nError, 0, &channel_info, sizeof(channel_info), uti, nSimID);
    }
    else if (bip_command == SIM_SAT_SEND_DATA_COM)
    {
        uint16_t size = bip_buffer_size - bip_sdata.nLength;
        uint8_t room = bip_buffer_size - bip_sdata.nLength;
        if (size > 255)
            room = 0xFF;
        CFW_SatResponse(SIM_SAT_SEND_DATA_COM, nError, 0, &room, 1, uti, nSimID);
    }
    else if (bip_command == SIM_SAT_CLOSE_CHANNEL_COM)
    {
        CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, nError, 0, 0, 0, uti, nSimID);
    }
    else if (CFW_SatGetCurCMD(nSimID) == SIM_SAT_REFRESH_COM)
        CFW_SatResponse(SIM_SAT_REFRESH_COM, nError, 0, 0, 0, uti, nSimID);
}

void at_bip_reset(void)
{
    if (bip_apn.apn != 0)
    {
        free(bip_apn.apn);
        bip_apn.apn = 0;
        bip_apn.length = 0;
    }
    if (bip_user.user != 0)
    {
        free(bip_user.user);
        bip_user.user = 0;
        bip_user.length = 0;
    }
    if (bip_passwd.passwd != 0)
    {
        free(bip_passwd.passwd);
        bip_passwd.passwd = 0;
        bip_passwd.length = 0;
    }

    if (bip_sdata.pData != NULL)
    {
        free(bip_sdata.pData);
        bip_sdata.pData = 0;
        bip_sdata.nLength = 0;
    }
    at_bip_clean_receive_buffer();
    bip_rdata.total_len = 0;

    bip_protocol = IPPROTO_TCP;
    bip_type = SOCK_STREAM;
    bip_buffer_size = 0;
    bip_channel_staus[0] = 0;
    bip_channel_staus[1] = 0;
    bip_addrtype = 0;
    bip_qualifier = 0;

    bip_retry_num = 0;
    memset(&bip_addr.addr6, 0, sizeof(CFW_TCPIP_SOCKET_ADDR6));
}

void callback_tcpip(const osiEvent_t *event)
{
    OSI_LOGI(0, "callback_tcpip: bip_cid = %d", bip_cid);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;

    OSI_LOGI(0, "======cfw_event->nEventId=%X", cfw_event->nEventId);
    OSI_LOGI(0, "======cfw_event->nParam1=%X", cfw_event->nParam1);
    OSI_LOGI(0, "======cfw_event->nParam2=%X", cfw_event->nParam2);
    OSI_LOGI(0, "======cfw_event->nFlag=%d, nSim = %d", cfw_event->nFlag, nSim);
    OSI_LOGI(0, "======cfw_event->nType=%X", cfw_event->nType);

    uint8_t socket = (uint8_t)cfw_event->nParam1;
    uint16_t uti = cfwRequestNoWaitUTI();
    OSI_LOGI(0, "pParam->nEventId = %d", cfw_event->nEventId);
    switch (cfw_event->nEventId)
    {
    case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
        if (bip_status != BIP_CONNECTED)
        {
            at_bip_response(0, nSim);
#if 0
            uint32_t event_list = CFW_SatGetEventList(nSim);
            if (event_list & (1 << EVENT_CHANNEL_STATUS))
            {
                uint8_t channel_status[2];
                channel_status[0] = bip_cid | 0x80;
                channel_status[1] = 0;
                CFW_SatResponse(SIM_SAT_EVENT_DOWNLOAD, 0, EVENT_CHANNEL_STATUS, channel_status, 2, uti, nSim);
            }
#endif
            OSI_LOGI(0, "Socket connect OK, bip_command = %d", bip_command);
            bip_status = BIP_CONNECTED;
        }
        OSI_LOGI(0, "bip_status = %d, bip_sdata.nLength = %d", bip_status, bip_sdata.nLength);
        if ((bip_type == CFW_TCPIP_SOCK_STREAM) && (bip_sdata.nLength != 0))
        {
            int32_t retval = CFW_TcpipSocketSend(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0);
            if (retval == SOCKET_ERROR)
            {
                OSI_LOGI(0, "Socket SEND DATA ERROR!");
                at_bip_response(0x21, nSim);
            }
        }
        break;
    case EV_CFW_TCPIP_ERR_IND:
    case EV_CFW_TCPIP_CLOSE_IND:
        if (bip_socket != 0)
        {
            CFW_TcpipSocketClose(bip_socket);
            //bip_protocol = IPPROTO_TCP;
            //bip_type = SOCK_STREAM;
            //bip_socket = 0;
            //at_bip_response(0x20, nSim);
#if 0
            uint32_t event_list = CFW_SatGetEventList(nSim);
            OSI_LOGI(0, "event_list = 0x%X, condition = 0x%X", event_list, event_list & (1 << EVENT_CHANNEL_STATUS));
            if (event_list & (1 << EVENT_CHANNEL_STATUS))
            {
                uint8_t channel_status[2];
                channel_status[0] = bip_cid;
                channel_status[1] = 0x05;
                CFW_SatResponse(SIM_SAT_EVENT_DOWNLOAD, 0, EVENT_CHANNEL_STATUS, channel_status, 2, uti, nSim);
            }
#endif
            bip_status = BIP_ACTIVED;
        }
        break;
    case EV_CFW_TCPIP_REV_DATA_IND:
    {
        OSI_LOGI(0, "socket = %d, bip_socket = %d", socket, bip_socket);
        uint16_t uDataSize = (uint16_t)cfw_event->nParam2;
        OSI_LOGI(0, "uDataSize = %d, buffer len = %d", uDataSize, bip_rdata.total_len);

        if (bip_rdata.total_len + uDataSize > BIP_MAX_BUFFER_SIZE)
        {
            OSI_LOGI(0, "no more memory to store tcp/udp data(%d, %d)", uDataSize + bip_rdata.total_len, BIP_MAX_BUFFER_SIZE);
            return;
        }
        uint16_t size = sizeof(struct receive_buffer) + uDataSize;
        struct receive_buffer *package = (struct receive_buffer *)malloc(size);
        if (package == NULL)
        {
            OSI_LOGI(0, "no more memory, malloc memory failed(%d)", size);
            return;
        }
        OSI_LOGI(0, "uDataSize store into 0x%X", package);
        uint16_t offset = 0;
        do
        {
            uint32_t ret = CFW_TcpipSocketRecv(bip_socket, package->buffer + offset, uDataSize - offset, 0);
            if (SOCKET_ERROR == ret)
            {
                OSI_LOGI(0, "EV_CFW_TCPIP_REV_DATA_IND, CFW_TcpipSocketRecv error");
                if (offset != 0) //previous operation has got some data, send event to UICC
                    break;
                free(package);
                return;
            }
            else if (0 == ret) //no more data to be read
                break;

            OSI_LOGI(0, "ret = %d, offset = %d, total len = %d", ret, offset, bip_rdata.total_len);
            //CSW_TC_MEMBLOCK(10, package->buffer + offset, ret, 16);
            offset += ret;
        } while (uDataSize - offset > 0);
        if (offset == 0)
        {
            free(package);
            return;
        }
        package->next = NULL;
        package->size = offset;
        package->offset = 0;
        struct receive_buffer **p = &bip_rdata.first;
        while (*p != NULL)
            p = &(*p)->next;
        *p = package;
        bip_rdata.total_len += offset;
        OSI_LOGI(0, "uDataSize store into 0x%X", package);
        OSI_LOGI(0, "package->size = %d, total size = %d", package->size, bip_rdata.total_len);
        //CSW_TC_MEMBLOCK(10, package->buffer, package->size, 16);

        if (socket == bip_socket)
        {
            uint32_t event_list = CFW_SatGetEventList(nSim);
            OSI_LOGI(0, "event_list = %X", event_list);
            if (event_list & (1 << EVENT_DATA_AVAILABLE))
            {
                //string format: two byte channel status, one byte channel data length
                uint8_t event_data[3];
                event_data[0] = bip_cid | 0x80;
                event_data[1] = 0;
                event_data[2] = bip_rdata.total_len > 0xFF ? 0xFF : (bip_rdata.total_len & 0xFF);
                OSI_LOGI(0, "event_data = %X%X%X", event_data[0], event_data[1], event_data[2]);
                CFW_SatResponse(SIM_SAT_EVENT_DOWNLOAD, 0, EVENT_DATA_AVAILABLE, event_data, 3, uti, nSim);
            }
            OSI_LOGI(0, "++++++++++++ BIP receive data ++++++++++++");
            OSI_LOGI(0, "bip_rdata.first = %X", bip_rdata.first);
            OSI_LOGI(0, "bip_rdata.total_len = %d", bip_rdata.total_len);
            if (bip_rdata.first != NULL)
            {
                OSI_LOGI(0, "first node size = %d", bip_rdata.first->size);
                OSI_LOGI(0, "first node offset = %d", bip_rdata.first->offset);
            }
        }
    }
    break;
    case EV_CFW_TCPIP_SOCKET_SEND_RSP:
    {
        OSI_LOGI(0, "============ BIP recieve EV_CFW_TCPIP_SOCKET_SEND_RSP ============");
    }
    break;
    case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
        OSI_LOGI(0, "BIP recieve TCPIP_SOCKET_CLOSE_RSP, bip_socket = %d, bip_bearer_type = %d", bip_socket, bip_bearer_type);
        if (bip_bearer_type == OPEN_CHANNEL_BEARER_GPRS)
        {
            OSI_LOGI(0, "GPRS Bearer, deactivate gprs!");
            uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_active, NULL);
            if (ERR_SUCCESS != CFW_GprsAct(CFW_GPRS_DEACTIVED, bip_cid, uti, nSim))
            {
                at_bip_response(0x21, nSim);
            }
            OSI_LOGI(0, "GPRS Deactived!, bip_cid = %d", bip_cid);
        }
        else
            at_bip_response(0, nSim);
        bip_bearer_type = 0;
        bip_status = BIP_ACTIVED;
        break;
    default:
        OSI_LOGI(0, "receive Event ID = %X", cfw_event->nEventId);
    }
    return;
}

void at_bip_channel_status_event(uint8_t nSimId)
{
    OSI_LOGI(0, "receive EV_CFW_GPRS_CXT_DEACTIVE_IND, download channel status to sim");
#if 1
    bip_status = BIP_ATTACHED;
    uint32_t event_list = CFW_SatGetEventList(nSimId);
    OSI_LOGI(0, "event_list = %X, bip_cid = %d", event_list, bip_cid);
    if (event_list & (1 << EVENT_CHANNEL_STATUS))
    {
        uint8_t channel_status[2];
        channel_status[0] = bip_cid;
        channel_status[1] = 0x05;
        uint16_t uti = cfwRequestNoWaitUTI();
        CFW_SatResponse(SIM_SAT_EVENT_DOWNLOAD, 0, EVENT_CHANNEL_STATUS, channel_status, 2, uti, nSimId);
    }
#endif
    return;
}

struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);
void TCPIP_netif_create(uint8_t nCid, uint8_t nSimId);
void callback_gprs_active(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;

    if (EV_CFW_GPRS_ACT_RSP == cfw_event->nEventId)
    {
        OSI_LOGI(0, "receive EV_CFW_GPRS_ACT_RSP event, cfw_event->nType = %d", cfw_event->nType);

        if (cfw_event->nType == CFW_GPRS_ACTIVED)
        {
            OSI_LOGI(0, "call TCPIP_netif_create");

            TCPIP_netif_create(bip_cid, nSim);
            OSI_LOGI(0, "default ip = %X", (netif_default->ip_addr));
            //CFW_GprsHostAddress
            uint8_t IPAddress[21] = {0};
            uint8_t nLength = 21;
            uint8_t nIPType = 0;

            uint32_t retval = CFW_GprsHostAddress(IPAddress, &nLength, &nIPType, bip_cid, nSim);
            if ((retval != ERR_SUCCESS) || (nLength == 0))
            {
                OSI_LOGI(0, "BIP netif ERROR!");
                goto BIP_SOCKET_ERROR;
            }

            OSI_LOGI(0, "GPRS Actived -- Done");
            uint8_t domain = AF_INET;
            if (bip_addrtype == BIP_IPv6)
                domain = AF_INET6;
            bip_socket = CFW_TcpipSocketEX(domain, bip_type, bip_protocol, (osiCallback_t)callback_tcpip, 0);
            if (bip_socket == -1)
            {
                OSI_LOGI(0, "Create socket ERROR(%d)!", bip_socket);
            BIP_SOCKET_ERROR:
                at_bip_response(0x20, nSim);
                return;
            }
            bip_status = BIP_SOCKET;
            OSI_LOGI(0, "1bip_socket = %d!", bip_socket);
            if ((bip_protocol == IPPROTO_UDP) || ((bip_apn.apn != NULL) && (bip_apn.length != 0)))
            {
                OSI_LOGI(0, "nLength = %d, nIPType = %d, sizeof(ip4_addr_t) = %d, sizeof(ip6_addr_t) = %d", nLength, nIPType, sizeof(ip4_addr_t), sizeof(ip6_addr_t));
                uint32_t *host = (uint32_t *)(IPAddress);
#if 1
                if ((nIPType & CFW_IPV4) != 0)
                    OSI_LOGXI(OSI_LOGPAR_M, 0, "IPv4 :%*s", sizeof(ip4_addr_t), host);
                if ((nIPType & CFW_IPV6) != 0)
                {
                    if (nIPType == CFW_IPV6)
                        OSI_LOGXI(OSI_LOGPAR_M, 0, "IPv6 :%*s", sizeof(ip6_addr_t), host);
                    else
                        OSI_LOGXI(OSI_LOGPAR_M, 0, "IPv6 :%*s", sizeof(ip6_addr_t), host + 4);
                }
#endif
                OSI_LOGI(0, " ---------2------- ");

                if ((bip_addrtype == BIP_IPv6) && (nIPType == CFW_IPV4))
                {
                    OSI_LOGI(0, "IP addresses don't matched(%d,%d)!", bip_addrtype, nIPType);
                    goto BIP_SOCKET_ERROR;
                }
                if ((bip_addrtype == BIP_IPv4) && (nIPType == CFW_IPV6))
                {
                    OSI_LOGI(0, "IP addresses don't matched(%d,%d)!", bip_addrtype, nIPType);
                    goto BIP_SOCKET_ERROR;
                }

                OSI_LOGI(0, " ---------3------- ");
                if (bip_addrtype == BIP_IPv4)
                {
                    CFW_TCPIP_SOCKET_ADDR local_addr = {0};
                    local_addr.sin_family = AF_INET;
                    local_addr.sin_addr.s_addr = *host;
                    local_addr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
                    OSI_LOGXI(OSI_LOGPAR_I, 0, "IPv4 address: %X", local_addr.sin_addr.s_addr);
                    retval = CFW_TcpipSocketBind(bip_socket, &local_addr, sizeof(CFW_TCPIP_SOCKET_ADDR));
                }
                else if (bip_addrtype == BIP_IPv6)
                {
                    CFW_TCPIP_SOCKET_ADDR6 local_addr = {0};
                    local_addr.sin6_family = AF_INET6;
                    if (nIPType == CFW_IPV6)
                        memcpy(local_addr.sin6_addr.un.u32_addr, IPAddress, 16);
                    else
                        memcpy(local_addr.sin6_addr.un.u32_addr, IPAddress + 4, 16);
                    local_addr.sin6_len = sizeof(CFW_TCPIP_SOCKET_ADDR6);
                    OSI_LOGXI(OSI_LOGPAR_I, 0, "IPv6 address: %X", local_addr.sin6_addr.un.u32_addr);
                    retval = CFW_TcpipSocketBind(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&local_addr, sizeof(CFW_TCPIP_SOCKET_ADDR6));
                }

                OSI_LOGI(0, " ---------5------- ");
                if (SOCKET_ERROR == retval)
                {
                    OSI_LOGI(0, "Socket bind ERROR(%d)!", retval);
                    goto BIP_SOCKET_ERROR;
                }
                if (bip_protocol == IPPROTO_UDP)
                {
                    at_bip_response(0, nSim);
                    return;
                }
                OSI_LOGI(0, " ---------6------- ");
            }
            if (bip_protocol == CFW_TCPIP_IPPROTO_TCP)
            {
                int32_t retval = 0;
                if (bip_addrtype == BIP_IPv4)
                    retval = CFW_TcpipSocketConnect(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr4, sizeof(bip_addr.addr4));
                else
                    retval = CFW_TcpipSocketConnect(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr6, sizeof(bip_addr.addr6));
                if (retval == -1)
                {
                    OSI_LOGI(0, "Get OPEN CHANNEL ERROR!%d", netif_default);
                    at_bip_response(0x21, nSim);
                    return;
                }
                //bip_status = BIP_CONNECTED;
                OSI_LOGI(0, "3 wait connect OK, retval = %d, bip_status = %d", retval, bip_status);
            }
            else
            {
                at_bip_response(0x21, nSim);
                OSI_LOGI(0, "Response OPEN CHANNEL!");
            }
        }
        else if (cfw_event->nType == CFW_GPRS_DEACTIVED)
        {
            if (bip_cid != 0)
            {
                CFW_ReleaseCID(bip_cid, nSim);
                bip_cid = 0;
            }
#if 1
            if (CFW_SatGetCurCMD(nSim) == SIM_SAT_CLOSE_CHANNEL_COM)
            {
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, 0, 0, 0, 0, uti, nSim);
            }
#else
            OSI_LOGI(0, "GPRS deattached!");
            uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_attach, NULL);
            if (ERR_SUCCESS != CFW_GprsAtt(CFW_GPRS_DETACHED, uti, nSim))
            {
                OSI_LOGI(0, "Detach failed!");
                at_bip_response(0, nSim);
            }
#endif
            bip_status = BIP_ATTACHED;
        }
        else
        {
            if (bip_status == BIP_ATTACHED) //fail to activate
            {
                if (bip_cid != 0)
                {
                    CFW_ReleaseCID(bip_cid, nSim);
                    bip_cid = 0;
                }
            }
            at_bip_response(0x20, nSim);
            OSI_LOGI(0, "GPRS Actived failed!");
        }
    }

    OSI_LOGI(0, "GPRS:, bip_status = %d, bip_cid = %d!", bip_status, bip_cid);
    OSI_LOGI(0, "GPRS proc, bip_cid = %d", bip_cid);
}

void at_bipSetQos(CFW_SIM_ID nSimID)
{
    CFW_GPRS_QOS Qos;
    memset(&Qos, 0x00, sizeof(CFW_GPRS_QOS));

    Qos.nPrecedence = bip_qos.nPrecedence;
    Qos.nDelay = bip_qos.nDelay;
    Qos.nReliability = bip_qos.nReliability;
    Qos.nPeak = bip_qos.nPeak;
    Qos.nMean = bip_qos.nMean;
    CFW_GprsSetReqQos(bip_cid, &Qos, nSimID);
}

bool at_SetPdpCtx(CFW_SIM_ID nSimID)
{
    if ((bip_apn.length == 0) && (bip_passwd.length == 0) && (bip_user.length == 0))
    {
        OSI_LOGI(0, "BipSetPdPCtx return true!");
        return true;
    }
#if 0
    CFW_GPRS_PDPCONT_INFO bip_pdp;
    memset(&bip_pdp, 0, sizeof(CFW_GPRS_PDPCONT_INFO));
    bip_pdp.nApnUserSize = bip_user.length;
    bip_pdp.pApnUser = bip_user.user;

    bip_pdp.nApnPwdSize = bip_passwd.length;
    bip_pdp.pApnPwd = bip_passwd.passwd;

    bip_pdp.nApnSize = bip_apn.length;
    bip_pdp.pApn = bip_apn.apn;

    bip_pdp.nPdpAddrSize = 0;
    bip_pdp.pPdpAddr = 0;
    bip_pdp.nPdpType = 3; // 1 ipv4, 2 ipv6, 3 ipv4v6

    bip_pdp.nDComp = 0;
    bip_pdp.nHComp = 0;
    uint32_t retval = CFW_GprsSetPdpCxtV2(bip_cid, &bip_pdp, nSimID);
#else

    CFW_GPRS_PDPCONT_INFO_V2 bip_pdp;
    memset(&bip_pdp, 0, sizeof(CFW_GPRS_PDPCONT_INFO_V2));
    bip_pdp.nApnUserSize = bip_user.length;
    memcpy(bip_pdp.pApnUser, bip_user.user, bip_pdp.nApnUserSize);

    bip_pdp.nApnPwdSize = bip_passwd.length;
    memcpy(bip_pdp.pApnPwd, bip_passwd.passwd, bip_pdp.nApnPwdSize);

    bip_pdp.nApnSize = bip_apn.length;
    memcpy(bip_pdp.pApn, bip_apn.apn, bip_pdp.nApnSize);

    bip_pdp.nPdpAddrSize = 0;
    bip_pdp.nPdpType = 3; // 1 ipv4, 2 ipv6, 3 ipv4v6

    bip_pdp.nDComp = 0;
    bip_pdp.nHComp = 0;
    uint32_t retval = CFW_GprsSetPdpCxtV2(bip_cid, &bip_pdp, nSimID);

#endif
    if (retval == ERR_SUCCESS)
    {
        OSI_LOGI(0, "CFW_GprsSetPdpCtx OK(%d)", retval);
        return true;
    }
    else
    {
        OSI_LOGI(0, "CFW_GprsSetPdpCtx false(%d)", retval);
        return false;
    }
}

uint8_t callback_gprs_attach(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;

    if (EV_CFW_GPRS_ATT_RSP == cfw_event->nEventId)
    {
        OSI_LOGI(0, "receive EV_CFW_GPRS_ATT_RSP event, cfw_event->nType = %d", cfw_event->nType);

        if (cfw_event->nType == 1)
        {
            if (bip_cid == 0)
            {
                if (ERR_SUCCESS != CFW_GetFreeCID(&bip_cid, nSim))
                {
                    at_bip_response(0x20, nSim);
                    OSI_LOGI(0, "GPRS Actived failed(get cid error)!");
                    return ERR_SUCCESS;
                }
            }
            OSI_LOGI(0, "cid = %d", bip_cid);
            at_bipSetQos(nSim);
            at_SetPdpCtx(nSim);
            uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_active, NULL);
            int32_t retval = CFW_GprsAct(CFW_GPRS_ACTIVED, bip_cid, uti, nSim);
            if (ERR_SUCCESS != retval)
            {
                at_bip_response(0x20, nSim);
                OSI_LOGI(0, "GPRS Actived failed!");
                return ERR_SUCCESS;
            }

            OSI_LOGI(0, "GPRS Attached, to active GPRS");
        }
        else if (cfw_event->nType == 0)
        {
            if (CFW_SatGetCurCMD(nSim) == SIM_SAT_REFRESH_COM)
            {
                OSI_LOGI(0, "GPRS deattached, to NwDeRegister!");
                uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_nw_deregister, NULL);
                if (ERR_SUCCESS != CFW_NwDeRegister(uti, nSim))
                    at_bip_response(0x20, nSim);
            }
            else
            {
                OSI_LOGI(0, "Response to CLOSE CHANNEL!");
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, 0, 0, 0, 0, uti, nSim);
            }
            bip_status = BIP_IDLE;
        }
        else // if(CFWEvent.nType == 0xF0)
        {
            at_bip_response(0x20, nSim);
            //bip_status = BIP_IDLE;
            OSI_LOGI(0, "GPRS attached failed!");
        }
    }
    OSI_LOGI(0, "GPRS: bip_status = %d, bip_cid = %d!", bip_status, bip_cid);
    return ERR_SUCCESS;
}

uint8_t at_bip_process(uint8_t nCmdID, CFW_SIM_ID nSimID)
{
    uint8_t nSim = nSimID;
    uint16_t uti = 0;
    OSI_LOGI(0, "AT_BipProcess: bip_status = %d, bip_cid = %d", bip_status, bip_cid);
    switch (bip_status)
    {
    BIP_SOCKET_RETRY:
    case BIP_IDLE:
    {
        uint8_t nAttState = 0;
        CFW_GetGprsAttState(&nAttState, nSim);
        if (nAttState != CFW_GPRS_ATTACHED)
        {
            uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_attach, NULL);
            CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim);
            break;
        }
    }
    //break;  needn't break;
    case BIP_ATTACHED:
    {
        OSI_LOGI(0, "case BIP_ATTACHED");
        if (bip_apn.length == 0)
        {
            uint8_t active_status = 0;
            uint8_t MAX_CID_NUM = 7;
            for (uint8_t i = 1; i < MAX_CID_NUM + 1; i++)
            {
                if (CFW_GetGprsActState(i, &active_status, nSim) == ERR_SUCCESS)
                {
                    if (active_status == CFW_GPRS_ACTIVED)
                    {
                        bip_cid = i;
                        OSI_LOGI(0, "GPRS has been Actived! cid = %d", i);
                        break;
                    }
                }
            }
        }
        OSI_LOGI(0, "case BIP_ATTACHED: bip_cid = %d", bip_cid);

        if (bip_cid == 0)
        {
            if (ERR_SUCCESS != CFW_GetFreeCID(&bip_cid, nSim))
            {
                uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(nCmdID, 0x20, 0, 0, 0, uti, nSim);
                OSI_LOGI(0, "GPRS Actived failed!");
                return -1;
            }
            OSI_LOGI(0, "BIP cid:%d", bip_cid);

            at_SetPdpCtx(nSim);
            uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_active, NULL);
            if (ERR_CME_OPERATION_NOT_ALLOWED == CFW_GprsAct(CFW_GPRS_ACTIVED, bip_cid, uti, nSim))
            {
                uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_attach, NULL);
                CFW_GprsAtt(CFW_GPRS_ATTACHED, uti, nSim);
                return ERR_SUCCESS;
            }

            bip_status = BIP_ATTACHED;
            OSI_LOGI(0, "GPRS Attached, to active GPRS");
            OSI_LOGI(0, "return ERR_SUCCESS;");
            return ERR_SUCCESS;
        }
        else
            bip_status = BIP_ACTIVED;
        OSI_LOGI(0, "case BIP_ATTACHED end");
    }
    case BIP_ACTIVED:
    {
        uint8_t domain = AF_INET;
        if (bip_addrtype == BIP_IPv6)
            domain = AF_INET6;
        bip_socket = CFW_TcpipSocketEX(domain, bip_type, bip_protocol, (osiCallback_t)callback_tcpip, 0);
        if (bip_socket == -1)
        {
            OSI_LOGI(0, "Create socket ERROR(%d)!", bip_socket);
        BIP_SOCKET_ERROR:
            uti = cfwRequestNoWaitUTI();
            CFW_SatResponse(nCmdID, 0x20, 0, 0, 0, uti, nSim);
            return -1;
        }
        bip_status = BIP_SOCKET;
        OSI_LOGI(0, "2bip_socket = %d!", bip_socket);
    }
    //break;        //needn't break
    case BIP_SOCKET:
    {
        if ((bip_protocol == IPPROTO_UDP) || ((bip_apn.apn != NULL) && (bip_apn.length != 0)))
        {
            OSI_LOGI(0, "case BIP_SOCKET");
            struct netif *bit_netif = getGprsNetIf(nSim, bip_cid);
            if (bit_netif == NULL)
            {
                OSI_LOGI(0, "BIP netif ERROR!");
                CFW_TcpipSocketClose(bip_socket);
                if ((bip_qualifier & 0x02) == 0)
                    goto BIP_SOCKET_ERROR;
                else
                {
                    bip_status = BIP_IDLE;
                    if (bip_retry_num++ < BIP_RETRY_NUM)
                        goto BIP_SOCKET_RETRY;
                    else
                    {
                        at_bip_response(0, nSim);
                        return -1;
                    }
                }
            }
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", sizeof(ip6_addr_t), (uint8_t *)&bit_netif->ip6_addr);
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", sizeof(ip4_addr_t), (uint8_t *)&bit_netif->ip_addr);
            if ((bip_addrtype == BIP_IPv6) && (bit_netif->pdnType == CFW_GPRS_PDP_TYPE_IP))
            {
                OSI_LOGI(0, "IP addresses don't matched(%d,%d)!", bip_addrtype, bit_netif->pdnType);
                goto BIP_SOCKET_ERROR;
            }
            if ((bip_addrtype == BIP_IPv4) && (bit_netif->pdnType == CFW_GPRS_PDP_TYPE_IPV6))
            {
                OSI_LOGI(0, "IP addresses don't matched(%d,%d)!", bip_addrtype, bit_netif->pdnType);
                goto BIP_SOCKET_ERROR;
            }
            int8_t retval = 0;
            if (bip_addrtype == BIP_IPv4)
            {
                CFW_TCPIP_SOCKET_ADDR local_addr = {0};
                local_addr.sin_family = AF_INET;
                local_addr.sin_addr.s_addr = bit_netif->ip_addr.u_addr.ip4.addr;
                local_addr.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
                OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", sizeof(CFW_TCPIP_SOCKET_ADDR), (uint8_t *)&local_addr);
                retval = CFW_TcpipSocketBind(bip_socket, &local_addr, sizeof(CFW_TCPIP_SOCKET_ADDR));
            }
            else if (bip_addrtype == BIP_IPv6)
            {
#if LWIP_IPV6
                CFW_TCPIP_SOCKET_ADDR6 local_addr = {0};
                local_addr.sin6_family = AF_INET6;
                memcpy(local_addr.sin6_addr.un.u32_addr, &bit_netif->ip_addr.u_addr.ip6.addr, 16);
                local_addr.sin6_len = sizeof(CFW_TCPIP_SOCKET_ADDR6);
                OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", sizeof(CFW_TCPIP_SOCKET_ADDR), (uint8_t *)&local_addr);
                retval = CFW_TcpipSocketBind(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&local_addr, sizeof(CFW_TCPIP_SOCKET_ADDR6));
#else
                at_bip_response(0x30, nSim);
                return -1;
#endif
            }

            if (SOCKET_ERROR == retval)
            {
                OSI_LOGI(0, "Socket bind ERROR(%d)!", retval);
                goto BIP_SOCKET_ERROR;
            }
        }

        int32_t retval = 0;
        if (bip_protocol == CFW_TCPIP_IPPROTO_TCP)
        {
            if (bip_addrtype == BIP_IPv4)
                retval = CFW_TcpipSocketConnect(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr4, sizeof(bip_addr.addr4));
            else
                retval = CFW_TcpipSocketConnect(bip_socket, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr6, sizeof(bip_addr.addr6));
            if (retval == -1)
            {
                OSI_LOGI(0, "Socket connect ERROR(%d)!", BIP_ACTIVED);
                at_bip_response(0x21, nSim);
                return -1;
            }
            //bip_status = BIP_CONNECTED;
            OSI_LOGI(0, "1 wait connect OK, retval = %d, bip_status = %d", retval, bip_status);
        }
        else
        {
            if (bip_addrtype == BIP_IPv4)
                retval = CFW_TcpipSocketSendto(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr4, sizeof(bip_addr.addr4));
            else
                retval = CFW_TcpipSocketSendto(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr6, sizeof(bip_addr.addr6));
            OSI_LOGI(0, "retval = %d, bip_socket = %d, bip_addrtype = %X", retval, bip_socket, bip_addrtype);
            if (retval == SOCKET_ERROR)
            {
                OSI_LOGI(0, "Socket SEND DATA ERROR!");
                at_bip_response(0x21, nSim);
                return -1;
            }
            else if (retval != bip_sdata.nLength)
            {
                memcpy(bip_sdata.pData, bip_sdata.pData + retval, bip_sdata.nLength - retval);
                bip_sdata.nLength -= retval;
                OSI_LOGI(0, "------ UDP ------");
                //CSW_TC_MEMBLOCK(10, bip_sdata.pData, bip_sdata.nLength, 16);
            }
            else
            {
                bip_sdata.nLength = 0;
            }

            OSI_LOGI(0, "Response SEND DATA, bip_socket = %d!", bip_socket);
            at_bip_response(0, nSim);
        }
    }
    break;
    case BIP_CONNECTED:
    {
        if ((bip_type == CFW_TCPIP_SOCK_STREAM) && (bip_sdata.nLength != 0))
        {
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", bip_sdata.nLength, bip_sdata.pData);
            int32_t retval = CFW_TcpipSocketSend(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0);
            if (retval == SOCKET_ERROR)
            {
                OSI_LOGI(0, "Socket SEND DATA ERROR!");
                at_bip_response(0x21, nSim);
                return -1;
            }
            else if (retval != bip_sdata.nLength)
            {
                memcpy(bip_sdata.pData, bip_sdata.pData + retval, bip_sdata.nLength - retval);
                bip_sdata.nLength -= retval;
                OSI_LOGI(0, "------ TCP ------");
                //CSW_TC_MEMBLOCK(10, bip_sdata.pData, bip_sdata.nLength, 16);
            }
            else
            {
                bip_sdata.nLength = 0;
            }
        }
    }
    default:
        break;
    }
    return ERR_SUCCESS;
}

uint8_t auto_process = 0xFF;

uint32_t CFW_SimInitStage1(CFW_SIM_ID nSimID);
void callback_timer_expiration_rsp(atCommand_t *cmd, const osiEvent_t *event);
//osiTimer_t *stk_tms_timer[NUMBER_OF_SIM][8] = {NULL}; //timre management
struct stk_tms_info
{
    osiTimer_t *tms_timer;
    int64_t tms_value;
    uint32_t timerout;
    uint8_t sim_id;
    uint8_t timer_id;
} stk_tms[NUMBER_OF_SIM][8];

uint8_t tms_last_timer_id = 0;
void stk_tms_timeout(void *param)
{
    struct stk_tms_info *tms = (struct stk_tms_info *)param;

    OSI_LOGI(0, "Sat Timer management: timer id = %d(SimID = %d)", tms->timer_id, tms->sim_id);
    OSI_LOGXI(OSI_LOGPAR_D, 0, "Timer management: orignal time value = %lld", tms->tms_value);
    OSI_LOGI(0, "Timer management: tms->timerout = %X", tms->timerout);
    OSI_LOGI(0, "Timer management: tms_timer= %X", tms->tms_timer);

    int64_t time_value = (int64_t)osiUpTime();
    OSI_LOGXI(OSI_LOGPAR_D, 0, "Current time value = %lld", time_value);

    time_value -= tms->tms_value;
    OSI_LOGXI(OSI_LOGPAR_D, 0, "Timer management: time elapsed = %lld", time_value);
    time_value = time_value / 1000;
    uint8_t TimerManagement[4];
    TimerManagement[0] = tms->timer_id;
    TimerManagement[1] = time_value / 3600;
    uint32_t remain = time_value % 3600;
    TimerManagement[2] = remain / 60;
    TimerManagement[3] = remain % 60;
    OSI_LOGI(0, "Timer management: Hour = %d, Minute = %d, Second = %d", TimerManagement[1], TimerManagement[2], TimerManagement[3]);
#define SIM_SAT_TIMER_EXPIRATION 0xD7
    uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_timer_expiration_rsp, NULL);
    CFW_SatResponse(SIM_SAT_TIMER_EXPIRATION, 0, 0, TimerManagement, 4, uti, tms->sim_id);
    tms_last_timer_id = tms->timer_id;
    return;
}

uint32_t CFW_SimInitStage3(CFW_SIM_ID nSimID);
static void callback_sim_close(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;

    OSI_LOGI(0, "receive EV_CFW_SIM_CLOSE_RSP...");
    uint8_t nCmd = 0;
    uint8_t nQualifier = 0;

    uint16_t uti = cfwRequestNoWaitUTI();
    if (CFW_SatGetCurCMD(nSim) == SIM_SAT_REFRESH_COM)
    {
        if (false == CFW_SatGetCurrentCmd(&nCmd, &nQualifier, nSim))
        {
            CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x32, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "Current proactive command = %d, qualifier = %d", nCmd, nQualifier);
        SimSendStatusReq(0x02, nSim);
        switch (nQualifier)
        {
        case 0x04:
        {
            OSI_LOGI(0, "CFW_ResetDevice ...");
            CFW_SimInit(1, nSim);
            break;
        }
        case 0x05:
        {
            uint8_t aid[18];
            uint8_t length = 18;
            OSI_LOGI(0, "Reselect Application ...");
            CFW_GetUsimAID(aid, &length, nSim);
            uint32_t retval = SimSelectApplicationReqV2(aid, length, 0, nSim);
            if (ERR_SUCCESS != retval)
            {
                OSI_LOGI(0, "ReselectApplicationReq return 0x%x \n", retval);
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
                break;
            }
            //break;  DO NOT BREAK
        }
        case 0x00:
        case 0x02:
        case 0x03:
        case 0x06:
        {
            OSI_LOGI(0, "USIM Initiation ...");
            if (ERR_SUCCESS != CFW_SimInitStage1(nSim))
            {
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
            }
            break;
        }
        default:
            OSI_LOGI(0, "Noting to DO!");
        }
    }
}

static void callback_nw_deregister(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "receive EV_CFW_NW_DEREGISTER_RSP...");

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_sim_close, NULL);
    if (ERR_SUCCESS == CFW_SimClose(uti, nSim))
    {
        //        CFW_InvalideTmsiPTmis(nSim);
        //gSimStage3Finish[nSim] = false;
        //[nSim] = false;

        //extern bool g_GETIMSI[];
        //g_GETIMSI[nSim] = false;
    }
}

typedef struct
{
    uint8_t length;
    uint8_t result[18];
    uint8_t BaSize;
    uint16_t arfcn[32];
} AT_MEASINFO_IND;
uint32_t CFW_GetNMR(uint16_t nUTI, CFW_SIM_ID nSimID);
uint8_t packBcch(uint8_t *des, uint16_t *src, uint8_t size)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    if (size == 0)
        return 0;
    else if (size == 1)
    {
        *des = *src;
        return 1;
    }
    uint8_t remain = 0;
    uint8_t mask = 0xFF;
    for (i = 0; i < size; i++)
    {
        j += 2;
        if (j == 2)
            mask = 0xFF;
        else
            mask >>= 2;
        des[k] = (uint8_t)((src[i] >> j) & mask);
        des[k] |= remain;
        remain = (uint8_t)(src[i] << (8 - j));
        k++;
        if (j == 8)
        {
            des[k++] = remain;
            remain = 0;

            j = 0;
        }
    }
    if (j != 0)
        des[k++] = remain;
    return k;
}

static void callback_nw_measure_result(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "receive EV_CFW_NW_MEASURE_RESULT_RSP...");

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint16_t uti = cfwRequestNoWaitUTI();

    AT_MEASINFO_IND *pMeasInfo = (AT_MEASINFO_IND *)cfw_event->nParam1;
    OSI_LOGI(0, "pMeasInfo->BaSize = %d, pMeasInfo->length = %d", pMeasInfo->BaSize, pMeasInfo->length);
    OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", pMeasInfo->length, pMeasInfo->result);
    OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", pMeasInfo->BaSize * 2, pMeasInfo->arfcn);

    uint8_t bcch[64] = {0};
    uint8_t nBcchSize = packBcch(bcch, pMeasInfo->arfcn, pMeasInfo->BaSize);
    OSI_LOGI(0, "nBcchSize = %d(%d)", nBcchSize, ((pMeasInfo->BaSize * 10) >> 3) + 1);
    if (nBcchSize > 64)
    {
        OSI_LOGI(0, "nBcchSize is too large.(%d)", nBcchSize);
        CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, 0, 0, uti, nSim);
        return;
    }
    uint8_t nDataSize = pMeasInfo->length + 1 + nBcchSize + 1;
    OSI_LOGI(0, "nDataSize = %d", nDataSize);
    uint8_t *pData = (uint8_t *)malloc(nDataSize);
    if (pData == NULL)
    {
        OSI_LOGI(0, "SIM_SAT_PROVIDE_LOCAL_INFO_COM: no more memory");
        CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, 0, 0, uti, nSim);
        return;
    }
    else
    {
        memset(pData, 0x00, nDataSize);

        uint8_t offset = 0;
        pData[offset++] = pMeasInfo->length;
        memcpy(pData + offset, pMeasInfo->result, pMeasInfo->length);
        offset += pMeasInfo->length;
        pData[offset++] = nBcchSize;
        //pack bcch list
        memcpy(pData + offset, bcch, nBcchSize);
    }

    OSI_LOGI(0, "network measurement result length = %d", pMeasInfo->length);

    OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", nDataSize, pData);
    CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, pData, nDataSize, uti, nSim);
    free((void *)cfw_event->nParam1);
    memset(pData, 0x00, nDataSize);
    free(pData);
    pData = NULL;
    return;
}

static void callback_get_timing_advance(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "receive EV_CFW_TSM_INFO_IND...");

    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint16_t uti = cfwRequestNoWaitUTI();
    OSI_LOGI(0, "cfw_event->nType = %X(%X)", cfw_event->nType, EV_CFW_TSM_INFO_IND);
    if (cfw_event->nType == 0)
    {
        CFW_TSM_CURR_CELL_INFO CellInf;
        CFW_TSM_ALL_NEBCELL_INFO Dummy;
        CFW_GetCellInfo(&CellInf, &Dummy, nSim);
        SAT_TIMINGADV_RSP nTA;
        nTA.nME_Status = 0x00; // 00<- ME is in IDEL status, 0x01 not
        nTA.nTimingAdv = CellInf.nTSM_TimeADV;

        OSI_LOGI(0, "nTimingAdv =  %d", nTA.nTimingAdv);
        CFW_SatResponse(0x26, 0, 0, &nTA, sizeof(SAT_TIMINGADV_RSP), uti, nSim);
    }
    else
        CFW_SatResponse(0x26, 0x21, 0, 0, 0, uti, nSim);
    CFW_EmodOutfieldTestEnd(uti, nSim);
}

static void callback_sta(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    if (0 == cfw_event->nType)
    {
        AllowedInstance = INSTANCE;
        g_ActiveSatFlag = 1;
        RETURN_OK(cmd->engine);

        if (0x90 == cfw_event->nParam2)
        {
            const char *response = "STNN";
            atCmdRespUrcText(cmd->engine, response);
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
}

static void callback_stk_response(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    //uint8_t nSim = atCmdGetSim(cmd->engine);
    struct stk_tms_info *tms = &stk_tms[nSim][tms_last_timer_id - 1];
    OSI_LOGI(0, "nSim = %d", nSim);
    OSI_LOGI(0, "cfw_event->nEventId = %d", cfw_event->nEventId);
    OSI_LOGI(0, "cfw_event->nParam1 = %d", cfw_event->nParam1);
    OSI_LOGI(0, "cfw_event->nParam2 = %d", cfw_event->nParam2);
    OSI_LOGI(0, "cfw_event->nType = %d", cfw_event->nType);

    if (0 == cfw_event->nType)
    {
#define SIM_SAT_TIMER_EXPIRATION 0xD7
        if (cfw_event->nParam1 == SIM_SAT_TIMER_EXPIRATION)
        {
            uint8_t sw1 = cfw_event->nParam2;
            switch (sw1)
            {
            case 0x90:
            case 0x91:
                tms->tms_value = 0;
                break;
            case 0x93:
#define GCF_27_22_4_21_2_2a
#ifndef GCF_27_22_4_21_2_2a
                break;
#endif
                OSI_LOGI(0, "tms timer id = %d, nSimID = %d", tms->timer_id, nSim);
                OSI_LOGI(0, "tms->tms_value = %d", tms->tms_value);

                if (tms->tms_value != 0)
                {
                    uint16_t value = 4 * 1000; //4 s
                    bool ret = osiTimerStart(tms->tms_timer, value);
                    OSI_LOGI(0, "Timerout interval = 0x%X ms, ret = %d", value, ret);
                }
                break;
            default:
                break;
            }
        }
#ifndef GCF_27_22_4_21_2_2a
        else
        {
            if (tms->tms_value != 0)
            {
                OSI_LOGI(0, "Download Timer expiration, tms_last_timer_id = %d", tms_last_timer_id);
                uint16_t value = 4 * 1000; //4 s
                bool ret = osiTimerStart(tms->tms_timer, value);
                OSI_LOGI(0, "Timerout interval = 0x%X ms, ret = %d", value, ret);
            }
        }
#endif
        OSI_LOGI(0x100045ac, "SAT_AsyncEventProcess: gATSATSetupCallFlag=%d, gATSATLocalInfoFlag=%d,gATSATSendUSSDFlag=%d,gATSATSendSMSFlag=%d \n",
                 gATSATSetupCallFlag[nSim], gATSATLocalInfoFlag[nSim], gATSATSendUSSDFlag[nSim], gATSATSendSMSFlag[nSim]);

        if (!(gATSATSetupCallFlag[nSim] || gATSATLocalInfoFlag[nSim] || gATSATSendUSSDFlag[nSim] ||
              gATSATSendSMSFlag[nSim] || (auto_process & 0x01)))
        {
            if (cmd)
                RETURN_OK(cmd->engine);
        }
        else
        {
            auto_process &= 0xFE;

            gATSATLocalInfoFlag[nSim] = false;
            gATSATSendUSSDFlag[nSim] = false;
            gATSATSendSMSFlag[nSim] = false;
            gATSATSetupCallFlag[nSim] = false;
            gATSATSetupCallProcessingFlag[nSim] = false;
        }
        OSI_LOGI(0x100045ad, "EV_CFW_SAT_RESPONSE_RSP    CMD_FUNC_SUCC  pCfwEvent->nParam1~2 0x%x 0x%x", cfw_event->nParam1, cfw_event->nParam2);
        if ((0xD4 != cfw_event->nParam1) && (0xFE != cfw_event->nParam1))
        {
            if (0x90 == cfw_event->nParam2)
            {
                if ((auto_process & 0x02) != 2)
                {
                    const char *response = "STNN";
                    if (cmd)
                        atCmdRespUrcText(cmd->engine, response);
                }
                else
                    auto_process &= 0xFD;
            }
        }
        else if (0xD4 == cfw_event->nParam1)
        {
            CFW_SAT_TERMINAL_ENVELOPE_CNF *pEnvelope = (CFW_SAT_TERMINAL_ENVELOPE_CNF *)(cfw_event->nParam2);
#ifndef CHIP_HAS_AP
            if (0x90 == (pEnvelope->SW & 0xFF))
            {
                const char *response = "STNN";
                if (cmd)
                    atCmdRespUrcText(cmd->engine, response);
                return;
            }
#endif
            OSI_LOGI(0x100045ae, "EV_CFW_SAT_RESPONSE_RSP STNN 0x%x p->NbByte %d", cfw_event->nParam1, pEnvelope->NbByte);
            if (0x00 != pEnvelope->NbByte)
            {
                /*
                    '00' = Allowed, no modification
                    '01' = Not allowed
                    '02' = Allowed with modifications
                    */
                //                uint8_t nFlag = (p->ResponseData[0]) & 0xFF;
                //AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, pString, AT_ASYN_GET_DLCI(nSim));
            }
            else
            {
                if ((auto_process & 0x02) != 2)
                {
                    //AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, "STNN", AT_ASYN_GET_DLCI(nSim));
                    const char *response = "STNN";
                    if (cmd)
                        atCmdRespUrcText(cmd->engine, response);
                }
                else
                    auto_process &= 0xFD;
            }
            free(pEnvelope);
        }
        else
        {
            if (!(gATSATSetupCallFlag[nSim] || gATSATLocalInfoFlag[nSim] || gATSATSendUSSDFlag[nSim] ||
                  gATSATSendSMSFlag[nSim] || (auto_process & 0x01)))
            {
                OSI_LOGI(0, "error code  p1=  %x,p2=%x\n", cfw_event->nParam1, cfw_event->nParam2);
                if (cmd)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                auto_process &= 0xFE;
                gATSATSetupCallFlag[nSim] = gATSATLocalInfoFlag[nSim] = gATSATSendUSSDFlag[nSim] = gATSATSendSMSFlag[nSim] = false;
            }
            OSI_LOGI(0x100045b5, "EV_CFW_SAT_RESPONSE_RSP: CMD_FUNC_FAIL cfw_event->nType =0x%x", cfw_event->nType);
        }
        return;
    }
}

void callback_timer_expiration_rsp(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "======== _onEV_CFW_SAT_RESPONSE_RSP ========");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    OSI_LOGI(0, "======cfw_event->nEventId=%X", cfw_event->nEventId);
    OSI_LOGI(0, "======cfw_event->nParam1=%X", cfw_event->nParam1);
    OSI_LOGI(0, "======cfw_event->nParam2=%X", cfw_event->nParam2);
    OSI_LOGI(0, "======cfw_event->nFlag=%d, nSim = %d", cfw_event->nFlag, nSim);
    OSI_LOGI(0, "======cfw_event->nType=%X", cfw_event->nType);
    //uint16_t uti = cfwRequestNoWaitUTI();
    if (cfw_event->nType == 0)
    {
        bip_command = 0;
#define SIM_SAT_TIMER_EXPIRATION 0xD7
        if (cfw_event->nParam1 == SIM_SAT_TIMER_EXPIRATION)
        {
            struct stk_tms_info *tms = &stk_tms[nSim][tms_last_timer_id - 1];
            uint8_t sw1 = cfw_event->nParam2;
            switch (sw1)
            {
            case 0x90:
            case 0x91:
                tms->tms_value = 0;
                break;
            case 0x93:
                OSI_LOGI(0, "sat_last_timer_id = %d, nSimID = %d", tms_last_timer_id, nSim);
                OSI_LOGI(0, "tms->tms_value = %d", tms->tms_value);

                if (tms->tms_value != 0)
                {
                    tms->tms_timer = osiTimerCreate(atEngineGetThreadId(), stk_tms_timeout, (void *)tms);
                    //osiTimerStart(tms->tms_timer, tms->timerout * 1000);
                    osiTimerStart(tms->tms_timer, 10 * 1000);
                    OSI_LOGI(0, "Timerout interval = %d ms", tms->timerout * 1000);
                    //CFW_SatResponse(SIM_SAT_TIMER_MANAGEMENT_COM, 0, 0, &tms->timer_id, 1, uti, nSim);
                }
                break;
            default:
                break;
            }
        }
    }
}

static void SATSendSms_SendRspCB_V2(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0, "======== SATSendSms_SendRspCB_V2 ========");
}

static void SATSendSms_SendRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint16_t nUti = cfwRequestUTI((osiEventCallback_t)SATSendSms_SendRspCB_V2, NULL);
    if (0 == cfw_event->nType)
    {
        CFW_SatResponse(0x13, 0x00, 0x00, NULL, 0x00, nUti, nSim);
    }
    else
    {
        CFW_SatResponse(0x13, 0x35, 0x00, NULL, 0x00, nUti, nSim);
    }
}

void _onEV_CFW_SAT_CMDTYPE_IND(const osiEvent_t *event)
{
    OSI_LOGI(0, "======== _onEV_CFW_SAT_CMDTYPE_IND ========");
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    OSI_LOGI(0, "======cfw_event->nEventId=%X", cfw_event->nEventId);
    OSI_LOGI(0, "======cfw_event->nParam1=%X", cfw_event->nParam1);
    OSI_LOGI(0, "======cfw_event->nParam2=%X", cfw_event->nParam2);
    OSI_LOGI(0, "======cfw_event->nFlag=%d, nSim = %d", cfw_event->nFlag, nSim);
    OSI_LOGI(0, "======cfw_event->nType=%X", cfw_event->nType);

    atCommand_t *cmd = NULL;
    uint16_t uti = cfwRequestNoWaitUTI();
    auto_process = 0xFF;
    int32_t retval = 0;
    uint8_t nCmd = 0;
    uint8_t nQualifier = 0;
    if ((CFW_SatGetCurrentCmd(&nCmd, &nQualifier, nSim) == false) || (nCmd != cfw_event->nParam1))
    {
        OSI_LOGI(0, "CFW_SatGetCurrentCmd ERROR, nCmd = %d.", nCmd);
        CFW_SatResponse(cfw_event->nParam1, 0x32, 0, 0, 0, uti, nSim);
        return;
    }
    OSI_LOGI(0, "nCmd = 0x%X, nQualifier = %d", nCmd, nQualifier);
    switch (cfw_event->nParam1)
    {
    case SIM_SAT_SETUP_MENU_COM:
    {
        OSI_LOGI(0, "response SIM_SAT_SETUP_MENU_COM");
        char response[30] = {0};
        sprintf((char *)response, "^STN: %d", (int)cfw_event->nParam1);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "response == %s", response);
        atCmdRespSimUrcText(nSim, response);

        CFW_SatResponse(SIM_SAT_SETUP_MENU_COM, 0, 0, 0, 0, uti, nSim);
        break;
    }
    case SIM_SAT_REFRESH_COM:
    {
        OSI_LOGI(0x10004f4e, "SIM_SAT_REFRESH_COM");
        uint8_t nFilesList[32] = {0};
        uint8_t nFileNum = 32;
        if (CFW_SatGetRefreshFilesList(nFilesList, &nFileNum, nSim) == false)
        {
            OSI_LOGI(0, "CFW_SatGetRefreshFilesList ERROR.");
            CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x32, 0, 0, 0, uti, nSim);
            return;
        }
        switch (nQualifier)
        {
        case 0x01:
        {
            OSI_LOGI(0, "nFileNum = %d", nFileNum);
            for (uint8_t i = 0; i < nFileNum; i++)
                OSI_LOGI(0, "nFilesList[%d] = %d", i, nFilesList[i]);

            uint16_t uti = cfwRequestNoWaitUTI();
            if (nFileNum != 0)
                CFW_SimRefreshFiles(nFilesList, nFileNum, uti, nSim);
            OSI_LOGI(0, "REFRESH files, responed to UICC with 0x03");
            CFW_SatResponse(0x01, 0x03, 0, 0, 0, 0, nSim);
            break;
        }
#if 0
        case 0x00:
        case 0x02:
        case 0x03:
        {
#if 0
            uti = cfwRequestUTI((osiEventCallback_t)callback_nw_deregister, cmd);
            if (ERR_SUCCESS != CFW_NwDeRegister(uti, nSim))
            {
                //uti = cfwRequestUTI((osiEventCallback_t)callback_stk_response, cmd);
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
            }
#endif
            if (CFW_SimInitStage1(nSim) != ERR_SUCCESS)
            {
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
            }
        }
        break;
#endif
        case 0x04:
        {
            if (bip_socket != 0)
            {
                if (CFW_TcpipSocketClose(bip_socket) != 0)
                    bip_socket = 0;
            }
            at_bip_reset();
            CFW_SatSetEventList(0, nSim);

            for (uint8_t i = 0; i < 8; i++)
            {
                struct stk_tms_info *tms = &stk_tms[nSim][i];
                uint32_t timer_value = tms->tms_value;
                if ((timer_value != 0) && (timer_value != -1))
                {
                    OSI_LOGI(0, "Stop timer, system timer id = %d!", i);
                    osiTimerDelete(tms->tms_timer);
                }
            }
        }
        //break;
        case 0x00:
        case 0x02:
        case 0x03:
        case 0x05:
        case 0x06:
        {
            uint8_t cid = 1;
            uti = cfwRequestUTI((osiEventCallback_t)callback_gprs_active, cmd);
#if 0
            OSI_LOGI(0, "Deactivate cid %d", cid);
            if (ERR_SUCCESS != CFW_GprsAct(CFW_GPRS_DEACTIVED, bip_cid, uti, nSim))
            {
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
                OSI_LOGI(0, "refresh: Deactivate cid %d failed", cid);
                return;
            }
            CFW_ReleaseCID(bip_cid, nSim);
#else
            for (; cid < 8; cid++)
            {
                OSI_LOGI(0, "get status of cid = %d!", cid);
                uint8_t status = 0;
                if (ERR_SUCCESS == CFW_GetGprsActState(cid, &status, nSim))
                {
                    OSI_LOGI(0, "cid %d status = %d", cid, status);
                    if (status == CFW_GPRS_ACTIVED)
                    {
                        OSI_LOGI(0, "Deactivate cid %d", cid);
                        if (ERR_SUCCESS != CFW_GprsAct(CFW_GPRS_DEACTIVED, cid, uti, nSim))
                        {
                            //uti = cfwRequestUTI((osiEventCallback_t)callback_stk_response, cmd);
                            uint16_t uti = cfwRequestNoWaitUTI();
                            CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
                            OSI_LOGI(0, "refresh: Deactivate cid %d failed", cid);
                            return;
                        }
                        CFW_ReleaseCID(cid, nSim);
                    }
                }
            }
#endif
            uti = cfwRequestUTI((osiEventCallback_t)callback_nw_deregister, cmd);
            if (ERR_SUCCESS != CFW_NwDeRegister(uti, nSim))
            {
                //uti = cfwRequestUTI((osiEventCallback_t)callback_stk_response, cmd);
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x20, 0, 0, 0, uti, nSim);
            }
        }
        break;
        default:
        {
            OSI_LOGI(0, "The parameter(%u) of REFRESH command is not supported!", nQualifier);
            CFW_SatResponse(SIM_SAT_REFRESH_COM, 0x30, 0, 0, 0, uti, nSim);
        }
        break;
        }
    }
    break;
    case SIM_SAT_OPEN_CHANNEL_COM:
    {
        bip_command = SIM_SAT_OPEN_CHANNEL_COM;
        OSI_LOGI(0, "Get OPEN CHANNEL!, bip_command = %d", bip_command);
        if (false == CFW_SatGetOpenChannelBearerDes(&bip_qos, &bip_pdp_type, nSim))
        {
            OSI_LOGI(0, "Open Channel ERROR: bearer description error!");
            CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }

        uint16_t buffer_size = 0;
        uint8_t bearer_type = 0;
        uint8_t trans_type = 0;
        uint16_t trans_port = 0;
        if (false == CFW_SatGetOpenChannelNetInfoV2(&buffer_size, &bearer_type, &trans_type, &trans_port, nSim))
        {
            OSI_LOGI(0, "Open Channel ERROR: apn error!");
            CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }

        struct CFW_SAT_CHANNEL channel_info;
        OSI_LOGI(0, "OpenChannel: buffer_size = %X!", buffer_size);

        if (buffer_size > BIP_MAX_BUFFER_SIZE)
        {
            bip_buffer_size = BIP_MAX_BUFFER_SIZE;
            bip_oc_tr_status = 0x07;
        }
        else
        {
            bip_buffer_size = buffer_size;
            bip_oc_tr_status = 0;
        }

        bip_sdata.nLength = 0;
        bip_sdata.pData = (uint8_t *)malloc(bip_buffer_size);
        if (bip_sdata.pData == NULL)
        {
            OSI_LOGI(0, "Open Channel ERROR: No more memory!");
            CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }

        bip_bearer_type = bearer_type;
        channel_info.nBearerType = bip_bearer_type;
        channel_info.nBufferSize = bip_buffer_size;
        channel_info.nStatus = 0;
        switch (bearer_type)
        {
        case OPEN_CHANNEL_BEARER_CSD:
        {
            CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }
        case OPEN_CHANNEL_BEARER_GPRS:
        case OPEN_CHANNEL_BEARER_DEFAULT:
        {
            //when BearerType == Default, APN object mustn't exist;

            uint8_t apn_length = 50;
            uint8_t apn[50] = {0};
            if (false == CFW_SatGetOpenChannelAPN(apn, &apn_length, nSim))
            {
                OSI_LOGI(0, "Open Channel ERROR: apn error!");
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x20, 0, 0, 0, uti, nSim);
                return;
            }
            OSI_LOGI(0, "apn_length = %d!", apn_length);
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", apn_length, apn);
            if (apn_length != 0)
            {
                if (bip_apn.apn != NULL)
                    free(bip_apn.apn);
                bip_apn.apn = malloc(apn_length);
                if (bip_apn.apn == NULL)
                {
                    CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, 0, 0, uti, nSim);
                    return;
                }
                //===========================================================
                uint8_t i = 0;
                uint8_t length = apn_length;
                uint8_t *p = bip_apn.apn;
                if (length < '0')
                {
                    if (apn[i] < length)
                    {
                        memcpy(p, apn + 1, length - 1);
                        i = apn[i];
                        for (; i < length - 1; i += apn[i + 1] + 1)
                            if (p[i] < length - i)
                                p[i] = '.';
                        bip_apn.length = length - 1;
                    }
                }
                if (i == 0)
                {
                    memcpy(bip_apn.apn, apn, length);
                    bip_apn.length = length;
                }
                //===========================================================
                OSI_LOGI(0, "bip_apn.length = %d!", bip_apn.length);
                OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", bip_apn.length, bip_apn.apn);
            }
            uint8_t user_length = 50;
            uint8_t user_name[50] = {0};
            uint8_t passwd_length = 32;
            uint8_t passwd[32] = {0};
            if (false == CFW_SatGetOpenChannelUserInfo(user_name, &user_length, passwd, &passwd_length, nSim))
            {
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x20, 0, 0, 0, uti, nSim);
                return;
            }

            if (user_length != 0)
            {
                if (bip_user.user != NULL)
                    free(bip_user.user);

                bip_user.user = malloc(user_length);
                if (bip_user.user == NULL)
                {
                    CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, 0, 0, uti, nSim);
                    return;
                }
                bip_user.length = user_length;
                memcpy(bip_user.user, user_name, bip_user.length);
                OSI_LOGI(0, "bip_user.length = %d!", bip_user.length);
                OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", bip_user.length, bip_user.user);
            }
            if (passwd_length != 0)
            {
                if (bip_passwd.passwd != NULL)
                    free(bip_passwd.passwd);

                bip_passwd.passwd = malloc(passwd_length);
                if (bip_passwd.passwd == NULL)
                {
                    CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, 0, 0, uti, nSim);
                    return;
                }
                bip_user.length = passwd_length;
                memcpy(bip_passwd.passwd, passwd, bip_passwd.length);
                OSI_LOGI(0, "bip_passwd.length = %d!", bip_passwd.length);
                OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", bip_passwd.length, bip_passwd.passwd);
            }

            bip_protocol = IPPROTO_TCP;
            bip_type = SOCK_STREAM;
            bip_socket = 0;

            OSI_LOGI(0, "pOpenChannel->TransportLevelType = %d!", trans_type);
            if (trans_type == 0x01)
            {
                bip_protocol = IPPROTO_UDP;
                bip_type = SOCK_DGRAM;
                OSI_LOGI(0, "UDP mode!");
            }
            else if (trans_type != 0x02)
            {
                OSI_LOGI(0, "ERROR:TransportLevelType = %d!", trans_type);
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x30, 0, &channel_info, sizeof(struct CFW_SAT_CHANNEL), uti, nSim);
                return;
            }

            OSI_LOGI(0, "OpenChannel nQualifier = %d!", nQualifier);
            uint8_t address_type = 0;
            uint8_t address_length = 16;
            uint8_t dest_address[16] = {0};
            if (false == CFW_SatGetOpenChannelDestAddress(dest_address, &address_length, &address_type, nSim))
            {
                OSI_LOGI(0, "Open Channel ERROR: Dest address!");
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x20, 0, 0, 0, uti, nSim);
                return;
            }

            bip_addrtype = address_type;
            if (address_type == 0x21)
            {
                OSI_LOGI(0, "==== IPv4 ====, IP = %x, Port = %x", *(uint32_t *)(dest_address), (trans_port));
                bip_addr.addr4.sin_family = AF_INET;
                bip_addr.addr4.sin_port = htons(trans_port);
                bip_addr.addr4.sin_addr.s_addr = (*(uint32_t *)dest_address);
                bip_addr.addr4.sin_len = sizeof(CFW_TCPIP_SOCKET_ADDR);
                OSI_LOGI(0, "bip_addr.sin_addr.s_addr = %x, Port = %x", bip_addr.addr4.sin_addr.s_addr, bip_addr.addr4.sin_port);

                bip_addr.type = BIP_IPv4;
            }
            else if (address_type == 0x57)
            {
                OSI_LOGI(0, "==== IPv6 ====, IP = %x, Port = %x", *(uint32_t *)(dest_address), (trans_port));
                bip_addr.addr6.sin6_family = AF_INET6;
                bip_addr.addr6.sin6_port = htons(trans_port);
                memcpy(bip_addr.addr6.sin6_addr.un.u32_addr, (uint32_t *)dest_address, 4);
                bip_addr.addr6.sin6_len = sizeof(CFW_TCPIP_SOCKET_ADDR6);

                OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", bip_addr.addr6.sin6_len, (uint8_t *)bip_addr.addr6.sin6_addr.un.u32_addr);
                OSI_LOGI(0, "bip_addr.sin6_addr.u32_addr[0] = %x, Port = %x", bip_addr.addr6.sin6_addr.un.u32_addr[0], bip_addr.addr6.sin6_port);
                bip_addr.type = BIP_IPv6;
            }
            else
            {
                OSI_LOGI(0, "ERROR: DestAddressType = %d!", address_type);
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x32, 0, &channel_info, sizeof(struct CFW_SAT_CHANNEL), uti, nSim);
                return;
            }
            OSI_LOGI(0, "OpenChannel ,nQualifier = %d!", nQualifier);

            bip_qualifier = nQualifier;
            if ((bip_qualifier & 0x01) == 0x01) //immediate link establishment
            {
                OSI_LOGI(0, "immediate link establishment");
                at_bip_process(SIM_SAT_OPEN_CHANNEL_COM, nSim);
            }
            else
            {
                CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, bip_oc_tr_status, 0, &channel_info, sizeof(struct CFW_SAT_CHANNEL), uti, nSim);
                if ((bip_qualifier & 0x04) == 0x04) //immediate link establishment in background
                    at_bip_process(SIM_SAT_OPEN_CHANNEL_COM, nSim);
                //on demand mode
                return;
            }
        }
        break;
        default:
        {
            OSI_LOGI(0, "ERROR: BearerType = %d!", bearer_type);
            CFW_SatResponse(SIM_SAT_OPEN_CHANNEL_COM, 0x32, 0, &channel_info, sizeof(struct CFW_SAT_CHANNEL), uti, nSim);
            return;
        }
        }
        break;
    }
    case SIM_SAT_CLOSE_CHANNEL_COM:
    {
        uint8_t channel;
        if ((false == CFW_SatGetChannelID(&channel, nSim)) || (channel != bip_cid))
        {
            OSI_LOGI(0, "Get CLOSE CHANNEL!, bip_cid = %d, channel = %d", bip_cid, channel);
            uint8_t additional = 0x03;
            if (bip_cid == 0)
                additional = 0x02;
            CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, 0x3A, 0, &additional, 1, uti, nSim);
            return;
        }
        at_bip_reset();
        if (bip_socket != 0)
        {
            if (CFW_TcpipSocketClose(bip_socket) == ERR_SUCCESS)
            {
                OSI_LOGI(0, "Close socket in CLOSE CHANNEL command successful!");
                bip_socket = 0;
                break;
            }
            else
            {
                OSI_LOGI(0, "Close socket in CLOSE CHANNEL command failed!");
                CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, 0, 0, 0, 0, uti, nSim);
            }
        }
        else
            CFW_SatResponse(SIM_SAT_CLOSE_CHANNEL_COM, 0, 0, 0, 0, uti, nSim);
        break;
    }
    case SIM_SAT_SEND_DATA_COM:
    {
        bip_command = SIM_SAT_SEND_DATA_COM;
        OSI_LOGI(0, "Get SEND DATA!, bip_command = %d", bip_command);
        uint8_t channel;
        if ((false == CFW_SatGetChannelID(&channel, nSim)) || (channel != bip_cid))
        {
            OSI_LOGI(0, "Get CLOSE CHANNEL!, bip_cid = %d, channel = %d", bip_cid, channel);
            uint8_t additional = 0x03;
            if (bip_cid == 0)
                additional = 0x02;
            CFW_SatResponse(SIM_SAT_SEND_DATA_COM, 0x3A, 0, &additional, 1, uti, nSim);
            return;
        }

        uint8_t data[255] = {0};
        uint8_t length = 255;
        if (false == CFW_SatGetSendData(data, &length, nSim))
        {
            CFW_SatResponse(SIM_SAT_SEND_DATA_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "Send data: bip_type = %d, bip_protocol = %d!", bip_type, bip_protocol);
        OSI_LOGI(0, "Send data: length = %d, bip_sdata.nLength = %d!", length, bip_sdata.nLength);
        if (length + bip_sdata.nLength > bip_buffer_size)
        {
            CFW_SatResponse(SIM_SAT_SEND_DATA_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }
        else
        {
            memcpy(bip_sdata.pData + bip_sdata.nLength, data, length);
            bip_sdata.nLength += length;
        }

        OSI_LOGI(0, "SendData nQualifier = %d!", nQualifier);
        if (nQualifier == 0) //store data to buffer
        {
            uint16_t size = bip_buffer_size - bip_sdata.nLength;
            uint8_t room = bip_buffer_size - bip_sdata.nLength;
            if (size > 255)
                room = 0xFF;
            CFW_SatResponse(SIM_SAT_SEND_DATA_COM, 0, 0, &room, 1, uti, nSim);
            return;
        }
        else if (nQualifier != 1) //unkown Qualifier
        {
            CFW_SatResponse(SIM_SAT_SEND_DATA_COM, 0x32, 0, 0, 0, uti, nSim);
            return;
        }

        if ((bip_qualifier & 0x02) == 0x02)
        {
            OSI_LOGI(0, "automatic reconnection");
            at_bip_process(SIM_SAT_SEND_DATA_COM, nSim);
        }
        else
        {
            if (bip_protocol == CFW_TCPIP_IPPROTO_TCP)
                retval = CFW_TcpipSocketSend(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0);
            else
            {
                if (bip_addrtype == BIP_IPv4)
                    retval = CFW_TcpipSocketSendto(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr4, sizeof(bip_addr.addr4));
                else
                    retval = CFW_TcpipSocketSendto(bip_socket, bip_sdata.pData, bip_sdata.nLength, 0, (CFW_TCPIP_SOCKET_ADDR *)&bip_addr.addr6, sizeof(bip_addr.addr6));
            }
            OSI_LOGI(0, "retval = %d, bip_socket = %d, bip_addrtype = %X", retval, bip_socket, bip_addrtype);
            if (retval == SOCKET_ERROR)
            {
                OSI_LOGI(0, "Socket SEND DATA ERROR!");
                at_bip_response(0x21, nSim);
            }
            else
            {
                if (retval != bip_sdata.nLength)
                {
                    memcpy(bip_sdata.pData, bip_sdata.pData + retval, bip_sdata.nLength - retval);
                    bip_sdata.nLength -= retval;
                    OSI_LOGI(0, "------ SEND Data immediately ------");
                }
                else
                    bip_sdata.nLength = 0;
                at_bip_response(0, nSim);
            }
        }
        OSI_LOGI(0, "Response to SEND DATA, bip_socket = %d!", bip_socket);
    }
    break;
    case SIM_SAT_RECEIVE_DATA_COM:
    {
        OSI_LOGI(0, "------------ BIP receive data ------------");
        uint8_t channel;
        if ((false == CFW_SatGetChannelID(&channel, nSim)) || (channel != bip_cid))
        {
            OSI_LOGI(0, "Get CLOSE CHANNEL!, bip_cid = %d, channel = %d", bip_cid, channel);
            uint8_t additional = 0x03;
            if (bip_cid == 0)
                additional = 0x02;
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0x3A, 0, &additional, 1, uti, nSim);
            return;
        }

        OSI_LOGI(0, "bip_rdata.first = %X", bip_rdata.first);
        OSI_LOGI(0, "bip_rdata.total_len = %d", bip_rdata.total_len);
        if (bip_rdata.first != NULL)
        {
            OSI_LOGI(0, "first node size = %d", bip_rdata.first->size);
            OSI_LOGI(0, "first node offset = %d", bip_rdata.first->offset);
        }
        bip_command = SIM_SAT_RECEIVE_DATA_COM;
        uint8_t data_length = 0;
        if (false == CFW_SatGetReceiveData(&data_length, nSim))
        {
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "Get RECEIVE DATA!, data_length = %d, bip_rdata.total_len = %d", data_length, bip_rdata.total_len);

        if ((bip_rdata.total_len == 0) || (bip_rdata.total_len < data_length))
        {
            OSI_LOGI(0, "There is no enough data!");
            OSI_LOGI(0, "bip_rdata.total_len = %d, pReceiveData->ChannelDataLength = %d", bip_rdata.total_len, data_length);
            bip_rdata.buffer[0] = 0xFF;
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0, 0, bip_rdata.buffer, 1, uti, nSim);
            return;
        }
        if ((bip_socket == 0) || (bip_cid == 0))
        {
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0x3A, 0, 0, 0, uti, nSim);
            bip_rdata.total_len = 0;
            return;
        }
        if (bip_rdata.total_len == 0)
        {
            OSI_LOGI(0, "There is no enough data!");
            OSI_LOGI(0, "bip_rdata.total_len = %d, pReceiveData->ChannelDataLength = %d", bip_rdata.total_len, data_length);
            bip_rdata.buffer[0] = 0;
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0, 0, bip_rdata.buffer, 1, uti, nSim);
            return;
        }
        struct receive_buffer **p = &bip_rdata.first;
        if ((*p) == NULL)
        {
            bip_rdata.buffer[0] = 0;
            CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0x02, 0, bip_rdata.buffer, 1, uti, nSim);
            return;
        }
        OSI_LOGI(0, "bip_rdata.first = 0x%X", bip_rdata.first);
        while ((*p)->next != NULL)
        {
            p = &(*p)->next;
            OSI_LOGI(0, "*p = 0x%X", *p);
        }

        uint16_t length = (*p)->size - (*p)->offset;
        OSI_LOGI(0, "length = %d, (*p)->size = %d, (*p)->offset = %d", length, (*p)->size, (*p)->offset);
        if (length > data_length)
            length = data_length;
        if (length > 237)
            length = 237;
        OSI_LOGI(0, "length = %d", length);

        memcpy(bip_rdata.buffer + 1, (*p)->buffer + (*p)->offset, length);
        bip_rdata.total_len -= length;
        bip_rdata.buffer[0] = bip_rdata.total_len > 0xFF ? 0xFF : bip_rdata.total_len;
        //CSW_TC_MEMBLOCK(10, bip_rdata.buffer, length + 1, 16);

        CFW_SatResponse(SIM_SAT_RECEIVE_DATA_COM, 0, 0, bip_rdata.buffer, length + 1, uti, nSim);
        if ((*p)->size - (*p)->offset == length)
        {
            OSI_LOGI(0, "Free node: 0x%X", *p);
            free(*p);
            (*p) = NULL;
        }
        else
            (*p)->offset += length;

        OSI_LOGI(0, "============ BIP receive data ============");
        OSI_LOGI(0, "bip_rdata.first = %X", bip_rdata.first);
        OSI_LOGI(0, "bip_rdata.total_len = %d", bip_rdata.total_len);
        if (bip_rdata.first != NULL)
        {
            OSI_LOGI(0, "first node size = %d", bip_rdata.first->size);
            OSI_LOGI(0, "first node offset = %d", bip_rdata.first->offset);
        }
    }
    break;
    case SIM_SAT_GET_CHANNEL_STATUS_COM:
    {
        bip_command = SIM_SAT_GET_CHANNEL_STATUS_COM;
        uint16_t nStatus = 0;
        OSI_LOGI(0, "Get GET CHANNEL STATUS!, bip_status = %d, bip_type = %d", bip_status, bip_type);
        if (bip_cid != 0)
        {
            if ((bip_status >= BIP_SOCKET) && (bip_type == CFW_TCPIP_SOCK_STREAM))
                nStatus = (0x80 + bip_cid) << 0x08;
            else if ((bip_status >= BIP_ACTIVED) && (bip_type == CFW_TCPIP_SOCK_DGRAM))
                nStatus = (0x80 + bip_cid) << 0x08;
            else
                nStatus = (bip_cid << 0x08) | 0x0005;

            OSI_LOGI(0, "Get GET CHANNEL STATUS!, bip_cid = %d, nStatus = %X", bip_cid, nStatus);
        }
        CFW_SatResponse(SIM_SAT_GET_CHANNEL_STATUS_COM, 0, 0, &nStatus, 2, uti, nSim);
    }
    break;
    case SIM_SAT_SETUP_EVENT_LIST_COM:
    {
        uint8_t events[32] = {0};
        uint8_t num = 32;
        uint8_t max = CFW_SatGetSetupEventList(events, &num, nSim);
        OSI_LOGI(0, "num = %d, max = %d", num, max);

        uint32_t event_list = 0;
        uint8_t i = 0;
        if (max != 0)
        {
            for (i = 0; i < num; i++)
            {
                OSI_LOGI(0, "events[%d] = %d", i, events[i]);
                event_list |= (1 << events[i]);
            }
        }
        else
            event_list = 0;
        CFW_SatSetEventList(event_list, nSim);
        OSI_LOGI(0, "Current event list map: %X", event_list);

        CFW_SatResponse(SIM_SAT_SETUP_EVENT_LIST_COM, 0, 0, 0, 0, uti, nSim);
    }
    break;
    case SIM_SAT_TIMER_MANAGEMENT_COM:
    {
        uint8_t timer = 0;
        uint8_t hour = 0;
        uint8_t minute = 0;
        uint8_t second = 0;
        if (false == CFW_SatGetTimerManagement(&timer, &hour, &minute, &second, nSim))
        {
        TIMER_ERROR:
            CFW_SatResponse(SIM_SAT_TIMER_MANAGEMENT_COM, 0x20, 0, 0, 0, uti, nSim);
        }

        OSI_LOGI(0, "SAT Timer id = %d!", timer);
        if ((timer == 0) || (timer > 8))
            goto TIMER_ERROR;
        struct stk_tms_info *tms = &stk_tms[nSim][timer - 1];
        tms->timer_id = timer;
        tms->sim_id = nSim;
        OSI_LOGXI(OSI_LOGPAR_D, 0, "Timer management: orignal time value = %lld", tms->tms_value);

        OSI_LOGI(0, "TIMER MANAGEMENT: Qualifier = %d!", nQualifier);
        if (nQualifier == 0)
        {
            OSI_LOGI(0, "Timer: hour = %d, minute = %d, second = %d!", hour, minute, second);
            uint32_t timerout = hour * 3600 + minute * 60 + second;
            OSI_LOGI(0, "Timer out = %d s", timerout);
            tms->timerout = timerout;
            int64_t timer_value = tms->tms_value;
            if ((timer_value != 0) && (timer_value != -1))
            {
                OSI_LOGI(0, "reStart Timer, SAT timer id = %d!", timer);
                osiTimerDelete(tms->tms_timer);
            }
            tms->tms_value = osiUpTime();
            OSI_LOGXI(OSI_LOGPAR_D, 0, "Timer management: orignal time value = %lld", tms->tms_value);

            tms->tms_timer = osiTimerCreate(atEngineGetThreadId(), stk_tms_timeout, (void *)tms);
            bool ret = osiTimerStart(tms->tms_timer, timerout * 1000);
            OSI_LOGI(0, "Timerout interval = %d ms(%X)", timerout * 1000, tms->tms_timer);
            if (ret == true)
            {
                CFW_SatResponse(SIM_SAT_TIMER_MANAGEMENT_COM, 0, 0, &timer, 1, uti, nSim);
            }
            else
                goto TIMER_ERROR;
        }
        else if ((nQualifier == 0x01) || (nQualifier == 0x02))
        {
            uint64_t time_value = tms->tms_value;
            OSI_LOGXI(OSI_LOGPAR_D, 0, "Timer management: orignal time value = %lld", time_value);

            if ((time_value == 0) || (time_value == -1)) //timer isn't starting
                CFW_SatResponse(SIM_SAT_TIMER_MANAGEMENT_COM, 0x24, 0, &timer, 1, uti, nSim);
            else
            {
                if (nQualifier == 0x01)
                {
                    if (osiTimerIsRunning(tms->tms_timer))
                    {
                        OSI_LOGI(0, "killed System timer id = %d!", tms->timer_id);
                        osiTimerStop(tms->tms_timer);
                    }
                }

                time_value = (uint64_t)osiUpTime() - time_value;
                OSI_LOGI(0, "Timer management: time elapsed = %d ms", time_value);
                time_value = time_value / 1000; //convert from ms to s

                uint8_t TimerManagement[4];
                TimerManagement[0] = timer;
                TimerManagement[1] = time_value / 3600;
                uint32_t remain = time_value % 3600;
                TimerManagement[2] = remain / 60;
                TimerManagement[3] = remain % 60;
                OSI_LOGI(0, "Timer management: Hour = %d, Minute = %d, Second = %d", TimerManagement[1], TimerManagement[2], TimerManagement[3]);
                CFW_SatResponse(SIM_SAT_TIMER_MANAGEMENT_COM, 0, 0, TimerManagement, 4, uti, nSim);

                if (nQualifier == 0x01)
                {
                    tms->tms_value = 0;
                }
            }
        }
    }
    break;
    case SIM_SAT_DISPLAY_TEXT_COM:
    {
        uint8_t text[255] = {0};
        uint8_t length = 255;
        uint8_t scheme = 0;
        uint8_t status = 0;
        if (CFW_SatGetDisplayText(text, &length, &scheme, nSim) == false)
        {
            status = 0x30;
            OSI_LOGI(0, "call CFW_SatGetDisplayText error, status = %d", status);
        }
        else
        {
            if (length == 0)
                status = 0x32;
            else if ((scheme != 0) && (scheme != 4) && (scheme != 8))
                status = 0x32;
            OSI_LOGI(0, "length = %d, scheme = %d, status = %d", length, scheme, status);
        }
        CFW_SatResponse(SIM_SAT_DISPLAY_TEXT_COM, status, 0, 0, 0, uti, nSim);
    }
    break;
    case SIM_SAT_POLLING_OFF_COM:
    {
        OSI_LOGI(0, "Recevie POLLING OFF COMMAND");
        SimPollOffReq(nSim);
        CFW_SatResponse(SIM_SAT_POLLING_OFF_COM, 0, 0, 0, 0, uti, nSim);
        return;
    }
    break;
    case SIM_SAT_MORE_TIME_COM:
        CFW_SatResponse(cfw_event->nParam1, 0, 0, NULL, 0, uti, nSim);
        break;
    case SIM_SAT_POLL_INTERVAL_COM:
    {
        uint8_t time_unit = 0;
        uint8_t time_interval = 0;
        if (false == CFW_SatGetPollIntervalDuration(&time_unit, &time_interval, nSim))
        {
            CFW_SatResponse(SIM_SAT_POLL_INTERVAL_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "time_unit = %d, time_interval = %d", time_unit, time_interval);
        uint16_t nTimerOut = 0; //unit to stack is 1/10s
        if (time_unit == 0)
            nTimerOut = 600 * time_interval;
        else if (time_unit == 0x01)
            nTimerOut = time_interval * 10;
        else if (time_unit == 0x02)
            nTimerOut = time_interval;
        else
        {
            CFW_SatResponse(SIM_SAT_POLL_INTERVAL_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "nTimerOut = %d", nTimerOut);

        CFW_SimPollReq(nTimerOut, nSim);
        uint8_t Duration[2];
        Duration[0] = time_unit;
        Duration[1] = time_interval;

        OSI_LOGI(0, "Duration = %X, %X", Duration[0], Duration[1]);
        CFW_SatResponse(SIM_SAT_POLL_INTERVAL_COM, 0, 0, Duration, 2, uti, nSim);
        return;
    }
    break;
    case SIM_SAT_PLAY_TONE_COM:
    {
        OSI_LOGI(0, "get SIM_SAT_PLAY_TONE_COM");
        uint8_t length = 255;
        uint8_t text[255] = {0};
        if (false == CFW_SatGetPlayToneText(text, &length, nSim))
        {
            CFW_SatResponse(SIM_SAT_PLAY_TONE_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "Play Tone: text length = %d", length);
        if (length != 0)
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%c", length, text);

        uint8_t tone = 0;
        uint8_t unit = 0;
        uint8_t interval = 0;
        if (false == CFW_SatGetPlayToneDuration(&tone, &unit, &interval, nSim))
        {
            CFW_SatResponse(SIM_SAT_PLAY_TONE_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }
        OSI_LOGI(0, "Play Tone: TimeInterval = %d", interval);
        OSI_LOGI(0, "Play Tone: TimeUnit = %d", unit);
        OSI_LOGI(0, "Play Tone: Tone = %d", tone);

        CFW_SatResponse(SIM_SAT_PLAY_TONE_COM, 0, 0, 0, 0, uti, nSim);
        return;
    }
    break;
    case SIM_SAT_SETUP_IDLE_MODE_COM:
    {
        OSI_LOGI(0, "get SIM_SAT_SETUP_IDLE_MODE_COM");
        uint8_t scheme = 0;
        uint8_t length = 255;
        uint8_t text[255] = {0};
        if (false == CFW_SatGetSetupIdleModeText(text, &length, &scheme, nSim))
        {
            CFW_SatResponse(SIM_SAT_SETUP_IDLE_MODE_COM, 0x30, 0, 0, 0, uti, nSim);
            return;
        }
        //OSI_LOGI(0, "Frame Identifier = %x", setup_idle->nFrameIdentifier);
        //OSI_LOGI(0, "Icon Qualifier = %x", setup_idle->nIconQualifier);
        //OSI_LOGI(0, "Icon Idenitifier = %x", setup_idle->nIconIdentifier);
        OSI_LOGI(0, "length = %d, scheme = %X", length, scheme);
        uint8_t idle_text[512] = {0};
        if ((scheme == 0) || (scheme == 0xF0))
        {
            length = SUL_Decode7Bit(text, idle_text, length);
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%c", length, idle_text);
        }
        uint8_t status = 0;
        CFW_SatResponse(SIM_SAT_SETUP_IDLE_MODE_COM, status, 0, 0, 0, uti, nSim);
        return;
    }
    break;
    case SIM_SAT_PROVIDE_LOCAL_INFO_COM:
    {
        if (nQualifier == 0)
        {
            uint8_t LocalInfo[9] = {0xFF};
            CFW_NW_STATUS_INFO status;
            uint8_t length = 0;
            uint32_t retval = 0;
            uint8_t rat = CFW_NWGetStackRat(nSim);
            OSI_LOGI(0, "current rat = %d", rat);

#define API_RAT_GSM (1 << 1)
#define API_RAT_LTE (1 << 2)
#define API_RAT_CAMP_NO 0
#define API_RAT_CAMP_IRAT (1 << 0)
#define API_RAT_CAMP_GSM API_RAT_GSM
#define API_RAT_CAMP_LTE API_RAT_LTE

            if (rat == API_RAT_CAMP_LTE)
            {
                retval = CFW_GprsGetstatus(&status, nSim);
                OSI_LOGI(0, "1nCellId = %02X%02X%02X%02X", status.nCellId[0], status.nCellId[1], status.nCellId[2], status.nCellId[3]);
                LocalInfo[5] = status.nCellId[0] << 4 | status.nCellId[1] >> 4;
                LocalInfo[6] = status.nCellId[1] << 4 | status.nCellId[2] >> 4;
                LocalInfo[7] = status.nCellId[2] << 4 | status.nCellId[3] >> 4;
                LocalInfo[8] = status.nCellId[3] << 4 | 0x0F;
                length = 9;
            }
            else if (rat == API_RAT_CAMP_GSM)
            {
                retval = CFW_NwGetStatus(&status, nSim);
                OSI_LOGI(0, "2nCellId = %02X%02X", status.nCellId[0], status.nCellId[1]);
                memcpy(LocalInfo + 5, status.nCellId, 2);
                length = 7;
            }
            else
            {
                retval = ~ERR_SUCCESS;
                OSI_LOGI(0, "3rat is illegal");
            }
            if (retval != ERR_SUCCESS)
            {
                OSI_LOGI(0, "Process Provide ocal information:CFW_GetCellInfo");
                goto LOCAL_ERROR;
            }
            OSI_LOGI(0, "nAreaCode = %02X%02X%02X%02X%02X", status.nAreaCode[0], status.nAreaCode[1], status.nAreaCode[2],
                     status.nAreaCode[3], status.nAreaCode[4]);

            memcpy(LocalInfo, status.nAreaCode, 5);
            OSI_LOGI(0, "MCC, MNC = %02X%02X%02X", LocalInfo[0], LocalInfo[1], LocalInfo[2]);
            OSI_LOGI(0, "LAC = %02X%02X", LocalInfo[3], LocalInfo[4]);
            CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, &LocalInfo, length, uti, nSim);
        }
        else if (nQualifier == 0x01)
        {
            uint8_t imei_str[15] = {0};
            if (nvmReadImei(nSim, (nvmImei_t *)imei_str) == -1)
                goto LOCAL_ERROR;

            OSI_LOGI(0, "--- IMEIsv ---");
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%*s", 15, imei_str);
            uint8_t imei[8];
            uint8_t i = 1;
            uint8_t j = 1;
            imei[0] = ((imei_str[0] - '0') << 4) | 0x0A;
            while (j < 15)
            {
                imei[i++] = ((imei_str[j + 1] - '0') << 4) | (imei_str[j] - '0');
                j += 2;
            }
            imei[i - 1] &= 0x0F;
            OSI_LOGXI(OSI_LOGPAR_M, 0, "%*X", 8, imei);
            CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, imei, 8, uti, nSim);
        }
        else if (nQualifier == 0x02)
        {
            uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_nw_measure_result, NULL);
            retval = CFW_GetNMR(uti, nSim);
            if (retval != ERR_SUCCESS)
            {
                cfwReleaseUTI(uti);
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0x20, 0, 0, 0, uti, nSim);
            }
        }
        else if (nQualifier == 0x03)
        {
            int timeoffset = gAtSetting.timezone * CFW_TIME_ZONE_SECOND;
            time_t lt = osiEpochSecond() + timeoffset;
            struct tm system_time;
            gmtime_r(&lt, &system_time);

            //1 byte year, 1 byte month, 1 byte day, 1 hour, 1 minute, 1 second, 1 timezone
            uint8_t time[7];
            time[0] = system_time.tm_year % 100;
            time[1] = system_time.tm_mon + 1;
            time[2] = system_time.tm_mday;
            time[3] = system_time.tm_hour;
            time[4] = system_time.tm_min;
            time[5] = system_time.tm_sec;
            time[6] = gAtSetting.timezone;
            OSI_LOGI(0, "Year = %d, Month = %d, Day = %d", time[0], time[1], time[2]);
            OSI_LOGI(0, "Hour = %d, Minute = %d, Second = %d", time[3], time[4], time[5]);
            OSI_LOGI(0, "Time Zone = %d", time[6]);
            CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0x0, time, 7, uti, nSim);
        }
        else if (nQualifier == 0x04)
        {
            uint8_t elp[4] = {0};
            uint8_t length = 4;
            CFW_SimGetLanguage(elp, &length, nSim);
            OSI_LOGI(0, "AT: elp = 0x%X,%X, length = %d", elp[0], elp[1], length);
            if (((elp[0] != 0xFF) && (elp[1] != 0xFF)) && (length != 0))
                CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, elp, 2, uti, nSim);
            else
                CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, 0, 0, uti, nSim);
        }
        else if (nQualifier == 0x05)
        {
            CFW_TSM_FUNCTION_SELECT SelecFUN = {0};
            SelecFUN.nServingCell = true;
            SelecFUN.nNeighborCell = false;
            uint16_t uti = cfwRequestUTI((osiEventCallback_t)callback_get_timing_advance, NULL);
            uint32_t retval = CFW_EmodOutfieldTestStart(&SelecFUN, uti, nSim);
            if (retval != ERR_SUCCESS)
            {
                cfwReleaseUTI(uti);
                uint16_t uti = cfwRequestNoWaitUTI();
                CFW_SatResponse(0x26, 0x21, 0, 0, 0, uti, nSim);
            }
            OSI_LOGI(0, "retval = %d", retval);
        }
        else if (nQualifier == 0x06)
        {
            uint8_t mode = 0x08;
            uint8_t type = CFW_NWGetStackRat(nSim);
            if (type == 4) //  API_NW_NBIOT)
                mode = 0x08;
            else if (type == 1) //API_NW_GPRS)
                mode = 0x00;
            else
            {
                OSI_LOGI(0, "nw_status.PsType = %d(%d,%d)", type, 4, 1); // API_NW_NBIOT, API_NW_GPRS);
            }
            OSI_LOGI(0, "PS Type = %X, mode =  %x", type, mode);

            CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0, 0, &mode, 1, uti, nSim);
        }
        else
        {
        LOCAL_ERROR:
            CFW_SatResponse(SIM_SAT_PROVIDE_LOCAL_INFO_COM, 0x20, 0, 0, 0, uti, nSim);
        }
    }
    break;
    case SIM_SAT_SEND_SMS_COM:
    {
        uint8_t nFormat = 0;
        uint8_t nLengthPDU = 0;
        uint8_t *pPDUPacket = NULL;
        uint8_t data[255] = {0};
        uint8_t data_len = 255;
        if (CFW_SatGetSendSmsData(data, &data_len, nSim) == false)
        SMS_ERROR:
        {
            CFW_SatResponse(SIM_SAT_SEND_SMS_COM, 0x20, 0, 0, 0, uti, nSim);
            return;
        }
            uint8_t address[32] = {0};
        uint8_t addr_len = 32;
        uint8_t type = 0;
        if (CFW_SatGetSendSmsSCInfo(address, &addr_len, &type, nSim) == false)
            goto SMS_ERROR;

        OSI_LOGI(0, "data_len = %u, addr_len = %u", data_len, addr_len);
        if (0 == addr_len)
            pPDUPacket = (uint8_t *)malloc(12 + data_len);
        else
            pPDUPacket = (uint8_t *)malloc(2 + addr_len + data_len);
        if (NULL == pPDUPacket)
            goto SMS_ERROR;

        if (0 == addr_len) //如果没有传地址上来，就取得短信息中心号码
        {
            CFW_SMS_PARAMETER smsInfo;
            memset(&smsInfo, 0, sizeof(CFW_SMS_PARAMETER));
            CFW_CfgGetSmsParam(&smsInfo, 0, nSim);
            if (smsInfo.nNumber[0] > 11)
            {
                OSI_LOGI(0x10004f57, "smsInfo.nNumber[0] = %u, is error!", smsInfo.nNumber[0]);
                free(pPDUPacket);
                pPDUPacket = NULL;
                goto SMS_ERROR;
            }
            memcpy(pPDUPacket, smsInfo.nNumber, smsInfo.nNumber[0] + 1);
            nLengthPDU = smsInfo.nNumber[0] + 1;
        }
        else //如果传上来地址，在前面加91 代表加号
        {
            pPDUPacket[0] = addr_len + 1; //one byte for  the address type
            pPDUPacket[1] = type;
            memcpy(pPDUPacket + 2, address, addr_len);
            nLengthPDU = addr_len + 2;
        }
        memcpy(pPDUPacket + nLengthPDU, data, data_len);
        nLengthPDU += data_len;
        OSI_LOGXI(OSI_LOGPAR_M, 0, "%X", nLengthPDU, pPDUPacket);

        CFW_CfgGetSmsFormat(&nFormat, nSim);
        CFW_CfgSetSmsFormat(0, nSim);
        CFW_DIALNUMBER_V2 dummy;
        memset(&dummy, 0x00, sizeof(CFW_DIALNUMBER_V2));
        uint16_t nUti = cfwRequestUTI((osiEventCallback_t)SATSendSms_SendRspCB, NULL);
        uint32_t ret = CFW_SmsSendMessage_V2(&dummy, pPDUPacket, nLengthPDU, nUti, nSim);
        CFW_CfgSetSmsFormat(nFormat, nSim);
        if (ret == ERR_SUCCESS)
        {
            OSI_LOGI(0x10004f58, "CFW_SmsSendMessage success!");
        }
        else
            OSI_LOGI(0x10004f5a, "CFW_SmsSendMessage error(0x%x)!", ret);

        free(pPDUPacket);
        gATSATSendSMSFlag[nSim] = true;
        break;
    }
    case SIM_SAT_GET_INKEY_COM:
    case SIM_SAT_GET_INPUT_COM:
    case SIM_SAT_SELECT_ITEM_COM:
    {
        char response[30] = {0};
        sprintf((char *)response, "^STN: %d", (int)cfw_event->nParam1);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "response == %s", response);
        atCmdRespSimUrcText(nSim, response);
    }
    break;
    default:
        CFW_SatResponse(cfw_event->nParam1, 0x30, 0, 0, 0, uti, nSim);
        OSI_LOGI(0, "The command(0x%x) does not support!", cfw_event->nParam1);
        auto_process = 0;
        break;
    }
}

#ifndef AT_STSF
void atCmdHandleSTSF(atCommand_t *cmd)
{
}

//extern uint32_t CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);
void atCmdHandleSTA(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        //uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || (cmd->params[0]->length != 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#if 0
        if (mode == 1)
            gATCurrentAlphabet = ALPHA_SET;
        else if (mode == 0)
            gATCurrentAlphabet = HEX_SET;
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
#endif
        if (AllowedInstance == NONE_INSTANCE)
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_sta, cmd);
            uint32_t retval = CFW_SatActivation(0, cmd->uti, nSim);
            if (retval != ERR_SUCCESS)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
                RETURN_FOR_ASYNC();
        }
        else
            RETURN_OK(cmd->engine);
        OSI_LOGI(0x100045c1, "STA EXEC ok");
        return; // ERR_SUCCESS;
    }
    case AT_CMD_TEST:
    {
        const char *response = "^STA: (0,1)";
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_READ:
    {
        CFW_PROFILE pProfile;
        char response[100] = {0};
        char *pDest = response;

        //est += sprintf(response, "^STA: %u,%u,", gATCurrentAlphabet, AllowedInstance);
        // Adjust the code because csw code has been upgrade.
        CFW_SimGetMeProfile(&pProfile, nSim);
        cfwHexStrToBytes(pProfile.pSat_ProfileData, pProfile.nSat_ProfileLen, pDest);
        OSI_LOGXI(OSI_LOGPAR_SI, 0x100045c2, "STA=(%s),LEN=%d", response, pProfile.nSat_ProfileLen);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#else
void atCmdHandleSTA(atCommand_t *cmd)
{
    RETURN_OK(cmd->engine);
}

//Start For STSF command
#define STK_ACTIVATES_PROFILE 0x00
#define STK_ACTIVATES 0x01
#define STK_DEACTIVATES 0x02
uint8_t g_STK_Profile = 0;    //This variable used to mark whether the profile Download has executed.
uint8_t g_Profile_Change = 0; //This variable used to mark whether the profile data has changed or not

extern uint32_t CFW_SimGetMeProfile(CFW_PROFILE *pMeProfile, CFW_SIM_ID nSimID);
void atCmdHandleSTSF(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t result = ERR_AT_CME_PARAM_INVALID;

    switch (cmd->type == AT_CMD_SET)

        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            uint8_t Mode = 0;
            uint8_t ProfileBuffer[12] = {0};
            uint16_t TimeOut = 0;
            uint8_t AutoResp = 0;

            uint8_t NumOfParam = 0;
            INT32 RetValue = 0;

            if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &NumOfParam))
                goto STSF_ERROR;

            OSI_LOGI(0x100045c4, "STSF: Verfy number of parameter!NumOfParam = %d", NumOfParam);
            if ((NumOfParam == 0) || (NumOfParam > 4))
                goto STSF_ERROR;

            uint16_t uSize = 1;
            RetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_uint8_t, &Mode, &uSize);
            if ((ERR_SUCCESS != RetValue) || (uSize > 1) || (Mode > 2))
            {
                OSI_LOGI(0x100045c5, "STSF:ERROR!Get first parameter is err.");
                goto STSF_ERROR;
            }

            if (NumOfParam > 1)
            {
                uSize = 12;
                RetValue = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &ProfileBuffer, &uSize);
                OSI_LOGI(0x100045c6, "============RetValue = %d,uSize =%d============", RetValue, uSize);

                if ((RetValue == ERR_SUCCESS) && (uSize < 11))
                {
                    uint8_t ProfileDownload[6] = {0};
                    uint8_t Length = SUL_ascii2hex((uint8_t *)ProfileBuffer, (uint8_t *)ProfileDownload);
                    OSI_LOGI(0x100045c7, "Length = %d,uSize = %d", Length, uSize);
                    if ((Length == 0) || (Length != (uSize >> 1)))
                        goto STSF_ERROR;
                    CFW_SimSetMeProfile((uint8_t *)ProfileDownload, Length);
                    SUL_MemCopy8(STK_ProfileBuffer, ProfileBuffer, uSize); //For Read command
                    g_Profile_Change = 1;
                }
                else if (ERR_AT_UTIL_CMD_PARA_NULL == RetValue)
                    STK_ProfileBuffer[0] = 0;
                else
                    goto STSF_ERROR;
            }
            if (NumOfParam > 2)
            {
                uSize = 2;
                RetValue = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_uint16_t, &TimeOut, &uSize);
                OSI_LOGI(0x100045c8, "RetValue = %d--uSize = %d", RetValue, uSize);
                if ((RetValue != ERR_SUCCESS) || (TimeOut > 255))
                    goto STSF_ERROR;
                STK_TimeOut = TimeOut;
            }

            if (NumOfParam > 3)
            {
                uSize = 1;
                RetValue = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_uint8_t, &AutoResp, &uSize);
                OSI_LOGI(0x100045c9, "AutoResp = %d, uSize = %d", AutoResp, uSize);
                if ((RetValue != ERR_SUCCESS) || (uSize > 1) || ((AutoResp & 0xFE) != 0))
                    goto STSF_ERROR;
                AutoResp = 1;
            }

            if (Mode == 0) //Deactives STK
            {
                if (ERR_SUCCESS != CFW_SatActivation(STK_DEACTIVATES, pParam->nDLCI, nSim))
                    goto STSF_ERROR;
                if (g_Profile_Change == 1)
                    g_STK_Profile = 0;
            }
            else if (Mode == 1) //Activates STK
            {
                if (g_STK_Profile == 1)
                {
                    if (ERR_SUCCESS != CFW_SatActivation(STK_ACTIVATES_PROFILE, pParam->nDLCI, nSim))
                        goto STSF_ERROR;
                    g_STK_Profile = 1;
                    g_Profile_Change = 0;
                }
                else
                {
                    if (ERR_SUCCESS != CFW_SatActivation(STK_ACTIVATES, pParam->nDLCI, nSim))
                        goto STSF_ERROR;
                }
            }
            STK_Mode = Mode;
            AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, "", pParam->nDLCI);
            return;
        }

        case AT_CMD_TEST:
        {
            uint8_t nResp[] = "+STSF: (0-2),(160060C01F-5FFFFFFF7F),(1-255),(0-1)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, pParam->nDLCI);
            return;
        }

        case AT_CMD_READ:
        {
            CFW_PROFILE pProfile;
            uint8_t nResp[80] = {0};
            uint8_t ProfileBuffer[10] = {0};
            uint8_t i = 0, j;
            i += sprintf(nResp, "+STSF: %u", STK_Mode);

            if (STK_ProfileBuffer[0] == 0)
            {
                CFW_SimGetMeProfile(&pProfile, nSim);
                j = cfwHexStrToBytes(pProfile.pSat_ProfileData, pProfile.nSat_ProfileLen, ProfileBuffer);
                if (j == 0)
                    goto STSF_ERROR;

                i += sprintf(nResp + i, ",%s", ProfileBuffer);
                OSI_LOGXI(OSI_LOGPAR_SI, 0x100045ca, "ProfileDownload = %s, Length = %d", ProfileBuffer, pProfile.nSat_ProfileLen);
            }
            else
            {
                i += sprintf(nResp + i, ",%s", STK_ProfileBuffer);
                OSI_LOGXI(OSI_LOGPAR_SII, 0x100045cb, "ProfileDownload = %s, Length = %d,i = %d", STK_ProfileBuffer, AT_StrLen(STK_ProfileBuffer), i);
            }
            i += sprintf(nResp + i, ",%d,%d", STK_TimeOut, STK_AutoResp);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, pParam->nDLCI);
            return;
        }
        default:
            goto STSF_ERROR;
        }

STSF_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}
//End For STSF command
#endif

void atCmdHandleSTR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if ((cmd->param_count < 2) || (cmd->param_count > 5))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        uint8_t cmd_type = atParamUintInRange(cmd->params[0], 1, 255, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (((cmd_type & 0xFE) == 0xFE) && (cmd->param_count == 3))
        {
            bool bRet = CFW_GetSATIndFormate();
            if (CFW_SAT_IND_PDU == bRet)
            {
                const char *pdu_data = atParamStr(cmd->params[2], &paramok);
                if (!paramok)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                uint8_t data[255] = {0};
                uint16_t length = at_ascii2hex((uint8_t *)pdu_data, data);

                cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_stk_response, cmd);
                uint32_t retval = CFW_SatResponse(cmd_type, 0x00, 0x00, data, length, cmd->uti, nSim);
                if (ERR_SUCCESS != retval)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                RETURN_FOR_ASYNC();
            }
        }

        uint8_t status = atParamUintInRange(cmd->params[1], 0, 255, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        OSI_LOGI(0, "status = %d", status);

        uint8_t item = 0;
        if (cmd->param_count > 2)
        {
            item = atParamDefUintInRange(cmd->params[2], 0, 0, 255, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        OSI_LOGI(0, "item = %d", item);
        // Get fourth parameter
        const char *data = 0;
        if (cmd->param_count > 3)
        {
            data = atParamStr(cmd->params[3], &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        OSI_LOGI(0, "data = %x", data);
        uint16_t length = 0;
        if (data != 0)
            length = strlen((const char *)data);
        OSI_LOGI(0, "length = %d", length);

        int scheme;
        if (cmd->param_count > 4)
        {
            scheme = atParamDefInt(cmd->params[4], 0x04, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
            scheme = 0x04;
        OSI_LOGI(0, "scheme = %d", scheme);

        uint8_t stk_cmd = 0;
        uint8_t qualifier = 0;
        if (CFW_SatGetCurrentCmd(&stk_cmd, &qualifier, nSim) == false)
        {
            OSI_LOGI(0, "AT call CFW_SatGetCurrentCmd failed");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
        if (stk_cmd != cmd_type)
        {
            OSI_LOGI(0, "User input command is %d, wait response command is %d", cmd_type, stk_cmd);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        uint8_t *cmd_data = NULL;
        uint32_t retval = 0;
        cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_stk_response, cmd);
        if (cmd_type == SIM_SAT_SELECT_ITEM_COM)
        {
            retval = CFW_SatResponse(cmd_type, status, item, 0, 0, cmd->uti, nSim);
            OSI_LOGI(0, "AT response SIM_SAT_SELECT_ITEM_COM");
        }
        else if (cmd_type == SIM_SAT_GET_INKEY_COM)
        {
            //          if(qualifier & 0x01) == 0)
            uint8_t *inkey = malloc(length + 1);
            if (inkey == NULL)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            memset(inkey, 0x00, length + 1);

            inkey[0] = scheme;
            memcpy(inkey + 1, data, length);
            length++;
            retval = CFW_SatResponse(cmd_type, status, item, (void *)inkey, length, cmd->uti, nSim);

            if (inkey != NULL)
            {
                memset(inkey, 0x00, length + 1);
                free(inkey);
                inkey = NULL;
            }

            OSI_LOGI(0, "AT response SIM_SAT_GET_INKEY_COM");
        }
        else if (cmd_type == SIM_SAT_GET_INPUT_COM)
        {
            uint8_t nMin, nMax;
            CFW_SAT_INPUT_RSP *pGetInput = (CFW_SAT_INPUT_RSP *)cmd_data;
            nMax = pGetInput->nResMaxLen;
            nMin = pGetInput->nResMinLen;

            OSI_LOGI(0x100045e1, "max = %u,min=%u,Len=%u", nMax, nMin, length);
            if ((length > nMax) || (length < nMin))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            uint8_t *input = malloc(length + 1);
            if (input == NULL)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            memset(input, 0x00, length + 1);

            input[0] = scheme;
            memcpy(input + 1, data, length);
            length++; //Plus one byte of sch
            retval = CFW_SatResponse(cmd_type, status, item, (void *)input, length, cmd->uti, nSim);
            if (input != NULL)
            {
                memset(input, 0x00, length + 1);
                free(input);
                input = NULL;
            }

            OSI_LOGI(0, "AT response SIM_SAT_GET_INKEY_COM");
        }
        OSI_LOGI(0, "CFW_SatResponse: retval = %d", retval);
        if (retval == ERR_SUCCESS)
            RETURN_FOR_ASYNC();
        else
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }
    }
    case AT_CMD_TEST:
    {
        const char *response = "^STR: (16,19,33,35,36,37,38,211,254,255)";
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_READ:
    {
        uint8_t cmd_type = CFW_SatGetCurCMD(nSim);
        if (cmd_type == 0xFF)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        char response[15] = {0};
        sprintf(response, "^STR: %u", cmd_type);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    return;
}

#if 0
//
//Using to get STN message data
//we will save the STN message for android.
//
void atCmdHandleSTNR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t result = ERR_AT_CME_PARAM_INVALID;

    switch (cmd->type == AT_CMD_SET)
    {
        case AT_CMD_SET:
        {
#if 0
            uint8_t NumOfParam = 0;
            uint8_t *Num       = Param->pPara;
            uint8_t Buf[5];
            uint8_t Cmd, State, Item;
            uint8_t *InString = NULL;

            //CFW_SimListCountPbkEntries(CFW_PBK_SIM_FIX_DIALLING,1,2,3,0);

            CFW_SimInit( nSim );

            pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

            I_AM_HERE();

            AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }
            return;
#endif
            uint8_t nString[524] = {
                0x00,
            };
            sprintf(nString, "^STNR: %s", gDestAsciiDataSetUpMenu[nSim]);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nString, pParam->nDLCI);
            return;
        }
        case AT_CMD_TEST:
        {
            uint8_t nString[] = "^STNR: (Setup Menu, Display text.......)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nString, pParam->nDLCI);
            return;
        }
        case AT_CMD_READ:
        {
            uint8_t nString[524] = {
                0,
            };
            sprintf(nString, "^STNR: %s", gDestAsciiData[nSim]);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nString, pParam->nDLCI);
            return;
        }
        default:
            goto STNR_ERROR;
        }
    }

STNR_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}
#endif

//Do communciation work from RIL
#define AT_STRC_MAKE_CALL 0x01
#define AT_STRC_SEND_SMS 0x02
#define AT_STRC_SEND_USSD 0x03

#define AT_STRC_OK 0x01
#define AT_STRC_CANCEL 0x00

void atCmdHandleSTRC(atCommand_t *cmd)
{
    uint8_t sim_id = atCmdGetSim(cmd->engine);

    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count > 2)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || (mode > 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        uint8_t type = atParamUintInRange(cmd->params[1], 1, 3, &paramok);
        if (!paramok || (mode > 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        OSI_LOGI(0, "Checking parameters is passed");
        if (mode == AT_STRC_OK)
        {
            switch (type)
            {
#if 0
            case AT_STRC_SEND_SMS:
                AT_SAT_SpecialCMDSendSMS(gSATSpecialCMDData[nSimID], gSATSpecialCMDDataLen[nSimID], nSimID);
                break;

            case AT_STRC_SEND_USSD:
                AT_SAT_SpecialCMDSendUSSD(gSATSpecialCMDData[nSimID], gSATSpecialCMDDataLen[nSimID], nSimID);
                break;

        case AT_STRC_MAKE_CALL:
            if (gSATSpecialCMDData[sim_id] && gSATSpecialCMDDataLen[sim_id])
            {
                AT_SAT_SpecialCMDCallSetup(gSATSpecialCMDData[sim_id], gSATSpecialCMDDataLen[sim_id], sim_id);
            }
            else
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            break;
#endif

            default:
                break;
            }
        }
        else
        {
            uint8_t cmd_type;
            switch (type)
            {
            case AT_STRC_SEND_SMS:
                cmd_type = SIM_SAT_SEND_SMS_COM;
                break;
            case AT_STRC_SEND_USSD:
                cmd_type = SIM_SAT_SEND_USSD_COM;
                break;
            case AT_STRC_MAKE_CALL:
                cmd_type = SIM_SAT_CALL_SETUP_COM;
                break;
            default:
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_sta, cmd);
            uint32_t retval = CFW_SatResponse(cmd_type, 10, 0x00, NULL, 0x00, cmd->uti, sim_id);
            OSI_LOGI(0x100045ea, "User cancel nRet 0x%x", retval);
        }
        //free(gSATSpecialCMDData[sim_id]);
        //gSATSpecialCMDData[sim_id] = NULL;
        //gSATSpecialCMDDataLen[sim_id] = 0x00;
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_TEST:
    {
        const char *response = "^STRC: (AT_STRC_MAKE_CALL, AT_STRC_SEND_SMS, AT_STRC_SEND_USSD)";
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_READ:
    {
        uint8_t cmd_type = CFW_SatGetCurCMD(sim_id);
        if (cmd_type == 0xFF)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

        char response[15] = {0};
        sprintf(response, "^STRC: %u", cmd_type);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}
#if 0
void _OutputLocalLanuge(uint16_t *pUCS2, uint32_t nLen)
{
uint8_t nOutString;
uint8_t *pOutString = &nOutString;
uint32_t nOutStringLen;
uint16_t *p = (uint16_t *)pOutString;

if (nLen)
    ML_Unicode2LocalLanguageBigEnding((uint8_t *)pUCS2, nLen, &pOutString, &nOutStringLen, ML_CP936);
//AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, pOutString, 0);
OSI_LOGXI(OSI_LOGPAR_S, 0x100045eb, "pOutString %s", p);
OSI_LOGI(0x100045ec, "pOutString %x%x", pOutString[0], pOutString[1]);
free(pOutString);
}
#endif

void swap16bits(uint8_t *pOutput, uint8_t *pInput, uint8_t nLength)
{
    uint8_t i;
    for (i = 0; i < nLength; i += 2)
    {
        *(pOutput + i) = *(pInput + i + 1);
        *(pOutput + i + 1) = *(pInput + i);
        //AT_TC(g_sw_AT_SAT, "pOutString %02x%02x", *(pOutput + i) , *(pOutput + i + 1));
    }
}

void atCmdHandleSTGI(atCommand_t *cmd)
{
    return;
}

#if 0
uint16_t unicode2gbk(uint8_t *pOutput, uint8_t *pInput, uint8_t nLength)
{
    uint8_t k = 255;
    uint8_t output[255];

    swap16bits(output, pInput, nLength);
    //SUL_MemCopy8(output,pInput,nLength);
    uint8_t *p;
    OSI_LOGI(0x100045ed, "nLength = %d", nLength);
    //uint32_t nRet = ML_Unicode2LocalLanguage(output, nLength, &p, (uint32_t *)&k, "cp936");
    OSI_LOGI(0x100045ee, "p = %x, k = %d", p, k);
    //if (ERR_SUCCESS == nRet)
    {
        memcpy(pOutput, p, k);
        free(p);
        CSW_TC_MEMBLOCK(10, (uint8_t *)pOutput, 12, 16);
        pOutput += k;

        return k;
    }
    //else return 0;
}

//extern bool charset_gsm2ascii(uint8_t *src, uint16_t slen, uint8_t *dst, uint16_t *dlen);
void atCmdHandleSTGI(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t result = ERR_AT_CME_PARAM_INVALID;

    switch (cmd->type == AT_CMD_SET)
    {
        case AT_CMD_SET:
        {
            if(cmd->param_count != 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            bool paramok = true;
            uint8_t cmd_type = atParamUintInRange(cmd->params[0], 1, 255, &paramok);
            if (!paramok)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            cmd->uti = cfwRequestUTI((osiEventCallback_t)callback_sta, cmd);
            result = CFW_SatActivation(0, cmd->uti, nSim);

            OSI_LOGI(0, "cmd_type = %x", cmd_type);

            uint8_t mode = CFW_GetSATIndFormate();
            if (mode == CFW_SAT_IND_PDU)
            {
                uint8_t response[50] = {0,};
                uint8_t *q = response;
                q += sprintf(q, "^STGI: %u(Unsupported in PDU mode!)", cmd_type);
                atCmdRespInfoText(cmd->engine, response);
                RETURN_OK(cmd->engine);
            }
            else
            {
                uint32_t retval = 0;
                uint8_t* cmd_data = NULL;
                if ((retval = CFW_SatGetInformation(cmd_type, (void **)&cmd_data, nSim)) != ERR_SUCCESS)
                {
                    OSI_LOGI(0, "CMD STGI:ERROR!SatGetInformation is fail(%x)", retval);
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                OSI_LOGI(0x100045fc, "gATCurrentAlphabet = %d, GSM_SET = %d", gATCurrentAlphabet, GSM_SET);
                if (cmd_type == SIM_SAT_SETUP_MENU_COM || cmd_type == SIM_SAT_SELECT_ITEM_COM)
                {
                    uint8_t i;
                    uint32_t Count = 0;
                    uint16_t nNum, nMem;
                    uint8_t *q = NULL;

                    CFW_SAT_ITEM_LIST *pItemList = NULL;
                    p = (CFW_SAT_MENU_RSP *)cmd_data;
                    if (p == NULL)
STGI_ERROR:         {
                        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
                    }

                    pItemList = p->pItemList;
                    if (pItemList == NULL)
                        RETURN_OK(cmd->engine);


                    OSI_LOGI(0x100045fd, "pItemList = %x, %d, %d", pItemList, pItemList->nItemID, pItemList->nItemLen);
                    Count = AT_SAT_Count(pItemList);
                    nNum = Count >> 16;
                    nMem = Count & 0xFFFF;

                    OSI_LOGI(0x100045fe, "1nMem = %d,12*(uint8_t)nNum=%d,%d", nMem, 27 * (uint8_t)nNum, (p->nAlphaLen - 1) * 2 + 28);
                    nMem = (nMem << 1) + 19 * (uint8_t)nNum + (MEM_ALIGN_SIZE(p->nAlphaLen - 1) << 1) + 10 /*^STGI: 37,*/ + 2 /*""*/ + 12 /*3x???,*/;
                    OSI_LOGI(0x100045ff, "2nMem = %d", nMem);

                    char* pMessageBuf = (uint8_t *)malloc(nMem);
                    if (pMessageBuf == NULL)
                        goto STGI_ERROR;
                    memset(pMessageBuf, 0,  nMem);
                    q = pMessageBuf;
                    q += sprintf(q, "^STGI: %u,0,%u", cmd_type, nNum);

                    q += sprintf(q, ",\"");
                    if (p->nAlphaLen > 0)
                    {
                        bool nDone = 0;
                        uint16_t nOutLen = nMem;
                        OSI_LOGI(0x10004601, "p->nAlphaLen = %d", p->nAlphaLen);
                        if (gATCurrentAlphabet == ALPHA_SET)
                        {
                            OSI_LOGI(0x10004602, "p->pAlphaStr[0] = %x", p->pAlphaStr[0]);
                            if ((p->pAlphaStr[0] & 0x80) == 0)
                            {
                                charset_gsm2ascii(p->pAlphaStr, (uint16_t)p->nAlphaLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if (p->pAlphaStr[0] == 0x80)
                            {
                                OSI_LOGI(0x10004603, "q  adddress= %x", q);
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, p->pAlphaStr + 1, p->nAlphaLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(p->pAlphaStr, p->nAlphaLen, q);
                    }
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u\r\n", p->nComQualifier);
                    if (pItemList != NULL)
                    {
                        for (i = 0; i < nNum; i++)
                        {
                            bool nDone = 0;
                            uint16_t nOutLen = nMem;
                            OSI_LOGI(0x10004604, "nCmdType = %d,pItemList->nItemID = %d,nDone = %d", cmd_type, pItemList->nItemID, nDone);
                            q += sprintf(q, "%d,%d", cmd_type, pItemList->nItemID);

                            q += sprintf(q, ",\"");
                            if (pItemList->nItemLen > 0)
                            {
                                if (gATCurrentAlphabet == ALPHA_SET)
                                {
                                    OSI_LOGI(0x10004605, "pItemList->pItemStr[0] = %x", pItemList->pItemStr[0]);
                                    if ((pItemList->pItemStr[0] & 0x80) == 0)
                                    {
                                        charset_gsm2ascii(pItemList->pItemStr, (uint16_t)pItemList->nItemLen, q, (uint16_t *)&nOutLen);
                                        q += nOutLen;
                                        nDone = 1;
                                    }
                                }
                                else
                                {
                                    if (pItemList->pItemStr[0] == 0x80)
                                    {

                                        uint8_t *pStart = q;
                                        q += unicode2gbk(q, pItemList->pItemStr + 1, pItemList->nItemLen - 1);
                                        if (pStart != q)
                                            nDone = 1;
                                    }
                                }
                                if (nDone == 0)
                                    q += cfwHexStrToBytes(pItemList->pItemStr + 1, pItemList->nItemLen - 1, q);
                            }
                            q += sprintf(q, "\"");

                            q += sprintf(q, ",%u\r\n", p->nComQualifier);
                            OSI_LOGI(0x10004606, "pItemList->ID = %x", pItemList->nItemID);

                            if (pItemList->pNextItem != NULL)
                                pItemList = pItemList->pNextItem;
                            else
                                break;
                        }
                    }
                    *q = '\0';

                    OSI_LOGI(0x10004607, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    OSI_LOGI(0x10004608, "36-37nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = AT_StrLen(pMessageBuf);
                    OSI_LOGI(0x10004609, "pMessageBuf end Address = 0x%x", pMessageBuf + AT_StrLen(pMessageBuf));

                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == DISPLAY_TEXT_COM)
                {
                    uint16_t nMem;
                    CFW_SAT_DTEXT_RSP *pDText;
                    uint8_t *q = NULL;
                    pDText = (CFW_SAT_DTEXT_RSP *)CmdData;
                    if (pDText == NULL)
                        goto STGI_ERROR;
                    nMem = 2 * (uint8_t)(pDText->nTextLen) + 15 + 10;
                    pMessageBuf = (uint8_t *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        OSI_LOGI(0x1000460a, "CMD STGI:ERROR!nCmdType == 33 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;

                    q += sprintf(q, "^STGI: 33");
                    OSI_LOGI(0x1000460b, "pDText->nTextSch =%x", pDText->nTextSch);

                    bool nDone = 0;
                    uint16_t nOutLen = nMem;

                    q += sprintf(q, ",\"");
                    if (gATCurrentAlphabet == GSM_SET)
                    {
                        if ((pDText->nTextSch & 0xF4) == 0xF0) //GSM 7 bits
                        {
                            charset_gsm2ascii(pDText->pTextStr, (uint16_t)pDText->nTextLen, q, (uint16_t *)&nOutLen);
                            q += nOutLen;
                            nDone = 1;
                        }
                        else if ((pDText->nTextSch & 0xF4) == 0xF4) //8 bits
                        {
                            AT_MemCpy(q, pDText->pTextStr, pDText->nTextLen);
                            q += pDText->nTextLen;
                            nDone = 1;
                        }
                        else if ((pDText->nTextSch & 0x20) == 0)
                        {
                            if ((pDText->nTextSch & 0x0C) == 0) //GSM 7 bits
                            {
                                charset_gsm2ascii(pDText->pTextStr, (uint16_t)pDText->nTextLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                            else if ((pDText->nTextSch & 0x0C) == 0x04) //8 bits
                            {
                                AT_MemCpy(q, pDText->pTextStr, pDText->nTextLen);
                                q += pDText->nTextLen;
                                nDone = 1;
                            }
                        }
                    }
                    else
                    {
                        if ((pDText->nTextSch & 0x0C) == 0x08) //UCS2 as Chinese
                        {
                            uint8_t *pStart = q;
                            OSI_LOGI(0x1000460c, "pDText->nTextLen =%d", pDText->nTextLen);
                            OSI_LOGI(0x1000460d, "pDText->pTextStr =%x", pDText->pTextStr);
                            CSW_TC_MEMBLOCK(g_sw_AT_SAT, pDText->pTextStr, pDText->nTextLen, 16);
                            q += unicode2gbk(q, pDText->pTextStr, pDText->nTextLen);
                            if (pStart != q)
                                nDone = 1;
                        }
                    }
                    if (nDone == 0)
                        q += cfwHexStrToBytes(pDText->pTextStr, pDText->nTextLen, q);
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u,%u\r\n", pDText->nTextSch, pDText->nComQualifier);
                    *q = '\0';

                    OSI_LOGI(0x10004607, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    OSI_LOGI(0x1000460e, "33nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == PROVIDE_LOCAL_INFO_COM)
                {
                    SAT_BASE_RSP *pBase = NULL;
                    uint8_t nMessageBuf[20] = {0};
                    pBase = (SAT_BASE_RSP *)CmdData;
                    if (pBase == NULL)
                        goto STGI_ERROR;
                    sprintf(nMessageBuf, "^STGI: %u,%u", pBase->nComID, pBase->nComQualifier);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nMessageBuf, pParam->nDLCI);
                    return;
                }
                else if (nCmdType == SEND_SHORT_MESSAGE_COM)
                {
                    uint16_t nMem;
                    uint8_t *q = NULL;
                    CFW_SAT_SMS_RSP *pShortMsg;

                    pShortMsg = (CFW_SAT_SMS_RSP *)CmdData;
                    if (pShortMsg == NULL)
                        goto STGI_ERROR;
                    nMem = pShortMsg->nAlphaLen + pShortMsg->nAddrLen + pShortMsg->nPDULen;
                    nMem = 2 * nMem + 8 + 6 + 12;
                    pMessageBuf = (uint8_t *)AT_MALLOC(nMem);

                    if (pMessageBuf == NULL)
                    {
                        OSI_LOGI(0x1000460f, "CMD STGI:ERROR!nCmdType == 19 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += sprintf(q, "^STGI: 19");

                    OSI_LOGI(0x10004610, "pShortMsg->pAlphaStr[0] = %x", pShortMsg->pAlphaStr);
                    OSI_LOGI(0x10004611, "pShortMsg->nAlphaLen = %d", pShortMsg->nAlphaLen);

                    q += sprintf(q, ",\"");
                    if (pShortMsg->nAlphaLen > 0)
                    {
                        bool nDone = 0;
                        uint16_t nOutLen = nMem;

                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pShortMsg->pAlphaStr[0] & 0x80) == 0)
                            {
                                charset_gsm2ascii(pShortMsg->pAlphaStr, (uint16_t)pShortMsg->nAlphaLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if (pShortMsg->pAlphaStr[0] == 0x80)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pShortMsg->pAlphaStr + 1, pShortMsg->nAlphaLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(pShortMsg->pAlphaStr, pShortMsg->nAlphaLen, q);
                    }
                    q += sprintf(q, "\"");
                    q += sprintf(q, ",%u", pShortMsg->nAddrType);

                    q += sprintf(q, ",\"");
                    if (pShortMsg->nAddrLen > 0)
                    {
                        uint16_t nOutLen = nMem;
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            charset_gsm2ascii(pShortMsg->pAddrStr, (uint16_t)pShortMsg->nAddrLen, q, (uint16_t *)&nOutLen);
                            q += nOutLen;
                        }
                        else
                        {
                            q += SUL_GsmBcdToAscii(pShortMsg->pAddrStr, pShortMsg->nAddrLen, q);
                        }
                    }
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",\"");
                    if (pShortMsg->nPDULen > 0)
                        q += cfwHexStrToBytes(pShortMsg->pPDUStr, pShortMsg->nPDULen, q);
                    q += sprintf(q, "\"");

                    q += sprintf(q, "\r\n");

                    OSI_LOGI(0x10004607, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    OSI_LOGI(0x10004612, "19nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == SETUP_CALL_COM)
                {
                    uint16_t nMem = 0;
                    uint8_t *q = NULL;
                    CFW_SAT_CALL_RSP *pCall = (CFW_SAT_CALL_RSP *)CmdData;
                    if (pCall == NULL)
                        goto STGI_ERROR;
                    nMem = pCall->nAlphaConfirmLen + pCall->nAddrLen + pCall->nSubAddrLen + pCall->nAlphaSetupLen;
                    nMem = 2 * (nMem + 16) + 6 + 17;
                    pMessageBuf = (uint8_t *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        OSI_LOGI(0x10004613, "CMD STGI:ERROR!nCmdType == 16 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += sprintf(q, "^STGI: 16");

                    q += sprintf(q, ",\"");
                    if (pCall->nAlphaConfirmLen > 0)
                    {
                        bool nDone = 0;
                        uint16_t nOutLen = nMem;
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pCall->pAlphaConfirmStr[0] & 0x80) == 0)
                            {
                                charset_gsm2ascii(pCall->pAlphaConfirmStr, (uint16_t)pCall->nAlphaConfirmLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if (pCall->pAlphaConfirmStr[0] == 0x80)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pCall->pAlphaConfirmStr + 1, pCall->nAlphaConfirmLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(pCall->pAlphaConfirmStr, pCall->nAlphaConfirmLen, q);
                    }

                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u", pCall->nAddrType);

                    q += sprintf(q, ",\"");
                    if (pCall->nAddrLen > 0)
                        q += SUL_GsmBcdToAscii(pCall->pAddrStr, pCall->nAddrLen, q);
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",\"");
                    if (pCall->nSubAddrLen > 0)
                        q += SUL_GsmBcdToAscii(pCall->pSubAddrStr, pCall->nSubAddrLen, q);
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",\"");
                    OSI_LOGI(0x10004614, "pCall->nAlphaSetupLen = %d", pCall->nAlphaSetupLen);
                    CSW_TC_MEMBLOCK(g_sw_AT_SAT, pCall->pAlphaSetupStr, 50, 16);

                    if (pCall->nAlphaSetupLen > 0)
                    {
                        bool nDone = 0;
                        uint16_t nOutLen = nMem;
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pCall->pAlphaSetupStr[0] & 0x80) == 0)
                            {
                                charset_gsm2ascii(pCall->pAlphaSetupStr, (uint16_t)pCall->nAlphaSetupLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if (pCall->pAlphaSetupStr[0] == 0x80)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pCall->pAlphaSetupStr + 1, pCall->nAlphaSetupLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(pCall->pAlphaSetupStr, pCall->nAlphaSetupLen, q);
                    }
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u,%u,%u\r\n", pCall->nTuint, pCall->nTinterval, pCall->nComQualifier);
                    *q = '\0';

                    OSI_LOGI(0x10004607, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    OSI_LOGI(0x10004615, "nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == GET_INPUT_COM)
                {
                    uint16_t nMem;
                    uint8_t *q = NULL;
                    CFW_SAT_INPUT_RSP *pInput;

                    pInput = (CFW_SAT_INPUT_RSP *)CmdData;
                    if (pInput == NULL)
                        goto STGI_ERROR;
                    nMem = pInput->nDefTextLen + pInput->nTextLen;
                    nMem = 2 * (nMem + 3) + 28 + 30; /* 3; */
                    pMessageBuf = (uint8_t *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        OSI_LOGI(0x10004616, "CMD STGI:ERROR!nCmdType == 35 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += sprintf(q, "^STGI: 35");

                    bool nDone = 0;
                    uint16_t nOutLen = nMem;
                    q += sprintf(q, ",\"");
                    if (pInput->nTextLen > 0)
                    {
                        OSI_LOGI(0x10004617, "CMD STGI:pInput->nTextSch = %x", pInput->nTextSch);
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pInput->nTextSch & 0xF4) == 0xF0)
                            {
                                charset_gsm2ascii(pInput->pTextStr, (uint16_t)pInput->nTextLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                            else if ((pInput->nTextSch & 0xF4) == 0xF4)
                            {
                                AT_MemCpy(q, pInput->pTextStr, pInput->nTextLen);
                                q += pInput->nTextLen;
                                nDone = 1;
                            }
                            else if ((pInput->nTextSch & 0x20) == 0)
                            {
                                if ((pInput->nTextSch & 0x0C) == 0)
                                {
                                    charset_gsm2ascii(pInput->pTextStr, (uint16_t)pInput->nTextLen, q, (uint16_t *)&nOutLen);
                                    q += nOutLen;
                                    nDone = 1;
                                }
                                else if ((pInput->nTextSch & 0x0C) == 0x04)
                                {
                                    AT_MemCpy(q, pInput->pTextStr, pInput->nTextLen);
                                    q += pInput->nTextLen;
                                    nDone = 1;
                                }
                            }
                        }
                        else
                        {
                            if ((pInput->nTextSch & 0x0C) == 0x08)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pInput->pTextStr, pInput->nTextLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(pInput->pTextStr, pInput->nTextLen, q);
                    }
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u", pInput->nDefTextSch);
                    nOutLen = nMem;

                    q += sprintf(q, ",\"");
                    if (pInput->nDefTextLen > 0)
                    {
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pInput->nDefTextSch & 0xF4) == 0xF0)
                            {
                                charset_gsm2ascii(pInput->pDefTextStr, (uint16_t)pInput->nDefTextLen, q, (uint16_t *)&nOutLen);
                                q += nOutLen;
                                nDone = 1;
                            }
                            else if ((pInput->nDefTextSch & 0xF4) == 0xF4)
                            {
                                AT_MemCpy(q, pInput->pDefTextStr, pInput->nDefTextLen);
                                q += pInput->nTextLen;
                                nDone = 1;
                            }
                            else if ((pInput->nDefTextSch & 0x20) == 0)
                            {
                                if ((pInput->nDefTextSch & 0x0C) == 0)
                                {
                                    charset_gsm2ascii(pInput->pDefTextStr, (uint16_t)pInput->nDefTextLen, q, (uint16_t *)&nOutLen);
                                    q += nOutLen;
                                    nDone = 1;
                                }
                                else if ((pInput->nDefTextSch & 0x0C) == 0x04)
                                {
                                    AT_MemCpy(q, pInput->pDefTextStr, pInput->nDefTextLen);
                                    q += pInput->nTextLen;
                                    nDone = 1;
                                }
                            }
                        }
                        else
                        {
                            if ((pInput->nDefTextSch & 0x0C) == 0x08)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pInput->pDefTextStr, pInput->nDefTextLen - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                            q += cfwHexStrToBytes(pInput->pDefTextStr, pInput->nDefTextLen, q);
                    }
                    q += sprintf(q, "\"");

                    q += sprintf(q, ",%u", pInput->nDefTextSch);
                    q += sprintf(q, ",%u,%u,%u", pInput->nResMaxLen, pInput->nResMinLen, pInput->nComQualifier);
                    q += sprintf(q, "\r\n");
                    *q = '\0';
                    OSI_LOGI(0x10004607, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    OSI_LOGI(0x10004618, "35nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == SEND_SS_COM)
                {
                    OSI_LOGI(0x10004619, "SEND_SS_COM");
                }
                else if (nCmdType == SEND_USSD_COM)
                {
                    OSI_LOGI(0x1000461a, "SEND_USSD_COM");
                }
                else if (nCmdType == LAUNCH_BROWSER_C_COM)
                {
                    uint16_t nMem;
                    OSI_LOGI(0x1000461b, "LAUNCH_BROWSER_C_COM");
                    CFW_SAT_BROWSER_RSP *pLB = (CFW_SAT_BROWSER_RSP *)CmdData;
                    if (pLB == NULL)
                        goto STGI_ERROR;

                    OSI_LOGI(0x1000461c, "pLB->URL Length =%d", pLB->nURLLength);
                    nMem = (pLB->nAlphaLength << 1) + (pLB->nTextLength << 1) + (pLB->nURLLength << 1);

                    nMem += (pLB->nTextLength << 1);
                    uint8_t i = 0;
                    for (i = 0; i < pLB->nProvisioningFileRefNum; i++)
                    {
                        nMem += ((pLB->pProvisioningFileRef[i])[0] << 1);
                    }

                    for (i = 0; i < pLB->nURLLength; i++)
                        OSI_LOGI(0x1000461d, "%c-%x", pLB->pURL[i], pLB->pURL[i]);
                    nMem += 30; //plus ^STGI: 21,"," ...
                    OSI_LOGI(0x1000461e, "LAUNCH_BROWSER: nMem =%d", nMem);

                    pMessageBuf = (uint8_t *)AT_MALLOC(nMem);

                    AT_MemZero(pMessageBuf, nMem);

                    uint8_t *q = pMessageBuf;
                    //Command ID, Qualifier
                    q += sprintf(pMessageBuf, "^STGI: %d,%d,", pLB->nComID, pLB->nComQualifier);
                    //Browser Identity
                    if (pLB->nBrowserIdentity == 0)
                        q += sprintf(q, "0,"); //Default browser
                    else
                        q += sprintf(q, ",");
                    //URL
                    if (pLB->nURLLength != 0)
                    {
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            AT_MemCpy(q, pLB->pURL, pLB->nURLLength);
                            q += pLB->nURLLength;
                        }
                        else
                        {
                            q += cfwHexStrToBytes(pLB->pURL, pLB->nURLLength, q);
                        }
                    }
                    //Gateway/Proxy
                    //scheme, gateway or proxy
                    OSI_LOGI(0x1000461f, "LAUNCH_BROWSER: pLB->nTextLength =%d", pLB->nTextLength);
                    if (pLB->nTextLength != 0)
                    {
                        q += sprintf(q, ",%d,", pLB->nCodeScheme);
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            AT_MemCpy(q, pLB->nTextString, pLB->nTextLength);
                            q += pLB->nTextLength;
                        }
                        else
                        {
                            q += cfwHexStrToBytes(pLB->nTextString, pLB->nTextLength, q);
                        }
                        q += sprintf(q, ",");
                    }
                    else
                    {
                        q += sprintf(q, ",,,");
                    }
                    //alphaidentifier
                    OSI_LOGI(0x10004620, "LAUNCH_BROWSER: pLB->nAlphaLength =%d", pLB->nAlphaLength);
                    if (pLB->nAlphaLength != 0)
                    {
                        bool nDone = 0;
                        if (gATCurrentAlphabet == GSM_SET)
                        {
                            if ((pLB->nAlphaIdentifier[0] & 0x80) == 0)
                            {
                                AT_MemCpy(q, pLB->nAlphaIdentifier, pLB->nAlphaLength);
                                q += pLB->nAlphaLength;

                                nDone = 1;
                            }
                        }
                        else
                        {
                            if (pLB->nAlphaIdentifier[0] == 0x80)
                            {
                                uint8_t *pStart = q;
                                q += unicode2gbk(q, pLB->nAlphaIdentifier + 1, pLB->nAlphaLength - 1);
                                if (pStart != q)
                                    nDone = 1;
                            }
                        }
                        if (nDone == 0)
                        {
                            OSI_LOGI(0x10004621, "----------Hex---------");
                            q += cfwHexStrToBytes(pLB->nAlphaIdentifier, pLB->nAlphaLength, q);
                        }
                    }

                    //Provision File Reference
                    if (pLB->nProvisioningFileRefNum != 0)
                    {
                        q += sprintf(q, ",%d,", pLB->nProvisioningFileRefNum);
                        for (i = 0; i < pLB->nProvisioningFileRefNum; i++)
                        {
                            if (gATCurrentAlphabet == GSM_SET)
                            {
                                AT_MemCpy(q, pLB->pProvisioningFileRef[i] + 1, pLB->pProvisioningFileRef[i][0]);
                                q += pLB->pProvisioningFileRef[i][0];
                            }
                            else
                            {
                                q += cfwHexStrToBytes(pLB->pProvisioningFileRef[i] + 1, pLB->pProvisioningFileRef[i][0], q);
                            }
                        }
                        q += sprintf(q, ",");
                    }
                    else
                    {
                        q += sprintf(q, ",0,,");
                    }

                    //IconQualifier
                    OSI_LOGI(0x10004622, "LAUNCH_BROWSER_C_COM: pLB->nIconQualifier =%x", pLB->nIconQualifier);
                    if (pLB->nIconQualifier != 0xFF)
                    {
                        q += sprintf(q, "%d,", pLB->nIconQualifier);
                    }
                    else
                    {
                        q += sprintf(q, ",");
                    }

                    //IconIdentifier
                    OSI_LOGI(0x10004623, "LAUNCH_BROWSER_C_COM: pLB->nIconIdentifier =%x", pLB->nIconIdentifier);
                    if (pLB->nIconIdentifier != 0xFF)
                    {
                        q += sprintf(q, "%d", pLB->nIconIdentifier);
                    }

                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pMessageBuf, pParam->nDLCI);
                    OSI_LOGI(0x10004624, "LAUNCH_BROWSER_C_COM: nMem =%d", nMem);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else
                    Len = 0;
            }
            RETURN_OK(cmd->engine);
        }
        case AT_CMD_TEST:
        {
            uint8_t nString[] = "^STGI: (16,19,33,35,36,37,38,211)";
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        break;
        case AT_CMD_READ:
        {
            uint8_t response[50];
            uint8_t nCmd = CFW_SatGetCurCMD(nSim);
            if (nCmd == 0xFF)
                goto STGI_ERROR;
            sprintf(response, "^STGI: %u", nCmd);
            atCmdRespInfoText(cmd->engine, response);
            RETURN_OK(cmd->engine);
        }
        break;
        default:
            goto STGI_ERROR;
        }
    }
    return;
}
#endif
void at_item_info(CFW_SAT_ITEM_LIST *pMenu, uint8_t *num, uint16_t *mem_size)
{
    CFW_SAT_ITEM_LIST *p = pMenu;
    uint16_t nMemSum = 0;
    uint16_t i = 0;

    while (p != NULL)
    {
        nMemSum += MEM_ALIGN_SIZE(p->nItemLen - 1);
        i++;
        p = p->pNextItem;
    }
    OSI_LOGI(0x10004625, "Item number = %d, nMemSum = %d", i, nMemSum);
    *num = i;
    *mem_size = nMemSum;
    return;
}

void atCmdHandleSTF(atCommand_t *cmd)
{
    //uint8_t nSim = atCmdGetSim(cmd->engine);
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        if (cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || (cmd->params[0]->length != 1))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode > 1)
        {
            OSI_LOGI(0x100052c2, "AT^STF:nService : %d !", mode);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        const char *response = NULL;
        if (CFW_SAT_IND_PDU == mode)
            response = "^STF: PDU Mode";
        else if (CFW_SAT_IND_TEXT == mode)
            response = "^STF: TEXT Mode";
        else
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        CFW_SetSATIndFormate(mode);
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    break;
    case AT_CMD_TEST:
    {
        const char *response = "^STF: (0,1)";
        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_READ:
    {
        const char *response = NULL;
        if (CFW_SAT_IND_PDU == CFW_GetSATIndFormate())
            response = "^STF: PDU mode";
        else // if(CFW_SAT_IND_TEXT == CFW_GetSATIndFormate())
            response = "^STF: TEXT mode";

        atCmdRespInfoText(cmd->engine, response);
        RETURN_OK(cmd->engine);
    }
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
    return;
}
