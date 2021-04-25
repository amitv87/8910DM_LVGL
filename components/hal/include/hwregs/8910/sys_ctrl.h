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

#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define NB_SYS_AXI_CLK_REV (2)
#define NB_SYS_AXI_CLK_AEN (6)
#define NB_SYS_AXI_CLK_EN (7)
#define NB_SYS_AXI_CLK (7)
#define NB_SYS_AHB_CLK_REV (3)
#define NB_SYS_AHB_CLK_AEN (6)
#define NB_SYS_AHB_CLK_EN (11)
#define NB_SYS_AHB_CLK (11)
#define NB_AP_APB_CLK_AEN (23)
#define NB_AP_APB_CLK_EN (31)
#define NB_AP_APB_CLK (31)
#define NB_AIF_APB_CLK_REV (8)
#define NB_AIF_APB_CLK_AEN (12)
#define NB_AIF_APB_CLK_EN (13)
#define NB_AIF_APB_CLK (13)
#define NB_AON_AHB_CLK_REV (1)
#define NB_AON_AHB_CLK_AEN (5)
#define NB_AON_AHB_CLK_EN (6)
#define NB_AON_AHB_CLK (6)
#define NB_AON_APB_CLK_AEN (20)
#define NB_AON_APB_CLK_EN (29)
#define NB_AON_APB_CLK (29)
#define NB_RF_AHB_CLK_AEN (1)
#define NB_RF_AHB_CLK_EN (2)
#define NB_RF_AHB_CLK (2)
#define NB_RF_APB_CLK_AEN (1)
#define NB_RF_APB_CLK_EN (2)
#define NB_RF_APB_CLK (2)
#define NB_CLK_OTHER_AEN (8)
#define OC_ID_SYS_SPIFLASH_BASE (13)
#define OC_ID_PSRAM_BASE (21)
#define NB_CLK_OTHER_EN (25)
#define NB_CLK_OTHER (25)
#define NB_CLK_OTHER_1_AEN (16)
#define NB_CLK_OTHER_1_EN (31)
#define NB_CLK_OTHER_1 (31)
#define NB_CLK_PSRAM (4)
#define NB_SYS_AXI_RST (7)
#define NB_SYS_AHB_RST (13)
#define NB_AP_APB_RST (25)
#define NB_AIF_APB_RST (6)
#define NB_AON_AHB_RST (5)
#define NB_AON_APB_RST (20)
#define NB_RF_AHB_RST (2)
#define NB_RF_APB_RST (2)
#define NB_APCPU_RST (4)
#define NB_CPCPU_RST (3)
#define NB_BBLTE_RST (2)
#define NB_RSTO (32)
#define SYS_CTRL_PROTECT_LOCK (0xa50000)
#define SYS_CTRL_PROTECT_UNLOCK (0xa50001)

typedef enum
{
    A5CPU = 0,
} CPU_ID_T;

typedef enum
{
    SYS_AXI_CLK_ID_IMEM = 0,
    SYS_AXI_CLK_ID_LZMA = 1,
    SYS_AXI_CLK_ID_REV0 = 2,
    SYS_AXI_CLK_ID_REV1 = 3,
    SYS_AXI_CLK_ID_REV2 = 4,
    SYS_AXI_CLK_ID_SDMMC1 = 5,
    SYS_AXI_CLK_ID_ALWAYS = 6,
} SYS_AXI_CLKS_T;

typedef enum
{
    SYS_AHB_CLK_ID_GOUDA = 0,
    SYS_AHB_CLK_ID_GGE = 1,
    SYS_AHB_CLK_ID_GEA3 = 2,
    SYS_AHB_CLK_ID_LZMA = 3,
    SYS_AHB_CLK_ID_F8 = 4,
    SYS_AHB_CLK_ID_AXIDMA = 5,
    SYS_AHB_CLK_ID_USBC = 6,
    SYS_AHB_CLK_ID_LCD = 7,
    SYS_AHB_CLK_ID_AES = 8,
    SYS_AHB_CLK_ID_USB11 = 9,
    SYS_AHB_CLK_ID_ALWAYS = 10,
} SYS_AHB_CLKS_T;

typedef enum
{
    AP_APB_CLK_ID_CONF = 0,
    AP_APB_CLK_ID_MOD_CAMERA = 1,
    AP_APB_CLK_ID_I2C1 = 2,
    AP_APB_CLK_ID_I2C3 = 3,
    AP_APB_CLK_ID_IFC = 4,
    AP_APB_CLK_ID_IFC_CH0 = 5,
    AP_APB_CLK_ID_IFC_CH1 = 6,
    AP_APB_CLK_ID_IFC_CH2 = 7,
    AP_APB_CLK_ID_IFC_CH3 = 8,
    AP_APB_CLK_ID_IFC_CH4 = 9,
    AP_APB_CLK_ID_IFC_CH5 = 10,
    AP_APB_CLK_ID_IFC_CH6 = 11,
    AP_APB_CLK_ID_IFC_CHDBG = 12,
    AP_APB_CLK_ID_GOUDA = 13,
    AP_APB_CLK_ID_SDMMC2 = 14,
    AP_APB_CLK_ID_SPI1 = 15,
    AP_APB_CLK_ID_SPI2 = 16,
    AP_APB_CLK_ID_SCID1 = 17,
    AP_APB_CLK_ID_SCID2 = 18,
    AP_APB_CLK_ID_SCI1 = 19,
    AP_APB_CLK_ID_SCI2 = 20,
    AP_APB_CLK_ID_CAMERA = 21,
    AP_APB_CLK_ID_PAGESPY = 22,
    AP_APB_CLK_ID_LZMA = 23,
    AP_APB_CLK_ID_REV2 = 24,
    AP_APB_CLK_ID_TIMER1 = 25,
    AP_APB_CLK_ID_TIMER2 = 26,
    AP_APB_CLK_ID_TIMER4 = 27,
    AP_APB_CLK_ID_LCD = 28,
    AP_APB_CLK_ID_CQIRQ = 29,
    AP_APB_CLK_ID_ALWAYS = 30,
} AP_APB_CLKS_T;

typedef enum
{
    AIF_APB_CLK_ID_CONF = 0,
    AIF1_APB_CLK_ID_AIF = 1,
    AIF2_APB_CLK_ID_AIF = 2,
    AIF_APB_CLK_ID_IFC = 3,
    AIF_APB_CLK_ID_IFC_CH0 = 4,
    AIF_APB_CLK_ID_IFC_CH1 = 5,
    AIF_APB_CLK_ID_IFC_CH2 = 6,
    AIF_APB_CLK_ID_IFC_CH3 = 7,
    AIF_APB_CLK_ID_REV0 = 8,
    AIF_APB_CLK_ID_REV1 = 9,
    AIF_APB_CLK_ID_REV2 = 10,
    AIF_APB_CLK_ID_AUD_2AD = 11,
    AIF_APB_CLK_ID_ALWAYS = 12,
} AIF_APB_CLKS_T;

typedef enum
{
    AON_AHB_CLK_ID_BB_SYSCTRL = 0,
    AON_AHB_CLK_ID_BB_REV0 = 1,
    AON_AHB_CLK_ID_BB_REV1 = 2,
    AON_AHB_CLK_ID_BB_REV2 = 3,
    AON_AHB_CLK_ID_CSSYS = 4,
    AON_AHB_CLK_ID_ALWAYS = 5,
} AON_AHB_CLKS_T;

typedef enum
{
    AON_APB_CLK_ID_CONF = 0,
    AON_APB_CLK_ID_DBG_HOST = 1,
    AON_APB_CLK_ID_DBG_UART = 2,
    AON_APB_CLK_ID_IFC = 3,
    AON_APB_CLK_ID_IFC_CH0 = 4,
    AON_APB_CLK_ID_IFC_CH1 = 5,
    AON_APB_CLK_ID_IFC_CH2 = 6,
    AON_APB_CLK_ID_IFC_CH3 = 7,
    AON_APB_CLK_ID_IFC_CHDBG = 8,
    AON_APB_CLK_ID_REV0 = 9,
    AON_APB_CLK_ID_REV1 = 10,
    AON_APB_CLK_ID_PWM = 11,
    AON_APB_CLK_ID_I2C2 = 12,
    AON_APB_CLK_ID_ANA_REG = 13,
    AON_APB_CLK_ID_SPINLOCK = 14,
    AON_APB_CLK_ID_LPS_GSM = 15,
    AON_APB_CLK_ID_LPS_NB = 16,
    AON_APB_CLK_ID_EFUSE = 17,
    AON_APB_CLK_ID_REV2 = 18,
    AON_APB_CLK_ID_VAD = 19,
    AON_APB_CLK_ID_REV3 = 20,
    AON_APB_CLK_ID_REV4 = 21,
    AON_APB_CLK_ID_GPIO = 22,
    AON_APB_CLK_ID_TIMER3 = 23,
    AON_APB_CLK_ID_KEYPAD = 24,
    AON_APB_CLK_ID_AHB2APB_ADI = 25,
    AON_APB_CLK_ID_ADI = 26,
    AON_APB_CLK_ID_LVDS = 27,
    AON_APB_CLK_ID_ALWAYS = 28,
} AON_APB_CLKS_T;

typedef enum
{
    RF_AHB_CLK_ID_REV0 = 0,
    RF_AHB_CLK_ID_ALWAYS = 1,
} RF_AHB_CLKS_T;

typedef enum
{
    RF_APB_CLK_ID_REV0 = 0,
    RF_APB_CLK_ID_ALWAYS = 1,
} RF_APB_CLKS_T;

typedef enum
{
    OC_ID_HOST_UART = 0,
    OC_ID_BCK1 = 1,
    OC_ID_BCK2 = 2,
    OC_ID_UART1 = 3,
    OC_ID_UART2 = 4,
    OC_ID_UART3 = 5,
    OC_ID_AP_A5 = 6,
    OC_ID_CP_A5 = 7,
    OC_ID_GPIO = 8,
    OC_ID_USBPHY = 9,
    OC_ID_PIX = 10,
    OC_ID_CLK_OUT = 11,
    OC_ID_ISP = 12,
    OC_ID_SYS_SPIFLASH = 13,
    OC_ID_SYS_SPIFLASH_ALWAYS = 14,
    OC_ID_SYS_SPIFLASH1 = 15,
    OC_ID_SYS_SPIFLASH1_ALWAYS = 16,
    OC_ID_SPIFLASH = 17,
    OC_ID_SPIFLASH1 = 18,
    OC_ID_SPICAM = 19,
    OC_ID_CAM = 20,
    OC_ID_PSRAM_CONF = 21,
    OC_ID_PSRAM_DMC = 22,
    OC_ID_PSRAM_PAGESPY = 23,
    OC_ID_PSRAM_ALWAYS = 24,
} OTHERS_CLKS_T;

typedef enum
{
    OC_ID_GGE_CLK32K = 0,
    OC_ID_GGE_26M = 1,
    OC_ID_NB_61P44M = 2,
    OC_ID_BT_FM_CLK32K = 3,
    OC_ID_BT_FM_26M = 4,
    OC_ID_PMIC_CLK32K = 5,
    OC_ID_PMIC_26M = 6,
    OC_ID_CP_CLK32K = 7,
    OC_ID_CP_26M = 8,
    OC_ID_ZSP_UART = 9,
    OC_ID_CP_BBLTE = 10,
    OC_ID_494M_LTE = 11,
    OC_ID_REV0 = 12,
    OC_ID_REV1 = 13,
    OC_ID_REV2 = 14,
    OC_ID_REV3 = 15,
    OC_ID_SDM_26M = 16,
    OC_ID_LPS_GSM = 17,
    OC_ID_LPS_NB = 18,
    OC_ID_EFUSE_26M = 19,
    OC_ID_USB_ADP_32K = 20,
    OC_ID_USB_UTMI_48M = 21,
    OC_ID_AP_26M = 22,
    OC_ID_AP_32K = 23,
    OC_ID_MIPIDSI = 24,
    OC_ID_AHB_BTFM = 25,
    OC_ID_VAD = 26,
    OC_ID_USB11_48M = 27,
    OC_ID_TRNG_CLKEN = 28,
    OC_ID_CORESIGHT = 29,
    OC_ID_ADI = 30,
} OTHERS_CLKS_1_T;

typedef enum
{
    PSRAM_CLK_ID_CONF = 0,
    PSRAM_CLK_ID_DMC = 1,
    PSRAM_CLK_ID_PAGESPY = 2,
    PSRAM_CLK_ID_ALWAYS = 3,
} PSRAM_CLKS_T;

typedef enum
{
    SPIFLASH_CLK_ID_SPIFLASH = 0,
    SPIFLASH_CLK_ID_ALWAYS = 1,
    SPIFLASH_CLK_ID_SPIFLASH1 = 2,
    SPIFLASH1_CLK_ID_ALWAYS = 3,
} SYS_SPIFLASH_CLKS_T;

typedef enum
{
    SYS_AXI_RST_ID_SYS = 0,
    SYS_AXI_RST_ID_IMEM = 1,
    SYS_AXI_RST_ID_LZMA = 2,
    SYS_AXI_RST_ID_REV0 = 3,
    SYS_AXI_RST_ID_REV1 = 4,
    SYS_AXI_RST_ID_REV2 = 5,
    SYS_AXI_RST_ID_REV3 = 6,
} SYS_AXI_RST_T;

typedef enum
{
    SYS_AHB_RST_ID_SYS = 0,
    SYS_AHB_RST_ID_GOUDA = 1,
    SYS_AHB_RST_ID_GGE = 2,
    SYS_AHB_RST_ID_GEA3 = 3,
    SYS_AHB_RST_ID_USBC = 4,
    SYS_AHB_RST_ID_BTFM = 5,
    SYS_AHB_RST_ID_GIC400 = 6,
    SYS_AHB_RST_ID_F8 = 7,
    SYS_AHB_RST_ID_AXIDMA = 8,
    SYS_AHB_RST_ID_LZMA = 9,
    SYS_AHB_RST_ID_LCD = 10,
    SYS_AHB_RST_ID_AES = 11,
    SYS_AHB_RST_ID_USB11 = 12,
} SYS_AHB_RST_T;

typedef enum
{
    AP_APB_RST_ID_SYS = 0,
    AP_APB_RST_ID_CAMERA = 1,
    AP_APB_RST_ID_I2C1 = 2,
    AP_APB_RST_ID_I2C3 = 3,
    AP_APB_RST_ID_IFC = 4,
    AP_APB_RST_ID_IMEM = 5,
    AP_APB_RST_ID_SDMMC1 = 6,
    AP_APB_RST_ID_SDMMC2 = 7,
    AP_APB_RST_ID_SPI1 = 8,
    AP_APB_RST_ID_SPI2 = 9,
    AP_APB_RST_ID_UART2 = 10,
    AP_APB_RST_ID_UART3 = 11,
    AP_APB_RST_ID_ZSP_UART = 12,
    AP_APB_RST_ID_REV0 = 13,
    AP_APB_RST_ID_REV1 = 14,
    AP_APB_RST_ID_SCI1 = 15,
    AP_APB_RST_ID_SCI2 = 16,
    AP_APB_RST_ID_PAGESPY = 17,
    AP_APB_RST_ID_LZMA = 18,
    AP_APB_RST_ID_PSRAM = 19,
    AP_APB_RST_ID_TIMER1 = 20,
    AP_APB_RST_ID_TIMER2 = 21,
    AP_APB_RST_ID_TIMER4 = 22,
    AP_APB_RST_ID_GOUDA = 23,
    AP_APB_RST_ID_CQIRQ = 24,
} AP_APB_RST_T;

typedef enum
{
    AIF1_APB_RST_ID_SYS = 0,
    AIF2_APB_RST_ID_SYS = 1,
    AIF1_APB_RST_ID_AIF = 2,
    AIF2_APB_RST_ID_AIF = 3,
    AIF_APB_RST_ID_IFC = 4,
    AIF_APB_RST_ID_AUD_2AD = 5,
} AIF_APB_RST_T;

typedef enum
{
    AON_AHB_RST_ID_SYS = 0,
    AON_AHB_RST_ID_REV0 = 1,
    AON_AHB_RST_ID_REV1 = 2,
    AON_AHB_RST_ID_REV2 = 3,
    AON_AHB_RST_ID_CSSYS = 4,
} AON_AHB_RST_T;

typedef enum
{
    AON_APB_RST_ID_SYS = 0,
    AON_APB_RST_ID_CALENDAR = 1,
    AON_APB_RST_ID_GPIO = 2,
    AON_APB_RST_ID_IFC = 3,
    AON_APB_RST_ID_KEYPAD = 4,
    AON_APB_RST_ID_PWM = 5,
    AON_APB_RST_ID_REV0 = 6,
    AON_APB_RST_ID_REV1 = 7,
    AON_APB_RST_ID_TIMER3 = 8,
    AON_APB_RST_ID_I2C2 = 9,
    AON_APB_RST_ID_ANA_REG = 10,
    AON_APB_RST_ID_SPINLOCK = 11,
    AON_APB_RST_ID_LPS_GSM = 12,
    AON_APB_RST_ID_LPS_NB = 13,
    AON_APB_RST_ID_EFUSE = 14,
    AON_APB_RST_ID_AHB2APB_ADI = 15,
    AON_APB_RST_ID_ADI = 16,
    AON_APB_RST_ID_LVDS = 17,
    AON_APB_RST_ID_UART1 = 18,
    AON_APB_RST_ID_VAD = 19,
} AON_APB_RST_T;

typedef enum
{
    RF_AHB_RST_ID_SYS = 0,
    RF_AHB_RST_ID_RF_REV0 = 1,
} RF_AHB_RST_T;

typedef enum
{
    RF_APB_RST_ID_SYS = 0,
    RF_APB_RST_ID_REV0 = 1,
} RF_APB_RST_T;

typedef enum
{
    APCPU_RST_ID_SYS = 0,
    APCPU_RST_ID_CORE = 1,
    APCPU_RST_ID_DBG = 2,
    APCPU_RST_ID_GIC400 = 3,
    GLOBAL_SOFT_RST = 4,
} APCPU_RST_T;

typedef enum
{
    CPCPU_RST_ID_SYS = 0,
    CPCPU_RST_ID_CORE = 1,
    CPCPU_RST_ID_DBG = 2,
} CPCPU_RST_T;

typedef enum
{
    BBLTE_RST_ID_SYS = 0,
    BBLTE_RST_ID_REV0 = 1,
} BBLTE_RST_T;

typedef enum
{
    RSTO_ID_BCK1 = 0,
    RSTO_ID_BCK2 = 1,
    RSTO_ID_DBG_HOST = 2,
    RSTO_ID_GPIO = 3,
    RSTO_ID_UART1 = 4,
    RSTO_ID_UART2 = 5,
    RSTO_ID_UART3 = 6,
    RSTO_ID_USBC = 7,
    RSTO_ID_WDTIMER0 = 8,
    RSTO_ID_WDTIMER1 = 9,
    RSTO_ID_WDTIMER2 = 10,
    RSTO_ID_SPIFLASH = 11,
    RSTO_ID_SPIFLASH_SYS = 12,
    RSTO_ID_SPIFLASH1 = 13,
    RSTO_ID_SPIFLASH1_SYS = 14,
    RSTO_ID_PSRAM_DMC = 15,
    RSTO_ID_PSRAM_SYS = 16,
    RSTO_ID_PSRAM_PAGESPY = 17,
    RSTO_ID_VAD = 18,
    RSTO_ID_PIX = 19,
    RSTO_ID_SDM_26M = 20,
    RSTO_ID_WDTIMER4 = 21,
    RSTO_ID_LPS_GSM = 22,
    RSTO_ID_LPS_NB = 23,
    RSTO_ID_EFUSE = 24,
    RSTO_ID_USB_ADP_32K = 25,
    RSTO_ID_MIPIDSI = 26,
    RSTO_ID_MIPIDSI_PHY = 27,
    RSTO_ID_AUD_2AD = 28,
    RSTO_ID_USB11_48M = 29,
    RSTO_ID_ZSP_UART = 30,
    RSTO_ID_CORESIGHT = 31,
} OTHERS_RST_T;

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_SYS_CTRL_BASE (0x05100000)
#else
#define REG_SYS_CTRL_BASE (0x50100000)
#endif

typedef volatile struct
{
    uint32_t reg_dbg;                  // 0x00000000
    uint32_t sys_axi_rst_set;          // 0x00000004
    uint32_t sys_axi_rst_clr;          // 0x00000008
    uint32_t sys_ahb_rst_set;          // 0x0000000c
    uint32_t sys_ahb_rst_clr;          // 0x00000010
    uint32_t ap_apb_rst_set;           // 0x00000014
    uint32_t ap_apb_rst_clr;           // 0x00000018
    uint32_t aif_apb_rst_set;          // 0x0000001c
    uint32_t aif_apb_rst_clr;          // 0x00000020
    uint32_t aon_ahb_rst_set;          // 0x00000024
    uint32_t aon_ahb_rst_clr;          // 0x00000028
    uint32_t aon_apb_rst_set;          // 0x0000002c
    uint32_t aon_apb_rst_clr;          // 0x00000030
    uint32_t rf_ahb_rst_set;           // 0x00000034
    uint32_t rf_ahb_rst_clr;           // 0x00000038
    uint32_t rf_apb_rst_set;           // 0x0000003c
    uint32_t rf_apb_rst_clr;           // 0x00000040
    uint32_t apcpu_rst_set;            // 0x00000044
    uint32_t apcpu_rst_clr;            // 0x00000048
    uint32_t cpcpu_rst_set;            // 0x0000004c
    uint32_t cpcpu_rst_clr;            // 0x00000050
    uint32_t bblte_rst_set;            // 0x00000054
    uint32_t bblte_rst_clr;            // 0x00000058
    uint32_t others_rst_set;           // 0x0000005c
    uint32_t others_rst_clr;           // 0x00000060
    uint32_t clk_sys_axi_mode;         // 0x00000064
    uint32_t clk_sys_axi_enable;       // 0x00000068
    uint32_t clk_sys_axi_disable;      // 0x0000006c
    uint32_t clk_sys_ahb_mode;         // 0x00000070
    uint32_t clk_sys_ahb_enable;       // 0x00000074
    uint32_t clk_sys_ahb_disable;      // 0x00000078
    uint32_t clk_ap_apb_mode;          // 0x0000007c
    uint32_t clk_ap_apb_enable;        // 0x00000080
    uint32_t clk_ap_apb_disable;       // 0x00000084
    uint32_t clk_aif_apb_mode;         // 0x00000088
    uint32_t clk_aif_apb_enable;       // 0x0000008c
    uint32_t clk_aif_apb_disable;      // 0x00000090
    uint32_t clk_aon_ahb_mode;         // 0x00000094
    uint32_t clk_aon_ahb_enable;       // 0x00000098
    uint32_t clk_aon_ahb_disable;      // 0x0000009c
    uint32_t clk_aon_apb_mode;         // 0x000000a0
    uint32_t clk_aon_apb_enable;       // 0x000000a4
    uint32_t clk_aon_apb_disable;      // 0x000000a8
    uint32_t clk_rf_ahb_mode;          // 0x000000ac
    uint32_t clk_rf_ahb_enable;        // 0x000000b0
    uint32_t clk_rf_ahb_disable;       // 0x000000b4
    uint32_t clk_rf_apb_mode;          // 0x000000b8
    uint32_t clk_rf_apb_enable;        // 0x000000bc
    uint32_t clk_rf_apb_disable;       // 0x000000c0
    uint32_t clk_others_mode;          // 0x000000c4
    uint32_t clk_others_enable;        // 0x000000c8
    uint32_t clk_others_disable;       // 0x000000cc
    uint32_t clk_others_1_mode;        // 0x000000d0
    uint32_t clk_others_1_enable;      // 0x000000d4
    uint32_t clk_others_1_disable;     // 0x000000d8
    uint32_t pll_ctrl;                 // 0x000000dc
    uint32_t sel_clock;                // 0x000000e0
    uint32_t __228[1];                 // 0x000000e4
    uint32_t cfg_clk_out;              // 0x000000e8
    uint32_t __236[1];                 // 0x000000ec
    uint32_t cfg_clk_audiobck1_div;    // 0x000000f0
    uint32_t cfg_clk_audiobck2_div;    // 0x000000f4
    uint32_t cfg_clk_uart[4];          // 0x000000f8
    uint32_t cfg_clk_pwm;              // 0x00000108
    uint32_t cfg_clk_dbg_div;          // 0x0000010c
    uint32_t cfg_clk_camera_out;       // 0x00000110
    uint32_t reset_cause;              // 0x00000114
    uint32_t wakeup;                   // 0x00000118
    uint32_t ignore_charger;           // 0x0000011c
    uint32_t __288[2];                 // 0x00000120
    uint32_t cfg_pll_spiflash_div;     // 0x00000128
    uint32_t cfg_pll_spiflash1_div;    // 0x0000012c
    uint32_t cfg_pll_mem_bridge_div;   // 0x00000130
    uint32_t cfg_dbg_clk_source_sel;   // 0x00000134
    uint32_t cfg_clk_494m_lte_div;     // 0x00000138
    uint32_t cfg_pll_isp_div;          // 0x0000013c
    uint32_t cfg_pll_pix_div;          // 0x00000140
    uint32_t cfg_pll_sys_axi_div;      // 0x00000144
    uint32_t cfg_pll_sys_ahb_apb_div;  // 0x00000148
    uint32_t cfg_pll_sys_ahb_btfm_div; // 0x0000014c
    uint32_t cfg_pll_csi_div;          // 0x00000150
    uint32_t cfg_pll_sys_spiflash_div; // 0x00000154
    uint32_t cfg_pll_cp_div;           // 0x00000158
    uint32_t cfg_pll_ap_cpu_div;       // 0x0000015c
    uint32_t cfg_ap_cpu_aclken_div;    // 0x00000160
    uint32_t cfg_ap_cpu_dbgen_div;     // 0x00000164
    uint32_t cfg_pll_cp_cpu_div;       // 0x00000168
    uint32_t cfg_cp_cpu_aclken_div;    // 0x0000016c
    uint32_t cfg_cp_cpu_dbgen_div;     // 0x00000170
    uint32_t cfg_trng_clken_div;       // 0x00000174
    uint32_t cfg_coresight_div;        // 0x00000178
    uint32_t cfg_usb11_48m_div;        // 0x0000017c
    uint32_t cfg_pll_usb;              // 0x00000180
    uint32_t cfg_vad_div;              // 0x00000184
    uint32_t cfg_hmprot;               // 0x00000188
    uint32_t cfg_mem_cq;               // 0x0000018c
    uint32_t cfg_arm_sys_cfg_mem;      // 0x00000190
    uint32_t cfg_audio_cfg_mem;        // 0x00000194
    uint32_t cfg_lcd_cfg_mem;          // 0x00000198
    uint32_t cfg_cam_cfg_mem;          // 0x0000019c
    uint32_t cfg_peri_cfg_mem;         // 0x000001a0
    uint32_t cfg_aon_sys_cfg_mem;      // 0x000001a4
    uint32_t cfg_rf_sys_cfg_mem;       // 0x000001a8
    uint32_t cfg_coresight_cfg_mem;    // 0x000001ac
    uint32_t cfg_vad_cfg_mem;          // 0x000001b0
    uint32_t cfg_aif_cfg;              // 0x000001b4
    uint32_t cfg_misc_cfg;             // 0x000001b8
    uint32_t cfg_misc1_cfg;            // 0x000001bc
    uint32_t cfg_force_lp_mode_lp;     // 0x000001c0
    uint32_t __452[4];                 // 0x000001c4
    uint32_t cfg_reserve;              // 0x000001d4
    uint32_t cfg_reserve1;             // 0x000001d8
    uint32_t cfg_reserve2;             // 0x000001dc
    uint32_t cfg_reserve3;             // 0x000001e0
    uint32_t cfg_reserve4;             // 0x000001e4
    uint32_t cfg_reserve5;             // 0x000001e8
    uint32_t cfg_reserve6;             // 0x000001ec
    uint32_t cfg_reserve7;             // 0x000001f0
    uint32_t cfg_reserve8;             // 0x000001f4
    uint32_t cfg_reserve9;             // 0x000001f8
    uint32_t cfg_reserve10;            // 0x000001fc
    uint32_t cfg_reserve11;            // 0x00000200
    uint32_t cfg_reserve12;            // 0x00000204
    uint32_t cfg_reserve13;            // 0x00000208
    uint32_t cfg_reserve14;            // 0x0000020c
    uint32_t cfg_reserve15;            // 0x00000210
    uint32_t cfg_reserve16;            // 0x00000214
    uint32_t cfg_reserve1_in;          // 0x00000218
    uint32_t cfg_reserve2_in;          // 0x0000021c
    uint32_t cfg_reserve3_in;          // 0x00000220
    uint32_t cfg_reserve4_in;          // 0x00000224
    uint32_t cfg_reserve5_in;          // 0x00000228
    uint32_t cfg_reserve6_in;          // 0x0000022c
    uint32_t cfg_reserve7_in;          // 0x00000230
    uint32_t cfg_chip_prod_id;         // 0x00000234
    uint32_t cfg_qos_wcn_a5_gge;       // 0x00000238
    uint32_t cfg_qos_axidma_cpa5_f8;   // 0x0000023c
    uint32_t cfg_qos_lcdc_lzma_gouda;  // 0x00000240
    uint32_t cfg_qos_lte_usb;          // 0x00000244
    uint32_t cfg_qos_merge_mem;        // 0x00000248
    uint32_t cfg_bcpu_dbg_bkp;         // 0x0000024c
} HWP_SYS_CTRL_T;

