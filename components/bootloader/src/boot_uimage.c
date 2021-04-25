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

#include "boot_uimage.h"
#include "calclib/crc32.h"
#include <string.h>

#define IH_MAGIC 0x27051956 /* Image Magic Number */
#define IH_NMLEN 32         /* Image Name Length */

/*
 * Legacy format image header,
 * all data in network byte order (aka natural aka bigendian).
 */
typedef struct image_header
{
    uint32_t _ih_magic;        /* Image Header Magic Number */
    uint32_t _ih_hcrc;         /* Image Header CRC Checksum */
    uint32_t _ih_time;         /* Image Creation Timestamp  */
    uint32_t _ih_size;         /* Image Data Size           */
    uint32_t _ih_load;         /* Data Load  Address        */
    uint32_t _ih_ep;           /* Entry Point Address       */
    uint32_t _ih_dcrc;         /* Image Data CRC Checksum   */
    uint8_t ih_os;             /* Operating System          */
    uint8_t ih_arch;           /* CPU architecture          */
    uint8_t ih_type;           /* Image Type                */
    uint8_t ih_comp;           /* Compression Type          */
    uint8_t ih_name[IH_NMLEN]; /* Image Name                */
} image_header_t;

static inline uint32_t prvUimageMagic(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_magic); }
static inline uint32_t prvUimageHcrc(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_hcrc); }
static inline uint32_t prvUimageTime(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_time); }
static inline uint32_t prvUimageSize(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_size); }
static inline uint32_t prvUimageLoad(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_load); }
static inline uint32_t prvUimageEntry(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_ep); }
static inline uint32_t prvUimageDcrc(const image_header_t *h) { return OSI_FROM_BE32(h->_ih_dcrc); }

static inline void prvUimageSetHcrc(image_header_t *h, uint32_t hcrc) { h->_ih_hcrc = OSI_TO_BE32(hcrc); }

bool bootUimageCheck(const void *buf, unsigned len)
{
    if (buf == NULL || len < sizeof(image_header_t) || !OSI_IS_ALIGNED((uintptr_t)buf, 4))
        return false;

    const image_header_t *hdr = (const image_header_t *)buf;
    if (prvUimageMagic(hdr) != IH_MAGIC)
        return false;

    // hcrc is calculated by setting ih_crc as 0
    image_header_t hdr_for_crc;
    memcpy(&hdr_for_crc, hdr, sizeof(image_header_t));
    prvUimageSetHcrc(&hdr_for_crc, 0);
    if (crc32Calc(&hdr_for_crc, sizeof(image_header_t)) != prvUimageHcrc(hdr))
        return false;

    const void *data = &hdr[1];
    uint32_t data_size = prvUimageSize(hdr);
    if (sizeof(image_header_t) + data_size > len)
        return false;

    if (crc32Calc(data, data_size) != prvUimageDcrc(hdr))
        return false;

    return true;
}

bool bootUimageQuickCheck(const void *buf, unsigned len)
{
    if (buf == NULL || len < sizeof(image_header_t) || !OSI_IS_ALIGNED((uintptr_t)buf, 4))
        return false;

    const image_header_t *hdr = (const image_header_t *)buf;
    if (prvUimageMagic(hdr) != IH_MAGIC)
        return false;

    // hcrc is calculated by setting ih_crc as 0
    image_header_t hdr_for_crc;
    memcpy(&hdr_for_crc, hdr, sizeof(image_header_t));
    prvUimageSetHcrc(&hdr_for_crc, 0);
    if (crc32Calc(&hdr_for_crc, sizeof(image_header_t)) != prvUimageHcrc(hdr))
        return false;

    uint32_t data_size = prvUimageSize(hdr);
    if (sizeof(image_header_t) + data_size > len)
        return false;

    return true;
}

uint32_t bootUimageEntry(const void *buf)
{
    if (!OSI_IS_ALIGNED((uintptr_t)buf, 4))
        return 0;

    const image_header_t *hdr = (const image_header_t *)buf;
    return prvUimageEntry(hdr);
}

unsigned bootUimageSize(const void *buf)
{
    if (!OSI_IS_ALIGNED((uintptr_t)buf, 4))
        return sizeof(image_header_t);

    const image_header_t *hdr = (const image_header_t *)buf;
    return prvUimageSize(hdr) + sizeof(image_header_t);
}
