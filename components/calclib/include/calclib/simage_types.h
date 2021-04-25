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

#ifndef _SIMAGE_TYPES_H_
#define _SIMAGE_TYPES_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#define SIMAGE_HEADER_MAGIC (0x52484953) // SIHR in little endian
#define SIMAGE_FORMAT_VERSION_V1 (1)     // vesion 1 for this definition
#define SIMAGE_HEADER_CHECKSUM_OFFSET (sizeof(uint32_t) + sizeof(osiBytes32_t))
#define SIMAGE_HEADER_SIZE (256)
#define SIMAGE_HEADER_NAME_LEN (64)
#define SIMAGE_HEADER_CHECK_MASK (0xf)
#define SIMAGE_HEADER_CHECK_CRC (0)
#define SIMAGE_HEADER_CHECK_SHA256 (1)
#define SIMAGE_ALIGNMENT (64)
#define SIMAGE_CERTTYPE_CONTENT (0)
#define SIMAGE_CERTTYPE_KEY (1)
#define SIMAGE_CERTTYPE_DEBUG (2)
#define SIMAGE_CERTTYPE_DEVELOP (3)
#define SIMAGE_PKATYPE_ECC (0)
#define SIMAGE_PKATYPE_RSA (1)
#define SIMAGE_PKATYPE_RSA_PSS (2)
#define SIMAGE_SIGN_DATA_OFFSET (sizeof(simagePubkey_t) + sizeof(simageSignature_t))

#define SIMAGE_ECC_CURVE_secp160k1 (0)
#define SIMAGE_ECC_CURVE_secp160r1 (1)
#define SIMAGE_ECC_CURVE_secp160r2 (2)
#define SIMAGE_ECC_CURVE_secp192k1 (3)
#define SIMAGE_ECC_CURVE_secp192r1 (4)
#define SIMAGE_ECC_CURVE_secp224k1 (5)
#define SIMAGE_ECC_CURVE_secp224r1 (6)
#define SIMAGE_ECC_CURVE_secp256k1 (7)
#define SIMAGE_ECC_CURVE_secp256r1 (8)
#define SIMAGE_ECC_CURVE_secp384r1 (9)
#define SIMAGE_ECC_CURVE_secp521r1 (10)

/**
 * \brief function prototype of signed image data entry
 */
typedef void (*simageEntry_t)(uint32_t param);

/**
 * \brief ecc public key
 */
typedef struct
{
    osiBytes80_t x; ///< ecp x, real bits depends on curve
    osiBytes80_t y; ///< ecp y, real bits depends on curve
    uint32_t curve; ///< curve type, \p SIMAGE_ECC_CURVE_.
} simageEccPubkey_t;

/**
 * \brief ecc signature
 */
typedef struct
{
    osiBytes80_t r; ///< signature r, real bits depends on curve
    osiBytes80_t s; ///< signature s, real bits depends on curve
} simageEccSignature_t;

/**
 * \brief RSA public key
 */
typedef struct
{
    uint32_t n_bytes; ///< bytes of N
    uint32_t e;       ///< E, 32bits at most
    osiBits4096_t n;  ///< N, 4096 bits at most
} simageRsaPubkey_t;

/**
 * \brief RSA signature
 */
typedef struct
{
    uint32_t bytes;    ///< number of bytes, should be the same of N
    osiBits4096_t sig; ///< signature data
} simageRsaSignature_t;

/**
 * \brief union of RSA and ECC public key
 */
typedef union {
    simageRsaPubkey_t rsa; ///< RSA public key
    simageEccPubkey_t ecc; ///< ECC public key
} simagePubkey_t;

/**
 * \brief union of RSA and ECC signature
 */
typedef union {
    simageRsaSignature_t rsa; ///< RSA signature
    simageEccSignature_t ecc; ///< ECC signature
} simageSignature_t;

/**
 * \brief Cert common header
 *
 * The header of certs is the public key and signature. The signature
 * is calculated for the hash (SHA256) of all data followed:
 * <tt>header->cert_size - SIMAGE_SIGN_DATA_OFFSET</tt>.
 */
typedef struct
{
    /**
     * Public key.
     */
    simagePubkey_t pubkey;
    /**
     * Signature.
     */
    simageSignature_t sig;
    /**
     * Cert type, refer to \p SIMAGE_CERTTYPE_.
     */
    uint32_t cert_type;
    /**
     * PKA type, refer to \p SIMAGE_PKATYPE_.
     */
    uint32_t pk_type;
    /**
     * Total cert type. For each type of cert, the total size if fixed.
     */
    uint32_t cert_size;
} simageCertHeader_t;

/**
 * \brief key cert
 */
typedef struct
{
    /**
     * Cert header
     */
    simageCertHeader_t header;
    /**
     * SHA256 hash of data, to verify data.
     */
    osiBits256_t hash_data;
    /**
     * SHA256 hash of next public key, for trust chain. In bootloader,
     * it is the hash of primary debug cert public key.
     */
    osiBits256_t hash_next_key;
    /**
     * Security version, for anti-rollback.
     */
    uint32_t security_version;
    /**
     * Data size. It should match \p data_size in image header.
     */
    uint32_t data_size;
    /**
     * Data flags. It should match \p data_flags in image header.
     */
    uint32_t data_flags;
    /**
     * Data load address. It should match \p data_load in image header.
     */
    uint32_t data_load;
    /**
     * Data load address [63:32]. It should match \p data_load_hi in image header.
     */
    uint32_t data_load_hi;
    /**
     * Data entry address. It should match \p data_entry in image header.
     */
    uint32_t data_entry;
    /**
     * Data entry address [63:32]. It should match \p data_entry_hi in image header.
     */
    uint32_t data_entry_hi;
    /**
     * Padding to make cert 64B aligned.
     */
    uint32_t zero[3];
} simageKeyCert_t;

