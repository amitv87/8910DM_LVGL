/**  @file
  ql_api_spi.h

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


#ifndef QL_API_SPI_H
#define QL_API_SPI_H
#include "ql_api_common.h"


#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Variable Definition
 *========================================================================*/
#if 0
#define QL_SPI1_CS_PIN                  134
#define QL_SPI1_CS_FUNC                 0x2
#define QL_SPI1_CLK_PIN                 133
#define QL_SPI1_CLK_FUNC                0x2
#define QL_SPI1_DO_PIN                  132
#define QL_SPI1_DO_FUNC                 0x2
#define QL_SPI1_DI_PIN                  131
#define QL_SPI1_DI_FUNC                 0x2
#else
#define QL_SPI1_CS_PIN                  37                  //引脚号
#define QL_SPI1_CS_FUNC                 0x1                 //此引脚选用第几功能，选用第1功能
#define QL_SPI1_CLK_PIN                 40
#define QL_SPI1_CLK_FUNC                0x1
#define QL_SPI1_DO_PIN                  38
#define QL_SPI1_DO_FUNC                 0x1
#define QL_SPI1_DI_PIN                  39
#define QL_SPI1_DI_FUNC                 0x1
#endif

#if 0
#define QL_SPI2_CS_PIN                  32
#define QL_SPI2_CS_FUNC                 0x7
#define QL_SPI2_CLK_PIN                 30
#define QL_SPI2_CLK_FUNC                0x7
#define QL_SPI2_DO_PIN                  29
#define QL_SPI2_DO_FUNC                 0x7
#define QL_SPI2_DI_PIN                  28
#define QL_SPI2_DI_FUNC                 0x7
#else
#define QL_SPI2_CS_PIN                  26
#define QL_SPI2_CS_FUNC                 0x2
#define QL_SPI2_CLK_PIN                 27
#define QL_SPI2_CLK_FUNC                0x2
#define QL_SPI2_DO_PIN                  24
#define QL_SPI2_DO_FUNC                 0x2
#define QL_SPI2_DI_PIN                  25
#define QL_SPI2_DI_FUNC                 0x2
#endif

/****************************  error code about ql spi    ***************************/
typedef enum
{
	QL_SPI_SUCCESS                  =   0,
    
    QL_SPI_ERROR                    =   1 | (QL_COMPONENT_BSP_SPI << 16),   //SPI总线其他错误
    QL_SPI_PARAM_TYPE_ERROR,                        //参数类型错误
    QL_SPI_PARAM_DATA_ERROR,                        //参数数据错误
    QL_SPI_PARAM_ACQUIRE_ERROR,                     //参数无法获取
    QL_SPI_PARAM_NULL_ERROR,                        //参数NULL错误
    QL_SPI_DEV_NOT_ACQUIRE_ERROR,                   //无法获取SPI总线
    QL_SPI_PARAM_LENGTH_ERROR,                      //参数长度错误
    QL_SPI_MALLOC_MEM_ERROR,                        //申请内存错误
}ql_errcode_spi_e;

typedef enum
{
    QL_SPI_CS0 = 0,                                 //选择cs0为SPI片选CS引脚
    QL_SPI_CS1,                                     //选择cs1为SPI片选CS引脚
    QL_SPI_CS2,                                     //选择cs2为SPI片选CS引脚
    QL_SPI_CS3,                                     //选择cs3为SPI片选CS引脚
}ql_spi_cs_sel_e;

typedef enum
{
    QL_SPI_INPUT_FALSE,                             //SPI不允许输入（读取）
    QL_SPI_INPUT_TURE,                              //SPI允许输入（读取）
}ql_spi_input_mode_e;

typedef enum
{
    QL_SPI_PORT1,                                   //SPI1总线
    QL_SPI_PORT2,                                   //SPI2总线
}ql_spi_port_e;

typedef enum
{
    QL_SPI_CS_ACTIVE_HIGH,                          //SPI总线操作时，CS脚为高
    QL_SPI_CS_ACTIVE_LOW,                           //SPI总线操作时，CS脚为低
} ql_spi_cs_pol_e;
    
typedef enum
{
    QL_SPI_CPOL_LOW = 0,                            //选择上升沿操作
    QL_SPI_CPOL_HIGH,                               //选择下降沿操作
} ql_spi_cpol_pol_e;

typedef enum
{
    QL_SPI_CPHA_1Edge,                              //时钟延时一个边沿
    QL_SPI_CPHA_2Edge,                              //时钟延时两个边沿
}ql_spi_cpha_pol_e;

typedef enum
{
    QL_SPI_DI_0 = 0,                                //选择DI0为数据输入引脚
    QL_SPI_DI_1,                                    //选择DI1为数据输入引脚
    QL_SPI_DI_2,                                    //选择DI2为数据输入引脚
}ql_spi_input_sel_e;

typedef enum
{
    QL_SPI_DIRECT_POLLING = 0,                      //FIFO读写，轮询等待
    QL_SPI_DIRECT_IRQ,                              //FIFO读写，中断通知
    QL_SPI_DMA_POLLING,                             //DMA读写，轮询等待
    QL_SPI_DMA_IRQ,                                 //DMA读写，中断通知
}ql_spi_transfer_mode_e;

