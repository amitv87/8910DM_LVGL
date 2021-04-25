/**
 * @file    bluetooth.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

/**
* @defgroup bt_host Host Interface
* @{
*/
/** @} */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "bluetooth/bt_types.h"
#include "bluetooth/bt_driver.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bt_config.h"

#define BT_PROFILE_GATT_ID "gatt"
#define BT_PROFILE_A2DP_ID "a2dp"
#define BT_PROFILE_AVRCP_ID "avrcp"
#define BT_PROFILE_SPP_ID "spp"
#define BT_PROFILE_HFP_ID "hfp"
#define BT_PROFILE_SDP_ID "sdp"
#define BT_PROFILE_UWS_ID "uws"
#define BT_AT_PTS_ID "pts"

#define BT_BTAT_NPI_ID "npi"

/**
 * @enum bt_state_t
 * @brief Enumeration with possible bt open state.
 *
 */

typedef enum
{
    BT_STATE_OFF,                  /**< Bt is off */
    BT_STATE_TURNING_OFF,  /**< Bt is turning on */
    BT_STATE_TURNING_ON,   /**< Bt is turning off */
    BT_STATE_ON,                   /**< Bt is on */
} bt_state_t;

/** ACL state enumeration*/
typedef enum
{
    BT_ACL_STATE_DISCONNECTED,/**< ACL is disconnected */
    BT_ACL_STATE_CONNECTED,     /**< ACL is connected */
} bt_acl_state_t;

/** Bond state enumeration*/
typedef enum
{
    BT_BOND_STATE_NONE,         /**< There is no bonding */
    BT_BOND_STATE_BONDING,   /**< Bt is bonding */
    BT_BOND_STATE_BONDED,     /**< Bt is bonded */
} bt_bond_state_t;

/**Bt property type enumeration*/
typedef enum
{
    BT_PRO_BDNAME = 0x01,                  /**< Use to set/get local name */
    BT_PRO_BDADDR,		                     /**< Use to set/get local addr */
    BT_PRO_UUIDS,                                /**< Use to set/get uuids */
    BT_PRO_CLASSIC_PACKAGES,           /**< They are classic packages */
    BT_PRO_BLE_PACKAGES,                  /**< They are ble packages */
    BT_PRO_INQUIRY_SCAN_ACTIVITY,  /**< It is an inquiry scan activity */
    BT_PRO_PAGE_SCAN_ACTIVITY,       /**< It is an page scan activity */
    BT_PRO_BLE_PHY,                            /**< Use to get ble phy */
    BT_PRO_REMOTE_VERSION,              /**< Use to set/get remote version */
    BT_PRO_STACK_SMP_CONFIG,           /**< Use to set/get smp configuration */
    BT_PRO_BT_STORAGE,               /**< Use to get bt storage */
} bt_pro_type_t;

typedef enum
{
    BT_DEVICE_TYPE_BREDR = 1,
    BT_DEVICE_TYPE_BLE,
    BT_DEVICE_TYPE_DUMO,
}bt_device_type_t;

typedef struct
{
    UINT8 device_type; //bt_device_type_t

    UINT8 *raw_data;
    UINT32 raw_data_len;

    UINT32 list_count;

    UINT32 *profile_uuid_list;
    UINT16 *feature_list;
    UINT16 *profile_ver_list;

}bt_service_record;
/**Bt property structure*/
typedef struct
{
    bt_pro_type_t type;  /**< The property type */
    void *val;                /**< The value of "type" */
    int len;                   /**<  The lenght of "type" */
} bt_property_t;

/**Bt LMP version enumeration*/
typedef enum
{
    LL_VERSION_RESERVE = 0x05,/**< Reserved */
    LL_VERSION_CORE_4_0,         /**< Link layer version core specification 4.0 */
    LL_VERSION_CORE_4_1,         /**< Link layer version core specification 4.1 */
    LL_VERSION_CORE_4_2,         /**< Link layer version core specification 4.2 */
    LL_VERSION_CORE_5_0,         /**< Link layer version core specification 5.0 */
    LL_VERSION_CORE_5_1,         /**< Link layer version core specification 5.1 */
    LL_VERSION_CORE_5_2,         /**< Link layer version core specification 5.2 */
} bt_lmp_version;

/**Bt enable state enumeration */
typedef enum
{
    BT_STATE_DISABLE = 0x00,  /**< Initial state of bluetooth */
    BT_STATE_ENABLE,		 /**< Bt is enabled  */
    BT_STATE_NOUSE,               /**< Bt is not used  */
} bt_enable_state_t;

/**Bt remote device version information structure*/
typedef struct
{
    ///UINT16 handle;
    bdaddr_t addr;                          /**< The address of remote device */
    UINT8 lmp_ver;                         /**< Lmp version number */
    UINT16 manufacturer_name;     /**< Manufacturer name */
    UINT16 lmp_sub_ver;                /**< Lmp sub version number */
} bt_remote_device_version_info;

/**Bt callback function structure*/
typedef struct
{
    void (*state_changed_callback) (bt_state_t state);                                            /**< Bt state changed callback function */
    void (*acl_state_changed_callback) (bt_acl_state_t state, bdaddr_t *addr);          /**< ACL state changed callback function */
    void (*bond_state_changed_callback) (bt_bond_state_t state, bdaddr_t *addr);    /**< Bond state changed callback function */
    void (*property_changed_callback) (bt_property_t *property);                              /**< Property changed callback function */
    int  (*inquiry_result_cb)(bdaddr_t *addr, UINT8 rssi, UINT8 *device_name, UINT32 device_class);           /**< Inquiry result callback function */
    void (*inquiry_complete_cb)(UINT8 status);                                                        /**< Inquiry complete callback function */
    void (*pin_request_callback)(bdaddr_t *remote_bd_addr, UINT8 *device_name, UINT32 cod);
    UINT8 (*bond_request_handle_cb)(const bdaddr_t* const address,UINT8 *device_name,UINT8 name_length, UINT32 value);
} bt_callbacks_t;

/**Bt scan acitivity parameter structure*/
typedef struct
{
    UINT16 interval;
    UINT16 window;
} bt_scan_activity_param;

/**
* @addtogroup bt_host
* @{
* @defgroup fuc_interface Bt Fuc Interface
* @{  */

/** Bt interface function structure*/
typedef struct
{
    int (*init) (const bt_callbacks_t *bt_callbacks, const bt_config_t *bt_config, bt_storage_t *bt_storage);
    int (*enable) (void);
    int (*disable) (void);
    int (*cleanup) (void);
    const void *(*get_profile_interface) (const char *profile_id);
    int (*set_osi_callbacks) (void *args);
    int (*set_property) (bt_property_t *property);
    int (*get_property) (bt_property_t *property);
    void (*set_visible)(int is_visiable);
    UINT8 (*get_visable)(void);
    UINT8 (*get_enable_state)(void);
    int (*inquiry) (UINT8 max_result, UINT8 is_liac);
    void (*inquiry_cancel)(void);
    void (*buffer_free)(hci_data_t *data);
    int (*pin_reply)(bdaddr_t *addr, UINT8 accept, UINT8 pin_len, UINT8* pin_code);
    int (*create_bond)(bdaddr_t *addr);
    int (*cancel_bond)(bdaddr_t *addr);
    int (*remove_bond)(bdaddr_t *addr);
} bt_interface_t;
/** @} */
/** @} */


/**
 * @brief get bt interface
 * @return bt interface
 */
const bt_interface_t *get_bt_interface(void);

/**
 * @brief get bt callback
 * @return bt callback
 */

bt_callbacks_t * get_bt_callback(void);

#endif
