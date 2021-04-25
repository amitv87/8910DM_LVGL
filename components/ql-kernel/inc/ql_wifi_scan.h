/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_WIFI_SCAN_H
#define QL_WIFI_SCAN_H


#include "ql_api_common.h"


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_DEV_ERRCODE_BASE (QL_COMPONENT_BSP_WIFISCAN<<16)

#define QL_WIFI_SCAN_MAX_TIME (5000) //max scan time for a channel
#define QL_WIFI_SCAN_MIN_TIME (120) //min scan time for a channel
#define QL_WIFI_SCAN_DEFAULT_TIME (600)//default scan time for a channel

#define QL_WIFI_SCAN_MAX_AP_CNT (300)//max wifi ap count to scan
#define QL_WIFI_SCAN_MIN_AP_CNT (1)//min wifi ap count to scan
#define QL_WIFI_SCAN_DEFAULT_AP_CNT (10)//default wifi ap count to scan

#define QL_WIFI_SCAN_MAX_ROUND (10)//max round to scan
#define QL_WIFI_SCAN_MIN_ROUND (1)//min round to scan
#define QL_WIFI_SCAN_DEFAULT_ROUND (1)//default round to scan


/*========================================================================
*  Enumeration Definition
*========================================================================*/
/********************  error code about wifiscan    **********************/
typedef enum
{
	QL_WIFISCAN_SUCCESS  				        = 0,
    
    QL_WIFISCAN_EXECUTE_ERR                     = 1|QL_DEV_ERRCODE_BASE,
    QL_WIFISCAN_MEM_ADDR_NULL_ERR,
    QL_WIFISCAN_INVALID_PARAM_ERR,
    QL_WIFISCAN_SEMAPHORE_WAIT_ERR,
    QL_WIFISCAN_MUTEX_TIMEOUT_ERR,
    
    QL_WIFISCAN_OPEN_FAIL,
    QL_WIFISCAN_BUSY_ERR,
    QL_WIFISCAN_ALREADY_OPEN_ERR,
    QL_WIFISCAN_NOT_OPEN_ERR,
    QL_WIFISCAN_HW_OCCUPIED_ERR,
    
} ql_errcode_wifi_scan_e;

typedef enum
{
    QL_WIFISCAN_CHANNEL_ALL_BIT     = 0x1FFF,
    QL_WIFISCAN_CHANNEL_ALL         = 0,
    QL_WIFISCAN_CHANNEL_ONE,
    QL_WIFISCAN_CHANNEL_TWO,
    QL_WIFISCAN_CHANNEL_THREE,
    QL_WIFISCAN_CHANNEL_FOUR,
    QL_WIFISCAN_CHANNEL_FIVE,
    QL_WIFISCAN_CHANNEL_SIX,
    QL_WIFISCAN_CHANNEL_SEVEN,
    QL_WIFISCAN_CHANNEL_EIGHT,
    QL_WIFISCAN_CHANNEL_NINE,
    QL_WIFISCAN_CHANNEL_TEN,
    QL_WIFISCAN_CHANNEL_ELEVEN,
    QL_WIFISCAN_CHANNEL_TWELVE,
    QL_WIFISCAN_CHANNEL_THIRTEEN,
    QL_WIFISCAN_CHANNEL_TOTAL       = QL_WIFISCAN_CHANNEL_THIRTEEN,
    QL_WIFISCAN_CHANNEL_MAX,
}ql_wifiscan_channel_e;


/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
	uint32_t bssid_low;  ///< mac address low
	uint16_t bssid_high; ///< mac address high
	uint8_t channel;	 ///< channel id
	int8_t rssival; 	 ///< signal strength
} ql_wifi_ap_info_s;

typedef struct
{
	uint32_t ap_cnt;
	ql_wifi_ap_info_s *ap_infos;
} ql_wifiscan_result_s;

typedef struct
{
    uint32_t msg_id;
    uint32_t msg_err_code;
	void *msg_data;
} ql_wifiscan_ind_msg_s;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/*
* wifi scan callback function type definition.
* ql_wifiscan_ind_msg_s as paramter, all the buffer will be released automatically,
* so if your want, you need to save the msg data in your callback function.
*/
typedef void (*ql_wifiscan_callback)(ql_wifiscan_ind_msg_s *msg_buf);

/*
* to open the wifiscan device, if it is already opened, the function will return QL_WIFISCAN_ALREADY_OPEN_ERR.
*/
ql_errcode_wifi_scan_e ql_wifiscan_open(void);

/*
* to close the wifiscan device, return QL_WIFISCAN_SUCCESS always.
*/
ql_errcode_wifi_scan_e ql_wifiscan_close(void);

/*
* to config the options for one time scan
* expect_ap_cnt: the max ap cnt you want to find
* round: scan times for all channel
* scan_channel: the channel you want to scan, from bit 0 to bit 12, each bit is a channel.
*               if want to scan all channel, please set scan_channel to 0 or set the low 13 bits as 1 like QL_WIFISCAN_CHANNEL_ALL_BIT or 0xFFFF
* channel_time: time to scan a channel
*/
ql_errcode_wifi_scan_e ql_wifiscan_option_set(uint16_t expect_ap_cnt, uint8_t round, uint16_t scan_channel, uint32_t channel_time);

/*
* to scan the wifi AP at synchronous mode.
* p_ap_cnt is to return the AP number, and the max is your expect_ap_cnt in ql_wifiscan_option_set..
* p_ap_infos is to save the AP info, you need to apply the enough buffer first, like expect_ap_cnt*sizeof(ql_wifi_ap_info_s)
* please don't forget to close wifiscan device by call ql_wifiscan_close
*/
ql_errcode_wifi_scan_e ql_wifiscan_do(uint16_t *p_ap_cnt, ql_wifi_ap_info_s *p_ap_infos);

/*
* to register a callback funciton for wifiscan task.
*/
ql_errcode_wifi_scan_e ql_wifiscan_register_cb(ql_wifiscan_callback wifiscan_cb);

/*
* start to do wifiscan at asynchronous mode, and the result will return by call your callback function.
* please don't forget to close wifiscan device by call ql_wifiscan_close
*/
ql_errcode_wifi_scan_e ql_wifiscan_async(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_WIFI_SCAN_H */


