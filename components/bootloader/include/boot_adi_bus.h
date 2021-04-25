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

#ifndef _BOOT_ADI_BUS_H_
#define _BOOT_ADI_BUS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

void bootAdiBusInit(void);
uint32_t bootAdiBusRead(volatile uint32_t *reg);
void bootAdiBusWrite(volatile uint32_t *reg, uint32_t value);
void bootAdiBusChange(volatile uint32_t *reg, uint32_t mask, uint32_t value);
void bootAdiBusBatchChange(volatile uint32_t *reg, uint32_t write_mask, uint32_t write_value, ...);

#ifdef __cplusplus
}
#endif
#endif
