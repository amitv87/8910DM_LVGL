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

#include "srv_config.h"
#include "osi_api.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <osi_log.h>
#include "simlock.h"
#include "simlock_storage.h"
#include "simlock_aescrypt.h"
#include "sha1.h"
#include "sha2.h"
#include "rsa.h"
#include "srv_simlock.h"
#include "nvm.h"
#include "drv_efuse.h"

// SHA information
#define SL_SHA256_LEN (32)

// AES information
#define SL_AES_USER_LEN (32)

#ifdef SIMLOCK_PROTECT_DATA
simlock_protect_data_t s_sl_protect_data = {0};
static uint32_t s_protect_data_err_info = 0;
#endif

/* just test add start*/
typedef struct
{
    uint8_t multips_create_set;
    uint32_t sim_slot_map;
    uint32_t reserve0;
    uint32_t reserve1;
    uint32_t reserve2;
    uint32_t reserve3; //the bit0 means whether the AP side check FDN,          0 -- FALSE, 1 -- TRUE
                       //the bit1 means whether the SCA num needs to check FDN, 0 -- TRUE, 1 -- FALSE
} MULTIPS_CREATE_PARAM_T;
/* just test add end*/

SIMLOCK_ERROR_TYPE simlockReadPublicKey(PUBLIC_KEY *public_key)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    if (NULL == public_key)
    {
        return SIMLOCK_ERROR;
    }
    result = simlock_read(SIMLOCK_PUBLICKEY, (void *)public_key, 0, sizeof(PUBLIC_KEY));
    if (!result)
    {
        ret = SIMLOCK_READ_PUBLICKEY_ERROR;
    }
    return ret;
}

SIMLOCK_ERROR_TYPE simlockWritePublicKey(PUBLIC_KEY *public_key, uint32_t size)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    if (NULL == public_key || size > sizeof(PUBLIC_KEY))
    {
        return SIMLOCK_ERROR;
    }
    result = simlock_write(SIMLOCK_PUBLICKEY, (void *)public_key, 0, size);
    if (!result)
    {
        ret = SIMLOCK_WRITE_PUBLICKEY_ERROR;
    }
    return ret;
}
SIMLOCK_ERROR_TYPE simlockReadheader(SIMLOCK_HEADER *simlock_header)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    OSI_ASSERT(NULL != simlock_header, "simlock_header invlid");
    simlock_read(HEADER, simlock_header, 0, sizeof(SIMLOCK_HEADER));
    if (0 != strncmp((char *)(simlock_header->storage_magic), "SIMK", strlen("SIMK")))
    {
        OSI_LOGI(0, "[SIM_LOCK]magic num error\n");
        ret = SIMLOCK_READ_HEADER_ERROR;
    }
    return ret;
}
/*****************************************************************************/
//  Description : Read simlock data size from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadDataSize(uint32_t *length)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != length, "simlockReadDataSize, length invalid");
    result = simlock_read(SIMLOCK_DATA_SIZE, length, 0, STOR_SIZE(SIMLOCK_DATA_SIZE));
    if ((!result) || (*length > 32 * 1024) || (*length == 0))
    {
        OSI_LOGI(0, "simlockReadDataSize read the data len=%x,result=%d", *length, result);
        ret = SIMLOCK_READ_DATASIZE_ERROR;
    }
    return ret;
}
SIMLOCK_ERROR_TYPE simlockReadData(uint8_t *buf, uint32_t length)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != buf, "simlockReadData buf invalid.");
    result = simlockStorageRead(buf, STOR_OFFS(SIMLOCK_DATA), length, 0, length);
    if (!result)
    {
        ret = SIMLOCK_READ_DATA_ERROR;
    }
    return ret;
}

#ifdef SIMLOCK_PROTECT_DATA
SIMLOCK_ERROR_TYPE simlockInitRFProtectData(uint8_t *rf_data)
{
    uint32_t rf_size = 0;
    NVITEM_ERROR_E nv_ret = NVERR_NONE;
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;

    /* get RF NV */
    rf_size = EFS_GetItemLength(NV_MODEM_RF_NV);

    ret = nvmReadItem(NV_MODEM_RF_NV, rf_data, rf_size);
    if (nv_ret != NVERR_NONE)
    {
        OSI_LOGI(0, "Simlock_InitRFProtectData NV_MODEM_RF_NV failed!\n");
        ret = SIMLOCK_PRE_RF_REVERT_ERROR;
    }
    return ret;
}

