/**  @file
  quec_atcmd_def.h

  @brief
  This file is used to define at command for different Quectel Project.

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
#ifndef QUEC_ATCMD_DEF_H
#define QUEC_ATCMD_DEF_H

#include "ql_api_bt_common.h"
/*========================================================================
 *  Macro Definition
 *========================================================================*/

	
/*========================================================================
 *	Enumeration Definition
 *========================================================================*/
typedef enum egmr_type_e
{
    DSAT_EGMR_BASEBAND_CHIPSET = 0,
	DSAT_EGMR_DSP_CODE = 1,
	DSAT_EGMR_DSP_PATCH = 2,
	DSAT_EGMR_MCU_SW = 3,
	DSAT_EGMR_MD_BOARD = 4,
    DSAT_EGMR_SN = 5,
    DSAT_EGMR_IMEI = 7,
    DSAT_EGMR_MEID_GSN = 8,
    DSAT_EGMR_IMEISV_SVN = 9,
    DSAT_EGMR_IMEI_SVN = 10, //IMEI is the first 15 bytes of IMEI.
    DSAT_EGMR_MAX
}dsat_egmr_type_e;

typedef enum
{
    QUECTEL_URC_ALL_IND = 0,    //use to contol all urc report set true as default
    QUECTEL_URC_CSQ_IND,        // use to contol CSQ urc report
    QUECTEL_URC_ACTCHGRPT_IND,  //use to contol network mode change urc report
    QUECTEL_URC_SMSFULL_IND,    //use to control smsfull urc report
    QUECTEL_URC_SMSINCOMING_IND,//use to control sms incoming urc report
    QUECTEL_URC_RING_IND,
    QUECTEL_URC_MAX_IND,
}quectel_urc_ind_cfg_e_type;

typedef enum {
    AT_AND_C = 0,
    AT_AND_D,
    AT_AND_F,
    AT_AND_W,
    AT_E,
    AT_Q,
    AT_V,
    AT_X,
    AT_Z,
    AT_S_0,
    AT_S_3,
    AT_S_4,
    AT_S_5,
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
    AT_S_7,
#endif
    AT_CONFIG_MAX_NUM,
}quec_at_config_type_e;


typedef enum
{
    QUEC_POWD_ON,
    QUEC_POWD_IMMDLY,//快速关�?
    QUEC_POWD_NORMAL,//正常关机
    QUEC_RESET_NORMAL//正常复位
}quec_Powd_Mode;

typedef struct
{
    void* quec_qpowd_task;//AT关机指令中的线程
    void *semaphore;
    atCommand_t *cmd;//AT指令中句柄备份的地址
    quec_Powd_Mode mode;//当前AT指令中关机模式或者复�?
} quec_qpowd_context;


/*========================================================================
*  Type Definition
*========================================================================*/
typedef void (*QCFG_ProcessFunc)(atCommand_t *);

typedef struct
{
    char *pCmdName;
    QCFG_ProcessFunc pCmdHandler;
    char *test_info;//set test info as "", it will ouput at the cmd handler; set it as null, it will be ignored.
} QCFG_SCmdListStruct;

typedef QCFG_SCmdListStruct quec_cfg_cmdlist_s;

typedef struct
{
    uint8_t sim_stat_urc;
} quec_at_setting_s;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
typedef struct
{
    uint32_t baudrate;
	uint32_t icf_format;
	uint32_t icf_parity;
	uint32_t ifc_rx; 
    uint32_t ifc_tx;
} quec_uart_setting_s;
#endif

typedef struct {
    quec_at_config_type_e config_type;
    char  *config_name;
}quec_at_config_context_s;

/*========================================================================
 *	Variable Definition
 *========================================================================*/
extern quec_at_setting_s quec_at_current_setting;
extern ql_bt_ble_local_name_s ql_bt_ble_local_name;
/*========================================================================
 *	function Definition
 *========================================================================*/
void quec_at_setting_reset_default(void);
void quec_at_setting_read(void);
bool quec_wdt_cfg_read(void);
bool quec_at_setting_save(void);
bool quec_uart_setting_save(void);


uint8_t quectel_read_urc_ind_flag(quectel_urc_ind_cfg_e_type flag);
void quectel_exec_qcfg_urc_ri_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_urc_delay_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_urc_cache_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_urc_signaltype_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_fota_cid_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_fota_http_timeout_cmd(atCommand_t *cmd);
void quectel_exec_qcfg_pdp_kalive_ip_cmd(atCommand_t *cmd);
void quec_at_ecc_read();
bool quec_convert_string_to_hex(unsigned char *str_ptr, unsigned char length, unsigned char *hex);

bool quec_bt_ble_set_localname(ql_bt_ble_local_name_s local_name);
bool quec_bt_ble_get_localname(ql_bt_ble_local_name_s *local_name);
bool quec_ble_write_ibeacon_cfg(ql_ble_ibeacon_cfg_s cfg);
bool quec_ble_read_ibeacon_cfg(ql_ble_ibeacon_cfg_s *cfg);

#endif /* QUEC_ATCMD_DEF_H */

