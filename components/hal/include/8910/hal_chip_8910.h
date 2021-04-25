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

#ifndef _HAL_CHIP_8910_H_
#define _HAL_CHIP_8910_H_

#include "hal_chip.h"
#include "hwregs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_RESUME_SRC_PMIC (1 << 0)
#define HAL_RESUME_SRC_VAD (1 << 1)
#define HAL_RESUME_SRC_KEY (1 << 2)
#define HAL_RESUME_SRC_GPIO1 (1 << 3)
#define HAL_RESUME_SRC_UART1 (1 << 4)
#define HAL_RESUME_SRC_UART1_RXD (1 << 5)
#define HAL_RESUME_SRC_WCN2SYS (1 << 6)
#define HAL_RESUME_SRC_WCN_OSC (1 << 7)
#define HAL_RESUME_SRC_IDLE_TIMER1 (1 << 8)
#define HAL_RESUME_SRC_IDLE_TIMER2 (1 << 9)
#define HAL_RESUME_SRC_SELF (1 << 10)
#define HAL_RESUME_SRC_USB_MON (1 << 11)

#define HAL_32KSLEEP_SRC_USB_RESUME (1 << 0)

// The followings are output by: halMmuShowDescriptors
#define HAL_DESCL1_SECTION_NORMAL_RWX 0x00005c06
#define HAL_DESCL1_SECTION_NORMAL_RW 0x00005c16
#define HAL_DESCL1_SECTION_NORMAL_RX 0x0000dc06
#define HAL_DESCL1_SECTION_NORMAL_R 0x0000dc16
#define HAL_DESCL1_SECTION_NC_RWX 0x00004c02
#define HAL_DESCL1_SECTION_NC_RW 0x00004c12
#define HAL_DESCL1_SECTION_NC_RX 0x0000cc02
#define HAL_DESCL1_SECTION_NC_R 0x0000cc12
#define HAL_DESCL1_SECTION_DEVICE_RW 0x00000c12
#define HAL_DESCL1_SECTION_DEVICE_R 0x00008c12
#define HAL_DESCL1_SECTION_NO_ACCESS 0x00000012
#define HAL_DESCL1_SECTION_PAGE64K 0x00000001
#define HAL_DESCL2_PAGE64K_NORMAL_RWX 0x00005035
#define HAL_DESCL2_PAGE64K_NORMAL_RW 0x0000d035
#define HAL_DESCL2_PAGE64K_NORMAL_RX 0x00005235
#define HAL_DESCL2_PAGE64K_NORMAL_R 0x0000d235
#define HAL_DESCL2_PAGE64K_NC_RWX 0x00004031
#define HAL_DESCL2_PAGE64K_NC_RW 0x0000c031
#define HAL_DESCL2_PAGE64K_NC_RX 0x00004231
#define HAL_DESCL2_PAGE64K_NC_R 0x0000c231
#define HAL_DESCL2_PAGE64K_DEVICE_RW 0x00008031
#define HAL_DESCL2_PAGE64K_DEVICE_R 0x00008231
#define HAL_DESCL2_PAGE64K_NO_ACCESS 0x00008001
#define HAL_DESCL1_SECTION_PAGE4K 0x00000001
#define HAL_DESCL2_PAGE4K_NORMAL_RWX 0x00000176
#define HAL_DESCL2_PAGE4K_NORMAL_RW 0x00000177
#define HAL_DESCL2_PAGE4K_NORMAL_RX 0x00000376
#define HAL_DESCL2_PAGE4K_NORMAL_R 0x00000377
#define HAL_DESCL2_PAGE4K_NC_RWX 0x00000132
#define HAL_DESCL2_PAGE4K_NC_RW 0x00000133
#define HAL_DESCL2_PAGE4K_NC_RX 0x00000332
#define HAL_DESCL2_PAGE4K_NC_R 0x00000333
#define HAL_DESCL2_PAGE4K_DEVICE_RW 0x00000033
#define HAL_DESCL2_PAGE4K_DEVICE_R 0x00000233
#define HAL_DESCL2_PAGE4K_NO_ACCESS 0x00000003

#define HAL_TIMER_CURVAL_LO (hwp_timer4->hwtimer_curval_l)
#define HAL_TIMER_CURVAL_HI (hwp_timer4->hwtimer_curval_h)
#define HAL_TIMER_16K_CURVAL (hwp_timer2->hwtimer_curval)

