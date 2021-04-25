/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QUEC_PIN_INDEX_H
#define QUEC_PIN_INDEX_H


#include "quec_cust_feature.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * Pin Number
 ===========================================================================*/
/************    GPIO Default Name    ************/
/************    use Function0 Name   ************///don't change
#define QUEC_GPIO_DNAME_GPIO_0           0
#define QUEC_GPIO_DNAME_SPI_LCD_SIO      0
#define QUEC_GPIO_DNAME_GPIO_1           1
#define QUEC_GPIO_DNAME_SPI_LCD_SDC      1
#define QUEC_GPIO_DNAME_GPIO_2           2
#define QUEC_GPIO_DNAME_SPI_LCD_CLK      2
#define QUEC_GPIO_DNAME_GPIO_3           3
#define QUEC_GPIO_DNAME_SPI_LCD_CS       3
#define QUEC_GPIO_DNAME_GPIO_4           4
#define QUEC_GPIO_DNAME_SPI_LCD_SELECT   4
#define QUEC_GPIO_DNAME_GPIO_5           5
#define QUEC_GPIO_DNAME_LCD_FMARK        5
#define QUEC_GPIO_DNAME_GPIO_7           7
#define QUEC_GPIO_DNAME_GPIO_8           8
#define QUEC_GPIO_DNAME_AP_JTAG_TCK      8
#define QUEC_GPIO_DNAME_GPIO_9           9
#define QUEC_GPIO_DNAME_AP_JTAG_TRST     9
#define QUEC_GPIO_DNAME_GPIO_10          10
#define QUEC_GPIO_DNAME_AP_JTAG_TMS      10
#define QUEC_GPIO_DNAME_GPIO_11          11
#define QUEC_GPIO_DNAME_AP_JTAG_TDI      11
#define QUEC_GPIO_DNAME_GPIO_12          12
#define QUEC_GPIO_DNAME_AP_JTAG_TDO      12
#define QUEC_GPIO_DNAME_GPIO_13          13
#define QUEC_GPIO_DNAME_GPIO_14          14
#define QUEC_GPIO_DNAME_GPIO_15          15
#define QUEC_GPIO_DNAME_I2C_M1_SCL       16
#define QUEC_GPIO_DNAME_I2C_M1_SDA       17
#define QUEC_GPIO_DNAME_GPIO_18          18
#define QUEC_GPIO_DNAME_CAMERA_RST_L     18
#define QUEC_GPIO_DNAME_GPIO_19          19
#define QUEC_GPIO_DNAME_CAMERA_PWDN      19
#define QUEC_GPIO_DNAME_GPIO_20          20
#define QUEC_GPIO_DNAME_CAMERA_REF_CLK   20
#define QUEC_GPIO_DNAME_GPIO_21          21
#define QUEC_GPIO_DNAME_SPI_CAMERA_SI_0  21
#define QUEC_GPIO_DNAME_GPIO_22          22
#define QUEC_GPIO_DNAME_SPI_CAMERA_SI_1  22
#define QUEC_GPIO_DNAME_GPIO_23          23
#define QUEC_GPIO_DNAME_SPI_CAMERA_SCK   23
#define QUEC_GPIO_DNAME_SDMMC1_CMD       24
#define QUEC_GPIO_DNAME_SDMMC1_DATA_0    25
#define QUEC_GPIO_DNAME_SDMMC1_DATA_1    26
#define QUEC_GPIO_DNAME_SDMMC1_DATA_2    27
#define QUEC_GPIO_DNAME_SDMMC1_DATA_3    28
#define QUEC_GPIO_DNAME_SIM_2_CLK        29
#define QUEC_GPIO_DNAME_SIM_2_DIO        30
#define QUEC_GPIO_DNAME_SIM_2_RST        31
/************ End of GPIO Default Name ************/


/************    Pin Default Name     ************/
/************    use Function0 Name   ************///add pin number for each project
#ifdef CONFIG_QL_PROJECT_DEF_EC200U

/************* Pin Max **************/
#define QUEC_PIN_CFG_MAX    (53)     /* multiple pins amount */

