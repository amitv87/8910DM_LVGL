/**
 * @file     bt_abs.c
 * @brief    bt interface adaptor
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */

#include "bluetooth/bt_types.h"
#include "bt_abs.h"
#include "bluetooth/bt_hfp.h"
#include "sci_types.h"
#include "bluetooth/bt_config.h"
#include <stdlib.h>
#include <string.h>
//#include "allocator.h"
#include "bluetooth/bluetooth.h"
// #include "bt_utils.h" // build err
// #include "osi.h"   // build err
#include "osi_log.h"

#include "bluetooth/bt_avrcp.h"
#include "bluetooth/bt_a2dp.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bt_spp.h"


static const bt_interface_t        *bt_interface = NULL;
static const btgatt_interface_t           *bt_gatt_interface = NULL;
static const btgatt_server_interface_t *bt_gatt_server_interface = NULL;
static const btgatt_client_interface_t  *bt_gatt_client_interface = NULL;
static const btgatt_smp_interface_t    *bt_gatt_smp_interface = NULL;
static const bta2dp_interface_t          *bt_a2dp_interface = NULL;
static const btavrcp_interface_t         *bt_avrcp_interface = NULL;
static const btspp_interface_t            *bt_spp_interface = NULL;
static const bthfp_interface_t            *bt_hfp_interface = NULL;

/**
* @brief    Bluetooth core initialization function
* @details When system is powered on, this function will be used to start bluetooth
*              thread and register @bt_callbacks @bt_config @bt_storage into memory
* @param [in] bt_callbacks -The structure of bluetooth callback funtion
* @param [in] bt_config -The structure of bluetooth configuration
* @param [in] bt_storage -The structure of bluetooth storage
* @return void
*/
void bt_core_init(const bt_callbacks_t *bt_callbacks, const bt_config_t *bt_config, bt_storage_t *bt_storage)
{
    bt_interface = get_bt_interface();

    bt_gatt_interface = (btgatt_interface_t *)bt_interface->get_profile_interface(BT_PROFILE_GATT_ID);
    bt_gatt_server_interface = (btgatt_server_interface_t *)bt_gatt_interface->server;
    bt_gatt_client_interface  = (btgatt_client_interface_t *)bt_gatt_interface->client;
    bt_gatt_smp_interface = (btgatt_smp_interface_t *)bt_gatt_interface->smp;

    bt_a2dp_interface = (bta2dp_interface_t *)bt_interface->get_profile_interface(BT_PROFILE_A2DP_ID);

    //avrcp
    bt_avrcp_interface = (btavrcp_interface_t *)bt_interface->get_profile_interface(BT_PROFILE_AVRCP_ID);

    //hfp
    bt_hfp_interface = (bthfp_interface_t *)bt_interface->get_profile_interface(BT_PROFILE_HFP_ID);

    //spp
    bt_spp_interface = (btspp_interface_t *)bt_interface->get_profile_interface(BT_PROFILE_SPP_ID);

    bt_interface->init(bt_callbacks, bt_config, bt_storage);
}

/**
* @brief    Bluetooth state get function
* @details This function returns the state of bluetooth 
* @param [in] void
* @return The state of bluetooth 
* @retval	 BT_STATE_ON -The bluetooth is already on
* @retval	 BT_STATE_OFF -The bluetooth is already off or the bt interface is not initialized
* @retval	 BT_STATE_TURNING_ON -The bluetooth is turning on 
* @retval	 BT_STATE_TURNING_OFF -The bluetooth is turning off
*/ 
bt_state_t bt_state_get(void)
{
    if(bt_interface != NULL)
    {
        return bt_interface->get_enable_state();
    }

    return BT_STATE_OFF;
}

/**
* @brief  Bluetooth start function
* @details This function returns the status of bluetooth start
* @param [in] void
* @return The status of bluetooth start
* @retval BT_SUCCESS -The bluetooth is successfully started only when it is already off
* @retval	 BT_NOT_READY -The bluetooth is busy
* @retval	 BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_start(void)
{
    if(bt_interface != NULL)
    {
        return bt_interface->enable();
    }

    return BT_FAIL;
}

/**
* @brief  Bluetooth stop function
* @details This function returns the status of bluetooth stop
* @param [in] void 
* @return The status of bluetooth stop
* @retval	 0 -The bluetooth is successfully stopped only when it is already on
* @retval BT_NOT_READY -The bluetooth is busy
* @retval BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_stop(void)
{
    if(bt_interface != NULL)
    {
        return bt_interface->disable();
    }
    
    return BT_FAIL;
}

/**
* @brief  Bluetooth visibility get function
* @details This function returns the value of bluetooth visibility
* @param [in] void
* @return The value of bluetooth visibility
* @retval -visable  bit4                                    bit1 bit0        
*                          0 --->NONCONNECTABLE    0     0 --->NONDISCOVERABLE
*                          1 --->CONNECTABLE          0     1 --->GENERALDISCOVERY
*                                                                  1     0 --->LIMITEDDISCOVERY
* @retval 0 -The bt interface is not initialized
*/ 
uint8 bt_visibility_get(void)
{ 
    if(bt_interface)
    {
        return bt_interface->get_visable();
    }

    return 0;
}

/**
* @brief   Bluetooth visibility set function
* @details This function is used to set bluetooth visibility 
* @param [in] visible -The value of visible
* @return void
*/ 
void bt_visibility_set(uint8 visible)
{
    if(bt_interface)
    {
        bt_interface->set_visible(visible);
    }
}

/**
* @brief   Bluetooth local name get function
* @details This function is used to get the local name of classic bluetooth
* @param [inout] name, the name space must be more than 31 bytes.
* @return The status of local name get
* @retval BT_SUCCESS -OP successfully
* @retval BT_FAIL -OP fail
*/ 
bt_status_t bt_local_name_get(char* name)
{
    bt_status_t status = BT_FAIL;
    bt_property_t bt_property;
    
    if(bt_interface)
    {
        bt_property.type = BT_PRO_BDNAME;
        bt_property.val   = (void*)name;

        return bt_interface->get_property(&bt_property);
    }

    return status;
}