/*****************************************************************************/
// Description :
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
simlock_protect_data_t *simlockInitProtectData()
{
    uint32_t nv_size = 0;
    uint8_t *rf_data;
    NVITEM_ERROR_E ret = NVERR_NONE;

    s_protect_data_err_info = 0;

    /* get RF CRC */
    nv_size = EFS_GetItemLength(NV_MODEM_RF_NV);
    rf_data = malloc(SL_RESTORE_RF_LEN);
    if (!rf_data || (nv_size > SL_RESTORE_RF_LEN))
    {
        OSI_LOGI(0, "simlockInitProtectData RF malloc failed !\n");
        s_protect_data_err_info |= SIMLOCK_ERR_INIT_MALLOC_RF;
    }
    else
    {
        memset(rf_data, 0, SL_RESTORE_RF_LEN);
        ret = nvmReadItem(NV_MODEM_RF_NV, rf_data, nv_size);
        if (ret != NVERR_NONE)
        {
            OSI_LOGI(0, "simlockInitProtectData NV_MODEM_RF_NV failed !\n");
            s_protect_data_err_info |= SIMLOCK_ERR_INIT_RF;
        }
        else
        {
            s_sl_protect_data.rf_crc = crc16(0, rf_data, SL_RESTORE_RF_LEN);
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
            memcpy((uint8_t *)&s_sl_protect_data.rf_debug[0], (uint8_t *)rf_data, 8);
#endif
        }
        free(rf_data);
    }
    /* read BT MAC */
    nv_size = EFS_GetItemLength(NV_BT_CONFIG);
    ret = nvmReadItem(NV_BT_CONFIG, (uint8_t *)&(s_sl_protect_data.bt_addr), nv_size);
    if (ret != NVERR_NONE)
    {
        OSI_LOGI(0, "SIMLOCK: protect_data init BT failed !\n");
        s_protect_data_err_info |= SIMLOCK_ERR_INIT_BT;
    }

    /* read IMEI */
    ret = nvmReadItem(NV_IMEI, (uint8_t *)&s_sl_protect_data.IMEI1, 8);
    if (ret != NVERR_NONE)
    {
        OSI_LOGI(0, "simlockInitProtectData NV_IMEI failed !\n");
        s_protect_data_err_info |= SIMLOCK_ERR_INIT_IMEI;
    }
#ifdef MULTI_SIM_SYS_DUAL
    ret = nvmReadItem(NV_IMEI1, (uint8_t *)&s_sl_protect_data.IMEI2, 8);
    if (ret != NVERR_NONE)
    {
        OSI_LOGI(0, "simlockInitProtectData NV_IMEI2 failed !\n");
        s_protect_data_err_info |= SIMLOCK_ERR_INIT_IMEI2;
    }
#else
    memset(s_sl_protect_data.IMEI2, 0, 8);
#endif

    if ((SIMLOCK_ERR_INIT_ERR & s_protect_data_err_info) != 0)
    {
        return NULL;
    }
    return &s_sl_protect_data;
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
void simlockGetCurProtectData(simlock_protect_data_t *buf)
{
    memcpy(buf, &s_sl_protect_data, sizeof(simlock_protect_data_t));
}

/*****************************************************************************/
//  Description : Read the designed simlock protect dada from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadProtectData(uint8_t *buf)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != buf, "Simlock_ReadProtectData, buf invalid.");
    result = simlock_read(PROTECT_DATA, (void *)buf, 0, STOR_SIZE(PROTECT_DATA));
    if (!result)
    {
        ret = SIMLOCK_READ_PROTECT_ERROR;
        s_protect_data_err_info |= SIMLOCK_ERR_READ_PRO;
    }
    return ret;
}

/*****************************************************************************/
//  Description : Append protect data into flash
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockWriteProtectData(uint8_t *buf)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != buf, "Simlock_WriteProtectData, buf invalid.");
    result = simlock_write(PROTECT_DATA, (void *)buf, 0, STOR_SIZE(PROTECT_DATA));
    if (!result)
    {
        ret = SIMLOCK_WRITE_PROTECT_ERROR;
        s_protect_data_err_info |= SIMLOCK_ERR_WRITE_PRO;
    }
    return ret;
}

/*****************************************************************************/
//  Description : Read the designed simlock protect dada from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadRFRestoreData(uint8_t *buf)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != buf, "Simlock_ReadRFRestoreData, buf invalid.");
    result = simlock_read(RF_REVERT_DATA, (void *)buf, 0, STOR_SIZE(RF_REVERT_DATA));
    if (!result)
    {
        ret = SIMLOCK_READ_RF_REVERT_ERROR;
        s_protect_data_err_info |= SIMLOCK_ERR_READ_REV;
    }
    return ret;
}

/*****************************************************************************/
//  Description : Append protect data into flash
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockWriteRFRestoreData(uint8_t *buf)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != buf, "Simlock_WriteRFRestoreData, buf invalid.");
    result = simlock_write(RF_REVERT_DATA, (void *)buf, 0, STOR_SIZE(RF_REVERT_DATA));
    if (!result)
    {
        ret = SIMLOCK_WRITE_RF_REVERT_ERROR;
        s_protect_data_err_info |= SIMLOCK_ERR_WRITE_REV;
    }
    return ret;
}

/*****************************************************************************/
// Description :   .
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
uint32_t Simlock_GetProtectDataErrInfo(uint32_t *buf)
{
    *buf = s_protect_data_err_info;
    return s_protect_data_err_info;
}

