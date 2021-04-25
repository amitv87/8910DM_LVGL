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

#ifndef _HAL_CHIP_H_
#define _HAL_CHIP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_config.h"
#include "osi_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_CHIP_ID_UNKNOWN (0xffff0000)
#define HAL_CHIP_ID_8955 (0x89550000)
#define HAL_CHIP_ID_8909 (0x89090000)
#define HAL_CHIP_ID_8910 (0x89100000)

#define HAL_FREQ_16K (16384)
#define HAL_FREQ_32K (32768)
#define HAL_FREQ_1M (1000000)
#define HAL_FREQ_2M (2000000)
#define HAL_FREQ_6P5M (6500000)
#define HAL_FREQ_13M (13 * 1000000)
#define HAL_FREQ_20M (20 * 1000000)
#define HAL_FREQ_26M (26 * 1000000)
#define HAL_FREQ_39M (39 * 1000000)
#define HAL_FREQ_45M (45 * 1000000)
#define HAL_FREQ_52M (52 * 1000000)
#define HAL_FREQ_57M (57 * 1000000)
#define HAL_FREQ_62M (62 * 1000000)
#define HAL_FREQ_69M (69 * 1000000)
#define HAL_FREQ_78M (78 * 1000000)
#define HAL_FREQ_89M (89 * 1000000)
#define HAL_FREQ_104M (104 * 1000000)
#define HAL_FREQ_113M (113 * 1000000)
#define HAL_FREQ_125M (125 * 1000000)
#define HAL_FREQ_139M (139 * 1000000)
#define HAL_FREQ_156M (156 * 1000000)
#define HAL_FREQ_178M (178 * 1000000)
#define HAL_FREQ_208M (208 * 1000000)
#define HAL_FREQ_250M (250 * 1000000)
#define HAL_FREQ_312M (312 * 1000000)

#define HAL_NAME_DMA OSI_MAKE_TAG('D', 'M', 'A', ' ')
#define HAL_NAME_LTE_F8 OSI_MAKE_TAG('L', 'T', 'F', '8')
#define HAL_NAME_SIM OSI_MAKE_TAG('S', 'I', 'M', ' ')
#define HAL_NAME_XCV OSI_MAKE_TAG('X', 'C', 'V', ' ')
#define HAL_NAME_CALIB OSI_MAKE_TAG('C', 'A', 'L', 'I')
#define HAL_NAME_PAL OSI_MAKE_TAG('P', 'A', 'L', ' ')
#define HAL_NAME_USB OSI_MAKE_TAG('U', 'S', 'B', ' ')
#define HAL_NAME_NETCARD OSI_MAKE_TAG('N', 'E', 'T', 'C')
#define HAL_NAME_CAM OSI_MAKE_TAG('C', 'A', 'M', ' ')
#define HAL_NAME_PWM OSI_MAKE_TAG('P', 'W', 'M', ' ')

#define REG_APPLY_TYPE_END 0
#define REG_APPLY_TYPE_UDELAY 1

#define REG_APPLY_WRITE(address, value) (uint32_t)(address), (uint32_t)(value)
#define REG_APPLY_UDELAY(us) (uint32_t)(REG_APPLY_TYPE_UDELAY), (uint32_t)(us)
#define REG_APPLY_END REG_APPLY_TYPE_END

#define HAL_FLASH_OFFSET(address) ((unsigned)(address)&0xffffff)
#define HAL_FLASH_DEVICE_NAME(address) HAL_CHIP_FLASH_DEVICE_NAME((uintptr_t)(address))

#define HAL_ADDR_IS_ADI(address) HAL_CHIP_ADDR_IS_ADI((uintptr_t)(address))
#define HAL_ADDR_IS_SRAM(address) HAL_CHIP_ADDR_IS_SRAM((uintptr_t)(address))
#define HAL_ADDR_IS_RAM(address) HAL_CHIP_ADDR_IS_RAM((uintptr_t)(address))
#define HAL_ADDR_IS_SRAM_RAM(address) HAL_CHIP_ADDR_IS_SRAM_RAM((uintptr_t)(address))

