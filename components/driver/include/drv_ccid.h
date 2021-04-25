/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _DRV_CCID_H_
#define _DRV_CCID_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "usb/ccid_message.h"
#include <stddef.h>
#include <stdint.h>

/**
 * \brief ccid driver
 */
typedef struct drv_ccid drvCCID_t;

/**
 * \brief function type callback on ccid device line state change
 */
typedef void (*ccidLineCallback_t)(void *priv, bool online);

/**
 * \brief open the ccid device
 *
 * \param name  the CCID device name
 * \return
 *      - NULL  fail
 *      - other point of the instance
 */
drvCCID_t *drvCCIDOpen(uint32_t name);

/**
 * \brief set CCID line change callback
 * \note the callback will be called in interrupt
 *
 * \param ccid  the CCID device
 * \param cb    callback on line state change
 * \param ctx   caller context
 */
void drvCCIDSetLineChangeCallback(drvCCID_t *ccid, ccidLineCallback_t cb, void *ctx);

/**
 * \brief get CCID device online or not (connect to a host and enable the config)
 *
 * \param ccid  the CCID device
 * \return
 *      - true if online else false
 */
bool drvCCIDOnline(drvCCID_t *ccid);

/**
 * \brief read CCID slot state, each bit indicated a slot
 *
 * \param ccid      the CCID device
 * \param slotmap   the slotmap must non-null
 * \return
 *      - 0                     succeed
 *      - (negative integer)    negative error number
 */
int drvCCIDSlotCheck(drvCCID_t *ccid, uint8_t *slotmap);

/**
 * \brief slot state change
 *
 * \param ccid      the ccid device
 * \param index     slot index
 * \param insert    insert or remove
 * \return
 *      - 0                     succeed
 *      - (negative integer)    negative error number
 */
int drvCCIDSlotChange(drvCCID_t *ccid, uint8_t index, bool insert);

/**
 * \brief read CCID device
 * \note this api is synchronous, if no data coming, the call will block
 *
 * \param ccid  the CCID device
 * \param buf   buf to store read data
 * \param size  buffer size, must greater than struct ccid_bulk_out_header
 * \return
 *      - (negative integer)    negative error number
 *      - other                 actual read size
 */
int drvCCIDRead(drvCCID_t *ccid, struct ccid_bulk_out_header *buf, unsigned size);

/**
 * \brief write CCID device
 *
 * \param ccid  the CCID device
 * \param data  data buffer
 * \param size  data size
 * \return
 *      - (negative integer)    negative error number
 *      - other                 actual write size
 */
int drvCCIDWrite(drvCCID_t *ccid, const struct ccid_bulk_in_header *data, unsigned size);

/**
 * \brief close CCID device
 *
 * \param ccid  the CCID device
 */
void drvCCIDClose(drvCCID_t *ccid);

OSI_EXTERN_C_END

#endif
