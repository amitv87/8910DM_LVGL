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
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"
#include <stdlib.h>
#include <string.h>
#include "hal_chip.h"
#include "drv_i2c.h"
#include "drv_camera.h"
#include "drv_names.h"
#include "drv_ifc.h"
#include "image_sensor.h"
#include "drv_uart.h"

drvIfcChannel_t camp_ifc;

#define PLL_PIX_CLK 40000000
#define PLL_CSI_CLK 100000000

static void prvCamCtrlEnable(bool enable)
{
    REG_CAMERA_CTRL_T ctrl;
    ctrl.v = hwp_camera->ctrl;

    if (enable)
    {
        ctrl.b.enable = 1;
    }
    else
    {
        ctrl.b.enable = 0;
    }
    hwp_camera->ctrl = ctrl.v;
}

static void prvCamSpiRegInit(void)
{
    hwp_camera->spi_camera_reg0 = 0;
    hwp_camera->spi_camera_reg1 = 0;
    hwp_camera->spi_camera_reg2 = 0;
    hwp_camera->spi_camera_reg3 = 0;
    hwp_camera->spi_camera_reg4 = 0;
    hwp_camera->spi_camera_reg5 = 0;
    hwp_camera->spi_camera_reg6 = 0;

    hwp_camera->spi_camera_reg5 = 0x00ffffff;
    hwp_camera->spi_camera_reg6 = 0x01000240;
}

static void prvCamIomuxCfg(camInputSensor_t type)
{
    switch (type)
    {
    case camSpi_In:
    {
        REG_IOMUX_PAD_SPI_CAMERA_SI_0_CFG_REG_T pad_spi_camera_si_0_cfg_reg = {0};
        REG_IOMUX_PAD_SPI_CAMERA_SI_1_CFG_REG_T pad_spi_camera_si_1_cfg_reg = {0};
        REG_IOMUX_PAD_SPI_CAMERA_SCK_CFG_REG_T pad_spi_camera_sck_cfg_reg = {0};
        REG_IOMUX_PAD_CAMERA_RST_L_CFG_REG_T pad_camera_rst_l_cfg_reg = {0};
        REG_IOMUX_PAD_CAMERA_PWDN_CFG_REG_T pad_camera_pwdn_cfg_reg = {0};
        REG_IOMUX_PAD_CAMERA_REF_CLK_CFG_REG_T pad_camera_ref_clk_cfg_reg = {0};
        REG_IOMUX_PAD_I2C_M1_SCL_CFG_REG_T pad_i2c_m1_scl_cfg_reg = {0};
        REG_IOMUX_PAD_I2C_M1_SDA_CFG_REG_T pad_i2c_m1_sda_cfg_reg = {0};

        pad_spi_camera_si_0_cfg_reg.b.pad_spi_camera_si_0_sel = 0;
        hwp_iomux->pad_spi_camera_si_0_cfg_reg = pad_spi_camera_si_0_cfg_reg.v;

        pad_spi_camera_si_1_cfg_reg.b.pad_spi_camera_si_1_sel = 0;
        hwp_iomux->pad_spi_camera_si_1_cfg_reg = pad_spi_camera_si_1_cfg_reg.v;

        pad_spi_camera_sck_cfg_reg.b.pad_spi_camera_sck_sel = 0;
        hwp_iomux->pad_spi_camera_sck_cfg_reg = pad_spi_camera_sck_cfg_reg.v;

        pad_camera_rst_l_cfg_reg.b.pad_camera_rst_l_sel = 0;
        hwp_iomux->pad_camera_rst_l_cfg_reg = pad_camera_rst_l_cfg_reg.v;

        pad_camera_pwdn_cfg_reg.b.pad_camera_pwdn_sel = 0;
        hwp_iomux->pad_camera_pwdn_cfg_reg = pad_camera_pwdn_cfg_reg.v;

        pad_camera_ref_clk_cfg_reg.b.pad_camera_ref_clk_sel = 0;
        hwp_iomux->pad_camera_ref_clk_cfg_reg = pad_camera_ref_clk_cfg_reg.v;

        pad_i2c_m1_scl_cfg_reg.b.pad_i2c_m1_scl_sel = 0;
        hwp_iomux->pad_i2c_m1_scl_cfg_reg = pad_i2c_m1_scl_cfg_reg.v;

        pad_i2c_m1_sda_cfg_reg.b.pad_i2c_m1_sda_sel = 0;
        hwp_iomux->pad_i2c_m1_sda_cfg_reg = pad_i2c_m1_sda_cfg_reg.v;
    }
    break;
    case camCsi_In:
    {
        REG_IOMUX_PAD_CAMERA_RST_L_CFG_REG_T pad_camera_rst_l_cfg_reg = {0};
        REG_IOMUX_PAD_SPI_CAMERA_SCK_CFG_REG_T pad_spi_camera_sck_cfg_reg = {0};
        REG_IOMUX_PAD_SPI_CAMERA_SI_1_CFG_REG_T pad_spi_camera_si_1_cfg_reg = {0};
        REG_IOMUX_PAD_SPI_CAMERA_SI_0_CFG_REG_T pad_spi_camera_si_0_cfg_reg = {0};

        pad_camera_rst_l_cfg_reg.b.pad_camera_rst_l_oen_reg = 1;
        pad_camera_rst_l_cfg_reg.b.pad_camera_rst_l_oen_frc = 1;
        hwp_iomux->pad_camera_rst_l_cfg_reg = pad_camera_rst_l_cfg_reg.v;

        pad_spi_camera_sck_cfg_reg.b.pad_spi_camera_sck_oen_reg = 1;
        pad_spi_camera_sck_cfg_reg.b.pad_spi_camera_sck_oen_frc = 1;
        hwp_iomux->pad_spi_camera_sck_cfg_reg = pad_spi_camera_sck_cfg_reg.v;

        pad_spi_camera_si_1_cfg_reg.b.pad_spi_camera_si_1_oen_reg = 1;
        pad_spi_camera_si_1_cfg_reg.b.pad_spi_camera_si_1_oen_frc = 1;
        hwp_iomux->pad_spi_camera_si_1_cfg_reg = pad_spi_camera_si_1_cfg_reg.v;

        pad_spi_camera_si_0_cfg_reg.b.pad_spi_camera_si_0_oen_reg = 1;
        pad_spi_camera_si_0_cfg_reg.b.pad_spi_camera_si_0_oen_frc = 1;
        hwp_iomux->pad_spi_camera_si_0_cfg_reg = pad_spi_camera_si_0_cfg_reg.v;
    }
    break;
    default:
        break;
    }
}

