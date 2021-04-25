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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "boot_fdl.h"
#include "boot_platform.h"
#include "boot_secure.h"
#include "boot_aes.h"
#include "osi_log.h"
#include <string.h>
#include <machine/endian.h>
#include "image_header.h"

struct chip_id
{
    uint32_t chip; /* CHIP(31:15) NA(15:15) BOND(14:12) METAL(11:0) */
    uint32_t res1;
    uint16_t date; /*  YYYY:MMMM:DDDDD */
    uint16_t wafer;
    uint16_t xy;
    uint16_t res2;
};

struct chip_security_context
{
    uint8_t rda_key_index; /* 0-5 : 0 default key */
    uint8_t vendor_id;     /* 0-50 */
    uint16_t flags;
#define RDA_SE_CFG_UNLOCK_ALLOWED (1 << 0)
#define RDA_SE_CFG_SECURITY_ENABLE_BIT (1 << 4)
#define RDA_SE_CFG_INDIRECT_SIGN_BIT (1 << 5)
#define RDA_SE_CFG_RDCERT_DISABLE_BIT (1 << 6)
#define RDA_SE_CFG_TRACE_DISABLE_BIT (1 << 7)
};

struct chip_unique_id
{
    uint8_t id[32];
};

#define UNIQUE_ID_ANTI_CLONE 0
#define UNIQUE_ID_RD_CERT 1

#define SIGBYTES 64
#define PUBLICBYTES 32
#define FPLEN 8
struct pubkey
{
    uint8_t rdasign[4]; /* RDAS */
    uint8_t pkalg[2];   /*Ed */
    uint8_t dummy[2];
    uint8_t name[16];
    uint8_t fingerprint[FPLEN];
    uint8_t pubkey[PUBLICBYTES];
};

struct sig
{
    uint8_t rdasign[4]; /* RDAS */
    uint8_t pkalg[2];   /* Ed */
    uint8_t hashalg[2]; /* Po/B2/SH */
    uint8_t name[16];
    uint8_t fingerprint[FPLEN];
    uint8_t sig[SIGBYTES];
};

struct pubkey_cert_t
{
    struct
    {
        uint16_t se_cfg;      /* vendor_id and key_index */
        uint8_t dummy[94];    /* 96 */
        struct pubkey pubkey; /* +64 */
    } c;
    struct sig signature; /* +96 */
};

struct rda_cert_t
{
    struct sig rnd_cert; /**
                          * R&D Certificate:
                          * Create a signature with name[16] =
                          *    "  R&D-CERT"(10)
                          *    "F:0000"(6)
                          * zeros[16] = 0...
                          * Then Hash:
                          *     sig.name, zeros, UNIQUE-ID
                          * len = 96
                          */

    uint8_t dummy[160]; /* free space without sign checking. len = 160 */
};

struct rda_se_image
{
    uint8_t spl_code[48128 + 160];
    struct sig image_signature;              /* 96  */
    struct rda_cert_t cert;                  /* 256 */
    struct pubkey_cert_t vendor_pubkey_cert; /* 256 */
};

struct spl_security_info
{
    uint32_t version;
    int secure_mode;
    struct chip_id chip_id;
    struct chip_security_context chip_security_context;
    struct chip_unique_id chip_unique_id;
    struct pubkey pubkey;
    uint8_t random[32];
};

struct ROM_crypto_api
{
    char magic[8];    /* "RDA API" */
    unsigned version; /* 100 */

    /* signature */
    int (*signature_open)(
        const uint8_t *message, unsigned length,
        const struct sig *sig,
        const struct pubkey *pubkey);
    /* Return values
     * positive values is for invalid arguments
     */
#define ROM_API_SIGNATURE_OK 0
#define ROM_API_SIGNATURE_FAIL -1

    /* hash */
    unsigned sz_hash_context;
    int (*hash_init)(unsigned *S, uint8_t outlen);
    int (*hash_update)(unsigned *S, const uint8_t *in, unsigned inlen);
    int (*hash_final)(unsigned *S, uint8_t *out, uint8_t outlen);

    /* info API */
    void (*get_chip_id)(struct chip_id *id);
    void (*get_chip_unique)(struct chip_unique_id *out, int kind);
    int (*_getChipSecurityContext)(struct chip_security_context *context,
                                   struct pubkey *pubkey);
    /* Return values */
#define ROM_API_SECURITY_ENABLED 0
#define ROM_API_SECURITY_DISABLED 1
#define ROM_API_INVALID_KEYINDEX 2
#define ROM_API_INVALID_VENDOR_ID 3
#define ROM_API_SECURITY_UNAVAILABLE 4

    /* RND */
    void (*get_chip_true_random)(uint8_t *out, uint8_t outlen);

    int (*signature_open_w_hash)(uint8_t message_hash[64],
                                 const struct sig *signature,
                                 const struct pubkey *pubkey);
    void (*decrypt_aes_image)(uint8_t *buf, int len);
    /* Return values
     * same as signature_open() plus ROM_API_UNAVAILABLE if method is unavailable.
     */
#define ROM_API_UNAVAILABLE 100

    /* Future extension */
    unsigned dummy[15];
};

