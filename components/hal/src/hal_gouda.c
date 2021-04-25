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

#include "hal_chip.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_log.h"
#include "osi_profile.h"
#include "osi_byte_buf.h"
#include "hal_gouda.h"
#include "quec_proj_config.h"

#ifdef CONFIG_SOC_8910
uint32_t halGoudaEnterCriticalSection(void)
{
    uint32_t critical = osiEnterCritical();
    return critical;
}
void halGoudaExitCriticalSection(uint32_t status)
{
    osiExitCritical(status);
}

#define DEFERRED_TIMINGS_SET

static HAL_GOUDA_SPI_LINE_T g_line_type;
static uint16_t g_gouda_fmark = 0;
static uint32_t g_gouda_fmark_delay = 0x10000;

#define LCDD_SPI_FREQ_READ (500000)

typedef struct __gouda_context
{
    bool goudaOpened;
    bool fmarkBusy;
    HAL_GOUDA_IRQ_HANDLER_T userIrqHandler;
} goudaContext_t;

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================
static goudaContext_t g_gouda_ctx =
    {
        //.lcdClk = NULL,
        /*.goudaOpened = */ false,
        /*.fmarkBusy = */ false,
        /*.userIrqHandler = */ NULL,
};

/// Store the LCD configuration from tgt to add command value in blit
static HAL_GOUDA_LCD_CONFIG_T g_halGoudaLcdConfig;
static uint32_t g_halGoudaEbcCsAddr;

/// Store the timings
static const HAL_GOUDA_LCD_TIMINGS_T *g_halGoudaLcdTimings = NULL;

/// Store the serial LCD configuration from tgt to
/// add command value in blit
static HAL_GOUDA_SERIAL_LCD_CONFIG_T g_halGoudaSlcdCfg = {/*.reg = */ 0};
/// Store the SPI frequency for the serial LCD
static uint32_t g_halGoudaSpiFreq = 0;
static uint32_t g_halGoudaSpiFreqDivider = 0;

static uint32_t g_halGoudaBlock = 1;

bool halGoudaIsActive(void);

// ============================================================================
//  FUNCTIONS
// ============================================================================

void halGoudaSetFmarkBusy(bool busy)
{
    g_gouda_ctx.fmarkBusy = busy;
}
#define LCDD_SYS_FREQ (200000000)

// =============================================================================
// halGoudaUpdateTimings
// -----------------------------------------------------------------------------
/// Update the timings used by the Gouda module to fit optimally with the
/// system frequency. After the call to the #halGoudaOpen() function, an
/// array of timings, one for each system frequency, is registered in to the
/// HAL GOUDA LCD driver.
/// @param sysFreq (New) System frequency for which new timings must be
/// configured.
// =============================================================================
uint32_t halGoudaUpdateTimings(uint32_t sysFreq)
{
    return sysFreq;
}

void halGoudaClkEnable(void)
{
    hwp_sysCtrl->reg_dbg = 0xa50001; /*allow to set disable_reg*/
    REG_SYS_CTRL_CLK_SYS_AHB_ENABLE_T clk_sys_ahb_enable;
    REG_SYS_CTRL_CLK_AP_APB_ENABLE_T clk_ap_apb_enable;
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_ahb_enable, clk_sys_ahb_enable,
                     enable_sys_ahb_clk_id_gouda, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_ap_apb_enable, clk_ap_apb_enable,
                     enable_ap_apb_clk_id_gouda, 1);
}

void halGoudaClkDisable(void)
{
    hwp_sysCtrl->reg_dbg = 0xa50001; /*allow to set disable_reg*/
    REG_SYS_CTRL_CLK_SYS_AHB_DISABLE_T clk_sys_ahb_disable;
    REG_SYS_CTRL_CLK_AP_APB_DISABLE_T clk_ap_apb_disable;
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_ahb_disable, clk_sys_ahb_disable,
                     disable_sys_ahb_clk_id_gouda, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_ap_apb_disable, clk_ap_apb_disable,
                     disable_ap_apb_clk_id_gouda, 1);
}

// =============================================================================
// halGoudaUpdateSerialTiming
// -----------------------------------------------------------------------------
/// Update the timing used by the Gouda module to fit optimally with the
/// system frequency. After the call to the #halGoudaOpen() function,
/// the timing is registered in to the HAL GOUDA LCD driver.
/// @param sysFreq (New) System frequency for which new timing must be
/// configured.
// =============================================================================
uint32_t halGoudaUpdateSerialTiming(uint32_t sysFreq)
{
    uint32_t status;
    uint32_t divider = 250;
    if (g_halGoudaSpiFreq == 0)
        return 0;
    status = halGoudaEnterCriticalSection();
    if (g_halGoudaSpiFreq == LCDD_SPI_FREQ_READ)
    {
        divider = 250;
    }
    else
    {
        if (g_gouda_fmark == ENABLE_FMARK_LCD)
        {
            hwp_analogReg->pad_spi_lcd_cfg1 |= (((15) & 0xF) << 10);
            hwp_analogReg->pad_spi_lcd_cfg2 |= (((15) & 0xF) << 17) | (((15) & 0xF) << 3);
        }
        divider = LCDD_SYS_FREQ / sysFreq;
    }

    g_halGoudaSpiFreqDivider = divider;
    g_halGoudaSlcdCfg.b.clkDivider = divider;

    hwp_gouda->gd_spilcd_config = g_halGoudaSlcdCfg.reg;

    halGoudaExitCriticalSection(status);

    OSI_LOGI(0, "lcd:  gouda clk divider set to %d", divider);
    return g_halGoudaSpiFreq;
}

