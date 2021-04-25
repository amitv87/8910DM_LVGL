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

#include "cfw_chset.h"
#include "osi_api.h"
#include "cfw.h"
#include <string.h>
#include <time.h>

#define FORM_HEX(hi, lo) ((((hi)&0xf) << 4) | ((lo)&0xf))
#define FORM_DECIMAL(hi, lo) ((hi)*10 | (lo))

#define DM_TONE_DTMF_0 0
#define DM_TONE_DTMF_1 1
#define DM_TONE_DTMF_2 2
#define DM_TONE_DTMF_3 3
#define DM_TONE_DTMF_4 4
#define DM_TONE_DTMF_5 5
#define DM_TONE_DTMF_6 6
#define DM_TONE_DTMF_7 7
#define DM_TONE_DTMF_8 8
#define DM_TONE_DTMF_9 9
#define DM_TONE_DTMF_A 10
#define DM_TONE_DTMF_B 11
#define DM_TONE_DTMF_C 12
#define DM_TONE_DTMF_D 13
#define DM_TONE_DTMF_S 14
#define DM_TONE_DTMF_P 15
#define DM_TONE_DIAL 16
#define DM_TONE_SUBSCRIBER_BUSY 17
#define DM_TONE_CONGESTION 18
#define DM_TONE_RADIO_PATHACKNOWLEDGEMENT 19
#define DM_TONE_CALL_DROPPED 20
#define DM_TONE_SPECIAL_INFORMATION 21
#define DM_TONE_CALL_WAITING 22
#define DM_TONE_RINGING 23

static int _asciiToBcd(char c)
{
    if (c == '*')
        return 0x0a;
    if (c == '#')
        return 0x0b;
    if (c == 'p' || c == 'P')
        return 0x0c;
    if (c == 'w' || c == 'W')
        return 0x0d;
    if (c == '+')
        return 0x0e;
    if (c >= '0' && c <= '9')
        return c - '0';
    return -1;
}

static int _bcdToAscii(uint8_t v)
{
    if (v == 0x0f)
        return -1;

    if (v == 0x0a)
        return '*';
    if (v == 0x0b)
        return '#';
    if (v == 0x0c)
        return 'p';
    if (v == 0x0d)
        return 'w';
    if (v == 0x0e)
        return '+';
    return v + '0';
}

static int _asciiToHex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static int _hexToAscii(uint8_t v)
{
    if (v >= 0 && v <= 9)
        return v + '0';
    if (v >= 0xa && v <= 0xf)
        return v + 'A' - 10;
    return -1;
}

int cfwDialStringToBcd(const void *src, size_t size, void *dst)
{
    if (size == 0)
        return 0;
    if (src == NULL || dst == NULL)
        return -1;

    const uint8_t *psrc = (const uint8_t *)src;
    uint8_t *pdst = (uint8_t *)dst;
    for (size_t n = 0; n < size; n++)
    {
        int v = _asciiToBcd(*psrc++);
        if (v < 0)
            return -1;

        if ((n % 2) == 0)
        {
            *pdst = 0xf0 | v;
        }
        else
        {
            *pdst = (*pdst & 0xf) | (v << 4);
            pdst++;
        }
    }

    return (size + 1) / 2;
}

int cfwBcdToDialString(const void *src, size_t size, void *dst)
{
    if (size == 0)
        return 0;

    if (src == NULL || dst == NULL)
        return -1;

    const uint8_t *psrc = (const uint8_t *)src;
    uint8_t *pdst = dst;
    uint8_t *pdst_old = pdst;
    for (size_t n = 0; n < size; n++)
    {
        uint8_t lo = *psrc & 0xf;
        uint8_t hi = (*psrc++ >> 4) & 0xf;

        int slo = _bcdToAscii(lo);
        int shi = _bcdToAscii(hi);
        if (slo < 0)
            return -1;
        *pdst++ = slo;
        if (shi < 0)
            break;
        *pdst++ = shi;
    }

    *pdst = '\0';
    return pdst - pdst_old;
}

int cfwHexStrToBytes(const void *src, size_t size, void *dst)
{
    if (size == 0)
        return 0;
    if (src == NULL || dst == NULL)
        return -1;

    const uint8_t *psrc = (const uint8_t *)src;
    uint8_t *pdst = (uint8_t *)dst;
    for (size_t n = 0; n < size; n++)
    {
        int v = _asciiToHex(*psrc++);
        if (v < 0)
            return -1;

        if ((n % 2) == 0)
        {
            *pdst = v << 4;
        }
        else
        {
            *pdst |= v;
            pdst++;
        }
    }

    return (size + 1) / 2;
}

int cfwBytesToHexStr(const void *src, size_t size, void *dst)
{
    if (size == 0)
        return 0;

    if (src == NULL || dst == NULL)
        return -1;

    const uint8_t *psrc = (const uint8_t *)src;
    uint8_t *pdst = dst;
    for (size_t n = 0; n < size; n++)
    {
        uint8_t lo = *psrc & 0xf;
        uint8_t hi = (*psrc++ >> 4) & 0xf;
        *pdst++ = _hexToAscii(hi);
        *pdst++ = _hexToAscii(lo);
    }

    *pdst = '\0';
    return size * 2;
}

int cfwDtmfCharToTone(char c)
{
    if (c == '#')
        return DM_TONE_DTMF_S;
    if (c == '*')
        return DM_TONE_DTMF_P;
    if (c >= 'A' && c <= 'D')
        return c - ('A' - DM_TONE_DTMF_A);
    if (c >= 'a' && c <= 'd')
        return c - ('a' - DM_TONE_DTMF_A);
    if (c >= '0' && c <= '9')
        return c - ('0' - DM_TONE_DTMF_0);
    return -1;
}

