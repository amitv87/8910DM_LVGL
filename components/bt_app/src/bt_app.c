/**
 * @file     bti_8910.c
 * @brief    BT interface for 8910
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-03-11
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#include <assert.h>
#include "bluetooth/bluetooth.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bt_a2dp.h"
#include "bluetooth/bt_avrcp.h"
#include "drv_wcn.h"
#include "drv_wcn_ipc.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "stdlib.h"
#include "stdio.h"
//#include "osi_utils.h"  // build err
#include "bt_cfg.h"
#include "bt_gatt_server_demo.h"
#include "bt_gatt_client_demo.h"
#include "drv_md_ipc.h"
#include "osi_api.h"
#include "bluetooth/bt_spp.h"
#include "bluetooth/bt_hfp.h"
#include "bluetooth/bt_gatt.h"
#include "audio_device.h"
#include "osi_pipe.h"
#include "audio_player.h"
#include "audio_types.h"
#include "osi_log.h"
#include "osi_profile.h"
#include "audio_device.h"
#include "bt_app_config.h"
#include "bt_hci_tra.h"
#include "bt_drv.h"
#include "ddb.h"
#include "bt_abs.h"
#include "bt_app.h"
#include "ats_config.h"
#ifdef CONFIG_AT_BLUEU_VERIFY_SUPPORT
#include "test/bt_at.h"
#endif


#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT

#define CONFIG_QUEC_BT_BLE_APP

#include "ql_log.h"
#include "ql_osi_def.h"
#include "ql_api_osi.h"

#define QL_BT_BLE_APP_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_BT_BLE_APP_LOG(msg, ...)			QL_LOG(QL_BT_BLE_APP_LOG_LEVEL, "ql_BT_APP", msg, ##__VA_ARGS__)
#define QL_BT_BLE_APP_LOG_PUSH(msg, ...)	QL_LOG_PUSH("ql_BT_APP", msg, ##__VA_ARGS__)

#endif


static bt_storage_t s_bt_storage;
static bt_dm_device_t s_bt_device[BT_MAX_DEVICE_NUM];
static osiMessageQueue_t *s_bt_task_queue = NULL;
static osiMessageQueue_t *s_bt_audio_task_queue = NULL;
static BT_SCHED_TASK_STATE_E s_bt_task_state = BT_TASK_STOP;
static BT_STATE_E s_bt_state = BT_STOPED;
uint32 s_bt_protocol_connect_state = 0;

static UINT32 s_bt_inquiry_state = BT_INQ_DISABLE;
uint32 g_inquiry_service_type;
static const char g_BuildRevision[] = BUILD_IDENTIFY;
static bt_at_state_t s_at_bt_state = BT_NONE_STATE;
static st_white_list_info g_AddWhiteInfo = {0, {0, 0, 0, 0, 0, 0}};
static st_white_list_info g_RemoveWhiteInfo = {0, {0, 0, 0, 0, 0, 0}};

extern uint8 gatt_client_role;

//init bt feature config
const bt_config_t bt_config = {
    .task_priority = 28 /*BT_TASK_PRIORITY*/,
    .max_device_num = BT_MAX_DEVICE_NUM,
    .support_spp = TRUE,
    .support_a2dp = TRUE,
    .support_avrcp = TRUE,
    .support_aac = FALSE,
    .support_hid = FALSE,
    .support_hfp = TRUE,
    .support_ssp_debug = FALSE,
};

extern void bt_app_task_create(void);
extern void bt_config_load_nv();
extern void npi_btLoad_register(void);
extern void npi_btLoad_deregister(void);
extern void app_bt_start_cfm(void);
extern void app_bt_stop_cfm(void);
extern void osiChipSetBtSleepChecker(int (*checker)(void));

#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT
osiThread_t *ql_bt_ble_app_thread = NULL;

void ql_bt_ble_app_set_bt_thread(osiThread_t *thread)
{
    ql_bt_ble_app_thread = thread;
}

bool ql_bt_ble_app_send_event(ql_event_t *ql_event)
{
    QL_BT_BLE_APP_LOG("ql_event->id=%x", ql_event->id);
    if (ql_bt_ble_app_thread)
    {
        return ql_rtos_event_send(ql_bt_ble_app_thread, ql_event);
    }
    else 
    {
        QL_BT_BLE_APP_LOG("ql_bt_ble_app_thread not exit");
        return false;
    }
}
#endif

//used to blueu protocal turn on/off bt chip
void bt_state_changed_callback(bt_state_t state)
{
#ifdef CONFIG_QUEC_BT_BLE_APP
    ql_event_t event = {0};
    QL_BT_BLE_APP_LOG("state=%d", state);
#endif
    OSI_LOGI(0, "[BT] enter bt_state_changed_callback state=%d", state);
    switch (state)
    {
    case BT_STATE_ON:
        //bt_SendMsgToSchedTask(BT_START_CFM_MSG, NULL);
        app_bt_start_cfm();
        // set page scan params
        bt_page_scan_param_set(/*interval*/ 0x800, /*window*/ 0x12);
#ifdef CONFIG_QUEC_BT_BLE_APP
        
        event.id = QUEC_BT_START_STATUS_IND;
        ql_bt_ble_app_send_event(&event);
#endif
        break;
    case BT_STATE_OFF:
        //bt_SendMsgToSchedTask(BT_STOP_CFM_MSG, NULL);
        app_bt_stop_cfm();

        //ipc uninit
#ifdef CONFIG_QUEC_BT_BLE_APP
        event.id = QUEC_BT_STOP_STATUS_IND;
        ql_bt_ble_app_send_event(&event);
#endif
        break;

    default:
        break;
    }
}

