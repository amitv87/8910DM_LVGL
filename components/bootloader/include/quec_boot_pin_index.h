/**  @file
  quec_boot_pin_index.h

  @brief
  This file is used to define boot pin index api for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
24/01/2021        Neo         Init version
=================================================================*/


#ifndef QUEC_BOOT_PIN_INDEX_H
#define QUEC_BOOT_PIN_INDEX_H


#include "quec_cust_feature.h"
#include "hwregs.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t             pin_num;
    volatile uint32_t   *reg;
} quec_boot_pin_cfg_t;


#ifdef CONFIG_QL_PROJECT_DEF_EC200U
/************* Pin  *****************/
#define QUEC_PIN_CFG_MAX    (4)     /* multiple pins amount */
#define QUEC_PIN_DNAME_KEYOUT_4          81
#define QUEC_PIN_DNAME_KEYOUT_5          82
#define QUEC_PIN_DNAME_KEYIN_4           137
#define QUEC_PIN_DNAME_KEYIN_5           138
/***********    EC200U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EC600U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (6)     /* multiple pins amount */
#define QUEC_PIN_DNAME_GPIO_20           123
#define QUEC_PIN_DNAME_GPIO_21           124
#define QUEC_PIN_DNAME_SDMMC1_DATA_2     49
#define QUEC_PIN_DNAME_SDMMC1_DATA_3     50
#define QUEC_PIN_DNAME_KEYOUT_4          104
#define QUEC_PIN_DNAME_KEYOUT_5          103
/***********    EC600U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG700U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (4)     /* multiple pins amount */
#define QUEC_PIN_DNAME_GPIO_20           64
#define QUEC_PIN_DNAME_GPIO_21           63
#define QUEC_PIN_DNAME_KEYOUT_4          24
#define QUEC_PIN_DNAME_KEYOUT_5          23
/***********    EG700U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG500U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (2)     /* multiple pins amount */
#define QUEC_PIN_DNAME_GPIO_20           30
#define QUEC_PIN_DNAME_GPIO_21           31
/***********    EG500U End    ***********/

#endif
/************ End of Pin Default Name ************/

/************    for standard     ************/
#ifdef CONFIG_QL_PROJECT_DEF_EC200U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_KEYIN_4
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_KEYIN_5

#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5

/***********    EC200U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EC600U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21

#ifndef __QUEC_OEM_VER_LD__
#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5
#else
#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_SDMMC1_DATA_2
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_SDMMC1_DATA_3
#endif


/***********    EC600U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG700U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21

#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5


/***********    EG700U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG500U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21


/***********    EG500U End    ***********/
#endif




#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_BOOT_PIN_INDEX_H */


