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

#include "hal_blue_screen.h"
#include "hal_blue_screen_imp.h"
#include "hal_config.h"
#include "hal_chip.h"
#include "osi_log.h"
#include "osi_api.h"
#include "osi_profile.h"
#include "drv_md_ipc.h"
#include "calclib/crc32.h"
#ifdef CONFIG_CPU_ARM
#include "cmsis_core.h"
#endif
#include <stdio.h>
#include <assert.h>

#define RAM_START (CONFIG_RAM_PHY_ADDRESS)
#define RAM_END (CONFIG_RAM_PHY_ADDRESS + CONFIG_RAM_SIZE)
#define SRAM_START (CONFIG_SRAM_PHY_ADDRESS)
#define SRAM_END (CONFIG_SRAM_PHY_ADDRESS + CONFIG_SRAM_SIZE)

#define BLUE_SCREEN_INFO_STRLEN (96 - 1)
static char gBlueScreenInfo[BLUE_SCREEN_INFO_STRLEN + 1];

#ifdef CONFIG_SOC_8910
#define CP_CONTEXT_ADDR (0x80002800)
uint32_t gGdbCtxType OSI_SECTION_RW_KEEP = 1;
uint32_t *gCpGdbRegAddress OSI_SECTION_RW_KEEP = (uint32_t *)CP_CONTEXT_ADDR;
uint32_t *gGdbRegAddress OSI_SECTION_RW_KEEP = NULL;
#define BSCORE_USED_SIZE (sizeof(halBscoreHeader_t) /*header*/ +                  \
                          sizeof(halBscoreSectHeader_t) + 8 /*coreend*/ +         \
                          sizeof(halBscoreSectHeader_t) + 64 /*swver*/ +          \
                          sizeof(halBscoreSectHeader_t) + 96 /*apexec*/ +         \
                          sizeof(halBscoreSectHeader_t) + 96 /*cpexec*/ +         \
                          sizeof(halBscoreSectArmv7aReg_t) +                      \
                          sizeof(halBscoreSectArmv7aCpReg_t) +                    \
                          sizeof(halBscoreSectMem_t) + CONFIG_BSCORE_STACK_SIZE + \
                          sizeof(halBscoreSectMem_t) + CONFIG_BSCORE_STACK_SIZE + \
                          sizeof(halBscoreSectHeader_t) + CONFIG_BSCORE_PROFILE_SIZE)
#endif

#ifdef CONFIG_SOC_8811
#define BSCORE_USED_SIZE (sizeof(halBscoreHeader_t) /*header*/ +                  \
                          sizeof(halBscoreSectHeader_t) + 8 /*coreend*/ +         \
                          sizeof(halBscoreSectHeader_t) + 64 /*swver*/ +          \
                          sizeof(halBscoreSectHeader_t) + 96 /*apexec*/ +         \
                          sizeof(halBscoreSectArmv8mReg_t) +                      \
                          sizeof(halBscoreSectMem_t) + CONFIG_BSCORE_STACK_SIZE + \
                          sizeof(halBscoreSectHeader_t) + CONFIG_BSCORE_PROFILE_SIZE)
#endif

#ifdef CONFIG_BSCORE_ENABLE
static_assert(CONFIG_BSCORE_SIZE >= BSCORE_USED_SIZE, "CONFIG_BSCORE_SIZE is too small");
#endif

/**
 * Non-standard strncpy, \p dst will always add null byte.
 */
OSI_UNUSED static void prvStrncpy(char *dst, const char *src, unsigned size)
{
    for (unsigned n = 0; n < size - 1; n++)
    {
        char ch = *src++;
        if (ch == '\0')
            break;
        *dst++ = ch;
    }
    *dst++ = '\0';
}

/**
 * Non-standard memcpy.
 */
OSI_UNUSED static void prvMemcpy32(uint32_t *dst, const uint32_t *src, unsigned size)
{
    for (unsigned n = 0; n < size / 4; n++)
        *dst++ = *src++;
}

/**
 * Non-standard memset.
 */
OSI_UNUSED static void prvMemset32(uint32_t *dst, unsigned val, unsigned size)
{
    for (unsigned n = 0; n < size / 4; n++)
        *dst++ = val;
}

