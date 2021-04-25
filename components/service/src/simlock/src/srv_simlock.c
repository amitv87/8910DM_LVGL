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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'I', 'M', 'L')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_api.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <osi_log.h>
#include "rsa.h"
#include "aes.h"
#include "sha1.h"
#include "sha2.h"
#include "simlock.h"
#include "srv_simlock.h"
#include "simlock_storage.h"
#include "simlock_aescrypt.h"
#include "srv_config.h"
#include "nvm.h"

#define SL_SSK_STRING "Secure Storage Key"
#define SL_SSK_STR_LEN 18
#define SL_RSA2048_C

//#define EN_SL_SAVE_EFUSE
#define SPK_EFUSE_BLKID 14

// AES information
#define AES_KEY_LEN (32)
#define SL_AES_IMEI_LEN (16)
#define SL_AES_USER_LEN (32)
#define SL_AES_SIGN_LEN (64)
#define SL_AES_SIGN_H_LEN (SL_AES_SIGN_LEN >> 1)

// HUK(random number) definition
#define SL_HUK_LEN (8)
#define SL_HUK_H_LEN (SL_HUK_LEN >> 1)
#define SL_RAND_NUM_LEN (8)
#define SL_RAND_NUM_H_LEN (SL_RAND_NUM_LEN >> 1)

// SHA information
#define SL_SHA256_LEN (32)
#define SL_SHA1_LEN (20)

static uint8_t s_huk[SL_HUK_H_LEN] = {0};

static int s_identifypass = 0;
static uint32_t s_rand = 0;
//static uint32_t public_key_efuse=0;

extern uint16_t crc16(uint16_t crc, uint8_t const *buffer, uint32_t len);

static uint32_t prvSimlockRand(void)
{
    uint32_t rand_num = 0;

    rand_num = rand();

    return rand_num;
}

static uint32_t prvSimlockAscii2Hex(const char *src, uint8_t *dst, int len)
{
    int i, j, tmp_len;
    uint8_t tmpData;
    char *ascii_data = (char *)malloc(len);

    if (ascii_data == NULL)
        return 0;

    memcpy(ascii_data, src, len);

    for (i = 0; i < len; i++)
    {
        if ((ascii_data[i] >= '0') && (ascii_data[i] <= '9'))
        {
            tmpData = ascii_data[i] - '0';
        }
        else if ((ascii_data[i] >= 'A') && (ascii_data[i] <= 'F'))
        { //A....F
            tmpData = ascii_data[i] - 'A' + 10;
        }
        else if ((ascii_data[i] >= 'a') && (ascii_data[i] <= 'f'))
        { //a....f
            tmpData = ascii_data[i] - 'a' + 10;
        }
        else
        {
            break;
        }
        ascii_data[i] = tmpData;
    }

    for (tmp_len = 0, j = 0; j < i; j += 2)
    {
        dst[tmp_len++] = (ascii_data[j] << 4) | ascii_data[j + 1];
    }

    free(ascii_data);
    return tmp_len;
}

static uint32_t prvSimlock_hex2ascii(uint8_t *src, char *dst, int len)
{
    int i;
    char tmp[3];

    for (i = 0; i < len; i++)
    {
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%02x", src[i]);
        strcat(dst, tmp);
    }

    return strlen(dst);
}

/*****************************************************************************/
// Description :   This function is used to save the random number "HUK". This
//         number will be used for tool authentication.
//
// Global resource dependence :
// Author :         alvin.zhou
// Note :
/*****************************************************************************/
uint32_t srvSimlockSetHuk(const char *data, uint32_t len)
{
    int ret = -1;
    uint8_t huk[SL_HUK_H_LEN] = {0};

    if (!data || len != SL_HUK_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: parameter error, len: %d !\n", len);
        return 0;
    }

    ret = prvSimlockAscii2Hex(data, huk, len);

    if (ret != SL_HUK_H_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: orial data error, ret: %d !\n", ret);
        return 0;
    }
    memcpy(s_huk, huk, SL_HUK_H_LEN);

    return 1;
}