// =============================================================================
// halGoudaOpen
// -----------------------------------------------------------------------------
/// Common operations to open Gouda for a normal LCD or a serial LCD.
/// @param handler Function to update Gouda timings.
// =============================================================================

static void halGoudaCommonOpen(const HAL_GOUDA_LCD_CONFIG_T *config, uint32_t ebcCsAddr, HAL_SYS_FREQ_CHANGE_HANDLER_T handler)
{

    // handler(78000000);

    // Reset LCD after requesting freq since it cannot be interrupted by deep sleep
    g_halGoudaLcdConfig.b.resetb = false;
    hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg;

    osiDelayUS(100 * 1000);
    g_halGoudaLcdConfig.b.resetb = true;
    hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg;

    // This describes the way the LCD is plugged to the
    // controller.
    g_halGoudaLcdConfig.reg = config->reg;
    hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg;

    g_halGoudaEbcCsAddr = ebcCsAddr;

    hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;

    if (g_gouda_fmark == ENABLE_FMARK_LCD)
    {
        hwp_gouda->tecon = GOUDA_TE_EN | GOUDA_TE_EDGE_SEL;
        hwp_gouda->tecon2 = GOUDA_TE_COUNT1(g_gouda_fmark_delay); //GOUDA_TE_COUNT1(0x10000);
    }
    else
    {
        hwp_gouda->tecon = 0;
    }
}

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
void halGoudaOpen(const HAL_GOUDA_LCD_CONFIG_T *config, const HAL_GOUDA_LCD_TIMINGS_T *timings, uint32_t ebcCsAddr)
{
    // Set configuration register
    g_halGoudaSlcdCfg.b.isSlcd = false;
    hwp_gouda->gd_spilcd_config = g_halGoudaSlcdCfg.reg;
    // Register the timings
    g_halGoudaLcdTimings = timings;
    //halGoudaBlitRoi block default
    g_halGoudaBlock = 1;
    // Common open operations
    halGoudaCommonOpen(config, ebcCsAddr, halGoudaUpdateTimings);
    g_gouda_ctx.goudaOpened = true;
}

void halGoudaSetSpiFreq(uint32_t spiFreq)
{
    g_halGoudaSpiFreq = spiFreq;
    halGoudaUpdateSerialTiming(spiFreq); //hal_SysGetFreq());
}

void halSetGoudaClockMode(bool bAuto)
{
#define REG_WRITE_ENABLE 0xA50001
#define REG_WRITE_DISABLE 0xA50000
#define SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_MANUAL (1 << 0)

    hwp_sysCtrl->reg_dbg = REG_WRITE_ENABLE;

    if (bAuto)
        hwp_sysCtrl->clk_sys_ahb_mode &= ~SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_MANUAL;
    else
        hwp_sysCtrl->clk_sys_ahb_mode |= SYS_CTRL_MODE_SYS_AHB_CLK_ID_GOUDA_MANUAL;

    //hwp_sysCtrlAp->REG_DBG = REG_WRITE_DISABLE;
}
// =============================================================================
// halGoudaSerialOpen
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

void halGoudaSerialOpen(HAL_GOUDA_SPI_LINE_T spiLineType, uint32_t spiFreq, const HAL_GOUDA_LCD_CONFIG_T *config, uint32_t ebcCsAddr)
{
    OSI_ASSERT(config->b.outputFormat == HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565,
               "Serial interface only accepts 16-bit RGB565 output format");

    // Set configuration register
    g_halGoudaSlcdCfg.b.isSlcd = true;
    g_halGoudaSlcdCfg.b.clkDivider = 10;
    g_halGoudaSlcdCfg.b.lineType = spiLineType;
    g_halGoudaSlcdCfg.b.isRead = false;
    hwp_gouda->gd_spilcd_config = g_halGoudaSlcdCfg.reg;

    // Register the timings
    g_halGoudaSpiFreq = spiFreq;

    halGoudaCommonOpen(config, ebcCsAddr, halGoudaUpdateSerialTiming);
    g_gouda_ctx.goudaOpened = true;
    halGoudaSetSpiFreq(spiFreq);
}

void halGoudaSerialMode(HAL_GOUDA_SPI_LINE_T spiLineType)
{
    if (spiLineType == HAL_GOUDA_SPI_LINE_3_2_LANE)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_3_TWO_LANE;
    }
    else if (spiLineType == HAL_GOUDA_SPI_LINE_3)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_3;
    }
    else if (spiLineType == HAL_GOUDA_SPI_LINE_4)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_4;
    }
}

