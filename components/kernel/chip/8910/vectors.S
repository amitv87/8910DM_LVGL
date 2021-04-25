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

    .syntax unified
    .text
    .arm

#define CPSR_MODE_SYS       0x1f
#define CPSR_MODE_SVC       0x13
#define CPSR_T              (1 << 5)

    .extern __fiq_stack_top
    .extern __irq_stack_top
    .extern __svc_stack_top
    .extern __abt_stack_top
    .extern __und_stack_top
    .extern __sys_stack_top
    .extern Reset_Handler
    .extern SVC_Handler
    .extern IRQ_Handler
    .extern osiBlueScreen

    .align 5
    .section VECTORS, #alloc, #execinstr

    .global Vectors
    .type   Vectors, %function
Vectors:
    LDR     PC, =Reset_Handler
    LDR     PC, =Undef_Handler
    LDR     PC, =SVC_Handler
    LDR     PC, =PAbt_Handler
    LDR     PC, =DAbt_Handler
    B       .
    LDR     PC, =IRQ_Handler
    LDR     PC, =FIQ_Handler

    .weak   Undef_Handler
    .type   Vectors, %function
Undef_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    mrs     r1, spsr
    tst     r1, #CPSR_T
    subne   lr, lr, #2              /* thumb elr: -2 bytes */
    subeq   lr, lr, #4              /* arm elr: -4 bytes */
    b       .LAbortCommon

    .weak   PAbt_Handler
    .type   Vectors, %function
PAbt_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    sub     lr, lr, #4              /* elr: -4 bytes */
    b       .LAbortCommon

    .weak   DAbt_Handler
    .type   Vectors, %function
DAbt_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    sub     lr, lr, #8              /* elr: -8 bytes */
    b       .LAbortCommon

    .weak   FIQ_Handler
    .type   Vectors, %function
FIQ_Handler:
    cpsid   aif                     /* disable irq/fiq */
    stmdb   sp!, {r0-r11, ip}       /* store r0-r12 */
    b       .LAbortCommon

.LAbortCommon:
    mrs     r2, cpsr
    mrs     r3, spsr
    stmdb   sp!, {r2, r3, lr}       /* store: cpsr, spsr, elr */

    stmdb   sp!, {r2, r3}           /* store: padding */

    mov     r0, sp                  /* backup sp */
    cps     #CPSR_MODE_SVC
    str     lr, [r0, #-4]!          /* store: SVC lr */
    str     sp, [r0, #-4]!          /* store: SVC sp */
    cps     #CPSR_MODE_SYS
    str     lr, [r0, #-4]!          /* store: SYS lr */
    str     sp, [r0, #-4]!          /* store: SYS sp */

    msr     cpsr, r2
    sub     sp, #32
    bl      osiBlueScreen

.LAbortEnd:
    b      .LAbortEnd

    .global osiCallWithStack
    .func   osiCallWithStack
    .thumb_func
osiCallWithStack:
    push    {r4, lr}
    mov     r4, sp
    mov     sp, r0
    mov     r0, r2
    mov     r2, r1
    mov     r1, r3
    blx     r2
    mov     sp, r4
    pop     {r4, pc}
    .endfunc
