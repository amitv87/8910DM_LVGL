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

#ifndef _HAL_GOUDA_H_
#define _HAL_GOUDA_H_

/// @defgroup gouda HAL GOUDA Driver
/// This document describes the characteristics of the GOUDA module and how
/// to use them via its Hardware Abstraction Layer API.
///
/// The GOUDA is a dedicated controller that is capable of driving
/// a parallel LCD module with a multi layer engine.
///
// =============================================================================
// MACROS
// =============================================================================

/// Number of different set of timings available for the GOUDA LCD
/// configuration. This number should be equal to the number
/// of different system frequencies, so that the GOUDA LCD interface
/// can be configured optimaly for every system frequency.
#define HAL_GOUDA_LCD_TIMINGS_QTY 12

#define ENABLE_FMARK_LCD 1 // now ,only support two data line mode lcd.
#define DISABLE_FMARK_LCD 0

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// HAL_GOUDA_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user handler function called when a GOUDA related interrupt
/// is triggered.
// =============================================================================
typedef void (*HAL_GOUDA_IRQ_HANDLER_T)(void);

typedef uint32_t (*HAL_SYS_FREQ_CHANGE_HANDLER_T)(uint32_t sysFreq);

typedef enum
{
    /// No error occured
    HAL_ERR_NO,

    /// A resource reset is required
    HAL_ERR_RESOURCE_RESET,

    /// An attempt to access a busy resource failed
    HAL_ERR_RESOURCE_BUSY,

    /// Timeout while trying to access the resource
    HAL_ERR_RESOURCE_TIMEOUT,

    /// An attempt to access a resource that is not enabled
    HAL_ERR_RESOURCE_NOT_ENABLED,

    /// Invalid parameter
    HAL_ERR_BAD_PARAMETER,

    /// Uart RX FIFO overflowed
    HAL_ERR_UART_RX_OVERFLOW,

    /// Uart TX FIFO overflowed
    HAL_ERR_UART_TX_OVERFLOW,
    HAL_ERR_UART_PARITY,
    HAL_ERR_UART_FRAMING,
    HAL_ERR_UART_BREAK_INT,
    HAL_ERR_TIM_RTC_NOT_VALID,
    HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED,
    HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED,

    /// Communication failure
    HAL_ERR_COMMUNICATION_FAILED,

    HAL_ERR_QTY
} HAL_ERR_T;
// =============================================================================
// HAL_GOUDA_SPI_LINE_T
// -----------------------------------------------------------------------------
/// Define the type of the SLCD SPI line.
// =============================================================================

typedef enum
{
    HAL_GOUDA_SPI_LINE_4 = 0,
    HAL_GOUDA_SPI_LINE_3 = 1,
    HAL_GOUDA_SPI_LINE_4_START_BYTE = 2,
    HAL_GOUDA_SPI_LINE_3_2_LANE = 3,
} HAL_GOUDA_SPI_LINE_T;

// =============================================================================
// HAL_GOUDA_LCD_TIMINGS_T
// -----------------------------------------------------------------------------
/// This types describes the timing used by the GOUDA LCD interface.
/// An array os those will be needed to describe the GOUDA LCD
/// timings for each system frequency.
// =============================================================================

typedef union {
    struct
    {
        /// Address setup time (RS to WR, RS to RD) in clock number
        uint32_t tas : 3;
        uint32_t : 1;
        /// Adress hold time in clock number
        uint32_t tah : 3;
        uint32_t : 1;
        /// Control  pulse width low, in clock number
        uint32_t pwl : 6;
        uint32_t : 2;
        /// Control pulse width high, in clock number
        uint32_t pwh : 6;
        uint32_t : 10;
    };

    /// Use this 'reg' field of the union
    /// to access this timing configuration as
    /// a 32 bits value than can be written to
    /// a register.
    uint32_t reg;
} HAL_GOUDA_LCD_TIMINGS_T;