void halGoudaChangeSerialMode(HAL_GOUDA_SPI_LINE_T spiLineType)
{
    if (spiLineType == HAL_GOUDA_SPI_LINE_3_2_LANE)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_3_TWO_LANE;
    }
    else if (spiLineType == HAL_GOUDA_SPI_LINE_3)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_3;
    }
    else if (spiLineType == HAL_GOUDA_SPI_LINE_4)
    {
        hwp_gouda->gd_spilcd_config = (hwp_gouda->gd_spilcd_config & (~GOUDA_SPI_LINE(-1U))) | GOUDA_SPI_LINE_4;
    }
}

// =============================================================================
// halGoudaClose
// -----------------------------------------------------------------------------
/// Close Gouda.
// =============================================================================
void halGoudaClose(void)
{
    while (halGoudaIsActive())
        ;
    g_halGoudaSlcdCfg.reg = 0;
    hwp_gouda->gd_spilcd_config = g_halGoudaSlcdCfg.reg;

    g_halGoudaLcdConfig.reg = 0;
    hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg;
    g_gouda_ctx.userIrqHandler = NULL;
    g_gouda_ctx.goudaOpened = false;
}

// =============================================================================
// halGoudaWriteReg
// -----------------------------------------------------------------------------
/// Write a command and then a data to the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// @param addr Address of the register to write, or command to send
/// to the LCD screen
/// @param data Corresponding data to write to the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteReg(uint16_t addr, uint16_t data)
{
    uint32_t status;
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    OSI_ASSERT(g_gouda_ctx.goudaOpened == true, "can not call halGoudaWriteReg before gouda open !");
    status = halGoudaEnterCriticalSection();

    if ((hwp_gouda->gd_status & GOUDA_LCD_BUSY) != 0)
    {
        // GOUDA LCD is busy
        errorStatus = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Set configuration register and commands
        hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg; // from tgt
        // GOUDA LCD is available, we use it
        hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;
        hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_LCD_DATA(addr);
        // Unavoidable wait for the previous
        // command to be sent.
        while ((hwp_gouda->gd_status & GOUDA_LCD_BUSY))
            ;
        hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_TYPE | GOUDA_LCD_DATA(data);
        errorStatus = HAL_ERR_NO;
    }
    halGoudaExitCriticalSection(status);
    return errorStatus;
}

// =============================================================================
// halGoudaReadData
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @param len Data length to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================

HAL_ERR_T halGoudaReadData(uint16_t addr, uint8_t *pData, uint32_t len)
{
    uint32_t status;

    int i;
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    OSI_ASSERT(len > 0, "Invalid LCD read length:");
    OSI_ASSERT(g_gouda_ctx.goudaOpened == true, "can not call halGoudaReadData before gouda open !");
    if (g_halGoudaSlcdCfg.b.isSlcd)
    {
        OSI_ASSERT(len <= GD_MAX_SLCD_READ_LEN, "Invalid SLCD read length: %d (max: %d)");
    }

    status = halGoudaEnterCriticalSection();
    if ((hwp_gouda->gd_status & GOUDA_LCD_BUSY) != 0)
    {
        // GOUDA LCD is busy
        errorStatus = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Set configuration register and commands
        hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg; // from tgt
        // GOUDA LCD is available, we use it
        hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;

        if (g_halGoudaSlcdCfg.b.isSlcd)
        {
            HAL_GOUDA_SERIAL_LCD_CONFIG_T slcdCfg;
            slcdCfg.reg = g_halGoudaSlcdCfg.reg;
            slcdCfg.b.rxByte = len;
            slcdCfg.b.isRead = true;

            slcdCfg.b.dummyCycle = 1;

            if ((g_line_type == HAL_GOUDA_SPI_LINE_3_2_LANE) || (g_line_type == HAL_GOUDA_SPI_LINE_3))
            {
                slcdCfg.b.lineType = HAL_GOUDA_SPI_LINE_3;
            }
            else
            {
                slcdCfg.b.lineType = HAL_GOUDA_SPI_LINE_4;
            }
            slcdCfg.b.clkDivider = 250;

            hwp_gouda->gd_spilcd_config = slcdCfg.reg;
        }

        // Send register address
        hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_LCD_DATA(addr);
        while ((hwp_gouda->gd_status & GOUDA_LCD_BUSY))
            ;

        if (g_halGoudaSlcdCfg.b.isSlcd)
        {
            i = 0;
            while (i < len)
            {
                pData[i] = (hwp_gouda->gd_spilcd_rd >> (8 * i)) & 0xff;
                i++;
            }
            // Recover the original register context
            hwp_gouda->gd_spilcd_config = g_halGoudaSlcdCfg.reg;
        }
        else
        {
            // Start to read
            hwp_gouda->gd_lcd_single_access = GOUDA_START_READ | GOUDA_TYPE;
            while ((hwp_gouda->gd_status & GOUDA_LCD_BUSY))
                ;

            // Get the data
            i = 0;
            while (i < len)
            {
                if (1) //g_halCfg->goudaCfg.lcdData16Bit)
                {
                    pData[i] = hwp_gouda->gd_lcd_single_access & 0xff;
                    pData[i + 1] = (hwp_gouda->gd_lcd_single_access >> 8) & 0xff;
                    i += 2;
                }
                else
                {
                    pData[i] = hwp_gouda->gd_lcd_single_access & 0xff;
                    i++;
                }
                hwp_gouda->gd_lcd_single_access = GOUDA_START_READ | GOUDA_TYPE;
                while ((hwp_gouda->gd_status & GOUDA_LCD_BUSY))
                    ;
            }
        }

        errorStatus = HAL_ERR_NO;
    }

    halGoudaExitCriticalSection(status);
    return errorStatus;
}

