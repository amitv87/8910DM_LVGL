/**  @file
  quec_log.h

  @brief
  This file is used to define version information for different Quectel Project.

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

	
#ifndef QUEC_LOG_H
#define QUEC_LOG_H
	
#include "osi_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QUEC_LOG_LEVEL OSI_LOG_LEVEL_INFO
#define QUEC_LOG_TAG OSI_MAKE_LOG_TAG('Q', 'U', 'E', 'C')


extern bool ql_quec_trace_is_enabled(void);

#define QUEC_LOG_PRINTF(level, fmt, ...)                                        \
    do                                                                          \
    {                                                                           \
        if (QUEC_LOG_LEVEL >= level && ql_quec_trace_is_enabled()) 				\
            osiTracePrintf((level << 28) | (QUEC_LOG_TAG), fmt, ##__VA_ARGS__); \
    } while (0)
			
#define custom_log(user_str, msg, ...)				QUEC_LOG_PRINTF(QUEC_LOG_LEVEL, user_str":%s,%d "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define custom_log_level(level, user_str, msg, ...)	QUEC_LOG_PRINTF(level, user_str":%s,%d "msg"", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_LOG_H */