#define hwp_sysCtrl ((HWP_SYS_CTRL_T *)REG_ACCESS_ADDRESS(REG_SYS_CTRL_BASE))

// reg_dbg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t scratch : 16;            // [15:0]
        uint32_t __29_16 : 14;            // [29:16]
        uint32_t write_unlock_status : 1; // [30], read only
        uint32_t write_unlock : 1;        // [31], read only
    } b;
} REG_SYS_CTRL_REG_DBG_T;

// sys_axi_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_sys_axi_rst_id_sys : 1;  // [0], write set
        uint32_t set_sys_axi_rst_id_imem : 1; // [1], write set
        uint32_t set_sys_axi_rst_id_lzma : 1; // [2], write set
        uint32_t set_sys_axi_rst_id_rev0 : 1; // [3], write set
        uint32_t set_sys_axi_rst_id_rev1 : 1; // [4], write set
        uint32_t set_sys_axi_rst_id_rev2 : 1; // [5], write set
        uint32_t set_sys_axi_rst_id_rev3 : 1; // [6], write set
        uint32_t __31_7 : 25;                 // [31:7]
    } b;
} REG_SYS_CTRL_SYS_AXI_RST_SET_T;

// sys_axi_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_sys_axi_rst_id_sys : 1;  // [0], write clear
        uint32_t clr_sys_axi_rst_id_imem : 1; // [1], write clear
        uint32_t clr_sys_axi_rst_id_lzma : 1; // [2], write clear
        uint32_t clr_sys_axi_rst_id_rev0 : 1; // [3], write clear
        uint32_t clr_sys_axi_rst_id_rev1 : 1; // [4], write clear
        uint32_t clr_sys_axi_rst_id_rev2 : 1; // [5], write clear
        uint32_t clr_sys_axi_rst_id_rev3 : 1; // [6], write clear
        uint32_t __31_7 : 25;                 // [31:7]
    } b;
} REG_SYS_CTRL_SYS_AXI_RST_CLR_T;

// sys_ahb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_sys_ahb_rst_id_sys : 1;    // [0], write set
        uint32_t set_sys_ahb_rst_id_gouda : 1;  // [1], write set
        uint32_t set_sys_ahb_rst_id_gge : 1;    // [2], write set
        uint32_t set_sys_ahb_rst_id_gea3 : 1;   // [3], write set
        uint32_t set_sys_ahb_rst_id_usbc : 1;   // [4], write set
        uint32_t set_sys_ahb_rst_id_btfm : 1;   // [5], write set
        uint32_t set_sys_ahb_rst_id_gic400 : 1; // [6], write set
        uint32_t set_sys_ahb_rst_id_f8 : 1;     // [7], write set
        uint32_t set_sys_ahb_rst_id_axidma : 1; // [8], write set
        uint32_t set_sys_ahb_rst_id_lzma : 1;   // [9], write set
        uint32_t set_sys_ahb_rst_id_lcd : 1;    // [10], write set
        uint32_t set_sys_ahb_rst_id_aes : 1;    // [11], write set
        uint32_t set_sys_ahb_rst_id_usb11 : 1;  // [12], write set
        uint32_t __31_13 : 19;                  // [31:13]
    } b;
} REG_SYS_CTRL_SYS_AHB_RST_SET_T;

// sys_ahb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_sys_ahb_rst_id_sys : 1;    // [0], write clear
        uint32_t clr_sys_ahb_rst_id_gouda : 1;  // [1], write clear
        uint32_t clr_sys_ahb_rst_id_gge : 1;    // [2], write clear
        uint32_t clr_sys_ahb_rst_id_gea3 : 1;   // [3], write clear
        uint32_t clr_sys_ahb_rst_id_usbc : 1;   // [4], write clear
        uint32_t clr_sys_ahb_rst_id_btfm : 1;   // [5], write clear
        uint32_t clr_sys_ahb_rst_id_gic400 : 1; // [6], write clear
        uint32_t clr_sys_ahb_rst_id_f8 : 1;     // [7], write clear
        uint32_t clr_sys_ahb_rst_id_axidma : 1; // [8], write clear
        uint32_t clr_sys_ahb_rst_id_lzma : 1;   // [9], write clear
        uint32_t clr_sys_ahb_rst_id_lcd : 1;    // [10], write clear
        uint32_t clr_sys_ahb_rst_id_aes : 1;    // [11], write clear
        uint32_t clr_sys_ahb_rst_id_usb11 : 1;  // [12], write clear
        uint32_t __31_13 : 19;                  // [31:13]
    } b;
} REG_SYS_CTRL_SYS_AHB_RST_CLR_T;

// ap_apb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_ap_apb_rst_id_sys : 1;      // [0], write set
        uint32_t set_ap_apb_rst_id_camera : 1;   // [1], write set
        uint32_t set_ap_apb_rst_id_i2c1 : 1;     // [2], write set
        uint32_t set_ap_apb_rst_id_i2c3 : 1;     // [3], write set
        uint32_t set_ap_apb_rst_id_ifc : 1;      // [4], write set
        uint32_t set_ap_apb_rst_id_imem : 1;     // [5], write set
        uint32_t set_ap_apb_rst_id_sdmmc1 : 1;   // [6], write set
        uint32_t set_ap_apb_rst_id_sdmmc2 : 1;   // [7], write set
        uint32_t set_ap_apb_rst_id_spi1 : 1;     // [8], write set
        uint32_t set_ap_apb_rst_id_spi2 : 1;     // [9], write set
        uint32_t set_ap_apb_rst_id_uart2 : 1;    // [10], write set
        uint32_t set_ap_apb_rst_id_uart3 : 1;    // [11], write set
        uint32_t set_ap_apb_rst_id_zsp_uart : 1; // [12], write set
        uint32_t set_ap_apb_rst_id_rev0 : 1;     // [13], write set
        uint32_t set_ap_apb_rst_id_rev1 : 1;     // [14], write set
        uint32_t set_ap_apb_rst_id_sci1 : 1;     // [15], write set
        uint32_t set_ap_apb_rst_id_sci2 : 1;     // [16], write set
        uint32_t set_ap_apb_rst_id_pagespy : 1;  // [17], write set
        uint32_t set_ap_apb_rst_id_lzma : 1;     // [18], write set
        uint32_t set_ap_apb_rst_id_psram : 1;    // [19], write set
        uint32_t set_ap_apb_rst_id_timer1 : 1;   // [20], write set
        uint32_t set_ap_apb_rst_id_timer2 : 1;   // [21], write set
        uint32_t set_ap_apb_rst_id_timer4 : 1;   // [22], write set
        uint32_t set_ap_apb_rst_id_gouda : 1;    // [23], write set
        uint32_t set_ap_apb_rst_id_cqirq : 1;    // [24], write set
        uint32_t __31_25 : 7;                    // [31:25]
    } b;
} REG_SYS_CTRL_AP_APB_RST_SET_T;

// ap_apb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_ap_apb_rst_id_sys : 1;      // [0], write clear
        uint32_t clr_ap_apb_rst_id_camera : 1;   // [1], write clear
        uint32_t clr_ap_apb_rst_id_i2c1 : 1;     // [2], write clear
        uint32_t clr_ap_apb_rst_id_i2c3 : 1;     // [3], write clear
        uint32_t clr_ap_apb_rst_id_ifc : 1;      // [4], write clear
        uint32_t clr_ap_apb_rst_id_imem : 1;     // [5], write clear
        uint32_t clr_ap_apb_rst_id_sdmmc1 : 1;   // [6], write clear
        uint32_t clr_ap_apb_rst_id_sdmmc2 : 1;   // [7], write clear
        uint32_t clr_ap_apb_rst_id_spi1 : 1;     // [8], write clear
        uint32_t clr_ap_apb_rst_id_spi2 : 1;     // [9], write clear
        uint32_t clr_ap_apb_rst_id_uart2 : 1;    // [10], write clear
        uint32_t clr_ap_apb_rst_id_uart3 : 1;    // [11], write clear
        uint32_t clr_ap_apb_rst_id_zsp_uart : 1; // [12], write clear
        uint32_t clr_ap_apb_rst_id_rev0 : 1;     // [13], write clear
        uint32_t clr_ap_apb_rst_id_rev1 : 1;     // [14], write clear
        uint32_t clr_ap_apb_rst_id_sci1 : 1;     // [15], write clear
        uint32_t clr_ap_apb_rst_id_sci2 : 1;     // [16], write clear
        uint32_t clr_ap_apb_rst_id_pagespy : 1;  // [17], write clear
        uint32_t clr_ap_apb_rst_id_lzma : 1;     // [18], write clear
        uint32_t clr_ap_apb_rst_id_psram : 1;    // [19], write clear
        uint32_t clr_ap_apb_rst_id_timer1 : 1;   // [20], write clear
        uint32_t clr_ap_apb_rst_id_timer2 : 1;   // [21], write clear
        uint32_t clr_ap_apb_rst_id_timer4 : 1;   // [22], write clear
        uint32_t clr_ap_apb_rst_id_gouda : 1;    // [23], write clear
        uint32_t clr_ap_apb_rst_id_cqirq : 1;    // [24], write clear
        uint32_t __31_25 : 7;                    // [31:25]
    } b;
} REG_SYS_CTRL_AP_APB_RST_CLR_T;

// aif_apb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_aif1_apb_rst_id_sys : 1;    // [0], write set
        uint32_t set_aif2_apb_rst_id_sys : 1;    // [1], write set
        uint32_t set_aif1_apb_rst_id_aif : 1;    // [2], write set
        uint32_t set_aif2_apb_rst_id_aif : 1;    // [3], write set
        uint32_t set_aif_apb_rst_id_ifc : 1;     // [4], write set
        uint32_t set_aif_apb_rst_id_aud_2ad : 1; // [5], write set
        uint32_t __31_6 : 26;                    // [31:6]
    } b;
} REG_SYS_CTRL_AIF_APB_RST_SET_T;

// aif_apb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_aif1_apb_rst_id_sys : 1;    // [0], write clear
        uint32_t clr_aif2_apb_rst_id_sys : 1;    // [1], write clear
        uint32_t clr_aif1_apb_rst_id_aif : 1;    // [2], write clear
        uint32_t clr_aif2_apb_rst_id_aif : 1;    // [3], write clear
        uint32_t clr_aif_apb_rst_id_ifc : 1;     // [4], write clear
        uint32_t clr_aif_apb_rst_id_aud_2ad : 1; // [5], write clear
        uint32_t __31_6 : 26;                    // [31:6]
    } b;
} REG_SYS_CTRL_AIF_APB_RST_CLR_T;

// aon_ahb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_aon_ahb_rst_id_sys : 1;   // [0], write set
        uint32_t set_aon_ahb_rst_id_rev0 : 1;  // [1], write set
        uint32_t set_aon_ahb_rst_id_rev1 : 1;  // [2], write set
        uint32_t set_aon_ahb_rst_id_rev2 : 1;  // [3], write set
        uint32_t set_aon_ahb_rst_id_cssys : 1; // [4], write set
        uint32_t __31_5 : 27;                  // [31:5]
    } b;
} REG_SYS_CTRL_AON_AHB_RST_SET_T;

// aon_ahb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_aon_ahb_rst_id_sys : 1;   // [0], write clear
        uint32_t clr_aon_ahb_rst_id_rev0 : 1;  // [1], write clear
        uint32_t clr_aon_ahb_rst_id_rev1 : 1;  // [2], write clear
        uint32_t clr_aon_ahb_rst_id_rev2 : 1;  // [3], write clear
        uint32_t clr_aon_ahb_rst_id_cssys : 1; // [4], write clear
        uint32_t __31_5 : 27;                  // [31:5]
    } b;
} REG_SYS_CTRL_AON_AHB_RST_CLR_T;

// aon_apb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_aon_apb_rst_id_sys : 1;         // [0], write set
        uint32_t set_aon_apb_rst_id_calendar : 1;    // [1], write set
        uint32_t set_aon_apb_rst_id_gpio : 1;        // [2], write set
        uint32_t set_aon_apb_rst_id_ifc : 1;         // [3], write set
        uint32_t set_aon_apb_rst_id_keypad : 1;      // [4], write set
        uint32_t set_aon_apb_rst_id_pwm : 1;         // [5], write set
        uint32_t set_aon_apb_rst_id_rev0 : 1;        // [6], write set
        uint32_t set_aon_apb_rst_id_rev1 : 1;        // [7], write set
        uint32_t set_aon_apb_rst_id_timer3 : 1;      // [8], write set
        uint32_t set_aon_apb_rst_id_i2c2 : 1;        // [9], write set
        uint32_t set_aon_apb_rst_id_ana_reg : 1;     // [10], write set
        uint32_t set_aon_apb_rst_id_spinlock : 1;    // [11], write set
        uint32_t set_aon_apb_rst_id_lps_gsm : 1;     // [12], write set
        uint32_t set_aon_apb_rst_id_lps_nb : 1;      // [13], write set
        uint32_t set_aon_apb_rst_id_efuse : 1;       // [14], write set
        uint32_t set_aon_apb_rst_id_ahb2apb_adi : 1; // [15], write set
        uint32_t set_aon_apb_rst_id_adi : 1;         // [16], write set
        uint32_t set_aon_apb_rst_id_lvds : 1;        // [17], write set
        uint32_t set_aon_apb_rst_id_uart1 : 1;       // [18], write set
        uint32_t set_aon_apb_rst_id_vad : 1;         // [19], write set
        uint32_t __31_20 : 12;                       // [31:20]
    } b;
} REG_SYS_CTRL_AON_APB_RST_SET_T;

// aon_apb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_aon_apb_rst_id_sys : 1;         // [0], write clear
        uint32_t clr_aon_apb_rst_id_calendar : 1;    // [1], write clear
        uint32_t clr_aon_apb_rst_id_gpio : 1;        // [2], write clear
        uint32_t clr_aon_apb_rst_id_ifc : 1;         // [3], write clear
        uint32_t clr_aon_apb_rst_id_keypad : 1;      // [4], write clear
        uint32_t clr_aon_apb_rst_id_pwm : 1;         // [5], write clear
        uint32_t clr_aon_apb_rst_id_rev0 : 1;        // [6], write clear
        uint32_t clr_aon_apb_rst_id_rev1 : 1;        // [7], write clear
        uint32_t clr_aon_apb_rst_id_timer3 : 1;      // [8], write clear
        uint32_t clr_aon_apb_rst_id_i2c2 : 1;        // [9], write clear
        uint32_t clr_aon_apb_rst_id_ana_reg : 1;     // [10], write clear
        uint32_t clr_aon_apb_rst_id_spinlock : 1;    // [11], write clear
        uint32_t clr_aon_apb_rst_id_lps_gsm : 1;     // [12], write clear
        uint32_t clr_aon_apb_rst_id_lps_nb : 1;      // [13], write clear
        uint32_t clr_aon_apb_rst_id_efuse : 1;       // [14], write clear
        uint32_t clr_aon_apb_rst_id_ahb2apb_adi : 1; // [15], write clear
        uint32_t clr_aon_apb_rst_id_adi : 1;         // [16], write clear
        uint32_t clr_aon_apb_rst_id_lvds : 1;        // [17], write clear
        uint32_t clr_aon_apb_rst_id_uart1 : 1;       // [18], write clear
        uint32_t clr_aon_apb_rst_id_vad : 1;         // [19], write clear
        uint32_t __31_20 : 12;                       // [31:20]
    } b;
} REG_SYS_CTRL_AON_APB_RST_CLR_T;

// rf_ahb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_rf_ahb_rst_id_sys : 1;     // [0], write set
        uint32_t set_rf_ahb_rst_id_rf_rev0 : 1; // [1], write set
        uint32_t __31_2 : 30;                   // [31:2]
    } b;
} REG_SYS_CTRL_RF_AHB_RST_SET_T;

// rf_ahb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_rf_ahb_rst_id_sys : 1;     // [0], write clear
        uint32_t clr_rf_ahb_rst_id_rf_rev0 : 1; // [1], write clear
        uint32_t __31_2 : 30;                   // [31:2]
    } b;
} REG_SYS_CTRL_RF_AHB_RST_CLR_T;

// rf_apb_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_rf_apb_rst_id_sys : 1;  // [0], write set
        uint32_t set_rf_apb_rst_id_rev0 : 1; // [1], write set
        uint32_t __31_2 : 30;                // [31:2]
    } b;
} REG_SYS_CTRL_RF_APB_RST_SET_T;

// rf_apb_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_rf_apb_rst_id_sys : 1;  // [0], write clear
        uint32_t clr_rf_apb_rst_id_rev0 : 1; // [1], write clear
        uint32_t __31_2 : 30;                // [31:2]
    } b;
} REG_SYS_CTRL_RF_APB_RST_CLR_T;

// apcpu_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_apcpu_rst_id_sys : 1;    // [0], write set
        uint32_t set_apcpu_rst_id_core : 1;   // [1], write set
        uint32_t set_apcpu_rst_id_dbg : 1;    // [2], write set
        uint32_t set_apcpu_rst_id_gic400 : 1; // [3], write set
        uint32_t set_global_soft_rst : 1;     // [4], write set
        uint32_t __31_5 : 27;                 // [31:5]
    } b;
} REG_SYS_CTRL_APCPU_RST_SET_T;

// apcpu_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_apcpu_rst_id_sys : 1;    // [0], write clear
        uint32_t clr_apcpu_rst_id_core : 1;   // [1], write clear
        uint32_t clr_apcpu_rst_id_dbg : 1;    // [2], write clear
        uint32_t clr_apcpu_rst_id_gic400 : 1; // [3], write clear
        uint32_t clr_global_soft_rst : 1;     // [4], write clear
        uint32_t __31_5 : 27;                 // [31:5]
    } b;
} REG_SYS_CTRL_APCPU_RST_CLR_T;

// cpcpu_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_cpcpu_rst_id_sys : 1;  // [0], write set
        uint32_t set_cpcpu_rst_id_core : 1; // [1], write set
        uint32_t set_cpcpu_rst_id_dbg : 1;  // [2], write set
        uint32_t set_3 : 1;                 // [3], write set
        uint32_t __31_4 : 28;               // [31:4]
    } b;
} REG_SYS_CTRL_CPCPU_RST_SET_T;

// cpcpu_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_cpcpu_rst_id_sys : 1;  // [0], write clear
        uint32_t clr_cpcpu_rst_id_core : 1; // [1], write clear
        uint32_t clr_cpcpu_rst_id_dbg : 1;  // [2], write clear
        uint32_t clr_3 : 1;                 // [3], write clear
        uint32_t __31_4 : 28;               // [31:4]
    } b;
} REG_SYS_CTRL_CPCPU_RST_CLR_T;

// bblte_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_bblte_rst_id_sys : 1;  // [0], write set
        uint32_t set_bblte_rst_id_rev0 : 1; // [1], write set
        uint32_t __31_2 : 30;               // [31:2]
    } b;
} REG_SYS_CTRL_BBLTE_RST_SET_T;

// bblte_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_bblte_rst_id_sys : 1;  // [0], write clear
        uint32_t clr_bblte_rst_id_rev0 : 1; // [1], write clear
        uint32_t __31_2 : 30;               // [31:2]
    } b;
} REG_SYS_CTRL_BBLTE_RST_CLR_T;

// others_rst_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_rsto_id_bck1 : 1;          // [0], write set
        uint32_t set_rsto_id_bck2 : 1;          // [1], write set
        uint32_t set_rsto_id_dbg_host : 1;      // [2], write set
        uint32_t set_rsto_id_gpio : 1;          // [3], write set
        uint32_t set_rsto_id_uart1 : 1;         // [4], write set
        uint32_t set_rsto_id_uart2 : 1;         // [5], write set
        uint32_t set_rsto_id_uart3 : 1;         // [6], write set
        uint32_t set_rsto_id_usbc : 1;          // [7], write set
        uint32_t set_rsto_id_wdtimer0 : 1;      // [8], write set
        uint32_t set_rsto_id_wdtimer1 : 1;      // [9], write set
        uint32_t set_rsto_id_wdtimer2 : 1;      // [10], write set
        uint32_t set_rsto_id_spiflash : 1;      // [11], write set
        uint32_t set_rsto_id_spiflash_sys : 1;  // [12], write set
        uint32_t set_rsto_id_spiflash1 : 1;     // [13], write set
        uint32_t set_rsto_id_spiflash1_sys : 1; // [14], write set
        uint32_t set_rsto_id_psram_dmc : 1;     // [15], write set
        uint32_t set_rsto_id_psram_sys : 1;     // [16], write set
        uint32_t set_rsto_id_psram_pagespy : 1; // [17], write set
        uint32_t set_rsto_id_vad : 1;           // [18], write set
        uint32_t set_rsto_id_pix : 1;           // [19], write set
        uint32_t set_rsto_id_sdm_26m : 1;       // [20], write set
        uint32_t set_rsto_id_wdtimer4 : 1;      // [21], write set
        uint32_t set_rsto_id_lps_gsm : 1;       // [22], write set
        uint32_t set_rsto_id_lps_nb : 1;        // [23], write set
        uint32_t set_rsto_id_efuse : 1;         // [24], write set
        uint32_t set_rsto_id_usb_adp_32k : 1;   // [25], write set
        uint32_t set_rsto_id_mipidsi : 1;       // [26], write set
        uint32_t set_rsto_id_mipidsi_phy : 1;   // [27], write set
        uint32_t set_rsto_id_aud_2ad : 1;       // [28], write set
        uint32_t set_rsto_id_usb11_48m : 1;     // [29], write set
        uint32_t set_rsto_id_zsp_uart : 1;      // [30], write set
        uint32_t set_rsto_id_coresight : 1;     // [31], write set
    } b;
} REG_SYS_CTRL_OTHERS_RST_SET_T;

