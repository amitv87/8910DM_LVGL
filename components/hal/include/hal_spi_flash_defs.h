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

#ifndef _HAL_SPI_FLASH_DEFS_H_
#define _HAL_SPI_FLASH_DEFS_H_

#include "osi_api.h"
#include "hwregs.h"

#include "quec_proj_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HAL_SPI_FLASH_TYPE_UNKNOWN,
    HAL_SPI_FLASH_TYPE_GD,
    HAL_SPI_FLASH_TYPE_WINBOND,
    HAL_SPI_FLASH_TYPE_XTX,
    HAL_SPI_FLASH_TYPE_XMCA,
    HAL_SPI_FLASH_TYPE_XMCC,
    HAL_SPI_FLASH_TYPE_XMCB,
    HAL_SPI_FLASH_TYPE_PUYA,
} halSpiFlashType_t;

typedef enum
{
    HAL_SPI_FLASH_UID_NONE,
    HAL_SPI_FLASH_UID_4BH_8,
    HAL_SPI_FLASH_UID_4BH_16,
    HAL_SPI_FLASH_UID_SFDP_80H_12,
    HAL_SPI_FLASH_UID_SFDP_94H_16,
    HAL_SPI_FLASH_UID_SFDP_194H_16,
} halSpiFlashUidType_t;

typedef enum
{
    HAL_SPI_FLASH_CPID_NONE,
    HAL_SPI_FLASH_CPID_4BH, // byte [17:16]
} halSpiFlashCpidType_t;

typedef enum
{
    HAL_SPI_FLASH_WP_NONE,
    HAL_SPI_FLASH_WP_GD,   // GD compatible BP0/1/2/3/4/CMP
    HAL_SPI_FLASH_WP_XMCA, // XMCA compatible SR.2/3/4/5
} halSpiFlashWpType_t;

#define DELAY_AFTER_RESET (100)                  // us, tRST(20), tRST_E(12ms) won't happen
#define DELAY_AFTER_RELEASE_DEEP_POWER_DOWN (50) // us, tRES1(20), tRES2(20)

#define MID_MANUFACTURE(mid) ((mid)&0xff)
#define MID_MEMTYPE(mid) (((mid) >> 8) & 0xff)
#define MID_CAPBITS(mid) ((((mid) >> 16) & 0xff))

#ifdef CONFIG_QUEC_PROJECT_FEATURE
#define SIZE_4K (4096)
#define SHIFT_BIT_NUM_4K (12)   //4096=1<<12
#define SIZE_8K (8192)
#define SIZE_16K (16384)
#define SIZE_32K (32768)
#define SIZE_64K (65536)
#else
#define SIZE_4K (4 * 1024)
#define SIZE_8K (8 * 1024)
#define SIZE_16K (16 * 1024)
#define SIZE_32K (32 * 1024)
#define SIZE_64K (64 * 1024)
#endif
#define PAGE_SIZE (256)

// status register bits, based on GD
#define STREG_WIP REG_BIT(0)
#define STREG_WEL REG_BIT(1)

#define GD_SR_BP0 REG_BIT(2)
#define GD_SR_BP1 REG_BIT(3)
#define GD_SR_BP2 REG_BIT(4)
#define GD_SR_BP3 REG_BIT(5)
#define GD_SR_BP4 REG_BIT(6)
#define GD_SR_SRP0 REG_BIT(7)
#define GD_SR_SRP1 REG_BIT(8)
#define GD_SR_QE REG_BIT(9)
#define GD_SR_SUS2 REG_BIT(10)
#define GD_SR_LB1 REG_BIT(11)
#define GD_SR_LB2 REG_BIT(12)
#define GD_SR_LB3 REG_BIT(13)
#define GD_SR_CMP REG_BIT(14)
#define GD_SR_SUS1 REG_BIT(15)

#define XTX_SR_LB REG_BIT(10)

#define XMCA_SR_BP0 REG_BIT(2)
#define XMCA_SR_BP1 REG_BIT(3)
#define XMCA_SR_BP2 REG_BIT(4)
#define XMCA_SR_BP3 REG_BIT(5)
#define XMCA_SR_EBL REG_BIT(6) // Enable boot lock
#define XMCA_SR_SRP REG_BIT(7)

