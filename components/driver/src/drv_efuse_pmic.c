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

#include "hwregs.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_adi_bus.h"
#include <stdlib.h>
#include <string.h>
#include "hal_chip.h"
#include "hal_adi_bus.h"
#include "drv_efuse_pmic.h"

/*
	Bit define
*/
#define BIT_31 0x80000000
#define EFUSE_BLK_MAX_DATA 0x7FFFFFFF
#define EFUSE_LOCK_BIT (BIT_31)

#define EFUSE_READ_TIMEOUT 6000  //3ms
#define EFUSE_WRITE_TIMEOUT 6000 //3ms
#define PMIC2720_MAGIC_NUM (0x2720)

typedef enum
{
    EFUSE_PROG_MODE = 0,
    EFUSE_READ_MODE,
    EFUSE_NORMAL_RD_FLAG_CLR_MODE,
} EFUSE_MODE_CTL_E;

/* the function return value */
typedef enum
{
    EFUSE_RESULT_SUCCESS = 0x00,          //success to read & write
    EFUSE_READ_FAIL = 0x01,               //fail to read
    EFUSE_WRITE_FAIL = 0x02,              //fail to write
    EFUSE_WRITE_SOFT_COMPARE_FAIL = 0x03, //fail to write
    EFUSE_WRITE_VERIFY_FAIL = 0x04,
    EFUSE_PARAMETER_ERROR = 0x05,
    EFUSE_ID_ERROR = 0x06, //block id is invalid
    EFUSE_HAS_WRITEED_ERROR = 0x07,
    EFUSE_PTR_NULL_ERROR = 0x08,               //pointer is NULL
    EFUSE_PTR_NUM_ERROR = 0x09,                //bit length error
    EFUSE_WRITE_HARD_COMPARE_FAIL = 0x0A,      //fail to write data
    EFUSE_WRITE_SOFT_HARD_COMPARE_FAIL = 0x0B, //fail to write
    EFUSE_BIST_TEST_FAIL = 0x0C,               //efuse data is not zero
    EFUSE_BIST_TIMEOUT = 0x0D,
    EFUSE_NOT_LOCK = 0x0E, //could be written again
    EFUSE_LOCKED = 0x0F    //forbidden to be written again
} EFUSE_RETURN_E;

typedef enum
{
    EFUSE_MIN_ID,
    EFUSE_HASH_START_ID,
    EFUSE_HASH_END_ID,
    EFUSE_IMEI_START_ID,
    EFUSE_IMEI_END_ID,
    EFUSE_UID_START_ID,
    EFUSE_UID_END_ID,
    EFUSE_CAL_ID, //calibaration and CCCV ID
    EFUSE_LDOA_REF_CAL_ID,
    EFUSE_LDOB_REF_CAL_ID,
    EFUSE_LDODCXO_REF_CAL_ID,
    EFUSE_ADC_CAL_HEADMIC_ID,
    EFUSE_ADC_CAL_BIG_ID,
    EFUSE_ADC_CAL_SMALL_START_ID,
    EFUSE_ADC_CAL_SMALL_END_ID,

    EFUSE_ADC_CAL_SMALL_ID,

    EFUSE_BT_CAL_ID,
    EFUSE_AUD_CAL_ID,
    EFUSE_RC_CAL_ID, //calibration RC1M
    EFUSE_IB_CAL_ID,
    EFUSE_CHRG_CV_CAL_ID,
    EFUSE_LDOCORE_SLP_CAL_ID,
    EFUSE_CPLL_LDO_CAL_ID,
    EFUSE_RG_LDO_RTC_CAL_ID,
    EFUSE_MAX_ID
} EFUSE_BLK_FUN_INDEX;