// =============================================================================
// HAL_GOUDA_LCD_CS_T
// -----------------------------------------------------------------------------
/// Defines the Chip Select use to select the LCD screen connected to the
/// GOUDA_LCD controller.
// =============================================================================
typedef enum
{
    HAL_GOUDA_LCD_CS_0 = 0,
    HAL_GOUDA_LCD_CS_1 = 1,
    HAL_GOUDA_LCD_CS_QTY,
    HAL_GOUDA_LCD_MEMORY_IF = 2,
    HAL_GOUDA_LCD_IN_RAM = 3

} HAL_GOUDA_LCD_CS_T;

// =============================================================================
// HAL_GOUDA_LCD_OUTPUT_FORMAT_T
// -----------------------------------------------------------------------------
/// Describes the Output format of the LCD bus.
/// The MSB select also the AHB access size (8-bit or 16-bit) when Memory destination is selected.
/// Must set to RGB565 when RAM type destination selected
// =============================================================================
typedef enum
{
    /// 8-bit - RGB3:3:2 - 1cycle/1pixel - RRRGGGBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_BIT_RGB332 = 0,
    /// 8-bit - RGB4:4:4 - 3cycle/2pixel - RRRRGGGG/BBBBRRRR/GGGGBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_bit_RGB444 = 1,
    /// 8-bit - RGB5:6:5 - 2cycle/1pixel - RRRRRGGG/GGGBBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_8_bit_RGB565 = 2,
    /// 16-bit - RGB3:3:2 - 1cycle/2pixel - RRRGGGBBRRRGGGBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB332 = 4,
    /// 16-bit - RGB4:4:4 - 1cycle/1pixel - XXXXRRRRGGGGBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB444 = 5,
    /// 16-bit - RGB5:6:5 - 1cycle/1pixel - RRRRRGGGGGGBBBBB
    HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565 = 6

} HAL_GOUDA_LCD_OUTPUT_FORMAT_T;

// =============================================================================
// HAL_GOUDA_LCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the GOUDA LCD interface.
// =============================================================================
typedef union {
    struct
    {
        HAL_GOUDA_LCD_CS_T cs : 2;
        uint32_t : 2;
        HAL_GOUDA_LCD_OUTPUT_FORMAT_T outputFormat : 3;
        bool highByte : 1;
        /// \c FALSE is active low, \c TRUE is active high.
        bool cs0Polarity : 1;
        /// \c FALSE is active low, \c TRUE is active high.
        bool cs1Polarity : 1;
        /// \c FALSE is active low, \c TRUE is active high.
        bool rsPolarity : 1;
        /// \c FALSE is active low, \c TRUE is active high.
        bool wrPolarity : 1;
        /// \c FALSE is active low, \c TRUE is active high.
        bool rdPolarity : 1;
        uint32_t : 3;
        /// Number of command to be send to the LCD command (up to 31)
        uint32_t nbCmd : 5;
        uint32_t : 3;
        /// Start command transfer only. Autoreset
        bool startCmdOnly : 1;
        bool resetb : 1;
        uint32_t : 6;
    } b;

    /// Use this 'reg' field of the union
    /// to access this timing configuration as
    /// a 32 bits value than can be written to
    /// a register.
    uint32_t reg;
} HAL_GOUDA_LCD_CONFIG_T;

// =============================================================================
// HAL_GOUDA_SERIAL_LCD_CONFIG_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the GOUDA serial LCD interface.
// =============================================================================
typedef union {
    struct
    {
        bool isSlcd : 1;
        uint32_t deviceId : 6;
        uint32_t clkDivider : 8;
        uint32_t dummyCycle : 3;
        uint32_t lineType : 2;
        uint32_t rxByte : 3;
        bool isRead : 1;
    } b;

    uint32_t reg;
} HAL_GOUDA_SERIAL_LCD_CONFIG_T;

// =============================================================================
// HAL_GOUDA_LCD_CMD_T
// -----------------------------------------------------------------------------
/// Describes a command send to LCD
// =============================================================================
typedef struct
{
    /// \FALSE for a command, \TRUE for a data
    bool isData;
    /// value of data or command
    uint16_t value;
} HAL_GOUDA_LCD_CMD_T;