enum
{
    HAL_CLK_DIV1P0 = 2,
    HAL_CLK_DIV1P5 = 3,
    HAL_CLK_DIV2P0 = 4,
    HAL_CLK_DIV2P5 = 5,
    HAL_CLK_DIV3P0 = 6,
    HAL_CLK_DIV3P5 = 7,
    HAL_CLK_DIV4P0 = 7,
    HAL_CLK_DIV4P5 = 9,
    HAL_CLK_DIV5P0 = 10,
    HAL_CLK_DIV5P5 = 11,
    HAL_CLK_DIV6P0 = 12,
    HAL_CLK_DIV7P0 = 14,
    HAL_CLK_DIV8P0 = 16,
    HAL_CLK_DIV12P0 = 24,
    HAL_CLK_DIV16P0 = 32,
    HAL_CLK_DIV24P0 = 48,
};

enum
{
    HAL_METAL_ID_U01 = 1,
    HAL_METAL_ID_U02,
    HAL_METAL_ID_U03,
    HAL_METAL_ID_U04,
};

#define HAL_METAL_BEFORE(metal, n) ((metal) < (n))
#define HAL_METAL_FROM(metal, n) ((metal) >= (n))

enum
{
    HAL_VCORE_USER_SYS,
    HAL_VCORE_USER_VOC,
    HAL_VCORE_USER_COUNT,
};

enum
{
    HAL_VCORE_LEVEL_LOW,
    HAL_VCORE_LEVEL_MEDIUM,
    HAL_VCORE_LEVEL_HIGH,
    HAL_VCORE_LEVEL_COUNT,
};

enum
{
    HAL_VCORE_MODE_LDO,
    HAL_VCORE_MODE_DCDC
};

enum
{
    HAL_POWER_DCDC_GEN = OSI_MAKE_TAG('D', 'C', 'D', 'C'),
    HAL_POWER_CORE = OSI_MAKE_TAG('C', 'O', 'R', 'E'),
    HAL_POWER_BUCK_PA = OSI_MAKE_TAG('D', 'C', 'P', 'A'),
    HAL_POWER_SPIMEM = OSI_MAKE_TAG('S', 'P', 'I', 'M'),
    HAL_POWER_MEM = OSI_MAKE_TAG('M', 'E', 'M', ' '),
    HAL_POWER_AVDD18 = OSI_MAKE_TAG('A', 'V', '1', '8'),
    HAL_POWER_RF15 = OSI_MAKE_TAG('R', 'F', '1', '5'),
    HAL_POWER_WCN = OSI_MAKE_TAG('W', 'C', 'N', ' '),
    HAL_POWER_DDR12 = OSI_MAKE_TAG('D', 'D', 'R', '1'),
    HAL_POWER_CAMD = OSI_MAKE_TAG('C', 'A', 'M', 'D'),
    HAL_POWER_CAMA = OSI_MAKE_TAG('C', 'A', 'M', 'A'),
    HAL_POWER_USB = OSI_MAKE_TAG('U', 'S', 'B', ' '),
    HAL_POWER_LCD = OSI_MAKE_TAG('L', 'C', 'D', ' '),
    HAL_POWER_SD = OSI_MAKE_TAG('S', 'D', ' ', ' '),
    HAL_POWER_SIM0 = OSI_MAKE_TAG('S', 'I', 'M', '0'),
    HAL_POWER_SIM1 = OSI_MAKE_TAG('S', 'I', 'M', '1'),
    HAL_POWER_VIBR = OSI_MAKE_TAG('V', 'I', 'B', 'R'),
    HAL_POWER_DCXO = OSI_MAKE_TAG('D', 'C', 'X', 'O'),
    HAL_POWER_VDD28 = OSI_MAKE_TAG('V', 'D', 'D', '2'),
    HAL_POWER_VIO18 = OSI_MAKE_TAG('V', 'I', 'O', '1'),
    HAL_POWER_KEYLED = OSI_MAKE_TAG('K', 'E', 'Y', 'L'),
    HAL_POWER_BACK_LIGHT = OSI_MAKE_TAG('B', 'A', 'C', 'K'),
    HAL_POWER_ANALOG = OSI_MAKE_TAG('A', 'N', 'A', 'L'),
    HAL_POWER_CAMFLASH = OSI_MAKE_TAG('C', 'A', 'M', 'F'),
    HAL_POWER_VBAT_RF = OSI_MAKE_TAG('V', 'B', 'R', 'F'),
};