static void bt_default_config(void)
{
    bt_pskey_nv_info_t g_bt_pskey_nv_info;
    BT_ADDRESS bd_addr;
    uint32 status = NVERR_None;

    //when bt on, app will get the bt addr from nv and set it to stack.
    DB_GetLocalPublicAddr(&bd_addr);

    //load bt_sprd of nvitem
    status = EFS_NvitemRead(NV_BT_SPRD, sizeof(bt_pskey_nv_info_t), (uint8 *)&g_bt_pskey_nv_info);
    SCI_ASSERT(NVERR_None == status); /*assert verified*/

    //set s_bt_storage with nvitem value
    memset(&s_bt_storage, 0, sizeof(bt_storage_t));
    s_bt_storage.adapter.device_class = g_bt_pskey_nv_info.device_class; // BT_DEVICE_OF_CLASS;

    memcpy(s_bt_storage.adapter.local_addr.bytes, bd_addr.addr, 6);
    memcpy(s_bt_storage.adapter.password, BT_PASSWORD, strlen(BT_PASSWORD));

    s_bt_storage.adapter.le_addr.bytes[0] = rand() & 0xff;
    s_bt_storage.adapter.le_addr.bytes[1] = rand() & 0xff;
    s_bt_storage.adapter.le_addr.bytes[2] = rand() & 0xff;
    s_bt_storage.adapter.le_addr.bytes[3] = rand() & 0xff;
    s_bt_storage.adapter.le_addr.bytes[4] = rand() & 0xff;
    s_bt_storage.adapter.le_addr.bytes[5] = (rand() & 0xff) | 0xC0;

    OSI_LOGI(0, "[bt_default_config]random addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
             s_bt_storage.adapter.le_addr.bytes[0], s_bt_storage.adapter.le_addr.bytes[1], s_bt_storage.adapter.le_addr.bytes[2],
             s_bt_storage.adapter.le_addr.bytes[3], s_bt_storage.adapter.le_addr.bytes[4], s_bt_storage.adapter.le_addr.bytes[5]);

    //when bt on, app will get the bt name from nv and set it to stack.
    DB_GetLocalNameUTF8(s_bt_storage.adapter.local_name);
    DB_GetLeNameUTF8(s_bt_storage.adapter.le_name);

    s_bt_storage.adapter.connect_index = 0;
    s_bt_storage.adapter.max_device_num = bt_config.max_device_num;
    s_bt_storage.adapter.device_list = &s_bt_device[0];
    memset(s_bt_storage.adapter.device_list, 0, sizeof(bt_dm_device_t) * bt_config.max_device_num);

    s_bt_storage.support_LE_2M_AUTOSET = 0;
    s_bt_storage.bt_default_visable = DB_GetVisibility();
}