#define QUEC_PIN_DNAME_GPIO_0            27
#define QUEC_PIN_DNAME_SPI_LCD_SIO       125
#define QUEC_PIN_DNAME_GPIO_1            26
#define QUEC_PIN_DNAME_SPI_LCD_SDC       124
#define QUEC_PIN_DNAME_GPIO_2            24
#define QUEC_PIN_DNAME_SPI_LCD_CLK       123
#define QUEC_PIN_DNAME_GPIO_3            35
#define QUEC_PIN_DNAME_SPI_LCD_CS        122
#define QUEC_PIN_DNAME_GPIO_4            13
#define QUEC_PIN_DNAME_SPI_LCD_SELECT    121
#define QUEC_PIN_DNAME_GPIO_5            135
#define QUEC_PIN_DNAME_LCD_FMARK         119
#define QUEC_PIN_DNAME_GPIO_7            133
#define QUEC_PIN_DNAME_GPIO_8            3
#define QUEC_PIN_DNAME_AP_JTAG_TCK       134
#define QUEC_PIN_DNAME_GPIO_9            40
#define QUEC_PIN_DNAME_AP_JTAG_TRST      132
#define QUEC_PIN_DNAME_GPIO_10           37
#define QUEC_PIN_DNAME_AP_JTAG_TMS       131
#define QUEC_PIN_DNAME_GPIO_11           38
#define QUEC_PIN_DNAME_AP_JTAG_TDI       130
#define QUEC_PIN_DNAME_GPIO_12           39
#define QUEC_PIN_DNAME_AP_JTAG_TDO       129
#define QUEC_PIN_DNAME_GPIO_13           5
#define QUEC_PIN_DNAME_GPIO_14           141
#define QUEC_PIN_DNAME_GPIO_15           142
#define QUEC_PIN_DNAME_I2C_M1_SCL        41
#define QUEC_PIN_DNAME_I2C_M1_SDA        42
#define QUEC_PIN_DNAME_GPIO_18           65
#define QUEC_PIN_DNAME_CAMERA_RST_L      61
#define QUEC_PIN_DNAME_GPIO_19           64
#define QUEC_PIN_DNAME_CAMERA_PWDN       62
#define QUEC_PIN_DNAME_GPIO_20           136
#define QUEC_PIN_DNAME_CAMERA_REF_CLK    63
#define QUEC_PIN_DNAME_GPIO_21           139
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_0   66
#define QUEC_PIN_DNAME_GPIO_22           126
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_1   6
#define QUEC_PIN_DNAME_GPIO_23           127
#define QUEC_PIN_DNAME_SPI_CAMERA_SCK    23
#define QUEC_PIN_DNAME_SDMMC1_CMD        33
#define QUEC_PIN_DNAME_SDMMC1_DATA_0     31
#define QUEC_PIN_DNAME_SDMMC1_DATA_1     30
#define QUEC_PIN_DNAME_SDMMC1_DATA_2     29
#define QUEC_PIN_DNAME_SDMMC1_DATA_3     28
#define QUEC_PIN_DNAME_SIM_2_CLK         1
#define QUEC_PIN_DNAME_SIM_2_DIO         2
#define QUEC_PIN_DNAME_SIM_2_RST         4
//pinmux(not gpio part)
#define QUEC_PIN_DNAME_SDMMC1_CLK        32
#define QUEC_PIN_DNAME_KEYOUT_4          81
#define QUEC_PIN_DNAME_KEYOUT_5          82
#define QUEC_PIN_DNAME_KEYIN_4           137
#define QUEC_PIN_DNAME_KEYIN_5           138
/***********    EC200U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EC600U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (53)     /* multiple pins amount */

