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

    .text
    .arm

    .extern __fiq_stack_top
    .extern __irq_stack_top
    .extern __svc_stack_top
    .extern __abt_stack_top
    .extern __und_stack_top
    .extern __sys_stack_top
    .extern osiBootStart
    .extern osiWakePm1
    .extern osiWakePm2
    .extern Vectors

    .align 5
    .section RESET, #alloc, #execinstr

    .global Reset_Handler
    .type   Reset_Handler, %function
    .func
Reset_Handler:
    CPSID   if

    // Reset SCTLR Settings
    MRC     p15, 0, R4, c1, c0, 0   // Read CP15 System Control register
    BIC     R4, R4, #(0x1 << 12)    // Clear I bit 12 to disable I Cache
    BIC     R4, R4, #(0x1 <<  2)    // Clear C bit  2 to disable D Cache
    BIC     R4, R4, #0x1            // Clear M bit  0 to disable MMU
    BIC     R4, R4, #(0x1 << 11)    // Clear Z bit 11 to disable branch prediction
    BIC     R4, R4, #(0x1 << 13)    // Clear V bit 13 to disable hivecs
    MCR     p15, 0, R4, c1, c0, 0   // Write value back to CP15 System Control register
    ISB

    ORR     R4, R4, #(0x1 << 12)    // Set I bit 12 to enable I Cache
    MCR     p15, 0, R4, c1, c0, 0   // Write value back to CP15 System Control register
    ISB

    // Set Vector Base Address Register (VBAR) to point to this application's vector table
    LDR     R4, =Vectors
    MCR     p15, 0, R4, c12, c0, 0

    // Setup Stack for each exceptional mode
    CPS     #0x11
    LDR     SP, =__blue_screen_end
    CPS     #0x12
    LDR     SP, =__irq_stack_end
    CPS     #0x13
    LDR     SP, =__svc_stack_end
    CPS     #0x17
    LDR     SP, =__blue_screen_end
    CPS     #0x1B
    LDR     SP, =__blue_screen_end
    CPS     #0x1F
    LDR     SP, =__svc_stack_end    // reuse svc stack

    MOV     R0, #0
    BLX     osiBootStart

    .ltorg
    .size   Reset_Handler, .-Reset_Handler
    .endfunc

    .global osiWakePm1Entry
    .align  5
    .type   osiWakePm1Entry, %function
    .func
osiWakePm1Entry:
    // SCTLR: 0xc5187a, I Cache is enabled

    MOV     R0, #0x221
    MOV     R1, #0x00880000
    STR     R0, [R1, #0x14]     // hwp_spiFlash->spi_config

    // Set Vector Base Address Register (VBAR) to point to this application's vector table
    LDR     R0, =Vectors
    MCR     p15, 0, R0, c12, c0, 0

    // Setup Stack for each exceptional mode
    CPS     #0x11
    LDR     SP, =__blue_screen_end
    CPS     #0x12
    LDR     SP, =__irq_stack_end
    CPS     #0x13
    LDR     SP, =__svc_stack_end
    CPS     #0x17
    LDR     SP, =__blue_screen_end
    CPS     #0x1B
    LDR     SP, =__blue_screen_end
    CPS     #0x1F
    LDR     SP, =__svc_stack_end    // reuse svc stack

    BLX     osiWakePm1

    .ltorg
    .size   osiWakePm1Entry, .-osiWakePm1Entry
    .endfunc

    .global osiWakePm2Entry
    .type   osiWakePm2Entry, %function
    .func
osiWakePm2Entry:
    MRC     p15, 0, R0, c1, c0, 0   // Read CP15 System Control register
    ORR     R0, R0, #(0x1 << 12)    // Set I bit 12 to enable I Cache
    MCR     p15, 0, R0, c1, c0, 0   // Write value back to CP15 System Control register
    ISB

    // Set Vector Base Address Register (VBAR) to point to this application's vector table
    LDR     R0, =Vectors
    MCR     p15, 0, R0, c12, c0, 0

    // Setup Stack for each exceptional mode
    CPS     #0x11
    LDR     SP, =__blue_screen_end
    CPS     #0x12
    LDR     SP, =__irq_stack_end
    CPS     #0x13
    LDR     SP, =__svc_stack_end
    CPS     #0x17
    LDR     SP, =__blue_screen_end
    CPS     #0x1B
    LDR     SP, =__blue_screen_end
    CPS     #0x1F
    LDR     SP, =__svc_stack_end    // reuse svc stack

    BLX     osiWakePm2

    .ltorg
    .size   osiWakePm2Entry, .-osiWakePm2Entry
    .endfunc
