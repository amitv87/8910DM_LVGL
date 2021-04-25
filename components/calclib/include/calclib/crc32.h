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

#ifndef _CALCLIB_CRC32_H_
#define _CALCLIB_CRC32_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * initial value for CRC32 calculation
 *
 * \return  initial value
 */
uint32_t crc32Init(void);

/**
 * update of CRC32 calculation
 *
 * \param crc   current value
 * \param p     buffer for CRC 32 calculation
 * \param size  buffer size
 * \return  updated value
 */
uint32_t crc32Update(uint32_t crc, const void *p, size_t size);

/**
 * calculate CRC32
 *
 * \param p     buffer for CRC 32 calculation
 * \param size  buffer size
 * \return  CRC32 value
 */
uint32_t crc32Calc(const void *p, size_t length);

/**
 * variant of CRC32 with different initial value
 *
 * This is not a common used CRC32 calculation. Don't use it unless
 * for backward compatible.
 *
 * \return  initial value
 */
uint32_t crc32VariantInit(void);

/**
 * variant of CRC32 with different initial value
 *
 * This is not a common used CRC32 calculation. Don't use it unless
 * for backward compatible.
 *
 * \param crc   current value
 * \param p     buffer for CRC 32 calculation
 * \param size  buffer size
 * \return  updated value
 */
uint32_t crc32VariantUpdate(uint32_t crc, const void *p, size_t size);

/**
 * variant of CRC32 with different initial value
 *
 * This is not a common used CRC32 calculation. Don't use it unless
 * for backward compatible.
 *
 * \param p     buffer for CRC 32 calculation
 * \param size  buffer size
 * \return  CRC32 value
 */
uint32_t crc32VariantCalc(const void *p, size_t length);

/**
 * initial value for CRC8/ROHC calculation
 *
 * \return  CRC8/ROHC initial value
 */
uint8_t crc8RohcInit(void);

/**
 * update of CRC8/ROHC calculation
 *
 * \param crc   current value
 * \param p     buffer for CRC8/ROHC calculation
 * \param size  buffer size
 * \return  updated value
 */
uint8_t crc8RohcUpdate(uint8_t crc, const void *p, size_t size);

/**
 * calculate CRC8/ROHC
 *
 *  poly=0x07, init=0xff, refin=true, refout=true, xorout=0x00, residue=0x00
 *
 * \param p     buffer for CRC8/ROHC calculation
 * \param size  buffer size
 * \return  CRC8 value
 */
uint8_t crc8RohcCalc(const void *p, size_t size);

/**
 * calculate CRC16 used in NV
 *
 * \param p     buffer for CRC16/NV calculation
 * \param size  buffer size
 * \return  CRC16 value
 */
uint16_t crc16NvCalc(const void *p, size_t size);

/**
 * calculate CRC16 used in BSL/FDL
 *
 * \param p         buffer
 * \param size      buffer size
 * \return  CRC16 value
 */
uint16_t crc16BslCalc(const void *p, unsigned size);

/**
 * check whether BSL/FDL CCR16 matches
 *
 * It will check whether the last 2 bytes of the buffer is the big endian
 * CRC16 calculated by \p romBslCrc16 for the preceeding bytes.
 *
 * \param p         buffer
 * \param size      buffer size
 * \return
 *      - true if the tail CR16 matches
 *      - false if the tail CRC16 doesn't match
 */
bool crc16BslCheck(const void *p, unsigned size);

#ifdef __cplusplus
}
#endif
#endif
