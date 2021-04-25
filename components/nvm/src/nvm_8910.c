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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'V', 'M', ' ')

#include "nvm.h"
#include "vfs.h"
#include "osi_log.h"
#include "nvm_config.h"
#include "hal_shmem_region.h"
#include "calclib/crc32.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define NV_FULL_NAME_MAX (40)

#define FACTORYNV_DIR CONFIG_FS_FACTORY_MOUNT_POINT
#define MODEMNV_DIR CONFIG_FS_MODEM_MOUNT_POINT "/nvm"
#define RUNNINGNV_DIR CONFIG_FS_MODEM_NVM_DIR

#define PHASECHECK_FNAME FACTORYNV_DIR "/phasecheck.bin"
#define NVDIRECT_FNAME FACTORYNV_DIR "/imei.bin"

enum
{
    NVID_MODEM_STATIC_NV = 0x259,
    NVID_MODEM_PHY_NV = 0x25a,
    NVID_MODEM_DYNAMIC_NV = 0x25b,
    NVID_MODEM_STATIC_CARD2_NV = 0x25c,
    NVID_MODEM_PHY_CARD2_NV = 0x25d,
    NVID_MODEM_DYNAMIC_CARD2_NV = 0x25e,

    NVID_MODEM_RF_NV = 0x26c,
    NVID_RF_CALIB_GSM = 0x26d,
    NVID_RF_CALIB_LTE = 0x26e,
    NVID_RF_GOLDEN_BOARD = 0x26f,

    NVID_CFW_NV = 0x277,
    NVID_AUD_CALIB = 0x278,
    NVID_PHY_NV_CATM = 0x279,
    NVID_AUD_CODEC_NV = 0x27a,
#if defined(__QUEC_OEM_VER_TY__)
    NVID_Customer_NV = 0x27b,
#endif
    NVID_BT_CONFIG = 0x191,
    NVID_BT_ADDRESS = 0x192,
    NVID_BT_SPRD = 0x1ba,
#if defined(__QUEC_OEM_VER_TY__)
    NVID_LIC_NV = 0x280,
    NVID_TEST_MODE_FLAG = 0x281,
    NVID_USB_MODE_FLAG = 0x282
#endif
};

typedef struct
{
    uint16_t crc; // big endian
    uint16_t reserved;
} nvBinHeader_t;

typedef struct
{
    uint16_t nvid;
    uint16_t size;
} nvBinItemHeader_t;

typedef struct
{
    uint16_t nvid;             ///< 16bits NV ID
    const char *fname;         ///< file name, not including directory name, NULL for "nv%04x.bin"
    const char *dname;         ///< directory name
    const char *running_dname; ///< directory name for runningnv, NULL for not writable
} nvDescription_t;

