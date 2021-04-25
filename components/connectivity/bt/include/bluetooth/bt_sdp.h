/**
 * @file     bt_sdp.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020Äê10ÔÂ10ÈÕ
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef BLUEU_INCLUDE_BLUETOOTH_BT_SDP_H_
#define BLUEU_INCLUDE_BLUETOOTH_BT_SDP_H_

#include "bluetooth/bt_types.h"
#include "bluetooth/bluetooth.h"

typedef struct
{
    /*
     * status: 0 success
     * count: pro count
     * properties: type BT_PRO_UUIDS val bt_service_record
     */
    UINT32 (*remote_device_properties_callback)(UINT32 status, bdaddr_t *bd_addr, UINT32 count, bt_property_t *properties);
} btsdp_callbacks_t;

typedef struct
{
    bt_status_t (*init) (btsdp_callbacks_t *callbacks);

    /*
     * uuid:
     * avrcp target: 0x110C
     * avrcp controller: 0x110F
     * hfp hf: 0x111E
     * hfp ag: 0x111F
     * avdtp source: 0x110A
     * avdtp sink: 0x110B
     * serial port: 0x1101
     */
    int (*get_remote_service_record)(bdaddr_t *remote_addr, UINT32 *uuid, UINT32 uuid_count);
} btsdp_interface_t;


#endif /* BLUEU_INCLUDE_BLUETOOTH_BT_SDP_H_ */
