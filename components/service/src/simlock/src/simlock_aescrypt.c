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

/*aescrpt*/
#include "srv_config.h"
#include "osi_api.h"
#include "osi_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "aes.h"
/**---------------------------------------------------------------------------*
 **                         macro define                                 *
 **---------------------------------------------------------------------------*/
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

/*****************************************************************************/
//  Description : do aes_ecb Encrypted algrithm
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
bool simlockAesEncrypt(const unsigned char *input, unsigned short input_len, unsigned char *output, unsigned char *inkey, unsigned short key_len)
{
    int offset;
    unsigned char buffer[16] = {0};
    aes_context aes_ctx = {0};

    OSI_ASSERT(input != NULL && output != NULL && inkey != NULL, "simlockAesEncrypt valid");

    if ((input_len & 0x0F) != 0)
    {
        OSI_LOGI(0, "SPRD_AES_Enc():input size=%d not a multiple of 16", input_len);
        return false;
    }
    aes_setkey_enc(&aes_ctx, inkey, key_len << 3);

    for (offset = 0; offset < input_len; offset += 16)
    {
        memcpy(buffer, &input[offset], 16);
        aes_crypt_ecb(&aes_ctx, AES_ENCRYPT, buffer, buffer);
        memcpy(&output[offset], buffer, 16);
    }
    return true;
}

/*****************************************************************************/
//  Description : do aes_ecb Decrypted algrithm
//  Global resource dependence : none
//  Note:
/*****************************************************************************/
bool simlockAesDecrypt(unsigned char *input, unsigned short input_len, unsigned char *output, unsigned char *inkey, unsigned short key_len)
{
    int offset;
    unsigned char buffer[16] = {0};
    aes_context aes_ctx = {0};

    OSI_ASSERT(input != NULL && output != NULL && inkey != NULL, "simlock_aes_decrpty valid");
    if ((input_len & 0x0F) != 0)
    {
        OSI_LOGI(0, "SPRD_AES_Enc():input size=%d not a multiple of 16,", input_len);
        return false;
    }

    aes_setkey_dec(&aes_ctx, inkey, key_len << 3);

    for (offset = 0; offset < input_len; offset += 16)
    {

        memcpy(buffer, &input[offset], 16);
        aes_crypt_ecb(&aes_ctx, AES_DECRYPT, buffer, buffer);
        memcpy(&output[offset], buffer, 16);
    }
    return true;
}
