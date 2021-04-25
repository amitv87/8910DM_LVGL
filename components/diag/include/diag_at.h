/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _DIAG_AT_H_
#define _DIAG_AT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct diag_at;

/**
 * @brief Diag AT
 */
typedef struct diag_at diagAt_t;

/**
 * @brief Function type to notify diag at receive any data
 */
typedef void (*diagAtNotify_t)(void *ctx);

/**
 * @brief Diag AT config
 */
typedef struct
{
    uint32_t rxfifo_size;  ///< rx fifo size
    diagAtNotify_t notify; ///< notifier for data coming
    void *context;         ///< caller context
} diagAtCfg_t;

/**
 * @brief Create diag at instance
 *
 * @param cfg   the config
 * @return
 *      - NULL  fail
 *      - other diag at instance
 */
diagAt_t *diagAtCreate(const diagAtCfg_t *cfg);

/**
 * @brief Destroy the diag at
 *
 * @param d     the diag at instance
 */
void diagAtDestroy(diagAt_t *d);

/**
 * @brief Open the diag at channel
 *
 * @param d     the diag at instance
 * @return
 *      - true  success
 *      - false fail
 */
bool diagAtOpen(diagAt_t *d);

/**
 * @brief Close the diag at channel
 *
 * @param d     the diag at instance
 */
void diagAtClose(diagAt_t *d);

/**
 * @brief receive diag at success, send OK
 *
 * @param d         the diag at
 */
void diagAtSendOk(diagAt_t *d);

/**
 * @brief Write data via diag at channel
 *
 * @param d         the diag at
 * @param data      data to write
 * @param length    data length
 * @return
 *      - (-1)              fail
 *      - positive number   actually written
 */
int diagAtWrite(diagAt_t *d, const void *data, size_t length);

/**
 * @brief Read data from diag at channel
 *
 * @param d     the diag at
 * @param buf   read buffer
 * @param size  buffer size
 * @return
 *      - (-1)              fail
 *      - positive number   actually read
 */
int diagAtRead(diagAt_t *d, void *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif