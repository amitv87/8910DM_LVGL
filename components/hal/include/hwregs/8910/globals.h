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

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define NB_BITS_ADDR (32)
#define AP_NB_BITS_ADDR (32)
#define AON_NB_BITS_PSEL (7)
#define AON_NB_BITS_PADDR (12)
#define AON_APB_STEP (0x1000)
#define AON_APB_ID_IFC_BASE (16)
#define AON_NB_PSEL (19)
#define DEBUG_HOST_SLAVE_ID (127)
#define AP_NB_BITS_PSEL (7)
#define AP_NB_BITS_PADDR (12)
#define AP_APB_STEP (0x1000)
#define AP_APB_ID_IFC_BASE (16)
#define AP_NB_PSEL (28)
#define SYS_NB_BITS_HADDR (18)
#define SYS_AHB_STEP (0x40000)
#define NB_SYS_AHB_SLAVES (7)
#define AIF_NB_BITS_PSEL (5)
#define AIF_NB_BITS_PADDR (12)
#define AIF_APB_STEP (0x1000)
#define AIF_NB_PSEL (8)
#define AIF_SLAVE_ID (7)
#define AP_APB1_NB_BITS_PADDR (12)
#define AP_APB1_NB_BITS_PSEL (7)
#define AUDIO_IFC_APB_STEP (256)
#define AP_NB_DMA_REQ_WIDTH (5)
#define AP_NB_DMA_REQ (20)
#define AON_NB_DMA_REQ_WIDTH (5)
#define AON_NB_DMA_REQ (6)
#define NB_SYS_IRQ (91)
#define NB_CP_IRQ (64)
#define NB_CP_OTHER_IRQ (41)
#define RF_SPI_SLAVE_ID (22)
#define SYS_NB_MASTERS (9)
#define BB_NB_MASTERS (6)
#define BB_NB_SLAVES (7)
#define BB_NB_BITS_PSEL (5)
#define BB_NB_BITS_PADDR (12)
#define BB_NB_PSEL (8)
#define RF_SLAVE_ID (6)
#define BB_SYS_BITS_PADDR (12)
#define BB_SYS_NB_BITS_PSEL (4)
#define BB_SYS_STEP (0x1000)
#define CP_ZSP_SYS_PADDR (12)
#define CP_ZSP_NB_BITS_PSEL (12)
#define CP_ZSP_SYS_STEP (0x1000)
#define CP_LTE_SYS_PADDR (20)
#define CP_LTE_NB_BITS_PSEL (4)
#define CP_LTE_SYS_STEP (0x100000)
#define GGE_SYS_NB_BITS_PSEL (5)
#define GGE_SYS_NB_BITS_PADDR (12)
#define GGE_SYS_APB_STEP (0x1000)
#define GGE_SYS_APB_NB_PSEL (16)
#define GGE_SYS_NB_DMA_REQ_WIDTH (5)
#define GGE_SYS_NB_DMA_REQ (0)
#define GGE_BB_NB_BITS_PSEL (5)
#define GGE_BB_NB_BITS_PADDR (12)
#define GGE_BB_APB_STEP (0x1000)
#define GGE_BB_APB_NB_PSEL (27)
#define NB_GGE_BB_IRQ_PULSE (8)
#define NB_GGE_BB_IRQ (30)
#define NB_GGE_BB_IRQ_LEVEL (22)
#define BB_SYMBOL_SIZE (13)
#define VITAC_MULT_SIZE (14)
#define BB_SRAM_ADDR_WIDTH (13)
#define RF_NB_BITS_PADDR (12)
#define RF_APB_STEP (0x1000)
#define RF_NB_PSEL (13)
#define WCN_SYS_NB_BITS_PSEL (4)
#define WCN_SYS_NB_BITS_PADDR (12)
#define WCN_SYS_APB_STEP (0x1000)
#define WCN_SYS_APB_NB_PSEL (13)

