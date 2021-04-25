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
#include "hal_efuse.h"
#include "image_header.h"
#include "hwregs.h"
#include <string.h>

#define RDASIGN "RDAS"
#define PKALG "Ed"
#define HASHALG "B2"

#define ROM_HASH_CONTEXT_WORD_COUNT (0x2e)
#define ROM_HASH_OUTBYTES (32)

#define RDA_SE_CFG_SECURITY_ENABLE_BIT0 (1 << 4)
#define RDA_SE_CFG_SECURITY_ENABLE_BIT1 (1 << 23)
#define RDA_SE_CFG_SECURITY_ENABLE_BIT2 (1 << 25)

#define RDA_RD_CERT_MAGIC 0x16158767
#define RDA_ANTI_CLONE_MAGIC 0x32456321

#define UNIQUE_ID_ANTI_CLONE 0
#define UNIQUE_ID_RD_CERT 1

#define SIGBYTES 64
#define PUBLICBYTES 32
#define FPLEN 8

#define ROMAPI_BASE_POINTER (0x3c)

struct rom_hash_context
{
    uint32_t data[ROM_HASH_CONTEXT_WORD_COUNT];
};

struct rom_hash
{
    uint8_t data[64]; // though only 32 bytes are used
};

struct chip_unique_id
{
    uint8_t id[32];
};

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

struct ROM_crypto_api
{
    char magic[8];    /* "RDA API" */
    unsigned version; /* 100 */
    void *signature_open_fn;
    unsigned sz_hash_context;
    int (*hash_init)(unsigned *S, uint8_t outlen);
    int (*hash_update)(unsigned *S, const uint8_t *in, unsigned inlen);
    int (*hash_final)(unsigned *S, uint8_t *out, uint8_t outlen);
    void *get_chip_id_fn;
    void *get_chip_unique_fn;
    void *get_chip_security_context_fn;
    void *get_chip_true_random_fn;
    int (*signature_open_w_hash)(uint8_t message_hash[64],
                                 const struct sig *signature,
                                 const struct pubkey *pubkey);
    void (*decrypt_aes_image)(uint8_t *buf, int len);
};

typedef struct
{
    uint32_t security_cfg; // efuse value
    uint32_t pubkey[8];    // efuse value
    uint32_t unique_id[2]; // efuse value
    struct ROM_crypto_api *romapi;
} bootSecureContext_t;

static bootSecureContext_t gSecureCtx;

/**
 * Initialization. There are no software cache in hal efuse, so the efuse
 * are read at initialization
 */
static void prvSecureInit(void)
{
    if (gSecureCtx.romapi != NULL)
        return;

    gSecureCtx.romapi = *(struct ROM_crypto_api **)ROMAPI_BASE_POINTER;
    OSI_LOGD(0, "romapi 0x%x %d hash 0x%x/0x%x/0x%x sig 0x%x decrypt 0x%x",
             gSecureCtx.romapi,
             gSecureCtx.romapi->version,
             gSecureCtx.romapi->hash_init,
             gSecureCtx.romapi->hash_update,
             gSecureCtx.romapi->hash_final,
             gSecureCtx.romapi->signature_open_w_hash,
             gSecureCtx.romapi->decrypt_aes_image);

    halEfuseOpen();
    halEfuseDoubleRead(RDA_EFUSE_DOUBLE_BLOCK_SECURITY_CFG_INDEX, &gSecureCtx.security_cfg);
    halEfuseDoubleRead(RDA_PUBKEY0_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[0]);
    halEfuseDoubleRead(RDA_PUBKEY1_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[1]);
    halEfuseDoubleRead(RDA_PUBKEY2_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[2]);
    halEfuseDoubleRead(RDA_PUBKEY3_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[3]);
    halEfuseDoubleRead(RDA_PUBKEY4_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[4]);
    halEfuseDoubleRead(RDA_PUBKEY5_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[5]);
    halEfuseDoubleRead(RDA_PUBKEY6_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[6]);
    halEfuseDoubleRead(RDA_PUBKEY7_EFUSE_DOUBLE_BLOCK_INDEX, &gSecureCtx.pubkey[7]);
    halEfuseDoubleRead(RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_LOW_INDEX, &gSecureCtx.unique_id[0]);
    halEfuseDoubleRead(RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_HIGH_INDEX, &gSecureCtx.unique_id[1]);
    halEfuseClose();

    OSI_LOGD(0, "efuse 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x",
             gSecureCtx.security_cfg,
             gSecureCtx.pubkey[0], gSecureCtx.pubkey[1],
             gSecureCtx.pubkey[2], gSecureCtx.pubkey[3],
             gSecureCtx.pubkey[4], gSecureCtx.pubkey[5],
             gSecureCtx.pubkey[6], gSecureCtx.pubkey[7],
             gSecureCtx.unique_id[0], gSecureCtx.unique_id[1]);
}

