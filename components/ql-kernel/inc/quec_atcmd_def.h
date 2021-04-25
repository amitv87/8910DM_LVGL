��?/**  @file
  quec_atcmd_def.h

  @brief
  This file is used to define at command for different Quectel Project.

*/

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
#ifndef QUEC_ATCMD_DEF_H
#define QUEC_ATCMD_DEF_H

#include "quec_cust_feature.h"

#if QUEC_MYFUNC_SUPPORT
+MYFUNC, callback_funct , 0  
#endif

#if MY_TEST_BT_FUNC
+QTESTDEMO,atStartDemo, 0
#endif

//********************** TCPIP/udp at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_TCPIP
#ifdef CONFIG_QUEC_PROJECT_FEATURE_TCPIP_AT
+QICSGP,     at_tcpip_qicsgp_cmd_func,         0
+QIACT,      at_tcpip_qiact_cmd_func,          0
+QIACTEX,    at_tcpip_qiactex_cmd_func,        0
+QIDEACT,    at_tcpip_qideact_cmd_func,        0
+QIDEACTEX,  at_tcpip_qideactex_cmd_func,      0
+QIOPEN,     at_tcpip_qiopen_cmd_func,         0
+QISEND,     at_tcpip_qisend_cmd_func,         0
+QISENDEX,   at_tcpip_qisendex_cmd_func,       0
+QISDE,      at_tcpip_qisde_cmd_func,          0
+QIRD,       at_tcpip_qird_cmd_func,           0
+QIACCEPT,   at_tcpip_qiaccept_cmd_func,       0
+QICLOSE,    at_tcpip_qiclose_cmd_func,        0
+QISTATE,    at_tcpip_qistate_cmd_func,        0
+QISWTMD,    at_tcpip_qiswtmd_cmd_func,        0
+QICFG,      at_tcpip_qicfg_cmd_func,          0
+QIGETERROR, at_tcpip_qigeterror_cmd_func,     0
+QIDNSCFG,   at_tcpip_qidnscfg_cmd_func,       0
+QIDNSGIP,   at_tcpip_qidnsgip_cmd_func,       0

/************************ping at cmd ***************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PING_AT
+QPING,      at_ping_qping_cmd_func,           0
#endif

/************************ntp at cmd ***************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NTP_AT
+QNTP, 	     at_ntp_qntp_cmd_func,		       0
#endif
//********************** ssl at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SSL
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SSL_AT
+QSSLCFG,	 at_ssl_qsslcfg_cmd_func,		   0
+QSSLOPEN,	 at_ssl_qsslopen_cmd_func,		   0
+QSSLSTATE,  at_ssl_qsslstate_cmd_func, 	   0
+QSSLSEND,	 at_ssl_qsslsend_cmd_func,		   0
+QSSLRECV,	 at_ssl_qsslrecv_cmd_func,		   0
+QSSLCLOSE,  at_ssl_qsslclose_cmd_func, 	   0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_SSL_AT*/
#endif

#endif /*CONFIG_QUEC_PROJECT_FEATURE_TCPIP_AT*/
#endif


//********************** http at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_HTTP
#ifdef CONFIG_QUEC_PROJECT_FEATURE_HTTP_AT
+QHTTPURL,       at_http_qhttpurl_cmd_func,        0
+QHTTPCFG,       at_http_qhttpcfg_cmd_func,        0
+QHTTPGET,       at_http_qhttpget_cmd_func,        0
+QHTTPGETEX,	 at_http_qhttpgetex_cmd_func, 	   0
+QHTTPPOST,      at_http_qhttppost_cmd_func,       0
+QHTTPREAD,      at_http_qhttpread_cmd_func,       0
+QHTTPPUT,       at_http_qhttpput_cmd_func,        0
+QHTTPPOSTFILE,  at_http_qhttppostf_cmd_func,	   0
+QHTTPREADFILE,  at_http_qhttpreadf_cmd_func,	   0
+QHTTPSTOP,      at_http_qhttpstop_cmd_func,       0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_HTTP_AT*/
#endif