typedef enum
{
    EFUSE_PHY_MIN_ID = 0,
    EFUSE_PHY_BLK_ID_0 = EFUSE_PHY_MIN_ID, /* DCDC CLK_3M */
    EFUSE_PHY_BLK_ID_1,                    /* DCDC CLK_3M */
    EFUSE_PHY_BLK_ID_2,                    /* ADI option */
    EFUSE_PHY_BLK_ID_3,                    /* Fuel gage 的单点校准值 */
    EFUSE_PHY_BLK_ID_4,
    EFUSE_PHY_BLK_ID_5, /* LDOs参考电压校准 */
    EFUSE_PHY_BLK_ID_6,
    EFUSE_PHY_BLK_ID_7,
    EFUSE_PHY_BLK_ID_8,
    EFUSE_PHY_BLK_ID_9,
    EFUSE_PHY_BLK_ID_10,
    EFUSE_PHY_BLK_ID_11,
    EFUSE_PHY_BLK_ID_12, /* RGB Current driver内部电阻 */
    EFUSE_PHY_BLK_ID_13, /* CHG CC-CV的校准值6bit CHG_CC-CV<5:0>  */
    EFUSE_PHY_BLK_ID_14, /* Effuse THM校准 */
    EFUSE_PHY_BLK_ID_15, /* DCDC PFM threshold, ZX offset threshold */
    EFUSE_PHY_BLK_ID_16, /* RTC_LDO校准值 */
    EFUSE_PHY_BLK_ID_17, /* AUXADC offset calibration*/
    EFUSE_PHY_BLK_ID_18, /* AuxADC 的校准 大量程校准电压点为4.2V和3.6V */
    EFUSE_PHY_BLK_ID_19, /* AuxADC 的校准 小量程 电压为0.1V和1.0V */
    EFUSE_PHY_BLK_ID_20, /* 小量程的校准电压Headmic */
    EFUSE_PHY_BLK_ID_21, /* Audio Ibias/PA OVP/LDO VB calibration */
    EFUSE_PHY_BLK_ID_22, /* Headmicbias calibration */
    EFUSE_PHY_BLK_ID_23,
    EFUSE_PHY_BLK_ID_24,
    EFUSE_PHY_BLK_ID_25,
    EFUSE_PHY_BLK_ID_26, /* sleep DCDC CORE */
    EFUSE_PHY_BLK_ID_27, /* normal DCDC CORE */
    EFUSE_PHY_BLK_ID_28,
    EFUSE_PHY_BLK_ID_29, /* DCDC CORE REFERENCE */
    EFUSE_PHY_BLK_ID_30, /* DCDC GEN */
    EFUSE_PHY_BLK_ID_31, /* DCDC WPA */
    EFUSE_PHY_MAX_ID = EFUSE_PHY_BLK_ID_31,
    EFUSE_PHY_BLK_SUM = EFUSE_PHY_MAX_ID + 1
} EFUSE_BLOCK_ID_E;

static uint32_t s_efuse_blks_data[EFUSE_PHY_BLK_SUM] = {0};

/******************************************************************************
 * [Description] :                                                            *
 *      set block id for "reading" / "writing" efuse block                    *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 ******************************************************************************/
static void _efuseConfigBlkId(uint32_t block_id)
{
    REG_RDA2720M_EFS_EFUSE_ADDR_INDEX_T addr_index;
    REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_addr_index, addr_index, efuse_addr_index, block_id);
}

/******************************************************************************
 * [Description] :                                                            *
 *      set data that is going to be write to an efuse block                  *
 * [Global resource dependence]:                                              *
 *      data: data that is to be written to the respective EFuse block        *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 ******************************************************************************/
static void _efuseConfigWriteData(uint32_t data)
{
    REG_RDA2720M_EFS_EFUSE_DATA_WR_T data_wr;
    REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_data_wr, data_wr, efuse_data_wr, data);
}

/******************************************************************************
 * [Description] :                                                            *
 *      send program/read/standy/bist start commmand                          *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *   EFUSE_PROG_MODE : efuse program start command                            *
 *   EFUSE_READ_MODE : efuse read start command                               *
 *   EFUSE_STANDBY_MODE : efuse standy by start command                       *
 *   EFUSE_BIST_MODE : efuse bist start command                               *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 ******************************************************************************/
static void _efuseConfigMode(EFUSE_MODE_CTL_E mode)
{
    REG_RDA2720M_EFS_EFUSE_MODE_CTRL_T mode_ctrl;

    switch (mode)
    {
    case EFUSE_PROG_MODE:
        REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_mode_ctrl, mode_ctrl, efuse_pg_start, 1);
        break;
    case EFUSE_READ_MODE:
        REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_mode_ctrl, mode_ctrl, efuse_rd_start, 1);
        break;
    case EFUSE_NORMAL_RD_FLAG_CLR_MODE:
        REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_mode_ctrl, mode_ctrl, efuse_normal_rd_flag_clr, 1);
        break;
    default:
        break;
    }
}

static void _efuseEnalePgm(void)
{
    REG_RDA2720M_EFS_EFUSE_GLB_CTRL_T glb_ctrl;
    REG_ADI_CHANGE1(hwp_rda2720mEfs->efuse_glb_ctrl, glb_ctrl, efuse_pgm_enable, 1);
}

/******************************************************************************
 * [Description] :                                                            *
 *      enable/disable magic number prot register                             *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      is_enable: enable/disable magic protect                               *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 ******************************************************************************/
static void _efuseEnableMagicNumProt(
    bool is_enable)
{
    REG_RDA2720M_EFS_EFUSE_MAGIC_NUMBER_T magic;
    if (is_enable)
    {
        REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_magic_number, magic, efuse_magin_magic_number, 0);
    }
    else
    {
        REG_ADI_WRITE1(hwp_rda2720mEfs->efuse_magic_number, magic, efuse_magin_magic_number, PMIC2720_MAGIC_NUM);
    }
}