#if 0
/**
* @brief   Bluetooth local name set function
* @details This function is used to set the local name of classic bluetooth
*             Note that this command can only save the name into NV and can not take effect until the bluetooth start again
* @param [in] name -The new local name 
* @return The status of local name set
* @retval BT_SUCCESS -The name is successfully set
* @retval BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_local_name_set(const uint8 *name)
{
    bt_status_t status = BT_FAIL;
    bt_property_t bt_property;

    if(bt_interface)
    {
        bt_property.type = BT_PRO_BDNAME;
        bt_property.len = strlen((const char *)name); //MAX_DEVICE_NAME_SIZE;
        bt_property.val = (void *)&name[0];           //(void*)&cfg_ptr->local_name[0];
        bt_interface->set_property(&bt_property);

        status = BT_SUCCESS;
    }

    return status;
}
#endif

/**
* @brief   Bluetooth local name change function
* @details This function is used to change the local name of classic bluetooth
*             Note that this command can only take effect when the bluetooth is on
* @param [in] name -The new local name 
* @return The status of local name change
* @retval	 BT_SUCCESS -The name is successfully changed
* @retval	 BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_local_name_change(const uint8 *name)
{
    bt_status_t status = BT_FAIL;
    bt_property_t bt_property;

    if(bt_interface)
    {
        bt_property.type = BT_PRO_BDNAME;
        bt_property.len = strlen((const char *)name); //MAX_DEVICE_NAME_SIZE;
        bt_property.val = (void *)&name[0];           //(void*)&cfg_ptr->local_name[0];
        bt_interface->set_property(&bt_property);

        status = BT_SUCCESS;
    }

    return status;
}

/**
* @brief   Bluetooth public address get function
* @details This function is used to get the public address of bluetooth
* @param [inout] addr
* @return The status of public address get
* @retval BT_SUCCESS -OP successfully
* @retval BT_FAIL -OP fail
*/ 
bt_status_t bt_public_addr_get(bdaddr_t* addr)
{
    bt_property_t bt_property;
    
    if(bt_interface)
    {
        bt_property.type = BT_PRO_BDADDR;
        bt_property.val   = (void*)addr;

        return bt_interface->get_property(&bt_property);
    }
    
    return BT_FAIL;
}
#if 0
/**
* @brief   Bluetooth public address set function
* @details This function returns the status of public address set
* @param [in] void
* @return The status of public address set
* @retval BT_SUCCESS -The public address is set successfully
* @retval BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_public_addr_set(uint8 *addr)
{
    bt_property_t bt_property;

    if(bt_interface == NULL)
        return BT_FAIL;
    
    bt_property.type = BT_PRO_BDADDR;
    bt_property.len = SIZE_OF_BDADDR;
    bt_property.val = (void *)addr;
    bt_interface->set_property(&bt_property);
    
    return BT_SUCCESS;
}
#endif

//uint32 bt_device_search_state_get(void)
//{
//    return 0;
//}

/******************************************************************************
 * FUNCTION:        bt_searched_device_count_get
 *
 * DESCRIPTION: only for ATC Test
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *      the number of devices which is searched
 *
 * MESSAGES:
 *
 ******************************************************************************/
//uint32 bt_searched_device_count_get(void)
//{
//    return 0;
//}

//uint32 bt_searched_device_info_get(UINT8* addr, UINT8* name, UINT8 idx)
//{
//    return 1;
//}

/******************************************************************************
 * FUNCTION:        bt_device_search_start
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      service_type -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_DEV_INQ_RES
 *          data.dev_info
 *
 *      ID_STATUS_DEV_INQ_ADDR_RES
 *          data.dev_info
 *
 *      ID_STATUS_DEV_INQ_CMP
 *
 *      ID_STATUS_DEV_INQ_CANCEL
 ******************************************************************************/
/**
* @brief   Bluetooth start search device function
* @details This function returns the status of bluetooth inquiry 
* @param [in] max_result -The maximum number that can be inquiried
* @param [in] is_liac -Is it a limited inquiry access code or not?
* @return The status of bluetooth inquiry 
* @retval BT_SUCCESS -The bluetooth successfully starts to search other devices
* @retval BT_FAIL -The bt interface is not initialized
*/ 
bt_status_t bt_device_search_start(uint8 max_result, uint8 is_liac)
{
    if(bt_interface == NULL)
        return BT_FAIL;

    bt_interface->inquiry(max_result, is_liac);
    
    return  BT_SUCCESS;
}

/**
* @brief   Bluetooth device search cancel function
* @details This function returns the status of bluetooth search device cancel
* @param [in] void
* @return The status of bluetooth search device cancel
* @retval BT_SUCCESS -The bluetooth successfully cancels searching other devices
* @retval BT_FAIL -The bluetooth is not initialized
*/ 
bt_status_t bt_device_search_cancel(void)
{
    bt_status_t status = BT_SUCCESS;

    return status;
}

/******************************************************************************
 * FUNCTION:        bt_pair_pin_input
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr   -
 *      pin    -
 *      length -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_SUCCESS / BT_ERROR
 ******************************************************************************/
bt_status_t bt_pair_pin_code_input(bdaddr_t *addr, uint8 accept, uint8 pin_len, uint8* pin_code)
{
    bt_status_t status = BT_SUCCESS;

    return status;
}

/******************************************************************************
 * FUNCTION:        bt_paired_device_remove
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_DELETED
 *          status = BT_SUCCESS / BT_ERROR
 ******************************************************************************/
bt_status_t bt_paired_device_remove(bdaddr_t *addr)
{
    bt_status_t status = BT_SUCCESS;

    return status;
}

/******************************************************************************
 * FUNCTION:        bt_device_pair
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_SUCCESS / BT_ERROR
 *
 *      ID_STATUS_DEV_PIN_REQ
 *          data.dev_info
 ******************************************************************************/
bt_status_t bt_device_pair(bdaddr_t *addr)
{
    bt_status_t status = BT_SUCCESS;

    return status;
}

/******************************************************************************
 * FUNCTION:        bt_device_pair_cancel
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_ERROR
 ******************************************************************************/
bt_status_t bt_device_pair_cancel(bdaddr_t *addr)
{
    bt_status_t status = BT_SUCCESS;

    return status;
}

/**
* @brief  Set inquiry scan parameters
* @details This function returns the status of inquiry scan parameters setting
* @param [in] interval -interval of inquiry scan
* @param [in] window -window of inquiry scan
* @return The status of inquiry scan parameters setting 
* @retval BT_SUCCESS - successfully
* @retval BT_FAIL -failed
*/ 
bt_status_t bt_inquiry_scan_param_set(UINT16 interval, UINT16 window)
{
    bt_property_t bt_property;
    bt_scan_activity_param bt_scan_param;

    if(bt_interface != NULL)
    {
        bt_scan_param.interval = interval;
        bt_scan_param.window = window;
        
        bt_property.type = BT_PRO_INQUIRY_SCAN_ACTIVITY;
        bt_property.val = (void*)(&bt_scan_param);

        return bt_interface->set_property(&bt_property);
    }

    return BT_FAIL;
}

/**
* @brief  Set page scan parameters
* @details This function returns the status of page scan parameters setting
* @param [in] interval -interval of page scan
* @param [in] window -window of page scan
* @return The status of page scan parameters setting 
* @retval BT_SUCCESS - successfully
* @retval BT_FAIL -failed
*/ 
bt_status_t bt_page_scan_param_set(UINT16 interval, UINT16 window)
{
    bt_property_t bt_property;
    bt_scan_activity_param bt_scan_param;

    if(bt_interface != NULL)
    {
        bt_scan_param.interval = interval;
        bt_scan_param.window = window;
        
        bt_property.type = BT_PRO_PAGE_SCAN_ACTIVITY;
        bt_property.val = (void*)(&bt_scan_param);

        return bt_interface->set_property(&bt_property);
    }

    return BT_FAIL;
}

