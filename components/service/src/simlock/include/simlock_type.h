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
#ifndef _MN_TYPE_H
#define _MN_TYPE_H

#include <stdint.h>
#include "osi_api.h"

#define MN_MAX_IMSI_ARR_LEN 8
#define MN_MAX_MOBILE_ID_LENGTH 9
#define MN_MAX_IMEI_LENGTH (MN_MAX_MOBILE_ID_LENGTH - 1)

#define SIM_LOCK_KEY_MAX_LEN 16
#ifdef GSM_CUSTOMER_AFP_SUPPORT
#define SIM_LOCK_OTHER_KEY_MIN_LEN 20 // the length of encryped key
#else
#define SIM_LOCK_OTHER_KEY_MIN_LEN 8 // NCK,NSCK,SPCK,CCK: 8 to 16 digits, raw key
#endif

#ifdef GSM_CUSTOMER_AFP_SUPPORT
#define MAX_PERSONALISATIONS 16
#else
#define MAX_PERSONALISATIONS 40
#endif

#define SIM_LOCK_KEY_LEN (SIM_LOCK_KEY_MAX_LEN > SIM_LOCK_OTHER_KEY_MIN_LEN ? SIM_LOCK_KEY_MAX_LEN : SIM_LOCK_OTHER_KEY_MIN_LEN)

#define SIM_LOCK_ENCRYPTED_KEY_MAX_LEN 32 /* MAX length of control key in encrypted data format */

typedef enum
{
    MN_SIM_LOCK_NONE = 0,
    MN_SIM_NETWORK_LOCK = 0x1,
    MN_SIM_NETWORK_SUBSET_LOCK = 0x10,
    MN_SIM_SP_LOCK = 0x100,
    MN_SIM_CORPORATE_LOCK = 0x1000,
    MN_SIM_USER_LOCK = 0x10000,
    MN_SIM_ALL_LOCK = 0x11111,
} srvSimlockType_t;

typedef struct _MN_PLMN_T
{
    uint16_t mcc;
    uint16_t mnc;
    uint16_t mnc_digit_num;
} MN_PLMN_T;

typedef struct _MN_IMSI_T
{
    uint8_t imsi_len;
    uint8_t imsi_val[MN_MAX_IMSI_ARR_LEN];
} MN_IMSI_T;

typedef uint8_t MN_IMEI_T[MN_MAX_IMEI_LENGTH];

typedef enum _MN_PHONE_COMPANY_ID_E
{
    MN_PHONE_COMPANY_UNKNOWN = 0x00,
    MN_PHONE_COMPANY_ORANGE,
    MN_PHONE_COMPANY_INVALID = 0xFFFF
} MN_PHONE_COMPANY_ID_E;

typedef struct _SIM_LOCK_KEY_T
{
    uint8_t key_len; // PCK: 6 to 16 digits; NCK,NSCK,SPCK,CCK: 8 to 16 digits
    uint8_t keys[SIM_LOCK_KEY_LEN];
} SIM_LOCK_KEY_T;

typedef struct _MN_SIM_CONTROL_KEYS_T
{
    SIM_LOCK_KEY_T nck;
    SIM_LOCK_KEY_T nsck;
    SIM_LOCK_KEY_T spck;
    SIM_LOCK_KEY_T cck;
    SIM_LOCK_KEY_T pck;
} MN_SIM_CONTROL_KEYS_T;

typedef struct _NETWORK_LOCKS_T
{
    uint8_t numLocks;
    uint8_t reserved; // Byte alignment
    MN_PLMN_T locks[MAX_PERSONALISATIONS];
} NETWORK_LOCKS_T;

typedef struct _NETWORK_SUBSET_LOCK_DATA_T
{
    MN_PLMN_T plmn;
    uint8_t network_subset[2]; // IMSI digits 6 and 7
} NETWORK_SUBSET_LOCK_DATA_T;

typedef struct _NETWORK_SUBSET_LOCKS_T
{
    uint8_t numLocks;
    uint8_t reserved; // Byte alignment
    NETWORK_SUBSET_LOCK_DATA_T locks[MAX_PERSONALISATIONS];
} NETWORK_SUBSET_LOCKS_T;

typedef struct _SP_LOCK_DATA_T
{
    MN_PLMN_T plmn;
    uint8_t sp;       // GID1 1 byte
    uint8_t reserved; // Byte alignment
} SP_LOCK_DATA_T;

typedef struct _SP_LOCKS_T
{
    uint8_t numLocks;
    uint8_t reserved; // Byte alignment
    SP_LOCK_DATA_T locks[MAX_PERSONALISATIONS];
} SP_LOCKS_T;

typedef struct _CORPORATE_LOCK_DATA_T
{
    MN_PLMN_T plmn;
    uint8_t sp;        // GID1 1 byte
    uint8_t corporate; // GID2 1 byte
} CORPORATE_LOCK_DATA_T;

typedef struct _CORPORATE_LOCKS_T
{
    uint8_t numLocks;
    uint8_t reserved; // Byte alignment
    CORPORATE_LOCK_DATA_T locks[MAX_PERSONALISATIONS];
} CORPORATE_LOCKS_T;

typedef MN_IMSI_T USER_LOCK_DATA_T;

typedef struct _USER_LOCKS_T
{
    uint8_t numLocks;
    USER_LOCK_DATA_T locks[MAX_PERSONALISATIONS];
} USER_LOCKS_T;

typedef struct _USER_LOCKS2_T
{
    uint8_t numLocks;
    USER_LOCK_DATA_T locks[3];
} USER_LOCKS2_T;

typedef struct _MN_SIM_LOCK_CUSTOMIZE_DATA_T
{
    MN_PHONE_COMPANY_ID_E company_id;
    uint32_t SIM_lock_status;
    uint32_t max_num_trials;
    uint32_t time_interval;
    uint32_t time_multiplier;
    NETWORK_LOCKS_T network_locks;
    NETWORK_SUBSET_LOCKS_T network_subset_locks;
    SP_LOCKS_T SP_locks;
    CORPORATE_LOCKS_T corporate_locks;
    uint32_t dummy1; //reserve for future
    uint32_t dummy2; //reserve for future
    uint32_t dummy3; //reserve for future
    uint32_t dummy4; //reserve for future
    uint32_t encrypted_CRC[4];
} MN_SIM_LOCK_CUSTOMIZE_DATA_T;

typedef struct _MN_SIM_LOCK_USER_DATA_T
{
    uint32_t SIM_lock_status;
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
    uint32_t dummy1; //reserve for future
    uint32_t dummy2; //reserve for future
    uint32_t dummy3; //reserve for future
    uint32_t dummy4; //reserve for future
    USER_LOCKS2_T user_locks;
    uint32_t encrypted_CRC[4];
} MN_SIM_LOCK_USER_DATA_T;

typedef struct MN_SIM_LOCK_CONTROL_KEY_T
{
    uint32_t simlock_support;
    uint32_t oper_mode;
    uint32_t control_key_type;
    MN_SIM_CONTROL_KEYS_T unlock_keys;
    MN_SIM_CONTROL_KEYS_T lock_keys;
    uint32_t encrypted_CRC[4];
} MN_SIM_LOCK_CONTROL_KEY_T;
#endif /* _MN_TYPE_H */
