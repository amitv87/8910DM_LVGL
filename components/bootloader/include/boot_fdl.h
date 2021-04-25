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

#ifndef _BOOT_FDL_H_
#define _BOOT_FDL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "boot_fdl_channel.h"
#include "osi_api.h"

struct bootSpiFlash;

/**
 * @brief the FDL instance
 */
typedef struct fdl_engine fdlEngine_t;

struct fdl_packet
{
    uint16_t type;      ///< cpu endian
    uint16_t size;      ///< cpu endian
    uint8_t content[1]; ///< placeholder for data
} __attribute__((packed));

/**
 * @brief FDL packet struct
 */
typedef struct fdl_packet fdlPacket_t;

typedef enum
{
    SYS_STAGE_NONE,
    SYS_STAGE_CONNECTED,
    SYS_STAGE_START,
    SYS_STAGE_GATHER,
    SYS_STAGE_END,
    SYS_STAGE_ERROR
} fdlDnldStage_t;

typedef enum
{
    FLASH_STAGE_NONE,
    FLASH_STAGE_ERASE,
    FLASH_STAGE_WRITE,
    FLASH_STAGE_WAIT_DATA,
    FLASH_STAGE_FINISH,
} fdlFlashDnldStage_t;

/**
 * @brief structure to store download state
 */
typedef struct
{
    fdlDnldStage_t stage;            ///< download stage
    fdlFlashDnldStage_t flash_stage; ///< async flash stage
    uint32_t total_size;             ///< download total size
    uint32_t received_size;          ///< received size
    uint32_t start_address;          ///< download start address
    uint32_t write_address;          ///< written address, end address in async
    uint32_t erase_address;          ///< erased address, end address in async
    uint32_t data_verify;            ///< data crc
    struct bootSpiFlash *flash;      ///< flash instance for flash download
} fdlDataDnld_t;

/**
 * @brief FDL packet processor
 */
typedef void (*fdlProc_t)(fdlEngine_t *fdl, fdlPacket_t *pkt, void *ctx);

/**
 * @brief FDL polling processing
 */
typedef void (*fdlPolling_t)(fdlEngine_t *fdl, void *ctx);

/**
 * @brief Create a fdl engine
 *
 * @param channel       the fdl channel
 * @param max_packet_len    maximum packet length
 * @return
 *      - NULL          fail
 *      - otherwise     the fdl engine instance
 */
fdlEngine_t *fdlEngineCreate(fdlChannel_t *channel, unsigned max_packet_len);

/**
 * @brief Destroy the fdl engine
 *
 * @param fdl   the fdl engine
 */
void fdlEngineDestroy(fdlEngine_t *fdl);

/**
 * @brief Get channel of fdl engine
 *
 * @param fdl   the fdl engine
 * @return channel
 */
fdlChannel_t *fdlEngineGetChannel(fdlEngine_t *fdl);

/**
 * @brief Get maximum packet length of fdl engine
 *
 * @param fdl   the fdl engine
 * @return maximum packet lemgth
 */
unsigned fdlEngineGetMaxPacketLen(fdlEngine_t *fdl);

/**
 * @brief Read raw data from fdl
 *
 * @param fdl   the fdl engine
 * @param buf   the room to store data
 * @param size  the buffer size
 * @retuen
 *      - num   actual read size in bytes
 */
uint32_t fdlEngineReadRaw(fdlEngine_t *fdl, void *buf, unsigned size);

/**
 * @brief Write raw data via fdl engine
 *
 * @param fdl       the fdl engine
 * @param data      the data to be written
 * @param length    the data length
 * @return
 *      - num       actual length written
 */
uint32_t fdlEngineWriteRaw(fdlEngine_t *fdl, const void *data, unsigned length);

/**
 * @brief FDL Send version string to the other peer
 *
 * @param fdl   the fdl engine
 * @return
 *      - true  success
 *      - false false
 */
bool fdlEngineSendVersion(fdlEngine_t *fdl);

/**
 * @brief FDL identify
 */
bool fdlEngineIdentify(fdlEngine_t *fdl, unsigned timeout);

/**
 * @brief FDL send no data response of specific type
 *
 * @param fdl   the fdl engine
 * @param type  response type
 * @return
 *      - true  success
 *      - false fail
 */

bool fdlEngineSendRespNoData(fdlEngine_t *fdl, uint16_t type);

/**
 * @brief FDL send response of specific type with data
 *
 * @param fdl   the fdl engine
 * @param type  response type
 * @param data  the data to be sent
 * @param len   the data length
 * @return
 *      - true  success
 *      - false fail
 */

bool fdlEngineSendRespData(fdlEngine_t *fdl, uint16_t type, const void *data, unsigned length);

/**
 * @brief FDL change device baud rate
 *
 * @param fdl   the fdl engine
 * @param baud  baud rate to be changed
 * @return
 *      - true on success
 *      - false on fail, usually the baud rate is not supported
 */
bool fdlEngineSetBaud(fdlEngine_t *fdl, unsigned baud);

/**
 * @brief FDL engine process
 *
 * @note never return except failed
 *
 * @param fdl       the fdl engine
 * @param proc      the fdl packet processor
 * @param ctx       caller context
 * @return
 *      - false fail
 */
bool fdlEngineProcess(fdlEngine_t *fdl, fdlProc_t proc, fdlPolling_t polling, void *ctx);

/**
 * @brief start FDL download
 *
 * \p fdlEngineProcess will be called inside.
 *
 * @param fdl       the fdl engine
 * @param devtype   device type
 * @return
 *      - true on success
 *      - false on fail
 */
bool fdlDnldStart(fdlEngine_t *fdl, unsigned devtype);

#ifdef __cplusplus
}
#endif

#endif