static const nvDescription_t gNvDesc[] = {
    {NVID_IMEI1, NULL, FACTORYNV_DIR, NULL},
    {NVID_IMEI2, NULL, FACTORYNV_DIR, NULL},
    {NVID_IMEI3, NULL, FACTORYNV_DIR, NULL},
    {NVID_IMEI4, NULL, FACTORYNV_DIR, NULL},
    {NVID_CALIB_PARAM, "calibparam.bin", FACTORYNV_DIR, NULL},
    {NVID_RF_CALIB_GSM, "gsm_rf_calib.bin", FACTORYNV_DIR, NULL},
    {NVID_RF_CALIB_LTE, "lte_rf_calib.bin", FACTORYNV_DIR, NULL},
    {NVID_RF_GOLDEN_BOARD, "golden_board.bin", FACTORYNV_DIR, NULL},
    {NVID_BT_CONFIG, "bt_config.bin", FACTORYNV_DIR, NULL},
#ifdef QUEC_PATCH_FACTORY_UPDATE
	{NVID_MODEM_RF_NV, "rf_nv.bin", FACTORYNV_DIR, NULL},
#endif
#ifdef __QUEC_OEM_VER_TY__
	{NVID_Customer_NV, "customer.bin", FACTORYNV_DIR, NULL},
	{NVID_LIC_NV, "lic.bin", FACTORYNV_DIR, NULL},
	{NVID_TEST_MODE_FLAG, "test_mode_flag.bin", FACTORYNV_DIR, NULL},
	{NVID_USB_MODE_FLAG, "usb_mode_flag.bin", FACTORYNV_DIR, NULL},
#endif

    {NVID_MODEM_STATIC_NV, "static_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_MODEM_PHY_NV, "phy_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_MODEM_DYNAMIC_NV, "dynamic_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_MODEM_STATIC_CARD2_NV, "static_nv_2.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_MODEM_PHY_CARD2_NV, "phy_nv_2.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_MODEM_DYNAMIC_CARD2_NV, "dynamic_nv_2.bin", MODEMNV_DIR, RUNNINGNV_DIR},
#ifndef QUEC_PATCH_FACTORY_UPDATE
    {NVID_MODEM_RF_NV, "rf_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
#endif
    {NVID_CFW_NV, "cfw_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_AUD_CALIB, "audio_calib.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_BT_ADDRESS, "db_record.bin", RUNNINGNV_DIR, RUNNINGNV_DIR}, //no init data
    {NVID_BT_SPRD, "bt_sprd.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_AUD_CODEC_NV, "audio_codec_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
#if defined(CONFIG_NON_BL_CE_SUPPORT)
    {NVID_PHY_NV_CATM, "phy_nv_catm.bin", MODEMNV_DIR, RUNNINGNV_DIR},
#endif
    {NVID_SIM_LOCK_CUSTOMIZE_DATA, "sim_lock_customize_data.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_SIM_LOCK_USER_DATA, "sim_lock_user_data.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_SIM_LOCK_CONTROL_KEY_ID, "sim_lock_control_key.bin", MODEMNV_DIR, RUNNINGNV_DIR},

    {NVID_IMS_CSM_NV, "ims_csm_setting.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_IMS_SAPP_NV, "ims_sapp_setting.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_IMS_ISIM_NV, "ims_isim_setting.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_SIM_DELTA_NV, "sim_delta_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_SIM_DELTA_CARD2_NV, "sim_delta_nv_card2.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_PLMN_DELTA_NV, "plmn_delta_nv.bin", MODEMNV_DIR, RUNNINGNV_DIR},
    {NVID_PLMN_DELTA_CARD2_NV, "plmn_delta_nv_card2.bin", MODEMNV_DIR, RUNNINGNV_DIR},
};

void nvmInit(void)
{
    // Make directory if not exists. So, the return value doesn't matter.
    vfs_mkpath(MODEMNV_DIR, 0);
    vfs_mkpath(RUNNINGNV_DIR, 0);
}

int nvmGetIdFromFileName(const char *fname)
{
    if (fname == NULL)
        return -1;

    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gNvDesc); n++)
    {
        if (gNvDesc[n].fname != NULL && strcmp(gNvDesc[n].fname, fname) == 0)
            return gNvDesc[n].nvid;
    }

    return -1;
}

static const nvDescription_t *prvGetDescById(uint16_t nvid)
{
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gNvDesc); n++)
    {
        if (gNvDesc[n].nvid == nvid)
            return &gNvDesc[n];
    }

    return NULL;
}

static const char *prvReadFileName(char *fname, uint16_t nvid, bool force_fixed)
{
    const nvDescription_t *desc = prvGetDescById(nvid);
    if (desc == NULL)
        return NULL;

    if (!force_fixed && desc->running_dname != NULL)
    {
        strcpy(fname, desc->running_dname);
        strcat(fname, "/");
        strcat(fname, desc->fname);
        if (vfs_sfile_size(fname) >= 0)
            return fname;
    }

    strcpy(fname, desc->dname);
    strcat(fname, "/");
    strcat(fname, desc->fname);
    return (vfs_sfile_size(fname) >= 0) ? fname : NULL;
}

static const char *prvWriteFileName(char *fname, uint16_t nvid, bool force_fixed)
{
    const nvDescription_t *desc = prvGetDescById(nvid);
    if (desc == NULL || desc->fname == NULL)
        return NULL;

    if (!force_fixed && desc->running_dname != NULL)
        strcpy(fname, desc->running_dname);
    else
        strcpy(fname, desc->dname);

    strcat(fname, "/");
    strcat(fname, desc->fname);
    return fname;
}

static int prvReadNvDirect(unsigned offset, unsigned nv_size, void *buf, unsigned size)
{
    if (buf == NULL || size == 0)
        return nv_size;

    if (size < nv_size)
        return -1;

    refNVDirect_t nv;
    if (vfs_sfile_read(NVDIRECT_FNAME, &nv, sizeof(nv)) < 0)
        return -1;

    memcpy(buf, (char *)&nv + offset, nv_size);
    return nv_size;
}

