/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "ats_config.h"

#if defined(CONFIG_AT_BT_CLASSIC_SUPPORT) || defined(CONFIG_AT_SPBLE_SUPPORT) || defined(CONFIG_AT_BTCOMM_SUPPORT)
#include "stdlib.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_engine.h"

#include "bt_abs.h"
#include "at_cmd_btmsg.h"
#include "bt_app.h"

typedef struct _app_bt_msg_t
{
    unsigned int BT_ID;
    unsigned int BT_APP_ID;
} app_bt_msg_t;

static app_bt_msg_t gBtMsgTable[] = {
#if defined(CONFIG_AT_BT_CLASSIC_SUPPORT) || defined(CONFIG_AT_BTCOMM_SUPPORT)
    {ID_STATUS_BT_ON_RES, APP_BT_ME_ON_CNF},
    {ID_STATUS_BT_OFF_RES, APP_BT_ME_OFF_CNF},
    {ID_STATUS_CM_VISIBLE_RES, APP_BT_VISIBILE_CNF},
    {ID_STATUS_CM_DISVISIBLE_RES, APP_BT_HIDDEN_CNF},
    {ID_STATUS_CM_SET_LOCAL_NAME_RES, APP_BT_SET_LOCAL_NAME_RES},
    {ID_STATUS_CM_SET_LOCAL_ADDR_RES, APP_BT_SET_LOCAL_ADDR_RES},
    {ID_STATUS_CM_INQUIRY_RES, APP_BT_INQ_DEV_NAME},
    {ID_STATUS_CM_INQUIRY_FINISH_RES, APP_BT_INQ_COMP_CNF},
    {ID_STATUS_CM_CANCEL_INQUIRY_RES, APP_BT_INQUIRY_CANCEL},
    {ID_STATUS_SC_PAIR_RES, APP_BT_DEV_PAIR_COMPLETE},
    {ID_STATUS_SC_DEPAIR_RES, APP_BT_DELETE_DEVICE_RES},
    {ID_STATUS_SC_PASSKEY_REQ, APP_BT_DEV_PIN_REQ},
    {ID_STATUS_SC_SSP_NUM_IND, APP_BT_SSP_NUM_IND},
    {ID_STATUS_SPP_CONNECT_RES, APP_BT_SPP_CONNECT_IND},
    {ID_STATUS_SPP_DISCONNECT_RES, APP_BT_SPP_DISCONNECT_IND},
    {ID_STATUS_SPP_DATA_RECIEVE_IND, APP_BT_SPP_DATA_RECIEVE_IND},
    {ID_STATUS_SPP_DATA_SEND_RES, APP_BT_SPP_DATA_SEND_IND},
    {ID_STATUS_HFP_CONNECT_RES, APP_BT_HFP_CONNECT_RES},
    {ID_STATUS_HFP_DISCONNECT_RES, APP_BT_HFP_DISCONNECT_RES},
    {ID_STATUS_BT_REOPEN_IND, APP_BT_REOPEN_IND},               //stop of BT reopen
    {ID_STATUS_BT_REOPEN_ACTION_IND, APP_BT_REOPEN_ACTION_IND}, //start of BT reopen
#endif
#ifdef CONFIG_AT_SPBLE_SUPPORT
    {ID_STATUS_AT_BLE_SET_PUBLIC_ADDR_RES, APP_BLE_SET_PUBLIC_ADDR},
    {ID_STATUS_AT_BLE_SET_RANDOM_ADDR_RES, APP_BLE_SET_RANDOM_ADDR},
    {ID_STATUS_AT_BLE_ADD_WHITE_LIST_RES, APP_BLE_ADD_WHITE_LIST},
    {ID_STATUS_AT_BLE_REMOVE_WHITE_LIST_RES, APP_BLE_REMOVE_WHITE_LIST},
    {ID_STATUS_AT_BLE_CLEAR_WHITE_LIST_RES, APP_BLE_CLEAR_WHITE_LIST},
    {ID_STATUS_AT_BLE_CONNECT_RES, APP_BLE_CONNECT},
    {ID_STATUS_AT_BLE_DISCONNECT_RES, APP_BLE_DISCONNECT},
    {ID_STATUS_AT_BLE_UPDATA_CONNECT_RES, APP_BLE_UPDATA_CONNECT},

    {ID_STATUS_AT_BLE_SET_ADV_PARA_RES, APP_BLE_SET_ADV_PARA},
    {ID_STATUS_AT_BLE_SET_ADV_DATA_RES, APP_BLE_SET_ADV_DATA},
    {ID_STATUS_AT_BLE_SET_ADV_ENABLE_RES, APP_BLE_SET_ADV_ENABLE},
    {ID_STATUS_AT_BLE_SET_SCAN_RSP_RES, APP_BLE_SET_ADV_SCAN_RSP},

    {ID_STATUS_AT_BLE_SET_SCAN_PARA_RES, APP_BLE_SET_SCAN_PARA},
    {ID_STATUS_AT_BLE_SET_SCAN_ENABLE_RES, APP_BLE_SET_SCAN_ENABLE},
    {ID_STATUS_AT_BLE_SET_SCAN_DISABLE_RES, APP_BLE_SET_SCAN_DISENABLE},
    {ID_STATUS_AT_BLE_SET_SCAN_REPORT_RES, APP_BLE_SET_SCAN_REPORT},
    {ID_STATUS_AT_BLE_SET_SCAN_FINISH_RES, APP_BLE_SET_SCAN_FINISH},
#endif
};