/******************************************************************************
 * [Description] :                                                            *
 *      Get mask data of the bits                                             *
 *      eg:                                                                   *
 *            input         output                                            *
 *              0            0x0                                              *
 *              1            0x01                                             *
 *              2            0x03                                             *
 *              3            0x07                                             *
 *             ...           ...                                              *
 *             ...           ...                                              *
 *             ...           ...                                              *
 *             31        0x7FFF_FFFF                                          *
 *             32        0xFFFF_FFFF                                          *
 * [Global resource dependence]:                                              *
 *      length : number of bits                                               *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 *      max blocks used to stored som bits                                    *
 * [Note]:                                                                    *
 *    1.Each block has 31-bit data.                                           *
 ******************************************************************************/
static uint32_t _efuseGetBitMaskData(
    uint32_t bit_num)
{
    uint32_t sum = 0;

    if (bit_num >= 32)
    {
        sum = 0xFFFFFFFF;
    }
    else
    {
        while (bit_num)
        {
            sum = sum << 1;
            sum += 0x1;
            bit_num--;
        }
    }

    return sum;
}

/******************************************************************************
 * [Description] :                                                            *
 *      read data directly from an EFuse block                                *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 *      read efuse data including lock bit                                    *
 ******************************************************************************/
static uint16_t _efuseGetReadData(void)
{
    return halAdiBusRead(&hwp_rda2720mEfs->efuse_data_rd);
}

/******************************************************************************
 * [Description] :                                                            *
 *      program/read/standy/bist start busy status                            *
 *      if efuse is busy, return true.                                        *
 *      if efuse is idle , return false.                                      *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *   EFUSE_PROG_MODE    : efuse program busy status                           *
 *   EFUSE_READ_MODE    : efuse read start command                            *
 *   EFUSE_STANDBY_MODE : efuse standy by start command                       *
 *   EFUSE_BIST_MODE    : efuse bist start command                            *
 * [Return]:                                                                  *
 *   true  : busy                                                             *
 *   false : idle                                                             *
 * [Note]:                                                                    *
 ******************************************************************************/
static bool _efuseIsModeBusy(
    EFUSE_MODE_CTL_E mode)
{
    REG_RDA2720M_EFS_EFUSE_STATUS_T status = {};
    status.v = halAdiBusRead(&hwp_rda2720mEfs->efuse_status);
    uint32_t reg_value;
    bool is_busy;

    switch (mode)
    {
    case EFUSE_PROG_MODE:
        reg_value = status.b.pgm_busy;
        break;
    case EFUSE_READ_MODE:
        reg_value = status.b.read_busy;
        break;
    case EFUSE_NORMAL_RD_FLAG_CLR_MODE:
        reg_value = status.b.efuse_normal_rd_done;
        break;

    default:
        reg_value = 0;
        break;
    }

    if (reg_value)
    {
        is_busy = true;
    }
    else
    {
        is_busy = false;
    }

    return is_busy;
}

static bool _efuseIsBusy(void)
{
    bool is_busy = true;
    REG_RDA2720M_EFS_EFUSE_STATUS_T status = {};
    status.v = halAdiBusRead(&hwp_rda2720mEfs->efuse_status);

    if (status.b.standby_busy)
    {
        is_busy = false;
    }

    return is_busy;
}

static bool _efuseIsReadDone(void)
{
    bool readDone = false;
    REG_RDA2720M_EFS_EFUSE_STATUS_T status = {};
    status.v = halAdiBusRead(&hwp_rda2720mEfs->efuse_status);

    if (status.b.efuse_normal_rd_done)
    {
        readDone = true;
    }

    return readDone;
}

/******************************************************************************
 * [Description] :                                                            *
 *      Copy num of bit to an array                                           *
 * [Global resource dependence]:                                              *
 *      src_ptr: the pointer of source                                        *
 *      des_ptr: the pointer of destination                                   *
 *      bit_num: number of bits                                               *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 ******************************************************************************/
static void _efuseSplitDataCopy(
    uint8_t *src_ptr,
    uint8_t *des_ptr,
    uint32_t bit_num)
{
    uint32_t len, remainder;
    uint32_t i;
    uint32_t divider;

    divider = 8; //len of uint8_t
    len = (bit_num + (divider - 1)) / divider;
    remainder = bit_num % divider;

    if (src_ptr != des_ptr)
    {
        for (i = 0; i < len; i++)
        {
            if ((i == (len - 1)) && (0 != remainder))
            {
                remainder = _efuseGetBitMaskData(remainder);
                *(des_ptr + i) = *(src_ptr + i) & remainder;
            }
            else
            {
                *(des_ptr + i) = *(src_ptr + i);
            }
        }
    }
    else
    {
        if (0 != remainder)
        {
            remainder = _efuseGetBitMaskData(remainder);
            *(des_ptr + len - 1) = *(src_ptr + len - 1) & remainder;
        }
    }
}