// =============================================================================
// HAL_GOUDA_WINDOW_T
// -----------------------------------------------------------------------------
/// Describes a window by a top left point
/// and a bottom right point
// =============================================================================
typedef struct
{
    /// top left point
    uint16_t tlPX;
    uint16_t tlPY;
    /// bottom right point
    uint16_t brPX;
    uint16_t brPY;

} HAL_GOUDA_WINDOW_T;

// =============================================================================
// HAL_GOUDA_IMG_FORMAT_T
// -----------------------------------------------------------------------------
/// Describes the image input format
// =============================================================================
typedef enum
{
    HAL_GOUDA_IMG_FORMAT_RGB565,
    HAL_GOUDA_IMG_FORMAT_RGBA,
    HAL_GOUDA_IMG_FORMAT_UYVY,
    HAL_GOUDA_IMG_FORMAT_YUYV,
    HAL_GOUDA_IMG_FORMAT_IYUV
} HAL_GOUDA_IMG_FORMAT_T;

// =============================================================================
// HAL_GOUDA_CKEY_MASK_T
// -----------------------------------------------------------------------------
/// Describes the color mask for Chroma Keying.
/// This allows a range of color as transparent.
// =============================================================================
typedef enum
{
    /// exact color match
    HAL_GOUDA_CKEY_MASK_OFF = 0,
    /// disregard 1 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_1LSB = 1,
    /// disregard 2 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_2LSB = 3,
    /// disregard 3 LSBit of each color component for matching
    HAL_GOUDA_CKEY_MASK_3LSB = 7
} HAL_GOUDA_CKEY_MASK_T;

// =============================================================================
// HAL_GOUDA_OVL_LAYER_ID_T
// -----------------------------------------------------------------------------
/// Describes the overlay layers Ids.
// =============================================================================
typedef enum
{
    HAL_GOUDA_OVL_LAYER_ID0,
    HAL_GOUDA_OVL_LAYER_ID1,
    HAL_GOUDA_OVL_LAYER_ID2,

    HAL_GOUDA_OVL_LAYER_ID_QTY
} HAL_GOUDA_OVL_LAYER_ID_T;

// =============================================================================
// HAL_GOUDA_OVL_LAYER_DEF_T
// -----------------------------------------------------------------------------
/// Describes the configuration of an overlay layer.
// =============================================================================
typedef struct
{
    ///@todo check with uint16_t* (?)
    uint32_t *addr;
    HAL_GOUDA_IMG_FORMAT_T fmt;
    /// buffer width in bytes including optional padding, when 0 will be calculated
    uint16_t stride;
    HAL_GOUDA_WINDOW_T pos;
    uint8_t alpha;
    bool cKeyEn;
    uint16_t cKeyColor;
    HAL_GOUDA_CKEY_MASK_T cKeyMask;
} HAL_GOUDA_OVL_LAYER_DEF_T;

// =============================================================================
// HAL_GOUDA_VID_LAYER_DEPTH_T
// -----------------------------------------------------------------------------
/// Describes the video layer depth
// =============================================================================
typedef enum
{
    /// Video layer behind all Overlay layers
    HAL_GOUDA_VID_LAYER_BEHIND_ALL = 0,
    /// Video layer between Overlay layers 1 and 0
    HAL_GOUDA_VID_LAYER_BETWEEN_1_0,
    /// Video layer between Overlay layers 2 and 1
    HAL_GOUDA_VID_LAYER_BETWEEN_2_1,
    /// Video layer on top of all Overlay layers
    HAL_GOUDA_VID_LAYER_OVER_ALL

} HAL_GOUDA_VID_LAYER_DEPTH_T;

