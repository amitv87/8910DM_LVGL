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

#ifndef _QL_FS_DEMO_H_
#define _QL_FS_DEMO_H_


#ifdef __cplusplus
extern "C" {
#endif



/*========================================================================
 *  function Definition
 *========================================================================*/
#define QL_FS_TASK_PRIO 			APP_PRIORITY_NORMAL
#define QL_FS_TASK_STACK_SIZE		2*1024
#define QL_FS_TASK_EVENT_CNT        1

#define TEST_STR     "0123456789ABCDEFG"
#define TEST_DISK    "UFS:"
#define TEST_DIR	 "test_dir"
#define FILE_NAME	 "test_file.txt"

#define DIR_PATH     ""TEST_DISK"/"TEST_DIR""
#define FILE_PATH	 ""DIR_PATH"/"FILE_NAME""

#define TEST_NVM     "my_test.config"

int ql_fs_demo_init(void);

#ifdef __cplusplus
}/*"C" */
#endif

#endif   