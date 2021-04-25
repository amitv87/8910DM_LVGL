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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('A', 'S', 'M', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "cfw_errorcode.h"
#include "ml.h"
#include "audio_device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>

#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_SMS)
#include "ql_api_sms.h"
#endif
#include "quec_proj_config.h"
#include "quec_urc.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
#include "quec_atcmd_sms.h"
#endif
#define AT_SMS_STATUS_UNREAD 0
#define AT_SMS_STATUS_READ 1
#define AT_SMS_STATUS_UNSENT 2
#define AT_SMS_STATUS_SENT 3
#define AT_SMS_STATUS_ALL 4

static const osiValueStrMap_t gSmsState[] = {
    {CFW_SMS_STORED_STATUS_UNREAD, "REC UNREAD"},
    {CFW_SMS_STORED_STATUS_READ, "REC READ"},
    {CFW_SMS_STORED_STATUS_UNSENT, "STO UNSENT"},
    {CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ, "STO SENT"},
    {CFW_SMS_STORED_STATUS_STORED_ALL, "ALL"},
    {0, NULL},
};

static const osiValueStrMap_t gSmsNumState[] = {
    {AT_SMS_STATUS_UNREAD, "REC UNREAD"},
    {AT_SMS_STATUS_READ, "REC READ"},
    {AT_SMS_STATUS_UNSENT, "STO UNSENT"},
    {AT_SMS_STATUS_SENT, "STO SENT"},
    {AT_SMS_STATUS_ALL, "ALL"},
    {0, NULL},
};

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static uint8_t g_SMS_CIND_NewMsg = 0;
#else
uint8_t g_SMS_CIND_NewMsg = 0;
uint8_t qpara_cmms = 0;
#endif
// =============================================================================
// memfull according to different storage, we support storage
// "ME" and "SM", this gloable variable set flag as following:
// bit 7 6 5 4 3 2 1 0
// bit0  "ME" memfull flag, 0: not full 1: memfull
// bit1  "SM" memfull flag, 0: not full 1: memfull
// bit 2~bit7 reserved for other storage future
// =============================================================================
static uint8_t g_SMS_CIND_MemFull[2] = {0x00};

extern bool g_SMSConcat[CFW_SIM_COUNT];
uint8_t g_CmdLine_CMSS_Number = 0; //the number of +CMSS in input CMD line.

extern CFW_INIT_INFO cfwInitInfo;

bool AT_SMS_GetPDUNUM(const uint8_t *pPDUData, uint8_t *pPDUNUM, uint8_t *pType, uint8_t *nPDULen);
bool AT_SMS_GetSCANUM(uint8_t *pPDUData, uint8_t *pSACNUM, uint8_t *nPDULen);
void AT_SMS_RecvCbMessage_Indictation(osiEvent_t *pEvent);
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static bool AT_SMS_IsValidPhoneNumber(const char *s, size_t size, bool *bIsInternational);
static bool SMS_AddSCA2PDU(uint8_t *InPDU, uint8_t *OutPDU, uint16_t *pSize, uint8_t nSim);
static bool AT_SMS_CheckPDUIsValid(uint8_t *pPDUData, uint8_t *nPDULen, bool bReadOrListMsgFlag);
#endif
static bool SMS_PDUBCDToASCII(uint8_t *pBCD, uint16_t *pLen, uint8_t *pStr);
static bool AT_SMS_StatusMacroFlagToStringOrData(uint8_t *pStatusMacroFlagBuff, uint8_t nFormat);


static int _paramHex2Num(uint8_t c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    return -1;
}

static bool _checkNumber(const uint8_t *data, size_t size)
{
    uint16_t index;
    for (index = 0; index < size; index++)
    {
        if (_paramHex2Num(data[index]) < 0)
            return false;
    }
    return true;
}

/**
 * When cscs is UCS2, convert real number to ucs2 format
 * 10086
 * 31 30 30 38 36 -> 00 31 00 30 00 30 00 38 00 36 ->
 * 30 30 33 31 30 30 33 30 30 30 33 30 30 30 33 38 30 30 33 36
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static char *generateUrcUcs2Data(char *src, uint16_t src_length)
#else
char *generateUrcUcs2Data(char *src, uint16_t src_length)
#endif
{
    char *dest = NULL;
    char *ucs2Data = NULL;
    int ucs2Size = 0;
    ucs2Data = mlConvertStr(src, src_length, ML_CP936, ML_UTF16BE, &ucs2Size);
    if (ucs2Data != NULL)
    {
        uint16_t length = ucs2Size * 2;
        dest = malloc(length + 4);
        if (dest != NULL)
        {
            memset(dest, 0x00, length + 4);
            char *temp_data = ucs2Data;
            uint16_t n = 0;
            for (uint16_t index = 0; index < ucs2Size; index++)
            {
                uint8_t num0 = temp_data[0] >> 4;
                uint8_t num1 = temp_data[0] & 0x0f;

                if (num0 <= 9)
                    dest[n++] = num0 + '0';
                else
                    dest[n++] = num0 - 0xa + 'A';

                if (num1 <= 9)
                    dest[n++] = num1 + '0';
                else
                    dest[n++] = num1 - 0xa + 'A';
                temp_data += 1;
            }
        }
        free(ucs2Data);
        return dest;
    }
    return NULL;
}

/*
 * Convert cp936 ucs2 format number to real number
 *
 * cp936 ucs2 number 30 30 33 31 30 30 33 30 30 30 33 30 30 30 33 38 30 30 33 36
 * real number 10086
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static char *_generateRealNumberForCmgsAndCmgwInUcs2(const char *da, size_t *daLength, uint32_t *errorCode)
#else
char *_generateRealNumberForCmgsAndCmgwInUcs2(const char *da, size_t *daLength, uint32_t *errorCode)
#endif
{
    if (strlen(da) % 4 != 0)
    {
        *errorCode = ERR_AT_CMS_INVALID_PARA;
        return NULL;
    }

    // convert cp936 data to ucs2 data
    uint16_t length = strlen(da) / 2;
    char *number = malloc(length);
    if (number == NULL)
    {
        *errorCode = ERR_AT_CMS_MEM_FAIL;
        return NULL;
    }
    memset(number, 0x00, length);

    uint16_t n;
    uint16_t index;
    const char *temp_data = da;
    // check whether the number is valid
    for (n = 0; n < length; n++)
    {
        if (_paramHex2Num(*temp_data++) < 0)
        {
            free(number);
            *errorCode = ERR_AT_CMS_INVALID_TXT_PARAM;
            return NULL;
        }
    }

    const char *temp_data2 = da;
    uint8_t num0;
    uint8_t num1;
    n = 0;
    // generate real ucs2 data
    for (index = 0; index < length; index++)
    {
        num0 = _paramHex2Num(temp_data2[0]);
        num1 = _paramHex2Num(temp_data2[1]);
        number[n++] = (num0 << 4) | num1;
        temp_data2 += 2;
    }

    char *number2 = NULL;
    int number_size;
    // convert ucs2 data to cp936 data, generate real number
    number2 = mlConvertStr(number, n, ML_UTF16BE, ML_CP936, &number_size);
    free(number);

    if (number2 == NULL)
    {
        *errorCode = ERR_AT_CMS_INVALID_TXT_PARAM;
        return NULL;
    }

    number = number2;
    *daLength = number_size;
    *errorCode = 0;
    return number;
}

/*
 * Convert real SMS data to ucs2 format
 */
static char *_convertDataToUcs2(char *data, uint16_t size)
{
    uint16_t urcLength;
    char *urc = NULL;
    urcLength = size * 2;
    urc = malloc(urcLength + 4);
    if (urc != NULL)
    {
        memset(urc, 0x00, urcLength + 4);
        uint16_t index = 0;
        char *temp_data = data;
        uint16_t n = 0;
        for (; index < size; index++)
        {
            uint8_t num0 = temp_data[0] >> 4;
            uint8_t num1 = temp_data[0] & 0x0f;

            if (num0 <= 9)
                urc[n++] = num0 + '0';
            else
                urc[n++] = num0 - 0xa + 'A';

            if (num1 <= 9)
                urc[n++] = num1 + '0';
            else
                urc[n++] = num1 - 0xa + 'A';
            temp_data += 1;
        }

        return urc;
    }
    return mlConvertStr(data, size, ML_UTF16BE, ML_CP936, NULL);
}

/**
 * check the number inside PDU is prohibited by FDN feature
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static bool _smsFDNProhibit(const uint8_t *pdu_data, uint8_t nSim)
#else
bool _smsFDNProhibit(const uint8_t *pdu_data, uint8_t nSim)
#endif
{
    CFW_DIALNUMBER_V2 dial;
    if (!AT_SMS_GetPDUNUM(pdu_data, dial.pDialNumber, &dial.nType, &dial.nDialNumberSize))
        return false;

    if (dial.nType == 0x91)
        dial.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;

    return atFDNProhibit(dial.pDialNumber, dial.nDialNumberSize, dial.nType, nSim);
}

static const char *_smsStatusStr(unsigned status)
{
    const osiValueStrMap_t *m = osiVsmapFindByVal(gSmsState, status);
    return (m != NULL) ? m->str : "";
}

/**
 * convert the number to string
 */
 #ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static void _smsNumberStr(const uint8_t *bcd, unsigned size, unsigned type, char *s)
#else
void _smsNumberStr(const uint8_t *bcd, unsigned size, unsigned type, char *s)
#endif
{
    if (type == 0xD0) // what is it
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

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static uint8_t _smsCfwDcsToOutDcs(uint8_t InDCS)
#else
uint8_t _smsCfwDcsToOutDcs(uint8_t InDCS)
#endif
{
    switch (InDCS)
    {
    case 0:
    default:
        return 0; // GSM_7BIT_UNCOMPRESSED
    case 1:
        return 4; // GSM_8BIT_UNCOMPRESSED
    case 2:
        return 8; // GSM_UCS2_UNCOMPRESSED
    case 0x10:
        return 0x20; // GSM_7BIT_COMPRESSED
    case 0x11:
        return 0x24; // GSM_8BIT_COMPRESSED
    case 0x12:
        return 0x28; // GSM_UCS2_COMPRESSED
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static uint8_t _smsCfwStatusToAt(uint8_t status)
#else 
uint8_t _smsCfwStatusToAt(uint8_t status)
#endif
{
    switch (status)
    {
    case CFW_SMS_STORED_STATUS_UNREAD:
        return AT_SMS_STATUS_UNREAD;
    case CFW_SMS_STORED_STATUS_READ:
        return AT_SMS_STATUS_READ;
    case CFW_SMS_STORED_STATUS_UNSENT:
        return AT_SMS_STATUS_UNSENT;
    case CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ:
        return AT_SMS_STATUS_SENT;
    case CFW_SMS_STORED_STATUS_STORED_ALL:
        return AT_SMS_STATUS_ALL;
    default:
        return 0xff;
    }
}

static unsigned _smsAtStatusToCfw(unsigned stat)
{
    switch (stat)
    {
    case AT_SMS_STATUS_UNREAD:
        return CFW_SMS_STORED_STATUS_UNREAD;
    case AT_SMS_STATUS_READ:
        return CFW_SMS_STORED_STATUS_READ;
    case AT_SMS_STATUS_UNSENT:
        return CFW_SMS_STORED_STATUS_UNSENT;
    case AT_SMS_STATUS_SENT:
        return CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ |
               CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
               CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE |
               CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
    case AT_SMS_STATUS_ALL:
        return CFW_SMS_STORED_STATUS_STORED_ALL;
    default:
        return 0xff;
    }
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static const char *_smsAtStatusStr(uint8_t status)
#else
const char *_smsAtStatusStr(uint8_t status)
#endif
{
    const osiValueStrMap_t *m = osiVsmapFindByVal(gSmsNumState, status);
    return (m != NULL) ? m->str : "";
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static char *_smsNodeDataStr(uint8_t type, CFW_SMS_INFO_UNION *info, uint8_t nSim)
#else
char *_smsNodeDataStr(uint8_t type, CFW_SMS_INFO_UNION *info, uint8_t nSim)
#endif
{
    OSI_LOGI(0, "_smsNodeDataStr type: %X, dcs=%d", type);
    if (type == 1)
    {
        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *pType1Data =
            (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *)info;
		OSI_LOGI(0, "_smsNodeDataStr type: dcs=%d", pType1Data->dcs);
        if (pType1Data->dcs == 2)
        {
            if (gAtSetting.cscs == cs_ucs2)
                return _convertDataToUcs2((char *)pType1Data->data, pType1Data->length);
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr(pType1Data->data, pType1Data->length,
                                             ML_UTF16BE, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            else
                return mlConvertStr(pType1Data->data, pType1Data->length,
                                    ML_UTF16BE, ML_CP936, NULL);
        }
        if (pType1Data->dcs == 0)
        {
            if (gAtSetting.cscs == cs_ucs2)
            {
                int ucs2Size = 0;
                char *ucs2Data = mlConvertStr(pType1Data->data, pType1Data->length,
                                              ML_GSM, ML_UTF16BE, &ucs2Size);
                if (ucs2Data == NULL)
                    return NULL;

                char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
                free(ucs2Data);

                if (urcUcs2Data == NULL)
                    return NULL;
                else
                    return urcUcs2Data;
            }
            else if (gAtSetting.cscs == cs_ira)
            {
                char *iraUrcStr = NULL;
                iraUrcStr = malloc(pType1Data->length + 1);
                if (NULL != iraUrcStr)
                {
                    memset(iraUrcStr, 0x00, pType1Data->length + 1);
                    memcpy(iraUrcStr, pType1Data->data, pType1Data->length);
                    return iraUrcStr;
                }
                else
                    return NULL;
            }
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr(pType1Data->data, pType1Data->length,
                                             ML_GSM, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            return mlConvertStr(pType1Data->data, pType1Data->length,
                                ML_GSM, ML_CP936, NULL);
        }
        // dcs=1
        if (gAtSetting.cscs == cs_ucs2)
        {
            int ucs2Size = 0;
            char *ucs2Data = mlConvertStr(pType1Data->data, pType1Data->length,
                                          ML_CP936, ML_UTF16BE, &ucs2Size);
            if (ucs2Data == NULL)
                return strndup((char *)pType1Data->data, pType1Data->length);

            char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
            free(ucs2Data);
            if (urcUcs2Data == NULL)
                return NULL;
            else
                return urcUcs2Data;
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            char *urcHexData = (char *)malloc(pType1Data->length * 2 + 2);
            if (urcHexData == NULL)
                return NULL;

            cfwBytesToHexStr((char *)pType1Data->data, pType1Data->length, urcHexData);
            return urcHexData;
        }
        else
            return strndup((char *)pType1Data->data, pType1Data->length);
    }
    else if (type == 3)
    {
        CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO_V2 *pType3Data =
            (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO_V2 *)info;

        if (pType3Data->dcs == 2)
        {
            if (gAtSetting.cscs == cs_ucs2)
                return _convertDataToUcs2((char *)pType3Data->data, pType3Data->length);
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr((char *)pType3Data->data, pType3Data->length,
                                             ML_UTF16BE, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            else
                return mlConvertStr(pType3Data->data, pType3Data->length,
                                    ML_UTF16BE, ML_CP936, NULL);
        }
        if (pType3Data->dcs == 0)
        {
            if (gAtSetting.cscs == cs_ucs2)
            {
                int ucs2Size = 0;
                char *ucs2Data = mlConvertStr(pType3Data->data, pType3Data->length,
                                              ML_GSM, ML_UTF16BE, &ucs2Size);
                if (ucs2Data == NULL)
                    return NULL;
                char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
                free(ucs2Data);
                if (urcUcs2Data == NULL)
                    return NULL;
                else
                    return urcUcs2Data;
            }
            else if (gAtSetting.cscs == cs_ira)
            {
                char *iraUrcStr = NULL;
                iraUrcStr = malloc(pType3Data->length + 1);
                if (NULL != iraUrcStr)
                {
                    memset(iraUrcStr, 0x00, pType3Data->length + 1);
                    memcpy(iraUrcStr, pType3Data->data, pType3Data->length);
                    return iraUrcStr;
                }
                else
                    return NULL;
            }
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr(pType3Data->data, pType3Data->length,
                                             ML_GSM, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            else
                return mlConvertStr(pType3Data->data, pType3Data->length,
                                    ML_GSM, ML_CP936, NULL);
        }
        if (gAtSetting.cscs == cs_ucs2)
        {
            int ucs2Size = 0;
            char *ucs2Data = mlConvertStr(pType3Data->data, pType3Data->length,
                                          ML_CP936, ML_UTF16BE, &ucs2Size);
            if (ucs2Data == NULL)
                return NULL;
            char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
            free(ucs2Data);
            if (urcUcs2Data == NULL)
                return NULL;
            else
                return urcUcs2Data;
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            char *urcHexData = (char *)malloc(pType3Data->length * 2 + 2);
            if (urcHexData == NULL)
                return NULL;

            cfwBytesToHexStr((char *)pType3Data->data, pType3Data->length, urcHexData);
            return urcHexData;
        }
        else
        {
            return strndup((char *)pType3Data->data, pType3Data->length);
        }
    }
    else if (type == 0x21)
    {
        CFW_SMS_TXT_HRD_IND_V2 *pTextWithHead = (CFW_SMS_TXT_HRD_IND_V2 *)info;

        if (pTextWithHead->dcs == 2)
        {
            if (gAtSetting.cscs == cs_ucs2)
                return _convertDataToUcs2((char *)pTextWithHead->pData, pTextWithHead->nDataLen);
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr((char *)pTextWithHead->pData, pTextWithHead->nDataLen,
                                             ML_UTF16BE, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            else
            {
                return mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                    ML_UTF16BE, ML_CP936, NULL);
            }
        }
        if (pTextWithHead->dcs == 0)
        {
            if (gAtSetting.cscs == cs_ucs2)
            {
                int ucs2Size = 0;
                char *ucs2Data = mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                              ML_GSM, ML_UTF16BE, &ucs2Size);
                if (ucs2Data == NULL)
                    return mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                        ML_GSM, ML_CP936, NULL);
                char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
                free(ucs2Data);
                if (urcUcs2Data == NULL)
                    return mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                        ML_GSM, ML_CP936, NULL);
                return urcUcs2Data;
            }
            else if (gAtSetting.cscs == cs_ira)
            {
                char *iraUrcStr = NULL;
                iraUrcStr = malloc(pTextWithHead->nDataLen + 1);
                if (NULL != iraUrcStr)
                {
                    memset(iraUrcStr, 0x00, pTextWithHead->nDataLen + 1);
                    memcpy(iraUrcStr, pTextWithHead->pData, pTextWithHead->nDataLen);
                    return iraUrcStr;
                }
                else
                    return NULL;
            }
            else if (gAtSetting.cscs == cs_hex)
            {
                int hexSize = 0;
                char *hexData = mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                             ML_GSM, ML_CP936, &hexSize);
                if (hexData == NULL)
                    return NULL;

                char *urcHexData = (char *)malloc(hexSize * 2 + 2);
                if (urcHexData == NULL)
                {
                    free(hexData);
                    return NULL;
                }

                cfwBytesToHexStr(hexData, hexSize, urcHexData);
                free(hexData);
                return urcHexData;
            }
            else
                return mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                    ML_GSM, ML_CP936, NULL);
        }
        if (gAtSetting.cscs == cs_ucs2)
        {
            int ucs2Size = 0;
            char *ucs2Data = mlConvertStr(pTextWithHead->pData, pTextWithHead->nDataLen,
                                          ML_CP936, ML_UTF16BE, &ucs2Size);
            if (ucs2Data == NULL)
                return NULL;

            char *urcUcs2Data = _convertDataToUcs2(ucs2Data, ucs2Size);
            free(ucs2Data);
            if (urcUcs2Data == NULL)
                return NULL;

            return urcUcs2Data;
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            char *urcHexData = (char *)malloc(pTextWithHead->nDataLen * 2 + 2);
            if (urcHexData == NULL)
                return NULL;

            cfwBytesToHexStr((char *)pTextWithHead->pData, pTextWithHead->nDataLen, urcHexData);
            return urcHexData;
        }
        else
            return strndup((char *)pTextWithHead->pData, pTextWithHead->nDataLen);
    }
    else if (type == 7)
    {
        CFW_SMS_PDU_INFO_V2 *pType7Data = (CFW_SMS_PDU_INFO_V2 *)info;

        /* add sca if have not sca */
        uint16_t nTotalDataLen = pType7Data->nDataSize;
        uint8_t *BCDDataBuff = (uint8_t *)malloc(nTotalDataLen + 23);
        if (!SMS_AddSCA2PDU(pType7Data->pData, BCDDataBuff, &nTotalDataLen, nSim))
        {
            free(BCDDataBuff);
            return NULL;
        }

        /* BCD to ASCII */
        uint8_t *ASCIIDataBuff = (uint8_t *)malloc(nTotalDataLen * 2 + 1);
        memset(ASCIIDataBuff, 0, (nTotalDataLen * 2 + 1));
        if (!SMS_PDUBCDToASCII(BCDDataBuff, &nTotalDataLen, ASCIIDataBuff))
        {
            free(BCDDataBuff);
            free(ASCIIDataBuff);
            return NULL;
        }
        free(BCDDataBuff);
        return (char *)ASCIIDataBuff;
    }

    return NULL;
}

/**
 * Convert SMS AT command dial number parameters to CFW_DIAlNUMBER_V2, and return
 * CMS error on fail.
 */
static unsigned _smsParamNumber(const char *da, unsigned toda, CFW_DIALNUMBER_V2 *dial)
{
    bool bIsInternational = false;
    unsigned da_len = strlen(da);
    if (!AT_SMS_IsValidPhoneNumber(da, da_len, &bIsInternational))
        return ERR_AT_CMS_INVALID_TXT_CHAR;

    if (bIsInternational)
    {
        da++;
        da_len--;
    }

    if (da_len > 20)
        return ERR_AT_CMS_INVALID_LEN;

    // can't fail due to it is checked.
    dial->nDialNumberSize = cfwDialStringToBcd(da, da_len, dial->pDialNumber);

    if (toda != CFW_TELNUMBER_TYPE_INTERNATIONAL &&
        toda != CFW_TELNUMBER_TYPE_NATIONAL &&
        toda != CFW_TELNUMBER_TYPE_UNKNOWN)
        return ERR_AT_CMS_INVALID_TXT_PARAM;

    /* according to spec, we check dial number first */
    if (bIsInternational)
    {
        toda = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    }
    else if (toda == 0)
    {
        /* no input num type  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else if (toda == CFW_TELNUMBER_TYPE_INTERNATIONAL)
    {
        /* have no '+' , but the num type is 145, replace 129 with is  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else
    {
        /* get the input num type  */
    }
    dial->nType = toda;
    return 0;
}

static void NewSms_TimerCB(void *ctx)
{
    uint8_t nSim = (uint32_t)ctx;
    gAtSetting.sim[nSim].cnmi_mt = 0;
    CFW_CfgSetNewSmsOptionMT(0, CFW_SMS_STORAGE_NS, nSim);

    gAtSetting.sim[nSim].cnmi_ds = 0;
#ifdef CONFIG_SOC_8910
    CFW_SmsMtSmsPPAckReq(nSim);
#endif
    osiTimerDelete(gAtCfwCtx.sim[nSim].newsms_timer);
    gAtCfwCtx.sim[nSim].newsms_timer = NULL;
    gAtCfwCtx.sim[nSim].is_waiting_cnma = false;
}

static uint8_t _GetPduDcs(uint8_t *data)
{
    uint8_t *tem = &data[data[0] + 1];
    return (tem[1 + (uint8_t)(tem[1] / 2 + tem[1] % 2 + 2) + 1]);
}

static void _onEV_CFW_NEW_SMS_IND(const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    int nSim = cfw_event->nFlag;
    uint8_t nFormat = gAtSetting.sim[nSim].cmgf;
    char *rsp = NULL;
    rsp = malloc(1024);
    if (rsp == NULL)
    {
        OSI_LOGI(0, "_onEV_CFW_NEW_SMS_IND malloc error");
        CFW_SmsMtSmsPPAckReq(nSim);
        return;
    }
    memset(rsp, 0x00, 1024);
    atMemFreeLater(rsp);

    /* get new message node and check it */
    CFW_NEW_SMS_NODE_EX *pNewMsgNode = (CFW_NEW_SMS_NODE_EX *)cfw_event->nParam1;
    if (NULL == pNewMsgNode || cfw_event->nType != 0)
    {
        CFW_SmsMtSmsPPAckReq(nSim);
        return;
    }

    atMemFreeLater(pNewMsgNode);
    if (pNewMsgNode->node.nType == 0x20 ||
        pNewMsgNode->node.nType == 0x21 ||
        pNewMsgNode->node.nType == 0x22 ||
        pNewMsgNode->node.nType == 0x5)
    {
        if (gAtSetting.sim[nSim].cind_message)
            atCmdRespSimUrcText(nSim, "+CIEV: \"MESSAGE\",1");
    }

    OSI_LOGI(0, "NEW_SMS_NODE: type/%d", pNewMsgNode->node.nType);

    /* check the msg node type */
    switch (pNewMsgNode->node.nType)
    {
    case 0x20: // PDU Message Content
    {
        g_SMS_CIND_NewMsg++;
        if (nFormat != 0)
        {
            OSI_LOGE(0, "NEW_SMS_IND: receive type/%d in format/%d",
                     pNewMsgNode->node.nType, nFormat);
            CFW_SmsMtSmsPPAckReq(nSim);
            return;
        }

        CFW_SMS_PDU_INFO_V2 *pPduNodeInfo = NULL;
        pPduNodeInfo = (CFW_SMS_PDU_INFO_V2 *)&pNewMsgNode->info;
        uint8_t *pPduData = NULL;
        pPduData = pPduNodeInfo->pData;

        uint8_t dcs = _GetPduDcs(pPduData);
        uint8_t messageClass = dcs & 0x03;
        uint8_t nMt = gAtSetting.sim[nSim].cnmi_mt;
        bool isClass0 = ((dcs & 0x10) == 0x10) && (messageClass == 0);
        bool isClass1 = ((dcs & 0x10) == 0x10) && (messageClass == 1);
        bool isClass2 = ((dcs & 0x10) == 0x10) && (messageClass == 2);
        bool isClass3 = ((dcs & 0x10) == 0x10) && (messageClass == 3);

        if (((nMt == 1) || ((nMt == 2) && isClass2) || ((nMt == 3) && !isClass3)) && !isClass0) /* dump report MT == 1 */
        {
            uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem1;
            if (nStorage == CFW_SMS_STORAGE_ME)
                pNewMsgNode->node.nConcatCurrentIndex -= (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim;
            const char *stor = (nStorage == CFW_SMS_STORAGE_MT) ? "MT" : ((pNewMsgNode->node.nStorage == 1) ? "ME" : "SM");
            sprintf(rsp, "+CMTI: \"%s\",%u", stor, pNewMsgNode->node.nConcatCurrentIndex);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
			{
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
			}
#else
            atCmdRespSimUrcText(nSim, rsp);
#endif
        }
        else if (isClass0 || ((nMt == 2) || ((nMt == 3) && isClass3))) /* dump report MT == 2 */
        {
            /* check recv pdu and discard unused char */
            if (!(AT_SMS_CheckPDUIsValid((uint8_t *)pPduNodeInfo->pData,
                                         (uint8_t *)&(pPduNodeInfo->nDataSize), true)))
            {
                OSI_LOGE(0, "NEW_SMS_IND: check pdu is invalid");
                CFW_SmsMtSmsPPAckReq(nSim);
                return;
            }

            if (CFW_GetSmsSeviceMode() && ((nMt == 3 && isClass3) || (nMt == 2 && isClass3) || (nMt == 2 && isClass1)))
            {
                gAtCfwCtx.sim[nSim].is_waiting_cnma = true;
                gAtCfwCtx.sim[nSim].newsms_timer = osiTimerCreate(atEngineGetThreadId(), NewSms_TimerCB, (void *)nSim);
                osiTimerStart(gAtCfwCtx.sim[nSim].newsms_timer, 15 * 1000);
            }

            if (isClass0)
                sprintf(rsp, "+CLASS0: ,%u", pPduNodeInfo->nDataSize - pPduNodeInfo->pData[0] - 1);
            else
                sprintf(rsp, "+CMT: ,%u", pPduNodeInfo->nDataSize - pPduNodeInfo->pData[0] - 1);
			
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
			{
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
			}
#else
			atCmdRespSimUrcText(nSim, rsp);
#endif


            char *pdu_str = (char *)malloc(pPduNodeInfo->nDataSize * 2 + 1);
            if (pdu_str != NULL)
            {
                cfwBytesToHexStr(pPduNodeInfo->pData, pPduNodeInfo->nDataSize, pdu_str);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
				if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
				{
					quec_urc_send(NULL, pdu_str, strlen(pdu_str), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
				}
#else
                atCmdRespSimUrcText(nSim, pdu_str);
#endif
                free(pdu_str);
            }
            if (CFW_GetSmsSeviceMode() && ((nMt == 3 && isClass3) || (nMt == 2 && isClass3) || (nMt == 2 && isClass1)))
            {
                break;
            }
        }
        CFW_SmsMtSmsPPAckReq(nSim);
        break;
    }
    case 0x21: // 1. Text Message Content with header
    case 0x22: // 2. Text Message Content without header
    case 0x5:  // 3. MMS PUSH header
    {
        CFW_SMS_TXT_HRD_IND_V2 *pTextWithHead = (CFW_SMS_TXT_HRD_IND_V2 *)&pNewMsgNode->info;
        uint8_t dcs = pTextWithHead->real_dcs;
        uint8_t messageClass = dcs & 0x03;
        uint8_t nMt = gAtSetting.sim[nSim].cnmi_mt;
        bool isClass0 = ((dcs & 0x10) == 0x10) && (messageClass == 0);
        bool isClass1 = ((dcs & 0x10) == 0x10) && (messageClass == 1);
        bool isClass2 = ((dcs & 0x10) == 0x10) && (messageClass == 2);
        bool isClass3 = ((dcs & 0x10) == 0x10) && (messageClass == 3);

        if (((nMt == 1) || ((nMt == 2) && isClass2) || ((nMt == 3) && !isClass3)) && !isClass0) /* dump report MT == 1 */
        {
            uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem1;
            if (nStorage == CFW_SMS_STORAGE_ME)
                pNewMsgNode->node.nConcatCurrentIndex -= (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim;
            const char *stor = (nStorage == CFW_SMS_STORAGE_MT) ? "MT" : ((pNewMsgNode->node.nStorage == 1) ? "ME" : "SM");
            sprintf(rsp, "+CMTI: \"%s\",%u", stor, pNewMsgNode->node.nConcatCurrentIndex);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
			{
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
			}
#else
			atCmdRespSimUrcText(nSim, rsp);
#endif
        }
        else if (isClass0 || ((nMt == 2) || ((nMt == 3) && isClass3))) /* dump report MT == 2 */
        {
            char *urcOaNumber = NULL;
            char oaNumber[TEL_NUMBER_MAX_LEN + 2] = {0};
            _smsNumberStr(pTextWithHead->oa, pTextWithHead->oa_size, pTextWithHead->tooa, oaNumber);
            if (gAtSetting.cscs == cs_ucs2)
                urcOaNumber = generateUrcUcs2Data(oaNumber, strlen(oaNumber));

            char *urcScaNumber = NULL;
            char scaNumber[TEL_NUMBER_MAX_LEN + 2] = {0};
            _smsNumberStr(pTextWithHead->sca, pTextWithHead->sca_size, pTextWithHead->tosca, scaNumber);
            if (gAtSetting.cscs == cs_ucs2)
                urcScaNumber = generateUrcUcs2Data(scaNumber, strlen(scaNumber));

            if (CFW_GetSmsSeviceMode() && ((nMt == 3 && isClass3) || (nMt == 2 && isClass3) || (nMt == 2 && isClass1)))
            {
                gAtCfwCtx.sim[nSim].newsms_timer = osiTimerCreate(atEngineGetThreadId(), NewSms_TimerCB, (void *)nSim);
                osiTimerStart(gAtCfwCtx.sim[nSim].newsms_timer, 15 * 1000);
                gAtCfwCtx.sim[nSim].is_waiting_cnma = true;
            }

            uint8_t nDcs = _smsCfwDcsToOutDcs(pTextWithHead->dcs);
            const char *format = NULL;
            if (gAtSetting.sim[nSim].csdh)
            {
                if (isClass0)
                    format = "+CLASS0: \"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\",%u,%u,%u,%u,\"%s\",%u,%u";
                else
                    format = "+CMT: \"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\",%u,%u,%u,%u,\"%s\",%u,%u";
                sprintf(rsp, format,
                        urcOaNumber ? urcOaNumber : oaNumber, pTextWithHead->scts.uYear, pTextWithHead->scts.uMonth,
                        pTextWithHead->scts.uDay, pTextWithHead->scts.uHour, pTextWithHead->scts.uMinute,
                        pTextWithHead->scts.uSecond, pTextWithHead->scts.iZone, pTextWithHead->tooa,
                        pTextWithHead->fo, pTextWithHead->pid, nDcs,
                        urcScaNumber ? urcScaNumber : scaNumber, pTextWithHead->tosca, pTextWithHead->nDataLen);
            }
            else
            {
                if (isClass0)
                    format = "+CLASS0: \"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\"";
                else
                    format = "+CMT: \"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\"";
                sprintf(rsp, format,
                        urcOaNumber ? urcOaNumber : oaNumber, pTextWithHead->scts.uYear, pTextWithHead->scts.uMonth,
                        pTextWithHead->scts.uDay, pTextWithHead->scts.uHour, pTextWithHead->scts.uMinute,
                        pTextWithHead->scts.uSecond, pTextWithHead->scts.iZone);
            }

            if (urcScaNumber)
                free(urcScaNumber);
            if (urcOaNumber)
                free(urcOaNumber);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
			{
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
			}
#else
			atCmdRespSimUrcText(nSim, rsp);
#endif
            char *data = _smsNodeDataStr(pNewMsgNode->node.nType, &pNewMsgNode->info, nSim);
            if (data != NULL)
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
				if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
				{
					quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
				}
#else
                atCmdRespSimUrcText(nSim, data);
#endif
            free(data);

            if (CFW_GetSmsSeviceMode() && ((nMt == 3 && isClass3) || (nMt == 2 && isClass3) || (nMt == 2 && isClass1)))
            {
                break;
            }
        }
        CFW_SmsMtSmsPPAckReq(nSim);
        break;
    }
    case 0x40: // PDU status report
        CFW_SmsMtSmsPPAckReq(nSim);
        if (gAtSetting.sim[nSim].cnmi_ds == 1)
        {
            uint16_t nATBAckLen = 0;
            char ATBAckBuff[512] = {0};
            CFW_SMS_PDU_INFO_V2 *pPduSR = &(pNewMsgNode->info.uPDUV2);
            uint16_t hexStrLen = 0;
            char *local_data = (char *)malloc(pPduSR->nDataSize * 2 + 1);
            if (local_data != NULL)
            {
                memset(local_data, 0x00, pPduSR->nDataSize * 2 + 1);
                hexStrLen = cfwBytesToHexStr(pPduSR->pData, pPduSR->nDataSize, local_data);
                sprintf(ATBAckBuff, "+CDS: %u\r\n", pPduSR->nDataSize);
                nATBAckLen = strlen(ATBAckBuff);
                memcpy(&ATBAckBuff[nATBAckLen], local_data, hexStrLen);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
				if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
				{
					quec_urc_send(NULL, ATBAckBuff, strlen(ATBAckBuff), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
				}
#else
				atCmdRespSimUrcText(nSim, ATBAckBuff);
#endif

                memset(local_data, 0x00, pPduSR->nDataSize * 2 + 1);
                free(local_data);
                local_data = NULL;
            }
        }
        break;
    case 0x41: // text status report
        CFW_SmsMtSmsPPAckReq(nSim);
        if (gAtSetting.sim[nSim].cnmi_ds == 1)
        {
            CFW_SMS_TXT_STATUS_IND *pTextWithHeadSR = &(pNewMsgNode->info.uTxtStatusInd);

            char raNumber[TEL_NUMBER_MAX_LEN + 1] = {0};
            _smsNumberStr(pTextWithHeadSR->ra, pTextWithHeadSR->ra_size,
                          pTextWithHeadSR->tora, raNumber);

            char *urcScaNumber = NULL;
            if (gAtSetting.cscs == cs_ucs2)
                urcScaNumber = generateUrcUcs2Data(raNumber, strlen(raNumber));
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS //åœ¨breakä¹‹å‰å·²ç»freeäº†urcScaNumberæŒ‡é’ˆ, æ­¤å¤„ä¸éœ€è¦atMemFreeLater
            atMemFreeLater(urcScaNumber);
#endif
            sprintf(rsp, "+CDS: %u,%u,\"%s\",%u,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\",\"%u/%02u/%02u,%02u:%02u:%02u%+03d\",%u",
                    pTextWithHeadSR->fo, pTextWithHeadSR->mr, urcScaNumber ? urcScaNumber : raNumber,
                    pTextWithHeadSR->tora, pTextWithHeadSR->scts.uYear, pTextWithHeadSR->scts.uMonth,
                    pTextWithHeadSR->scts.uDay, pTextWithHeadSR->scts.uHour, pTextWithHeadSR->scts.uMinute,
                    pTextWithHeadSR->scts.uSecond, pTextWithHeadSR->scts.iZone, pTextWithHeadSR->dt.uYear,
                    pTextWithHeadSR->dt.uMonth, pTextWithHeadSR->dt.uDay, pTextWithHeadSR->dt.uHour,
                    pTextWithHeadSR->dt.uMinute, pTextWithHeadSR->dt.uSecond, pTextWithHeadSR->dt.iZone,
                    pTextWithHeadSR->st);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
			if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSINCOMING_IND))
			{
				quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
			}
#else
			atCmdRespSimUrcText(nSim, rsp);
#endif
            if (urcScaNumber)
                free(urcScaNumber);
        }
        break;

    case 0x30: // not implemented
    case 0x31: // not implemented
    default:
        CFW_SmsMtSmsPPAckReq(nSim);
        break;
    }
#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_SMS)
	if(ql_sms_callback){
		ql_sms_callback(QL_SMS_NEW_MSG_IND, &pNewMsgNode->node.nConcatCurrentIndex);
	}
#endif
#ifdef CONFIG_SOC_8910
    if (gAtSetting.smsmode == 0)
    {
        audevStopTone();
        audevPlayTone(AUDEV_TONE_DIAL, 1000);
    }
#endif
}

static void _onEV_CFW_CB_MSG_IND(const osiEvent_t *event)
{
    // TODO
}

static void _onEV_CFW_CB_PAGE_IND(const osiEvent_t *event)
{
    _onEV_CFW_CB_MSG_IND(event);
}

static void _onEV_CFW_SMS_INFO_IND(const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    char rsp[32];

    /* check event type */
    if (0 == cfw_event->nType)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		switch (cfw_event->nParam1)
		{
		// should not report smsfull urc?
		case 0: /* delete message and storage is spare */
			if (gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_SM)
			{
				/* save msgfull flag of SM, bit1 is set to "0" */
				gAtCfwCtx.sim[nSim].sms_memfull &= ~CFW_SMS_STORAGE_SM;
			}
			else if (gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_ME || 
			         gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_MT)
			{
				/* save msgfull flag of ME, bit0 is set to "0" */
				gAtCfwCtx.sim[nSim].sms_memfull &= ~CFW_SMS_STORAGE_ME;
			}
			break;

		case 1: /* mem full and no message waiting for */
		case 2: /* mem full and have message waiting for */
			if ((gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_SM) ||
				(gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_SM) )
			{
				/* save msgfull flag of SM, bit1 is set to "1" */
				gAtCfwCtx.sim[nSim].sms_memfull |= CFW_SMS_STORAGE_SM;
				if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSFULL_IND))
				{
					sprintf(rsp, "+QIND: \"SMSFULL\",\"SM\"");
					quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
				}
			}
			else if ((gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_ME) ||
					 (gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_MT) ||
					 (gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_ME) ||
					 (gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_MT))
			{
				/* save msgfull flag of ME, bit0 is set to "1" */
				gAtCfwCtx.sim[nSim].sms_memfull |= CFW_SMS_STORAGE_ME;
				if(quectel_read_urc_ind_flag(QUECTEL_URC_ALL_IND) && quectel_read_urc_ind_flag(QUECTEL_URC_SMSFULL_IND))
				{
					sprintf(rsp, "+QIND: \"SMSFULL\",\"ME\"");
					quec_urc_send(NULL, rsp, strlen(rsp), URC_OUT_METHOD_UNICAST, RI_CATEG_SMS, 0);
				}
			}
			break;
		default:
			break;
		}
#else
        switch (cfw_event->nParam1)
        {
        // should not report smsfull urc?
        case 0: /* delete message and storage is spare */
            if (gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_SM ||
                gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_MT)
            {
                /* save msgfull flag of SM, bit1 is set to "0" */
                gAtCfwCtx.sim[nSim].sms_memfull &= ~CFW_SMS_STORAGE_SM;
            }
            else if (gAtSetting.sim[nSim].cpms_mem1 & CFW_SMS_STORAGE_ME)
            {
                /* save msgfull flag of ME, bit0 is set to "0" */
                gAtCfwCtx.sim[nSim].sms_memfull &= ~CFW_SMS_STORAGE_ME;
            }
            else
            {
            }

            sprintf(rsp, "+CIEV: \"SMSFULL\",0");
            atCmdRespSimUrcText(nSim, rsp);

            break;

        case 1: /* mem full and no message waiting for */
        case 2: /* mem full and have message waiting for */
            if ((gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_SM) ||
                (gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_MT) ||
                (gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_SM) ||
                (gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_MT))
            {
                /* save msgfull flag of SM, bit1 is set to "1" */
                gAtCfwCtx.sim[nSim].sms_memfull |= CFW_SMS_STORAGE_SM;
            }
            else if ((gAtSetting.sim[nSim].cpms_mem2 & CFW_SMS_STORAGE_ME) ||
                     (gAtSetting.sim[nSim].cpms_mem3 & CFW_SMS_STORAGE_ME))
            {
                /* save msgfull flag of ME, bit0 is set to "1" */
                gAtCfwCtx.sim[nSim].sms_memfull |= CFW_SMS_STORAGE_ME;
            }
            else
            {
            }

            /* report to CIEV or not  */
            /*
            if (gAtSetting.cmer_ind)
            {
                // sprintf(rsp, "+CIEV: %d,%lu", AT_IND_SMSFULL, cfw_event->nParam2);
                sprintf(rsp, "+CIEV: \"SMSFULL\",%lu", cfw_event->nParam2 & 0xFF);
                atCmdRespSimUrcText(nSim, rsp);
            } */

            sprintf(rsp, "+CIEV: \"SMSFULL\",1");
            atCmdRespSimUrcText(nSim, rsp);

            break;

        default:
            break;
        }
#endif
    }
}

static void _onEV_CFW_NEW_EMS_IND(const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    int nSim = cfw_event->nFlag;
    if (gAtSetting.sim[nSim].cind_message)
        atCmdRespSimUrcText(nSim, "+CIEV: \"MMS\",1");
    CFW_SmsMtSmsPPAckReq(nSim);
    // gAtCfwCtx.sim[nSim].newsms_timer = osiTimerCreate(atEngineGetThreadId(), NewSms_TimerCB, (void *)nSim);
    // osiTimerStart(gAtCfwCtx.sim[nSim].newsms_timer, 10);
    return;
}

uint32_t atCfwSmsInit(uint16_t nUTI, uint8_t nSim)
{
    /* Define the variable */
    CFW_SMS_PARAMETER sms_par = {0};
    CFW_SMS_STORAGE_INFO sms_stor_info = {0};
    uint8_t nOption = 0;
    uint8_t nNewSmsStorage = 0;
    //uint8_t ReportBuff[32] = {0};
    uint32_t ret = 0;

    atEventsRegister(
        EV_CFW_NEW_SMS_IND, _onEV_CFW_NEW_SMS_IND,
        EV_CFW_CB_MSG_IND, _onEV_CFW_CB_MSG_IND,
        EV_CFW_CB_PAGE_IND, _onEV_CFW_CB_PAGE_IND,
        EV_CFW_SMS_INFO_IND, _onEV_CFW_SMS_INFO_IND,
        EV_CFW_NEW_EMS_IND, _onEV_CFW_NEW_EMS_IND,
        0);

    OSI_LOGI(0x100047da, "SMS init beginning .......");
    // gATCurrentuCmer_ind[nSim] = 2;
    /* set default show text mode parameters, show all header or not */
    if (CFW_CfgSetSmsShowTextModeParam(1, nSim) != 0)
        return ERR_AT_CMS_MEM_FAIL;

    /* set defualt value for +ciev, the buffer overflow, notify to AT or not */
    if (CFW_CfgSetSmsOverflowInd(1, nSim) != 0)
        return ERR_AT_CMS_MEM_FAIL;

    // ====================================================
    // update SMS parameters, we first get the current value ,then
    // modify the segments we needed.
    // ====================================================

    if (CFW_CfgGetSmsParam(&sms_par, 0, nSim) != 0)
        return ERR_AT_CMS_MEM_FAIL;

#if 0
    // ====================================================
    // modify the segments we need to change in default value
    // according to the 3GPP 27.005 the default value is:
    // "fo" = 17, "vp" = 167, "dcs" = 0, "pid" = 0
    // ====================================================
    // ====================================================
    // "fo" composition :
    // bit    7    6      5     4      3     2    1     0
    // RP UDHI SSR VPF  VPF  RD MTI MTI
    // ====================================================

    sms_par.rp   = (17 & 0x80) >> 7;
    sms_par.udhi = (17 & 0x40) >> 6;
#endif
    sms_par.ssr = (17 & 0x20) >> 5;

#if 0
    sms_par.vpf = (17 & 0x18) >> 3;
    sms_par.rd  = (17 & 0x04) >> 2;
    sms_par.mti = 17 & 0x03;
#endif

    //sms_par.vp = 167;
    //sms_par.dcs = 8;
    //sms_par.pid = 0;

#if 0
    /* set param location */
    sms_par.nIndex         = 0; // the first profile of parameters
    sms_par.nSaveToSimFlag = 0x00;  // set param to mobile flash , sync procedure calling
    sms_par.bDefault       = true;  // as current parameters

    /* get the UTI and free it after finished calling */
    if (0 == (nUTI = AT_AllocUserTransID()))
    {
        return ERR_AT_CMS_MEM_FAIL;
    }

    /* set sms parameters request, sync procedure calling */
    nOperationRet = CFW_CfgSetSmsParam(&sms_par, nUTI);
    AT_FreeUserTransID(nUTI);
#else

    ret = CFW_CfgSetSmsParam(&sms_par, nUTI, nSim);
#endif

    /* asyn procedure calling */
    if (0 != ret)
        return ERR_AT_CMS_MEM_FAIL;

    // ====================================================
    //  status report value, get the current value and
    //  then update we needed
    // ====================================================

    if (0 != CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim))
        return ERR_AT_CMS_MEM_FAIL;

    if (0 != CFW_CfgSetNewSmsOption(nOption | CFW_SMS_ROUT_DETAIL_INFO,
                                    gAtSetting.sim[nSim].cpms_mem3, nSim))
        return ERR_AT_CMS_MEM_FAIL;

    nOption = 0;
    uint8_t storage = CFW_SMS_STORAGE_NS;
    if (CFW_CfgGetNewSmsOptionMT(&nOption, &storage, nSim) != 0)
        return ERR_AT_CMS_MEM_FAIL;

    if (CFW_CfgSetNewSmsOptionMT(gAtSetting.sim[nSim].cnmi_mt & 0x03, CFW_SMS_STORAGE_NS, nSim) != 0)
        return ERR_AT_CMS_MEM_FAIL;

    //===========================================
    /* support multi language                                   */
    //===========================================
    // ML_Init();

    // ====================================================
    // get SM storage info and save the init value
    // ====================================================

    if (0 != CFW_CfgGetSmsStorageInfo(&sms_stor_info, CFW_SMS_STORAGE_SM, nSim))
        return ERR_AT_CMS_MEM_FAIL;

#ifdef PORTING_ON_GOING
    /* SM is default storage of Mobile */
    gATCurrentAtSMsms_par[nSim].nTotal1 = gATCurrentAtSMsms_par[nSim].nTotal2 = gATCurrentAtSMsms_par[nSim].nTotal3 = sms_stor_info.totalSlot;
    gATCurrentAtSMsms_par[nSim].nUsed1 = gATCurrentAtSMsms_par[nSim].nUsed2 = gATCurrentAtSMsms_par[nSim].nUsed3 = sms_stor_info.usedSlot;

    /* if SM storage buffer is full and set flag */
    if ((sms_stor_info.totalSlot == sms_stor_info.usedSlot) && (sms_stor_info.usedSlot != 0))
    {
        /* SM storage is full, set bit1 is 1 */
        g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_SM;
    }

    if (sms_stor_info.unReadRecords != 0)
    {
        g_SMS_CIND_NewMsg += sms_stor_info.unReadRecords;
    }
#endif

    // ====================================================
    // ME storage info and save the init value
    // ====================================================

    if (0 != CFW_CfgGetSmsStorageInfo(&sms_stor_info, CFW_SMS_STORAGE_ME, nSim))
        return ERR_AT_CMS_MEM_FAIL;

#ifdef PORTING_ON_GOING
    /* ME storage buffer is full and set flag */
    if ((sms_stor_info.totalSlot == sms_stor_info.usedSlot) && (sms_stor_info.usedSlot != 0))
    {
        /* ME storage is full, set bit0 is 1 */
        g_SMS_CIND_MemFull[nSim] |= CFW_SMS_STORAGE_ME;
    }

    if (sms_stor_info.unReadRecords != 0) // [+]for cind command new message 2007.11.05
    {
        g_SMS_CIND_NewMsg += sms_stor_info.unReadRecords;
    }

    OSI_LOGI(0x100047db, "We Got gATCurrentuCmer_ind nSim[%d] to %d\n", nSim, gATCurrentuCmer_ind[nSim]);
#endif

#if 0
    // ====================================================
    //  Report to CMER, if necessary
    // ====================================================
    if (gATCurrentuCmer_ind[nSim])
    {
        /* if SM and ME storage bit flag are both "1", bit0 = 1 and bit1 =1 , then the SMSFULL is 1 */
        sprintf(ReportBuff, "+CIEV: \"SMSFULL\",%u",
                   ((CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM))
                    && (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))) ? 1 : 0);
        AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ReportBuff, (uint16_t)AT_StrLen(ReportBuff), 0, nSim);
        AT_MemZero(ReportBuff, 32);
        sprintf(ReportBuff, "+CIEV: \"MESSAGE\",%u", g_SMS_CIND_NewMsg ? 1 : 0);
        AT_SMS_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, ReportBuff, (uint16_t)AT_StrLen(ReportBuff), 0, nSim);
    }