#define HAL_CHIP_FLASH_DEVICE_NAME(address) ({ \
    uintptr_t _p = (address);                  \
    _p &= 0xff000000;                          \
    (_p == CONFIG_NOR_PHY_ADDRESS)             \
        ? DRV_NAME_SPI_FLASH                   \
        : (_p == CONFIG_NOR_EXT_PHY_ADDRESS)   \
              ? DRV_NAME_SPI_FLASH_EXT         \
              : DRV_NAME_INVALID;              \
})

OSI_FORCE_INLINE static bool HAL_CHIP_ADDR_IS_ADI(uintptr_t p)
{
    return (p & 0xfffff000) == 0x50308000;
}

OSI_FORCE_INLINE static bool HAL_CHIP_ADDR_IS_SRAM(uintptr_t p)
{
    return p >= CONFIG_SRAM_PHY_ADDRESS &&
           p < CONFIG_SRAM_PHY_ADDRESS + CONFIG_SRAM_SIZE;
}

OSI_FORCE_INLINE static bool HAL_CHIP_ADDR_IS_RAM(uintptr_t p)
{
    return p >= CONFIG_RAM_PHY_ADDRESS &&
           p < CONFIG_RAM_PHY_ADDRESS + CONFIG_RAM_SIZE;
}

OSI_FORCE_INLINE static bool HAL_CHIP_ADDR_IS_SRAM_RAM(uintptr_t p)
{
    return HAL_CHIP_ADDR_IS_SRAM(p) || HAL_CHIP_ADDR_IS_RAM(p);
}

void halPmuUnlockPowerReg(void);

/**
 * \brief prepare PSM module
 *
 * This is called before PSM sleep or power down. PSM counter should be
 * started here. The PSM counter will be used to measure PSM duration.
 */
void halPmuPsmPrepare(void);

/**
 * \brief Config pmic Power key 7s reset funtion
 *
 * \param enable
 *      - true enable 7s reset funtion.
 *      - false disable 7s reset funtion.
 */
void halPmuSet7sReset(bool enable);

typedef enum
{
    CLK_26M_USER_AUDIO = (1 << 0),
    CLK_26M_USER_WCN = (1 << 1),
    CLK_26M_USER_ADC = (1 << 2),
    CLK_26M_USER_AUX1 = (1 << 3),
} clock26MUser_t;

typedef enum
{
    CLK_CAMA_USER_CAMERA = (1 << 0),
    CLK_CAMA_USER_AUDIO = (1 << 1),
} cameraUser_t;

// enum: mclk of chip output
typedef enum
{
    CAMA_CLK_OUT_FREQ_12M = 12,
    CAMA_CLK_OUT_FREQ_13M = 13,
    CAMA_CLK_OUT_FREQ_24M = 24,
    CAMA_CLK_OUT_FREQ_26M = 26,
    CAMA_CLK_OUT_FREQ_MAX
} cameraClk_t;

/**
 * \brief request 26M clock
 *
 * \param user  the 26M user
 */
void halClock26MRequest(clock26MUser_t user);

/**
 * \brief release 26M clock
 *
 * \param user  the 26M user
 */
void halClock26MRelease(clock26MUser_t user);

/**
 * \brief request Mclk clock for user
 *
 * \param user and clock
 */
void halCameraClockRequest(cameraUser_t user, cameraClk_t Mclk);

/**
 * \brief release clock for user
 *
 * \param user  the user
 */
void halCameraClockRelease(cameraUser_t user);

/**
 * \brief init hw aes trng module
 */
void halAesTrngInit();

/**
 * \brief reset hw aes trng module
 */
void halAesTrngReset();

/**
 * \brief start hw aes trng module
 */
void halAesTrngEnable();

/**
 * \brief get hw aes generated random data, two word generated each time
 * \param v0    data0
 * \param v1    data1
 */
void halAesTrngGetValue(uint32_t *v0, uint32_t *v1);

/**
 * \brief check hw aes trng module if has already generated random data
 * \return
 *      - true if done else false
 */
bool halAesCheckTrngComplete();

/**
 * \brief read 2720 RTC and convert to second
 *
 * \return  rtc second
 */
int64_t halPmuRtcReadSecond(void);

/**
 * \brief trigger watchdog reset immediately
 */
void halWatchdogReset(void);

#ifdef __cplusplus
}
#endif
#endif
