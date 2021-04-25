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

#ifndef __TB_H__
#define __TB_H__

#include "osi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

//macro
#define TB_SUCCESS 0
#define TB_FAILURE 1

#define TB_GSM_SIGNAL_BAR_5_VALUE 16
#define TB_GSM_SIGNAL_BAR_4_VALUE 13
#define TB_GSM_SIGNAL_BAR_3_VALUE 10
#define TB_GSM_SIGNAL_BAR_2_VALUE 7
#define TB_GSM_SIGNAL_BAR_1_VALUE 3

#define TB_LTE_SIGNAL_BAR_5_VALUE 51
#define TB_LTE_SIGNAL_BAR_4_VALUE 45
#define TB_LTE_SIGNAL_BAR_3_VALUE 39
#define TB_LTE_SIGNAL_BAR_2_VALUE 33
#define TB_LTE_SIGNAL_BAR_1_VALUE 26

#define TB_SIGNAL_BAR_5 "5"
#define TB_SIGNAL_BAR_4 "4"
#define TB_SIGNAL_BAR_3 "3"
#define TB_SIGNAL_BAR_2 "2"
#define TB_SIGNAL_BAR_1 "1"
#define TB_SIGNAL_BAR_0 "0"

#define TB_ROAMING_STATUS_HOME "Home"
#define TB_ROAMING_STATUS_INTERNAL "Internal"
#define TB_ROAMING_STATUS_INTERNATIONAL "International"

#define TB_NET_TYPE_GSM "GSM"
#define TB_NET_TYPE_LTE "LTE"

#define TB_CS_PS_STAUS_CS_ONLY "CS_ONLY"
#define TB_CS_PS_STAUS_PS_ONLY "PS_ONLY"
#define TB_CS_PS_STAUS_CS_PS "CS_PS"
#define TB_CS_PS_STAUS_CAMPED "CAMPED"

#define TB_NETWORK_TYPE_4G "4G"
#define TB_NETWORK_TYPE_2G "2G"
#define TB_NETWORK_TYPE_LIMITED "Limited Service"
#define TB_NETWORK_TYPE_NO_SERVICE "No Service"

#define TB_PROVIDER_LONG_NAME_SIZE 64
#define TB_PROVIDER_SHORT_NAME_SIZE 32
#define TB_NETWORK_TYPE_SIZE 32
#define TB_MCC_MNC_SIZE 3
#define TB_LAC_CODE_SIZE 2
#define TB_CELL_ID_SIZE 4
#define TB_NETWORK_BAND_SIZE 2

#define TB_PROFILE_PARA_LEN 128

#define TB_IPV4_ADDR_LEN 16

#define TB_DATA_STATISTICS_TIME_THRESHOLD 60

#define TB_SMS_SEND_NUM_MAX 5
#define TB_WMS_ADDRESS_LEN_MAX 21
#define TB_SMS_MSG_CONTENT_LEN_MAX 140
#define TB_SMS_CONCAT_SMS_COUNT_MAX 5

//enum
typedef enum tb_wan_notify_type_enum
{
    TB_WAN_NOTIFY_TYPE_SIG_BAR = 0,
    TB_WAN_NOTIFY_TYPE_ROAMING_STATUS,
    TB_WAN_NOTIFY_TYPE_NET_PROVIDER,
    TB_WAN_NOTIFY_TYPE_NET_TYPE,
    TB_WAN_NOTIFY_TYPE_CS_PS_STATUS,
    TB_WAN_NOTIFY_TYPE_CELL_ID_INFO
} tb_wan_notify_type_e;

typedef enum tb_wan_domain_state_enum
{
    TB_WAN_DOMIAN_STATE_UNKOWN = -1,
    TB_WAN_DOMAIN_STATE_CS_PS,
    TB_WAN_DOMAIN_STATE_CS_ONLY,
    TB_WAN_DOMAIN_STATE_PS_ONLY
} tb_wan_domain_state_e;

typedef enum lsdk_wan_roam_status_enum
{
    TB_WAN_ROAM_STATUS_UNKOWN = 0,
    TB_WAN_ROAM_STATUS_HOME,
    TB_WAN_ROAM_STATUS_INTERNAL,
    TB_WAN_ROAM_STATUS_INTERNATIONAL
} tb_wan_roam_status_e;