#define QUEC_PIN_DNAME_GPIO_0            61
#define QUEC_PIN_DNAME_SPI_LCD_SIO       66
#define QUEC_PIN_DNAME_GPIO_1            58
#define QUEC_PIN_DNAME_SPI_LCD_SDC       63
#define QUEC_PIN_DNAME_GPIO_2            59
#define QUEC_PIN_DNAME_SPI_LCD_CLK       67
#define QUEC_PIN_DNAME_GPIO_3            60
#define QUEC_PIN_DNAME_SPI_LCD_CS        65
#define QUEC_PIN_DNAME_GPIO_4            69
#define QUEC_PIN_DNAME_SPI_LCD_SELECT    137
#define QUEC_PIN_DNAME_GPIO_5            70
#define QUEC_PIN_DNAME_LCD_FMARK         62
#define QUEC_PIN_DNAME_GPIO_7            118
#define QUEC_PIN_DNAME_GPIO_8            9
#define QUEC_PIN_DNAME_AP_JTAG_TCK       99
#define QUEC_PIN_DNAME_GPIO_9            1
#define QUEC_PIN_DNAME_AP_JTAG_TRST      98
#define QUEC_PIN_DNAME_GPIO_10           4
#define QUEC_PIN_DNAME_AP_JTAG_TMS       95
#define QUEC_PIN_DNAME_GPIO_11           3
#define QUEC_PIN_DNAME_AP_JTAG_TDI       119
#define QUEC_PIN_DNAME_GPIO_12           2
#define QUEC_PIN_DNAME_AP_JTAG_TDO       100
#define QUEC_PIN_DNAME_GPIO_13           54
#define QUEC_PIN_DNAME_GPIO_14           57
#define QUEC_PIN_DNAME_GPIO_15           56
#define QUEC_PIN_DNAME_I2C_M1_SCL        11
#define QUEC_PIN_DNAME_I2C_M1_SDA        12
#define QUEC_PIN_DNAME_GPIO_18           34
#define QUEC_PIN_DNAME_CAMERA_RST_L      120
#define QUEC_PIN_DNAME_GPIO_19           33
#define QUEC_PIN_DNAME_CAMERA_PWDN       16
#define QUEC_PIN_DNAME_GPIO_20           123
#define QUEC_PIN_DNAME_CAMERA_REF_CLK    10
#define QUEC_PIN_DNAME_GPIO_21           124
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_0   14
#define QUEC_PIN_DNAME_GPIO_22           122
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_1   15
#define QUEC_PIN_DNAME_GPIO_23           121
#define QUEC_PIN_DNAME_SPI_CAMERA_SCK    13
#define QUEC_PIN_DNAME_SDMMC1_CMD        48
#define QUEC_PIN_DNAME_SDMMC1_DATA_0     39
#define QUEC_PIN_DNAME_SDMMC1_DATA_1     40
#define QUEC_PIN_DNAME_SDMMC1_DATA_2     49
#define QUEC_PIN_DNAME_SDMMC1_DATA_3     50
#define QUEC_PIN_DNAME_SIM_2_CLK         53
#define QUEC_PIN_DNAME_SIM_2_DIO         52
#define QUEC_PIN_DNAME_SIM_2_RST         51
//pinmux(not gpio part)
#define QUEC_PIN_DNAME_SDMMC1_CLK        132
#define QUEC_PIN_DNAME_KEYOUT_4          104
#define QUEC_PIN_DNAME_KEYOUT_5          103
#define QUEC_PIN_DNAME_KEYIN_4           126
#define QUEC_PIN_DNAME_KEYIN_5           125
/***********    EC600U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG700U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (43)     /* multiple pins amount */