/**
* @brief  Hfp callback register function
* @details This function returns the status of hfp callback register
* @param [in] callbacks -The hfp callback function structure
* @return The status of hfp callback register 
* @retval BT_SUCCESS -The hfp callback is registered successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_callback_register(bthfp_callbacks_t *callbacks)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->init(callbacks);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief   Hfp connection function
* @details This function returns the status of hfp connection 
* @param [in] pAddr -The adddress of the device to be connected 
* @return The status of hfp connection 
* @retval BT_SUCCESS -The hfp connection is established successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_connect(bdaddr_t *pAddr)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->connect(pAddr);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief   Hfp disconnect function
* @details This function returns the status of hfp disconnection 
* @param [in] pAddr -The adddress of the device to be disconnected 
* @return The status of hfp disconnection 
* @retval BT_SUCCESS -The hfp disconnection is done successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_disconnect(bdaddr_t *pAddr)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->disconnect(pAddr);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief   Hfp call status get function
* @details This function returns the status of hfp call
* @param [in] void
* @return The status of hfp call 
* @retval 0 There is no device
* @retval hfp_call_status -The status of hfp call
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_call_status_get(void)
{
    if(bt_hfp_interface != NULL)
    {
        return bt_hfp_interface->get_call_status();
    }

    return BT_FAIL;
}

/**
* @brief  Establish hfp volume set function
* @details This function returns the status of hfp volume set
* @param [in] addr -The address of the device to be set
* @param [in] vol -The volume value to be set. Note that the volume value is 0~15
* @return The status of hfp volume set
* @retval BT_SUCCESS -The volume is set successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_vol_set(bdaddr_t addr, uint8 vol)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->set_volume(addr,vol);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp call reject function
* @details This function returns the status of hfp call reject
* @param [in] pAddr -The address of the device to be rejected
* @return The status of hfp call reject
* @retval BT_SUCCESS -The hfp call is rejected successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_call_reject(bdaddr_t *pAddr)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->call_reject(pAddr);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp call answer function
* @details This function returns the status of hfp call answer
* @param [in] pAddr -The address of the device to be accepted
* @param [in] call_status  
* @return The status of hfp call answer
* @retval BT_SUCCESS -The hfp call is accepted successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_call_answer(bdaddr_t *pAddr, UINT8 call_status)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->call_accept(pAddr,call_status);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp dial function
* @details This function returns the status of hfp calling the assigned phone number
* @param [in] pAddr -The address of the device to be dailed
* @param [in] call_type -The type of the call. Here it is HFP_CALL_REQ_CALLNUMBER
* @param [in] phone_number -The phone number to be called
* @return The status of hfp calling the assigned phone number
* @retval BT_SUCCESS -The hfp dials the phone number successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_dial(bdaddr_t *pAddr, UINT8 *phone_number)
{
    if(bt_hfp_interface != NULL)
    {
     //   bt_hfp_interface->call_req(pAddr,HF_CALL_NUM,phone_number);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp the lat call dial function
* @details This function returns the status of hfp calling the last phone number
* @param [in] pAddr -The address of the device to be dailed
* @param [in] call_type -The type of the call. Here it is HFP_CALL_REQ_REDIAL
* @return The status of hfp calling the last phone number
* @retval BT_SUCCESS -The hfp call the last phone number successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_redial(bdaddr_t *pAddr)
{
    if(bt_hfp_interface != NULL)
    {
     //   bt_hfp_interface->call_req(pAddr,HF_CALL_REDIAL,NULL);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp voice recognition enable function
* @details This function returns the status of hfp voice recognition enable
* @param [in] addr -The address of the device to be set
* @return The status of hfp voice recognition enable
* @retval BT_SUCCESS -The hfp voice recognition is enabled successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_voice_recognition_enable(bdaddr_t *addr)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->set_voice_recognition(addr, HF_VR_START);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

bt_status_t bt_hfp_voice_recognition_disable(bdaddr_t *addr)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->set_voice_recognition(addr, HF_VR_STOP);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp three way call control function
* @details This function returns the status of hfp three way call control
* @param [in] pAddr -The address of the device to be set
* @param [in] call_type -The type of the call
* @param [in] param -The parameter
* @return The status of hfp three way call control
* @retval BT_SUCCESS -The hfp three way call is established successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_three_way_call_ctrl(bdaddr_t *pAddr, UINT8 call_type, UINT8 param)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->three_way_calling(pAddr,call_type,param);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Hfp send battery function
* @details This function returns the status of hfp send battery
* @param [in] battery -The battey level
* @return The status of hfp send battery
* @retval BT_SUCCESS -The battery is sent successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/ 
bt_status_t bt_hfp_battery_send(uint8 battery)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->send_battery(battery);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

hfp_device_info* bt_hfp_device_info_get(bdaddr_t *addr)
{
    if(bt_hfp_interface != NULL)
    {
     //   bt_hfp_interface->send_battery(battery);
        return NULL;
    }

    return NULL;
}

/**
* @brief  Hfp disconnect sco/esco link
* @details This function returns the status of disconnect audio link
* @param void
* @return The status of hfp disconnect audio link
* @retval BT_SUCCESS -audio disconnect is sent successfully
* @retval BT_FAIL -The hfp interface is not initialized
*/
bt_status_t bt_hfp_disconnect_audio(void)
{
    if(bt_hfp_interface != NULL)
    {
        bt_hfp_interface->audio_disconnect();
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Avrcp callback register function
* @details This function returns the status of avrcp callback register
* @param [in] callbacks -The avrcp callback structure
* @return The current status of avrcp callback register
* @retval BT_SUCCESS -The avrcp callback is registered successfully
* @retval BT_FAIL -The avrcp interface is not initialized
*/ 
bt_status_t bt_avrcp_callback_register (btavrcp_callbacks_t *callbacks)
{
    if(bt_avrcp_interface != NULL)
    {
        return bt_avrcp_interface->init(callbacks);
    }

    return BT_FAIL;
}

/**
* @brief  Avrcp play state get function
* @details This function returns the current status of avrcp playing 
* @param [in] void
* @return The current status of avrcp playing 
* @retval AVRCP_PLAYBACK_STATUS_STOPPED -The play is stopped
* @retval AVRCP_PLAYBACK_STATUS_PLAYING -The play is going on
* @retval AVRCP_PLAYBACK_STATUS_PAUSED -The play is paused
* @retval AVRCP_PLAYBACK_STATUS_FWD_SEEK -The paly is seeking forward
* @retval AVRCP_PLAYBACK_STATUS_REV_SEEK -The paly is seeking rewind
* @retval AVRCP_PLAYBACK_STATUS_ERROR -The avrcp interface is not initialized
*/ 
t_AVRCP_PLAYBACK_STATUS bt_avrcp_play_state_get(void)
{   
    if(bt_avrcp_interface != NULL)
    {
        return bt_avrcp_interface->get_play_state();
    }

    return AVRCP_PLAYBACK_STATUS_ERROR;
}

/**
* @brief  Avrcp key send function
* @details This function returns the status of avrcp key send
* @param [in] key -The key value        
* @param [in] press -The press value
* @param [in] param  -The volume value
* @param [in] addr The address of the device to send
* @return The status of avrcp key send
* @retval BT_SUCCESS -The avrcp key is sent successfully
* @retval BT_FAIL -The avrcp interface is not initialized
*/ 
bt_status_t bt_avrcp_key_send(uint8 key, uint8 press, uint8 param, bdaddr_t addr)
{
    if(bt_avrcp_interface != NULL)
    {
        return bt_avrcp_interface->send_key(key, press, param, addr);
    }
    return BT_FAIL;
}

/**
* @brief  Avrcp volume set function
* @details This function returns the status of avrcp volume set
* @param [in] addr -The address of the device to set
* @param [in] volume -The value of the volume to be set
* @return The status of avrcp volume set
* @retval BT_SUCCESS -The volume is set successfully
* @retval BT_FAIL -The avrcp interface is not initialized or the device does not support volume handling or the volume is failed to set 
*/ 
bt_status_t bt_avrcp_volume_set(bdaddr_t addr,UINT8 volume)
{
    if(bt_avrcp_interface == NULL)
        return BT_FAIL;

    if(bt_avrcp_interface->is_support_volume_handling(addr) == false)
        return BT_FAIL;
    
    if(BT_SUCCESS == bt_avrcp_interface->sync_volume(addr, volume))
    {
        bt_avrcp_interface->reset_volume_change_state(addr);
        return BT_SUCCESS;
    }

    return BT_FAIL;
}

/**
* @brief  Avrcp volume get function
* @details This function returns the status of volume value get 
* @param [in] addr -The address of the peer device
* @param [out] vol -The volume value
* @return The status of volume value get   
* @retval BT_SUCCESS -The volume is get successfully
* @retval BT_UNSUPPORTED -The peer device does not support volume handling
* @retval BT_FAIL -The avrcp interface is not initialized
*/
bt_status_t bt_avrcp_volume_get(bdaddr_t addr, uint8 *vol)
{
    if(bt_avrcp_interface != NULL)
    {
        if(bt_avrcp_interface->is_support_volume_handling(addr) == false)
            return BT_UNSUPPORTED;
        
        *vol = bt_avrcp_interface->get_volume();
        return BT_SUCCESS;
    }
    return BT_FAIL;
}

/**
* @brief  Spp callback register function
* @details This function returns the status of spp callback register
* @param [in] callbacks -The spp callback structure
* @return The status of spp callback register
* @retval BT_SUCCESS -The spp callback is registered successfully
* @retval BT_FAIL -The spp interface is not initialized
*/ 
bt_status_t bt_spp_callback_register(btspp_callbacks_t *callbacks)
{
    if(bt_spp_interface != NULL)
    {
        return bt_spp_interface->init(callbacks);
    }

    return BT_FAIL;
}

bt_status_t bt_a2dp_connect(bdaddr_t *pAddr)
{
    if(bt_a2dp_interface != NULL)
    {
        bt_a2dp_interface->connect(pAddr);
        return BT_SUCCESS;
    }

    return BT_FAIL; 
}


bt_status_t bt_a2dp_disconnect(bdaddr_t *pAddr)
{
    if(bt_a2dp_interface != NULL)
    {
        bt_a2dp_interface->disconnect(pAddr);
        return BT_SUCCESS;
    }

    return BT_FAIL; 
}

a2dp_device_info_t *bt_a2dp_get_device(void)
{
     if(bt_a2dp_interface != NULL)
    {
        return bt_a2dp_interface->get_devices();
    }

    return NULL; 
}



bt_status_t bt_a2dp_callback_register(bta2dp_callbacks_t *callbacks)
{
    if(bt_a2dp_interface != NULL)
    {
        return bt_a2dp_interface->init(callbacks);
    }

    return BT_FAIL;
}


/**
* @brief  Spp connection function
* @details This function returns the status of spp connection
* @param [in] addr -The address of the peer device to connect
* @return The status of spp connection
* @retval BT_SUCCESS -The spp connection is done successfully
* @retval BT_FAIL -The spp interface is not initialized
*/ 
bt_status_t bt_spp_connect(bdaddr_t *addr)
{
    if(bt_spp_interface != NULL)
    {
        return bt_spp_interface->connect(addr);
    }

    return BT_FAIL;
}

/**
* @brief  Spp disconnection function
* @details This function returns the status of spp disconnection
* @param [in] addr -The address of the peer device to disconnect
* @return The status of spp disconnection
* @retval BT_SUCCESS -The spp is disconnected successfully
* @retval BT_FAIL -The spp interface is not initialized
*/ 
bt_status_t bt_spp_disconnect(bdaddr_t *addr)
{
    if(bt_spp_interface != NULL)
    {
        return bt_spp_interface->disconnect(addr);
    }

    return BT_FAIL;
}

/**
* @brief  Spp send data function
* @details This function returns the status of spp send data
* @param [in] addr -The address of the peer device
* @param [in] data -The data to be sent
* @param [in] length -The data length
* @return The status of spp send data
* @retval BT_SUCCESS -The data is sent successfully
* @retval BT_INVALIDPARAM -One or more input parameter is invalid
* @retval BT_FAIL -The spp interface is not initialized or the input address can not match with paired address
*/ 
bt_status_t bt_spp_data_send(bdaddr_t *addr, uint8* data, uint16 length)
{
    if(bt_spp_interface != NULL)
    {
        return bt_spp_interface->send(addr, data, length);
    }

    return BT_FAIL;
}

/**
* @brief  Get ble adv state
* @details There are three conditions in this function,if ble adv is disable,this
*    function will return  BlE_ADV_DISABLE,if ble adv is enable, this function
*    will return  BlE_ADV_ENABLE, if the gatt server interface is not registered,
*    this function will return BlE_ADV_NOUSE
* @param void
* @return The status of BLE_ADV
* @retval BlE_ADV_DISABLE The status of ble adv is disable
* @retval BlE_ADV_ENABLE The status of ble adv is enable
*/
ble_adv_enable_state_t ble_adv_state_get(void)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->get_adv_enable_state();
    }

    return BlE_ADV_DISABLE;
}

