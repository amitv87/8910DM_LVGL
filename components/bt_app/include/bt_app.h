#ifndef BT_APP_H
#define BT_APP_H

#include "sci_types.h"
#include "bt_abs.h"
#include "quec_proj_config.h"

#define BT_OS_THREAD_STACK_SIZE (1024 * 4)
#define BT_THREAD_QUEUE_NUM (100)

#define BT_DEVICE_NAME_SIZE 41
#define BT_MAX_DEVICE_NUM 40

#define BT_SPP_SUPPORT 0
#define BT_BLE_GATT_CLIENT_SUPPORT 1
#define BT_BLE_GATT_SERVER_SUPPORT 1
#define BT_HFP_SUPPORT 1
#define BT_AVRCP_SUPPORT 1
#define BT_A2DP_SUPPORT 1

#define BT_OS_MSG_START (0x100)
#define BT_FP_MSG_START (0x200)
#define BT_APP_MSG_START (0x300)

#define BLE_SMP_SUPPORT 0

/******************************************************************************
 * BT service type
 ******************************************************************************/
#define BT_SERVICE_NONE 0x0000
#define BT_SERVICE_HANDFREE 0x0001
#define BT_SERVICE_HANDSET 0x0002
#define BT_SERVICE_OPP 0x0004
#define BT_SERVICE_FTP 0x0008
#define BT_SERVICE_A2DP 0x0010
#define BT_SERVICE_AVRCP 0x0020
#define BT_SERVICE_SPP 0x0040
#define BT_SERVICE_A2DP_SRC 0x0080
#define BT_SERVICE_AVRCP_TG 0x0100
#define BT_SERVICE_MAP_SERVER 0x0200
#define BT_SERVICE_PBAP_SERVER 0x0400
#define BT_SERVICE_HANDFREE_GATEWAY 0x0800
#define BT_SERVICE_HEADSET_GATEWAY 0x1000
#define BT_SERVICE_PBAP_CLIENT 0x2000
#define BT_SERVICE_BLE_HID 0x3000
#define BT_SERVICE_ALL 0xFFFF

/***********************************************************************************
* Uart Define
************************************************************************************/
#define BT_COM0 0
#define BT_COM1 1
#define BT_COM2 2
#define BT_COM3 3
#define BT_COM_MAX 4

/***********************************************************************************
* Uart Baudrate Define
************************************************************************************/
#define BT_BAUD_1200 1200
#define BT_BAUD_2400 2400
#define BT_BAUD_4800 4800
#define BT_BAUD_9600 9600
#define BT_BAUD_19200 19200
#define BT_BAUD_38400 38400
#define BT_BAUD_57600 57600
#define BT_BAUD_115200 115200
#define BT_BAUD_230400 230400
#define BT_BAUD_460800 460800
#define BT_BAUD_921600 921600
#define BT_BAUD_1382400 1382400
#define BT_BAUD_3250000 3250000

enum
{
    LEGACY_JUSTWORK,
    LEGACY_PASSKEY,
    SECURE_CONNECTION_JUSTWORK,
    SECURE_CONNECTION_PASSKEY,
};

#define DISPLAY_ONLY 0x00
#define DISPLAY_YESNO 0x01
#define KEYBOARD_ONLY 0x02
#define NOIN_NOOUT 0x03
#define KEYBOARD_DISP 0x04

typedef enum
{
    BT_START_SCHED_MSG = BT_OS_MSG_START,
    BT_STOP_SCHED_MSG,
    BT_INT_HANDLE_MSG,
    BT_GSM_L1_INFO_MSG,
    BT_START_CFM_MSG,
    BT_STOP_CFM_MSG,
    BT_START_GATT_CLIENT_MSG
} CSR_BT_SCHED_MSG_E;

typedef enum
{
    BT_AVRCP_PAUSE = 0x00,
    BT_AVRCP_START,
    BT_AVRCP_PREVI,
    BT_AVRCP_NEXT,
} bt_handle_avrcp_state_t;

