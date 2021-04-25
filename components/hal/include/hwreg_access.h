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

#ifndef _HWREG_ACCESS_H_
#define _HWREG_ACCESS_H_

#include <stdint.h>
#include "hal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXP2(n) (1 << (n))
typedef uint32_t REG32;
typedef uint16_t REG16;
typedef uint8_t REG8;
typedef uint8_t UINT8;

#define REG_BIT(n) (1U << (n))
#define REG_BIT0 (1U << 0)
#define REG_BIT1 (1U << 1)
#define REG_BIT2 (1U << 2)
#define REG_BIT3 (1U << 3)
#define REG_BIT4 (1U << 4)
#define REG_BIT5 (1U << 5)
#define REG_BIT6 (1U << 6)
#define REG_BIT7 (1U << 7)
#define REG_BIT8 (1U << 8)
#define REG_BIT9 (1U << 9)
#define REG_BIT10 (1U << 10)
#define REG_BIT11 (1U << 11)
#define REG_BIT12 (1U << 12)
#define REG_BIT13 (1U << 13)
#define REG_BIT14 (1U << 14)
#define REG_BIT15 (1U << 15)
#define REG_BIT16 (1U << 16)
#define REG_BIT17 (1U << 17)
#define REG_BIT18 (1U << 18)
#define REG_BIT19 (1U << 19)
#define REG_BIT20 (1U << 20)
#define REG_BIT21 (1U << 21)
#define REG_BIT22 (1U << 22)
#define REG_BIT23 (1U << 23)
#define REG_BIT24 (1U << 24)
#define REG_BIT25 (1U << 25)
#define REG_BIT26 (1U << 26)
#define REG_BIT27 (1U << 27)
#define REG_BIT28 (1U << 28)
#define REG_BIT29 (1U << 29)
#define REG_BIT30 (1U << 30)
#define REG_BIT31 (1U << 31)

#ifdef CONFIG_SOC_8910
#define HAL_SYSIRQ_NUM(irq) ((irq) + 32)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#ifdef CONFIG_SOC_6760
#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#ifdef CONFIG_SOC_8811
#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)
#endif

#ifdef __mips__
#define OSI_KSEG0(addr) (((unsigned long)(addr)&0x1fffffff) | 0x80000000)
#define OSI_KSEG1(addr) (((unsigned long)(addr)&0x1fffffff) | 0xa0000000)
#define OSI_IS_KSEG0(addr) (((unsigned long)(addr)&0xe0000000) == 0x80000000)
#define OSI_IS_KSEG1(addr) (((unsigned long)(addr)&0xe0000000) == 0xa0000000)
#define MEM_ACCESS_CACHED(addr) ((UINT32 *)OSI_KSEG0(addr))
#define MEM_ACCESS_UNCACHED(addr) ((UINT32 *)OSI_KSEG1(addr))
#endif

#ifdef __arm__
#define HAL_READ_CP(...) _HAL_READ_CP(__VA_ARGS__)
#define HAL_WRITE_CP(...) _HAL_WRITE_CP(__VA_ARGS__)

#define _HAL_READ_CP(cp, CRn, opc1, CRm, opc2) __HAL_READ_CP(cp, CRn, opc1, CRm, opc2)
#define _HAL_WRITE_CP(v, cp, CRn, opc1, CRm, opc2) __HAL_WRITE_CP(v, cp, CRn, opc1, CRm, opc2)

#define HAL_CP_MIDR 15, c0, 0, c0, 0      // Main ID Register
#define HAL_CP_CTR 15, c0, 0, c0, 1       // Cache Type Register
#define HAL_CP_TCMTR 15, c0, 0, c0, 2     // TCM Type Register
#define HAL_CP_TLBTR 15, c0, 0, c0, 3     // TLB Type Register
#define HAL_CP_MPIDR 15, c0, 0, c0, 5     // Multiprocessor Affinity Register
#define HAL_CP_PFR0 15, c0, 0, c1, 0      // Processor Feature Register 0
#define HAL_CP_PFR1 15, c0, 0, c1, 1      // Processor Feature Register 1
#define HAL_CP_DFR0 15, c0, 0, c1, 2      // Debug Feature Register 0
#define HAL_CP_AFR0 15, c0, 0, c1, 3      // Auxiliary Feature Register 0
#define HAL_CP_MMFR0 15, c0, 0, c1, 4     // Memory Model Feature Register 0
#define HAL_CP_MMFR1 15, c0, 0, c1, 5     // Memory Model Feature Register 1
#define HAL_CP_MMFR2 15, c0, 0, c1, 6     // Memory Model Feature Register 2
#define HAL_CP_MMFR3 15, c0, 0, c1, 7     // Memory Model Feature Register 3
#define HAL_CP_ISAR0 15, c0, 0, c2, 0     // ISA Feature Register 0
#define HAL_CP_ISAR1 15, c0, 0, c2, 1     // ISA Feature Register 1
#define HAL_CP_ISAR2 15, c0, 0, c2, 2     // ISA Feature Register 2
#define HAL_CP_ISAR3 15, c0, 0, c2, 3     // ISA Feature Register 3
#define HAL_CP_ISAR4 15, c0, 0, c2, 4     // ISA Feature Register 4
#define HAL_CP_ISAR5 15, c0, 0, c0, 5     // ISA Feature Register 5
#define HAL_CP_CCSIDR 15, c0, 1, c0, 0    // Cache Level ID Register
#define HAL_CP_CLIDR 15, c0, 1, c0, 1     // Cache Size ID Registers
#define HAL_CP_AIDR 15, c0, 1, c0, 7      // Auxiliary ID Register
#define HAL_CP_CSSELR 15, c0, 2, c0, 0    // Cache Size Selection Register
#define HAL_CP_SCTLR 15, c1, 0, c0, 0     // System Control Register
#define HAL_CP_ACTLR 15, c1, 0, c0, 1     // Auxiliary Control Register
#define HAL_CP_CPACR 15, c1, 0, c0, 2     // Coprocessor Access Control Register
#define HAL_CP_SCR 15, c1, 0, c1, 0       // Secure Configuration Register
#define HAL_CP_SDER 15, c1, 0, c1, 1      // Secure Debug Enable Register
#define HAL_CP_NSACR 15, c1, 0, c1, 2     // Non-Secure Access Control Register
#define HAL_CP_VCR 15, c1, 0, c1, 3       // Virtualization Control Register
#define HAL_CP_TTBR0 15, c2, 0, c0, 0     // Translation Table Base Register 0
#define HAL_CP_TTBR1 15, c2, 0, c0, 1     // Translation Table Base Register 1
#define HAL_CP_TTBCR 15, c2, 0, c0, 2     // Translation Table Base Control Register
#define HAL_CP_DACR 15, c3, 0, c0, 0      // Domain Access Control Register
#define HAL_CP_DFSR 15, c5, 0, c0, 0      // Data Fault Status Register
#define HAL_CP_IFSR 15, c5, 0, c0, 1      // Instruction Fault Status Register
#define HAL_CP_ADFSR 15, c5, 0, c1, 0     // Auxiliary Data Fault Status Register
#define HAL_CP_AIFSR 15, c5, 0, c1, 1     // Auxiliary Instruction Fault Status Register
#define HAL_CP_DFAR 15, c6, 0, c0, 0      // Data Fault Address Register
#define HAL_CP_IFAR 15, c6, 0, c0, 2      // Instruction Fault Address Register
#define HAL_CP_PAR 15, c7, 0, c4, 0       // Physical Address Register
#define HAL_CP_PRRR 15, c10, 0, c2, 0     // Primary Region Remap Register
#define HAL_CP_NMRR 15, c10, 0, c2, 1     // Normal Region Remap Register
#define HAL_CP_VBAR 15, c12, 0, c0, 0     // Vector Base Address Register
#define HAL_CP_MVBAR 15, c12, 0, c0, 1    // Monitor Vector Base Address Register
#define HAL_CP_ISR 15, c12, 0, c1, 0      // Interrupt Status Register
#define HAL_CP_VIR 15, c12, 0, c1, 1      // Virtualization Interrupt Register
#define HAL_CP_CONTEXT 15, c13, 0, c0, 1  // Context ID Register
#define HAL_CP_TPIDRURW 15, c13, 0, c0, 2 // User Read/Write Thread ID Register
#define HAL_CP_TPIDRURO 15, c13, 0, c0, 3 // User Read-Only Thread ID Register
#define HAL_CP_TPIDRPRW 15, c13, 0, c0, 4 // PL1 only Thread ID Register
#define HAL_CP_CBAR 15, c15, 4, c0, 0     // Configuration Base Address
#define HAL_CP_TLBHR 15, c15, 5, c0, 0    // TLB Hitmap