typedef enum
{	
	QL_SPI_CLK_INVALID=-1,                          //无效时钟选择
	QL_SPI_CLK_781_25KHZ=0,                         //时钟：781.25K
	QL_SPI_CLK_1_5625MHZ,                           //时钟：1.5625M
	QL_SPI_CLK_3_125MHZ,                            //时钟：3.125M
	QL_SPI_CLK_6_25MHZ,                             //时钟：6.25M
	QL_SPI_CLK_12_5MHZ,                             //时钟：12.5M
	QL_SPI_CLK_25MHZ,                               //时钟：25M
	QL_SPI_CLK_50MHZ,                               //时钟：50M
	QL_SPI_CLK_MAX,                                 //时钟：100M
}ql_spi_clk_e;

typedef struct
{
    ql_spi_input_mode_e input_mode;
    ql_spi_port_e port;
    unsigned int framesize;
    ql_spi_clk_e spiclk;
    ql_spi_cs_pol_e cs_polarity0;
    ql_spi_cs_pol_e cs_polarity1;
    ql_spi_cpol_pol_e cpol;
    ql_spi_cpha_pol_e cpha;
    ql_spi_input_sel_e input_sel;
    ql_spi_transfer_mode_e transmode;
    ql_spi_cs_sel_e cs;
} ql_spi_config_s;

typedef enum
{
    QL_SPI_TRIGGER_1_DATA,                      //FIFO有1个字节，触发中断
    QL_SPI_TRIGGER_4_DATA,                      //FIFO有4个字节，触发中断
    QL_SPI_TRIGGER_8_DATA,                      //FIFO有8个字节，触发中断
    QL_SPI_TRIGGER_12_DATA,                     //FIFO有12个字节，触发中断
}ql_spi_threshold_e;

typedef struct
{
    unsigned int rx_ovf : 1;
    unsigned int tx_th : 1;
    unsigned int tx_dma_done : 1;
    unsigned int rx_th : 1;
    unsigned int rx_dma_done : 1;
    ql_spi_threshold_e tx_threshold;
    ql_spi_threshold_e rx_threshold;
}ql_spi_irq_s;

typedef void (*ql_spi_callback)(ql_spi_irq_s cause);

/*========================================================================
 *  function Definition
 *========================================================================*/
/*****************************************************************
* Function: ql_spi_init
*
* Description:
*   初始化SPI总线
* 
* Parameters:
*   port        [in]    SPI总线选择
*   transmode   [in]    SPI总线工作的传输模式，仅支持QL_SPI_DIRECT_POLLING模式
*   spiclk      [in]    SPI总线传输速率配置
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_init(ql_spi_port_e port, ql_spi_transfer_mode_e transmode, ql_spi_clk_e spiclk);

/*****************************************************************
* Function: ql_spi_init_ext
*
* Description:
*   初始化SPI总线，与ql_spi_init相比，有更多的配置选择
* 
* Parameters:
*   spi_config  [in]    SPI总线配置
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_init_ext(ql_spi_config_s spi_config);

/*****************************************************************
* Function: ql_spi_write_read
*
* Description:
*   对SPI总线执行写和�?2个操�?
* 
* Parameters:
*   port        [in]    SPI总线选择
*   inbuf       [in]    写入的数�?
*   inlen       [in]    写入的数据长�?
*   outbuf      [out]   读取的数�?
*   outlen      [in]    需要读取的数据长度
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_write_read(ql_spi_port_e port, unsigned char *inbuf, unsigned int inlen, unsigned char *outbuf, unsigned int outlen);

/*****************************************************************
* Function: ql_spi_read
*
* Description:
*   对SPI总线执行读操�?
* 
* Parameters:
*   port        [in]    SPI总线选择
*   buf         [out]   读取的数�?
*   len         [in]    需要读取的数据长度
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_read(ql_spi_port_e port, unsigned char *buf, unsigned int len);

/*****************************************************************
* Function: ql_spi_write
*
* Description:
*   对SPI总线执行写操�?
* 
* Parameters:
*   port        [in]    SPI总线选择
*   buf         [in]    写入的数�?
*   len         [in]    写入的数据长�?
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_write(ql_spi_port_e port, unsigned char *buf, unsigned int len);

/*****************************************************************
* Function: ql_spi_release
*
* Description:
*   释放SPI总线
* 
* Parameters:
*   port        [in]    SPI总线选择
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_release(ql_spi_port_e port);

/*****************************************************************
* Function: ql_spi_cs_low
*
* Description:
*   对SPI总线的CS引脚强制拉低
* 
* Parameters:
*   port        [in]    SPI总线选择
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_cs_low(ql_spi_port_e port);

/*****************************************************************
* Function: ql_spi_cs_high
*
* Description:
*   对SPI总线的CS引脚强制拉高
* 
* Parameters:
*   port        [in]    SPI总线选择
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_cs_high(ql_spi_port_e port);

/*****************************************************************
* Function: ql_spi_cs_auto
*
* Description:
*   SPI总线的CS引脚恢复成系统控制，系统默认CS引脚由系统控�?
* 
* Parameters:
*   port        [in]    SPI总线选择
*
* Return:ql_errcode_spi_e
*
*****************************************************************/
ql_errcode_spi_e ql_spi_cs_auto(ql_spi_port_e port);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_API_SPI_H */



