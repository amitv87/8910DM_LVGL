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

#ifndef __LZMAR_H__
#define __LZMAR_H__

#include "osi_compiler.h"
#include <stddef.h>

OSI_EXTERN_C_BEGIN

// Software LZMAr compress and decompress.
//
// This compress and decompress can match hardware LZMA. It is a modified
// LZMA. And it is the reason of "r".
//
// These APIs aren'y optimized for memory usage and performace. At compress,
// it will allocate 245,904 + 65,536 + 1,536 + 1,536 + 536,584 bytes.
// Also, the decompress is not thread safe.

/**
 * \brief compress data to blocked compressed output
 *
 * The output buffer should be larger enough to hole the compressed data.
 *
 * \param input     input data
 * \param insize    input data size
 * \param output    output buffer
 * \param outsize   output buffer size
 * \parem block_size    compress block size
 * \return
 *      - compressed data size
 *      - negative on error
 */
int lzmarCompress(const void *input, size_t insize, void *output, size_t outsize,
                  unsigned block_size);

/**
 * \brief decompress blocked compressed data
 *
 * The output buffer should be larger enough to hole the decompressed data.
 *
 * \param input     input data
 * \param insize    input data size
 * \param output    output buffer
 * \param outsize   output buffer size
 * \return
 *      - decompressed data size
 *      - negative on error
 */
int lzmarDecompress(const void *input, size_t insize, void *output, size_t outsize);

/**
 * \brief get blocked compressed data size
 *
 * \param input     input data
 * \return
 *      - decompressed data size
 *      - negative on error
 */
int lzmarDataSize(const void *input);

OSI_EXTERN_C_END
#endif