// =============================================================================
// halGoudaReadReg
// -----------------------------------------------------------------------------
/// Read a data from the LCD screen (Or a value at a register
/// address, depending on the point of view).
/// @param addr Address of the register to read
/// @param pData Pointer of the corresponding data to read from the LCD.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the GOUDA LCD controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaReadReg(uint16_t addr, uint16_t *pData)
{
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    uint32_t len;
    OSI_ASSERT(g_gouda_ctx.goudaOpened == true, "can not call halGoudaReadReg before gouda open !");
    *pData = 0;
    if (g_halGoudaSlcdCfg.b.isSlcd)
    {
        len = 1;
    }
    else
    {
        len = 2;
    }
    errorStatus = halGoudaReadData(addr, (uint8_t *)pData, len);
    if (errorStatus == HAL_ERR_NO)
    {
        // Construct 16-bit register content for 8-bit interface
        if (1) // (!g_halCfg->goudaCfg.lcdData16Bit)
        {
            // Swap the byte order
            *pData = (((*pData) & 0xff) << 8) | (((*pData) >> 8) & 0xff);
        }
    }
    return errorStatus;
}

HAL_ERR_T halGoudaReadRegLength(uint16_t addr, uint8_t *pData, uint8_t length)
{
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    uint32_t len;
    *pData = 0;
    if (g_halGoudaSlcdCfg.b.isSlcd)
    {
        len = 1;
    }
    else
    {
        len = length;
    }
    errorStatus = halGoudaReadData(addr, (uint8_t *)pData, len);
    return errorStatus;
}

// =============================================================================
// halGoudaWriteCmd
// -----------------------------------------------------------------------------
/// Write a command to the LCD.
/// @param addr Command to write.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteCmd(uint16_t addr)
{
    uint32_t status;
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    DEFERRED_TIMINGS_SET;
    OSI_ASSERT(g_gouda_ctx.goudaOpened == true, "can not call halGoudaWriteCmd before gouda open !");
    status = halGoudaEnterCriticalSection();

    if ((hwp_gouda->gd_status & GOUDA_LCD_BUSY) != 0)
    {
        // GOUDA LCD is busy
        errorStatus = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Set configuration register and commands
        hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg; // from tgt
        // GOUDA LCD is available, we use it
        hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;
        hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_LCD_DATA(addr);
        errorStatus = HAL_ERR_NO;
    }
    halGoudaExitCriticalSection(status);
    return errorStatus;
}

// =============================================================================
// halGoudaWriteData
// -----------------------------------------------------------------------------
/// Write a data to the LCD screen.
///
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaWriteData(uint16_t data)
{
    uint32_t status;
    HAL_ERR_T errorStatus = HAL_ERR_NO;
    OSI_ASSERT(g_gouda_ctx.goudaOpened == true, "can not call halGoudaWriteData before gouda open !");
    status = halGoudaEnterCriticalSection();

    if ((hwp_gouda->gd_status & GOUDA_LCD_BUSY) != 0)
    {
        // GOUDA LCD is busy
        errorStatus = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Set configuration register and commands
        hwp_gouda->gd_lcd_ctrl = g_halGoudaLcdConfig.reg; // from tgt
        // GOUDA LCD is available, we use it
        hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;
        hwp_gouda->gd_lcd_single_access = GOUDA_START_WRITE | GOUDA_TYPE | GOUDA_LCD_DATA(data);
        errorStatus = HAL_ERR_NO;
    }
    halGoudaExitCriticalSection(status);
    return errorStatus;
}