#define QUEC_PIN_DNAME_GPIO_0            34
#define QUEC_PIN_DNAME_SPI_LCD_SIO       84
#define QUEC_PIN_DNAME_GPIO_1            35
#define QUEC_PIN_DNAME_SPI_LCD_SDC       85
#define QUEC_PIN_DNAME_GPIO_2            77
#define QUEC_PIN_DNAME_SPI_LCD_CLK       43
#define QUEC_PIN_DNAME_GPIO_3            78
#define QUEC_PIN_DNAME_SPI_LCD_CS        42
#define QUEC_PIN_DNAME_GPIO_4            79
#define QUEC_PIN_DNAME_SPI_LCD_SELECT    81
#define QUEC_PIN_DNAME_GPIO_5            80
#define QUEC_PIN_DNAME_LCD_FMARK         82
#define QUEC_PIN_DNAME_GPIO_7            46
#define QUEC_PIN_DNAME_GPIO_8            55
//#define QUEC_PIN_DNAME_AP_JTAG_TCK
#define QUEC_PIN_DNAME_GPIO_9            17
//#define QUEC_PIN_DNAME_AP_JTAG_TRST
#define QUEC_PIN_DNAME_GPIO_10           16
//#define QUEC_PIN_DNAME_AP_JTAG_TMS
#define QUEC_PIN_DNAME_GPIO_11           19
//#define QUEC_PIN_DNAME_AP_JTAG_TDI
#define QUEC_PIN_DNAME_GPIO_12           18
//#define QUEC_PIN_DNAME_AP_JTAG_TDO
#define QUEC_PIN_DNAME_GPIO_13           45
#define QUEC_PIN_DNAME_GPIO_14           86
#define QUEC_PIN_DNAME_GPIO_15           87
#define QUEC_PIN_DNAME_I2C_M1_SCL        28
#define QUEC_PIN_DNAME_I2C_M1_SDA        29
#define QUEC_PIN_DNAME_GPIO_18           1
#define QUEC_PIN_DNAME_CAMERA_RST_L      75
#define QUEC_PIN_DNAME_GPIO_19           90
#define QUEC_PIN_DNAME_CAMERA_PWDN       98
#define QUEC_PIN_DNAME_GPIO_20           64
#define QUEC_PIN_DNAME_CAMERA_REF_CLK    99
#define QUEC_PIN_DNAME_GPIO_21           63
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_0   76
#define QUEC_PIN_DNAME_GPIO_22           20
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_1   100
#define QUEC_PIN_DNAME_GPIO_23           22
#define QUEC_PIN_DNAME_SPI_CAMERA_SCK    101
#define QUEC_PIN_DNAME_SDMMC1_CMD        21
#define QUEC_PIN_DNAME_SDMMC1_DATA_0     72
#define QUEC_PIN_DNAME_SDMMC1_DATA_1     73
#define QUEC_PIN_DNAME_SDMMC1_DATA_2     69
#define QUEC_PIN_DNAME_SDMMC1_DATA_3     68
//#define QUEC_PIN_DNAME_SIM_2_CLK
//#define QUEC_PIN_DNAME_SIM_2_DIO
//#define QUEC_PIN_DNAME_SIM_2_RST
//pinmux(not gpio part)
#define QUEC_PIN_DNAME_SDMMC1_CLK        66
#define QUEC_PIN_DNAME_KEYOUT_4          24
#define QUEC_PIN_DNAME_KEYOUT_5          23
//#define QUEC_PIN_DNAME_KEYIN_4
//#define QUEC_PIN_DNAME_KEYIN_5
/***********    EG700U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG500U

/*************** Pin ****************/
#define QUEC_PIN_CFG_MAX    (44)     /* multiple pins amount */