static int32_t _getTickCount(void)
{
    return hwp_timer4->hwtimer_curval_l;
}

/******************************************************************************
 * [Description] :                                                            *
 *      check efuse block id                                                  *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      block_id : id number of the selected efuse block                      *
 * [Return]:                                                                  *
 *      true : id is correct.                                                 *
 *      false: id is wrong                                                    *
 * [Note]:                                                                    *
 ******************************************************************************/
static bool _efuseIsBlkIdValid(
    uint32_t block_id)
{
    bool is_valid = true;

    if (block_id >= EFUSE_PHY_BLK_SUM)
    {
        is_valid = false;
    }

    return is_valid;
}
/******************************************************************************
 * [Description] :                                                            *
 *      Get max bits of the selected efuse blocks, and these blocks are used  *
 *      to store hash/imei                                                    *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 *      max bits of HASH/IMEI blocks                                          *
 * [Note]:                                                                    *
 *    1.Hash block is several efuse block, and there index of block are       *
 *      consecutive.                                                          *
 *    2.Each block has 31-bit data.                                           *
 ******************************************************************************/
static uint32_t _efuseGetAppBitLen(
    EFUSE_BLK_FUN_INDEX blk_hal)
{
    uint32_t min_id = 0;
    uint32_t max_id = 0;

    switch (blk_hal)
    {
    case EFUSE_IMEI_START_ID:
        min_id = EFUSE_PHY_BLK_ID_10;
        max_id = EFUSE_PHY_BLK_ID_13;
        break;
    case EFUSE_UID_START_ID:
        min_id = EFUSE_PHY_BLK_ID_0;
        max_id = EFUSE_PHY_BLK_ID_1;
        break;
    case EFUSE_HASH_START_ID:
        min_id = EFUSE_PHY_BLK_ID_9;
        max_id = EFUSE_PHY_BLK_ID_13;
        break;
    case EFUSE_MAX_ID:
        min_id = EFUSE_PHY_MIN_ID;
        max_id = EFUSE_PHY_MAX_ID;
        break;
    default:
        break;
    }

    return (uint32_t)((max_id - min_id + 1) * 31);
}

/******************************************************************************
 * [Description] :                                                            *
 *      parameter is efuse block index, and return efuse block id number      *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:return efuse phy id according to hash id                            *
 *        if fun_index is invalid, the return id is invalid block id.         *
 ******************************************************************************/
static uint32_t _efuseGetBlkId(
    EFUSE_BLK_FUN_INDEX fun_index)
{
    uint32_t phy_blk_id = EFUSE_PHY_BLK_SUM; //initial a invalid data

    switch (fun_index)
    {
        /*
            block 0~1:      uid for first 2 block,
            block 2~8:      calibration for 7 block,
            blcok 9~13:     hash value for 5 block,
            blcok 14~15:    reserved.
         */
    case EFUSE_MIN_ID:
        phy_blk_id = EFUSE_PHY_MIN_ID;
        break;
    case EFUSE_UID_START_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_0;
        break;
    case EFUSE_UID_END_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_1;
        break;
    case EFUSE_HASH_START_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_9;
        break;
    case EFUSE_IMEI_START_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_10;
        break;
    case EFUSE_HASH_END_ID:
    case EFUSE_IMEI_END_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_13;
        break;
    case EFUSE_CPLL_LDO_CAL_ID:
    case EFUSE_BT_CAL_ID:
    case EFUSE_IB_CAL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_3;
        break;

    case EFUSE_ADC_CAL_HEADMIC_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_4;
        break;

    case EFUSE_ADC_CAL_BIG_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_18;
        break;
    case EFUSE_CHRG_CV_CAL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_13;
        break;

    case EFUSE_ADC_CAL_SMALL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_19;
        break;

    case EFUSE_LDOB_REF_CAL_ID:
    case EFUSE_LDODCXO_REF_CAL_ID:
    case EFUSE_LDOA_REF_CAL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_5;
        break;

    case EFUSE_RC_CAL_ID:
    case EFUSE_AUD_CAL_ID:
    case EFUSE_LDOCORE_SLP_CAL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_6;
        break;

    case EFUSE_RG_LDO_RTC_CAL_ID:
        phy_blk_id = EFUSE_PHY_BLK_ID_16;
        break;

    case EFUSE_MAX_ID:
        phy_blk_id = EFUSE_PHY_MAX_ID;
        break;
    default:
        break;
    }

    return phy_blk_id;
}

/******************************************************************************
 * [Description] :                                                            *
 *      Get max blocks of the bits, and 31 bit is stored in a block           *
 * [Global resource dependence]:                                              *
 *      length : number of bits                                               *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 *      max blocks used to stored som bits                                    *
 * [Note]:                                                                    *
 *    1.Each block has 31-bit data.                                           *
 ******************************************************************************/
