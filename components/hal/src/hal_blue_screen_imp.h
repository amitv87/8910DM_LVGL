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

#ifndef _HAL_BLUE_SCREEN_IMP_H_
#define _HAL_BLUE_SCREEN_IMP_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#define HAL_BSCORE_TAG OSI_MAKE_TAG('B', 'S', 'C', 'R')

enum
{
    HAL_BSCORE_SECT_COREEND,
    HAL_BSCORE_SECT_SWVERSION,
    HAL_BSCORE_SECT_MEM,
    HAL_BSCORE_SECT_PROFILE,
    HAL_BSCORE_SECT_EXEC,
    HAL_BSCORE_SECT_8910CP_EXEC,
    HAL_BSCORE_SECT_REG_8910AP = 0x40,
    HAL_BSCORE_SECT_REG_8910CP,
    HAL_BSCORE_SECT_REG_8811,
};

extern uint32_t __bscore_start[];
extern const char gBuildRevision[];

typedef struct
{
    uint32_t tag;        ///< HAL_BSCORE_TAG
    uint32_t size;       ///< total size
    uint32_t crc;        ///< checksum of data after this field
    uint32_t payload[0]; ///< payload followed
} halBscoreHeader_t;

typedef struct
{
    uint32_t type;       ///< section type
    uint32_t size;       ///< section total size
    uint32_t payload[0]; ///< payload followed
} halBscoreSectHeader_t;

typedef struct
{
    uint32_t type;       ///< HAL_BSCORE_SECT_MEM
    uint32_t size;       ///< section total size
    uint32_t flags;      ///< reserved for future
    uint32_t address;    ///< address for the content
    uint32_t payload[0]; ///< payload followed
} halBscoreSectMem_t;

typedef struct
{
    uint32_t MIDR;     ///< Main ID Register
    uint32_t CTR;      ///< Cache Type Register
    uint32_t TCMTR;    ///< TCM Type Register
    uint32_t TLBTR;    ///< TLB Type Register
    uint32_t MPIDR;    ///< Multiprocessor Affinity Register
    uint32_t PFR0;     ///< Processor Feature Register 0
    uint32_t PFR1;     ///< Processor Feature Register 1
    uint32_t DFR0;     ///< Debug Feature Register 0
    uint32_t AFR0;     ///< Auxiliary Feature Register 0
    uint32_t MMFR0;    ///< Memory Model Feature Register 0
    uint32_t MMFR1;    ///< Memory Model Feature Register 1
    uint32_t MMFR2;    ///< Memory Model Feature Register 2
    uint32_t MMFR3;    ///< Memory Model Feature Register 3
    uint32_t ISAR0;    ///< ISA Feature Register 0
    uint32_t ISAR1;    ///< ISA Feature Register 1
    uint32_t ISAR2;    ///< ISA Feature Register 2
    uint32_t ISAR3;    ///< ISA Feature Register 3
    uint32_t ISAR4;    ///< ISA Feature Register 4
    uint32_t ISAR5;    ///< ISA Feature Register 4
    uint32_t CCSIDR;   ///< Cache Level ID Register
    uint32_t CLIDR;    ///< Cache Size ID Registers
    uint32_t AIDR;     ///< Auxiliary ID Register
    uint32_t CSSELR;   ///< Cache Size Selection Register
    uint32_t SCTLR;    ///< System Control Register
    uint32_t ACTLR;    ///< Auxiliary Control Register
    uint32_t CPACR;    ///< Coprocessor Access Control Register
    uint32_t SCR;      ///< Secure Configuration Register
    uint32_t SDER;     ///< Secure Debug Enable Register
    uint32_t NSACR;    ///< Non-Secure Access Control Register
    uint32_t VCR;      ///< Virtualization Control Register
    uint32_t TTBR0;    ///< Translation Table Base Register 0
    uint32_t TTBR1;    ///< Translation Table Base Register 1
    uint32_t TTBCR;    ///< Translation Table Base Control Register
    uint32_t DACR;     ///< Domain Access Control Register
    uint32_t DFSR;     ///< Data Fault Status Register
    uint32_t IFSR;     ///< Instruction Fault Status Register
    uint32_t ADFSR;    ///< Auxiliary Data Fault Status Register
    uint32_t AIFSR;    ///< Auxiliary Instruction Fault Status Register
    uint32_t DFAR;     ///< Data Fault Address Register
    uint32_t IFAR;     ///< Instruction Fault Address Register
    uint32_t PAR;      ///< Physical Address Register
    uint32_t PRRR;     ///< Primary Region Remap Register
    uint32_t NMRR;     ///< Normal Region Remap Register
    uint32_t VBAR;     ///< Vector Base Address Register
    uint32_t MVBAR;    ///< Monitor Vector Base Address Register
    uint32_t ISR;      ///< Interrupt Status Register
    uint32_t VIR;      ///< Virtualization Interrupt Register
    uint32_t CONTEXT;  ///< Context ID Register
    uint32_t TPIDRURW; ///< User Read/Write Thread ID Register
    uint32_t TPIDRURO; ///< User Read-Only Thread ID Register
    uint32_t TPIDRPRW; ///< PL1 only Thread ID Register
    uint32_t CBAR;     ///< Configuration Base Address
    uint32_t TLBHR;    ///< TLB Hitmap
} halArmv7aCp15_t;