//********************** mqtt at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_MQTT
#ifdef CONFIG_QUEC_PROJECT_FEATURE_MQTT_AT
+QMTCFG,   at_mqtt_qmtcfg_cmd_func,            0
+QMTOPEN,  at_mqtt_qmtopen_cmd_func,           0
+QMTCONN,  at_mqtt_qmtconn_cmd_func,           0
+QMTSUB,   at_mqtt_qmtsub_cmd_func,            0
+QMTUNS,   at_mqtt_qmtunsub_cmd_func,          0
+QMTPUB,   at_mqtt_qmtpub_cmd_func,            0
+QMTPUBEX, at_mqtt_qmtpub_cmd_func, 		   0
+QMTDISC,  at_mqtt_qmtdisc_cmd_func,           0
+QMTRECV,  at_mqtt_qmtrecv_cmd_func,           0
+QMTCLOSE, at_mqtt_qmtclose_cmd_func,          0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_MQTT_AT*/
#endif

//********************** mms at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_MMS
#ifdef CONFIG_QUEC_PROJECT_FEATURE_MMS_AT
+QMMSCFG,  at_mms_qmmscfg_cmd_func,            0
+QMMSEDIT, at_mms_qmmsedit_cmd_func,           0
+QMMSEND,  at_mms_qmmsend_cmd_func,            0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_MMS_AT*/
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_LBS
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LBS_AT
+QLBSCFG,  at_lbs_qlbscfg_cmd_func,            0
+QLBS,     at_lbs_qlbs_cmd_func,               0
+QLBSEX,   at_lbs_qlbs_ext_input_cmd_func,     0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_LBS_AT*/
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CTSREG_AT
+QIOTRPTCFG,  at_ctsreg_qiotrptcfg_cmd_func,            0
+QIOTRPT,     at_ctsreg_qiotrpt_cmd_func,               0
#endif

//********************** ftp at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_FTP_AT
+QFTPCFG,		at_ftp_qftpcfg_cmd_func,			0
+QFTPOPEN,		at_ftp_qftpopen_cmd_func,			0
+QFTPCLOSE,		at_ftp_qftpclose_cmd_func,			0
+QFTPPUT,		at_ftp_qftpput_cmd_func,			0
+QFTPGET,		at_ftp_qftpget_cmd_func,			0
+QFTPSIZE,		at_ftp_qftpsize_cmd_func,			0
+QFTPSTAT,		at_ftp_qftpstat_cmd_func,			0
+QFTPLEN,		at_ftp_qftplen_cmd_func,			0
+QFTPRENAME,	at_ftp_qftprename_cmd_func,			0
+QFTPDEL,   	at_ftp_qftpdel_cmd_func,			0
+QFTPMKDIR,		at_ftp_qftpmkdir_cmd_func,			0
+QFTPRMDIR,		at_ftp_qftprmdir_cmd_func,			0
+QFTPLIST,		at_ftp_qftplist_cmd_func,			0
+QFTPNLST,		at_ftp_qftpnlst_cmd_func,			0
+QFTPCWD,		at_ftp_qftpcwd_cmd_func,			0
+QFTPPWD,		at_ftp_qftppwd_cmd_func,			0
+QFTPMDTM,		at_ftp_qftpmdtm_cmd_func,			0
+QFTPMLSD,		at_ftp_qftpmlsd_cmd_func,			0
+QFTPDBG,       at_ftp_qftpdbg_cmd_func,            0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_FTP_AT*/

//********************** lwm2m at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LWM2M_AT
+QLWCFG,		dsat_lwm2m_exec_qlwcfg_cmd,			0
+QLWREG,		dsat_lwm2m_exec_qlwreg_cmd,			0
+QLWUPDATE,		dsat_lwm2m_exec_qlwupdate_cmd,		0
+QLWDEREG,		dsat_lwm2m_exec_qlwdereg_cmd,		0
+QLWSTAT,		dsat_lwm2m_exec_qlwstat_cmd,		0
//#ifdef CHINA_MOBILE_LWM2M_CLIENT	
+QDMCFG,		dsat_lwm2m_exec_qdmcfg_cmd,			0
+QDMSTART,		dsat_lwm2m_exec_qdmstart_cmd,		0
+QDMSTOP,		dsat_lwm2m_exec_qdmstop_cmd,		0
+QDMSTAT,		dsat_lwm2m_exec_qdmstat_cmd,		0
+QDMCFGEX,		dsat_lwm2m_exec_qdmcfgex_cmd,		0
//#endif
#endif