static uint32_t _efuseGetBlkNumFromBit(
    uint32_t length)
{
    uint32_t blk_num;

    if (length % 31)
    {
        blk_num = (length / 31) + 1;
    }
    else
    {
        blk_num = length / 31;
    }

    return (blk_num);
}

/******************************************************************************
 * [Description] :                                                            *
 *      Efuse applys an array. This function returns its address              *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 *      address of an uint32_t array, and its lenght is equal to num of blocks  *
 * [Note]:                                                                    *
 *   1. If efuse has 8 blocks, and each block has 32-bit length. So length of *
 *      the efuse array is 8.                                                 *
 ******************************************************************************/
static uint32_t *_efuseGetEfusePtr(void)
{
    return s_efuse_blks_data;
}

/******************************************************************************
 * [Description] :                                                            *
 *      Reading data is 32-bit length. But each block is 31-bit length.       *
 *      Each block is only one writting operation. For using all 31-bit       *
 *      data, transform 32-bit to 31-bit, and each 31-bit data is to be       *
 *      written for an block.                                                 *
 *  eg: Reading data is a pointer, and its lenght is 4.                       *
 *      Its valid bit are 31*4. Bit 124~128 would be neglected.               *
 *       original       result                                                *
 *      [31 : 0 ]      [30 : 0 ]                                              *
 *      [63 : 32]      [61 :31 ]                                              *
 *      [95 : 64]      [92 :62 ]                                              *
 *      [127: 96]      [123:93 ]                                              *
 *                                                                            *
 * [Global resource dependence]:                                              *
 *      s_efuse_blks_data: reading or writting data of efuse blocks           *
 * [Parameters]:                                                              *
 *      src_dat_ptr: the pointer of original data                             *
 *      des_dat_ptr: the pointer of result data                               *
 *      len_id     : max number of blocks                                     *
 *      blk_num    : max length of bits                                       *
 *      is_split   : direction of splitting data                              *
 *                   true : transforom original data to 31-bit block data     *
 *                   false: transform block data to original data             *
 * [Return]:                                                                  *
 *      If direction is from orginal to result, pointer of 31-bit result is   *
 *      returned. And if it is from result to orignal data, pointer of orignal*
 *      data is returned.                                                     *
 * [Note]:                                                                    *
 *    1.If length of reading data is larger than limit, NULL is returned.     *
 *    2.Reading data would not be changed. And result is stored in return.    *
 ******************************************************************************/
static bool _efuseSplitData(
    uint8_t *src_dat_ptr,
    uint8_t *des_dat_ptr,
    uint32_t blk_num,
    uint32_t bit_num,
    bool is_split)
{
    uint32_t i;
    uint32_t len;
    uint32_t *src_ptr;
    uint32_t *des_ptr;
    uint32_t divider;

    if (blk_num > EFUSE_PHY_BLK_SUM) //max block number
    {
        return false;
    }

    src_ptr = (uint32_t *)src_dat_ptr;
    des_ptr = (uint32_t *)des_dat_ptr;
    divider = 31;
    len = (bit_num + divider - 1) / divider;

    if (is_split)
    {
        /* true branch: 32-bit to 31-bit data */

        _efuseSplitDataCopy(src_dat_ptr, des_dat_ptr, bit_num);

        for (i = len - 1; i < len; i--)
        {
            if (i == 0)
            {
                *(des_ptr) = *(des_ptr)&EFUSE_BLK_MAX_DATA;
            }
            else
            {
                *(des_ptr + i) = ((*(des_ptr + i) & (EFUSE_BLK_MAX_DATA >> i)) << i) + (*(des_ptr + i - 1) >> (divider - i + 1));
            }
        }
    }
    else
    {
        /* false branch: 31-bit to 32-bit data */

        for (i = 0; i < len; i++)
        {
            if (len - 1 == i)
            {
                *(src_ptr + i) = *(src_ptr + i) >> i;
            }
            else
            {
                *(src_ptr + i) = (*(src_ptr + i) >> i) + (*(src_ptr + i + 1) << (divider - i));
            }
        }

        _efuseSplitDataCopy(src_dat_ptr, des_dat_ptr, bit_num);
    }

    return true;
}

/******************************************************************************
 * [Description] :                                                            *
 *      If an efuse block has not been written before, it is not locked.      *
 *      But if it has been written before, it is locke.                       *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      block_id : id number of the selected block                            *
 * [Return]:                                                                  *
 *      EFUSE_LOCKED    : block is protected                                  *
 *      EFUSE_NOT_LOCK  : block is not protected                              *
 * [Note]:                                                                    *
 *    1.If an efuse block is written, it would be protected at once.          *
 *    2.If an efuse block is protected, it would not be written any more.     *
 *    3.Each efuse block has only one writting operation.                     *
 ******************************************************************************/