typedef enum
{
    AON_APB_ID_SYS_CTRL = 0,
    AON_APB_ID_LVDS = 1,
    AON_APB_ID_GSM_LPS = 2,
    AON_APB_ID_I2C_M2 = 3,
    AON_APB_ID_MAILBOX = 4,
    AON_APB_ID_TIMER_3 = 5,
    AON_APB_ID_KEYPAD = 6,
    AON_APB_ID_GPIO_1 = 7,
    AON_APB_ID_PWM = 8,
    AON_APB_ID_ANALOG_REG = 9,
    AON_APB_ID_AON_IFC = 10,
    AON_APB_ID_NB_LPS = 11,
    AON_APB_ID_IOMUX = 12,
    AON_APB_ID_SPINLOCK = 13,
    AON_APB_ID_EFUSE = 14,
    AON_APB_ID_NORMAL_LAST = 15,
    AON_APB_ID_DEBUG_UART = 16,
    AON_APB_ID_17_RESERVED = 17,
    AON_APB_ID_VAD = 18,
    AON_APB_ID_DEBUG_HOST = 127,
} AON_APB_ID_T;

typedef enum
{
    AP_APB_ID_IRQH = 0,
    AP_APB_ID_LPDDR_PSRAM_CTRL = 1,
    AP_APB_ID_PAGESPY = 2,
    AP_APB_ID_DMC_CTRL = 3,
    AP_APB_ID_SYSIMEM = 4,
    AP_APB_ID_LZMA = 5,
    AP_APB_ID_GOUDA = 6,
    AP_APB_ID_LCDC = 7,
    AP_APB_ID_TIMER_1 = 8,
    AP_APB_ID_TIMER_2 = 9,
    AP_APB_ID_I2C_M1 = 10,
    AP_APB_ID_I2C_M3 = 11,
    AP_APB_ID_12_RESERVED = 12,
    AP_APB_ID_AUD_2AD = 13,
    AP_APB_ID_TIMER_4 = 14,
    AP_APB_ID_AP_IFC = 15,
    AP_APB_ID_NORMAL_LAST = 15,
    AP_APB_ID_SDMMC1 = 16,
    AP_APB_ID_SDMMC2 = 17,
    AP_APB_ID_SPI_1 = 18,
    AP_APB_ID_SPI_2 = 19,
    AP_APB_ID_SCI_1 = 20,
    AP_APB_ID_SCI_2 = 21,
    AP_APB_ID_ZSP_UART = 22,
    AP_APB_ID_UART_2 = 23,
    AP_APB_ID_UART_3 = 24,
    AP_APB_ID_CAMERA = 25,
    AP_APB1_ID_AIF = 26,
    AP_APB1_ID_AIF2 = 27,
} AP_APB_ID_T;

typedef enum
{
    SYS_AHB_ID_F8 = 1,
    SYS_AHB_ID_USBC = 2,
    SYS_AHB_ID_GOUDA = 3,
    SYS_AHB_ID_AXIDMA = 4,
    SYS_AHB_ID_GEA3 = 5,
    SYS_AHB_ID_AES = 6,
    SYS_AHB_ID_USB11 = 7,
} SYS_AHB_ID_T;

typedef enum
{
    AIF_APB_ID_AIF = 7,
} AIF_APB_ID_T;

typedef enum
{
    AP_APB_DMA_ID_SDMMC1_TX = 0,
    AP_APB_DMA_ID_SDMMC1_RX = 1,
    AP_APB_DMA_ID_SDMMC2_TX = 2,
    AP_APB_DMA_ID_SDMMC2_RX = 3,
    AP_APB_DMA_ID_SPI_1_TX = 4,
    AP_APB_DMA_ID_SPI_1_RX = 5,
    AP_APB_DMA_ID_SPI_2_TX = 6,
    AP_APB_DMA_ID_SPI_2_RX = 7,
    AP_APB_DMA_ID_SCI_1_TX = 8,
    AP_APB_DMA_ID_SCI_1_RX = 9,
    AP_APB_DMA_ID_SCI_2_TX = 10,
    AP_APB_DMA_ID_SCI_2_RX = 11,
    AP_APB_DMA_ID_FREE_TX = 18,
    AP_APB_DMA_ID_CAMERA_RX = 19,
} AP_IFC_REQUEST_IDS_T;

typedef enum
{
    SYS_AXI_DMA_ID_UART_1_RX = 0,
    SYS_AXI_DMA_ID_UART_1_TX = 1,
    SYS_AXI_DMA_ID_UART_2_RX = 2,
    SYS_AXI_DMA_ID_UART_2_TX = 3,
    SYS_AXI_DMA_ID_UART_3_RX = 4,
    SYS_AXI_DMA_ID_UART_3_TX = 5,
    SYS_AXI_DMA_ID_ZBUSMON = 6,
    SYS_AXI_DMA_ID_ZSP_UART = 7,
} SYS_AXI_DMA_REQUEST_IDS_T;