#define QUEC_PIN_DNAME_GPIO_0            46
#define QUEC_PIN_DNAME_SPI_LCD_SIO       19
#define QUEC_PIN_DNAME_GPIO_1            13
#define QUEC_PIN_DNAME_SPI_LCD_SDC       24
#define QUEC_PIN_DNAME_GPIO_2            45
#define QUEC_PIN_DNAME_SPI_LCD_CLK       20
#define QUEC_PIN_DNAME_GPIO_3            44
#define QUEC_PIN_DNAME_SPI_LCD_CS        25
#define QUEC_PIN_DNAME_GPIO_4            54
#define QUEC_PIN_DNAME_SPI_LCD_SELECT    26
#define QUEC_PIN_DNAME_GPIO_5            17
#define QUEC_PIN_DNAME_LCD_FMARK         27
#define QUEC_PIN_DNAME_GPIO_7            55
#define QUEC_PIN_DNAME_GPIO_8            56
//#define QUEC_PIN_DNAME_AP_JTAG_TCK
//#define QUEC_PIN_DNAME_GPIO_9
//#define QUEC_PIN_DNAME_AP_JTAG_TRST
//#define QUEC_PIN_DNAME_GPIO_10
//#define QUEC_PIN_DNAME_AP_JTAG_TMS
//#define QUEC_PIN_DNAME_GPIO_11
//#define QUEC_PIN_DNAME_AP_JTAG_TDI
//#define QUEC_PIN_DNAME_GPIO_12
//#define QUEC_PIN_DNAME_AP_JTAG_TDO
#define QUEC_PIN_DNAME_GPIO_13           53
#define QUEC_PIN_DNAME_GPIO_14           35
#define QUEC_PIN_DNAME_GPIO_15           36
#define QUEC_PIN_DNAME_I2C_M1_SCL        29
#define QUEC_PIN_DNAME_I2C_M1_SDA        28
#define QUEC_PIN_DNAME_GPIO_18           14
#define QUEC_PIN_DNAME_CAMERA_RST_L      85
#define QUEC_PIN_DNAME_GPIO_19           10
#define QUEC_PIN_DNAME_CAMERA_PWDN       84
#define QUEC_PIN_DNAME_GPIO_20           30
#define QUEC_PIN_DNAME_CAMERA_REF_CLK    83
#define QUEC_PIN_DNAME_GPIO_21           31
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_0   86
#define QUEC_PIN_DNAME_GPIO_22           32
#define QUEC_PIN_DNAME_SPI_CAMERA_SI_1   87
#define QUEC_PIN_DNAME_GPIO_23           33
#define QUEC_PIN_DNAME_SPI_CAMERA_SCK    88
#define QUEC_PIN_DNAME_SDMMC1_CMD        81
#define QUEC_PIN_DNAME_SDMMC1_DATA_0     90
#define QUEC_PIN_DNAME_SDMMC1_DATA_1     97
#define QUEC_PIN_DNAME_SDMMC1_DATA_2     101
#define QUEC_PIN_DNAME_SDMMC1_DATA_3     111
#define QUEC_PIN_DNAME_SIM_2_CLK         120
#define QUEC_PIN_DNAME_SIM_2_DIO         72
#define QUEC_PIN_DNAME_SIM_2_RST         43
//pinmux(not gpio part)
#define QUEC_PIN_DNAME_SDMMC1_CLK        80
#define QUEC_PIN_DNAME_KEYOUT_4          75
#define QUEC_PIN_DNAME_KEYOUT_5          76
#define QUEC_PIN_DNAME_KEYIN_4           78
#define QUEC_PIN_DNAME_KEYIN_5           79
/***********    EG500U End    ***********/

#endif
/************ End of Pin Default Name ************/


/************    Multiple Function Name    ************/
/************    use Quec Function Name    ************///add location:name for applicaiton
#define QUEC_PIN_LCD_SPI_SIO       QUEC_PIN_DNAME_SPI_LCD_SIO
#define QUEC_PIN_LCD_SPI_SDC       QUEC_PIN_DNAME_SPI_LCD_SDC
#define QUEC_PIN_LCD_SPI_CLK       QUEC_PIN_DNAME_SPI_LCD_CLK
#define QUEC_PIN_LCD_SPI_CS        QUEC_PIN_DNAME_SPI_LCD_CS
#define QUEC_PIN_LCD_SPI_SEL       QUEC_PIN_DNAME_SPI_LCD_SELECT
#define QUEC_PIN_LCD_FMARK         QUEC_PIN_DNAME_LCD_FMARK

#define QUEC_PIN_CAM_I2C_SCL       QUEC_PIN_DNAME_I2C_M1_SCL
#define QUEC_PIN_CAM_I2C_SDA       QUEC_PIN_DNAME_I2C_M1_SDA
#define QUEC_PIN_CAM_RSTL          QUEC_PIN_DNAME_CAMERA_RST_L
#define QUEC_PIN_CAM_PWDN          QUEC_PIN_DNAME_CAMERA_PWDN
#define QUEC_PIN_CAM_REFCLK        QUEC_PIN_DNAME_CAMERA_REF_CLK
#define QUEC_PIN_CAM_SPI_DATA0     QUEC_PIN_DNAME_SPI_CAMERA_SI_0
#define QUEC_PIN_CAM_SPI_DATA1     QUEC_PIN_DNAME_SPI_CAMERA_SI_1
#define QUEC_PIN_CAM_SPI_SCK       QUEC_PIN_DNAME_SPI_CAMERA_SCK

