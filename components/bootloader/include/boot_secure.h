/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_SECURE_H_
#define _BOOT_SECURE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct simageHeader;

/**
 * @brief Check secure boot state.
 *
 * @return
 *      - false secure boot disable .
 *      - true  secure boot enable.
 */
bool bootSecureBootEnable(void);

/**
 * @brief Check the uimage signature (8910).
 *
 * For compatible, when secure boot is not enabled, it will return true
 * directly.
 *
 * @param header Input the address of the uimage header.
 * @return
 *      - true for signature check pass.
 *      - false for signature check fail.
 */
bool bootSecureUimageSigCheck(const void *header);

/**
 * @brief Check data signature (8910).
 *
 * It is assumed that the signature is at the end of data.
 *
 * For compatible, when secure boot is not enabled, it will return true
 * directly.
 *
 * @param buf Input the address of the signed data.
 * @param len Input the length of the signed data.
 * @return
 *      - true for signature check pass.
 *      - false for signature check fail.
 */
bool bootSecureEmbeddedSigCheck(const void *buf, uint32_t len);

/**
 * @brief Encrypt the data using aes algorithm in place (8910).
 *
 * It won't check whether secure boot is enabled. It is suggested to
 * call this only when secure boot is enabled.
 *
 * @param buf Input the address of the data.
 * @param len Input the length of the data.
 */
void bootAntiCloneEncryption(void *buf, uint32_t len);

/**
 * @brief Check the simage signature.
 *
 * @header Input the address of the simage header.
 * @return
 *      - 0 verify ok.
 *      - nonzero verify fail.
 */
bool bootSimageCheckSign(const struct simageHeader *header);

#ifdef __cplusplus
}
#endif
#endif