void drvCamSetMCLK(cameraClk_t Mclk)
{
    REG_SYS_CTRL_CLK_AP_APB_ENABLE_T clk_ap_apb_enable;
    REG_SYS_CTRL_CLK_OTHERS_ENABLE_T clk_others_enable;
    REG_SYS_CTRL_SEL_CLOCK_T sel_clock;

    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK; // unlock
    clk_ap_apb_enable.v = hwp_sysCtrl->clk_ap_apb_enable;
    clk_ap_apb_enable.b.enable_ap_apb_clk_id_mod_camera = 1;
    clk_ap_apb_enable.b.enable_ap_apb_clk_id_camera = 1;
    hwp_sysCtrl->clk_ap_apb_enable = clk_ap_apb_enable.v;

    clk_others_enable.v = hwp_sysCtrl->clk_others_enable;
    clk_others_enable.b.enable_oc_id_pix = 1;
    clk_others_enable.b.enable_oc_id_isp = 1;
    clk_others_enable.b.enable_oc_id_spicam = 1;
    clk_others_enable.b.enable_oc_id_cam = 1;
    hwp_sysCtrl->clk_others_enable = clk_others_enable.v;

    sel_clock.v = hwp_sysCtrl->sel_clock;
    sel_clock.b.sys_sel_fast = SYS_CTRL_SYS_SEL_FAST_V_FAST;
    hwp_sysCtrl->sel_clock = sel_clock.v;
    osiDelayUS(10);

    halCameraClockRequest(CLK_CAMA_USER_CAMERA, Mclk);
    osiDelayUS(40);
}

