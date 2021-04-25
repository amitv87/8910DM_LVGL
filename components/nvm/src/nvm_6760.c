/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'V', 'M', ' ')
#define UNUSED_PARAMETER(x) (void)x

#define SN_MAX_LENGTH (48)

//#include "8910/nvm_6760.h"
#include "nvm.h"
#include "osi_log.h"
#include "osi_compiler.h"
#include <stdlib.h>
#include <string.h>

extern bool pal_GetFactoryImei(uint8_t simIndex, uint8_t *imei);
extern bool pal_SaveFactoryImei(uint8_t simIndex, uint8_t *imei);
extern int32_t pal_GetMobilePhoneSn(uint8_t nvmSn[64], int32_t size);
extern bool pal_SaveMobilePhoneSn(uint8_t nvmSn[64], int32_t size);
extern int32_t pal_GeMobileBoardSn(uint8_t nvmSn[64], int32_t size);
extern bool pal_SaveMobileBoardSn(uint8_t nvmSn[64], int32_t size);

static const uint8_t g_palDefaultImeiSv_2sim[2][8] = {
    {0x33, 0x95, 0x57, 0x09, 0x20, 0x15, 0x94, 0x13},
    {0x33, 0x95, 0x37, 0x08, 0x90, 0x55, 0x43, 0x70},
};

int nvmReadImei(int index, nvmImei_t *imei)
{
    if (imei == NULL)
        return -1;
    uint8_t rawImei[8];
    //uint16_t imei_len = 8;
    uint8_t *rdImei = (uint8_t *)imei;

    OSI_LOGD(0, "read index %d", index);

    if (!pal_GetFactoryImei(index, rawImei))
    {
        memcpy(rawImei, (uint8_t *)g_palDefaultImeiSv_2sim[index], 8);
    }
    *(rdImei++) = (rawImei[0] >> 4 & 0xF) + (uint8_t)'0';

    for (int i = 1; i < 8; i++)
    {
        *(rdImei++) = (rawImei[i] & 0xF) + (uint8_t)'0';
        *(rdImei++) = (rawImei[i] >> 4 & 0xF) + (uint8_t)'0';
    }
    //OSI_LOGXI(OSI_LOGPAR_M, 0, "readImei: %*s ", sizeof(nvmImei_t), (uint8_t *)imei);

    return sizeof(nvmImei_t);
}

int nvmReadImei2(int index, nvmImei2_t *imei)
{
    OSI_LOGD(0, "nvmReadImei2");
    if (imei == NULL)
        return -1;
    uint8_t rawImei[8];
    uint8_t *rdImei = (uint8_t *)imei;
    if (!pal_GetFactoryImei(index, rawImei))
    {
        memcpy(rawImei, (uint8_t *)g_palDefaultImeiSv_2sim[index], 8);
    }

    rdImei[0] = ((rawImei[0] & 0xf0) | 0xa);
    for (int i = 1; i < 8; i++)
    {
        rdImei[i] = (((rawImei[i] << 4) & 0xf0) | ((rawImei[i] >> 4) & 0xf));
    }
    //   memcpy(rdImei,rawImei,sizeof(rawImei));
    return sizeof(nvmImei2_t);
}

int nvmWriteImei(int index, const nvmImei_t *imei)
{
    if (imei == NULL)
        return -1;

    uint8_t rawImei[8];
    //uint16_t imei_len = 8;
    uint8_t *wrImei = (uint8_t *)imei;

    for (int j = 0; j < sizeof(nvmImei_t); j++)
    {
        wrImei[j] -= (uint8_t)'0';
        if (wrImei[j] < 0 || wrImei[j] > 9)
        {
            OSI_LOGE(0, "write Imei num illegal %d", wrImei[j]);
            return -1;
        }
    }
    OSI_LOGD(0, "write index %d", index);
    OSI_LOGXI(OSI_LOGPAR_M, 0, "writeImei: %*s ", sizeof(nvmImei_t), (uint8_t *)imei);

    rawImei[0] = ((*wrImei) << 4) | 0xa;

    for (int i = 1; i < 8; i++)
    {
        rawImei[i] = *(++wrImei);
        rawImei[i] |= (*(++wrImei)) << 4;
    }
    if (!pal_SaveFactoryImei(index, rawImei))
        return -1;

    return sizeof(nvmImei_t);
}

int nvmWriteImei2(int index, const nvmImei2_t *imei)
{
    if (imei == NULL)
        return -1;
    uint8_t rawImei[8];
    uint8_t *wrImei = (uint8_t *)imei;
    rawImei[0] = ((*wrImei) & 0xf0) | 0xa;
    wrImei++;
    for (int i = 1; i < 8; i++)
    {
        rawImei[i] = (((*wrImei << 4) & 0xf0) | ((*wrImei >> 4) & 0xf));
        wrImei++;
    }
    if (!pal_SaveFactoryImei(index, rawImei))
    {
        OSI_LOGD(0, "pal_SaveFactoryImei error");
        return -1;
    }
    //return sizeof(nvmImei2_t);

    return -1;
}

#if 0
int nvmReadPhasecheck(phaseCheckHead_t *chk)
{
    const char *pro_file = MEM_PRODUCT_FILE_NAME;
    vfs_sfile_init(pro_file);
    int len = vfs_sfile_read(pro_file, chk, sizeof(phaseCheckHead_t));
    if (len != (ssize_t)sizeof(phaseCheckHead_t))
    {
        OSI_LOGE(0, "PHSCHK: Read product fail.(Not enough data)");
        return -1;
    }

    return len;
}

int nvmWritePhaseCheck(const phaseCheckHead_t *chk)
{
    const char *pro_file = MEM_PRODUCT_FILE_NAME;
    vfs_sfile_init(pro_file);
    int len = vfs_sfile_write(pro_file, chk, sizeof(phaseCheckHead_t));
    if (len != sizeof(phaseCheckHead_t))
    {
        OSI_LOGE(0, "PHSCHK: Write product information fail");
        return -1;
    }

    return len;
}
#endif

// function:     read mobile phone sn
// parameter:  index, other product need it, so keep it.
// return:        failed: -1 failed,   success: other value, the length of the sn
OSI_WEAK int nvmReadSN(int index, void *nvmSn, int size)
{
    int32_t result = -1;
    uint8_t Sn[64];

    UNUSED_PARAMETER(index);

    if ((nvmSn == NULL) || (size <= 0))
        return -1;

    if (size > SN_MAX_LENGTH)
        size = SN_MAX_LENGTH;

    // nvmWriteSN(0,"66880099", 8);  // test code

    result = pal_GetMobilePhoneSn(Sn, size);

    if (result <= 0)
    {
        result = -1;
    }
    else
    {
        memset(nvmSn, 0, size);
        memcpy(nvmSn, Sn, result);
    }

    return result;
}

// function:     save mobile phone sn
// parameter:  index, other product need it, so keep it.
// return:        failed: -1 failed,   success: other value, the length of the sn
OSI_WEAK int nvmWriteSN(int index, const void *nvmSn, int size)
{
    UNUSED_PARAMETER(index);
    uint8_t Sn[64];

    if (nvmSn == NULL)
        return -1;

    if (size <= 0 || size > SN_MAX_LENGTH)
        return -1;

    memset(Sn, 0, 64);
    memcpy(Sn, nvmSn, size);

    if (!pal_SaveMobilePhoneSn(Sn, size))
    {
        return -1;
    }

    return size;
}