// =============================================================================
// halGoudaBlitRoi
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
HAL_ERR_T halGoudaBlitRoi(const HAL_GOUDA_WINDOW_T *pRoi, const uint32_t nbCmd, const HAL_GOUDA_LCD_CMD_T *pCmds, HAL_GOUDA_IRQ_HANDLER_T handler)
{
    uint32_t status;

    // check ROI

    OSI_ASSERT((g_gouda_ctx.goudaOpened == true), "can not call halGoudaBlitRoi before gouda open !");
    OSI_ASSERT((pRoi != NULL), "halGoudaBlitRoi pRoi must not be NULL!");
    OSI_ASSERT(((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)),
               "halGoudaBlitRoi invalid, top point and bottom right point not ordered");
    osiDCacheCleanAll();

    // check Ready
    status = halGoudaEnterCriticalSection();

    if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) != 0)
    {
        // GOUDA is busy, abort
        halGoudaExitCriticalSection(status);
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        uint32_t i;
        HAL_GOUDA_LCD_CONFIG_T goudaLcdConfig;
        // GOUDA LCD is available, we use it

        hwp_gouda->gd_lcd_mem_address = g_halGoudaEbcCsAddr;

        // configure ROI in Gouda
        hwp_gouda->gd_roi_tl_ppos = GOUDA_X0(pRoi->tlPX) | GOUDA_Y0(pRoi->tlPY);
        hwp_gouda->gd_roi_br_ppos = GOUDA_X1(pRoi->brPX) | GOUDA_Y1(pRoi->brPY);

        // Set configuration register and commands
        goudaLcdConfig.reg = g_halGoudaLcdConfig.reg; // from tgt
        goudaLcdConfig.b.nbCmd = nbCmd;               // from driver
        hwp_gouda->gd_lcd_ctrl = goudaLcdConfig.reg;
        for (i = 0; i < nbCmd; i++)
        {
            // it usses addressing to configure the command/data bit in Gouda
            if (pCmds[i].isData)
            {
                // datas are after workbuf and commands
                ((uint16_t *)hwp_goudaSram)[GD_NB_WORKBUF_WORDS + GD_NB_LCD_CMD_WORDS + i] = pCmds[i].value;
            }
            else
            {
                // commands are after workbuf
                ((uint16_t *)hwp_goudaSram)[GD_NB_WORKBUF_WORDS + i] = pCmds[i].value;
            }
        }
        // Record handler and enable interrupt if needed.
        g_gouda_ctx.userIrqHandler = handler;
        if (handler != NULL)
        {
            // Set Mask
            hwp_gouda->gd_eof_irq_mask = GOUDA_EOF_MASK;
        }
        else
        {
            // Disable IRQ
            hwp_gouda->gd_eof_irq_mask = 0;
        }

        // Run GOUDA
        hwp_gouda->gd_command = GOUDA_START;

        // that's it
        halGoudaExitCriticalSection(status);

        return HAL_ERR_NO;
    }
}

// =============================================================================
// halGoudaBlitRoi2Ram
// -----------------------------------------------------------------------------
/// Send a block of pixel data to a flat ram buffer in RGB565 format
/// WARNING: only even pixel number are supported, that is to say size must
/// be a mutiple of 4 bytes.
///
/// @param pRoi Pointer to the roi description to send to the screen.
/// @param handler If not NULL, user function that is called when the transfer
/// is finished.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
HAL_ERR_T halGoudaBlitRoi2Ram(uint32_t *pBuffer, const uint16_t width, const HAL_GOUDA_WINDOW_T *pRoi, HAL_GOUDA_IRQ_HANDLER_T handler)
{
    uint32_t status;
    int32_t offset = 0; // default to 0 when width == 0

    OSI_ASSERT(g_gouda_ctx.goudaOpened, "can not call halGoudaBlitRoi2Ram before gouda open !");
    OSI_ASSERT(pRoi != NULL, "halGoudaBlitRoi2Ram pRoi must not be NULL!");
    OSI_ASSERT(((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)),
               "halGoudaBlitRoi2Ram roi invalid, top point and bottom right point not ordered");
    osiDCacheCleanAll();

    // compute offset
    if (width != 0)
    {
        int32_t roiWidth = pRoi->brPX - pRoi->tlPX + 1;
        OSI_ASSERT(width >= roiWidth, "halGoudaBlitRoi2Ram width (%d) must be greater or equal to width of roi (%d)");
        // offset is in byte
        offset = (width - roiWidth) * 2;
    }

    // check Ready
    status = halGoudaEnterCriticalSection();

    if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) != 0)
    {
        // GOUDA is busy, abort
        halGoudaExitCriticalSection(status);
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        HAL_GOUDA_LCD_CONFIG_T goudaLcdConfig;
        // GOUDA LCD is available, we use it

        // configure ROI in Gouda
        hwp_gouda->gd_roi_tl_ppos = GOUDA_X0(pRoi->tlPX) | GOUDA_Y0(pRoi->tlPY);
        hwp_gouda->gd_roi_br_ppos = GOUDA_X1(pRoi->brPX) | GOUDA_Y1(pRoi->brPY);

        // Set configuration register
        goudaLcdConfig.reg = g_halGoudaLcdConfig.reg; // from tgt
        goudaLcdConfig.b.cs = HAL_GOUDA_LCD_IN_RAM;
        goudaLcdConfig.b.outputFormat = HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565;
        hwp_gouda->gd_lcd_ctrl = goudaLcdConfig.reg;

        // Record handler and enable interrupt if needed.
        g_gouda_ctx.userIrqHandler = handler;
        if (handler != NULL)
        {
            // Set Mask
            hwp_gouda->gd_eof_irq_mask = GOUDA_EOF_MASK;
        }
        else
        {
            // Disable IRQ
            hwp_gouda->gd_eof_irq_mask = 0;
        }
        hwp_gouda->gd_lcd_mem_address = (uint32_t)pBuffer;
        hwp_gouda->gd_lcd_stride_offset = offset;
        // Run GOUDA
        hwp_gouda->gd_command = GOUDA_START;
        // that's it
        halGoudaExitCriticalSection(status);
        return HAL_ERR_NO;
    }
}