/**
* @brief  Start ble adv
* @details This function is to Start ble adv,if the bt_gatt_server_interface
*    has not been registered before,this function will  return  BT_FAIL
* @param void
* @return The status of ble_adv_enable
* @retval BT_SUCCESS It means adv ble is enabled now
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_adv_enable(void)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->adv_enable();
    }

    return BT_FAIL;
}

/**
* @brief  Stop ble adv
* @details This function is to Stop ble adv,if the bt_gatt_server_interface
*    has not been registered before,this function will  return  BT_FAIL
* @param void
* @return The status of ble_adv_disable
* @retval BT_SUCCESS It means adv ble is disabled now
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_adv_disable(void)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->adv_disable();
    }

    return BT_FAIL;
}

/**
* @brief  Set ble adv data
* @details This function is to Set ble adv data,if the bt_gatt_server_interface
*    has not been registered before,this function will  return  BT_FAIL
* @param [in] len The length of adv data
* @param [in] data Adv data
* @return The status of ble_adv_data_set
* @retval BT_SUCCESS It means the adv data has been set
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_adv_data_set (uint8 *data, uint32 len)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->set_adv_data(data, len);
    }
    return BT_FAIL;
}

/**
* @brief  Set ble adv parameter
* @details This function is to Set ble adv parameter,if the bt_gatt_server_interface
*    has not been registered before,this function will  return  BT_FAIL
* @param [in] adv_param The parameter of adv
* @param [in] ownAddrType The type of own address
* @return The status of ble_adv_param_set
* @retval BT_SUCCESS It means the ble adv parameters have been set
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_adv_param_set(gatt_adv_param_t* adv_param,uint8 ownAddrType)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->set_adv_param(adv_param, ownAddrType);
    }
    return BT_FAIL;
}

/**
* @brief  Set ble scan response data
* @details This function is to Set ble scan response data,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] data The scan response data
* @param [in] length The length of scan response data
* @return The status of ble_scan_rsp_data_set
* @retval BT_SUCCESS It means the scan response data has been set
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_scan_rsp_data_set (uint8 *data, uint8 length)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->set_scanrsp_data(data, length);
    }
    return BT_FAIL;
}

/**
* @brief  Start ble scan
* @details This function is to Start ble scan,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param void
* @return The status of ble_scan_enable
* @retval BT_SUCCESS It means ble scan is enabled now
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_scan_enable (void)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->start_scan();
    }

    return BT_FAIL;
}

/**
* @brief  Stop ble scan
* @details This function is to Stop ble scan,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param void
* @return The status of ble_scan_disable
* @retval BT_SUCCESS It means ble scan is disabled now
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_scan_disable (void)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->stop_scan();
    }

    return BT_FAIL;
}

/**
* @brief  Set ble scan parameter
* @details This function is to Set ble scan parameter,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] param The parameter of ble scan
* @param [in] own_addr_type The type of own address
* @return The status of ble_scan_param_set
* @retval BT_SUCCESS It means the ble scan parameters have been set
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_scan_param_set (gatt_scan_param_t *param, uint8 own_addr_type)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->set_scan_param(param, own_addr_type);
    }

    return BT_FAIL;
}

/**
* @brief  Set ble random address
* @details This function is to set ble random address,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] addr The random address will be set
* @return The status of ble_random_addr_set
* @retval BT_SUCCESS It means the ble random address has been set
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t  ble_random_addr_set(bdaddr_t *addr)
{
    if(bt_gatt_server_interface)
    {
        return bt_gatt_server_interface->set_le_random_addr(addr);
    }

    return BT_FAIL;
}

/**
* @brief  Get ble random address
* @details This function is to get ble random address,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] addr The random address will be got
* @return The status of ble_random_addr_get
* @retval BT_SUCCESS It means the ble random address has been got
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_random_addr_get(bdaddr_t *addr)
{
    if(bt_gatt_server_interface)
    {
        return bt_gatt_server_interface->get_le_random_addr(addr);
    }

    return BT_FAIL;
}

/**
* @brief  Add ble white list
* @details This function is to add ble white list,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] addr The address will be added in white list
* @param [in] addr_type The type of the address will be added in white list
* @return The status of ble_white_list_add
* @retval BT_SUCCESS It means the address has been added in white list
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_white_list_add(bdaddr_t *addr, uint8 addr_type)
{
    if(bt_gatt_server_interface)
    {
        return bt_gatt_server_interface->add_device_to_white_list(addr,addr_type);
    }

    return BT_FAIL;
}

/**
* @brief  Remove ble white list
* @details This function is to remove ble white list,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] addr The address will be removed in white list
* @param [in] addr_type The type of the address will be removed in white list
* @return The status of ble_white_list_remove
* @retval BT_SUCCESS It means the address has been removed in white list
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_white_list_remove (bdaddr_t *addr, uint8 addr_type)
{
     if(bt_gatt_server_interface)
    {
        return bt_gatt_server_interface->remove_device_from_white_list(addr,addr_type);
    }

    return BT_FAIL;
}

/**
* @brief  Clear ble white list
* @details This function is to clear ble white list,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param void
* @return The status of ble_white_list_clear
* @retval BT_SUCCESS It means the white list has been cleaned
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_white_list_clear (void)
{
     if(bt_gatt_server_interface)
    {
        return bt_gatt_server_interface->clear_white_list();
    }

    return BT_FAIL;
}

/**
* @brief  Get the max number of ble white list
* @details This function is to Get the max number of ble white list,if the bt_gatt_
*    server_interfacehas not been registered before,this function will return BT_FAIL
* @param void
* @return The status of ble_white_list_max_num_get
* @retval BT_SUCCESS It means the max number of white list has been got
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
*/
bt_status_t ble_white_list_max_num_get (void)
{
     if(bt_gatt_server_interface)
    {
//        return bt_gatt_server_interface->clear_white_list();
    }

    return BT_FAIL;
}

