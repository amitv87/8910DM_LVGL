/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_DEBUGUART_H_
#define _BOOT_DEBUGUART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize debuguart
 */
void bootDebuguartInit(void);

/**
 * @brief write to debuguart
 *
 * It is non-blocking, it will not wait fifo space available.
 *
 * @param data  data to be written
 * @param size  size of the buffer for write
 * @return      actual written size
 */
int bootDebuguartWrite(const void *data, size_t size);

/**
 * @brief write and wait to debuguart
 *
 * It is blocking, it will wait fifo space available.
 *
 * @param data  data to be written
 * @param size  size of the buffer for write
 */
void bootDebuguartWriteAll(const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif
