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

#ifndef _BT_MSG_H_
#define _BT_MSG_H_

#define APP_BT_MSG 0
#define APP_BLE_MSG 1

typedef void (*AppBtMsgHandler_t)(void *param);
bool AppRegisterBtMsgCB(uint32_t type, AppBtMsgHandler_t handler);

//--------------------------------- Classic ------------------------------------

#define APP_BT_MSG_FLAG 0x80000000
#define APP_BT_MSG_BASE 0x00550000

#define APP_BT_ME_ON_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 1))
#define APP_BT_ME_OFF_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 2))
#define APP_BT_VISIBILE_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 3))
#define APP_BT_HIDDEN_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 4))
#define APP_BT_SET_LOCAL_NAME_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 5))
#define APP_BT_SET_LOCAL_ADDR_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 6))
#define APP_BT_INQ_DEV_NAME (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 7))
#define APP_BT_INQ_COMP_CNF (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 8))
#define APP_BT_INQUIRY_CANCEL (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 9))
#define APP_BT_DEV_PAIR_COMPLETE (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 10))
#define APP_BT_DELETE_DEVICE_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 11))
#define APP_BT_DEV_PIN_REQ (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 12))
#define APP_BT_SSP_NUM_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 13))
#define APP_BT_SPP_CONNECT_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 14))
#define APP_BT_SPP_DISCONNECT_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 15))
#define APP_BT_SPP_DATA_RECIEVE_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 16))
#define APP_BT_SPP_DATA_SEND_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 17))
#define APP_BT_HFP_CONNECT_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 18))
#define APP_BT_HFP_DISCONNECT_RES (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 19))
#define APP_BT_REOPEN_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 20))        //stop of BT reopen
#define APP_BT_REOPEN_ACTION_IND (APP_BT_MSG_FLAG | (APP_BT_MSG_BASE + 21)) //start of BT reopen

//--------------------------------- BLE ----------------------------------------
#define APP_BLE_MSG_FLAG 0x08000000
#define APP_BLE_COMM_BASE 0x00600000
#define APP_BLE_ADV_BASE 0x00610000
#define APP_BLE_SCAN_BASE 0x00620000

#define APP_BLE_SET_PUBLIC_ADDR (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 1))
#define APP_BLE_SET_RANDOM_ADDR (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 2))
#define APP_BLE_ADD_WHITE_LIST (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 3))
#define APP_BLE_REMOVE_WHITE_LIST (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 4))
#define APP_BLE_CLEAR_WHITE_LIST (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 5))
#define APP_BLE_CONNECT (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 6))
#define APP_BLE_DISCONNECT (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 7))
#define APP_BLE_UPDATA_CONNECT (APP_BLE_MSG_FLAG | (APP_BLE_COMM_BASE + 8))

#define APP_BLE_SET_ADV_PARA (APP_BLE_MSG_FLAG | (APP_BLE_ADV_BASE + 1))
#define APP_BLE_SET_ADV_DATA (APP_BLE_MSG_FLAG | (APP_BLE_ADV_BASE + 2))
#define APP_BLE_SET_ADV_ENABLE (APP_BLE_MSG_FLAG | (APP_BLE_ADV_BASE + 3))
#define APP_BLE_SET_ADV_SCAN_RSP (APP_BLE_MSG_FLAG | (APP_BLE_ADV_BASE + 4))

#define APP_BLE_SET_SCAN_PARA (APP_BLE_MSG_FLAG | (APP_BLE_SCAN_BASE + 1))
#define APP_BLE_SET_SCAN_ENABLE (APP_BLE_MSG_FLAG | (APP_BLE_SCAN_BASE + 2))
#define APP_BLE_SET_SCAN_DISENABLE (APP_BLE_MSG_FLAG | (APP_BLE_SCAN_BASE + 3))
#define APP_BLE_SET_SCAN_REPORT (APP_BLE_MSG_FLAG | (APP_BLE_SCAN_BASE + 4))
#define APP_BLE_SET_SCAN_FINISH (APP_BLE_MSG_FLAG | (APP_BLE_SCAN_BASE + 5))

#endif