/*****************************************************************************/
// Description : restore the protected NV data, if the NV data was modified.
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockRestoreProtectedNV()
{
    uint16_t nv_len = 0;
    simlock_protect_data_t flash_data = {0};
    simlock_protect_data_t unwriten_data = {0xFF};
    simlock_protect_data_t *sl_protect = NULL;
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;

    sl_protect = simlockInitProtectData();
    if (NULL == sl_protect)
    {
        return SIMLOCK_READ_PROTECT_NV_ERROR;
    }
    ret = Simlock_ReadProtectData(&flash_data);
    if (SIMLOCK_SUCCESS != ret)
    {
        return ret;
    }

    if (memcpy(&unwriten_data, &flash_data, sizeof(simlock_protect_data_t)) == 0)
    { // Simlock data is not writen, return fail
        s_protect_data_err_info |= (SIMLOCK_ERR_RESTORE_BT | SIMLOCK_ERR_RESTORE_SIM);
        return SIMLOCK_PROTECT_NV_ERROR;
    }

    /* BT */
    if (memcpy(&(sl_protect->bt_addr), &(flash_data.bt_addr), sizeof(flash_data.bt_addr)) != 0)
    {
        nv_len = NVITEM_GetLength(NV_BT_CONFIG);
        if (NVERR_NONE != EFS_NvitemWrite(NV_BT_CONFIG, nv_len, &(flash_data.bt_addr), true))
        {
            s_protect_data_err_info |= SIMLOCK_ERR_RESTORE_BT;
            ret = SIMLOCK_PROTECT_NV_ERROR;
        }
    }
    /* IMEI */
    if (memcpy(sl_protect->IMEI1, flash_data.IMEI1, sizeof(flash_data.IMEI1)) != 0)
    {
        if (NVERR_NONE != EFS_NvitemWrite(NV_IMEI, 8, flash_data.IMEI1, true))
        {
            s_protect_data_err_info |= SIMLOCK_ERR_RESTORE_IMEI1;
            ret = SIMLOCK_PROTECT_NV_ERROR;
        }
    }
    if (memcpy(sl_protect->IMEI2, flash_data.IMEI2, sizeof(flash_data.IMEI2)) != 0)
    {
        if (NVERR_NONE != EFS_NvitemWrite(NV_IMEI1, 8, flash_data.IMEI2, true))
        {
            s_protect_data_err_info |= SIMLOCK_ERR_RESTORE_IMEI2;
            ret = SIMLOCK_PROTECT_NV_ERROR;
        }
    }
    /* RF */
    if (sl_protect->rf_crc != flash_data.rf_crc)
    {
        uint8_t *rf_restore = NULL; //[SL_RESTORE_RF_LEN]
        rf_restore = SCI_ALLOCAZ(SL_RESTORE_RF_LEN);
        if (NULL == rf_restore)
        {
            s_protect_data_err_info |= SIMLOCK_ERR_RESTORE_RF_ALOC;
            return SIMLOCK_PROTECT_NV_ERROR;
        }
        if (SIMLOCK_SUCCESS != Simlock_ReadRFRestoreData(rf_restore))
        {
            free(rf_restore);
            return SIMLOCK_PROTECT_NV_ERROR;
        }
        nv_len = NVITEM_GetLength(NV_MODEM_RF_NV);
        if (NVERR_NONE != EFS_NvitemWrite(NV_MODEM_RF_NV, nv_len, rf_restore, true))
        {
            s_protect_data_err_info |= SIMLOCK_ERR_RESTORE_RF;
            ret = SIMLOCK_PROTECT_NV_ERROR;
        }
        free(rf_restore);
    }

    return ret;
}
#endif

/*****************************************************************************/
//  Description : Write the designed size of simlock dada to simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockWriteDataAndSize(uint8_t *buf, uint32_t size)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(size < 32 * 1024, "Simlock_WriteDataAndSize, size is invalid");
    result = simlockStorageWrite(buf, STOR_OFFS(SIMLOCK_DATA_SIZE), size, 0, size);
    if (!result)
    {
        ret = SIMLOCK_WRITE_DATA_ERROR;
    }
    return ret;
}
/*****************************************************************************/
//  Description : Write simlock encrypted dummy to simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockWriteDummy(uint8_t *dummy, uint32_t length)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT((NULL != dummy) && (length <= 32), "Simlock_WriteDummy, invalid");
    result = simlock_write(ENCRYPTED_DUMMY, dummy, 0, length);
    if (!result)
    {
        ret = SIMLOCK_WRITE_DUMMY_ERROR;
    }
    return ret;
}
/*****************************************************************************/
//  Description : Read simlock encrypted dummy from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadDummy(uint8_t *dummy)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != dummy, "simlockReadDummy, invalid");
    result = simlock_read(ENCRYPTED_DUMMY, dummy, 0, STOR_SIZE(ENCRYPTED_DUMMY));
    if (!result)
    {
        ret = SIMLOCK_READ_DUMMY_ERROR;
    }
    return ret;
}