HAL_ERR_T hal_GoudaRun(void)
{
    // check Ready
    uint32_t status = halGoudaEnterCriticalSection();
    if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) != 0)
    {
        // GOUDA is busy, abort
        halGoudaExitCriticalSection(status);
        return HAL_ERR_RESOURCE_BUSY;
    }
    // Run GOUDA
    hwp_gouda->gd_command = GOUDA_START;

    // that's it
    halGoudaExitCriticalSection(status);

    return HAL_ERR_NO;
}
// =============================================================================
// halGoudaIsActive
// -----------------------------------------------------------------------------
/// Get the activity status
/// @return \c true when active
// =============================================================================
bool halGoudaIsActive(void)
{

    uint32_t status = halGoudaEnterCriticalSection();
    if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) != 0)
    {
        halGoudaExitCriticalSection(status);
        return true;
    }
    else
    {
        halGoudaExitCriticalSection(status);
        return false;
    }
}

// =============================================================================
// hal_GoudaSetBlock
// -----------------------------------------------------------------------------
/// Indicate whether to wait until gouda blit operations finish.
/// @param block The block flag, with a non-zero value to wait, 0 otherwise.
// =============================================================================
void hal_GoudaSetBlock(uint32_t block)
{
    g_halGoudaBlock = block;
}

// =============================================================================
// halGoudaSetBgColor
// -----------------------------------------------------------------------------
/// Set the background color
/// @param color in RGB565 format
// =============================================================================
void halGoudaSetBgColor(uint16_t color)
{
    hwp_gouda->gd_roi_bg_color = color;
}

// =============================================================================
// hal_GoudaGetBgColor
// -----------------------------------------------------------------------------
/// Get the background color
/// @param color in RGB565 format
// =============================================================================
uint16_t hal_GoudaGetBgColor(void)
{

    return hwp_gouda->gd_roi_bg_color;
}

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
HAL_ERR_T halGoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID_T id, HAL_GOUDA_OVL_LAYER_DEF_T *def)
{
    uint8_t wpp = (def->fmt == HAL_GOUDA_IMG_FORMAT_RGBA) ? 2 : 1;
    // Note that stride could be different in case of padding
    uint16_t stride;

    DEFERRED_TIMINGS_SET;

    OSI_ASSERT(id < HAL_GOUDA_OVL_LAYER_ID_QTY, "halGoudaOvlLayerOpen invalid ID:%d");
    if ((hwp_gouda->overlay_layer[id].gd_ol_input_fmt & GOUDA_ACTIVE) != 0)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    if (def->stride)
    {
        stride = def->stride;
    }
    else
    {
        stride = (def->pos.brPX - def->pos.tlPX + 1) * wpp;
    }

    OSI_ASSERT(!((def->pos.tlPX > def->pos.brPX) || (def->pos.tlPY > def->pos.brPY)),
               "halGoudaOvlLayerOpen pos invalid, top point and bottom right point not ordered");

    // TODO check supported formats
    hwp_gouda->overlay_layer[id].gd_ol_input_fmt =
        ((def->fmt == HAL_GOUDA_IMG_FORMAT_RGB565) ? GOUDA_FORMAT(0) : GOUDA_FORMAT(1)) |
        GOUDA_STRIDE(stride) |
        GOUDA_PREFETCH |
        GOUDA_ACTIVE;

    hwp_gouda->overlay_layer[id].gd_ol_tl_ppos =
        GOUDA_X0(def->pos.tlPX) | GOUDA_Y0(def->pos.tlPY);
    hwp_gouda->overlay_layer[id].gd_ol_br_ppos =
        GOUDA_X1(def->pos.brPX) | GOUDA_Y1(def->pos.brPY);
    hwp_gouda->overlay_layer[id].gd_ol_blend_opt =
        GOUDA_ALPHA(def->alpha) |
        GOUDA_CHROMA_KEY_COLOR(def->cKeyColor) |
        ((def->cKeyEn) ? GOUDA_CHROMA_KEY_ENABLE : 0) |
        GOUDA_CHROMA_KEY_MASK(def->cKeyMask);

    hwp_gouda->overlay_layer[id].gd_ol_rgb_src = (uint32_t)def->addr;

    g_gouda_ctx.goudaOpened = true;
    return HAL_ERR_NO;
}