#define __HAL_READ_CP(cp, CRn, opc1, CRm, opc2) ({                         \
    uint32_t result;                                                       \
    asm volatile("MRC p" #cp ", " #opc1 ", %0, " #CRn ", " #CRm ", " #opc2 \
                 : "=r"(result)                                            \
                 :                                                         \
                 : "memory");                                              \
    result;                                                                \
})

#define __HAL_WRITE_CP(value, cp, CRn, opc1, CRm, opc2)                        \
    do                                                                         \
    {                                                                          \
        asm volatile("MCR p" #cp ", " #opc1 ", %0, " #CRn ", " #CRm ", " #opc2 \
                     :                                                         \
                     : "r"(value)                                              \
                     : "memory");                                              \
    } while (0)

#endif

// Range in [start, end]
#define HWP_ADDRESS_RANGE(hwp) (uintptr_t)(hwp), (uintptr_t)(hwp) + sizeof(*(hwp)) - 1

/**
 * \brief write register fields by register type
 *
 * All other fields not listed are write to 0. Now, at most 9 fields are supported.
 * The return value is the value to be written to register.
 *
 * Example:
 * \code{.cpp}
 * unsigned val = REGT_FIELD_WRITE(hwp_sysCtrl->sel_clock, REG_SYS_CTRL_SEL_CLOCK_T,
 *                                 sys_sel_fast, 1, soft_sel_spiflash, 1);
 * \endcode
 */
#define REGT_FIELD_WRITE(reg, type, ...) __REGT_FIELD_WRITE(reg, type, ##__VA_ARGS__)

/**
 * \brief change register fields by register type
 *
 * Similar to \p REGT_FIELD_WRITE, just all other fields not listed will be kept.
 * Inside, it is read-modify-write.
 */
#define REGT_FIELD_CHANGE(reg, type, ...) __REGT_FIELD_CHANGE(reg, type, ##__VA_ARGS__)

/**
 * \brief get register field
 */
#define REGT_FIELD_GET(reg, type, field) __REGT_FIELD_GET(reg, type, field)

/**
 * \brief write ADI register fields by register type
 *
 * Similar to \p REGT_FIELD_WRITE, just the register is ADI register.
 */
#define REGT_ADI_FIELD_WRITE(reg, type, ...) __REGT_ADI_FIELD_WRITE(reg, type, ##__VA_ARGS__)

/**
 * \brief change ADI register fields by register type
 *
 * Similar to \p REGT_FIELD_CHANGE, just the register is ADI register.
 */
#define REGT_ADI_FIELD_CHANGE(reg, type, ...) __REGT_ADI_FIELD_CHANGE(reg, type, ##__VA_ARGS__)

/**
 * \brief get ADI register field
 */
#define REGT_ADI_FIELD_GET(reg, type, field) __REGT_ADI_FIELD_GET(reg, type, field)

/**
 * \brief write register fields by a variable
 *
 * Similar to \p REGT_FIELD_WRITE, just the second parameter is a variable with
 * type of the register.
 *
 * Example:
 * \code{.cpp}
 * REG_SYS_CTRL_SEL_CLOCK_T sel_clock;
 * unsigned val = REGV_FIELD_WRITE(hwp_sysCtrl->sel_clock, sel_clock,
 *                                sys_sel_fast, 1, soft_sel_spiflash, 1);
 * \endcode
 */
#define REGV_FIELD_WRITE(reg, var, ...) __REGV_FIELD_WRITE(reg, var, ##__VA_ARGS__)

/**
 * \brief change register fields by a variable
 *
 * Similar to \p REGT_FIELD_CHANGE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_FIELD_CHANGE(reg, var, ...) __REGV_FIELD_CHANGE(reg, var, ##__VA_ARGS__)

/**
 * \brief get register field
 */
#define REGV_FIELD_GET(reg, var, field) __REGV_FIELD_GET(reg, var, field)

