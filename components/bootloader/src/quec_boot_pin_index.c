/**  
  @file
  quec_boot_pin_index.c

  @brief
  quectel boot pin index interface.

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


/*===========================================================================
 * include files
 ===========================================================================*/
#include "quec_boot_pin_index.h"


#ifdef CONFIG_QL_PROJECT_DEF_EC200U
quec_boot_pin_cfg_t quec_pin_cfg_map[] = /* pin initialize */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
{            /* pin_num                reg                  */
/*KEYOUT4    */{  81 ,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  82 ,        &hwp_iomux->pad_keyout_5_cfg_reg         },
/*KEYIN4     */{  137,        &hwp_iomux->pad_keyin_4_cfg_reg          },
/*KEYIN5     */{  138,        &hwp_iomux->pad_keyin_5_cfg_reg          }
};
#else /* Open */
{            /* pin_num                reg                  */
/*KEYOUT4    */{  81 ,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  82 ,        &hwp_iomux->pad_keyout_5_cfg_reg         },
/*KEYIN4     */{  137,        &hwp_iomux->pad_keyin_4_cfg_reg          },
/*KEYIN5     */{  138,        &hwp_iomux->pad_keyin_5_cfg_reg          }
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

#elif defined CONFIG_QL_PROJECT_DEF_EC600U
quec_boot_pin_cfg_t quec_pin_cfg_map[] = /* pin initialize */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  123,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  124,        &hwp_iomux->pad_gpio_21_cfg_reg          },
#ifndef __QUEC_OEM_VER_LD__
/*WAKEUP_IN  */{  49 ,        &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*AP_READY   */{  50 ,        &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
#else
/*WAKEUP_IN  */{  49 ,        &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*AP_READY   */{  50 ,        &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
#endif
/*KEYOUT4    */{  104,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  103,        &hwp_iomux->pad_keyout_5_cfg_reg         },
};
#else /* Open */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  123,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  124,        &hwp_iomux->pad_gpio_21_cfg_reg          },
/*WAKEUP_IN  */{  49 ,        &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*AP_READY   */{  50 ,        &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
/*KEYOUT4    */{  104,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  103,        &hwp_iomux->pad_keyout_5_cfg_reg         },
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

#elif defined CONFIG_QL_PROJECT_DEF_EG700U
quec_boot_pin_cfg_t quec_pin_cfg_map[] = /* pin initialize */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  64 ,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  63 ,        &hwp_iomux->pad_gpio_21_cfg_reg          },
/*KEYOUT4    */{  24 ,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  23 ,        &hwp_iomux->pad_keyout_5_cfg_reg         }
};
#else /* Open */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  64 ,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  63 ,        &hwp_iomux->pad_gpio_21_cfg_reg          },
/*KEYOUT4    */{  24 ,        &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  23 ,        &hwp_iomux->pad_keyout_5_cfg_reg         }
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

#elif defined CONFIG_QL_PROJECT_DEF_EG500U
quec_boot_pin_cfg_t quec_pin_cfg_map[] = /* pin initialize */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  30 ,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  31 ,        &hwp_iomux->pad_gpio_21_cfg_reg          },
};
#else /* Open */
{            /* pin_num                reg                  */
/*UART2_RXD  */{  30 ,        &hwp_iomux->pad_gpio_20_cfg_reg          },
/*UART2_TXD  */{  31 ,        &hwp_iomux->pad_gpio_21_cfg_reg          },
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

#endif

