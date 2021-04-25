/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include "quec_pin_cfg.h"

#include "hwregs.h"

/*===========================================================================
 * GPIO Map
 ===========================================================================*/
quec_pin_cfg_t quec_pin_cfg_map[] = /* pin initialize */
#ifndef CONFIG_QL_OPEN_EXPORT_PKG /* Standard */
{            /* pin_num  default_func     gpio_func    gpio_num       gpio_dir         gpio_pull          gpio_lvl              reg                  */
/*FLASH_CLK  */{  34 ,        1,              0,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_0_cfg_reg           },
/*W_DISABLE  */{  84 ,        1,              1,        GPIO_0,    GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sio_cfg_reg      },
/*FLASH_CS   */{  35 ,        1,              0,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_1_cfg_reg           },
/*STATUS     */{  85 ,        1,              1,        GPIO_1,    GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_spi_lcd_sdc_cfg_reg      },
/*FLASH_DATA0*/{  77 ,        1,              0,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_2_cfg_reg           },
/*WAKEUP_IN  */{  43 ,        1,              1,        GPIO_2,    GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_clk_cfg_reg      },
/*FLASH_DATA1*/{  78 ,        1,              0,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_3_cfg_reg           },
/*AP_READY   */{  42 ,        1,              1,        GPIO_3,    GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_cs_cfg_reg       },
/*FLASH_DATA2*/{  79 ,        1,              0,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_4_cfg_reg           },
/*NET_MODE   */{  81 ,        1,              1,        GPIO_4,    GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_spi_lcd_select_cfg_reg   },
/*FLASH_DATA3*/{  80 ,        1,              0,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_5_cfg_reg           },
/*SLEEP_IND  */{  82 ,        1,              1,        GPIO_5,    GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_lcd_fmark_cfg_reg        },
/*RESERVED   */{  46 ,        0,              0,        GPIO_7,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_7_cfg_reg           },// pay attention to USB
/*MAIN_DTR   */{  55 ,        0,              0,        GPIO_8,    GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_8_cfg_reg           },
/*SPI_1_CLK  */{  17 ,        1,              0,        GPIO_9,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_9_cfg_reg           },
/*SPI_1_CS   */{  16 ,        1,              0,        GPIO_10,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_10_cfg_reg          },
/*SPI_1_MOSI */{  19 ,        1,              0,        GPIO_11,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_11_cfg_reg          },
/*SPI_1_MISO */{  18 ,        1,              0,        GPIO_12,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_12_cfg_reg          },
/*NET_STATUS */{  45 ,        1,              0,        GPIO_13,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_13_cfg_reg          },
/*MAIN_DCD   */{  86 ,        0,              0,        GPIO_14,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_gpio_14_cfg_reg          },
/*MAIN_RI    */{  87 ,        0,              0,        GPIO_15,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_gpio_15_cfg_reg          },
/*I2C_SCL    */{  28 ,        0,              4,        GPIO_16,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_scl_cfg_reg       },// output only
/*I2C_SDA    */{  29 ,        0,              4,        GPIO_17,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_sda_cfg_reg       },
/*MAIN_RTS   */{  1  ,        1,              0,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_18_cfg_reg          },
/*CAM_RSTL   */{  75 ,        0,              4,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_rst_l_cfg_reg     },
/*MAIN_CTS   */{  90 ,        1,              0,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_19_cfg_reg          },
/*CAM_PWDN   */{  98 ,        0,              4,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_pwdn_cfg_reg      },
/*UART2_RXD  */{  64 ,        1,              0,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_20_cfg_reg          },
/*CAM_REFCLK */{  99 ,        0,              4,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_ref_clk_cfg_reg   },
/*UART2_TXD  */{  63 ,        1,              0,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_21_cfg_reg          },
/*CAM_DATA0  */{  76 ,        0,              4,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_0_cfg_reg  },
/*CP_TXD     */{  20 ,        3,              0,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_22_cfg_reg          },
/*CAM_DATA1  */{  100,        0,              4,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_1_cfg_reg  },
/*USIM_DET   */{  22 ,        0,              0,        GPIO_23,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_23_cfg_reg          },
/*CAM_SCK    */{  101,        0,              4,        GPIO_23,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_sck_cfg_reg   },
/*SDIO1_CMD  */{  21 ,        0,              1,        GPIO_24,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_cmd_cfg_reg       },
/*SDIO1_DATA0*/{  72 ,        0,              1,        GPIO_25,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_0_cfg_reg    },
/*SDIO1_DATA1*/{  73 ,        0,              1,        GPIO_26,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_1_cfg_reg    },
/*SDIO1_DATA2*/{  69 ,        0,              1,        GPIO_27,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*SDIO1_DATA3*/{  68 ,        0,              1,        GPIO_28,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
//pinmux(not gpio part)
/*SDIO1_CLK  */{  66 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_clk_cfg_reg       },
/*KEYOUT4    */{  24 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  23 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_5_cfg_reg         }
};
#else /* Open */
{            /* pin_num  default_func     gpio_func    gpio_num       gpio_dir         gpio_pull          gpio_lvl              reg                  */
/*FLASH_CLK  */{  34 ,        1,              0,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_0_cfg_reg           },
/*LCD_SIO    */{  84 ,        0,              1,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sio_cfg_reg      },
/*FLASH_CS   */{  35 ,        1,              0,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_1_cfg_reg           },
/*LCD_SDC    */{  85 ,        0,              1,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sdc_cfg_reg      },
/*FLASH_DATA0*/{  77 ,        1,              0,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_2_cfg_reg           },
/*LCD_CLK    */{  43 ,        0,              1,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_clk_cfg_reg      },
/*FLASH_DATA1*/{  78 ,        1,              0,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_3_cfg_reg           },
/*LCD_CS     */{  42 ,        0,              1,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_cs_cfg_reg       },
/*FLASH_DATA2*/{  79 ,        1,              0,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_4_cfg_reg           },
/*LCD_SEL    */{  81 ,        0,              1,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_select_cfg_reg   },
/*FLASH_DATA3*/{  80 ,        1,              0,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_5_cfg_reg           },
/*LCD_FMARK  */{  82 ,        0,              1,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_lcd_fmark_cfg_reg        },
/*GPIO_7     */{  46 ,        0,              0,        GPIO_7,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_7_cfg_reg           },// pay attention to USB
/*MAIN_DTR   */{  55 ,        0,              0,        GPIO_8,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_8_cfg_reg           },
/*SPI_1_CLK  */{  17 ,        1,              0,        GPIO_9,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_9_cfg_reg           },
/*SPI_1_CS   */{  16 ,        1,              0,        GPIO_10,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_10_cfg_reg          },
/*SPI_1_MOSI */{  19 ,        1,              0,        GPIO_11,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_11_cfg_reg          },
/*SPI_1_MISO */{  18 ,        1,              0,        GPIO_12,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_12_cfg_reg          },
/*NET_STATUS */{  45 ,        0,              0,        GPIO_13,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_13_cfg_reg          },
/*MAIN_DCD   */{  86 ,        0,              0,        GPIO_14,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_14_cfg_reg          },
/*MAIN_RI    */{  87 ,        0,              0,        GPIO_15,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_15_cfg_reg          },
/*I2C_SCL    */{  28 ,        0,              4,        GPIO_16,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_scl_cfg_reg       },// output only
/*I2C_SDA    */{  29 ,        0,              4,        GPIO_17,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_sda_cfg_reg       },
/*MAIN_RTS   */{  1  ,        1,              0,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_18_cfg_reg          },
/*CAM_RSTL   */{  75 ,        0,              4,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_rst_l_cfg_reg     },
/*MAIN_CTS   */{  90 ,        1,              0,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_19_cfg_reg          },
/*CAM_PWDN   */{  98 ,        0,              4,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_pwdn_cfg_reg      },
/*UART2_RXD  */{  64 ,        1,              0,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_20_cfg_reg          },
/*CAM_REFCLK */{  99 ,        0,              4,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_ref_clk_cfg_reg   },
/*UART2_TXD  */{  63 ,        1,              0,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_21_cfg_reg          },
/*CAM_DATA0  */{  76 ,        0,              4,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_0_cfg_reg  },
/*CP_TXD     */{  20 ,        5,              0,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_22_cfg_reg          },
/*CAM_DATA1  */{  100,        0,              4,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_1_cfg_reg  },
/*USIM_DET   */{  22 ,        0,              0,        GPIO_23,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_23_cfg_reg          },
/*CAM_SCK    */{  101,        0,              4,        GPIO_23,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_sck_cfg_reg   },
/*SDIO1_CMD  */{  21 ,        0,              1,        GPIO_24,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_cmd_cfg_reg       },
/*SDIO1_DATA0*/{  72 ,        0,              1,        GPIO_25,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_0_cfg_reg    },
/*SDIO1_DATA1*/{  73 ,        0,              1,        GPIO_26,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_1_cfg_reg    },
/*SDIO1_DATA2*/{  69 ,        0,              1,        GPIO_27,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*SDIO1_DATA3*/{  68 ,        0,              1,        GPIO_28,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
//pinmux(not gpio part)
/*SDIO1_CLK  */{  66 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_clk_cfg_reg       },
/*KEYOUT4    */{  24 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  23 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_5_cfg_reg         }
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

