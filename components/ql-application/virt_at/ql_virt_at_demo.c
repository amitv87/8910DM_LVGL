/**  
  @file
  ql_virt_at_demo.c

  @brief
  quectel virt at demo.

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
27/01/2021        Neo         Init version
=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_virt_at.h"
#include "ql_virt_at_demo.h"


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_VIRTAT_DEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_VIRTAT_DEMO_LOG(msg, ...)         QL_LOG(QL_VIRTAT_DEMO_LOG_LEVEL, "ql_VIRTATDEMO", msg, ##__VA_ARGS__)
#define QL_VIRTAT_DEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_VIRTATDEMO", msg, ##__VA_ARGS__)

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define QL_VIRT_AT_RX_BUFF_SIZE                1024


/*========================================================================
 *  function Definition
 *========================================================================*/
void ql_virt_at0_notify_cb(unsigned int ind_type, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_VIRT_AT_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    
    if(QUEC_VIRT_AT_RX_RECV_DATA_IND == ind_type)
    {
        memset(recv_buff, 0, QL_VIRT_AT_RX_BUFF_SIZE+1);
        real_size= MIN(size, QL_VIRT_AT_RX_BUFF_SIZE);
        
        ql_virt_at_read(QL_VIRT_AT_PORT_0, recv_buff, real_size);
        
        QL_VIRTAT_DEMO_LOG("VAT0 <--: %s", recv_buff);
    }
    free(recv_buff);
    recv_buff = NULL;
}
 
void ql_virt_at1_notify_cb(unsigned int ind_type, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_VIRT_AT_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    
    if(QUEC_VIRT_AT_RX_RECV_DATA_IND == ind_type)
    {
        memset(recv_buff, 0, QL_VIRT_AT_RX_BUFF_SIZE+1);
        real_size= MIN(size, QL_VIRT_AT_RX_BUFF_SIZE);
        
        ql_virt_at_read(QL_VIRT_AT_PORT_1, recv_buff, real_size);
        
        QL_VIRTAT_DEMO_LOG("VAT1 <--: %s", recv_buff);
    }
    free(recv_buff);
    recv_buff = NULL;
}

void ql_virt_at2_notify_cb(unsigned int ind_type, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_VIRT_AT_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    
    if(QUEC_VIRT_AT_RX_RECV_DATA_IND == ind_type)
    {
        memset(recv_buff, 0, QL_VIRT_AT_RX_BUFF_SIZE+1);
        real_size= MIN(size, QL_VIRT_AT_RX_BUFF_SIZE);
        
        ql_virt_at_read(QL_VIRT_AT_PORT_2, recv_buff, real_size);
        
        QL_VIRTAT_DEMO_LOG("VAT2 <--: %s", recv_buff);
    }
    free(recv_buff);
    recv_buff = NULL;
}

static void ql_virt_at_demo_thread(void *param)
{
    int ret = 0;
    QlOSStatus err = 0; 
    
    char *cmd0 = "ATI\r\n";
    char *cmd1 = "AT^HEAPINFO\r\n";
    char *cmd2 = "at+qdbgcfg=\"oem\"\r\n";

    ret = ql_virt_at_open(QL_VIRT_AT_PORT_0,ql_virt_at0_notify_cb);
    if(QL_VIRT_AT_SUCCESS != ret)
    {
        QL_VIRTAT_DEMO_LOG("virt at0 open error,ret: 0x%x", ret);
        goto exit;
    }
    QL_VIRTAT_DEMO_LOG("virt at0 open success");  

    ret = ql_virt_at_open(QL_VIRT_AT_PORT_1,ql_virt_at1_notify_cb);
    if(QL_VIRT_AT_SUCCESS != ret)
    {
        QL_VIRTAT_DEMO_LOG("virt at1 open error,ret: 0x%x", ret);
        goto exit;
    }
    QL_VIRTAT_DEMO_LOG("virt at1 open success");
    
    ret = ql_virt_at_open(QL_VIRT_AT_PORT_2,ql_virt_at2_notify_cb);

    if(QL_VIRT_AT_SUCCESS != ret)
    {
        QL_VIRTAT_DEMO_LOG("virt at2 open error,ret: 0x%x", ret);
        goto exit;
    }
    QL_VIRTAT_DEMO_LOG("virt at2 open success");
                
    while(1)
    {     
        QL_VIRTAT_DEMO_LOG("VAT0 -->: %s", cmd0);
        ql_virt_at_write(QL_VIRT_AT_PORT_0, (unsigned char*)cmd0, strlen((char *)cmd0));       
        ql_rtos_task_sleep_ms(5000);
        
        QL_VIRTAT_DEMO_LOG("VAT1 -->: %s", cmd1);
        ql_virt_at_write(QL_VIRT_AT_PORT_1, (unsigned char*)cmd1, strlen((char *)cmd1));        
        ql_rtos_task_sleep_ms(5000);
        
        QL_VIRTAT_DEMO_LOG("VAT2 -->: %s", cmd2);
        ql_virt_at_write(QL_VIRT_AT_PORT_2, (unsigned char*)cmd2, strlen((char *)cmd2));
        ql_rtos_task_sleep_ms(5000);
    }
        
exit:    
    err = ql_rtos_task_delete(NULL);
    if(err != QL_OSI_SUCCESS)
    {
        QL_VIRTAT_DEMO_LOG("task deleted failed");
    } 
}

void ql_virt_at_app_init()
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_task_t virt_at_task = NULL;

    err = ql_rtos_task_create(&virt_at_task, 1024, APP_PRIORITY_NORMAL, "ql_virtatdemo", ql_virt_at_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_VIRTAT_DEMO_LOG("virt at demo task created failed");
        return;
    }
}