//********************** ssl at cmd ***********************/


//********************** quecthing at cmd ***********************/


//********************** queclocator at cmd ***********************/


//********************** mms at cmd ***********************/


//********************** common/platform at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_URC
+QURCCFG,   quec_exec_qurccfg_cmd,				0
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GENERAL_AT
#ifdef CONFIG_QL_OPEN_EXPORT_PKG
+QTESTDEMO,     quec_exec_at_start_app,         0
#endif
+QPOWD, 	quec_exec_qpowd_cmd , 				0
+QRESET,	quec_exec_qreset_cmd,				0
+RESET,	    quec_exec_qreset_cmd,				0
+QPROD, 	quec_exec_qprod_cmd,				0
+QDOWNLOAD, atCmdHandleFORCEDNLD,				0
+CVERSION,	quec_exec_cversion_cmd,				0
+CSUB,		quec_exec_csub_cmd,					0
+QGMR,		quec_exec_qgmr_cmd,					0
+QGSN,		quec_exec_qgsn_cmd,					0
+QINF,		quec_exec_qinf_cmd,					0
+QSVN,      at_qsvn_cmd_func,                   0
#ifdef QUECTEL_AT_AP_MODEM_NEW_VERSION
+QAPVER,	quec_exec_qapver_cmd,				0
+QSUB,		quec_exec_qsub_cmd,					0
+QAPSUB,	quec_exec_qapsub_cmd,				0
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_QDBGCFG_AT
+QDBGCFG,	quec_exec_qdbgcfg_cmd,				0
#endif
+QWSETMAC,	quec_exec_qwsetmac_cmd,				0
+QINDCFG,   quec_exec_qindcfg_cmd,				0
+QUSBCFG,   quec_exec_qusbcfg_cmd,              0
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
+CBAUD,     quec_exec_cbaud_cmd,                0
+QUARTCFG,  quec_exec_quartcfg_cmd,				0
+QIPR,      quec_exec_qipr_cmd,                 0
+QICF,      quec_exec_qicf_cmd,                 0
+QIFC,      quec_exec_qifc_cmd,                 0
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
S24,        quec_exec_ats24_cmd,                0     //add by sum 2021/2/25
+QSCLK,     quec_exec_qsclk_cmd,				0
+QSCLKEX,   quec_exec_qsclkex_cmd,              0
#endif
&C,         atCmdHandleAndC,                    0      
&V, 		atCmdHandleAndV,					0 
#ifdef CONFIG_QUEC_PROJECT_FEATURE_QCFG_AT
+QCFG,      quec_exec_qcfg_cmd,                 0
#endif
+QTEMP,		quec_exec_qtemp_cmd,				0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_GENERAL_AT*/

#ifdef CONFIG_QUEC_PROJECT_FEATURE_FOTA
#ifdef CONFIG_QUEC_PROJECT_FEATURE_FOTA_AT
+QFOTADL,   quec_exec_qfotadl_cmd,              0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_FOTA_AT*/
#endif/*CONFIG_QUEC_PROJECT_FEATURE_FOTA*/

#ifdef CONFIG_QUEC_PROJECT_FEATURE_GPIO_AT
+QGPIOR,	quec_exec_qgpior_cmd,				0
+QGPIOW,	quec_exec_qgpiow_cmd,				0
+QGPIOS,	quec_exec_qgpios_cmd,               0
+QPINS,     quec_exec_qpins_cmd,                0
#endif