void bt_bond_state_changed_cb(bt_bond_state_t state, bdaddr_t *addr)
{
    OSI_LOGI(0, "[bt_bond_state_changed_cb]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
             addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    OSI_LOGI(0, "[bt_bond_state_changed_cb]  state=%d", state);
}

UINT8 bt_bond_request_handle_cb(const bdaddr_t *const addr, UINT8 *device_name, UINT8 name_length, UINT32 value)
{
    char name_buff[64];

    OSI_LOGI(0, "[bond_request_handle]  addr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
             addr->bytes[0], addr->bytes[1], addr->bytes[2], addr->bytes[3], addr->bytes[4], addr->bytes[5]);
    OSI_LOGI(0, "[bond_request_handle]  value=%d", value);

    sprintf(name_buff, "device_name = %s,name_length = %d", device_name, name_length);
    OSI_LOGI(0, name_buff);

    bdaddr_t bd = {{0x7d, 0x50, 0xb2, 0xda, 0x45, 0x40}};

    if (memcmp(&bd, addr, 6) == 0)
    {
        return 0; // not accept for devices addr is bd
    }
    else
    {
        return 1; // accept for default
    }
}

bt_callbacks_t bt_callbacks = {
    .state_changed_callback = bt_state_changed_callback,
    .bond_state_changed_callback = bt_bond_state_changed_cb,
    .bond_request_handle_cb = bt_bond_request_handle_cb,
};

int bt_deep_sleep_enable_check(void)
{
    int sleep_en = false;

    if (BT_STATE_OFF == bt_state_get())
    {
        OSI_LOGI(0, "bt_deep_sleep_enable_check  BT_STATE_OFF");
        return true;
    }

    sleep_en = bt_chip_enter_sleep_check();

    OSI_LOGI(0, "bt_deep_sleep_enable_check sleep_en=0x%x", sleep_en);

    return sleep_en;
}

void bt_app_audio_task_create(void);

void bt_ap_init(void)
{
    OSI_LOGI(0, "[BT] enter bt_ap_init");

    DB_Init();

    hci_trace_init();

    //init bt configure
    bt_default_config();

    //init blueu protocal
    bt_core_init(&bt_callbacks, &bt_config, &s_bt_storage);

    bt_app_task_create();

    bt_app_audio_task_create();

#ifdef BLUEU_VERIFY_BT_NPI_EN
    //set npi or bqb mode platform init callback
    at_platform_cb_t npi_at_cb = {0};
    npi_at_cb.app_bt_start_cp_unsleep = app_npi_bt_start;
    npi_at_cb.app_bt_stop_cp_en_sleep = app_npi_bt_stop;
    at_set_platform_callback(&npi_at_cb);
#endif

#ifdef BLUEU_VERIFY_BT_COMM_EN
    //set at command platform init callback
    at_platform_cb_t at_cb = {0};
    at_cb.app_bt_normal_start = app_bt_start;
    at_cb.app_bt_normal_stop = app_bt_stop;
    at_set_platform_callback(&at_cb);
#endif

    //set system sleep check callback
    osiChipSetBtSleepChecker(bt_deep_sleep_enable_check);

    OSI_LOGI(0, "[BT] exit bt_ap_init");
}
static void StopState_HandleStartMsg(void)
{
#if BLE_SMP_SUPPORT
    sm_le_device_info_t device_list[LE_SMP_DEVICE_INFO_NUM];
#endif

    OSI_LOGI(0, "StopState_HandleStartMsg, s_bt_task_state=%d\n", s_bt_task_state);

    //startup chip bsp, and start controller
    if (false == bt_chip_startup())
    {
        s_bt_state = BT_STOPED;
        return;
    }

    //disable deep sleep with power on process
    bt_disable_deep_sleep();

    //start bt host
    bt_start();

#if BLE_SMP_SUPPORT
    //when bt is on, you should restore the device list from nv to stack
    //and enable resolving list at the same time
    DB_GetSmpKeyInfo(device_list);
    ble_smp_restore_pair_info((void *)device_list);
    ble_smp_resolving_list_enable(1);
#endif

    s_bt_task_state = BT_TASK_STARTED;

    return;
}

static void StartedState_HandleStopMsg(void)
{
    bt_stop();
}

void app_bt_start_cfm(void)
{
    s_bt_state = BT_STARTED;
#ifdef CONFIG_AT_BT_APP_SUPPORT
    char rsp_buf[32] = {0};
    //print state to uart
    memset(rsp_buf, 0, sizeof(rsp_buf));
    sprintf(rsp_buf, "%s\r\n", "+BTAPP:bt is start:OK");
    bt_app_print_data(rsp_buf, strlen(rsp_buf));
#endif
}

BT_STATUS app_bt_stop(void)
{
    BT_STATUS ret = BT_PENDING;

    if ((s_bt_state == BT_STARTED) || (s_bt_state == BT_STARTING))
    {
        s_bt_state = BT_STOPING;

        bt_SendMsgToSchedTask(BT_STOP_SCHED_MSG, NULL);
        return ret;
    }
    else
    {
        return BT_ERROR;
    }
}

void app_bt_stop_cfm(void)
{
    s_bt_task_state = BT_TASK_STOP;
    s_bt_state = BT_STOPED;

    bt_chip_shutdown();

#ifdef CONFIG_AT_BT_APP_SUPPORT
    char rsp_buf[32] = {0};
    //print state to uart
    memset(rsp_buf, 0, sizeof(rsp_buf));
    sprintf(rsp_buf, "%s\r\n", "+BTAPP:bt is stop");
    bt_app_print_data(rsp_buf, strlen(rsp_buf));
#endif

    //    extern void bt_app_hfp_stop(void);
    //    bt_app_hfp_stop();
    //    extern void bt_app_a2dp_stop(void);
    //    bt_app_a2dp_stop();
    //    extern void app_bt_avrcp_stop(void);
    //    app_bt_avrcp_stop();
}

extern void UartDrv_Rx(void);
static void StartedState_HandleIntMsg(BT_SIGNAL_T *sig_ptr)
{
    BT_INT_MSG_T *stack_msg_ptr = (BT_INT_MSG_T *)sig_ptr->SigP;

    switch (stack_msg_ptr->message[1])
    {
    case OS_TIME_EVENT:
        //        Handle_TimerEvent(stack_msg_ptr->message[0]);   //closed by xu.wu
        break;

    case OS_MSG_EXTEND_EVM:
        //        EVM_Process();
        break;

    case OS_MSG_EXTEND_TRIG_DATA:
        UartDrv_Rx();
        break;

#ifdef BT_TEST_SAVE_PCM_DATA
    case OS_MSG_SAVE_PCM_DATA:
        SavePcmDataToFile(stack_msg_ptr->message[3]);
        OS_Pfree(stack_msg_ptr->message[3]);
        break;
#endif

    default:

        Assert(0); /*assert verified*/
        break;
    }

    free(stack_msg_ptr);
    stack_msg_ptr = NULL;

    return;
}

static void StopStateProcessor(void)
{
    BT_SIGNAL_T *sig_ptr = NULL;

    if (!osiMessageQueueGet(s_bt_task_queue, &sig_ptr))
    {
        OSI_LOGI(0, "osiMessageQueueGet failed");
        return;
    }

    OSI_LOGI(0, "StopStateProcessor sig_ptr->SignalCode=%d", sig_ptr->SignalCode);
    switch (sig_ptr->SignalCode)
    {
    case BT_START_SCHED_MSG:
        StopState_HandleStartMsg();
        break;

    case BT_INT_HANDLE_MSG:

        if (sig_ptr->SigP != NULL) //for bug204984
        {
            free(sig_ptr->SigP);
            sig_ptr->SigP = NULL;
        }

        break;

    default:
        break;
    }
    if (sig_ptr != NULL)
    {
        free(sig_ptr);
        sig_ptr = NULL;
    }

    return;
}

static void StartedStateProcessor(void)
{
    BT_SIGNAL_T *sig_ptr = NULL;

    if (!osiMessageQueueGet(s_bt_task_queue, (void *)&sig_ptr))
    {
        OSI_LOGI(0, "osiMessageQueueGet failed");
        return;
    }
    //sys_tick1 = SCI_GetTickCount();

    switch (sig_ptr->SignalCode)
    {
    case BT_STOP_SCHED_MSG:
        StartedState_HandleStopMsg();
        break;

    case BT_INT_HANDLE_MSG:
        StartedState_HandleIntMsg(sig_ptr);
        break;

    case BT_START_CFM_MSG:
        //        app_bt_start_cfm();
        break;

    case BT_STOP_CFM_MSG:
        //        app_bt_stop_cfm();
        break;

    case BT_START_GATT_CLIENT_MSG:
#ifdef CONFIG_QUEC_BT_BLE_APP
        {
            ql_event_t event = {0};
            event.id = QUEC_BLE_GATT_START_DISCOVER_SERVICE_IND;
            ql_bt_ble_app_send_event(&event);
        }
#else
        Gatt_client_demo_run_statemachine(GATT_CLIENT_DISCOVER_PRME_SER);
#endif
        break;

    default:
        break;
    }

    if (sig_ptr != NULL)
    {
        free(sig_ptr);
        sig_ptr = NULL;
    }

    return;
}

bool bt_SendMsgToSchedTask(uint32 event, void *msg_ptr)
{
    BT_SIGNAL_T *sig_ptr = NULL;

    if (NULL == s_bt_task_queue)
    {
        OSI_LOGI(0, "(NULL == s_bt_task_queue)");
        return FALSE;
    }

    sig_ptr = (BT_SIGNAL_T *)malloc(sizeof(BT_SIGNAL_T));
    sig_ptr->SignalCode = event;
    sig_ptr->SignalSize = sizeof(BT_SIGNAL_T);
    sig_ptr->SigP = msg_ptr;
    if (!osiMessageQueueTryPut(s_bt_task_queue, (void *)&sig_ptr, 0))
    {
        OSI_LOGI(0, "send s_bt_task_queue Error!!!");
        return FALSE;
    }
    return TRUE;
}

static void bt_Sched_Task(void *argv)
{
    void (*bt_task_process_funtion[])(void) =
        {
            StopStateProcessor,
            StartedStateProcessor,
        };

    for (;;)
    {
        bt_task_process_funtion[s_bt_task_state]();
    }
}

void bt_app_task_create(void)
{
    OSI_LOGI(0, "[BT] enter bt_app_task_create");
    s_bt_task_queue = osiMessageQueueCreate(BT_THREAD_QUEUE_NUM, sizeof(void *));
    if (s_bt_task_queue != NULL)
    {
        osiThreadCreate("T_BT_SCHED", bt_Sched_Task, NULL, OSI_PRIORITY_BELOW_NORMAL /*OSI_PRIORITY_HIGH*/, BT_OS_THREAD_STACK_SIZE, 0);
    }
    else
    {
        OSI_LOGI(0, "Create bt sched task Fail");
    }
}

bool bt_send_msg_btAud_task(uint16 msg_id, void *data_ptr)
{
    bt_audio_msg_t *msg = NULL;

    if (NULL == s_bt_audio_task_queue)
    {
        OSI_LOGI(0, "(NULL == s_bt_audio_task_queue)");
        return FALSE;
    }

    msg = (bt_audio_msg_t *)calloc(1, sizeof(bt_audio_msg_t));
    msg->msg_id = msg_id;
    msg->msg_data = data_ptr;

    if (!osiMessageQueueTryPut(s_bt_audio_task_queue, (void *)&msg, 0))
    {
        OSI_LOGI(0, "send s_bt_audio_task_queue Error!!!");
        return FALSE;
    }
    return TRUE;
}

extern void bt_a2dp_msg_handle(void *msg_data);
extern void bt_avrcp_msg_handler(void *msg_data);

static void bt_audio_task(void *argv)
{
    bt_audio_msg_t *audio_msg = NULL;

    for (;;)
    {
        if (!osiMessageQueueGet(s_bt_audio_task_queue, &audio_msg))
        {
            continue;
        }

        if (audio_msg == NULL)
        {
            osiPanic();
        }

        switch (audio_msg->msg_id)
        {
        case BT_AUD_AVRCP_MSG:
            bt_avrcp_msg_handler(audio_msg->msg_data);
            break;
        case BT_AUD_A2DP_MSG:
            bt_a2dp_msg_handle(audio_msg->msg_data);
            break;
        default:
            break;
        }

        if (audio_msg->msg_data)
        {
            free(audio_msg->msg_data);
            audio_msg->msg_data = NULL;
        }

        if (audio_msg)
        {
            free(audio_msg);
        }

        audio_msg = NULL;
    }
}

void bt_app_audio_task_create(void)
{
    OSI_LOGI(0, "[BT] enter bt_app_audio_task_create");
    s_bt_audio_task_queue = osiMessageQueueCreate(10, sizeof(void *));
    if (s_bt_audio_task_queue != NULL)
    {
        osiThreadCreate("T_BT_AUDIO", bt_audio_task, NULL, OSI_PRIORITY_NORMAL /*OSI_PRIORITY_HIGH*/, BT_OS_THREAD_STACK_SIZE, 0);
    }
    else
    {
        OSI_LOGI(0, "Create bt audio task Fail");
    }
}

BT_STATUS app_bt_set_task_status(BT_SCHED_TASK_STATE_E status)
{
    s_bt_task_state = status;

    return BT_SUCCESS;
}

BT_STATUS app_bt_start(void)
{
    BT_STATUS ret = BT_PENDING;

    OSI_LOGI(0, "app_bt_start s_bt_state=%d", s_bt_state);

    switch (s_bt_state)
    {
    case BT_STOPING:
    case BT_STOPED:
        //        s_bt_state = BT_STARTING;
        bt_SendMsgToSchedTask(BT_START_SCHED_MSG, NULL);

        break;

    default:
        ret = BT_SUCCESS;
        break;
    }

    return ret;
}

void app_npi_bt_start(void)
{
    OSI_LOGI(0, "[BT] enter app_npi_bt_start");
    //register npi_btLoad first
    npi_btLoad_register();
    app_bt_start();
}

void app_npi_bt_stop(void)
{
    OSI_LOGI(0, "[BT] enter app_npi_bt_stop");
    npi_btLoad_deregister();
    app_bt_stop();
}

BOOLEAN app_bt_state_get(void)
{
    return (s_bt_state != BT_STOPED) ? (TRUE) : (FALSE);
}

void app_bt_protocol_state_set(uint8 pos, int flag)
{
    uint32 n = 1;

    n = n << pos;

    if (flag == BT_PROTOCOL_DISCONNECTED)
    {
        s_bt_protocol_connect_state = s_bt_protocol_connect_state & (~n);
    }
    else if (flag == BT_PROTOCOL_CONNECTED)
    {
        s_bt_protocol_connect_state = s_bt_protocol_connect_state | n;
    }
}

uint8 app_bt_specific_protocol_state_get(uint8 pos)
{
    if ((s_bt_protocol_connect_state >> pos) & 0x0001)
    {
        return BT_PROTOCOL_CONNECTED;
    }
    else
    {
        return BT_PROTOCOL_DISCONNECTED;
    }
}

const char *app_bt_version_info_get(void)
{
    return g_BuildRevision;
}

BT_STATUS app_bt_local_name_set(const uint16 *name)
{
    BT_STATUS ret = BT_PENDING;
    uint8 deviceName[MAX_BT_DEVICE_NAME];

    if (NULL != name)
    {
        // Save local name and from unicode to utf8.
        DB_SaveLocalNameAndToUTF8(name, deviceName);
    }
    else
    {
        return BT_ERROR;
    }

    if ((s_bt_state != BT_STOPED) && (s_bt_state != BT_STOPING))
    {
        // Change bt local name
        bt_local_name_change(deviceName);
    }
    else
    {
        // finished.
        ret = BT_SUCCESS;
    }

    return ret;
}

BT_STATUS app_bt_local_name_get(uint16 *name)
{
    DB_GetLocalName(name);

    return BT_SUCCESS;
}

BT_STATUS app_ble_adv_enable(bool adv_en)
{
    BT_STATUS ret = BT_ERROR;

    if (s_bt_state != BT_STARTED)
    {
        return ret;
    }

    OSI_LOGI(0, "[BT] enter app_ble_adv_enable: %d", adv_en);

    if (adv_en == false)
    {
        s_at_bt_state = BT_NONE_STATE;
        ret = (BT_SUCCESS != ble_adv_disable()) ? BT_ERROR : BT_SUCCESS;
    }
    else if (adv_en == true)
    {
#if 1
        s_at_bt_state = BT_ADV_STATE;
        ret = (BT_SUCCESS != ble_adv_enable()) ? BT_ERROR : BT_SUCCESS;
#else
        //Gatt_server_demo_function();//call this to run ble server demo, just for debug
        Gatt_client_demo_function(); //call this to run ble client demo, just for debug
        return 0;
#endif
    }
    else
    {
        // do nothing
    }

    return ret;
}

void bt_device_search_state_change(uint32 state)
{
    s_bt_inquiry_state = state;
}

BT_STATUS app_bt_device_search_start(uint32 service_type)
{
    uint8 max_result = 50;
    uint8 is_liac = 0;

    if (s_bt_inquiry_state == BT_INQ_PENDING)
    {
        return BT_ERROR;
    }

    g_inquiry_service_type = service_type;

    bt_device_search_start(max_result, is_liac);

    bt_device_search_state_change(BT_INQ_PENDING);

    return BT_PENDING;
}

BT_STATUS app_bt_device_search_cancel(void)
{
    if (s_bt_inquiry_state != BT_INQ_PENDING)
    {
        return BT_ERROR;
    }

    bt_device_search_cancel();

    return BT_SUCCESS;
}

uint32 app_bt_device_search_state_get(void)
{
    return 0;
}

uint32 app_bt_searched_device_count_get(void)
{
    return 0;
}

uint32 app_bt_searched_device_info_get(uint8 *addr, uint8 *name, uint8 idx)
{
    return 1;
}

BT_STATUS app_bt_device_pair(const BT_ADDRESS *btaddr)
{
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, btaddr->addr, SIZE_OF_BDADDR);

    bt_device_pair(&bdAddr);

    return BT_PENDING;
}

