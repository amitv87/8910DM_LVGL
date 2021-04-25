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

#ifndef _SECURE_BOOT_ENCRYPT_H_
#define _SECURE_BOOT_ENCRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encrypt part of the first stage bootloader
 *
 * When security is enabled, part of first stage bootloader is encrypted
 * in flash. When ROM loads from flash, this part will be decrypted before
 * execute first stage bootloader. This encrypt function can match the
 * decrypt procedure in ROM.
 *
 * After encryption, the data is replaced at the original place.
 *
 * Caller should ensure \a data contains all data to be loaded by ROM.
 */
void secureBootEncrypt(void *data);

#ifdef __cplusplus
}
#endif
#endif
