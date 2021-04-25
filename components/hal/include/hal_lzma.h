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

#ifndef _HAL_LZMA_H_
#define _HAL_LZMA_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * enable LZMA hardware decompress module
 *
 * It should always be called LZMA hardware module is about to be used.
 * It will be called inside \p halLzmaDecompressBlock, and usually it is
 * not needed to call it manually.
 */
void halLzmaEnable(void);

/**
 * disable LZMA hardware decompress module
 *
 * It can be used after LZMA hardware module won't be used anymore. It
 * will be called inside \p halLzmaDecompressBlock.
 */
void halLzmaDisable(void);

/**
 * reset LZMA hardware decompress module
 *
 * After decompress error, LZMA hardware module should be reset. It is called
 * inside \p halLzmaDecompressBlock on decompress error, and usually it is not
 * needed to call it manually.
 */
void halLzmaReset(void);

/**
 * get data size from LZMA stream
 *
 * \param [in] stream   LZMA stream
 * \return
 *      - decompressed data size
 *      - -1 in error
 */
int halLzmaDataSize(const void *stream);

/**
 * get maximum block stream size from LZMA stream
 *
 * \param [in] stream   LZMA stream
 * \param [in] size     LZMA stream size
 * \return
 *      - maximum block stream size
 *      - -1 in error
 */
int halLzmaMaxBlockStreamSize(const void *stream, uint32_t size);

/**
 * decompress LZMA stream
 *
 * Though LZMA can work in interrupt mode, this will use polling mode.
 *
 * Some limitations:
 * - \p src must be 8 bytes aligned
 * - \p dest must be 32 bytes aligned
 * - \p src_size must be the exact compressed stream size
 * - \p dest_size must be the exact decompressed data size
 * - \p dict size must be the same as compress tool
 * - \p src and \dest must be PSRAM/DDR address
 * - Hardware may overwrite destination buffer up to 32 bytes aligned. For
 *   example, when \p dest_size is 20, then hardware may overwrite up to
 *   address 31.
 *
 * \p src_size, \p dest_size and \p dict_size are embedded in the stream
 * created by compress tool.
 *
 * At decompress, hardware will calculate a *simple* CRC of the decompressed
 * data. This can be used to verify decompressed data, if the expected CRC
 * is known.
 *
 * On error, it is needed to reset LZMA hardware module. And it will be
 * done inside.
 *
 * There are no multi-thread protection inside.
 *
 * \param [in] src          LZMA stream
 * \param [in] src_size     LZMA stream size
 * \param [in] dest         decompressed buffer
 * \param [in] dest_size    decompressed data size
 * \param [in] dict_size    dictionary size
 * \param [out] crc         decompressed data CRC, can be NULL
 * \return
 *      - true on success
 *      - false on error
 */
bool halLzmaDecompressBlock(const void *src, uint32_t src_size,
                            void *dest, uint32_t dest_size,
                            uint32_t dict_size, uint32_t *crc);

/**
 * decompress LZMA file stream
 *
 * Limitations:
 * - \p src must be 8 bytes aligned
 * - \p dest must be 32 bytes aligned
 * - \p src and \dest must be PSRAM/DDR address
 * - Hardware may overwrite destination buffer up to 32 bytes aligned.
 *
 * Though LZMA can work in interrupt mode, this will use polling mode.
 *
 * \param [in] src      LZMA file stream
 * \param [in] dest     decompressed buffer
 * \return
 *      - decompressed data size
 *      - -1 on error
 */
int halLzmaDecompressFile(const void *stream, void *dest);

#ifdef __cplusplus
}
#endif
#endif
