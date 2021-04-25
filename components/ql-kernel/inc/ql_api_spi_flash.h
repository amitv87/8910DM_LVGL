/**  @file
  ql_api_spi_flash.h

  @brief
  This file is used to define bt api for different Quectel Project.

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
=================================================================*/


#ifndef QL_API_SPI_FLASH_H
#define QL_API_SPI_FLASH_H
#include "ql_api_common.h"
#include "ql_api_spi.h"


#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Variable Definition
 *========================================================================*/
/****************************  error code about ql spi flash ***************************/
typedef enum
{
    QL_SPI_FLASH_SUCCESS                  =   0,
    //is the same with nomal spi
    QL_SPI_FLASH_ERROR                    =   1 | (QL_COMPONENT_STORAGE_EXTFLASH << 16),    //SPI总线其他错误
    QL_SPI_FLASH_PARAM_TYPE_ERROR,                  //参数类型错误
    QL_SPI_FLASH_PARAM_DATA_ERROR,                  //参数数据错误
    QL_SPI_FLASH_PARAM_ACQUIRE_ERROR,               //参数无法获取
    QL_SPI_FLASH_PARAM_NULL_ERROR,                  //参数NULL错误
    QL_SPI_FLASH_DEV_NOT_ACQUIRE_ERROR,             //无法获取SPI总线
    QL_SPI_FLASH_PARAM_LENGTH_ERROR,                //参数长度错误
    QL_SPI_FLASH_MALLOC_MEM_ERROR,                  //申请内存错误

    QL_SPI_FLASH_NOT_FLASH_CONN_ERROR,              //没有接入FLASH芯片
    QL_SPI_FLASH_NOT_SUPPORT_ERROR,                 //FLASH型号不支持
    QL_SPI_FLASH_OP_NOT_SUPPORT_ERROR,              //FLASH型号不支持此操作
}ql_errcode_spi_nor_e;
typedef struct
{
    ql_spi_port_e port;                             //SPI总线选择
    ql_spi_clk_e spiclk;                            //SPI时钟配置
    ql_spi_input_sel_e input_sel;                   //SPI输入引脚选择
    ql_spi_transfer_mode_e transmode;               //SPI工作模式，目前仅支持QL_SPI_DIRECT_POLLING
    ql_spi_cs_sel_e cs;                             //SPI片选CS引脚选择
} ql_spi_nor_config_s;


/*========================================================================
 *  function Definition
 *========================================================================*/
 
/*****************************************************************
* Function: ql_spi_nor_init
*
* Description:
*   初始化NOR FLASH
* 
* Parameters:
*   port     [in]  SPI总线选择
*   spiclk   [in]  SPI时钟选择
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_init(ql_spi_port_e port, ql_spi_clk_e spiclk);

/*****************************************************************
* Function: ql_spi_nor_init_ext
*
* Description:
*   扩展初始化NOR FLASH，与ql_spi_nor_init有更多的设置选项
* 
* Parameters:
*   nor_config   [in]  SPI配置参数
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_init_ext(ql_spi_nor_config_s nor_config);

/*****************************************************************
* Function: ql_spi_nor_write_8bit_status
*
* Description:
*   写8bit的状态寄存器
* 
* Parameters:
*   port     [in]  SPI总线选择
*   status   [in]  设置状态寄存器的值
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_write_8bit_status(ql_spi_port_e port, unsigned char status);

/*****************************************************************
* Function: ql_spi_nor_write_16bit_status
*
* Description:
*   写16bit的状态寄存器
* 
* Parameters:
*   port     [in]  SPI总线选择
*   status   [in]  设置状态寄存器的值
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_write_16bit_status(ql_spi_port_e port, unsigned short status);

/*****************************************************************
* Function: ql_spi_nor_read_status_low
*
* Description:
*   读状态寄存器的低8bit
* 
* Parameters:
*   port     [in]  SPI总线选择
*   status   [out]  读取状态寄存器的值
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_read_status_low(ql_spi_port_e port, unsigned char *status);

/*****************************************************************
* Function: ql_spi_nor_read_status_high
*
* Description:
*   读状态寄存器的高8bit
* 
* Parameters:
*   port     [in]   SPI总线选择
*   status   [out]  读取状态寄存器的值
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_read_status_high(ql_spi_port_e port, unsigned char *status);

/*****************************************************************
* Function: ql_spi_nor_read
*
* Description:
*   从NOR FLASH芯片读数据
* 
* Parameters:
*   port     [in]   SPI总线选择
*   data     [out]  读取数据的缓存
*   addr     [in]   读取数据地址
*   len      [in]   读取数据长度
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_read(ql_spi_port_e port, unsigned char* data, unsigned int addr, unsigned short len);

/*****************************************************************
* Function: ql_spi_nor_write
*
* Description:
*   写数据到NOR FLASH芯片
* 
* Parameters:
*   port     [in]   SPI总线选择
*   data     [in]   写入数据的缓存
*   addr     [in]   写入数据地址
*   len      [in]   写入数据长度
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_write(ql_spi_port_e port, unsigned char *data, unsigned int addr, unsigned short len);

/*****************************************************************
* Function: ql_spi_nor_erase_chip
*
* Description:
*   擦除整个NOR FLASH芯片的数据
* 
* Parameters:
*   port     [in]   SPI总线选择
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_erase_chip(ql_spi_port_e port);

/*****************************************************************
* Function: ql_spi_nor_erase_sector
*
* Description:
*   擦除NOR FLASH数据，4K大小
* 
* Parameters:
*   port     [in]   SPI总线选择
*   addr     [in]   擦除数据地址
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_erase_sector(ql_spi_port_e port, unsigned int addr);

/*****************************************************************
* Function: ql_spi_nor_erase_64k_block
*
* Description:
*   擦除NOR FLASH数据，64K大小
* 
* Parameters:
*   port     [in]   SPI总线选择
*   addr     [in]   擦除数据地址
*
* Return:ql_errcode_spi_nor_e
*
*****************************************************************/
ql_errcode_spi_nor_e ql_spi_nor_erase_64k_block(ql_spi_port_e port, unsigned int addr);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_API_SPI_FLASH_H */