void drvCamDisableMCLK(void)
{
    REG_SYS_CTRL_CLK_AP_APB_DISABLE_T clk_ap_apb_disable;
    REG_SYS_CTRL_CLK_OTHERS_DISABLE_T clk_others_disable;

    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;
    clk_ap_apb_disable.v = 0;
    clk_ap_apb_disable.b.disable_ap_apb_clk_id_mod_camera = 1;
    hwp_sysCtrl->clk_ap_apb_disable = clk_ap_apb_disable.v;

    clk_others_disable.v = 0;
    clk_others_disable.b.disable_oc_id_pix = 1;
    clk_others_disable.b.disable_oc_id_isp = 1;
    clk_others_disable.b.disable_oc_id_spicam = 1;
    clk_others_disable.b.disable_oc_id_cam = 1;
    hwp_sysCtrl->clk_others_disable = clk_others_disable.v;

    halCameraClockRelease(CLK_CAMA_USER_CAMERA);
    osiDelayUS(30);
}

void drvCamInitIfc(void)
{
    if (drvIfcChannelInit(&camp_ifc, DRV_NAME_CAMERA1, DRV_IFC_RX))
    {
        drvIfcRequestChannel(&camp_ifc);
    }
}

bool drvWaitCamIfcDone(void)
{
    int64_t startTime = osiUpTimeUS();

    while (!(drvIfcGetTC(&camp_ifc) == 0))
    {
        if (osiUpTimeUS() - startTime > 1000000)
        {
            OSI_LOGE(0x10007f4f, "READ timeout\n");
            return false;
        }
    }
    return true;
}

void drvCampStartTransfer(uint32_t size, uint8_t *data)
{
    uint32_t critical = osiEnterCritical();
    drvIfcFlush(&camp_ifc);
    osiDCacheInvalidate(data, size);
    drvIfcStart(&camp_ifc, data, size);
    osiExitCritical(critical);
}

bool drvCampStopTransfer(uint32_t size, uint8_t *data)
{
    //drvIfcStop(&camp_ifc);

    uint32_t tc = 0;
    uint32_t critical = osiEnterCritical();
    tc = drvIfcGetTC(&camp_ifc);
    OSI_LOGI(0x10007f50, "cam drvCampStopTransfer tc %d", tc);
    if (tc != 0)
    {
        return false;
    }
    osiDCacheInvalidate(data, size);
    osiExitCritical(critical);
    if (!(hwp_camera->irq_cause & (1 << 0)))
    {
        return true;
    }
    return false;
}

void drvCampIfcDeinit(void)
{
    drvIfcReleaseChannel(&camp_ifc);
}

void drvCamSetIrqHandler(osiIrqHandler_t hanlder, void *ctx)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_CAMERA);

    osiIrqDisable(irqn);
    osiIrqSetHandler(irqn, hanlder, ctx);
    osiIrqSetPriority(irqn, SYS_IRQ_PRIO_CAMERA);
    osiIrqEnable(irqn);
}

void drvCamDisableIrq(void)
{
    uint32_t irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_CAMERA);
    osiIrqDisable(irqn);
}

void drvCamCmdSetFifoRst(void)
{
    REG_CAMERA_CMD_SET_T cmd_set;
    cmd_set.v = hwp_camera->cmd_set;
    cmd_set.b.fifo_reset = 1; //
    hwp_camera->cmd_set = cmd_set.v;
}

void drvCamStopIfc(void)
{
    drvIfcStop(&camp_ifc);
}

void drvCamSetIrqMask(cameraIrqCause_t mask)
{
    REG_CAMERA_IRQ_MASK_T irq_mask;

    irq_mask.v = hwp_camera->irq_mask;
    irq_mask.b.ovfl = mask.overflow;
    irq_mask.b.vsync_r = mask.fstart;
    irq_mask.b.vsync_f = mask.fend;
    irq_mask.b.dma_done = mask.dma;

    hwp_camera->irq_mask = irq_mask.v;
}
void drvCamClrIrqMask(void)
{
    REG_CAMERA_IRQ_MASK_T irq_mask;

    irq_mask.v = hwp_camera->irq_mask;
    irq_mask.b.ovfl = 0;
    irq_mask.b.vsync_f = 0;
    irq_mask.b.vsync_r = 0;
    irq_mask.b.dma_done = 0;

    hwp_camera->irq_mask = irq_mask.v;
}

