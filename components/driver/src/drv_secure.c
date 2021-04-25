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

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_secure.h"
#include <string.h>
#include <stdlib.h>
#include "hal_chip.h"
#include "hal_config.h"
#include "hwregs.h"
#include "hal_mmu.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_adi_bus.h"
#include "drv_efuse.h"
#include "drv_aes.h"
#include "drv_spi_flash.h"
#include "osi_api.h"
#include <machine/endian.h>
#include "cmsis_core.h"
#include "vfs.h"
#include <stdio.h>

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

#define ROM_API_SIGNATURE_OK 0
#define ROM_API_SIGNATURE_FAIL -1

#define ROM_API_SECURITY_ENABLED 0
#define ROM_API_SECURITY_DISABLED 1
#define ROM_API_INVALID_KEYINDEX 2
#define ROM_API_INVALID_VENDOR_ID 3
#define ROM_API_SECURITY_UNAVAILABLE 4

#define SECTOR_SIZE_4K (0x1000)
#define ENCRYPT_OFF 0x1000
#define ENCRYPT_LEN 0x400
#define BOOT_PUBKEY_FLASH_OFFSET (0xbcc0)
#define PUBLICKEYWORDS 8

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

struct chip_security_context;
struct chip_id;
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
    void *decrypt_aes_image_fn;
};

struct drvSigChecker
{
    struct sig sig;
    struct pubkey pubkey;
    struct rom_hash hash;
    struct rom_hash_context hash_ctx;
};

typedef struct
{
    struct ROM_crypto_api *romapi;
    unsigned rom_user_count;
} drvSecureContext_t;

static drvSecureContext_t gSecureCtx;

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
 * Enable ROM access, take care MMU and romapi pointer
 */
static void prvEnableRomAccess(void)
{
    unsigned critical = osiEnterCritical();
    if (gSecureCtx.rom_user_count == 0)
        halMmuSetNullProtection(false);

    gSecureCtx.rom_user_count++;
    if (gSecureCtx.romapi == NULL)
        gSecureCtx.romapi = *(struct ROM_crypto_api **)ROMAPI_BASE_POINTER;
    osiExitCritical(critical);
}

/**
 * Disable ROM access
 */
static void prvDisableRomAccess(void)
{
    unsigned critical = osiEnterCritical();
    if (gSecureCtx.rom_user_count <= 1)
    {
        halMmuSetNullProtection(true);
        gSecureCtx.rom_user_count = 0;
    }
    else
    {
        gSecureCtx.rom_user_count--;
    }
    osiExitCritical(critical);
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
    drvEfuseBatchRead(true, RDA_PUBKEY0_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[0],
                      RDA_PUBKEY1_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[1],
                      RDA_PUBKEY2_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[2],
                      RDA_PUBKEY3_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[3],
                      RDA_PUBKEY4_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[4],
                      RDA_PUBKEY5_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[5],
                      RDA_PUBKEY6_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[6],
                      RDA_PUBKEY7_EFUSE_DOUBLE_BLOCK_INDEX, &efuse_pubkey[7],
                      DRV_EFUSE_ACCESS_END);

    for (int n = 0; n < 8; n++)
        efuse_pubkey[n] = OSI_FROM_BE32(efuse_pubkey[n]);

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

    drvEfuseBatchRead(true, RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_LOW_INDEX, &unique.lo,
                      RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_HIGH_INDEX, &unique.hi,
                      DRV_EFUSE_ACCESS_END);

    if (kind == UNIQUE_ID_ANTI_CLONE)
        unique.lo += RDA_ANTI_CLONE_MAGIC;
    else if (kind == UNIQUE_ID_RD_CERT)
        unique.lo += RDA_RD_CERT_MAGIC;

    struct rom_hash_context ctx;
    prvEnableRomAccess();
    prvRomHashInit(&ctx);
    prvRomHashUpdate(&ctx, &unique, sizeof(unique));
    prvRomHashFinal(&ctx, id, 32);
    prvDisableRomAccess();
}

/**
 * Anti-clone encryption
 */
static void prvAntiCloneEncryption(void *buf, uint32_t len)
{
    struct chip_unique_id rom_id;

    prvReadChipUniqueId(&rom_id, UNIQUE_ID_ANTI_CLONE);
    aesEncryptObj(buf, len, &rom_id);
}

/**
 * Flash read-modify-write, for anti-clone encryption
 */