#if defined(CONFIG_QUEC_PROJECT_FEATURE_ADC_AT) || defined(CONFIG_QUEC_PROJECT_FEATURE_GENERAL_AT)
+QADC,		quectel_exec_adc_cmd,				0
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_ADC_AT
+QADCCFG,	quec_exec_qadccfg_cmd,				0
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG_AT
+QLEDCFG, 	quec_exec_qledcfg_cmd, 				0
#ifndef CONFIG_QL_OPEN_EXPORT_PKG
+QPWMCFG,   quec_exec_qpwmcfg_cmd,              0
#endif
#ifdef QUEC_LED_TEST
+QLEDTEST,  quec_exec_qledtest_cmd,				0
#endif
+QLEDSTAT, 	quec_exec_qledstat_cmd, 			0
#ifdef QUEC_VIRTUAL_PWM
+QVPWM,     quec_exec_virtualpwm_cmd,           0
#endif
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO
#ifdef CONFIG_QUEC_PROJECT_FEATURE_AUDIO_AT
+QAUDRD,	  quec_exec_qaudrd_cmd,				0
+QAUDSTOP,	  quec_exec_qaudstop_cmd,			0
+QPSND,		  quec_exec_qpsnd_cmd,				0
+QAUDPLAY,	  quec_exec_qaudplay_cmd,           0
+CLVL,		  quec_exec_clvl_cmd,				0
+CMUT,		  quec_exec_cmut_cmd,				0
+QAUDMOD,	  quec_exec_qaudmod_cmd,			0
+QAUDPASW,    quec_exec_qaudpasw_cmd,			0
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EXT_CODEC
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EXT_CODEC_AT
+QIIC,		  quec_exec_qiic_cmd,				0
+QAUDSW,	  quec_exec_audsw_cmd,				0
+QDAI,	  	  quec_exec_qdai_cmd,				0
#endif
#endif
#ifdef QUEC_AT_CRSL_SUPPORT
+CRSL,        quec_exec_crsl_cmd,				0
#endif
+QAUDLOOP,	  quec_exec_qaudloop_cmd,			0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_AUDIO*/
#endif

//********************** network/modem/rf at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW_AT
+QNWINFO,   quec_exec_qnwinfo_cmd,              0
+QENG,      quec_exec_qeng_cmd,                 0
+QCSQ,      quec_exec_qcsq_cmd,                 0
+QLTS,   	quec_exec_qlts_cmd,                 0
+QGDCNT,    quec_exec_qgdcnt_cmd,               0
+QAUGDCNT,  quec_exec_qaugdcnt_cmd,             0
+QCELLINFO,	quec_exec_qcellinfo_cmd, 			0
+QDFTPDN,	quec_exec_qdftpdn_cmd,				0
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
+QNETDEVCTL,quec_exec_qnetdevctl_cmd,           0
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_RFTEST_AT
+QRXFTM,    quec_exec_qrxftm_cmd,               0
+QRFTEST,   quec_exec_qrftest_cmd,              0
+QRFTESTMODE, quec_exec_qrftestmode_cmd,        0
#endif  /* CONFIG_QUEC_PROJECT_FEATURE_RFTEST_AT */

//********************** sms/call/volte at cmd ***********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SIM_AT
+QSIMWRITECNT,  quectel_exec_qsimwritecnt_cmd,  0
+QCCID,         atCmdHandleCCID,                0
+ICCID,         atCmdHandleCCID,                0
+QINISTAT,      quectel_exec_qinistat_cmd,      0
+QSIMDET,       quectel_exec_qsimdet_cmd,       0
+QSIMSTAT,      quectel_exec_qsimstat_cmd,      0
+QPINC,			quec_exec_qpinc_cmd,			0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_SIM_AT*/

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS_AT
+CSCB,          atCmdHandleCSCB,                0
+QCMGS,         quectel_exec_qcmgs_cmd,         0
+QCMGR,         quectel_exec_qcmgr_cmd,         0
+QCSMP, 		quectel_exec_qcsmp_cmd, 		0
#endif 
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CALL_AT
+CVHU,          quectel_exec_cvhu_cmd,          0
+QCHLDIPMPTY,   quectel_exec_QCHLDIPMPTY_cmd,   0
S7,             quectel_exec_S7_cmd,            0
+QECCNUM,       quectel_exec_QECCNUM_cmd,       0
H0,		        atCmdHandleH,		            0
O0,			    atCmdHandleO,					0
+QHUP,          quectel_exec_QHUP_cmd,          0
^DSCI,          quec_exec_dsci_cmd,             0
+QIMS,          quec_exec_qims_cmd,             0
+CVMOD,         quec_exec_cvmod_cmd,            0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_CALL_AT*/
#endif