/**
* @brief  Set ble local name
* @details This function is to set ble local name,if the bt_gatt_server_interface
*     has not been registered before,this function will return BT_FAIL
* @param [in] name The ble local name
* @param [in] len The length of ble local name
* @return The status of ble_local_name_set
* @retval BT_SUCCESS It means the ble local name has been set
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_local_name_set(const char *name, uint8 len)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->set_le_device_name(name,len);
    }

    return BT_FAIL;
}

/**
* @brief  Get ble local name
* @details This function is to get ble local name,if the bt_gatt_server_interface
*     has not been registered before,this function will return BT_FAIL
* @param void
* @return The status of ble_local_name_get
* @retval NULL It means ble_local_name is not initialized before
* @retval le_name[MAX_LE_DEVICE_NAME] This is local name of device
*/
char* ble_local_name_get(void)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->get_le_device_name();
    }

    return NULL;
}

/**
* @brief  Establish ble connect with another address
* @details This function is to establish ble connect with another address,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] address The address will be connected with our device
* @param [in] module
* @param [in] addr_type The type of peer device address
* @return The status of ble_connect
* @retval BT_SUCCESS It means the ble connection has been established
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
*/
bt_status_t ble_connect (bdaddr_t *address, uint8 module, uint8 addr_type)
{
    if(bt_gatt_client_interface != NULL)
    {
        return bt_gatt_client_interface->connect(address, module, addr_type);
    }

    return BT_FAIL;
}