static EFUSE_RETURN_E _efuseIsLock(
    uint32_t block_id)
{
    uint32_t is_busy_sts = 0;
    uint32_t old_tick = 0;
    uint32_t new_tick = 0;
    EFUSE_RETURN_E result;

    result = EFUSE_RESULT_SUCCESS;

    _efuseConfigBlkId(block_id);
    _efuseConfigMode(EFUSE_READ_MODE);

    is_busy_sts = _efuseIsModeBusy(EFUSE_READ_MODE);

    old_tick = _getTickCount();

    while (true == is_busy_sts)
    {
        is_busy_sts = _efuseIsModeBusy(EFUSE_READ_MODE);
        new_tick = _getTickCount();

        if ((new_tick - old_tick) > EFUSE_READ_TIMEOUT)
        {
            break;
        }
    }

    if (true == is_busy_sts)
    {
        result = EFUSE_LOCKED;
    }
    else
    {
        if (0 == (EFUSE_LOCK_BIT & _efuseGetReadData()))
        {
            result = EFUSE_NOT_LOCK;
        }
        else
        {
            result = EFUSE_LOCKED;
        }
    }

    return result;
}

/******************************************************************************
 * [Description] :                                                            *
 *      read data from the selected efuse block                               *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      block_id : id number of the selected bllock                           *
 *      des_data_ptr : the pointer of data reading from the selected block    *
 * [Return]:                                                                  *
 *      EFUSE_READ_FAIL        : fail to read data                            *
 *      EFUSE_RESULT_SUCCESS   : successful to read data                      *
 * [Note]:                                                                    *
 *    1.Return is the status of reading operation, such as successful or fail.*
 *    2.Data of the selected efuse block is stored in the 2rd parameter.      *
 *    3.Each block has 31-bit data.                                           *
 ******************************************************************************/
static EFUSE_RETURN_E _drvEfusePmicReadEx(
    uint32_t block_id,
    uint32_t *des_data_ptr)
{
    uint32_t is_busy_sts = 0;
    uint32_t old_tick = 0;
    uint32_t new_tick = 0;
    EFUSE_RETURN_E result;

    result = EFUSE_RESULT_SUCCESS;

    _efuseConfigBlkId(block_id);
    _efuseConfigMode(EFUSE_READ_MODE);

    is_busy_sts = _efuseIsModeBusy(EFUSE_READ_MODE);

    old_tick = _getTickCount();

    while (true == is_busy_sts)
    {
        is_busy_sts = _efuseIsModeBusy(EFUSE_READ_MODE);
        new_tick = _getTickCount();

        if ((new_tick - old_tick) > EFUSE_READ_TIMEOUT)
        {
            break;
        }
    }

    if (true == is_busy_sts)
    {
        result = EFUSE_READ_FAIL;
    }
    else
    {
        *des_data_ptr = _efuseGetReadData() & EFUSE_BLK_MAX_DATA;
    }

    return result;
}

/******************************************************************************
 * [Description] :                                                            *
 *      EFuse open operation                                            *
 * [Global resource dependence]:                                              *
 *      s_vcama_org                                                           *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 *      power on effuse; open efuse clk; Enable program                       *
 ******************************************************************************/
void drvEfusePmicOpen(void)
{
    REG_RDA2720M_GLOBAL_MODULE_EN0_T moudle_en0;
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, moudle_en0, efs_en, 1);
}

/******************************************************************************
 * [Description] :                                                            *
 *      EFuse close operation                                            *
 * [Global resource dependence]:                                              *
 *      s_vcama_org                                                           *
 * [Parameters]:                                                              *
 * [Return]:                                                                  *
 * [Note]:                                                                    *
 *      power on effuse; open efuse clk; Enable program                       *
 ******************************************************************************/
void drvEfusePmicClose(void)
{
    REG_RDA2720M_GLOBAL_MODULE_EN0_T moudle_en0;
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, moudle_en0, efs_en, 0);
}

/******************************************************************************
 * [Description] :                                                            *
 *      read data from the selected efuse block                               *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      block_id : id number of the selected bllock                           *
 *      des_data_ptr : the pointer of data reading from the selected block    *
 * [Return]:                                                                  *
 *      EFUSE_READ_FAIL        : fail to read data                            *
 *      EFUSE_RESULT_SUCCESS   : successful to read data                      *
 * [Note]:                                                                    *
 *    1.Return is the status of reading operation, such as successful or fail.*
 *    2.Data of the selected efuse block is stored in the 2rd parameter.      *
 *    3.Each block has 31-bit data.                                           *
 ******************************************************************************/
