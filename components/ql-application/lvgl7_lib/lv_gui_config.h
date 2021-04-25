/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _LV_GUI_CONFIG_H_
#define _LV_GUI_CONFIG_H_

// @AUTO_GENERATION_NOTICE@

#define LOG_TAG_LVGL OSI_MAKE_LOG_TAG('L', 'V', 'G', 'L')

/**
 * LittlevGL GUI stack size
 */
#define CONFIG_LV_GUI_THREAD_STACK_SIZE 4096
/**
 * LittlevGL GUI event count
 */
#define CONFIG_LV_GUI_THREAD_EVENT_COUNT 64

/**
 * LittlevGL GUI screen width
 */
#define CONFIG_LV_GUI_HOR_RES 128
/**
 * LittlevGL GUI screen height
 */
#define CONFIG_LV_GUI_VER_RES 128

/**
 * Screen off timeout
 */
#define CONFIG_LV_GUI_SCREEN_OFF_TIMEOUT 30*60*1000

#endif
