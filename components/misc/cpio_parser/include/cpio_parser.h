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

#ifndef __CPIO_PARSER_H_
#define __CPIO_PARSER_H_

#include "osi_compiler.h"
#include <stdint.h>

OSI_EXTERN_C_BEGIN

typedef struct cpio_stream cpioStream_t;

/**
 * \brief Cpio file struct
 */
typedef struct
{
    char *name;
    uint8_t *data;
    uint32_t data_size;
    uint16_t mode;
} cpioFile_t;

/**
 * \brief Cpio stream configure
 */
typedef struct
{
    uint32_t file_size_max;
    uint32_t file_path_max;
} cpioStreamCfg_t;

/**
 * \brief Create a cpio stream
 *
 * \param cfg   stream configure
 * \return  cpio stream or NULL
 */
cpioStream_t *cpioStreamCreate(const cpioStreamCfg_t *cfg);

/**
 * \brief Destroy the cpio stream
 * \param stream    the stream
 */
void cpioStreamDestroy(cpioStream_t *stream);

/**
 * \brief Push data to the stream
 * \param stream    the stream
 * \param data      the data
 * \param len       the data length
 */
void cpioStreamPushData(cpioStream_t *stream, void *data, uint32_t len);

/**
 * \brief Check the number of unused files in current stream
 * \param stream    the stream
 * \return the number of files
 */
uint32_t cpioStreamGetFileCount(cpioStream_t *stream);

/**
 * \brief Pop a file from the stream
 * \param stream    the stream
 * \return  one cpio file (and file count -1)
 */
cpioFile_t *cpioStreamPopFile(cpioStream_t *stream);

/**
 * \brief Destroy a cpio file from stream
 * \param stream    the stream
 * \param file      the cpio file
 */
void cpioStreamDestroyFile(cpioStream_t *stream, cpioFile_t *file);

OSI_EXTERN_C_END

#endif