uint32_t drvEfusePmicRead(uint32_t block_id, uint32_t *des_data_ptr)
{
    if (NULL == des_data_ptr)
    {
        return EFUSE_PTR_NULL_ERROR;
    }

    if (!_efuseIsBlkIdValid(block_id))
    {
        return EFUSE_ID_ERROR;
    }

    uint32_t is_busy_sts = 0;
    uint32_t old_tick = 0;
    uint32_t new_tick = 0;
    EFUSE_RETURN_E result;
    bool read_done = false;

    result = EFUSE_RESULT_SUCCESS;

    is_busy_sts = _efuseIsBusy();

    old_tick = _getTickCount();

    while (true == is_busy_sts)
    {
        is_busy_sts = _efuseIsBusy();
        new_tick = _getTickCount();

        if ((new_tick - old_tick) > EFUSE_READ_TIMEOUT)
        {
            break;
        }
    }

    if (true == is_busy_sts)
    {
        OSI_LOGE(0, "drvEfusePmicRead READ BUSY");
        return EFUSE_READ_FAIL;
    }

    _efuseConfigBlkId(block_id);

    _efuseConfigMode(EFUSE_READ_MODE);

    old_tick = _getTickCount();
    new_tick = old_tick;

    read_done = _efuseIsReadDone();
    while ((false == read_done) && ((new_tick - old_tick) < EFUSE_READ_TIMEOUT))
    {
        read_done = _efuseIsReadDone();
        new_tick = _getTickCount();
    }

    if (false == read_done)
    {
        OSI_LOGE(0, "drvEfusePmicRead READ_DONE Error");
        return EFUSE_READ_FAIL;
    }

    *des_data_ptr = _efuseGetReadData();

    _efuseConfigMode(EFUSE_NORMAL_RD_FLAG_CLR_MODE);

    return result;
}
/******************************************************************************
 * [Description] :                                                            *
 *      write data to the selected efuse block                                *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      block_id : id number of the selected block                            *
 *      write_data : the pointer of data that is to be written                *
 * [Return]:                                                                  *
 *      EFUSE_RESULT_SUCCESS          : successful to write data, and data of *
 *                                      the block is equal to writting data   *
 *      EFUSE_WRITE_SOFT_COMPARE_FAIL : successful to write data, but data of *
 *                                      the block is not equal to writting    *
 *                                      data                                  *
 *      EFUSE_WRITE_HARD_COMPARE_FAIL : fail to write data                    *
 *      EFUSE_WRITE_SOFT_HARD_COMPARE_: fail to write data, and data of the   *
 *                                      block is not equal to writting data   *
 *      EFUSE_WRITE_VERIFY_FAIL       : fail to read after write              *
 * [Note]:                                                                    *
 *    1.Return is the status of reading operation, such as successful or fail.*
 *    2.Each block is only one writing operation.                             *
 *    3.Each block has 31-bit data.                                           *
 ******************************************************************************/
uint32_t drvEfusePmicWrite(uint32_t block_id, uint32_t write_data)
{
    uint32_t sts = 0;
    uint32_t read_data;
    uint32_t old_tick = 0;
    uint32_t new_tick = 0;
    EFUSE_RETURN_E result;

    result = EFUSE_RESULT_SUCCESS;
    return result;
    _efuseConfigBlkId(block_id);

    _efuseConfigWriteData(write_data);

    _efuseEnalePgm();

    _efuseEnableMagicNumProt(false);
    _efuseConfigMode(EFUSE_PROG_MODE);

    sts = _efuseIsModeBusy(EFUSE_PROG_MODE);

    old_tick = _getTickCount();

    while (true == sts)
    {
        sts = _efuseIsModeBusy(EFUSE_PROG_MODE);
        new_tick = _getTickCount();

        if ((new_tick - old_tick) > EFUSE_WRITE_TIMEOUT)
        {
            break;
        }
    }

    if (true == sts)
    {
        OSI_LOGE(0, "drvEfusePmicWrite WRITE BUSY");
        return EFUSE_WRITE_FAIL;
    }
    else
    {
        /* read data and compare */
        result = drvEfusePmicRead(block_id, &read_data);

        if (EFUSE_RESULT_SUCCESS != result)
        {
            result = EFUSE_WRITE_VERIFY_FAIL;
        }
        else
        {
            /* compare write and read data */
            if (write_data == read_data)
            {
                result = EFUSE_RESULT_SUCCESS;
            }
            else
            {
                result = EFUSE_WRITE_SOFT_COMPARE_FAIL;
            }
        }
    }
    return result;
}

/******************************************************************************
 * [Description] :                                                            *
 *      Read data from these selected efuse blocks.And these blocks are used  *
 *      to store IMEA.                                                        *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      des_data_ptr : the pointer of data reading from the selected block    *
 *      bit_num      : number of bit                                          *
 * [Return]:                                                                  *
 *      EFUSE_PTR_NULL_ERROR   : pointer is NULL                              *
 *      EFUSE_PTR_NUM_ERROR    : num of bit is larger than limit              *
 *      EFUSE_ID_ERROR  : bit max length used for IMEI is less than 1st       *
 *                        parameter                                           *
 *      EFUSE_READ_FAIL        : fail to read data                            *
 *      EFUSE_RESULT_SUCCESS   : successful to read data                      *
 * [Note]:                                                                    *
 *    1.Return is the status of reading operation, such as successful or fail.*
 *    2.Data of the selected efuse block is stored in the 2rd parameter.      *
 *    3.Hash block is several efuse block, and there index of block are       *
 *      consecutive.                                                          *
 ******************************************************************************/