// others_rst_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_rsto_id_bck1 : 1;          // [0], write clear
        uint32_t clr_rsto_id_bck2 : 1;          // [1], write clear
        uint32_t clr_rsto_id_dbg_host : 1;      // [2], write clear
        uint32_t clr_rsto_id_gpio : 1;          // [3], write clear
        uint32_t clr_rsto_id_uart1 : 1;         // [4], write clear
        uint32_t clr_rsto_id_uart2 : 1;         // [5], write clear
        uint32_t clr_rsto_id_uart3 : 1;         // [6], write clear
        uint32_t clr_rsto_id_usbc : 1;          // [7], write clear
        uint32_t clr_rsto_id_wdtimer0 : 1;      // [8], write clear
        uint32_t clr_rsto_id_wdtimer1 : 1;      // [9], write clear
        uint32_t clr_rsto_id_wdtimer2 : 1;      // [10], write clear
        uint32_t clr_rsto_id_spiflash : 1;      // [11], write clear
        uint32_t clr_rsto_id_spiflash_sys : 1;  // [12], write clear
        uint32_t clr_rsto_id_spiflash1 : 1;     // [13], write clear
        uint32_t clr_rsto_id_spiflash1_sys : 1; // [14], write clear
        uint32_t clr_rsto_id_psram_dmc : 1;     // [15], write clear
        uint32_t clr_rsto_id_psram_sys : 1;     // [16], write clear
        uint32_t clr_rsto_id_psram_pagespy : 1; // [17], write clear
        uint32_t clr_rsto_id_vad : 1;           // [18], write clear
        uint32_t clr_rsto_id_pix : 1;           // [19], write clear
        uint32_t clr_rsto_id_sdm_26m : 1;       // [20], write clear
        uint32_t clr_rsto_id_wdtimer4 : 1;      // [21], write clear
        uint32_t clr_rsto_id_lps_gsm : 1;       // [22], write clear
        uint32_t clr_rsto_id_lps_nb : 1;        // [23], write clear
        uint32_t clr_rsto_id_efuse : 1;         // [24], write clear
        uint32_t clr_rsto_id_usb_adp_32k : 1;   // [25], write clear
        uint32_t clr_rsto_id_mipidsi : 1;       // [26], write clear
        uint32_t clr_rsto_id_mipidsi_phy : 1;   // [27], write clear
        uint32_t clr_rsto_id_aud_2ad : 1;       // [28], write clear
        uint32_t clr_rsto_id_usb11_48m : 1;     // [29], write clear
        uint32_t clr_rsto_id_zsp_uart : 1;      // [30], write clear
        uint32_t clr_rsto_id_coresight : 1;     // [31], write clear
    } b;
} REG_SYS_CTRL_OTHERS_RST_CLR_T;

// clk_sys_axi_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_sys_axi_clk_id_imem : 1;   // [0]
        uint32_t mode_sys_axi_clk_id_lzma : 1;   // [1]
        uint32_t mode_sys_axi_clk_id_rev0 : 1;   // [2]
        uint32_t mode_sys_axi_clk_id_rev1 : 1;   // [3]
        uint32_t mode_sys_axi_clk_id_rev2 : 1;   // [4]
        uint32_t mode_sys_axi_clk_id_sdmmc1 : 1; // [5]
        uint32_t __31_6 : 26;                    // [31:6]
    } b;
} REG_SYS_CTRL_CLK_SYS_AXI_MODE_T;

// clk_sys_axi_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_sys_axi_clk_id_imem : 1;   // [0], write set
        uint32_t enable_sys_axi_clk_id_lzma : 1;   // [1], write set
        uint32_t enable_sys_axi_clk_id_rev0 : 1;   // [2], write set
        uint32_t enable_sys_axi_clk_id_rev1 : 1;   // [3], write set
        uint32_t enable_sys_axi_clk_id_rev2 : 1;   // [4], write set
        uint32_t enable_sys_axi_clk_id_sdmmc1 : 1; // [5], write set
        uint32_t enable_sys_axi_clk_id_always : 1; // [6], write set
        uint32_t __31_7 : 25;                      // [31:7]
    } b;
} REG_SYS_CTRL_CLK_SYS_AXI_ENABLE_T;

// clk_sys_axi_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_sys_axi_clk_id_imem : 1;   // [0], write clear
        uint32_t disable_sys_axi_clk_id_lzma : 1;   // [1], write clear
        uint32_t disable_sys_axi_clk_id_rev0 : 1;   // [2], write clear
        uint32_t disable_sys_axi_clk_id_rev1 : 1;   // [3], write clear
        uint32_t disable_sys_axi_clk_id_rev2 : 1;   // [4], write clear
        uint32_t disable_sys_axi_clk_id_sdmmc1 : 1; // [5], write clear
        uint32_t disable_sys_axi_clk_id_always : 1; // [6], write clear
        uint32_t __31_7 : 25;                       // [31:7]
    } b;
} REG_SYS_CTRL_CLK_SYS_AXI_DISABLE_T;

// clk_sys_ahb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_sys_ahb_clk_id_gouda : 1;  // [0]
        uint32_t mode_sys_ahb_clk_id_gge : 1;    // [1]
        uint32_t mode_sys_ahb_clk_id_gea3 : 1;   // [2]
        uint32_t mode_sys_ahb_clk_id_lzma : 1;   // [3]
        uint32_t mode_sys_ahb_clk_id_f8 : 1;     // [4]
        uint32_t mode_sys_ahb_clk_id_axidma : 1; // [5]
        uint32_t __31_6 : 26;                    // [31:6]
    } b;
} REG_SYS_CTRL_CLK_SYS_AHB_MODE_T;

// clk_sys_ahb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_sys_ahb_clk_id_gouda : 1;  // [0], write set
        uint32_t enable_sys_ahb_clk_id_gge : 1;    // [1], write set
        uint32_t enable_sys_ahb_clk_id_gea3 : 1;   // [2], write set
        uint32_t enable_sys_ahb_clk_id_lzma : 1;   // [3], write set
        uint32_t enable_sys_ahb_clk_id_f8 : 1;     // [4], write set
        uint32_t enable_sys_ahb_clk_id_axidma : 1; // [5], write set
        uint32_t enable_sys_ahb_clk_id_usbc : 1;   // [6], write set
        uint32_t enable_sys_ahb_clk_id_lcd : 1;    // [7], write set
        uint32_t enable_sys_ahb_clk_id_aes : 1;    // [8], write set
        uint32_t enable_sys_ahb_clk_id_usb11 : 1;  // [9], write set
        uint32_t enable_sys_ahb_clk_id_always : 1; // [10], write set
        uint32_t __31_11 : 21;                     // [31:11]
    } b;
} REG_SYS_CTRL_CLK_SYS_AHB_ENABLE_T;

// clk_sys_ahb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_sys_ahb_clk_id_gouda : 1;  // [0], write clear
        uint32_t disable_sys_ahb_clk_id_gge : 1;    // [1], write clear
        uint32_t disable_sys_ahb_clk_id_gea3 : 1;   // [2], write clear
        uint32_t disable_sys_ahb_clk_id_lzma : 1;   // [3], write clear
        uint32_t disable_sys_ahb_clk_id_f8 : 1;     // [4], write clear
        uint32_t disable_sys_ahb_clk_id_axidma : 1; // [5], write clear
        uint32_t disable_sys_ahb_clk_id_usbc : 1;   // [6], write clear
        uint32_t disable_sys_ahb_clk_id_lcd : 1;    // [7], write clear
        uint32_t disable_sys_ahb_clk_id_aes : 1;    // [8], write clear
        uint32_t disable_sys_ahb_clk_id_usb11 : 1;  // [9], write clear
        uint32_t disable_sys_ahb_clk_id_always : 1; // [10], write clear
        uint32_t __31_11 : 21;                      // [31:11]
    } b;
} REG_SYS_CTRL_CLK_SYS_AHB_DISABLE_T;

// clk_ap_apb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_ap_apb_clk_id_conf : 1;       // [0]
        uint32_t mode_ap_apb_clk_id_mod_camera : 1; // [1]
        uint32_t mode_ap_apb_clk_id_i2c1 : 1;       // [2]
        uint32_t mode_ap_apb_clk_id_i2c3 : 1;       // [3]
        uint32_t mode_ap_apb_clk_id_ifc : 1;        // [4]
        uint32_t mode_ap_apb_clk_id_ifc_ch0 : 1;    // [5]
        uint32_t mode_ap_apb_clk_id_ifc_ch1 : 1;    // [6]
        uint32_t mode_ap_apb_clk_id_ifc_ch2 : 1;    // [7]
        uint32_t mode_ap_apb_clk_id_ifc_ch3 : 1;    // [8]
        uint32_t mode_ap_apb_clk_id_ifc_ch4 : 1;    // [9]
        uint32_t mode_ap_apb_clk_id_ifc_ch5 : 1;    // [10]
        uint32_t mode_ap_apb_clk_id_ifc_ch6 : 1;    // [11]
        uint32_t mode_ap_apb_clk_id_ifc_chdbg : 1;  // [12]
        uint32_t mode_ap_apb_clk_id_gouda : 1;      // [13]
        uint32_t mode_ap_apb_clk_id_sdmmc2 : 1;     // [14]
        uint32_t mode_ap_apb_clk_id_spi1 : 1;       // [15]
        uint32_t mode_ap_apb_clk_id_spi2 : 1;       // [16]
        uint32_t mode_ap_apb_clk_id_scid1 : 1;      // [17]
        uint32_t mode_ap_apb_clk_id_scid2 : 1;      // [18]
        uint32_t mode_ap_apb_clk_id_sci1 : 1;       // [19]
        uint32_t mode_ap_apb_clk_id_sci2 : 1;       // [20]
        uint32_t mode_ap_apb_clk_id_camera : 1;     // [21]
        uint32_t mode_ap_apb_clk_id_pagespy : 1;    // [22]
        uint32_t __31_23 : 9;                       // [31:23]
    } b;
} REG_SYS_CTRL_CLK_AP_APB_MODE_T;

// clk_ap_apb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_ap_apb_clk_id_conf : 1;       // [0], write set
        uint32_t enable_ap_apb_clk_id_mod_camera : 1; // [1], write set
        uint32_t enable_ap_apb_clk_id_i2c1 : 1;       // [2], write set
        uint32_t enable_ap_apb_clk_id_i2c3 : 1;       // [3], write set
        uint32_t enable_ap_apb_clk_id_ifc : 1;        // [4], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch0 : 1;    // [5], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch1 : 1;    // [6], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch2 : 1;    // [7], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch3 : 1;    // [8], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch4 : 1;    // [9], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch5 : 1;    // [10], write set
        uint32_t enable_ap_apb_clk_id_ifc_ch6 : 1;    // [11], write set
        uint32_t enable_ap_apb_clk_id_ifc_chdbg : 1;  // [12], write set
        uint32_t enable_ap_apb_clk_id_gouda : 1;      // [13], write set
        uint32_t enable_ap_apb_clk_id_sdmmc2 : 1;     // [14], write set
        uint32_t enable_ap_apb_clk_id_spi1 : 1;       // [15], write set
        uint32_t enable_ap_apb_clk_id_spi2 : 1;       // [16], write set
        uint32_t enable_ap_apb_clk_id_scid1 : 1;      // [17], write set
        uint32_t enable_ap_apb_clk_id_scid2 : 1;      // [18], write set
        uint32_t enable_ap_apb_clk_id_sci1 : 1;       // [19], write set
        uint32_t enable_ap_apb_clk_id_sci2 : 1;       // [20], write set
        uint32_t enable_ap_apb_clk_id_camera : 1;     // [21], write set
        uint32_t enable_ap_apb_clk_id_pagespy : 1;    // [22], write set
        uint32_t enable_ap_apb_clk_id_lzma : 1;       // [23], write set
        uint32_t enable_ap_apb_clk_id_rev2 : 1;       // [24], write set
        uint32_t enable_ap_apb_clk_id_timer1 : 1;     // [25], write set
        uint32_t enable_ap_apb_clk_id_timer2 : 1;     // [26], write set
        uint32_t enable_ap_apb_clk_id_timer4 : 1;     // [27], write set
        uint32_t enable_ap_apb_clk_id_lcd : 1;        // [28], write set
        uint32_t enable_ap_apb_clk_id_cqirq : 1;      // [29], write set
        uint32_t enable_ap_apb_clk_id_always : 1;     // [30], write set
        uint32_t __31_31 : 1;                         // [31]
    } b;
} REG_SYS_CTRL_CLK_AP_APB_ENABLE_T;

// clk_ap_apb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_ap_apb_clk_id_conf : 1;       // [0], write clear
        uint32_t disable_ap_apb_clk_id_mod_camera : 1; // [1], write clear
        uint32_t disable_ap_apb_clk_id_i2c1 : 1;       // [2], write clear
        uint32_t disable_ap_apb_clk_id_i2c3 : 1;       // [3], write clear
        uint32_t disable_ap_apb_clk_id_ifc : 1;        // [4], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch0 : 1;    // [5], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch1 : 1;    // [6], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch2 : 1;    // [7], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch3 : 1;    // [8], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch4 : 1;    // [9], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch5 : 1;    // [10], write clear
        uint32_t disable_ap_apb_clk_id_ifc_ch6 : 1;    // [11], write clear
        uint32_t disable_ap_apb_clk_id_ifc_chdbg : 1;  // [12], write clear
        uint32_t disable_ap_apb_clk_id_gouda : 1;      // [13], write clear
        uint32_t disable_ap_apb_clk_id_sdmmc2 : 1;     // [14], write clear
        uint32_t disable_ap_apb_clk_id_spi1 : 1;       // [15], write clear
        uint32_t disable_ap_apb_clk_id_spi2 : 1;       // [16], write clear
        uint32_t disable_ap_apb_clk_id_scid1 : 1;      // [17], write clear
        uint32_t disable_ap_apb_clk_id_scid2 : 1;      // [18], write clear
        uint32_t disable_ap_apb_clk_id_sci1 : 1;       // [19], write clear
        uint32_t disable_ap_apb_clk_id_sci2 : 1;       // [20], write clear
        uint32_t disable_ap_apb_clk_id_camera : 1;     // [21], write clear
        uint32_t disable_ap_apb_clk_id_pagespy : 1;    // [22], write clear
        uint32_t disable_ap_apb_clk_id_lzma : 1;       // [23], write clear
        uint32_t disable_ap_apb_clk_id_rev2 : 1;       // [24], write clear
        uint32_t disable_ap_apb_clk_id_timer1 : 1;     // [25], write clear
        uint32_t disable_ap_apb_clk_id_timer2 : 1;     // [26], write clear
        uint32_t disable_ap_apb_clk_id_timer4 : 1;     // [27], write clear
        uint32_t disable_ap_apb_clk_id_lcd : 1;        // [28], write clear
        uint32_t disable_ap_apb_clk_id_cqirq : 1;      // [29], write clear
        uint32_t disable_ap_apb_clk_id_always : 1;     // [30], write clear
        uint32_t __31_31 : 1;                          // [31]
    } b;
} REG_SYS_CTRL_CLK_AP_APB_DISABLE_T;

// clk_aif_apb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_aif_apb_clk_id_conf : 1;    // [0]
        uint32_t mode_aif1_apb_clk_id_aif : 1;    // [1]
        uint32_t mode_aif2_apb_clk_id_aif : 1;    // [2]
        uint32_t mode_aif_apb_clk_id_ifc : 1;     // [3]
        uint32_t mode_aif_apb_clk_id_ifc_ch0 : 1; // [4]
        uint32_t mode_aif_apb_clk_id_ifc_ch1 : 1; // [5]
        uint32_t mode_aif_apb_clk_id_ifc_ch2 : 1; // [6]
        uint32_t mode_aif_apb_clk_id_ifc_ch3 : 1; // [7]
        uint32_t mode_aif_apb_clk_id_rev0 : 1;    // [8]
        uint32_t mode_aif_apb_clk_id_rev1 : 1;    // [9]
        uint32_t mode_aif_apb_clk_id_rev2 : 1;    // [10]
        uint32_t mode_aif_apb_clk_id_aud_2ad : 1; // [11]
        uint32_t __31_12 : 20;                    // [31:12]
    } b;
} REG_SYS_CTRL_CLK_AIF_APB_MODE_T;

// clk_aif_apb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_aif_apb_clk_id_conf : 1;    // [0], write set
        uint32_t enable_aif1_apb_clk_id_aif : 1;    // [1], write set
        uint32_t enable_aif2_apb_clk_id_aif : 1;    // [2], write set
        uint32_t enable_aif_apb_clk_id_ifc : 1;     // [3], write set
        uint32_t enable_aif_apb_clk_id_ifc_ch0 : 1; // [4], write set
        uint32_t enable_aif_apb_clk_id_ifc_ch1 : 1; // [5], write set
        uint32_t enable_aif_apb_clk_id_ifc_ch2 : 1; // [6], write set
        uint32_t enable_aif_apb_clk_id_ifc_ch3 : 1; // [7], write set
        uint32_t enable_aif_apb_clk_id_rev0 : 1;    // [8], write set
        uint32_t enable_aif_apb_clk_id_rev1 : 1;    // [9], write set
        uint32_t enable_aif_apb_clk_id_rev2 : 1;    // [10], write set
        uint32_t enable_aif_apb_clk_id_aud_2ad : 1; // [11], write set
        uint32_t enable_aif_apb_clk_id_always : 1;  // [12], write set
        uint32_t __31_13 : 19;                      // [31:13]
    } b;
} REG_SYS_CTRL_CLK_AIF_APB_ENABLE_T;

// clk_aif_apb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_aif_apb_clk_id_conf : 1;    // [0], write clear
        uint32_t disable_aif1_apb_clk_id_aif : 1;    // [1], write clear
        uint32_t disable_aif2_apb_clk_id_aif : 1;    // [2], write clear
        uint32_t disable_aif_apb_clk_id_ifc : 1;     // [3], write clear
        uint32_t disable_aif_apb_clk_id_ifc_ch0 : 1; // [4], write clear
        uint32_t disable_aif_apb_clk_id_ifc_ch1 : 1; // [5], write clear
        uint32_t disable_aif_apb_clk_id_ifc_ch2 : 1; // [6], write clear
        uint32_t disable_aif_apb_clk_id_ifc_ch3 : 1; // [7], write clear
        uint32_t disable_aif_apb_clk_id_rev0 : 1;    // [8], write clear
        uint32_t disable_aif_apb_clk_id_rev1 : 1;    // [9], write clear
        uint32_t disable_aif_apb_clk_id_rev2 : 1;    // [10], write clear
        uint32_t disable_aif_apb_clk_id_aud_2ad : 1; // [11], write clear
        uint32_t disable_aif_apb_clk_id_always : 1;  // [12], write clear
        uint32_t __31_13 : 19;                       // [31:13]
    } b;
} REG_SYS_CTRL_CLK_AIF_APB_DISABLE_T;

// clk_aon_ahb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_aon_ahb_clk_id_bb_sysctrl : 1; // [0]
        uint32_t mode_aon_ahb_clk_id_bb_rev0 : 1;    // [1]
        uint32_t mode_aon_ahb_clk_id_bb_rev1 : 1;    // [2]
        uint32_t mode_aon_ahb_clk_id_bb_rev2 : 1;    // [3]
        uint32_t mode_aon_ahb_clk_id_cssys : 1;      // [4]
        uint32_t __31_5 : 27;                        // [31:5]
    } b;
} REG_SYS_CTRL_CLK_AON_AHB_MODE_T;

// clk_aon_ahb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_aon_ahb_clk_id_bb_sysctrl : 1; // [0], write set
        uint32_t enable_aon_ahb_clk_id_bb_rev0 : 1;    // [1], write set
        uint32_t enable_aon_ahb_clk_id_bb_rev1 : 1;    // [2], write set
        uint32_t enable_aon_ahb_clk_id_bb_rev2 : 1;    // [3], write set
        uint32_t enable_aon_ahb_clk_id_cssys : 1;      // [4], write set
        uint32_t enable_aon_ahb_clk_id_always : 1;     // [5], write set
        uint32_t __31_6 : 26;                          // [31:6]
    } b;
} REG_SYS_CTRL_CLK_AON_AHB_ENABLE_T;

// clk_aon_ahb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_aon_ahb_clk_id_bb_sysctrl : 1; // [0], write clear
        uint32_t disable_aon_ahb_clk_id_bb_rev0 : 1;    // [1], write clear
        uint32_t disable_aon_ahb_clk_id_bb_rev1 : 1;    // [2], write clear
        uint32_t disable_aon_ahb_clk_id_bb_rev2 : 1;    // [3], write clear
        uint32_t disable_aon_ahb_clk_id_cssys : 1;      // [4], write clear
        uint32_t disable_aon_ahb_clk_id_always : 1;     // [5], write clear
        uint32_t __31_6 : 26;                           // [31:6]
    } b;
} REG_SYS_CTRL_CLK_AON_AHB_DISABLE_T;

// clk_aon_apb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_aon_apb_clk_id_conf : 1;      // [0]
        uint32_t mode_aon_apb_clk_id_dbg_host : 1;  // [1]
        uint32_t mode_aon_apb_clk_id_dbg_uart : 1;  // [2]
        uint32_t mode_aon_apb_clk_id_ifc : 1;       // [3]
        uint32_t mode_aon_apb_clk_id_ifc_ch0 : 1;   // [4]
        uint32_t mode_aon_apb_clk_id_ifc_ch1 : 1;   // [5]
        uint32_t mode_aon_apb_clk_id_ifc_ch2 : 1;   // [6]
        uint32_t mode_aon_apb_clk_id_ifc_ch3 : 1;   // [7]
        uint32_t mode_aon_apb_clk_id_ifc_chdbg : 1; // [8]
        uint32_t mode_aon_apb_clk_id_rev0 : 1;      // [9]
        uint32_t mode_aon_apb_clk_id_rev1 : 1;      // [10]
        uint32_t mode_aon_apb_clk_id_pwm : 1;       // [11]
        uint32_t mode_aon_apb_clk_id_i2c2 : 1;      // [12]
        uint32_t mode_aon_apb_clk_id_ana_reg : 1;   // [13]
        uint32_t mode_aon_apb_clk_id_spinlock : 1;  // [14]
        uint32_t mode_aon_apb_clk_id_lps_gsm : 1;   // [15]
        uint32_t mode_aon_apb_clk_id_lps_nb : 1;    // [16]
        uint32_t mode_aon_apb_clk_id_efuse : 1;     // [17]
        uint32_t mode_aon_apb_clk_id_rev2 : 1;      // [18]
        uint32_t mode_aon_apb_clk_id_vad : 1;       // [19]
        uint32_t __31_20 : 12;                      // [31:20]
    } b;
} REG_SYS_CTRL_CLK_AON_APB_MODE_T;

// clk_aon_apb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_aon_apb_clk_id_conf : 1;        // [0], write set
        uint32_t enable_aon_apb_clk_id_dbg_host : 1;    // [1], write set
        uint32_t enable_aon_apb_clk_id_dbg_uart : 1;    // [2], write set
        uint32_t enable_aon_apb_clk_id_ifc : 1;         // [3], write set
        uint32_t enable_aon_apb_clk_id_ifc_ch0 : 1;     // [4], write set
        uint32_t enable_aon_apb_clk_id_ifc_ch1 : 1;     // [5], write set
        uint32_t enable_aon_apb_clk_id_ifc_ch2 : 1;     // [6], write set
        uint32_t enable_aon_apb_clk_id_ifc_ch3 : 1;     // [7], write set
        uint32_t enable_aon_apb_clk_id_ifc_chdbg : 1;   // [8], write set
        uint32_t enable_aon_apb_clk_id_rev0 : 1;        // [9], write set
        uint32_t enable_aon_apb_clk_id_rev1 : 1;        // [10], write set
        uint32_t enable_aon_apb_clk_id_pwm : 1;         // [11], write set
        uint32_t enable_aon_apb_clk_id_i2c2 : 1;        // [12], write set
        uint32_t enable_aon_apb_clk_id_ana_reg : 1;     // [13], write set
        uint32_t enable_aon_apb_clk_id_spinlock : 1;    // [14], write set
        uint32_t enable_aon_apb_clk_id_lps_gsm : 1;     // [15], write set
        uint32_t enable_aon_apb_clk_id_lps_nb : 1;      // [16], write set
        uint32_t enable_aon_apb_clk_id_efuse : 1;       // [17], write set
        uint32_t enable_aon_apb_clk_id_rev2 : 1;        // [18], write set
        uint32_t enable_aon_apb_clk_id_vad : 1;         // [19], write set
        uint32_t enable_aon_apb_clk_id_rev3 : 1;        // [20], write set
        uint32_t enable_aon_apb_clk_id_rev4 : 1;        // [21], write set
        uint32_t enable_aon_apb_clk_id_gpio : 1;        // [22], write set
        uint32_t enable_aon_apb_clk_id_timer3 : 1;      // [23], write set
        uint32_t enable_aon_apb_clk_id_keypad : 1;      // [24], write set
        uint32_t enable_aon_apb_clk_id_ahb2apb_adi : 1; // [25], write set
        uint32_t enable_aon_apb_clk_id_adi : 1;         // [26], write set
        uint32_t enable_aon_apb_clk_id_lvds : 1;        // [27], write set
        uint32_t enable_aon_apb_clk_id_always : 1;      // [28], write set
        uint32_t __31_29 : 3;                           // [31:29]
    } b;
} REG_SYS_CTRL_CLK_AON_APB_ENABLE_T;