BT_STATUS app_bt_device_pair_cancel(const BT_ADDRESS *btaddr)
{
    BT_STATUS status = BT_PENDING;
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, btaddr->addr, SIZE_OF_BDADDR);

    if (0 == DB_RemovePairedInfo(bdAddr))
    {
        bt_device_pair_cancel(&bdAddr);
    }
    else
    {
        status = BT_ERROR;
    }

    return status;
}

int app_bt_paired_device_num_get(uint32 service_type)
{
    return DB_GetPairedNum(service_type);
}

BT_STATUS app_bt_paired_device_info_get(uint32 service_type, int index, BT_DEVICE_INFO *info)
{
    bt_dm_device_t bt_dm_device;
    BT_STATUS RetVal = BT_ERROR;

    Assert(info); /*assert verified*/

    if (0 == DB_GetPairedInfoFormIndex(index, &bt_dm_device))
    {
        UTF8toUnicode(bt_dm_device.device_name, 0, info->name, MAX_BT_DEVICE_NAME);
        memcpy((uint8 *)&info->addr, (uint8 *)&bt_dm_device.addr, sizeof(BT_ADDRESS));
        info->dev_class = bt_dm_device.cod;
        info->rssi = bt_dm_device.rssi;
        RetVal = BT_SUCCESS;
    }

    return RetVal;
}