SIMLOCK_ERROR_TYPE simlockWriteEncryptUserData(uint8_t *enc_data)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT((NULL != enc_data), "Simlock_WriteEncryptUserData, invalid");
    result = simlock_write(ENCRYPTED_USER_DATA, enc_data, 0, 32);
    if (!result)
    {
        ret = SIMLOCK_WRITE_USERDATA_ERROR;
    }
    return ret;
}
/*****************************************************************************/
//  Description : Read simlock encrypted dummy from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadEncryptUserData(uint8_t *enc_data)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT((NULL != enc_data), "simlockReadEncryptUserData, invalid");
    result = simlock_read(ENCRYPTED_USER_DATA, enc_data, 0, STOR_SIZE(ENCRYPTED_USER_DATA));
    if (!result)
    {
        ret = SIMLOCK_READ_USERDATA_ERROR;
    }
    return ret;
}

SIMLOCK_ERROR_TYPE simlockReadUserData(MN_SIM_LOCK_USER_DATA_T *simlock_user_data)
{
    uint32_t data_length;
    simlock_data_t *data_buf = NULL;
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_ERROR;

    OSI_ASSERT(NULL != simlock_user_data, "Simlock_ReadUserData, invalid");
    simlockReadDataSize(&data_length);
    data_buf = (simlock_data_t *)malloc(data_length);
    if (!data_buf)
    {
        return SIMLOCK_ERROR;
    }

    if (SIMLOCK_SUCCESS == simlockReadData((uint8_t *)data_buf, data_length))
    {
        memcpy((void *)&simlock_user_data->user_locks, (void *)&data_buf->cust_data.user_locks, sizeof(simlock_user_data->user_locks));

        OSI_LOGI(0, "the simlock user crc0=%x,crc1=%x,crc2=%x,crc3=%x", simlock_user_data->encrypted_CRC[0], simlock_user_data->encrypted_CRC[1],
                 simlock_user_data->encrypted_CRC[2], simlock_user_data->encrypted_CRC[3]);

        ret = SIMLOCK_SUCCESS;
    }
    else
    {
        ret = SIMLOCK_READ_DATA_ERROR;
    }

    if (NULL != data_buf)
    {
        free(data_buf);
    }

    return ret;
}
/*****************************************************************************/
//  Description : Read  control key data from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
SIMLOCK_ERROR_TYPE simlockReadControlkeyData(MN_SIM_LOCK_CONTROL_KEY_T *simlock_controlkey_data)
{
    uint32_t data_length;
    simlock_data_t *data_buf = NULL;
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_ERROR;

    OSI_ASSERT(NULL != simlock_controlkey_data, "simlock_controlkey_data invalid");
    simlockReadDataSize(&data_length);
    data_buf = (simlock_data_t *)malloc(data_length);
    if (!data_buf)
    {
        return SIMLOCK_ERROR;
    }
    if (SIMLOCK_SUCCESS == simlockReadData((uint8_t *)data_buf, data_length))
    {

        simlock_controlkey_data->simlock_support = data_buf->cust_data.simlock_support;
        simlock_controlkey_data->oper_mode = data_buf->cust_data.oper_mode;
        simlock_controlkey_data->control_key_type = data_buf->cust_data.ctrl_key_type;

        OSI_LOGI(0, "simlock_on_off=%x,opera_mode=%x,key_type=%x", simlock_controlkey_data->simlock_support, simlock_controlkey_data->oper_mode,
                 simlock_controlkey_data->control_key_type);

        ret = SIMLOCK_SUCCESS;
    }
    else
    {
        ret = SIMLOCK_READ_DATA_ERROR;
    }

    if (NULL != data_buf)
    {
        free(data_buf);
    }
    return ret;
}
/*****************************************************************************/
//  Description : Read encrypted signature, signature of simlock data (SIM_LOCK_CUSTOMISE_DATA+
//SIM_LOCK_USER_DATA+SIM_LOCK_CONTROL_KEY )from simlock partition
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
static SIMLOCK_ERROR_TYPE simlockReadDataSignature(uint8_t *signature)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != signature, "simlockReadDataSignature invalid");
    result = simlock_read(SIGNATURE, signature, 0, STOR_SIZE(SIGNATURE));
    if (!result)
    {
        ret = SIMLOCK_READ_SIGNATURE_ERROR;
    }
    return ret;
}

SIMLOCK_ERROR_TYPE simlockWriteDataSignature(uint8_t *signature)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != signature, "Simlock_WriteDataSignature, invalid");
    result = simlock_write(SIGNATURE, signature, 0, STOR_SIZE(SIGNATURE));
    if (!result)
    {
        ret = SIMLOCK_WRITE_SIGNATURE_ERROR;
    }
    return ret;
}
/*****************************************************************************/
//  Description : read encrypted keys (pin/puk simlock password)
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
static SIMLOCK_ERROR_TYPE prvSimlock_ReadKeys(uint8_t *keys)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != keys, "prvSimlock_ReadKeys invalid");
    result = simlock_read(ENCRYPTED_KEYS, keys, 0, STOR_SIZE(ENCRYPTED_KEYS));
    if (!result)
    {
        ret = SIMLOCK_READ_KEYS_ERROR;
    }
    return ret;
}