// clk_aon_apb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_aon_apb_clk_id_conf : 1;        // [0], write clear
        uint32_t disable_aon_apb_clk_id_dbg_host : 1;    // [1], write clear
        uint32_t disable_aon_apb_clk_id_dbg_uart : 1;    // [2], write clear
        uint32_t disable_aon_apb_clk_id_ifc : 1;         // [3], write clear
        uint32_t disable_aon_apb_clk_id_ifc_ch0 : 1;     // [4], write clear
        uint32_t disable_aon_apb_clk_id_ifc_ch1 : 1;     // [5], write clear
        uint32_t disable_aon_apb_clk_id_ifc_ch2 : 1;     // [6], write clear
        uint32_t disable_aon_apb_clk_id_ifc_ch3 : 1;     // [7], write clear
        uint32_t disable_aon_apb_clk_id_ifc_chdbg : 1;   // [8], write clear
        uint32_t disable_aon_apb_clk_id_rev0 : 1;        // [9], write clear
        uint32_t disable_aon_apb_clk_id_rev1 : 1;        // [10], write clear
        uint32_t disable_aon_apb_clk_id_pwm : 1;         // [11], write clear
        uint32_t disable_aon_apb_clk_id_i2c2 : 1;        // [12], write clear
        uint32_t disable_aon_apb_clk_id_ana_reg : 1;     // [13], write clear
        uint32_t disable_aon_apb_clk_id_spinlock : 1;    // [14], write clear
        uint32_t disable_aon_apb_clk_id_lps_gsm : 1;     // [15], write clear
        uint32_t disable_aon_apb_clk_id_lps_nb : 1;      // [16], write clear
        uint32_t disable_aon_apb_clk_id_efuse : 1;       // [17], write clear
        uint32_t disable_aon_apb_clk_id_rev2 : 1;        // [18], write clear
        uint32_t disable_aon_apb_clk_id_vad : 1;         // [19], write clear
        uint32_t disable_aon_apb_clk_id_rev3 : 1;        // [20], write clear
        uint32_t disable_aon_apb_clk_id_rev4 : 1;        // [21], write clear
        uint32_t disable_aon_apb_clk_id_gpio : 1;        // [22], write clear
        uint32_t disable_aon_apb_clk_id_timer3 : 1;      // [23], write clear
        uint32_t disable_aon_apb_clk_id_keypad : 1;      // [24], write clear
        uint32_t disable_aon_apb_clk_id_ahb2apb_adi : 1; // [25], write clear
        uint32_t disable_aon_apb_clk_id_adi : 1;         // [26], write clear
        uint32_t disable_aon_apb_clk_id_lvds : 1;        // [27], write clear
        uint32_t disable_aon_apb_clk_id_always : 1;      // [28], write clear
        uint32_t __31_29 : 3;                            // [31:29]
    } b;
} REG_SYS_CTRL_CLK_AON_APB_DISABLE_T;

// clk_rf_ahb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_clk_rf_ahb : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_SYS_CTRL_CLK_RF_AHB_MODE_T;

// clk_rf_ahb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_rf_ahb_clk_id_rev0 : 1;   // [0], write set
        uint32_t enable_rf_ahb_clk_id_always : 1; // [1], write set
        uint32_t __31_2 : 30;                     // [31:2]
    } b;
} REG_SYS_CTRL_CLK_RF_AHB_ENABLE_T;

// clk_rf_ahb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_rf_ahb_clk_id_rev0 : 1;   // [0], write clear
        uint32_t disable_rf_ahb_clk_id_always : 1; // [1], write clear
        uint32_t __31_2 : 30;                      // [31:2]
    } b;
} REG_SYS_CTRL_CLK_RF_AHB_DISABLE_T;

// clk_rf_apb_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_clk_rf_apb : 1; // [0]
        uint32_t __31_1 : 31;         // [31:1]
    } b;
} REG_SYS_CTRL_CLK_RF_APB_MODE_T;

// clk_rf_apb_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_rf_apb_clk_id_rev0 : 1;   // [0], write set
        uint32_t enable_rf_apb_clk_id_always : 1; // [1], write set
        uint32_t __31_2 : 30;                     // [31:2]
    } b;
} REG_SYS_CTRL_CLK_RF_APB_ENABLE_T;

// clk_rf_apb_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_rf_apb_clk_id_rev0 : 1;   // [0], write clear
        uint32_t disable_rf_apb_clk_id_always : 1; // [1], write clear
        uint32_t __31_2 : 30;                      // [31:2]
    } b;
} REG_SYS_CTRL_CLK_RF_APB_DISABLE_T;

// clk_others_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_oc_id_host_uart : 1; // [0]
        uint32_t mode_oc_id_bck1 : 1;      // [1]
        uint32_t mode_oc_id_bck2 : 1;      // [2]
        uint32_t mode_oc_id_uart1 : 1;     // [3]
        uint32_t mode_oc_id_uart2 : 1;     // [4]
        uint32_t mode_oc_id_uart3 : 1;     // [5]
        uint32_t mode_oc_id_ap_a5 : 1;     // [6]
        uint32_t mode_oc_id_cp_a5 : 1;     // [7]
        uint32_t __31_8 : 24;              // [31:8]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_MODE_T;

// clk_others_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_oc_id_host_uart : 1;            // [0], write set
        uint32_t enable_oc_id_bck1 : 1;                 // [1], write set
        uint32_t enable_oc_id_bck2 : 1;                 // [2], write set
        uint32_t enable_oc_id_uart1 : 1;                // [3], write set
        uint32_t enable_oc_id_uart2 : 1;                // [4], write set
        uint32_t enable_oc_id_uart3 : 1;                // [5], write set
        uint32_t enable_oc_id_ap_a5 : 1;                // [6], write set
        uint32_t enable_oc_id_cp_a5 : 1;                // [7], write set
        uint32_t enable_oc_id_gpio : 1;                 // [8], write set
        uint32_t enable_oc_id_usbphy : 1;               // [9], write set
        uint32_t enable_oc_id_pix : 1;                  // [10], write set
        uint32_t enable_oc_id_clk_out : 1;              // [11], write set
        uint32_t enable_oc_id_isp : 1;                  // [12], write set
        uint32_t enable_oc_id_sys_spiflash : 1;         // [13], write set
        uint32_t enable_oc_id_sys_spiflash_always : 1;  // [14], write set
        uint32_t enable_oc_id_sys_spiflash1 : 1;        // [15], write set
        uint32_t enable_oc_id_sys_spiflash1_always : 1; // [16], write set
        uint32_t enable_oc_id_spiflash : 1;             // [17], write set
        uint32_t enable_oc_id_spiflash1 : 1;            // [18], write set
        uint32_t enable_oc_id_spicam : 1;               // [19], write set
        uint32_t enable_oc_id_cam : 1;                  // [20], write set
        uint32_t enable_oc_id_psram_conf : 1;           // [21], write set
        uint32_t enable_oc_id_psram_dmc : 1;            // [22], write set
        uint32_t enable_oc_id_psram_pagespy : 1;        // [23], write set
        uint32_t enable_oc_id_psram_always : 1;         // [24], write set
        uint32_t __31_25 : 7;                           // [31:25]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_ENABLE_T;

// clk_others_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_oc_id_host_uart : 1;            // [0], write clear
        uint32_t disable_oc_id_bck1 : 1;                 // [1], write clear
        uint32_t disable_oc_id_bck2 : 1;                 // [2], write clear
        uint32_t disable_oc_id_uart1 : 1;                // [3], write clear
        uint32_t disable_oc_id_uart2 : 1;                // [4], write clear
        uint32_t disable_oc_id_uart3 : 1;                // [5], write clear
        uint32_t disable_oc_id_ap_a5 : 1;                // [6], write clear
        uint32_t disable_oc_id_cp_a5 : 1;                // [7], write clear
        uint32_t disable_oc_id_gpio : 1;                 // [8], write clear
        uint32_t disable_oc_id_usbphy : 1;               // [9], write clear
        uint32_t disable_oc_id_pix : 1;                  // [10], write clear
        uint32_t disable_oc_id_clk_out : 1;              // [11], write clear
        uint32_t disable_oc_id_isp : 1;                  // [12], write clear
        uint32_t disable_oc_id_sys_spiflash : 1;         // [13], write clear
        uint32_t disable_oc_id_sys_spiflash_always : 1;  // [14], write clear
        uint32_t disable_oc_id_sys_spiflash1 : 1;        // [15], write clear
        uint32_t disable_oc_id_sys_spiflash1_always : 1; // [16], write clear
        uint32_t disable_oc_id_spiflash : 1;             // [17], write clear
        uint32_t disable_oc_id_spiflash1 : 1;            // [18], write clear
        uint32_t disable_oc_id_spicam : 1;               // [19], write clear
        uint32_t disable_oc_id_cam : 1;                  // [20], write clear
        uint32_t disable_oc_id_psram_conf : 1;           // [21], write clear
        uint32_t disable_oc_id_psram_dmc : 1;            // [22], write clear
        uint32_t disable_oc_id_psram_pagespy : 1;        // [23], write clear
        uint32_t disable_oc_id_psram_always : 1;         // [24], write clear
        uint32_t __31_25 : 7;                            // [31:25]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_DISABLE_T;

// clk_others_1_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mode_oc_id_gge_clk32k : 1;   // [0]
        uint32_t mode_oc_id_gge_26m : 1;      // [1]
        uint32_t mode_oc_id_nb_61p44m : 1;    // [2]
        uint32_t mode_oc_id_bt_fm_clk32k : 1; // [3]
        uint32_t mode_oc_id_bt_fm_26m : 1;    // [4]
        uint32_t mode_oc_id_pmic_clk32k : 1;  // [5]
        uint32_t mode_oc_id_pmic_26m : 1;     // [6]
        uint32_t mode_oc_id_cp_clk32k : 1;    // [7]
        uint32_t mode_oc_id_cp_26m : 1;       // [8]
        uint32_t mode_oc_id_zsp_uart : 1;     // [9]
        uint32_t mode_oc_id_cp_bblte : 1;     // [10]
        uint32_t mode_oc_id_494m_lte : 1;     // [11]
        uint32_t mode_oc_id_rev0 : 1;         // [12]
        uint32_t mode_oc_id_rev1 : 1;         // [13]
        uint32_t mode_oc_id_rev2 : 1;         // [14]
        uint32_t mode_oc_id_rev3 : 1;         // [15]
        uint32_t __31_16 : 16;                // [31:16]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_1_MODE_T;

// clk_others_1_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_oc_id_gge_clk32k : 1;   // [0], write set
        uint32_t enable_oc_id_gge_26m : 1;      // [1], write set
        uint32_t enable_oc_id_nb_61p44m : 1;    // [2], write set
        uint32_t enable_oc_id_bt_fm_clk32k : 1; // [3], write set
        uint32_t enable_oc_id_bt_fm_26m : 1;    // [4], write set
        uint32_t enable_oc_id_pmic_clk32k : 1;  // [5], write set
        uint32_t enable_oc_id_pmic_26m : 1;     // [6], write set
        uint32_t enable_oc_id_cp_clk32k : 1;    // [7], write set
        uint32_t enable_oc_id_cp_26m : 1;       // [8], write set
        uint32_t enable_oc_id_zsp_uart : 1;     // [9], write set
        uint32_t enable_oc_id_cp_bblte : 1;     // [10], write set
        uint32_t enable_oc_id_494m_lte : 1;     // [11], write set
        uint32_t enable_oc_id_rev0 : 1;         // [12], write set
        uint32_t enable_oc_id_rev1 : 1;         // [13], write set
        uint32_t enable_oc_id_rev2 : 1;         // [14], write set
        uint32_t enable_oc_id_rev3 : 1;         // [15], write set
        uint32_t enable_oc_id_sdm_26m : 1;      // [16], write set
        uint32_t enable_oc_id_lps_gsm : 1;      // [17], write set
        uint32_t enable_oc_id_lps_nb : 1;       // [18], write set
        uint32_t enable_oc_id_efuse_26m : 1;    // [19], write set
        uint32_t enable_oc_id_usb_adp_32k : 1;  // [20], write set
        uint32_t enable_oc_id_usb_utmi_48m : 1; // [21], write set
        uint32_t enable_oc_id_ap_26m : 1;       // [22], write set
        uint32_t enable_oc_id_ap_32k : 1;       // [23], write set
        uint32_t enable_oc_id_mipidsi : 1;      // [24], write set
        uint32_t enable_oc_id_ahb_btfm : 1;     // [25], write set
        uint32_t enable_oc_id_vad : 1;          // [26], write set
        uint32_t enable_oc_id_usb11_48m : 1;    // [27], write set
        uint32_t enable_oc_id_trng_clken : 1;   // [28], write set
        uint32_t enable_oc_id_coresight : 1;    // [29], write set
        uint32_t enable_oc_id_adi : 1;          // [30], write set
        uint32_t __31_31 : 1;                   // [31]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_1_ENABLE_T;

// clk_others_1_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_oc_id_gge_clk32k : 1;   // [0], write clear
        uint32_t disable_oc_id_gge_26m : 1;      // [1], write clear
        uint32_t disable_oc_id_nb_61p44m : 1;    // [2], write clear
        uint32_t disable_oc_id_bt_fm_clk32k : 1; // [3], write clear
        uint32_t disable_oc_id_bt_fm_26m : 1;    // [4], write clear
        uint32_t disable_oc_id_pmic_clk32k : 1;  // [5], write clear
        uint32_t disable_oc_id_pmic_26m : 1;     // [6], write clear
        uint32_t disable_oc_id_cp_clk32k : 1;    // [7], write clear
        uint32_t disable_oc_id_cp_26m : 1;       // [8], write clear
        uint32_t disable_oc_id_zsp_uart : 1;     // [9], write clear
        uint32_t disable_oc_id_cp_bblte : 1;     // [10], write clear
        uint32_t disable_oc_id_494m_lte : 1;     // [11], write clear
        uint32_t disable_oc_id_rev0 : 1;         // [12], write clear
        uint32_t disable_oc_id_rev1 : 1;         // [13], write clear
        uint32_t disable_oc_id_rev2 : 1;         // [14], write clear
        uint32_t disable_oc_id_rev3 : 1;         // [15], write clear
        uint32_t disable_oc_id_sdm_26m : 1;      // [16], write clear
        uint32_t disable_oc_id_lps_gsm : 1;      // [17], write clear
        uint32_t disable_oc_id_lps_nb : 1;       // [18], write clear
        uint32_t disable_oc_id_efuse_26m : 1;    // [19], write clear
        uint32_t disable_oc_id_usb_adp_32k : 1;  // [20], write clear
        uint32_t disable_oc_id_usb_utmi_48m : 1; // [21], write clear
        uint32_t disable_oc_id_ap_26m : 1;       // [22], write clear
        uint32_t disable_oc_id_ap_32k : 1;       // [23], write clear
        uint32_t disable_oc_id_mipidsi : 1;      // [24], write clear
        uint32_t disable_oc_id_ahb_btfm : 1;     // [25], write clear
        uint32_t disable_oc_id_vad : 1;          // [26], write clear
        uint32_t disable_oc_id_usb11_48m : 1;    // [27], write clear
        uint32_t disable_oc_id_trng_clken : 1;   // [28], write clear
        uint32_t disable_oc_id_coresight : 1;    // [29], write clear
        uint32_t disable_oc_id_adi : 1;          // [30], write clear
        uint32_t __31_31 : 1;                    // [31]
    } b;
} REG_SYS_CTRL_CLK_OTHERS_1_DISABLE_T;

// pll_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pll_enable : 1;          // [0]
        uint32_t __3_1 : 3;               // [3:1]
        uint32_t pll_lock_reset : 1;      // [4]
        uint32_t __7_5 : 3;               // [7:5]
        uint32_t pll_bypass_lock : 1;     // [8]
        uint32_t __11_9 : 3;              // [11:9]
        uint32_t pll_clk_fast_enable : 1; // [12]
        uint32_t __31_13 : 19;            // [31:13]
    } b;
} REG_SYS_CTRL_PLL_CTRL_T;

// sel_clock
typedef union {
    uint32_t v;
    struct
    {
        uint32_t slow_sel_rf : 1;            // [0]
        uint32_t sys_sel_fast : 1;           // [1]
        uint32_t osc_32k_26m_div32k_sel : 1; // [2]
        uint32_t pll_disable_lps : 1;        // [3]
        uint32_t rf_detected : 1;            // [4], read only
        uint32_t rf_detect_bypass : 1;       // [5]
        uint32_t rf_detect_reset : 1;        // [6]
        uint32_t rf_selected_l : 1;          // [7], read only
        uint32_t pll_locked : 1;             // [8], read only
        uint32_t fast_selected_l : 1;        // [9], read only
        uint32_t soft_sel_spiflash : 1;      // [10]
        uint32_t soft_sel_mem_bridge : 1;    // [11]
        uint32_t bblte_clk_pll_sel : 1;      // [12]
        uint32_t camera_clk_pll_sel : 1;     // [13]
        uint32_t usb_pll_locked_h : 1;       // [14], read only
        uint32_t bb26m_sel : 1;              // [15]
        uint32_t soft_sel_spiflash1 : 1;     // [16]
        uint32_t apll_locked_h : 1;          // [17], read only
        uint32_t mempll_locked_h : 1;        // [18], read only
        uint32_t audiopll_locked_h : 1;      // [19], read only
        uint32_t bbpll2_locked_h : 1;        // [20], read only
        uint32_t bbpll1_locked_h : 1;        // [21], read only
        uint32_t usbpll_locked_h : 1;        // [22], read only
        uint32_t __31_23 : 9;                // [31:23]
    } b;
} REG_SYS_CTRL_SEL_CLOCK_T;

// cfg_clk_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clkout_divider : 5; // [4:0]
        uint32_t clkout_dbg_sel : 1; // [5]
        uint32_t clkout_enable : 1;  // [6]
        uint32_t clkout_update : 1;  // [7]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_SYS_CTRL_CFG_CLK_OUT_T;

// cfg_clk_audiobck1_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audiobck1_divider : 11; // [10:0]
        uint32_t audiobck1_update : 1;   // [11]
        uint32_t __31_12 : 20;           // [31:12]
    } b;
} REG_SYS_CTRL_CFG_CLK_AUDIOBCK1_DIV_T;

// cfg_clk_audiobck2_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t audiobck2_divider : 11; // [10:0]
        uint32_t audiobck2_update : 1;   // [11]
        uint32_t __31_12 : 20;           // [31:12]
    } b;
} REG_SYS_CTRL_CFG_CLK_AUDIOBCK2_DIV_T;

// cfg_clk_uart
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_divider : 24;       // [23:0]
        uint32_t uart_divider_update : 1; // [24]
        uint32_t uart_sel_pll : 1;        // [25]
        uint32_t __31_26 : 6;             // [31:26]
    } b;
} REG_SYS_CTRL_CFG_CLK_UART_T;

// cfg_clk_pwm
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwm_divider : 8; // [7:0]
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_SYS_CTRL_CFG_CLK_PWM_T;

// cfg_clk_dbg_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_dbg_divider : 11; // [10:0]
        uint32_t __31_11 : 21;         // [31:11]
    } b;
} REG_SYS_CTRL_CFG_CLK_DBG_DIV_T;

// cfg_clk_camera_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_camera_out_en : 1;      // [0]
        uint32_t clk_camera_div_src_sel : 1; // [1]
        uint32_t clk_camera_out_div : 11;    // [12:2]
        uint32_t clk_camera_div_update : 1;  // [13]
        uint32_t clk_spi_cam_en : 1;         // [14]
        uint32_t clk_spi_cam_pol : 1;        // [15]
        uint32_t clk_spi_cam_sel : 1;        // [16]
        uint32_t clk_spi_cam_div : 11;       // [27:17]
        uint32_t clk_spi_cam_div_update : 1; // [28]
        uint32_t __31_29 : 3;                // [31:29]
    } b;
} REG_SYS_CTRL_CFG_CLK_CAMERA_OUT_T;

// reset_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t watchdog_reset_1 : 1;     // [0]
        uint32_t watchdog_reset_2 : 1;     // [1]
        uint32_t watchdog_reset_3 : 1;     // [2]
        uint32_t watchdog_reset_4 : 1;     // [3]
        uint32_t watchdog_reset_rf : 1;    // [4]
        uint32_t watchdog_reset_gge : 1;   // [5]
        uint32_t watchdog_reset_zsp : 1;   // [6]
        uint32_t globalsoft_reset : 1;     // [7], read only
        uint32_t hostdebug_reset : 1;      // [8], read only
        uint32_t watchdog_reset_cp : 1;    // [9]
        uint32_t __11_10 : 2;              // [11:10]
        uint32_t alarmcause : 1;           // [12], read only
        uint32_t __15_13 : 3;              // [15:13]
        uint32_t boot_mode : 6;            // [21:16]
        uint32_t sw_boot_mode : 6;         // [27:22]
        uint32_t __30_28 : 3;              // [30:28]
        uint32_t fonctional_test_mode : 1; // [31], read only
    } b;
} REG_SYS_CTRL_RESET_CAUSE_T;

// wakeup
typedef union {
    uint32_t v;
    struct
    {
        uint32_t force_wakeup : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_SYS_CTRL_WAKEUP_T;

// ignore_charger
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ignore_charger : 1; // [0]
        uint32_t __31_1 : 31;        // [31:1]
    } b;
} REG_SYS_CTRL_IGNORE_CHARGER_T;

// cfg_pll_spiflash_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_spiflash_div : 4;        // [3:0]
        uint32_t cfg_pll_spiflash_div_update : 1; // [4]
        uint32_t __31_5 : 27;                     // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SPIFLASH_DIV_T;

// cfg_pll_spiflash1_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_spiflash1_div : 4;        // [3:0]
        uint32_t cfg_pll_spiflash1_div_update : 1; // [4]
        uint32_t __31_5 : 27;                      // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SPIFLASH1_DIV_T;

// cfg_pll_mem_bridge_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_mem_bridge_div : 4;            // [3:0]
        uint32_t cfg_pll_mem_bridge_div_update : 1; // [4]
        uint32_t __31_5 : 27;                       // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_MEM_BRIDGE_DIV_T;

// cfg_dbg_clk_source_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_dbg_clk_source_sel : 6; // [5:0]
        uint32_t __31_6 : 26;                // [31:6]
    } b;
} REG_SYS_CTRL_CFG_DBG_CLK_SOURCE_SEL_T;

// cfg_clk_494m_lte_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_clk_494m_lte_apll_div : 4;        // [3:0]
        uint32_t cfg_clk_494m_lte_apll_div_update : 1; // [4]
        uint32_t cfg_clksw_494m_lte_usbphy_sel : 1;    // [5]
        uint32_t cfg_clksw_494m_clk_sel : 1;           // [6]
        uint32_t cfg_clk_96m_26m_sel : 1;              // [7]
        uint32_t cfg_clk_96m_div : 23;                 // [30:8]
        uint32_t cfg_clk_96m_div_update : 1;           // [31]
    } b;
} REG_SYS_CTRL_CFG_CLK_494M_LTE_DIV_T;

// cfg_pll_isp_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_isp_div : 12;       // [11:0]
        uint32_t cfg_pll_isp_div_update : 1; // [12]
        uint32_t __31_13 : 19;               // [31:13]
    } b;
} REG_SYS_CTRL_CFG_PLL_ISP_DIV_T;

// cfg_pll_pix_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_pix_div : 7;        // [6:0]
        uint32_t cfg_pll_pix_div_update : 1; // [7]
        uint32_t __31_8 : 24;                // [31:8]
    } b;
} REG_SYS_CTRL_CFG_PLL_PIX_DIV_T;

// cfg_pll_sys_axi_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_sys_axi_div : 4;        // [3:0]
        uint32_t cfg_pll_sys_axi_div_update : 1; // [4]
        uint32_t __31_5 : 27;                    // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SYS_AXI_DIV_T;

// cfg_pll_sys_ahb_apb_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_ap_apb_div : 4;        // [3:0]
        uint32_t cfg_pll_ap_apb_div_update : 1; // [4]
        uint32_t __31_5 : 27;                   // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SYS_AHB_APB_DIV_T;

// cfg_pll_sys_ahb_btfm_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_ahb_btfm_div : 4;        // [3:0]
        uint32_t cfg_pll_ahb_btfm_div_update : 1; // [4]
        uint32_t __31_5 : 27;                     // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SYS_AHB_BTFM_DIV_T;

// cfg_pll_csi_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_csi_div : 7;        // [6:0]
        uint32_t cfg_pll_csi_div_update : 1; // [7]
        uint32_t __31_8 : 24;                // [31:8]
    } b;
} REG_SYS_CTRL_CFG_PLL_CSI_DIV_T;

// cfg_pll_sys_spiflash_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_sys_spiflash_div : 4;        // [3:0]
        uint32_t cfg_pll_sys_spiflash_div_update : 1; // [4]
        uint32_t __31_5 : 27;                         // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_SYS_SPIFLASH_DIV_T;

// cfg_pll_cp_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_cp_div : 4;        // [3:0]
        uint32_t cfg_pll_cp_div_update : 1; // [4]
        uint32_t __31_5 : 27;               // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_CP_DIV_T;

// cfg_pll_ap_cpu_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_ap_cpu_div : 4;        // [3:0]
        uint32_t cfg_pll_ap_cpu_div_update : 1; // [4]
        uint32_t __31_5 : 27;                   // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_AP_CPU_DIV_T;