/**
* @brief  Establish ble connect with another address, this function is extended from ble_connect
* @details This function is to establish ble connect with another address,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL.
*    it can be used to connect with encrypted adv(own_addr_type should be 2 or 3).
*    we can also use it to replace ble_connect, and just need set the own_addr_type to 0(public) or 1(random).
* @param [in] address The address will be connected with our device
* @param [in] module
* @param [in] addr_type The type of peer device address
* @param [in] own_addr_type The type of own address(0 or 1 for unencrypted adv, 2 or 3 for encrypted adv)
* @return The status of ble_connect_ext
* @retval BT_SUCCESS It means the ble connection has been established
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
*/
bt_status_t ble_connect_ext (bdaddr_t *address, uint8 module, uint8 addr_type, uint8 own_addr_type)
{
    if(bt_gatt_client_interface != NULL)
    {
        return bt_gatt_client_interface->connect_ext(address, module, addr_type, own_addr_type);
    }

    return BT_FAIL;
}

/**
 * @brief  cancel establish ble connect with another address
* @details This function is cancel establish ble connect with another address,if
*    it cannot do, this function will return BT_FAIL
* @param [in] address -The address will be connected with our device
* @return The status of ble_connect_cancel
* @retval BT_SUCCESS -It means the ble cancel connection has been established
* @retval BT_FAIL -It means the ble cancel connection was not established
*/
bt_status_t ble_connect_cancel(bdaddr_t *address)
{
    if(bt_gatt_client_interface != NULL)
    {
        return bt_gatt_client_interface->connect_cancel(address);
    }
    return BT_FAIL;
}

/**
* @brief   Disconnect the ble link with another address
* @details This function is to disconnect the ble link with another address,if the  bt_gatt_
*    client_interface has not been registered before,this function will return BT_FAIL
* @param [in] conn_handle
* @return The status of ble_disconnect
* @retval BT_SUCCESS It means the ble connection has been disconnected
* @retval BT_FAIL The bt_gatt_client_interface has not been registered before
*/
bt_status_t ble_disconnect(uint16 conn_handle)
{
    if(bt_gatt_client_interface != NULL)
    {
        return bt_gatt_client_interface->disconnect(conn_handle);
    }
    return BT_FAIL;
}

/**
 * @brief  TWS use this function
* @details This function is for TWS use,if the  bt_gatt_client_interface has not been registered before,
*    this function will return BT_FAIL
* @param [in] none
* @return The status of gc init
* @retval BT_SUCCESS -It means gc init has been established
* @retval BT_FAIL -It means the gc init was not established
*/
bt_status_t ble_gc_init(void)
{
    if(bt_gatt_client_interface != NULL)
    {
        return bt_gatt_client_interface->gc_init();
    }
    return BT_FAIL;
}

/**
* @brief   Update the ble connect parameters
* @details This function is to update the ble connect parameters,if the bt_gatt_server_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] param The parameters of gatt connection
* @return The status of ble_connect_para_update
* @retval BT_SUCCESS It means the ble connection parameters have been updated
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
*/
bt_status_t ble_connect_para_update(gatt_connect_param_t* param)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->update_connect_param(param);
    }
    return BT_FAIL;
}

/**
* @brief   Ble send notification to another address
* @details This function is to send notification by ble,if the bt_gatt_server_interface has
*    not been registered before,this function will return BT_FAIL
* @param [in] data_info The information of gatt le data
* @return The status of ble_send_notification
* @retval BT_SUCCESS It means the ble send notification to another address successfully
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
* @retval BT_RETRY It means the data sending in the L2CAP link is suspended
* @retval BT_DISCONNECTED It means the connection with client is disconnected
*/
bt_status_t ble_send_notification(gatt_le_data_info_t *data_info)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->send_notification(data_info, GATT_NOTIFICATION);
    }
    return BT_FAIL;
}

/**
* @brief   Ble send indication to another address
* @details This function is to send indication by ble,if the bt_gatt_server_interface has
*    not been registered before,this function will return BT_FAIL
* @param [in] data_info The information of gatt le data
* @return The status of ble_send_indication
* @retval BT_SUCCESS It means the ble send indication to another address successfully
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
* @retval BT_RETRY It means the data sending in the L2CAP link is suspended
* @retval BT_DISCONNECTED It means the connection with client is disconnected
*/
bt_status_t ble_send_indication(gatt_le_data_info_t *data_info)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->send_notification(data_info, GATT_INDICATION);
    }
    return BT_FAIL;
}

/**
* @brief   Set ble smp pair mode
* @details This function is to set ble smp pair mode,if the bt_gatt_smp_interface has
*    not been registered before,this function will return BT_FAIL
* @param [in] pair_mode The ble smp pair_mode will be set
* @param [in] io_cap IO capabilities of some devices
* @return The status of ble_send_indication
* @retval BT_SUCCESS It means the ble send indication to another address successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
* @retval BT_INVALIDPARAM It means the input paramter is invalid
* @retval BT_NORESOURCES It means the resources are not enough
* @retval BT_RETRY It means the data sending in the L2CAP link is suspended
* @retval BT_DISCONNECTED It means the connection with client is disconnected
*/
bt_status_t ble_smp_pair_mode_set(uint8 pair_mode, uint8 io_cap)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->set_pair_mode(pair_mode, io_cap);
    }
    return BT_FAIL;
}

/**
* @brief   Set ble smp pair passkey
* @details This function is to set ble smp pair passkey,if the bt_gatt_smp_interface has
*    not been registered before,this function will return BT_FAIL
* @param [in] passKey The passkey or pin code will be set
* @return The status of ble_smp_pair_passkey_set
* @retval BT_SUCCESS It means set ble smp pair passkey successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
*/
bt_status_t ble_smp_pair_passkey_set(uint32 passkey)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->set_pair_passkey(passkey);
    }
    return BT_FAIL;
}

/**
* @brief   Request security from slave of ble smp
* @details This function is to request security from slave of ble smp,if the bt_gatt_smp_interface has
*    not been registered before,this function will return BT_FAIL
* @param [in] void
* @return The status of ble_smp_slave_sec_request
* @retval BT_SUCCESS It means request security from slave of ble smp successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
*/
bt_status_t ble_smp_slave_sec_request(void)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->slave_sec_request();
    }
    return BT_FAIL;
}