#define QUEC_PIN_SPI1_CLK          QUEC_PIN_DNAME_GPIO_9
#define QUEC_PIN_SPI1_CS           QUEC_PIN_DNAME_GPIO_10
#define QUEC_PIN_SPI1_MOSI         QUEC_PIN_DNAME_GPIO_11
#define QUEC_PIN_SPI1_MISO         QUEC_PIN_DNAME_GPIO_12

#define QUEC_PIN_SPI2_CLK          QUEC_PIN_DNAME_SDMMC1_DATA_1
#define QUEC_PIN_SPI2_CS           QUEC_PIN_DNAME_SDMMC1_CLK
#define QUEC_PIN_SPI2_MOSI         QUEC_PIN_DNAME_SDMMC1_DATA_2
#define QUEC_PIN_SPI2_MISO         QUEC_PIN_DNAME_SDMMC1_DATA_3

#define QUEC_PIN_NET_STATUS        QUEC_PIN_DNAME_GPIO_13           //for Standard:sleep low power

/************    for standard     ************/
#ifdef CONFIG_QL_PROJECT_DEF_EC200U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_KEYIN_4
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_KEYIN_5

#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5

#define QUEC_PIN_MAIN_RI           QUEC_PIN_DNAME_CAMERA_PWDN       //for Standard:advanced initialization

/*************** GPIO ***************/
#define QUEC_GPIO_SLEEP_IND        QUEC_GPIO_DNAME_GPIO_8
#define QUEC_GPIO_NET_STATUS       QUEC_GPIO_DNAME_GPIO_13
#define QUEC_GPIO_NET_MODE         QUEC_GPIO_DNAME_SPI_CAMERA_SI_1

#define QUEC_GPIO_MAIN_RI          QUEC_GPIO_DNAME_CAMERA_PWDN
#define QUEC_GPIO_MAIN_DTR         QUEC_GPIO_DNAME_SPI_CAMERA_SI_0
#define QUEC_GPIO_MAIN_DCD         QUEC_GPIO_DNAME_CAMERA_REF_CLK

#define QUEC_GPIO_WAKEUP_IN        QUEC_GPIO_DNAME_SIM_2_CLK
#define QUEC_GPIO_AP_READY         QUEC_GPIO_DNAME_SIM_2_DIO
#define QUEC_GPIO_W_DISABLE        QUEC_GPIO_DNAME_SIM_2_RST

/***********    EC200U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EC600U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21
#define QUEC_PIN_UART2_RTS         QUEC_PIN_DNAME_GPIO_22    //unisoc func is CTS
#define QUEC_PIN_UART2_CTS         QUEC_PIN_DNAME_GPIO_23    //unisoc func is RTS

#ifndef __QUEC_OEM_VER_LD__
//#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
//#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5
//#define QUEC_PIN_UART3_RTS         QUEC_PIN_DNAME_KEYIN_4
//#define QUEC_PIN_UART3_CTS         QUEC_PIN_DNAME_KEYIN_5
#else
#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_SDMMC1_DATA_2
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_SDMMC1_DATA_3
#define QUEC_PIN_UART3_RTS         QUEC_PIN_DNAME_SDMMC1_DATA_0    //unisoc func is CTS
#define QUEC_PIN_UART3_CTS         QUEC_PIN_DNAME_SDMMC1_DATA_1    //unisoc func is RTS
#endif

#define QUEC_PIN_MAIN_RI           QUEC_PIN_DNAME_SDMMC1_DATA_1     //for Standard:advanced initialization

/*************** GPIO ***************/
#define QUEC_GPIO_SLEEP_IND        QUEC_GPIO_DNAME_SIM_2_CLK
#define QUEC_GPIO_NET_STATUS       QUEC_GPIO_DNAME_GPIO_13
#define QUEC_GPIO_NET_MODE         QUEC_GPIO_DNAME_SIM_2_DIO

#define QUEC_GPIO_MAIN_RI          QUEC_GPIO_DNAME_SDMMC1_DATA_1
#define QUEC_GPIO_MAIN_DTR         QUEC_GPIO_DNAME_SDMMC1_DATA_0
#define QUEC_GPIO_MAIN_DCD         QUEC_GPIO_DNAME_SDMMC1_CMD

