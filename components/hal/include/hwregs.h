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

#ifndef _HWREGS_H_
#define _HWREGS_H_

#include <stdint.h>
#include "hal_config.h"
#include "hwreg_access.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_SOC_8910)
#include "8910/hal_irq_priority.h"
#include "hwregs/8910/globals.h"
#include "hwregs/8910/gallite_generic_config.h"
#include "hwregs/8910/adi_mst.h"
#include "hwregs/8910/analog_reg.h"
#include "hwregs/8910/aon_ifc.h"
#include "hwregs/8910/ap_ifc.h"
#include "hwregs/8910/arm_uart.h"
#include "hwregs/8910/arm_axidma.h"
#include "hwregs/8910/cp_clkrst.h"
#include "hwregs/8910/cp_idle.h"
#include "hwregs/8910/cp_mailbox.h"
#include "hwregs/8910/cp_pwrctrl.h"
#include "hwregs/8910/cp_sysreg.h"
#include "hwregs/8910/debug_uart.h"
#include "hwregs/8910/camera.h"
#include "hwregs/8910/debug_host.h"
#include "hwregs/8910/dmc400.h"
#include "hwregs/8910/efuse_ctrl.h"
#include "hwregs/8910/gpio.h"
#include "hwregs/8910/i2c_master.h"
#include "hwregs/8910/spi.h"
#include "hwregs/8910/pwm.h"
#include "hwregs/8910/iomux.h"
#include "hwregs/8910/keypad.h"
#include "hwregs/8910/lpddr_phy.h"
#include "hwregs/8910/lzma.h"
#include "hwregs/8910/psram_phy.h"
#include "hwregs/8910/rf_registers.h"
#include "hwregs/8910/rf_rtc.h"
#include "hwregs/8910/spinlock.h"
#include "hwregs/8910/spi_flash.h"
#include "hwregs/8910/sys_ctrl.h"
#include "hwregs/8910/sys_ifc.h"
#include "hwregs/8910/timer.h"
#include "hwregs/8910/timer_ap.h"
#include "hwregs/8910/rda2720m_adc.h"
#include "hwregs/8910/rda2720m_efs.h"
#include "hwregs/8910/rda2720m_eic.h"
#include "hwregs/8910/rda2720m_global.h"
#include "hwregs/8910/rda2720m_int.h"
#include "hwregs/8910/rda2720m_rtc.h"
#include "hwregs/8910/rda2720m_psm.h"
#include "hwregs/8910/rda2720m_wdg.h"
#include "hwregs/8910/rda2720m_bltc.h"
#include "hwregs/8910/rda2720m_aud_codec.h"
#include "hwregs/8910/rda2720m_aud.h"
#include "hwregs/8910/aif.h"
#include "hwregs/8910/aud_2ad.h"
#include "hwregs/8910/sdmmc.h"
#include "hwregs/8910/gouda.h"
#include "hwregs/8910/usbc.h"
#include "hwregs/8910/wcn_rf_if.h"
#include "hwregs/8910/wcn_wlan.h"
#include "hwregs/8910/wcn_bt_modem.h"
#include "hwregs/8910/wcn_bt_link.h"
#include "hwregs/8910/wcn_comregs.h"
#include "hwregs/8910/wcn_sys_ctrl.h"
#include "hwregs/8910/aes.h"
#elif defined(CONFIG_SOC_6760)
#include "hwregs/6760/debug_host.h"
#include "hwregs/6760/debug_uart.h"
#include "hwregs/6760/timer.h"
#include "hwregs/6760/gpio.h"
#include "hwregs/6760/iomux.h"
#elif defined(CONFIG_SOC_8955)
#include "8955/hal_irq_priority.h"
#include "hwregs/8955/abb.h"
#include "hwregs/8955/cfg_regs.h"
#include "hwregs/8955/calendar.h"
#include "hwregs/8955/debug_host.h"
#include "hwregs/8955/debug_uart.h"
#include "hwregs/8955/dma.h"
#include "hwregs/8955/gallite_generic_config.h"
#include "hwregs/8955/globals.h"
#include "hwregs/8955/gouda.h"
#include "hwregs/8955/i2c_master.h"
#include "hwregs/8955/iomux.h"
#include "hwregs/8955/pmu.h"
#include "hwregs/8955/page_spy.h"
#include "hwregs/8955/psram8_ctrl.h"
#include "hwregs/8955/rf_spi.h"
#include "hwregs/8955/spi.h"
#include "hwregs/8955/pwm.h"
#include "hwregs/8955/spi_flash.h"
#include "hwregs/8955/sys_ctrl.h"
#include "hwregs/8955/sys_ifc.h"
#include "hwregs/8955/sys_irq.h"
#include "hwregs/8955/timer.h"
#include "hwregs/8955/uart.h"
#elif defined(CONFIG_SOC_8909)
#include "8909/hal_irq_priority.h"
#include "hwregs/8909/abb.h"
#include "hwregs/8909/calendar.h"
#include "hwregs/8909/cau_dig.h"
#include "hwregs/8909/debug_host.h"
#include "hwregs/8909/debug_uart.h"
#include "hwregs/8909/dma.h"
#include "hwregs/8909/globals.h"
#include "hwregs/8909/gouda.h"
#include "hwregs/8909/gpio.h"
#include "hwregs/8909/i2c_master.h"
#include "hwregs/8909/iomux.h"
#include "hwregs/8909/lps.h"
#include "hwregs/8909/page_spy.h"
#include "hwregs/8909/pmuc.h"
#include "hwregs/8909/psram8_ctrl.h"
#include "hwregs/8909/rf_spi.h"
#include "hwregs/8909/spi.h"
#include "hwregs/8909/pwm.h"
#include "hwregs/8909/spi_flash.h"
#include "hwregs/8909/sys_ctrl.h"
#include "hwregs/8909/sys_ifc.h"
#include "hwregs/8909/sys_irq.h"
#include "hwregs/8909/tcu.h"
#include "hwregs/8909/timer.h"
#include "hwregs/8909/uart.h"
#endif

#ifdef __cplusplus
}
#endif
#endif