/**
 * Fake enable secure boot (hidden API). The pubkey matches the default key.db
 */
void bootFakeEnableSecure(void)
{
    // call init to avoid overwrite by efuse value
    prvSecureInit();

    gSecureCtx.security_cfg = RDA_SE_CFG_SECURITY_ENABLE_BIT0 |
                              RDA_SE_CFG_SECURITY_ENABLE_BIT1 |
                              RDA_SE_CFG_SECURITY_ENABLE_BIT2;
    gSecureCtx.pubkey[0] = 0xB9578CC9;
    gSecureCtx.pubkey[1] = 0x5E1AC3C8;
    gSecureCtx.pubkey[2] = 0x95D61F64;
    gSecureCtx.pubkey[3] = 0xA2D010CB;
    gSecureCtx.pubkey[4] = 0x7E5A664B;
    gSecureCtx.pubkey[5] = 0xF3D5BE57;
    gSecureCtx.pubkey[6] = 0x61F6C9F9;
    gSecureCtx.pubkey[7] = 0x2C6AA995;
}

/**
 * Call ROM - hash_init, ROM access should be enabled
 */
static inline int prvRomHashInit(struct rom_hash_context *state)
{
    return gSecureCtx.romapi->hash_init((unsigned *)state, 32);
}

/**
 * Call ROM - hash_update, ROM access should be enabled
 */
static inline int prvRomHashUpdate(struct rom_hash_context *state,
                                   const void *in, unsigned inlen)
{
    return gSecureCtx.romapi->hash_update((unsigned *)state, (const uint8_t *)in, inlen);
}

/**
 * Call ROM - hash_final, ROM access should be enabled
 */
static inline int prvRomHashFinal(struct rom_hash_context *state, void *out, uint8_t outlen)
{
    return gSecureCtx.romapi->hash_final((unsigned *)state, (uint8_t *)out, outlen);
}

/**
 * Call ROM - signature_open_w_hash, ROM access should be enabled
 */
static inline int prvRomSigCheckHash(void *hash, const void *sig, const void *pubkey)
{
    return gSecureCtx.romapi->signature_open_w_hash((uint8_t *)hash, sig, pubkey);
}

/**
 * Call ROM - anti-clone decrypt
 */
static inline void prvRomDecrypt(void *buf, unsigned len)
{
    return gSecureCtx.romapi->decrypt_aes_image((uint8_t *)buf, len);
}

/**
 * Read public key from efuse, and not check it.
 */
static void prvReadEfusePubkey(struct pubkey *key)
{
    memset(key, 0, sizeof(struct pubkey));

    key->rdasign[0] = 'R';
    key->rdasign[1] = 'D';
    key->rdasign[2] = 'A';
    key->rdasign[3] = 'S';
    key->pkalg[0] = 'E';
    key->pkalg[1] = 'd';

    uint32_t efuse_pubkey[8];
    for (int n = 0; n < 8; n++)
        efuse_pubkey[n] = OSI_FROM_BE32(gSecureCtx.pubkey[n]);

    memcpy(key->pubkey, efuse_pubkey, 32);
}

/**
 * Read unique id, and hash it to increase randomness
 */
static void prvReadChipUniqueId(struct chip_unique_id *id, int kind)
{
    struct
    {
        uint32_t lo;
        uint32_t hi;
    } unique;

    unique.lo = gSecureCtx.unique_id[0];
    unique.hi = gSecureCtx.unique_id[1];

    if (kind == UNIQUE_ID_ANTI_CLONE)
        unique.lo += RDA_ANTI_CLONE_MAGIC;
    else if (kind == UNIQUE_ID_RD_CERT)
        unique.lo += RDA_RD_CERT_MAGIC;

    struct rom_hash_context ctx;
    prvRomHashInit(&ctx);
    prvRomHashUpdate(&ctx, &unique, sizeof(unique));
    prvRomHashFinal(&ctx, id, 32);
}

