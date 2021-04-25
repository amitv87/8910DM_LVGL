/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef __ML_INTERNAL_H__
#define __ML_INTERNAL_H__

#include "ml.h"
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_CODE_POINT (0xffffffff)

typedef SLIST_ENTRY(mlCharset) mlCharsetEntry_t;
typedef SLIST_HEAD(mlCharsetHead, mlCharset) mlCharsetHead_t;

typedef struct mlCharset
{
    /**
     * character encoding tag
     */
    unsigned chset;

    /**
     * minimal bytes for a character
     */
    uint8_t min_bytes;

    /**
     * maximum bytes for a character
     */
    uint8_t max_bytes;

    /**
     * suggested replacement for invalid character
     */
    unsigned invalid_fill_char;

    /**
     * read a character from input
     *
     * When input size is not enough for a valid character, the output
     * value is set to 0 (for end of string), and the return size is the
     * bytes to be skipped. The return bytes will never exceed the input
     * size, and it won't be 0 if the input \a size is not 0.
     *
     * At invalid character, the output value is set to
     * \a INVALID_CODE_POINT. And the return size is the bytes to be
     * skipped.
     *
     * For UTF16, it is not required that \a in to be 2 bytes aligned.
     *
     * @param in        input bytes/string
     * @param size      input size in bytes
     * @param val       output UNICODE value, or INVALID_CODE_POINT
     * @return
     *      - 0: if end of string or input size is 0
     *      - others: bytes consumed
     */
    int (*read_char)(const void *in, unsigned size, unsigned *val);

    /**
     * write a character to output
     *
     * When \a val is 0, it means end of string. The end of string
     * byte/bytes shall be write to output.
     *
     * When \a val is invalid, it will return -1.
     *
     * When \a out is NULL, no write shall be performed, and the return
     * byte count is valid still.
     *
     * When output size is not enough, no write shall be performed, and
     * the return byte count is valid still.
     *
     * For UTF16, it is not required that \a in to be 2 bytes aligned.
     *
     * @param out       output bytes/string
     * @param size      output size
     * @param val       UNICODE value to be written
     * @return  byte count for the UNICODE, no matter whether it is written.
     *          or -1 for invalid character.
     */
    int (*write_char)(void *out, unsigned size, unsigned val);

    mlCharsetEntry_t iter;
} mlCharset_t;

void mlAddCharset(mlCharset_t *chset);
void mlAddGsm(void);
void mlAddUtf8(void);
void mlAddUtf16be(void);
void mlAddUtf16le(void);
void mlAddCP936(void);
void mlAddIso8859_1(void);

#ifdef __cplusplus
}
#endif
#endif