typedef enum
{
    TB_PROFILE_ID_MIN_ENUM_VAL = -2147483647,
    TB_PROFILE_ID_PUBLIC = 1,
    TB_PROFILE_ID_PRIVATE = 2,
    TB_PROFILE_ID_MAX_ENUM_VAL = 2147483647
} tb_data_profile_id;

typedef enum
{
    TB_DIAL_STATUS_MIN_ENUM_VAL = -2147483647,
    TB_DIAL_STATUS_CONNECTING = 0,
    TB_DIAL_STATUS_CONNECTED = 1,
    TB_DIAL_STATUS_DISCONNECTING = 2,
    TB_DIAL_STATUS_DISCONNECTED = 3,
    TB_DIAL_STATUS_MAX_ENUM_VAL = 2147483647
} tb_data_dial_status;

typedef enum
{
    TB_PDP_TYPE_MIN_ENUM_VAL = -2147483647,
    TB_PDP_TYPE_IPV4 = 0,
    TB_PDP_TYPE_IPV6 = 1,
    TB_PDP_TYPE_IPV4V6 = 2,
    TB_DATA_PDP_TYPE_UNUSED,
    TB_PDP_TYPE_MAX_ENUM_VAL = 2147483647,
} tb_data_pdp_type;

typedef enum
{
    TB_DATA_AUTH_MODE_MIN_ENUM_VAL = -2147483647,
    TB_DATA_AUTH_MODE_NONE = 0,
    TB_DATA_AUTH_MODE_PAP = 1,
    TB_DATA_AUTH_MODE_CHAP = 2,
    TB_DATA_AUTH_MODE_MAX_ENUM_VAL = 2147483647,
} tb_data_auth_mode;

typedef enum
{
    TB_SIM_STATUS_UNDETECTED,
    TB_SIM_STATUS_WAIT_PIN,
    TB_SIM_STATUS_WAIT_PUK,
    TB_SIM_STATUS_READY,
} tb_sim_status;

typedef enum
{
    TB_SIM_NV_UNREAD_SMS,
    TB_SMS_RECEIVED,
    TB_SMS_FULL_FLAG,
    TB_SMS_NV_TOTAL_SMS,
    TB_SMS_RECEIVED_WITH_ID,
    TB_SMS_DELETE_RESULT,
    TB_SMS_SEND_RESULT,
} tb_sms_status;

typedef enum
{
    TB_SMS_STORAGE_ME,
    TB_SMS_STORAGE_SM,
    TB_SMS_STORAGE_NS,
} tb_wms_storage_type;

typedef enum
{
    TB_VOICE_REPORT_CALL_INFO = 0,
    TB_VOICE_REPORT_MSD_TRANSMISSION_STATUS = 1,
} tb_voice_report_event_e;

typedef enum
{
    TB_VOICE_CALL_TYPE_VOICE,
    TB_VOICE_CALL_TYPE_VIDEO
} tb_voice_call_type;

typedef enum
{
    TB_VOICE_NOTIFY_CALL_START = 0,
    TB_VOICE_NOTIFY_CALL_INCOMING,
    TB_VOICE_NOTIFY_CALL_FAILED,
    TB_VOICE_NOTIFY_CALL_CONNECTING,
    TB_VOICE_NOTIFY_CALL_CONNECTED,
    TB_VOICE_NOTIFY_CALL_ENDED,
    TB_VOICE_NOTIFY_CALL_STATUS_MAX
} tb_voice_notify_call_status;

typedef enum
{
    TB_VOICE_PRESENTATION_NAME_PRESENTATION_ALLOWED = 0x00,
    TB_VOICE_PRESENTATION_NAME_PRESENTATION_RESTRICTED = 0x01,
    TB_VOICE_PRESENTATION_NAME_UNAVAILABLE = 0x02,
    TB_VOICE_PRESENTATION_NAME_NAME_PRESENTATION_RESTRICTED = 0x03,
} tb_voice_pi_name_type;

//struct
typedef struct
{
    char provider_long_name[TB_PROVIDER_LONG_NAME_SIZE];
    char provider_short_name[TB_PROVIDER_SHORT_NAME_SIZE];
    char net_type[TB_NETWORK_TYPE_SIZE];
    tb_wan_domain_state_e domain_state;
    tb_wan_roam_status_e roam_state;
    char mcc[TB_MCC_MNC_SIZE];
    char mnc[TB_MCC_MNC_SIZE];
    char lac_code[TB_LAC_CODE_SIZE];
    char cell_id[TB_CELL_ID_SIZE];
    char net_band[TB_NETWORK_BAND_SIZE];
} tb_wan_network_info_s;

