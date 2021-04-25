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

#include "calclib/simage.h"
#include "calclib/crc32.h"
#include "osi_api.h"
#include <string.h>
#include <assert.h>

static_assert(sizeof(simageHeader_t) == SIMAGE_HEADER_SIZE, "simage header mismatch");
static_assert(OSI_IS_ALIGNED(sizeof(simageKeyCert_t), SIMAGE_ALIGNMENT), "simageKeyCert_t is not aligned");
static_assert(OSI_IS_ALIGNED(sizeof(simageDebugCert_t), SIMAGE_ALIGNMENT), "simageDebugCert_t is not aligned");
static_assert(OSI_IS_ALIGNED(sizeof(simageDevelCert_t), SIMAGE_ALIGNMENT), "simageDevelCert_t is not aligned");

#define ALIGNED_END(offset, size) ((offset) + OSI_ALIGN_UP((size), SIMAGE_ALIGNMENT))

static void prvCalcChecksum(uint32_t type, osiBits256_t *calc, const void *data, unsigned size)
{
    memset(calc, 0, 32);

    if (type == SIMAGE_HEADER_CHECK_CRC)
    {
        unsigned crc = crc32Calc(data, size);
        memcpy(calc, &crc, 4);
    }
    else if (type == SIMAGE_HEADER_CHECK_SHA256)
    {
        ; // TODO
    }
}

static bool prvVerifyChecksum(uint32_t type, const osiBits256_t *check, const void *data, unsigned size)
{
    if (size == 0) // not to check checksum for empty
        return true;

    osiBits256_t calc;
    prvCalcChecksum(type, &calc, data, size);
    return memcmp(&calc, check, 32) == 0;
}

bool simageHeaderValid(const simageHeader_t *header, unsigned max_size)
{
    if (header == NULL)
        return false;

    unsigned check_type = header->header_flags & SIMAGE_HEADER_CHECK_MASK;

    if (header->magic != SIMAGE_HEADER_MAGIC ||
        header->header_size != SIMAGE_HEADER_SIZE ||
        header->format_version != SIMAGE_FORMAT_VERSION_V1)
        return false;

    if (header->image_size > max_size)
        return false;

    if (check_type != SIMAGE_HEADER_CHECK_CRC &&
        check_type != SIMAGE_HEADER_CHECK_SHA256)
        return false;

#define CHECK_LAYOUT(offset, size)                        \
    if (size > 0)                                         \
    {                                                     \
        if (offset != expected)                           \
            return false;                                 \
        expected += OSI_ALIGN_UP(size, SIMAGE_ALIGNMENT); \
    }

    // the layout is fixed
    unsigned expected = header->header_size;
    CHECK_LAYOUT(header->data_offset, header->data_size);
    CHECK_LAYOUT(header->priv_data_offset, header->priv_data_size);
    CHECK_LAYOUT(header->keycert_offset, header->keycert_size);
    CHECK_LAYOUT(header->privcert_offset, header->privcert_size);
    CHECK_LAYOUT(header->debugcert_offset, header->debugcert_size);
    CHECK_LAYOUT(header->develcert_offset, header->develcert_size);

    // no other contents
    if (expected != header->image_size)
        return false;

    const char *header_data = (const char *)header + SIMAGE_HEADER_CHECKSUM_OFFSET;
    unsigned header_data_len = header->header_size - SIMAGE_HEADER_CHECKSUM_OFFSET;
    if (!prvVerifyChecksum(check_type, &header->header_checksum, header_data, header_data_len))
        return false;

    return true;
}

bool simagePayloadValid(const simageHeader_t *header)
{
    const char *payload = (const char *)header + header->header_size;
    unsigned payload_len = header->image_size - header->header_size;
    unsigned check_type = header->header_flags & SIMAGE_HEADER_CHECK_MASK;
    if (!prvVerifyChecksum(check_type, &header->payload_checksum, payload, payload_len))
        return false;

    return true;
}

bool simageValid(const simageHeader_t *header, unsigned max_size)
{
    return simageHeaderValid(header, max_size) && simagePayloadValid(header);
}

bool simageSignCheck(const simageHeader_t *header)
{
    return true;
}

OSI_NO_RETURN void simageJumpEntry(const simageHeader_t *header, unsigned loadpar)
{
    simageEntry_t entry = (simageEntry_t)header->data_entry;
    osiICacheSyncAll();
    entry(loadpar);

    // The jump entry shouldn't return. Even it returns, we will stop here.
    OSI_DEAD_LOOP;
}
