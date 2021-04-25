/**  @file
  ql_log.h

  @brief
  This file is used to define log interface for different Quectel Project.

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
	
#ifndef QL_LOG_H
#define QL_LOG_H
	
#include "osi_log.h"
#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Macro Definition
 *========================================================================*/
#define QL_LOG_ERRCODE_BASE (QL_COMPONENT_LOG<<16)

#define QL_LOG_LEVEL_NEVER		OSI_LOG_LEVEL_NEVER
#define QL_LOG_LEVEL_ERROR		OSI_LOG_LEVEL_ERROR
#define QL_LOG_LEVEL_WARN		OSI_LOG_LEVEL_WARN
#define QL_LOG_LEVEL_INFO		OSI_LOG_LEVEL_INFO
#define QL_LOG_LEVEL_DEBUG		OSI_LOG_LEVEL_DEBUG
#define QL_LOG_LEVEL_VERBOSE	OSI_LOG_LEVEL_VERBOSE

#define QL_LOG_OUTPUT_LEVEL		QL_LOG_LEVEL_INFO
#define QL_LOG_TAG				OSI_MAKE_LOG_TAG('Q', 'O', 'P', 'N')

#define QL_LOG_PRINTF(level, fmt, ...)                                          \
    do                                                                          \
    {                                                                           \
        if (QL_LOG_OUTPUT_LEVEL >= level)                                       \
            osiTracePrintf((level << 28) | (QL_LOG_TAG), fmt, ##__VA_ARGS__);   \
    } while (0)
			
#define QL_LOG_PUSH(module_str, msg, ...)			QL_LOG_PRINTF(QL_LOG_OUTPUT_LEVEL, "["module_str"][%s, %d] "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define QL_LOG(level, module_str, msg, ...)			QL_LOG_PRINTF(level, "["module_str"][%s, %d] "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#undef printf
#define printf(fmt, args...)	QL_LOG_PRINTF(QL_LOG_OUTPUT_LEVEL, fmt, ##args)


/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
//log component detail errcode, 4 bytes
typedef enum
{
    QL_LOG_SUCCESS = QL_SUCCESS,
    
    QL_LOG_EXECUTE_ERR                      = 1|QL_LOG_ERRCODE_BASE,
    QL_LOG_MEM_ADDR_NULL_ERR,
    QL_LOG_INVALID_PARAM_ERR,
    QL_LOG_BUSY_ERR,
    QL_LOG_SEMAPHORE_CREATE_ERR,
    QL_LOG_SEMAPHORE_TIMEOUT_ERR,
    
    QL_LOG_PORT_SET_ERR                    = 15|QL_LOG_ERRCODE_BASE,
} ql_errcode_log_e;

typedef enum
{
    QL_LOG_PORT_NONE = 0x0,
    QL_LOG_PORT_UART = 0x1,
    QL_LOG_PORT_USB  = 0x2,
    QL_LOG_PORT_SDCARD = 0x4,
}ql_log_port_e;


/*========================================================================
 *	function Definition
 *========================================================================*/
/*
* to config the log port for AP, it will take effect after reboot.
*/
ql_errcode_log_e ql_log_set_port(ql_log_port_e dst_port);

/*
* to enable or disable quectel log
*/
void ql_quec_trace_enable(bool allow_Put);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_LOG_H */

