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

#ifndef _UNITY_PORT_H_
#define _UNITY_PORT_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief initialize unity environment
 *
 * \return
 *      - true on success
 *      - false on fail, out of memory
 */
bool unityInit(void);

/**
 * \brief open uart for unity
 *
 * By default, unity in application will ouput through log. When it is
 * called, unity will open the specified uart, and output to uart.
 *
 * This should be called only once.
 *
 * \param name      unity uart name
 * \param baud      unity uart baud rate
 * \return
 *      - true on success
 *      - false on fail, unit tests are running or open uart failed
 */
bool unityOpenUart(uint32_t name, unsigned baud);

/**
 * \brief close uart for unity
 *
 * \return
 *      - true on success
 *      - false on fail, unit tests are running
 */
bool unityCloseUart(void);

/**
 * \brief start unity test cases
 *
 * This will run all unittests \p UnityRunAllTests in the caller thread.
 * When it is not desired, caller should manage unit test thread.
 *
 * There is only one unity environment. So, when there are unit tests
 * running, the following start will fail.
 *
 * \return
 *      - failed test case count
 *      - -1 when unity is not initialized or unit tests are running.
 */
int unityStart(void);

/**
 * \brief start unity test cases
 *
 * It is similar to \p unityStart, just the function for all test cases
 * are provided.
 *
 * \return
 *      - failed test case count
 *      - -1 on invalid parameter
 *      - -1 when unity is not initialized or unit tests are running.
 */
int unityStartCases(void (*run_cases)(void));

OSI_EXTERN_C_END
#endif
