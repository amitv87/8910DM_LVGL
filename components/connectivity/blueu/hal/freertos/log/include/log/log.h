/**
 * @file     log.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __BT_HAL_LOG_H__
#define __BT_HAL_LOG_H__

#include "bt_types.h"
#include <stdio.h>
#include "host_log.h"
#include "osi_log.h"

//#define DEBUG_LOG

/*
* DSP LOG POINT:
*
*   0xDAxx
*   0xDBxx
*   0xDCxx
*   0xDDxx
*/

#define LPT_PAYLOAD      0xDA00
#define LPT_HCI_CMD      0xDA01
#define LPT_HCI_ACL_C2H  0xDA02
#define LPT_HCI_ACL_H2C  0xDA06
#define LPT_HCI_SCO      0xDA03
#define LPT_HCI_EVT      0xDA04
#define LPT_L2CAP_IN     0xDA05

#define LPT_TMP     0xDDF0


#define LPT_GEN_ASSERT      0xDAE0





#define LPT_H2C 0x00
#define LPT_C2H 0x01

void logpt_hci_dump(UINT8 direction, UINT8 type, UINT8 *src, UINT32 length);
void logpt_primary(UINT16 tag, UINT16 points);
void logpt_msg(UINT16 tag, UINT16 index, UINT16 payload);

void bt_assert_mask(unsigned int value);
void bt_assert();

#define BTD(...)                    \
    do {                            \
        printf(__VA_ARGS__);        \
    } while(0)

#ifdef DEBUG_LOG

#define BT_ERR(...)                 \
    do {                        \
        printf(__VA_ARGS__);    \
    } while(0)

#define BT_WAN(...)                 \
    do {                        \
        printf(__VA_ARGS__);    \
    } while(0)

#define BT_INF(...)                 \
    do {                        \
        printf(__VA_ARGS__);    \
    } while(0)

#define BT_DBG(...)                 \
    do {                        \
        printf(__VA_ARGS__);    \
    } while(0)
#else
#define BT_ERR(...)
#define BT_WAN(...)
#define BT_INF(...)
#define BT_DBG(...)

#endif

#endif /* __BT_HAL_LOG_H__ */

