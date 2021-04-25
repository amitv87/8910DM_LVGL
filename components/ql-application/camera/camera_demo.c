/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_camera.h"
#include "camera_demo.h"
#include "ql_i2c.h"
#include "ql_lcd.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_CAMERADEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_CAMERADEMO_LOG(msg, ...)         QL_LOG(QL_CAMERADEMO_LOG_LEVEL, "ql_CAMERADEMO", msg, ##__VA_ARGS__)
#define QL_CAMERADEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_CAMERADEMO", msg, ##__VA_ARGS__)
    
#define QL_CAMERA_TASK_STACK_SIZE     		10240
#define QL_CAMERA_TASK_PRIO          	 	APP_PRIORITY_NORMAL
#define QL_CAMERA_TASK_EVENT_CNT      		5

#define SalveAddr_w (0x42 >> 1)
#define SalveAddr_r (0x43 >> 1)

#define TEST_CAPTURE_COUNTS (10)
#define TEST_PREVIEW_COUNTS (500)

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef enum
{
    preview_TEST = 0,      //test preview
    capture_TEST = 1,      //test capture
}QL_CAM_TEST_E;

/*===========================================================================
 * Variate
 ===========================================================================*/

/*===========================================================================
 * Functions
 ===========================================================================*/

void ql_CamTest(QL_CAM_TEST_E TestMode)
{
    uint16_t *pCamDataBuffer = NULL;
    uint16_t i;
    if(TestMode == preview_TEST)
    {   
        for(i=0;i<TEST_PREVIEW_COUNTS;i++)
        {
            ql_camPrint(pCamDataBuffer);
            //下面延时设置请根据cam的接口(spi\mipi)以及buf配置适当调节
            ql_rtos_task_sleep_ms(60);      
        }
    }
    else
    {
        for(i=0;i<TEST_CAPTURE_COUNTS;i++)
        {
            ql_CamCaptureImage(&pCamDataBuffer);
            ql_camPrint(pCamDataBuffer);
            ql_rtos_task_sleep_ms(2000);
        }
    }
}

void ql_camera_demo_thread(void *param)
{
    uint8_t testID[2] = {0, 0};
    uint8_t data = 0x1c;

    QL_CAMERADEMO_LOG("Camera demo entry");
    int lcdstatus;
    
    ql_CamInit(320, 240);
    //ql_CamInit(640, 480);

    ql_CamPowerOn();     
    
    lcdstatus = ql_lcd_init();
    if(lcdstatus == QL_SUCCESS)
        ql_lcd_display_on();

    /*I2C test*/
    ql_I2cInit(i2c_1, STANDARD_MODE);
    QL_CAMERADEMO_LOG("testID[0] = 0x%x, testID[1] = 0x%x", testID[0], testID[1]);
    ql_I2cWrite(i2c_1, SalveAddr_w, 0xf4, &data, 1);
    ql_rtos_task_sleep_ms(1);
    ql_I2cRead(i2c_1, SalveAddr_r, 0xf0, testID, 2);
    QL_CAMERADEMO_LOG("testID[0] = 0x%x, testID[1] = 0x%x", testID[0], testID[1]);

    /*Camera test*/
    ql_CamPreview();
    ql_CamTest(preview_TEST);
    ql_CamTest(capture_TEST);
    ql_CamStopPreview();  
    
    ql_CamClose();
    
    if(lcdstatus == QL_SUCCESS)
        ql_lcd_display_off();
    ql_rtos_task_delete(NULL);
}

void ql_camera_app_init(void)
{
    QlCAMERAStatus err = QL_OSI_SUCCESS;
    ql_task_t camera_task = NULL;
        
    err = ql_rtos_task_create(&camera_task, QL_CAMERA_TASK_STACK_SIZE, QL_CAMERA_TASK_PRIO, "camera DEMO", ql_camera_demo_thread, NULL, QL_CAMERA_TASK_EVENT_CNT);
    if (err != QL_OSI_SUCCESS)
    {
        QL_CAMERADEMO_LOG("CAMERA demo task created failed");
    }
}


