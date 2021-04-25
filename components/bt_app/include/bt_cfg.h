/******************************************************************************
 ** File Name:      bt_cfg.h                                                 *
 ** Author:         Liangwen.Zhen                                             *
 ** DATE:           03/13/2008                                                *
 ** Copyright:      2007 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:    This file defines the basic operation interfaces of BT    *
 **					about product
 **                                                                           *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 03/13/2008     Liangwen.Zhen    Create.                         *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "bt_abs.h"

#define BT_DEFAULT_LOCAL_NAME "UIS8910_BT"

#define BT_FILE_DIR "D:\\bt"

#define BT_THREAD_TASK_PRIORITY (SCI_PRIORITY_LOWEST - 4)

typedef struct _BT_NV_PARAM
{
    BT_ADDRESS bd_addr;
    uint16 xtal_dac;
} BT_NV_PARAM;

typedef struct _BT_SPRD_NV_PARAM
{
    uint32 host_reserved[2]; //0x0
    uint32 config0;          //0x6e
    uint32 device_class;     //0x001f00
    uint32 win_ext;          //0x1E
    uint32 g_aGainValue[6];
    uint32 g_aPowerValue[5];
    uint32 feature_set[4]; //0xFE8DFFFF 0x8379FF9B 0x7FFFA7FF 0x3EF7E000
    uint32 config1;        //0x3F300E05
    uint32 g_PrintLevel;   //0xFFFFFFFF
    uint32 config2;        //0xDC198CFB
    uint32 config3;        //0x4A9C4BBE
} BT_SPRD_NV_PARAM;

typedef struct sharememory
{
    uint8 audio_rx_buf[2 * 160];
    uint8 audio_tx_buf[2 * 240];
    uint8 cont_host_hand;
    uint8 chip_id;
    uint8 controller_version[8];
    uint32 host_sys_timer;
    uint32 audio_tx_para;
    uint8 rf_powerdown;
    uint8 reserved[205];

} t_sharememory;
/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/
#ifndef _BT_CFG_H_
#define _BT_CFG_H_

#include "bt_abs.h"

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
//  Description:    This function is used to get BT configure information
//  Author:
//  Note:
/*****************************************************************************/
//PUBLIC void BT_GetProdConfig(BT_CONFIG *config);

/***************************************************************************
    Function Name:
    Description:  surpport share crystal or not
    Note:         
    Author:       paul.luo
    Date:       5.21.2009
****************************************************************************/

PUBLIC BOOLEAN BT_GetCrystCfg(void);
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
}

#endif // End of bt_prod.h

#endif // _BT_CFG_H_
