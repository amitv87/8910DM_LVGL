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

#ifndef __TB_TEST_H__
#define __TB_TEST_H__

#include "osi_api.h"
#include "tb.h"

#ifdef __cplusplus
extern "C" {
#endif

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
extern void tbtest_data_get_ip_address(void);
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