uint32_t simlockGetRsaPubkey(rsa_context *rsa_ctx_ptr)
{
    PUBLIC_KEY public_key = {0};
    char rsa_N[SL_RSA_N_LEN + 1] = {0};
    char rsa_E[SL_RSA_E_LEN + 1] = {0};

    //get the rsa signature
    if (SIMLOCK_SUCCESS != simlockReadPublicKey(&public_key))
    {
        OSI_LOGI(0, "SIMLOCK: get public key from simlock partition failed\n");
        return 0;
    }

    memcpy(rsa_N, public_key.n, SL_RSA_N_LEN);
    memcpy(rsa_E, public_key.e, SL_RSA_E_LEN);
    rsa_init(rsa_ctx_ptr, RSA_PKCS_V15, 0, NULL, NULL);
    rsa_ctx_ptr->len = SL_RSA_KEY_LEN;
    mpi_read_string(&rsa_ctx_ptr->N, 16, rsa_N);
    mpi_read_string(&rsa_ctx_ptr->E, 16, rsa_E);

    return 1;
}

SIMLOCK_ERROR_TYPE simlockWriteKeys(uint8_t *keys)
{
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;
    bool result;
    OSI_ASSERT(NULL != keys, "Simlock_WriteKeys invalid");
    result = simlock_write(ENCRYPTED_KEYS, keys, 0, STOR_SIZE(ENCRYPTED_KEYS));
    if (!result)
    {
        ret = SIMLOCK_WRITE_KEYS_ERROR;
    }
    return ret;
}

SIMLOCK_ERROR_TYPE simlockReadCustomiseData(MN_SIM_LOCK_CUSTOMIZE_DATA_T *simlock_customize_data)
{

    bool result;
    uint8_t password_len;
    uint8_t decrypted_dummy[SIMLOCK_STORAGE_DUMMY_LEN];
    uint8_t encrypted_dummy[SIMLOCK_STORAGE_DUMMY_LEN];
    uint8_t aes_key[SL_SHA256_LEN];
    uint32_t data_length;
    simlock_data_t *data_buf = NULL;
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_ERROR;

    OSI_ASSERT(NULL != simlock_customize_data, "simlock_customize_data invalid");

    if (SIMLOCK_SUCCESS == simlockReadDataSize(&data_length))
    {
        OSI_LOGI(0, "simlock data length=%4x", data_length);
        data_buf = (simlock_data_t *)malloc(data_length);
        if (!data_buf)
        {
            return SIMLOCK_ERROR;
        }
        if (SIMLOCK_SUCCESS == simlockReadData((uint8_t *)data_buf, data_length))
        {

            simlock_customize_data->SIM_lock_status = data_buf->cust_data.simlock_status;
            simlock_customize_data->max_num_trials = data_buf->cust_data.max_num_trials;
            memcpy((void *)&simlock_customize_data->network_locks, (void *)&data_buf->cust_data.network_locks, sizeof(simlock_customize_data->network_locks));
            memcpy((void *)&simlock_customize_data->network_subset_locks, (void *)&data_buf->cust_data.network_subset_locks, sizeof(simlock_customize_data->network_subset_locks));
            memcpy((void *)&simlock_customize_data->SP_locks, (void *)&data_buf->cust_data.sp_locks, sizeof(simlock_customize_data->SP_locks));
            memcpy((void *)&simlock_customize_data->corporate_locks, (void *)&data_buf->cust_data.corporate_locks, sizeof(simlock_customize_data->corporate_locks));
        }
        else
        {
            ret = SIMLOCK_READ_DATA_ERROR;
            goto SL_ReadCustomiseDataEnd;
        }
        //read encrypted dummy or customise password
        if (SIMLOCK_SUCCESS == simlockReadDummy(encrypted_dummy))
        {
            if (!simlockGetUid(aes_key))
            {
                ret = SIMLOCK_READ_UID_ERROR;
                goto SL_ReadCustomiseDataEnd;
            }
            result = simlockAesDecrypt(encrypted_dummy, SIMLOCK_STORAGE_DUMMY_LEN, decrypted_dummy, aes_key, SL_SHA256_LEN);
            if (!result)
            {
                OSI_LOGI(0, "[SIM_LOCK]simlock password decrypt failed\n");
                ret = SIMLOCK_DECRYPT_ERROR;
                goto SL_ReadCustomiseDataEnd;
            }
            else
            {
                password_len = (uint8_t)*decrypted_dummy;
                memcpy((uint8_t *)&(simlock_customize_data->dummy1), decrypted_dummy, password_len + 1); //size and data to customise_data_dummy1~3
                OSI_LOGI(0, "simlock password decrypt ok\n");
                ret = SIMLOCK_SUCCESS;
                goto SL_ReadCustomiseDataEnd;
            }
        }
        else
        {
            OSI_LOGI(0, "simlock read dummy fail\n");
            ret = SIMLOCK_READ_DUMMY_ERROR;
            goto SL_ReadCustomiseDataEnd;
        }
    }
    else
    {
        OSI_LOGI(0, "simlock data length read failed");
        ret = SIMLOCK_READ_DATASIZE_ERROR;
        goto SL_ReadCustomiseDataEnd;
    }

SL_ReadCustomiseDataEnd:
    if (data_buf)
    {
        free(data_buf);
    }

    return ret;
}

