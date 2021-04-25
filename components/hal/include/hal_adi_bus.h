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

#ifndef _HAL_ADI_H_
#define _HAL_ADI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * mask indicates write rather than read-modify-write
 */
#define HAL_ADI_BUS_OVERWITE(value) (0xffffffff), (value)

/**
 * indicates end of variadic variables of \p halAdiBusBatchChange
 */
#define HAL_ADI_BUS_CHANGE_END (0)

/**
 * @brief initialize ADI bus
 */
void halAdiBusInit(void);

/**
 * @brief read register through ADI bus
 *
 * @param reg   register physical address
 * @return  read value
 */
uint32_t halAdiBusRead(volatile uint32_t *reg);

/**
 * @brief read register through ADI bus
 *
 * @param reg   register physical address
 * @param value value to be written
 */
void halAdiBusWrite(volatile uint32_t *reg, uint32_t value);

/**
 * @brief change register
 *
 * When \p write_mask is not all 1s, the operation is read-modify-write.
 *
 *      new_value = (old_value & ~writ_mask) | (write_value & write_mask)
 *
 * @param reg   register physical address
 * @param write_mask    mask for change, 1 for change, 0 for no change
 * @param write_value   value to be changed
 * @return the value to be written
 */
uint32_t halAdiBusChange(volatile uint32_t *reg, uint32_t write_mask, uint32_t write_value);

/**
 * @brief batch change registers
 *
 * Batch change several registers through ADI bus. When \p write_mask is
 * not all 1s, the operation is read-modify-write.
 *
 *      new_value = (old_value & ~writ_mask) | (write_value & write_mask)
 *
 * \p reg value of 0 indicates the end of variadic variables.
 *
 * @param reg   register physical address
 * @param write_mask    mask for change, 1 for change, 0 for no change
 * @param write_value   value to be changed
 */
void halAdiBusBatchChange(volatile uint32_t *reg, uint32_t write_mask, uint32_t write_value, ...);

#ifdef __cplusplus
}
#endif
#endif
