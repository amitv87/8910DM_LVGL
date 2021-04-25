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

#ifndef _DRV_NAMES_H_
#define _DRV_NAMES_H_

#include "osi_compiler.h"
#include "quec_proj_config.h"

OSI_EXTERN_C_BEGIN

#define DRV_NAME_INVALID 0

#define DRV_NAME_DEBUGUART OSI_MAKE_TAG('D', 'U', 'R', 'T')     // DURT
#define DRV_NAME_CP OSI_MAKE_TAG('C', 'P', ' ', ' ')            // CP
#define DRV_NAME_SCI1 OSI_MAKE_TAG('S', 'C', 'I', '1')          // SCI1
#define DRV_NAME_SCI2 OSI_MAKE_TAG('S', 'C', 'I', '2')          // SCI2
#define DRV_NAME_SPI1 OSI_MAKE_TAG('S', 'P', 'I', '1')          // SPI1
#define DRV_NAME_SPI2 OSI_MAKE_TAG('S', 'P', 'I', '2')          // SPI2
#define DRV_NAME_SPI3 OSI_MAKE_TAG('S', 'P', 'I', '3')          // SPI3
#define DRV_NAME_UART1 OSI_MAKE_TAG('U', 'R', 'T', '1')         // URT1
#define DRV_NAME_UART2 OSI_MAKE_TAG('U', 'R', 'T', '2')         // URT2
#define DRV_NAME_UART3 OSI_MAKE_TAG('U', 'R', 'T', '3')         // URT3
#define DRV_NAME_UART4 OSI_MAKE_TAG('U', 'R', 'T', '4')         // URT4
#define DRV_NAME_UART5 OSI_MAKE_TAG('U', 'R', 'T', '5')         // URT5
#define DRV_NAME_SPI_FLASH OSI_MAKE_TAG('S', 'F', 'L', '1')     // SFL1
#define DRV_NAME_SPI_FLASH_EXT OSI_MAKE_TAG('S', 'F', 'L', '2') // SFL2
#define DRV_NAME_AON_WDT OSI_MAKE_TAG('A', 'O', 'W', 'D')       // AOWD
#define DRV_NAME_USB11 OSI_MAKE_TAG('U', 'S', 'B', '1')         // USB1
#define DRV_NAME_USB20 OSI_MAKE_TAG('U', 'S', 'B', '2')         // USB2
#define DRV_NAME_USRL_COM0 OSI_MAKE_TAG('U', 'S', 'L', '0')     // USL0
#define DRV_NAME_USRL_COM1 OSI_MAKE_TAG('U', 'S', 'L', '1')     // USL1
#define DRV_NAME_USRL_COM2 OSI_MAKE_TAG('U', 'S', 'L', '2')     // USL2
#define DRV_NAME_USRL_COM3 OSI_MAKE_TAG('U', 'S', 'L', '3')     // USL3
#define DRV_NAME_USRL_COM4 OSI_MAKE_TAG('U', 'S', 'L', '4')     // USL4
#define DRV_NAME_USRL_COM5 OSI_MAKE_TAG('U', 'S', 'L', '5')     // USL5
#define DRV_NAME_USRL_COM6 OSI_MAKE_TAG('U', 'S', 'L', '6')     // USL6
#define DRV_NAME_USRL_COM7 OSI_MAKE_TAG('U', 'S', 'L', '7')     // USL7
#define DRV_NAME_ACM0 OSI_MAKE_TAG('A', 'C', 'M', '0')          // ACM0
#define DRV_NAME_ACM1 OSI_MAKE_TAG('A', 'C', 'M', '1')          // ACM1
#define DRV_NAME_ACM2 OSI_MAKE_TAG('A', 'C', 'M', '2')          // ACM2
#define DRV_NAME_ACM3 OSI_MAKE_TAG('A', 'C', 'M', '3')          // ACM3
#define DRV_NAME_SDMMC1 OSI_MAKE_TAG('S', 'D', 'C', '1')        // SDC1
#define DRV_NAME_SDMMC2 OSI_MAKE_TAG('S', 'D', 'C', '2')        // SDC2
#define DRV_NAME_CAMERA1 OSI_MAKE_TAG('C', 'A', 'M', '1')       // CAM1
#define DRV_NAME_I2C1 OSI_MAKE_TAG('I', '2', 'C', '1')          // I2C1
#define DRV_NAME_I2C2 OSI_MAKE_TAG('I', '2', 'C', '2')          // I2C2
#define DRV_NAME_I2C3 OSI_MAKE_TAG('I', '2', 'C', '3')          // I2C3
#define DRV_NAME_WCN OSI_MAKE_TAG('W', 'C', 'N', ' ')           // WCN
#ifndef CONFIG_QUEC_PROJECT_FEATURE_LEDCFG
#define DRV_NAME_PWM1 OSI_MAKE_TAG('P', 'W', 'M', '1')          // PWM1
#else
#define DRV_NAME_PWT OSI_MAKE_TAG('P', 'W', 'T', ' ')           // PWT
#define DRV_NAME_LPG OSI_MAKE_TAG('L', 'P', 'G', ' ')           // LPG
#define DRV_NAME_PWL0 OSI_MAKE_TAG('P', 'W', 'L', '0')          // PWL0
#define DRV_NAME_PWL1 OSI_MAKE_TAG('P', 'W', 'L', '1')          // PWL1
#endif
#define DRV_NAME_LCD1 OSI_MAKE_TAG('L', 'C', 'D', '1')          // LCD1
#define DRV_NAME_LCD2 OSI_MAKE_TAG('L', 'C', 'D', '2')          // LCD2
#define DRV_NAME_GOUDA OSI_MAKE_TAG('G', 'O', 'U', 'D')         // GOUD
#define DRV_NAME_ADI_BUS OSI_MAKE_TAG('A', 'D', 'I', 'B')       // ADIB
#define DRV_NAME_IOMUX OSI_MAKE_TAG('I', 'O', 'M', 'X')         // IOMX
#define DRV_NAME_GPIO OSI_MAKE_TAG('G', 'P', 'I', 'O')          // GPIO
#define DRV_NAME_AXI_DMA OSI_MAKE_TAG('A', 'D', 'M', 'A')       // ADMA
#define DRV_NAME_CCID0 OSI_MAKE_TAG('C', 'I', 'D', '0')         // CID0

#define DRV_NAME_PREFIX_MASK OSI_MAKE_TAG('\xff', '\xff', '\xff', '\x00')
#define DRV_NAME_IS_UART(name) (((name)&DRV_NAME_PREFIX_MASK) == OSI_MAKE_TAG('U', 'R', 'T', '\0'))
#define DRV_NAME_IS_USRL(name) (((name)&DRV_NAME_PREFIX_MASK) == OSI_MAKE_TAG('U', 'S', 'L', '\0'))

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SLEEP
#define DRV_NAME_OPEN_SLEEP_TAG     OSI_MAKE_TAG('Q','L','S','\0')
#define DRV_NAME_KERNEL_SLEEP_MAIN  		OSI_MAKE_TAG('Q', 'K', 'S', 'M')
#define DRV_NAME_KERNEL_SLEEP_WAKEUP_IN  	OSI_MAKE_TAG('Q', 'K', 'S', 'W')
#endif

OSI_EXTERN_C_END
#endif
