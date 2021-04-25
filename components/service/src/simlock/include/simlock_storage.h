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

#ifndef SIMLOCK_STORAGE_H
#define SIMLOCK_STORAGE_H

#include "simlock_type.h"

#ifdef SIMLOCK_PROTECT_DATA
#define SIM_LOCK_PART_SIZE (30 * 1024)
#else
#define SIM_LOCK_PART_SIZE (6 * 1024)
#endif

#define SIMLOCK_STORAGE_DUMMY_LEN (32)

// RSA public key information
//#define SL_RSA1024_C
#define SL_RSA2048_C

#if defined(SL_RSA1024_C)
#define SL_KEY_LEN (128)
#elif defined(SL_RSA2048_C)
#define SL_KEY_LEN (256)
#endif

#define SL_RSA_N_LEN (SL_KEY_LEN << 1)
#define SL_RSA_E_LEN (6)
#define SL_RSA_N_H_LEN (SL_RSA_N_LEN >> 1)
#define SL_RSA_PK_LEN (SL_RSA_N_LEN + SL_RSA_E_LEN)

#define SL_RSA_SIGN_LEN (SL_KEY_LEN)
#define SL_RSA_KEY_LEN (SL_KEY_LEN)

#define SIMLOCK_DATA_MAX_LEN (5 * 1024)

#ifdef SIMLOCK_PROTECT_DATA
#define SL_RESTORE_RF_LEN (24 * 1024) // equal to NVITEM_MAX_SIZE
#endif

typedef struct
{
    uint8_t n[SL_RSA_N_LEN];
    uint8_t e[SL_RSA_E_LEN];
    uint8_t reserved[2];
} PUBLIC_KEY;

typedef struct
{
    uint8_t storage_magic[4];
    uint16_t secure_version;
    uint16_t config_version;
} SIMLOCK_HEADER;

// Simlock key information
typedef struct
{
    uint8_t nck[SIM_LOCK_KEY_MAX_LEN];
    uint8_t nsck[SIM_LOCK_KEY_MAX_LEN];
    uint8_t spck[SIM_LOCK_KEY_MAX_LEN];
    uint8_t cck[SIM_LOCK_KEY_MAX_LEN];
    uint8_t pck[SIM_LOCK_KEY_MAX_LEN];
} simlock_keyinfo_t;

typedef struct
{
    uint8_t nck[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN];
    uint8_t nsck[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN];
    uint8_t spck[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN];
    uint8_t cck[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN];
    uint8_t pck[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN];
} simlock_encrypt_keyinfo_t;

typedef struct
{
    simlock_keyinfo_t pin_keys;
    simlock_keyinfo_t puk_keys;
} simlock_keys_t;

typedef struct
{
    simlock_encrypt_keyinfo_t pin_keys;
    simlock_encrypt_keyinfo_t puk_keys;
} simlock_encrypt_keys_t;

#ifdef SIMLOCK_PROTECT_DATA
typedef struct
{
    BT_NV_PARAM bt_addr; // 8
    uint16_t rf_crc;     // 2
    uint8_t IMEI1[8];    // BCD 8
    uint8_t IMEI2[8];    // BCD 8
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
    uint8_t rf_debug[8]; // 2
    uint8_t debug[126];
#else
    uint8_t reseve[134];
#endif
} simlock_protect_data_t; // 160 byte
#endif

typedef struct
{
    SIMLOCK_HEADER HEADER;
    PUBLIC_KEY SIMLOCK_PUBLICKEY;
    uint8_t SIGNATURE[SL_RSA_SIGN_LEN]; /*aes encrypted RSA SIMLOCK signature*/
    simlock_encrypt_keys_t ENCRYPTED_KEYS;
    uint8_t ENCRYPTED_DUMMY[SIMLOCK_STORAGE_DUMMY_LEN]; /*AES encrypted of customise "password"  with the structure: size+encrypted password*/
    uint8_t ENCRYPTED_USER_DATA[32];                    //other encrypted customise config should follow the encrypted dummy when need to be added.
    uint32_t SIMLOCK_DATA_SIZE;
    uint8_t SIMLOCK_DATA[SIMLOCK_DATA_MAX_LEN];
#ifdef SIMLOCK_PROTECT_DATA
    simlock_protect_data_t PROTECT_DATA;
    uint8_t RF_REVERT_DATA[SL_RESTORE_RF_LEN];
#endif
} SIMLOCK_STORAGE_STR;

#define STOR_OFFS(stor_id) \
    ((uint32_t)((char *)&((SIMLOCK_STORAGE_STR *)0)->stor_id - (char *)0))
#define STOR_SIZE(stor_id) \
    sizeof(((SIMLOCK_STORAGE_STR *)0)->stor_id)

#define simlock_read(stor_id, buffer, offset, length) \
    simlockStorageRead(buffer, STOR_OFFS(stor_id),    \
                       STOR_SIZE(stor_id), offset, length)
#define simlock_write(stor_id, buffer, offset, length) \
    simlockStorageWrite(buffer, STOR_OFFS(stor_id),    \
                        STOR_SIZE(stor_id), offset, length)

/**
 * @brief Read the simlock data
 * @param buffer Store the output data read from simlock partition.
 * @param stor_offset The field offset storage.
 * @param stor_length The filed lenght.
 * @param offset The offset in the field.
 * @param length The length read from the field.
 * @return:
 *      - true Read successfully.
 *      - false Fail to read.
 */
bool simlockStorageRead(void *buffer,
                        uint32_t stor_offset, uint32_t stor_length,
                        uint32_t offset, uint32_t length);
/**
 * @brief Write the simlock data
 * @param buffer Store the input data write to simlock partition.
 * @param stor_offset The field offset storage.
 * @param stor_length The filed lenght.
 * @param offset The offset in the field.
 * @param length The length write to the field.
 * @return:
 *      - true Read successfully.
 *      - false Fail to read.
 */
bool simlockStorageWrite(void *buffer,
                         uint32_t stor_offset, uint32_t stor_length,
                         uint32_t offset, uint32_t length);

/**
 * @brief Dump the buffer data
 * @param buffer Store the dump data
 * @param len The length of dump data
 * @return:
 */
void simlockDump(uint8_t *pbuf, uint32_t len);
/**
 * @brief Init the storage of simlock
 * @return:
 */
bool simlockStorageInit(void);
#endif
