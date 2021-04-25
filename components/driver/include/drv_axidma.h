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

#ifndef __AXIDMA_H__
#define __AXIDMA_H__

#include "osi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AXIDMA interrupt event
 */
typedef enum
{
    AD_EVT_FINISH = (1 << 0),      ///< event transfer done
    AD_EVT_PART_FINISH = (1 << 1), ///< event transfer once done (part size)
    AD_EVT_STOP = (1 << 2),        ///< event the channel stop
} drvAxidmaIrqEvent_t;

/**
 * @brief AXIDMA data type
 */
typedef enum
{
    AD_DATA_8BIT = 0,
    AD_DATA_16BIT = 1,
    AD_DATA_32BIT = 2,
    AD_DATA_64BIT = 3,
} drvAxidmaDataType_t;

/**
 * @brief function type to process AXIDMA channel interrupt
 */
typedef void (*drvAxidmaIsr_t)(drvAxidmaIrqEvent_t evt, void *p);

struct drv_axidma_channel;
struct drv_axidma_config
{
    uint32_t src_addr;             ///< source address
    uint32_t dst_addr;             ///< destination address
    uint32_t data_size;            ///< data size in byte
    uint32_t part_trans_size;      ///< transfer size once in byte
    drvAxidmaDataType_t data_type; ///< dma data type
    drvAxidmaIrqEvent_t mask;      ///< react interrupt mask
    uint8_t src_addr_fix : 1;      ///< source address fixed (like hw fifo)
    uint8_t dst_addr_fix : 1;      ///< destination address fixed
    uint8_t sync_irq : 1;          ///< request trigger mode
    uint8_t force_trans : 1;       ///< force trans start
    uint8_t req_sel_level : 1;     ///< high level trigger
};

/**
 * @brief the AXIDMA channel, call do not need to know its implement
 */
typedef struct drv_axidma_channel drvAxidmaCh_t;

/**
 * @brief AXIDMA channel config
 */
typedef struct drv_axidma_config drvAxidmaCfg_t;

/**
 * @brief AXIDMA module initialization
 *
 * CP will rely on the initialization, so it should be called before
 * release CP reset.
 */
void drvAxidmaInit(void);

/**
 * @brief AXIDMA resume after wakeup
 *
 * Due to CP will rely on AXIDMA, it will be called explicitly rather
 * than using the suspend/resume callback. This will make it earlier to
 * release CP reset.
 */
void drvAxidmaResume(osiSuspendMode_t mode, uint32_t source);

/**
 * @brief allocate an AXIDMA channel
 *
 * @return
 *      - NULL      no free axidma channel
 *      - OTHERS    axidma channel
 */
drvAxidmaCh_t *drvAxidmaChAllocate();

/**
 * @brief release an AXIDMA channel
 *
 * @param ch    the AXIDMA channel
 */
void drvAxidmaChRelease(drvAxidmaCh_t *ch);

/**
 * @brief start the AXIDMA channel
 *
 * @param ch    the AXIDMA channel
 * @param cfg   start config
 * @return
 *      - true  success
 *      - flase fail
 */
bool drvAxidmaChStart(drvAxidmaCh_t *ch, const drvAxidmaCfg_t *cfg);

/**
 * @brief stop the AXIDMA channel
 *
 * Stop the AXIDMA channel, all transfer will stop, may triger a STOP event.
 *
 * During stop, channel pending data count may be modified. So, the return
 * value of \p drvAxidmaChCount after this is unreliable. Rather, the return
 * value of this function shall be used.
 *
 * @param ch    the AXIDMA channel
 * @return
 *      - channel pending data count
 *      - 0 on invalid parameter
 */
unsigned drvAxidmaChStop(drvAxidmaCh_t *ch);

/**
 * @brief check if the AXIDMA channel is busy
 *
 * @param ch    the AXIDMA channel
 * @return
 *      - true  busy
 *      - false not busy
 */
bool drvAxidmaChBusy(drvAxidmaCh_t *ch);

/**
 * @brief set AXIDMA map for a specific channel
 *
 * @param ch            the AXIDMA channel
 * @param req_source    request source
 * @param ack_map       ack map
 */
void drvAxidmaChSetDmamap(drvAxidmaCh_t *ch, uint8_t req_source, uint8_t ack_map);

/**
 * @brief get AXIDMA channel pending count
 *
 * Get pending data for an AXIDMA channel in bytes.
 * Caller can calculate the AXIDMA actually transfered data size
 * by total size minus this count.
 *
 * @param ch    the AXIDMA channel
 * @return
 *      - (Non-negative integer) pending data count in byte
 */
uint32_t drvAxidmaChCount(drvAxidmaCh_t *ch);

/**
 * @brief register a interrupt handler for a specific AXIDMA channel
 *
 * @param ch    the AXIDMA channel
 * @param isr   interrupt routine
 * @param param call private data (like context)
 */
void drvAxidmaChRegisterIsr(drvAxidmaCh_t *ch, drvAxidmaIsr_t isr, void *param);

/**
 * @brief stop all AXIDMA channel
 *
 * It is only intented to be called at enter panic.
 */
void drvAxidmaStopAll(void);

#endif /* __AXIDMA_H__ */