enum
{
    BT_MODULE_HOST_CORE = 0,
    BT_MODULE_ADP = 1,
    BT_MODULE_HCI = 2,
    BT_MODULE_MANAGER = 3,
    BT_MODULE_L2CAP = 4,
    BT_MODULE_AMP = 5,
    BT_MODULE_SDP_CLI = 6,
    BT_MODULE_SDP_SRV = 7,
    BT_MODULE_RFCOMM = 8,
    BT_MODULE_OBEX_CLI = 9,
    BT_MODULE_OBEX_SRV = 10,
    BT_MODULE_A2DP = 11,
    BT_MODULE_AVRCP = 12,
    BT_MODULE_HFP = 13,
    BT_MODULE_HID = 14,
    BT_MODULE_TCS = 15,
    BT_MODULE_SPP = 16,
    BT_MODULE_HCRP = 17,
    BT_MODULE_PAN = 18,
    BT_MODULE_ATT = 19,
    BT_MODULE_SMP = 20,
    BT_MODULE_GATT = 21,
    BT_MODULE_MESH = 22,
    BT_MAX_MODULE = 36
};

typedef enum
{
    BT_STOPED,
    BT_STOPING,
    BT_STARTING,
    BT_STARTED
} BT_STATE_E;

typedef enum
{
    BT_NONE_STATE = 0x00,
    BT_ADV_STATE,
    BT_SCAN_STATE,
    BT_CONN_STATE
} bt_at_state_t;

typedef enum
{
    BT_TASK_STOP,
    BT_TASK_STARTED
} BT_SCHED_TASK_STATE_E;

typedef enum
{
    OS_TIME_EVENT = 0x1000,
    OS_MSG_EXTEND_EVM = 0X1001,
    OS_MSG_EXTEND_TRIG_DATA = 0X1002,
    OS_MSG_SAVE_PCM_DATA = 0X1003,
} OS_INT_MSG_TYPE_E;

typedef struct _st_white_list_info
{
    uint8 addr_type;
    uint8 addr[SIZE_OF_BDADDR];
} st_white_list_info;

typedef struct _st_white_list
{
    uint8 is_used;
    st_white_list_info white_list_info;
} st_white_list;

typedef struct _st_scan_report_info
{
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    UINT8 addr_type;
    BT_ADDRESS bdAddress;
    UINT8 event_type;
    UINT8 data_length;
    UINT8 rssi;
} st_scan_report_info;

typedef struct _st_at_scan_report_info
{
    uint8 num;
    st_scan_report_info report_info[10];
} st_at_scan_report_info;

typedef struct _BT_DEVICE_INFO
{
    uint16 name[BT_DEVICE_NAME_SIZE];
    BT_ADDRESS addr;
    uint32 dev_class;
    uint32 rssi;
} BT_DEVICE_INFO;

typedef struct _BT_PCM_CONFIG
{
    uint8 slot_number; /* starting from 0 */
    uint32 config;     //config the pcm of bt chip
    uint32 format;
} BT_PCM_CONFIG;

typedef struct bt_config_info_tag
{
    BT_ADDRESS bd_addr;
    uint16 local_name[BT_DEVICE_NAME_SIZE];
    uint16 local_ble_name[BT_DEVICE_NAME_SIZE];
    uint32 port_num;
    uint32 baud_rate;
    BT_PCM_CONFIG pcm_config;
    uint16 bt_file_dir[16];
    uint32 bt_task_priority;
    uint32 bt_crystal_freq; //KHZ
    uint16 xtal_dac;
    uint32 host_nv_ctrl[2];
} bt_config_info_t;

typedef struct bt_nv_param_tag
{
    BT_ADDRESS bd_addr;
    uint8 local_name[32];
    uint16 xtal_dac;
} bt_nv_param_t;

typedef struct bt_pskey_nv_info_tag
{
    uint8 g_dbg_source_sink_syn_test_data;
    uint8 g_sys_sleep_in_standby_supported;
    uint8 g_sys_sleep_master_supported;
    uint8 g_sys_sleep_slave_supported;

    uint32 default_ahb_clk;
    uint32 device_class;
    uint32 win_ext;
    uint32 g_aGainValue[6];
    uint32 g_aPowerValue[5];

    uint8 feature_set[16];
    uint8 device_addr[6];
    uint8 g_sys_sco_transmit_mode;             //true tramsmit by uart, otherwise by share memory
    uint8 g_sys_uart0_communication_supported; //true use uart0, otherwise use uart1 for debug

    uint8 edr_tx_edr_delay;
    uint8 edr_rx_edr_delay;
    uint16 g_wbs_nv_117;

    uint32 is_wdg_supported;
    uint32 share_memo_rx_base_addr;
    uint32 share_memo_tx_base_addr;
    uint32 share_memo_tx_packet_num_addr;
    uint32 share_memo_tx_data_base_addr;
    uint32 g_PrintLevel;

    uint16 share_memo_tx_block_length;
    uint16 share_memo_rx_block_length;

    uint16 share_memo_tx_water_mark;
    uint16 share_memo_tx_timeout_value;

    uint16 uart_rx_watermark;
    uint16 uart_flow_control_thld;

    uint32 comp_id;

    uint16 pcm_clk_divd;
    uint16 g_nbs_nv_118;

    uint16 br_edr_diff_reserved;
    uint16 br_edr_powercontrol_flag;

    uint32 gain_br_channel_power[8];
    uint32 gain_edr_channel_power[8];
    uint32 g_aLEPowerControlFlag;
    uint16 g_aLEChannelpwrvalue[8];
    uint32 host_reserved[2];
    uint32 config0;
    uint32 config1;
    uint32 config2;
    uint32 config3;
    //uint16 bt_calibration_flag;//this is NOT in NV bt_sprd, this is defined in host/controller PSK structrue
    //uint16 bt_log_set;//this is NOT in NV bt_sprd, this is defined in host/controller PSK structrue
    uint32 reserved1[4];
    //uint32  chip_version;// 0 old 8910: 1 new 8910 -----
    uint16 reserved2[3];
} bt_pskey_nv_info_t;

