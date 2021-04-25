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

#ifndef DECOMPRESS_UNLZMA_H
#define DECOMPRESS_UNLZMA_H

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#define LZMA_HEADER_SIZE (13)

/**
 * \brief probe lzma stream properties
 *
 * There is 13 bytes header at the beginning of lzma stream. This will get
 * size information from the header.
 *
 * \param buf       lzma stream buffer, must be valid
 * \param in_len    lzma stream buffer size, must be larger than header size
 * \param out_sizep output for decompressed size
 * \param dec_bufsizep  output for decompress buffer
 * \return
 *      - 0 on success
 *      - -1 on error
 */
int lzmaDecompressProbe(const void *buf, long in_len, long *out_sizep, long *dec_bufsizep);

/**
 * \brief decompress lzma stream in memory
 *
 * \param buf       lzma stream buffer, must be valid
 * \param in_len    lzma stram buffer size
 * \param output    output buffer, must be enough to hold all output data
 * \param dec_buf   decompress work buffer, NULL for dynamic allocation
 * \param posp      position consumed of input buffer
 * \return
 *      - 0 on success
 *      - -1 on error
 */
int lzmaDecompressMem(const void *buf, long in_len, void *output, void *dec_buf, long *posp);

/**
 * \brief decompress lzma stream in file
 *
 * \param fd        file descriptor, decompress from current position
 * \param output    output buffer
 * \return
 *      - 0 on success
 *      - -1 on error
 */
int lzmaDecompressFile(int fd, void *output);

/**
 * \brief decompress lzma stream in file
 *
 * \param fname     file descriptor, decompress from current position
 * \param output    output buffer
 * \param out_sizep output for decompressed size
 * \return
 *      - 0 on success
 *      - -1 on error
 */
int lzmaDecompressFileName(const char *fname, void *output, long *out_sizep);

OSI_EXTERN_C_END
#endif