/**
 * Get the blue screen information string
 */
const char *halBlueScreenGetInfo()
{
    return gBlueScreenInfo;
}

/**
 * Post-processing of blue screen context
 */
void halBlueScreenSaveContext(void *ctx)
{
#ifdef CONFIG_SOC_8910
    // ctx points to "usr_sp", gGdbRegAddress points to "sp"
    halBsContextArmv7a_t *regs = (halBsContextArmv7a_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv7a_t, usr_sp));
    gGdbRegAddress = (uint32_t *)((uintptr_t)regs + OSI_OFFSETOF(halBsContextArmv7a_t, sp));

    if ((regs->spsr & 0x1f) == 0x1f)
    {
        regs->sp = regs->usr_sp;
        regs->lr = regs->usr_lr;
    }
    else
    {
        regs->sp = regs->svc_sp;
        regs->lr = regs->svc_lr;
    }

    regs->fpscr = __get_FPSCR();
#endif

#ifdef CONFIG_CPU_ARMV8M
    // ctx points to "msp"
    halBsContextArmv8m_t *p = (halBsContextArmv8m_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv8m_t, msp));

    bool exc_return_spsel = (p->exc_return & EXC_RETURN_SPSEL);
    uint32_t psp = __get_PSP();

    uint32_t *frameptr = (uint32_t *)(exc_return_spsel ? psp : p->msp);
    p->r0 = *frameptr++;
    p->r1 = *frameptr++;
    p->r2 = *frameptr++;
    p->r3 = *frameptr++;
    p->r12 = *frameptr++;
    p->r14 = *frameptr++;
    p->r15 = *frameptr++;
    uint32_t xpsr = *frameptr++;

    p->fpscr = __get_FPSCR();
    if ((p->exc_return & EXC_RETURN_FTYPE) == 0)
    {
        for (int n = 0; n < 16; n++)
            frameptr++;
        p->fpscr = *frameptr++;
        frameptr++;
    }

    p->r13 = (uint32_t)frameptr + ((xpsr & (1 << 9)) ? 4 : 0);
    p->psp = exc_return_spsel ? p->r13 : psp;
    p->xpsr = xpsr;
    p->msplim = __get_MSPLIM();
    p->psplim = __get_PSPLIM();
    p->primask = __get_PRIMASK();
    p->basepri = __get_BASEPRI();
    p->faultmask = __get_FAULTMASK();
    p->control = __get_CONTROL();
    if (p->exc_return & EXC_RETURN_SPSEL)
        p->control |= CONTROL_SPSEL_Msk;
    p->curr_xpsr = __get_xPSR();
#endif
}

/**
 * Save/generate blue screen information string
 */
void halBlueScreenSaveInfo(void *ctx)
{
#ifdef CONFIG_SOC_8910
    // ctx points to "usr_sp", gGdbRegAddress points to "sp"
    halBsContextArmv7a_t *regs = (halBsContextArmv7a_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv7a_t, usr_sp));

    snprintf(gBlueScreenInfo, BLUE_SCREEN_INFO_STRLEN, "!!! BLUE SCREEN pc/0x%lx lr/0x%lx sp/0x%lx cpsr/0x%lx spsr/0x%lx",
             regs->elr, regs->lr, regs->sp, regs->cpsr, regs->spsr);
#endif

#ifdef CONFIG_CPU_ARMV8M
    // ctx points to "msp"
    halBsContextArmv8m_t *p = (halBsContextArmv8m_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv8m_t, msp));

    snprintf(gBlueScreenInfo, BLUE_SCREEN_INFO_STRLEN, "!!! BLUE SCREEN pc/0x%lx lr/0x%lx sp/0x%lx xspr/0x%lx",
             p->r15, p->r14, p->r13, p->xpsr);
#endif
    OSI_LOGXE(OSI_LOGPAR_S, 0, "%s", gBlueScreenInfo);
}