#define QUEC_GPIO_WAKEUP_IN        QUEC_GPIO_DNAME_SDMMC1_DATA_2
#define QUEC_GPIO_AP_READY         QUEC_GPIO_DNAME_SDMMC1_DATA_3
#define QUEC_GPIO_W_DISABLE        QUEC_GPIO_DNAME_SIM_2_RST

/***********    EC600U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG700U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21

#define QUEC_PIN_UART3_RXD         QUEC_PIN_DNAME_KEYOUT_4
#define QUEC_PIN_UART3_TXD         QUEC_PIN_DNAME_KEYOUT_5

#define QUEC_PIN_MAIN_RI           QUEC_PIN_DNAME_GPIO_15           //for Standard:advanced initialization

/*************** GPIO ***************/
#define QUEC_GPIO_SLEEP_IND        QUEC_GPIO_DNAME_LCD_FMARK
#define QUEC_GPIO_NET_STATUS       QUEC_GPIO_DNAME_GPIO_13
#define QUEC_GPIO_NET_MODE         QUEC_GPIO_DNAME_SPI_LCD_SELECT

#define QUEC_GPIO_MAIN_RI          QUEC_GPIO_DNAME_GPIO_15
#define QUEC_GPIO_MAIN_DTR         QUEC_GPIO_DNAME_GPIO_8
#define QUEC_GPIO_MAIN_DCD         QUEC_GPIO_DNAME_GPIO_14

#define QUEC_GPIO_WAKEUP_IN        QUEC_GPIO_DNAME_SPI_LCD_CLK
#define QUEC_GPIO_AP_READY         QUEC_GPIO_DNAME_SPI_LCD_CS
#define QUEC_GPIO_W_DISABLE        QUEC_GPIO_DNAME_SPI_LCD_SIO

/***********    EG700U End    ***********/

#elif defined CONFIG_QL_PROJECT_DEF_EG500U

/*************** Pin ****************/
#define QUEC_PIN_UART2_RXD         QUEC_PIN_DNAME_GPIO_20
#define QUEC_PIN_UART2_TXD         QUEC_PIN_DNAME_GPIO_21
#define QUEC_PIN_UART2_RTS         QUEC_PIN_DNAME_GPIO_22    //unisoc func is CTS
#define QUEC_PIN_UART2_CTS         QUEC_PIN_DNAME_GPIO_23    //unisoc func is RTS

//#define QUEC_PIN_UART3_RXD                                        //haven't this function in Standard
//#define QUEC_PIN_UART3_TXD                                        //haven't this function in Standard
//#define QUEC_PIN_UART3_RTS                                        //haven't this function in Standard
//#define QUEC_PIN_UART3_CTS                                        //haven't this function in Standard

#define QUEC_PIN_MAIN_RI           QUEC_PIN_DNAME_GPIO_1            //for Standard:advanced initialization

/*************** GPIO ***************/
//#define QUEC_GPIO_SLEEP_IND                                       //haven't this function in Standard
#define QUEC_GPIO_NET_STATUS       QUEC_GPIO_DNAME_GPIO_13
#define QUEC_GPIO_NET_MODE         QUEC_GPIO_DNAME_GPIO_8

#define QUEC_GPIO_MAIN_RI          QUEC_GPIO_DNAME_GPIO_1
#define QUEC_GPIO_MAIN_DTR         QUEC_GPIO_DNAME_GPIO_0
#define QUEC_GPIO_MAIN_DCD         QUEC_GPIO_DNAME_GPIO_2
#define QUEC_GPIO_MAIN_DSR         QUEC_GPIO_DNAME_GPIO_3

#define QUEC_GPIO_WAKEUP_IN        QUEC_GPIO_DNAME_SIM_2_CLK
#define QUEC_GPIO_AP_READY         QUEC_GPIO_DNAME_SIM_2_DIO
#define QUEC_GPIO_W_DISABLE        QUEC_GPIO_DNAME_SIM_2_RST

/***********    EG500U End    ***********/

#endif


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_PIN_INDEX_H */