typedef enum
{
    HAL_SIM_VOLT_CLASS_A = 0, // 5v
    HAL_SIM_VOLT_CLASS_B = 1, // 3.3v
    HAL_SIM_VOLT_CLASS_C = 2, // 1.8v
    HAL_SIM_VOLT_OFF = 3,
} halSimVoltClass_t;

typedef enum
{
    SENSOR_VDD_3300MV = 3300,
    SENSOR_VDD_3000MV = 3000,
    SENSOR_VDD_2800MV = 2800,
    SENSOR_VDD_2500MV = 2500,
    SENSOR_VDD_1800MV = 1800,
    SENSOR_VDD_1500MV = 1500,
    SENSOR_VDD_1300MV = 1300,
    SENSOR_VDD_1200MV = 1200,
    SENSOR_VDD_CLOSED,
    SENSOR_VDD_UNUSED
} cameraVDD_t;

typedef enum
{
    WCN_VDD_1800MV = 1800,
    WCN_VDD_1700MV = 1700,
    WCN_VDD_1600MV = 1600,
    WCN_VDD_1500MV = 1500, //default
    WCN_VDD_1400MV = 1400,
    WCN_VDD_1300MV = 1300,
    WCN_VDD_1200MV = 1200,
    WCN_VDD_1100MV = 1100,
    WCN_VDD_UNUSED
} wcnVDD_t;

typedef enum
{
    POWER_LEVEL_3200MV = 3200,
    POWER_LEVEL_3100MV = 3100,
    POWER_LEVEL_3000MV = 3000,
    POWER_LEVEL_2900MV = 2900,
    POWER_LEVEL_2800MV = 2800,
    POWER_LEVEL_2700MV = 2700,
    POWER_LEVEL_2600MV = 2600,
    POWER_LEVEL_2500MV = 2500,
    POWER_LEVEL_2400MV = 2400,
    POWER_LEVEL_2300MV = 2300,
    POWER_LEVEL_2200MV = 2200,
    POWER_LEVEL_2100MV = 2100,
    POWER_LEVEL_2000MV = 2000,
    POWER_LEVEL_1900MV = 1900,
    POWER_LEVEL_1800MV = 1800,
    POWER_LEVEL_1700MV = 1700,
    POWER_LEVEL_1600MV = 1600,
    POWER_LEVEL_1500MV = 1500,
    POWER_LEVEL_1400MV = 1400,
    POWER_LEVEL_1300MV = 1300,
    POWER_LEVEL_1200MV = 1200,
    POWER_LEVEL_1100MV = 1100,
    POWER_LEVEL_UNUSED
} powerLevel_t;

/**
 * \brief read chip id
 *
 * The returned chip id is not the raw data of hardware registers, it is the
 * *logical* chip id.
 *
 * Due to the method to detect chip id is different among various chips, the
 * implementation depends on chip. When the hardware register is unexpected,
 * it returns \p HAL_CHIP_ID_UNKNOWN.
 *
 * \return  *logical* chip id
 */
uint32_t halGetChipId(void);