/*****************************************************************************/
// Description :   This function is used to check and save the RSA public key.
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t srvSimlockFacauthPubkey(const char *rsa_pubkey, uint32_t pubkey_len, const char *sign, uint32_t sign_len)
{
    int i = 0;
    int ret = -1;
    uint8_t sign_hex[SL_AES_SIGN_H_LEN] = {0};
    uint8_t key_data[SL_HUK_H_LEN + SL_SSK_STR_LEN] = {0};
    uint8_t aes_key[SL_AES_SIGN_H_LEN] = {0};
    uint8_t rsa_sh256[SL_SHA256_LEN] = {0};
    uint8_t sign_now[SL_AES_SIGN_H_LEN] = {0};

    if (!rsa_pubkey || pubkey_len != SL_RSA_PK_LEN || !sign || sign_len != SL_AES_SIGN_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: parameter error, %x,%x !\n", pubkey_len, sign_len);
        return 0;
    }
    // get signature
    ret = prvSimlockAscii2Hex(sign, sign_hex, sign_len);
    if (ret != SL_AES_SIGN_H_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: sign string error, ret: %d!\n", ret);
        return 0;
    }

    memcpy(key_data, s_huk, SL_HUK_H_LEN);

    memcpy(key_data + SL_HUK_H_LEN, SL_SSK_STRING, SL_SSK_STR_LEN);

    // general AES key
    sha2((unsigned char *)key_data, SL_HUK_H_LEN + SL_SSK_STR_LEN, aes_key, 0);

    // general data will be encrypted
    sha2((unsigned char *)rsa_pubkey, SL_RSA_PK_LEN, rsa_sh256, 0);

    // AES encrypt
    if (!simlockAesEncrypt(rsa_sh256, SL_SHA256_LEN, sign_now, aes_key, SL_AES_SIGN_H_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: aes encrypt failed \n");
        return 0;
    }

    // compare the two signature
    for (i = 0; i < SL_AES_SIGN_H_LEN; i++)
    {
        if (sign_hex[i] != sign_now[i])
        {
            OSI_LOGI(0, "SIMLOCK: sign error !\n");
            return 0;
        }
    }

    // write back to simlock partition
    if (SIMLOCK_SUCCESS != simlockWritePublicKey((PUBLIC_KEY *)rsa_pubkey, pubkey_len))
    {
        OSI_LOGI(0, "SIMLOCK: rsa pk can't be saved !\n");
        return 0;
    }
    return 1;
}

/*****************************************************************************/
// Description :   This function is used to start authentication.
//
// Global resource dependence :
// Author :         alvin.zhou
// Note :
/*****************************************************************************/
uint32_t srvSimlockAuthStart(char *data, uint32_t *len)
{
    int ret = -1;
    unsigned char rsa_cipher_data[SL_RSA_N_LEN >> 1] = {0};
    rsa_context rsa_ctx;

    // reset the auth flag,
    s_identifypass = 0;
    // general the rand number
    s_rand = prvSimlockRand();

    if (NULL == data)
    {
        OSI_LOGI(0, "SIMLOCK: parameter error ptr is null");
        return 0;
    }
    if (!simlockGetRsaPubkey(&rsa_ctx))
    {
        OSI_LOGI(0, "SIMLOCK: get rsa pubkey failed.\n");
        return 0;
    }

    ret = rsa_pkcs1_encrypt(&rsa_ctx, RSA_PUBLIC, SL_RAND_NUM_H_LEN, (uint8_t *)&s_rand, rsa_cipher_data);
    if (ret)
    {
        OSI_LOGI(0, "SIMLOCK: rsa encrypt failed, ret: %d\n", ret);
        return 0;
    }

    if (*len < (SL_RSA_N_LEN >> 1))
    {
        OSI_LOGI(0, "SIMLOCK: output buffer is not enough for data, len: %d\n", *len);
        return 0;
    }

    ret = prvSimlock_hex2ascii(rsa_cipher_data, data, (SL_RSA_N_LEN >> 1));
    if (ret != SL_RSA_N_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: rsa data convert to string failed, ret: %d\n", ret);
        return 0;
    }

    *len = ret;

    return 1;
}

/*****************************************************************************/
// Description :   This function is used to end authentication.
//
// Global resource dependence :
// Author :         alvin.zhou
// Note :
/*****************************************************************************/
uint32_t srvSimlockAuthEnd(const char *data, uint32_t len)
{
    int ret = -1;
    int cipher_len = 0;
    uint32_t rand = 0;
    unsigned char rsa_cipher_data[SL_RSA_N_LEN >> 1] = {0};
    rsa_context rsa_ctx;

    if (!data || len != SL_RSA_N_LEN)
    {
        OSI_LOGI(0, "SIMLOCK: parameter error, len:%d\n", len);
        return 0;
    }

    ret = prvSimlockAscii2Hex(data, rsa_cipher_data, SL_RSA_N_LEN);
    if (ret != (SL_RSA_N_LEN >> 1))
    {
        OSI_LOGI(0, "SIMLOCK: cipher data error, ret: %d\n", ret);
        return 0;
    }

    if (!simlockGetRsaPubkey(&rsa_ctx))
    {
        OSI_LOGI(0, "SIMLOCK: get rsa pubkey failed.\n");
        return 0;
    }

    ret = rsa_pkcs1_decrypt(&rsa_ctx, RSA_PUBLIC, &cipher_len, rsa_cipher_data, (unsigned char *)&rand);
    if (ret)
    {
        OSI_LOGI(0, "SIMLOCK: rsa decrypt error , ret: %d!\n", ret);
        return 0;
    }

    // compare the two rand number
    if (rand == s_rand)
    {
        s_identifypass = 1;
    }
    else
    {
        OSI_LOGI(0, "SIMLOCK:auth failed, rand: %x!\n", rand);
        return 0;
    }

    return 1;
}

/*****************************************************************************/
// Description :   This function is used to encrypt the original simlock
//         keys(PIN/PUK). This cipher data will be returned to PC tools for keys signature.
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t srvSimlockEncryptKeys(const char *data, uint32_t len, char *encrypt_data, uint32_t en_len)
{
    int i = 0;
    int ret = 0;
    int data_offset = 0;
    int key_offset = 0;
    int enc_key_offset = 0;
    int dec_len = 0;
    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t *enc_data = NULL;
    simlock_keyinfo_t *keys_ptr = NULL;
    simlock_encrypt_keys_t *enc_key = NULL;
    simlock_encrypt_keys_t *sha256_key = NULL;
    rsa_context rsa_ctx;

    if (!s_identifypass)
    {
        OSI_LOGI(0, "SIMLOCK:  oper is not authorization");
        return 0;
    }

    if (len != (SL_RSA_N_LEN << 1) || !encrypt_data)
    {
        OSI_LOGI(0, "SIMLOCK: para err,len:%d !\n", len);
        return 0;
    }
    // alloc then assigned to zero, don't change, since there is string like "strcat,strlen" used
    enc_data = malloc(SL_RSA_N_H_LEN);
    memset(enc_data, 0, SL_RSA_N_H_LEN);
    keys_ptr = malloc(sizeof(simlock_keyinfo_t));
    memset(keys_ptr, 0, sizeof(simlock_keyinfo_t));
    enc_key = malloc(sizeof(simlock_encrypt_keys_t));
    memset(enc_key, 0, sizeof(simlock_encrypt_keys_t));
    sha256_key = malloc(sizeof(simlock_encrypt_keys_t));
    memset(sha256_key, 0, sizeof(simlock_encrypt_keys_t));

    if (enc_data == NULL || keys_ptr == NULL || enc_key == NULL || sha256_key == NULL)
    {
        OSI_LOGI(0, "SIMLOCK: simlock_encrypt_keys malloc fail.");
        goto failed_handle;
    }

    do
    {
        ret = prvSimlockAscii2Hex(data + data_offset, enc_data, SL_RSA_N_LEN);
        if (ret != SL_RSA_N_H_LEN)
        {
            OSI_LOGI(0, "SIMLOCK: data convert to hex failed !\n");
            goto failed_handle;
        }

        ret = simlockGetRsaPubkey(&rsa_ctx);
        if (!ret)
        {
            OSI_LOGI(0, "SIMLOCK: get rsa pub key failed !\n");
            goto failed_handle;
        }

        ret = rsa_pkcs1_decrypt(&rsa_ctx, RSA_PUBLIC, &dec_len, enc_data, (uint8_t *)keys_ptr);
        if (ret)
        {
            OSI_LOGI(0, "SIMLOCK: decrypt failed!err:%x\n", ret);
            goto failed_handle;
        }
        if (dec_len != sizeof(simlock_keyinfo_t))
        {
            OSI_LOGI(0, "SIMLOCK: wrong keys, dec_len:%d!\n", dec_len);
            goto failed_handle;
        }

        if (!simlockGetUid(uid))
        {
            OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
            goto failed_handle;
        }

        for (i = 5; i >= 1; i--)
        {
            if (!simlockAesEncrypt(keys_ptr->nck + key_offset, SIM_LOCK_KEY_MAX_LEN,
                                   enc_key->pin_keys.nck + enc_key_offset, uid, SL_SHA256_LEN))
            {
                OSI_LOGI(0, "SIMLOCK: PIN/PUK encrypt failed!\n");
                goto failed_handle;
            }

            sha2(enc_key->pin_keys.nck + enc_key_offset, SIM_LOCK_ENCRYPTED_KEY_MAX_LEN,
                 sha256_key->pin_keys.nck + enc_key_offset, 0);

            key_offset += SIM_LOCK_KEY_MAX_LEN;
            enc_key_offset += SIM_LOCK_ENCRYPTED_KEY_MAX_LEN;
        }

        key_offset = 0;
        data_offset += SL_RSA_N_LEN;
    } while (len != data_offset);

    if (SIMLOCK_SUCCESS != simlockWriteKeys((uint8_t *)sha256_key))
    {
        OSI_LOGI(0, "SIMLOCK: buffer is not enough for pin/puk encrypt data!\n");
        goto failed_handle;
    }

    ret = prvSimlock_hex2ascii((uint8_t *)sha256_key, encrypt_data, sizeof(simlock_encrypt_keys_t));
    if (ret != (sizeof(simlock_encrypt_keys_t) << 1))
    {
        OSI_LOGI(0, "SIMLOCK: encrypt key convert to hex format failed!,ret=%x,\n", ret);
        goto failed_handle;
    }
    if (enc_data)
        free(enc_data);
    if (keys_ptr)
        free(keys_ptr);
    if (enc_key)
        free(enc_key);
    if (sha256_key)
        free(sha256_key);

    return 1;

failed_handle:
    if (enc_data)
        free(enc_data);
    if (keys_ptr)
        free(keys_ptr);
    if (enc_key)
        free(enc_key);
    if (sha256_key)
        free(sha256_key);
    return 0;
}

/*****************************************************************************/
// Description :   This function is used to update the simlock data (cust data
//         + user data) and sign the data again with AES keys. Before updating,
//        it will check the integrity and correctness of the data.
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t srvSimlockDataProcess(const char *data, const uint32_t len)
{
    rsa_context rsa_ctx;
    int dec_len = 0;
    int ret = 0;
#if defined(SL_RSA2048_C)
    uint8_t sha_digest[SL_SHA256_LEN] = {0};
#elif defined(SL_RSA1024_C)
    uint8_t sha_digest[SL_SHA1_LEN] = {0};
#endif
    //    uint8_t sha1_digest[20] = {0};

    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t sha256_data[SL_SHA256_LEN] = {0};
    uint8_t dummy_sign[16] = {0};
    uint8_t user_data_sign[SL_SHA256_LEN] = {0};
    uint8_t *data_buf = NULL;
    simlock_data_t *simlock_data = NULL;
    simlock_user_data_t sl_user_data;
    MN_SIM_LOCK_USER_DATA_T *user_data = NULL;
#ifdef SIMLOCK_PROTECT_DATA
    simlock_protect_data_t *sl_protect = NULL;
    uint8_t *rf_restore = NULL;
#endif

    if (!s_identifypass)
    {
        OSI_LOGI(0, "SIMLOCK:  oper is not authorization");
        return 0;
    }

    //OSI_LOGI(0, "SIMLOCK: len: %d, simlock_data_t: %d \n", len, (sizeof(simlock_data_t) << 1) );

    if (len != (sizeof(simlock_data_t) << 1) || !data)
    {
        OSI_LOGI(0, "SIMLOCK: para err,len:%d (sizeof(simlock_data_t) << 1) = %d\n", len, (sizeof(simlock_data_t) << 1));
        return 0;
    }

    data_buf = malloc(sizeof(simlock_data_t) + 4 /*(data_size)*/);
    memset(data_buf, 0, sizeof(simlock_data_t) + 4 /*(data_size)*/);

    user_data = malloc(sizeof(MN_SIM_LOCK_USER_DATA_T));
    memset(user_data, 0, sizeof(MN_SIM_LOCK_USER_DATA_T));

    if ((!data_buf) || (!user_data))
    {
        OSI_LOGI(0, "SIMLOCK: simlock_data malloc failed !\n");
        goto failed_handle;
    }
#ifdef SIMLOCK_PROTECT_DATA
    rf_restore = malloc(SL_RESTORE_RF_LEN);
    if (!rf_restore)
    {
        OSI_LOGI(0, "SIMLOCK: rf_restore malloc failed !\n");
        goto failed_handle;
    }
    memset(rf_restore, 0, SL_RESTORE_RF_LEN);
#endif
    ret = prvSimlockAscii2Hex(data, (data_buf + /*data_size*/ 4), len);
    if (ret != sizeof(simlock_data_t))
    {
        OSI_LOGI(0, "SIMLOCK: data convert to hex failed !\n");
        goto failed_handle;
    }
    *(uint32_t *)data_buf = ret;
    if (!simlockGetRsaPubkey(&rsa_ctx))
    {
        OSI_LOGI(0, "SIMLOCK: get rsa pubkey failed !\n");
        goto failed_handle;
    }
    simlock_data = (simlock_data_t *)((uint8_t *)data_buf + /*data_size*/ 4);

    ret = rsa_pkcs1_decrypt(&rsa_ctx, RSA_PUBLIC, &dec_len, simlock_data->enc_user_data, (uint8_t *)&sl_user_data);
    if (ret)
    {
        OSI_LOGI(0, "SIMLOCK: decrypt failed!err:%d\n", ret);
        goto failed_handle;
    }

    if (dec_len != sizeof(simlock_user_data_t))
    {
        OSI_LOGI(0, "SIMLOCK: wrong user data!\n");
        goto failed_handle;
    }

#if defined(SL_RSA1024_C) //rsa 1024 verify
    sha1((uint8_t *)simlock_data, sizeof(simlock_cust_data_t) + sizeof(simlock_encrypt_keys_t), sha_digest);

    OSI_LOGI(0, "SIMLOCK: digest data:");

    ret = rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA1, SL_SHA1_LEN, sha_digest, simlock_data->sign);
#elif defined(SL_RSA2048_C)
    sha2((uint8_t *)simlock_data, sizeof(simlock_cust_data_t) + sizeof(simlock_encrypt_keys_t), sha_digest, 0);

    OSI_LOGI(0, "SIMLOCK: digest data:");

    ret = rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA256, SL_SHA256_LEN, sha_digest, simlock_data->sign);