typedef enum
{
    AON_APB_DMA_ID_TX_DEBUG_UART = 0,
    AON_APB_DMA_ID_RX_DEBUG_UART = 1,
    AON_APB_DMA_ID_RX_VAD = 5,
} AON_IFC_REQUEST_IDS_T;

typedef enum
{
    SYS_IRQ_ID_PAGE_SPY = 0,
    SYS_IRQ_ID_IMEM = 1,
    SYS_IRQ_ID_TIMER_1 = 2,
    SYS_IRQ_ID_TIMER_1_OS = 3,
    SYS_IRQ_ID_TIMER_2 = 4,
    SYS_IRQ_ID_TIMER_2_OS = 5,
    SYS_IRQ_ID_TIMER_4 = 6,
    SYS_IRQ_ID_TIMER_4_OS = 7,
    SYS_IRQ_ID_I2C_M1 = 8,
    SYS_IRQ_ID_AIF_APB_0 = 9,
    SYS_IRQ_ID_AIF_APB_1 = 10,
    SYS_IRQ_ID_AIF_APB_2 = 11,
    SYS_IRQ_ID_AIF_APB_3 = 12,
    SYS_IRQ_ID_AUD_2AD = 13,
    SYS_IRQ_ID_SDMMC1 = 14,
    SYS_IRQ_ID_SDMMC2 = 15,
    SYS_IRQ_ID_SPI_1 = 16,
    SYS_IRQ_ID_SPI_2 = 17,
    SYS_IRQ_ID_ZSP_UART = 18,
    SYS_IRQ_ID_UART_2 = 19,
    SYS_IRQ_ID_UART_3 = 20,
    SYS_IRQ_ID_CAMERA = 21,
    SYS_IRQ_ID_LZMA = 22,
    SYS_IRQ_ID_GOUDA = 23,
    SYS_IRQ_ID_F8 = 24,
    SYS_IRQ_ID_USBC = 25,
    SYS_IRQ_ID_USB11 = 26,
    SYS_IRQ_ID_AXIDMA = 27,
    SYS_IRQ_ID_AXIDMA_1_SECURITY = 28,
    SYS_IRQ_ID_AXIDMA_1_UNSECURITY = 29,
    SYS_IRQ_ID_PMU_APCPU = 30,
    SYS_IRQ_ID_31_RESERVED = 31,
    SYS_IRQ_ID_LCD = 32,
    SYS_IRQ_ID_SPIFLASH = 33,
    SYS_IRQ_ID_SPIFLASH1 = 34,
    SYS_IRQ_ID_GPRS_0 = 35,
    SYS_IRQ_ID_GPRS_1 = 36,
    SYS_IRQ_ID_DMC = 37,
    SYS_IRQ_ID_AES = 38,
    SYS_IRQ_ID_CTI_APCPU = 39,
    SYS_IRQ_ID_AP_TZ_SLV = 40,
    SYS_IRQ_ID_AP_TZ_MEM = 41,
    SYS_IRQ_ID_I2C_M3 = 42,
    SYS_IRQ_ID_GSM_LPS = 43,
    SYS_IRQ_ID_I2C_M2 = 44,
    SYS_IRQ_ID_TIMER_3 = 45,
    SYS_IRQ_ID_TIMER_3_OS = 46,
    SYS_IRQ_ID_KEYPAD = 47,
    SYS_IRQ_ID_GPIO_1 = 48,
    SYS_IRQ_ID_DEBUG_UART = 49,
    SYS_IRQ_ID_SCI_1 = 50,
    SYS_IRQ_ID_SCI_2 = 51,
    SYS_IRQ_ID_ADI = 52,
    SYS_IRQ_ID_UART_1 = 53,
    SYS_IRQ_ID_VAD = 54,
    SYS_IRQ_ID_VAD_PULSE = 55,
    SYS_IRQ_ID_AON_TZ = 56,
    SYS_IRQ_ID_NB_LPS = 57,
    SYS_IRQ_ID_CP_IDLE_H = 58,
    SYS_IRQ_ID_CP_IDLE_2_H = 59,
    SYS_IRQ_ID_MAILBOX_ARM_AP = 60,
    SYS_IRQ_ID_LTEM1_FRAME = 61,
    SYS_IRQ_ID_LTEM2_FRAME = 62,
    SYS_IRQ_ID_RFSPI_CONFLICT = 63,
    SYS_IRQ_ID_CP_WD_RESET = 64,
    SYS_IRQ_ID_GSM_FRAME = 65,
    SYS_IRQ_ID_PWRCTRL_TIMEOUT = 66,
    SYS_IRQ_ID_NB_FRAME = 67,
    SYS_IRQ_ID_ZSP_AXIDMA = 68,
    SYS_IRQ_ID_ZSP_BUSMON = 69,
    SYS_IRQ_ID_ZSP_WD = 70,
    SYS_IRQ_ID_71_RESERVED = 71,
    SYS_IRQ_ID_GGE_FINT = 72,
    SYS_IRQ_ID_GGE_TCU_0 = 73,
    SYS_IRQ_ID_GGE_TCU_1 = 74,
    SYS_IRQ_ID_NB_FINT = 75,
    SYS_IRQ_ID_NB_TCU_0 = 76,
    SYS_IRQ_ID_NB_TCU_1 = 77,
    SYS_IRQ_ID_NB_TCU_SYNC = 78,
    SYS_IRQ_ID_GGE_COMP_INT = 79,
    SYS_IRQ_ID_GGE_NB_RX_DSP = 80,
    SYS_IRQ_ID_GGE_NB_RX_MCU = 81,
    SYS_IRQ_ID_GGE_NB_TX_DSP = 82,
    SYS_IRQ_ID_GGE_NB_ACC_DSP = 83,
    SYS_IRQ_ID_84_RESERVED = 84,
    SYS_IRQ_ID_WCN_WLAN = 85,
    SYS_IRQ_ID_WCN_SOFT = 86,
    SYS_IRQ_ID_WCN_WDT_RST = 87,
    SYS_IRQ_ID_WCN_AWAKE = 88,
    SYS_IRQ_ID_RF_WDG_RST = 89,
    SYS_IRQ_ID_LVDS = 90,
} SYS_IRQ_ID_T;

typedef enum
{
    CP_IRQ_ID_PAGE_SPY = 0,
    CP_IRQ_ID_TIMER_1 = 1,
    CP_IRQ_ID_TIMER_1_OS = 2,
    CP_IRQ_ID_UART_2 = 3,
    CP_IRQ_ID_UART_3 = 4,
    CP_IRQ_ID_F8 = 5,
    CP_IRQ_ID_AXIDMA = 6,
    CP_IRQ_ID_ZSP_UART = 7,
    CP_IRQ_ID_CTI_CPCPU = 8,
    CP_IRQ_ID_PMU_CPCPU = 9,
    CP_IRQ_ID_UART_1 = 10,
    CP_IRQ_ID_LPS_GSM = 11,
    CP_IRQ_ID_LPS_NB = 12,
    CP_IRQ_ID_SCI_1 = 13,
    CP_IRQ_ID_SCI_2 = 14,
    CP_IRQ_ID_VAD = 15,
    CP_IRQ_ID_VAD_PULSE = 16,
    CP_IRQ_ID_CP_IDLE_H = 17,
    CP_IRQ_ID_CP_IDLE_2_H = 18,
    CP_IRQ_ID_MAILBOX_ARM_CP = 19,
    CP_IRQ_ID_LTEM1_FRAME = 20,
    CP_IRQ_ID_LTEM2_FRAME = 21,
    CP_IRQ_ID_RFSPI_CONFLICT = 22,
    CP_IRQ_ID_GSM_FRAME = 23,
    CP_IRQ_ID_NB_FRAME = 24,
    CP_IRQ_ID_PWRCTRL_TIMEOUT = 25,
    CP_IRQ_ID_LTE_COEFF = 26,
    CP_IRQ_ID_LTE_CSIRS = 27,
    CP_IRQ_ID_LTE_DLFFT = 28,
    CP_IRQ_ID_LTE_DLFFT_ERR = 29,
    CP_IRQ_ID_LTE_IDDET = 30,
    CP_IRQ_ID_LTE_LDTC1_CTRL = 31,
    CP_IRQ_ID_LTE_LDTC1_DATA = 32,
    CP_IRQ_ID_LTE_LDTC1_VIT = 33,
    CP_IRQ_ID_LTE_LDTC = 34,
    CP_IRQ_ID_LTE_MEASPWR = 35,
    CP_IRQ_ID_LTE_PUSCH = 36,
    CP_IRQ_ID_LTE_RX_TRACE = 37,
    CP_IRQ_ID_LTE_TXRX = 38,
    CP_IRQ_ID_LTE_TX_TRACE = 39,
    CP_IRQ_ID_LTE_ULDFT = 40,
    CP_IRQ_ID_LTE_ULDFT_ERR = 41,
    CP_IRQ_ID_LTE_OTDOA = 42,
    CP_IRQ_ID_LTE_PBMEAS = 43,
    CP_IRQ_ID_LTE_RXCAPT = 44,
    CP_IRQ_ID_LTE_ULPCDCI = 45,
    CP_IRQ_ID_LTE_CORR = 46,
    CP_IRQ_ID_ZSP_AXIDMA = 47,
    CP_IRQ_ID_ZSP_BUSMON = 48,
    CP_IRQ_ID_ZSP_WD = 49,
    CP_IRQ_ID_ZSP_AUD_DFT = 50,
    CP_IRQ_ID_GGE_FINT = 51,
    CP_IRQ_ID_GGE_TCU_0 = 52,
    CP_IRQ_ID_GGE_TCU_1 = 53,
    CP_IRQ_ID_NB_FINT = 54,
    CP_IRQ_ID_NB_TCU_0 = 55,
    CP_IRQ_ID_NB_TCU_1 = 56,
    CP_IRQ_ID_GGE_NB_RX_DSP = 57,
    CP_IRQ_ID_GGE_NB_RX_MCU = 58,
    CP_IRQ_ID_GGE_NB_TX_DSP = 59,
    CP_IRQ_ID_GGE_NB_ACC_DSP = 60,
    CP_IRQ_ID_NB_TCU_SYNC = 61,
    CP_IRQ_ID_TIMER_3 = 62,
    CP_IRQ_ID_OTHERS = 63,
} CP_IRQ_ID_T;

