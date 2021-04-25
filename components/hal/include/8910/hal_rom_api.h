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

#ifndef _HAL_ROM_API_H_
#define _HAL_ROM_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_WRITE_REGS_END 0
#define ROM_WRITE_REGS_UDELAY 1

/**
 * ROM APIs accessible to application
 */
typedef struct
{
    char magic[8];      ///< "RDA API"
    unsigned version;   ///< version number
    unsigned dummy[29]; ///< not documented

    /**
     * Batch write registers
     *
     * Due to the code is located in ROM, it is useful when the registers
     * to be written may affect memory access. For example, when some
     * register change may cause flash inaccessible temporally.
     *
     * The variadic parameters are located in stack. This function will
     * only access ROM and stack.
     *
     * The parameters format:
     * - (address, value): write the value to the address
     * - (1, microseconds): delay microseconds
     * - (0): end of variadic parameters
     *
     * It is available from ROM version 102.
     */
    void (*write_regs)(unsigned addr, ...);
} bootRomApi_t;

/**
 * Fixed location ROM API table
 */
#define bootRomApi ((const bootRomApi_t *)0x3f00)

#ifdef __cplusplus
}
#endif
#endif