/**
* @brief   Request security with another address from master of ble smp
* @details This function is to request security another address from master of ble smp,if the bt_gatt_smp_
*    interface hasnot been registered before,this function will return BT_FAIL
* @param [in] address The address ble smp request security to
* @return The status of ble_smp_master_sec_request
* @retval BT_SUCCESS It means request security from master of ble smp successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_smp_master_sec_request(bdaddr_t address)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->master_sec_request(address);
    }
    return BT_FAIL;
}

/**
* @brief   Delete paired info of device from device list
* @details This function is to delete paired info of device from device list,if the bt_gatt_smp_
*    interface hasnot been registered before,this function will return BT_FAIL
* @param [in] address The address of paired device
* @return The status of ble_smp_delete_pair_device
* @retval BT_SUCCESS It means delete paired info of device from device list successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_smp_delete_pair_device(bdaddr_t address)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->delete_pair_device(address);
    }
    return BT_FAIL;
}

/**
* @brief   Remove device from resolving list in controller
* @details This function is to remove device from resolving list,if the bt_gatt_smp_
*    interface hasnot been registered before,this function will return BT_FAIL
*    we should call it when reconnect to unencrypted adv with smp.
* @param [in] address The address of device will be removed
* @return The status of this function
* @retval BT_SUCCESS It means remove device from resolving list successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_smp_remove_device_from_resolving_list(bdaddr_t address)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->remove_device_from_resolving_list(address);
    }
    return BT_FAIL;
}

/**
* @brief   Enable the resolving list in controller
* @details This function is to enable the resolving list in controller,if the bt_gatt_smp_
*    interface hasnot been registered before,this function will return BT_FAIL
* @param [in] privacy_enable 0->disable 1->enable
* @return The status of this function
* @retval BT_SUCCESS It means enable the resolving list in controller successfully
* @retval BT_FAIL The bt_gatt_smp_interface has not been registered before
* @retval BT_NORESOURCES It means the resources are not enough
*/
bt_status_t ble_smp_resolving_list_enable(UINT8 privacy_enable)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->resolving_list_enable(privacy_enable);
    }
    return BT_FAIL;
}

/**
* @brief   Find whether the rpa address scaned by master is in the device list or not
* @details This function is to find whether the rpa address scaned by master is in the device list or not,
*    if the bt_gatt_smp_interface hasnot been registered before,this function will return NUL
*    it used for encrypted adv, when master scaned encrypted adv, we can call this function to decide to
*    start reconnect or not.
* @param [in] rpa The rpa address scaned by master
* @return NULL or Not NULL
* @retval Not NULL It means the rpa address is in the device list
* @retval NULL It means the rpa address is not in the device list
*/
void *ble_smp_rpa_calc(bdaddr_t *rpa)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->smp_rpa_calc(rpa);
    }
    return NULL;
}

/**
* @brief   Find whether the identify address scaned by master is in the device list or not
* @details This function is to find whether the identify address scaned by master is in the device list or not,
*    if the bt_gatt_smp_interface hasnot been registered before,this function will return NULL
*    it used for unencrypted adv, when master scaned unencrypted adv, we can call this function to decide to
*    start reconnect or not.
* @param [in] ida The identify address scaned by master
* @return NULL or Not NULL
* @retval Not NULL It means the identify address is in the device list
* @retval NULL It means the identify address is not in the device list
*/
void *ble_smp_ida_calc(bdaddr_t *ida)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->smp_ida_calc(ida);
    }
    return NULL;
}

/**
* @brief   Get device list from stack
* @details This function is to get device list from stack(contained ltk/irk/address/name and so on),
*    if the bt_gatt_smp_interface hasnot been registered before,this function will return 0
*    it gets the device paired info which can used for reconnect, if smp is used. we save it in running nv.
* @param [in] buf The buffer used to store device list
* @return the length of device list
* @retval Not 0 It means the length of device list
* @retval 0 The bt_gatt_smp_interface has not been registered before
*/
UINT32 ble_smp_get_pair_info(void **buf)
{
    if(bt_gatt_smp_interface != NULL)
    {
        return bt_gatt_smp_interface->get_pair_info(buf);
    }
    return 0;
}

/**
* @brief   Restore device list to stack
* @details This function is to restore device list to stack(contained ltk/irk/address/name and so on),
*    if the bt_gatt_smp_interface hasnot been registered before,this function will return 0
*    when bt restart, we should call this function, if smp is used.
* @param [in] buf The buffer of device list
* @return void
*/
void ble_smp_restore_pair_info(void *buf)
{
    if(bt_gatt_smp_interface != NULL)
    {
        bt_gatt_smp_interface->restore_pair_info(buf);
    }
}

/**
* @brief   Add ble service
* @details This function is to add ble service, if the bt_gatt_server_interface has not been
*    registered before,this function will return BT_FAIL
* @param [in] pElementArray The elements in ble add service
* @param [in] numElement The number of element array
* @return The status of ble_add_service
* @retval BT_SUCCESS It means add ble service successfully
* @retval BT_FAIL The bt_gatt_server_interface has not been registered before
*/
bt_status_t ble_add_service(gatt_element_t *pElementArray, UINT16 numElement)
{
    if(bt_gatt_server_interface != NULL)
    {
        return bt_gatt_server_interface->add_services(pElementArray, numElement);
    }
    return BT_FAIL;
}

/**
* @brief   Set ble ibeacon data
* @details This function is to set ble ibeacon data
* @param [in] uuid The uuid of ibeacon data
* @param [in] major The major of ibeacon data
* @param [in] minor The minor of ibeacon data
* @return The status of ble_beacon_data_set
* @retval BT_SUCCESS It means set ble ibeacon data successfully
*/
bt_status_t ble_beacon_data_set(UINT8 uuid[16], UINT8 major[2], UINT8 minor[2])
{
    //ibeacon 9 bytes prefix is fixed
    UINT8 adv_data[31] = {0x02,0x01,0x1A,0x1A,0xFF,0x4C,0x00,0x02,0x15};

    OSI_LOGI(0, "ble_beacon_data_set enter");

    //uuid is 16 bytes
    memcpy(adv_data + 9, uuid, 16);

    //major is 2 bytes
    memcpy(adv_data + 25, major, 2);

    //minor is 2 bytes
    memcpy(adv_data + 27, minor, 2);

    ////tx power of one meter for ibeacon, default is -59dBm
    adv_data[30] = 0xC5;

    ble_adv_data_set(adv_data, 30);

    return BT_SUCCESS;
}

//
// GATT Client Interface
//

/**
* @brief  Ble client init
* @details This function is initialize gatt client module,if the bt_gatt_client_interface has not been
*    registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT";
* @param [in] uuid -uuid should be 0;
* @param [in] func -message call back function
* @return The status of ble_client_init
* @retval BT_SUCCESS It means the ble client init has been established
* @retval BT_FAIL It means the ble client init was not established
*/