static AppBtMsgHandler_t gbtMsgHandler = NULL;
static AppBtMsgHandler_t gbleMsgHandler = NULL;

static void AtBtPostEvent(uint32_t nEventId, uint32_t status, void *data)
{
    osiEvent_t *ev = malloc(sizeof(osiEvent_t));
    if (ev != NULL)
    {
        ev->id = nEventId;
        ev->param1 = status;
        ev->param2 = (uint32_t)data;

        if (((nEventId & 0xff000000) == APP_BT_MSG_FLAG) &&
            (gbtMsgHandler != NULL))
        {
            osiThreadCallback(atEngineGetThreadId(), gbtMsgHandler, (void *)ev);
        }
        else if (((nEventId & 0xff000000) == APP_BLE_MSG_FLAG) &&
                 (gbleMsgHandler != NULL))
        {
            osiThreadCallback(atEngineGetThreadId(), gbleMsgHandler, (void *)ev);
        }
        else
        {
            OSI_LOGI(0, "BT#msg: post event failed, nEventId:0x%x", nEventId);

            if (data != NULL)
                free(data);

            free(ev);
        }
    }

    return;
}

static void AppBtMsgCallback(unsigned int msg_id, char status, void *data_ptr)
{
    uint32_t i = 0;
    uint32_t nEventId = 0xFFFFFFFF;

    OSI_LOGI(0, "BT#_AppBtMsgCallback: id=0x%x, param1=0x%x, param2=0x%x", msg_id, status, data_ptr);

    for (i = 0; i < sizeof(gBtMsgTable) / sizeof(gBtMsgTable[0]); i++)
    {
        if (gBtMsgTable[i].BT_ID == msg_id)
            nEventId = gBtMsgTable[i].BT_APP_ID;
    }

    if (0xFFFFFFFF != nEventId)
    {
        AtBtPostEvent(nEventId, status, data_ptr);
    }
    else
    {
        OSI_LOGI(0, "BT#_AppBtMsgCallback: Unkown msgid to ignore");
    }

    return;
}

bool AppRegisterBtMsgCB(uint32_t type, AppBtMsgHandler_t handler)
{
    if (type == APP_BT_MSG)
    {
        if (gbtMsgHandler != NULL && handler != NULL)
            return false;
        else
            gbtMsgHandler = handler;
    }
    else if (type == APP_BLE_MSG)
    {
        if (gbleMsgHandler != NULL && handler != NULL)
            return false;
        else
            gbleMsgHandler = handler;
    }
    else
    {
        return false;
    }

    if ((gbtMsgHandler != NULL) || (gbleMsgHandler != NULL))
        bt_at_callback_register(AppBtMsgCallback);
    else
        bt_at_callback_register(NULL);

    return true;
}

#endif