// cfg_ap_cpu_aclken_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_ap_cpu_aclken_div : 3;        // [2:0]
        uint32_t cfg_ap_cpu_aclken_div_update : 1; // [3]
        uint32_t __31_4 : 28;                      // [31:4]
    } b;
} REG_SYS_CTRL_CFG_AP_CPU_ACLKEN_DIV_T;

// cfg_ap_cpu_dbgen_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_ap_cpu_dbgen_div : 3;        // [2:0]
        uint32_t cfg_ap_cpu_dbgen_div_update : 1; // [3]
        uint32_t __31_4 : 28;                     // [31:4]
    } b;
} REG_SYS_CTRL_CFG_AP_CPU_DBGEN_DIV_T;

// cfg_pll_cp_cpu_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_cp_cpu_div : 4;        // [3:0]
        uint32_t cfg_pll_cp_cpu_div_update : 1; // [4]
        uint32_t __31_5 : 27;                   // [31:5]
    } b;
} REG_SYS_CTRL_CFG_PLL_CP_CPU_DIV_T;

// cfg_cp_cpu_aclken_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_cp_cpu_aclken_div : 3;        // [2:0]
        uint32_t cfg_cp_cpu_aclken_div_update : 1; // [3]
        uint32_t __31_4 : 28;                      // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CP_CPU_ACLKEN_DIV_T;

// cfg_cp_cpu_dbgen_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_cp_cpu_dbgen_div : 3;        // [2:0]
        uint32_t cfg_cp_cpu_dbgen_div_update : 1; // [3]
        uint32_t __31_4 : 28;                     // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CP_CPU_DBGEN_DIV_T;

// cfg_trng_clken_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_trng_clken_div : 5;        // [4:0]
        uint32_t cfg_trng_clken_div_update : 1; // [5]
        uint32_t __31_6 : 26;                   // [31:6]
    } b;
} REG_SYS_CTRL_CFG_TRNG_CLKEN_DIV_T;

// cfg_coresight_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_coresight_div : 4;        // [3:0]
        uint32_t cfg_coresight_div_update : 1; // [4]
        uint32_t __31_5 : 27;                  // [31:5]
    } b;
} REG_SYS_CTRL_CFG_CORESIGHT_DIV_T;

// cfg_usb11_48m_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_usb11_48m_div : 5;        // [4:0]
        uint32_t cfg_usb11_48m_div_update : 1; // [5]
        uint32_t __31_6 : 26;                  // [31:6]
    } b;
} REG_SYS_CTRL_CFG_USB11_48M_DIV_T;

// cfg_pll_usb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_pll_usb_reset : 1; // [0]
        uint32_t cfg_pll_usb_rev : 1;   // [1]
        uint32_t __31_2 : 30;           // [31:2]
    } b;
} REG_SYS_CTRL_CFG_PLL_USB_T;

// cfg_vad_div
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_vad_div : 4;        // [3:0]
        uint32_t cfg_vad_div_update : 1; // [4]
        uint32_t __31_5 : 27;            // [31:5]
    } b;
} REG_SYS_CTRL_CFG_VAD_DIV_T;

// cfg_hmprot
typedef union {
    uint32_t v;
    struct
    {
        uint32_t hprot_bbdma : 4;       // [3:0]
        uint32_t hprot_aif_ifc : 4;     // [7:4]
        uint32_t hprot_gouda : 4;       // [11:8]
        uint32_t hprot_ap_ifc : 4;      // [15:12]
        uint32_t hprot_usb : 4;         // [19:16]
        uint32_t hprot_sys_aon_ifc : 4; // [23:20]
        uint32_t hsprot_psram_ctrl : 4; // [27:24]
        uint32_t __31_28 : 4;           // [31:28]
    } b;
} REG_SYS_CTRL_CFG_HMPROT_T;

// cfg_aif_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aif1_load_pos : 6;     // [5:0]
        uint32_t aif2_load_pos : 6;     // [11:6]
        uint32_t aif1_sel : 3;          // [14:12]
        uint32_t aif2_sel : 3;          // [17:15]
        uint32_t i2s1_sel : 3;          // [20:18]
        uint32_t i2s2_sel : 3;          // [23:21]
        uint32_t i2s3_sel : 3;          // [26:24]
        uint32_t i2s1_bck_lrck_oen : 1; // [27]
        uint32_t i2s2_bck_lrck_oen : 1; // [28]
        uint32_t i2s3_bck_lrck_oen : 1; // [29]
        uint32_t __31_30 : 2;           // [31:30]
    } b;
} REG_SYS_CTRL_CFG_AIF_CFG_T;

// cfg_misc_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t limit_en_spi1 : 1;      // [0]
        uint32_t limit_en_spi2 : 1;      // [1]
        uint32_t limit_en_spi3 : 1;      // [2]
        uint32_t cfgsdisable_gic400 : 1; // [3]
        uint32_t aif1_i2s_bck_sel : 3;   // [6:4]
        uint32_t __7_7 : 1;              // [7]
        uint32_t aif2_i2s_bck_sel : 3;   // [10:8]
        uint32_t wcn_uart_out_sel : 1;   // [11]
        uint32_t ap_uart_out_sel : 1;    // [12]
        uint32_t cfg_mode_lp : 1;        // [13]
        uint32_t cfg_force_lp : 1;       // [14]
        uint32_t cfg_number_lp : 16;     // [30:15]
        uint32_t bcpu_stall_ack : 1;     // [31], read only
    } b;
} REG_SYS_CTRL_CFG_MISC_CFG_T;

// cfg_misc1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t debug_mon_sel : 5;         // [4:0]
        uint32_t iomux_clk_force_on : 1;    // [5]
        uint32_t ap_rst_ctrl : 1;           // [6]
        uint32_t gge_rst_ctrl : 1;          // [7]
        uint32_t btfm_rst_ctrl : 1;         // [8]
        uint32_t ap_clk_ctrl : 1;           // [9]
        uint32_t gge_clk_ctrl : 1;          // [10]
        uint32_t btfm_clk_ctrl : 1;         // [11]
        uint32_t bbpll1_enable : 1;         // [12]
        uint32_t bbpll2_enable : 1;         // [13]
        uint32_t mempll_enable : 1;         // [14]
        uint32_t usbpll_enable : 1;         // [15]
        uint32_t audiopll_enable : 1;       // [16]
        uint32_t apll_zsp_clk_sel : 1;      // [17]
        uint32_t apll_bbpll2_clk_sel : 1;   // [18]
        uint32_t reg_gic400_aruser_sel : 1; // [19]
        uint32_t reg_gic400_aruser_dbg : 1; // [20]
        uint32_t reg_gic400_awuser_sel : 1; // [21]
        uint32_t reg_gic400_awuser_dbg : 1; // [22]
        uint32_t lvds_wcn_rfdig_sel : 1;    // [23]
        uint32_t lvds_rfdig_rf_bb_sel : 1;  // [24]
        uint32_t wcn_osc_en_ctrl : 1;       // [25]
        uint32_t vad_clk_force_on : 1;      // [26]
        uint32_t vad_clk_pn_sel : 1;        // [27]
        uint32_t aud_sclk_o_pn_sel : 1;     // [28]
        uint32_t aon_lp_rst_ctrl : 1;       // [29]
        uint32_t aon_lp_clk_ctrl : 1;       // [30]
        uint32_t dump_path : 1;             // [31]
    } b;
} REG_SYS_CTRL_CFG_MISC1_CFG_T;

// cfg_force_lp_mode_lp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_force_lp_a5 : 1;        // [0]
        uint32_t cfg_force_lp_ahb : 1;       // [1]
        uint32_t cfg_force_lp_connect : 1;   // [2]
        uint32_t cfg_force_lp_cp_a5 : 1;     // [3]
        uint32_t cfg_force_lp_psram : 1;     // [4]
        uint32_t cfg_force_lp_spiflash : 1;  // [5]
        uint32_t cfg_force_lp_spiflash1 : 1; // [6]
        uint32_t cfg_mode_lp_a5 : 1;         // [7]
        uint32_t cfg_mode_lp_ahb : 1;        // [8]
        uint32_t cfg_mode_lp_connect : 1;    // [9]
        uint32_t cfg_mode_lp_cp_a5 : 1;      // [10]
        uint32_t cfg_mode_lp_psram : 1;      // [11]
        uint32_t cfg_mode_lp_spiflash : 1;   // [12]
        uint32_t cfg_mode_lp_spiflash1 : 1;  // [13]
        uint32_t cfg_mode_lp_ahb_merge : 1;  // [14]
        uint32_t force_lp_ahb_merge : 1;     // [15]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_SYS_CTRL_CFG_FORCE_LP_MODE_LP_T;

// cfg_reserve
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wd_rst_mode : 1;     // [0]
        uint32_t sys_bb_side : 1;     // [1]
        uint32_t cam_out0_sel : 1;    // [2]
        uint32_t vpu_clk_en : 1;      // [3]
        uint32_t wd_1_rst_en : 1;     // [4]
        uint32_t wd_2_rst_en : 1;     // [5]
        uint32_t wd_3_rst_en : 1;     // [6]
        uint32_t wd_4_rst_en : 1;     // [7]
        uint32_t wd_rf_rst_en : 1;    // [8]
        uint32_t wd_gge_rst_en : 1;   // [9]
        uint32_t wd_zsp_rst_en : 1;   // [10]
        uint32_t wd_cp_rst_en : 1;    // [11]
        uint32_t dmc_phy_rst_en : 1;  // [12]
        uint32_t hmprot_wcn_peri : 4; // [16:13]
        uint32_t hmprot_wcn_mem : 4;  // [20:17]
        uint32_t hmprot_aes : 4;      // [24:21]
        uint32_t reserve : 7;         // [31:25]
    } b;
} REG_SYS_CTRL_CFG_RESERVE_T;

// cfg_chip_prod_id
typedef union {
    uint32_t v;
    struct
    {
        uint32_t metal_id : 12; // [11:0], read only
        uint32_t bond_id : 4;   // [15:12], read only
        uint32_t prod_id : 16;  // [31:16], read only
    } b;
} REG_SYS_CTRL_CFG_CHIP_PROD_ID_T;

// cfg_qos_wcn_a5_gge
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wcn_mem_arqos : 5; // [4:0]
        uint32_t wcn_mem_awqos : 5; // [9:5]
        uint32_t gge_arqos : 5;     // [14:10]
        uint32_t gge_awqos : 5;     // [19:15]
        uint32_t a5_arqos : 5;      // [24:20]
        uint32_t a5_awqos : 5;      // [29:25]
        uint32_t __31_30 : 2;       // [31:30]
    } b;
} REG_SYS_CTRL_CFG_QOS_WCN_A5_GGE_T;

// cfg_qos_axidma_cpa5_f8
typedef union {
    uint32_t v;
    struct
    {
        uint32_t axidma_arqos : 5; // [4:0]
        uint32_t axidma_awqos : 5; // [9:5]
        uint32_t cp_a5_arqos : 5;  // [14:10]
        uint32_t cp_a5_awqos : 5;  // [19:15]
        uint32_t f8_arqos : 5;     // [24:20]
        uint32_t f8_awqos : 5;     // [29:25]
        uint32_t __31_30 : 2;      // [31:30]
    } b;
} REG_SYS_CTRL_CFG_QOS_AXIDMA_CPA5_F8_T;

// cfg_qos_lcdc_lzma_gouda
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lcdc_arqos : 5;  // [4:0]
        uint32_t lcdc_awqos : 5;  // [9:5]
        uint32_t lzma_arqos : 5;  // [14:10]
        uint32_t lzma_awqos : 5;  // [19:15]
        uint32_t gouda_arqos : 5; // [24:20]
        uint32_t gouda_awqos : 5; // [29:25]
        uint32_t __31_30 : 2;     // [31:30]
    } b;
} REG_SYS_CTRL_CFG_QOS_LCDC_LZMA_GOUDA_T;

// cfg_qos_lte_usb
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lte_arqos : 5; // [4:0]
        uint32_t lte_awqos : 5; // [9:5]
        uint32_t usb_arqos : 5; // [14:10]
        uint32_t usb_awqos : 5; // [19:15]
        uint32_t __31_20 : 12;  // [31:20]
    } b;
} REG_SYS_CTRL_CFG_QOS_LTE_USB_T;

// cfg_qos_merge_mem
typedef union {
    uint32_t v;
    struct
    {
        uint32_t merge_mem_awqos : 5; // [4:0]
        uint32_t merge_mem_arqos : 5; // [9:5]
        uint32_t __31_10 : 22;        // [31:10]
    } b;
} REG_SYS_CTRL_CFG_QOS_MERGE_MEM_T;

// cfg_bcpu_dbg_bkp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bcpu_bkpt_addr : 28;  // [27:0]
        uint32_t bcpu_bkpt_mode : 2;   // [29:28]
        uint32_t bcpu_bkpt_en : 1;     // [30]
        uint32_t bcpu_stalled_w1c : 1; // [31]
    } b;
} REG_SYS_CTRL_CFG_BCPU_DBG_BKP_T;

// reg_dbg
#define SYS_CTRL_SCRATCH(n) (((n)&0xffff) << 0)
#define SYS_CTRL_WRITE_UNLOCK_STATUS (1 << 30)
#define SYS_CTRL_WRITE_UNLOCK (1 << 31)

// sys_axi_rst_set
#define SYS_CTRL_SET_SYS_AXI_RST(n) (((n)&0x7f) << 0)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_IMEM (1 << 1)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_LZMA (1 << 2)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_REV0 (1 << 3)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_REV1 (1 << 4)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_REV2 (1 << 5)
#define SYS_CTRL_SET_SYS_AXI_RST_ID_REV3 (1 << 6)

// sys_axi_rst_clr
#define SYS_CTRL_CLR_SYS_AXI_RST(n) (((n)&0x7f) << 0)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_IMEM (1 << 1)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_LZMA (1 << 2)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_REV0 (1 << 3)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_REV1 (1 << 4)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_REV2 (1 << 5)
#define SYS_CTRL_CLR_SYS_AXI_RST_ID_REV3 (1 << 6)

// sys_ahb_rst_set
#define SYS_CTRL_SET_SYS_AHB_RST(n) (((n)&0x1fff) << 0)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_GOUDA (1 << 1)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_GGE (1 << 2)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_GEA3 (1 << 3)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_USBC (1 << 4)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_BTFM (1 << 5)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_GIC400 (1 << 6)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_F8 (1 << 7)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_AXIDMA (1 << 8)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_LZMA (1 << 9)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_LCD (1 << 10)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_AES (1 << 11)
#define SYS_CTRL_SET_SYS_AHB_RST_ID_USB11 (1 << 12)

// sys_ahb_rst_clr
#define SYS_CTRL_CLR_SYS_AHB_RST(n) (((n)&0x1fff) << 0)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_GOUDA (1 << 1)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_GGE (1 << 2)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_GEA3 (1 << 3)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_USBC (1 << 4)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_BTFM (1 << 5)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_GIC400 (1 << 6)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_F8 (1 << 7)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_AXIDMA (1 << 8)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_LZMA (1 << 9)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_LCD (1 << 10)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_AES (1 << 11)
#define SYS_CTRL_CLR_SYS_AHB_RST_ID_USB11 (1 << 12)

// ap_apb_rst_set
#define SYS_CTRL_SET_AP_APB_RST(n) (((n)&0x1ffffff) << 0)
#define SYS_CTRL_SET_AP_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_AP_APB_RST_ID_CAMERA (1 << 1)
#define SYS_CTRL_SET_AP_APB_RST_ID_I2C1 (1 << 2)
#define SYS_CTRL_SET_AP_APB_RST_ID_I2C3 (1 << 3)
#define SYS_CTRL_SET_AP_APB_RST_ID_IFC (1 << 4)
#define SYS_CTRL_SET_AP_APB_RST_ID_IMEM (1 << 5)
#define SYS_CTRL_SET_AP_APB_RST_ID_SDMMC1 (1 << 6)
#define SYS_CTRL_SET_AP_APB_RST_ID_SDMMC2 (1 << 7)
#define SYS_CTRL_SET_AP_APB_RST_ID_SPI1 (1 << 8)
#define SYS_CTRL_SET_AP_APB_RST_ID_SPI2 (1 << 9)
#define SYS_CTRL_SET_AP_APB_RST_ID_UART2 (1 << 10)
#define SYS_CTRL_SET_AP_APB_RST_ID_UART3 (1 << 11)
#define SYS_CTRL_SET_AP_APB_RST_ID_ZSP_UART (1 << 12)
#define SYS_CTRL_SET_AP_APB_RST_ID_REV0 (1 << 13)
#define SYS_CTRL_SET_AP_APB_RST_ID_REV1 (1 << 14)
#define SYS_CTRL_SET_AP_APB_RST_ID_SCI1 (1 << 15)
#define SYS_CTRL_SET_AP_APB_RST_ID_SCI2 (1 << 16)
#define SYS_CTRL_SET_AP_APB_RST_ID_PAGESPY (1 << 17)
#define SYS_CTRL_SET_AP_APB_RST_ID_LZMA (1 << 18)
#define SYS_CTRL_SET_AP_APB_RST_ID_PSRAM (1 << 19)
#define SYS_CTRL_SET_AP_APB_RST_ID_TIMER1 (1 << 20)
#define SYS_CTRL_SET_AP_APB_RST_ID_TIMER2 (1 << 21)
#define SYS_CTRL_SET_AP_APB_RST_ID_TIMER4 (1 << 22)
#define SYS_CTRL_SET_AP_APB_RST_ID_GOUDA (1 << 23)
#define SYS_CTRL_SET_AP_APB_RST_ID_CQIRQ (1 << 24)

// ap_apb_rst_clr
#define SYS_CTRL_CLR_AP_APB_RST(n) (((n)&0x1ffffff) << 0)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_AP_APB_RST_ID_CAMERA (1 << 1)
#define SYS_CTRL_CLR_AP_APB_RST_ID_I2C1 (1 << 2)
#define SYS_CTRL_CLR_AP_APB_RST_ID_I2C3 (1 << 3)
#define SYS_CTRL_CLR_AP_APB_RST_ID_IFC (1 << 4)
#define SYS_CTRL_CLR_AP_APB_RST_ID_IMEM (1 << 5)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SDMMC1 (1 << 6)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SDMMC2 (1 << 7)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SPI1 (1 << 8)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SPI2 (1 << 9)
#define SYS_CTRL_CLR_AP_APB_RST_ID_UART2 (1 << 10)
#define SYS_CTRL_CLR_AP_APB_RST_ID_UART3 (1 << 11)
#define SYS_CTRL_CLR_AP_APB_RST_ID_ZSP_UART (1 << 12)
#define SYS_CTRL_CLR_AP_APB_RST_ID_REV0 (1 << 13)
#define SYS_CTRL_CLR_AP_APB_RST_ID_REV1 (1 << 14)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SCI1 (1 << 15)
#define SYS_CTRL_CLR_AP_APB_RST_ID_SCI2 (1 << 16)
#define SYS_CTRL_CLR_AP_APB_RST_ID_PAGESPY (1 << 17)
#define SYS_CTRL_CLR_AP_APB_RST_ID_LZMA (1 << 18)
#define SYS_CTRL_CLR_AP_APB_RST_ID_PSRAM (1 << 19)
#define SYS_CTRL_CLR_AP_APB_RST_ID_TIMER1 (1 << 20)
#define SYS_CTRL_CLR_AP_APB_RST_ID_TIMER2 (1 << 21)
#define SYS_CTRL_CLR_AP_APB_RST_ID_TIMER4 (1 << 22)
#define SYS_CTRL_CLR_AP_APB_RST_ID_GOUDA (1 << 23)
#define SYS_CTRL_CLR_AP_APB_RST_ID_CQIRQ (1 << 24)

// aif_apb_rst_set
#define SYS_CTRL_SET_AIF_APB_RST(n) (((n)&0x3f) << 0)
#define SYS_CTRL_SET_AIF1_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_AIF2_APB_RST_ID_SYS (1 << 1)
#define SYS_CTRL_SET_AIF1_APB_RST_ID_AIF (1 << 2)
#define SYS_CTRL_SET_AIF2_APB_RST_ID_AIF (1 << 3)
#define SYS_CTRL_SET_AIF_APB_RST_ID_IFC (1 << 4)
#define SYS_CTRL_SET_AIF_APB_RST_ID_AUD_2AD (1 << 5)

// aif_apb_rst_clr
#define SYS_CTRL_CLR_AIF_APB_RST(n) (((n)&0x3f) << 0)
#define SYS_CTRL_CLR_AIF1_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_AIF2_APB_RST_ID_SYS (1 << 1)
#define SYS_CTRL_CLR_AIF1_APB_RST_ID_AIF (1 << 2)
#define SYS_CTRL_CLR_AIF2_APB_RST_ID_AIF (1 << 3)
#define SYS_CTRL_CLR_AIF_APB_RST_ID_IFC (1 << 4)
#define SYS_CTRL_CLR_AIF_APB_RST_ID_AUD_2AD (1 << 5)

// aon_ahb_rst_set
#define SYS_CTRL_SET_AON_AHB_RST(n) (((n)&0x1f) << 0)
#define SYS_CTRL_SET_AON_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_AON_AHB_RST_ID_REV0 (1 << 1)
#define SYS_CTRL_SET_AON_AHB_RST_ID_REV1 (1 << 2)
#define SYS_CTRL_SET_AON_AHB_RST_ID_REV2 (1 << 3)
#define SYS_CTRL_SET_AON_AHB_RST_ID_CSSYS (1 << 4)

// aon_ahb_rst_clr
#define SYS_CTRL_CLR_AON_AHB_RST(n) (((n)&0x1f) << 0)
#define SYS_CTRL_CLR_AON_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_AON_AHB_RST_ID_REV0 (1 << 1)
#define SYS_CTRL_CLR_AON_AHB_RST_ID_REV1 (1 << 2)
#define SYS_CTRL_CLR_AON_AHB_RST_ID_REV2 (1 << 3)
#define SYS_CTRL_CLR_AON_AHB_RST_ID_CSSYS (1 << 4)

// aon_apb_rst_set
#define SYS_CTRL_SET_AON_APB_RST(n) (((n)&0xfffff) << 0)
#define SYS_CTRL_SET_AON_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_AON_APB_RST_ID_CALENDAR (1 << 1)
#define SYS_CTRL_SET_AON_APB_RST_ID_GPIO (1 << 2)
#define SYS_CTRL_SET_AON_APB_RST_ID_IFC (1 << 3)
#define SYS_CTRL_SET_AON_APB_RST_ID_KEYPAD (1 << 4)
#define SYS_CTRL_SET_AON_APB_RST_ID_PWM (1 << 5)
#define SYS_CTRL_SET_AON_APB_RST_ID_REV0 (1 << 6)
#define SYS_CTRL_SET_AON_APB_RST_ID_REV1 (1 << 7)
#define SYS_CTRL_SET_AON_APB_RST_ID_TIMER3 (1 << 8)
#define SYS_CTRL_SET_AON_APB_RST_ID_I2C2 (1 << 9)
#define SYS_CTRL_SET_AON_APB_RST_ID_ANA_REG (1 << 10)
#define SYS_CTRL_SET_AON_APB_RST_ID_SPINLOCK (1 << 11)
#define SYS_CTRL_SET_AON_APB_RST_ID_LPS_GSM (1 << 12)
#define SYS_CTRL_SET_AON_APB_RST_ID_LPS_NB (1 << 13)
#define SYS_CTRL_SET_AON_APB_RST_ID_EFUSE (1 << 14)
#define SYS_CTRL_SET_AON_APB_RST_ID_AHB2APB_ADI (1 << 15)
#define SYS_CTRL_SET_AON_APB_RST_ID_ADI (1 << 16)
#define SYS_CTRL_SET_AON_APB_RST_ID_LVDS (1 << 17)
#define SYS_CTRL_SET_AON_APB_RST_ID_UART1 (1 << 18)
#define SYS_CTRL_SET_AON_APB_RST_ID_VAD (1 << 19)

// aon_apb_rst_clr
#define SYS_CTRL_CLR_AON_APB_RST(n) (((n)&0xfffff) << 0)
#define SYS_CTRL_CLR_AON_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_AON_APB_RST_ID_CALENDAR (1 << 1)
#define SYS_CTRL_CLR_AON_APB_RST_ID_GPIO (1 << 2)
#define SYS_CTRL_CLR_AON_APB_RST_ID_IFC (1 << 3)
#define SYS_CTRL_CLR_AON_APB_RST_ID_KEYPAD (1 << 4)
#define SYS_CTRL_CLR_AON_APB_RST_ID_PWM (1 << 5)
#define SYS_CTRL_CLR_AON_APB_RST_ID_REV0 (1 << 6)
#define SYS_CTRL_CLR_AON_APB_RST_ID_REV1 (1 << 7)
#define SYS_CTRL_CLR_AON_APB_RST_ID_TIMER3 (1 << 8)
#define SYS_CTRL_CLR_AON_APB_RST_ID_I2C2 (1 << 9)
#define SYS_CTRL_CLR_AON_APB_RST_ID_ANA_REG (1 << 10)
#define SYS_CTRL_CLR_AON_APB_RST_ID_SPINLOCK (1 << 11)
#define SYS_CTRL_CLR_AON_APB_RST_ID_LPS_GSM (1 << 12)
#define SYS_CTRL_CLR_AON_APB_RST_ID_LPS_NB (1 << 13)
#define SYS_CTRL_CLR_AON_APB_RST_ID_EFUSE (1 << 14)
#define SYS_CTRL_CLR_AON_APB_RST_ID_AHB2APB_ADI (1 << 15)
#define SYS_CTRL_CLR_AON_APB_RST_ID_ADI (1 << 16)
#define SYS_CTRL_CLR_AON_APB_RST_ID_LVDS (1 << 17)
#define SYS_CTRL_CLR_AON_APB_RST_ID_UART1 (1 << 18)
#define SYS_CTRL_CLR_AON_APB_RST_ID_VAD (1 << 19)