// =============================================================================
// HAL_GOUDA_VID_LAYER_DEF_T
// -----------------------------------------------------------------------------
/// Describes the configuration of the video layer.
// =============================================================================
typedef struct
{
    uint32_t *addrY;
    uint32_t *addrU;
    uint32_t *addrV;
    HAL_GOUDA_IMG_FORMAT_T fmt;
    uint16_t height;
    uint16_t width;
    uint16_t stride;
    HAL_GOUDA_WINDOW_T pos;
    uint8_t alpha;
    bool cKeyEn;
    uint16_t cKeyColor;
    HAL_GOUDA_CKEY_MASK_T cKeyMask;
    HAL_GOUDA_VID_LAYER_DEPTH_T depth;
    uint8_t rotation;
} HAL_GOUDA_VID_LAYER_DEF_T;

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// halGoudaOpen
// -----------------------------------------------------------------------------
/// Open the LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and
/// the controller timings to use depending on the system clock frequency.
/// @param config Pointer to the controller configuration.
/// @param timings Controller timing array (must be provided and valid,
///   even if unused in case of #HAL_GOUDA_LCD_MEMORY_IF)
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
// =============================================================================
void halGoudaOpen(const HAL_GOUDA_LCD_CONFIG_T *config, const HAL_GOUDA_LCD_TIMINGS_T *timings, uint32_t ebcCsAddr);

// =============================================================================
// hal_GoudaSerialOpen
// -----------------------------------------------------------------------------
/// Open the serial LCD controller of Gouda
/// Parameters are used to describe the screen connexion configuration and
/// the controller timings to use depending on the system clock frequency.
/// @param spiLineType The SPI line type of the serial LCD.
/// @param spiFreq The SPI frequency of the serial LCD.
/// @param config Pointer to the controller configuration.
/// @param ebcCsAddr address of ebc CS for #HAL_GOUDA_LCD_MEMORY_IF,
///   can be 0 for gouda CS.
// =============================================================================
void halGoudaSerialOpen(HAL_GOUDA_SPI_LINE_T spiLineType, uint32_t spiFreq, const HAL_GOUDA_LCD_CONFIG_T *config, uint32_t ebcCsAddr);

void halGoudaSerialMode(HAL_GOUDA_SPI_LINE_T spiLineType);

// =============================================================================
// hal_GoudaClose
// -----------------------------------------------------------------------------
/// Close Gouda.
// =============================================================================
void halGoudaClose(void);

// =============================================================================
// hal_GoudaWriteReg
// -----------------------------------------------------------------------------
/// Write a command and then a data to the LCD screen (Or a value at a register
/// address, depending on the point of view).
///
/// @param addr Address of the register to write, or command to send
/// to the LCD screen
/// @param data Corresponding data to write to the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteReg(uint16_t addr, uint16_t data);

// =============================================================================
// halGoudaReadData
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
///
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @param len Data length to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaReadData(uint16_t addr, uint8_t *pData, uint32_t len);

// =============================================================================
// halGoudaReadReg
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
///
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaReadReg(uint16_t addr, uint16_t *pData);

// =============================================================================
// halGoudaWriteCmd
// -----------------------------------------------------------------------------
/// Write a command to the LCD.
///
/// @param addr Command to write.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteCmd(uint16_t addr);

// =============================================================================
// halGoudaWriteData
// -----------------------------------------------------------------------------
/// Write a data to the LCD screen.
///
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteData(uint16_t data);

// =============================================================================
// hal_GoudaBlitRoi
// -----------------------------------------------------------------------------
/// Send a block of pixel data to the LCD screen.
///
/// @param pRoi Pointer to the roi description to send to the screen.
/// @param nbCmd number of commands to send to LCD before pixel data
/// @param pCmds commands to send to LCD before pixel data can be \c NULL if \p nbCmd is 0
/// @param handler If not NULL, user function that is called when the transfer
/// is finished.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaBlitRoi(const HAL_GOUDA_WINDOW_T *pRoi, const uint32_t nbCmd, const HAL_GOUDA_LCD_CMD_T *pCmds, HAL_GOUDA_IRQ_HANDLER_T handler);

