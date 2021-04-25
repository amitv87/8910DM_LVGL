/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _IMAGE_HEADER_H_
#define _IMAGE_HEADER_H_

#include <stdint.h>

/*
 * Legacy format image header,
 * all data in network byte order (aka natural aka bigendian).
 */
#define IH_MAGIC 0x27051956 /* Image Magic Number */
#define IH_NMLEN 32         /* Image Name Length */

typedef struct image_header
{
    uint32_t ih_magic;         /* Image Header Magic Number */
    uint32_t ih_hcrc;          /* Image Header CRC Checksum */
    uint32_t ih_time;          /* Image Creation Timestamp  */
    uint32_t ih_size;          /* Image Data Size           */
    uint32_t ih_load;          /* Data   Load  Address      */
    uint32_t ih_ep;            /* Entry Point Address       */
    uint32_t ih_dcrc;          /* Image Data CRC Checksum   */
    uint8_t ih_os;             /* Operating System          */
    uint8_t ih_arch;           /* CPU architecture          */
    uint8_t ih_type;           /* Image Type                */
    uint8_t ih_comp;           /* Compression Type          */
    uint8_t ih_name[IH_NMLEN]; /* Image Name                */
} image_header_t;

#endif