#endif

    // Sync AT CMGF to CFW
    if (CFW_CfgSetSmsFormat(gAtSetting.sim[nSim].cmgf, nSim) != 0)
        return ERR_AT_CMS_ME_FAIL;

    // Just for debug
    OSI_LOGI(0x100047dc, "SMS init OK!");
	

#if defined(CONFIG_QL_OPEN_EXPORT_PKG) && defined(CONFIG_QUEC_PROJECT_FEATURE_SMS)
	if(ql_sms_callback){
		ql_sms_callback(QL_SMS_INIT_OK_IND, NULL);
	}
#endif

    return 0;
}

typedef struct
{
    CFW_DIALNUMBER_V2 number;
    unsigned number_strlen;
    unsigned nindex;
    uint8_t status;
} cmssAsyncContext_t;

static void CMSS_SendRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    unsigned nSim = atCmdGetSim(cmd->engine);
    cmssAsyncContext_t *async = (cmssAsyncContext_t *)cmd->async_ctx;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
	//set cmms to 0
	//set cmms to 0
	if(qpara_cmms == 1)
	{
		if(QcmmsTimer == NULL)
		{
			ql_rtos_timer_create(&QcmmsTimer, (ql_task_t)atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
			if(QcmmsTimer != NULL)
			{
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
		}
		else
		{
			if(osiTimerIsRunning(QcmmsTimer) == true)
			{
				ql_rtos_timer_stop(QcmmsTimer);
				ql_rtos_timer_delete(QcmmsTimer);
				QcmmsTimer = NULL;
				ql_rtos_timer_create(&QcmmsTimer, atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
				if(QcmmsTimer != NULL)
				{
					ql_rtos_timer_start(QcmmsTimer, 5000, 0);
				}
			}
			else
			{
				
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
			
		}
	}
#endif
    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    if (CFW_SMS_STORED_STATUS_UNSENT == async->status)
    {
        uint16_t nUTI = cfwRequestNoWaitUTI();
        uint8_t storage = gAtSetting.sim[nSim].cpms_mem2;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        if (storage == CFW_SMS_STORAGE_MT)
            storage = CFW_SMS_STORAGE_SM;
#else
		if (storage == CFW_SMS_STORAGE_MT)
			storage = CFW_SMS_STORAGE_ME;

#endif

        if (CFW_SmsSetUnSent2Sent(storage, 0x01, async->nindex,
                                  CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ, nUTI, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
    }

    char rsp[32];
    sprintf(rsp, "+CMSS: %u", (unsigned)cfw_event->nParam1);
    atCmdRespInfoText(cmd->engine, rsp);
    atCmdRespOK(cmd->engine);
}

static void CMSS_ReadRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    unsigned nSim = atCmdGetSim(cmd->engine);
    cmssAsyncContext_t *async = (cmssAsyncContext_t *)cmd->async_ctx;

    /* check event type */
    if (0 != cfw_event->nType)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    CFW_SMS_NODE *pNode = (CFW_SMS_NODE *)cfw_event->nParam1;
    CFW_SMS_INFO_UNION *pinfo = (CFW_SMS_INFO_UNION *)&pNode[1];
    atMemFreeLater(pNode);

    unsigned nFormat = gAtSetting.sim[nSim].cmgf;
    uint8_t nstatus_tmp[12] = {0};

    // save the cmss related status
    async->status = pNode->nStatus & 0x0f;

    if (!AT_SMS_StatusMacroFlagToStringOrData(nstatus_tmp, nFormat))
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_STATUS);

    /* just for debug */
    OSI_LOGI(0x10004835, "cmss read msg resp: pNode->nType = %u", pNode->nType);

    //===========================================/
    // modif csdh so ptype0,ptype2 not use 2007.09.27
    // delete ptype0,ptype2 2007.10.10
    //===========================================
    if (pNode->nType == 1) /* text mode,show param, state = unread/read msg */
    {
#if 0
        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *pType1Data = &pinfo->uTxtDevliveredWithHRDV2;

        /* just for debug */
        OSI_LOGI(0x10004816, "pType1Data->length = %u", pType1Data->length);

        CFW_DIALNUMBER_V2 number = {};
        if (async->number.nDialNumberSize != 0)
        {
            number = async->number;
        }
        else
        {
            number.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            number.nDialNumberSize = pType1Data->oa_size;
            memcpy(number.pDialNumber, pType1Data->oa, pType1Data->oa_size);
        }

        if (atFDNProhibit(number.pDialNumber, number.nDialNumberSize, number.nType, nSim))
        {
            sprintf(gAtCfwCtx.g_pCeer, "+CEER: Condition not fullfilled");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
        }

        size_t send_size = pType1Data->length;
        uint8_t *send_data = NULL;
        CFW_SMS_PARAMETER sms_param;
        if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        OSI_LOGI(0, "CMSS_ReadRspCB:dcs=%d", pType1Data->dcs);
        if (sms_param.dcs == 8)
        {
            int unicode_size;
            send_data = mlConvertStr(pType1Data->data, pType1Data->length, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
            atMemUndoFreeLater(send_data);
            send_size = unicode_size;
        }
        else if (sms_param.dcs == 0)
        {
            int gsm_size;
            send_data = mlConvertStr(pType1Data->data, pType1Data->length, ML_CP936, ML_GSM, &gsm_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = gsm_size;
        }
        else if (sms_param.dcs == 4)
        {
            send_data = pType1Data->data;
            send_size = pType1Data->length;
        }
        else
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        OSI_LOGI(0, "CMSS_ReadRspCB: send_size=%d", send_size);
        if (send_size > 420)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMSS_SendRspCB, cmd);
        if (CFW_SmsSendMessage_V2(&number, send_data, send_size, cmd->uti, nSim) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_UNKNOWN_ERROR);
        }
        RETURN_FOR_ASYNC();
#else
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
#endif
    }
    else if (pNode->nType == 3) /* text mode, show param, state =sent/unsent msg */
    {
        CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO_V2 *pType3Data = &pinfo->uSubmitWithHRDV2;

        /* just for debug */
        OSI_LOGI(0x1000481b, "pType3Data->length = %u", pType3Data->length);

        CFW_DIALNUMBER_V2 number = {};
        if (async->number.nDialNumberSize != 0)
        {
            number = async->number;
        }
        else
        {
            number.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            number.nDialNumberSize = pType3Data->da_size;
            memcpy(number.pDialNumber, pType3Data->da, pType3Data->da_size);
        }

        if (atFDNProhibit(number.pDialNumber, number.nDialNumberSize, number.nType, nSim))
        {
            sprintf(gAtCfwCtx.g_pCeer, "+CEER: Condition not fullfilled");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMSS_SendRspCB, cmd);
        if (CFW_SmsSendMessage_V2(&number, pType3Data->data, pType3Data->length, cmd->uti, nSim) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_UNKNOWN_ERROR);
        }
        RETURN_FOR_ASYNC();
    }
    else if (pNode->nType == 7) /* PDU mode, pp msg */
    {
        uint16_t SmsDatalen = 0;
        uint8_t *SmsData = NULL;
        CFW_SMS_PDU_INFO_V2 *pType7Data = &pinfo->uPDUV2;
        uint8_t BCDDataBuff[200] = {0};

        /* just for debug */
        OSI_LOGI(0x10004820, "Original Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

        /* check recv pdu and discard unused char */
        if (!(AT_SMS_CheckPDUIsValid((uint8_t *)pType7Data->pData, (uint8_t *)&(pType7Data->nDataSize), true)))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

        /* just for debug */
        OSI_LOGI(0x10004822, "Worked Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

        if (async->number.nDialNumberSize > 0)
        {
            uint8_t nSCAlen = 0, nDAlen = 0, PDUCopyIndex = 0;
            uint8_t oldDABcdlen = 0;

            nSCAlen = pType7Data->pData[0];
            // READ and UNREAD data have no TP-PID
            if (pType7Data->nStatus == CFW_SMS_STORED_STATUS_READ || pType7Data->nStatus == CFW_SMS_STORED_STATUS_UNREAD)
            {
                nDAlen = pType7Data->pData[nSCAlen + 2];
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
            }
            else
            {
                nDAlen = pType7Data->pData[nSCAlen + 1 + 2];
            }

            oldDABcdlen = (nDAlen % 2 ? (nDAlen / 2) + 1 : (nDAlen / 2));
            SmsDatalen = pType7Data->nDataSize + async->number.nDialNumberSize - oldDABcdlen;

            SmsData = (uint8_t *)calloc(1, SmsDatalen);
            if (SmsData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

            atMemFreeLater(SmsData);

            SmsData[nSCAlen + 3] = async->number_strlen;
            if (async->number.nType != 0)
            {
                SmsData[nSCAlen + 3 + 1] = async->number.nType;
            }
            else
            {
                if (async->number.pDialNumber[0] == 0x68)
                    SmsData[nSCAlen + 3 + 1] = 145;
                else
                    SmsData[nSCAlen + 3 + 1] = 129;
            }

            memcpy(&SmsData[nSCAlen + 5], async->number.pDialNumber, async->number.nDialNumberSize);

            PDUCopyIndex = nSCAlen + 1 + 2 + oldDABcdlen + 2;
            memcpy(&(SmsData[PDUCopyIndex + async->number.nDialNumberSize - oldDABcdlen]),
                   &(pType7Data->pData[PDUCopyIndex]),
                   pType7Data->nDataSize - PDUCopyIndex);
            memcpy(SmsData, pType7Data->pData, nSCAlen + 3);
        }
        else
        {
            CFW_SMS_PARAMETER Info = {0};
            uint32_t nReturn = CFW_CfgGetDefaultSmsParam(&Info, nSim);
            if (nReturn != 0)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

            if (pType7Data->nStatus == CFW_SMS_STORED_STATUS_READ || pType7Data->nStatus == CFW_SMS_STORED_STATUS_UNREAD)
            {
#if 0
                uint8_t nSCAlen = pType7Data->pData[0];
                uint8_t nDAlen = pType7Data->pData[nSCAlen + 2];
                uint8_t oldDABcdlen = (nDAlen % 2 ? (nDAlen / 2) + 1 : (nDAlen / 2));
                uint8_t dataLen = pType7Data->pData[nSCAlen + 1 + 2 + oldDABcdlen + 2 + 7 + 1];
                SmsDatalen = nSCAlen + 1 + 2 + 2 + oldDABcdlen + 2 + 7 + 1 + dataLen;
                SmsData = (uint8_t *)calloc(1, SmsDatalen);
                if (SmsData == NULL)
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
                atMemFreeLater(SmsData);

                memcpy(SmsData, pType7Data->pData, nSCAlen + 1); // sca data
                memcpy(SmsData + nSCAlen + 1 + 1, &(pType7Data->pData[nSCAlen + 1]), 1 + 2 + oldDABcdlen + 2);
                memcpy(SmsData + nSCAlen + 2 + 2 + oldDABcdlen + +2 + 1,
                       &(pType7Data->pData[nSCAlen + 1 + 2 + oldDABcdlen + 2 + 7 + 1]), dataLen + 1);
#else
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
#endif
            }
            else
            {
                SmsDatalen = pType7Data->nDataSize;
                SmsData = (uint8_t *)calloc(1, SmsDatalen);
                if (SmsData == NULL)
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
                atMemFreeLater(SmsData);
                memcpy(SmsData, pType7Data->pData, SmsDatalen);
            }
        }

        /* add sca for read data if have not SCA */
        if (!(SMS_AddSCA2PDU(SmsData, BCDDataBuff, &(SmsDatalen), nSim)))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

        if (_smsFDNProhibit(BCDDataBuff, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMSS_SendRspCB, cmd);
        if (CFW_SmsSendPduMessage(BCDDataBuff, SmsDatalen, cmd->uti, nSim) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_UNKNOWN_ERROR);
        }
        RETURN_FOR_ASYNC();
    }
    else if (pNode->nType == 8) // PDU mode, CB msg
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_ERR_IN_MS);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_ERR_IN_MS);
    }
}

// 3.5.2/4.7 Send Message from Storage
void atCmdHandleCMSS(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    unsigned ret = 0;

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
	uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem1;

#else
	uint8_t nStorage = (gAtSetting.sim[nSim].cpms_mem1 == CFW_SMS_STORAGE_MT ? CFW_SMS_STORAGE_ME:gAtSetting.sim[nSim].cpms_mem1);
#endif
    CFW_SMS_STORAGE_INFO sinfo = {};
    if (CFW_CfgGetSmsStorageInfo(&sinfo, nStorage, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

    if (cmd->type == AT_CMD_SET)
    {
        // +CMSS=<index>[,<da>[,<toda>]]
        bool paramok = true;
        unsigned nindex = atParamUintInRange(cmd->params[0], 1, sinfo.totalSlot, &paramok);
        const char *da = atParamDefStr(cmd->params[1], "", &paramok);
        unsigned toda = atParamDefUint(cmd->params[2], CFW_TELNUMBER_TYPE_UNKNOWN, &paramok);
        if (!paramok || cmd->param_count > 3)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        uint8_t daLen = strlen(da);
        // From storage2's message
        CFW_SMS_STORAGE_INFO sinfo = {};
        if (CFW_CfgGetSmsStorageInfo(&sinfo, gAtSetting.sim[nSim].cpms_mem2, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_MEM_INDEX);

        unsigned mem = gAtSetting.sim[nSim].cpms_mem2;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        if (mem == CFW_SMS_STORAGE_MT)
            mem = CFW_SMS_STORAGE_SM;
#else
		if (mem == CFW_SMS_STORAGE_MT)
			mem = CFW_SMS_STORAGE_ME;
#endif


        cmssAsyncContext_t *async = (cmssAsyncContext_t *)calloc(1, sizeof(cmssAsyncContext_t));
        if (async == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;
        async->nindex = nindex;
        async->number_strlen = daLen;

        if (cmd->param_count >= 2)
        {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
        	if (gAtSetting.cscs == cs_ucs2)
		    {
		    	char *number = NULL;
    			size_t da_len = strlen(da);
		        uint32_t errorCode;
		        number = _generateRealNumberForCmgsAndCmgwInUcs2(da, &da_len, &errorCode);
		        if (errorCode != 0)
		        {
		            if (number)free(number);
		            RETURN_CMS_ERR(cmd->engine, errorCode);
		        }
		        if ((ret = _smsParamNumber(number, toda, &async->number)) != 0)
                	RETURN_CMS_ERR(cmd->engine, ret);
                	
                if (number)free(number);
		    }
		    else
#endif
            if ((ret = _smsParamNumber(da, toda, &async->number)) != 0)
                RETURN_CMS_ERR(cmd->engine, ret);
			
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMSS_ReadRspCB, cmd);
        if (nStorage == CFW_SMS_STORAGE_ME)
            async->nindex = (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim + async->nindex;
        ret = CFW_SmsReadMessage(mem, CFW_SMS_TYPE_PP, async->nindex, cmd->uti, nSim);
        if (ret != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

uint32_t CFW_SmsSendSetting()
{
#ifdef PORTING_ON_GOING
    CFW_SMS_PARAMETER sInfo;

    SUL_ZeroMemory8(&sInfo, sizeof(CFW_SMS_PARAMETER));
    CFW_CfgGetSmsParam(&sInfo, 0, 0);
    sInfo.bearer = 0; //GSM Only.
    sInfo.ssr = 1;    //A status report is requested.
    sInfo.vp = 0;
    sInfo.dcs = 8; //0:gsm 7bit, 8:ucs2
    sInfo.mti = 1;
    sInfo.pid = 0;
    sInfo.rp = 0;
    CFW_CfgSetSmsParam(&sInfo, 0, 0);
#endif
    return 0;
}

uint32_t CFW_SmsSendPresetMessage(uint8_t *pSendNum, uint8_t *pSendData, uint8_t nUTI, uint8_t nSim)
{
#ifdef PORTING_ON_GOING
    uint8_t sTmpNumber[50];
    uint16_t nDataSize = 0x0;
    uint32_t ret = 0x0;
    uint8_t nFormat = 0x0, nNumSize = 0x0, nTmpSize = 0x0;
    CFW_DIALNUMBER sNumber;
    CFW_SMS_PARAMETER sInfo;

    OSI_LOGXI(OSI_LOGPAR_SS, 0x100047ea, "CFW_SmsSendPresetMessage pSendNum=%s, pSendData=%s ", pSendNum, pSendData);
    SUL_ZeroMemory8(&sTmpNumber, 50);
    SUL_ZeroMemory8(&sInfo, sizeof(CFW_SMS_PARAMETER));
    SUL_ZeroMemory8(&sNumber, sizeof(CFW_DIALNUMBER));

    CFW_SmsSendSetting();
    CFW_CfgGetSmsFormat(&nFormat, nSim);
    nTmpSize = (uint8_t)SUL_Strlen(pSendNum);
    SUL_AsciiToGsmBcd(pSendNum, nTmpSize, sTmpNumber);
    nNumSize = nTmpSize / 2 + nTmpSize % 2;

    sNumber.nDialNumberSize = (uint8_t)nNumSize;
    sNumber.pDialNumber = (uint8_t *)CSW_Malloc(nNumSize);
    sNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN; //0x81
    if (sNumber.pDialNumber == NULL)
    {
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(sNumber.pDialNumber, nNumSize);
    SUL_MemCopy8(sNumber.pDialNumber, sTmpNumber, nNumSize);
    CFW_CfgSetSmsFormat(1, nSim);
    CFW_CfgGetSmsParam(&sInfo, 0, nSim);
    nDataSize = (uint16_t)SUL_Strlen(pSendData);
    TS_OutputText(CFW_SMS_TS_ID, "Current DCS =  %d\n ", sInfo.dcs);
    if (sInfo.dcs == 8)
    {
        ret = CFW_SmsSendMessage(&sNumber, pSendData, 6, nUTI, nSim);
    }
    else
    {
        ret = CFW_SmsSendMessage(&sNumber, pSendData, nDataSize, nUTI, nSim);
    }
    CFW_CfgSetSmsFormat(nFormat, nSim);
#endif
    return 0;
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
#define SMS_MAX_SIZE (5 * 160)
typedef struct
{
    CFW_DIALNUMBER_V2 number;
    uint8_t *data;
    unsigned pdu_length;
} cmgsAsyncContext_t;

#endif

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static void CMGS_AsyncCtxDestroy(atCommand_t *cmd)
#else
void CMGS_AsyncCtxDestroy(atCommand_t *cmd)
#endif

{
    cmgsAsyncContext_t *async = (cmgsAsyncContext_t *)cmd->async_ctx;
    if (async == NULL)
        return;

    free(async->data);
    free(async);
    cmd->async_ctx = NULL;
}

static void CMGS_SendRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    char rsp[64];
    sprintf(rsp, "+CMGS: %lu", cfw_event->nParam1);
    atCmdRespInfoText(cmd->engine, rsp);
    atCmdRespOK(cmd->engine);
}

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static void CMGS_TextPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
#else
void CMGS_TextPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
#endif
{
    atCommand_t *cmd = (atCommand_t *)param;
    cmgsAsyncContext_t *async = (cmgsAsyncContext_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    // trim last CTRL_Z
    size--;

    CFW_SMS_PARAMETER sms_param;
    if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    OSI_LOGI(0, "AT CMGS send size/%d, dcs/%d", size, sms_param.dcs);

    int send_size = size;
    uint16_t index = 0;
    uint16_t count = 0;
    uint8_t *tempData = NULL;
    uint8_t *send_data = NULL;

    /* check the dcs value */
    if (sms_param.dcs == 0 || sms_param.dcs == 16) // GSM 7bit
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            tempData = malloc(size / 2 + 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2 + 2);
            atMemFreeLater(tempData);

            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            for (index = 0; index < size / 2; index++)
            {
                int num0 = _paramHex2Num(async->data[index * 2]);
                int num1 = _paramHex2Num(async->data[index * 2 + 1]);
                if (num0 < 0 || num1 < 0)
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

                tempData[count++] = (num0 << 4) | num1;
            }

            // UCS2->GSM
            send_data = mlConvertStr(tempData, count, ML_UTF16BE, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_gsm)
        {
            tempData = malloc(2 * size);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, 2 * size);
            atMemFreeLater(tempData);

            // The input \0 may be 00
            for (index = 0; index < size; index++)
            {
                if (async->data[index] == 0x00)
                {
                    tempData[count++] = 0x5C;
                    tempData[count++] = 0x30;
                }
                else
                    tempData[count++] = async->data[index];
            }

            send_data = mlConvertStr(tempData, count, ML_CP936, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            tempData = malloc(size / 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2);
            atMemFreeLater(tempData);

            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[count++] = (num0 << 4) | num1;
            }

            send_data = mlConvertStr(tempData, count, ML_CP936, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else // default IRA
        {
            send_data = async->data;
            send_size = size;
        }
    }
    else if (sms_param.dcs == 4 || sms_param.dcs == 20) // 8 bit
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            tempData = malloc(size / 2 + 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2 + 2);
            atMemFreeLater(tempData);

            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[count++] = (num0 << 4) | num1;
            }

            // UCS2->CP936
            // 00 33 00 34 -> 33 34
            send_data = mlConvertStr(tempData, count, ML_UTF16BE, ML_CP936, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            send_data = malloc(size / 2);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(send_data, 0x00, size / 2);
            atMemFreeLater(send_data);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                send_data[send_size++] = (num0 << 4) | num1;
            }
        }
        else
        {
            send_data = async->data;
            send_size = size;
        }
    }
    else if (sms_param.dcs == 8 || sms_param.dcs == 24) // UCS2, This for chinese message
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            send_data = malloc(size / 2 + 2);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(send_data, 0x00, size / 2 + 2);
            atMemFreeLater(send_data);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                send_data[send_size++] = (num0 << 4) | num1;
            }
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            tempData = malloc(size / 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2);
            atMemFreeLater(tempData);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[send_size++] = (num0 << 4) | num1;
            }

            int unicode_size;
            send_data = mlConvertStr(tempData, send_size, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = unicode_size;
        }
        else
        {
            int unicode_size;
            send_data = mlConvertStr(async->data, size, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = unicode_size;
        }
    }
    else
    {
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
    }

    OSI_LOGXI(OSI_LOGPAR_M, 0, "AT CMGS send %*s", send_size, send_data);
    if (send_size > 420)
    {
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
    }

    cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGS_SendRspCB, cmd);
    ret = CFW_SmsSendMessage_V2(&async->number, send_data,
                                send_size, cmd->uti, nSim);
    if (ret != 0)
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
	//set cmms to 0
	//set cmms to 0
	if(qpara_cmms == 1)
	{
		if(QcmmsTimer == NULL)
		{
			ql_rtos_timer_create(&QcmmsTimer, (ql_task_t)atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
			if(QcmmsTimer != NULL)
			{
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
		}
		else
		{
			if(osiTimerIsRunning(QcmmsTimer) == true)
			{
				ql_rtos_timer_stop(QcmmsTimer);
				ql_rtos_timer_delete(QcmmsTimer);
				QcmmsTimer = NULL;
				ql_rtos_timer_create(&QcmmsTimer, atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
				if(QcmmsTimer != NULL)
				{
					ql_rtos_timer_start(QcmmsTimer, 5000, 0);
				}			}
			else
			{
				
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
			
		}
	}

#endif
    RETURN_FOR_ASYNC();
}

static void CMGS_TextCmdSet(atCommand_t *cmd)
{
    // +CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC>
    bool paramok = true;
    const char *da = atParamStr(cmd->params[0], &paramok);
    uint8_t toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    static const uint32_t number_type_list[3] = {CFW_TELNUMBER_TYPE_UNKNOWN, CFW_TELNUMBER_TYPE_INTERNATIONAL, CFW_TELNUMBER_TYPE_NATIONAL};
    if (cmd->param_count == 2)
        toda = atParamUintInList(cmd->params[1], number_type_list, sizeof(number_type_list) / sizeof(number_type_list[0]), &paramok);

    if (!paramok || cmd->param_count > 2)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

    char *number = NULL;
    size_t da_len = strlen(da);
    if (gAtSetting.cscs == cs_ucs2)
    {
        uint32_t errorCode;
        number = _generateRealNumberForCmgsAndCmgwInUcs2(da, &da_len, &errorCode);
        if (errorCode != 0)
        {
            if (number != NULL)
                free(number);
            RETURN_CMS_ERR(cmd->engine, errorCode);
        }
    }
    else
    {
        number = malloc(da_len + 1);
        if (number == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
        memset(number, 0x00, da_len);
        memcpy(number, da, da_len);
    }

    cmgsAsyncContext_t *async = (cmgsAsyncContext_t *)malloc(sizeof(*async));
    if (async == NULL)
    {
        free(number);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
    }

    memset(async, 0, sizeof(*async));
    cmd->async_ctx = async;
    cmd->async_ctx_destroy = CMGS_AsyncCtxDestroy;
    async->data = (uint8_t *)malloc(SMS_MAX_SIZE);
    if (async->data == NULL)
    {
        free(number);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
    }

    bool bIsInternational = false;
    if (!AT_SMS_IsValidPhoneNumber(number, da_len, &bIsInternational))
    {
        free(number);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_CHAR);
    }

    char *tempNumber = number;
    if (bIsInternational)
    {
        number++;
        da_len--;
    }

    if (da_len > 20)
    {
        free(tempNumber);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);
    }

    // can't fail due to it is checked.
    async->number.nDialNumberSize = cfwDialStringToBcd(number, da_len, async->number.pDialNumber);

    if (toda != CFW_TELNUMBER_TYPE_INTERNATIONAL &&
        toda != CFW_TELNUMBER_TYPE_NATIONAL &&
        toda != CFW_TELNUMBER_TYPE_UNKNOWN)
    {
        free(tempNumber);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
    }

    /* according to spec, we check dial number first */
    if (bIsInternational)
    {
        toda = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    }
    else if (toda == 0)
    {
        /* no input num type  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else if (toda == CFW_TELNUMBER_TYPE_INTERNATIONAL)
    {
        /* have no '+' , but the num type is 145, replace 129 with is  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else
    {
        /* get the input num type  */
    }
    async->number.nType = toda;

    // display in the screen, and need input
    atCmdRespOutputPrompt(cmd->engine);
    atCmdSetPromptMode(cmd->engine, CMGS_TextPromptCB, cmd, async->data, SMS_MAX_SIZE);
    free(tempNumber);
}

static void CMGS_PduPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    cmgsAsyncContext_t *async = (cmgsAsyncContext_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    // ignore the last CTRL_Z or ESC
    size--;

    uint8_t *pdu_data = (uint8_t *)calloc(1, (size + 1) / 2);
    if (pdu_data == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    atMemFreeLater(pdu_data);
    int pdu_len = cfwHexStrToBytes(async->data, size, pdu_data);
    if (pdu_len < 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

    if (pdu_len != async->pdu_length + 1 + pdu_data[0])
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);

    uint8_t *pdu_with_sca = (uint8_t *)calloc(1, pdu_len + 23);
    if (pdu_with_sca == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    atMemFreeLater(pdu_with_sca);
    uint16_t pdu_len_with_sca = pdu_len;
    if (!SMS_AddSCA2PDU(pdu_data, pdu_with_sca, &pdu_len_with_sca, nSim))
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);

    uint8_t pdu_len_check = pdu_len_with_sca;
    if (!AT_SMS_CheckPDUIsValid(pdu_with_sca, &pdu_len_check, false))
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

    OSI_LOGD(0, "CMGS PDU length/%d/%d/%d", pdu_len, pdu_len_with_sca, pdu_len_check);

    if (_smsFDNProhibit(pdu_with_sca, nSim))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);

    cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGS_SendRspCB, cmd);
    if ((ret = CFW_SmsSendPduMessage(pdu_with_sca, pdu_len_check, cmd->uti, nSim)) != 0)
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
	//set cmms to 0
	//set cmms to 0
	if(qpara_cmms == 1)
	{
		if(QcmmsTimer == NULL)
		{
			ql_rtos_timer_create(&QcmmsTimer, (ql_task_t)atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
			if(QcmmsTimer != NULL)
			{
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
		}
		else
		{
			if(osiTimerIsRunning(QcmmsTimer) == true)
			{
				ql_rtos_timer_stop(QcmmsTimer);
				ql_rtos_timer_delete(QcmmsTimer);
				QcmmsTimer = NULL;
				ql_rtos_timer_create(&QcmmsTimer, atEngineGetThreadId(), QCMMS_TimmerHandler, (void *)cmd);
				if(QcmmsTimer != NULL)
				{
					ql_rtos_timer_start(QcmmsTimer, 5000, 0);
				}
			}
			else
			{
				
				ql_rtos_timer_start(QcmmsTimer, 5000, 0);
			}
			
		}
	}

#endif
    RETURN_FOR_ASYNC();
}

static void CMGS_PduCmdSet(atCommand_t *cmd)
{
    OSI_LOGD(0, "CMGS PDU mode");

    // +CMGS=<length><CR>PDU is given<ctrl-Z/ESC>
    bool paramok = true;
    unsigned length = atParamUintInRange(cmd->params[0], 1, 164, &paramok);
    if (cmd->param_count != 1 || !paramok)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);

    cmgsAsyncContext_t *async = (cmgsAsyncContext_t *)calloc(1, sizeof(*async));
    if (async == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    cmd->async_ctx = async;
    cmd->async_ctx_destroy = CMGS_AsyncCtxDestroy;
    async->pdu_length = length;
    async->data = (uint8_t *)malloc(SMS_MAX_SIZE);
    if (async->data == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    // display in the screen, and need input
    atCmdRespOutputPrompt(cmd->engine);
    atCmdSetPromptMode(cmd->engine, CMGS_PduPromptCB, cmd, async->data, SMS_MAX_SIZE);
}

// 3.5.1/4.3 Send Message
void atCmdHandleCMGS(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        if (gAtSetting.sim[nSim].cmgf == 0) // PDU mode
            CMGS_PduCmdSet(cmd);
        else // text mode
            CMGS_TextCmdSet(cmd);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

typedef struct
{
    CFW_DIALNUMBER_V2 number;
    uint8_t data[SMS_MAX_SIZE];
    unsigned pdu_length;
} cmgcAsyncContext_t;

static uint32_t _smsPduData(const uint8_t *in, unsigned in_size, unsigned pdu_length,
                            uint8_t **out, unsigned *out_size, uint8_t nSim)
{
    uint8_t *pdu_data = (uint8_t *)calloc(1, (in_size + 1) / 2);
    if (pdu_data == NULL)
        return ERR_AT_CMS_MEM_FAIL;

    int pdu_len = cfwHexStrToBytes(in, in_size, pdu_data);
    if (pdu_len < 0)
    {
        free(pdu_data);
        return ERR_AT_CMS_INVALID_PDU_CHAR;
    }

    // if (pdu_len != pdu_length + 1 + pdu_data[0])
    //     return ERR_AT_CMS_INVALID_LEN;

    uint8_t *pdu_with_sca = (uint8_t *)calloc(1, pdu_len + 23);
    if (pdu_with_sca == NULL)
    {
        free(pdu_data);
        return ERR_AT_CMS_MEM_FAIL;
    }

    uint16_t pdu_len_with_sca = pdu_len;
    if (!SMS_AddSCA2PDU(pdu_data, pdu_with_sca, &pdu_len_with_sca, nSim))
    {
        free(pdu_data);
        free(pdu_with_sca);
        return ERR_AT_CMS_INVALID_PDU_PARAM;
    }

    uint8_t pdu_len_check = pdu_len_with_sca;
    if (!AT_SMS_CheckPDUIsValid(pdu_with_sca, &pdu_len_check, false))
    {
        free(pdu_data);
        free(pdu_with_sca);
        return ERR_AT_CMS_INVALID_PDU_CHAR;
    }

    *out = pdu_with_sca;
    *out_size = pdu_len_check;
    free(pdu_data);
    return 0;
}

static void CMGC_SendRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    char rsp[64];
    sprintf(rsp, "+CMGC: %lu", cfw_event->nParam1);
    atCmdRespInfoText(cmd->engine, rsp);
    atCmdRespOK(cmd->engine);
}

static void CMGC_PromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    cmgcAsyncContext_t *async = (cmgcAsyncContext_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    // trim last CTRL_Z
    size--;

    CFW_SMS_PARAMETER sms_param;
    if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    uint8_t nFormat = gAtSetting.sim[nSim].cmgf;
    uint8_t *send_data = NULL;
    unsigned send_size = size;
    OSI_LOGI(0, "CMGC_PromptCB sms_param.dcs==%d", sms_param.dcs);
    if (nFormat) // text mode procedure and send message
    {
        /* check the dcs value */
        if (sms_param.dcs == 0 || sms_param.dcs == 16)
        {
            if (size > 160)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);

            int gsm_size;
            send_data = mlConvertStr(async->data, size, ML_CP936, ML_GSM, &gsm_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = gsm_size;
        }
        else if (sms_param.dcs == 4 || sms_param.dcs == 20)
        {
            if (size > 140)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);

            send_data = async->data;
            send_size = size;
        }
        else if (sms_param.dcs == 8 || sms_param.dcs == 24) // This for chinese message
        {
            int unicode_size;
            send_data = mlConvertStr(async->data, size, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = unicode_size;
        }
        else
        {
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
        }

        if (atFDNProhibit(async->number.pDialNumber, async->number.nDialNumberSize,
                          async->number.nType, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);

        if (send_size >= 10)
        {
            OSI_LOGD(0, "CMGC_PromptCB send_data: %x %x %x %x %x %x %x %x %x %x", send_data[0], send_data[1], send_data[2], send_data[3],
                     send_data[4], send_data[5], send_data[6], send_data[7], send_data[8], send_data[9]);
        }

        if (send_size > 852)
        {
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGC_SendRspCB, cmd);
        if ((ret = CFW_SmsSendMessage_V2(&async->number, send_data, send_size, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
    else // pdu mode procedure and send message
    {
        if ((ret = _smsPduData(async->data, size, async->pdu_length,
                               &send_data, &send_size, nSim)) != 0)
            RETURN_CMS_ERR(cmd->engine, ret);

        atMemFreeLater(send_data);

        if (_smsFDNProhibit(send_data, nSim))
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGC_SendRspCB, cmd);
        if ((ret = CFW_SmsSendPduMessage(send_data, send_size, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
}

// 3.5.5/4.5 Send Command
void atCmdHandleCMGC(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret = 0;

    if (cmd->type == AT_CMD_SET)
    {
        uint8_t nFormat = gAtSetting.sim[nSim].cmgf;
        bool paramok = true;

        cmgcAsyncContext_t *async = (cmgcAsyncContext_t *)calloc(1, sizeof(cmgcAsyncContext_t));
        if (async == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FULL);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        if (nFormat != 0) // text mode
        {
            // +CMGC=<fo>,<ct>[,<pid>[,<mn>[,<da>[,<toda>]]]]<CR>text is entered<ctrl-Z/ESC>

            const char *fo = atParamStr(cmd->params[0], &paramok);
            uint8_t ct = atParamUint(cmd->params[1], &paramok);
            uint8_t pid = atParamDefUint(cmd->params[2], 0, &paramok);
            uint8_t mn = atParamDefUint(cmd->params[3], 0, &paramok);
            const char *da = atParamDefStr(cmd->params[4], "", &paramok);
            uint8_t toda = atParamDefUint(cmd->params[5], 0, &paramok);

            (void)pid;
            (void)mn;
            (void)da;
            (void)toda;

            if (!paramok || cmd->param_count > 6)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            if ((ret = _smsParamNumber(fo, ct, &async->number)) != 0)
                RETURN_CMS_ERR(cmd->engine, ret);
        }
        else if (nFormat == 0) // pdu mode prompt
        {
            // +CMGC=<length><CR>PDU is given<ctrl-Z/ESC>

            uint8_t length = atParamUintInRange(cmd->params[0], 1, 164, &paramok);
            if (!paramok || cmd->param_count > 1)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);

            async->pdu_length = length;
        }

        // display in the screen, and need input
        atCmdRespOutputPrompt(cmd->engine);
        atCmdSetPromptMode(cmd->engine, CMGC_PromptCB, cmd, async->data, SMS_MAX_SIZE);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

typedef struct
{
    CFW_DIALNUMBER_V2 number;
    uint8_t stat;
    uint8_t *data;
    uint8_t pdu_data[SMS_MAX_SIZE];
    uint32_t pdu_length;
} cmgwAsyncCtx_t;

static void CMGW_SendRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    char rsp[64];
    uint8_t nSim = atCmdGetSim(cmd->engine);
    unsigned urcIndex = (unsigned)cfw_event->nParam1 & 0xffff;
    if (gAtSetting.sim[nSim].cpms_mem2 == CFW_SMS_STORAGE_ME)
        urcIndex = urcIndex - ((PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim);
    sprintf(rsp, "+CMGW: %u", urcIndex);
    atCmdRespInfoText(cmd->engine, rsp);
    atCmdRespOK(cmd->engine);
}

static void CMGW_TextPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    cmgwAsyncCtx_t *async = (cmgwAsyncCtx_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    // trim last CTRL_Z
    size--;

    CFW_SMS_PARAMETER sms_param;
    if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    OSI_LOGI(0, "AT CMGW send size/%d, dcs/%d", size, sms_param.dcs);

    int send_size = size;
    uint16_t index = 0;
    uint16_t count = 0;
    uint8_t *tempData = NULL;
    uint8_t *send_data = NULL;

    /* check the dcs value */
    if (sms_param.dcs == 0 || sms_param.dcs == 16) // GSM 7bit
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            tempData = malloc(size / 2 + 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2 + 2);
            atMemFreeLater(tempData);

            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[count++] = (num0 << 4) | num1;
            }

            // UCS2->GSM
            send_data = mlConvertStr(tempData, count, ML_UTF16BE, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_gsm || gAtSetting.cscs == cs_gbk)
        {
            tempData = malloc(2 * size);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, 2 * size);
            atMemFreeLater(tempData);

            // The input \0 may be 00
            for (index = 0; index < size; index++)
            {
                if (async->data[index] == 0x00)
                {
                    tempData[count++] = 0x5C;
                    tempData[count++] = 0x30;
                }
                else
                    tempData[count++] = async->data[index];
            }

            send_data = mlConvertStr(tempData, count, ML_CP936, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            tempData = malloc(size / 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2);
            atMemFreeLater(tempData);

            if (!_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[count++] = (num0 << 4) | num1;
            }

            send_data = mlConvertStr(tempData, count, ML_CP936, ML_GSM, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else // default IRA
        {
            send_data = async->data;
            send_size = size;
        }
    }
    else if (sms_param.dcs == 4 || sms_param.dcs == 20) // 8 bit
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            tempData = malloc(size / 2 + 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2 + 2);
            atMemFreeLater(tempData);

            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[count++] = (num0 << 4) | num1;
            }

            // UCS2->CP936
            // 00 33 00 34 -> 33 34
            send_data = mlConvertStr(tempData, count, ML_UTF16BE, ML_CP936, &send_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            send_data = malloc(size / 2);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(send_data, 0x00, size / 2);
            atMemFreeLater(send_data);

            if (!_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                send_data[send_size++] = (num0 << 4) | num1;
            }
        }
        else
        {
            send_data = async->data;
            send_size = size;
        }
    }
    else if (sms_param.dcs == 8 || sms_param.dcs == 24) // UCS2, This for chinese message
    {
        if (gAtSetting.cscs == cs_ucs2)
        {
            if (((size % 4) != 0) || !_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            // CP936->UCS2
            // real value 34
            // 30 30 33 33 30 30 33 34 -> 00 33 00 34
            send_data = malloc(size / 2 + 2);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(send_data, 0x00, size / 2 + 2);
            atMemFreeLater(send_data);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                send_data[send_size++] = (num0 << 4) | num1;
            }
        }
        else if (gAtSetting.cscs == cs_hex)
        {
            if (!((size > 0) || ((size % 2) == 0)))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            tempData = malloc(size / 2);
            if (tempData == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            memset(tempData, 0x00, size / 2);
            atMemFreeLater(tempData);

            if (!_checkNumber(async->data, size))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            send_size = 0;
            for (index = 0; index < size / 2; index++)
            {
                uint8_t num0 = _paramHex2Num(async->data[index * 2]);
                uint8_t num1 = _paramHex2Num(async->data[index * 2 + 1]);
                tempData[send_size++] = (num0 << 4) | num1;
            }

            int unicode_size;
            send_data = mlConvertStr(tempData, send_size, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = unicode_size;
        }
        else
        {
            int unicode_size;
            send_data = mlConvertStr(async->data, size, ML_CP936, ML_UTF16BE, &unicode_size);
            if (send_data == NULL)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);

            atMemFreeLater(send_data);
            send_size = unicode_size;
        }
    }
    else
    {
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
    }

    OSI_LOGXI(OSI_LOGPAR_M, 0, "AT CMGW send %*s", send_size, send_data);
    if (send_size > 420)
    {
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
    }

    uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem2;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
    if (nStorage == CFW_SMS_STORAGE_MT)
    {
        nStorage = CFW_SMS_STORAGE_SM;
    }
#else
	if (nStorage == CFW_SMS_STORAGE_MT)
	{
		nStorage = CFW_SMS_STORAGE_ME;
	}

#endif


    cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGW_SendRspCB, cmd);
    ret = CFW_SmsWriteMessage_V2(&async->number, send_data, send_size,
                                 0, nStorage, CFW_SMS_TYPE_PP,
                                 async->stat, cmd->uti, nSim);

    if (ret != 0)
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
    }
    RETURN_FOR_ASYNC();
}

static void CMGW_PduPromptCB(void *param, atCmdPromptEndMode_t end_mode, size_t size)
{
    atCommand_t *cmd = (atCommand_t *)param;
    cmgwAsyncCtx_t *async = (cmgwAsyncCtx_t *)cmd->async_ctx;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;
    OSI_LOGD(0, "CMGW_PduPromptCB");
    if (end_mode == AT_PROMPT_END_ESC)
        RETURN_OK(cmd->engine);
    if (end_mode == AT_PROMPT_END_OVERFLOW)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    // trim last CTRL_Z
    size--;

    uint8_t *pdu_data = (uint8_t *)calloc(1, (size + 1) / 2);
    if (pdu_data == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    atMemFreeLater(pdu_data);
    int pdu_len = cfwHexStrToBytes(async->pdu_data, size, pdu_data);
    if (pdu_len < 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

    if (pdu_len != async->pdu_length + 1 + pdu_data[0])
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);

    uint8_t *send_data = NULL;
    unsigned send_size = size;

    if ((ret = _smsPduData(async->pdu_data, size, async->pdu_length,
                           &send_data, &send_size, nSim)) != 0)
        RETURN_CMS_ERR(cmd->engine, ret);

    atMemFreeLater(send_data);

    if (_smsFDNProhibit(send_data, nSim))
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_SIM_CONDITION_NO_FULLFILLED);
    uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem2;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
    if (nStorage == CFW_SMS_STORAGE_MT)
        nStorage = CFW_SMS_STORAGE_SM;
#else
	if (nStorage == CFW_SMS_STORAGE_MT)
		nStorage = CFW_SMS_STORAGE_ME;
#endif

    CFW_DIALNUMBER_V2 pNumber;
    cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGW_SendRspCB, cmd);
    ret = CFW_SmsWriteMessage_V2(&pNumber, send_data, send_size,
                                 0, nStorage, CFW_SMS_TYPE_PP,
                                 async->stat, cmd->uti, nSim);
    if (ret != 0)
    {
        cfwReleaseUTI(cmd->uti);
        RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
    }
    RETURN_FOR_ASYNC();
}

static void CMGW_TextCmdSet(atCommand_t *cmd)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
	if(cmd->type ==  AT_CMD_EXE)
	{
		cmd->param_count = 0;
		cmgwAsyncCtx_t *async = (cmgwAsyncCtx_t *)calloc(1, sizeof(*async) + SMS_MAX_SIZE);
		if (async == NULL)
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
		const osiValueStrMap_t *stat_m = osiVsmapFindByIStr(gSmsState, "STO UNSENT");
		
		cmd->async_ctx = async;
		cmd->async_ctx_destroy = atCommandAsyncCtxFree;
		async->stat = stat_m->value;
		async->data = (uint8_t *)async + sizeof(*async);
		// display in the screen, and need input
		atCmdRespOutputPrompt(cmd->engine);
		atCmdSetPromptMode(cmd->engine, CMGW_TextPromptCB, cmd, async->data, SMS_MAX_SIZE);
		return;
	}
#endif
    // +CMGW[=<oa/da>[,<tooa/toda>[,<stat>]]]<CR>text is entered<ctrl-Z/ESC>
    bool paramok = true;
    const char *da = atParamStr(cmd->params[0], &paramok);
    uint8_t toda = atParamDefUint(cmd->params[1], CFW_TELNUMBER_TYPE_UNKNOWN, &paramok);
    const char *stat_str = atParamDefStr(cmd->params[2], "STO UNSENT", &paramok);
    if (!paramok || cmd->param_count > 3)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    const osiValueStrMap_t *stat_m = osiVsmapFindByIStr(gSmsState, stat_str);
    if (stat_m == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

    char *number = NULL;
    size_t da_len = strlen(da);
    if (gAtSetting.cscs == cs_ucs2)
    {
        uint32_t errorCode;
        number = _generateRealNumberForCmgsAndCmgwInUcs2(da, &da_len, &errorCode);
        if (errorCode != 0)
        {
            if (number)
                free(number);
            RETURN_CMS_ERR(cmd->engine, errorCode);
        }
    }
    else
    {
        number = malloc(da_len + 1);
        if (number == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

        memset(number, 0x00, da_len);
        memcpy(number, da, da_len);
    }

    cmgwAsyncCtx_t *async = (cmgwAsyncCtx_t *)calloc(1, sizeof(*async) + SMS_MAX_SIZE);
    if (async == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

    cmd->async_ctx = async;
    cmd->async_ctx_destroy = atCommandAsyncCtxFree;
    async->stat = stat_m->value;
    async->data = (uint8_t *)async + sizeof(*async);

    bool bIsInternational = false;
    if (!AT_SMS_IsValidPhoneNumber(number, da_len, &bIsInternational))
    {
        free(number);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_CHAR);
    }

    char *tempNumber = number;
    if (bIsInternational)
    {
        number++;
        da_len--;
    }

    if (da_len > 20)
    {
        free(tempNumber);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_LEN);
    }

    // can't fail due to it is checked.
    async->number.nDialNumberSize = cfwDialStringToBcd(number, da_len, async->number.pDialNumber);

    if (toda != CFW_TELNUMBER_TYPE_INTERNATIONAL &&
        toda != CFW_TELNUMBER_TYPE_NATIONAL &&
        toda != CFW_TELNUMBER_TYPE_UNKNOWN)
    {
        free(tempNumber);
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
    }

    /* according to spec, we check dial number first */
    if (bIsInternational)
    {
        toda = CFW_TELNUMBER_TYPE_INTERNATIONAL;
    }
    else if (toda == 0)
    {
        /* no input num type  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else if (toda == CFW_TELNUMBER_TYPE_INTERNATIONAL)
    {
        /* have no '+' , but the num type is 145, replace 129 with is  */
        toda = CFW_TELNUMBER_TYPE_UNKNOWN;
    }
    else
    {
        /* get the input num type  */
    }
    async->number.nType = toda;

    // display in the screen, and need input
    atCmdRespOutputPrompt(cmd->engine);
    atCmdSetPromptMode(cmd->engine, CMGW_TextPromptCB, cmd, async->data, SMS_MAX_SIZE);
    free(tempNumber);
}

static void CMGW_PduCmdSet(atCommand_t *cmd)
{
    bool paramok = true;
    OSI_LOGD(0, "CMGW PDU mode");
    if (cmd->param_count != 1 && cmd->param_count != 2)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
    uint32_t pdu_length = atParamUintInRange(cmd->params[0], 0, 164, &paramok);
    if (!paramok)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);

    cmgwAsyncCtx_t *async = (cmgwAsyncCtx_t *)calloc(1, sizeof(*async));
    if (async == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
    async->stat = CFW_SMS_STORED_STATUS_UNSENT; // set default value of status

    uint32_t pdu_stat = 0;
    if (cmd->param_count == 2)
    {
        pdu_stat = atParamUintInRange(cmd->params[1], 0, 3, &paramok);
        if (!paramok)
        {
            free(async);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
        }
        if (pdu_stat < 3)
            async->stat = 1 << pdu_stat;
        else if (pdu_stat == 3)
            async->stat = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ;
        else
        {
            free(async);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
        }
    }

    cmd->async_ctx = async;
    cmd->async_ctx_destroy = atCommandAsyncCtxFree;
    async->pdu_length = pdu_length;

    atCmdRespOutputPrompt(cmd->engine);
    atCmdSetPromptMode(cmd->engine, CMGW_PduPromptCB, cmd, async->pdu_data, SMS_MAX_SIZE);
}

// 3.5.3 Write Message to Memory
void atCmdHandleCMGW(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        CFW_SMS_STORAGE_INFO sStorageInfo = {};
        if (CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        if (!gAtCfwCtx.sim[nSim].sms_init_done)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

        if (gAtSetting.sim[nSim].cmgf == 0) // PDU mode
            CMGW_PduCmdSet(cmd);
        else // text mode
            CMGW_TextCmdSet(cmd);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
	else if(cmd->type == AT_CMD_EXE)
	{
		CFW_SMS_STORAGE_INFO sStorageInfo = {};
		if (CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim) != 0)
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
		
		if (!gAtCfwCtx.sim[nSim].sms_init_done)
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
			
		if (gAtSetting.sim[nSim].cmgf == 0) // PDU mode
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

		CMGW_TextCmdSet(cmd);
	}
#endif
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

static uint8_t _getSmsStorageInfo(const char *mem, CFW_SMS_STORAGE_INFO *info, uint8_t *storId, uint8_t nSim)
{
    static const osiValueStrMap_t stors[] = {
        {CFW_SMS_STORAGE_ME, "ME"},
        {CFW_SMS_STORAGE_SM, "SM"},
        {CFW_SMS_STORAGE_MT, "MT"},
        {0, NULL},
    };

    uint8_t stor;
    const osiValueStrMap_t *stor_m = osiVsmapFindByIStr(stors, mem);
    if (stor_m == NULL)
        return 1;
    stor = stor_m->value;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
    if (stor == CFW_SMS_STORAGE_MT)
        stor = CFW_SMS_STORAGE_SM;
#else
	if (stor == CFW_SMS_STORAGE_MT)
		stor = CFW_SMS_STORAGE_ME;

#endif


    uint8_t nOption = 0;
    uint8_t nNewSmsStorage = 0;

    if (CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim))
        return 2;
    if (CFW_CfgSetNewSmsOption(nOption, stor_m->value, nSim))
        return 2;

    if (CFW_CfgGetSmsStorageInfo(info, stor, nSim))
        return 2;
    if (CFW_CfgSetSmsStorageInfo(info, stor, nSim))
        return 2;
    *storId = stor_m->value;
    return 0;
}

// 3.2.2 Preferred Message Storage
void atCmdHandleCPMS(atCommand_t *cmd)
{
    static const osiValueStrMap_t stors[] = {
        {CFW_SMS_STORAGE_ME, "ME"},
        {CFW_SMS_STORAGE_SM, "SM"},
        {CFW_SMS_STORAGE_MT, "MT"},
        {0, NULL},
    };

    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CPMS=<mem1>[,<mem2>[,<mem3>]]
        if (cmd->param_count > 3)
        {
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        }

        bool paramok = true;
        uint8_t storId = 0;
        CFW_SMS_STORAGE_INFO info1 = {};
        CFW_SMS_STORAGE_INFO info2 = {};
        CFW_SMS_STORAGE_INFO info3 = {};
        char rsp[64];
        char *prsp = rsp;

        const char *mem1 = atParamStr(cmd->params[0], &paramok);
        uint8_t result = _getSmsStorageInfo(mem1, &info1, &storId, nSim);
        if (!paramok || result != 0)
        {
            if (result == 2)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            else
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        }
        gAtSetting.sim[nSim].cpms_mem1 = storId;

        // mem2 and mem3 use saved data
        if (cmd->param_count == 1)
        {
            storId = gAtSetting.sim[nSim].cpms_mem2;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
            if (storId == CFW_SMS_STORAGE_MT)
                storId = CFW_SMS_STORAGE_SM;
#else
			if (storId == CFW_SMS_STORAGE_MT)
				storId = CFW_SMS_STORAGE_ME;

#endif
            if (CFW_CfgGetSmsStorageInfo(&info2, storId, nSim))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

            storId = gAtSetting.sim[nSim].cpms_mem3;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
            if (storId == CFW_SMS_STORAGE_MT)
                storId = CFW_SMS_STORAGE_SM;
#else
			if (storId == CFW_SMS_STORAGE_MT)
				storId = CFW_SMS_STORAGE_ME;

#endif


            if (CFW_CfgGetSmsStorageInfo(&info3, storId, nSim))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
        }
        else
        {
            const char *mem2 = atParamStr(cmd->params[1], &paramok);
            result = _getSmsStorageInfo(mem2, &info2, &storId, nSim);
            if (!paramok || result != 0)
            {
                if (result == 2)
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
                else
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
            gAtSetting.sim[nSim].cpms_mem2 = storId;

            // mem3 use saved data
            if (cmd->param_count == 2)
            {
                storId = gAtSetting.sim[nSim].cpms_mem3;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS

                if (storId == CFW_SMS_STORAGE_MT)
                    storId = CFW_SMS_STORAGE_SM;
#else

				if (storId == CFW_SMS_STORAGE_MT)
					storId = CFW_SMS_STORAGE_ME;

#endif


                if (CFW_CfgGetSmsStorageInfo(&info3, storId, nSim))
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
            }
            else
            {
                const char *mem3 = atParamStr(cmd->params[2], &paramok);
                result = _getSmsStorageInfo(mem3, &info3, &storId, nSim);
                if (!paramok || result != 0)
                {
                    if (result == 2)
                        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
                    else
                        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
                }

                gAtSetting.sim[nSim].cpms_mem3 = storId;
            }
        }
        prsp += sprintf(prsp, "+CPMS: %d,%d,%d,%d,%d,%d", info1.usedSlot, info1.totalSlot, info2.usedSlot, info2.totalSlot, info3.usedSlot, info3.totalSlot);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        CFW_SMS_STORAGE_INFO info1 = {};
        CFW_SMS_STORAGE_INFO info2 = {};
        CFW_SMS_STORAGE_INFO info3 = {};

#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        if (CFW_CfgGetSmsStorageInfo(&info1, gAtSetting.sim[nSim].cpms_mem1, nSim))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

        if (CFW_CfgGetSmsStorageInfo(&info2, gAtSetting.sim[nSim].cpms_mem2, nSim))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

        if (CFW_CfgGetSmsStorageInfo(&info3, gAtSetting.sim[nSim].cpms_mem3, nSim))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
#else
		if (CFW_CfgGetSmsStorageInfo(&info1, 
		   gAtSetting.sim[nSim].cpms_mem1 == CFW_SMS_STORAGE_MT?CFW_SMS_STORAGE_ME:gAtSetting.sim[nSim].cpms_mem1, nSim))
		   RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

	    if (CFW_CfgGetSmsStorageInfo(&info2, 
		   gAtSetting.sim[nSim].cpms_mem2 == CFW_SMS_STORAGE_MT?CFW_SMS_STORAGE_ME:gAtSetting.sim[nSim].cpms_mem2, nSim))
		   RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

	    if (CFW_CfgGetSmsStorageInfo(&info3, 
		   gAtSetting.sim[nSim].cpms_mem3 == CFW_SMS_STORAGE_MT?CFW_SMS_STORAGE_ME:gAtSetting.sim[nSim].cpms_mem3, nSim))
		   RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
#endif

        const osiValueStrMap_t *stor1 = osiVsmapFindByVal(stors, gAtSetting.sim[nSim].cpms_mem1);
        const osiValueStrMap_t *stor2 = osiVsmapFindByVal(stors, gAtSetting.sim[nSim].cpms_mem2);
        const osiValueStrMap_t *stor3 = osiVsmapFindByVal(stors, gAtSetting.sim[nSim].cpms_mem3);

        // shouldn't happen
        if (stor1 == NULL || stor2 == NULL || stor3 == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);

        char rsp[64];
        sprintf(rsp, "+CPMS: \"%s\",%u,%u,\"%s\",%u,%u,\"%s\",%u,%u",
                stor1->str, info1.usedSlot, info1.totalSlot,
                stor2->str, info2.usedSlot, info2.totalSlot,
                stor3->str, info3.usedSlot, info3.totalSlot);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		atCmdRespInfoText(cmd->engine, "+CPMS: (\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")");
#else
		atCmdRespInfoText(cmd->engine, "+CPMS: (\"ME\",\"SM\",\"MT\"),(\"ME\",\"SM\",\"MT\"),(\"ME\",\"SM\",\"MT\")");
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

static void CMGL_ListRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = atCmdGetSim(cmd->engine);

    /* check event type */
    if (0 != cfw_event->nType){
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    void *pListResult = (void *)cfw_event->nParam1;
    CFW_SMS_NODE_EX node;
    char number[TEL_NUMBER_MAX_LEN] = {0};
    char *rsp = (char *)malloc(1024);
    atMemFreeLater(rsp);

    for (int nNodeIndex = 0; nNodeIndex < (cfw_event->nParam2 >> 16); nNodeIndex++)
    {
        if (CFW_SmsGetMessageNodeEx(pListResult, nNodeIndex, &node, nSim) != 0)
        {
            OSI_LOGI(0x10004813, "list sms: some one message node exception");
            continue; /* Find next node */
        }

        unsigned status = node.node.nStatus & 0x0f;
        if (CFW_SMS_STORED_STATUS_UNREAD == status)
        {
            uint16_t nUTI = cfwRequestNoWaitUTI();

            if (CFW_SmsSetUnRead2Read(node.node.nStorage, 0x01, node.node.nConcatCurrentIndex,
                                      nUTI, nSim) != 0)
            {
                CFW_SmsListFree(pListResult, nNodeIndex++, cfw_event->nParam2 >> 16, nSim);
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
        }

        // printf the sms message node type
        OSI_LOGI(0x10004815, "list msg: pPendSmsMsgNode->nType = %u", node.node.nType);

        // modif csdh so ptype0,ptype2 not use 2007.09.27
        // delete ptype0,ptype2 2007.10.10
        // text mode,show param, state = unread/read msg
        if (node.node.nType == 1)
        {
            CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO_V2 *pType1Data = &node.info.uTxtDevliveredWithHRDV2;

            /* just for debug */
            OSI_LOGI(0x10004816, "pType1Data->length = %u", pType1Data->length);

            int number_size = 0;
            if (0xD0 != pType1Data->tooa)
                number_size = cfwBcdToDialString(pType1Data->oa, pType1Data->oa_size, number);
            else
                number_size = cfwDecode7Bit(pType1Data->oa, number, pType1Data->oa_size);

            if (number_size < 0 || number_size >= TEL_NUMBER_MAX_LEN)
                continue;

            number[number_size] = '\0';
            const char *number_prefix = NULL;

            char *urcOaNumber = NULL;
            if (gAtSetting.cscs == cs_ucs2)
            {
                char oaNumber[TEL_NUMBER_MAX_LEN + 2] = {0};
                if (pType1Data->tooa == CFW_TELNUMBER_TYPE_INTERNATIONAL)
                {
                    oaNumber[0] = '+';
                    memcpy(oaNumber + 1, number, number_size);
                }
                else
                    memcpy(oaNumber, number, number_size);

                number_prefix = "";
                urcOaNumber = generateUrcUcs2Data(oaNumber, strlen(oaNumber));
            }
            else
                number_prefix = (pType1Data->tooa == CFW_TELNUMBER_TYPE_INTERNATIONAL) ? "+" : "";

            if (gAtSetting.sim[nSim].cpms_mem1 == CFW_SMS_STORAGE_ME)
                node.node.nConcatCurrentIndex -= (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim;

            if (gAtSetting.sim[nSim].csdh == 1) /* CSDH=1 */
            {
                const char *str1 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\",%u,%u";
                const char *str2 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\",%u,%u";
                const char *str = (pType1Data->scts.iZone < 0) ? str2 : str1;

                // output param first
                sprintf(rsp, str,
                        node.node.nConcatCurrentIndex,
                        _smsStatusStr(status),
                        number_prefix,
                        urcOaNumber ? urcOaNumber : number,
                        pType1Data->scts.uYear,
                        pType1Data->scts.uMonth,
                        pType1Data->scts.uDay,
                        pType1Data->scts.uHour,
                        pType1Data->scts.uMinute,
                        pType1Data->scts.uSecond,
                        pType1Data->scts.iZone,
                        pType1Data->tooa,
                        pType1Data->length);
            }
            else /* csdh=0 */
            {
                const char *str1 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u+%02d\"";
                const char *str2 = "+CMGL: %u,\"%s\",\"%s%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%03d\"";
                const char *str = (pType1Data->scts.iZone < 0) ? str2 : str1;

                // output param first
                sprintf(rsp, str,
                        node.node.nConcatCurrentIndex,
                        _smsStatusStr(status),
                        number_prefix,
                        urcOaNumber ? urcOaNumber : number,
                        pType1Data->scts.uYear,
                        pType1Data->scts.uMonth,
                        pType1Data->scts.uDay,
                        pType1Data->scts.uHour,
                        pType1Data->scts.uMinute,
                        pType1Data->scts.uSecond,
                        pType1Data->scts.iZone);
            }

            if (urcOaNumber)
                free(urcOaNumber);

            atCmdRespInfoText(cmd->engine, rsp);

            /* Chinese message list */
            char *data = _smsNodeDataStr(node.node.nType, &node.info, nSim);
            if (data == NULL)
            {
                CFW_SmsListFree(pListResult, nNodeIndex++, cfw_event->nParam2 >> 16, nSim);
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
            }

            atCmdRespInfoText(cmd->engine, data);
            free(data);
        }
        else if (node.node.nType == 3) // text mode, show param, state =sent/unsent msg
        {
            CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO_V2 *pType3Data = &node.info.uSubmitWithHRDV2;

            /* just for debug */
            OSI_LOGI(0x1000481b, "pType3Data->length = %u", pType3Data->length);

            // change to ascii and get size
            int number_size = cfwBcdToDialString(pType3Data->da, pType3Data->da_size, number);
            if (number_size < 0 || number_size >= TEL_NUMBER_MAX_LEN)
                continue;

            number[number_size] = '\0';
            const char *number_prefix = NULL;

            char *urcOaNumber = NULL;
            if (gAtSetting.cscs == cs_ucs2)
            {
                char oaNumber[TEL_NUMBER_MAX_LEN + 2] = {0};
                if (pType3Data->toda == CFW_TELNUMBER_TYPE_INTERNATIONAL)
                {
                    oaNumber[0] = '+';
                    memcpy(oaNumber + 1, number, number_size);
                }
                else
                    memcpy(oaNumber, number, number_size);

                number_prefix = "";
                urcOaNumber = generateUrcUcs2Data(oaNumber, strlen(oaNumber));
            }
            else
                number_prefix = (pType3Data->toda == CFW_TELNUMBER_TYPE_INTERNATIONAL) ? "+" : "";

            if (gAtSetting.sim[nSim].cpms_mem1 == CFW_SMS_STORAGE_ME)
                node.node.nConcatCurrentIndex -= (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim;
            if (gAtSetting.sim[nSim].csdh == 1) // CSDH=1
            {
                sprintf(rsp, "+CMGL: %u,\"%s\",\"%s%s\",,%u,%u",
                        node.node.nConcatCurrentIndex,
                        _smsStatusStr(status),
                        number_prefix,
                        urcOaNumber ? urcOaNumber : number,
                        pType3Data->toda,
                        pType3Data->length);
            }
            else // csdh=0
            {
                // [+]for csdh use global 2007.09.27
                // output param first
                sprintf(rsp, "+CMGL: %u,\"%s\",\"%s%s\"",
                        node.node.nConcatCurrentIndex,
                        _smsStatusStr(status),
                        number_prefix,
                        urcOaNumber ? urcOaNumber : number);
            }

            if (urcOaNumber)
                free(urcOaNumber);

            atCmdRespInfoText(cmd->engine, rsp);

            char *data = _smsNodeDataStr(node.node.nType, &node.info, nSim);
            if (data == NULL)
            {
                CFW_SmsListFree(pListResult, nNodeIndex++, cfw_event->nParam2 >> 16, nSim);
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
            }

            atCmdRespInfoText(cmd->engine, data);
            free(data);
        }
        else if (node.node.nType == 7) // PDU mode, pp msg
        {
            CFW_SMS_PDU_INFO_V2 *pType7Data = (CFW_SMS_PDU_INFO_V2 *)&node.info.uPDUV2;

            /* just for debug */
            OSI_LOGI(0x10004820, "Original Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

            /* check the PDU and discard unused char */
            if (!AT_SMS_CheckPDUIsValid(pType7Data->pData, (uint8_t *)&(pType7Data->nDataSize), true))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

            /* just for debug */
            OSI_LOGI(0x10004822, "Worked Data: pType7Data->nDataSize = %u", pType7Data->nDataSize);

            if (gAtSetting.sim[nSim].cpms_mem1 == CFW_SMS_STORAGE_ME)
                node.node.nConcatCurrentIndex -= (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim;
            /* output param first */
            sprintf(rsp, "+CMGL: %u,%u,,%u",
                    node.node.nConcatCurrentIndex,
                    _smsCfwStatusToAt(pType7Data->nStatus),
                    pType7Data->nDataSize - (pType7Data->pData[0] + 1));
            atCmdRespInfoText(cmd->engine, rsp);

            uint8_t BCDDataBuff[200] = {0};
            uint8_t ASCIIDataBuff[400] = {0};

            /* add sca if have not sca */
            uint16_t nTotalDataLen = pType7Data->nDataSize;
            if (!SMS_AddSCA2PDU(pType7Data->pData, BCDDataBuff, &nTotalDataLen, nSim))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

            /* BCD to ASCII */
            if (!SMS_PDUBCDToASCII(BCDDataBuff, &nTotalDataLen, ASCIIDataBuff))
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

            atCmdRespInfoNText(cmd->engine, (const char *)ASCIIDataBuff, nTotalDataLen);
        }
        else if (node.node.nType == 8) // PDU mode, CB msg
        {
            CFW_SmsListFree(pListResult, nNodeIndex++, cfw_event->nParam2 >> 16, nSim);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_RFQ_FAC_NOT_IMP);
        }
        else
        {
            CFW_SmsListFree(pListResult, nNodeIndex++, cfw_event->nParam2 >> 16, nSim);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_UNKNOWN_ERROR);
        }
    }

    CFW_SmsListFree(pListResult, cfw_event->nParam2 >> 16, cfw_event->nParam2 >> 16, nSim);
    atCmdRespOK(cmd->engine);
}

// 3.4.2/4.1 List Messages
void atCmdHandleCMGL(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

    unsigned nFormat = gAtSetting.sim[nSim].cmgf;
    unsigned storage = gAtSetting.sim[nSim].cpms_mem1;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
    if (storage == CFW_SMS_STORAGE_MT)
        storage = CFW_SMS_STORAGE_SM;
#else
	if (storage == CFW_SMS_STORAGE_MT)
		storage = CFW_SMS_STORAGE_ME;
#endif


    CFW_SMS_STORAGE_INFO sStorageInfo = {0};
    if (CFW_CfgGetSmsStorageInfo(&sStorageInfo, storage, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

    OSI_LOGD(0, "SMS stor/%d used/%d total/%d unread/%d read/%d unsent/%d unknown/%d",
             sStorageInfo.storageId, sStorageInfo.usedSlot, sStorageInfo.totalSlot,
             sStorageInfo.unReadRecords, sStorageInfo.readRecords, sStorageInfo.sentRecords,
             sStorageInfo.unsentRecords, sStorageInfo.unknownRecords);

    if (cmd->type == AT_CMD_SET)
    {
        // +CMGL[=<stat>]
        bool paramok = true;
        unsigned status;
        if (cmd->param_count == 0)
        {
            status = CFW_SMS_STORED_STATUS_UNREAD;
        }
        else if (nFormat != 0)
        {
            status = atParamUintByStrMap(cmd->params[0], gSmsState, &paramok);
            if (!paramok || cmd->param_count > 1)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_TXT_PARAM);
        }
        else
        {
            unsigned pdu_stat = atParamUint(cmd->params[0], &paramok);
            if (!paramok || cmd->param_count > 1)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);

            status = _smsAtStatusToCfw(pdu_stat);
            if (status == 0xff)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_PARAM);
        }

        //===========================================
        // 1. list status is: READ, but readRecords is 0, return OK immediately
        // 2. list status is: UNREAD, but unReadRecords is 0, return OK immediately
        // 3. list status is: SENT, but sentRecords is 0, return OK immediately
        // 4. list status is: UNSENT, but unsentRecords is 0, return OK immediately
        // 5. list status is: ALL, but usedSlot is 0, return OK immediately
        //===========================================
        if (((CFW_SMS_STORED_STATUS_READ == status) && (0 == sStorageInfo.readRecords)) ||
            ((CFW_SMS_STORED_STATUS_UNREAD == status) && (0 == sStorageInfo.unReadRecords)) ||
            (((CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ & status) != 0) && (0 == sStorageInfo.sentRecords)) ||
            ((CFW_SMS_STORED_STATUS_UNSENT == status) && (0 == sStorageInfo.unsentRecords)) ||
            ((CFW_SMS_STORED_STATUS_STORED_ALL == status) && (0 == sStorageInfo.usedSlot)))
            RETURN_OK(cmd->engine);

        CFW_SMS_LIST ListInfo = {};
        ListInfo.nCount = sStorageInfo.totalSlot;
        if (storage == CFW_SMS_STORAGE_ME)
            ListInfo.nStartIndex = (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim + 1;
        else
            ListInfo.nStartIndex = 1;
        ListInfo.nStorage = storage;
        ListInfo.nType = CFW_SMS_TYPE_PP;
        ListInfo.nStatus = status;
        ListInfo.nOption = CFW_SMS_LIST_OPTION__TIME_DESCEND_ORDER;

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGL_ListRspCB, cmd);
        if ((ret = CFW_SmsListMessages(&ListInfo, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        if (nFormat)
            atCmdRespInfoText(cmd->engine, "+CMGL: (\"REC UNREAD\",\"REC READ\","
                                           "\"STO UNSENT\",\"STO SENT\",\"ALL\")");
        else
            atCmdRespInfoText(cmd->engine, "+CMGL: (0-4)");
        atCmdRespOK(cmd->engine);
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
	else if(cmd->type == AT_CMD_EXE)
	{
		unsigned status = CFW_SMS_STORED_STATUS_UNREAD;
		if ( (CFW_SMS_STORED_STATUS_UNREAD == status) && (0 == sStorageInfo.unReadRecords))
            RETURN_OK(cmd->engine);
		
	    CFW_SMS_LIST ListInfo = {};
        ListInfo.nCount = sStorageInfo.totalSlot;
        ListInfo.nStartIndex = 1;
        ListInfo.nStorage = storage;
        ListInfo.nType = CFW_SMS_TYPE_PP;
        ListInfo.nStatus = status;
        ListInfo.nOption = CFW_SMS_LIST_OPTION__TIME_DESCEND_ORDER;
		
		cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGL_ListRspCB, cmd);
        if ((ret = CFW_SmsListMessages(&ListInfo, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
		RETURN_FOR_ASYNC();
	}
#endif
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

static void CSCA_SetParamRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;

    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);

    atCmdRespOK(cmd->engine);
}

// 3.3.1 Service Centre Address
void atCmdHandleCSCA(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    unsigned ret;

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

    if (cmd->type == AT_CMD_SET)
    {
        // +CSCA=<sca>[,<tosca>]

        bool paramok = true;
        const char *sca = atParamStr(cmd->params[0], &paramok);
        unsigned tosca = atParamDefUint(cmd->params[1], CFW_TELNUMBER_TYPE_UNKNOWN, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        CFW_DIALNUMBER_V2 number = {};
        if ((ret = _smsParamNumber(sca, tosca, &number)))
            RETURN_CMS_ERR(cmd->engine, ret);

        CFW_SMS_PARAMETER sms_param;
        if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        // set param value
        sms_param.nNumber[0] = number.nDialNumberSize + 1;
        sms_param.nNumber[1] = number.nType;
        memcpy(&sms_param.nNumber[2], number.pDialNumber, number.nDialNumberSize);
        sms_param.nSaveToSimFlag = 0x01; // set sca to sim card

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CSCA_SetParamRspCB, cmd);
        if ((ret = CFW_CfgSetSmsParam(&sms_param, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_READ)
    {
        CFW_SMS_PARAMETER sms_param;
        if (CFW_CfgGetSmsParam(&sms_param, 0, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        if (sms_param.nNumber[0] == 0)
        {
            atCmdRespInfoText(cmd->engine, "+CSCA: \"\"");
        }
        else
        {
            char number[TEL_NUMBER_MAX_LEN + 2] = {0};
            _smsNumberStr(&sms_param.nNumber[2], sms_param.nNumber[0] - 1,
                          sms_param.nNumber[1], number);

            char rsp[64];
            sprintf(rsp, "+CSCA: \"%s\",%u", number, sms_param.nNumber[1]);
            atCmdRespInfoText(cmd->engine, rsp);
        }
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

// 3.2.1 Select Message Service
void atCmdHandleCSMS(atCommand_t *cmd)
{
    char urc[50];
    if (AT_CMD_SET == cmd->type)
    {
        // +CSMS=<service>
        bool paramok = true;
        uint8_t service = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        CFW_SetSmsSeviceMode(service);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
		sprintf(urc, "%s: 1,1,1", cmd->desc->name);
#else
		sprintf(urc, "%s: %d,1,1,1", cmd->desc->name, CFW_GetSmsSeviceMode());
#endif
        atCmdRespInfoText(cmd->engine, urc);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type) // Read command
    {
        sprintf(urc, "%s: %d,1,1,1", cmd->desc->name, CFW_GetSmsSeviceMode());
        atCmdRespInfoText(cmd->engine, urc);
        atCmdRespOK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        sprintf(urc, "%s: (0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, urc);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

static void CMGD_DelRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
		if(0 != cfw_event->nType)
		{
			//É¾³ýÒÑÉ¾³ý¶ÌÐÅÖ±½Ó·µ»ØOK
			if(ERR_CMS_INVALID_MEMORY_INDEX == cfw_event->nParam1){
				atCmdRespOK(cmd->engine);
				return;
			}else{
				RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);
			}
		}
#else
    if (cfw_event->nType != 0)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);
#endif
    atCmdRespOK(cmd->engine);
}

// 3.5.4 Delete Message
void atCmdHandleCMGD(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

    /* check storage1 have no error and get totalslot */
    uint8_t nStorage = gAtSetting.sim[nSim].cpms_mem1;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
    if (nStorage == CFW_SMS_STORAGE_MT)
        nStorage = CFW_SMS_STORAGE_SM;
#else
    if (nStorage == CFW_SMS_STORAGE_MT)
        nStorage = CFW_SMS_STORAGE_ME;
#endif


    CFW_SMS_STORAGE_INFO sinfo = {};
    if (CFW_CfgGetSmsStorageInfo(&sinfo, nStorage, nSim) != 0)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

    if (cmd->type == AT_CMD_SET)
    {
        // +CMGD=<index>[,<delflag>]

        bool paramok = true;
        unsigned nindex = atParamUintInRange(cmd->params[0], 1, sinfo.totalSlot, &paramok);
        unsigned delflag = atParamDefUintInRange(cmd->params[1], 0, 0, 4, &paramok);
        if (!paramok || cmd->param_count > 2)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        /* Get the delete flag flag */
        uint8_t nStatus = 0;
        switch (delflag)
        {
        case 0:
            nStatus = 0;
            break;

        case 1:
            nStatus = CFW_SMS_STORED_STATUS_READ;
            if (0 == sinfo.readRecords)
                RETURN_OK(cmd->engine);
            break;

        case 2:
            nStatus = CFW_SMS_STORED_STATUS_READ | CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ |
                      CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE |
                      CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
            if ((0 == sinfo.readRecords) && (0 == sinfo.sentRecords))
                RETURN_OK(cmd->engine);
            break;

        case 3:
            nStatus = CFW_SMS_STORED_STATUS_READ | CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ |
                      CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE |
                      CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE | CFW_SMS_STORED_STATUS_UNSENT;
            if ((0 == sinfo.readRecords) && (0 == sinfo.sentRecords) && (0 == sinfo.unsentRecords))
                RETURN_OK(cmd->engine);
            break;

        case 4:
            nStatus = CFW_SMS_STORED_STATUS_STORED_ALL;
            if ((0 == sinfo.readRecords) && (0 == sinfo.unReadRecords) &&
                (0 == sinfo.sentRecords) && (0 == sinfo.unsentRecords))
                RETURN_OK(cmd->engine);
            break;

        default:
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        }

        // for AT+CMGD=0 (WHY?)
        if (0 == nindex)
            nStatus = CFW_SMS_STORED_STATUS_STORED_ALL;

        // If the nDeleteFlag is given, then we delete kinds of message,
        // and the nDelMsgIndex is omit and value is set to 0
        if (delflag != 0)
            nindex = 0;
        else
        {
            if (nStorage == CFW_SMS_STORAGE_ME)
                nindex = (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim + nindex;
        }

        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGD_DelRspCB, cmd);
        if ((ret = CFW_SmsDeleteMessage(nindex, nStatus, nStorage, 1, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        char rsp[64];
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		sprintf(rsp, "+CMGD: (1-%u),(0-4)", sinfo.totalSlot);
#else
		sprintf(rsp, "+CMGD: (1-%u),(0,1,2,3,4)", sinfo.totalSlot);
#endif
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

// 3.2.3 Message Format
void atCmdHandleCMGF(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char rsp[32];

    if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t format = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        if (CFW_CfgSetSmsFormat(format, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        gAtSetting.sim[nSim].cmgf = format;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CMGF:(0,1)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        sprintf(rsp, "+CMGF: %u", gAtSetting.sim[nSim].cmgf);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

// 3.5.6 More Messages to Send
void atCmdHandleCMMS(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CMMS=[<n>]
        bool paramok = true;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
        unsigned n = atParamDefUintInRange(cmd->params[0], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
#else
		unsigned n = atParamDefUintInRange(cmd->params[0], 0, 0, 2, &paramok);
        if (!paramok || cmd->param_count > 2)
			RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
		qpara_cmms = n;

		//set cmms to 0
		if(qpara_cmms == 1)
		{
			//If reset then recalculate time
			if(QcmmsTimer != NULL)
			{
				if(osiTimerIsRunning(QcmmsTimer) == true)
				{
					ql_rtos_timer_stop(QcmmsTimer);
				}
			}
		}
#endif


        CFW_SetSMSConcat(n != 0, nSim);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        atCmdRespInfoText(cmd->engine, "+CMMS: (0,1)");
#else
		atCmdRespInfoText(cmd->engine, "+CMMS: (0-2)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        bool concat = CFW_GetSMSConcat(nSim);
		sprintf(rsp, "+CMMS: %u", concat);
#else
		bool concat = CFW_GetSMSConcat(nSim);
		uint8_t value = concat; 
		if(concat == true)
		{
			value = qpara_cmms;
		}
		sprintf(rsp, "+CMMS: %u", value);
#endif

        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

static void CMGR_ReadRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
		if(0 != cfw_event->nType)
		{
			//¶ÁÈ¡ÒÑÉ¾³ý¶ÌÐÅÖ±½Ó·µ»ØOK
			if(ERR_CMS_INVALID_MEMORY_INDEX == cfw_event->nParam1){
				atCmdRespOK(cmd->engine);
				return;
			}else{
				RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);
			}
		}
#else

    /* check event type */
    if (0 != cfw_event->nType)
        RETURN_CMS_CFW_ERR(cmd->engine, cfw_event->nParam1);
#endif
    CFW_SMS_NODE_EX *pNode = (CFW_SMS_NODE_EX *)cfw_event->nParam1;
    atMemFreeLater(pNode);

    uint8_t nstatus = _smsCfwStatusToAt(pNode->node.nStatus & 0x0f);
    if (nstatus == 0xff || nstatus == AT_SMS_STATUS_ALL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_STATUS);

    if (nstatus == AT_SMS_STATUS_UNREAD)
    {
        if (g_SMS_CIND_NewMsg > 0)
            g_SMS_CIND_NewMsg--;
    }

    /* just for debug */
    OSI_LOGI(0x10004838, "cmgr read msg resp: pNode->nType = %u", pNode->node.nType);

    char *rsp = NULL;
    rsp = malloc(1024);
    if (rsp == NULL)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
    memset(rsp, 0x00, 1024);
    atMemFreeLater(rsp);

    if (pNode->node.nType == 1) // text mode,show param, state = unread/read msg
    {
        // if text mode (+CMGF=1), command successful and SMS-DELIVER:
        // +CMGR: <stat>,<oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>, <sca>,<tosca>,<length>]<CR><LF><data>
        CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *pType1Data =
            (CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO *)&pNode->info;

        OSI_LOGI(0, "SMS_NODE type 1: length/%u", pType1Data->length);

        char oaNumber[TEL_NUMBER_MAX_LEN + 1] = {0};
        _smsNumberStr(pType1Data->oa, pType1Data->oa_size, pType1Data->tooa, oaNumber);
        char *urcOaNumber = NULL;
        if (gAtSetting.cscs == cs_ucs2)
            urcOaNumber = generateUrcUcs2Data(oaNumber, strlen(oaNumber));

        char scaNumber[TEL_NUMBER_MAX_LEN + 1] = {0};
        _smsNumberStr(pType1Data->sca, pType1Data->sca_size, pType1Data->tosca, scaNumber);
        char *urcScaNumber = NULL;
        if (gAtSetting.cscs == cs_ucs2)
            urcScaNumber = generateUrcUcs2Data(scaNumber, strlen(scaNumber));

        uint8_t nDcs = _smsCfwDcsToOutDcs(pType1Data->dcs);
        if (gAtSetting.sim[nSim].csdh == 1) /* CSDH=1 */
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\",%u,%u,%u,%u,\"%s\",%u,%u",
                    _smsAtStatusStr(nstatus), urcOaNumber ? urcOaNumber : oaNumber, pType1Data->scts.uYear,
                    pType1Data->scts.uMonth, pType1Data->scts.uDay, pType1Data->scts.uHour,
                    pType1Data->scts.uMinute, pType1Data->scts.uSecond, pType1Data->scts.iZone,
                    pType1Data->tooa, pType1Data->fo, pType1Data->pid,
                    nDcs, urcScaNumber ? urcScaNumber : scaNumber, pType1Data->tosca, pType1Data->length);
        }
        else /* CSDH = 0 */
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\",,\"%u/%02u/%02u,%02u:%02u:%02u%+03d\"",
                    _smsAtStatusStr(nstatus), urcOaNumber ? urcOaNumber : oaNumber, pType1Data->scts.uYear,
                    pType1Data->scts.uMonth, pType1Data->scts.uDay, pType1Data->scts.uHour,
                    pType1Data->scts.uMinute, pType1Data->scts.uSecond, pType1Data->scts.iZone);
        }

        if (urcScaNumber)
            free(urcScaNumber);
        if (urcOaNumber)
            free(urcOaNumber);

        atCmdRespInfoText(cmd->engine, rsp);

        char *data = _smsNodeDataStr(pNode->node.nType, &pNode->info, nSim);
        if (data == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        atCmdRespInfoText(cmd->engine, data);
        atCmdRespOK(cmd->engine);
        free(data);
    }
    else if (pNode->node.nType == 3) // text mode, show param, state =sent/unsent msg
    {
        // if text mode (+CMGF=1), command successful and SMS-SUBMIT:
        // +CMGR: <stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>], <sca>,<tosca>,<length>]<CR><LF><data>
        CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *pType3Data =
            (CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO *)&pNode->info;

        OSI_LOGI(0, "SMS_NODE type 3: length/%u", pType3Data->length);

        char daNumber[TEL_NUMBER_MAX_LEN + 1] = {0};
        _smsNumberStr(pType3Data->da, pType3Data->da_size, pType3Data->toda, daNumber);
        char *urcdaNumber = NULL;
        if (gAtSetting.cscs == cs_ucs2)
            urcdaNumber = generateUrcUcs2Data(daNumber, strlen(daNumber));

        char scaNumber[TEL_NUMBER_MAX_LEN + 1] = {0};
        _smsNumberStr(pType3Data->sca, pType3Data->sca_size, pType3Data->tosca, scaNumber);
        char *urcscaNumber = NULL;
        if (gAtSetting.cscs == cs_ucs2)
            urcscaNumber = generateUrcUcs2Data(scaNumber, strlen(scaNumber));

        uint8_t nDcs = _smsCfwDcsToOutDcs(pType3Data->dcs);
        if (gAtSetting.sim[nSim].csdh == 1)
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\",,%u,%u,%u,%u,%u,\"%s\",%u,%u",
                    _smsAtStatusStr(nstatus), daNumber, pType3Data->toda,
                    pType3Data->fo, pType3Data->pid, nDcs,
                    pType3Data->vp, urcscaNumber ? urcscaNumber : scaNumber, pType3Data->tosca,
                    pType3Data->length);
        }
        else /* CSDH = 0 */
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\"", _smsAtStatusStr(nstatus), urcdaNumber ? urcdaNumber : daNumber);
        }

        if (urcdaNumber)
            free(urcdaNumber);
        if (urcscaNumber)
            free(urcscaNumber);

        atCmdRespInfoText(cmd->engine, rsp);

        char *data = _smsNodeDataStr(pNode->node.nType, &pNode->info, nSim);
        if (data == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        atCmdRespInfoText(cmd->engine, data);
        atCmdRespOK(cmd->engine);
        free(data);
    }
    else if (pNode->node.nType == 7) // PDU mode, pp msg
    {
        // +CMGR: <stat>,[<alpha>],<length><CR><LF><pdu>
        CFW_SMS_PDU_INFO *pType7Data = (CFW_SMS_PDU_INFO *)&pNode->info;

        /* check recv pdu and discard unused char */
        if (!(AT_SMS_CheckPDUIsValid((uint8_t *)pType7Data->pData, (uint8_t *)&(pType7Data->nDataSize), true)))
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

        /* output size exclude scasize */
        unsigned nDataSizeWithoutSCA = pType7Data->nDataSize - (pType7Data->pData[0] + 1);

        sprintf(rsp, "+CMGR: %u,,%u\r\n", nstatus, nDataSizeWithoutSCA);
        atCmdRespInfoText(cmd->engine, rsp);

        char *data = _smsNodeDataStr(pNode->node.nType, &pNode->info, nSim);
        if (data == NULL)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PDU_CHAR);

        atCmdRespInfoText(cmd->engine, data);
        atCmdRespOK(cmd->engine);
        free(data);
    }
    else if (pNode->node.nType == 8) // PDU mode, CB msg
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_ERR_IN_MS);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_ERR_IN_MS);
    }
}

// 3.4.3/4.2 Read Message
void atCmdHandleCMGR(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint32_t ret;

    if (!gAtCfwCtx.sim[nSim].sms_init_done)
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

    if (cmd->type == AT_CMD_SET)
    {
        unsigned nStorage = gAtSetting.sim[nSim].cpms_mem1;
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
        if (nStorage == CFW_SMS_STORAGE_MT)
            nStorage = CFW_SMS_STORAGE_SM;
#else
		if (nStorage == CFW_SMS_STORAGE_MT)
			nStorage = CFW_SMS_STORAGE_ME;

#endif


        CFW_SMS_STORAGE_INFO sinfo;
        if (CFW_CfgGetSmsStorageInfo(&sinfo, nStorage, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_ME_FAIL);

        // +CMGR=<index>
        bool paramok = true;
        unsigned nindex = atParamUintInRange(cmd->params[0], 1, sinfo.totalSlot, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        if (nStorage == CFW_SMS_STORAGE_ME)
            nindex = (PHONE_SMS_ENTRY_COUNT / CONFIG_NUMBER_OF_SIM) * nSim + nindex;
        cmd->uti = cfwRequestUTI((osiEventCallback_t)CMGR_ReadRspCB, cmd);
        if ((ret = CFW_SmsReadMessage(nStorage, 1, nindex, cmd->uti, nSim)) != 0)
        {
            cfwReleaseUTI(cmd->uti);
            RETURN_CMS_ERR(cmd->engine, atCfwToCmsError(ret));
        }
        RETURN_FOR_ASYNC();
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

// 3.3.3 Show Text Mode Parameters
void atCmdHandleCSDH(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CSDH=[<show>]
        bool paramok = true;
        unsigned show = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok || cmd->param_count > 1)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        gAtSetting.sim[nSim].csdh = show;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+CSDH: (0,1)");
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[32];
        sprintf(rsp, "+CSDH: %u", gAtSetting.sim[nSim].csdh);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

// 3.4.1 New Message Indications to TE
void atCmdHandleCNMI(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_SET)
    {
        // +CNMI=<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]
        bool paramok = true;
        unsigned mode = atParamUintInRange(cmd->params[0], 0, 3, &paramok);
        unsigned mt = atParamDefUintInRange(cmd->params[1], gAtSetting.sim[nSim].cnmi_mt, 0, 3, &paramok);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		uint32_t bm_types[] = {0, 2};
		unsigned bm = atParamDefUintInList(cmd->params[2], gAtSetting.sim[nSim].cnmi_bm, bm_types, OSI_ARRAY_SIZE(bm_types), &paramok);
#else
        unsigned bm = atParamDefUintInRange(cmd->params[2], gAtSetting.sim[nSim].cnmi_bm, 0, 3, &paramok);
#endif
        unsigned ds = atParamDefUintInRange(cmd->params[3], gAtSetting.sim[nSim].cnmi_bfr, 0, 1, &paramok);
        unsigned bfr = atParamDefUintInRange(cmd->params[4], 0, 0, 1, &paramok);
        if (!paramok || cmd->param_count > 5)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        uint8_t nOption = 0;
        uint8_t nNewSmsStorage = 0;
        if (CFW_CfgGetNewSmsOption(&nOption, &nNewSmsStorage, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_CMD_CANNOT_ACT);

        nOption = (nOption & 0x1f) | CFW_SMS_ROUT_DETAIL_INFO;
        if (CFW_CfgSetNewSmsOption(nOption, gAtSetting.sim[nSim].cpms_mem3, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_CMD_CANNOT_ACT);

        if (CFW_CfgSetNewSmsOptionMT(mt & 0x03, CFW_SMS_STORAGE_NS, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_CMD_CANNOT_ACT);

        CFW_SMS_PARAMETER sInfo;
        CFW_CfgGetDefaultSmsParam(&sInfo, nSim);
        sInfo.ssr = ds;
        CFW_CfgSetDefaultSmsParam(&sInfo, nSim);

        gAtSetting.sim[nSim].cnmi_mode = mode;
        gAtSetting.sim[nSim].cnmi_mt = mt;
        gAtSetting.sim[nSim].cnmi_bm = bm;
        gAtSetting.sim[nSim].cnmi_ds = ds;
        gAtSetting.sim[nSim].cnmi_bfr = bfr;
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        char rsp[64];
        sprintf(rsp, "+CNMI: %u,%u,%u,%u,%u",
                gAtSetting.sim[nSim].cnmi_mode, gAtSetting.sim[nSim].cnmi_mt,
                gAtSetting.sim[nSim].cnmi_bm, gAtSetting.sim[nSim].cnmi_ds,
                gAtSetting.sim[nSim].cnmi_bfr);
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		atCmdRespInfoText(cmd->engine, "+CNMI: (0-3),(0-3),(0,2),(0-1),(0-1)");
#else
        atCmdRespInfoText(cmd->engine, "+CNMI: (0-3),(0-3),(0-2),(0-1),(0,1)");
#endif
        atCmdRespOK(cmd->engine);
    }
    else
    {
        atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

/******************************************************************************************
Function            :   atCmdHandleCSMP
Description     :       CSMP procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       atCommand_t *cmd
Output          :
Return          :   void
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.07
*******************************************************************************************/
void atCmdHandleCSMP(atCommand_t *cmd)
{
    CFW_SMS_PARAMETER sInfo = {0};
    char PromptInfoBuff[50] = {0};
    uint32_t nOperationRet = 0;
    uint8_t nSim = atCmdGetSim(cmd->engine);
    bool paramok = true;
    static const uint32_t list[6] = {0, 4, 8, 16, 20, 24};
    static const uint32_t fo_list[2] = {17, 49};
    switch (cmd->type)
    {
    case AT_CMD_SET:
    {
        nOperationRet = CFW_CfgGetSmsParam(&sInfo, 0, nSim);
        uint8_t fo = atParamUintInList(cmd->params[0], fo_list, sizeof(fo_list) / sizeof(fo_list[0]), &paramok);
        uint8_t vp = atParamUintInRange(cmd->params[1], 0, 255, &paramok);
        uint8_t pid = atParamUintInRange(cmd->params[2], 0, 255, &paramok);
        uint8_t dcs = atParamUintInList(cmd->params[3], list, sizeof(list) / sizeof(list[0]), &paramok);
        if (cmd->param_count == 0 || cmd->param_count > 4 || !paramok || nOperationRet != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        sInfo.mti = fo;
        sInfo.vp = vp;
        sInfo.pid = pid;
        sInfo.dcs = dcs;
        sInfo.nSaveToSimFlag = 0;
        if (CFW_CfgSetSmsParam(&sInfo, 0 /*pParam->nDLCI*/, nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_MEM_FAIL);
        RETURN_OK(cmd->engine);
    }
    case AT_CMD_TEST:
    {
        atCmdRespOK(cmd->engine);
        break;
    }
    case AT_CMD_READ:
    {
        /*
           ** default value of 'fo' is 17, we can get the value
           ** of CFW_SMS_PARAMETER
         */
        nOperationRet = CFW_CfgGetSmsParam(&sInfo, 0, nSim);
        if (0 == nOperationRet)
        {
            sprintf(PromptInfoBuff, "+CSMP: %d,%d,%d,%d", sInfo.mti, sInfo.vp, sInfo.pid, sInfo.dcs);
            atCmdRespInfoText(cmd->engine, PromptInfoBuff);
            atCmdRespOK(cmd->engine);
        }
        else
        {
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
        }
        break;
    }

    default:
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
        break;
    }

    return;
}

// 3.4.4 New Message Acknowledgement to ME/TA
void atCmdHandleCNMA(atCommand_t *cmd)
{
#ifdef CONFIG_SOC_8910
    uint8_t nSim = atCmdGetSim(cmd->engine);

    switch (cmd->type)
    {
    case AT_CMD_EXE:
        if (!CFW_GetSmsSeviceMode() || !gAtCfwCtx.sim[nSim].is_waiting_cnma)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

        if (gAtCfwCtx.sim[nSim].newsms_timer != NULL)
        {
            osiTimerDelete(gAtCfwCtx.sim[nSim].newsms_timer);
            gAtCfwCtx.sim[nSim].newsms_timer = NULL;
            gAtCfwCtx.sim[nSim].is_waiting_cnma = false;
        }

        if (CFW_SmsMtSmsPPAckReq(nSim) != 0)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

        atCmdRespOK(cmd->engine);
        break;
    case AT_CMD_SET:
    {
        if (!CFW_GetSmsSeviceMode() || (gAtSetting.sim[nSim].cmgf == 1) || !gAtCfwCtx.sim[nSim].is_waiting_cnma)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);

        bool paramok = true;
        uint8_t n = atParamUintInRange(cmd->params[0], 0, 2, &paramok);
        if ((cmd->param_count != 1 && cmd->param_count != 2) || !paramok)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        if (gAtCfwCtx.sim[nSim].newsms_timer != NULL)
        {
            osiTimerDelete(gAtCfwCtx.sim[nSim].newsms_timer);
            gAtCfwCtx.sim[nSim].newsms_timer = NULL;
            gAtCfwCtx.sim[nSim].is_waiting_cnma = false;
        }

        if (n == 0 || n == 1)
        {
            if (CFW_SmsMtSmsPPAckReq(nSim) != 0)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
        }
        else
        {
            if (CFW_SendMtSmsAckPPError(0xd3, nSim) != 0)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
        }

        atCmdRespOK(cmd->engine);
        break;
    }
    case AT_CMD_TEST:
        if (gAtSetting.sim[nSim].cmgf == 0) // PDU mode
        {
            char rsp[50];
            sprintf(rsp, "%s: (0-2)", cmd->desc->name);
            atCmdRespInfoText(cmd->engine, rsp);
            atCmdRespOK(cmd->engine);
            break;
        }
        else
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    default:
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
#else
    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
#endif
}

// 3.3.5 Save Settings
void atCmdHandleCSAS(atCommand_t *cmd)
{
    atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
}

// 3.3.6 Restore Settings
void atCmdHandleCRES(atCommand_t *cmd)
{
    atCmdRespCmsError(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);
}

#if 0
//extern CB_CTX_T;
typedef struct
{
    uint16_t SimFileSize ;
    uint8_t   CbActiveStatus;
    uint8_t   CbType;
    uint8_t   MidNb;
    uint16_t   Mid[20];
    uint8_t   DcsNb;
    uint8_t   Dcs[20];
} CB_CTX_TYPE;
#endif
typedef struct
{
    uint16_t SimFileSize;
    uint8_t CbActiveStatus;
    uint8_t CbType;
    uint8_t MidNb;
    uint16_t Mid[20];
    uint8_t MirNb;
    uint16_t Mir[20];
    uint8_t DcsNb;
    uint8_t Dcs[17];
} CB_CTX_TYPE;
#define AT_CSCB_MODE_MASK 0
/******************************************************************************************
Function            :   atCmdHandleCSCB
Description     :
Test command        :   AT+CSCB?
Exe command         :       AT+CSCB
Called By           :   AT_RunCommand()
Data Accessed       :
Data Updated        :
Input           :       pParam
Output          :
Return          :   void
Others          :
*******************************************************************************************/
extern uint32_t CFW_CbSetContext(void *CtxData, CFW_SIM_ID nSimID);
extern uint32_t CFW_CbGetContext(void *CtxData, CFW_SIM_ID nSimID);
extern uint32_t CFW_CbWriteContext(CB_CTX_T CtxData, CFW_SIM_ID nSimID);
extern uint32_t CFW_CbReadContext(CB_CTX_T *CtxData, CFW_SIM_ID nSimID);
extern uint32_t CFW_CbAct(uint16_t nUTI, CFW_SIM_ID nSimID);
extern uint32_t CFW_CbDeAct(uint16_t nUTI, CFW_SIM_ID nSimID);
//#ifdef CB_SUPPORT
void atCmdHandleCSCB(atCommand_t *cmd)
{
    bool paramok = true;
    int i = 0, k = 0, j = 0;
    char str_ascii = '0';
    char CscbAckBuff[80] = {0};
    char *mids[40] = {
        0,
    };
    char *dcss[34] = {
        0,
    };
    uint8_t midnb = 0, dcsnb = 0;
    uint16_t val_mids[20] = {
        0,
    };
    uint8_t val_dcss[17] = {
        0,
    };
    uint8_t nSim = atCmdGetSim(cmd->engine);
    CB_CTX_T nCbCtxSet, nCbCtxRead;

    OSI_LOGI(0x1000480c, "+CSCB\n ");
    memset(&nCbCtxRead, 0x00, sizeof(CB_CTX_TYPE));
    memset(&nCbCtxSet, 0x00, sizeof(CB_CTX_TYPE));
    switch (cmd->type)
    {
    case AT_CMD_SET:
        if (cmd->param_count > 3)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        uint16_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

        if (cmd->param_count > 1)
        {
            const char *const_mids = atParamStr(cmd->params[1], &paramok);
            for (j = 0; j < strlen(const_mids); j++)
            {
                str_ascii = const_mids[j];
                if (('a' <= str_ascii && str_ascii <= 'z') || ('A' <= str_ascii && str_ascii <= 'Z'))
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
            if (!paramok)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            mids[0] = strtok((char *)const_mids, ",");
            while (mids[i] != NULL)
            {
                i++;
                mids[i] = strtok(NULL, ",");
            }
            mids[i] = "\0";
            midnb = i;

            for (j = 0; j < midnb; j++)
            {
                val_mids[j] = atoi(mids[j]);
                if ((val_mids[j] != 0) && (val_mids[j] != 1) && (val_mids[j] != 5) && ((val_mids[j] < 320) || (val_mids[j] > 478)) && (val_mids[j] != 922))
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
        }

        if (cmd->param_count > 2)
        {
            const char *const_dcss = atParamStr(cmd->params[2], &paramok);
            for (j = 0; j < strlen(const_dcss); j++)
            {
                str_ascii = const_dcss[j];
                if (('a' <= str_ascii && str_ascii <= 'z') || ('A' <= str_ascii && str_ascii <= 'Z'))
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
            if (!paramok)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);

            dcss[0] = strtok((char *)const_dcss, ",");
            while (dcss[k] != NULL)
            {
                k++;
                dcss[k] = strtok(NULL, ",");
            }
            dcss[k] = "\0";
            dcsnb = k;

            for (j = 0; j < dcsnb; j++)
            {
                val_dcss[j] = (uint8_t)atoi(dcss[j]);
                if ((val_dcss[j] > 3) && (val_dcss[j] != 5))
                    RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            }
        }

        memcpy(&nCbCtxSet.Mid, val_mids, sizeof(val_mids));
        memcpy(&nCbCtxSet.Dcs, val_dcss, sizeof(val_dcss));
        nCbCtxSet.MidNb = midnb;
        nCbCtxSet.DcsNb = dcsnb;
        uint16_t nUTI = cfwRequestNoWaitUTI();
        if (mode == 0)
        {
            if (CFW_CbAct(nUTI, nSim) != 0)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            nCbCtxSet.CbActiveStatus = 1;
        }
        else
        {
            if (CFW_CbDeAct(nUTI, nSim) != 0)
                RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_INVALID_PARA);
            nCbCtxSet.CbActiveStatus = 0;
        }
        CFW_CbWriteContext(nCbCtxSet, nSim);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_READ:
        CFW_CbReadContext(&nCbCtxRead, nSim);
        char rsp[400];
        char *prsp = rsp;
        if (nCbCtxRead.CbActiveStatus == 1)
            mode = 0;
        else
            mode = 1;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		prsp += sprintf(prsp, "+CSCB: %d,\"", mode);
#else
        prsp += sprintf(prsp, "+CSCB:%d,\"", mode);
#endif
        if (nCbCtxRead.MidNb > 10)
            nCbCtxRead.MidNb = 0;
        for (j = 0; j < nCbCtxRead.MidNb; j++)
        {
            if (j < nCbCtxRead.MidNb - 1)
                prsp += sprintf(prsp, "%d,", nCbCtxRead.Mid[j]);
            else
                prsp += sprintf(prsp, "%d", nCbCtxRead.Mid[j]);
        }
        prsp += sprintf(prsp, "\",\"");
        for (j = 0; j < nCbCtxRead.DcsNb; j++)
        {
            if (j < nCbCtxRead.DcsNb - 1)
                prsp += sprintf(prsp, "%d,", nCbCtxRead.Dcs[j]);
            else
                prsp += sprintf(prsp, "%d", nCbCtxRead.Dcs[j]);
        }
        prsp += sprintf(prsp, "\"");
        atCmdRespInfoText(cmd->engine, rsp);
        atCmdRespOK(cmd->engine);
        break;

    case AT_CMD_TEST:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
		sprintf(CscbAckBuff, "+CSCB: (0,1)");
#else
        sprintf(CscbAckBuff, "+CSCB:(0,1),(0,1,5,320-478,922),(0-3,5)");
#endif
        atCmdRespInfoText(cmd->engine, CscbAckBuff);
        atCmdRespOK(cmd->engine);
        break;
    default:
        RETURN_CMS_ERR(cmd->engine, ERR_AT_CMS_OPER_NOT_SUPP);

        break;
    }
}

typedef struct _CFW_EV
{
    uint32_t nEventId;
    uint32_t nTransId; // only low 8bit is valid for all CFW applications.
    uint32_t nParam1;
    uint32_t nParam2;
    uint8_t nType;
    uint8_t nFlag;
    uint8_t padding[2];
} CFW_EV;

uint32_t CFW_MeGetStorageInfo(CFW_SIM_ID nSimId, uint8_t nStatus, CFW_EV *pEvent);

void AT_SMS_MOInit_MEMessage()
{
#ifdef PORTING_ON_GOING
    uint8_t nIsFail = 0x0;
    CFW_SIM_ID nSimId;

    for (nSimId = CFW_SIM_0; nSimId < CFW_SIM_COUNT; nSimId++)
    {
        CFW_EV sMeEvent = {
            0x00,
        };
        sMeEvent.nParam1 = (uint32_t)((uint8_t *)AT_MALLOC(sizeof(CFW_SMS_STORAGE_INFO)));

        if ((uint8_t *)sMeEvent.nParam1 == NULL)
        {
            OSI_LOGI(0x10004828, "AT_SMS_MOInit_MEMessage  sMeEvent.nParam1  malloc error!!! \n ");
        }

        SUL_ZeroMemory8((uint8_t *)sMeEvent.nParam1, sizeof(CFW_SMS_STORAGE_INFO));

        CFW_SMS_STORAGE_INFO *pMEStorageInfo = (CFW_SMS_STORAGE_INFO *)(sMeEvent.nParam1);
        CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_STORED_ALL, (CFW_EV *)&sMeEvent);
        CFW_SMS_STORAGE_INFO sStorageInfo;
        SUL_ZeroMemory8(&sStorageInfo, sizeof(CFW_SMS_STORAGE_INFO));

        if (sMeEvent.nType == 0)
        {
            sStorageInfo.storageId = CFW_SMS_STORAGE_ME;
            sStorageInfo.totalSlot = pMEStorageInfo->totalSlot;
            sStorageInfo.usedSlot = pMEStorageInfo->usedSlot;

            //Get ME unReadRecords
            CFW_SMS_STORAGE_INFO *nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

            SUL_ZeroMemory8(&sMeEvent, sizeof(CFW_EV));
            SUL_ZeroMemory8((uint8_t *)nTmpEventP1, sizeof(CFW_SMS_STORAGE_INFO));

            sMeEvent.nParam1 = (uint32_t)nTmpEventP1;

            CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNREAD, &sMeEvent);
            if (sMeEvent.nType == 0)
            {
                sStorageInfo.unReadRecords = pMEStorageInfo->usedSlot;
                //Get ME readRecords
                nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;
                SUL_ZeroMemory8(&sMeEvent, sizeof(CFW_EV));
                SUL_ZeroMemory8((uint8_t *)nTmpEventP1, sizeof(CFW_SMS_STORAGE_INFO));

                sMeEvent.nParam1 = (uint32_t)nTmpEventP1;
                CFW_MeGetStorageInfo(CFW_SMS_STORAGE_ME, CFW_SMS_STORED_STATUS_READ, &sMeEvent);
                if (sMeEvent.nType == 0)
                {
                    uint8_t nSentStatus = 0x0;

                    sStorageInfo.readRecords = pMEStorageInfo->usedSlot;
                    //Get ME sentRecords
                    nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

                    SUL_ZeroMemory8(&sMeEvent, sizeof(CFW_EV));
                    SUL_ZeroMemory8((uint8_t *)nTmpEventP1, sizeof(CFW_SMS_STORAGE_INFO));

                    sMeEvent.nParam1 = (uint32_t)nTmpEventP1;

                    nSentStatus = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
                                  CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE | CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
                    CFW_MeGetStorageInfo(nSimId, nSentStatus, &sMeEvent);

                    if (sMeEvent.nType == 0)
                    {
                        sStorageInfo.sentRecords = pMEStorageInfo->usedSlot;

                        //Get ME unsentRecords
                        nTmpEventP1 = (CFW_SMS_STORAGE_INFO *)sMeEvent.nParam1;

                        SUL_ZeroMemory8(&sMeEvent, sizeof(CFW_EV));
                        SUL_ZeroMemory8((uint8_t *)nTmpEventP1, sizeof(CFW_SMS_STORAGE_INFO));

                        sMeEvent.nParam1 = (uint32_t)nTmpEventP1;
                        CFW_MeGetStorageInfo(nSimId, CFW_SMS_STORED_STATUS_UNSENT, &sMeEvent);
                        OSI_LOGI(0x0810108f, "MeGetSI 0\n");

                        if (sMeEvent.nType == 0)
                        {
                            sStorageInfo.unsentRecords = pMEStorageInfo->usedSlot;
                            CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_ME, nSimId);
                            OSI_LOGI(0x08101090, "MeGetSI 1 \n");

                            //Add by lixp at 20090213 for free crash issue
                            if (nSimId == (CFW_SIM_COUNT - 1))
                            {
                                CSW_SMS_FREE((uint8_t *)sMeEvent.nParam1);
                            }
                        } //unsentRecords
                        else
                        {
                            nIsFail = 1;
                        }
                    } //sentRecords
                    else
                    {
                        nIsFail = 2;
                    }
                } //readRecords
                else
                {
                    nIsFail = 3;
                }
            } //unReadRecords
            else
            {
                nIsFail = 4;
            }
        } //totalSlot and usedSlot
        else
        {
            nIsFail = 5;
        }

        if (nIsFail)
        {
            OSI_LOGI(0x10004829, "AT_SMS_MOInit_MEMessage failed  nIsFail %d\n", nIsFail);
            break;
        }
    }
#endif
}

void AT_SMS_MOInit_ReadMessage_rsp(osiEvent_t *pEvent)
{
#ifdef PORTING_ON_GOING
    static uint8_t nIndex[NUMBER_OF_SIM] = {1, 1};

    uint8_t nFormat = 0;
    uint8_t nstatus_tmp[12] = {0};
    uint8_t urc_str[100] = {0};
    CFW_EVENT CfwEvent = {0};
    CFW_SMS_NODE *pNode = NULL;
    PAT_CMD_RESULT pResult = NULL;
    uint16_t nUTI = 0;
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    uint8_t nSim = CfwEvent.nFlag;
    ;

    /* just for debug */
    OSI_LOGI(0x100052cf, "AT_SMS_MOInit_ReadMessage_rsp: cfw_event->nType = 0x%x, cfw_event->nParam1 = 0x%x, CfwEvent.nParam2 = %u,nSim %d",
             cfw_event->nType, cfw_event->nParam1, CfwEvent.nParam2, nSim);

    CFW_SMS_STORAGE_INFO sStorageInfo;
    SUL_ZeroMemory8(&sStorageInfo, sizeof(CFW_SMS_STORAGE_INFO));

    CFW_CfgGetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim);
    OSI_LOGI(0x1000482b, "sStorageInfo.totalSlot 0x%x nIndex[nSim] %d nSim %d", sStorageInfo.totalSlot, nIndex[nSim], nSim);
    //hal_HstSendEvent(0xA1300000 + (nIndex[nSim]<<8) + nSim);
    if (0x00 != cfw_event->nType)
    {
        if (0xF0 == cfw_event->nType)
        {
            if (++nIndex[nSim] <= sStorageInfo.totalSlot)
            {

                OSI_LOGI(0x1000482c, "Begin Read nIndex[nSim] %d nSim %d", nIndex[nSim], nSim);

                uint32_t nRet = CFW_SmsReadMessage(CFW_SMS_STORAGE_SM, CFW_SMS_TYPE_PP, nIndex[nSim], cfw_event->nUTI, nSim);

                if (0 != nRet)
                {
                    OSI_LOGI(0x10003ede, "CFW_SmsReadMessage Error 0x%x", nRet);
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: ReadMessage error", nUTI, nSim);
                    at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_ERROR);
                    return;
                }
            }
            else
            {
                //Init Over
                OSI_LOGI(0x1000482d, "SMS INIT Success usedSlot %d total %d nSim %d", sStorageInfo.usedSlot, sStorageInfo.totalSlot, nSim);

                CFW_EVENT cfw_event;
                //uint32_t ats_stat = 0;

                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_OK);

                /* get the UTI and free it after finished calling */
                if (0 == (nUTI = AT_AllocUserTransID()))
                {
                    //hal_HstSendEvent(0xa130001);

                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
                    return;
                }

                uint32_t ret = CFW_SmsInitComplete(nUTI, nSim);
                if (0 != ret)
                {
                    AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS Init Complete failed", cfw_event.nUTI, nSim);
                    return;
                }
                AT_SMS_INIT(nSim);

                SUL_ZeroMemory8(&cfw_event, sizeof(cfw_event));

                cfw_event.nType = CFW_INIT_STATUS_SMS;
                cfw_event.nParam1 = sStorageInfo.totalSlot;
                memcpy(&(cfwInitInfo.sms[nSim]), &cfw_event, sizeof(cfw_event));

                // Notify SA that the SMS (ATS) module is ready for use.
                at_ModuleSetSmsReady(true);

                sprintf(urc_str, "^CINIT: SMS %u, %u, %u", cfw_event.nType, cfw_event.nParam1, cfw_event.nParam2);
                pResult = AT_CreateRC(CSW_IND_NOTIFICATION,
                                      CMD_RC_CR,
                                      CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, urc_str, (uint16_t)AT_StrLen(urc_str), 0);
                OSI_LOGXI(OSI_LOGPAR_S, 0x1000482e, "send an SMS URC to ATM, URC string=%s", urc_str);
                AT_Notify2ATM(pResult, cfw_event.nUTI);
                OSI_LOGI(0x1000482f, "set gATCurrentuCmer_ind nSim[%d] to 2\n", nSim);

                gATCurrentuCmer_ind[nSim] = 2;
                gAtSetting.sim[nSim].cnmi_mt = 2;
                AT_SMS_MOInit_MEMessage();
            }
        }
        else
        {
            AT_SMS_Result_Err(ERR_AT_CMS_UNKNOWN_ERROR, NULL, 0, "MO Init read msg resp: exception happened", cfw_event->nUTI,
                              nSim);
        }

        return;
    }
    pNode = (CFW_SMS_NODE *)cfw_event->nParam1;
    nFormat = gAtSetting.sim[nSim].cmgf;
    nstatus_tmp[0] = pNode->nStatus & 0x0f;

    if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_READ)
    {
        sStorageInfo.readRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_UNREAD)
    {
        sStorageInfo.unReadRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE ||
             nstatus_tmp[0] == CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE)
    {
        sStorageInfo.sentRecords++;
    }
    else if (nstatus_tmp[0] == CFW_SMS_STORED_STATUS_UNSENT)
    {
        sStorageInfo.unsentRecords++;
    }
    else
    {
        OSI_LOGI(0x10004830, "Exception Status 0x%x", nstatus_tmp[0]);
        sStorageInfo.unknownRecords++;
        //return ;
    }

    sStorageInfo.usedSlot++;

    CFW_CfgSetSmsStorageInfo(&sStorageInfo, CFW_SMS_STORAGE_SM, nSim);

    if (++nIndex[nSim] <= sStorageInfo.totalSlot)
    {

        OSI_LOGI(0x10004831, "Current Index %d,Total %d nSim %d", nIndex[nSim], sStorageInfo.totalSlot, nSim);
        uint32_t nRet = CFW_SmsReadMessage(CFW_SMS_STORAGE_SM, CFW_SMS_TYPE_PP, nIndex[nSim], cfw_event->nUTI, nSim);

        if (0 != nRet)
        {
            OSI_LOGI(0x10003ede, "CFW_SmsReadMessage Error 0x%x", nRet);
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: ReadMessage error", nUTI, nSim);
            at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_ERROR);
            //hal_HstSendEvent(0xa130003);

            return;
        }
    }
    else
    {
        CFW_EVENT cfw_event;
        //Init Over
        OSI_LOGI(0x1000482d, "SMS INIT Success usedSlot %d total %d nSim %d", sStorageInfo.usedSlot, sStorageInfo.totalSlot, nSim);
        //uint32_t ats_stat = 0;

        at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_OK);

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
            return;
        }

        CFW_SmsInitComplete(nUTI, nSim);
        AT_SMS_INIT(nSim); // hameina[+]

        SUL_ZeroMemory8(&cfw_event, sizeof(cfw_event));
        cfw_event.nType = CFW_INIT_STATUS_SMS;
        cfw_event.nParam1 = 0;

        sprintf(urc_str, "^CINIT: SMS %u, %u, %u", cfw_event.nType, cfw_event.nParam1, cfw_event.nParam2);
        pResult = AT_CreateRC(CSW_IND_NOTIFICATION,
                              CMD_RC_CR,
                              CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, urc_str, (uint16_t)AT_StrLen(urc_str), 0);
        OSI_LOGXI(OSI_LOGPAR_S, 0x1000482e, "send an SMS URC to ATM, URC string=%s", urc_str);

        OSI_LOGI(0x10004832, "sStorageInfo.usedSlot 0x%x", sStorageInfo.usedSlot);

        AT_Notify2ATM(pResult, cfw_event.nUTI);
        OSI_LOGI(0x10004833, "Set gATCurrentuCmer_ind nSim[%d] to 2\n", nSim);
        gATCurrentuCmer_ind[nSim] = 2;
        gAtSetting.sim[nSim].cnmi_mt = 2;
        memcpy(&(cfwInitInfo.sms[nSim]), &cfw_event, sizeof(cfw_event));

        // Notify SA that the SMS (ATS) module is ready for use.
        at_ModuleSetSmsReady(true);
        AT_SMS_MOInit_MEMessage();
        //Init Over
    }

    return;
#endif
}

/******************************************************************************************
Function            :   AT_SMS_RecvCbMessage_Indictation
Description     :       AT receive indication of cb
Called By           :   At SMS module
Data Accessed       :
Data Updated        :
Input           :       osiEvent_t *pEvent
Output          :
Return          :   void
*******************************************************************************************/
typedef struct
{
    uint16_t MId; // Message Id the SMS-CB belongs to
    uint16_t DataLen;
    uint8_t DCS;     // Data Coding Scheme applicable to the SMS-CB message
    uint8_t Data[1]; // In fact, DataLen elts must be allocated for this
    // array
} CB_MSG_IND_STRUCT_EX;

typedef enum _AT_MSG_ALPHABET
{
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;

uint8_t gnRptCBMBuff[680] = {0};
uint8_t gATAsciiBuff[600] = {0};

/**
 * macro status flag to string or data
 */
static bool AT_SMS_StatusMacroFlagToStringOrData(uint8_t *pStatusMacroFlagBuff, uint8_t nFormat)
{
    if (nFormat) /* text mode */
    {
        if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_READ)
        {
            strcpy((char *)pStatusMacroFlagBuff, "REC READ");
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNREAD)
        {
            strcpy((char *)pStatusMacroFlagBuff, "REC UNREAD");
        }
        else if (pStatusMacroFlagBuff[0] == 8 || pStatusMacroFlagBuff[0] == 0)
        {
            strcpy((char *)pStatusMacroFlagBuff, "STO SENT");
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNSENT)
        {
            strcpy((char *)pStatusMacroFlagBuff, "STO UNSENT");
        }
        else
        {
            OSI_LOGI(0x10004852, "text mode: get status exception 1");
            return false;
        }
    }
    else /* pdu mode */
    {
        if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_READ)
        {
            pStatusMacroFlagBuff[0] = 1;
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNREAD)
        {
            pStatusMacroFlagBuff[0] = 0;
        }
        else if (pStatusMacroFlagBuff[0] == 8 || pStatusMacroFlagBuff[0] == 0)
        {
            pStatusMacroFlagBuff[0] = 3;
        }
        else if (pStatusMacroFlagBuff[0] == CFW_SMS_STORED_STATUS_UNSENT)
        {
            pStatusMacroFlagBuff[0] = 2;
        }
        else
        {
            OSI_LOGI(0x10004853, "pdu mode: get status exception 2");
            return false;
        }
    }
    return true;
}

/******************************************************************************************
Function            :   AT_SMS_StringOrDataToStatusMacroFlag
Description     :   string or data to macro flag
Called By           :   AT_SMS_RESULT_OK
Data Accessed   :
Data Updated    :
Input           :   uint8_t* pStatusFlag, uint8_t* pGetSaveStatus, uint8_t nFormat
Output          :
Return          :   bool
Others          :   build by wangqunyang 09/04/2008
*******************************************************************************************/
bool AT_SMS_StringOrDataToStatusMacroFlag(uint8_t *pStatusValue, uint8_t *pStatusFlag, uint8_t nFormat)
{
#ifdef PORTING_ON_GOING
    OSI_LOGI(0x10004854, " AT_SMS_StringOrDataToStatusMacroFlag nState %d", *pStatusValue);

    if (nFormat) /* text mode */
    {

        if (!strcmp("REC UNREAD", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNREAD;
        }
        else if (!strcmp("REC READ", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_READ;
        }
        else if (!strcmp("STO UNSENT", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNSENT;
        }
        else if (!strcmp("STO SENT", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ;
        }
        else if (!strcmp("ALL", pStatusValue))
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_STORED_ALL;
        }
        else
        {
            OSI_LOGI(0x10004855, "text mode: get status exception 3");
            return false;
        }
    }
    else /* pdu mode */
    {

        if (pStatusValue[0] == 0)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNREAD;
        }
        else if (pStatusValue[0] == 1)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_READ;
        }
        else if (pStatusValue[0] == 2)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_UNSENT;
        }
        else if (pStatusValue[0] == 3)
        {
            pStatusFlag[0] =
                CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ | CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV |
                CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE | CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE;
        }
        else if (pStatusValue[0] == 4)
        {
            pStatusFlag[0] = CFW_SMS_STORED_STATUS_STORED_ALL;
        }
        else
        {
            OSI_LOGI(0x10004856, "pdu mode: get status exception 4");
            return false;
        }
    }
#endif
    return true;
}

/******************************************************************************************
Function            :   AT_SMS_CheckPDUIsValid
Description     :       
Called By           :   by SMS module
Data Accessed   :
Data Updated        :
Input           :       uint8_t * pPDUData,
                    uint8_t * nPDULen,
                    bool bReadMsgFlag
Output          :
Return          :   bool
Others          :   build by wangqunyang 2008.05.13
*******************************************************************************************/
/**
 * check pdu is valid or not
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static bool AT_SMS_CheckPDUIsValid(uint8_t *pPDUData, uint8_t *nPDULen, bool bReadOrListMsgFlag)
#else
bool AT_SMS_CheckPDUIsValid(uint8_t *pPDUData, uint8_t *nPDULen, bool bReadOrListMsgFlag)
#endif
{
    uint8_t nIndex = 0;
    uint8_t nUserDataLen = 0;
    uint8_t nDCSFlag = 0;
    uint8_t nDABytes = 0;
    bool bSubmitFlag = false;
    uint8_t nDABCDLen = 0;
    uint8_t nVPLen = 0;

    /* Input data check */
    if (pPDUData == NULL || *nPDULen == 0)
    {
        OSI_LOGI(0x10004857, "check pdu: PDUData pointer is NULL or PDU len is 0");
        return false;
    }

    /* Get the index following SCA */
    nIndex = pPDUData[0] + 1;

    /********************************************************
    ** first , we get the MTI flag,  message or not
    ** MTI 00 : deliver mti 01 : submit
    ** other bits ,we omit and now implement
    *********************************************************/
    if (0x01 == (pPDUData[nIndex] & 0x03))
    {
        bSubmitFlag = true;
    }

    //===========================================/////
    // PDU is to write message or send message
    //===========================================/////
    if (bSubmitFlag)
    {

        /* FO flag parse Beginning........
           ** MTI 00 : deliver mti 01 : submit
         */

        /* FO is omit parse */

        /* RD is omit parse */

        /* VPF parse and set the VP lenght */
        if ((pPDUData[nIndex] & 0x18) == 0x00) /* 00 000 */
        {
            nVPLen = 0;
        }
        else if ((pPDUData[nIndex] & 0x18) == 0x08) /* 01 000  reserved */
        {
            nVPLen = 8;
        }
        else if ((pPDUData[nIndex] & 0x18) == 0x10) /* 10 000 */
        {
            nVPLen = 8;
        }
        else /* 11 000 */
        {
            nVPLen = 4;
        }

        /* SRR is omit parse */

        /* UDHI is omit parse */

        /* RP is omit parse */

        /*
           ** FO flag parsed completely and increase the index
         */
        //
        nIndex++;

        /* MR: message reference is omit parse */
        nIndex++;

        /* DA: How many Dial number's */
        if (pPDUData[nIndex] % 2)
        {
            nDABCDLen = pPDUData[nIndex] / 2 + 1; // odd number
        }
        else
        {
            nDABCDLen = pPDUData[nIndex] / 2; // even number
        }

        // All DA area is less than 12 bytes
        if ((1 + 1 + nDABCDLen) > 12)
        {
            OSI_LOGI(0x10004858, "MO check pdu: SCA field octs length is more than 12 ");
            return false;
        }
        nIndex = nIndex + 1 + 1 + nDABCDLen;

        /*
           ** PID byte index, set default 0x00 for PID
         */
        if (0x00 != pPDUData[nIndex])
        {
            //pPDUData[nIndex] = 0x00; //Modify by Lixp for GCF test
        }
        nIndex++;

        //===========================================
        // here: only parse the encode bits of DCS byte,
        // other bits omit and not implement
        //
        // bit7..bit4 - encode group
        // bit7 - reserved
        // bit6 - reserved
        // bit5 - 0:text uncompress 1: GSM default compress
        // bit4 - 0: bit0 and bit1 no use 1: bit0 and bit1 useful
        //
        // bit0: bit1:
        // 0     0 class1
        // 0     1 class2
        // 1     0 class3
        // 1     1 class4
        //
        // bit2: bit3:
        // 0 0 GSM default 7 bit
        // 0 1 8 bit dcs
        // 1 0 16bit ucs
        // 1 1 reserved
        //===========================================
        if (0x00 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 7;
        }
        else if (0x04 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 8;
        }
        else if (0x08 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 16;
        }
        else
        {
            OSI_LOGI(0x10004859, "MO check pdu: DCS field value is not supported by us ");
            return false;
        }

        nIndex++;

        /*
           ** parse VP is exist or not
         */
        if (0 == nVPLen) /* No VP */
        {
        }
        else if (8 == nVPLen) /* one byte */
        {
            nIndex++;
        }
        else /* half byte */
        {
            nIndex++;

            /* read the spec and make sure which half byte, high or low?
               continue ...... to do
               if(0x00!= pPDUData[nIndex] & 0xF0)
               {
               return false;
               } */
        }

        /* UDL parse */
        if (7 == nDCSFlag)
        {
            if (pPDUData[nIndex] > 160)
            {
                OSI_LOGI(0x1000485a, "MO check pdu: 7bit encode and user data len more 160 ");
                return false;
            }

            if ((pPDUData[nIndex] * 7) % 8)
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8 + 1;
            }
            else
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8;
            }
        }
        else
        {
            if (pPDUData[nIndex] > 140)
            {
                OSI_LOGI(0x1000485b, "MO check pdu: 8bit or 16bit encode and user data len more than 140 ");
                return false;
            }

            nUserDataLen = pPDUData[nIndex];
        }

        if (bReadOrListMsgFlag)
        {
            /* out parameters of  nOutputPDULen */
            *nPDULen = nUserDataLen + nIndex + 1;
            pPDUData[*nPDULen] = '\0';
        }
        else
        {

            /* check the input length is valid */
            if ((pPDUData[0] + 1 > *nPDULen) || ((*nPDULen > AT_SMS_BODY_BUFF_LEN)))
            {

                OSI_LOGI(0x1000485c, "MO check pdu: PDU len overflow");
                return false;
            }

            /* check the total length */
            if (nUserDataLen + nIndex + 1 != *nPDULen)
            {
                OSI_LOGI(0x1000485d, "MO check pdu: length is not match");
                return false;
            }
        }

        return true;
    }

    //===========================================
    // parse PDU is recv from others
    //===========================================/////
    else
    {
        nIndex++;

        /* DA parser */
        if (pPDUData[nIndex] % 2)
        {
            nDABytes = (pPDUData[nIndex] + 1) / 2;
        }
        else
        {
            nDABytes = pPDUData[nIndex] / 2;
        }

        nIndex = nIndex + 1 + 1 + nDABytes; /* length, toda DA */

        /* PID omit parser */
        nIndex++;

        //===========================================
        // here: only parse the encode bits of  DCS byte,
        // other bits omit and not implement now,
        // the bit meaning as above description
        //===========================================
        if (0x00 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 7;
        }
        else if (0x04 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 8;
        }
        else if (0x08 == (pPDUData[nIndex] & 0x0C))
        {
            nDCSFlag = 16;
        }
        else
        {
            OSI_LOGI(0x1000485e, "MT check pdu: DCS field value is not supported by us ");
            return false;
        }
        nIndex++;

        /* 7 bytes of time stamp */
        nIndex = nIndex + 7;

        /* UDL parse */
        if (7 == nDCSFlag)
        {
            if (pPDUData[nIndex] > 160)
            {
                OSI_LOGI(0x1000485f, "MT check pdu: 7bit encode and user data len more 160 ");
                return false;
            }

            if ((pPDUData[nIndex] * 7) % 8)
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8 + 1;
            }
            else
            {
                nUserDataLen = (pPDUData[nIndex] * 7) / 8;
            }
        }
        else
        {
            if (pPDUData[nIndex] > 140)
            {
                OSI_LOGI(0x10004860, "MT check pdu: 8bit or 16bit encode and user data len more 140 ");
                return false;
            }

            nUserDataLen = pPDUData[nIndex];
        }

        if (bReadOrListMsgFlag)
        {
            /* check the total length */
            if (nUserDataLen + nIndex + 1 > AT_SMS_BODY_BUFF_LEN)
            {
                OSI_LOGI(0x10004861, "MT check pdu: totol pdu len is overflow ");
                return false;
            }

            /* out parameters of  nOutputPDULen */
            *nPDULen = nUserDataLen + nIndex + 1;
            pPDUData[*nPDULen] = '\0';
        }
        else
        {
            OSI_LOGI(0x10004862, "nUserDataLen = %u, nIndex = %u, *nPDULen = %u", nUserDataLen, nIndex, *nPDULen);

            /* check the input length is valid */
            if ((pPDUData[0] + 1 > *nPDULen) || ((*nPDULen > AT_SMS_BODY_BUFF_LEN)))
            {
                OSI_LOGI(0x10004861, "MT check pdu: totol pdu len is overflow ");
                return false;
            }

            /* check the total length */
            if (nUserDataLen + nIndex + 1 != *nPDULen)
            {
                OSI_LOGI(0x10004863, "MT check pdu: PDU length is not match ");
                return false;
            }
        }

        return true;
    }
    return true;
}

/******************************************************************************************
Function            :   AT_SMS_IsValidPhoneNumber
Description     :       check number copy from pbk module is valid or not
Called By           :   by SMS module
Data Accessed       :
Data Updated        :
Input           :       uint8_t * arrNumber, uint8_t nNumberSize, bool * bIsInternational
Output          :
Return          :   bool
Others          :       //[+]2007.11.29 for check number copy from pbk module
                    //arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    //legal phone number char: 0-9,*,#,+ (+ can only be the first position)
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static bool AT_SMS_IsValidPhoneNumber(const char *s, size_t size, bool *bIsInternational)
#else
bool AT_SMS_IsValidPhoneNumber(const char *s, size_t size, bool *bIsInternational)
#endif
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

/******************************************************************************************
Function            :   SMS_PDUBCDToASCII
Description     :       transfer PDU BCD Code to ASCII Code
Called By           :   by SMS module
Data Accessed   :
Data Updated    :
Input           :       uint8_t * pBCD,
                    uint16_t *pLen,
                    uint8_t * pStr
Output          :
Return          :   uint16_t
Others          :       build by unknown and unknown date
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
static bool SMS_PDUBCDToASCII(uint8_t *pBCD, uint16_t *pLen, uint8_t *pStr)
{
    uint8_t Tmp = 0;
    uint32_t i;

    if (pStr == NULL || pBCD == NULL)
    {
        OSI_LOGI(0x10004866, "BCD to ASCII: pointer is NULL");
        return false;
    }

    for (i = 0; i < *pLen; i++)
    {
        // ////////////////////////////
        // high 4 bits
        // ////////////////////////////
        Tmp = pBCD[i] >> 4;
        if (Tmp < 10) // 0~9
        {
            pStr[i * 2] = Tmp + '0';
        }
        else if (Tmp > 9 && Tmp < 16) // 10~15
        {
            pStr[i * 2] = Tmp + 55;
        }
        else
        {
            OSI_LOGI(0x10004867, "BCD to ASCII: high bits some char is more than 'F' of HEX");
            return false;
        }

        // ////////////////////////////
        // low 4 bits
        // ////////////////////////////
        Tmp = pBCD[i] & 0x0f;
        if (Tmp < 10) // 0~9
        {
            pStr[i * 2 + 1] = Tmp + '0';
        }
        else if (Tmp > 9 && Tmp < 16) // 10~15
        {
            pStr[i * 2 + 1] = Tmp + 55;
        }
        else
        {
            OSI_LOGI(0x10004868, "BCD to ASCII: low bits some char is more than 'F' of HEX");
            return false;
        }
    }

    *pLen = i * 2;

    return true;
}

/******************************************************************************************
Function            :   AT_CIND_SMS
Description     :   CIND Command inner procedure function
Called By           :   AT_GC_CmdFunc_CIND
Data Accessed       :
Data Updated        :
Input           :       uint8_t* pNewMsg,uint8_t* pMemFull
Output          :
Return          :   bool
Others          :       //[+]2007.10.30 for +cind interface func
                    modify and add comment by wangqunyang 03/04/2008
*******************************************************************************************/
bool AT_CIND_SMS(uint8_t *pNewMsg, uint8_t *pMemFull, uint8_t nSim)
{
    *pNewMsg = g_SMS_CIND_NewMsg ? 1 : 0;

    /* if ME storage and SM storage are both full, SMSFULL is 1, orthers is 0 */
    *pMemFull = ((CFW_SMS_STORAGE_SM == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_SM)) && (CFW_SMS_STORAGE_ME == (g_SMS_CIND_MemFull[nSim] & CFW_SMS_STORAGE_ME))) ? 1 : 0;
    if (*pNewMsg > 1 || *pMemFull > 1)
    {
        OSI_LOGI(0x10004869, "AT_CIND_SMS: new msg or msg full indicator more than 1");
        return false;
    }

    return true;
}

/**
 * add sca to pdu
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_SMS
static bool SMS_AddSCA2PDU(uint8_t *InPDU, uint8_t *OutPDU, uint16_t *pSize, uint8_t nSim)
#else
bool SMS_AddSCA2PDU(uint8_t *InPDU, uint8_t *OutPDU, uint16_t *pSize, uint8_t nSim)
#endif
{
    CFW_SMS_PARAMETER sInfo = {0};

    if (InPDU == NULL || OutPDU == NULL || *pSize == 0)
    {
        OSI_LOGI(0x1000486c, "AddSCA2PDU: pointer is NULL or size is 0");
        return false;
    }

    // SCA not present
    if (InPDU[0] == 0)
    {

        if (0 != CFW_CfgGetSmsParam(&sInfo, 0, nSim))
        {
            OSI_LOGI(0x1000486d, "AddSCA2PDU: get SMS param error");
            return false;
        }

        memcpy(OutPDU, sInfo.nNumber, sInfo.nNumber[0] + 1);

        memcpy(&OutPDU[OutPDU[0] + 1], &InPDU[1], (*pSize) - 1);
        *pSize = OutPDU[0] + *pSize;
    }
    else
    {
        memcpy(OutPDU, InPDU, *pSize);
    }
    return true;
}

/******************************************************************************************
Function            :   SMS_PDUAsciiToBCD
Description     :       this function used to check the input PDU has the SCA or not,
                                if not, add the SCA to the begin of the OUTPDU string.
Called By           :   atCmdHandleCMGS
                                atCmdHandleCMGW
Data Accessed       :
Data Updated        :
Input           :       uint8_t * pNumber, uint16_t nNumberLen, uint8_t * pBCD
Output          :
Return          :   uint16_t
Others          :       modify and add comment by wangqunyang 06/04/2008
*******************************************************************************************/
uint16_t SMS_PDUAsciiToBCD(uint8_t *pNumber, uint16_t nNumberLen, uint8_t *pBCD)
{
#ifdef PORTING_ON_GOING
    uint8_t *pTmp = pBCD;
    uint8_t Tmp = 0;
    uint32_t i = 0;
    uint16_t nBcdSize = 0;

    if (pNumber == NULL || pBCD == NULL)
    {
        OSI_LOGI(0x1000486e, "AsciiToBCD: ponter is NULL");
        return false;
    }

    memset(pBCD, 0, nNumberLen >> 1);

    for (i = 0; i < nNumberLen; i++)
    {
        if ((*pNumber >= 65) && (*pNumber <= 70))
        {
            Tmp = (INT8)(*pNumber - 55);
        }
        else if ((*pNumber >= 97) && (*pNumber <= 102))
        {
            Tmp = (INT8)(*pNumber - 87);
        }
        else if ((*pNumber >= '0') && (*pNumber <= '9'))
        {
            Tmp = (INT8)(*pNumber - '0');
        }
        else
        {
            OSI_LOGI(0x1000486f, "AsciiToBCD: some ascii char is more than 'F' of HEX");
            return false;
        }

        if (i % 2)
        {
            *pTmp++ |= (INT8)(Tmp & 0x0F);
        }
        else
        {
            *pTmp = (Tmp << 4) & 0xF0;
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
#endif
    return 0;
}

bool AT_SMS_GetPDUNUM(const uint8_t *pPDUData, uint8_t *pPDUNUM, uint8_t *pType, uint8_t *nPDULen)
{
    uint8_t nIndex = 0;

    /* Input data check */
    if (pPDUData == NULL)
    {
        OSI_LOGI(0x10004870, "GetPDUNUM is NULL or PDU len is 0");
        return false;
    }

    /* Get the index following SCA */
    nIndex = pPDUData[0] + 1;
    /* SRR is omit parse */
    /* UDHI is omit parse */
    /* RP is omit parse */
    /*
    ** FO flag parsed completely and increase the index
    */
    //
    nIndex++;

    /* MR: message reference is omit parse */
    nIndex++;

    /* DA: How many Dial number's */
    if (pPDUData[nIndex] % 2)
    {
        *nPDULen = pPDUData[nIndex] / 2 + 1; // odd number
    }
    else
    {
        *nPDULen = pPDUData[nIndex] / 2; // even number
    }
    // All DA area is less than 12 bytes
    if ((1 + 1 + *nPDULen) > 12)
    {
        OSI_LOGI(0x10004871, "GetPDUNUM: SCA field octs length is more than 12 ");
        return false;
    }
    *pType = pPDUData[nIndex + 1];

    memcpy(pPDUNUM, &(pPDUData[nIndex + 2]), *nPDULen);

    OSI_LOGI(0x10004872, "GetPDUNUM is end\n");
    //  AT_TC_MEMBLOCK(g_sw_AT_SMS, pPDUNUM, *nPDULen, 16);
    return true;
}

bool AT_SMS_GetSCANUM(uint8_t *pPDUData, uint8_t *pSACNUM, uint8_t *nPDULen)
{
#ifdef PORTING_ON_GOING
    uint8_t nIndex = 0;

    /* Input data check */
    if (pPDUData == NULL || *pSACNUM == 0)
    {
        OSI_LOGI(0x10004870, "GetPDUNUM is NULL or PDU len is 0");
        return false;
    }

    /* Get the index following SCA */
    nIndex = 1;

    /* DA: How many Dial number's */
    *nPDULen = pPDUData[0] - 1;
    // All DA area is less than 12 bytes
    if ((1 + 1 + *nPDULen) > 12)
    {
        OSI_LOGI(0x10004871, "GetPDUNUM: SCA field octs length is more than 12 ");
        return false;
    }
    memcpy(pSACNUM, &(pPDUData[nIndex + 1]), *nPDULen);
    OSI_LOGI(0x10004873, "GetSCANUM is end\n");
    //  AT_TC_MEMBLOCK(g_sw_AT_SMS, pSACNUM, *nPDULen, 16);
#endif
    return true;
}