/**
 * \brief write ADI register fields by a variable
 *
 * Similar to \p REGT_ADI_FIELD_WRITE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_ADI_FIELD_WRITE(reg, var, ...) __REGV_ADI_FIELD_WRITE(reg, var, ##__VA_ARGS__)

/**
 * \brief change ADI register fields by a variable
 *
 * Similar to \p REGT_ADI_FIELD_CHANGE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_ADI_FIELD_CHANGE(reg, var, ...) __REGV_ADI_FIELD_CHANGE(reg, var, ##__VA_ARGS__)

/**
 * \brief get register field
 */
#define REGV_ADI_FIELD_GET(reg, var, field) __REGV_ADI_FIELD_GET(reg, var, field)

/**
 * \brief loop wait register field to meet a condition by register type
 *
 * Example:
 * \code{.cpp}
 * REGT_WAIT_FIELD_NEZ(hwp_sysCtrl->sel_clock, REG_SYS_CTRL_SEL_CLOCK_T, pll_locked);
 * \endcode
 */
#define REGT_WAIT_FIELD_EQ(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_EQ)
#define REGT_WAIT_FIELD_NE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_NE)
#define REGT_WAIT_FIELD_GT(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GT)
#define REGT_WAIT_FIELD_GE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GE)
#define REGT_WAIT_FIELD_LT(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LT)
#define REGT_WAIT_FIELD_LE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LE)
#define REGT_WAIT_FIELD_EQZ(reg, type, field) REGT_WAIT_FIELD_EQ(reg, type, field, 0)
#define REGT_WAIT_FIELD_NEZ(reg, type, field) REGT_WAIT_FIELD_NE(reg, type, field, 0)

/**
 * \brief loop wait ADI register field to meet a condition by register type
 */
#define REGT_ADI_WAIT_FIELD_EQ(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_EQ)
#define REGT_ADI_WAIT_FIELD_NE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_NE)
#define REGT_ADI_WAIT_FIELD_GT(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GT)
#define REGT_ADI_WAIT_FIELD_GE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GE)
#define REGT_ADI_WAIT_FIELD_LT(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LT)
#define REGT_ADI_WAIT_FIELD_LE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LE)
#define REGT_ADI_WAIT_FIELD_EQZ(reg, type, field) REGT_ADI_WAIT_FIELD_EQ(reg, type, field, 0)
#define REGT_ADI_WAIT_FIELD_NEZ(reg, type, field) REGT_ADI_WAIT_FIELD_NE(reg, type, field, 0)

/**
 * \brief loop wait register field to meet a condition by a variable
 *
 * Example:
 * \code{.cpp}
 * REG_SYS_CTRL_SEL_CLOCK_T sel_clock;
 * REG_WAIT_FIELD_NEZ(sel_clock, hwp_sysCtrl->sel_clock, pll_locked);
 * \endcode
 */
#define REGV_WAIT_FIELD_EQ(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_EQ)
#define REGV_WAIT_FIELD_NE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_NE)
#define REGV_WAIT_FIELD_GT(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GT)
#define REGV_WAIT_FIELD_GE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GE)
#define REGV_WAIT_FIELD_LT(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LT)
#define REGV_WAIT_FIELD_LE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LE)
#define REGV_WAIT_FIELD_EQZ(reg, var, field) REGV_WAIT_FIELD_EQ(reg, var, field, 0)
#define REGV_WAIT_FIELD_NEZ(reg, var, field) REGV_WAIT_FIELD_NE(reg, var, field, 0)

/**
 * \brief loop wait ADI register field to meet a condition by a variable
 */
#define REGV_ADI_WAIT_FIELD_EQ(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_EQ)
#define REGV_ADI_WAIT_FIELD_NE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_NE)
#define REGV_ADI_WAIT_FIELD_GT(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GT)
#define REGV_ADI_WAIT_FIELD_GE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GE)
#define REGV_ADI_WAIT_FIELD_LT(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LT)
#define REGV_ADI_WAIT_FIELD_LE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LE)
#define REGV_ADI_WAIT_FIELD_EQZ(reg, var, field) REGV_ADI_WAIT_FIELD_EQ(reg, var, field, 0)
#define REGV_ADI_WAIT_FIELD_NEZ(reg, var, field) REGV_ADI_WAIT_FIELD_NE(reg, var, field, 0)

