/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_app_feature_config.h"
#include "ql_api_osi.h"
#include "ql_log.h"
#include "nw_demo.h"
#include "gpio_demo.h"
#include "gpio_int_demo.h"
#include "datacall_demo.h"
#include "osi_demo.h"
#include "ql_dev_demo.h"
#include "adc_demo.h"
#include "led_cfg_demo.h"
#include "ql_sim_demo.h"
#include "power_demo.h"
#ifdef QL_APP_FEATURE_MMS
#include "mms_demo.h"
#endif

#ifdef QL_APP_FEATURE_UART
#include "uart_demo.h"
#endif
#ifdef QL_APP_FEATURE_AUDIO
#include "audio_demo.h"
#endif
#ifdef QL_APP_FEATURE_LCD
#include "lcd_demo.h"
#ifdef QL_APP_FEATURE_LVGL
#include "lvgl_demo.h"
#endif
#endif
#ifdef QL_APP_FEATURE_HTTP
#include "http_demo.h"
#endif
#ifdef QL_APP_FEATURE_FTP
#include "ftp_demo.h"
#endif
#ifdef QL_APP_FEATURE_MQTT
#include "mqtt_demo.h"
#endif
#ifdef QL_APP_FEATURE_SSL
#include "ssl_demo.h"
#endif
#ifdef QL_APP_FEATURE_PING
#include "ping_demo.h"
#endif
#ifdef QL_APP_FEATURE_NTP
#include "ntp_demo.h"
#endif
#ifdef QL_APP_FEATURE_LBS
#include "lbs_demo.h"
#endif
#ifdef QL_APP_FEATURE_SOCKET
#include "socket_demo.h"
#endif


#ifdef QL_APP_FEATURE_BT
#include "bt_demo.h"
#include "bt_a2dp_avrcp_demo.h"
#endif

#ifdef QL_APP_FEATURE_FILE
#include "ql_fs_demo.h"
#endif
#ifdef QL_APP_FEATURE_SMS
#include "sms_demo.h"
#endif
#ifdef QL_APP_FEATURE_VOICE_CALL
#include "voice_call_demo.h"
#endif
#ifdef QL_APP_FEATURE_GNSS
#include "gnss_demo.h"
#endif

#ifdef QL_APP_FEATURE_SPI
#include "spi_demo.h"
#endif

#ifdef QL_APP_FEATURE_SPI_FLASH
#include "spi_flash_demo.h"
#endif

#ifdef QL_APP_FEATURE_CAMERA
#include "camera_demo.h"
#endif

#ifdef QL_APP_FEATURE_WIFISCAN
#include "wifi_scan_demo.h"
#endif

#ifdef QL_APP_FEATURE_HTTP_FOTA
#include "fota_http_demo.h"
#endif

#ifdef QL_APP_FEATURE_DECODER
#include "decoder_demo.h"
#endif

#ifdef QL_APP_FEATURE_VOLTE
#include "volte_demo.h"
#endif

#ifdef QL_APP_FEATURE_KEYPAD
#include "keypad_demo.h"
#endif

#ifdef QL_APP_FEATURE_RTC
#include "ql_rtc_demo.h"
#endif

#ifdef QL_APP_FEATURE_SECURE_BOOT
#include "ql_api_dev.h"
#endif

#ifdef QL_APP_FEATURE_USB_CHARGE
#include "charge_demo.h"
#endif

#ifdef QL_APP_FEATURE_VIRT_AT
#include "ql_virt_at_demo.h"
#endif

#define QL_INIT_LOG_LEVEL	QL_LOG_LEVEL_INFO
#define QL_INIT_LOG(msg, ...)			QL_LOG(QL_INIT_LOG_LEVEL, "ql_INIT", msg, ##__VA_ARGS__)
#define QL_INIT_LOG_PUSH(msg, ...)	QL_LOG_PUSH("ql_INIT", msg, ##__VA_ARGS__)