/**
 * \brief read chip metal id
 *
 * When the hardware register is unexpected, the returned value is the value
 * the latest metal id. For example, when current revision is \p U02, all
 * unexpected values will be regarded as \p U02.
 *
 * When it is needed to distinguish metal id, **never** use the raw hardware
 * register, and always use macros for metal id check. Examples:
 *
 * - HAL_METAL_BEFORE(metal, HAL_METAL_ID_U02): true for metal id *less than*
 *   U02, and not including U02.
 * - HAL_METAL_FROM(metal, HAL_METAL_ID_U02): true for metal id
 *   *larger or equal than* U02, and including U02.
 *
 * \return  *logical* metal id
 */
uint32_t halGetMetalId(void);

/**
 * \brief bare metal boot initialization
 *
 * This will be called before run time initialization. Only the small piece of
 * codes are copied from flash into SRAM, namely *.sramboottext* section. All
 * *.data* and *.bss* sections are not initialized. Typically, it can access:
 *
 * - stack
 * - flash codes
 * - osiDebugEvent, osiDelayUS, halApplyRegisters, halApplyRegisterList
 */
void halChipPreBoot(void);

/**
 * \brief clock and memory initialization
 *
 * When this is called, only partial run time is initialized. All sections on
 * SRAM can be accessed, and sections on external RAM (PSRAM or DDR) can't be
 * accessed.
 */
void halClockInit(void);

/**
 * \brief PSRAM/DDR initialization at cold boot
 */
void halRamInit(void);

/**
 * \brief PSRAM/DDR initialization at resume
 *
 * This is called when system is power off, power of PSRAM is kept. Also,
 * the content of RAM should be kept.
 */
void halRamWakeInit(void);

/**
 * \brief PSRAM/DDR suspend
 *
 * When PSRAM half sleep is supported, it will be processed inside.
 */
void halRamSuspend(void);

/**
 * \brief PSRAM/DDR suspend abort
 */
void halRamSuspendAbort(void);

/**
 * \brief batch write write registers, and executed on SRAM
 *
 * This function should be located in *.sramboottext* section, and the section
 * will be loaded to SRAM at the beginning of boot.
 *
 * The main purpose of this is to write registers which may affect memory
 * access. For example, it will make flash unaccessible to write some flash
 * controller registers.
 *
 * It use variadic variables. The variadic variable format is:
 *
 * - (address, value): write the value to specified register
 * - (REG_APPLY_TYPE_UDELAY, value): delay specified microseconds
 * - (REG_APPLY_TYPE_END): indicate the end of variadic variables
 *
 * \param [in] address  register address, or REG_APPLY_TYPE_UDELAY,
 *                      REG_APPLY_TYPE_END
 */
void halApplyRegisters(uint32_t address, ...);

/**
 * \brief batch write write registers, and executed on SRAM
 *
 * This function should be located in *.sramboottext* section, and the section
 * will be loaded to SRAM at the beginning of boot.
 *
 * It is similar to \p halApplyRegisters, just the parameters are located in
 * memory pointed by \p data.
 *
 * \param [in ] data    batch write parameter pointer
 */
void halApplyRegisterList(const uint32_t *data);

/**
 * \brief calculate 24bits divider
 *
 * The divider is: output = (input * num) / denom
 * - [9:0]: num
 * - [23:10]: denom
 *
 * It is used in UART divider calculation.
 *
 * \param input     input clock frequency
 * \param output    output clock frequency
 * \return
 *      - caclulated divider
 *      - 0 on failed
 */
unsigned halCalcDivider24(unsigned input, unsigned output);

/**
 * \brief output frequency of 24bits divider
 *
 * \param input     input clock frequency
 * \param divider   divider setting
 * \result  output frequency
 */
unsigned halDivider24OutFreq(unsigned input, unsigned divider);

/**
 * \brief calculate 20bits divider
 *
 * The divider is: output = input / (div * set)
 * - [15:0]: div - 1, div >= 2
 * - [19:16]: set - 1, set >= 6
 *
 * It is better to choose larger set. And it is used in ARM UART divider
 * calculation.
 *
 * \param input     input clock frequency
 * \param output    output clock frequency
 * \return
 *      - caclulated divider
 *      - 0 on failed
 */