void drvCampRegInit(sensorInfo_t *config)
{
    REG_CAMERA_CTRL_T campCtrl;
    //REG_CAMERA_SCL_CONFIG_T campSclCfg;
    REG_CAMERA_DSTWINCOL_T campCol;
    REG_CAMERA_DSTWINROW_T campRow;
    REG_SYS_CTRL_SEL_CLOCK_T sel_clock;
    REG_SYS_CTRL_CFG_PLL_PIX_DIV_T cfg_pll_pix_div;
    REG_SYS_CTRL_CFG_PLL_CSI_DIV_T cfg_pll_csi_div;
    REG_SYS_CTRL_CLK_AP_APB_MODE_T clk_ap_apb_mode;
    REG_SYS_CTRL_CLK_OTHERS_ENABLE_T clk_others_enable;

    campCtrl.v = hwp_camera->ctrl;
    //campSclCfg.v = hwp_camera->scl_config;
    campCol.v = hwp_camera->dstwincol;
    campRow.v = hwp_camera->dstwinrow;
    sel_clock.v = hwp_sysCtrl->sel_clock;
    cfg_pll_pix_div.v = hwp_sysCtrl->cfg_pll_pix_div;
    clk_ap_apb_mode.v = hwp_sysCtrl->clk_ap_apb_mode;
    clk_others_enable.v = hwp_sysCtrl->clk_others_enable;

    if (config->scaleEnable)
    {
        REG_CAMERA_SCL_CONFIG_T campSclCfg = {};
        campSclCfg.v = hwp_camera->scl_config;
        campSclCfg.b.scale_row = (config->rowRatio & 3);
        campSclCfg.b.scale_col = (config->colRatio & 3);
        campSclCfg.b.scale_en = 1;
        hwp_camera->scl_config = campSclCfg.v;
    }
    else
    {
        /*
		hwp_camera->scl_config = 0;
		campSclCfg.b.data_out_swap = 1;
		hwp_camera->scl_config = campSclCfg.v;
		*/
        switch (config->rowRatio)
        {
        case ROW_RATIO_1_1:
            campCtrl.b.decimrow = 0;
            break;
        case ROW_RATIO_1_2:
            campCtrl.b.decimrow = 1;
            break;
        case ROW_RATIO_1_3:
            campCtrl.b.decimrow = 2;
            break;
        case ROW_RATIO_1_4:
            campCtrl.b.decimrow = 3;
            break;
        default:
            break;
        }
        switch (config->colRatio)
        {
        case ROW_RATIO_1_1:
            campCtrl.b.decimrow = 0;
            break;
        case ROW_RATIO_1_2:
            campCtrl.b.decimrow = 1;
            break;
        case ROW_RATIO_1_3:
            campCtrl.b.decimrow = 2;
            break;
        case ROW_RATIO_1_4:
            campCtrl.b.decimrow = 3;
            break;
        default:
            break;
        }
    }

    if (config->cropEnable)
    {
        campCtrl.v = hwp_camera->ctrl;
        campCtrl.b.cropen = 1;
        hwp_camera->ctrl = campCtrl.v;
    }
    if (!config->rstActiveH)
    {
        campCtrl.v = hwp_camera->ctrl;
        campCtrl.b.reset_pol = 1;
        hwp_camera->ctrl = campCtrl.v;
    }
    if (!config->pdnActiveH)
    {
        campCtrl.v = hwp_camera->ctrl;
        campCtrl.b.pwdn_pol = 1;
        hwp_camera->ctrl = campCtrl.v;
    }
    if (config->dropFrame)
    {
        campCtrl.v = hwp_camera->ctrl;
        campCtrl.b.decimfrm = 1;
        hwp_camera->ctrl = campCtrl.v;
    }
    campCtrl.v = hwp_camera->ctrl;
    campCtrl.b.vsync_drop = 0;
    campCtrl.b.vsync_pol = 0;
    hwp_camera->ctrl = campCtrl.v;

    switch (config->image_format)
    {
    case CAM_FORMAT_YUV:
        campCtrl.b.dataformat = 1;
        break;
    case CAM_FORMAT_RGB565:
        campCtrl.b.dataformat = 0;
        break;
    case CAM_FORMAT_RAW8:
    case CAM_FORMAT_RAW10:
        campCtrl.b.dataformat = 3;
        break;
    }
    hwp_camera->ctrl = campCtrl.v;

    campCol.b.dstwincolstart = config->dstWinColStart;
    campCol.b.dstwincolend = config->dstWinColEnd;
    hwp_camera->dstwincol = campCol.v;
    campRow.b.dstwinrowstart = config->dstWinRowStart;
    campRow.b.dstwinrowend = config->dstWinRowEnd;
    hwp_camera->dstwinrow = campRow.v;

    sel_clock.b.camera_clk_pll_sel = 0;
    hwp_sysCtrl->sel_clock = sel_clock.v;
    cfg_pll_pix_div.b.cfg_pll_pix_div_update = 1;
    cfg_pll_pix_div.b.cfg_pll_pix_div = ((CONFIG_DEFAULT_CPUPLL_FREQ / PLL_PIX_CLK & 0x7F) << 0);
    hwp_sysCtrl->cfg_pll_pix_div = cfg_pll_pix_div.v;
    cfg_pll_csi_div.b.cfg_pll_csi_div_update = 1;
    cfg_pll_csi_div.b.cfg_pll_csi_div = ((CONFIG_DEFAULT_CPUPLL_FREQ / PLL_CSI_CLK & 0x7F) << 0);
    hwp_sysCtrl->cfg_pll_csi_div = cfg_pll_csi_div.v;
    clk_ap_apb_mode.b.mode_ap_apb_clk_id_camera = 1;
    hwp_sysCtrl->clk_ap_apb_mode = clk_ap_apb_mode.v;
    clk_others_enable.b.enable_oc_id_pix = 1;
    hwp_sysCtrl->clk_others_enable = clk_others_enable.v;
    prvCamIomuxCfg(config->sensorType);
    if (config->sensorType == camSpi_In)
    {
        if (config->camSpiMode != SPI_MODE_NO)
        {
            prvCamSpiRegInit();
            REG_CAMERA_SPI_CAMERA_REG0_T reg0Cfg;
            REG_CAMERA_SPI_CAMERA_REG1_T reg1Cfg;
            REG_CAMERA_SPI_CAMERA_REG4_T reg4Cfg;

            reg0Cfg.v = hwp_camera->spi_camera_reg0;
            reg1Cfg.v = hwp_camera->spi_camera_reg1;
            reg4Cfg.v = hwp_camera->spi_camera_reg4;

            reg0Cfg.b.line_num_per_frame = config->spi_pixels_per_column;
            reg0Cfg.b.block_num_per_line = config->spi_pixels_per_line >> 1;
            reg0Cfg.b.yuv_out_format = config->camYuvMode;
            reg0Cfg.b.big_end_dis = (config->spi_little_endian_en) ? 1 : 0;
            //reg1Cfg.b.vsync_inv = 0;
            hwp_camera->spi_camera_reg0 = reg0Cfg.v;

            reg1Cfg.b.camera_clk_div_num = config->spi_ctrl_clk_div;
            reg1Cfg.b.sck_ddr_en = config->ddr_en;
            hwp_camera->spi_camera_reg1 = reg1Cfg.v;
            switch (config->camSpiMode)
            {
#ifdef CONFIG_QUEC_PROJECT_FEATURE_CAMERA
            case SPI_MODE_MASTER2_1:
                reg4Cfg.b.camera_spi_master_en_2 = 1;
                reg4Cfg.b.block_num_per_packet = config->spi_pixels_per_line >> 1;
                reg4Cfg.b.sdo_line_choose_bit = 0;
                reg4Cfg.b.data_size_choose_bit = 1;
                reg4Cfg.b.image_height_choose_bit = 1;
                reg4Cfg.b.image_width_choose_bit = 1;
                hwp_camera->spi_camera_reg4 = reg4Cfg.v;
                break;
#endif
            case SPI_MODE_MASTER2_2:
                reg4Cfg.b.camera_spi_master_en_2 = 1;
                reg4Cfg.b.block_num_per_packet = config->spi_pixels_per_line >> 1;
                reg4Cfg.b.sdo_line_choose_bit = 1;
                reg4Cfg.b.data_size_choose_bit = 1;
                reg4Cfg.b.image_height_choose_bit = 1;
                reg4Cfg.b.image_width_choose_bit = 1;
                hwp_camera->spi_camera_reg4 = reg4Cfg.v;
                break;
            default:
                break;
            }
            hwp_camera->csi_enable = 0;
        }
    }
    if (config->sensorType == camCsi_In)
    {
        REG_CAMERA_CSI_CONFIG_REG0_T cis_cfg0 = {};
        REG_CAMERA_CSI_CONFIG_REG1_T cis_cfg1 = {};
        REG_CAMERA_CSI_CONFIG_REG2_T cis_cfg2 = {};
        REG_CAMERA_CSI_CONFIG_REG3_T cis_cfg3 = {};
        REG_CAMERA_CSI_CONFIG_REG5_T cis_cfg5 = {};
        REG_CAMERA_CSI_CONFIG_REG6_T cis_cfg6 = {};
        REG_CAMERA_SPI_CAMERA_REG2_T spi_cfg2 = {};

        campCtrl.v = hwp_camera->ctrl;
        campCtrl.b.reorder = 2;
        hwp_camera->ctrl = campCtrl.v;
        cis_cfg0.v = hwp_camera->csi_config_reg0;
        cis_cfg0.b.num_d_term_en = 0;
        cis_cfg0.b.cur_frame_line_num = config->spi_pixels_per_column;
        cis_cfg0.b.hs_sync_find_en = 1;
        cis_cfg0.b.csi_module_enable = 1;
        hwp_camera->csi_config_reg0 = cis_cfg0.v;

        switch (config->image_format)
        {
        case CAM_FORMAT_YUV:
            cis_cfg1.b.num_hs_settle = 1;
            cis_cfg1.b.rinc_mask_en = 1;
            cis_cfg1.b.hs_enable_mask_en = 1;
            hwp_camera->csi_config_reg1 = cis_cfg1.v;
            cis_cfg2.b.num_hs_settle_clk = 16;
            cis_cfg2.b.num_c_term_en = 16;
            hwp_camera->csi_config_reg2 = cis_cfg2.v;
            cis_cfg3.b.single_data_lane_en = 1;
            hwp_camera->csi_config_reg3 = cis_cfg3.v;
            cis_cfg5.b.csi_rinc_new_mode_dis = 1;
            hwp_camera->csi_config_reg5 = cis_cfg5.v;
            cis_cfg6.b.data_type_dp_reg = 42;
            cis_cfg6.b.data_type_le_reg = 3;
            cis_cfg6.b.data_type_ls_reg = 2;
            cis_cfg6.b.data_type_fe_reg = 1;
            hwp_camera->csi_config_reg6 = cis_cfg6.v;
            spi_cfg2.b.eco_bypass_isp = 1;
            hwp_camera->spi_camera_reg2 = spi_cfg2.v;
            break;
        default:
            break;
        }
        hwp_camera->csi_enable = 1;
    }

    drvCamSetRst(false);
    drvCamInitIfc();
}