#define XMCA_SR_OTP_TB REG_BIT(3)
#define XMCA_SR_OTP_4KBL REG_BIT(4)
#define XMCA_SR_OTP_LOCK REG_BIT(7)

#define XMCB_SR_BP0 REG_BIT(2)
#define XMCB_SR_BP1 REG_BIT(3)
#define XMCB_SR_BP2 REG_BIT(4)
#define XMCB_SR_BP3 REG_BIT(5)
#define XMCB_SR_QE REG_BIT(6)
#define XMCB_SR_SRWD REG_BIT(7)

#define XMCB_FR_TBS REG_BIT(1)
#define XMCB_FR_PSUS REG_BIT(2)
#define XMCB_FR_ESUS REG_BIT(3)
#define XMCB_FR_IRL0 REG_BIT(4)
#define XMCB_FR_IRL1 REG_BIT(5)
#define XMCB_FR_IRL2 REG_BIT(6)
#define XMCB_FR_IRL3 REG_BIT(7)

#define BITSEL(sel, val) ((sel) ? (val) : 0)
#define GD_WP_BITEXP(n) (BITSEL((n)&1, GD_SR_BP0) |  \
                         BITSEL((n)&2, GD_SR_BP1) |  \
                         BITSEL((n)&4, GD_SR_BP2) |  \
                         BITSEL((n)&8, GD_SR_BP3) |  \
                         BITSEL((n)&16, GD_SR_BP4) | \
                         BITSEL((n)&32, GD_SR_CMP))

// This matches GD/WINBOND 4/8/16 MB flash. Though 8/16 MB flash
// support more options to open-protect smaller area at upper address.
// However, it is not worth to bringing this complexity.
#define GD_WP32M_MASK GD_WP_BITEXP(0b111111)
#define GD_WP32M_NONE GD_WP_BITEXP(0b000000)
#define GD_WP32M_4K GD_WP_BITEXP(0b011001)
#define GD_WP32M_8K GD_WP_BITEXP(0b011010)
#define GD_WP32M_16K GD_WP_BITEXP(0b011011)
#define GD_WP32M_32K GD_WP_BITEXP(0b011100)
#define GD_WP32M_1_64 GD_WP_BITEXP(0b001001)
#define GD_WP32M_1_32 GD_WP_BITEXP(0b001010)
#define GD_WP32M_1_16 GD_WP_BITEXP(0b001011)
#define GD_WP32M_1_8 GD_WP_BITEXP(0b001100)
#define GD_WP32M_1_4 GD_WP_BITEXP(0b001101)
#define GD_WP32M_1_2 GD_WP_BITEXP(0b001110)
#define GD_WP32M_3_4 GD_WP_BITEXP(0b100101)
#define GD_WP32M_7_8 GD_WP_BITEXP(0b100100)
#define GD_WP32M_15_16 GD_WP_BITEXP(0b100011)
#define GD_WP32M_31_32 GD_WP_BITEXP(0b100010)
#define GD_WP32M_63_64 GD_WP_BITEXP(0b100001)
#define GD_WP32M_ALL GD_WP_BITEXP(0b011111)

// This matches GD/WINBOND 2MB flash.
#define GD_WP16M_MASK GD_WP_BITEXP(0b111111)
#define GD_WP16M_NONE GD_WP_BITEXP(0b000000)
#define GD_WP16M_4K GD_WP_BITEXP(0b011001)
#define GD_WP16M_8K GD_WP_BITEXP(0b011010)
#define GD_WP16M_16K GD_WP_BITEXP(0b011011)
#define GD_WP16M_32K GD_WP_BITEXP(0b011100)
#define GD_WP16M_1_32 GD_WP_BITEXP(0b001001)
#define GD_WP16M_1_16 GD_WP_BITEXP(0b001010)
#define GD_WP16M_1_8 GD_WP_BITEXP(0b001011)
#define GD_WP16M_1_4 GD_WP_BITEXP(0b001100)
#define GD_WP16M_1_2 GD_WP_BITEXP(0b001101)
#define GD_WP16M_3_4 GD_WP_BITEXP(0b100100)
#define GD_WP16M_7_8 GD_WP_BITEXP(0b100011)
#define GD_WP16M_15_16 GD_WP_BITEXP(0b100010)
#define GD_WP16M_31_32 GD_WP_BITEXP(0b100001)
#define GD_WP16M_ALL GD_WP_BITEXP(0b011111)