BT_STATUS app_bt_paired_device_remove(const BT_ADDRESS *addr)
{
    BT_STATUS status = BT_SUCCESS;
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, addr->addr, SIZE_OF_BDADDR);
    if (0 == DB_RemovePairedInfo(bdAddr))
    {
        bt_paired_device_remove(&bdAddr);
    }
    else
    {
        status = BT_ERROR;
    }

    return status;
}

BT_STATUS app_bt_public_addr_set(BT_ADDRESS *addr)
{
    memcpy(&s_bt_storage.adapter.local_addr, addr->addr, SIZE_OF_BDADDR);

    BT_UpdateAddrToNV(addr);

    return BT_SUCCESS;
}

BT_STATUS app_bt_public_addr_get(BT_ADDRESS *addr)
{
    memcpy(addr->addr, &s_bt_storage.adapter.local_addr, SIZE_OF_BDADDR);
    return BT_SUCCESS;
}

bt_status_t app_ble_local_name_set(const char *name, uint8 len)
{
    ble_local_name_set(name, len);
    return BT_SUCCESS;
}

char *app_ble_local_name_get(void)
{
    return ble_local_name_get();
}

void app_add_ble_whitelist(bdaddr_t *addr, UINT8 addr_type)
{
    g_AddWhiteInfo.addr_type = addr_type;
    memcpy(g_AddWhiteInfo.addr, addr->bytes, 6);

    ble_white_list_add(addr, addr_type);
}

