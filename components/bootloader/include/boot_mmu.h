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

#ifndef _BOOT_MMU_H_
#define _BOOT_MMU_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * @brief enable MMU
 *
 * When MMU is enabled, memory is needed for TTB. In bootloader, the memory
 * for TTB is passed as parameter.
 *
 * For 8910, L1 TTB memory should be 16KB, and 16KB aligned. L2 TTB should
 * be 1KB.
 *
 * \param l1            memory for L1 TTB
 * \param l2            memory for L2 TTB
 */
void bootMmuEnable(uint32_t *l1, uint32_t *l2);

/**
 * @brief disable MMU
 */
void bootMmuDisable(void);

OSI_EXTERN_C_END
#endif