SIMLOCK_ERROR_TYPE simlockSHA(uint8_t hash_id, uint8_t hashlen, uint8_t *shasum)
{
    uint32_t data_length;
    uint8_t *data_buf = NULL;
    uint16_t dalta_len = 0;
    MULTIPS_CREATE_PARAM_T sim_cfg;
#ifdef SIMLOCK_PROTECT_DATA
    simlock_protect_data_t *sl_protect = NULL;
#endif
    SIMLOCK_ERROR_TYPE ret = SIMLOCK_SUCCESS;

    if (SIMLOCK_SUCCESS == simlockReadDataSize(&data_length))
    {
#ifdef SIMLOCK_PROTECT_DATA
        data_buf = malloc(data_length + 8                /*sizeof(IMEI)*/
                          + sizeof(sim_cfg.sim_slot_map) /*sim_slot_cfg*/
                          + STOR_SIZE(ENCRYPTED_DUMMY) + sizeof(simlock_encrypt_keys_t) + sizeof(simlock_protect_data_t));
#else
        data_buf = malloc(data_length + 8                /*sizeof(IMEI)*/
                          + sizeof(sim_cfg.sim_slot_map) /*sim_slot_cfg*/
                          + STOR_SIZE(ENCRYPTED_DUMMY) + sizeof(simlock_encrypt_keys_t));
#endif

        OSI_ASSERT(data_buf, "simlockSHA databuf invalid");
        //read config data
        if (SIMLOCK_SUCCESS == simlockReadData(data_buf + dalta_len, data_length))
        {
            dalta_len += data_length;

            //read encrypted dummy
            ret = simlockReadDummy(data_buf + dalta_len);
            if (ret != SIMLOCK_SUCCESS)
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_DUMM_GET;
#endif
                goto end;
            }
            dalta_len += STOR_SIZE(ENCRYPTED_DUMMY);
            //read imei
            ret = nvmReadItem(NVID_IMEI1, (data_buf + dalta_len), 8);
            if (ret < 0)
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_IMEI_GET;
#endif
                goto end;
            }
            dalta_len += 8 /*sizeof(IMEI)*/;
/* just delete this temp */
#if 0
            //read sim cfg
            if(NVERR_NONE != nvmReadItem(NV_SIM_CFG,sizeof(MULTIPS_CREATE_PARAM_T), (uint8_t*)(&sim_cfg)))
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_SIMC_GET;
#endif
                goto end;
            }
               memcpy((void*)(data_buf+dalta_len),(void*)&sim_cfg.sim_slot_map,sizeof(sim_cfg.sim_slot_map));
               //OSI_LOGI(0, "SIM CFG: SIM_SLOT_MAP=%x",*(uint32_t*)(data_buf+dalta_len));
               dalta_len+=sizeof(sim_cfg.sim_slot_map);
#endif
            //read keys
            ret = prvSimlock_ReadKeys((uint8_t *)(data_buf + dalta_len));
            if (ret != SIMLOCK_SUCCESS)
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_KEY_GET;
#endif
                goto end;
            }
            dalta_len += sizeof(simlock_encrypt_keys_t);

#ifdef SIMLOCK_PROTECT_DATA
            sl_protect = simlockInitProtectData();
            if (NULL == sl_protect)
            {
                OSI_LOGI(0, "SIMLOCK SHA: init protect data failed!");
                goto end;
            }
            memcpy((void *)(data_buf + dalta_len), (void *)sl_protect, sizeof(simlock_protect_data_t));
            dalta_len += sizeof(simlock_protect_data_t);
#endif

            switch (hash_id)
            {
            case RSA_SHA1:
                sha1(data_buf, dalta_len, shasum);
                break;
            case RSA_SHA256:
                sha2(data_buf, dalta_len, shasum, 0);
                break;
            default:
                goto end;
            }
        }
        else
        {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
            s_protect_data_err_info |= SIMLOCK_ERR_USER_GET;
#endif
            OSI_LOGI(0, "[%s][%d]simlock data failed\n", __FUNCTION__, __LINE__);
            ret = SIMLOCK_READ_DATA_ERROR;
        }
    }
    else
    {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
        s_protect_data_err_info |= SIMLOCK_ERR_SIZE_GET;
#endif
        OSI_LOGI(0, "[%s][%d]simlock data size failed\n", __FUNCTION__, __LINE__);
        ret = SIMLOCK_READ_DATASIZE_ERROR;
    }

end:
    if (data_buf)
    {
        free(data_buf);
    }
    return ret;
}