typedef enum
{
    CP_IRQ_ID_IMEM = 0,
    CP_IRQ_ID_TIMER_2 = 1,
    CP_IRQ_ID_TIMER_2_OS = 2,
    CP_IRQ_ID_TIMER_4 = 3,
    CP_IRQ_ID_TIMER_4_OS = 4,
    CP_IRQ_ID_I2C_M1 = 5,
    CP_IRQ_ID_AIF_APB_0 = 6,
    CP_IRQ_ID_AIF_APB_1 = 7,
    CP_IRQ_ID_AIF_APB_2 = 8,
    CP_IRQ_ID_AIF_APB_3 = 9,
    CP_IRQ_ID_AUD_2AD = 10,
    CP_IRQ_ID_SDMMC1 = 11,
    CP_IRQ_ID_SDMMC2 = 12,
    CP_IRQ_ID_SPI_1 = 13,
    CP_IRQ_ID_SPI_2 = 14,
    CP_IRQ_ID_CAMERA = 15,
    CP_IRQ_ID_LZMA = 16,
    CP_IRQ_ID_GOUDA = 17,
    CP_IRQ_ID_USBC = 18,
    CP_IRQ_ID_USB11 = 19,
    CP_IRQ_ID_AXIDMA_1_SECURITY = 20,
    CP_IRQ_ID_AXIDMA_1_UNSECURITY = 21,
    CP_IRQ_ID_LCD = 22,
    CP_IRQ_ID_SPIFLASH = 23,
    CP_IRQ_ID_SPIFLASH1 = 24,
    CP_IRQ_ID_GPRS_0 = 25,
    CP_IRQ_ID_GPRS_1 = 26,
    CP_IRQ_ID_DMC = 27,
    CP_IRQ_ID_AES = 28,
    CP_IRQ_ID_AP_TZ_SLV = 29,
    CP_IRQ_ID_AP_TZ_MEM = 30,
    CP_IRQ_ID_I2C_M3 = 31,
    CP_IRQ_ID_GGE_COMP_INT = 32,
    CP_IRQ_ID_I2C_M2 = 33,
    CP_IRQ_ID_TIMER_3_OS = 34,
    CP_IRQ_ID_KEYPAD = 35,
    CP_IRQ_ID_GPIO_1 = 36,
    CP_IRQ_ID_DEBUG_UART = 37,
    CP_IRQ_ID_ADI = 38,
    CP_IRQ_ID_AON_TZ = 39,
    CP_IRQ_ID_CP_WD_RESET = 40,
} CP_OTHER_IRQ_ID_T;

