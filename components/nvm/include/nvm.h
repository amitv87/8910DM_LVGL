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

#ifndef _NVM_H_
#define _NVM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "quec_cust_patch.h"

#define MAX_SN_LEN (24)
#define MAX_STATION_NUM (15)
#define MAX_LAST_DESCRIPTION_LEN (32)
#define MAX_STATION_NAME_LEN (10)

typedef uint8_t nvmImei_t[15];
typedef uint8_t nvmImei2_t[8];

/**
 * \brief NVID may be refered by other modules
 */
enum
{
    NVID_CALIB_PARAM = 0x002,
    NVID_IMEI1 = 0x005,
    NVID_IMEI2 = 0x179,
    NVID_IMEI3 = 0x186,
    NVID_IMEI4 = 0x1e4,
    NVID_SIM_LOCK_CUSTOMIZE_DATA = 0x1f0,
    NVID_SIM_LOCK_USER_DATA = 0x1f1,
    NVID_SIM_LOCK_CONTROL_KEY_ID = 0x1f2,
    // ims nv
    NVID_IMS_CSM_NV = 0x800,
    NVID_IMS_SAPP_NV = 0x801,
    NVID_IMS_ISIM_NV = 0x813,
    NVID_SIM_DELTA_NV = 0x870,
    NVID_SIM_DELTA_CARD2_NV = 0x871,
    NVID_PLMN_DELTA_NV = 0x872,
    NVID_PLMN_DELTA_CARD2_NV = 0x873,
};

/**
 * \brief phaseCheck struct
 */
typedef struct
{
    uint32_t Magic;
    uint8_t SN[MAX_SN_LEN];
    uint8_t SN2[MAX_SN_LEN];
    uint32_t StationNum; // the test station number of the testing
    uint8_t StationName[MAX_STATION_NUM][MAX_STATION_NAME_LEN];
    // uint8_t   Reserved[48]; /*reserved*/
    // uint16_t  iItem; /*part1: Bit0~ Bit_14 indicate test Station reserved*/
    uint8_t Reserved[13];
    uint8_t SignFlag; // internal flag
    uint8_t szLastFailDescription[MAX_LAST_DESCRIPTION_LEN];
    uint16_t iTestSign; // Bit0~Bit14 ---> station0~station 14  if tested. 0: tested, 1: not tested
    uint16_t iItem;     // part1: Bit0~ Bit_14 indicate test Station, 0: Pass, 1: fail
} phaseCheckHead_t;

typedef struct
{
    uint8_t addr[6];
} btAddress_t;

typedef struct
{
    uint8_t engine_sn[24];
    uint8_t map_version[4];
    uint8_t activate_code[16];
} gpsNVInfo_t;

typedef struct
{
    uint8_t imei1[8]; /*imei1 number 8byte*/
    uint8_t imei2[8]; /*imei2 number 8byte*/
    btAddress_t bt_add;
    gpsNVInfo_t gps_info;
    uint8_t wifi_add[6];
    uint8_t reserve1[2];
    uint8_t imei3[8]; /*imei3 number 8byte*/
    uint8_t imei4[8]; /*imei4 number 8byte*/
    uint8_t reserved[16];
} refNVDirect_t;

typedef struct
{
    uint32_t ims_csm_addr;
    uint32_t ims_csm_len;

    uint32_t ims_sapp_addr;
    uint32_t ims_sapp_len;

    uint32_t ims_isim_addr;
    uint32_t ims_isim_len;

    uint32_t ims_sim1_delta_addr;
    uint32_t ims_sim1_delta_len;

    uint32_t ims_sim2_delta_addr;
    uint32_t ims_sim2_delta_len;

    uint32_t ims_plmn1_delta_addr;
    uint32_t ims_plmn1_delta_len;

    uint32_t ims_plmn2_delta_addr;
    uint32_t ims_plmn2_delta_len;
} imsNVInfo_t;

/**
 * \brief initialize nvm
 *
 * It should be called before any nv access.
 */
void nvmInit(void);

