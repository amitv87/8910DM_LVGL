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

#ifndef _BT_NV_H_
#define _BT_NV_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief to check btconfig.bin data
 *
 * \param odata     the point of old bt_config buf
 * \param osize     the old bt_config data size
 * \param nsize     the checked new bt_config data size
 * \return
 *          -true check ok
 *          -false check fail
 */
bool btConfigNvCheck(uint8_t *odata, int osize, uint32_t *nsize);

/**
 * \brief to migrate btconfig.bin
 * if buf is NULL or size < 0, the expected buf will fill in with 0
 *
 * \param buf       for current bt_config buf
 * \param size      for current bt_config size
 * \param ex_buf    for expected bt_config buf
 * \param ex_size   for expected bt_config buf size
 * \return
 *          -true migrate ok
 *          -false migrate fail
 */
bool btConfigNvMigration(uint8_t *buf, int size, uint8_t *ex_buf, int ex_size);

#ifdef __cplusplus
}
#endif
#endif