static void prvAntiCloneFlash(void)
{
    uint8_t *pbuf = malloc(SECTOR_SIZE_4K);

    drvSpiFlash_t *flash = drvSpiFlashOpen(DRV_NAME_SPI_FLASH);

    memcpy(pbuf, (void *)(CONFIG_NOR_PHY_ADDRESS + ENCRYPT_OFF), SECTOR_SIZE_4K);

    prvAntiCloneEncryption(pbuf, ENCRYPT_LEN);

    drvSpiFlashClearRangeWriteProhibit(flash, HAL_FLASH_OFFSET(CONFIG_BOOT_FLASH_ADDRESS),
                                       HAL_FLASH_OFFSET(CONFIG_BOOT_FLASH_ADDRESS) + CONFIG_BOOT_FLASH_SIZE);

    // Flash read-modify-write is not power failure safe. Critical
    // section make the period as short as possible.
    uint32_t critical = osiEnterCritical();
    drvSpiFlashErase(flash, ENCRYPT_OFF, SECTOR_SIZE_4K);
    drvSpiFlashWrite(flash, ENCRYPT_OFF, pbuf, SECTOR_SIZE_4K);
    osiExitCritical(critical);

    free(pbuf);
}

/**
 * Whether secure boot is enabled, by read efuse
 */
bool drvSecureBootEnable(void)
{
    uint32_t sec_cfg;
    drvEfuseRead(true, RDA_EFUSE_DOUBLE_BLOCK_SECURITY_CFG_INDEX, &sec_cfg);
    OSI_LOGD(0, "efuse sec_cfg 0x%08x", sec_cfg);

    int en_count = 0;
    if (sec_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT0)
        en_count++;
    if (sec_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT1)
        en_count++;
    if (sec_cfg & RDA_SE_CFG_SECURITY_ENABLE_BIT2)
        en_count++;

    return (en_count >= 2);
}

/**
 * Write secure boot flag.
 */
bool drvSecureWriteSecureFlags(void)
{
    // When secureboot is already enabled, it is not needed to modify the
    // efuse bits, and bootloader should be already anti-clone encrypted.
    if (drvSecureBootEnable())
        return true;

    // Secure boot enable is NOT power failure safe. It will be safer to
    // disable interrupt for the whole, though it flash operation will
    // take time.
    unsigned critical = osiEnterCritical();

    prvAntiCloneFlash();

    // Write public key, which is read from bootloader, just before signature
    unsigned *pubkey_words = (unsigned *)(CONFIG_NOR_PHY_ADDRESS + BOOT_PUBKEY_FLASH_OFFSET);

    // efuse can't change 1 to 0
    uint32_t sec_cfg = RDA_SE_CFG_SECURITY_ENABLE_BIT0 |
                       RDA_SE_CFG_SECURITY_ENABLE_BIT1 |
                       RDA_SE_CFG_SECURITY_ENABLE_BIT2;

    drvEfuseBatchWrite(true, RDA_PUBKEY0_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[0]),
                       RDA_PUBKEY1_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[1]),
                       RDA_PUBKEY2_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[2]),
                       RDA_PUBKEY3_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[3]),
                       RDA_PUBKEY4_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[4]),
                       RDA_PUBKEY5_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[5]),
                       RDA_PUBKEY6_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[6]),
                       RDA_PUBKEY7_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[7]),
                       RDA_EFUSE_DOUBLE_BLOCK_SECURITY_CFG_INDEX, sec_cfg,
                       DRV_EFUSE_ACCESS_END);

    osiExitCritical(critical);
    return true;
}

// Compatiblity: these functions are renamed.
OSI_DECL_STRONG_ALIAS(drvSecureBootEnable, bool ReadSecurityFlag(void));
OSI_DECL_STRONG_ALIAS(drvSecureWriteSecureFlags, bool writeSecuriyFlag(void));

/**
 * Fake enable secure boot, not write efuse physically (hidden API)
 */
void drvFakeEnableSecure(void)
{
    uint32_t sec_cfg = RDA_SE_CFG_SECURITY_ENABLE_BIT0 |
                       RDA_SE_CFG_SECURITY_ENABLE_BIT1 |
                       RDA_SE_CFG_SECURITY_ENABLE_BIT2;

    unsigned *pubkey_words = (unsigned *)(CONFIG_NOR_PHY_ADDRESS + BOOT_PUBKEY_FLASH_OFFSET);

    drvEfuseFakeWrite(true, RDA_PUBKEY0_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[0]));
    drvEfuseFakeWrite(true, RDA_PUBKEY1_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[1]));
    drvEfuseFakeWrite(true, RDA_PUBKEY2_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[2]));
    drvEfuseFakeWrite(true, RDA_PUBKEY3_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[3]));
    drvEfuseFakeWrite(true, RDA_PUBKEY4_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[4]));
    drvEfuseFakeWrite(true, RDA_PUBKEY5_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[5]));
    drvEfuseFakeWrite(true, RDA_PUBKEY6_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[6]));
    drvEfuseFakeWrite(true, RDA_PUBKEY7_EFUSE_DOUBLE_BLOCK_INDEX, OSI_TO_BE32(pubkey_words[7]));
    drvEfuseFakeWrite(true, RDA_EFUSE_DOUBLE_BLOCK_SECURITY_CFG_INDEX, sec_cfg);
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
 * Create progressive signature checker
 */