void drvCampRegDeInit(void)
{
    drvCamSetRst(true);
    hwp_camera->spi_camera_reg0 = 0;
    hwp_camera->spi_camera_reg1 = 0;
    hwp_camera->spi_camera_reg4 = 0;

    drvCameraControllerEnable(false);
    drvCampIfcDeinit();
}

void drvCamSetPdn(bool pdnActivH)
{
    REG_CAMERA_CMD_SET_T cmd_set;
    REG_CAMERA_CMD_CLR_T cmd_clr;

    cmd_set.v = hwp_camera->cmd_set;
    cmd_clr.v = hwp_camera->cmd_clr;

    if (pdnActivH)
    {
        cmd_set.b.pwdn = 1;
        hwp_camera->cmd_set = cmd_set.v;
    }
    else
    {
        cmd_clr.b.pwdn = 1;
        hwp_camera->cmd_clr = cmd_clr.v;
    }
}

void drvCamSetRst(bool rstActiveH)
{
    // Configures the Reset bit to the camera
    REG_CAMERA_CMD_SET_T cmd_set;
    REG_CAMERA_CMD_CLR_T cmd_clr;

    cmd_set.v = hwp_camera->cmd_set;
    cmd_clr.v = hwp_camera->cmd_clr;

    if (rstActiveH)
    {
        cmd_set.b.reset = 1;
        hwp_camera->cmd_set = cmd_set.v;
    }
    else
    {
        cmd_clr.b.reset = 1;
        hwp_camera->cmd_clr = cmd_clr.v;
    }
}

void drvCameraControllerEnable(bool enable)
{
    if (enable)
    {
        prvCamCtrlEnable(true);
    }
    else
    {
        prvCamCtrlEnable(false);
    }
}