static int prvWriteNvDirect(unsigned offset, unsigned nv_size, const void *buf, unsigned size)
{
    if (buf == NULL || size != nv_size)
        return -1;

    refNVDirect_t nv;
    memset(&nv, 0xff, sizeof(nv));

    int read = vfs_sfile_read(NVDIRECT_FNAME, &nv, sizeof(nv));
    if (read >= sizeof(nv) && memcmp((char *)&nv + offset, buf, nv_size) == 0)
        return 0;

    memcpy((char *)&nv + offset, buf, nv_size);
    int written = vfs_sfile_write(NVDIRECT_FNAME, &nv, sizeof(nv));
    return (written < 0) ? -1 : nv_size;
}

static int prvReadItem(uint16_t nvid, void *buf, unsigned size, bool force_fixed)
{
    // special case
    switch (nvid)
    {
    case NVID_IMEI1:
        return prvReadNvDirect(OSI_OFFSETOF(refNVDirect_t, imei1), 8, buf, size);

    case NVID_IMEI2:
        return prvReadNvDirect(OSI_OFFSETOF(refNVDirect_t, imei2), 8, buf, size);

    case NVID_IMEI3:
        return prvReadNvDirect(OSI_OFFSETOF(refNVDirect_t, imei3), 8, buf, size);

    case NVID_IMEI4:
        return prvReadNvDirect(OSI_OFFSETOF(refNVDirect_t, imei4), 8, buf, size);

    default:
        break;
    }

    char fname_buf[NV_FULL_NAME_MAX];
    const char *fname = prvReadFileName(fname_buf, nvid, force_fixed);
    if (fname == NULL)
        return -1;

    if (buf == NULL || size == 0)
        return vfs_sfile_size(fname);

    return vfs_sfile_read(fname, buf, size);
}

int nvmReadItem(uint16_t nvid, void *buf, unsigned size)
{
    return prvReadItem(nvid, buf, size, false);
}

int nvmReadFixedItem(uint16_t nvid, void *buf, unsigned size)
{
    return prvReadItem(nvid, buf, size, true);
}

static int prvWriteItem(uint16_t nvid, const void *buf, unsigned size, bool force_fixed)
{
    if (buf == NULL || size == 0)
        return -1;

    // special case
    switch (nvid)
    {
    case NVID_IMEI1:
        return prvWriteNvDirect(OSI_OFFSETOF(refNVDirect_t, imei1), 8, buf, size);

    case NVID_IMEI2:
        return prvWriteNvDirect(OSI_OFFSETOF(refNVDirect_t, imei2), 8, buf, size);

    case NVID_IMEI3:
        return prvWriteNvDirect(OSI_OFFSETOF(refNVDirect_t, imei3), 8, buf, size);

    case NVID_IMEI4:
        return prvWriteNvDirect(OSI_OFFSETOF(refNVDirect_t, imei4), 8, buf, size);

    default:
        break;
    }

    char fname_buf[NV_FULL_NAME_MAX];
    const char *fname = prvWriteFileName(fname_buf, nvid, force_fixed);
    if (fname == NULL)
        return -1;

    // check whether exist content matches
    if (vfs_sfile_size(fname) == size)
    {
        void *existed = malloc(size);
        if (existed == NULL)
            return -1;

        bool matches = (vfs_sfile_read(fname, existed, size) == size) &&
                       (memcmp(existed, buf, size) == 0);
        free(existed);

        if (matches)
            return 0;
    }

    OSI_LOGD(0, "nvm write nvid %d, size %d", nvid, size);
    return vfs_sfile_write(fname, buf, size);
}

int nvmWriteItem(uint16_t nvid, const void *buf, unsigned size)
{
    return prvWriteItem(nvid, buf, size, false);
}

int nvmWriteFixedItem(uint16_t nvid, const void *buf, unsigned size)
{
    return prvWriteItem(nvid, buf, size, true);
}

void nvmClearRunning(void)
{
    vfs_rmchildren(RUNNINGNV_DIR);
}

void nvmClearFactory(void)
{
    vfs_rmchildren(FACTORYNV_DIR);
}

bool nvmReadPhasecheck(phaseCheckHead_t *chk)
{
    const char *fname = PHASECHECK_FNAME;
    return vfs_sfile_read(fname, chk, sizeof(phaseCheckHead_t)) == sizeof(phaseCheckHead_t);
}