typedef struct
{
    uint32_t r[16];     ///< r0-r15
    uint32_t d[32 * 2]; ///< d0-d31
    uint32_t cpsr;      ///< cpsr before exception
    uint32_t fpscr;     ///< fpscr
    uint32_t epsr;      ///< exception cpsr
    uint32_t sp_usr;    ///< SP_usr
    uint32_t lr_usr;    ///< LR_usr
    uint32_t sp_hyp;    ///< SP_hyp
    uint32_t spsr_hyp;  ///< SPSR_hyp
    uint32_t elr_hyp;   ///< ELR_hyp
    uint32_t sp_svc;    ///< SP_svc
    uint32_t lr_svc;    ///< LR_svc
    uint32_t spsr_svc;  ///< SPSR_svc
    uint32_t sp_abt;    ///< SP_abt
    uint32_t lr_abt;    ///< LR_abt
    uint32_t spsr_abt;  ///< SPSR_abt
    uint32_t sp_und;    ///< SP_und
    uint32_t lr_und;    ///< LR_und
    uint32_t spsr_und;  ///< SPSR_und
    uint32_t sp_mon;    ///< SP_mon
    uint32_t lr_mon;    ///< LR_mon
    uint32_t spsr_mon;  ///< SPSR_mon
    uint32_t sp_irq;    ///< SP_irq
    uint32_t lr_irq;    ///< LR_irq
    uint32_t spsr_irq;  ///< SPSR_irq
    uint32_t r8_fiq[5]; ///< R8_fiq - R12_fiq
    uint32_t sp_fiq;    ///< SP_fiq
    uint32_t lr_fiq;    ///< LR_fiq
    uint32_t spsr_fiq;  ///< SPSR_fiq
} halArmv7aRegs_t;

typedef struct
{
    uint32_t r[15];
    uint32_t pc;
    uint32_t cpsr;
    uint32_t spsr;
    uint32_t sctlr;
    uint32_t ttbr0;
} halArmv7aCpRegs_t;

typedef struct
{
    uint32_t type;        ///< HAL_BSCORE_SECT_REG_8910AP
    uint32_t size;        ///< section total size
    halArmv7aRegs_t regs; ///< registers
    halArmv7aCp15_t cp15; ///< cp15 registers
} halBscoreSectArmv7aReg_t;

typedef struct
{
    uint32_t type; ///< HAL_BSCORE_SECT_REG_8910CP
    uint32_t size; ///< section total size
    halArmv7aCpRegs_t regs;
} halBscoreSectArmv7aCpReg_t;

typedef struct
{
    uint32_t r[16];
    uint32_t s[32];
    uint32_t XPSR;
    uint32_t FPSCR;
    uint32_t curr_xpsr;
    uint32_t EXC_RETURN;
    uint32_t CONTROL;
    uint32_t BASEPRI;
    uint32_t PRIMASK;
    uint32_t FAULTMASK;
    uint32_t MSP;
    uint32_t PSP;
    uint32_t MSPLIM;
    uint32_t PSPLIM;
    uint32_t CPUID;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint8_t SHPR[12];
    uint32_t SHCSR;
    uint32_t CFSR;
    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t MMFAR;
    uint32_t BFAR;
    uint32_t AFSR;
    uint32_t ID_PFR[2];
    uint32_t ID_DFR;
    uint32_t ID_ADR;
    uint32_t ID_MMFR[4];
    uint32_t ID_ISAR[6];
    uint32_t CLIDR;
    uint32_t CTR;
    uint32_t CCSIDR;
    uint32_t CPACR;
    uint32_t NSACR;
    uint32_t MVFR0;
    uint32_t MVFR1;
    uint32_t MVFR2;
    uint32_t ICTR;
    uint32_t ACTLR;
    uint32_t CPPWR;
    uint32_t FPCCR;
    uint32_t FPCAR;
    uint32_t FPDSCR;
} halArmv8mRegs_t;

typedef struct
{
    uint32_t type; ///< HAL_BSCORE_SECT_REG_8910CP
    uint32_t size; ///< section total size
    halArmv8mRegs_t regs;
} halBscoreSectArmv8mReg_t;

typedef struct
{
    uint32_t fpscr;
    uint32_t DACR;
    uint32_t TTBR0;
    uint32_t SCTLR;
    uint32_t usr_sp; // incoming pointer
    uint32_t usr_lr;
    uint32_t svc_sp;
    uint32_t svc_lr;
    uint32_t sp;
    uint32_t lr;
    uint32_t cpsr;
    uint32_t spsr;
    uint32_t elr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t ip;
} halBsContextArmv7a_t;

typedef struct
{
    uint32_t curr_xpsr;
    uint32_t psp;
    uint32_t xpsr;
    uint32_t primask;
    uint32_t basepri;
    uint32_t faultmask;
    uint32_t control;
    uint32_t msplim;
    uint32_t psplim;
    uint32_t fpscr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t r13;
    uint32_t r14;
    uint32_t r15;
    uint32_t msp; // incoming pointer
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t exc_return;
} halBsContextArmv8m_t;

OSI_EXTERN_C_END
#endif