// ============================================================================
// Implementation
// ============================================================================

#define _REGT_WAIT_FIELD(reg, type, field, expected, op) \
    do                                                   \
    {                                                    \
        type var;                                        \
        for (;;)                                         \
        {                                                \
            (var).v = (reg);                             \
            if (op((var).b.field, (expected)))           \
                break;                                   \
        }                                                \
    } while (0)

#define _REGT_ADI_WAIT_FIELD(reg, type, field, expected, op) \
    do                                                       \
    {                                                        \
        type var;                                            \
        for (;;)                                             \
        {                                                    \
            (var).v = halAdiBusRead(&(reg));                 \
            if (op((var).b.field, (expected)))               \
                break;                                       \
        }                                                    \
    } while (0)

#define _REGV_WAIT_FIELD(reg, var, field, expected, op) \
    do                                                  \
    {                                                   \
        for (;;)                                        \
        {                                               \
            (var).v = (reg);                            \
            if (op((var).b.field, (expected)))          \
                break;                                  \
        }                                               \
    } while (0)

#define _REGV_ADI_WAIT_FIELD(reg, var, field, expected, op) \
    do                                                      \
    {                                                       \
        for (;;)                                            \
        {                                                   \
            (var).v = halAdiBusRead(&(reg));                \
            if (op((var).b.field, (expected)))              \
                break;                                      \
        }                                                   \
    } while (0)

#define _REG_WAIT_OP_EQ(a, b) ((a) == (b))
#define _REG_WAIT_OP_NE(a, b) ((a) != (b))
#define _REG_WAIT_OP_GT(a, b) ((a) > (b))
#define _REG_WAIT_OP_GE(a, b) ((a) >= (b))
#define _REG_WAIT_OP_LT(a, b) ((a) < (b))
#define _REG_WAIT_OP_LE(a, b) ((a) <= (b))

#define __REGT_FIELD_WRITE(reg, type, ...) ({ \
    type _val = {};                           \
    __REGV_SET(_val, __VA_ARGS__);            \
    reg = _val.v;                             \
    _val.v;                                   \
})

#define __REGT_FIELD_CHANGE(reg, type, ...) ({ \
    type _val = {.v = reg};                    \
    __REGV_SET(_val, __VA_ARGS__);             \
    reg = _val.v;                              \
    _val.v;                                    \
})

#define __REGT_FIELD_GET(reg, type, f) ({ \
    type _val = {.v = reg};               \
    _val.b.f;                             \
})

#define __REGT_ADI_FIELD_WRITE(reg, type, ...) ({ \
    type _val = {};                               \
    __REGV_SET(_val, __VA_ARGS__);                \
    halAdiBusWrite(&(reg), _val.v);               \
    _val.v;                                       \
})

#define __REGT_ADI_FIELD_CHANGE(reg, type, ...) ({ \
    type _val = {};                                \
    __REGV_SET(_val, __VA_ARGS__);                 \
    type _maskoff = {0xffffffff};                  \
    __REGV_MASKOFF(_maskoff, __VA_ARGS__);         \
    halAdiBusChange(&(reg), ~_maskoff.v, _val.v);  \
})

#define __REGT_ADI_FIELD_GET(reg, type, f) ({ \
    type _val = {.v = halAdiBusRead(&(reg))}; \
    _val.b.f;                                 \
})

#define __REGV_FIELD_WRITE(reg, var, ...) ({ \
    var.v = 0;                               \
    __REGV_SET(var, __VA_ARGS__);            \
    reg = var.v;                             \
    var.v;                                   \
})

#define __REGV_FIELD_CHANGE(reg, var, ...) ({ \
    var.v = reg;                              \
    __REGV_SET(var, __VA_ARGS__);             \
    reg = var.v;                              \
    var.v;                                    \
})

#define __REGV_FIELD_GET(reg, var, f) ({ \
    var.v = reg;                         \
    var.b.f;                             \
})

#define __REGV_ADI_FIELD_WRITE(reg, var, ...) ({ \
    var.v = 0;                                   \
    __REGV_SET(var, __VA_ARGS__);                \
    halAdiBusWrite(&(reg), var.v);               \
    var.v;                                       \
})