typedef struct _BT_AT_SPP_DATA
{
    UINT8 *buf;
    UINT16 dataLen;
} BT_AT_SPP_DATA;

typedef struct BT_INT_MSG_T
{
    unsigned int message[4];
} BT_INT_MSG_T;

typedef struct BT_SIGNAL_T
{
    uint16 SignalCode;
    uint16 SignalSize;
    void *SigP;
} BT_SIGNAL_T;

typedef enum
{
    BT_AUD_AVRCP_MSG = 0xA500,
    BT_AUD_A2DP_MSG,
} BT_AUDIO_MSG_E;

typedef struct bt_audio_msg_tag
{
    uint16 msg_id;
    uint16 reserved;
    void *msg_data;
} bt_audio_msg_t;

typedef struct _BT_NONSIG_DATA
{
    uint8 rssi;
    BT_STATUS status;
    uint32 pkt_cnt;
    uint32 pkt_err_cnt;
    uint32 bit_cnt;
    uint32 bit_err_cnt;
} BT_NONSIG_DATA;
typedef void (*BT_NONSIGCALLBACK)(const BT_NONSIG_DATA *data);

typedef struct _BT_NONSIG_PACKET
{
    uint16 pac_type; //enum{ NULLpkt, POLLpkt, FHSpkt, DM1, DH1, HV1, HV2, HV3, DV, AUX1, DM3, DH3, EV4, EV5, DM5, DH5, IDpkt, INVALIDpkt, EDR_2DH1, EV3, EDR_2EV3, EDR_3EV3, EDR_3DH1, EDR_AUX1, EDR_2DH3, EDR_3DH3, EDR_2EV5, EDR_3EV5, EDR_2DH5, EDR_3DH5};
    uint16 pac_len;  //according to packet type, max length is {0, 0, 18, 17, 27, 10, 20, 30, 9, 29, 121, 183, 120, 180, 224, 339, 0, 0, 54, 30, 60, 90, 83, 29, 367, 552, 360, 540, 679, 1021}
    uint16 pac_cnt;  //0~65536
} BT_NONSIG_PACKET;

typedef struct _BT_NONSIG_POWER
{
    uint8 power_type;   //0 or 1
    uint16 power_value; //0~33
} BT_NONSIG_POWER;

typedef struct _BT_NONSIG_PARAM
{
    uint8 pattern; //00000000 ---1, 11111111 ---2, 10101010 ---3, PRBS9 ---4, 11110000 ---9
    uint8 channel; //255 or 0~78
    union {
        uint8 rx_gain; //0~32
        BT_NONSIG_POWER power;
    } item;
    BT_NONSIG_PACKET pac;
    uint8 le_phy;          // 1:LE1M, 2:LE2M, 3:LE Coded S8, 4:LE Coded S2 |  3:LE Coded in RX
    uint8 le_rx_mod_index; // 0:Standard;  1:Stable
} BT_NONSIG_PARAM;

typedef struct _BT_MSG_Tag
{
    uint32 msg_id;
    uint8 status;
    uint32 body_size;
    void *body_ptr;
} BT_MSG_T;

bool bt_SendMsgToSchedTask(uint32 event, void *msg_ptr);

void app_npi_bt_start(void);

void app_npi_bt_stop(void);

BT_STATUS app_bt_start(void);

BT_STATUS app_bt_stop(void);