//************************* sim at cmd **************************/

	
//************************ audio at cmd *************************/


//************************* file at cmd *************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_FILE
#ifdef CONFIG_QUEC_PROJECT_FEATURE_FILE_AT
+QFUPL, 	    quec_exec_qfupl_cmd,		    0
+QFOPEN,        quec_exec_qfopen_cmd,			0
+QFLST,	        quec_exec_file_qflst_cmd,		0
+QFDEL, 		quec_exec_file_qfdel_cmd,		0
+QFCLOSE, 		quec_file_qfclose_cmd,    		0
+QFREAD,		quec_exec_file_qfread_cmd,      0
+QFLDS,         quec_exec_file_qflds_cmd,	    0
+QFWRITE,     	quec_exec_file_qfwrite_cmd,	    0
+QFDWL,         quec_exec_file_qfdwl_cmd, 		0
+QFPOSITION,	quec_exec_file_qfposition_cmd,  0
+QFSEEK,	    quec_exec_file_qfseek_cmd,		0
#endif
#ifdef CONFIG_QL_OPEN_EXPORT_PKG
+QCUSTNVM,      quec_exec_qcustnvm_cmd,         0
#endif
#endif


//************************ bt/ble at cmd ************************/
+EBTADDR,       quec_exec_ebtaddr_cmd,          0
+QBTLEIBEACFG,  quec_exec_bt_ibeacon_cfg_cmd,   0
+QBTNAME,       quec_exec_bt_ble_name_cmd,      0
#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT
#ifdef CONFIG_QUEC_PROJECT_FEATURE_BT_AT
+QBTPWR,        quec_exec_bt_poweronoff_cmd,    0
+QBTLEADDR,     quec_exec_bt_leaddr_cmd,        0
+QBTLERANADDR,  quec_exec_bt_lerandomaddr_cmd,  0
+QBTGATADV,     quec_exec_bt_adv_param_cmd,     0
+QBTADVDATA,    quec_exec_bt_adv_data_cmd,      0
+QBTADVRSPDATA, quec_exec_bt_adv_rspdata_cmd,   0
+QBTADV,        quec_exec_bt_adv_cmd,           0
+QBTGATSNOD,    quec_exec_bt_sendnod_cmd,       0
+QBTGATSIND,    quec_exec_bt_sendind_cmd,       0
+QBTLESTATE,    quec_exec_bt_gatt_state_cmd,    0
+QBTLESEND,     quec_exec_bt_gatt_send_cmd,     0
+QBTLEGSND,     quec_exec_bt_gatt_channel_cmd,  0
+QBTLERCVM,     quec_exec_bt_gatt_recv_mode_cmd,  0
+QBTLEREAD,     quec_exec_bt_gatt_read_cmd,     0


+QBTSCANPARA,   quec_exec_bt_scan_param_cmd,    0
+QBTGATSCAN,    quec_exec_bt_scan_cmd,          0
+QBTGATCONN,    quec_exec_bt_gatt_connect_cmd,  0
+QBTGATDISCONN, quec_exec_bt_gatt_disconnect_cmd,  0
+QBTGATSERV,    quec_exec_bt_gatt_service_cmd,  0
+QBTGATINC,     quec_exec_bt_gatt_includes_cmd, 0
+QBTGATCHAR,    quec_exec_bt_gatt_chara_cmd,    0
+QBTGATDESC,    quec_exec_bt_gatt_desc_cmd,     0
+QBTWRCHAR,     quec_exec_bt_gatt_wrchar_cmd,   0
+QBTWRCHARNORSP,    quec_exec_bt_gatt_wrchar_norsp_cmd,   0
+QBTRDCHARHAND,     quec_exec_bt_gatt_rdchar_handle_cmd,  0
+QBTRDCHARUUID,     quec_exec_bt_gatt_rdchar_uuid_cmd,  0
+QBTGATWRDESC,  quec_exec_bt_gatt_wrdesc_cmd,   0
+QBTGATRDDESC,  quec_exec_bt_gatt_rddesc_cmd,   0

