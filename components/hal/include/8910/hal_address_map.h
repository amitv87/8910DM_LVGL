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

static const halAddressRange_t gHwMap[] = {
    {0x00001000, 0x10000000 - 1}, // ap
    {0x10000000, 0x20000000 - 1}, // wcn
    {0x20000000, 0x30000000 - 1}, // zsp
    {0x40000000, 0x50000000 - 1}, // gge
    {0x50000000, 0x60000000 - 1}, // aon & rf
    {0x60000000, 0x70000000 - 1}, // flash0
    {0x70000000, 0x80000000 - 1}, // flash1
    {0x80000000, 0xc0000000 - 1}, // psram, ddr
};

static const halBsDumpRegion_t gBsDumpRegions[] = {
    {0x60000000, CONFIG_FLASH_SIZE, HAL_BS_REGION_FLASH}, // flash
    {0x80000000, CONFIG_RAM_SIZE, 0},                     // psram
    {0x00800000, CONFIG_SRAM_SIZE, 0},                    // sram
    {0x08201000, 0x1000, 0},                              // gic
    {0x08300000, 0x1000, HAL_BS_REGION_IGNORE},           // axi_cfg
    {0x08800000, 0x150, 0},                               // irq0
    {0x08800800, 0x150, 0},                               // irq1
    {0x08801000, 0x730, 0},                               // lpddr_phy
    {0x08801800, 0x718, 0},                               // psram_phy
    {0x08802000, 0x200, 0},                               // pagespy
    {0x08803000, 0x1000, 0},                              // dmc
    {0x08804000, 0x40, 0},                                // sys_imem
    {0x08805000, 0x4c, 0},                                // lzma
    {0x08806000, 0x148, 0},                               // gouda
    {0x08807000, 0x680, 0},                               // lcdc
    {0x08808000, 0x28, 0},                                // timer1
    {0x08809000, 0x28, 0},                                // timer2
    {0x0880a000, 0x8, 0},                                 // i1c1
    {0x0880b000, 0x8, 0},                                 // i1c3
    {0x0880d000, 0x6c, 0},                                // aud_2ad
    {0x0880e000, 0x3c, 0},                                // timer4
    {0x0880f004, 0x90, 0},                                // ap_ifc
    {0x08810000, 0x844, 0},                               // sdmmc
    {0x08811000, 0x844, 0},                               // sdmmc2
    {0x08812000, 0x8, 0},                                 // spi1
    {0x08813000, 0x8, 0},                                 // spi2
    {0x08814000, 0x34, 0},                                // sci1
    {0x08815000, 0x34, 0},                                // sci2
    {0x08816008, 0x20, HAL_BS_REGION_ZSP},                // zsp_uart
    {0x08817008, 0x2c, 0},                                // uart2
    {0x08818008, 0x2c, 0},                                // uart3
    {0x08819000, 0xf5c, 0},                               // camera
    {0x0881a004, 0x18, 0},                                // aif1
    {0x0881a00b, 0x18, 0},                                // aif2
    {0x09000000, 0x1c, 0},                                // f8
    {0x09040000, 0xe04, 0},                               // usbc
    {0x090c0000, 0x32c, 0},                               // axi_dma
    {0x09100000, 0x100, HAL_BS_REGION_IGNORE},            // gea3
    {0x09140000, 0x544, 0},                               // aes
    {0x09180000, 0xe04, HAL_BS_REGION_IGNORE},            // usb11
    {0x10040000, 0x10000, HAL_BS_REGION_WCN},             // wcn_ram
    {0x14000000, 0x7c44, HAL_BS_REGION_WCN},              // pulp_debug
    {0x14008000, 0x10, HAL_BS_REGION_WCN},                // pulp_irq
    {0x1400c000, 0x8, HAL_BS_REGION_WCN},                 // pulp_sleep
    {0x14020000, 0x88, HAL_BS_REGION_WCN},                // pulp_icache_ctrl
    {0x14040000, 0x88, HAL_BS_REGION_WCN},                // pulp_dcache_ctrl
    {0x14100000, 0x200, HAL_BS_REGION_WCN},               // bt_link
    {0x14100200, 0x194, HAL_BS_REGION_WCN},               // ble_link
    {0x14110000, 0x6000, HAL_BS_REGION_WCN},              // exchange_mem
    {0x15000000, 0x74, HAL_BS_REGION_WCN},                // wcn_sys_ctrl
    {0x15001000, 0x1c, HAL_BS_REGION_WCN},                // wcn_dbm
    {0x15002004, 0x2c, HAL_BS_REGION_WCN},                // wcn_sys_ifc
    {0x15003000, 0x2c, HAL_BS_REGION_WCN},                // wcn_bt_core
    {0x15004000, 0x8, HAL_BS_REGION_WCN},                 // wcn_uart
    {0x15005000, 0x1f8, HAL_BS_REGION_WCN},               // wcn_rf_if
    {0x15006000, 0x1f4, HAL_BS_REGION_WCN},               // wcn_bt_modem
    {0x15007000, 0xa8, HAL_BS_REGION_WCN},                // wcn_wlan
    {0x15008000, 0x18, HAL_BS_REGION_WCN},                // wcn_wdt
    {0x15009000, 0x88, HAL_BS_REGION_WCN},                // wcn_trap
    {0x1500a000, 0x14, HAL_BS_REGION_WCN},                // wcn_systick
    {0x1500b000, 0x1c, HAL_BS_REGION_WCN},                // wcn_comreg_wcn
    {0x1500c000, 0x1c, HAL_BS_REGION_WCN},                // wcn_comreg_ap
    {0x15100000, 0x60, HAL_BS_REGION_WCN},                // wcn_aud_ifc
    {0x15102000, 0x7c, HAL_BS_REGION_WCN},                // wcn_fm_dsp
    {0x20000000, 0x80000, HAL_BS_REGION_ZSP},             // zsp_rom
    {0x21400000, 0x8000, HAL_BS_REGION_IGNORE},           // zsp_imem
    {0x21500000, 0x8000, HAL_BS_REGION_IGNORE},           // zsp_dmem
    {0x22000000, 0x32c, HAL_BS_REGION_ZSP},               // zsp_axidma
    {0x23000000, 0xc, HAL_BS_REGION_ZSP},                 // zsp_aud_dft
    {0x23001000, 0x2c, HAL_BS_REGION_ZSP},                // zsp_wd
    {0x23002000, 0x150, HAL_BS_REGION_ZSP},               // zsp_irq
    {0x23003000, 0x9c, HAL_BS_REGION_ZSP},                // zsp_busmon
    {0x23003000, 0x9c, HAL_BS_REGION_ZSP},                // zsp_busmon
    {0x40000000, 0x1b000, HAL_BS_REGION_GGE},             // bbapb
    {0x40021000, 0x1f0, HAL_BS_REGION_GGE},               // tcu_gsm
    {0x40022000, 0x1f0, HAL_BS_REGION_GGE},               // tcu_nb
    {0x40023004, 0x2c, HAL_BS_REGION_GGE},                // dma
    {0x40025000, 0x18, HAL_BS_REGION_GGE},                // wdt
    {0x40026004, 0xa4, HAL_BS_REGION_GGE},                // sys_ifc
    {0x4002e000, 0x30, HAL_BS_REGION_GGE},                // rfspi_gsm
    {0x4002f000, 0x30, HAL_BS_REGION_GGE},                // rfspi_nb
    {0x40080000, 0x20000, HAL_BS_REGION_GGE},             // bb2g_ram
    {0x50000000, 0xc000, 0},                              // rf_isram, rf_dsram
    {0x50030000, 0x1044, 0},                              // rf_registers
    {0x50032000, 0x5ec, 0},                               // dfe
    {0x50032a00, 0x124, 0},                               // dlpf
    {0x50033000, 0x48, 0},                                // rf_et
    {0x50034000, 0xdc, 0},                                // rf_rtc
    {0x50035000, 0x38, 0},                                // rf_sys_ctrl
    {0x50036000, 0x10, 0},                                // rf_timer1
    {0x50037000, 0x10, 0},                                // rf_timer2
    {0x50038000, 0x10, 0},                                // rf_timer3
    {0x50039004, 0x10, 0},                                // rf_uart
    {0x5003a000, 0x18, 0},                                // rf_wdt
    {0x5003b000, 0x54, 0},                                // tsen_adc
    {0x5003c000, 0x1c, 0},                                // rffe
    {0x50080000, 0x68, 0},                                // sysreg
    {0x50081000, 0xac, 0},                                // clkrst
    {0x50083000, 0x48, 0},                                // cp_monitor
    {0x50084000, 0x24, 0},                                // cp_bb_wd
    {0x50090000, 0x4030, 0},                              // idle
    {0x50098008, 0x2c, 0},                                // uart1
    {0x500a0000, 0xdc, 0},                                // pwrctrl
    {0x50100000, 0x248, 0},                               // sys_ctrl
    {0x50101000, 0x1ac, 0},                               // lvds
    {0x50102000, 0x2c, 0},                                // gsm_lps
    {0x50103000, 0x8, 0},                                 // i2c2
    {0x50104000, 0x680, 0},                               // mailbox
    {0x50105000, 0x3c, 0},                                // timer3
    {0x50106000, 0x1c, 0},                                // kaypad
    {0x50107000, 0x50, 0},                                // gpio1
    {0x50108000, 0x18, 0},                                // pwm
    {0x50109000, 0x240, 0},                               // analog_reg
    {0x5010a004, 0x40, 0},                                // aon_ifc
    {0x5010b000, 0x2c, 0},                                // nb_lps
    {0x5010c000, 0x178, 0},                               // iomux
    {0x5010d000, 0x100, 0},                               // spinlock
    {0x5010e000, 0x14c, HAL_BS_REGION_IGNORE},            // efuse_ctrl
    {0x50110000, 0x8, 0},                                 // debug_uart
    {0x50112000, 0x30, HAL_BS_REGION_IGNORE},             // vad
    {0x5017f000, 0x1c, 0},                                // debughost
    {0x50300000, 0x60, 0},                                // adi_mst
    {0x50308000, 0x1000, 0},                              // pmic
    {0, 0, HAL_BS_REGION_END},
};
