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

#define USR_MODE 0x10
#define SYS_MODE 0x1f
#define SVC_MODE 0x13
#define IRQ_MODE 0x12

#define CONTEXT_TYPE_NOFP 0
#define CONTEXT_TYPE_FULLFP 1
#define CONTEXT_TYPE_SMALLFP 2

    .text
    .arm

    /* Hardware registers. */
    .extern ulICCIAR
    .extern ulICCEOIR

    /* Variables and functions. */
    .extern pxCurrentTCB
    .extern vTaskSwitchContext
    .extern vApplicationIRQHandler
    .extern ulPortInterruptNesting

// ====================================================================
// Save small context. It is used by svc. It is assumed portYIELD function
// is called, rather than an individual instruction. Due to ABI convention,
// not all registers are needed to be saved. For floating point registers,
// only {d8-d15} are needed to be saved, others are caller save. Though
// {r0-r3} are caller save also, they are saved in context, to keep
// compatible, and the cost is small.
// ====================================================================
.macro portSAVE_CONTEXT_SMALL
    SRSDB   sp!, #SYS_MODE      // {SPSR, lr_svc} -> sp_sys

    CPS     #SYS_MODE
    PUSH    {r0-r12, r14}

    // Push the critical nesting count. (not used)
    MOV     r1, #0
    PUSH    {r1}

    // Save the small floating point context.
    VMRS    r1, FPSCR
    VPUSH   {d8-d15}

    // Save {context_type, FPSCR}
    MOV     r0, #CONTEXT_TYPE_SMALLFP
    PUSH    {r0, r1}

    // Save the stack pointer in the TCB.
    LDR     r0, pxCurrentTCBConst
    LDR     r1, [r0]
    STR     sp, [r1]
    .endm

// ====================================================================
// Save full context
// ====================================================================
.macro portSAVE_CONTEXT_FULL
    SRSDB   sp!, #SYS_MODE      // {SPSR, lr_irq} -> sp_sys

    CPS     #SYS_MODE
    PUSH    {r0-r12, r14}

    // Push the critical nesting count. (not used)
    MOV     r1, #0
    PUSH    {r1}

    // Save the full floating point context.
    VMRS    r1, FPSCR
    VPUSH   {d16-d31}
    VPUSH   {d0-d15}

    // Save {context_type, FPSCR}
    MOV     r0, #CONTEXT_TYPE_FULLFP
    PUSH    {r0, r1}

    // Save the stack pointer in the TCB.
    LDR     r0, pxCurrentTCBConst
    LDR     r1, [r0]
    STR     sp, [r1]
    .endm

// ====================================================================
// Restore context, either small or full
// ====================================================================
.macro portRESTORE_CONTEXT
    // Set the SP to point to the stack of the task being restored.
    LDR     r0, pxCurrentTCBConst
    LDR     r1, [r0]
    LDR     sp, [r1]

    POP     {r0, r1}            // {context_type, FPSCR}

    // Restore the floating point context
    CMP     r0, #CONTEXT_TYPE_SMALLFP
    VPOPEQ  {d8-d15}
    VPOPNE  {d0-d15}
    VPOPNE  {d16-d31}
    VMSR    FPSCR, r1

    // Restore the critical section nesting depth. (not used)
    POP     {r1}

    POP     {r0-r12, r14}

    CLREX

    // Return to the task code, loading CPSR on the way.
    RFEIA   sp!
    .endm

// ====================================================================
// SVC Handler
// ====================================================================
    .global SVC_Handler
    .align 5
    .type SVC_Handler, %function
    .func
SVC_Handler:
    portSAVE_CONTEXT_SMALL

    // Align down sp to 8 bytes aligned. Due to portRESTORE_CONTEXT will
    // load new sp, it is not needed to restore it.
    MOV     r0, sp
    BIC     sp, r0, #7

    LDR     r0, vTaskSwitchContextConst
    BLX     r0

    portRESTORE_CONTEXT

    .size   SVC_Handler, .-SVC_Handler
    .endfunc

// ====================================================================
// IRQ Handler
// ====================================================================
    .global IRQ_Handler
    .align 5
    .type IRQ_Handler, %function
    .func
