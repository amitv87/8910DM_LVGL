/**  
  @file
  osi_demo.h

  @brief
  This file provides the definitions for nw demo, and declares the 
  API functions.

*/
/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

#ifndef OSI_DEMO_H_
#define OSI_DEMO_H_


#ifdef __cplusplus
extern "C" {
#endif



/*========================================================================
 *  function Definition
 *========================================================================*/
#define DEMO_TASK_PRIO 				APP_PRIORITY_NORMAL
#define DEMO_TASK_STACK_SIZE		2*1024
#define DEMO_TASK_EVENT_CNT         2

QlOSStatus ql_osi_demo_init(void);

#ifdef __cplusplus
}/*"C" */
#endif

#endif   