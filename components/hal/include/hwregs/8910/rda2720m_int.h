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

#ifndef _RDA2720M_INT_H_
#define _RDA2720M_INT_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define REG_RDA2720M_INT_BASE (0x503080c0)

typedef volatile struct
{
    uint32_t int_mask_status; // 0x00000000
    uint32_t int_raw_status;  // 0x00000004
    uint32_t int_en;          // 0x00000008
} HWP_RDA2720M_INT_T;

#define hwp_rda2720mInt ((HWP_RDA2720M_INT_T *)REG_ACCESS_ADDRESS(REG_RDA2720M_INT_BASE))

// int_mask_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_int_mask_status : 1;         // [0], read only
        uint32_t rtc_int_mask_status : 1;         // [1], read only
        uint32_t wdg_int_mask_status : 1;         // [2], read only
        uint32_t fgu_int_mask_status : 1;         // [3], read only
        uint32_t eic_int_mask_status : 1;         // [4], read only
        uint32_t aud_protect_int_mask_status : 1; // [5], read only
        uint32_t tmr_int_mask_status : 1;         // [6], read only
        uint32_t cal_int_mask_status : 1;         // [7], read only
        uint32_t typec_int_mask_status : 1;       // [8], read only
        uint32_t __31_9 : 23;                     // [31:9]
    } b;
} REG_RDA2720M_INT_INT_MASK_STATUS_T;

// int_raw_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_int_raw_status : 1;         // [0], read only
        uint32_t rtc_int_raw_status : 1;         // [1], read only
        uint32_t wdg_int_raw_status : 1;         // [2], read only
        uint32_t fgu_int_raw_status : 1;         // [3], read only
        uint32_t eic_int_raw_status : 1;         // [4], read only
        uint32_t aud_protect_int_raw_status : 1; // [5], read only
        uint32_t tmr_int_raw_status : 1;         // [6], read only
        uint32_t cal_int_raw_status : 1;         // [7], read only
        uint32_t typec_int_raw_status : 1;       // [8], read only
        uint32_t __31_9 : 23;                    // [31:9]
    } b;
} REG_RDA2720M_INT_INT_RAW_STATUS_T;

// int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adc_int_en : 1;         // [0]
        uint32_t rtc_int_en : 1;         // [1]
        uint32_t wdg_int_en : 1;         // [2]
        uint32_t fgu_int_en : 1;         // [3]
        uint32_t eic_int_en : 1;         // [4]
        uint32_t aud_protect_int_en : 1; // [5]
        uint32_t tmr_int_en : 1;         // [6]
        uint32_t cal_int_en : 1;         // [7]
        uint32_t typec_int_en : 1;       // [8]
        uint32_t __31_9 : 23;            // [31:9]
    } b;
} REG_RDA2720M_INT_INT_EN_T;

// int_mask_status
#define RDA2720M_INT_ADC_INT_MASK_STATUS (1 << 0)
#define RDA2720M_INT_RTC_INT_MASK_STATUS (1 << 1)
#define RDA2720M_INT_WDG_INT_MASK_STATUS (1 << 2)
#define RDA2720M_INT_FGU_INT_MASK_STATUS (1 << 3)
#define RDA2720M_INT_EIC_INT_MASK_STATUS (1 << 4)
#define RDA2720M_INT_AUD_PROTECT_INT_MASK_STATUS (1 << 5)
#define RDA2720M_INT_TMR_INT_MASK_STATUS (1 << 6)
#define RDA2720M_INT_CAL_INT_MASK_STATUS (1 << 7)
#define RDA2720M_INT_TYPEC_INT_MASK_STATUS (1 << 8)

// int_raw_status
#define RDA2720M_INT_ADC_INT_RAW_STATUS (1 << 0)
#define RDA2720M_INT_RTC_INT_RAW_STATUS (1 << 1)
#define RDA2720M_INT_WDG_INT_RAW_STATUS (1 << 2)
#define RDA2720M_INT_FGU_INT_RAW_STATUS (1 << 3)
#define RDA2720M_INT_EIC_INT_RAW_STATUS (1 << 4)
#define RDA2720M_INT_AUD_PROTECT_INT_RAW_STATUS (1 << 5)
#define RDA2720M_INT_TMR_INT_RAW_STATUS (1 << 6)
#define RDA2720M_INT_CAL_INT_RAW_STATUS (1 << 7)
#define RDA2720M_INT_TYPEC_INT_RAW_STATUS (1 << 8)

// int_en
#define RDA2720M_INT_ADC_INT_EN (1 << 0)
#define RDA2720M_INT_RTC_INT_EN (1 << 1)
#define RDA2720M_INT_WDG_INT_EN (1 << 2)
#define RDA2720M_INT_FGU_INT_EN (1 << 3)
#define RDA2720M_INT_EIC_INT_EN (1 << 4)
#define RDA2720M_INT_AUD_PROTECT_INT_EN (1 << 5)
#define RDA2720M_INT_TMR_INT_EN (1 << 6)
#define RDA2720M_INT_CAL_INT_EN (1 << 7)
#define RDA2720M_INT_TYPEC_INT_EN (1 << 8)

#endif // _RDA2720M_INT_H_