bool simlockDataVerify(void)
{

    uint8_t i, hash_id, hashlen;
    uint8_t encrypt_user_data[SL_AES_USER_LEN];
    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t rsa_decrypt_sign[SL_RSA_SIGN_LEN] = {0};
    uint8_t sha256_data[SL_SHA256_LEN] = {0};
    uint8_t user_aes[SL_AES_USER_LEN] = {0};
    uint8_t shasum[SL_SHA256_LEN] = {0};
    MN_SIM_LOCK_USER_DATA_T user_data;
    rsa_context rsa_ctx;
    uint8_t *signature = NULL;
    bool ret = false;

#if defined(SL_RSA1024_C)
    hash_id = RSA_SHA1;
    hashlen = SL_SHA1_LEN;
#elif defined(SL_RSA2048_C)
    hash_id = RSA_SHA256;
    hashlen = SL_SHA256_LEN;
#endif

    signature = malloc(SL_RSA_SIGN_LEN);

    if (signature == NULL)
    {
        OSI_LOGI(0, "Simlock_Data_Verify(): MALLOC FAIL.");
        ret = false;
        goto verify_end;
    }

    if (SIMLOCK_SUCCESS == simlockReadDataSignature(signature))
    {
        if ((SIMLOCK_SUCCESS == simlockSHA(hash_id, hashlen, shasum)) && (SIMLOCK_SUCCESS == simlockReadEncryptUserData(encrypt_user_data)))
        {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
            memcpy((void *)s_sl_protect_data.debug, (void *)shasum, 20); //only display first 20 byte for SHA1
#endif

            memset(&rsa_ctx, 0, sizeof(rsa_context));
            if (!simlockGetRsaPubkey(&rsa_ctx))
            {
                OSI_LOGI(0, "[%s][%d]simlock data read public key failed\n", __FUNCTION__, __LINE__);
                ret = false;
                goto verify_end;
            }
            // get the uid
            if (!simlockGetUid(uid))
            {
                OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
                ret = false;
                goto verify_end;
            }

            // dynamic user data verify
            if (nvmReadItem(NVID_SIM_LOCK_USER_DATA, &user_data,
                            sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0)
            {
                OSI_LOGI(0, "SIMLOCK: get user data from nv failed\n");
                ret = false;
                goto verify_end;
            }

            user_data.SIM_lock_status &= 0xff;
            sha2((unsigned char *)&(user_data), sizeof(simlock_dynamic_data_t), sha256_data, 0);

            //encrypt user data with AES
            if (!simlockAesEncrypt(sha256_data, SL_SHA256_LEN, user_aes, uid, SL_SHA256_LEN))
            {
                OSI_LOGI(0, "SIMLOCK: user data encrypt failed!\n");
                ret = false;
                goto verify_end;
            }

            for (i = 0; i < SL_AES_USER_LEN; i++)
            {
                //OSI_LOGI(0, "SIMLOCK: user_aes[%d]=%x, encrypt_user_data[%d]=%x\n",i,user_aes[i],i,encrypt_user_data[i]);
                if (user_aes[i] != encrypt_user_data[i])
                {
                    OSI_LOGI(0, "SIMLOCK: user data verify failed!\n");
                    ret = false;
                    goto verify_end;
                }
            }

            // decrypt sign
            if (!simlockAesDecrypt(signature, SL_RSA_SIGN_LEN, rsa_decrypt_sign, uid, SL_SHA256_LEN))
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_SIGN_DES;
#endif
                OSI_LOGI(0, "SIMLOCK: AES decrypt sign failed!\n");
                ret = false;
                goto verify_end;
            }

            //static customise data rsa verify
#if defined(SL_RSA1024_C)
            if (rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA1, SL_SHA1_LEN, shasum, rsa_decrypt_sign))
#elif defined(SL_RSA2048_C)
            if (rsa_pkcs1_verify(&rsa_ctx, RSA_PUBLIC, RSA_SHA256, SL_SHA256_LEN, shasum, rsa_decrypt_sign))
#endif
            {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
                s_protect_data_err_info |= SIMLOCK_ERR_SIGN_VERI;
#endif
                OSI_LOGI(0, "[SIM_LOCK]simlock data veryfied failed\n");
                ret = false;
                goto verify_end;
            }
            else
            {
                OSI_LOGI(0, "simlock data verify ok\n");
                ret = true;
                goto verify_end;
            }
        }
        else
        {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
            s_protect_data_err_info |= SIMLOCK_ERR_SHA_GET;
#endif
            OSI_LOGI(0, "[%s][%d]simlock sha1 calculate fail\n", __FUNCTION__, __LINE__);
            ret = false;
            goto verify_end;
        }
    }
    else
    {
#ifdef SIMLOCK_PROTECT_DATA_DEBUG_SCN
        s_protect_data_err_info |= SIMLOCK_ERR_SIGN_GET;
#endif
        OSI_LOGI(0, "[%s][%d]simlock read signature failed\n", __FUNCTION__, __LINE__);
        ret = false;
        goto verify_end;
    }