// This matches GD/WINBOND 1MB flash.
#define GD_WP8M_MASK GD_WP_BITEXP(0b111111)
#define GD_WP8M_NONE GD_WP_BITEXP(0b000000)
#define GD_WP8M_4K GD_WP_BITEXP(0b011001)
#define GD_WP8M_8K GD_WP_BITEXP(0b011010)
#define GD_WP8M_16K GD_WP_BITEXP(0b011011)
#define GD_WP8M_32K GD_WP_BITEXP(0b011100)
#define GD_WP8M_1_16 GD_WP_BITEXP(0b001001)
#define GD_WP8M_1_8 GD_WP_BITEXP(0b001010)
#define GD_WP8M_1_4 GD_WP_BITEXP(0b001011)
#define GD_WP8M_1_2 GD_WP_BITEXP(0b001100)
#define GD_WP8M_3_4 GD_WP_BITEXP(0b100011)
#define GD_WP8M_7_8 GD_WP_BITEXP(0b100010)
#define GD_WP8M_15_16 GD_WP_BITEXP(0b100001)
#define GD_WP8M_ALL GD_WP_BITEXP(0b011111)

#define XMCA_WP_BITEXP(n) (BITSEL((n)&1, XMCA_SR_BP0) | \
                           BITSEL((n)&2, XMCA_SR_BP1) | \
                           BITSEL((n)&4, XMCA_SR_BP2) | \
                           BITSEL((n)&8, XMCA_SR_BP3))

#define XMCA_WP_MASK XMCA_WP_BITEXP(0b1111)
#define XMCA_WP_NONE XMCA_WP_BITEXP(0b0000)
#define XMCA_WP_1_128 XMCA_WP_BITEXP(0b0001)
#define XMCA_WP_2_128 XMCA_WP_BITEXP(0b0010)
#define XMCA_WP_4_128 XMCA_WP_BITEXP(0b0011)
#define XMCA_WP_8_128 XMCA_WP_BITEXP(0b0100)
#define XMCA_WP_16_128 XMCA_WP_BITEXP(0b0101)
#define XMCA_WP_32_128 XMCA_WP_BITEXP(0b0110)
#define XMCA_WP_64_128 XMCA_WP_BITEXP(0b0111)
#define XMCA_WP_96_128 XMCA_WP_BITEXP(0b1000)
#define XMCA_WP_112_128 XMCA_WP_BITEXP(0b1001)
#define XMCA_WP_120_128 XMCA_WP_BITEXP(0b1010)
#define XMCA_WP_124_128 XMCA_WP_BITEXP(0b1011)
#define XMCA_WP_126_128 XMCA_WP_BITEXP(0b1100)
#define XMCA_WP_127_128 XMCA_WP_BITEXP(0b1101)
#define XMCA_WP_ALL XMCA_WP_BITEXP(0b1111)

#define OPNAND_WRITE_ENABLE 0x06
#define OPNAND_WRITE_DISABLE 0x04
#define OPNAND_GET_FEATURE 0x0f
#define OPNAND_SET_FEATURE 0x1f
#define OPNAND_PAGE_READ_TO_CACHE 0x13
#define OPNAND_PAGE_READ 0x03 // or 0x0b
#define OPNAND_PAGE_READ_X4 0x6b
#define OPNAND_PAGE_READ_QUAD 0xeb
#define OPNAND_READ_ID 0x9f
#define OPNAND_PROGRAM_LOAD 0x02
#define OPNAND_PROGRAM_LOAD_X4 0x32
#define OPNAND_PROGRAM_EXEC 0x10
#define OPNAND_PROGRAM_LOAD_RANDOM 0x84
#define OPNAND_PROGRAM_LOAD_RANDOM_X4 0xc4 // or 0x34
#define OPNAND_PROGRAM_LOAD_RANDOM_QUAD 0x72
#define OPNAND_BLOCK_ERASE 0xd8
#define OPNAND_RESET 0xff

#ifdef __cplusplus
}
#endif
#endif
