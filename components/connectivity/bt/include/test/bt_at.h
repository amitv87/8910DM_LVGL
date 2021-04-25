/*
 *  <blueu file> - <bluetooth stack implementation>
 *
 *  Copyright (C) 2019 Unisoc Communications Inc.
 *  History:
 *      <2019.10.11> <wcn bt>
 *      Description
 */

#ifndef _BT_AT_H_
#define _BT_AT_H_

#define BLUEU_VERIFY_BT_NPI_EN
#define BLUEU_VERIFY_BLE_NPI_EN
#define BLUEU_VERIFY_BT_COMM_EN
//#define BLUEU_VERIFY_BT_HFP_HF_EN
//#define BLUEU_VERIFY_BT_HFP_AG_EN
//#define BLUEU_VERIFY_BT_A2DP_SINK_EN
//#define BLUEU_VERIFY_BT_A2DP_SRC_EN
//#define BLUEU_VERIFY_BT_SPP_EN
#define BLUEU_VERIFY_BLE_NORMAL_EN

#define BT_CMD_MAX_PARAM_NUM        20
#define BT_INQUIRY_DEV_MAX_NUM      30

#define BT_AT_SPEC_VERSION          "V2.0.8"

#define IS_HEX(ch)  ((('0' <= ch) && ('9' >= ch)) || \
                     (('a' <= ch) && ('f' >= ch)) || \
                     (('A' <= ch) && ('F' >= ch)))

/**
 * BT NPI CMD
 **/
//BT SPBTTEST
#define BT_SPBTTEST_OK          "+SPBTTEST:OK"
#define BT_SPBTTEST_ERR         "+SPBTTEST:ERR=ERROR"


//BT SPBLETEST
#define BT_SPBLETEST_OK         "+SPBLETEST:OK"
#define BT_SPBLETEST_ERR         "+SPBLETEST:ERR=ERROR"

/**
 * BT AT CMD
 **/
//BT COMM CMD
#define BT_COMM_RESP_ERR        "+BTCOMM:ERR="
#define BT_COMM_RESP_OK         "+BTCOMM:OK"

//BT AVRCP CMD
#define BT_AVRCP_OK                 "+BTAVRCP:OK"
#define BT_AVRCP_ERR             "+BTAVRCP:ERR=ERROR"

//BT HFP CMD
#define BT_HFP_OK            "+BTHFP:OK"
#define BT_HFP_ERR             "+BTHFP:ERR=ERROR"

//BT SPP CMD
#define BT_SPP_RESP_ERR          "+BTSPP:ERR="
#define BT_SPP_RESP_OK            "+BTSPP:OK"

//BLE COMM CMD
#define BLE_COMM_RESP_ERR      "+BLECOMM:ERR="
#define BLE_COMM_RESP_OK        "+BLECOMM:OK"

//BLE ADV CMD
#define BLE_ADV_RESP_ERR          "+BLEADV:ERR="
#define BLE_ADV_RESP_OK            "+BLEADV:OK"

//BLE SMP CMD
#define BLE_SMP_RESP_ERR          "+BLESMP:ERR="
#define BLE_SMP_RESP_OK            "+BLESMP:OK"

//BLE SCAN CMD
#define BLE_SCAN_RESP_ERR          "+BLESCAN:ERR="
#define BLE_SCAN_RESP_OK        "+BLESCAN:OK"

enum{
    BT_AT_RET_OK,
    BT_AT_RET_ERROR,
    BT_AT_WAIT,
};

typedef struct {
    int (*app_bt_normal_start) (void); //bt normal open
    int (*app_bt_normal_stop) (void); //bt normal close
    void (*app_bt_start_cp_unsleep) (void); //bt open with CP unsleep, used for NPI/BQB test
    void (*app_bt_stop_cp_en_sleep) (void); //bt close and enable cp sleep, used for NPI/BQB test
    void (*at_print_data)(char* buff, int len); //it used by AT handle, and async print data to terminal.
}at_platform_cb_t;

typedef enum
{
    BT_TESTMODE_NONE = 0,  // Leave Test mode
    BT_TESTMODE_SIG = 1,   // Enter EUT signal mode
    BT_TESTMODE_NONSIG = 2 // Enter Nonsignal mode
} BT_TEST_MODE_T;

typedef struct {
  int index;
  const char *name;
  int (*handler)(char *argv[], int argc, char *rsp, int *rsp_len);
  char * help;
} bt_eut_cmd_t;

extern bt_eut_cmd_t bt_npi_cmds[];
extern bt_eut_cmd_t ble_npi_cmds[];

extern bt_eut_cmd_t bt_comm_cmds[];
extern bt_eut_cmd_t bt_hfp_hf_cmds[];
extern bt_eut_cmd_t bt_hfp_ag_cmds[];
extern bt_eut_cmd_t bt_a2dp_sink_cmds[];
extern bt_eut_cmd_t bt_a2dp_src_cmds[];
extern bt_eut_cmd_t bt_spp_cmds[];

#ifdef BLUEU_VERIFY_BLE_NORMAL_EN
extern bt_eut_cmd_t ble_comm_cmds[];
extern bt_eut_cmd_t ble_adv_cmds[];
extern bt_eut_cmd_t ble_smp_cmds[];
extern bt_eut_cmd_t ble_scan_cmds[];
#endif

/**
 * @func  at_set_platform_callback
 * @brief set platform callback for at command process
 * @describe it must be registed by app, cause of chip init
 * @param at_platform_cb_t
 * @return void
 */
void at_set_platform_callback(at_platform_cb_t *at_cb);

/**
 * @func  at_get_platform_callback
 * @brief get platform callback for at command process
 * @describe used for init platform with bluestack enable bt
 * @param void
 * @return at_platform_cb_t
 */
at_platform_cb_t* at_get_platform_callback(void);

/**
 * @func  bt_test_mode_get
 * @brief get bt test mode
 * @describe used for get bt test mode in npi test
 * @param void
 * @return bt_test_mode
 */
BT_TEST_MODE_T bt_test_mode_get(void);

#endif //_BT_AT_H_
