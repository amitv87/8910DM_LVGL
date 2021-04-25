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

#include "osi_api.h"
#include "osi_log.h"
#include "osi_profile.h"
#include "hal_config.h"
#include "cmsis_core.h"
#include "hwregs.h"
#include <stddef.h>

#define GICD_GROUP_REG_COUNT OSI_DIV_ROUND_UP(IRQ_GIC_LINE_COUNT, 32)
#define GICD_ENABLE_REG_COUNT OSI_DIV_ROUND_UP(IRQ_GIC_LINE_COUNT, 32)
#define GICD_PRIORITY_REG_COUNT OSI_DIV_ROUND_UP(IRQ_GIC_LINE_COUNT, 4)
#define GICD_TARGET_REG_COUNT OSI_DIV_ROUND_UP(IRQ_GIC_LINE_COUNT, 4)
#define GICD_CFG_REG_COUNT OSI_DIV_ROUND_UP(IRQ_GIC_LINE_COUNT, 16)

struct osiIrqHandlerBody
{
    osiIrqHandler_t handler;
    void *ctx;
};

typedef struct
{
    uint32_t gicd_enable[GICD_ENABLE_REG_COUNT];
    uint32_t gicd_priority[GICD_PRIORITY_REG_COUNT];
    uint32_t gicd_target[GICD_TARGET_REG_COUNT];
    uint32_t gicd_cfg[GICD_CFG_REG_COUNT];
    uint32_t gicd_group[GICD_GROUP_REG_COUNT];
    uint32_t gicc_pmr;
    uint32_t gicc_bpr;
} osiIrqContext_t;

static osiIrqContext_t gOsiIrqCtx;

static struct osiIrqHandlerBody gOsiIrqBody[IRQ_GIC_LINE_COUNT] = {};

static void prvGicEnable(void)
{
    osiIrqContext_t *p = &gOsiIrqCtx;

    //Disable interrupt forwarding
    GICDistributor->CTLR &= ~(REG_BIT0 | REG_BIT1); // enablegrp0, enablegrp1

    for (int n = 1; n < GICD_ENABLE_REG_COUNT; n++) // 1 bit
        GICDistributor->ISENABLER[n] = p->gicd_enable[n];
    for (int n = 2; n < GICD_CFG_REG_COUNT; n++) // 2 bits
        GICDistributor->ICFGR[n] = p->gicd_cfg[n];
    for (int n = 8; n < GICD_PRIORITY_REG_COUNT; n++) // 8 bits
        GICDistributor->IPRIORITYR[n] = p->gicd_priority[n];
    for (int n = 8; n < GICD_TARGET_REG_COUNT; n++) // 8 bits
        GICDistributor->ITARGETSR[n] = p->gicd_target[n];
    for (int n = 1; n < GICD_GROUP_REG_COUNT; n++) // 1 bit
        GICDistributor->IGROUPR[n] = p->gicd_group[n];

    //Enable distributor
    GICDistributor->CTLR |= (REG_BIT0 | REG_BIT1); // enablegrp0, enablegrp1

    //Disable interrupt forwarding
    GICInterface->CTLR &= ~(REG_BIT0 | REG_BIT1); // enablegrp0, enablegrp1

    for (int n = 0; n < 2; n++) // 2 bits
        GICDistributor->ICFGR[n] = p->gicd_cfg[n];
    for (int n = 0; n < 1; n++) // 1 bit
        GICDistributor->ISENABLER[n] = p->gicd_enable[n];
    for (int n = 0; n < 8; n++) // 8 bits
        GICDistributor->IPRIORITYR[n] = p->gicd_priority[n];
    for (int n = 0; n < 1; n++) // 1 bit
        GICDistributor->IGROUPR[n] = p->gicd_group[n];

    //Enable interface
    GICInterface->CTLR |= (REG_BIT0 | REG_BIT1 | REG_BIT2 | REG_BIT3 | REG_BIT4); // enablegrp0, enablegrp1, ackctl, fiqen, cbpr
    GICInterface->BPR = p->gicc_bpr;
    GICInterface->PMR = p->gicc_pmr;
}