typedef struct
{
    int signal_bar;
    int rssi;
    int rsrp;
    int rsrq;
    int rscp;
} tb_wan_signal_info_s;

typedef struct
{
    char config_name[TB_PROFILE_PARA_LEN];
    char apn[TB_PROFILE_PARA_LEN];
    char username[TB_PROFILE_PARA_LEN];
    char password[TB_PROFILE_PARA_LEN];
    tb_data_pdp_type pdp_type;
    tb_data_auth_mode auth_mode;
} tb_data_connect_profile;

typedef struct
{
    unsigned long long realtime_rx_bytes;
    unsigned long long realtime_tx_bytes;
    unsigned long long realtime_rx_thrpt;
    unsigned long long realtime_tx_thrpt;
    unsigned long long realtime_time;
    unsigned long long monthly_rx_bytes;
    unsigned long long monthly_tx_bytes;
    unsigned long long monthly_time;
    unsigned long long total_rx_bytes;
    unsigned long long total_tx_bytes;
    unsigned long long total_time;
} tb_data_statistics;

typedef struct
{
    char ip_addr[TB_IPV4_ADDR_LEN];
    char pref_dns_addr[TB_IPV4_ADDR_LEN];
    char standy_dns_addr[TB_IPV4_ADDR_LEN];
} tb_data_ipv4_addr;

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t izone;
} tb_sms_time_stamp;

typedef struct
{
    tb_wms_storage_type mem_store;
    unsigned char dest_num[TB_WMS_ADDRESS_LEN_MAX + 1];
    unsigned short msg_len;
    unsigned short msg_content[TB_SMS_MSG_CONTENT_LEN_MAX * TB_SMS_CONCAT_SMS_COUNT_MAX];
    long id;
    tb_sms_time_stamp date;
    unsigned char draft_group_id[32];
    unsigned char send_and_save_sms;
    int classtype;
} tb_sms_msg_type;

typedef struct
{
    int index;
    int rpt_index;
    int submit_index;
    int tag;
    char content[TB_SMS_MSG_CONTENT_LEN_MAX * TB_SMS_CONCAT_SMS_COUNT_MAX];
    char phone[41];
    int isfull;
    int total;
    int current_index;
    int received;
    int callsType;
    tb_sms_time_stamp date;
    char mem_store[10];
    int content_len;
    struct tb_sms_message *next;
} tb_sms_message;

typedef struct
{
    tb_voice_pi_name_type name_pi;
    unsigned char coding_scheme;
    unsigned int name_len;
    char name[182];
} tb_voice_remote_party_name_type;

typedef struct
{
    tb_voice_notify_call_status call_state;
    tb_voice_call_type call_type;
    int is_call_of_callsession;
    char number[32];
    tb_voice_remote_party_name_type remote_party_name;
    unsigned char remote_party_name_valid;
    int dial_result;
    int connect_result;
    int hungup_result;
} tb_voice_notify_call_state_number_info;

//type
typedef void (*tb_wan_network_info_cb)(tb_wan_notify_type_e type, const char *value);
typedef void (*tb_data_callback_fun)(tb_data_profile_id cid, tb_data_dial_status status);
typedef void (*tb_sim_callback_fun)(tb_sim_status type);
typedef void (*tb_sms_callback_fun)(tb_sms_status type, int value);
typedef void (*tb_sms_received_contents_cb)(tb_sms_message *received_sms_message);
typedef void (*tb_voice_callback_fun)(tb_voice_report_event_e event, void *voice_report_info);

typedef osiTimer_t *tb_timer_p;
typedef void (*tb_timer_callbck)(void *ctx);

//api
//Nw module
extern int tb_wan_reg_callback(tb_wan_network_info_cb wan_network_info_func);
extern int tb_wan_get_network_type(char *type, int size);
extern int tb_wan_get_network_info(tb_wan_network_info_s *network_info);
extern int tb_wan_get_signal_info(tb_wan_signal_info_s *signal_info);
extern int tb_data_reg_callback(tb_data_callback_fun fun);
extern int tb_data_wan_disconnect(tb_data_profile_id cid);
extern int tb_data_get_dial_status(tb_data_profile_id cid, tb_data_dial_status *istatus);
extern int tb_data_set_connect_parameter(tb_data_profile_id cid, tb_data_connect_profile para);
extern int tb_data_wan_connect(tb_data_profile_id cid);
extern int tb_data_get_connect_parameter(tb_data_profile_id cid, tb_data_connect_profile *para);
extern int tb_data_get_statistics(tb_data_profile_id cid, tb_data_statistics *statistics);
extern int tb_data_get_ipv4_address(tb_data_profile_id cid, tb_data_ipv4_addr *address);

