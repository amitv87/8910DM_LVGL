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

#ifndef __ML_H__
#define __ML_H__

#include "osi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    ML_ISO8859_1,
    ML_UTF8,
    ML_UTF16BE,
    ML_UTF16LE,
    ML_GSM,
    ML_CP936
};

void mlInit(void);

/**
 * convert a string charset
 *
 * When \a from_charset or \a to_charset is unknown or unsupported,
 * return NULL.
 *
 * The returned pointer is allocated inside, caller should free it.
 *
 * At most \a from_size byte count will be processed. When \a from_size
 * is -1, process till null chracter.
 *
 * null character will always be inserted into the output string.
 * Invalid characters are replaced silently.
 *
 * \a to_size can be NULL in case that the output byte count is not needed.
 *
 * @param from          input string
 * @param from_size     input string byte count
 * @param from_chset    input string charset
 * @param to_chset      output string charset
 * @param to_size       output string byte count
 * @return
 *      - NULL: invalid parameters
 *      - output string
 */
void *mlConvertStr(const void *from, int from_size, unsigned from_chset, unsigned to_chset, int *to_size);

int mlCharCount(const void *s, unsigned chset);

int mlStrBytes(const void *s, unsigned chset);

uint16_t mlGetUnicode(uint16_t from, uint32_t from_chset);
uint16_t mlGetOEM(uint32_t unicd, uint32_t to_chset);

#ifdef __cplusplus
}
#endif
#endif