static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void ql_init_demo_thread(void *param)
{
    QL_INIT_LOG("init demo thread enter, param 0x%x", param);

/*Caution:If the macro of secure boot and the function are opened, download firmware and restart will enable secure boot.
          the secret key cannot be changed forever*/    
#ifdef QL_APP_FEATURE_SECURE_BOOT
    //ql_dev_enable_secure_boot();
#endif

#if 0
    ql_gpio_app_init();
    ql_gpioint_app_init();
#endif

#ifdef QL_APP_FEATURE_LEDCFG
    ql_ledcfg_app_init();
#endif

#ifdef QL_APP_FEATURE_AUDIO
    //ql_audio_app_init();
#endif

#ifdef QL_APP_FEATURE_LCD
    //ql_lcd_app_init();
#ifdef QL_APP_FEATURE_LVGL
    ql_lvgl_app_init();
	ic_main_entry();//功能入口
#endif
#endif

    //ql_nw_app_init();
    //ql_datacall_app_init();
	//ql_osi_demo_init();

#ifdef QL_APP_FEATURE_FILE
	//ql_fs_demo_init();
#endif

    //ql_dev_app_init();
    //ql_adc_app_init();
#ifdef QL_APP_FEATURE_UART
    //ql_uart_app_init();
#endif

#ifdef QL_APP_FEATURE_BT
	//ql_bt_spp_server_demo_init();
	//ql_bt_spp_client_demo_init();
	//ql_ble_gatt_server_demo_init();
    //ql_ble_gatt_client_demo_init();
    //ql_bt_hfp_demo_init();
    //ql_bt_a2dp_avrcp_app_init();
#endif
    //ql_sim_app_init();
    //ql_power_app_init();
    
#ifdef QL_APP_FEATURE_FTP
	//ql_ftp_app_init();
#endif
#ifdef QL_APP_FEATURE_HTTP
	//ql_http_app_init();
#endif

#ifdef QL_APP_FEATURE_MQTT
	//ql_mqtt_app_init();
#endif
#ifdef QL_APP_FEATURE_SSL
	//ql_ssl_app_init();
#endif
#ifdef QL_APP_FEATURE_PING
	//ql_ping_app_init();
#endif
#ifdef QL_APP_FEATURE_NTP
	//ql_ntp_app_init();
#endif
#ifdef QL_APP_FEATURE_LBS
	//ql_lbs_app_init();
#endif
#ifdef QL_APP_FEATURE_SOCKET
	//ql_socket_app_init();
#endif
#ifdef QL_APP_FEATURE_MMS
   //ql_mms_app_init();
#endif

#ifdef QL_APP_FEATURE_SMS
	//ql_sms_app_init();
#endif
#ifdef QL_APP_FEATURE_VOICE_CALL
	//ql_voice_call_app_init();
#endif
#ifdef QL_APP_FEATURE_VOLTE
	//ql_volte_app_init();
#endif

#ifdef QL_APP_FEATURE_GNSS
    //ql_gnss_app_init();
#endif

#ifdef QL_APP_FEATURE_SPI
    //ql_spi_demo_init();
#endif

#ifdef QL_APP_FEATURE_CAMERA
    //ql_camera_app_init(); 
#endif

#ifdef QL_APP_FEATURE_SPI_FLASH
    //ql_spi_flash_demo_init();
#endif

#if defined (QL_APP_FEATURE_WIFISCAN)
    //ql_wifiscan_app_init();
#endif

#ifdef QL_APP_FEATURE_HTTP_FOTA
	//ql_fota_http_app_init();
#endif
#ifdef QL_APP_FEATURE_DECODER
    //ql_decoder_app_init();
#endif

#ifdef QL_APP_FEATURE_KEYPAD
    //ql_keypad_app_init();
#endif

#ifdef QL_APP_FEATURE_RTC
    //ql_rtc_app_init();
#endif

#ifdef QL_APP_FEATURE_USB_CHARGE
    //ql_charge_app_init();
#endif

#ifdef QL_APP_FEATURE_VIRT_AT
    //ql_virt_at_app_init();
#endif
#ifdef QL_APP_FEATURE_ALI_LINKSDK
    //ql_ali_sdk_mqtt_client_test_init();
    //ql_ali_sdk_bootstrap_test_init();
    //ql_ali_sdk_cota_test_init();
    //ql_ali_sdk_das_test_init();
    //ql_ali_sdk_data_model_test_init();
    //ql_ali_sdk_devinfo_test_init();
    //ql_ali_sdk_diag_test_init();
    //ql_ali_sdk_dynreg_test_init();
    //ql_ali_sdk_dynregmq_test_init();
    //ql_ali_sdk_fota_test_init();
    //ql_ali_sdk_http_client_test_init();
    //ql_ali_sdk_logpost_test_init();
    //ql_ali_sdk_mqtt_broadcast_test_init();
    //ql_ali_sdk_rrpc_test_init();
    //ql_ali_sdk_ntp_test_init();
    //ql_ali_sdk_shadow_test_init();
#endif
    ql_rtos_task_sleep_ms(1000); /*Chaos change: set to 1000 for the camera power on*/
    ql_rtos_task_delete(NULL);
}

int appimg_enter(void *param)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t ql_init_task = NULL;
	
    QL_INIT_LOG("init demo enter: %s", QL_APP_VERSION);
    prvInvokeGlobalCtors();
	
    err = ql_rtos_task_create(&ql_init_task, 1024, APP_PRIORITY_NORMAL, "ql_init", ql_init_demo_thread, NULL, 1);
	if(err != QL_OSI_SUCCESS)
    {
		QL_INIT_LOG("init failed");
	}
    return err;
}

void appimg_exit(void)
{
    QL_INIT_LOG("init demo exit");
}
