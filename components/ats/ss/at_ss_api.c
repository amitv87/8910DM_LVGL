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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "cfw.h"
#include "at_cfw.h"
#include <at_cfg.h>
#include "ats_config.h"
#include "at_cmd_ss.h"

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

// =========================================================
// GsmToBcd:If an odd number is converted, 0 will be added to the corresponding high order.
// Support: '*' '#' 'p'
// 13811189836 --> 0x31 0x18 0x11 0x98 0x38 0xF6
// =========================================================
uint16_t AT_SsAsciiToGsmBcd(const char *pNumber,              // input
                            uint8_t nNumberLen, uint8_t *pBCD // output should >= nNumberLen/2+1
)
{
    uint8_t Tmp;
    uint32_t i;
    uint32_t nBcdSize = 0;
    uint8_t *pTmp = pBCD;

    if (pNumber == NULL || pBCD == (uint8_t *)NULL)
    {
        return false;
    }

    memset(pBCD, 0, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        switch (*pNumber)
        {

        case '*':
            Tmp = (uint8_t)0x0A;
            break;

        case '#':
            Tmp = (uint8_t)0x0B;
            break;

        case 'p':
            Tmp = (uint8_t)0x0C;
            break;

        default:
            Tmp = (uint8_t)(*pNumber - '0');
            break;
        }

        if (i % 2)
        {
            *pTmp++ |= (Tmp << 4) & 0xF0;
        }
        else
        {
            *pTmp = (uint8_t)(Tmp & 0x0F);
        }

        pNumber++;
    }

    nBcdSize = nNumberLen >> 1;

    if (i % 2)
    {
        *pTmp |= 0xF0;
        nBcdSize += 1;
    }

    return nBcdSize;
}