/**
 * Calculate hash for signature check
 */
static void prvHashCalc(struct rom_hash *hash, const void *message, unsigned size,
                        const struct sig *sig, const struct pubkey *pubkey)
{
    struct rom_hash_context ctx;

    memset(hash, 0, sizeof(struct rom_hash));
    prvRomHashInit(&ctx);
    prvRomHashUpdate(&ctx, sig->sig, 32);
    prvRomHashUpdate(&ctx, pubkey->pubkey, 32);
    prvRomHashUpdate(&ctx, message, size);
    prvRomHashFinal(&ctx, hash, 32);
}

/**
 * Signature check by message hash
 */
static bool prvHashSigCheck(struct rom_hash *hash, const struct sig *sig,
                            const struct pubkey *pubkey)
{
    if (memcmp(sig, RDASIGN PKALG HASHALG, 8) != 0)
        return false;

    if (sig->sig[63] & 224)
        return false; // match rom

    if (memcmp(pubkey, RDASIGN PKALG, 6) != 0)
        return false;

    unsigned pubkey_sum = 0;
    for (int n = 0; n < PUBLICBYTES; ++n)
        pubkey_sum |= pubkey->pubkey[n];

    // In case of all '0', the efuse of public key shouldn't be written.
    if (pubkey_sum == 0)
        return false;

    int ret = prvRomSigCheckHash(hash, sig, pubkey);
    return (ret == 0);
}

/**
 * Signature check by message
 */
static bool prvMessageSigCheck(const void *message, unsigned size,
                               const struct sig *sig,
                               const struct pubkey *pubkey)
{
    struct rom_hash hash;
    prvHashCalc(&hash, message, size, sig, pubkey);
    return prvHashSigCheck(&hash, sig, pubkey);
}

/**
 * Signature check, signature is embedded at the end
 */
static bool prvEmbeddedSigCheck(const void *data, uint32_t size)
{
    struct pubkey pubkey;
    prvReadEfusePubkey(&pubkey);

    size -= sizeof(struct sig);
    const void *sig = (const char *)data + size;
    return prvMessageSigCheck(data, size, sig, &pubkey);
}

/**
 * Whether secure boot is enabled
 */
bool bootSecureBootEnable(void)
{
    prvSecureInit();

    int en_count = 0;
    if (gSecureCtx.security_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT0)
        en_count++;
    if (gSecureCtx.security_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT1)
        en_count++;
    if (gSecureCtx.security_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT2)
        en_count++;

    return (en_count >= 2);
}

/**
 * uimage signature check
 */
bool bootSecureUimageSigCheck(const void *header)
{
    prvSecureInit();
    if (!bootSecureBootEnable())
        return true;

    const image_header_t *hdr = (const image_header_t *)header;
    unsigned image_size = OSI_FROM_BE32(hdr->ih_size) + sizeof(image_header_t);
    return prvEmbeddedSigCheck(header, image_size + sizeof(struct sig));
}

/**
 * Signature check, signature is embedded at the end
 */
bool bootSecureEmbeddedSigCheck(const void *buf, uint32_t len)
{
    prvSecureInit();
    if (!bootSecureBootEnable())
        return true;

    return prvEmbeddedSigCheck(buf, len);
}

/**
 * Anti-clone encryption
 */
void bootAntiCloneEncryption(void *buf, uint32_t len)
{
    prvSecureInit();

    struct chip_unique_id rom_id;
    prvReadChipUniqueId(&rom_id, UNIQUE_ID_ANTI_CLONE);
    aesEncryptObj(buf, len, &rom_id);
}

/**
 * Anti-clone decryption (hidden API)
 */
void bootAntiCloneDecryption(void *buf, uint32_t len)
{
    prvSecureInit();

    halEfuseOpen();
    REGT_FIELD_CHANGE(hwp_efuseCtrl->efuse_sec_en,
                      REG_EFUSE_CTRL_EFUSE_SEC_EN_T,
                      double_bit_en_sec, 0);
    prvRomDecrypt(buf, len);
    halEfuseClose();
}