+QBTGATSS,      quec_exec_bt_gatt_add_service_cmd,   0
+QBTGATSC,      quec_exec_bt_gatt_add_chara_cmd,   0
+QBTGATSCV,     quec_exec_bt_gatt_add_charaval_cmd,   0
+QBTGATSCD,     quec_exec_bt_gatt_add_charades_cmd,   0
+QBTGATSSC,     quec_exec_bt_gatt_add_service_complete_cmd,   0
+QBTGATCONNP,   quec_exec_bt_gatt_update_conn_param_cmd,   0
+QBTLEADDWHL,   quec_exec_bt_add_whitelist_cmd,   0
+QBTLEDELWHL,   quec_exec_bt_del_whitelist_cmd,   0
+QBTLEWHLINFO,  quec_exec_bt_get_whitelist_cmd,   0
+QBTLEEXMTU,    quec_exec_bt_exchange_mtu_cmd,   0
+QBTLEIBEA,     quec_exec_bt_ibeacon_cmd,       0

+QBTSCANMODE,   quec_exec_bt_scanmode_cmd,      0
+QBTINQUERY,    quec_exec_bt_inquery_cmd,       0

+QBTHFPCONN,    quec_exec_hfp_connect_cmd,      0
+QBTHFPDISCONN, quec_exec_hfp_disconnect_cmd,   0
+QBTHFPVOL,     quec_exec_hfp_set_vol_cmd,      0
+QBTHFPCALL,    quec_exec_hfp_call_cmd,         0
+QBTHFPDIAL,    quec_exec_hfp_dial_cmd,         0
+QBTHFPVOLR,    quec_exec_hfp_voice_rec_cmd,    0
+QBTHFPTHRC,    quec_exec_hfp_three_way_cmd,    0

+QBTAVRCPDISCONN,   quec_exec_avrcp_disconnect_cmd,     0
+QBTAVRCPVOL,       quec_exec_avrcp_vol_cmd,            0
+QBTAVRCPCTRL,      quec_exec_avrcp_ctrl_cmd,         0
+QBTAVRCPSTATE,     quec_exec_avrcp_get_state_cmd,      0

#endif /*CONFIG_QUEC_PROJECT_FEATURE_BT_AT*/
#endif

//************************ wifi at cmd ************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_WIFISCAN_AT
+QWIFISCAN,     quec_exec_wifiscan_cmd,          0
+QWIFISCANEX,	quec_exec_async_wifiscan_cmd,	 0
#endif

//************************* GNSS at cmd *************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS
#ifdef CONFIG_QUEC_PROJECT_FEATURE_GNSS_AT
+QGPSCFG, 	    at_exec_qgpscfg_cmd,		    0
+QGPSDEL,       at_exec_qgpsdel_cmd,			0
+QGPS,	        at_exec_qgps_cmd,		0
+QGPSEND, 		at_exec_qgpsend_cmd,		0
+QGPSLOC, 		at_exec_qgpsloc_cmd,    		0
+QGPSGNMEA,		at_exec_qgpsgnmea_cmd,      0
+QAGPS,         at_exec_qagps_cmd,      0
+QGPSINFO,      at_exec_qgpsinfo_cmd,      0
//+QGPSXTRA,      at_exec_qgpsxtra_cmd,	    0
//+QGPSXTRATIME,  at_exec_qgpsxtratime_cmd,	    0
//+QGPSXTRADATA,  at_exec_qgpsxtradata_cmd, 		0
+QGPSCMD,	    at_exec_qgpscmd_cmd,		0
+QAGPSCFG,      at_exec_qagpscfg_cmd,            0
#endif /*CONFIG_QUEC_PROJECT_FEATURE_GNSS_AT*/
#endif


#endif /* QUEC_ATCMD_DEF_H */