// rf_ahb_rst_set
#define SYS_CTRL_SET_RF_AHB_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_SET_RF_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_RF_AHB_RST_ID_RF_REV0 (1 << 1)

// rf_ahb_rst_clr
#define SYS_CTRL_CLR_RF_AHB_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_CLR_RF_AHB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_RF_AHB_RST_ID_RF_REV0 (1 << 1)

// rf_apb_rst_set
#define SYS_CTRL_SET_RF_APB_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_SET_RF_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_RF_APB_RST_ID_REV0 (1 << 1)

// rf_apb_rst_clr
#define SYS_CTRL_CLR_RF_APB_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_CLR_RF_APB_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_RF_APB_RST_ID_REV0 (1 << 1)

// apcpu_rst_set
#define SYS_CTRL_SET_APCPU_RST(n) (((n)&0x1f) << 0)
#define SYS_CTRL_SET_APCPU_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_APCPU_RST_ID_CORE (1 << 1)
#define SYS_CTRL_SET_APCPU_RST_ID_DBG (1 << 2)
#define SYS_CTRL_SET_APCPU_RST_ID_GIC400 (1 << 3)
#define SYS_CTRL_SET_GLOBAL_SOFT_RST (1 << 4)

// apcpu_rst_clr
#define SYS_CTRL_CLR_APCPU_RST(n) (((n)&0x1f) << 0)
#define SYS_CTRL_CLR_APCPU_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_APCPU_RST_ID_CORE (1 << 1)
#define SYS_CTRL_CLR_APCPU_RST_ID_DBG (1 << 2)
#define SYS_CTRL_CLR_APCPU_RST_ID_GIC400 (1 << 3)
#define SYS_CTRL_CLR_GLOBAL_SOFT_RST (1 << 4)

// cpcpu_rst_set
#define SYS_CTRL_SET_CPCPU_RST(n) (((n)&0xf) << 0)
#define SYS_CTRL_SET_CPCPU_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_CPCPU_RST_ID_CORE (1 << 1)
#define SYS_CTRL_SET_CPCPU_RST_ID_DBG (1 << 2)
#define SYS_CTRL_SET_3 (1 << 3)

// cpcpu_rst_clr
#define SYS_CTRL_CLR_CPCPU_RST(n) (((n)&0xf) << 0)
#define SYS_CTRL_CLR_CPCPU_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_CPCPU_RST_ID_CORE (1 << 1)
#define SYS_CTRL_CLR_CPCPU_RST_ID_DBG (1 << 2)
#define SYS_CTRL_CLR_3 (1 << 3)

// bblte_rst_set
#define SYS_CTRL_SET_BBLTE_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_SET_BBLTE_RST_ID_SYS (1 << 0)
#define SYS_CTRL_SET_BBLTE_RST_ID_REV0 (1 << 1)

// bblte_rst_clr
#define SYS_CTRL_CLR_BBLTE_RST(n) (((n)&0x3) << 0)
#define SYS_CTRL_CLR_BBLTE_RST_ID_SYS (1 << 0)
#define SYS_CTRL_CLR_BBLTE_RST_ID_REV0 (1 << 1)

// others_rst_set
#define SYS_CTRL_SET_OTHERS_RST(n) (((n)&0xffffffff) << 0)
#define SYS_CTRL_SET_RSTO_ID_BCK1 (1 << 0)
#define SYS_CTRL_SET_RSTO_ID_BCK2 (1 << 1)
#define SYS_CTRL_SET_RSTO_ID_DBG_HOST (1 << 2)
#define SYS_CTRL_SET_RSTO_ID_GPIO (1 << 3)
#define SYS_CTRL_SET_RSTO_ID_UART1 (1 << 4)
#define SYS_CTRL_SET_RSTO_ID_UART2 (1 << 5)
#define SYS_CTRL_SET_RSTO_ID_UART3 (1 << 6)
#define SYS_CTRL_SET_RSTO_ID_USBC (1 << 7)
#define SYS_CTRL_SET_RSTO_ID_WDTIMER0 (1 << 8)
#define SYS_CTRL_SET_RSTO_ID_WDTIMER1 (1 << 9)
#define SYS_CTRL_SET_RSTO_ID_WDTIMER2 (1 << 10)
#define SYS_CTRL_SET_RSTO_ID_SPIFLASH (1 << 11)
#define SYS_CTRL_SET_RSTO_ID_SPIFLASH_SYS (1 << 12)
#define SYS_CTRL_SET_RSTO_ID_SPIFLASH1 (1 << 13)
#define SYS_CTRL_SET_RSTO_ID_SPIFLASH1_SYS (1 << 14)
#define SYS_CTRL_SET_RSTO_ID_PSRAM_DMC (1 << 15)
#define SYS_CTRL_SET_RSTO_ID_PSRAM_SYS (1 << 16)
#define SYS_CTRL_SET_RSTO_ID_PSRAM_PAGESPY (1 << 17)
#define SYS_CTRL_SET_RSTO_ID_VAD (1 << 18)
#define SYS_CTRL_SET_RSTO_ID_PIX (1 << 19)
#define SYS_CTRL_SET_RSTO_ID_SDM_26M (1 << 20)
#define SYS_CTRL_SET_RSTO_ID_WDTIMER4 (1 << 21)
#define SYS_CTRL_SET_RSTO_ID_LPS_GSM (1 << 22)
#define SYS_CTRL_SET_RSTO_ID_LPS_NB (1 << 23)
#define SYS_CTRL_SET_RSTO_ID_EFUSE (1 << 24)
#define SYS_CTRL_SET_RSTO_ID_USB_ADP_32K (1 << 25)
#define SYS_CTRL_SET_RSTO_ID_MIPIDSI (1 << 26)
#define SYS_CTRL_SET_RSTO_ID_MIPIDSI_PHY (1 << 27)
#define SYS_CTRL_SET_RSTO_ID_AUD_2AD (1 << 28)
#define SYS_CTRL_SET_RSTO_ID_USB11_48M (1 << 29)
#define SYS_CTRL_SET_RSTO_ID_ZSP_UART (1 << 30)
#define SYS_CTRL_SET_RSTO_ID_CORESIGHT (1 << 31)

// others_rst_clr
#define SYS_CTRL_CLR_OTHERS_RST(n) (((n)&0xffffffff) << 0)
#define SYS_CTRL_CLR_RSTO_ID_BCK1 (1 << 0)
#define SYS_CTRL_CLR_RSTO_ID_BCK2 (1 << 1)
#define SYS_CTRL_CLR_RSTO_ID_DBG_HOST (1 << 2)
#define SYS_CTRL_CLR_RSTO_ID_GPIO (1 << 3)
#define SYS_CTRL_CLR_RSTO_ID_UART1 (1 << 4)
#define SYS_CTRL_CLR_RSTO_ID_UART2 (1 << 5)
#define SYS_CTRL_CLR_RSTO_ID_UART3 (1 << 6)
#define SYS_CTRL_CLR_RSTO_ID_USBC (1 << 7)
#define SYS_CTRL_CLR_RSTO_ID_WDTIMER0 (1 << 8)
#define SYS_CTRL_CLR_RSTO_ID_WDTIMER1 (1 << 9)
#define SYS_CTRL_CLR_RSTO_ID_WDTIMER2 (1 << 10)
#define SYS_CTRL_CLR_RSTO_ID_SPIFLASH (1 << 11)
#define SYS_CTRL_CLR_RSTO_ID_SPIFLASH_SYS (1 << 12)
#define SYS_CTRL_CLR_RSTO_ID_SPIFLASH1 (1 << 13)
#define SYS_CTRL_CLR_RSTO_ID_SPIFLASH1_SYS (1 << 14)
#define SYS_CTRL_CLR_RSTO_ID_PSRAM_DMC (1 << 15)
#define SYS_CTRL_CLR_RSTO_ID_PSRAM_SYS (1 << 16)
#define SYS_CTRL_CLR_RSTO_ID_PSRAM_PAGESPY (1 << 17)
#define SYS_CTRL_CLR_RSTO_ID_VAD (1 << 18)
#define SYS_CTRL_CLR_RSTO_ID_PIX (1 << 19)
#define SYS_CTRL_CLR_RSTO_ID_SDM_26M (1 << 20)
#define SYS_CTRL_CLR_RSTO_ID_WDTIMER4 (1 << 21)
#define SYS_CTRL_CLR_RSTO_ID_LPS_GSM (1 << 22)
#define SYS_CTRL_CLR_RSTO_ID_LPS_NB (1 << 23)
#define SYS_CTRL_CLR_RSTO_ID_EFUSE (1 << 24)
#define SYS_CTRL_CLR_RSTO_ID_USB_ADP_32K (1 << 25)
#define SYS_CTRL_CLR_RSTO_ID_MIPIDSI (1 << 26)
#define SYS_CTRL_CLR_RSTO_ID_MIPIDSI_PHY (1 << 27)
#define SYS_CTRL_CLR_RSTO_ID_AUD_2AD (1 << 28)
#define SYS_CTRL_CLR_RSTO_ID_USB11_48M (1 << 29)
#define SYS_CTRL_CLR_RSTO_ID_ZSP_UART (1 << 30)
#define SYS_CTRL_CLR_RSTO_ID_CORESIGHT (1 << 31)

// clk_sys_axi_mode
#define SYS_CTRL_MODE_CLK_SYS_AXI(n) (((n)&0x3f) << 0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_IMEM_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_IMEM_MANUAL (1 << 0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_LZMA_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_LZMA_MANUAL (1 << 1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV0_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV0_MANUAL (1 << 2)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV1_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV1_MANUAL (1 << 3)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV2_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV2_MANUAL (1 << 4)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_SDMMC1_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_SDMMC1_MANUAL (1 << 5)

#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_IMEM_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_IMEM_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_LZMA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_LZMA_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV0_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV1_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_REV2_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_SDMMC1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AXI_CLK_ID_SDMMC1_V_MANUAL (1)

// clk_sys_axi_enable
#define SYS_CTRL_ENABLE_CLK_SYS_AXI(n) (((n)&0x7f) << 0)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_IMEM (1 << 0)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_LZMA (1 << 1)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_REV0 (1 << 2)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_REV1 (1 << 3)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_REV2 (1 << 4)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_SDMMC1 (1 << 5)
#define SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_ALWAYS (1 << 6)

// clk_sys_axi_disable
#define SYS_CTRL_DISABLE_CLK_SYS_AXI(n) (((n)&0x7f) << 0)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_IMEM (1 << 0)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_LZMA (1 << 1)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_REV0 (1 << 2)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_REV1 (1 << 3)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_REV2 (1 << 4)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_SDMMC1 (1 << 5)
#define SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_ALWAYS (1 << 6)

// clk_sys_ahb_mode
#define SYS_CTRL_MODE_CLK_SYS_AHB(n) (((n)&0x3f) << 0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_MANUAL (1 << 0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GGE_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GGE_MANUAL (1 << 1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GEA3_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GEA3_MANUAL (1 << 2)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_LZMA_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_LZMA_MANUAL (1 << 3)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_F8_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_F8_MANUAL (1 << 4)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_AXIDMA_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_AXIDMA_MANUAL (1 << 5)

#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GGE_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GGE_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GEA3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GEA3_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_LZMA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_LZMA_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_F8_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_F8_V_MANUAL (1)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_AXIDMA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_AXIDMA_V_MANUAL (1)

// clk_sys_ahb_enable
#define SYS_CTRL_ENABLE_CLK_SYS_AHB(n) (((n)&0x7ff) << 0)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_GOUDA (1 << 0)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_GGE (1 << 1)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_GEA3 (1 << 2)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_LZMA (1 << 3)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_F8 (1 << 4)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_AXIDMA (1 << 5)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_USBC (1 << 6)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_LCD (1 << 7)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_AES (1 << 8)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_USB11 (1 << 9)
#define SYS_CTRL_ENABLE_SYS_AHB_CLK_ID_ALWAYS (1 << 10)

// clk_sys_ahb_disable
#define SYS_CTRL_DISABLE_CLK_SYS_AHB(n) (((n)&0x7ff) << 0)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_GOUDA (1 << 0)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_GGE (1 << 1)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_GEA3 (1 << 2)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_LZMA (1 << 3)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_F8 (1 << 4)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_AXIDMA (1 << 5)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_USBC (1 << 6)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_LCD (1 << 7)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_AES (1 << 8)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_USB11 (1 << 9)
#define SYS_CTRL_DISABLE_SYS_AHB_CLK_ID_ALWAYS (1 << 10)

// clk_ap_apb_mode
#define SYS_CTRL_MODE_CLK_AP_APB(n) (((n)&0x7fffff) << 0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CONF_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CONF_MANUAL (1 << 0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_MOD_CAMERA_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_MOD_CAMERA_MANUAL (1 << 1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C1_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C1_MANUAL (1 << 2)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C3_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C3_MANUAL (1 << 3)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_MANUAL (1 << 4)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH0_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH0_MANUAL (1 << 5)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH1_AUTOMATIC (0 << 6)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH1_MANUAL (1 << 6)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH2_AUTOMATIC (0 << 7)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH2_MANUAL (1 << 7)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH3_AUTOMATIC (0 << 8)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH3_MANUAL (1 << 8)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH4_AUTOMATIC (0 << 9)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH4_MANUAL (1 << 9)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH5_AUTOMATIC (0 << 10)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH5_MANUAL (1 << 10)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH6_AUTOMATIC (0 << 11)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH6_MANUAL (1 << 11)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CHDBG_AUTOMATIC (0 << 12)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CHDBG_MANUAL (1 << 12)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_GOUDA_AUTOMATIC (0 << 13)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_GOUDA_MANUAL (1 << 13)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SDMMC2_AUTOMATIC (0 << 14)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SDMMC2_MANUAL (1 << 14)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI1_AUTOMATIC (0 << 15)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI1_MANUAL (1 << 15)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI2_AUTOMATIC (0 << 16)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI2_MANUAL (1 << 16)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID1_AUTOMATIC (0 << 17)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID1_MANUAL (1 << 17)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID2_AUTOMATIC (0 << 18)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID2_MANUAL (1 << 18)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI1_AUTOMATIC (0 << 19)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI1_MANUAL (1 << 19)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI2_AUTOMATIC (0 << 20)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI2_MANUAL (1 << 20)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CAMERA_AUTOMATIC (0 << 21)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CAMERA_MANUAL (1 << 21)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_PAGESPY_AUTOMATIC (0 << 22)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_PAGESPY_MANUAL (1 << 22)

#define SYS_CTRL_MODE_AP_APB_CLK_ID_CONF_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CONF_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_MOD_CAMERA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_MOD_CAMERA_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C1_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_I2C3_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH0_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH1_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH2_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH3_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH4_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH4_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH5_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH5_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH6_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CH6_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CHDBG_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_IFC_CHDBG_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_GOUDA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_GOUDA_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SDMMC2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SDMMC2_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI1_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SPI2_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID1_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCID2_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI1_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_SCI2_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CAMERA_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_CAMERA_V_MANUAL (1)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_PAGESPY_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AP_APB_CLK_ID_PAGESPY_V_MANUAL (1)

// clk_ap_apb_enable
#define SYS_CTRL_ENABLE_CLK_AP_APB(n) (((n)&0x7fffffff) << 0)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_MOD_CAMERA (1 << 1)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_I2C1 (1 << 2)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_I2C3 (1 << 3)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC (1 << 4)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH0 (1 << 5)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH1 (1 << 6)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH2 (1 << 7)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH3 (1 << 8)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH4 (1 << 9)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH5 (1 << 10)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CH6 (1 << 11)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_IFC_CHDBG (1 << 12)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_GOUDA (1 << 13)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SDMMC2 (1 << 14)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SPI1 (1 << 15)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SPI2 (1 << 16)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SCID1 (1 << 17)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SCID2 (1 << 18)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SCI1 (1 << 19)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_SCI2 (1 << 20)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_CAMERA (1 << 21)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_PAGESPY (1 << 22)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_LZMA (1 << 23)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_REV2 (1 << 24)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_TIMER1 (1 << 25)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_TIMER2 (1 << 26)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_TIMER4 (1 << 27)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_LCD (1 << 28)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_CQIRQ (1 << 29)
#define SYS_CTRL_ENABLE_AP_APB_CLK_ID_ALWAYS (1 << 30)

// clk_ap_apb_disable
#define SYS_CTRL_DISABLE_CLK_AP_APB(n) (((n)&0x7fffffff) << 0)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_MOD_CAMERA (1 << 1)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_I2C1 (1 << 2)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_I2C3 (1 << 3)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC (1 << 4)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH0 (1 << 5)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH1 (1 << 6)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH2 (1 << 7)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH3 (1 << 8)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH4 (1 << 9)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH5 (1 << 10)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CH6 (1 << 11)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_IFC_CHDBG (1 << 12)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_GOUDA (1 << 13)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SDMMC2 (1 << 14)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SPI1 (1 << 15)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SPI2 (1 << 16)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SCID1 (1 << 17)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SCID2 (1 << 18)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SCI1 (1 << 19)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_SCI2 (1 << 20)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_CAMERA (1 << 21)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_PAGESPY (1 << 22)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_LZMA (1 << 23)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_REV2 (1 << 24)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_TIMER1 (1 << 25)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_TIMER2 (1 << 26)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_TIMER4 (1 << 27)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_LCD (1 << 28)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_CQIRQ (1 << 29)
#define SYS_CTRL_DISABLE_AP_APB_CLK_ID_ALWAYS (1 << 30)

// clk_aif_apb_mode
#define SYS_CTRL_MODE_CLK_AIF_APB(n) (((n)&0xfff) << 0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_CONF_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_CONF_MANUAL (1 << 0)
#define SYS_CTRL_MODE_AIF1_APB_CLK_ID_AIF_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_AIF1_APB_CLK_ID_AIF_MANUAL (1 << 1)
#define SYS_CTRL_MODE_AIF2_APB_CLK_ID_AIF_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_AIF2_APB_CLK_ID_AIF_MANUAL (1 << 2)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_MANUAL (1 << 3)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH0_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH0_MANUAL (1 << 4)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH1_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH1_MANUAL (1 << 5)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH2_AUTOMATIC (0 << 6)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH2_MANUAL (1 << 6)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH3_AUTOMATIC (0 << 7)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH3_MANUAL (1 << 7)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV0_AUTOMATIC (0 << 8)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV0_MANUAL (1 << 8)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV1_AUTOMATIC (0 << 9)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV1_MANUAL (1 << 9)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV2_AUTOMATIC (0 << 10)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV2_MANUAL (1 << 10)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_AUD_2AD_AUTOMATIC (0 << 11)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_AUD_2AD_MANUAL (1 << 11)

#define SYS_CTRL_MODE_AIF_APB_CLK_ID_CONF_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_CONF_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF1_APB_CLK_ID_AIF_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF1_APB_CLK_ID_AIF_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF2_APB_CLK_ID_AIF_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF2_APB_CLK_ID_AIF_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH0_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH1_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH2_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_IFC_CH3_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV0_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV1_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_REV2_V_MANUAL (1)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_AUD_2AD_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AIF_APB_CLK_ID_AUD_2AD_V_MANUAL (1)

// clk_aif_apb_enable
#define SYS_CTRL_ENABLE_CLK_AIF_APB(n) (((n)&0x1fff) << 0)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_ENABLE_AIF1_APB_CLK_ID_AIF (1 << 1)
#define SYS_CTRL_ENABLE_AIF2_APB_CLK_ID_AIF (1 << 2)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_IFC (1 << 3)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_IFC_CH0 (1 << 4)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_IFC_CH1 (1 << 5)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_IFC_CH2 (1 << 6)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_IFC_CH3 (1 << 7)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_REV0 (1 << 8)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_REV1 (1 << 9)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_REV2 (1 << 10)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_AUD_2AD (1 << 11)
#define SYS_CTRL_ENABLE_AIF_APB_CLK_ID_ALWAYS (1 << 12)

// clk_aif_apb_disable
#define SYS_CTRL_DISABLE_CLK_AIF_APB(n) (((n)&0x1fff) << 0)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_DISABLE_AIF1_APB_CLK_ID_AIF (1 << 1)
#define SYS_CTRL_DISABLE_AIF2_APB_CLK_ID_AIF (1 << 2)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_IFC (1 << 3)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_IFC_CH0 (1 << 4)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_IFC_CH1 (1 << 5)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_IFC_CH2 (1 << 6)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_IFC_CH3 (1 << 7)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_REV0 (1 << 8)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_REV1 (1 << 9)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_REV2 (1 << 10)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_AUD_2AD (1 << 11)
#define SYS_CTRL_DISABLE_AIF_APB_CLK_ID_ALWAYS (1 << 12)

// clk_aon_ahb_mode
#define SYS_CTRL_MODE_CLK_AON_AHB(n) (((n)&0x1f) << 0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_SYSCTRL_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_SYSCTRL_MANUAL (1 << 0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV0_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV0_MANUAL (1 << 1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV1_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV1_MANUAL (1 << 2)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV2_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV2_MANUAL (1 << 3)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_CSSYS_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_CSSYS_MANUAL (1 << 4)

#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_SYSCTRL_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_SYSCTRL_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV0_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV1_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_BB_REV2_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_CSSYS_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_AHB_CLK_ID_CSSYS_V_MANUAL (1)

// clk_aon_ahb_enable
#define SYS_CTRL_ENABLE_CLK_AON_AHB(n) (((n)&0x3f) << 0)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_BB_SYSCTRL (1 << 0)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_BB_REV0 (1 << 1)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_BB_REV1 (1 << 2)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_BB_REV2 (1 << 3)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_CSSYS (1 << 4)
#define SYS_CTRL_ENABLE_AON_AHB_CLK_ID_ALWAYS (1 << 5)

// clk_aon_ahb_disable
#define SYS_CTRL_DISABLE_CLK_AON_AHB(n) (((n)&0x3f) << 0)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_BB_SYSCTRL (1 << 0)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_BB_REV0 (1 << 1)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_BB_REV1 (1 << 2)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_BB_REV2 (1 << 3)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_CSSYS (1 << 4)
#define SYS_CTRL_DISABLE_AON_AHB_CLK_ID_ALWAYS (1 << 5)

// clk_aon_apb_mode
#define SYS_CTRL_MODE_CLK_AON_APB(n) (((n)&0xfffff) << 0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_CONF_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_CONF_MANUAL (1 << 0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_HOST_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_HOST_MANUAL (1 << 1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_UART_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_UART_MANUAL (1 << 2)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_MANUAL (1 << 3)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH0_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH0_MANUAL (1 << 4)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH1_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH1_MANUAL (1 << 5)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH2_AUTOMATIC (0 << 6)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH2_MANUAL (1 << 6)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH3_AUTOMATIC (0 << 7)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH3_MANUAL (1 << 7)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CHDBG_AUTOMATIC (0 << 8)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CHDBG_MANUAL (1 << 8)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV0_AUTOMATIC (0 << 9)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV0_MANUAL (1 << 9)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV1_AUTOMATIC (0 << 10)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV1_MANUAL (1 << 10)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_PWM_AUTOMATIC (0 << 11)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_PWM_MANUAL (1 << 11)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_I2C2_AUTOMATIC (0 << 12)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_I2C2_MANUAL (1 << 12)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_ANA_REG_AUTOMATIC (0 << 13)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_ANA_REG_MANUAL (1 << 13)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_SPINLOCK_AUTOMATIC (0 << 14)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_SPINLOCK_MANUAL (1 << 14)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_GSM_AUTOMATIC (0 << 15)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_GSM_MANUAL (1 << 15)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_NB_AUTOMATIC (0 << 16)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_NB_MANUAL (1 << 16)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_EFUSE_AUTOMATIC (0 << 17)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_EFUSE_MANUAL (1 << 17)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV2_AUTOMATIC (0 << 18)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV2_MANUAL (1 << 18)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_VAD_AUTOMATIC (0 << 19)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_VAD_MANUAL (1 << 19)

#define SYS_CTRL_MODE_AON_APB_CLK_ID_CONF_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_CONF_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_HOST_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_HOST_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_UART_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_DBG_UART_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH0_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH1_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH2_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CH3_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CHDBG_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_IFC_CHDBG_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV0_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV1_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_PWM_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_PWM_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_I2C2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_I2C2_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_ANA_REG_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_ANA_REG_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_SPINLOCK_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_SPINLOCK_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_GSM_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_GSM_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_NB_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_LPS_NB_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_EFUSE_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_EFUSE_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_REV2_V_MANUAL (1)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_VAD_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_AON_APB_CLK_ID_VAD_V_MANUAL (1)

// clk_aon_apb_enable
#define SYS_CTRL_ENABLE_CLK_AON_APB(n) (((n)&0x1fffffff) << 0)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_DBG_HOST (1 << 1)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_DBG_UART (1 << 2)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC (1 << 3)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC_CH0 (1 << 4)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC_CH1 (1 << 5)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC_CH2 (1 << 6)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC_CH3 (1 << 7)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_IFC_CHDBG (1 << 8)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_REV0 (1 << 9)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_REV1 (1 << 10)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_PWM (1 << 11)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_I2C2 (1 << 12)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_ANA_REG (1 << 13)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_SPINLOCK (1 << 14)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_LPS_GSM (1 << 15)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_LPS_NB (1 << 16)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_EFUSE (1 << 17)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_REV2 (1 << 18)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_VAD (1 << 19)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_REV3 (1 << 20)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_REV4 (1 << 21)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_GPIO (1 << 22)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_TIMER3 (1 << 23)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_KEYPAD (1 << 24)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_AHB2APB_ADI (1 << 25)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_ADI (1 << 26)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_LVDS (1 << 27)
#define SYS_CTRL_ENABLE_AON_APB_CLK_ID_ALWAYS (1 << 28)