void app_remove_ble_whitelist(bdaddr_t *addr, UINT8 addr_type)
{
    g_RemoveWhiteInfo.addr_type = addr_type;
    memcpy(g_RemoveWhiteInfo.addr, addr->bytes, 6);

    ble_white_list_remove(addr, addr_type);
}

void app_clear_ble_whitelist(void)
{
    ble_white_list_clear();
}

static char *del_char(char *s, int len, char c)
{
    int i = 0, j = 0;
    for (i = 0; i < len; i++)
    {
        if (s[i] != c)
        {
            s[j++] = s[i];
        }
    }
    s[j] = '\0';
    return (s);
}
void app_ble_adv_data_set(uint8 len, char *data)
{
    //BT_STATUS status = BT_PENDING;
    UINT8 adv_data[31] = {0};
    UINT8 i = 0, index = 0;
    char *adv_data_tmp = del_char(data, strlen(data), ' ');
    char hex_data[2] = {0};

    for (i = 0; i < strlen(adv_data_tmp);)
    {
        memcpy(hex_data, &adv_data_tmp[i], 2);
        adv_data[index] = strtoul(hex_data, NULL, 16) & 0xFF;
        //printf("\nlength:%d,adv_data[%d]:%x\n",strlen(adv_data_tmp),index,adv_data[index]);
        index++;
        i += 2;
    }

    ble_adv_data_set(adv_data, len);
}

uint8 app_ble_adv_para_set(unsigned short AdvMin, unsigned short AdvMax, unsigned char AdvType, unsigned char ownAddrType, unsigned char directAddrType, uint8 *directAddr, unsigned char AdvChannMap, unsigned char AdvFilter)
{
    gatt_adv_param_t adv_param;

    adv_param.advMin = AdvMin;
    adv_param.advMax = AdvMax;
    adv_param.adv_type = AdvType;
    adv_param.peer_addr_type = directAddrType;
    adv_param.adv_map = AdvChannMap;
    adv_param.advFilter = AdvFilter;
    memcpy(adv_param.peer_addr, directAddr, SIZE_OF_BDADDR);

    ble_adv_param_set(&adv_param, ownAddrType);

    return 0;
}