void osiIrqSuspend(osiSuspendMode_t mode)
{
    osiIrqContext_t *p = &gOsiIrqCtx;

    for (int n = 0; n < GICD_PRIORITY_REG_COUNT; n++)
        p->gicd_priority[n] = GICDistributor->IPRIORITYR[n];
    for (int n = 0; n < GICD_TARGET_REG_COUNT; n++)
        p->gicd_target[n] = GICDistributor->ITARGETSR[n];
    for (int n = 0; n < GICD_CFG_REG_COUNT; n++)
        p->gicd_cfg[n] = GICDistributor->ICFGR[n];
    for (int n = 0; n < GICD_GROUP_REG_COUNT; n++)
        p->gicd_group[n] = GICDistributor->IGROUPR[n];

    p->gicc_pmr = GICInterface->PMR;
    p->gicc_bpr = GICInterface->BPR;
}

void osiIrqResume(osiSuspendMode_t mode, bool aborted)
{
    if (aborted)
        return;

    prvGicEnable();
}

void osiIrqInit(void)
{
    osiIrqContext_t *p = &gOsiIrqCtx;

    for (unsigned n = 0; n < IRQ_GIC_LINE_COUNT; n++)
        gOsiIrqBody[n].handler = NULL;

    for (int n = 0; n < GICD_ENABLE_REG_COUNT; n++)
        p->gicd_enable[n] = 0;
    for (int n = 0; n < GICD_CFG_REG_COUNT; n++)
        p->gicd_cfg[n] = 0;
    for (int n = 0; n < GICD_PRIORITY_REG_COUNT; n++)
        p->gicd_priority[n] = 0x80;
    for (int n = 0; n < GICD_TARGET_REG_COUNT; n++)
        p->gicd_target[n] = 0x11111111; // CPU0
    for (int n = 0; n < GICD_GROUP_REG_COUNT; n++)
        p->gicd_group[n] = 0xffffffff; // Group 1

    p->gicc_bpr = 0;
    p->gicc_pmr = 0xff;
    prvGicEnable();

    // TIMER_2_OS set to group 0, used as FIQ to trigger blue screen
    osiIrqSetGroup(HAL_SYSIRQ_NUM(SYS_IRQ_ID_TIMER_2_OS), 0);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_TIMER_2_OS));
}

bool osiIrqSetHandler(uint32_t irqn, osiIrqHandler_t handler, void *ctx)
{
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return false;

    gOsiIrqBody[irqn].handler = handler;
    gOsiIrqBody[irqn].ctx = ctx;
    return true;
}

bool osiIrqEnable(uint32_t irqn)
{
    osiIrqContext_t *p = &gOsiIrqCtx;
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return false;

    p->gicd_enable[irqn / 32] |= 1 << (irqn % 32);
    GIC_EnableIRQ(irqn);
    return true;
}

bool osiIrqDisable(uint32_t irqn)
{
    osiIrqContext_t *p = &gOsiIrqCtx;
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return false;

    p->gicd_enable[irqn / 32] &= ~(1 << (irqn % 32));
    GIC_DisableIRQ(irqn);
    return true;
}

bool osiIrqEnabled(uint32_t irqn)
{
    return (irqn < IRQ_GIC_LINE_COUNT && GIC_GetEnableIRQ(irqn));
}

bool osiIrqSetPriority(uint32_t irqn, uint32_t priority)
{
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return false;

    GIC_SetPriority(irqn, priority);
    return true;
}

uint32_t osiIrqGetPriority(uint32_t irqn)
{
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return 0x80000000U;

    return GIC_GetPriority(irqn);
}

bool osiIrqSetGroup(uint32_t irqn, uint32_t group)
{
    if (irqn >= IRQ_GIC_LINE_COUNT)
        return false;

    GIC_SetGroup(irqn, group);
    return true;
}

bool OSI_SECTION_SRAM_TEXT osiIrqPending(void)
{
    uint32_t isr = __get_ISR();
    return (isr & (CPSR_I_Msk | CPSR_F_Msk | CPSR_A_Msk)) != 0;
}

void vApplicationIRQHandler(uint32_t ulICCIAR)
{
    uint32_t irqn = ulICCIAR & 0x3FFUL;
    if (irqn >= IRQ_GIC_LINE_COUNT)
    {
        osiProfileIrqEnter(IRQ_GIC_LINE_COUNT);
        osiProfileIrqExit(IRQ_GIC_LINE_COUNT);
        return;
    }

    osiProfileIrqEnter(irqn);
    __enable_irq();

    if (gOsiIrqBody[irqn].handler != NULL)
        gOsiIrqBody[irqn].handler(gOsiIrqBody[irqn].ctx);

    osiProfileIrqExit(irqn);
}