/**
 * \brief read nv item to buffer
 *
 * This will read the running data (rather than the fixed data)
 * of nv items.
 *
 * When \p buf is NULL or \p size is 0, it will return the size of
 * nv item if the nv item exists.
 *
 * \p buf should be large enough to hold the whole nv item. Otherwise,
 * it will return -1. That is, partial read of nv item is not supported.
 *
 * \param nvid      nv ID
 * \param buf       buffer for nv item read
 * \param size      buffer size
 * \return
 *      - nv item size on success
 *      - -1 on fail
 *          - invalid nvid, or not exist
 *          - \p size is less than nv item size
 */
int nvmReadItem(uint16_t nvid, void *buf, unsigned size);

/**
 * \brief write nv item from buffer
 *
 * This can be used to write both cp nv and factory nv, but the behavior
 * is different.
 *
 * For cp nv, the nv data will be written to the file for running data.
 *
 * For factory nv, the nv data will be written to the factory nv file
 * directly. However, /factory is mounted as read only by default, and
 * ythe write will fail. If the system is in calibration mode, in which
 * mode /factory will be mounted, or /factory is remounted to read write,
 * the factory nv data is changed.
 *
 * The content will be checked before write. When the exist contents is
 * the same, no operation will be taken and is regarded as success.
 *
 * \param nvid      nv ID
 * \param buf       buffer for nv item write
 * \param size      buffer size
 * \return
 *      - 0 when the existed content is the same
 *      - nv item size on changed
 *      - -1 on fail
 */
int nvmWriteItem(uint16_t nvid, const void *buf, unsigned size);

/**
 * \brief read nv item to buffer, not considering running data
 *
 * Comparing to \p nvmReadItem, the difference is this won't check running
 * data file.
 *
 * This is only for nv maintenance. It should be initiated by PC tool, and
 * normal application shouldn't call this.
 *
 * \param nvid      nv ID
 * \param buf       buffer for nv item read
 * \param size      buffer size
 * \return
 *      - nv item size on success
 *      - -1 on fail
 *          - invalid nvid, or not exist
 *          - \p size is less than nv item size
 */
int nvmReadFixedItem(uint16_t nvid, void *buf, unsigned size);

/**
 * \brief write nv item from buffer, not considering running data
 *
 * Comparing to \p nvmWriteItem, the difference is this will write to the
 * fixed nv file forcedly.
 *
 * This is only for nv maintenance. It should be initiated by PC tool, and
 * normal application shouldn't call this.
 *
 * \param nvid      nv ID
 * \param buf       buffer for nv item write
 * \param size      buffer size
 * \return
 *      - 0 when the existed content is the same
 *      - nv item size on changed
 *      - -1 on fail
 */
int nvmWriteFixedItem(uint16_t nvid, const void *buf, unsigned size);

/**
 * \brief clear all running nv data
 *
 * It is assumed that running nv data files are located under dedicated
 * directory. So, just remove all files under the dedicated directory
 * for running data to clean all running nv data.
 */
void nvmClearRunning(void);

/**
 * \brief clear all factory data
 *
 * It is dangeruous!! It should only be called in FactoryDownload.
 * So, it should *NOT* be linked into application, and only be linked
 * into FDL2.
 */
void nvmClearFactory(void);

/**
 * \brief read all fixed nv data and packed to bin
 *
 * The bin format follows the data format received in ResearchDownload.
 *
 * When \p size is not enough for all nv data, the buffer will be modified,
 * but the return value is -1.
 *
 * When \p size is larger than read nv size, the remaining will be filled
 * with 0xff, and the CRC at head will be the CRC of the total size.
 *
 * When \p buf is NULL or \p size is 0, it will return the size needed
 * for all nv data.
 *
 * \param buf       buffer for packed bin
 * \param size      buffer size
 * \return
 *      - size of packed bin
 *      - -1 on fail
 */
int nvmReadFixedBin(void *buf, unsigned size);

#ifdef QUEC_PATCH_FACTORY_UPDATE
int nvmWriteFixedBin_v2(const void *buf, unsigned size, bool parsecheck);
#endif