uint8 app_ble_connect(UINT8 addr_type, bdaddr_t *addr)
{
    gatt_client_role = TRUE;
    ble_connect(addr, BT_MODULE_GATT, addr_type);
    return 0;
}

uint8 app_ble_disconnect(UINT16 handle)
{
    ble_disconnect(handle);
    return 0;
}

uint8 app_ble_connect_para_update(unsigned short Handle, unsigned short MinInterval,
                                  unsigned short MaxInterval, unsigned short Latency, unsigned short Timeout)
{
    gatt_connect_param_t param;

    param.connect_handle = Handle;
    param.connect_interval_min = MinInterval;
    param.connect_interval_max = MaxInterval;
    param.slave_latency = Latency;
    param.super_timeout = Timeout;

    ble_connect_para_update(&param);
    return 0;
}

BT_STATUS app_bt_scan_mode_set(uint8 discoverable, uint8 connectable)
{
    uint8 scan_mode;

    scan_mode = (connectable << 4) | discoverable;

    bt_visibility_set(scan_mode);

    DB_SetVisibility(scan_mode);

    if (scan_mode) //visiable request
    {
    }
    else // invisiable request
    {
    }

    return BT_SUCCESS;
}

uint8 app_bt_scan_mode_get(void)
{
    return DB_GetVisibility();
}

BT_STATUS app_ble_scan_rsp_data_set(uint8 *data, uint8 length)
{
    ble_scan_rsp_data_set(data, length);

    return 0;
}

BT_STATUS app_ble_scan_para_set(UINT8 type, UINT16 interval, UINT16 window, UINT8 filter, UINT8 own_addr_type)
{
    gatt_scan_param_t scan_param;

    scan_param.scanType = type;
    scan_param.scanInterval = interval;
    scan_param.scanWindow = window;
    scan_param.filterPolicy = filter;

    ble_scan_param_set(&scan_param, own_addr_type);

    return 0;
}

bt_status_t app_ble_scan_enable(unsigned char ScanEnable)
{
    if (1 == ScanEnable)
    {
        s_at_bt_state = BT_SCAN_STATE;
        return ble_scan_enable();
    }

    if (0 == ScanEnable)
    {
        s_at_bt_state = BT_NONE_STATE;
        return ble_scan_disable();
    }

    return BT_ERROR;
}

#if (BT_SPP_SUPPORT)
extern uint32 app_bt_spp_state_get(void);
bool app_bt_is_spp_idle(void)
{
    return ((app_bt_spp_state_get() == BTSPP_CONNECTION_STATE_DISCONNECTED) ? true : false);
}

bool app_bt_is_spp_connected(void)
{
    return ((app_bt_spp_state_get() == BTSPP_CONNECTION_STATE_CONNECTED) ? true : false);
}

extern bool app_bt_spp_connect_req(bdaddr_t *bdAddr);
BT_STATUS app_bt_spp_connect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed)
{
    if (NULL == bd_addr)
    {
        return BT_ERROR;
    }

    bdaddr_t bdAddr;
    memcpy(bdAddr.bytes, bd_addr->addr, SIZE_OF_BDADDR);

    if (false == app_bt_spp_connect_req(&bdAddr))
    {
        return BT_ERROR;
    }

    return BT_PENDING;
}

extern bool app_bt_spp_disconnect_req(void);
BT_STATUS app_bt_spp_disconnect(void)
{
    if (false == app_bt_spp_disconnect_req())
    {
        return BT_ERROR;
    }

    return BT_PENDING;
}

extern bool app_bt_spp_data_send_req(const uint8 *src_buf, uint32 data_len);
uint32 app_bt_spp_data_send(const uint8 *src_buf, uint32 data_len)
{
    uint32 real_length = 0;

    if (src_buf == NULL)
    {
        return real_length;
    }

    real_length = (data_len > 512) ? 512 : data_len;

    if (app_bt_spp_data_send_req(src_buf, real_length) == false)
    {
        return 0;
    }

    return real_length;
}
#endif

#if (BT_HFP_SUPPORT)
extern bool app_bt_hfp_connected_check(bdaddr_t *bdAddr);
bool app_bt_hfp_conn_status_get(const BT_ADDRESS *addr)
{
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, addr->addr, SIZE_OF_BDADDR);

    return app_bt_hfp_connected_check(&bdAddr);
}

extern bool app_bt_hfp_connect_req(bdaddr_t *pAddr);
BT_STATUS app_bt_hfp_connect(const BT_ADDRESS *addr)
{
    BT_STATUS status = BT_PENDING;
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, addr->addr, SIZE_OF_BDADDR);

    if (app_bt_hfp_connect_req(&bdAddr) == false)
    {
        status = BT_ERROR;
    }

    return status;
}