// clk_aon_apb_disable
#define SYS_CTRL_DISABLE_CLK_AON_APB(n) (((n)&0x1fffffff) << 0)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_CONF (1 << 0)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_DBG_HOST (1 << 1)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_DBG_UART (1 << 2)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC (1 << 3)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC_CH0 (1 << 4)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC_CH1 (1 << 5)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC_CH2 (1 << 6)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC_CH3 (1 << 7)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_IFC_CHDBG (1 << 8)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_REV0 (1 << 9)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_REV1 (1 << 10)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_PWM (1 << 11)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_I2C2 (1 << 12)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_ANA_REG (1 << 13)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_SPINLOCK (1 << 14)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_LPS_GSM (1 << 15)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_LPS_NB (1 << 16)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_EFUSE (1 << 17)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_REV2 (1 << 18)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_VAD (1 << 19)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_REV3 (1 << 20)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_REV4 (1 << 21)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_GPIO (1 << 22)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_TIMER3 (1 << 23)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_KEYPAD (1 << 24)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_AHB2APB_ADI (1 << 25)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_ADI (1 << 26)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_LVDS (1 << 27)
#define SYS_CTRL_DISABLE_AON_APB_CLK_ID_ALWAYS (1 << 28)

// clk_rf_ahb_mode
#define SYS_CTRL_MODE_CLK_RF_AHB_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_CLK_RF_AHB_MANUAL (1 << 0)

#define SYS_CTRL_MODE_CLK_RF_AHB_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_CLK_RF_AHB_V_MANUAL (1)

// clk_rf_ahb_enable
#define SYS_CTRL_ENABLE_CLK_RF_AHB(n) (((n)&0x3) << 0)
#define SYS_CTRL_ENABLE_RF_AHB_CLK_ID_REV0 (1 << 0)
#define SYS_CTRL_ENABLE_RF_AHB_CLK_ID_ALWAYS (1 << 1)

// clk_rf_ahb_disable
#define SYS_CTRL_DISABLE_CLK_RF_AHB(n) (((n)&0x3) << 0)
#define SYS_CTRL_DISABLE_RF_AHB_CLK_ID_REV0 (1 << 0)
#define SYS_CTRL_DISABLE_RF_AHB_CLK_ID_ALWAYS (1 << 1)

// clk_rf_apb_mode
#define SYS_CTRL_MODE_CLK_RF_APB_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_CLK_RF_APB_MANUAL (1 << 0)

#define SYS_CTRL_MODE_CLK_RF_APB_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_CLK_RF_APB_V_MANUAL (1)

// clk_rf_apb_enable
#define SYS_CTRL_ENABLE_CLK_RF_APB(n) (((n)&0x3) << 0)
#define SYS_CTRL_ENABLE_RF_APB_CLK_ID_REV0 (1 << 0)
#define SYS_CTRL_ENABLE_RF_APB_CLK_ID_ALWAYS (1 << 1)

// clk_rf_apb_disable
#define SYS_CTRL_DISABLE_CLK_RF_APB(n) (((n)&0x3) << 0)
#define SYS_CTRL_DISABLE_RF_APB_CLK_ID_REV0 (1 << 0)
#define SYS_CTRL_DISABLE_RF_APB_CLK_ID_ALWAYS (1 << 1)

// clk_others_mode
#define SYS_CTRL_MODE_CLK_OTHERS(n) (((n)&0xff) << 0)
#define SYS_CTRL_MODE_OC_ID_HOST_UART_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_OC_ID_HOST_UART_MANUAL (1 << 0)
#define SYS_CTRL_MODE_OC_ID_BCK1_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_OC_ID_BCK1_MANUAL (1 << 1)
#define SYS_CTRL_MODE_OC_ID_BCK2_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_OC_ID_BCK2_MANUAL (1 << 2)
#define SYS_CTRL_MODE_OC_ID_UART1_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_OC_ID_UART1_MANUAL (1 << 3)
#define SYS_CTRL_MODE_OC_ID_UART2_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_OC_ID_UART2_MANUAL (1 << 4)
#define SYS_CTRL_MODE_OC_ID_UART3_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_OC_ID_UART3_MANUAL (1 << 5)
#define SYS_CTRL_MODE_OC_ID_AP_A5_AUTOMATIC (0 << 6)
#define SYS_CTRL_MODE_OC_ID_AP_A5_MANUAL (1 << 6)
#define SYS_CTRL_MODE_OC_ID_CP_A5_AUTOMATIC (0 << 7)
#define SYS_CTRL_MODE_OC_ID_CP_A5_MANUAL (1 << 7)

#define SYS_CTRL_MODE_OC_ID_HOST_UART_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_HOST_UART_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_BCK1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_BCK1_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_BCK2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_BCK2_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_UART1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_UART1_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_UART2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_UART2_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_UART3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_UART3_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_AP_A5_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_AP_A5_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_CP_A5_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_CP_A5_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_PIX_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_PIX_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_ISP_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_ISP_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_SPICAM_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_SPICAM_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_CAM_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_CAM_V_MANUAL (1)

// clk_others_enable
#define SYS_CTRL_ENABLE_CLK_OTHERS(n) (((n)&0x1ffffff) << 0)
#define SYS_CTRL_ENABLE_OC_ID_HOST_UART (1 << 0)
#define SYS_CTRL_ENABLE_OC_ID_BCK1 (1 << 1)
#define SYS_CTRL_ENABLE_OC_ID_BCK2 (1 << 2)
#define SYS_CTRL_ENABLE_OC_ID_UART1 (1 << 3)
#define SYS_CTRL_ENABLE_OC_ID_UART2 (1 << 4)
#define SYS_CTRL_ENABLE_OC_ID_UART3 (1 << 5)
#define SYS_CTRL_ENABLE_OC_ID_AP_A5 (1 << 6)
#define SYS_CTRL_ENABLE_OC_ID_CP_A5 (1 << 7)
#define SYS_CTRL_ENABLE_OC_ID_GPIO (1 << 8)
#define SYS_CTRL_ENABLE_OC_ID_USBPHY (1 << 9)
#define SYS_CTRL_ENABLE_OC_ID_PIX (1 << 10)
#define SYS_CTRL_ENABLE_OC_ID_CLK_OUT (1 << 11)
#define SYS_CTRL_ENABLE_OC_ID_ISP (1 << 12)
#define SYS_CTRL_ENABLE_OC_ID_SYS_SPIFLASH (1 << 13)
#define SYS_CTRL_ENABLE_OC_ID_SYS_SPIFLASH_ALWAYS (1 << 14)
#define SYS_CTRL_ENABLE_OC_ID_SYS_SPIFLASH1 (1 << 15)
#define SYS_CTRL_ENABLE_OC_ID_SYS_SPIFLASH1_ALWAYS (1 << 16)
#define SYS_CTRL_ENABLE_OC_ID_SPIFLASH (1 << 17)
#define SYS_CTRL_ENABLE_OC_ID_SPIFLASH1 (1 << 18)
#define SYS_CTRL_ENABLE_OC_ID_SPICAM (1 << 19)
#define SYS_CTRL_ENABLE_OC_ID_CAM (1 << 20)
#define SYS_CTRL_ENABLE_OC_ID_PSRAM_CONF (1 << 21)
#define SYS_CTRL_ENABLE_OC_ID_PSRAM_DMC (1 << 22)
#define SYS_CTRL_ENABLE_OC_ID_PSRAM_PAGESPY (1 << 23)
#define SYS_CTRL_ENABLE_OC_ID_PSRAM_ALWAYS (1 << 24)

// clk_others_disable
#define SYS_CTRL_DISABLE_CLK_OTHERS(n) (((n)&0x1ffffff) << 0)
#define SYS_CTRL_DISABLE_OC_ID_HOST_UART (1 << 0)
#define SYS_CTRL_DISABLE_OC_ID_BCK1 (1 << 1)
#define SYS_CTRL_DISABLE_OC_ID_BCK2 (1 << 2)
#define SYS_CTRL_DISABLE_OC_ID_UART1 (1 << 3)
#define SYS_CTRL_DISABLE_OC_ID_UART2 (1 << 4)
#define SYS_CTRL_DISABLE_OC_ID_UART3 (1 << 5)
#define SYS_CTRL_DISABLE_OC_ID_AP_A5 (1 << 6)
#define SYS_CTRL_DISABLE_OC_ID_CP_A5 (1 << 7)
#define SYS_CTRL_DISABLE_OC_ID_GPIO (1 << 8)
#define SYS_CTRL_DISABLE_OC_ID_USBPHY (1 << 9)
#define SYS_CTRL_DISABLE_OC_ID_PIX (1 << 10)
#define SYS_CTRL_DISABLE_OC_ID_CLK_OUT (1 << 11)
#define SYS_CTRL_DISABLE_OC_ID_ISP (1 << 12)
#define SYS_CTRL_DISABLE_OC_ID_SYS_SPIFLASH (1 << 13)
#define SYS_CTRL_DISABLE_OC_ID_SYS_SPIFLASH_ALWAYS (1 << 14)
#define SYS_CTRL_DISABLE_OC_ID_SYS_SPIFLASH1 (1 << 15)
#define SYS_CTRL_DISABLE_OC_ID_SYS_SPIFLASH1_ALWAYS (1 << 16)
#define SYS_CTRL_DISABLE_OC_ID_SPIFLASH (1 << 17)
#define SYS_CTRL_DISABLE_OC_ID_SPIFLASH1 (1 << 18)
#define SYS_CTRL_DISABLE_OC_ID_SPICAM (1 << 19)
#define SYS_CTRL_DISABLE_OC_ID_CAM (1 << 20)
#define SYS_CTRL_DISABLE_OC_ID_PSRAM_CONF (1 << 21)
#define SYS_CTRL_DISABLE_OC_ID_PSRAM_DMC (1 << 22)
#define SYS_CTRL_DISABLE_OC_ID_PSRAM_PAGESPY (1 << 23)
#define SYS_CTRL_DISABLE_OC_ID_PSRAM_ALWAYS (1 << 24)

// clk_others_1_mode
#define SYS_CTRL_MODE_CLK_OTHERS_1(n) (((n)&0xffff) << 0)
#define SYS_CTRL_MODE_OC_ID_GGE_CLK32K_AUTOMATIC (0 << 0)
#define SYS_CTRL_MODE_OC_ID_GGE_CLK32K_MANUAL (1 << 0)
#define SYS_CTRL_MODE_OC_ID_GGE_26M_AUTOMATIC (0 << 1)
#define SYS_CTRL_MODE_OC_ID_GGE_26M_MANUAL (1 << 1)
#define SYS_CTRL_MODE_OC_ID_NB_61P44M_AUTOMATIC (0 << 2)
#define SYS_CTRL_MODE_OC_ID_NB_61P44M_MANUAL (1 << 2)
#define SYS_CTRL_MODE_OC_ID_BT_FM_CLK32K_AUTOMATIC (0 << 3)
#define SYS_CTRL_MODE_OC_ID_BT_FM_CLK32K_MANUAL (1 << 3)
#define SYS_CTRL_MODE_OC_ID_BT_FM_26M_AUTOMATIC (0 << 4)
#define SYS_CTRL_MODE_OC_ID_BT_FM_26M_MANUAL (1 << 4)
#define SYS_CTRL_MODE_OC_ID_PMIC_CLK32K_AUTOMATIC (0 << 5)
#define SYS_CTRL_MODE_OC_ID_PMIC_CLK32K_MANUAL (1 << 5)
#define SYS_CTRL_MODE_OC_ID_PMIC_26M_AUTOMATIC (0 << 6)
#define SYS_CTRL_MODE_OC_ID_PMIC_26M_MANUAL (1 << 6)
#define SYS_CTRL_MODE_OC_ID_CP_CLK32K_AUTOMATIC (0 << 7)
#define SYS_CTRL_MODE_OC_ID_CP_CLK32K_MANUAL (1 << 7)
#define SYS_CTRL_MODE_OC_ID_CP_26M_AUTOMATIC (0 << 8)
#define SYS_CTRL_MODE_OC_ID_CP_26M_MANUAL (1 << 8)
#define SYS_CTRL_MODE_OC_ID_ZSP_UART_AUTOMATIC (0 << 9)
#define SYS_CTRL_MODE_OC_ID_ZSP_UART_MANUAL (1 << 9)
#define SYS_CTRL_MODE_OC_ID_CP_BBLTE_AUTOMATIC (0 << 10)
#define SYS_CTRL_MODE_OC_ID_CP_BBLTE_MANUAL (1 << 10)
#define SYS_CTRL_MODE_OC_ID_494M_LTE_AUTOMATIC (0 << 11)
#define SYS_CTRL_MODE_OC_ID_494M_LTE_MANUAL (1 << 11)
#define SYS_CTRL_MODE_OC_ID_REV0_AUTOMATIC (0 << 12)
#define SYS_CTRL_MODE_OC_ID_REV0_MANUAL (1 << 12)
#define SYS_CTRL_MODE_OC_ID_REV1_AUTOMATIC (0 << 13)
#define SYS_CTRL_MODE_OC_ID_REV1_MANUAL (1 << 13)
#define SYS_CTRL_MODE_OC_ID_REV2_AUTOMATIC (0 << 14)
#define SYS_CTRL_MODE_OC_ID_REV2_MANUAL (1 << 14)
#define SYS_CTRL_MODE_OC_ID_REV3_AUTOMATIC (0 << 15)
#define SYS_CTRL_MODE_OC_ID_REV3_MANUAL (1 << 15)

#define SYS_CTRL_MODE_OC_ID_GGE_CLK32K_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_GGE_CLK32K_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_GGE_26M_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_GGE_26M_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_NB_61P44M_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_NB_61P44M_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_BT_FM_CLK32K_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_BT_FM_CLK32K_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_BT_FM_26M_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_BT_FM_26M_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_PMIC_CLK32K_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_PMIC_CLK32K_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_PMIC_26M_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_PMIC_26M_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_CP_CLK32K_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_CP_CLK32K_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_CP_26M_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_CP_26M_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_ZSP_UART_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_ZSP_UART_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_CP_BBLTE_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_CP_BBLTE_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_494M_LTE_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_494M_LTE_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_REV0_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_REV0_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_REV1_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_REV1_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_REV2_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_REV2_V_MANUAL (1)
#define SYS_CTRL_MODE_OC_ID_REV3_V_AUTOMATIC (0)
#define SYS_CTRL_MODE_OC_ID_REV3_V_MANUAL (1)

// clk_others_1_enable
#define SYS_CTRL_ENABLE_CLK_OTHERS_1(n) (((n)&0x7fffffff) << 0)
#define SYS_CTRL_ENABLE_OC_ID_GGE_CLK32K (1 << 0)
#define SYS_CTRL_ENABLE_OC_ID_GGE_26M (1 << 1)
#define SYS_CTRL_ENABLE_OC_ID_NB_61P44M (1 << 2)
#define SYS_CTRL_ENABLE_OC_ID_BT_FM_CLK32K (1 << 3)
#define SYS_CTRL_ENABLE_OC_ID_BT_FM_26M (1 << 4)
#define SYS_CTRL_ENABLE_OC_ID_PMIC_CLK32K (1 << 5)
#define SYS_CTRL_ENABLE_OC_ID_PMIC_26M (1 << 6)
#define SYS_CTRL_ENABLE_OC_ID_CP_CLK32K (1 << 7)
#define SYS_CTRL_ENABLE_OC_ID_CP_26M (1 << 8)
#define SYS_CTRL_ENABLE_OC_ID_ZSP_UART (1 << 9)
#define SYS_CTRL_ENABLE_OC_ID_CP_BBLTE (1 << 10)
#define SYS_CTRL_ENABLE_OC_ID_494M_LTE (1 << 11)
#define SYS_CTRL_ENABLE_OC_ID_REV0 (1 << 12)
#define SYS_CTRL_ENABLE_OC_ID_REV1 (1 << 13)
#define SYS_CTRL_ENABLE_OC_ID_REV2 (1 << 14)
#define SYS_CTRL_ENABLE_OC_ID_REV3 (1 << 15)
#define SYS_CTRL_ENABLE_OC_ID_SDM_26M (1 << 16)
#define SYS_CTRL_ENABLE_OC_ID_LPS_GSM (1 << 17)
#define SYS_CTRL_ENABLE_OC_ID_LPS_NB (1 << 18)
#define SYS_CTRL_ENABLE_OC_ID_EFUSE_26M (1 << 19)
#define SYS_CTRL_ENABLE_OC_ID_USB_ADP_32K (1 << 20)
#define SYS_CTRL_ENABLE_OC_ID_USB_UTMI_48M (1 << 21)
#define SYS_CTRL_ENABLE_OC_ID_AP_26M (1 << 22)
#define SYS_CTRL_ENABLE_OC_ID_AP_32K (1 << 23)
#define SYS_CTRL_ENABLE_OC_ID_MIPIDSI (1 << 24)
#define SYS_CTRL_ENABLE_OC_ID_AHB_BTFM (1 << 25)
#define SYS_CTRL_ENABLE_OC_ID_VAD (1 << 26)
#define SYS_CTRL_ENABLE_OC_ID_USB11_48M (1 << 27)
#define SYS_CTRL_ENABLE_OC_ID_TRNG_CLKEN (1 << 28)
#define SYS_CTRL_ENABLE_OC_ID_CORESIGHT (1 << 29)
#define SYS_CTRL_ENABLE_OC_ID_ADI (1 << 30)

// clk_others_1_disable
#define SYS_CTRL_DISABLE_CLK_OTHERS_1(n) (((n)&0x7fffffff) << 0)
#define SYS_CTRL_DISABLE_OC_ID_GGE_CLK32K (1 << 0)
#define SYS_CTRL_DISABLE_OC_ID_GGE_26M (1 << 1)
#define SYS_CTRL_DISABLE_OC_ID_NB_61P44M (1 << 2)
#define SYS_CTRL_DISABLE_OC_ID_BT_FM_CLK32K (1 << 3)
#define SYS_CTRL_DISABLE_OC_ID_BT_FM_26M (1 << 4)
#define SYS_CTRL_DISABLE_OC_ID_PMIC_CLK32K (1 << 5)
#define SYS_CTRL_DISABLE_OC_ID_PMIC_26M (1 << 6)
#define SYS_CTRL_DISABLE_OC_ID_CP_CLK32K (1 << 7)
#define SYS_CTRL_DISABLE_OC_ID_CP_26M (1 << 8)
#define SYS_CTRL_DISABLE_OC_ID_ZSP_UART (1 << 9)
#define SYS_CTRL_DISABLE_OC_ID_CP_BBLTE (1 << 10)
#define SYS_CTRL_DISABLE_OC_ID_494M_LTE (1 << 11)
#define SYS_CTRL_DISABLE_OC_ID_REV0 (1 << 12)
#define SYS_CTRL_DISABLE_OC_ID_REV1 (1 << 13)
#define SYS_CTRL_DISABLE_OC_ID_REV2 (1 << 14)
#define SYS_CTRL_DISABLE_OC_ID_REV3 (1 << 15)
#define SYS_CTRL_DISABLE_OC_ID_SDM_26M (1 << 16)
#define SYS_CTRL_DISABLE_OC_ID_LPS_GSM (1 << 17)
#define SYS_CTRL_DISABLE_OC_ID_LPS_NB (1 << 18)
#define SYS_CTRL_DISABLE_OC_ID_EFUSE_26M (1 << 19)
#define SYS_CTRL_DISABLE_OC_ID_USB_ADP_32K (1 << 20)
#define SYS_CTRL_DISABLE_OC_ID_USB_UTMI_48M (1 << 21)
#define SYS_CTRL_DISABLE_OC_ID_AP_26M (1 << 22)
#define SYS_CTRL_DISABLE_OC_ID_AP_32K (1 << 23)
#define SYS_CTRL_DISABLE_OC_ID_MIPIDSI (1 << 24)
#define SYS_CTRL_DISABLE_OC_ID_AHB_BTFM (1 << 25)
#define SYS_CTRL_DISABLE_OC_ID_VAD (1 << 26)
#define SYS_CTRL_DISABLE_OC_ID_USB11_48M (1 << 27)
#define SYS_CTRL_DISABLE_OC_ID_TRNG_CLKEN (1 << 28)
#define SYS_CTRL_DISABLE_OC_ID_CORESIGHT (1 << 29)
#define SYS_CTRL_DISABLE_OC_ID_ADI (1 << 30)

// pll_ctrl
#define SYS_CTRL_PLL_ENABLE_POWER_DOWN (0 << 0)
#define SYS_CTRL_PLL_ENABLE_ENABLE (1 << 0)
#define SYS_CTRL_PLL_LOCK_RESET_RESET (0 << 4)
#define SYS_CTRL_PLL_LOCK_RESET_NO_RESET (1 << 4)
#define SYS_CTRL_PLL_BYPASS_LOCK_PASS (0 << 8)
#define SYS_CTRL_PLL_BYPASS_LOCK_BYPASS (1 << 8)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE (1 << 12)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE (0 << 12)

#define SYS_CTRL_PLL_ENABLE_V_POWER_DOWN (0)
#define SYS_CTRL_PLL_ENABLE_V_ENABLE (1)
#define SYS_CTRL_PLL_LOCK_RESET_V_RESET (0)
#define SYS_CTRL_PLL_LOCK_RESET_V_NO_RESET (1)
#define SYS_CTRL_PLL_BYPASS_LOCK_V_PASS (0)
#define SYS_CTRL_PLL_BYPASS_LOCK_V_BYPASS (1)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_V_ENABLE (1)
#define SYS_CTRL_PLL_CLK_FAST_ENABLE_V_DISABLE (0)

// sel_clock
#define SYS_CTRL_SLOW_SEL_RF_OSCILLATOR (1 << 0)
#define SYS_CTRL_SLOW_SEL_RF_RF (0 << 0)
#define SYS_CTRL_SYS_SEL_FAST_SLOW (1 << 1)
#define SYS_CTRL_SYS_SEL_FAST_FAST (0 << 1)
#define SYS_CTRL_OSC_32K_26M_DIV32K_SEL (1 << 2)
#define SYS_CTRL_PLL_DISABLE_LPS_DISABLE (1 << 3)
#define SYS_CTRL_PLL_DISABLE_LPS_ENABLE (0 << 3)
#define SYS_CTRL_RF_DETECTED_OK (1 << 4)
#define SYS_CTRL_RF_DETECTED_NO (0 << 4)
#define SYS_CTRL_RF_DETECT_BYPASS (1 << 5)
#define SYS_CTRL_RF_DETECT_RESET (1 << 6)
#define SYS_CTRL_RF_SELECTED_L (1 << 7)
#define SYS_CTRL_PLL_LOCKED_LOCKED (1 << 8)
#define SYS_CTRL_PLL_LOCKED_NOT_LOCKED (0 << 8)
#define SYS_CTRL_FAST_SELECTED_L (1 << 9)
#define SYS_CTRL_SOFT_SEL_SPIFLASH (1 << 10)
#define SYS_CTRL_SOFT_SEL_MEM_BRIDGE (1 << 11)
#define SYS_CTRL_BBLTE_CLK_PLL_SEL (1 << 12)
#define SYS_CTRL_CAMERA_CLK_PLL_SEL (1 << 13)
#define SYS_CTRL_USB_PLL_LOCKED_H (1 << 14)
#define SYS_CTRL_BB26M_SEL (1 << 15)
#define SYS_CTRL_SOFT_SEL_SPIFLASH1 (1 << 16)
#define SYS_CTRL_APLL_LOCKED_H (1 << 17)
#define SYS_CTRL_MEMPLL_LOCKED_H (1 << 18)
#define SYS_CTRL_AUDIOPLL_LOCKED_H (1 << 19)
#define SYS_CTRL_BBPLL2_LOCKED_H (1 << 20)
#define SYS_CTRL_BBPLL1_LOCKED_H (1 << 21)
#define SYS_CTRL_USBPLL_LOCKED_H (1 << 22)

#define SYS_CTRL_SLOW_SEL_RF_V_OSCILLATOR (1)
#define SYS_CTRL_SLOW_SEL_RF_V_RF (0)
#define SYS_CTRL_SYS_SEL_FAST_V_SLOW (1)
#define SYS_CTRL_SYS_SEL_FAST_V_FAST (0)
#define SYS_CTRL_PLL_DISABLE_LPS_V_DISABLE (1)
#define SYS_CTRL_PLL_DISABLE_LPS_V_ENABLE (0)
#define SYS_CTRL_RF_DETECTED_V_OK (1)
#define SYS_CTRL_RF_DETECTED_V_NO (0)
#define SYS_CTRL_PLL_LOCKED_V_LOCKED (1)
#define SYS_CTRL_PLL_LOCKED_V_NOT_LOCKED (0)

// cfg_clk_out
#define SYS_CTRL_CLKOUT_DIVIDER(n) (((n)&0x1f) << 0)
#define SYS_CTRL_CLKOUT_DBG_SEL (1 << 5)
#define SYS_CTRL_CLKOUT_ENABLE (1 << 6)
#define SYS_CTRL_CLKOUT_UPDATE (1 << 7)

// cfg_clk_audiobck1_div
#define SYS_CTRL_AUDIOBCK1_DIVIDER(n) (((n)&0x7ff) << 0)
#define SYS_CTRL_AUDIOBCK1_UPDATE (1 << 11)

