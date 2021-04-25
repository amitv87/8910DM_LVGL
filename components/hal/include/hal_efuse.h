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

#ifndef _HAL_EFUSE_H_
#define _HAL_EFUSE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_SOC_8910)
#include "8910/hal_efuse_def.h"
#elif defined(CONFIG_SOC_8811)
#include "8811/hal_efuse_def.h"
#endif

/**
 * @brief open the efuse module
 *
 * It should be called before reading or writing efuse.
 */
void halEfuseOpen(void);

/**
 * @brief close the efuse module
 *
 * It should be called when finsh reading or writing efuse.
 */
void halEfuseClose(void);

/**
 * @brief read the efuse in single bit.
 *
 * @param block_index The index of efuse block. The ranges is from 0 to 81.
 * @param val The pointer of the data reading form the block.
 *
 * @return
 *      - true          success
 *      - false         fail
 */
bool halEfuseRead(uint32_t block_index, uint32_t *val);

/**
 * @brief write the efuse in single bit.
 *
 * @param block_index   The index of efuse block. The ranges is from 0 to 81.
 * @param val           The value will write to the efuse.
 *
 * @return
 *      - true          success
 *      - false         fail
 */
bool halEfuseWrite(uint32_t block_index, uint32_t val);

/**
 * @brief read the efuse in double bit
 *
 * @param block_index   the double bit block index
 * @param val           the value point for read
 * @return
 *      - true on sccess else fail
 */
bool halEfuseDoubleRead(uint32_t block_index, uint32_t *val);

/**
 * @brief write the efuse in double bit
 *
 * @param block_index   the double bit block index
 * @param val           the value to be writen
 * @return
 *      - true on success else fail
 */
bool halEfuseDoubleWrite(uint32_t block_index, uint32_t val);

#ifdef __cplusplus
}
#endif
#endif /* _DRV_EFUSE_H_ */