uint32_t drvEfuseReadImei(uint8_t *des_data_ptr, uint32_t bit_num)
{
    uint32_t i, min_id, len;
    uint32_t max_bit;
    uint32_t *efuse_ptr, *temp_ptr;
    EFUSE_RETURN_E result;

    if (NULL == des_data_ptr)
    {
        return EFUSE_PTR_NULL_ERROR;
    }

    result = EFUSE_RESULT_SUCCESS;
    max_bit = _efuseGetAppBitLen(EFUSE_IMEI_START_ID);
    min_id = _efuseGetBlkId(EFUSE_IMEI_START_ID);
    len = _efuseGetBlkNumFromBit(bit_num);
    efuse_ptr = _efuseGetEfusePtr();
    temp_ptr = efuse_ptr;

    if (max_bit < bit_num)
    {
        return EFUSE_PTR_NUM_ERROR;
    }

    for (i = min_id; i < (min_id + len); i++)
    {
        result = _drvEfusePmicReadEx(i, temp_ptr);

        if (EFUSE_RESULT_SUCCESS != result)
        {
            return result;
        }

        temp_ptr++;
    }

    if (false == _efuseSplitData((uint8_t *)efuse_ptr, des_data_ptr, len, bit_num, false))
    {
        result = EFUSE_READ_FAIL;
    }

    return result;
}

/******************************************************************************
 * [Description] :                                                            *
 *      Write data to these selected blocks.And these blocks are used to      *
 *      store IMEA                                                            *
 * [Global resource dependence]:                                              *
 * [Parameters]:                                                              *
 *      src_dat_ptr : the pointer of data that is to be written               *
 *      bit_num     : number of bit                                           *
 * [Return]:                                                                  *
 *      EFUSE_PTR_NULL_ERROR          : pointer is NULL                       *
 *      EFUSE_PTR_NUM_ERROR           : num of bit is larger than limit       *
 *      EFUSE_LOCKED                  : block is protected                    *
 *      EFUSE_WRITE_FAIL              : write fail                            *
 *      EFUSE_RESULT_SUCCESS          : successful to write data, and data of *
 *                                      the block is equal to writting data   *
 *      EFUSE_WRITE_SOFT_COMPARE_FAIL : successful to write data, but data of *
 *                                      the block is not equal to writting    *
 *                                      data                                  *
 *      EFUSE_WRITE_HARD_COMPARE_FAIL : fail to write data                    *
 *      EFUSE_WRITE_SOFT_HARD_COMPARE_: fail to write data, and data of the   *
 *                                      block is not equal to writting data   *
 *      EFUSE_WRITE_VERIFY_FAIL       : fail to read after write              *
 * [Note]:                                                                    *
 *    1.Return is the status of reading operation, such as successful or fail.*
 *    2.Each block is only one writing operation.                             *
 *    3.Hash block is several efuse block, and there index of block are       *
 *      consecutive.                                                          *
 ******************************************************************************/
uint32_t drvEfuseWriteImei(uint8_t *src_dat_ptr, uint32_t bit_num)
{
    uint32_t i, min_id, len;
    uint32_t *efuse_ptr;
    uint32_t max_bit;
    EFUSE_RETURN_E result;

    if (NULL == src_dat_ptr)
    {
        return EFUSE_PTR_NULL_ERROR;
    }

    result = EFUSE_RESULT_SUCCESS;
    max_bit = _efuseGetAppBitLen(EFUSE_IMEI_START_ID);
    min_id = _efuseGetBlkId(EFUSE_IMEI_START_ID);
    len = _efuseGetBlkNumFromBit(bit_num);
    efuse_ptr = _efuseGetEfusePtr();

    if (max_bit < bit_num)
    {
        return EFUSE_PTR_NUM_ERROR;
    }

    for (i = min_id; i < (min_id + len); i++)
    {
        if (EFUSE_LOCKED == _efuseIsLock(i))
        {
            return EFUSE_LOCKED;
        }
    }

    if (false == _efuseSplitData(src_dat_ptr, (uint8_t *)efuse_ptr, len, bit_num, true))
    {
        return EFUSE_PTR_NUM_ERROR;
    }
    else
    {
        for (i = min_id; i < (min_id + len); i++)
        {
            result = drvEfusePmicWrite(i, (*efuse_ptr) | BIT_31);

            if (EFUSE_RESULT_SUCCESS != result)
            {
                return result;
            }

            efuse_ptr++;
        }
    }

    return result;
}
