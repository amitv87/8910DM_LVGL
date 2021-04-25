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
/*PCM_CLK    */{  27 ,        3,              0,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_0_cfg_reg           },
/*LCD_SIO    */{  125,        0,              1,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sio_cfg_reg      },
/*PCM_SYNC   */{  26 ,        3,              0,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_1_cfg_reg           },
/*LCD_SDC    */{  124,        0,              1,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sdc_cfg_reg      },
/*PCM_IN     */{  24 ,        3,              0,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_2_cfg_reg           },
/*LCD_CLK    */{  123,        0,              1,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_clk_cfg_reg      },
/*PCM_OUT    */{  25 ,        3,              0,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_3_cfg_reg           },
/*LCD_CS     */{  122,        0,              1,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_cs_cfg_reg       },
/*USIM_DET   */{  13 ,        0,              0,        GPIO_4,    GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_4_cfg_reg           },
/*LCD_SEL    */{  121,        0,              1,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_select_cfg_reg   },
/*WLAN_WAKE  */{  135,        0,              0,        GPIO_5,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_5_cfg_reg           },
/*LCD_FMARK  */{  119,        0,              1,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_lcd_fmark_cfg_reg        },
/*SDIO2_CLK  */{  133,        1,              0,        GPIO_7,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_7_cfg_reg           },// pay attention to USB
/*SLEEP_IND  */{  3  ,        0,              0,        GPIO_8,    GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_gpio_8_cfg_reg           },
/*SDIO2_CMD  */{  134,        1,              5,        GPIO_8,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tck_cfg_reg      },
/*SPI_1_CLK  */{  40 ,        0,              0,        GPIO_9,    GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_gpio_9_cfg_reg           },
/*SDIO2_DATA0*/{  132,        1,              5,        GPIO_9,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_trst_cfg_reg     },
/*SPI_1_CS   */{  37 ,        0,              0,        GPIO_10,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_gpio_10_cfg_reg          },
/*SDIO2_DATA1*/{  131,        1,              5,        GPIO_10,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tms_cfg_reg      },
/*SPI_1_MOSI */{  38 ,        0,              0,        GPIO_11,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_gpio_11_cfg_reg          },
/*SDIO2_DATA2*/{  130,        1,              5,        GPIO_11,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tdi_cfg_reg      },
/*SPI_1_MISO */{  39 ,        0,              0,        GPIO_12,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_gpio_12_cfg_reg          },
/*SDIO2_DATA3*/{  129,        1,              5,        GPIO_12,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tdo_cfg_reg      },
/*NET_STATUS */{  5  ,        1,              0,        GPIO_13,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_13_cfg_reg          },
/*I2C2_SCL   */{  141,        1,              0,        GPIO_14,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_14_cfg_reg          },
/*I2C2_SDA   */{  142,        1,              0,        GPIO_15,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_15_cfg_reg          },
/*I2C_SCL    */{  41 ,        0,              4,        GPIO_16,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_scl_cfg_reg       },// output only
/*I2C_SDA    */{  42 ,        0,              4,        GPIO_17,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_sda_cfg_reg       },
/*MAIN_RTS   */{  65 ,        1,              0,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_18_cfg_reg          },
/*STATUS     */{  61 ,        4,              4,        GPIO_18,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_camera_rst_l_cfg_reg     },
/*MAIN_CTS   */{  64 ,        1,              0,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_19_cfg_reg          },
/*MAIN_RI    */{  62 ,        4,              4,        GPIO_19,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_camera_pwdn_cfg_reg      },
/*WLAN_EN    */{  136,        3,              0,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_20_cfg_reg          },
/*MAIN_DCD   */{  63 ,        4,              4,        GPIO_20,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_HIGH,         &hwp_iomux->pad_camera_ref_clk_cfg_reg   },
/*BT_EN      */{  139,        3,              0,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_21_cfg_reg          },
/*MAIN_DTR   */{  66 ,        4,              4,        GPIO_21,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_0_cfg_reg  },
/*GPIO1      */{  126,        3,              0,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_22_cfg_reg          },
/*NET_MODE   */{  6  ,        4,              4,        GPIO_22,   GPIO_OUTPUT,      QUEC_PIN_NONE,    LVL_LOW,          &hwp_iomux->pad_spi_camera_si_1_cfg_reg  },
/*WLAN_PWR_EN*/{  127,        3,              0,        GPIO_23,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_23_cfg_reg          },
/*SD_DET     */{  23 ,        4,              4,        GPIO_23,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_sck_cfg_reg   },
/*SDIO1_CMD  */{  33 ,        0,              1,        GPIO_24,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_cmd_cfg_reg       },
/*SDIO1_DATA0*/{  31 ,        0,              1,        GPIO_25,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_0_cfg_reg    },
/*SDIO1_DATA1*/{  30 ,        0,              1,        GPIO_26,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_1_cfg_reg    },
/*SDIO1_DATA2*/{  29 ,        0,              1,        GPIO_27,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*SDIO1_DATA3*/{  28 ,        0,              1,        GPIO_28,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
/*WAKEUP_IN  */{  1  ,        2,              2,        GPIO_29,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_clk_cfg_reg        },
/*AP_READY   */{  2  ,        2,              2,        GPIO_30,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_dio_cfg_reg        },
/*W_DISABLE  */{  4  ,        2,              2,        GPIO_31,   GPIO_INPUT,       PULL_UP,          QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_rst_cfg_reg        },
//pinmux(not gpio part)
/*SDIO1_CLK  */{  32 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_clk_cfg_reg       },
/*KEYOUT4    */{  81 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  82 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_5_cfg_reg         },
/*KEYIN4     */{  137,        3,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyin_4_cfg_reg          },
/*KEYIN5     */{  138,        3,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyin_5_cfg_reg          }
};
#else /* Open */
{            /* pin_num  default_func     gpio_func    gpio_num       gpio_dir         gpio_pull          gpio_lvl              reg                  */
/*PCM_CLK    */{  27 ,        1,              0,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_0_cfg_reg           },
/*LCD_SIO    */{  125,        0,              1,        GPIO_0,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sio_cfg_reg      },
/*PCM_SYNC   */{  26 ,        1,              0,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_1_cfg_reg           },
/*LCD_SDC    */{  124,        0,              1,        GPIO_1,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_sdc_cfg_reg      },
/*PCM_IN     */{  24 ,        1,              0,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_2_cfg_reg           },
/*LCD_CLK    */{  123,        0,              1,        GPIO_2,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_clk_cfg_reg      },
/*PCM_OUT    */{  25 ,        1,              0,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_3_cfg_reg           },
/*LCD_CS     */{  122,        0,              1,        GPIO_3,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_cs_cfg_reg       },
/*USIM_DET   */{  13 ,        1,              0,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_4_cfg_reg           },
/*LCD_SEL    */{  121,        0,              1,        GPIO_4,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_lcd_select_cfg_reg   },
/*WLAN_WAKE  */{  135,        1,              0,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_5_cfg_reg           },
/*LCD_FMARK  */{  119,        0,              1,        GPIO_5,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_lcd_fmark_cfg_reg        },
/*SDIO2_CLK  */{  133,        1,              0,        GPIO_7,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_7_cfg_reg           },// pay attention to USB
/*SLEEP_IND  */{  3  ,        0,              0,        GPIO_8,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_8_cfg_reg           },
/*SDIO2_CMD  */{  134,        1,              5,        GPIO_8,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tck_cfg_reg      },
/*SPI_1_CLK  */{  40 ,        0,              0,        GPIO_9,    GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_9_cfg_reg           },
/*SDIO2_DATA0*/{  132,        1,              5,        GPIO_9,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_trst_cfg_reg     },
/*SPI_1_CS   */{  37 ,        0,              0,        GPIO_10,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_10_cfg_reg          },
/*SDIO2_DATA1*/{  131,        1,              5,        GPIO_10,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tms_cfg_reg      },
/*SPI_1_MOSI */{  38 ,        0,              0,        GPIO_11,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_11_cfg_reg          },
/*SDIO2_DATA2*/{  130,        1,              5,        GPIO_11,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tdi_cfg_reg      },
/*SPI_1_MISO */{  39 ,        0,              0,        GPIO_12,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_12_cfg_reg          },
/*SDIO2_DATA3*/{  129,        1,              5,        GPIO_12,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_ap_jtag_tdo_cfg_reg      },
/*NET_STATUS */{  5  ,        0,              0,        GPIO_13,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_13_cfg_reg          },
/*I2C2_SCL   */{  141,        1,              0,        GPIO_14,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_14_cfg_reg          },
/*I2C2_SDA   */{  142,        1,              0,        GPIO_15,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_15_cfg_reg          },
/*I2C_SCL    */{  41 ,        0,              4,        GPIO_16,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_scl_cfg_reg       },// output only
/*I2C_SDA    */{  42 ,        0,              4,        GPIO_17,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_i2c_m1_sda_cfg_reg       },
/*MAIN_RTS   */{  65 ,        1,              0,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_18_cfg_reg          },
/*STATUS     */{  61 ,        0,              4,        GPIO_18,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_rst_l_cfg_reg     },
/*MAIN_CTS   */{  64 ,        1,              0,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_19_cfg_reg          },
/*MAIN_RI    */{  62 ,        0,              4,        GPIO_19,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_pwdn_cfg_reg      },
/*WLAN_EN    */{  136,        0,              0,        GPIO_20,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_20_cfg_reg          },
/*MAIN_DCD   */{  63 ,        0,              4,        GPIO_20,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_camera_ref_clk_cfg_reg   },
/*BT_EN      */{  139,        0,              0,        GPIO_21,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_21_cfg_reg          },
/*MAIN_DTR   */{  66 ,        0,              4,        GPIO_21,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_0_cfg_reg  },
/*GPIO1      */{  126,        0,              0,        GPIO_22,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_22_cfg_reg          },
/*NET_MODE   */{  6  ,        0,              4,        GPIO_22,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_si_1_cfg_reg  },
/*WLAN_PWR_EN*/{  127,        0,              0,        GPIO_23,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_gpio_23_cfg_reg          },
/*SD_DET     */{  23 ,        0,              4,        GPIO_23,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_spi_camera_sck_cfg_reg   },
/*SDIO1_CMD  */{  33 ,        0,              1,        GPIO_24,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_cmd_cfg_reg       },
/*SDIO1_DATA0*/{  31 ,        0,              1,        GPIO_25,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_0_cfg_reg    },
/*SDIO1_DATA1*/{  30 ,        0,              1,        GPIO_26,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_1_cfg_reg    },
/*SDIO1_DATA2*/{  29 ,        0,              1,        GPIO_27,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_2_cfg_reg    },
/*SDIO1_DATA3*/{  28 ,        0,              1,        GPIO_28,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_data_3_cfg_reg    },
/*WAKEUP_IN  */{  1  ,        2,              2,        GPIO_29,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_clk_cfg_reg        },
/*AP_READY   */{  2  ,        2,              2,        GPIO_30,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_dio_cfg_reg        },
/*W_DISABLE  */{  4  ,        2,              2,        GPIO_31,   GPIO_INPUT,       PULL_DOWN,        QUEC_PIN_NONE,    &hwp_iomux->pad_sim_2_rst_cfg_reg        },
//pinmux(not gpio part)
/*SDIO1_CLK  */{  32 ,        0,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_sdmmc1_clk_cfg_reg       },
/*KEYOUT4    */{  81 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_4_cfg_reg         },
/*KEYOUT5    */{  82 ,        4,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyout_5_cfg_reg         },
/*KEYIN4     */{  137,        3,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyin_4_cfg_reg          },
/*KEYIN5     */{  138,        3,  QUEC_PIN_NONE,  QUEC_PIN_NONE,   QUEC_PIN_NONE,    QUEC_PIN_NONE,    QUEC_PIN_NONE,    &hwp_iomux->pad_keyin_5_cfg_reg          }
};
#endif/* CONFIG_QL_OPEN_EXPORT_PKG */

