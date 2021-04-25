/*
* Copyright (c) 2020, Unisoc Communications Inc. All Rights Reserved.
** Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the 'Software'), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "bt_drv.h"
#include "bt_app.h"
#include "bt_gatt_client_demo.h"
#include "bt_gatt_server_demo.h"
#include "test/bt_at.h"

void app_ble_mtu_exchange_result_cb(UINT16 handle, UINT16 mtu)
{
    SCI_TRACE_LOW("[BLE_client]mtu_exchange - MTU = 0x%x", mtu);
}

bt_status_t app_ble_conn_param_update_cb(UINT8 status, gatt_connect_param_t *param)
{
    OSI_LOGI(0, "app_ble_conn_param_update_cb enter");
    return BT_SUCCESS;
}

btgatt_callback_t btgatt_callback =
    {
        .conn_param_update_result_cb = app_ble_conn_param_update_cb,
        .mtu_exchange_result_cb = app_ble_mtu_exchange_result_cb,
};

/**
 * @brief   ble client demo
 * @describe it will excute scanning ,and connecting device of "unisoc_test" with automatically.
 *                when connected, it will discovery service and charactor
 * @param
 *              AT+BTAPP=ble_client_demo,1  => start demo
 *              AT+BTAPP=ble_client_demo,0  => stop demo
 * @return
 */
static int ble_client_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len)
{
    btgatt_client_callbacks_t *btgatt_client_cb = NULL;
    const bt_interface_t *app_bt_if = NULL;
    const btgatt_interface_t *p_gatt_if = NULL;

    OSI_LOGI(0, "ble_client_demo_handle, argv[1]=%d\n", argv[1]);

    if (0 == strncmp(argv[1], "1", 1))
    {
        app_bt_if = get_bt_interface();
        p_gatt_if = (btgatt_interface_t *)app_bt_if->get_profile_interface(BT_PROFILE_GATT_ID);

        //get ble client callback
        example_ble_client_info_get(&btgatt_client_cb);
        btgatt_callback.client = btgatt_client_cb;

        //register ble callback
        p_gatt_if->init(&btgatt_callback);

        //start BT
        if (app_bt_start() != BT_PENDING)
        {
            sprintf(rsp, "%s\r\n", "+BTAPP:last procces ongoing, please try again");
            *rsp_len = strlen(rsp);

            return 0;
        }

        //delay 1000ms,wait BT chip boot complete
        osiThreadSleep(1000);

        //start scanning
        Gatt_client_demo_function();

        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt starting");
        //        *rsp_len = strlen(rsp);
    }
    else
    {
        //stop BT
        app_bt_stop();

        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt stoping");
        //        *rsp_len = strlen(rsp);
    }

    return 0;
}

/**
 * @brief   ble server demo
 * @describe it will excute advertise data
 * @param
 *              AT+BTAPP=ble_server_demo,1  => start demo
 *              AT+BTAPP=ble_server_demo,0  => stop demo
 * @return
 */
static int ble_server_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len)
{
    btgatt_server_callbacks_t *btgatt_server_cb = NULL;
    const bt_interface_t *app_bt_if = NULL;
    const btgatt_interface_t *p_gatt_if = NULL;

    OSI_LOGI(0, "ble_server_demo_handle, argv[1]=%s\n", argv[1]);

    if (0 == strncmp(argv[1], "1", 1))
    {
        app_bt_if = get_bt_interface();
        p_gatt_if = (btgatt_interface_t *)app_bt_if->get_profile_interface(BT_PROFILE_GATT_ID);

        //get ble server callback
        example_ble_server_info_get(&btgatt_server_cb);
        btgatt_callback.server = btgatt_server_cb;

        //register ble callback
        p_gatt_if->init(&btgatt_callback);

        //start BT
        if (app_bt_start() != BT_PENDING)
        {
            sprintf(rsp, "%s\r\n", "+BTAPP:last procces ongoing, please try again");
            *rsp_len = strlen(rsp);

            return 0;
        }

        //delay 1000ms,wait BT chip boot complete
        osiThreadSleep(1000);

        //start adv
        Gatt_server_demo_function();

        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt starting");
        //        *rsp_len = strlen(rsp);
    }
    else
    {
        //stop BT
        app_bt_stop();
        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt stoping");
        //        *rsp_len = strlen(rsp);
    }

    return 0;
}

/**
 * @brief   bt headset demo
 * @describe it will excute A2DP sink and HFP HF process when connected by phone
 * @param
 *              AT+BTAPP=bt_headset_demo,1  => start demo
 *              AT+BTAPP=bt_headset_demo,2 => set volume 
 *              AT+BTAPP=bt_headset_demo,3 => switch songs
 *              AT+BTAPP=bt_headset_demo,4 => call
 *              AT+BTAPP=bt_headset_demo,0  => stop demo
 * @return
 */