#ifdef CONFIG_SOC_8910
OSI_UNUSED static void prvGetCp15(halArmv7aCp15_t *cp15)
{
    cp15->MIDR = HAL_READ_CP(HAL_CP_MIDR);
    cp15->CTR = HAL_READ_CP(HAL_CP_CTR);
    cp15->TCMTR = HAL_READ_CP(HAL_CP_TCMTR);
    cp15->TLBTR = HAL_READ_CP(HAL_CP_TLBTR);
    cp15->MPIDR = HAL_READ_CP(HAL_CP_MPIDR);
    cp15->PFR0 = HAL_READ_CP(HAL_CP_PFR0);
    cp15->PFR1 = HAL_READ_CP(HAL_CP_PFR1);
    cp15->DFR0 = HAL_READ_CP(HAL_CP_DFR0);
    cp15->AFR0 = HAL_READ_CP(HAL_CP_AFR0);
    cp15->MMFR0 = HAL_READ_CP(HAL_CP_MMFR0);
    cp15->MMFR1 = HAL_READ_CP(HAL_CP_MMFR1);
    cp15->MMFR2 = HAL_READ_CP(HAL_CP_MMFR2);
    cp15->MMFR3 = HAL_READ_CP(HAL_CP_MMFR3);
    cp15->ISAR0 = HAL_READ_CP(HAL_CP_ISAR0);
    cp15->ISAR1 = HAL_READ_CP(HAL_CP_ISAR1);
    cp15->ISAR2 = HAL_READ_CP(HAL_CP_ISAR2);
    cp15->ISAR3 = HAL_READ_CP(HAL_CP_ISAR3);
    cp15->ISAR4 = HAL_READ_CP(HAL_CP_ISAR4);
    cp15->ISAR5 = HAL_READ_CP(HAL_CP_ISAR5);
    cp15->CCSIDR = HAL_READ_CP(HAL_CP_CCSIDR);
    cp15->CLIDR = HAL_READ_CP(HAL_CP_CLIDR);
    cp15->AIDR = HAL_READ_CP(HAL_CP_AIDR);
    cp15->CSSELR = HAL_READ_CP(HAL_CP_CSSELR);
    cp15->SCTLR = HAL_READ_CP(HAL_CP_SCTLR);
    cp15->ACTLR = HAL_READ_CP(HAL_CP_ACTLR);
    cp15->CPACR = HAL_READ_CP(HAL_CP_CPACR);
    cp15->SCR = HAL_READ_CP(HAL_CP_SCR);
    cp15->SDER = HAL_READ_CP(HAL_CP_SDER);
    cp15->NSACR = HAL_READ_CP(HAL_CP_NSACR);
    cp15->VCR = HAL_READ_CP(HAL_CP_VCR);
    cp15->TTBR0 = HAL_READ_CP(HAL_CP_TTBR0);
    cp15->TTBR1 = HAL_READ_CP(HAL_CP_TTBR1);
    cp15->TTBCR = HAL_READ_CP(HAL_CP_TTBCR);
    cp15->DACR = HAL_READ_CP(HAL_CP_DACR);
    cp15->DFSR = HAL_READ_CP(HAL_CP_DFSR);
    cp15->IFSR = HAL_READ_CP(HAL_CP_IFSR);
    cp15->ADFSR = HAL_READ_CP(HAL_CP_ADFSR);
    cp15->AIFSR = HAL_READ_CP(HAL_CP_AIFSR);
    cp15->DFAR = HAL_READ_CP(HAL_CP_DFAR);
    cp15->IFAR = HAL_READ_CP(HAL_CP_IFAR);
    cp15->PAR = HAL_READ_CP(HAL_CP_PAR);
    cp15->PRRR = HAL_READ_CP(HAL_CP_PRRR);
    cp15->NMRR = HAL_READ_CP(HAL_CP_NMRR);
    cp15->VBAR = HAL_READ_CP(HAL_CP_VBAR);
    cp15->MVBAR = HAL_READ_CP(HAL_CP_MVBAR);
    cp15->ISR = HAL_READ_CP(HAL_CP_ISR);
    cp15->VIR = HAL_READ_CP(HAL_CP_VIR);
    cp15->CONTEXT = HAL_READ_CP(HAL_CP_CONTEXT);
    cp15->TPIDRURW = HAL_READ_CP(HAL_CP_TPIDRURW);
    cp15->TPIDRURO = HAL_READ_CP(HAL_CP_TPIDRURO);
    cp15->TPIDRPRW = HAL_READ_CP(HAL_CP_TPIDRPRW);
    cp15->VBAR = HAL_READ_CP(HAL_CP_VBAR);
    cp15->TLBHR = HAL_READ_CP(HAL_CP_TLBHR);
}

OSI_UNUSED static void prvSaveArmca5CpRegs(halBscoreSectArmv7aCpReg_t *core)
{
    core->type = HAL_BSCORE_SECT_REG_8910CP;
    core->size = sizeof(halBscoreSectArmv7aCpReg_t);
    prvMemcpy32((uint32_t *)&core->regs, (uint32_t *)CP_CONTEXT_ADDR, core->size - 8);
}

OSI_UNUSED static void prvSaveBankedReg(halArmv7aRegs_t *regs)
{
    unsigned cpsr = __get_CPSR();
    asm("CPS    #0x1f\n" // SYS
        "MRS    r2, SPSR\n"
        "STR    lr, [%0, %1]\n"
        "STR    sp, [%0, %2]\n" ::
            "r"(regs),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, lr_usr)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, sp_usr))
        : "r2");
    asm("CPS    #0x12\n" // IRQ
        "MRS    r2, SPSR\n"
        "STR    lr, [%0, %1]\n"
        "STR    sp, [%0, %2]\n"
        "STR    r2, [%0, %3]\n" ::
            "r"(regs),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, lr_irq)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, sp_irq)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, spsr_irq))
        : "r2");
    asm("CPS    #0x13\n" // SVC
        "MRS    r2, SPSR\n"
        "STR    lr, [%0, %1]\n"
        "STR    sp, [%0, %2]\n"
        "STR    r2, [%0, %3]\n" ::
            "r"(regs),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, lr_svc)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, sp_svc)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, spsr_svc))
        : "r2");
    asm("CPS    #0x17\n" // ABT
        "MRS    r2, SPSR\n"
        "STR    lr, [%0, %1]\n"
        "STR    sp, [%0, %2]\n"
        "STR    r2, [%0, %3]\n" ::
            "r"(regs),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, lr_abt)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, sp_abt)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, spsr_abt))
        : "r2");
    asm("CPS    #0x1B\n" // UND
        "MRS    r2, SPSR\n"
        "STR    lr, [%0, %1]\n"
        "STR    sp, [%0, %2]\n"
        "STR    r2, [%0, %3]\n" ::
            "r"(regs),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, lr_und)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, sp_und)),
        "i"(OSI_OFFSETOF(halArmv7aRegs_t, spsr_und))
        : "r2");
    __set_CPSR(cpsr);
    __ISB();
}

OSI_UNUSED static void prvSaveArmca5Regs(halBscoreSectArmv7aReg_t *core, void *ctx)
{
    halBsContextArmv7a_t *regs = (halBsContextArmv7a_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv7a_t, usr_sp));

    prvMemset32((uint32_t *)core, 0, sizeof(halBsContextArmv7a_t));
    core->type = HAL_BSCORE_SECT_REG_8910AP;
    core->size = sizeof(halBscoreSectArmv7aReg_t);
    core->regs.epsr = regs->cpsr;
    core->regs.cpsr = regs->spsr;
    core->regs.r[0] = regs->r0;
    core->regs.r[1] = regs->r1;
    core->regs.r[2] = regs->r2;
    core->regs.r[3] = regs->r3;
    core->regs.r[4] = regs->r4;
    core->regs.r[5] = regs->r5;
    core->regs.r[6] = regs->r6;
    core->regs.r[7] = regs->r7;
    core->regs.r[8] = regs->r8;
    core->regs.r[9] = regs->r9;
    core->regs.r[10] = regs->r10;
    core->regs.r[11] = regs->r11;
    core->regs.r[12] = regs->ip;
    core->regs.r[13] = regs->sp;
    core->regs.r[14] = regs->lr;
    core->regs.r[15] = regs->elr;

    core->regs.fpscr = __get_FPSCR();
    prvSaveBankedReg(&core->regs);
    prvGetCp15(&core->cp15);

    asm("VSTM %0, {d0-d15}"
        :
        : "r"(&core->regs.d[0]));
    asm("VSTM %0, {d16-d31}"
        :
        : "r"(&core->regs.d[16]));
}
#endif

#ifdef CONFIG_SOC_8811
OSI_UNUSED static void prvSaveArmv8mRegs(halBscoreSectArmv8mReg_t *core, void *ctx)
{
    halBsContextArmv8m_t *regs = (halBsContextArmv8m_t *)((uintptr_t)ctx - OSI_OFFSETOF(halBsContextArmv8m_t, msp));

    prvMemset32((uint32_t *)core, 0, sizeof(halBsContextArmv8m_t));
    core->type = HAL_BSCORE_SECT_REG_8811;
    core->size = sizeof(halBsContextArmv8m_t);
    core->regs.r[0] = regs->r0;
    core->regs.r[1] = regs->r1;
    core->regs.r[2] = regs->r2;
    core->regs.r[3] = regs->r3;
    core->regs.r[4] = regs->r4;
    core->regs.r[5] = regs->r5;
    core->regs.r[6] = regs->r6;
    core->regs.r[7] = regs->r7;
    core->regs.r[8] = regs->r8;
    core->regs.r[9] = regs->r9;
    core->regs.r[10] = regs->r10;
    core->regs.r[11] = regs->r11;
    core->regs.r[12] = regs->r12;
    core->regs.r[13] = regs->r13;
    core->regs.r[14] = regs->r14;
    core->regs.r[15] = regs->r15;
    core->regs.XPSR = regs->xpsr;
    core->regs.FPSCR = regs->fpscr;
    core->regs.curr_xpsr = regs->curr_xpsr;
    core->regs.EXC_RETURN = regs->exc_return;
    core->regs.CONTROL = regs->control;
    core->regs.BASEPRI = regs->basepri;
    core->regs.PRIMASK = regs->primask;
    core->regs.FAULTMASK = regs->faultmask;
    core->regs.MSP = regs->msp;
    core->regs.PSP = regs->psp;
    core->regs.MSPLIM = regs->msplim;
    core->regs.PSPLIM = regs->psplim;
    core->regs.CPUID = SCB->CPUID;
    core->regs.ICSR = SCB->ICSR;
    core->regs.VTOR = SCB->VTOR;
    core->regs.AIRCR = SCB->AIRCR;
    core->regs.SCR = SCB->SCR;
    core->regs.CCR = SCB->CCR;
    for (unsigned n = 0; n < 12; n++)
        core->regs.SHPR[n] = SCB->SHPR[n];
    core->regs.SHCSR = SCB->SHCSR;
    core->regs.CFSR = SCB->CFSR;
    core->regs.HFSR = SCB->HFSR;
    core->regs.DFSR = SCB->DFSR;
    core->regs.MMFAR = SCB->MMFAR;
    core->regs.BFAR = SCB->BFAR;
    core->regs.AFSR = SCB->AFSR;
    core->regs.ID_PFR[0] = SCB->ID_PFR[0];
    core->regs.ID_PFR[1] = SCB->ID_PFR[1];
    core->regs.ID_DFR = SCB->ID_DFR;
    core->regs.ID_ADR = SCB->ID_ADR;
    for (unsigned n = 0; n < 4; n++)
        core->regs.ID_MMFR[n] = SCB->ID_MMFR[n];
    for (unsigned n = 0; n < 6; n++)
        core->regs.ID_ISAR[n] = SCB->ID_ISAR[n];
    core->regs.CLIDR = SCB->CLIDR;
    core->regs.CTR = SCB->CTR;
    core->regs.CCSIDR = SCB->CCSIDR;
    core->regs.CPACR = SCB->CPACR;
    core->regs.NSACR = SCB->NSACR;
    core->regs.MVFR0 = SCB->MVFR0;
    core->regs.MVFR1 = SCB->MVFR1;
    core->regs.MVFR2 = SCB->MVFR2;
    core->regs.ICTR = SCnSCB->ICTR;
    core->regs.ACTLR = SCnSCB->ACTLR;
    core->regs.CPPWR = SCnSCB->CPPWR;
    core->regs.FPCCR = FPU->FPCCR;
    core->regs.FPCAR = FPU->FPCAR;
    core->regs.FPDSCR = FPU->FPDSCR;

    asm("VSTM %0, {s0-s31}"
        :
        : "r"(&core->regs.s[0]));
}
#endif

#ifdef CONFIG_BSCORE_ENABLE
static void prvStackDump(halBscoreSectMem_t *spdump, uintptr_t sp)
{
    spdump->type = HAL_BSCORE_SECT_MEM;
    spdump->flags = 0;
    spdump->address = sp;
    spdump->size = sizeof(halBscoreSectMem_t);

    uintptr_t spdump_start = ((sp & ~3) - CONFIG_BSCORE_STACK_EXTRA);
    uintptr_t spdump_end = 0;
    unsigned spdump_size = CONFIG_BSCORE_STACK_SIZE;
#ifdef CONFIG_RAM_SIZE
    if (sp >= RAM_START && sp < RAM_END)
    {
        spdump_start = OSI_MAX(unsigned, RAM_START, spdump_start);
        spdump_end = OSI_MIN(unsigned, RAM_END, spdump_start + spdump_size);
    }
#endif
    if (sp >= SRAM_START && sp < SRAM_END)
    {
        spdump_start = OSI_MAX(unsigned, SRAM_START, spdump_start);
        spdump_end = OSI_MIN(unsigned, SRAM_END, spdump_start + spdump_size);
    }

    if (spdump_start >= spdump_end)
        return;

    spdump_size = spdump_end - spdump_start;
    prvMemcpy32((uint32_t *)spdump->payload, (uint32_t *)spdump_start, spdump_size);
    spdump->address = spdump_start;
    spdump->size += spdump_size;
}
#endif

/**
 * Save bscore buffer
 */
void halBscoreBufSave(void *ctx)
{
#ifdef CONFIG_BSCORE_ENABLE
    uintptr_t core_ptr = (uintptr_t)__bscore_start;
    halBscoreHeader_t *header = (halBscoreHeader_t *)core_ptr;

    // If there are bscore already, not to overwrite it.
    if (header->tag == HAL_BSCORE_TAG &&
        header->size > sizeof(halBscoreHeader_t) &&
        header->size <= CONFIG_BSCORE_SIZE)
        return;

    header->tag = HAL_BSCORE_TAG;
    header->size = sizeof(halBscoreHeader_t);
    header->crc = 0;
    core_ptr += OSI_ALIGN_UP(header->size, 4);

    // Insert swversion
    halBscoreSectHeader_t *swver = (halBscoreSectHeader_t *)core_ptr;
    swver->type = HAL_BSCORE_SECT_SWVERSION;
    swver->size = sizeof(halBscoreSectHeader_t) + 64;
    prvStrncpy((char *)swver->payload, gBuildRevision, 64);
    core_ptr += OSI_ALIGN_UP(swver->size, 4);
    header->size += OSI_ALIGN_UP(swver->size, 4);

    // Inser ap exec
    halBscoreSectHeader_t *apexec = (halBscoreSectHeader_t *)core_ptr;
    apexec->type = HAL_BSCORE_SECT_EXEC;
    apexec->size = sizeof(halBscoreSectHeader_t) + 96;
    prvStrncpy((char *)apexec->payload, gBlueScreenInfo, 96);
    core_ptr += OSI_ALIGN_UP(apexec->size, 4);
    header->size += OSI_ALIGN_UP(apexec->size, 4);

    // Insert profile
    halBscoreSectHeader_t *profile = (halBscoreSectHeader_t *)core_ptr;
    unsigned profile_size = osiProfileGetLastest((uint32_t *)profile->payload, CONFIG_BSCORE_PROFILE_SIZE);
    if (profile_size > 0)
    {
        profile->type = HAL_BSCORE_SECT_PROFILE;
        profile->size = sizeof(halBscoreSectHeader_t) + profile_size;
        core_ptr += OSI_ALIGN_UP(profile->size, 4);
        header->size += OSI_ALIGN_UP(profile->size, 4);
    }

#ifdef CONFIG_SOC_8910
    // Insert registers
    halBscoreSectArmv7aReg_t *regs = (halBscoreSectArmv7aReg_t *)core_ptr;
    prvSaveArmca5Regs(regs, ctx);
    core_ptr += OSI_ALIGN_UP(regs->size, 4);
    header->size += OSI_ALIGN_UP(regs->size, 4);

    // Insert stack dump
    halBscoreSectMem_t *spdump = (halBscoreSectMem_t *)core_ptr;
    prvStackDump(spdump, regs->regs.r[13]);
    core_ptr += OSI_ALIGN_UP(spdump->size, 4);
    header->size += OSI_ALIGN_UP(spdump->size, 4);

    // Wait CP written blue screen context
    osiDelayUS(200000);

    // Insert CP exception statement
    uintptr_t cpexec_cause_addr = (uintptr_t)ipc_get_cp_assert_cause();
    if (cpexec_cause_addr >= RAM_START && cpexec_cause_addr < RAM_END)
    {
        halBscoreSectHeader_t *cpexec = (halBscoreSectHeader_t *)core_ptr;
        cpexec->type = HAL_BSCORE_SECT_8910CP_EXEC;
        cpexec->size = sizeof(halBscoreSectHeader_t) + 96;
        prvStrncpy((char *)cpexec->payload, (const char *)cpexec_cause_addr, 96);
        core_ptr += OSI_ALIGN_UP(cpexec->size, 4);
        header->size += OSI_ALIGN_UP(cpexec->size, 4);
    }

    // Insert CP registers
    halBscoreSectArmv7aCpReg_t *cpregs = (halBscoreSectArmv7aCpReg_t *)core_ptr;
    prvSaveArmca5CpRegs(cpregs);
    core_ptr += OSI_ALIGN_UP(cpregs->size, 4);
    header->size += OSI_ALIGN_UP(cpregs->size, 4);

    // Insert CP stack dump
    halBscoreSectMem_t *cpspdump = (halBscoreSectMem_t *)core_ptr;
    prvStackDump(cpspdump, cpregs->regs.r[13]);
    core_ptr += OSI_ALIGN_UP(cpspdump->size, 4);
    header->size += OSI_ALIGN_UP(cpspdump->size, 4);
#endif

#ifdef CONFIG_SOC_8811
    // Insert registers
    halBscoreSectArmv8mReg_t *regs = (halBscoreSectArmv8mReg_t *)core_ptr;
    prvSaveArmv8mRegs(regs, ctx);
    core_ptr += OSI_ALIGN_UP(regs->size, 4);
    header->size += OSI_ALIGN_UP(regs->size, 4);

    // Insert stack dump
    halBscoreSectMem_t *spdump = (halBscoreSectMem_t *)core_ptr;
    prvStackDump(spdump, regs->regs.r[13]);
    core_ptr += OSI_ALIGN_UP(spdump->size, 4);
    header->size += OSI_ALIGN_UP(spdump->size, 4);
#endif

    // Insert core end
    halBscoreSectHeader_t *coreend = (halBscoreSectHeader_t *)core_ptr;
    coreend->type = HAL_BSCORE_SECT_COREEND;
    coreend->size = sizeof(halBscoreSectHeader_t) + 8;
    prvStrncpy((char *)coreend->payload, "COREEND", 8);
    core_ptr += OSI_ALIGN_UP(coreend->size, 4);
    header->size += OSI_ALIGN_UP(coreend->size, 4);

    // Update checksum
    header->crc = crc32Calc(header->payload, header->size - sizeof(halBscoreHeader_t));
#endif
}

/**
 * Get bscore buffer if valid
 */
osiBuffer_t halGetBscoreBuf(void)
{
    osiBuffer_t buf = {};
#ifdef CONFIG_BSCORE_ENABLE
    halBscoreHeader_t *header = (halBscoreHeader_t *)__bscore_start;
    if (header->tag == HAL_BSCORE_TAG &&
        header->size > sizeof(halBscoreHeader_t) &&
        header->size <= CONFIG_BSCORE_SIZE)
    {
        buf.ptr = (uintptr_t)header;
        buf.size = header->size;
    }
#endif
    return buf;
}

/**
 * Clear bscore buffer, mark as invalid
 */
void halClearBscoreBuf(void)
{
#ifdef CONFIG_BSCORE_ENABLE
    halBscoreHeader_t *header = (halBscoreHeader_t *)__bscore_start;
    header->tag = 0;
#endif
}
