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

#ifndef _OSI_SYSNV_H_
#define _OSI_SYSNV_H_

#include "kernel_config.h"
#include "osi_compiler.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "quec_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_KERNEL_SYSNV_ENABLED
#ifdef __SYSNV_DEFINE_VAR
#define OSI_SYSNV_VAR(decl, ...) OSI_DEF_GLOBAL_VAR(decl, ##__VA_ARGS__)
#else
#define OSI_SYSNV_VAR(decl, ...) OSI_DECL_GLOBAL_VAR(decl, ##__VA_ARGS__)
#endif
#else
#define OSI_SYSNV_VAR(decl, ...) OSI_DEF_CONST_VAR(decl, ##__VA_ARGS__)
#endif

#define OSI_SYSNV_OPT(decl, ...) OSI_DEF_CONST_VAR(decl, ##__VA_ARGS__)

OSI_SYSNV_VAR(bool gSysnvTraceEnabled, true);
OSI_SYSNV_VAR(bool gSysnvDeepSleepEnabled, true);
OSI_SYSNV_VAR(bool gSysnvPsmSleepEnabled, false);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
//jensen.fang 20200810: update the usb default mode as ECM+Serial
OSI_SYSNV_VAR(int gSysnvUsbWorkMode, 5);
OSI_SYSNV_VAR(int gSysnvTraceDevice, 2); // osi_trace.h
//jensen.fang 20200812: update the diag device for FT default as usb
OSI_SYSNV_VAR(int gSysnvDiagDevice, 2);  // diag.h
OSI_SYSNV_VAR(bool gSysnvSimHotPlug, false);//sim detect is disable as default
OSI_SYSNV_VAR(bool gSysnvSimVoltTrigMode, false);// low level voltage as default for sim detect
#else
OSI_SYSNV_VAR(int gSysnvUsbWorkMode, 2);
OSI_SYSNV_VAR(int gSysnvTraceDevice, 1); // osi_trace.h
OSI_SYSNV_VAR(int gSysnvDiagDevice, 1);  // diag.h
OSI_SYSNV_VAR(bool gSysnvSimHotPlug, true);
OSI_SYSNV_VAR(bool gSysnvSimVoltTrigMode, true);
#endif
OSI_SYSNV_VAR(int gSysnvUsbDetMode, 0); // drv_usb.h
OSI_SYSNV_VAR(int gSysnvNATCfg, 0);
OSI_SYSNV_VAR(unsigned gSysnvFstraceMask, 0);
OSI_SYSNV_VAR(bool gSysnvPoffChargeEnable, false);

/**
 * load system NV from persitent storage
 *
 * After load, the system NV in memory will be changed.
 *
 * \return
 *      - true on success
 *      - false on error
 */
bool osiSysnvLoad(void);

/**
 * save system NV to persitent storage
 *
 * \return
 *      - true on success
 *      - false on error
 */
bool osiSysnvSave(void);

/**
 * clear system NV from persitent storage
 */
void osiSysnvClear(void);

/**
 * get system NV encoded buffer size
 *
 * System NV encoded size may change when value changed.
 *
 * \return  encoded system NV buffer size
 */
int osiSysnvSize(void);

/**
 * load encoded system NV to buffer
 *
 * \p size must be larger than or equal to encoded system NV size, which
 * can be get by \p osiSysnvSize.
 *
 * \param buf   buffer for encoded system NV
 * \param size  buffer size
 * \return
 *      - encoded system NV size
 *      - -1 on error
 */
int osiSysnvLoadToMem(void *buf, size_t size);

/**
 * save buffer to system NV persistent storage
 *
 * After save, the system NV value in effect keep unchanged. It is
 * recommended to reboot system to make new value take effect.
 *
 * It is only used for PC tools. The content in persistent storage is encoded,
 * and the provided buffer must match the codec in platform.
 *
 * \param buf       buffer to be written
 * \param size      buffer size
 * \return
 *      - byte count written, should be the same as \p size
 *      - -1 if write failed
 */
int osiSysnvSaveFromMem(const void *buf, size_t size);

#ifdef __cplusplus
}
#endif
#endif