// =============================================================================
// halGoudaOvlLayerClose
// -----------------------------------------------------------------------------
/// Close an overlay layer.
// =============================================================================
void halGoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID_T id)
{

    OSI_ASSERT(id < HAL_GOUDA_OVL_LAYER_ID_QTY, "halGoudaOvlLayerClose invalid ID:%d");
    hwp_gouda->overlay_layer[id].gd_ol_input_fmt = 0;
}

// =============================================================================
// hal_GoudaVidLayerOpen
// -----------------------------------------------------------------------------
/// Open and configure the video layer.
///
/// @param def the configuration
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the layer is already enabled.
// =============================================================================
HAL_ERR_T halGoudaVidLayerOpen(HAL_GOUDA_VID_LAYER_DEF_T *def)
{
    uint32_t xpitch, ypitch;
    uint8_t bpp = (def->fmt == HAL_GOUDA_IMG_FORMAT_IYUV) ? 1 : 2;
    // Note that stride could be different in case of padding
    uint16_t stride = def->stride ? def->stride : def->width * bpp;
    uint16_t outWidth = def->pos.brPX - def->pos.tlPX + 1;
    uint16_t outHeight = def->pos.brPY - def->pos.tlPY + 1;

    if ((hwp_gouda->gd_vl_input_fmt & GOUDA_ACTIVE) != 0)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    OSI_ASSERT(((def->pos.tlPX < def->pos.brPX) && (def->pos.tlPY < def->pos.brPY)),
               "hal_GoudaVidLayerOpen pos invalid, top point and bottom right point not ordered");

    xpitch = (def->width << 8) / outWidth;
    ypitch = (def->height << 8) / outHeight;
    hwp_gouda->gd_vl_extents = GOUDA_MAX_COL(def->width - 1) | GOUDA_MAX_LINE(def->height - 1);

    hwp_gouda->gd_vl_y_src = (uint32_t)def->addrY;
    hwp_gouda->gd_vl_u_src = (uint32_t)def->addrU;
    hwp_gouda->gd_vl_v_src = (uint32_t)def->addrV;

    OSI_ASSERT(((xpitch < 0x800) && (ypitch < 0x800)), "hal_GoudaVidLayerOpen image zoom ratio out of range (0x%0x,0x%0x)");

    hwp_gouda->gd_vl_resc_ratio = GOUDA_XPITCH(xpitch) | GOUDA_YPITCH(ypitch) | GOUDA_YPITCH_SCALE_ENABLE;

    // TODO check supported formats
    hwp_gouda->gd_vl_input_fmt =
        ((def->fmt == HAL_GOUDA_IMG_FORMAT_RGB565) ? GOUDA_FORMAT(0) : (def->fmt == HAL_GOUDA_IMG_FORMAT_UYVY) ? GOUDA_FORMAT(1) : (def->fmt == HAL_GOUDA_IMG_FORMAT_YUYV) ? GOUDA_FORMAT(2) : (def->fmt == HAL_GOUDA_IMG_FORMAT_IYUV) ? GOUDA_FORMAT(3) : 0) |
        GOUDA_STRIDE(stride) |
        GOUDA_ACTIVE;
    hwp_gouda->gd_vl_tl_ppos =
        GOUDA_X0(def->pos.tlPX) | GOUDA_Y0(def->pos.tlPY);
    hwp_gouda->gd_vl_br_ppos =
        GOUDA_X1(def->pos.brPX) | GOUDA_Y1(def->pos.brPY);

    hwp_gouda->gd_vl_blend_opt =
        GOUDA_DEPTH(def->depth) | GOUDA_ROTATION(def->rotation) |
        GOUDA_ALPHA(def->alpha) |
        GOUDA_CHROMA_KEY_COLOR(def->cKeyColor) |
        ((def->cKeyEn) ? GOUDA_CHROMA_KEY_ENABLE : 0) |
        GOUDA_CHROMA_KEY_MASK(def->cKeyMask);

    return HAL_ERR_NO;
}

// =============================================================================
// hal_GoudaVidLayerClose
// -----------------------------------------------------------------------------
/// Close the video layer.
// =============================================================================
void halGoudaVidLayerClose(void)
{

    hwp_gouda->gd_vl_input_fmt = 0;
}

// =============================================================================
// hal_GoudaVidLayerState
// -----------------------------------------------------------------------------
/// Get an video layer state.
// =============================================================================
uint32_t halGoudaVidLayerState(void)
{

    return hwp_gouda->gd_vl_input_fmt;
}

// =============================================================================
// hal_GoudaInitResetPin
// -----------------------------------------------------------------------------
/// Initialize the status of LCD reset pin.
// =============================================================================
void hal_GoudaInitResetPin(void)
{
}

