/**  
  @file
  ql_api_dev.h

  @brief
  quectel dev info interface definition.

*/
/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_API_DEV_H
#define QL_API_DEV_H


#include "ql_api_common.h"


#ifdef __cplusplus
    extern "C" {
#endif

/*========================================================================
 *  Macro Definition
 *========================================================================*/
#define QL_DEV_ERRCODE_BASE (QL_COMPONENT_STATE_INFO<<16)

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    uint32_t total_size;     ///< memory pool total size
    uint32_t avail_size;     ///< available size. The actual allocatable size may be less than this
} ql_memory_pool_state_t;

/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
//dev component detail errcode, 4 bytes
typedef enum
{
    QL_DEV_SUCCESS = QL_SUCCESS,
    
    QL_DEV_EXECUTE_ERR                      = 1|QL_DEV_ERRCODE_BASE,
    QL_DEV_MEM_ADDR_NULL_ERR,
    QL_DEV_INVALID_PARAM_ERR,
    QL_DEV_BUSY_ERR,
    QL_DEV_SEMAPHORE_CREATE_ERR,
    QL_DEV_SEMAPHORE_TIMEOUT_ERR,
    
    QL_DEV_CFW_CFUN_GET_ERR                 = 15|QL_DEV_ERRCODE_BASE,
    QL_DEV_CFW_CFUN_SET_CURR_COMM_FLAG_ERR  = 18|QL_DEV_ERRCODE_BASE,
    QL_DEV_CFW_CFUN_SET_COMM_ERR,
    QL_DEV_CFW_CFUN_SET_COMM_RSP_ERR,
    QL_DEV_CFW_CFUN_RESET_BUSY              = 25|QL_DEV_ERRCODE_BASE,
    QL_DEV_CFW_CFUN_RESET_CFW_CTRL_ERR,
    QL_DEV_CFW_CFUN_RESET_CFW_CTRL_RSP_ERR,
    
    QL_DEV_IMEI_GET_ERR                     = 33|QL_DEV_ERRCODE_BASE,
    
    QL_DEV_SN_GET_ERR                       = 36|QL_DEV_ERRCODE_BASE,
    
    QL_DEV_UID_READ_ERR                     = 39|QL_DEV_ERRCODE_BASE,
    
    QL_DEV_TEMP_GET_ERR                     = 50|QL_DEV_ERRCODE_BASE,
    
    QL_DEV_WDT_CFG_ERR                      = 53|QL_DEV_ERRCODE_BASE,
    
	QL_DEV_MEM_QUERY_ERR 					= 56|QL_DEV_ERRCODE_BASE,
} ql_errcode_dev_e;


/*========================================================================
 *  Variable Definition
 *========================================================================*/


/*========================================================================
 *  function Definition
 *========================================================================*/
/*
* to get the IMEI label
*/
ql_errcode_dev_e ql_dev_get_imei(char *p_imei, size_t imei_len, uint8_t nSim);

/*
* to get the SN label
*/
ql_errcode_dev_e ql_dev_get_sn(char *p_sn, size_t sn_len, uint8_t nSim);

/*
* to get the product ID
*/
ql_errcode_dev_e ql_dev_get_product_id(char *p_product_id, size_t product_id_len);

/*
* to get the model type
*/
ql_errcode_dev_e ql_dev_get_model(char *p_model, size_t model_len);

/*
* to get the firmware version of model
*/
ql_errcode_dev_e ql_dev_get_firmware_version(char *p_version, size_t version_len);

/*
* to get the firmware subversion of model
*/
ql_errcode_dev_e ql_dev_get_firmware_subversion(char *p_subversion, size_t subversion_len);

/*
* to get the unique chip id of model
*/
ql_errcode_dev_e ql_dev_get_cpu_uid(unsigned long long *p_chip_id);


/*
* to get the current functionality of model
*/
ql_errcode_dev_e ql_dev_get_modem_fun(uint8_t *p_function, uint8_t nSim);

/*
* to set the functionality of model or to reset the model
*/
ql_errcode_dev_e ql_dev_set_modem_fun(uint8_t at_dst_fun, uint8_t rst, uint8_t nSim);

/*
* to get temperature value of chip
*/
ql_errcode_dev_e ql_dev_get_temp_value(int32_t *ql_temp);

/*
* to config watchdog
*/
ql_errcode_dev_e ql_dev_cfg_wdt(uint8_t opt);

/*
* to query memory pool state information
*/
ql_errcode_dev_e ql_dev_memory_size_query(ql_memory_pool_state_t *ql_pool_state);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SECURE_BOOT
/*
* to enable secure boot
*/
ql_errcode_dev_e ql_dev_enable_secure_boot();
#endif


#ifdef __cplusplus
    } /*"C" */
#endif

#endif /* QL_API_DEV_H */

