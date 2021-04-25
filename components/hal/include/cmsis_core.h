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

#ifndef _CMSIS_CORE_H_
#define _CMSIS_CORE_H_

#include "kernel_config.h"

#ifdef CONFIG_CPU_ARM_CA5
#define __CA_REV 0x0000U /* Core revision r0p0                            */
#define __CORTEX_A 5U    /* Cortex-A5 Core                                */
#define __FPU_PRESENT 1U /* FPU present                                   */
#define __GIC_PRESENT 1U /* GIC present                                   */
#define __TIM_PRESENT 0U /* TIM present                                   */
#define __L2C_PRESENT 0U /* L2C present                                   */

#define GIC_DISTRIBUTOR_BASE (0x08200000 + 0x1000)
#define GIC_INTERFACE_BASE (0x08200000 + 0x2000)
#define IRQ_GIC_LINE_COUNT (32 + 92)
typedef unsigned IRQn_Type;

#include "Core_A/cmsis_compiler.h"
#include "Core_A/cmsis_cp15.h"
#include "Core_A/core_ca.h"
#include "Core_A/irq_ctrl.h"
#elif defined(CONFIG_CPU_ARM_CM4F)
#define __FPU_PRESENT 1U
#define __MPU_PRESENT 0U
#define __NVIC_PRIO_BITS 5U
typedef unsigned IRQn_Type;
#include "Core_M/cmsis_compiler.h"
#include "Core_M/core_cm4.h"
#else
#endif

#endif