void halGoudatResetLcdPin(void)
{
    REG_GOUDA_GD_LCD_CTRL_T ctrl_lcd;

    ctrl_lcd.v = hwp_gouda->gd_lcd_ctrl;

    if (ctrl_lcd.b.lcd_resetb == 1)
    {
        ctrl_lcd.b.lcd_resetb = 0;
        hwp_gouda->gd_lcd_ctrl = ctrl_lcd.v;
        osiDelayUS(20000);
        ctrl_lcd.b.lcd_resetb = 1;
        hwp_gouda->gd_lcd_ctrl = ctrl_lcd.v;
    }
    else
    {
        ctrl_lcd.b.lcd_resetb = 1;
        hwp_gouda->gd_lcd_ctrl = ctrl_lcd.v;
        osiDelayUS(20000);
        ctrl_lcd.b.lcd_resetb = 0;
        hwp_gouda->gd_lcd_ctrl = ctrl_lcd.v;
    }
}

// =============================================================================
// halGoudaIrqHandler
// -----------------------------------------------------------------------------
/// Handles IRQ.
// =============================================================================

static void halGoudaIrqHandler(void *interruptId)
{
    uint32_t tmpRead;

    hwp_gouda->gd_eof_irq = GOUDA_EOF_CAUSE; // Clear interrupt
    tmpRead = hwp_gouda->gd_eof_irq;

    // Call user handler
    if (g_gouda_ctx.userIrqHandler)
    {
        g_gouda_ctx.userIrqHandler();
    }
    if (tmpRead)
    {
    }
}

// =============================================================================
// halGoudaGetTiming
// -----------------------------------------------------------------------------
/// Get lcd timing.
// =============================================================================
uint32_t halGoudaGetTiming(void)
{
    return hwp_gouda->gd_lcd_timing;
}

// =============================================================================
// halGoudaSetTiming
// -----------------------------------------------------------------------------
/// Set lcd timing.
// =============================================================================
void halGoudaSetTiming(uint32_t gouda_timing)
{

    hwp_gouda->gd_lcd_timing = gouda_timing;
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_BOOT_LCD
void ql_boot_gouda_init(HAL_GOUDA_LCD_CONFIG_T cfg, uint32_t clk)
{
    OSI_LOGI(0, "lcd:  halGoudaInit clk=%d", clk);

    halSetGoudaClockMode(false);
//    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), SYS_IRQ_PRIO_GOUDA);
//    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), halGoudaIrqHandler, (void *)0);
//    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA));

    halGoudaSerialOpen(g_line_type, clk, &cfg, 0);
}
#endif

void halGoudaInit(HAL_GOUDA_LCD_CONFIG_T cfg, uint32_t clk)
{
    OSI_LOGI(0, "lcd:  halGoudaInit clk=%d", clk);

    halSetGoudaClockMode(false);
    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), SYS_IRQ_PRIO_GOUDA);
    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA), halGoudaIrqHandler, (void *)0);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_GOUDA));

    halGoudaSerialOpen(g_line_type, clk, &cfg, 0);
}

void halGoudaReadConfig(uint16_t two_dataLine, bool fmark, uint32_t fmark_delay)
{
    HAL_GOUDA_LCD_CONFIG_T gd_lcd_config = {0};

    gd_lcd_config.b.cs = HAL_GOUDA_LCD_CS_0;
    gd_lcd_config.b.outputFormat = HAL_GOUDA_LCD_OUTPUT_FORMAT_16_bit_RGB565;
    gd_lcd_config.b.cs0Polarity = false;
    gd_lcd_config.b.cs1Polarity = false;
    gd_lcd_config.b.resetb = true;
    gd_lcd_config.b.rsPolarity = false;
    gd_lcd_config.b.wrPolarity = false;
    gd_lcd_config.b.rdPolarity = false;
    gd_lcd_config.b.highByte = false;

    if (fmark == true)
    {
        g_gouda_fmark = 1;
    }
    else
    {
        g_gouda_fmark = 0;
    }

    g_gouda_fmark_delay = fmark_delay;

    halSetGoudaClockMode(false);

    if (two_dataLine == 3)
    {
        g_line_type = HAL_GOUDA_SPI_LINE_3_2_LANE;
        halGoudaSerialOpen(HAL_GOUDA_SPI_LINE_3, LCDD_SPI_FREQ_READ, &gd_lcd_config, 0);
    }
    else if (two_dataLine == 0)
    {
        g_line_type = HAL_GOUDA_SPI_LINE_4;
        halGoudaSerialOpen(g_line_type, LCDD_SPI_FREQ_READ, &gd_lcd_config, 0);
    }
    else if (two_dataLine == 1)
    {
        g_line_type = HAL_GOUDA_SPI_LINE_3;
        halGoudaSerialOpen(g_line_type, LCDD_SPI_FREQ_READ, &gd_lcd_config, 0);
    }
}

void hal_GoudaOvlLayerActive(HAL_GOUDA_OVL_LAYER_ID_T id)
{
    hwp_gouda->overlay_layer[id].gd_ol_input_fmt |= (1 << 31);
}

void hal_GoudaOvlLayerNoActive(HAL_GOUDA_OVL_LAYER_ID_T id)
{
    hwp_gouda->overlay_layer[id].gd_ol_input_fmt &= ~(1 << 31);
}

#endif
