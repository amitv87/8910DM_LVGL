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

#ifndef _SIM_LOCK_SERV_H
#define _SIM_LOCK_SERV_H

#ifdef __cplusplus
extern "C" {
#endif
#include "simlock_type.h"
#include "simlock_storage.h"

// custimize data
typedef struct
{
    uint32_t simlock_support;
    uint32_t oper_mode;
    uint32_t ctrl_key_type;
    uint32_t simlock_status;
    uint32_t max_num_trials;
    NETWORK_LOCKS_T network_locks;
    NETWORK_SUBSET_LOCKS_T network_subset_locks;
    SP_LOCKS_T sp_locks;
    CORPORATE_LOCKS_T corporate_locks;
    USER_LOCKS_T user_locks;
} simlock_cust_data_t;

// user data
typedef struct
{
    uint32_t simlock_status;
    uint32_t nck_trials;
    uint32_t nck_unlock_time;
    uint32_t nsck_trials;
    uint32_t nsck_unlock_time;
    uint32_t spck_trials;
    uint32_t spck_unlock_time;
    uint32_t cck_trials;
    uint32_t cck_unlock_time;
    uint32_t pck_trials;
    uint32_t pck_unlock_time;
} simlock_dynamic_data_t;

typedef struct
{
    simlock_dynamic_data_t dynamic_data;
    uint32_t dummy[4];
} simlock_user_data_t;

typedef struct
{
    uint32_t SIM_lock_status;
    uint32_t max_num_trials;
    NETWORK_LOCKS_T network_locks;
    NETWORK_SUBSET_LOCKS_T network_subset_locks;
    SP_LOCKS_T SP_locks;
    CORPORATE_LOCKS_T corporate_locks;
    USER_LOCKS2_T user_locks;
} srvSimlockData_t;

typedef SIM_LOCK_KEY_T srvSimlockKey_t;
/**
 * data structure for PC tool or other clients
 */
typedef struct
{
    simlock_cust_data_t cust_data;
    simlock_encrypt_keys_t encrypt_keys;
    uint8_t sign[SL_RSA_SIGN_LEN];
    uint8_t enc_user_data[SL_RSA_N_H_LEN];
} simlock_data_t;

/**
 * @brief This function is used to save the random number "HUK".
 *        This number will be used for tool authentication.
 * @param data The data of huk.
 * @param len the len of huk.
 * @return:
 *      - 0 Write publick failure.
 *      - 1 Write public success.
 */
uint32_t srvSimlockSetHuk(const char *data, uint32_t len);

/**
 * @brief This function is used to check and save the RSA public key.
 * @param rsa_pubkey The original data of public key.
 * @param pubkey_len the len of public key.
 * @param sign The signature of public key.
 * @param sign_len the len of signtaure.
 * @return:
 *      - 0 Write publick failure.
 *      - 1 Write public success.
 */
uint32_t srvSimlockFacauthPubkey(const char *rsa_pubkey, uint32_t pubkey_len, const char *sign, uint32_t sign_len);

/**
 * @brief This function is used to start authentication.
 * @param data Output the data is used for authentication.
 * @param len the len of ouput data.
 * @return:
 *      - 0 The satrt of Authentication failure.
 *      - 1 The satrt of Authentication success.
 */
uint32_t srvSimlockAuthStart(char *data, uint32_t *len);

/**
 * @brief This function is used to end authentication.
 * @param data Input the data is used for authentication.
 * @param len the len of input data.
 * @return:
 *      - 0 Authentication failure.
 *      - 1 Authentication success.
 */
uint32_t srvSimlockAuthEnd(const char *data, uint32_t len);

/**
 * @brief This function is used to encrypt the original simlock keys
 * @param data Input the data of simlock keys.
 * @param len the len of data of simlock keys.
 * @param encrypt_data The return cipher data of simlock keys.
 * @param en_len The return len of cipher data.
 * @return:
 *      - 0 Fail to write.
 *      - 1 Write successfully.
 */
uint32_t srvSimlockEncryptKeys(const char *data, uint32_t len, char *encrypt_data, uint32_t en_len);

/**
 * @brief Tools write the simlock data to device.
 * @param data Input the data of simlock.
 * @param len the len of data.
 * @return:
 *      - 0 Fail to write.
 *      - 1 Write successfully.
 */
uint32_t srvSimlockDataProcess(const char *data, uint32_t len);

/**
 * @brief Tools write the rsa signature to device.
 * @param signed_data Input the signature of simlock data.
 * @param len the len of signature.
 * @return:
 *      - 0 Fail to write.
 *      - 1 Write successfully.
 */
uint32_t srvSimlockDdataSha(uint8_t *hash);

/**
 * @brief Tools write the rsa signature to device.
 * @param signed_data Input the signature of simlock data.
 * @param len the len of signature.
 * @return:
 *      - 0 Fail to write.
 *      - 1 Write successfully.
 */
uint32_t srvSimlockSignData(const uint8_t *signed_data, uint32_t len);

/**
 * @brief Read simlock data
 * @param simlock_data Store the data of user data.
 * @return:
 */
void srvSimlockGetLocksData(srvSimlockData_t *simlock_data);

/**
 * @brief This function is used to init the simlock service.
 */
void srvSimlockInit();

#ifdef __cplusplus
}
#endif

#endif // End of _SIM_LOCK_H
