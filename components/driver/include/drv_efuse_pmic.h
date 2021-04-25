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

#ifndef _DRV_EFUSE_PMIC_H_
#define _DRV_EFUSE_PMIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @breif EFuse open operation
 * Power on effuse; Open efuse clk;
 */
void drvEfusePmicOpen(void);

/**
 * @breif EFuse close operation
 * Power off effuse; Disbale efuse clk;
 */
void drvEfusePmicClose(void);

/**
 * breif Read data from the selected efuse block
 * @block_id  id number of the selected bllock
 * @des_data_ptr : the pointer of data reading from the selected block
 * @return
 *      - non-zero  fail to read data
 *      - 0   successful to read data
 */
uint32_t drvEfusePmicRead(uint32_t block_id, uint32_t *des_data_ptr);

/**
 * breif Write data to the selected efuse block
 * @block_id  Id number of the selected bllock
 * @write_data  The pointer of data that is to be written
 * @return
 *      - non-zero  fail to write data
 *      - 0   successful to write data
 */
uint32_t drvEfusePmicWrite(uint32_t block_id, uint32_t write_data);

/**
 * breif Read IMEI from the uix2720 efuse blocks.
 * @des_data_ptr  the pointer of data reading from the selected block
 * @bit_num       number of bit
 * @return
 *      - non-zero  fail to read imei
 *      - 0   successful to read imei
 */
uint32_t drvEfuseReadImei(uint8_t *des_data_ptr, uint32_t bit_num);

/**
 * breif Write IMEI to the uix2720 efuse blocks.
 * @src_dat_ptr  the pointer of data that is to be written
 * @bit_num      number of bit
 * @return
 *      - non-zero  fail to write imei
 *      - 0   successful to write imei
 */
uint32_t drvEfuseWriteImei(uint8_t *src_dat_ptr, uint32_t bit_num);

#ifdef __cplusplus
}
#endif

#endif /*_DRV_EFUSE_PMIC_H_ */
