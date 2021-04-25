/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "bt_nv.h"

#define SIZE_OF_BDADDR 6 ///< the number of bytes of the address

typedef struct _BT_ADDRESS
{
    uint8_t addr[SIZE_OF_BDADDR];
} BT_ADDRESS;

typedef struct bt_nv_param_tag
{
    BT_ADDRESS bd_addr;
    uint8_t local_name[32];
    uint16_t xtal_dac;
} bt_nv_param_t;

bool btConfigNvCheck(uint8_t *odata, int osize, uint32_t *nsize)
{
    if (nsize)
        *nsize = sizeof(bt_nv_param_t);
    // check bt_config for size only
    if (osize == (int)sizeof(bt_nv_param_t))
        return true;

    return false;
}

bool btConfigNvMigration(uint8_t *buf, int size, uint8_t *ex_buf, int ex_size)
{
    if (NULL == ex_buf || ex_size < 0)
        return false;

    memset(ex_buf, 0, ex_size);
    if (buf != NULL && size > 0)
        memcpy(ex_buf, buf, size);
    return true;
}