typedef enum
{
    BB_SYSCTRL_ID_SYSREG = 0,
    BB_SYSCTRL_ID_CLKRST = 1,
    BB_SYSCTRL_ID_2_RESERVED = 2,
    BB_SYSCTRL_ID_MONITOR = 3,
    BB_SYSCTRL_ID_CP_WD_TIMER = 4,
    BB_SYSCTRL_ID_IDLE = 16,
    BB_SYSCTRL_ID_UART1 = 24,
    BB_SYSCTRL_ID_PWRCTRL = 32,
} BB_SYSCTRL_ID_T;

typedef enum
{
    CP_ZSP_SYS_ID_ZSP_AXIDMA = 0,
    CP_ZSP_SYS_ID_ZSP_AUD_DFT = 0x1000,
    CP_ZSP_SYS_ID_ZSP_WD = 0x1001,
    CP_ZSP_SYS_ID_ZSP_IRQH = 0x1002,
    CP_ZSP_SYS_ID_BUSMON = 0x1003,
} CP_ZSP_SYS_ID_T;

typedef enum
{
    CP_LTE_SYS_ID_TXRX = 0,
    CP_LTE_SYS_ID_RFAD = 1,
    CP_LTE_SYS_ID_COEFF = 2,
    CP_LTE_SYS_ID_LDTC = 3,
    CP_LTE_SYS_ID_OTDOA = 4,
    CP_LTE_SYS_ID_MEASPWR = 5,
    CP_LTE_SYS_ID_IDDET = 6,
    CP_LTE_SYS_ID_ULDFT = 7,
    CP_LTE_SYS_ID_PUSCH = 8,
    CP_LTE_SYS_ID_ULPCDCI = 9,
    CP_LTE_SYS_ID_DLFFT = 10,
    CP_LTE_SYS_ID_CSIRS = 11,
    CP_LTE_SYS_ID_LDTC1 = 16,
    CP_LTE_SYS_ID_RXCAPT = 32,
} CP_LTE_SYS_ID_T;

typedef enum
{
    GGE_SYS_APB_ID_0_RSV = 0,
    GGE_SYS_APB_ID_TCU_GSM = 1,
    GGE_SYS_APB_ID_TCU_NB = 2,
    GGE_SYS_APB_ID_BB_DMA = 3,
    GGE_SYS_APB_ID_4_RSV = 4,
    GGE_SYS_APB_ID_WDT = 5,
    GGE_SYS_APB_ID_GGE_IFC = 6,
    GGE_SYS_APB_ID_7_RSV = 7,
    GGE_SYS_APB_ID_8_RSV = 8,
    GGE_SYS_APB_ID_9_RSV = 9,
    GGE_SYS_APB_ID_10_RSV = 10,
    GGE_SYS_APB_ID_11_RSV = 11,
    GGE_SYS_APB_ID_12_RSV = 12,
    GGE_SYS_APB_ID_13_RSV = 13,
    GGE_SYS_APB_ID_RFSPI_GSM = 14,
    GGE_SYS_APB_ID_RFSPI_NB = 15,
} GGE_SYS_APB_ID_T;

typedef enum
{
    GGE_BB_APB_ID_XCOR = 0,
    GGE_BB_APB_ID_CORDIC = 1,
    GGE_BB_APB_ID_ITLV = 2,
    GGE_BB_APB_ID_VITAC = 3,
    GGE_BB_APB_ID_EXCOR = 4,
    GGE_BB_APB_ID_CHOLK = 5,
    GGE_BB_APB_ID_CIPHER = 6,
    GGE_BB_APB_ID_EVITAC = 7,
    GGE_BB_APB_ID_CP2 = 8,
    GGE_BB_APB_ID_BCPU_DBG = 9,
    GGE_BB_APB_ID_BCPU_CORE = 10,
    GGE_BB_APB_ID_ROM = 11,
    GGE_BB_APB_ID_RF_IF = 12,
    GGE_BB_APB_ID_IRQ = 13,
    GGE_BB_APB_ID_SYSCTRL = 14,
    GGE_BB_APB_ID_A53 = 15,
    GGE_BB_APB_ID_NB_CTRL = 16,
    GGE_BB_APB_ID_NB_COMMON = 17,
    GGE_BB_APB_ID_NB_INTC = 18,
    GGE_BB_APB_ID_NB_CELL_SEARCH = 19,
    GGE_BB_APB_ID_NB_FFT_RSRP = 20,
    GGE_BB_APB_ID_NB_VITERBI = 21,
    GGE_BB_APB_ID_NB_MEAS = 22,
    GGE_BB_APB_ID_NB_DS_BSEL = 23,
    GGE_BB_APB_ID_NB_TX_PUSCH_ENC = 24,
    GGE_BB_APB_ID_NB_TX_CHSC = 25,
    GGE_BB_APB_ID_NB_TX_FRONTEND = 26,
} GGE_BB_APB_ID_T;