static int bt_headset_demo_handle(char *argv[], int argc, char *rsp, int *rsp_len)
{
    const char *dev_name = "8910_headset";
    OSI_LOGI(0, "bt_server_demo_handle, argv[1]=%s\n", argv[1]);

    if (0 == strncmp(argv[1], "1", 1))
    {
        //start BT
        if (app_bt_start() != BT_PENDING)
        {
            sprintf(rsp, "%s\r\n", "+BTAPP:last procces ongoing, please try again");
            *rsp_len = strlen(rsp);

            return 0;
        }

        //delay 1000ms,wait BT chip boot complete
        osiThreadSleep(1000);

        //set devices default name
        bt_local_name_change((uint8 *)dev_name);

        //enable devices discoverable and connectable
        app_bt_scan_mode_set(1, 1);

#if BT_HFP_SUPPORT
        //init HFP
        extern void app_bt_hfp_init();
        app_bt_hfp_init();
#endif

#if BT_AVRCP_SUPPORT
        //init avrcp
        extern void app_bt_avrcp_init();
        app_bt_avrcp_init();
#endif

#if BT_A2DP_SUPPORT
        //init a2dp
        extern void app_bt_a2dp_init();
        app_bt_a2dp_init();
#endif

        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt starting");
        //        *rsp_len = strlen(rsp);
    }
    else if (0 == strncmp(argv[1], "2", 1))
    {
        if (NULL != argv[2])
        {
            if (0 == strncmp(argv[2], "?", 1))
            {
                int8 device_volume = app_bt_avrcp_volume_get();
                sprintf(rsp, "+BTAPP:device_volume = %d\r\n", device_volume);
            }
            else
            {
                uint8 volume = strtoul(argv[2], NULL, 10);
                app_bt_avrcp_volume_set(volume);
                sprintf(rsp, "%s\r\n", "+BTAPP:set volume OK");
            }
        }
        *rsp_len = strlen(rsp);
    }
    else if (0 == strncmp(argv[1], "3", 1))
    {
        if (NULL != argv[2])
        {
            if (0 == strncmp(argv[2], "next", 4))
            {
                app_bt_avrcp_handle(BT_AVRCP_NEXT);
                sprintf(rsp, "%s\r\n", "+BTAPP:switch songs next OK");
            }
            else if (0 == strncmp(argv[2], "previ", 5))
            {
                app_bt_avrcp_handle(BT_AVRCP_PREVI);
                sprintf(rsp, "%s\r\n", "+BTAPP:switch songs previ OK");
            }
            else if (0 == strncmp(argv[2], "pause", 5))
            {
                app_bt_avrcp_handle(BT_AVRCP_PAUSE);
                sprintf(rsp, "%s\r\n", "+BTAPP:switch songs pause OK");
            }
            else if (0 == strncmp(argv[2], "play", 4))
            {
                app_bt_avrcp_handle(BT_AVRCP_START);
                sprintf(rsp, "%s\r\n", "+BTAPP:switch songs play OK");
            }
            else
            {
                sprintf(rsp, "%s\r\n", "+BTAPP:switch songs error");
            }
        }
        *rsp_len = strlen(rsp);
    }
    else if (0 == strncmp(argv[1], "4", 1))
    {
        if (NULL != argv[2])
        {
            if (0 == strncmp(argv[2], "call", 4))
            {
                OSI_LOGI(0, "ble_headset_demo_handle:call");
                app_bt_hfp_call_answer(1);
                sprintf(rsp, "%s\r\n", "+BTAPP:call answer OK");
            }
            else if (0 == strncmp(argv[2], "rejectcall", 10))
            {
                OSI_LOGI(0, "ble_headset_demo_handle:rejectcall");
                app_bt_hfp_call_answer(0);
                sprintf(rsp, "%s\r\n", "+BTAPP:call reject OK");
            }
            else if (0 == strncmp(argv[2], "hangup", 6))
            {
                OSI_LOGI(0, "ble_headset_demo_handle:hangup");
                app_bt_hfp_call_terminate();
                sprintf(rsp, "%s\r\n", "+BTAPP:hang up OK");
            }
            else
            {
                sprintf(rsp, "%s\r\n", "+BTAPP:call answer string error");
            }
        }
        *rsp_len = strlen(rsp);
    }
    else
    {
        extern void bt_app_hfp_stop(void);
        bt_app_hfp_stop();
        extern void bt_app_a2dp_stop(void);
        bt_app_a2dp_stop();
        extern void app_bt_avrcp_stop(void);
        app_bt_avrcp_stop();
        //stop BT
        app_bt_stop();

        //        sprintf(rsp, "%s\r\n", "+BTAPP:bt stoping");
        //        *rsp_len = strlen(rsp);
    }

    return 0;
}

bt_eut_cmd_t bt_app_cmds[] =
    {
        {0, "ble_client_demo", ble_client_demo_handle, NULL},
        {0, "ble_server_demo", ble_server_demo_handle, NULL},
        {0, "bt_headset_demo", bt_headset_demo_handle, NULL},

        {0, NULL, NULL, NULL}, //donot deleted, it's important
};