// cfg_clk_audiobck2_div
#define SYS_CTRL_AUDIOBCK2_DIVIDER(n) (((n)&0x7ff) << 0)
#define SYS_CTRL_AUDIOBCK2_UPDATE (1 << 11)

// cfg_clk_uart
#define SYS_CTRL_UART_DIVIDER(n) (((n)&0xffffff) << 0)
#define SYS_CTRL_UART_DIVIDER_UPDATE (1 << 24)
#define SYS_CTRL_UART_SEL_PLL (1 << 25)

// cfg_clk_pwm
#define SYS_CTRL_PWM_DIVIDER(n) (((n)&0xff) << 0)

// cfg_clk_dbg_div
#define SYS_CTRL_CLK_DBG_DIVIDER(n) (((n)&0x7ff) << 0)

// cfg_clk_camera_out
#define SYS_CTRL_CLK_CAMERA_OUT_EN_DISABLE (0 << 0)
#define SYS_CTRL_CLK_CAMERA_OUT_EN_ENABLE (1 << 0)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_26_MHZ (0 << 1)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_156_MHZ (1 << 1)
#define SYS_CTRL_CLK_CAMERA_OUT_DIV(n) (((n)&0x7ff) << 2)
#define SYS_CTRL_CLK_CAMERA_DIV_UPDATE (1 << 13)
#define SYS_CTRL_CLK_SPI_CAM_EN_DISABLE (0 << 14)
#define SYS_CTRL_CLK_SPI_CAM_EN_ENABLE (1 << 14)
#define SYS_CTRL_CLK_SPI_CAM_POL (1 << 15)
#define SYS_CTRL_CLK_SPI_CAM_SEL (1 << 16)
#define SYS_CTRL_CLK_SPI_CAM_DIV(n) (((n)&0x7ff) << 17)
#define SYS_CTRL_CLK_SPI_CAM_DIV_UPDATE (1 << 28)

#define SYS_CTRL_CLK_CAMERA_OUT_EN_V_DISABLE (0)
#define SYS_CTRL_CLK_CAMERA_OUT_EN_V_ENABLE (1)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_V_26_MHZ (0)
#define SYS_CTRL_CLK_CAMERA_DIV_SRC_SEL_V_156_MHZ (1)
#define SYS_CTRL_CLK_CAMERA_DIV_UPDATE_V_DISABLE (0)
#define SYS_CTRL_CLK_CAMERA_DIV_UPDATE_V_ENABLE (1)
#define SYS_CTRL_CLK_SPI_CAM_EN_V_DISABLE (0)
#define SYS_CTRL_CLK_SPI_CAM_EN_V_ENABLE (1)

// reset_cause
#define SYS_CTRL_WATCHDOG_RESET_1_HAPPENED (1 << 0)
#define SYS_CTRL_WATCHDOG_RESET_1_NO (0 << 0)
#define SYS_CTRL_WATCHDOG_RESET_2_HAPPENED (1 << 1)
#define SYS_CTRL_WATCHDOG_RESET_2_NO (0 << 1)
#define SYS_CTRL_WATCHDOG_RESET_3_HAPPENED (1 << 2)
#define SYS_CTRL_WATCHDOG_RESET_3_NO (0 << 2)
#define SYS_CTRL_WATCHDOG_RESET_4_HAPPENED (1 << 3)
#define SYS_CTRL_WATCHDOG_RESET_4_NO (0 << 3)
#define SYS_CTRL_WATCHDOG_RESET_RF_HAPPENED (1 << 4)
#define SYS_CTRL_WATCHDOG_RESET_RF_NO (0 << 4)
#define SYS_CTRL_WATCHDOG_RESET_GGE_HAPPENED (1 << 5)
#define SYS_CTRL_WATCHDOG_RESET_GGE_NO (0 << 5)
#define SYS_CTRL_WATCHDOG_RESET_ZSP_HAPPENED (1 << 6)
#define SYS_CTRL_WATCHDOG_RESET_ZSP_NO (0 << 6)
#define SYS_CTRL_GLOBALSOFT_RESET_HAPPENED (1 << 7)
#define SYS_CTRL_GLOBALSOFT_RESET_NO (0 << 7)
#define SYS_CTRL_HOSTDEBUG_RESET_HAPPENED (1 << 8)
#define SYS_CTRL_HOSTDEBUG_RESET_NO (0 << 8)
#define SYS_CTRL_WATCHDOG_RESET_CP_HAPPENED (1 << 9)
#define SYS_CTRL_WATCHDOG_RESET_CP_NO (0 << 9)
#define SYS_CTRL_ALARMCAUSE_HAPPENED (1 << 12)
#define SYS_CTRL_ALARMCAUSE_NO (0 << 12)
#define SYS_CTRL_BOOT_MODE(n) (((n)&0x3f) << 16)
#define SYS_CTRL_SW_BOOT_MODE(n) (((n)&0x3f) << 22)
#define SYS_CTRL_FONCTIONAL_TEST_MODE (1 << 31)

#define SYS_CTRL_WATCHDOG_RESET_1_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_1_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_2_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_2_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_3_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_3_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_4_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_4_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_RF_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_RF_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_GGE_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_GGE_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_ZSP_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_ZSP_V_NO (0)
#define SYS_CTRL_GLOBALSOFT_RESET_V_HAPPENED (1)
#define SYS_CTRL_GLOBALSOFT_RESET_V_NO (0)
#define SYS_CTRL_HOSTDEBUG_RESET_V_HAPPENED (1)
#define SYS_CTRL_HOSTDEBUG_RESET_V_NO (0)
#define SYS_CTRL_WATCHDOG_RESET_CP_V_HAPPENED (1)
#define SYS_CTRL_WATCHDOG_RESET_CP_V_NO (0)
#define SYS_CTRL_ALARMCAUSE_V_HAPPENED (1)
#define SYS_CTRL_ALARMCAUSE_V_NO (0)

// wakeup
#define SYS_CTRL_FORCE_WAKEUP (1 << 0)

// ignore_charger
#define SYS_CTRL_IGNORE_CHARGER (1 << 0)

// cfg_pll_spiflash_div
#define SYS_CTRL_CFG_PLL_SPIFLASH_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_SPIFLASH_DIV_UPDATE (1 << 4)

// cfg_pll_spiflash1_div
#define SYS_CTRL_CFG_PLL_SPIFLASH1_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_SPIFLASH1_DIV_UPDATE (1 << 4)

// cfg_pll_mem_bridge_div
#define SYS_CTRL_CFG_MEM_BRIDGE_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_MEM_BRIDGE_DIV_UPDATE (1 << 4)

// cfg_dbg_clk_source_sel
#define SYS_CTRL_CFG_DBG_CLK_SOURCE_SEL(n) (((n)&0x3f) << 0)

// cfg_clk_494m_lte_div
#define SYS_CTRL_CFG_CLK_494M_LTE_APLL_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_CLK_494M_LTE_APLL_DIV_UPDATE (1 << 4)
#define SYS_CTRL_CFG_CLKSW_494M_LTE_USBPHY_SEL (1 << 5)
#define SYS_CTRL_CFG_CLKSW_494M_CLK_SEL (1 << 6)
#define SYS_CTRL_CFG_CLK_96M_26M_SEL (1 << 7)
#define SYS_CTRL_CFG_CLK_96M_DIV(n) (((n)&0x7fffff) << 8)
#define SYS_CTRL_CFG_CLK_96M_DIV_UPDATE (1 << 31)

// cfg_pll_isp_div
#define SYS_CTRL_CFG_PLL_ISP_DIV(n) (((n)&0xfff) << 0)
#define SYS_CTRL_CFG_PLL_ISP_DIV_UPDATE (1 << 12)

// cfg_pll_pix_div
#define SYS_CTRL_CFG_PLL_PIX_DIV(n) (((n)&0x7f) << 0)
#define SYS_CTRL_CFG_PLL_PIX_DIV_UPDATE (1 << 7)

// cfg_pll_sys_axi_div
#define SYS_CTRL_CFG_PLL_SYS_AXI_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_SYS_AXI_DIV_UPDATE (1 << 4)

// cfg_pll_sys_ahb_apb_div
#define SYS_CTRL_CFG_PLL_AP_APB_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_AP_APB_DIV_UPDATE (1 << 4)

// cfg_pll_sys_ahb_btfm_div
#define SYS_CTRL_CFG_PLL_AHB_BTFM_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_AHB_BTFM_DIV_UPDATE (1 << 4)

// cfg_pll_csi_div
#define SYS_CTRL_CFG_PLL_CSI_DIV(n) (((n)&0x7f) << 0)
#define SYS_CTRL_CFG_PLL_CSI_DIV_UPDATE (1 << 7)

// cfg_pll_sys_spiflash_div
#define SYS_CTRL_CFG_PLL_SYS_SPIFLASH_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_SYS_SPIFLASH_DIV_UPDATE (1 << 4)

// cfg_pll_cp_div
#define SYS_CTRL_CFG_PLL_CP_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_CP_DIV_UPDATE (1 << 4)

// cfg_pll_ap_cpu_div
#define SYS_CTRL_CFG_PLL_AP_CPU_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_AP_CPU_DIV_UPDATE (1 << 4)

// cfg_ap_cpu_aclken_div
#define SYS_CTRL_CFG_AP_CPU_ACLKEN_DIV(n) (((n)&0x7) << 0)
#define SYS_CTRL_CFG_AP_CPU_ACLKEN_DIV_UPDATE (1 << 3)

// cfg_ap_cpu_dbgen_div
#define SYS_CTRL_CFG_AP_CPU_DBGEN_DIV(n) (((n)&0x7) << 0)
#define SYS_CTRL_CFG_AP_CPU_DBGEN_DIV_UPDATE (1 << 3)

// cfg_pll_cp_cpu_div
#define SYS_CTRL_CFG_PLL_CP_CPU_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_PLL_CP_CPU_DIV_UPDATE (1 << 4)

// cfg_cp_cpu_aclken_div
#define SYS_CTRL_CFG_CP_CPU_ACLKEN_DIV(n) (((n)&0x7) << 0)
#define SYS_CTRL_CFG_CP_CPU_ACLKEN_DIV_UPDATE (1 << 3)

// cfg_cp_cpu_dbgen_div
#define SYS_CTRL_CFG_CP_CPU_DBGEN_DIV(n) (((n)&0x7) << 0)
#define SYS_CTRL_CFG_CP_CPU_DBGEN_DIV_UPDATE (1 << 3)

// cfg_trng_clken_div
#define SYS_CTRL_CFG_TRNG_CLKEN_DIV(n) (((n)&0x1f) << 0)
#define SYS_CTRL_CFG_TRNG_CLKEN_DIV_UPDATE (1 << 5)

// cfg_coresight_div
#define SYS_CTRL_CFG_CORESIGHT_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_CORESIGHT_DIV_UPDATE (1 << 4)

// cfg_usb11_48m_div
#define SYS_CTRL_CFG_USB11_48M_DIV(n) (((n)&0x1f) << 0)
#define SYS_CTRL_CFG_USB11_48M_DIV_UPDATE (1 << 5)

// cfg_pll_usb
#define SYS_CTRL_CFG_PLL_USB_RESET (1 << 0)
#define SYS_CTRL_CFG_PLL_USB_REV (1 << 1)

// cfg_vad_div
#define SYS_CTRL_CFG_VAD_DIV(n) (((n)&0xf) << 0)
#define SYS_CTRL_CFG_VAD_DIV_UPDATE (1 << 4)

// cfg_hmprot
#define SYS_CTRL_HPROT_BBDMA(n) (((n)&0xf) << 0)
#define SYS_CTRL_HPROT_AIF_IFC(n) (((n)&0xf) << 4)
#define SYS_CTRL_HPROT_GOUDA(n) (((n)&0xf) << 8)
#define SYS_CTRL_HPROT_AP_IFC(n) (((n)&0xf) << 12)
#define SYS_CTRL_HPROT_USB(n) (((n)&0xf) << 16)
#define SYS_CTRL_HPROT_SYS_AON_IFC(n) (((n)&0xf) << 20)
#define SYS_CTRL_HSPROT_PSRAM_CTRL(n) (((n)&0xf) << 24)

// cfg_mem_cq
#define SYS_CTRL_MEM_EMA_CQ(n) (((n)&0xffffffff) << 0)

// cfg_arm_sys_cfg_mem
#define SYS_CTRL_ARM_SYS_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_audio_cfg_mem
#define SYS_CTRL_AUDIO_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_lcd_cfg_mem
#define SYS_CTRL_LCD_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_cam_cfg_mem
#define SYS_CTRL_CAM_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_peri_cfg_mem
#define SYS_CTRL_PERI_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_aon_sys_cfg_mem
#define SYS_CTRL_AON_SYS_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_rf_sys_cfg_mem
#define SYS_CTRL_RF_SYS_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_coresight_cfg_mem
#define SYS_CTRL_CORESIGHT_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_vad_cfg_mem
#define SYS_CTRL_VAD_CFG_MEM(n) (((n)&0xffffffff) << 0)

// cfg_aif_cfg
#define SYS_CTRL_AIF1_LOAD_POS(n) (((n)&0x3f) << 0)
#define SYS_CTRL_AIF2_LOAD_POS(n) (((n)&0x3f) << 6)
#define SYS_CTRL_AIF1_SEL(n) (((n)&0x7) << 12)
#define SYS_CTRL_AIF2_SEL(n) (((n)&0x7) << 15)
#define SYS_CTRL_I2S1_SEL(n) (((n)&0x7) << 18)
#define SYS_CTRL_I2S2_SEL(n) (((n)&0x7) << 21)
#define SYS_CTRL_I2S3_SEL(n) (((n)&0x7) << 24)
#define SYS_CTRL_I2S1_BCK_LRCK_OEN (1 << 27)
#define SYS_CTRL_I2S2_BCK_LRCK_OEN (1 << 28)
#define SYS_CTRL_I2S3_BCK_LRCK_OEN (1 << 29)

// cfg_misc_cfg
#define SYS_CTRL_LIMIT_EN_SPI1 (1 << 0)
#define SYS_CTRL_LIMIT_EN_SPI2 (1 << 1)
#define SYS_CTRL_LIMIT_EN_SPI3 (1 << 2)
#define SYS_CTRL_CFGSDISABLE_GIC400 (1 << 3)
#define SYS_CTRL_AIF1_I2S_BCK_SEL(n) (((n)&0x7) << 4)
#define SYS_CTRL_AIF2_I2S_BCK_SEL(n) (((n)&0x7) << 8)
#define SYS_CTRL_WCN_UART_OUT_SEL (1 << 11)
#define SYS_CTRL_AP_UART_OUT_SEL (1 << 12)
#define SYS_CTRL_CFG_MODE_LP (1 << 13)
#define SYS_CTRL_CFG_FORCE_LP (1 << 14)
#define SYS_CTRL_CFG_NUMBER_LP(n) (((n)&0xffff) << 15)
#define SYS_CTRL_BCPU_STALL_ACK (1 << 31)

// cfg_misc1_cfg
#define SYS_CTRL_DEBUG_MON_SEL(n) (((n)&0x1f) << 0)
#define SYS_CTRL_IOMUX_CLK_FORCE_ON (1 << 5)
#define SYS_CTRL_AP_RST_CTRL (1 << 6)
#define SYS_CTRL_GGE_RST_CTRL (1 << 7)
#define SYS_CTRL_BTFM_RST_CTRL (1 << 8)
#define SYS_CTRL_AP_CLK_CTRL (1 << 9)
#define SYS_CTRL_GGE_CLK_CTRL (1 << 10)
#define SYS_CTRL_BTFM_CLK_CTRL (1 << 11)
#define SYS_CTRL_BBPLL1_ENABLE (1 << 12)
#define SYS_CTRL_BBPLL2_ENABLE (1 << 13)
#define SYS_CTRL_MEMPLL_ENABLE (1 << 14)
#define SYS_CTRL_USBPLL_ENABLE (1 << 15)
#define SYS_CTRL_AUDIOPLL_ENABLE (1 << 16)
#define SYS_CTRL_APLL_ZSP_CLK_SEL (1 << 17)
#define SYS_CTRL_APLL_BBPLL2_CLK_SEL (1 << 18)
#define SYS_CTRL_REG_GIC400_ARUSER_SEL (1 << 19)
#define SYS_CTRL_REG_GIC400_ARUSER_DBG (1 << 20)
#define SYS_CTRL_REG_GIC400_AWUSER_SEL (1 << 21)
#define SYS_CTRL_REG_GIC400_AWUSER_DBG (1 << 22)
#define SYS_CTRL_LVDS_WCN_RFDIG_SEL (1 << 23)
#define SYS_CTRL_LVDS_RFDIG_RF_BB_SEL (1 << 24)
#define SYS_CTRL_WCN_OSC_EN_CTRL (1 << 25)
#define SYS_CTRL_VAD_CLK_FORCE_ON (1 << 26)
#define SYS_CTRL_VAD_CLK_PN_SEL (1 << 27)
#define SYS_CTRL_AUD_SCLK_O_PN_SEL (1 << 28)
#define SYS_CTRL_AON_LP_RST_CTRL (1 << 29)
#define SYS_CTRL_AON_LP_CLK_CTRL (1 << 30)
#define SYS_CTRL_DUMP_PATH (1 << 31)

// cfg_force_lp_mode_lp
#define SYS_CTRL_CFG_FORCE_LP_A5 (1 << 0)
#define SYS_CTRL_CFG_FORCE_LP_AHB (1 << 1)
#define SYS_CTRL_CFG_FORCE_LP_CONNECT (1 << 2)
#define SYS_CTRL_CFG_FORCE_LP_CP_A5 (1 << 3)
#define SYS_CTRL_CFG_FORCE_LP_PSRAM (1 << 4)
#define SYS_CTRL_CFG_FORCE_LP_SPIFLASH (1 << 5)
#define SYS_CTRL_CFG_FORCE_LP_SPIFLASH1 (1 << 6)
#define SYS_CTRL_CFG_MODE_LP_A5 (1 << 7)
#define SYS_CTRL_CFG_MODE_LP_AHB (1 << 8)
#define SYS_CTRL_CFG_MODE_LP_CONNECT (1 << 9)
#define SYS_CTRL_CFG_MODE_LP_CP_A5 (1 << 10)
#define SYS_CTRL_CFG_MODE_LP_PSRAM (1 << 11)
#define SYS_CTRL_CFG_MODE_LP_SPIFLASH (1 << 12)
#define SYS_CTRL_CFG_MODE_LP_SPIFLASH1 (1 << 13)
#define SYS_CTRL_CFG_MODE_LP_AHB_MERGE (1 << 14)
#define SYS_CTRL_FORCE_LP_AHB_MERGE (1 << 15)

// cfg_reserve
#define SYS_CTRL_WD_RST_MODE (1 << 0)
#define SYS_CTRL_SYS_BB_SIDE (1 << 1)
#define SYS_CTRL_CAM_OUT0_SEL (1 << 2)
#define SYS_CTRL_VPU_CLK_EN (1 << 3)
#define SYS_CTRL_WD_1_RST_EN (1 << 4)
#define SYS_CTRL_WD_2_RST_EN (1 << 5)
#define SYS_CTRL_WD_3_RST_EN (1 << 6)
#define SYS_CTRL_WD_4_RST_EN (1 << 7)
#define SYS_CTRL_WD_RF_RST_EN (1 << 8)
#define SYS_CTRL_WD_GGE_RST_EN (1 << 9)
#define SYS_CTRL_WD_ZSP_RST_EN (1 << 10)
#define SYS_CTRL_WD_CP_RST_EN (1 << 11)
#define SYS_CTRL_DMC_PHY_RST_EN (1 << 12)
#define SYS_CTRL_HMPROT_WCN_PERI(n) (((n)&0xf) << 13)
#define SYS_CTRL_HMPROT_WCN_MEM(n) (((n)&0xf) << 17)
#define SYS_CTRL_HMPROT_AES(n) (((n)&0xf) << 21)
#define SYS_CTRL_RESERVE(n) (((n)&0x7f) << 25)

// cfg_reserve1
#define SYS_CTRL_RESERVE1(n) (((n)&0xffffffff) << 0)

// cfg_reserve2
#define SYS_CTRL_RESERVE2(n) (((n)&0xffffffff) << 0)

// cfg_reserve3
#define SYS_CTRL_RESERVE3(n) (((n)&0xffffffff) << 0)

// cfg_reserve4
#define SYS_CTRL_RESERVE4(n) (((n)&0xffffffff) << 0)

// cfg_reserve5
#define SYS_CTRL_RESERVE5(n) (((n)&0xffffffff) << 0)

// cfg_reserve6
#define SYS_CTRL_RESERVE6(n) (((n)&0xffffffff) << 0)

// cfg_reserve7
#define SYS_CTRL_RESERVE7(n) (((n)&0xffffffff) << 0)

// cfg_reserve8
#define SYS_CTRL_RESERVE8(n) (((n)&0xffffffff) << 0)

// cfg_reserve9
#define SYS_CTRL_RESERVE9(n) (((n)&0xffffffff) << 0)

// cfg_reserve10
#define SYS_CTRL_RESERVE10(n) (((n)&0xffffffff) << 0)

// cfg_reserve11
#define SYS_CTRL_RESERVE11(n) (((n)&0xffffffff) << 0)

// cfg_reserve12
#define SYS_CTRL_RESERVE12(n) (((n)&0xffffffff) << 0)

// cfg_reserve13
#define SYS_CTRL_RESERVE13(n) (((n)&0xffffffff) << 0)

// cfg_reserve14
#define SYS_CTRL_RESERVE14(n) (((n)&0xffffffff) << 0)

// cfg_reserve15
#define SYS_CTRL_RESERVE15(n) (((n)&0xffffffff) << 0)

// cfg_reserve16
#define SYS_CTRL_RESERVE16(n) (((n)&0xffffffff) << 0)

// cfg_reserve1_in
#define SYS_CTRL_RESERVE1_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve2_in
#define SYS_CTRL_RESERVE2_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve3_in
#define SYS_CTRL_RESERVE3_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve4_in
#define SYS_CTRL_RESERVE4_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve5_in
#define SYS_CTRL_RESERVE5_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve6_in
#define SYS_CTRL_RESERVE6_IN(n) (((n)&0xffffffff) << 0)

// cfg_reserve7_in
#define SYS_CTRL_RESERVE7_IN(n) (((n)&0xffffffff) << 0)

// cfg_chip_prod_id
#define SYS_CTRL_METAL_ID(n) (((n)&0xfff) << 0)
#define SYS_CTRL_BOND_ID(n) (((n)&0xf) << 12)
#define SYS_CTRL_PROD_ID(n) (((n)&0xffff) << 16)

// cfg_qos_wcn_a5_gge
#define SYS_CTRL_WCN_MEM_ARQOS(n) (((n)&0x1f) << 0)
#define SYS_CTRL_WCN_MEM_AWQOS(n) (((n)&0x1f) << 5)
#define SYS_CTRL_GGE_ARQOS(n) (((n)&0x1f) << 10)
#define SYS_CTRL_GGE_AWQOS(n) (((n)&0x1f) << 15)
#define SYS_CTRL_A5_ARQOS(n) (((n)&0x1f) << 20)
#define SYS_CTRL_A5_AWQOS(n) (((n)&0x1f) << 25)

// cfg_qos_axidma_cpa5_f8
#define SYS_CTRL_AXIDMA_ARQOS(n) (((n)&0x1f) << 0)
#define SYS_CTRL_AXIDMA_AWQOS(n) (((n)&0x1f) << 5)
#define SYS_CTRL_CP_A5_ARQOS(n) (((n)&0x1f) << 10)
#define SYS_CTRL_CP_A5_AWQOS(n) (((n)&0x1f) << 15)
#define SYS_CTRL_F8_ARQOS(n) (((n)&0x1f) << 20)
#define SYS_CTRL_F8_AWQOS(n) (((n)&0x1f) << 25)

// cfg_qos_lcdc_lzma_gouda
#define SYS_CTRL_LCDC_ARQOS(n) (((n)&0x1f) << 0)
#define SYS_CTRL_LCDC_AWQOS(n) (((n)&0x1f) << 5)
#define SYS_CTRL_LZMA_ARQOS(n) (((n)&0x1f) << 10)
#define SYS_CTRL_LZMA_AWQOS(n) (((n)&0x1f) << 15)
#define SYS_CTRL_GOUDA_ARQOS(n) (((n)&0x1f) << 20)
#define SYS_CTRL_GOUDA_AWQOS(n) (((n)&0x1f) << 25)

// cfg_qos_lte_usb
#define SYS_CTRL_LTE_ARQOS(n) (((n)&0x1f) << 0)
#define SYS_CTRL_LTE_AWQOS(n) (((n)&0x1f) << 5)
#define SYS_CTRL_USB_ARQOS(n) (((n)&0x1f) << 10)
#define SYS_CTRL_USB_AWQOS(n) (((n)&0x1f) << 15)

// cfg_qos_merge_mem
#define SYS_CTRL_MERGE_MEM_AWQOS(n) (((n)&0x1f) << 0)
#define SYS_CTRL_MERGE_MEM_ARQOS(n) (((n)&0x1f) << 5)

// cfg_bcpu_dbg_bkp
#define SYS_CTRL_BCPU_BKPT_ADDR(n) (((n)&0xfffffff) << 0)
#define SYS_CTRL_BCPU_BKPT_MODE(n) (((n)&0x3) << 28)
#define SYS_CTRL_BCPU_BKPT_EN (1 << 30)
#define SYS_CTRL_BCPU_STALLED_W1C (1 << 31)

#endif // _SYS_CTRL_H_
