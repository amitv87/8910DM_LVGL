/**  @file
  quec_vfs_file.h

  @brief
  This file is used to define version information for different Quectel Project.

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


#ifndef QUEC_VFS_FILE_H
#define QUEC_VFS_FILE_H

#include "vfs.h"
#include "hal_config.h"
#include "quec_common.h"


#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Macro Definition
 *========================================================================*/
//********************** fota file definition ***************************/
//#define QUEC_FOTA_RESULT_FILE CONFIG_FS_FOTA_DATA_DIR"/q_fota_result.tmp"


//********************** factory file definition ***************************/
#define FACTORYNV_DIR_ROOT CONFIG_FS_FACTORY_MOUNT_POINT

#define QUEC_WIFI_MAC_FILE FACTORYNV_DIR_ROOT"/qwifimac.bin"
#define QUEC_MAX_MAC_DATA_LEN (17)
#define QUEC_MAX_MAC_TOTAL_LEN (QUEC_MAX_MAC_DATA_LEN+1)

#define QUEC_BT_ADDR_FILE FACTORYNV_DIR_ROOT"/qbtaddr.bin"
#define QUEC_MAX_BT_ADDR_LEN (12)
#define QUEC_MAX_BT_ADDR_TOTAL_LEN (QUEC_MAX_BT_ADDR_LEN+1)

//********************* quectel config nv file definition *******************/
// QUEC_CONFIG_DIR_ROOT is the directory for internal file
// under QUEC_CONFIG_DIR_ROOT, you should only use quec_vfs_sfile_xxxxx interface to do file access.
#define QUEC_CONFIG_DIR_ROOT	    "/config"
#define QUEC_CONFIG_DIR_ROOT_LEN    7

//********************** common/platform nv **********************/
#define QUEC_QSVN_NVM   QUEC_CONFIG_DIR_ROOT"/svn_str.nv"
#define QUEC_URC_CONFIG_FILE  QUEC_CONFIG_DIR_ROOT"/quec_urc_config.nv"
#define QUEC_LED_CFG_TBL  QUEC_CONFIG_DIR_ROOT"/led_cfg_tbl.nv"
#define QUEC_LED_MODE_CFG  QUEC_CONFIG_DIR_ROOT"/led_mode_cfg.nv"
#define QUEC_LED_PWM_CFG  QUEC_CONFIG_DIR_ROOT"/led_pwm_cfg.nv"
#define QUEC_FOAT_PACK_NAME_CFG  QUEC_CONFIG_DIR_ROOT"/quec_fota_packname.nv"

#ifdef CONFIG_ATR_CMUX_SUPPORT
#define QUEC_CMUX_URC_CFG	QUEC_CONFIG_DIR_ROOT"/quec_cmux_urc_cfg.nv"
#endif

//********************* network/modem/rf nv **********************/
#define QUEC_USBNETDEV_CONFIG QUEC_CONFIG_DIR_ROOT"/usbnetdev.nv"
#define QUEC_QINDCFG_CONFIG  QUEC_CONFIG_DIR_ROOT"/qindcfg.nv"
#define QUEC_NW_SYNCH_TIME  QUEC_CONFIG_DIR_ROOT"/nwtime.nv"
#define QUEC_DATA_COUNT  QUEC_CONFIG_DIR_ROOT"/datacnt.nv"
#define QUEC_AT_SETTING  QUEC_CONFIG_DIR_ROOT"/qatsetting.nv"
#define QUEC_UART_SETTING  QUEC_CONFIG_DIR_ROOT"/quartsetting.nv"
#define QUEC_USB_SETTING  QUEC_CONFIG_DIR_ROOT"/qusbsetting.nv"
#define QUEC_LBS_CONFIG  QUEC_CONFIG_DIR_ROOT"/qlbscfg.nv"
#define QUEC_CTSREG_CONFIG QUEC_CONFIG_DIR_ROOT"/ctsreg_cfg.nv"
#define QUEC_CTSREG_SWITCH_CONFIG QUEC_CONFIG_DIR_ROOT"/ctsreg_switch_cfg.nv"
#define QUEC_WDT_CFGING  QUEC_CONFIG_DIR_ROOT"/qwdtcfg.nv"
#define QUEC_AIRPLANE_CONFIG QUEC_CONFIG_DIR_ROOT"/airplane.nv"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PSM
#define QUEC_PSM_DATA  QUEC_CONFIG_DIR_ROOT"/psm_data.nv"
#endif
//********************** sms/call/volte nv ***********************/
#define QUEC_ECC_CALL_CONFIG  QUEC_CONFIG_DIR_ROOT"/qatecccfg.nv"
#define QUEC_VOLTE_CONFIG     QUEC_CONFIG_DIR_ROOT"/voltecfg.nv"

