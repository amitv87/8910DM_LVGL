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

#ifndef _BOOT_AES_H_
#define _BOOT_AES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Encrypt the data using aes algorithm.
 *
 * @buf Input the address of the data.
 * @len Input the length of the data.
 * @key Input the address of the key, the key len is 16 bytes.
 * @return
 *      - 0 verify ok.
 *      - nonzero verify fail.
 */
void aesEncryptObj(void *buf, uint32_t len, void *key);

#ifdef __cplusplus
}
#endif
#endif /* _BOOT_AES_H_ */