#define __REGV_ADI_FIELD_CHANGE(reg, var, ...) ({ \
    var.v = 0xffffffff;                           \
    __REGV_MASKOFF(var, __VA_ARGS__);             \
    unsigned _maskoff = var.v;                    \
    var.v = 0;                                    \
    __REGV_SET(var, __VA_ARGS__);                 \
    unsigned _val = var.v;                        \
    halAdiBusChange(&(reg), ~_maskoff, _val);     \
})

#define __REGV_ADI_FIELD_GET(reg, var, f) ({ \
    var.v = halAdiBusRead(&(reg));           \
    var.b.f;                                 \
})

#define __REGV_SET_IMP2(count, ...) __REGV_SET_##count(__VA_ARGS__)
#define __REGV_SET_IMP1(count, ...) __REGV_SET_IMP2(count, __VA_ARGS__)
#define __REGV_SET(...) __REGV_SET_IMP1(OSI_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#define __REGV_MASKOFF_IMP2(count, ...) __REGV_MASKOFF_##count(__VA_ARGS__)
#define __REGV_MASKOFF_IMP1(count, ...) __REGV_MASKOFF_IMP2(count, __VA_ARGS__)
#define __REGV_MASKOFF(...) __REGV_MASKOFF_IMP1(OSI_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#define __REGV_SET_3(v, f1, v1) ({ v.b.f1 = v1; })
#define __REGV_SET_5(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_3(v, __VA_ARGS__); })
#define __REGV_SET_7(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_5(v, __VA_ARGS__); })
#define __REGV_SET_9(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_7(v, __VA_ARGS__); })
#define __REGV_SET_11(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_9(v, __VA_ARGS__); })
#define __REGV_SET_13(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_11(v, __VA_ARGS__); })
#define __REGV_SET_15(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_13(v, __VA_ARGS__); })
#define __REGV_SET_17(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_15(v, __VA_ARGS__); })
#define __REGV_SET_19(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_17(v, __VA_ARGS__); })

#define __REGV_MASKOFF_3(v, f1, v1) ({ v.b.f1 = 0; })
#define __REGV_MASKOFF_5(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_3(v, __VA_ARGS__); })
#define __REGV_MASKOFF_7(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_5(v, __VA_ARGS__); })
#define __REGV_MASKOFF_9(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_7(v, __VA_ARGS__); })
#define __REGV_MASKOFF_11(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_9(v, __VA_ARGS__); })
#define __REGV_MASKOFF_13(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_11(v, __VA_ARGS__); })
#define __REGV_MASKOFF_15(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_13(v, __VA_ARGS__); })
#define __REGV_MASKOFF_17(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_15(v, __VA_ARGS__); })
#define __REGV_MASKOFF_19(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_17(v, __VA_ARGS__); })

// ============================================================================
// Oboleted
// ============================================================================

// hardware registers are defined as union. The template of union is
//      union {
//          uint32_t v;
//          struct {
//              uint32_t f1 : 1;
//              uint32_t f2 : 2;
//              uint32_t f3 : 3;
//              ......
//          } b;
//      };
//
// The following macros are helpers for bit field operations:
// * REG_FIELD_GET: read the register, and return the value of specified field.
// * REG_WAIT_FIELD_EQ/NE/GT/GE/LT/LE/EQZ/NEZ: wait until the register fields
//   meet the condition.
// * REG_FIELD_MASK: return a mask (1 for the specified fields) of one or more
//   fields. In the above example, mask for f1 and f3 is 0x00000039
// * REG_FIELD_VALUE: return a value with specified fields, other fields are 0.
// * REG_FIELD_MASKVAL: return "mask, value"
// * REG_FIELD_CHANGE: change the specified fields, and other fields are
//   untouched. The changed value is returned
// * REG_FIELD_WRITE: write register with specified fields, and other fields
//   are 0, return the value to be written to register

#define REGTYPE_FIELD_GET(type, var, f1) ({ type _var = {(var)}; _var.b.f1; })
#define REGTYPE_FIELD_VAL(type, f1, v1) ({ type _var = {.b={.f1=(v1)}}; _var.v; })
#define REGTYPE_FIELD_MASK(type, f1) ({ type _var = {0xffffffff}; _var.b.f1 = 0; ~_var.v; })

#define REG_FIELD_GET(r, var, f) ({ var.v = r; var.b.f; })

