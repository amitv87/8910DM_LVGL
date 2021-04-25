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

#ifndef _CFW_CHSET_H_
#define _CFW_CHSET_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief convert ASCII string to BCD
 *
 * Convert rules:
 * - 0-9:
 * - *: 0xa
 * - #: 0xb
 * - p/P: 0xc
 * - w/W: 0xd
 * - +: 0xe
 *
 * Example: 12345 -> 0x21 0x43 0xF5
 *
 * Caller should make sure there are enough space in output BCD array.
 * The output size is: (size + 1) >> 1
 *
 * @param s     input ASCII string
 * @param size  input ASCII string size
 * @param bsd   output BCD array
 * @return
 *      - -1: failed, due to invalid character in input string
 *      - size of BCD array
 */
int cfwDialStringToBcd(const void *src, size_t size, void *bcd);

/**
 * @brief convert BCD to ASCII string
 *
 * Convert rules:
 * - 0-9:
 * - 0xa: *
 * - 0xb: #
 * - 0xc: p
 * - 0xd: w
 * - 0xe: +
 * 0xf can only appear as the higher 4-bits of the last byte of BCD array.
 *
 * Example: 0x21 0x43 0xF5 -> 12345
 *
 * Caller should make sure there are enough space in output BCD array.
 * Also, \\0 will be appended to the end of output ASCII string.
 * The output size can be: size*2 or size*2+1.
 *
 * @param bcd   input BCD array
 * @param size  input BCD array size
 * @param s     output ASCII string
 * @return
 *      - -1: failed, due to invalid input
 *      - size of ASCII string, not including \\0
 */
int cfwBcdToDialString(const void *src, size_t size, void *dst);

/**
 * @brief convert HEX string to bytes
 *
 * Example:
 * - 123456 -> 0x12 0x34 0x56
 * - 1234567 -> 0x12 0x34 0x56 0x70
 *
 * Caller should make sure there are enough space in output byte array.
 * The output size is: (size + 1) >> 1
 *
 * @param src   input HEX string
 * @param size  input HEX string size
 * @param dst   output byte array
 * @return
 *      - -1: failed, due to invalid character in input string
 *      - size of bytes
 */
int cfwHexStrToBytes(const void *src, size_t size, void *dst);

/**
 * @brief convert bytes to HEX string
 *
 * An extra \\0 will be appended to the output string.
 *
 * Caller should make sure there are enough space in output string.
 * The output size is: (size * 2 + 1)
 *
 * @param src   input byte array
 * @param size  input byte array size
 * @param dst   output HEX string
 * @return
 *      - -1: failed, due to NULL pointer
 *      - size of string, , not including \\0
 */
int cfwBytesToHexStr(const void *src, size_t size, void *dst);

int cfwDtmfCharToTone(char c);
int cfwToneToDtmfChar(char c);

int cfwDecode7Bit(const void *src, void *dst, size_t size);
int cfwEncode7Bit(const void *src, void *dst, size_t size);

/**
 * @brief pack time and time zone
 *
 * Packed time is 7 bytes:
 * - [0]: BCD of (year - 2000)
 * - [1]: BCD of month (1-12)
 * - [2]: BCD of day in month (1-31)
 * - [3]: BCD of hour (0-23)
 * - [4]: BCD of minute (0-59)
 * - [5]: BCD of second (0-59)
 * - [6]: reversed BCD of time zone in quarter. WHen time zone is negative, it is
 *        ored with 0x08
 *
 * For example:
 * - 2018-01-02 13:14:15 +8 will be packed into: 0x18 0x01 0x02 0x13 0x14 0x15 0x23
 * - 2018-01-02 13:14:15 -8 will be packed into: 0x18 0x01 0x02 0x13 0x14 0x15 0x2b
 *
 * The relationship between epoch time, local time and time zone:
 *      local_time = epoch_time + time_zone
 *
 * @param pack      packed time
 * @param seconds   seconds from 1970-01-01 in local time
 * @param time_zone time zone offset in seconds
 */
void cfwPackTime(void *pack, uint32_t seconds, int time_zone);

/**
 * @brief unpack time and time zone
 *
 * Packed time is described in \p cfwPackTime.
 *
 * @param pack      packed time
 * @param seconds   seconds from 1970-01-01 in local time
 * @param time_zone time zone offset in seconds
 */
void cfwUnpackTime(const void *pack, uint32_t *seconds, int *time_zone);

#define SUL_AsciiToGsmBcd(src, size, dst) cfwDialStringToBcd(src, size, dst)
#define SUL_gsmbcd2ascii(src, size, dst) cfwBcdToDialString(src, size, dst)
#define SUL_GsmBcdToAscii(src, size, dst) cfwBcdToDialString(src, size, dst)
#define SUL_hex2ascii(src, size, dst) cfwHexStrToBytes(src, size, dst)
#define SUL_Decode7Bit(src, dst, size) cfwDecode7Bit(src, dst, size)
#define SUL_Encode7Bit(src, dst, size) cfwEncode7Bit(src, dst, size)

#ifdef __cplusplus
}
#endif
#endif
