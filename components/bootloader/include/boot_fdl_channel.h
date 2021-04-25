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

#ifndef _BOOT_FDL_CHANNEL_H_
#define _BOOT_FDL_CHANNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Abstract FDL channel
 *
 * For concrete FDL channel, it must be put at the beginning for data
 * type casting.
 */
typedef struct fdlChannel
{
    /**
     * whether baud rate is supported
     *
     * @param ch        the FDL channel
     * @param baud      baud rate to be checked
     * @return
     *      - true on supported
     *      - false on unsupported
     */
    bool (*baud_supported)(struct fdlChannel *ch, uint32_t baud);

    /**
     * Set baud rate
     *
     * If the channel doesn't have baud rate, just return \a true.
     *
     * @param ch    the FDL channel
     * @param baud  baud rate
     * @return
     *      - true if the baud rate is changed
     *      - false if failed
     */
    bool (*set_baud)(struct fdlChannel *ch, uint32_t baud);

    /**
     * Get available size for read
     *
     * @param ch    the FDL channel
     * @return  available size for read
     */
    int (*avail)(struct fdlChannel *ch);

    /**
     * Read from the channel
     *
     * @param ch    the FDL channel
     * @param data  memory for read data
     * @param size  maximum read size
     * @return  actual read size
     */
    int (*read)(struct fdlChannel *ch, void *data, size_t size);

    /**
     * Write to the channel
     *
     * @param ch    the FDL channel
     * @param data  pointer of data to be written
     * @param size  maximum write size
     * @return  actual written size
     */
    int (*write)(struct fdlChannel *ch, const void *data, size_t size);

    /**
     * Flush and drop input
     *
     * @param ch    the FDL channel
     */
    void (*flush_input)(struct fdlChannel *ch);

    /**
     * Flush output
     *
     * When there are cache in serial, flush the cache.
     *
     * @param ch    the FDL channel
     */
    void (*flush)(struct fdlChannel *ch);

    /**
     * Destroy fdl channel
     *
     * @param ch    the FDL channel
     */
    void (*destroy)(struct fdlChannel *ch);

    /**
     * Check fdl channel connected or not
     *
     * @param ch    the FDL channel
     * @return ture on connected else false
     */
    bool (*connected)(struct fdlChannel *ch);
} fdlChannel_t;

/**
 * @brief open UART FDL channel
 *
 * @param input UART baudrate
 * The UART used, and UART baud can be config.
 *
 * @return  UART FDL channel.
 */
fdlChannel_t *fdlOpenUart(uint32_t name, uint32_t baud, bool reconfig);

/**
 * @breif open USB Serial FDL channel
 * @return  USB Serial FDL channel
 */
fdlChannel_t *fdlOpenUsbSerial(void);

#ifdef __cplusplus
}
#endif
#endif
