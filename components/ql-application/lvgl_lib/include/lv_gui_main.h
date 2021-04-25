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

#ifndef _LV_GUI_MAIN_H_
#define _LV_GUI_MAIN_H_

#include "osi_api.h"
#include "lv_gui_config.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief forward declaration
 */
struct _lv_indev_t;

/**
 * \brief function prototype for gui creation
 */
typedef void (*lvGuiCreate_t)(void);

/**
 * \brief start gui based on littlevgl
 *
 * It will create gui thread, initialize littlevgl, and optional call gui
 * creation function \p create.
 *
 * LCD should be initialized before this, back light should be enabled. And
 * usually quick logo has been shown. Gui thread will blit to LCD directly.
 *
 * \param create        gui creation function
 */
void lvGuiInit(lvGuiCreate_t create);

/**
 * \brief terminate littlevgl gui
 *
 * Most likely, it won't be called.
 */
void lvGuiDeinit(void);

/**
 * \brief get littlevgl gui thread
 *
 * \return
 *      - littlevgl gui thread
 */
osiThread_t *lvGuiGetThread(void);

/**
 * \brief get keypad input device
 * \return
 *      - keypad device
 */
struct _lv_indev_t *lvGuiGetKeyPad(void);

/**
 * \brief schedule a callback to be executed in gui thread
 *
 * \param cb            callback function
 * \param param         callback parameter
 */
void lvGuiThreadCallback(osiCallback_t cb, void *param);

/**
 * \brief send event to gui thread
 */
void lvGuiSendEvent(const osiEvent_t *evt);

/**
 * \brief request screen on
 *
 * It is for an application to request the screen on even there are no
 * user inputs.
 *
 * \p id is the identification of application. In the system, it should
 * be unique. Internally, 32bits bitmap is used to record this. So, it
 * should be inside [0,31].
 *
 * The request is not counting. That is, for a given application id,
 * event there are multiple calls, single call of \p lvGuiReleaseScreenOn
 * will clear the request.
 *
 * \param id        application id
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool lvGuiRequestSceenOn(uint8_t id);

/**
 * \brief release screen on request
 *
 * \param id        application id
 * \return
 *      - true on success
 *      - false on invalid parameter
 */
bool lvGuiReleaseScreenOn(uint8_t id);

/**
 * \brief turn on screen
 *
 * When screen is already turned on, it will do nothing.
 */
void lvGuiScreenOn(void);

/**
 * \brief turn off screen
 *
 * When screen is already turned off, it will do nothing.
 */
void lvGuiScreenOff(void);

/**
 * \brief set screen off timeout at inactive
 *
 * It can change the default value. When \p timeout is 0, screen won't be
 * turned off by inactive timeout.
 *
 * \param timeout       inactive screen off timeout, 0 for never timeout
 */
void lvGuiSetInactiveTimeout(unsigned timeout);

/**
 * \brief set whether animation is regarded as inactive
 *
 * When \p inactive is true, animation update won't be regarded as *active*.
 * The inactive screen off timer will still count down even there are
 * animations. Otherwise, inactive screen off timer will stop counting when
 * there are animation.
 *
 * \param inactive      animation will be regarded as inactive
 */
void lvGuiSetAnimationInactive(bool inactive);

OSI_EXTERN_C_END
#endif
