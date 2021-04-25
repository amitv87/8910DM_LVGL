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

#ifndef _DRV_PS_PATH_H_
#define _DRV_PS_PATH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * opaque data structure for PS interface
 */
typedef struct drvPsIntf drvPsIntf_t;

/**
 * callback function of PS interface data arrival
 *
 * The callback is executed in PS path thread, or the thread calling
 * \p drvPsPathDataArrive. Inside the callback, it is permitted to call
 * \p drvPsIntfRead. However, it is not permitted to call
 * \p drvPsInterfaceOpen and \p drvPsIntfClose.
 */
typedef void (*drvPsIntfDataArriveCB_t)(void *ctx, drvPsIntf_t *p);

/**
 * @brief initialize PS path module
 */
void drvPsPathInit(void);

/**
 * @brief PS data arrive callback in external source
 *
 * For external source, this API is implemented inside this module and called
 * by external source.
 *
 * For non-external source, this API may be unimplemented.
 *
 * @param sim       SIM id
 * @param cid       CID
 * @param data      PS data
 * @param size      PS data size
 */
void drvPsPathDataArrive(uint8_t sim, uint8_t cid, const void *data, uint16_t size);

/**
 * @brief PS data send function
 *
 * For extenal source, this API will be called in this module, and sould be
 * implemented in other modules.
 *
 * For non-external source, this API won't be called.
 *
 * @param sim       SIM id
 * @param cid       CID
 * @param data      PS data
 * @param size      PS data size
 * @return
 *      - sent size, it should be equal to \p size on success
 *      - -1 on failed
 */
int drvPsPathDataSend(uint8_t sim, uint8_t cid, const void *data, uint16_t size);

/**
 * @brief open a PS interface
 *
 * Each sim and cid combination is an interface. For each interface,
 * it can be opened only once.
 *
 * @param sim   SIM number
 * @param cid   CID
 * @param cb    callback of data arrival
 * @param cb_ctx    callback contex pointer
 * @return
 *      - PS interface pointer
 *      - NULL on error
 *          - the SIM/CID is already opened
 *          - invalid sim cid combination
 */
drvPsIntf_t *drvPsIntfOpen(uint8_t sim, uint8_t cid, drvPsIntfDataArriveCB_t cb, void *cb_ctx);

/**
 * @brief close a PS interface
 *
 * When \p p is NULL or the interface is already opened, there are no
 * operations.
 *
 * @param p     PS interface pointer
 */
void drvPsIntfClose(drvPsIntf_t *p);

/**
 * @brief change PS interface data arrive callback
 *
 * When \p cb is NULL, the registered callback won't be changed. And the
 * original callback will be returned.
 *
 * @param p     PS interface pointer, can't be NULL
 * @param cb    data arrive callback
 * @return      the original data arrive callback
 */
drvPsIntfDataArriveCB_t drvPsIntfSetDataArriveCB(drvPsIntf_t *p, drvPsIntfDataArriveCB_t cb);

/**
 * @brief read from PS interface
 *
 * When there are no data in PS interface, return 0.
 *
 * When the input \p size is too small for one packet, return -1.
 * The maximum packet size is \p PS_BUF_DL_LEN_B.
 *
 * @param p     PS interface pointer, can't be NULL
 * @param data  memory for read, can't be NULL
 * @param size  memory size
 * @return
 *      - read size
 *      - 0 if no data
 *      - -1 if input \p size is too small for one packet
 */
int drvPsIntfRead(drvPsIntf_t *p, void *data, size_t size);

/**
 * @brief available read size of PS interface
 *
 * @param p     PS interface pointer, can't be NULL
 * @return  available read size
 */
int drvPsIntfReadAvail(drvPsIntf_t *p);

/**
 * @brief write to PS interface
 *
 * This won't wait available uplink buffer. When uplink buffer is unavailable,
 * this will return 0.
 *
 * @param p     PS interface pointer, can't be NULL
 * @param data  data to be written
 * @param size  data size
 * @return
 *      - 0 is uplink buffer is unavailable
 *      - written size, it should be equal to \p size on success
 *      - -1 if \p data is NULL or \p size is too large for one packet
 */
int drvPsIntfWrite(drvPsIntf_t *p, const void *data, size_t size);

/**
 * @brief write multiple buffers to PS interface
 *
 * When \p data[n] is NULL or \p size[n] is too large, the buffers from it
 * won't be written.
 *
 * This won't wait available uplink buffer. So, the returned written size will be
 * smaller than provided total size.
 *
 * @param p     PS interface pointer, can't be NULL
 * @param count buffer count
 * @param data  array of buffer address to be written
 * @param size  array of buffer size to be written
 * @return
 *      - written bytes, may be smaller than provided total size
 */
int drvPsIntfWriteMulti(drvPsIntf_t *p, size_t count, const void *data[], size_t size[]);

#ifdef __cplusplus
}
#endif
#endif