// SIM module
extern int tb_sim_reg_callback(tb_sim_callback_fun fun);
extern int tb_sim_get_sim_state(tb_sim_status *istate);
extern int tb_sim_get_imsi(char *imsi, int size);
extern int tb_sim_get_imei(char *imei, int size);
extern int tb_sim_get_phone_num(char *pBuf, int size);

// SMS module
extern int tb_sms_reg_callback(tb_sms_callback_fun fun);
extern int tb_sms_get_contents_cb(tb_sms_received_contents_cb fun);
extern int tb_sms_send_sms(tb_sms_msg_type *msg);
extern int tb_sms_delete_sms(int MessageIndex);
extern int tb_sms_get_sms_withid(int id);

// Voice module
extern int tb_voice_reg_callback(tb_voice_callback_fun fun);
extern int tb_voice_dial_call(char *call_number);
extern int tb_voice_answer_call();
extern int tb_voice_hungup_call();
extern int tb_voice_switch_audio_channel(int audio_channel);

//device info
extern int tb_device_get_modemserialnumber(char *msn, int size);
extern int tb_device_get_hwversion(char *hdver, int size);
extern int tb_device_getversion(char *ver, int size);
extern int tb_device_get_iccid(char *iccid, int size);
extern int tb_device_set_rtc_timer(unsigned long seconds, tb_timer_callbck pCallback, tb_timer_p *pTimer);
extern int tb_device_cancel_timer(tb_timer_p pTimer);
extern int tb_device_shutdown_system(void);
extern int tb_device_reboot_system(void);
extern int tb_device_get_update_result(void);
extern int tb_device_start_update_firmware(void);
extern int tb_device_query_hu_ipaddr(char *hu_ipaddr, int ipaddr_buf_len);
extern int tb_device_modem_wakeup_mcu(uint32_t gpio_num);
extern int tb_device_enable_sleep(bool enable);

//cfw event handle
extern int tb_handle_cfw_event(const osiEvent_t *event);

// test function
extern void tbtest_wan_network_info_cb(tb_wan_notify_type_e type, const char *value);
extern void tbtest_wan_get_network_type(void);
extern void tbtest_wan_get_network_info(void);
extern void tbtest_wan_get_signal_info(void);
extern void tbtest_data_callback_fun(tb_data_profile_id cid, tb_data_dial_status status);
extern void tbtest_data_wan_disconnect(void);
extern void tbtest_data_wan_connect(void);
extern void tbtest_data_set_connect_parameter(void);
extern void tbtest_data_get_connect_parameter(void);
extern void tbtest_data_get_dial_status(void);
extern void tbtest_data_get_statistics(void);
extern void tbtest_data_get_ipv4_address(void);
extern void tbtest_sim_callback_fun(tb_sim_status type);
extern void tbtest_sim_get_sim_state(void);
extern void tbtest_sim_get_imsi(void);
extern void tbtest_sim_get_imei(void);
extern void tbtest_sim_get_phone_num(void);
extern void tbtest_sms_callback_fun(tb_sms_status type, int value);
extern void tbtest_sms_get_contents_fun(tb_sms_message *received_message);
extern void tbtest_send_message();
extern void tbtest_sms_get_sms_withid();
extern void tbtest_sms_delete_sms();
extern void tbtest_voice_callback_fun(tb_voice_report_event_e event, void *voice_report_info);
extern void tbtest_voice_dial_call();
extern void tbtest_voice_connect_call();
extern void tbtest_voice_hungup_call();
extern void tbtest_voice_switch_audio_channel0();
extern void tbtest_voice_switch_audio_channel1();
extern void tbtest_device_get_modemserialnumber();
extern void tbtest_device_get_hwversion();
extern void tbtest_device_getversion();
extern void tbtest_device_get_iccid();
extern void tbtest_device_set_rtc_timer(void);
extern void tbtest_device_cancel_timer(void);
extern void tbtest_device_shutdown_system(void);
extern void tbtest_device_reboot_system(void);

#ifdef __cplusplus
}
#endif
#endif