bool nvmWritePhaseCheck(const phaseCheckHead_t *chk)
{
    const char *fname = PHASECHECK_FNAME;
    return vfs_sfile_write(fname, chk, sizeof(phaseCheckHead_t)) == sizeof(phaseCheckHead_t);
}

void nvmClearPhaseCheck(void)
{
    const char *fname = PHASECHECK_FNAME;
    vfs_unlink(fname);
}

int nvmReadFixedBin(void *buf, unsigned size)
{
    unsigned nv_binsize = 4;
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gNvDesc); n++)
    {
        int item_size = nvmReadFixedItem(gNvDesc[n].nvid, NULL, 0);
        if (item_size > 0)
            nv_binsize += 4 + OSI_ALIGN_UP(item_size, 4);
    }

    // 0xff x8
    nv_binsize += 8;

    if (buf == NULL || size == 0)
        return nv_binsize;

    if (size < nv_binsize)
        return -1;

    // fill all of the buffer
    memset(buf, 0xff, size);

    // Set each nv item data
    char *ptr = (char *)buf + sizeof(nvBinHeader_t);
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gNvDesc); n++)
    {
        int item_size = nvmReadFixedItem(gNvDesc[n].nvid, ptr + 4, size);
        if (item_size > 0)
        {
            nvBinItemHeader_t *header = (nvBinItemHeader_t *)ptr;
            header->nvid = gNvDesc[n].nvid;
            header->size = item_size;
            ptr += sizeof(nvBinItemHeader_t) + OSI_ALIGN_UP(item_size, 4);
            OSI_LOGD(0, "Read fixed nv bin id/0x%x size/%d", header->nvid, item_size);
        }
    }

    // calculate crc of all of the data
    nvBinHeader_t *bin_header = (nvBinHeader_t *)buf;
    bin_header->crc = OSI_TO_BE16(crc16NvCalc(&bin_header->reserved, size - sizeof(uint16_t)));
    return size;
}

int nvmWriteFixedBin(const void *buf, unsigned size)
{
    if (buf == NULL || size < sizeof(nvBinHeader_t))
        return -1;

    const nvBinHeader_t *bin_header = (const nvBinHeader_t *)buf;
    if (OSI_FROM_BE16(bin_header->crc) != crc16NvCalc(&bin_header->reserved, size - sizeof(uint16_t)))
        return -1;

    const char *ptr = (const char *)buf + sizeof(nvBinHeader_t);
    const char *ptr_end = ptr + (size - sizeof(nvBinHeader_t));
    while (ptr < ptr_end)
    {
        const nvBinItemHeader_t *header = (const nvBinItemHeader_t *)ptr;
        if (header->nvid == 0xffff)
            break;

        int written = nvmWriteFixedItem(header->nvid, &header[1], header->size);
        OSI_LOGD(0, "Write fixed nv bin id/0x%x size/%d written/%d", header->nvid, header->size, written);

        if (written < 0)
            return -1;

        ptr += sizeof(nvBinItemHeader_t) + OSI_ALIGN_UP(header->size, 4);
    }
    return size;
}


#ifdef QUEC_PATCH_FACTORY_UPDATE
int nvmWriteFixedBin_v2(const void *buf, unsigned size, bool parsecheck)
{
    int written;
    if (buf == NULL || size < sizeof(nvBinHeader_t))
        return -1;

    const nvBinHeader_t *bin_header = (const nvBinHeader_t *)buf;
    if (OSI_FROM_BE16(bin_header->crc) != crc16NvCalc(&bin_header->reserved, size - sizeof(uint16_t)))
        return -1;

    const char *ptr = (const char *)buf + sizeof(nvBinHeader_t);
    const char *ptr_end = ptr + (size - sizeof(nvBinHeader_t));
    while (ptr < ptr_end)
    {
        const nvBinItemHeader_t *header = (const nvBinItemHeader_t *)ptr;
        if (header->nvid == 0xffff)
            break;
        const nvDescription_t *desc = prvGetDescById(header->nvid);
        if(!parsecheck && (strcmp(CONFIG_FS_FACTORY_MOUNT_POINT,desc->dname) == 0))
        {
            written = header->size;
        }
        else
        {
            written = nvmWriteFixedItem(header->nvid, &header[1], header->size);
        }
        OSI_LOGD(0, "Write fixed nv bin id/0x%x size/%d written/%d", header->nvid, header->size, written);

        if (written < 0)
            return -1;

        ptr += sizeof(nvBinItemHeader_t) + OSI_ALIGN_UP(header->size, 4);
    }
    return size;
}
#endif