#define REG_WAIT_FIELD_EQ(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_WAIT_FIELD_NE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_WAIT_FIELD_GT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_WAIT_FIELD_GE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_WAIT_FIELD_LT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_WAIT_FIELD_LE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_WAIT_FIELD_EQZ(var, reg, field) REG_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_WAIT_FIELD_NEZ(var, reg, field) REG_WAIT_FIELD_NE(var, reg, field, 0)

#define REG_WAIT_COND(cond) \
    do                      \
    {                       \
        while (!(cond))     \
            ;               \
    } while (0)

#define REG_FIELD_MASK1(var, f1) ({ var.v = 0xffffffff; var.b.f1 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK2(var, f1, f2) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK3(var, f1, f2, f3) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK4(var, f1, f2, f3, f4) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK5(var, f1, f2, f3, f4, f5) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.b.f6 = 0; var.v = ~var.v; var.v; })

#define REG_FIELD_VALUE1(var, f1, v1) ({ var.v = 0; var.b.f1 = v1; var.v; })
#define REG_FIELD_VALUE2(var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.v; })
#define REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.v; })
#define REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.v; })
#define REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.v; })
#define REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; var.v; })

#define REG_FIELD_MASKVAL1(var, f1, v1) REG_FIELD_MASK1(var, f1), REG_FIELD_VALUE1(var, f1, v1)
#define REG_FIELD_MASKVAL2(var, f1, v1, f2, v2) REG_FIELD_MASK2(var, f1, f2), REG_FIELD_VALUE2(var, f1, v1, f2, v2)
#define REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3) REG_FIELD_MASK3(var, f1, f2, f3), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3)
#define REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4) REG_FIELD_MASK4(var, f1, f2, f3, f4), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4)
#define REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) REG_FIELD_MASK5(var, f1, f2, f3, f4, f5), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5)
#define REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6)

#define REG_FIELD_CHANGE1(r, var, f1, v1) ({ var.v = r; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_CHANGE2(r, var, f1, v1, f2, v2) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_FIELD_WRITE1(r, var, f1, v1) ({ var.v = 0; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_WRITE2(r, var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_WRITE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_ADI_CHANGE1(r, var, f1, v1) halAdiBusChange(&(r), REG_FIELD_MASKVAL1(var, f1, v1))
#define REG_ADI_CHANGE2(r, var, f1, v1, f2, v2) halAdiBusChange(&(r), REG_FIELD_MASKVAL2(var, f1, v1, f2, v2))
#define REG_ADI_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) halAdiBusChange(&(r), REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) halAdiBusChange(&(r), REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) halAdiBusChange(&(r), REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) halAdiBusChange(&(r), REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WRITE1(r, var, f1, v1) halAdiBusWrite(&(r), REG_FIELD_VALUE1(var, f1, v1))
#define REG_ADI_WRITE2(r, var, f1, v1, f2, v2) halAdiBusWrite(&(r), REG_FIELD_VALUE2(var, f1, v1, f2, v2))
#define REG_ADI_WRITE3(r, var, f1, v1, f2, v2, f3, v3) halAdiBusWrite(&(r), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) halAdiBusWrite(&(r), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) halAdiBusWrite(&(r), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) halAdiBusWrite(&(r), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WAIT_FIELD_EQ(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_ADI_WAIT_FIELD_NE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_ADI_WAIT_FIELD_GT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_ADI_WAIT_FIELD_GE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_ADI_WAIT_FIELD_LT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_ADI_WAIT_FIELD_LE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_ADI_WAIT_FIELD_EQZ(var, reg, field) REG_ADI_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_ADI_WAIT_FIELD_NEZ(var, reg, field) REG_ADI_WAIT_FIELD_NE(var, reg, field, 0)

#define _REG_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                 \
    {                                                  \
        for (;;)                                       \
        {                                              \
            (var).v = (reg);                           \
            if (op((var).b.field, (expected)))         \
                break;                                 \
        }                                              \
    } while (0)

#define _REG_ADI_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                     \
    {                                                      \
        for (;;)                                           \
        {                                                  \
            (var).v = halAdiBusRead(&(reg));               \
            if (op((var).b.field, (expected)))             \
                break;                                     \
        }                                                  \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif
