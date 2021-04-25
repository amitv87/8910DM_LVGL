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

#include "boot_config.h"

#define CPSR_M_USR 0x10U
#define CPSR_M_FIQ 0x11U
#define CPSR_M_IRQ 0x12U
#define CPSR_M_SVC 0x13U
#define CPSR_M_MON 0x16U
#define CPSR_M_ABT 0x17U
#define CPSR_M_HYP 0x1AU
#define CPSR_M_UND 0x1BU
#define CPSR_M_SYS 0x1FU
#define CPSR_T     (1UL << 5)

    .text
    .arm

    .section BOOT_ENTRY, #alloc, #execinstr

// ====================================================================
//  vectors, can be used as entry
// ====================================================================
    .align 5
    .type   Vectors, %function
Vectors:
    ldr     pc, =bootEntry
    ldr     pc, =Undef_Handler
    ldr     pc, =SVC_Handler
    ldr     pc, =PAbt_Handler
    ldr     pc, =DAbt_Handler
    b       .
    ldr     pc, =IRQ_Handler
    ldr     pc, =FIQ_Handler

    .size   Vectors, .-Vectors

// ====================================================================
//  IRQ handler
// ====================================================================
    .type   IRQ_Handler, %function
IRQ_Handler:
    push    {r0-r12, r14}

    blx     bootIrqHandler

    pop     {r0-r12, r14}
    subs    pc, lr, #4

    .size   IRQ_Handler, .-IRQ_Handler

// ====================================================================
//  UND/SVC handler
// ====================================================================
    .type   Undef_Handler, %function
    .type   SVC_Handler, %function
Undef_Handler:
SVC_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    mrs     r1, spsr
    tst     r1, #CPSR_T
    subne   lr, lr, #2              /* thumb elr: -2 bytes */
    subeq   lr, lr, #4              /* arm elr: -4 bytes */
    b       .LAbortCommon

// ====================================================================
//  PAbt handler
// ====================================================================
    .type   PAbt_Handler, %function
PAbt_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    sub     lr, lr, #4              /* elr: -4 bytes */
    b       .LAbortCommon

// ====================================================================
//  DAbt handler
// ====================================================================
    .type   DAbt_Handler, %function
DAbt_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    sub     lr, lr, #8              /* elr: -8 bytes */
    b       .LAbortCommon

// ====================================================================
//  FIQ handler
// ====================================================================
    .type   FIQ_Handler, %function
FIQ_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    b       .LAbortCommon

// ====================================================================
//  abort common, jump to blue screen
// ====================================================================
.LAbortCommon:
    mrs     r2, cpsr
    mrs     r3, spsr
    stmdb   sp!, {r2, r3, lr}       /* store: cpsr, spsr, elr */

    stmdb   sp!, {r2, r3}           /* store: padding */

    mov     r0, sp                  /* backup sp */
    cpsid   aif, #CPSR_M_IRQ
    str     lr, [r0, #-4]!          /* store: SVC lr */
    str     sp, [r0, #-4]!          /* store: SVC sp */
    cpsid   aif, #CPSR_M_SYS
    str     lr, [r0, #-4]!          /* store: SYS lr */
    str     sp, [r0, #-4]!          /* store: SYS sp */

    sub     sp, r0, #16             /* back to exception sp, and reserve 4 words */
    blx     bootBlueScreen

.LAbortEnd:
    b      .LAbortEnd

// ====================================================================
//  Entry of boot.
// ====================================================================
    .global bootEntry
    .type   bootEntry, %function
bootEntry:
    cpsid   if

    // Reset SCTLR Settings
    mrc     p15, 0, r4, c1, c0, 0   // Read CP15 System Control register
    bic     r4, r4, #(0x1 << 12)    // Clear I bit 12 to disable I Cache
    bic     r4, r4, #(0x1 <<  2)    // Clear C bit  2 to disable D Cache
    bic     r4, r4, #0x1            // Clear M bit  0 to disable MMU
    bic     r4, r4, #(0x1 << 11)    // Clear Z bit 11 to disable branch prediction
    bic     r4, r4, #(0x1 << 13)    // Clear V bit 13 to disable hivecs
    mcr     p15, 0, r4, c1, c0, 0   // Write value back to CP15 System Control register
    isb

    orr     r4, r4, #(0x1 << 12)    // Set I bit 12 to enable I Cache
    mcr     p15, 0, r4, c1, c0, 0   // Write value back to CP15 System Control register
    isb

    ldr     r4, =Vectors
    mcr     p15, 0, r4, c12, c0, 0

    cps     CPSR_M_UND
    ldr     sp, =__blue_screen_end
    cps     CPSR_M_ABT
    ldr     sp, =__blue_screen_end
    cps     CPSR_M_FIQ
    ldr     sp, =__blue_screen_end
    cps     CPSR_M_SVC
    ldr     sp, =__blue_screen_end

    cps     CPSR_M_IRQ
    ldr     sp, =__irq_stack_end
    cps     CPSR_M_SYS
    ldr     sp, =__sys_stack_end

    blx     bootStart

    .size   bootEntry, .-bootEntry

// ====================================================================
//  void bootJumpImageEntry(unsigned param, bootJumpEntry_t entry)
// ====================================================================
    .global bootJumpImageEntry
    .type   bootJumpImageEntry, %function
bootJumpImageEntry:
    cpsid   if

    mov     r4, #0
    mcr     p15, 0, r4, c7, c5, 0   // Invalidate I Cache All

    // Reset SCTLR Settings
    mrc     p15, 0, r4, c1, c0, 0   // Read CP15 System Control register
    bic     r4, r4, #(0x1 << 12)    // Clear I bit 12 to disable I Cache
    bic     r4, r4, #(0x1 <<  2)    // Clear C bit  2 to disable D Cache
    bic     r4, r4, #0x1            // Clear M bit  0 to disable MMU
    bic     r4, r4, #(0x1 << 11)    // Clear Z bit 11 to disable branch prediction
    bic     r4, r4, #(0x1 << 13)    // Clear V bit 13 to disable hivecs
    mcr     p15, 0, r4, c1, c0, 0   // Write value back to CP15 System Control register
    isb

    orr     r4, r4, #(0x1 << 12)    // Set I bit 12 to enable I Cache
    mcr     p15, 0, r4, c1, c0, 0   // Write value back to CP15 System Control register
    isb

    blx     r1

    .size   bootJumpImageEntry, .-bootJumpImageEntry