verify_end:
    if (signature)
    {
        free(signature);
    }
    return ret;
}
/*****************************************************************************/
// Description :   This function is used to check if the last updated keys from UI or NV can
//         match the encrypt keys, if match, the protocol stack can boot up
//         or else, the protocol stack will boot up restricted.
//
// Global resource dependence :
// Author :
// Note :
/*****************************************************************************/
bool simlockCheck(uint32_t type, SIM_LOCK_KEY_T *input_key)
{

    simlock_encrypt_keys_t simlock_keys;
    uint8_t encrypt_data[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN] = {0};
    uint8_t sha256_data[SIM_LOCK_ENCRYPTED_KEY_MAX_LEN] = {0};
    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t *encrypt_key_ptr = NULL;
    uint32_t i = 0;
    uint8_t key[SIM_LOCK_KEY_MAX_LEN] = {0};
    // get the encrypt keys
    if (SIMLOCK_SUCCESS != prvSimlock_ReadKeys((uint8_t *)&simlock_keys))
    {
        OSI_LOGI(0, "SIMLOCK: get sim key failed\n");
        return false;
    }

    switch (type)
    {
    case MN_SIM_NETWORK_LOCK:
        encrypt_key_ptr = simlock_keys.pin_keys.nck;
        break;
    case MN_SIM_NETWORK_SUBSET_LOCK:
        encrypt_key_ptr = simlock_keys.pin_keys.nsck;
        break;
    case MN_SIM_SP_LOCK:
        encrypt_key_ptr = simlock_keys.pin_keys.spck;
        break;
    case MN_SIM_CORPORATE_LOCK:
        encrypt_key_ptr = simlock_keys.pin_keys.cck;
        break;
    case MN_SIM_USER_LOCK:
        encrypt_key_ptr = simlock_keys.pin_keys.pck;
        break;
    default:
        OSI_LOGI(0, "SIMLOCK: not support this simlock type\n");
        return false;
    }

    // get the uid
    if (!simlockGetUid(uid))
    {
        OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
        return false;
    }
    memcpy(key, input_key->keys, input_key->key_len);
    if (!simlockAesEncrypt(key, SIM_LOCK_KEY_MAX_LEN, sha256_data, uid, SL_SHA256_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: PIN/PUK encrypt failed!\n");
        return false;
    }

    sha2((unsigned char *)sha256_data, SIM_LOCK_ENCRYPTED_KEY_MAX_LEN, encrypt_data, 0);
    // compare the two pwd

    for (i = 0; i < SIM_LOCK_ENCRYPTED_KEY_MAX_LEN; i++)
    {
        if (encrypt_data[i] != encrypt_key_ptr[i])
        {
            OSI_LOGI(0, "SIMLOCK: unlock key error !\n");
            return false;
        }
    }

    return true;
}

bool simlockEncryptDataUpdate(void /* user_data*/) // the last step to protect the simlock status
{
    uint8_t uid[SL_SHA256_LEN] = {0};
    uint8_t sha256_data[SL_SHA256_LEN] = {0};
    uint8_t encrypted_user_data[32];
    MN_SIM_LOCK_USER_DATA_T nv_user_data;

    if ((nvmReadItem(NVID_SIM_LOCK_USER_DATA, (uint8_t *)&nv_user_data,
                     sizeof(MN_SIM_LOCK_USER_DATA_T)) < 0))
    {
        OSI_LOGI(0, "SIMLOCK: get user data from nv failed\n");
        return false;
    }

    //get the uid
    if (!simlockGetUid(uid))
    {
        OSI_LOGI(0, "SIMLOCK: can't get uid!\n");
        return false;
    }
    nv_user_data.SIM_lock_status &= 0xff;

    sha2((unsigned char *)&(nv_user_data), sizeof(simlock_dynamic_data_t), sha256_data, 0);

    //encrypt user data with AES
    if (!simlockAesEncrypt(sha256_data, SL_SHA256_LEN, encrypted_user_data, uid, SL_SHA256_LEN))
    {
        OSI_LOGI(0, "SIMLOCK: user data encrypt failed!\n");
        return false;
    }

    // write back to nv
    if (SIMLOCK_SUCCESS != simlockWriteEncryptUserData(encrypted_user_data))
    {
        OSI_LOGI(0, "SIMLOCK: simlock_sign can't be saved to simlock partition !\n");
        return false;
    }

    return true;
}

uint32_t simlockGetControlData(uint8_t *data, uint16_t size)
{
    return 0;
}

bool prvEfuseGetUid(void *uid)
{
    uint32_t id_low, id_high;
    uint8_t *id = (uint8_t *)uid;

    bool result = drvEfuseBatchRead(true,
                                    RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_LOW_INDEX, &id_low,
                                    RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_HIGH_INDEX, &id_high,
                                    DRV_EFUSE_ACCESS_END);

    if (result)
    {
        *id++ = (id_low & 0xff000000) >> 24;
        *id++ = (id_low & 0x00ff0000) >> 16;
        *id++ = (id_low & 0x0000ff00) >> 8;
        *id++ = (id_low & 0x000000ff) >> 0;

        *id++ = (id_high & 0xff000000) >> 24;
        *id++ = (id_high & 0x00ff0000) >> 16;
        *id++ = (id_high & 0x0000ff00) >> 8;
        *id++ = (id_high & 0x000000ff) >> 0;

        OSI_LOGD(0, "EFUSE Read id_low = 0x%08x", id_low);
        OSI_LOGD(0, "EFUSE Read id_high = 0x%08x", id_high);
        return true;
    }
    return false;
}

uint32_t simlockGetUid(uint8_t *uid)
{
    char ori_uid[32] = {0};
    uint32_t ret;

    ret = prvEfuseGetUid((void *)ori_uid);
    if (!ret)
        return 0;

    sha2((unsigned char *)ori_uid, sizeof(ori_uid), uid, 0);

    return 1;
}
