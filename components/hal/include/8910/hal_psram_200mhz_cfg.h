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

static void prvDelayUS(uint32_t us);

static inline void prvRamPhyPadCfg(void)
{
    // PSRAM PHY
    hwp_psramPhy->psram_rf_cfg_phy = 0x00000001;
    hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads0 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_0_wr_ads1 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads0 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads0 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads0 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_1_wr_ads1 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_2_wr_ads1 = 0x0000003b;
    hwp_psramPhy->psram_rf_cfg_dll_dl_3_wr_ads1 = 0x0000003b;
    hwp_psramPhy->psram_drf_cfg_reg_sel = 0x00000000;
    hwp_psramPhy->psram_drf_cfg_dqs_ie_sel_f0 = 0x00000001;
    hwp_psramPhy->psram_drf_cfg_dqs_oe_sel_f0 = 0x00000001;
    hwp_psramPhy->psram_drf_cfg_dqs_out_sel_f0 = 0x00000004;
    hwp_psramPhy->psram_drf_cfg_dqs_gate_sel_f0 = 0x0000000f;
    hwp_psramPhy->psram_drf_cfg_data_ie_sel_f0 = 0x00000002;
    hwp_psramPhy->psram_drf_cfg_data_oe_sel_f0 = 0x00000001;
    hwp_psramPhy->psram_drf_t_data_oe_ext = 0x00000011;
    hwp_psramPhy->psram_drf_t_dqs_oe_ext = 0x00000001;
    hwp_psramPhy->psram_drf_format_control = 0x00000001;
    hwp_psramPhy->psram_drf_t_rcd = 0x00000006;
    hwp_psramPhy->psram_drf_t_rddata_en = 0x00000006;
    hwp_psramPhy->psram_drf_t_rddata_late = 0x00000002;
    hwp_psramPhy->psram_drf_t_cph_wr = 0x00000006;
    hwp_psramPhy->psram_drf_t_cph_rd = 0x00000012;
    hwp_psramPhy->psram_drf_t_xphs = 0x0000000c;
    hwp_psramPhy->psram_drf_cfg = 0x00000001;
    hwp_psramPhy->psram_rf_cfg_clock_gate = 0x0000001f;
    hwp_psramPhy->psram_rf_cfg_phy = 0x00000003;

    // PSRAM PAD
    hwp_analogReg->ddr_pad_cfg = 0;
    hwp_lpddrPhy->io_rf_ddr_pad_cfg = 0;
    hwp_lpddrPhy->io_rf_psram_drv_cfg = 0x420;
    hwp_lpddrPhy->io_rf_psram_pull_cfg = 0x551; // dqs/1, dq/0, dm/1, clkn/1, clk/1, cen/1
    hwp_lpddrPhy->io_rf_psram_pad_en_cfg = 1;   // pad_clk_en/1
}

static inline void prvRamDmcCfg(void)
{
    hwp_dmcCtrl->format_control = 0x11000101; // 32x16, BURST_2_DDR_BL4, ACC_GRANU_2_DDR_4N, ALIGN_BOUNDARY_2_COL_2BIT
    hwp_dmcCtrl->address_control = 0x20201;   // 9_COL_BITS, 13_ROW_BITS, 2_BANK_BITS_4BK, 0_CHIP_BITS_1CS, 0_CHANNEL_BITS_1MEMIF
    hwp_dmcCtrl->decode_control = 0x20;       // CHANNEL_CHIP_ROW_BANK_COL, PAGE_ADDR_11_10
    hwp_dmcCtrl->t_refi = 0x40;
    hwp_dmcCtrl->t_rfc = 0x40004;
    hwp_dmcCtrl->t_mrr = 0x00000002;
    hwp_dmcCtrl->t_mrw = 0x00000005;
    hwp_dmcCtrl->t_rcd = 0x00000006;
    hwp_dmcCtrl->t_ras = 0x00000011;
    hwp_dmcCtrl->t_rp = 0x4;
    hwp_dmcCtrl->t_rpall = 0x4;
    hwp_dmcCtrl->t_rrd = 0x00000004;
    hwp_dmcCtrl->t_faw = 0x00000014;
    hwp_dmcCtrl->read_latency = 0x0000000e;
    hwp_dmcCtrl->t_rtr = 0x00000006;
    hwp_dmcCtrl->t_rtw = 0x0000000c;
    hwp_dmcCtrl->t_rtp = 0x00000008;
    hwp_dmcCtrl->write_latency = 0x0000000a;
    hwp_dmcCtrl->t_wr = 0x0000000c;
    hwp_dmcCtrl->t_wtr = 0x000a000a;
    hwp_dmcCtrl->t_wtw = 0x000c000c;
    hwp_dmcCtrl->t_eckd = 0x0000000b;
    hwp_dmcCtrl->t_xckd = 0x0000000b;
    hwp_dmcCtrl->t_ep = 0x00000002;
    hwp_dmcCtrl->t_xp = 0x00030003;
    hwp_dmcCtrl->t_esr = 0x00000002;
    hwp_dmcCtrl->t_xsr = 0x40004;
    hwp_dmcCtrl->t_srckd = 0x0000000b;
    hwp_dmcCtrl->t_cksrd = 0x0000000b;
    hwp_dmcCtrl->t_rddata_en = 0x00000006; // RL=6
    hwp_dmcCtrl->t_phywrlat = 0x00000103;
}

static inline void prvRamBootCfg(void)
{
    prvDelayUS(200);                      // t_PU >= 150 us
    hwp_dmcCtrl->direct_cmd = 0x0;        // NOPE
    prvDelayUS(10);                       // wait dmc toggle dfi clk
    hwp_dmcCtrl->direct_cmd = 0x1000003f; // MRS, MR63, reset
    prvDelayUS(10);                       // t_RST >= 2 us
    prvDelayUS(1000);                     // t_HSPU - tPU - tRST >= 1 ms
    hwp_dmcCtrl->direct_cmd = 0x10001b00;
    prvDelayUS(10);
    hwp_dmcCtrl->direct_cmd = 0x10008004; // MRS, MR4, WL=2, PSRAM-200M
    prvDelayUS(10);

    hwp_dmcCtrl->memc_cmd = 3; // GO
    prvDelayUS(10);
}

static inline void prvRamWakeCfg(void)
{
    hwp_pwrctrl->ddr_hold_ctrl = 0;
    hwp_idle->ddr_latch = 0;

    hwp_dmcCtrl->memc_cmd = DMC400_MEMC_CMD_GO;
    prvDelayUS(10);
}