extern bool app_bt_hfp_disconnect_req(bdaddr_t *pAddr);
BT_STATUS app_bt_hfp_disconnect(const BT_ADDRESS *addr)
{
    BT_STATUS status = BT_PENDING;
    bdaddr_t bdAddr;

    memcpy(bdAddr.bytes, addr->addr, SIZE_OF_BDADDR);

    if (app_bt_hfp_disconnect_req(&bdAddr) == false)
    {
        status = BT_ERROR;
    }

    return status;
}

extern uint8 app_bt_hfp_call_state_get(void);
UINT8 app_bt_hfp_callstate_get(void)
{
    return app_bt_hfp_call_state_get();
}

extern bthfp_callsetup_state_t app_bt_hfp_call_setup_state_get(void);
UINT8 app_bt_hfp_call_setup_status_get(void)
{
    return (UINT8)app_bt_hfp_call_setup_state_get();
}

extern bthfp_connection_state_t app_bt_hfp_connection_state_get(void);
bool app_bt_hfp_one_connected_link_exist(void)
{
    if (app_bt_hfp_connection_state_get() == BTHFP_CONNECTION_STATE_DISCONNECTED)
    {
        return false;
    }

    return true;
}

extern bool app_bt_hfp_volume_set(uint8 vol);
BT_STATUS app_bt_hfp_set_vol(UINT8 Vol)
{
    BT_STATUS status = BT_SUCCESS;

    audevSetVoiceVolume(((Vol * 100) / 15));

    if (app_bt_hfp_volume_set(Vol) == false)
    {
        status = BT_ERROR;
    }

    return status;
}

extern bool app_bt_hfp_call_reject(void);
BT_STATUS app_bt_hfp_call_terminate(void)
{
    BT_STATUS status = BT_SUCCESS;
    OSI_LOGI(0, "app_bt_hfp_call_terminate");

    if (app_bt_hfp_call_reject() == false)
    {
        status = BT_ERROR;
    }

    return status;
}

extern bool app_bt_hfp_call_answer_accept(void);
extern bool app_bt_hfp_call_answer_reject(void);
BT_STATUS app_bt_hfp_call_answer(UINT8 call_status)
{
    BT_STATUS status;
    bool ret;
    OSI_LOGI(0, "app_bt_hfp_call_answer");

    if (call_status)
    {
        ret = app_bt_hfp_call_answer_accept();
    }
    else
    {
        ret = app_bt_hfp_call_answer_reject();
    }

    status = (ret) ? BT_SUCCESS : BT_ERROR;

    return status;
}

extern bool app_bt_hfp_dial_req(UINT8 *phone_number);
extern bool app_bt_hfp_redial_req(void);
BT_STATUS app_bt_hfp_dial(UINT8 *phone_number)
{
    BT_STATUS status;
    bool ret;

    if (phone_number)
    {
        ret = app_bt_hfp_dial_req(phone_number);
    }
    else
    {
        ret = app_bt_hfp_redial_req();
    }

    status = (ret) ? BT_SUCCESS : BT_ERROR;

    return status;
}

extern bool app_bt_hfp_vr_enable(void);
extern bool app_bt_hfp_vr_disable(void);
BT_STATUS app_bt_hfp_vr_set(bool enable)
{
    BT_STATUS status;
    bool ret;

    if (enable)
    {
        ret = app_bt_hfp_vr_enable();
    }
    else
    {
        ret = app_bt_hfp_vr_disable();
    }

    status = (ret) ? BT_SUCCESS : BT_ERROR;

    return status;
}

extern bool app_bt_hfp_three_way_call_ctrl(UINT8 CtrlCmd);
BT_STATUS app_bt_hfp_ThreeWayCall(UINT8 CtrlCmd)
{
    BT_STATUS status;
    bool ret;

    ret = app_bt_hfp_three_way_call_ctrl(CtrlCmd);

    status = (ret) ? BT_SUCCESS : BT_ERROR;

    return status;
}

#endif

#if (BT_AVRCP_SUPPORT)
extern bool app_bt_avrcp_start(void);
extern bool app_bt_avrcp_pause(void);
extern bool app_bt_avrcp_previ(void);
extern bool app_bt_avrcp_next(void);
bool app_bt_avrcp_handle(bt_handle_avrcp_state_t avrcp_state)
{
    bool ret;

    switch (avrcp_state)
    {
    case BT_AVRCP_START:
        ret = app_bt_avrcp_start();
        break;
    case BT_AVRCP_PAUSE:
        ret = app_bt_avrcp_pause();
        break;
    case BT_AVRCP_PREVI:
        ret = app_bt_avrcp_previ();
        break;
    case BT_AVRCP_NEXT:
        ret = app_bt_avrcp_next();
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

extern t_AVRCP_PLAYBACK_STATUS app_bt_avrcp_play_state_get(void);
INT32 app_bt_avrcp_status_query(void)
{
    t_AVRCP_PLAYBACK_STATUS state;

    state = app_bt_avrcp_play_state_get();

    if (AVRCP_PLAYBACK_STATUS_ERROR == state)
    {
        return -1;
    }

    return (INT32)state;
}

extern bool app_bt_avrcp_vol_set(uint8 volume);
bool app_bt_avrcp_volume_set(UINT8 volume)
{
    return app_bt_avrcp_vol_set(volume);
}

extern int8 app_bt_avrcp_vol_get(void);
int8 app_bt_avrcp_volume_get(void)
{
    return app_bt_avrcp_vol_get();
}
#endif