BOOLEAN app_bt_state_get(void);

BT_STATUS app_bt_local_name_set(const uint16 *name);

const char *app_bt_version_info_get(void);

BT_STATUS app_ble_adv_enable(bool adv_en);

BT_STATUS app_bt_device_search_start(uint32 service_type);

BT_STATUS app_bt_device_search_cancel(void);

uint32 app_bt_device_search_state_get(void);

uint32 app_bt_searched_device_count_get(void);

uint32 app_bt_searched_device_info_get(uint8 *addr, uint8 *name, uint8 idx);

uint8 app_ble_connect(UINT8 addr_type, bdaddr_t *addr);

uint8 app_ble_disconnect(UINT16 handle);

void app_ble_adv_data_set(uint8 len, char *data);

uint8 app_ble_adv_para_set(unsigned short AdvMin, unsigned short AdvMax, unsigned char AdvType, unsigned char ownAddrType,
                           unsigned char directAddrType, uint8 *directAddr, unsigned char AdvChannMap, unsigned char AdvFilter);

uint8 app_ble_connect_para_update(unsigned short Handle, unsigned short MinInterval,
                                  unsigned short MaxInterval, unsigned short Latency, unsigned short Timeout);

BT_STATUS app_bt_scan_mode_set(uint8 discoverable, uint8 connectable);

uint8 app_bt_scan_mode_get(void);

BT_STATUS app_bt_local_name_get(uint16 *name);

BT_STATUS app_bt_public_addr_set(BT_ADDRESS *addr);

BT_STATUS app_bt_public_addr_get(BT_ADDRESS *addr);

BT_STATUS app_bt_device_pair(const BT_ADDRESS *btaddr);

BT_STATUS app_bt_device_pair_cancel(const BT_ADDRESS *btaddr);

int app_bt_paired_device_num_get(uint32 service_type);

BT_STATUS app_bt_paired_device_info_get(uint32 service_type, int index, BT_DEVICE_INFO *info);

BT_STATUS app_bt_paired_device_remove(const BT_ADDRESS *addr);

bool app_bt_is_spp_idle(void);

bool app_bt_is_spp_connected(void);

BT_STATUS app_bt_spp_connect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);

BT_STATUS app_bt_spp_disconnect(void);

bool app_bt_hfp_conn_status_get(const BT_ADDRESS *addr);

BT_STATUS app_bt_hfp_connect(const BT_ADDRESS *addr);

BT_STATUS app_bt_hfp_disconnect(const BT_ADDRESS *addr);

UINT8 app_bt_hfp_callstate_get(void);

UINT8 app_bt_hfp_call_setup_status_get(void);

bool app_bt_hfp_one_connected_link_exist(void);

BT_STATUS app_bt_hfp_set_vol(UINT8 Vol);

BT_STATUS app_bt_hfp_call_terminate(void);

BT_STATUS app_bt_hfp_call_answer(UINT8 call_status);

BT_STATUS app_bt_hfp_dial(UINT8 *phone_number);

BT_STATUS app_bt_hfp_vr_set(bool enable);

BT_STATUS app_bt_hfp_ThreeWayCall(UINT8 CtrlCmd);

bool app_bt_avrcp_handle(bt_handle_avrcp_state_t avrcp_state);

INT32 app_bt_avrcp_status_query(void);

bool app_bt_avrcp_volume_set(UINT8 volume);

int8 app_bt_avrcp_volume_get(void);

BT_STATUS app_ble_scan_rsp_data_set(uint8 *data, uint8 length);

BT_STATUS app_ble_scan_para_set(UINT8 type, UINT16 interval, UINT16 window, UINT8 filter, UINT8 own_addr_type);

bt_status_t app_ble_scan_enable(unsigned char ScanEnable);

void app_add_ble_whitelist(bdaddr_t *addr, UINT8 addr_type);

void app_remove_ble_whitelist(bdaddr_t *addr, UINT8 addr_type);

void app_clear_ble_whitelist(void);

bt_status_t app_ble_local_name_set(const char *name, uint8 len);

char *app_ble_local_name_get(void);

uint32 app_bt_spp_data_send(const uint8 *src_buf, uint32 data_len);

uint32 app_bt_get_at_cmd_state(void);

BT_STATUS app_bt_a2dp_connect(const BT_ADDRESS *addr);
BT_STATUS app_bt_a2dp_disconnect(void);
bta2dp_connection_state_t app_bt_a2dp_connect_state(void);
void bt_app_print_data(char *buff, int len);

#endif