typedef enum
{
    GGE_BB_IRQ_ID_TCU0_GSM = 0,
    GGE_BB_IRQ_ID_TCU1_GSM = 1,
    GGE_BB_IRQ_ID_FRAME_GSM = 2,
    GGE_BB_IRQ_ID_TCU0_NB = 3,
    GGE_BB_IRQ_ID_TCU1_NB = 4,
    GGE_BB_IRQ_ID_FRAME_NB = 5,
    GGE_BB_IRQ_ID_RFSPI_CONFLICT = 6,
    GGE_BB_IRQ_ID_A53 = 7,
    GGE_BB_IRQ_ID_RFIF_DBG_2G = 8,
    GGE_BB_IRQ_ID_RFIF_DBG_NB = 9,
    GGE_BB_IRQ_ID_RFIF_RX = 10,
    GGE_BB_IRQ_ID_RFIF_TX = 11,
    GGE_BB_IRQ_ID_EVITAC = 12,
    GGE_BB_IRQ_ID_EXCOR = 13,
    GGE_BB_IRQ_ID_ITLV = 14,
    GGE_BB_IRQ_ID_VITAC = 15,
    GGE_BB_IRQ_ID_XCOR = 16,
    GGE_BB_IRQ_ID_CHOLK = 17,
    GGE_BB_IRQ_ID_RF_SPI_GSM = 18,
    GGE_BB_IRQ_ID_MAILBOX = 19,
    GGE_BB_IRQ_ID_TCU_SYNC = 20,
    GGE_BB_IRQ_ID_NB_RX = 21,
    GGE_BB_IRQ_ID_NB_TX = 22,
    GGE_BB_IRQ_ID_NB_ACC = 23,
    GGE_BB_IRQ_ID_AIF0 = 24,
    GGE_BB_IRQ_ID_AIF1 = 25,
    GGE_BB_IRQ_ID_AIF2 = 26,
    GGE_BB_IRQ_ID_AIF3 = 27,
    GGE_BB_IRQ_ID_RF_SPI_NB = 28,
    GGE_BB_IRQ_ID_BB_DMA = 29,
} GGE_BB_IRQ_ID_T;

typedef enum
{
    RF_APB_ID_CTRL_1 = 0,
    RF_APB_ID_CTRL_2 = 1,
    RF_APB_ID_DFE = 2,
    RF_APB_ID_ET = 3,
    RF_APB_ID_RTC = 4,
    RF_APB_ID_SYS_CTRL = 5,
    RF_APB_ID_TIMER_1 = 6,
    RF_APB_ID_TIMER_2 = 7,
    RF_APB_ID_TIMER_3 = 8,
    RF_APB_ID_UART = 9,
    RF_APB_ID_WATCHDOG = 10,
    RF_APB_ID_TSEN_ADC = 11,
    RF_APB_ID_RFFE = 12,
} RF_APB_ID_T;

typedef enum
{
    WCN_SYS_APB_ID_SYS_CTRL = 0,
    WCN_SYS_APB_ID_DBM = 1,
    WCN_SYS_APB_ID_SYS_IFC = 2,
    WCN_SYS_APB_ID_BT_CORE = 3,
    WCN_SYS_APB_ID_UART = 4,
    WCN_SYS_APB_ID_RF_IF = 5,
    WCN_SYS_APB_ID_MODEM = 6,
    WCN_SYS_APB_ID_WLAN = 7,
    WCN_SYS_APB_ID_WDT = 8,
    WCN_SYS_APB_ID_TRAP = 9,
    WCN_SYS_APB_ID_SYSTICK = 10,
    WCN_SYS_APB_ID_COMREGS_WCN = 11,
    WCN_SYS_APB_ID_COMREGS_AP = 12,
} WCN_SYS_APB_ID_T;

#endif // _GLOBALS_H_