/* The ROMAPI is allocated either at 0x3f00 or 0xff00. */
#define ROMAPI_BASE 0x3f00
#define ROMAPI_BASE_LIST ROMAPI_BASE, 0xff00, 0

struct ROM_crypto_api *romapi = (void *)ROMAPI_BASE;

/* Detect the ROMAPI base address */

static void _romapiSetBase(void)
{
    const unsigned rapi[] = {ROMAPI_BASE_LIST};
    const unsigned *p = &rapi[0];
    while (*p)
    {
        romapi = (void *)*p;
        if (memcmp(romapi->magic, "RDA API", 8) == 0)
            break;
        p++;
    }
}

static void _getChipUnique(struct chip_unique_id *id, int kind)
{
    romapi->get_chip_unique(id, kind);
}

static int _signatureCheck(
    const uint8_t *message, unsigned length,
    const struct sig *sig,
    const struct pubkey *pubkey)
{
    int ret = romapi->signature_open(message, length, sig, pubkey);
    if (!ret)
        OSI_LOGI(0, "BOOTSECURE: secure boot verify ok...");
    else
        OSI_LOGI(0, "BOOTSECURE: secure boot verify fail ret = %d", ret);

    return ret;
}

static int _getChipSecurityContext(struct chip_security_context *context, struct pubkey *pubkey)
{
    return romapi->_getChipSecurityContext(context, pubkey);
}

static void _getDdeviceSecurityContext(struct spl_security_info *info)
{
    info->secure_mode = _getChipSecurityContext(
        &info->chip_security_context,
        &info->pubkey);

    int flags = info->chip_security_context.flags;

    OSI_LOGD(0, "BOOTSECURE: PUBKEY0  = 0x%08x \n",
             __ntohl(*(uint32_t *)&info->pubkey.pubkey[0]));

    /* Fix the return code if security hasn't been enabled */
    if ((flags & RDA_SE_CFG_SECURITY_ENABLE_BIT) == 0)
    {
        info->secure_mode = ROM_API_SECURITY_DISABLED;
        return;
    }

    /* Sanity check for PKEY */
    if (memcmp(&info->pubkey, "RDASEd", 6) != 0)
    {
        OSI_LOGI(0, "BOOTSECURE: Public key for signature check invalid\n");
        info->secure_mode = -1; /* better: ROM_API_SECURITY_INVALID_PKEY */
        return;
    }
}

static int _imageSignVerify(const uint8_t *buffer, uint32_t len)
{
    struct spl_security_info info;
    _getDdeviceSecurityContext(&info);

    len -= sizeof(struct sig);
    const struct sig *signature = (const struct sig *)(buffer + len);

    int secure_mode = info.secure_mode;
    switch (secure_mode)
    {
    case ROM_API_SECURITY_ENABLED:
        OSI_LOGI(0, "BOOTSECUERE: secure boot enable...");
        return _signatureCheck(buffer, len, signature, &info.pubkey);
    case ROM_API_SECURITY_DISABLED:
    case ROM_API_SECURITY_UNAVAILABLE:
        OSI_LOGI(0, "BOOTSECURE: secure boot disable...");
        return 0;
    default:
        return secure_mode; /* This is != 0 -> verify error */
    }
}

#define uimage_to_cpu(x) __ntohl(x)
static uint32_t _image_GetSize(const image_header_t *hdr)
{
    return uimage_to_cpu(hdr->ih_size);
}

static uint32_t _imageGetImageSize(const image_header_t *hdr)
{
    return _image_GetSize(hdr) + sizeof(image_header_t);
}

bool checkSecureBootState(void)
{
    struct spl_security_info info;
    _romapiSetBase();
    _getDdeviceSecurityContext(&info);
    if (!info.secure_mode ||
        info.chip_security_context.flags & RDA_SE_CFG_RDCERT_DISABLE_BIT)
        return true;
    else
        return false;
}

int checkUimageSign(void *header)
{
    _romapiSetBase();
    const image_header_t *hdr = (const image_header_t *)header;
    return _imageSignVerify((const uint8_t *)header,
                            _imageGetImageSize(hdr) + sizeof(struct sig));
}

int checkDataSign(void *buf, uint32_t len)
{
    _romapiSetBase();
    return _imageSignVerify((const uint8_t *)buf, len);
}

int antiCloneEncryption(void *buf, uint32_t len)
{
    struct spl_security_info info;
    struct chip_unique_id rom_id;
    _getDdeviceSecurityContext(&info);
    int secure_mode = info.secure_mode;

    _romapiSetBase();
    _getChipUnique(&rom_id, UNIQUE_ID_ANTI_CLONE);
    switch (secure_mode)
    {
    case ROM_API_SECURITY_ENABLED:
        aesEncryptObj(buf, len, &rom_id);
        return 0;
    case ROM_API_SECURITY_DISABLED:
        if (info.chip_security_context.flags & RDA_SE_CFG_RDCERT_DISABLE_BIT)
        {
            OSI_LOGI(0, "BOOTSECURE: encrypt because of overwrite by RDCERT \n");
            aesEncryptObj(buf, len, &rom_id);
        }
        return 0;
    case ROM_API_SECURITY_UNAVAILABLE:
        return 0;
    default:
        return secure_mode; // This is != 0 -> verify error
    }
}
