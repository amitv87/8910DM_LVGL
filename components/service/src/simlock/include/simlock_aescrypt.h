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

#ifndef _SIM_AESCRYPT_H_
#define _SIM_AESCRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif
bool simlockAesEncrypt(const unsigned char *input, unsigned short input_len,
                       unsigned char *output, unsigned char *inkey, unsigned short key_len);
bool simlockAesDecrypt(unsigned char *input, unsigned short input_len,
                       unsigned char *output, unsigned char *inkey, unsigned short key_len);

#ifdef __cplusplus
}
#endif
#endif /*_SIM_AESCRYPT_H_*/
