#ifndef __IC_WIDGETS_INC_H__
#define __IC_WIDGETS_INC_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#if defined(PLATFORM_EC600)
#include "ql_log.h"
#include "lvgl.h"
#define LOGE(msg, ...)   QL_LOG(QL_LOG_LEVEL_INFO, "ql_LVGLDEMO", msg, ##__VA_ARGS__)
#define LOGW(msg, ...)   QL_LOG(QL_LOG_LEVEL_INFO, "ql_LVGLDEMO", msg, ##__VA_ARGS__)
#define LOGI(msg, ...)   QL_LOG(QL_LOG_LEVEL_INFO, "ql_LVGLDEMO", msg, ##__VA_ARGS__)
#elif defined(PLATFORM_MT25)
#include "hal.h"
#include "hal_g2d.h"
#include "hal_log.h"

#include "bsp_gpio_ept_config.h"
#include "bsp_lcd.h"
#include "mt25x3_hdk_lcd.h"
#include "mt25x3_hdk_backlight.h"
#include "memory_attribute.h"

#include "ic_lvgl_task_msg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "lvgl.h"

#define LOGE(fmt,arg...)   LOG_E(UI, "UI:"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(UI, "UI:"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(UI, "UI:"fmt,##arg)
#else
#include "lvgl/lvgl.h"


//log_create_module(UI, PRINT_LEVEL_INFO);
#define LOGE   printf
#define LOGW   printf
#define LOGI   printf
#endif

//#include "lv_png.h"

#include "image_id_list.h"
#include "image_resource.h"

#include "ic_hal.h"
#include "ic_obj_message.h"
#include "title_bar.h"
#include "animation.h"
#include "language_config.h"
#include "lv_i18n.h"
#include "iclv_font.h"
#include "screen_manager.h"

#define IC_CANVAS_WIDTH LV_HOR_RES_MAX
#define IC_CANVAS_HEIGHT LV_VER_RES_MAX
extern lv_color_t  current_screen_map[IC_CANVAS_WIDTH * IC_CANVAS_HEIGHT];
#endif
