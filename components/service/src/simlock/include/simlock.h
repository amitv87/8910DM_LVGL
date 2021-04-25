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

#ifndef _SIM_LOCK_H_
#define _SIM_LOCK_H_

#include "simlock_type.h"
#include "simlock_storage.h"
#include "rsa.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define SIMLOCK_PROTECT_DATA
#ifdef SIMLOCK_PROTECT_DATA
#define SIMLOCK_ERR_INIT_MALLOC_RF (0x00000001)
#define SIMLOCK_ERR_INIT_RF (0x00000002)
#define SIMLOCK_ERR_INIT_BT (0x00000004)
#define SIMLOCK_ERR_INIT_IMEI (0x00000008)
#define SIMLOCK_ERR_INIT_IMEI2 (0x00000010)
#define SIMLOCK_ERR_READ_PRO (0x00000020)
#define SIMLOCK_ERR_WRITE_PRO (0x00000040)
#define SIMLOCK_ERR_SHA_GET (0x00000080)
#define SIMLOCK_ERR_SIGN_DES (0x00000100)
#define SIMLOCK_ERR_SIGN_GET (0x00000200)
#define SIMLOCK_ERR_SIGN_VERI (0x00000400)
#define SIMLOCK_ERR_DUMM_GET (0x00000800)
#define SIMLOCK_ERR_IMEI_GET (0x00001000)
#define SIMLOCK_ERR_SIMC_GET (0x00002000)
#define SIMLOCK_ERR_KEY_GET (0x00004000)
#define SIMLOCK_ERR_USER_GET (0x00008000)
#define SIMLOCK_ERR_SIZE_GET (0x00010000)
#define SIMLOCK_ERR_READ_REV (0x00020000)
#define SIMLOCK_ERR_WRITE_REV (0x00040000)
#define SIMLOCK_ERR_RESTORE_RF_ALOC (0x00080000) // restore fail
#define SIMLOCK_ERR_RESTORE_BT (0x00100000)
#define SIMLOCK_ERR_RESTORE_IMEI1 (0x00200000)
#define SIMLOCK_ERR_RESTORE_IMEI2 (0x00400000)
#define SIMLOCK_ERR_RESTORE_RF (0x00800000)

#define SIMLOCK_ERR_INIT_ERR (SIMLOCK_ERR_INIT_MALLOC_RF | SIMLOCK_ERR_INIT_RF | \
                              SIMLOCK_ERR_INIT_BT | SIMLOCK_ERR_INIT_IMEI | SIMLOCK_ERR_INIT_IMEI2)
#define SIMLOCK_ERR_RESTORE_SIM (SIMLOCK_ERR_RESTORE_IMEI1 | SIMLOCK_ERR_RESTORE_IMEI2 | SIMLOCK_ERR_RESTORE_RF | SIMLOCK_ERR_RESTORE_RF_ALOC)
#endif

typedef enum _SIMLOCK_ERROR_TYPE
{
    SIMLOCK_SUCCESS = 0,
    SIMLOCK_ERROR = 1,
    SIMLOCK_READ_DATA_ERROR = 2,
    SIMLOCK_WRITE_DATA_ERROR = 3,
    SIMLOCK_READ_DATASIZE_ERROR = 4,
    SIMLOCK_WRITE_DUMMY_ERROR = 5,
    SIMLOCK_READ_DUMMY_ERROR = 6,
    SIMLOCK_WRITE_USERDATA_ERROR,
    SIMLOCK_READ_USERDATA_ERROR,
    SIMLOCK_READ_HEADER_ERROR,
    SIMLOCK_READ_PUBLICKEY_ERROR,
    SIMLOCK_WRITE_PUBLICKEY_ERROR,
    SIMLOCK_READ_SIGNATURE_ERROR,
    SIMLOCK_WRITE_SIGNATURE_ERROR,
    SIMLOCK_READ_KEYS_ERROR,
    SIMLOCK_WRITE_KEYS_ERROR,
    SIMLOCK_READ_UID_ERROR,
    SIMLOCK_SHA1_ERROR,
    SIMLOCK_DATA_VERIFY_ERROR,
#ifdef SIMLOCK_PROTECT_DATA
    SIMLOCK_READ_PROTECT_ERROR,
    SIMLOCK_WRITE_PROTECT_ERROR,
    SIMLOCK_READ_RF_REVERT_ERROR,
    SIMLOCK_PRE_RF_REVERT_ERROR, // prepare rf data for write
    SIMLOCK_WRITE_RF_REVERT_ERROR,
    SIMLOCK_READ_PROTECT_NV_ERROR,
    SIMLOCK_PROTECT_NV_ERROR,
#endif
    SIMLOCK_DECRYPT_ERROR
} SIMLOCK_ERROR_TYPE;

