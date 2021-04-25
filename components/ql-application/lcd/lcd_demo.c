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
#include "ql_gpio.h"

#include "lcd_demo.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LCDDEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_LCDDEMO_LOG(msg, ...)         QL_LOG(QL_LCDDEMO_LOG_LEVEL, "ql_LCDDEMO", msg, ##__VA_ARGS__)
#define QL_LCDDEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_LCDDEMO", msg, ##__VA_ARGS__)

/*===========================================================================
 * Variate
 ===========================================================================*/
uint16_t Test_image1[30*50] = {};
uint16_t Test_image2[50*100] = {};
uint16_t Test_image3[100*200] = {};

/*===========================================================================
 * Functions
 ===========================================================================*/
void image_test_set(void)
{
    uint16_t count = 0;
    uint16_t image1_len = sizeof(Test_image1)/sizeof(Test_image1[0]);
    uint16_t image2_len = sizeof(Test_image2)/sizeof(Test_image2[0]);
    uint16_t image3_len = sizeof(Test_image3)/sizeof(Test_image3[0]);

    for( count = 0; count < image1_len; count++ )
    {
        Test_image1[count] = LCD_BLACK;
    }
    for( count = 0; count < image2_len; count++ )
    {
        Test_image2[count] = LCD_YELLOW;
    }
    for( count = 0; count < image3_len; count++ )
    {
        Test_image3[count] = LCD_PURPLE;
    }
}

static void ql_lcd_demo_thread(void *param)
{
    QL_LCDDEMO_LOG("lcd demo thread enter, param 0x%x", param);

    ql_event_t event;

    image_test_set();

    if( ql_lcd_init() != QL_LCD_SUCCESS )
    {
        QL_LCDDEMO_LOG("LCD init failed");
        ql_rtos_task_delete(NULL);
    }

    while(1)
    {
        ql_event_wait(&event, 1);

        /* display on */
        ql_lcd_display_on();
        QL_LCDDEMO_LOG("LCD display on");
        ql_rtos_task_sleep_s(1);

        /* full screen RED */
        ql_lcd_clear_screen(LCD_RED);
        ql_rtos_task_sleep_s(5);
        /* block display BLACK */
        ql_lcd_write(Test_image1, 20, 20, 50-1, 70-1);
        ql_rtos_task_sleep_s(5);

        /* full screen GREEN */
        ql_lcd_clear_screen(LCD_GREEN);
        ql_rtos_task_sleep_s(5);
        /* block display YELLOW */
        ql_lcd_write(Test_image2, 50, 50, 100-1, 150-1);
        ql_rtos_task_sleep_s(5);

        /* full screen BLUE */
        ql_lcd_clear_screen(LCD_BLUE);
        ql_rtos_task_sleep_s(5);
        /* block display PURPLE */
        ql_lcd_write(Test_image3, 100, 100, 200-1, 300-1);
        ql_rtos_task_sleep_s(5);

        /* rotation 90 display */
        ql_spi_lcd_write_cmd(0x36);
        ql_spi_lcd_write_data(0x60);
        ql_lcd_write(Test_image1, 70, 100, 100-1, 150-1);
        ql_rtos_task_sleep_s(3);

        /* normal display */
        ql_spi_lcd_write_cmd(0x36);
        ql_spi_lcd_write_data(0x00);
        ql_lcd_write(Test_image2, 120, 150, 170-1, 250-1);
        ql_rtos_task_sleep_s(3);

        /* full screen WHITE */
        ql_lcd_clear_screen(LCD_WHITE);

        /* brightness adjust */
        ql_lcd_set_brightness(0);
        QL_LCDDEMO_LOG("LCD backlight 0");
        ql_rtos_task_sleep_s(3);

        ql_lcd_set_brightness(1);
        QL_LCDDEMO_LOG("LCD backlight 1");
        ql_rtos_task_sleep_s(3);

        ql_lcd_set_brightness(2);
        QL_LCDDEMO_LOG("LCD backlight 2");
        ql_rtos_task_sleep_s(3);

        ql_lcd_set_brightness(3);
        QL_LCDDEMO_LOG("LCD backlight 3");
        ql_rtos_task_sleep_s(3);

        ql_lcd_set_brightness(4);
        QL_LCDDEMO_LOG("LCD backlight 4");
        ql_rtos_task_sleep_s(3);

        ql_lcd_set_brightness(5);
        QL_LCDDEMO_LOG("LCD backlight 5");
        ql_rtos_task_sleep_s(3);

        /* display off */
        ql_lcd_display_off();
        QL_LCDDEMO_LOG("LCD display off");
        ql_rtos_task_sleep_s(1);
    }

    ql_rtos_task_delete(NULL);
}

void ql_lcd_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_task_t lcd_task = NULL;

    err = ql_rtos_task_create(&lcd_task, 1024, APP_PRIORITY_NORMAL, "ql_lcddemo", ql_lcd_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_LCDDEMO_LOG("lcd demo task created failed");
    }
}