int cfwToneToDtmfChar(char c)
{
    if (c >= DM_TONE_DTMF_0 && c <= DM_TONE_DTMF_9)
        return c + ('0' - DM_TONE_DTMF_0);
    if (c >= DM_TONE_DTMF_A && c <= DM_TONE_DTMF_D)
        return c + ('A' - DM_TONE_DTMF_A);
    if (c == DM_TONE_DTMF_S)
        return '#';
    if (c == DM_TONE_DTMF_P)
        return '*';
    return -1;
}

int cfwDecode7Bit(const void *src, void *dst, size_t size)
{
    const uint8_t *pSrc = (const uint8_t *)src;
    uint8_t *pDest = (uint8_t *)dst;
    uint16_t nSrc = 0;
    uint16_t nDst = 0;
    uint16_t nByte = 0;
    uint16_t nLeft = 0;

    while (nSrc < size)
    {
        *pDest = (((*pSrc << nByte) | nLeft) & 0x7f);
        nLeft = (*pSrc >> (7 - nByte));

        pDest++;
        nDst++;
        nByte++;

        if (nByte == 7)
        {
            *pDest = nLeft;
            pDest++;
            nDst++;
            nByte = 0;
            nLeft = 0;
        }
        pSrc++;
        nSrc++;
    }
    return nDst;
}

/******************************************************************************/
/* Function name:    SUL_Encode7Bit                                           */
/* Description:                                                               */
/*                  The function encode 8bit gsm coding to 7-bit with the     */
/*                  description in following drawing.                         */
/* Parameters:                                                                */
/*                  pSrc        The given SMS string.                         */
/*                  pDest       Output encode SMS string.                     */
/*                  nSrcSize    the given SMS sting size in bytes.            */
/* Return Values:                                                             */
/*                  If the function succeeds, the return value is the length  */
/*                  of Encoded string, and the pDest string will be filled    */
/*                  with encoded string.If the function fails,the return value*/
/*                  is -1. To get extended error information, call            */
/*                  SUL_GetLastError.                                         */
/******************************************************************************/
int cfwEncode7Bit(const void *src, void *dst, size_t size)
{
    const uint8_t *pSrc = (const uint8_t *)src;
    uint8_t *pDest = (uint8_t *)dst;
    uint16_t nSrc = 0;  //
    uint16_t nDst = 0;  //
    uint16_t nChar = 0; //Sequece number of current octet processed, from 0 to 7;
    uint16_t nLeft = 0; //Remained data of last octet

    if (size == 0)
        return 0;
    while (nSrc < size)
    {
        // Get value mol 7
        nChar = (uint16_t)(nSrc & 7);

        if (nChar == 0)
        {
            //only save the first octet of source string
            nLeft = *pSrc;
            *pDest = (uint8_t)nLeft;
        }
        else
        {
            //Get one encoded octet by following means
            *pDest = (uint8_t)((*pSrc << (8 - nChar)) | nLeft);

            //saved the  left part of current octet as remained data
            nLeft = (uint16_t)(*pSrc >> nChar);
            //change conter and point of destination string
            nDst++;
            pDest++;
            if (nChar != 7)
                *pDest = (uint8_t)nLeft; //Added 2004.9.13
        }
        pSrc++;
        nSrc++;
    }

    if (nChar != 7)
        nDst++;

    //Return the length of destination string
    if (!((size + 1) % 8))
        *pDest |= 0x1a;
    return nDst;
}

static uint8_t _tobcd(uint8_t val)
{
    return FORM_HEX(val / 10, val % 10);
}

static uint8_t _frombcd(uint8_t val)
{
    return FORM_DECIMAL(val >> 4, val & 0xf);
}

static uint8_t _tosbcd(int8_t val)
{
    if (val > 0)
        return FORM_HEX(val % 10, val / 10);

    val = -val;
    return FORM_HEX(val % 10, val / 10) | 0x08;
}

static int8_t _fromsbcd(uint8_t val)
{
    int timezone = FORM_DECIMAL(val & 0x7, val >> 4);
    return (val & 0x08) ? -timezone : timezone;
}

void cfwPackTime(void *pack, uint32_t seconds, int time_zone)
{
    time_t time = seconds;
    struct tm tm = {};
    gmtime_r(&time, &tm);

    uint8_t *p = (uint8_t *)pack;
    p[0] = _tobcd(tm.tm_year + 1900 - 2000); // Year-1900 -> Year-2000
    p[1] = _tobcd(tm.tm_mon + 1);            // (0-11) -> (1-12)
    p[2] = _tobcd(tm.tm_mday);               // (1-31)
    p[3] = _tobcd(tm.tm_hour);               // (0-23)
    p[4] = _tobcd(tm.tm_min);                // (0-59)
    p[5] = _tobcd(tm.tm_sec);                // (0-59)
    p[6] = _tosbcd(time_zone / (CFW_TIME_ZONE_SECOND));
}

void cfwUnpackTime(const void *pack, uint32_t *seconds, int *time_zone)
{
    const uint8_t *p = (const uint8_t *)pack;
    struct tm tm = {};

    tm.tm_year = _frombcd(p[0]) + 2000 - 1900; // Year-2000 -> Year-1900
    tm.tm_mon = _frombcd(p[1]) - 1;            // (1-12) -> (0-11)
    tm.tm_mday = _frombcd(p[2]);               // (1-31)
    tm.tm_hour = _frombcd(p[3]);               // (0-23)
    tm.tm_min = _frombcd(p[4]);                // (0-59)
    tm.tm_sec = _frombcd(p[5]);                // (0-59)

    *seconds = mktime(&tm);
    *time_zone = _fromsbcd(p[6]) * (CFW_TIME_ZONE_SECOND);
}