/**
 * @brief  Read simlock header from simlock partition.
 * @param  simlock_header return simlock header.
 * @return
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE Simlock_Readheader(SIMLOCK_HEADER *simlock_header);

/**
 * @brief Get RSA publick key
 * @param public_key Return RSA public key.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockReadPublicKey(PUBLIC_KEY *public_key);

/**
 * @brief Write RSA publick key.
 * @param public_key The data of RSA public key.
 * @param size The size of public key.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockWritePublicKey(PUBLIC_KEY *public_key, uint32_t size);

/**
 * @brief Write the designed size of simlock dada to simlock partition
 * @param buf  The data write to simlock partition.
 * @param size  The size of data being written.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockWriteDataAndSize(uint8_t *buf, uint32_t size);

/**
 * @brief write encrypted signature simlock data
 * @param signature The singature of simlock data
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockWriteDataSignature(uint8_t *signature);

/**
 * @brief write encrypted keys (pin/puk simlock password)
 * @param keys The keys of simlock.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockWriteKeys(uint8_t *keys);

/**
 * @brief Read Customize simlock data  from simlock partition
 * @param simlock_customize_data Store the simlock data inlcude of network_locks and return.
 *                               note: dummy1, dummy2, dummy3, dummy4,
 *                               the first byte is password length store in dummy1.
 *                               the password of simlock_customize data store in the following word dummy1,dummy2 ...
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockReadCustomiseData(MN_SIM_LOCK_CUSTOMIZE_DATA_T *simlock_customize_data);

/**
 * @brief Read Customize simlock data  from simlock partition
 * @param simlock_user_data Store the simlock user locks and return.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockReadUserData(MN_SIM_LOCK_USER_DATA_T *simlock_user_data);

/**
 * @brief Read control key data from simlock partition
 * @param simlock_control_key Store the simlock control key and return.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read
 */
SIMLOCK_ERROR_TYPE simlockReadControlkeyData(MN_SIM_LOCK_CONTROL_KEY_T *simlock_control_key);

/**
 * @brief Write simlock encrypted dynamic data to simlock partition
 * @param enc_data The encypted dynamic data write to simlock partition.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to write
 */
SIMLOCK_ERROR_TYPE simlockWriteEncryptUserData(uint8_t *enc_data);

/**
 * @brief Get simlock data hash
 * @param hash_id The hash algorithm selected,such as RSA_SHA1,RSA_SHA256,RSA_MD5.
 * @param hash_len The length of hash data.
 * @param shasum The hash data of simlock data.
 * @return:
 *      - 0 Read successfully
 *      - OTHERS (>0)  Fail to read.
 */
SIMLOCK_ERROR_TYPE simlockSHA(uint8_t hash_id, uint8_t hashlen, uint8_t *shasum);

/**
 * @brief Write simlock encrypted dummy to simlock partition
 * @param dummy The dummy data write to simlock partition.
 * @param length The length of dummy data.
 * @return:
 *      - 0 Write successfully
 *      - OTHERS (>0)  Fail to write.
 */
SIMLOCK_ERROR_TYPE simlockWriteDummy(uint8_t *dummy, uint32_t length);

/**
 * @brief Verify simlock data.
 * @return:
 *      - true Verify successfully
 *      - false Fail to verify.
 */
bool simlockDataVerify(void);

/**
 * @brief This function is used to update the user data.
 * @return:
 *      - true Update successfully
 *      - fail Fail to update.
 */
bool simlockEncryptDataUpdate(void /* user_data*/); // the last step to protect the simlock status

/**
 * @brief This function is used to check if the last updated keys from UI or NV can
 *        match the encrypt keys, if match, the protocol stack can boot up
 *        or else, the protocol stack will boot up restricted.
 * @param type The type of simlock key,such as nck, nsck, spck ,cck, pck.
 * @param key  The data of key will be check.
 * @return:
 *      - true Check successfully
 *      - fail Fail to check.
 */
bool simlockCheck(uint32_t type, SIM_LOCK_KEY_T *key);

/**
 * @brief This function is used to get the uid of this chip.
 * @param uid Store the data of uid and return.
 * @return:
 *      - 0 Read the uid successfully
 *      - OTHERS (>0)  Fail to get the uid.
 */
uint32_t simlockGetUid(uint8_t *uid);

/**
 * @brief Read simlock data size from simlock partition
 * @param length Store the length of user data and return.
 * @return:
 *      - 0 Read the uid successfully
 *      - OTHERS (>0)  Fail to get the uid.
 */
SIMLOCK_ERROR_TYPE simlockReadDataSize(uint32_t *length);

/**
 * @brief Read simlock data size from simlock partition
 * @param buf Store the data of user data.
 * @param length Store the length of user data.
 * @return:
 *      - 0 Read the uid successfully
 *      - OTHERS (>0)  Fail to get the uid.
 */
SIMLOCK_ERROR_TYPE simlockReadData(uint8_t *buf, uint32_t length);

/**
 * @brief Get the public key of simlock
 * @param rsa_ctx_ptr Store the data of simlock public key.
 * @return:
 *      - 0 Fail to read.
 *      - 1 Get successfully.
 */
uint32_t simlockGetRsaPubkey(rsa_context *rsa_ctx_ptr);

#ifdef __cplusplus
}
#endif

#endif /* _SIM_LOCK_H_*/