unsigned halCalcDivider20(unsigned input, unsigned output);

/**
 * \brief output frequency of 20bits divider
 *
 * \param input     input clock frequency
 * \param divider   divider setting
 * \result  output frequency
 */
unsigned halDivider20OutFreq(unsigned input, unsigned divider);

/**
 * \brief calculate half divider
 *
 * Half divider is 4 bits, and the real divider is:
 *  1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5
 *  5.0, 5.5, 6.0, 7.0, 8.0, 12.0, 16.0, 24.0
 *
 * \return half divider setting
 */
unsigned halCalHalfDivider(unsigned input, unsigned output);

/**
 * \brief output frequency of half divider
 *
 * \param input     input clock frequency
 * \param divider   divider setting
 * \result  output frequency
 */
unsigned halHalfDividerOutFreq(unsigned input, unsigned divider);

/**
 * \brief power on external flash
 *
 * The implementation may be empty. It should be implemented according to
 * board connection and flash type.
 *
 * When external flash isn't supported, or CONFIG_BOARD_WITH_EXT_FLASH is
 * not defined, it should be implemented as empty.
 */
void halPmuExtFlashPowerOn(void);

/**
 * \brief configure sys_wdt
 *
 * sys_wdt is used to generate interrupt, and system will come to blue
 * screen on this interrupt. It won't reset system, and pmic_wdt will
 * reset system.
 *
 * The main purpose is to monitor system hang, for example, dead loop in
 * ISR. And context will be saved in blue screen.
 *
 * It should be called before \p halSysWdtStart.
 *
 * In 8910, SYS_IRQ_ID_TIMER_1_OS will be configured as FIQ, and it is
 * used for sys_wdt. In 8811, hwp_sysWdt is used.
 *
 * \param intr_ms timeout to generate sys_wdt interrupt
 */
void halSysWdtConfig(unsigned intr_ms);

/**
 * \brief start or restart sys_wdt
 *
 * On restart, sys_wdt counting will be cleared.
 */
void halSysWdtStart(void);

/**
 * \brief stop sys_wdt
 */
void halSysWdtStop(void);

uint32_t halFreqToPllDivider(uint32_t freq);
void hal_SysSetBBClock(uint32_t freq);
void hal_SysStartBcpu(void *bcpu_main, void *stack_start_addr);

void halChangeSysClk(uint32_t freq, bool extram_access);

void halIspiInit(void);
uint16_t halPmuRead(volatile uint32_t *reg);
void halPmuWrite(volatile uint32_t *reg, uint16_t value);
uint16_t halAbbRead(volatile uint32_t *reg);
void halAbbWrite(volatile uint32_t *reg, uint16_t value);

void halPmuInit(void);

uint32_t halGetVcoreMode(void);
uint32_t halGetVcoreLevel(void);
uint8_t halRequestVcoreRegs(uint32_t id, uint32_t level, uint16_t *addr, uint16_t *value);

bool halPmuSwitchPower(uint32_t id, bool enabled, bool lp_enabled);
bool halPmuSetPowerLevel(uint32_t id, uint32_t mv);
bool halPmuSetCamFlashLevel(uint8_t level);

void halPmuEnterPm1(void);
void halPmuAbortPm1(void);
void halPmuExitPm1(void);

void halPmuEnterPm2(void);
void halPmuAbortPm2(void);
void halPmuExitPm2(void);

bool halPmuSetSimVolt(int idx, halSimVoltClass_t volt);
bool halPmuSelectSim(int idx);

OSI_NO_RETURN void halShutdown(int mode, int64_t wake_uptime);
void halPmuSet7sReset(bool enable);

#ifdef CONFIG_SOC_8955
#include "8955/hal_chip_8955.h"
#elif defined(CONFIG_SOC_8909)
#include "8909/hal_chip_8909.h"
#elif defined(CONFIG_SOC_8910)
#include "8910/hal_chip_8910.h"
#endif

#ifdef __cplusplus
}
#endif
#endif