drvSigChecker_t *drvSigCheckerCreate(const void *sig)
{
    if (sig == NULL)
        return NULL;

    drvSigChecker_t *checker = calloc(1, sizeof(drvSigChecker_t));
    if (checker == NULL)
        return NULL;

    memcpy(&checker->sig, sig, sizeof(struct sig));
    prvReadEfusePubkey(&checker->pubkey);

    prvEnableRomAccess();
    prvRomHashInit(&checker->hash_ctx);
    prvRomHashUpdate(&checker->hash_ctx, checker->sig.sig, 32);
    prvRomHashUpdate(&checker->hash_ctx, checker->pubkey.pubkey, 32);
    prvDisableRomAccess();
    return checker;
}

/**
 * Delete the progressive signature checker
 */
void drvSigCheckerDelete(drvSigChecker_t *checker)
{
    free(checker);
}

/**
 * Update message data of progressive signature checker
 */
void drvSigCheckerUpdate(drvSigChecker_t *checker, const void *data, uint32_t size)
{
    if (checker == NULL || data == NULL || size == 0)
        return;

    prvEnableRomAccess();
    prvRomHashUpdate(&checker->hash_ctx, data, size);
    prvDisableRomAccess();
}

/**
 * Finalize and signature check
 */
bool drvSigCheckerFinalCheck(drvSigChecker_t *checker)
{
    prvEnableRomAccess();
    prvRomHashFinal(&checker->hash_ctx, &checker->hash, 32);
    bool result = prvHashSigCheck(&checker->hash, &checker->sig, &checker->pubkey);
    prvDisableRomAccess();
    return result;
}

/**
 * Signature check for data with signature at the end
 */
bool drvSecureEmbedSigCheck(const void *data, uint32_t size)
{
    if (data == NULL || size < DRV_SECURE_BOOT_SIG_SIZE_8910)
        return false;

    struct pubkey pubkey;
    prvReadEfusePubkey(&pubkey);

    prvEnableRomAccess();
    size -= DRV_SECURE_BOOT_SIG_SIZE_8910; // size exclude signature
    const void *sig = (const char *)data + size;
    bool result = prvMessageSigCheck(data, size, sig, &pubkey);
    prvDisableRomAccess();
    return result;
}

/**
 * Signature check for data with separated signature
 */
bool drvSecureSigCheck(const void *data, uint32_t data_size, const void *sig)
{
    if (data == NULL || sig == NULL)
        return false;

    struct pubkey pubkey;
    prvReadEfusePubkey(&pubkey);

    prvEnableRomAccess();
    bool result = prvMessageSigCheck(data, data_size, sig, &pubkey);
    prvDisableRomAccess();
    return result;
}

/**
 * Signature check for file with signature at the end
 */
bool drvSecureFileEmbedSigCheck(const char *fname)
{
    bool result = false;
    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return false;

    long fsize = vfs_lseek(fd, 0, SEEK_END);
    if (fsize < DRV_SECURE_BOOT_SIG_SIZE_8910)
        return false;

    void *buf = malloc(DRV_SECURE_BOOT_SIG_SIZE_8910);
    if (buf == NULL)
        goto close_fd;

    vfs_lseek(fd, fsize - DRV_SECURE_BOOT_SIG_SIZE_8910, SEEK_SET);

    if (vfs_read(fd, buf, DRV_SECURE_BOOT_SIG_SIZE_8910) != DRV_SECURE_BOOT_SIG_SIZE_8910)
        goto free_buf;

    drvSigChecker_t *checker = drvSigCheckerCreate(buf);
    if (checker == NULL)
        goto free_buf;

    vfs_lseek(fd, 0, SEEK_SET);
    int remained = fsize - DRV_SECURE_BOOT_SIG_SIZE_8910;
    while (remained > 0)
    {
        int rbytes = OSI_MIN(int, remained, DRV_SECURE_BOOT_SIG_SIZE_8910);
        if (vfs_read(fd, buf, rbytes) != rbytes)
            goto free_checker;

        remained -= rbytes;
        drvSigCheckerUpdate(checker, buf, rbytes);
    }

    if (!drvSigCheckerFinalCheck(checker))
        goto free_checker;

    result = true;

free_checker:
    drvSigCheckerDelete(checker);
free_buf:
    free(buf);
close_fd:
    vfs_close(fd);
    return result;
}