#if 0
    // ////////////////////////////////////////////////////////////////////////////
    // Name:
    // Description: change display to real class
    //
    //
    // Parameter:
    // Caller:
    // Called:
    // Return:
    // Remark:
    //
    // ////////////////////////////////////////////////////////////////////////////
    VOID
    AT_SsClassConvertCcfc(UINT8 ucSrcClass, UINT8 *pucDestClass)
{

    switch (ucSrcClass)
    {

    case 1: // voice
        *pucDestClass = 11;
        break;

    case 2: // data
        *pucDestClass = 20;
        break;

    case 4: // fax
        *pucDestClass = 13;
        break;

    case 8: // sms
        *pucDestClass = 16;
        break;

    default:
        *pucDestClass = 11;
        break;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: change class to display
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////
void AT_SsClassConvert2Ccfc(uint8_t ucSrcClass, uint8_t *pucDestClass)
{
    OSI_LOGI(0x10004506, "<><><><><><>:%d\n", ucSrcClass);

    switch (ucSrcClass)
    {

    case 11:               // voice
        *pucDestClass = 1; // real value 2^0
        break;

    case 13:               // fax
        *pucDestClass = 4; // 2^1
        break;

    case 16: // sms
        *pucDestClass = 8;
        break;

    case 20: // data
        *pucDestClass = 2;
        break;

    default:
        *pucDestClass = 1;
        break;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: process event respond success return
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SsEventSuccess(AT_CMD_RESULT **ppResult, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI)
{

    *ppResult = AT_CreateRC(CMD_FUNC_SUCC,
                            CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, pBuf, uiBufLen, nUTI);
    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: process event response fail return
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SsEventFail(AT_CMD_RESULT **ppResult, UINT32 errCode, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI)
{

    *ppResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_ERROR, errCode, // CMD_ERROR_CODE_OK,
                            CMD_ERROR_CODE_TYPE_CME, 0, pBuf, uiBufLen, nUTI);
    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: call interface fail return
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SsFuncFail(AT_CMD_RESULT **ppResult, UINT32 errCode, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI)
{
    *ppResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, errCode, CMD_ERROR_CODE_TYPE_CME, 0, pBuf, uiBufLen, nUTI);
    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: call interface success return
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SsFuncSuccess(AT_CMD_RESULT **ppResult, UINT32 returnValue, UINT8 delaytime, UINT8 *pBuf, UINT8 uiBufLen,
                      UINT8 nUTI)
{
    *ppResult = AT_CreateRC(returnValue,
                            CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, delaytime, pBuf, uiBufLen, nUTI);
    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: call interface success return
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SsSynSuccessWithStr(AT_CMD_RESULT **ppResult, UINT8 *buf, UINT16 len, UINT8 nUTI)
{
    *ppResult = AT_CreateRC(CMD_FUNC_SUCC, CMD_RC_OK, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, buf, len, nUTI);
    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:
//
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

BOOL SS_String2PUCT(CFW_SIM_PUCT_INFO *pPUCT, UINT8 *FloatString)
{
    UINT8 i = 0;
    UINT8 dot = 0;
    pPUCT->iEPPU = 0;
    UINT8 length = strlen(FloatString);
    pPUCT->iSign = 0;

    for (i = 0; i < length; i++)
    {
        if (FloatString[i] == '.')
        {
            if (dot != 0)
                return FALSE;
            dot = i;
            pPUCT->iSign = 1;
            continue;
        }
        if ((FloatString[i] >= '0') && (FloatString[i] <= '9'))
            pPUCT->iEPPU = pPUCT->iEPPU * 10 + FloatString[i] - '0';
        else
            return FALSE;
        //      AT_TC(g_sw_AT_SS, "SS_PPUIntStructureToFloatStr,FloatString --%d\n", pPUCT->iEPPU);
    }
    if (dot != 0)
        pPUCT->iEX = i - dot - 1;
    else
        pPUCT->iEX = 0;
    if (pPUCT->iEPPU > 4096)
    {
        OSI_LOGI(0x10004507, "iEPPU value is larger than 4096, PUCT->iEPPU = %d\n", pPUCT->iEPPU);
        return FALSE;
    }
    //  AT_TC(g_sw_AT_SS, "SS_PPUIntStructureToFloatStr,iEPPU --%d, iEX--%d, iSign--%d", pPUCT->iEPPU, pPUCT->iEX,pPUCT->iSign);
    return TRUE;
}

void puct2string(CFW_SIM_PUCT_INFO *input, UINT8 *output)
{
    UINT16 quotient = input->iEPPU;
    UINT8 remainder[8] = {0};
    UINT8 i = 0;
    while (quotient != 0)
    {
        remainder[i++] = quotient % 10 + '0';
        quotient = quotient / 10;
    }
    UINT8 length = i;
    i = 0;
    UINT8 j = 0;

    if (input->iSign == 1)
    {
        if (input->iEX >= length)
        {
            output[i++] = '0';
            output[i++] = '.';
            j = input->iEX - length;
            while (j-- != 0)
                output[i++] = '0';

            j = 0;
            while (j++ < length)
                output[i++] = remainder[length - j];
        }
        else
        {
            while (j < length)
            {
                if (input->iEX == length - j)
                    output[i++] = '.';

                output[i++] = remainder[length - j - 1];
                j++;
            }
        }
        output[i] = 0;
    }
    else
    {
        j = 0;
        while (j++ < length)
            output[i++] = remainder[length - j];
        j = 0;
        while (j++ < input->iEX)
            output[i++] = '0';
        output[i] = 0;
    }
}
#endif

#if 0
BOOL SS_PUCT2String(UINT8* String, CFW_SIM_PUCT_INFO *pPUCT)
{
    UINT16 eppu = pPUCT->iEPPU;
    UINT8  ex = pPUCT->iEX;
    UINT8 buffer[20];       //The number of Float should be less than 20 digital.
    UINT8* p = buffer;
    UINT8 i = 0;
    INT8 dot = 0;
    memset(buffer,0,20);
    while(eppu != 0)        //calculate the number of digital
    {
        *(p++) = eppu %10 + '0';
        eppu = eppu / 10;
        i++;
    }
    INT8 length = i;
    i = 0;
    char j = 0;
    if(pPUCT->iSign == 1)
    {
        dot = ex - length;      // if the number < 1
        String[i++] = '0';
        String[i++] = '.';
        if(dot < 0)
            i = 0;
        else
            while(dot - j > 0)
            {
                String[i++] = '0';
                j++;
            }
        j = 0;
        dot = -1;
        while(length - j > 0)
        {
            if((j == length - ex) && ( j != 0))
            {
                String[i] = '.';
                dot++;
            }
            else
            {
                String[i] = buffer[length - j + dot];
            }
            i++;
            j++;
        }
        String[i] = '\0';
    }
    else        //it is not dot digital
    {
        for(i = 0; i < length; i++)
            String[i] = buffer[length - 1 - i];
        for(i = 0; i < ex; i++)
            String[length + i] = '0';
        String[length+i] = '\0';
    }
    return TRUE;
}
#endif