static int _snLength(const uint8_t *sn)
{
    for (int n = 0; n < MAX_SN_LEN; n++)
    {
        if (sn[n] == 0xff || sn[n] == 0)
            return n;
    }
    return MAX_SN_LEN;
}

int nvmReadImei(int index, nvmImei_t *imei)
{
    uint16_t nvid;
    switch (index)
    {
    case 0:
        nvid = NVID_IMEI1;
        break;
    case 1:
        nvid = NVID_IMEI2;
        break;
    case 2:
        nvid = NVID_IMEI3;
        break;
    case 3:
        nvid = NVID_IMEI4;
        break;
    default:
        return -1;
    }

    if (imei == NULL)
        return -1;

    uint8_t rawImei[8];
    uint16_t imei_len = 8;
    uint8_t *rdImei = (uint8_t *)imei;

    OSI_LOGD(0, "read index %d", index);

    if (nvmReadItem(nvid, rawImei, imei_len) < 0)
        return -1;

    *(rdImei++) = (rawImei[0] >> 4 & 0xF) + (uint8_t)'0';

    for (int i = 1; i < 8; i++)
    {
        *(rdImei++) = (rawImei[i] & 0xF) + (uint8_t)'0';
        *(rdImei++) = (rawImei[i] >> 4 & 0xF) + (uint8_t)'0';
    }
    return sizeof(nvmImei_t);
}

int nvmWriteImei(int index, const nvmImei_t *imei)
{
    uint16_t nvid;
    switch (index)
    {
    case 0:
        nvid = NVID_IMEI1;
        break;
    case 1:
        nvid = NVID_IMEI2;
        break;
    case 2:
        nvid = NVID_IMEI3;
        break;
    case 3:
        nvid = NVID_IMEI4;
        break;
    default:
        return -1;
    }

    if (imei == NULL)
        return -1;

    uint8_t rawImei[8];
    uint16_t imei_len = 8;
    uint8_t *wrImei = (uint8_t *)imei;

    for (int j = 0; j < sizeof(nvmImei_t); j++)
    {
        wrImei[j] -= (uint8_t)'0';
        if (wrImei[j] > 9)
        {
            OSI_LOGE(0, "write Imei num illegal");
            return -1;
        }
    }
    OSI_LOGD(0, "write index %d", index);

    rawImei[0] = ((*wrImei) << 4) | 0xa;

    for (int i = 1; i < 8; i++)
    {
        rawImei[i] = *(++wrImei);
        rawImei[i] |= (*(++wrImei)) << 4;
    }
    if (nvmWriteItem(nvid, rawImei, imei_len) < 0)
        return -1;

    return sizeof(nvmImei_t);
}

int nvmReadSN(int index, void *nvmSn, int size)
{
    if (size < 0 || size > MAX_SN_LEN)
        return -1;

    phaseCheckHead_t phaseCk = {};
    if (!nvmReadPhasecheck(&phaseCk))
        return -1;

    int len = -1;
    if (0 == index)
    {
        len = _snLength(phaseCk.SN);
        memcpy(nvmSn, &(phaseCk.SN[0]), len);
    }
    if (1 == index)
    {
        len = _snLength(phaseCk.SN2);
        memcpy(nvmSn, &(phaseCk.SN2[0]), len);
    }

    return len;
}

int nvmWriteSN(int index, const void *nvmSn, int size)
{
    if (size < 0 || size > MAX_SN_LEN)
        return -1;

    phaseCheckHead_t phaseCk = {};
    if (!nvmReadPhasecheck(&phaseCk))
        return -1;

    if (0 == index)
        memcpy(&(phaseCk.SN), nvmSn, size);
    if (1 == index)
        memcpy(&(phaseCk.SN2), nvmSn, size);

    if (!nvmWritePhaseCheck(&phaseCk))
        return -1;

    return size;
}

#ifdef CONFIG_SOC_8910
int nvmReadAudioCodec(void *nvmSn, int size)
{
    return nvmReadItem(NVID_AUD_CODEC_NV, nvmSn, size);
}

int nvmWriteAudioCodec(void *nvmSn, int size)
{
    return nvmWriteItem(NVID_AUD_CODEC_NV, nvmSn, size);
}
#endif
