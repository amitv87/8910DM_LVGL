/**  @file
  quec_cust_feature.h

  @brief
  This file is used to define feature for different Quectel Project.

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


#ifndef QUEC_CUST_FEATURE_H
#define QUEC_CUST_FEATURE_H
#include "QuecPrjName.h"
#include "quec_cust_patch.h"
#include "quec_proj_config.h"
#include "quec_proj_config_at.h"
#ifdef __cplusplus
extern "C" {
#endif


#ifdef CONFIG_QUEC_PROJECT_FEATURE

/******************************** customer specific function*********************************/
//#define QUECTEL_CUSTOMIZATION_FOR_LIANDI

/******************************** platform/bsp/rtos function*********************************/
#if defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_LA) \
	|| defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_AB) || defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_LB)\
    || defined (CONFIG_QL_PROJECT_DEF_EC200U_EU_AB)


#elif defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_LA) \
	|| defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_AB) || defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_LB) \
	|| defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_MC) || defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_SC)

#elif defined (CONFIG_QL_PROJECT_DEF_EC600U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EC600U_CN_LA) \
	|| defined (CONFIG_QL_PROJECT_DEF_EC600U_CN_AB) || defined (CONFIG_QL_PROJECT_DEF_EC600U_CN_LB) \
    || defined (CONFIG_QL_PROJECT_DEF_EC600U_EU_AB)

#elif defined (CONFIG_QL_PROJECT_DEF_EG500U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EG500U_CN_LA) \
	|| defined (CONFIG_QL_PROJECT_DEF_EG500U_CN_AB) || defined (CONFIG_QL_PROJECT_DEF_EG500U_CN_LB)


#endif

#if defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EC200U_CN_AB) \
	|| defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_AA) || defined (CONFIG_QL_PROJECT_DEF_EG700U_CN_AB)\
	|| defined (CONFIG_QL_PROJECT_DEF_EG500U_CN_AB) || defined (CONFIG_QL_PROJECT_DEF_EC200U_EU_AB)\
	|| defined (CONFIG_QL_PROJECT_DEF_EC600U_EU_AB)
#define QUEC_GSM_SUPPORT
#endif


/***** for led test *****/
//#define QUEC_LED_TEST
//#define QUEC_VIRTUAL_PWM


#define QUEC_API_USE_EVENT_DRIVER  0


/******************************** AT COMMAND function*********************************/
#define QUEC_AT_CONFIG_ALL_CHANNEL   1

//#define QUEC_MYFUNC_SUPPORT 0



/******************************** UART function*********************************/
#ifndef CONFIG_QUEC_PROJECT_FEATURE_GNSS
//
#endif

#define QUEC_CMUX_ENABLE


/******************************** USB function*********************************/
//#define QUEC_USB_ENABLE



/******************************** PPP function*********************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
//
#endif


/***************************** POWER_MANAGER function******************************/
// CONFIG_QUEC_PROJECT_FEATURE_SLEEP to replace QUEC_POWER_MANAGER_ENABLE
//#define QUEC_QSCLKEX_ENABLE


/******************************** NETWORK function*********************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_NW
//
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_USBNET
//
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
//
#endif

/******************** SMS/PHB/CALL/STK/BIP/USSD/(U)SIM function********************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
//
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VOICE_CALL
//
#endif

//#define QUEC_SIM_ENBALE


/******************************** TCPIP function*********************************/
#ifdef CONFIG_QUEC_PROJECT_FEATURE_DNS
//
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_TCPIP
//#define QUEC_TCP_CLOSE_FEATURE
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_MQTT
//
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_HTTP
//
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_FTP
//
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_NTP
//
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_LWM2M
//
#endif


//#define QUEC_CERT_SUPPORT
//#ifdef QUEC_CERT_SUPPORT
//#define QUECTEL_IOT_RPT_SUPPORT
//#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_MQTT
//#define QUEC_QUECTHING_SUPPORT
//#define QUECTEL_MQTT_TRANSPARENT
//#define QUECT_ALIOT_FEATURE_SUPPORT
#endif

/******************************** Audio function*********************************/
//#define QUEC_AT_CMD_AUDIO_SUPPORT
//#define QUEC_AUDIO_SUPPORT
//#define QUECTEL_TTS_FEATURE


#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_CUST_FEATURE_H */
/*END OF FIEL*/