IRQ_Handler:
    SUB     lr, lr, #4          // The real interrupted instruction
    SRSDB   sp!, #IRQ_MODE      // {SPSR, lr_irq} -> sp_irq

    CPS     #SVC_MODE           // Change to SVC mode to allow reentry
    PUSH    {r0-r4, r12, lr}    // Push registers to sp_svc

    // Save floating point context to svc stack. d8-d15 will be preserved
    // inside interrupt handler.
    VMRS    r0, FPSCR
    VPUSH   {d0-d7}             // Push d0-d7, d8-d15 won't be changed by ABI
    VPUSH   {d16-d31}           // Push d16-d31
    PUSH    {r0}                // Push FPSCR

    // Increment nesting count. r1, r3 will be used later.
    LDR     r3, ulPortInterruptNestingConst
    LDR     r1, [r3]
    ADD     r4, r1, #1
    STR     r4, [r3]

    /// Read value from the interrupt acknowledge register, r0 will be used later
    LDR     r2, ulICCIARConst
    LDR     r2, [r2]
    LDR     r0, [r2]

    // Align down sp to 8 bytes aligned, r2 will be used later.
    MOV     r2, sp
    BIC     sp, r2, #7

    // Call the interrupt handler.
    PUSH    {r0, r1, r2, r3}
    MOV     lr, #2      /* bogus address to make gdb happy */
    LDR     r1, vApplicationIRQHandlerConst
    BLX     r1
    POP     {r0, r1, r2, r3}

    // Restore the sp before alignment.
    MOV     sp, r2

    // Restore floating point context
    POP     {r2}
    VPOP    {d16-d31}
    VPOP    {d0-d7}
    VMSR    FPSCR, r2

    CPSID   i
    DSB
    ISB

    // Write the value read from ICCIAR to ICCEOIR.
    LDR     r4, ulICCEOIRConst
    LDR     r4, [r4]
    STR     r0, [r4]

    // Restore the old nesting count.
    STR     r1, [r3]

    // A context switch is never performed if the nesting count is not 0.
    CMP     r1, #0
    BNE     $L_exit_without_switch

    // Check whether context switch needed. r1 will be used later
    LDR     r1, =ulPortYieldRequired
    LDR     r0, [r1]
    CMP     r0, #0
    BNE     $L_switch_before_exit

$L_exit_without_switch:
    POP     {r0-r4, r12, lr}
    CPS     #IRQ_MODE
    RFEIA   sp!

$L_switch_before_exit:
    // Clear the context switch pending
    MOV     r0, #0
    STR     r0, [r1]

    POP     {r0-r4, r12, lr}
    CPS     #IRQ_MODE

    // Restore LR_irq and SPSR from IRQ stack, which will be stored to
    // thread stack later. No intermediate registers can be used.
    LDR     lr, [sp, #4]
    MSR     SPSR_cxsf, lr
    LDR     lr, [sp, #0]
    ADD     sp, sp, #8

    portSAVE_CONTEXT_FULL

    // Align down sp to 8 bytes aligned. Due to portRESTORE_CONTEXT will
    // load new sp, it is not needed to restore it.
    MOV     r0, sp
    BIC     sp, r0, #7

    LDR     r0, vTaskSwitchContextConst
    BLX     r0

    // Restore thread context
    portRESTORE_CONTEXT

    .size   IRQ_Handler, .-IRQ_Handler
    .endfunc

// ====================================================================
// Set memory by word. r2 is the value
// ====================================================================
.macro MEM_SET start_sym, end_sym
    LDR     r0, =\start_sym
    LDR     r1, =\end_sym
1:
    CMP     r0, r1
    BGE     2f
    STR     r2, [r0]
    ADD     r0, #4
    B       1b
2:
    .endm

// ====================================================================
// Start the scheduler
// ====================================================================
    .global vPortRestoreTaskContext
    .type vPortRestoreTaskContext, %function
    .func
vPortRestoreTaskContext:
    LDR     r2, =#0xa5a5a5a5
    MEM_SET __irq_stack_start, __irq_stack_end
    MEM_SET __svc_stack_start, __svc_stack_end

    CPS     #SYS_MODE
    portRESTORE_CONTEXT

    .size   vPortRestoreTaskContext, .-vPortRestoreTaskContext
    .endfunc

// ====================================================================
// YIELD with svc
// ====================================================================
    .global portYIELD
    .type portYIELD, %function
    .func
portYIELD:
    MRS     r3, CPSR
    AND     r3, r3, #0x1f
    CMP     r3, #USR_MODE
    BEQ     1f
    CMP     r3, #SYS_MODE
    BEQ     1f

    LDR     r3, =ulPortYieldRequired
    MOV     r2, #1
    STR     r2, [r3]
    BX      lr
1:
    SWI     0
    BX      lr

    .size   portYIELD, .-portYIELD
    .endfunc

ulICCIARConst:  .word ulICCIAR
ulICCEOIRConst: .word ulICCEOIR
pxCurrentTCBConst: .word pxCurrentTCB
vTaskSwitchContextConst: .word vTaskSwitchContext
vApplicationIRQHandlerConst: .word vApplicationIRQHandler
ulPortInterruptNestingConst: .word ulPortInterruptNesting

.end
