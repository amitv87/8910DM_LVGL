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

#ifndef _DRV_SERIAL_IMP_H_
#define _DRV_SERIAL_IMP_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/queue.h>
#include "osi_fifo.h"
#include "drv_serial.h"

struct drv_serial_impl;
typedef struct drv_serial_impl drvSerialImpl_t;

/**
 * @brief type of serial operations
 */
typedef struct
{
    bool (*ready)(drvSerialImpl_t *p);                               ///< the port ready or not
    int (*open)(drvSerialImpl_t *p);                                 ///< open a port
    void (*close)(drvSerialImpl_t *p);                               ///< close a port
    int (*write)(drvSerialImpl_t *p, const void *data, size_t size); ///< write data to port
    int (*read)(drvSerialImpl_t *p, void *buf, size_t size);         ///< read data from port
    int (*read_avail)(drvSerialImpl_t *p);                           ///< read avail size
    int (*write_avail)(drvSerialImpl_t *p);                          ///< write avail size
    bool (*wait_write_avail)(drvSerialImpl_t *p, unsigned timeout);  ///< wait write space available
    bool (*wait_write_finish)(drvSerialImpl_t *p, unsigned timeout); ///< wait read bytes available
    bool (*wait_read_avail)(drvSerialImpl_t *p, unsigned timeout);   ///< wait read bytes available
    bool (*write_direct)(drvSerialImpl_t *p, const void *data, size_t size,
                         size_t to_ms, uint32_t flags); ///< direct write
} drvSerialOps_t;

typedef struct
{
    uint32_t name;
    drvSerialCfg_t cfg;
} drvSerialInfo_t;

/**
 * @brief type indicate a serial
 */
struct drv_serial_impl
{
    drvSerialOps_t ops;          ///< serial port operations
    const drvSerialInfo_t *info; ///< serial port informations
};

/**
 * @brief register a serial implement
 * @note: this api is for serial instance, not for user
 *
 * @param name      the serial name
 * @param serial    the serial implement
 * @return
 *      - false     fail
 *      - true      success
 */
bool drvSerialRegisterImpl(uint32_t name, drvSerialImpl_t *serial);

/**
 * @brief unregister the serial implement
 * @note: this api for serial instance, not for user
 *
 * @param serial    the serial implement
 */
void drvSerialUnregisterImpl(drvSerialImpl_t *serial);

OSI_EXTERN_C_END

#endif // _DRV_SERIAL_IMP_H_
