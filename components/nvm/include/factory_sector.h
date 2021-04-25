/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _FACTORY_SECTOR_H_
#define _FACTORY_SECTOR_H_

#include "osi_compiler.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

OSI_EXTERN_C_BEGIN

#define FACTORY_VERSION (0XFAC00200)
#define FACTORY_BLOCK_SIZE (1024)
#define FACTORY_SIM_COUNT (4)
#define FACTORY_IMEI_LEN (9)
#define FACTORY_SN_LEN (64)
#define FACTORY_MAC_LEN (6)
#define FACTORY_STATION_COUNT (15)
#define FACTORY_STATION_NAME_LEN (12)
#define FACTORY_STATION_DESC_LEN (32)
#define FACTORY_USER_LEN (544)
#define FACTORY_BT_DEVNAME_LEN (32)
#define FACTORY_BT_PINCODE_LEN (16)
#define FACTORY_BT_MAC_FLAG (0X1D0E)

#define FACTORY_STATION_PASSED (1 << 0)
#define FACTORY_STATION_PERFORMED (1 << 1)

/**
 * IMEI in factory sector
 *
 * An example: 012345678901234 will be stored as:
 *      0x03 0x21 0x43 0x65 0x87 0x09 0x21 0x43
 *
 * Bits [3:0] is not a part of IMEI. It will be ignored at reading, and
 * it should be set as 3 at writing for backward compatible.
 */
typedef uint8_t factoryImei_t[FACTORY_IMEI_LEN];

/**
 * MAC address in factory sector
 */
typedef struct
{
    uint16_t activated;
    uint8_t mac[FACTORY_MAC_LEN];
} factoryMacAddress_t;

/**
 * station name in factory sector
 *
 * The last charactor can be not \0. And when station name is shorter than
 * \p FACTORY_STATION_NAME_LEN, the remaining bytes shall be filled with
 * \0.
 */
typedef uint8_t factoryStationName_t[FACTORY_STATION_NAME_LEN];

/**
 * bluetooth information in factory sector
 */
typedef struct
{
    uint16_t activated;
    uint8_t dev_addr[FACTORY_MAC_LEN];
    uint8_t dev_name[FACTORY_BT_DEVNAME_LEN];
    uint8_t pin_code[FACTORY_BT_PINCODE_LEN];
} factoryBtInfo_t;

/**
 * WiFi information in factory sector
 */
typedef struct
{
    uint16_t activated;
    uint8_t mac_addr[FACTORY_MAC_LEN];
    uint8_t mac_ap1[FACTORY_MAC_LEN];
    uint8_t mac_ap2[FACTORY_MAC_LEN];
    uint8_t mac_ap3[FACTORY_MAC_LEN];
    uint8_t reserved[2];
} factoryWifiInfo_t;

/**
 * block data structure in factory sector
 *
 * This data structure will be kept the exact same as the data structure in
 * PC tool. So, don't change the data structure if not needed.
 *
 * For specified product, it is possible that some fields are not used.
 *
 * \user is reserved for customer. Factory block management won't know the
 * content inside.
 */
typedef struct
{
    uint32_t version;                                          ///< data structure version
    factoryImei_t imei_sv[FACTORY_SIM_COUNT];                  ///< IMEI
    uint8_t mb_sn[FACTORY_SN_LEN];                             ///< SN #0, motherboard SN
    uint8_t mp_sn[FACTORY_SN_LEN];                             ///< SN #1, phone/module SN
    factoryBtInfo_t bt;                                        ///< bluetooth information
    factoryWifiInfo_t wifi;                                    ///< WiFi information
    factoryStationName_t station_names[FACTORY_STATION_COUNT]; ///< station names
    uint16_t station_performed;                                ///< 1: performed, 0: not performed
    uint16_t station_passed;                                   ///< 1: passed, 0: failed
    uint8_t station_last_desc[FACTORY_STATION_DESC_LEN];       ///< last station description
    uint8_t user[FACTORY_USER_LEN];                            ///< user defined area
    uint32_t sequence;                                         ///< internal sequence
    uint32_t crc;                                              ///< CRC from beginning to sequence
    uint32_t crc_inv;                                          ///< bit-reversed CRC value
} factoryBlock_t;

/**
 * initialize factor sector management
 *
 * There are redundant blocks in factory sectors. Before access factory
 * sectors, it is needed to call \p factorySectorInit to scan blocks in
 * factory sectors.
 */
void factorySectorInit(void);

/**
 * read factory block
 *
 * \p buf should be enough to hold the whole factory block.
 *
 * \param buf       buffer for read
 * \return
 *      - true on success
 *      - false on error
 *          - \p factorySectorInit isn't called beforehand
 *          - \p buf is NULL
 *          - there are no valid factory block
 */
bool factoryBlockRead(factoryBlock_t *buf);

/**
 * write factory block
 *
 * Before write, \p version, \p sequence, \p crc and \p crc_inv will be
 * changed to match factory block management rules. To avoid repeated
 * dynamic memory allocation and free, \p buf will be changed in place.
 *
 * \param buf       buffer for write
 * \return
 *      - true on success
 *      - false on error
 *          - \p buf is NULL
 */
bool factoryBlockWrite(factoryBlock_t *buf);

/**
 * write partial of factory block
 *
 * Even \p factoryBlockRead and \p factoryBlockWrite themselves are thread
 * safe, read-modify-write is not thread safe. This is thread safe to write
 * partial of factory block.
 *
 * \p buf will be write to \p offset of factory block.
 *
 * \param buf       buffer for modify
 * \param offset    offset in factory block to be modified
 * \param size      modification size
 * \return
 *      - true on success
 *      - false on error
 *          - parameters are invalid
 */
bool factoryBlockPartialWrite(size_t offset, const void *buf, size_t size);

/**
 * get IMEI from factory sector
 *
 * \param idx       index
 * \param imei      buffer for read imei
 * \return
 *      - true on success
 *      - false on error
 *          - parameters are invalid
 *          - imei not exist
 *          - imei in factory sector invalid
 */
bool factoryGetImei(int idx, void *imei);

/**
 * set IMEI to factory sector
 *
 * \param idx       index
 * \param imei      buffer for read imei
 * \return
 *      - true on success
 *      - false on error
 *          - parameters are invalid
 *          - \p imei is invalid
 */
bool factorySetImei(int idx, const void *imei);

/**
 * get station flags
 *
 * Supported flags:
 * - \p FACTORY_STATION_PERFORMED
 * - \p FACTORY_STATION_PASSED
 *
 * \param name      station name
 * \return
 *      - station flags
 *      - -1 if \p name not exist
 */
int factoryGetStationFlag(const char *name);

/**
 * set station flags
 *
 * Supported flags:
 * - \p FACTORY_STATION_PERFORMED
 * - \p FACTORY_STATION_PASSED
 *
 * \param name      station name
 * \param flags     station flags
 * \return
 *      - true on success
 *      - false if \p name not exist
 */
bool factorySetStationFlag(const char *name, uint32_t flags);

OSI_EXTERN_C_END
#endif