/**
 * \brief write fixed nv data from packed bin
 *
 * The bin format follows the data format received in ResearchDownload.
 *
 * The content will be checked before write. When the exist contents is
 * the same, no operation will be taken and is regarded as success.
 *
 * Also, when the nvid is production nv, and the production nv exists,
 * no operation will be taken and is regarded as success.
 *
 * When one item write fails, this will return -1 immediately. The
 * remaining data are ignored, and it is possible that some nv items
 * are already changed.
 *
 * \param buf       buffer for packed bin
 * \param size      buffer size
 * \return
 *      - sizeof packed bin
 *      - -1 on fail
 */
int nvmWriteFixedBin(const void *buf, unsigned size);

/**
 * \brief read phasecheck data
 *
 * It is not recommended to call this to access phasecheck data directly.
 * Rather, there are several APIs to access concrete fields of phasecheck.
 *
 * \param chk       buffer packet pointer
 * \return
 *      - true if read success
 *      - false if read fail
 */
bool nvmReadPhasecheck(phaseCheckHead_t *chk);

/**
 * \brief write phasecheck data
 *
 * \param chk       buffer packet pointer
 * \return
 *      - true if read success
 *      - false if read fail
 */
bool nvmWritePhaseCheck(const phaseCheckHead_t *chk);

/**
 * \brief clear PhaseCheck data
 *
 * It is dangeruous!! It will be delete PhaseCheck data.
 * It will unlink the file when erase PhaseCheck Flash in FDL2.
 */
void nvmClearPhaseCheck(void);

/**
 * \brief get nv id from file name
 *
 * This is for 8910 only. In shared memory list, each item is assigned
 * a file name. This file name will be used as the nv file name, not
 * including directory name.
 *
 * It just search the map, not check whether the nv file exists.
 *
 * \param fname     nv file name
 * \return
 *      - nv id
 *      - -1 if \p fname is invalid
 */
int nvmGetIdFromFileName(const char *fname);

/**
 * \brief read imei data from imei.bin
 *
 * \param index     for imei1/imei2
 * \param imei      buffer packet pointer, contents are ASCII format
 * \return          read bytes, if error return -1
 */
int nvmReadImei(int index, nvmImei_t *imei);

/**
 * \brief read imei data from imei.bin
 *
 * \param index     for imei1/imei2
 * \param imei      buffer packet pointer, contents are not ASCII format
 * \return          read bytes, if error return -1
 */
int nvmReadImei2(int index, nvmImei2_t *imei);

/**
 * \brief write imei data to imei.bin
 *
 * \param index     for imei1/imei2
 * \param imei      buffer packet pointer, contents should be ASCII format
 * \return          write bytes ,if error return -1
 */
int nvmWriteImei(int index, const nvmImei_t *imei);

/**
 * \brief write imei data to imei.bin
 *
 * \param index     for imei1/imei2
 * \param imei      buffer packet pointer, contents should be not ASCII format
 * \return          write bytes ,if error return -1
 */
int nvmWriteImei2(int index, const nvmImei2_t *imei);

/**
 * \brief read SN from phasecheck.bin
 *
 * \param index     for SN1/SN2
 * \param nvmSn     buffer packet pointer, ASCII format
 * \return          read size ,if error return -1
 */
int nvmReadSN(int index, void *nvmSn, int size);

/**
 * \brief write SN to phasecheck.bin
 *
 * \param index     for SN1/SN2
 * \param nvmSn     buffer packet pointer, ASCII format
 * \return          write size, if error return -1
 */
int nvmWriteSN(int index, const void *nvmSn, int len);

#ifdef CONFIG_SOC_8910
/**
 * \brief read audio codec config information
 *
 * \param nvmSn     audio codec struct
 * \return          read size, if error return -1
 */
int nvmReadAudioCodec(void *nvmSn, int size);

/**
 * \brief write audio codec config information
 *
 * \param nvmSn     audio codec struct
 * \return          write size, if error return -1
 */
int nvmWriteAudioCodec(void *nvmSn, int size);
#endif

/**
 * \brief migrate nvm data ensure the important nv.bin exist
 * \return
 *      - true if migrate success
 *      - false if migrate fail
 */
bool nvmMigration(void);

#ifdef __cplusplus
}
#endif
#endif
