/**  @file
  decoder_demo.c

  @brief
  This file is decoder demo .

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

/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decoder_demo.h"
#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_camera.h"
#include "ql_api_decoder.h"
#include "ql_lcd.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_DECODER_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_DECODER_LOG(msg, ...)         QL_LOG(QL_DECODER_LOG_LEVEL, "ql_DECODERDEMO", msg, ##__VA_ARGS__)
#define QL_DECODER_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_DECODERDEMO", msg, ##__VA_ARGS__)

#define QL_DECODER_TASK_STACK_SIZE     		(10*1024)
#define QL_DECODER_TASK_PRIO          	 	APP_PRIORITY_NORMAL
#define QL_DECODER_TASK_EVENT_CNT      		5


/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Enum
 ===========================================================================*/

/*===========================================================================
 * Variate
 ===========================================================================*/

/*===========================================================================
 * Functions
 ===========================================================================*/
void ql_decoder_demo_thread(void *param)
{
    uint16_t *pcamData =NULL;
    int iret = 0;
    unsigned char result[128] = {0};
    ql_cam_drv_s cam;
    ql_CamInit(320, 240);
    ql_CamGetSensorInfo(&cam);
    ql_CamPowerOn();
    ql_qr_decoder_init();
    if(ql_lcd_init() == QL_LCD_SUCCESS)
        ql_lcd_display_on();
    ql_CamPreview();
    while(1)
    {
        //ql_CamCaptureImage(&pcamData);
        ql_camPrint(pcamData);
        iret = ql_qr_image_decoder(pcamData, cam.img_width, cam.img_height);

        if(iret == QL_DECODER_SUCCESS)
		{
			iret = ql_qr_get_decoder_result(result);
			if(iret == QL_DECODER_SUCCESS)
			{   
				QL_DECODER_LOG("Get QR code decode result %s", result);
			}
			else		
			{
				QL_DECODER_LOG("QR code decode get result failed");
			}
		}
		else
			QL_DECODER_LOG("QR code decode failed");
        ql_rtos_task_sleep_ms(200);
    }
}

void ql_decoder_app_init(void)
{
    QlDECODERStatus err = QL_OSI_SUCCESS;
    ql_task_t decoder_task = NULL;
        
    err = ql_rtos_task_create(&decoder_task, QL_DECODER_TASK_STACK_SIZE, QL_DECODER_TASK_PRIO, "decoder DEMO", ql_decoder_demo_thread, NULL, QL_DECODER_TASK_EVENT_CNT);
    if (err != QL_OSI_SUCCESS)
    {
        QL_DECODER_LOG("DECODER demo task created failed");
    }
}