/**
 * \brief debug cert
 */
typedef struct
{
    /**
     * Cert header
     */
    simageCertHeader_t header;
    /**
     * SHA256 hash of developer debug cert public key, for trust chain.
     */
    osiBits256_t hash_devel_key;
    /**
     * Debug enable mask.
     */
    uint32_t debug_mask;
    /**
     * Padding to make cert 64B aligned.
     */
    uint32_t zero[17];
} simageDebugCert_t;

/**
 * \brief developer cert
 */
typedef struct
{
    /**
     * Cert header
     */
    simageCertHeader_t header;
    /**
     * SOC id. Developer debug cert is for specified chip.
     */
    osiBits256_t soc_id;
    /**
     * Debug enable mask.
     */
    uint32_t debug_mask;
    /**
     * Padding to make cert 64B aligned.
     */
    uint32_t zero[17];
} simageDevelCert_t;

/**
 * \brief signed image header
 *
 * The structure of signed image is:
 * - header
 * - data
 * - private data
 * - key cert
 * - private data cert
 * - debug primary cert
 * - debug devel cert
 *
 * Only header is mendatory, all others are optional. The cert names are for
 * bootloader. For other images, most likely, debug certs won't appear.
 */
typedef struct
{
    /**
     * Magic number: SIHR
     */
    uint32_t magic;
    /**
     * Image header checksum. Checksum calculation offset is
     * \p SIMAGE_HEADER_CHECKSUM_OFFSET.
     */
    osiBytes32_t header_checksum;
    /**
     * Total image size, including header and payload
     */
    uint32_t image_size;
    /**
     * Image name. The terminate NUL should be included
     */
    uint8_t image_name[64];
    /**
     * Image format version.
     */
    uint16_t format_version;
    /**
     * Version patch level.
     */
    uint16_t ver_patch;
    /**
     * Minor version
     */
    uint16_t ver_minor;
    /**
     * Major version
     */
    uint16_t ver_major;
    /**
     * Revision information. Usually, it is SVN revision number or
     * the first 4 bytes of git hash.
     */
    uint32_t revision;
    /**
     * Platform ID, 0 for unknown
     */
    uint32_t platform;
    /**
     * Creation timestamp, seconds since epoch
     */
    uint32_t timestamp;
    /**
     * Header size. Currently, 256B is used, other sizes are reserved
     * for future.
     */
    uint32_t header_size;
    /**
     * Header flags. Currently, [3:0] is used to indicate checksum
     * method:
     * - 0: CRC32
     * - 1: SHA256
     */
    uint32_t header_flags;
    /**
     * Payload checksum. Payload is the image data excluding header,
     * and including all paddings. When there are no payload, filling
     * with zeros.
     */
    osiBytes32_t payload_checksum;
    /**
     * Data offset in the image, from the beginning of image.
     */
    uint32_t data_offset;
    /**
     * Data size, not including padding. 0 for not exist.
     */
    uint32_t data_size;
    /**
     * Data flags, reserved for future (for example indicating whether
     * the data is compressed).
     */
    uint32_t data_flags;
    /**
     * Data load address.
     */
    uint32_t data_load;
    /**
     * Data load address [63:32], only needed for 64bits address
     */
    uint32_t data_load_hi;
    /**
     * Data entry address.
     */
    uint32_t data_entry;
    /**
     * Data entry address [63:32], only needed for 64bits address
     */
    uint32_t data_entry_hi;
    /**
     * Private data offset in the image, from the beginning of image.
     */
    uint32_t priv_data_offset;
    /**
     * Private data size, not including padding. 0 for not exist.
     */
    uint32_t priv_data_size;
    /**
     * Private data flags, reserved for future.
     */
    uint32_t priv_data_flags;
    /**
     * Key cert offset in the image, from the beginning of image.
     */
    uint32_t keycert_offset;
    /**
     * Key cert size, 0 for not exist.
     */
    uint32_t keycert_size;
    /**
     * Private data cert offset in the image, from the beginning of image.
     */
    uint32_t privcert_offset;
    /**
     * Private data cert size, 0 for not exist.
     */
    uint32_t privcert_size;
    /**
     * Private debug cert offset in the image, from the beginning of image
     */
    uint32_t debugcert_offset;
    /**
     * Private debug cert size, 0 for not exist.
     */
    uint32_t debugcert_size;
    /**
     * Developer debug cert offset in the image, from the beginning of image
     */
    uint32_t develcert_offset;
    /**
     * Developer debug cert size, 0 for not exist.
     */
    uint32_t develcert_size;
    /**
     * Zero padding to 256 bytes.
     */
    uint32_t zero[5];
} simageHeader_t;

OSI_EXTERN_C_END
#endif
