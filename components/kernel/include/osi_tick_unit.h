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

#ifndef _OSI_TICK_UNIT_H_
#define _OSI_TICK_UNIT_H_

#include "kernel_config.h" // CONFIG_KERNEL_HWTICK_FREQ

/**
 * \brief tick unit conversion with 64bits calculation
 *
 * It is implemented as a macro to be able to be used in const initializer.
 * Common divider \p cdiv is helpful to avoid intermediate overflow.
 *
 * \param t source tick
 * \param sfreq source tick frequency
 * \param dfres destination tick frequency
 * \param cdiv common divider of source and destination frequency
 * \return destination tick
 */
#define OSI_TICK_CONVERT(t, sfreq, dfreq, cdiv) ((int64_t)__OSI_TICK_CONVERT((int64_t)(t), (unsigned)(sfreq), (unsigned)(dfreq), (unsigned)(cdiv)))

/**
 * \brief tick unit conversion with 32bits calculation
 *
 * It is implemented as a macro to be able to be used in const initializer.
 * Common divider \p cdiv is helpful to avoid intermediate overflow. It will
 * only be used when performance is really cared.
 *
 * \param t source tick
 * \param sfreq source tick frequency
 * \param dfres destination tick frequency
 * \param cdiv common divider of source and destination frequency
 * \return destination tick
 */
#define OSI_TICK_CONVERT_U32(t, sfreq, dfreq, cdiv) ((uint32_t)__OSI_TICK_CONVERT((uint32_t)(t), (unsigned)(sfreq), (unsigned)(dfreq), (unsigned)(cdiv)))

/**
 * \brief various tick unit conversion with 64bits calculation
 */
#define OSI_TICK16K_TO_MS(t) OSI_TICK_CONVERT(t, 16384, 1000, 8)
#define OSI_MS_TO_TICK16K(t) OSI_TICK_CONVERT(t, 1000, 16384, 8)
#define OSI_TICK16K_TO_US(t) OSI_TICK_CONVERT(t, 16384, 1000000, 64)
#define OSI_US_TO_TICK16K(t) OSI_TICK_CONVERT(t, 1000000, 16384, 64)
#define OSI_TICK32K_TO_MS(t) OSI_TICK_CONVERT(t, 32768, 1000, 8)
#define OSI_MS_TO_TICK32K(t) OSI_TICK_CONVERT(t, 1000, 32768, 8)

#define OSI_HWTICK_TO_SECOND(t) OSI_TICK_CONVERT(t, CONFIG_KERNEL_HWTICK_FREQ, 1, 1)
#define OSI_SECOND_TO_HWTICK(t) OSI_TICK_CONVERT(t, 1, CONFIG_KERNEL_HWTICK_FREQ, 1)
#define OSI_HWTICK_TO_MS(t) OSI_TICK_CONVERT(t, CONFIG_KERNEL_HWTICK_FREQ, 1000, __OSI_HWTICK_CMN_DIV_MS)
#define OSI_MS_TO_HWTICK(t) OSI_TICK_CONVERT(t, 1000, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_MS)
#define OSI_HWTICK_TO_US(t) OSI_TICK_CONVERT(t, CONFIG_KERNEL_HWTICK_FREQ, 1000000, __OSI_HWTICK_CMN_DIV_US)
#define OSI_US_TO_HWTICK(t) OSI_TICK_CONVERT(t, 1000000, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_US)
#define OSI_HWTICK_TO_16K(t) OSI_TICK_CONVERT(t, CONFIG_KERNEL_HWTICK_FREQ, 16384, __OSI_HWTICK_CMN_DIV_16K)
#define OSI_16K_TO_HWTICK(t) OSI_TICK_CONVERT(t, 16384, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_16K)
#define OSI_HWTICK_TO_32K(t) OSI_TICK_CONVERT(t, CONFIG_KERNEL_HWTICK_FREQ, 32768, __OSI_HWTICK_CMN_DIV_32K)
#define OSI_32K_TO_HWTICK(t) OSI_TICK_CONVERT(t, 32768, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_32K)

/**
 * \brief various tick unit conversion with 32bits calculation
 */
#define OSI_TICK16K_TO_MS_U32(t) OSI_TICK_CONVERT_U32(t, 16384, 1000, 8)
#define OSI_MS_TO_TICK16K_U32(t) OSI_TICK_CONVERT_U32(t, 1000, 16384, 8)
#define OSI_TICK16K_TO_US_U32(t) OSI_TICK_CONVERT_U32(t, 16384, 1000000, 64)
#define OSI_US_TO_TICK16K_U32(t) OSI_TICK_CONVERT_U32(t, 1000000, 16384, 64)
#define OSI_TICK32K_TO_MS_U32(t) OSI_TICK_CONVERT_U32(t, 32768, 1000, 8)
#define OSI_MS_TO_TICK32K_U32(t) OSI_TICK_CONVERT_U32(t, 1000, 32768, 8)

#define OSI_HWTICK_TO_MS_U32(t) OSI_TICK_CONVERT_U32(t, CONFIG_KERNEL_HWTICK_FREQ, 1000, __OSI_HWTICK_CMN_DIV_MS)
#define OSI_MS_TO_HWTICK_U32(t) OSI_TICK_CONVERT_U32(t, 1000, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_MS)
#define OSI_HWTICK_TO_US_U32(t) OSI_TICK_CONVERT_U32(t, CONFIG_KERNEL_HWTICK_FREQ, 1000000, __OSI_HWTICK_CMN_DIV_US)
#define OSI_US_TO_HWTICK_U32(t) OSI_TICK_CONVERT_U32(t, 1000000, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_US)
#define OSI_HWTICK_TO_16K_U32(t) OSI_TICK_CONVERT_U32(t, CONFIG_KERNEL_HWTICK_FREQ, 16384, __OSI_HWTICK_CMN_DIV_16K)
#define OSI_16K_TO_HWTICK_U32(t) OSI_TICK_CONVERT_U32(t, 16384, CONFIG_KERNEL_HWTICK_FREQ, __OSI_HWTICK_CMN_DIV_16K)

// ============================================================================
// Implementation
// ============================================================================

#define __OSI_TICK_CONVERT(t, sfreq, dfreq, cdiv) (((t) * ((dfreq) / (cdiv))) / ((sfreq) / (cdiv)))

#if (CONFIG_KERNEL_HWTICK_FREQ == 16384)
#define __OSI_HWTICK_CMN_DIV_MS (8)
#define __OSI_HWTICK_CMN_DIV_US (64)
#define __OSI_HWTICK_CMN_DIV_16K (16384)
#define __OSI_HWTICK_CMN_DIV_32K (16384)
#endif

#if (CONFIG_KERNEL_HWTICK_FREQ == 2000000)
#define __OSI_HWTICK_CMN_DIV_MS (1000)
#define __OSI_HWTICK_CMN_DIV_US (1000000)
#define __OSI_HWTICK_CMN_DIV_16K (128)
#define __OSI_HWTICK_CMN_DIV_32K (128)
#endif

#if (CONFIG_KERNEL_HWTICK_FREQ == 1000000)
#define __OSI_HWTICK_CMN_DIV_MS (1000)
#define __OSI_HWTICK_CMN_DIV_US (1000000)
#define __OSI_HWTICK_CMN_DIV_16K (64)
#define __OSI_HWTICK_CMN_DIV_32K (64)
#endif

#endif