#endif

    if (ret)
    {
        OSI_LOGI(0, "SIMLOCK: verify failed! err:%d\n", ret);
        goto failed_handle;
    }
    //get the uid
    if (!simlockGetUid(uid))
    {
        OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
        goto failed_handle;
    }
    sl_user_data.dynamic_data.simlock_status = 0xFF;
    sha2((uint8_t *)&sl_user_data.dynamic_data, sizeof(sl_user_data.dynamic_data), sha256_data, 0);

    //encrypt user data with AES
    if (!simlockAesEncrypt(sha256_data, SL_AES_USER_LEN, user_data_sign, uid, SL_SHA256_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: signature encrypt failed!\n");
        goto failed_handle;
    }
    if (!simlockAesEncrypt((uint8_t *)sl_user_data.dummy, 16, dummy_sign, uid, SL_SHA256_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: signature encrypt failed!\n");
        goto failed_handle;
    }
    if (nvmReadItem(NVID_SIM_LOCK_USER_DATA, (uint8_t *)user_data, sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0)
    {
        OSI_LOGI(0, "SIMLOCK: get user data from nv failed\n");
        goto failed_handle;
    }
    memcpy((void *)user_data, (void *)&sl_user_data.dynamic_data, sizeof(sl_user_data.dynamic_data));
    if (nvmWriteItem(NVID_SIM_LOCK_USER_DATA, (void *)user_data, sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0)
    {
        OSI_LOGI(0, "SIMLOCK: write user data to nv failed\n");
        goto failed_handle;
    }
    if (SIMLOCK_SUCCESS != simlockWriteEncryptUserData(user_data_sign))
    {
        OSI_LOGI(0, "SIMLOCK: write encrypted dynamic user data failed!");
        goto failed_handle;
    }
    //write dynamic password that is encrpted dummy
    if (SIMLOCK_SUCCESS != simlockWriteDummy(dummy_sign, 16))
    {
        OSI_LOGI(0, "SIMLOCK: write encrypted dummy failed!");
        goto failed_handle;
    }
#ifdef SIMLOCK_PROTECT_DATA
    // write protect data
    sl_protect = Simlock_InitProtectData();
    if (PNULL == sl_protect)
    {
        OSI_LOGI(0, "SIMLOCK: init protect data failed!");
        goto failed_handle;
    }
    if (SIMLOCK_SUCCESS != Simlock_WriteProtectData((uint8_t *)sl_protect))
    {
        OSI_LOGI(0, "SIMLOCK: write protect data failed!");
        goto failed_handle;
    }
    if (SIMLOCK_SUCCESS != Simlock_InitRFProtectData(rf_restore))
    {
        OSI_LOGI(0, "SIMLOCK: init rf revert data failed!");
        goto failed_handle;
    }
    if (SIMLOCK_SUCCESS != Simlock_WriteRFRestoreData(rf_restore))
    {
        OSI_LOGI(0, "SIMLOCK: write rf data for revert failed!");
        goto failed_handle;
    }
#endif
    // write data
    if (SIMLOCK_SUCCESS != simlockWriteDataAndSize((uint8_t *)data_buf, (sizeof(simlock_data_t) + /*data_size*/ 4)))
    {
        OSI_LOGI(0, "SIMLOCK: write data failed!");
        goto failed_handle;
    }

    if (data_buf)
    {
        free(data_buf);
    }
    if (user_data)
    {
        free(user_data);
    }
#ifdef SIMLOCK_PROTECT_DATA
    if (rf_restore)
    {
        free(rf_restore);
    }
#endif
    return 1;
failed_handle:
    if (data_buf)
    {
        free(data_buf);
    }
    if (user_data)
    {
        free(user_data);
    }
#ifdef SIMLOCK_PROTECT_DATA
    if (rf_restore)
    {
        free(rf_restore);
    }
#endif
    return 0;
}

/*****************************************************************************/
// Description :   tools will get the hash of all simlock data for signature, device calculate the data hash
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t srvSimlockDdataSha(uint8_t *hash)
{
    if (!s_identifypass)
    {
        OSI_LOGI(0, "SIMLOCK:  oper is not authorization");
        return 0;
    }

#if defined(SL_RSA1024_C)
    if (SIMLOCK_SUCCESS != simlockSHA(RSA_SHA1, SL_SHA1_LEN, hash))
    {
        OSI_LOGI(0, "SIMLOCK: get simlock data hash failed");
        return 0;
    }
#elif defined(SL_RSA2048_C)
    if (SIMLOCK_SUCCESS != simlockSHA(RSA_SHA256, SL_SHA256_LEN, hash))
    {
        OSI_LOGI(0, "SIMLOCK: get simlock data hash failed");
        return 0;
    }
#endif

    return 1;
}

/*****************************************************************************/
// Description tools write the rsa signature to device
// the process include the rsa verify before updating the signature;  then update the encrypted signature
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t srvSimlockSignData(const uint8_t *signed_data, uint32_t len)
{
    rsa_context rsa_ctx;
#if defined(SL_RSA1024_C)
    uint8_t sha_digest[SL_SHA1_LEN] = {0};
#elif defined(SL_RSA2048_C)
    uint8_t sha_digest[SL_SHA256_LEN] = {0};
#endif
    //    uint8_t sha1_digest[20];
    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t aes_sign[SL_RSA_SIGN_LEN] = {0};
    int ret = 0;
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
    simlock_protect_data_t sl_protect;
#endif

    if (!s_identifypass)
    {
        OSI_LOGI(0, "SIMLOCK:  oper is not authorization");
        return 0;
    }

    if (len != (SL_RSA_N_H_LEN) || !signed_data)
    {
        OSI_LOGI(0, "SIMLOCK: para err,len:%d !\n", len);
        return 0;
    }
    if (!simlockGetRsaPubkey(&rsa_ctx))
    {
        OSI_LOGI(0, "SIMLOCK: get rsa pubkey failed !\n");
        return 0;
    }
    if (!srvSimlockDdataSha(sha_digest))
    {
        OSI_LOGI(0, "SIMLOCK: simlock sha1 calculate failed !\n");
        return 0;
        ;
    }
    OSI_LOGI(0, "SIMLOCK: digest data:");
    //simlock_dump(sha1_digest, 20);

#if defined(SL_RSA1024_C)
    ret = rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA1, SL_SHA1_LEN, sha_digest, signed_data);
#elif defined(SL_RSA2048_C)
    ret = rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA256, SL_SHA256_LEN, sha_digest, (unsigned char *)signed_data);
#endif
    if (ret)
    {
        OSI_LOGI(0, "SIMLOCK: verify failed! err:%d\n", ret);
        return 0;
    }
    if (!simlockGetUid(uid))
    {
        OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
        return 0;
    }
    if (!simlockAesEncrypt(signed_data, SL_RSA_SIGN_LEN, aes_sign, uid, SL_SHA256_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: signature encrypt failed!\n");
        return 0;
    }
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
    Simlock_ReadProtectData((uint8_t *)&sl_protect);
    memcpy((void *)sl_protect.debug, (void *)sha_digest, 20); //only display 20 byte for sha1
    Simlock_WriteProtectData((uint8_t *)&sl_protect);
#endif
    if (SIMLOCK_SUCCESS != simlockWriteDataSignature(aes_sign))
    {
        OSI_LOGI(0, "SIMLOCK: write simlock data signature failed!\n");
        return 0;
    }
    s_identifypass = 0;
    return 1;
}

bool srvSimlockGetLocksStatus(srvSimlockType_t lock_type)
{
    MN_SIM_LOCK_USER_DATA_T nv_user_data;

    if (nvmReadItem(NVID_SIM_LOCK_USER_DATA, (uint8_t *)&nv_user_data,
                    sizeof(MN_SIM_LOCK_USER_DATA_T) < 0))
    {
        OSI_LOGI(0, "SIMLOCK: get user data from nv failed\n");
        return false;
    }
    if (nv_user_data.SIM_lock_status & lock_type)
        return true;
    else
        return false;
}

void srvSimlockGetLocksData(srvSimlockData_t *simlock_data)
{
    uint32_t data_length = 0x00;
    simlock_data_t *data_buf = NULL;

    if (simlock_data == NULL)
    {
        OSI_LOGI(0, "SIMLOCK: srvSimlockGetLocksData check data buf error");
        return;
    }

    if (!simlockDataVerify())
        return;

    simlockReadDataSize(&data_length);
    data_buf = (simlock_data_t *)malloc(sizeof(simlock_data_t));
    if (!data_buf)
    {
        OSI_LOGE(0, "SIMLOCK: allocate data_buf fail");
        return;
    }

    if (SIMLOCK_SUCCESS == simlockReadData((uint8_t *)data_buf, data_length))
    {
        simlock_data->SIM_lock_status = data_buf->cust_data.simlock_status;
        simlock_data->max_num_trials = data_buf->cust_data.max_num_trials;
        memcpy((void *)&simlock_data->network_locks, (void *)&data_buf->cust_data.network_locks, sizeof(simlock_data->network_locks));
        memcpy((void *)&simlock_data->network_subset_locks, (void *)&data_buf->cust_data.network_subset_locks, sizeof(simlock_data->network_subset_locks));
        memcpy((void *)&simlock_data->SP_locks, (void *)&data_buf->cust_data.sp_locks, sizeof(simlock_data->SP_locks));
        memcpy((void *)&simlock_data->corporate_locks, (void *)&data_buf->cust_data.corporate_locks, sizeof(simlock_data->corporate_locks));
        memcpy((void *)&simlock_data->user_locks, (void *)&data_buf->cust_data.user_locks, sizeof(simlock_data->user_locks));
    }
    else
    {
        OSI_LOGI(0, "SIMLOCK: srvSimlockGetLocksData get data error");
    }

    free(data_buf);
}

bool srvSimlockUnLocks(srvSimlockType_t lock_type, srvSimlockKey_t *input_key)
{
    uint32_t max_num_trials;
    bool ret = false;

    MN_SIM_LOCK_USER_DATA_T nv_user_data = {};
    srvSimlockData_t *simlock_data = (srvSimlockData_t *)malloc(sizeof(*simlock_data));
    if (!simlock_data)
        return false;

    srvSimlockGetLocksData(simlock_data);
    max_num_trials = simlock_data->max_num_trials;

    if ((nvmReadItem(NVID_SIM_LOCK_USER_DATA, (uint8_t *)&nv_user_data,
                     sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0))
    {
        OSI_LOGI(0, "SIMLOCK: read data from nv error\n");
        goto failed_handle;
    }

    switch (lock_type)
    {
    case MN_SIM_NETWORK_LOCK:
        if (nv_user_data.nck_trials > max_num_trials)
            goto failed_handle;
        break;
    case MN_SIM_NETWORK_SUBSET_LOCK:
        if (nv_user_data.nsck_trials > max_num_trials)
            goto failed_handle;
        break;
    case MN_SIM_SP_LOCK:
        if (nv_user_data.spck_trials > max_num_trials)
            goto failed_handle;
        break;
    case MN_SIM_CORPORATE_LOCK:
        if (nv_user_data.cck_trials > max_num_trials)
            goto failed_handle;
        break;
    case MN_SIM_USER_LOCK:
        if (nv_user_data.pck_trials > max_num_trials)
            goto failed_handle;
        break;
    default:
        OSI_LOGI(0, "SIMLOCK: not support this simlock type\n");
        goto failed_handle;
    }

    if (!simlockCheck(lock_type, input_key))
    {
        switch (lock_type)
        {
        case MN_SIM_NETWORK_LOCK:
            nv_user_data.nck_trials = nv_user_data.nck_trials + 1;
            break;
        case MN_SIM_NETWORK_SUBSET_LOCK:
            nv_user_data.nsck_trials = nv_user_data.nsck_trials + 1;
            break;
        case MN_SIM_SP_LOCK:
            nv_user_data.spck_trials = nv_user_data.spck_trials + 1;
            break;
        case MN_SIM_CORPORATE_LOCK:
            nv_user_data.cck_trials = nv_user_data.cck_trials + 1;
            break;
        case MN_SIM_USER_LOCK:
            nv_user_data.pck_trials = nv_user_data.pck_trials + 1;
            break;
        default:
            OSI_LOGI(0, "SIMLOCK: not support this simlock type\n");
            break;
        }
        ret = false;
    }
    else
    {
        switch (lock_type)
        {
        case MN_SIM_NETWORK_LOCK:
            nv_user_data.nck_trials = 0;
            break;
        case MN_SIM_NETWORK_SUBSET_LOCK:
            nv_user_data.nsck_trials = 0;
            break;
        case MN_SIM_SP_LOCK:
            nv_user_data.spck_trials = 0;
            break;
        case MN_SIM_CORPORATE_LOCK:
            nv_user_data.cck_trials = 0;
            break;
        case MN_SIM_USER_LOCK:
            nv_user_data.pck_trials = 0;
            break;
        default:
            OSI_LOGI(0, "SIMLOCK: not support this simlock type\n");
            break;
        }
        ret = true;
    }

    if ((nvmWriteItem(NVID_SIM_LOCK_USER_DATA, (uint8_t *)&nv_user_data,
                      sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0))
    {
        OSI_LOGI(0, "SIMLOCK: write user data to nv fail\n");
        ret = false;
    }

failed_handle:
    free(simlock_data);
    return ret;
}

void srvSimlockInit()
{
    simlockStorageInit();
}