//**************************** sim nv ****************************/

//************************** audio nv ****************************/
#define QUEC_AUDIO_PA_TYPE_CONFIG QUEC_CONFIG_DIR_ROOT"/aud_pa_type.nv"

//************************** gnss nv ****************************/
#define QUEC_GNSS_PARAM_CONFIG  QUEC_CONFIG_DIR_ROOT"/qgnsscfg.nv"

//************************** tcpip nv ****************************/
#define QUEC_HEART_BEAT_CONFIG  QUEC_CONFIG_DIR_ROOT"/heartbeatcfg.nv"
#define QUEC_HB_RETRY_CONFIG  QUEC_CONFIG_DIR_ROOT"/hb_retry_cfg.nv"


#define QUEC_BTBLE_NAME_CONFIG  QUEC_CONFIG_DIR_ROOT"/btble_name.nv"
#define QUEC_BLE_IBEACON_CONFIG  QUEC_CONFIG_DIR_ROOT"/ble_ibeacon.nv"

//********************** quectel user file definition ***********************/
// QUEC_USER_DIR_ROOT is the directory for user file
// under QUEC_USER_DIR_ROOT, you should only use quec_vfs_xxxxx interface to do file access.
#define QUEC_USER_DIR_ROOT		    "/user/"
#define QUEC_USER_DIR_ROOT_LEN      5        //only check first 5 characters
#define QUEC_SD_DIR_ROOT			"SD/"    //current not support
#define QUEC_OPEN_NVM_ROOT_DIR      "/qlconfig/"
#define QUEC_OPEN_FOTA_ROOT_DIR     CONFIG_FS_FOTA_DATA_DIR"/"
#define QUEC_SFS_FILE_ROOT_DIR		"/qsfs/"

//this file is stored in the directory of QUEC_OPEN_NVM_ROOT_DIR, because it is called by ql_nvm_fread/ql_nvm_fwrite, there is no need to add the directory
#define QUEC_OPEN_CUST_NVM          "cust_nvm.nv"


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/


/*========================================================================
 *	General  Definition
 *========================================================================*/
 /* 80 bytes for filename max */
#define QUEC_FILE_MAX_NAME_LEN         (63)			// No '\0'
#define QUEC_FILE_MAX_FOLDER_LEN       (63)			// No '\0'
#define QUEC_FILE_PREFIX_LEN		   (5)			// No '\0',Max:"FOTA:"

#define QUEC_FILE_MAX_PATH_LEN         (QUEC_FILE_MAX_NAME_LEN+QUEC_FILE_MAX_FOLDER_LEN+1)
#define QUEC_FILE_FILENAME_MAX_LEN     (QUEC_FILE_PREFIX_LEN+QUEC_FILE_MAX_PATH_LEN)
#define QUEC_FILE_TYPE_NAME_MAX_LEN    10
#define QUEC_PATH_LEN_MAX              40
#define QUEC_FILE_MAX_OPEN_INDEX       10
#define QUEC_FILE_USER_FREE_SIZE_LIMIT 128*1024

/*========================================================================
 *	Utilities Definition
 *========================================================================*/


/*========================================================================
 *	function Definition
 *========================================================================*/
int quec_vfs_sfile_get_size(const char *name);
int quec_vfs_sfile_read(const char *name, void *data, int size);
int quec_vfs_sfile_write(const char *name, const void *data, int size);


// to udpate quectel cfg nv item under QUEC_CONFIG_DIR_ROOT
int quec_vfs_cfg_nv_write(const char *name, const void *data, int size);

//to update factory config, please call it carefully!
int quec_vfs_sfile_factory_write(const char *name, const void *data, int size);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_VFS_FILE_H */