// =============================================================================
// hal_GoudaBlitRoi2Ram
// -----------------------------------------------------------------------------
/// Send a block of pixel data to a flat ram buffer in RGB565 format
/// WARNING: only even pixel number are supported, that is to say size must
/// be a multiple of 4 bytes.
///
/// @param pBuffer Pointer to the ram buffer
/// @param width Buffer width in pixel, if 0 the buffer is considered to have
/// the same width as the roi.
/// @param pRoi Pointer to the roi description to send to the screen.
/// @param handler If not NULL, user function that is called when the transfer
/// is finished.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaBlitRoi2Ram(uint32_t *pBuffer, const uint16_t width, const HAL_GOUDA_WINDOW_T *pRoi, HAL_GOUDA_IRQ_HANDLER_T handler);

// =============================================================================
// hal_GoudaIsActive
// -----------------------------------------------------------------------------
/// Get the activity status
/// @return \c TRUE when active
// =============================================================================
bool halGoudaIsActive(void);

// =============================================================================
// halGoudaSetBgColor
// -----------------------------------------------------------------------------
/// Set the background color
/// @param color in RGB565 format
// =============================================================================
void halGoudaSetBgColor(uint16_t color);

// =============================================================================
// halGoudaGetBgColor
// -----------------------------------------------------------------------------
/// Get the background color
/// @param color in RGB565 format
// =============================================================================
uint16_t halGoudaGetBgColor(void);

// =============================================================================
// halGoudaOvlLayerOpen
// -----------------------------------------------------------------------------
/// Open and configure an overlay layer.
///
/// @param id the layer Id
/// @param def the configuration
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the layer is already enabled.
// =============================================================================
HAL_ERR_T halGoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID_T id, HAL_GOUDA_OVL_LAYER_DEF_T *def);

// =============================================================================
// halGoudaOvlLayerClose
// -----------------------------------------------------------------------------
/// Close an overlay layer.
// =============================================================================
void halGoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID_T id);

// =============================================================================
// halGoudaVidLayerOpen
// -----------------------------------------------------------------------------
/// Open and configure the video layer.
///
/// @param def the configuration
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the layer is already enabled.
// =============================================================================
HAL_ERR_T halGoudaVidLayerOpen(HAL_GOUDA_VID_LAYER_DEF_T *def);

// =============================================================================
// halGoudaVidLayerClose
// -----------------------------------------------------------------------------
/// Close the video layer.
// =============================================================================
void halGoudaVidLayerClose(void);

// =============================================================================
// halGoudaVidLayerState
// -----------------------------------------------------------------------------
/// Get an video layer state.
// =============================================================================
uint32_t halGoudaVidLayerState(void);

// =============================================================================
// halGoudaInitResetPin
// -----------------------------------------------------------------------------
/// Initialize the status of LCD reset pin.
// =============================================================================
void hal_GoudaInitResetPin(void);

// =============================================================================
// halGoudaGetTiming
// -----------------------------------------------------------------------------
/// Get lcd timing.
// =============================================================================
uint32_t halGoudaGetTiming(void);

// =============================================================================
// halGoudaSetTiming
// -----------------------------------------------------------------------------
/// Set lcd timing.
// =============================================================================
void halGoudaSetTiming(uint32_t lcd_timing);

void halGoudatResetLcdPin(void);

// =============================================================================
// hal_GoudaSetBlock
// -----------------------------------------------------------------------------
/// Indicate whether to wait until gouda blit operations finish.
/// @param block The block flag, with a non-zero value to wait, 0 otherwise.
// =============================================================================
void halGoudaSetBlock(uint32_t block);

void halGoudaInit(HAL_GOUDA_LCD_CONFIG_T cfg, uint32_t clk);
void halGoudaReadConfig(uint16_t two_dataLine, bool fmark, uint32_t fmark_delay);

// =============================================================================
// hal_GoudaSetSpiFreq
// -----------------------------------------------------------------------------
/// Set SPI Frequency timing.
// =============================================================================
void hal_GoudaSetSpiFreq(uint32_t spiFreq);
extern void halGoudaSetFmarkBusy(bool busy);
void halGoudaClkDisable(void);
void halGoudaClkEnable(void);

/// @} end of group
#endif // _HAL_GOUDA_H_