bt_status_t ble_client_init(UINT16 module, UINT16 uuid, ap_module_cb func)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->ble_client_init(module,uuid,func);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client discover all primary service
* @details This function is discover all primary service of remode device,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] acl_handle -handle of this ble link
* @return The status of ble_client_init
* @retval BT_SUCCESS -It means the ble client discover all primary service has been established
* @retval BT_FAIL -It means the ble client discover all primary service was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client discover all primary service is invalid
*/
bt_status_t ble_client_discover_all_primary_service(UINT16 module, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->discover_all_primary_service(module,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client discover primary service by uuid
* @details This function is discover primary service of known uuid,if the bt_gatt_client_interface has not been
*    registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] uuid_l -long uuid address
* @param [in] uuid_s -short uuid value
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client discover primary service by uuid
* @retval BT_SUCCESS -It means the ble client discover primary service by uuid has been established
* @retval BT_FAIL -It means the ble client discover primary service by uuid was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client discover primary service by uuid is invalid
*/
bt_status_t ble_client_discover_primary_service_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->discover_primary_service_by_uuid(module,uuid_l,uuid_s,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client discover all characteristic
* @details This function is discover all characteristic from start handle to end handle,if the
*    bt_gatt_client_interface has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] start_handle -start handle we want to disover
* @param [in] end_handle -end handle we want to disover
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client discover primary service by uuid
* @retval BT_SUCCESS -It means the ble client discover all characteristic has been established
* @retval BT_FAIL -It means the ble client discover all characteristic was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client discover all characteristic is invalid
*/
bt_status_t ble_client_discover_all_characteristic(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->discover_all_characteristic(module,start_handle,end_handle,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client read char value by handle
* @details This function is read the char value of known handle,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -att handle
* @param [in] is_long_att -is log char value or not
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client read char value by_handle
* @retval BT_SUCCESS -It means the ble client read char value by handle has been established
* @retval BT_FAIL -It means the ble client read char value by_handle was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client read char value by handle is invalid
*/
bt_status_t ble_client_read_char_value_by_handle(UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 offset, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->read_char_value_by_handle(module,att_handle,is_long_att, offset, acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client read char value by uuid
* @details This function is read the char value of uuid,if the bt_gatt_client_interface has not been
*    registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] uuid_l -long uuid
* @param [in] uuid_s -short uuid
* @param [in] start_handle -start handle
* @param [in] end_handle -end handle
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client read char value by uuid
* @retval BT_SUCCESS -It means the ble client read char value by uuid has been established
* @retval BT_FAIL -It means the ble client read char value by uuid was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client read char value by uuid is invalid
*/
bt_status_t ble_client_read_char_value_by_uuid(UINT16 module, UINT8 *uuid_l, UINT16 uuid_s, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->read_char_value_by_uuid(module,uuid_l,uuid_s,start_handle,end_handle,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client read multiple char value
* @details This function is read multiple char value from service by handle,if the bt_gatt_client_interface
*    has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] handle -handle
* @param [in] length -length
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client read multiple char valu
* @retval BT_SUCCESS -It means the ble client read multiple char value has been established
* @retval BT_FAIL -It means the ble client read multiple char value was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client read multiple char value is invalid
*/
bt_status_t ble_client_read_multiple_char_value(UINT16 module, UINT8* handle, UINT8 length, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->read_multiple_char_value(module,handle,length,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client write char value
* @details This function is write char value of known att handle,if the bt_gatt_client_interface has not
*    been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] value -value address
* @param [in] length -length of value
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client write char value
* @retval BT_SUCCESS -It means the ble client write char value has been established
* @retval BT_FAIL -It means the ble client write char value was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client write char value is invalid
*/
bt_status_t ble_client_write_char_value(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle, UINT16 offset, UINT8 islong)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->write_char_value(module,att_handle,value,length,acl_handle, offset, islong);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client write char value without rsp
* @details This function is write char value with write cmd pdu,witch need not response,if the bt_gatt_
*    client_interface has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -att handle
* @param [in] value -value address
* @param [in] length -length of value
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client write char value without rsp
* @retval BT_SUCCESS -It means the ble client write char value without rsp has been established
* @retval BT_FAIL -It means the ble client write char value without rsp was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client write char value without rsp is invalid
*/
bt_status_t ble_client_write_char_value_without_rsp(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->write_char_value_without_rsp(module,att_handle,value,length,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client write char value signed
* @details This function is write char value with signed write cmd pdu,witch need not response,if the
*    bt_gatt_client_interface has not been registered before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -chara value handle
* @param [in] value -value address
* @param [in] length -length of value
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client write char value signed
* @retval BT_SUCCESS -It means the ble client write char value signed has been established
* @retval BT_FAIL -It means the ble client write char value signed was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client write char value signed is invalid
*/
bt_status_t ble_client_write_char_value_signed(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->write_char_value_signed(module,att_handle,value,length,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client write char value reliable
* @details This function is write char value reliable,if the bt_gatt_client_interface has not been registered
*    before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -chara value handle
* @param [in] value -value address
* @param [in] length -length of value
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client write char value reliable
* @retval BT_SUCCESS -It means the ble client write char value reliable has been established
* @retval BT_FAIL -It means the ble client write char value reliable was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client write char value reliable is invalid
*/
bt_status_t ble_client_write_char_value_reliable(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->write_char_value_reliable(module,att_handle,value,length,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client find include service
* @details This function is include service,if the bt_gatt_client_interface has not been registered
*    before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] start_handle -start handle
* @param [in] end_handle -end handle
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client find include service
* @retval BT_SUCCESS -It means the ble client find include service has been established
* @retval BT_FAIL -It means the ble client find include service was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble client find include service is invalid
*/
bt_status_t ble_client_find_include_service(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->find_include_service(module,start_handle,end_handle,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client get char descriptor
* @details This function is to get client char descriptor,if the bt_gatt_client_interface has not been registered
*    before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] start_handle -start handle
* @param [in] end_handle -end handle
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client get char descriptor
* @retval BT_SUCCESS -It means the ble client get char descriptor has been established
* @retval BT_FAIL -It means the ble client get char descriptor was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble get char descriptor is invalid
*/
bt_status_t ble_client_get_char_descriptor(UINT16 module, UINT16 start_handle, UINT16 end_handle, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->get_char_descriptor(module,start_handle,end_handle,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client read char descriptor
* @details This function is to read client char descriptor,if the bt_gatt_client_interface has not been registered
*    before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -char value handle
* @param [in] is_long_att -is long att
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client get char descriptor
* @retval BT_SUCCESS -It means the ble client read char descriptor has been established
* @retval BT_FAIL -It means the ble client read char descriptor was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble read char descriptor is invalid
*/
bt_status_t ble_client_read_char_descriptor(UINT16 module, UINT16 att_handle, UINT8 is_long_att, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->read_char_descriptor(module,att_handle,is_long_att,acl_handle);
    }

    return BT_FAIL;
}

/**
* @brief  Ble client write char descriptor
* @details This function is to write client char descriptor,if the bt_gatt_client_interface has not been registered
*    before,this function will return BT_FAIL
* @param [in] module -Modeule id ,should be "BT_MODULE_GATT"
* @param [in] att_handle -att handle
* @param [in] value -value address
* @param [in] length -length
* @param [in] acl_handle -handle of this ble link
* @return The status of ble client get char descriptor
* @retval BT_SUCCESS -It means the ble client write char descriptor has been established
* @retval BT_FAIL -It means the ble client write char descriptor was not established
* @retval BT_INVALIDPARAM -It means the parameter of ble write char descriptor is invalid
*/
bt_status_t ble_client_write_char_descriptor(UINT16 module, UINT16 att_handle, void *value, UINT16 length, UINT16 acl_handle)
{
    if(bt_gatt_client_interface)
    {
        return bt_gatt_client_interface->write_char_descriptor(module,att_handle,value,length,acl_handle);
    }

    return BT_FAIL;
}



