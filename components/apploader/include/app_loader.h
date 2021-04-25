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

#ifndef _APP_LOADER_H_
#define _APP_LOADER_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief application image entry function type
 *
 * When calling the entry function of application image, typically
 * \p param will be NULL, except there are known convention of the
 * parameter.
 *
 * When the return value of 0 means success, other values mean error.
 * The error code convention depends on application.
 *
 * Typically, in the entry function, application shall initialize the
 * runtime, and create threads for application. It is not expected
 * that the entry function itself will take long time.
 *
 * For any application, the entry function is necessary.
 */
typedef int (*appImageEnter_t)(void *param);

/**
 * \brief application exit function type
 *
 * For application, the exit function is optional, and it can be NULL.
 */
typedef void (*appImageExit_t)(void);

/**
 * \brief application get parameter
 *
 * It is a general purpose API to get application parameters. The
 * parameter ID and value type are defined by application.
 *
 * For application, the get parameter function is optional, and it can
 * be NULL. Caller should always check whether \p header->get_param
 * is not NULL before calling it.
 *
 * It can be used to access application variable and functions. For
 * example,
 *
 * \code{.cpp}
 * if (header->get_param != NULL) {
 *     // get application API address
 *     function_t api = NULL;
 *     if (header->get_param(API_ID, &api) && api != NULL)
 *         api();
 * }
 * \endcode
 *
 * \param id        parameter id
 * \param value     output parameter value
 * \return
 *      - true on success
 *      - false on failed
 */
typedef bool (*appImageGetParam_t)(unsigned id, void *value);

/**
 * \brief application set parameter
 *
 * It is a general purpose API to set application parameters. The
 * parameter ID, parameter value are defined by application.
 *
 * For application, the set parameter function is optional, and it can
 * be NULL. Caller should always check whether \p header->set_param
 * is not NULL before calling it.
 *
 * It can be used to trigger application action, send events to
 * application, and etc.
 *
 * \code{.cpp}
 * if (header->set_param != NULL) {
 *     // trigger an application action
 *     int value = 1;
 *     header->set_param(ACTION_ID, &value);
 * }
 * \endcode
 *
 * \param id        parameter id
 * \param value     parameter value
 * \return
 *      - true on success
 *      - false on failed
 */
typedef bool (*appImageSetParam_t)(unsigned id, const void *value);

/**
 * \brief data structure for application image load
 */
typedef struct
{
    appImageEnter_t enter;        ///< entry function
    appImageExit_t exit;          ///< exit function
    appImageGetParam_t get_param; ///< get parameter function
    appImageSetParam_t set_param; ///< set parameter function
} appImageHandler_t;

/**
 * \brief global application image handler in flash
 *
 * It is defined only when \p CONFIG_APPIMG_LOAD_FLASH is defined
 */
extern appImageHandler_t gAppImgFlash;

/**
 * \brief global application image handler in file
 *
 * It is defined only when \p CONFIG_APPIMG_LOAD_FILE is defined
 */
extern appImageHandler_t gAppImgFile;

/**
 * \brief load application from memory
 *
 * \p address can be FLASH or RAM, though typically it shoule be FLASH.
 *
 * When the application is loaded successfully, \p handler->enter can
 * be called to run the application.
 *
 * \param address   application image address
 * \param handler   application image handler to be loaded
 * \return
 *      - true on success
 *      - false on fail, such as invalid parameters, or invalid image
 */
bool appImageFromMem(const void *address, appImageHandler_t *handler);

/**
 * \brief load application from file
 *
 * When the application is loaded successfully, \p handler->enter can
 * be called to run the application.
 *
 * \param fname     application image file name
 * \param handler   application image handler to be loaded
 * \return
 *      - true on success
 *      - false on fail, such as invalid parameters, or invalid image
 */
bool appImageFromFile(const char *fname, appImageHandler_t *handler);

OSI_EXTERN_C_END
#endif
