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
    hwp_lpddrPhy->lpddr_rf_cfg_phy = 0x1;
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ac = 0x1b;  //! ac line edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ds0 = 0x1b; //! output edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_1_wr_ds0 = 0x11; //! input pos edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_2_wr_ds0 = 0x11; //! input neg edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_3_wr_ds0 = 0x1b; //! gate edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_0_wr_ds1 = 0x1c; //! outupt edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_1_wr_ds1 = 0x12; //! input pos edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_2_wr_ds1 = 0x12; //! input neg edge
    hwp_lpddrPhy->lpddr_rf_cfg_dll_dl_3_wr_ds1 = 0x1c; //! gate edge
    hwp_lpddrPhy->lpddr_drf_cfg_reg_sel = 0x0;
    hwp_lpddrPhy->lpddr_drf_cfg_dqs_oe_sel_f0 = 0x1;
    hwp_lpddrPhy->lpddr_drf_cfg_dqs_out_sel_f0 = 0x4;
    hwp_lpddrPhy->lpddr_drf_cfg_data_oe_sel_f0 = 0x1;
    hwp_lpddrPhy->lpddr_drf_cfg_dqs_ie_sel_f0 = 0x7f;   //!
    hwp_lpddrPhy->lpddr_drf_cfg_dqs_gate_sel_f0 = 0xff; //!
    hwp_lpddrPhy->lpddr_drf_cfg_data_ie_sel_f0 = 0x2;   //!
    hwp_lpddrPhy->lpddr_drf_cfg_read_en_sel_f0 = 0x4;   //!
    hwp_lpddrPhy->lpddr_rf_cfg_ext = 0x401111;
    hwp_lpddrPhy->lpddr_drf_cfg = 0x1;
    hwp_lpddrPhy->lpddr_rf_cfg_sample_resync = 0x00010002;
    hwp_lpddrPhy->lpddr_drf_train_cfg = 1; // drf_phyupd_en/1
    hwp_lpddrPhy->lpddr_rf_cfg_clock_gate = 0x1f;
    hwp_lpddrPhy->lpddr_rf_cfg_phy = 0x3;

    hwp_analogReg->psram_pad_cfg = 0;
    hwp_lpddrPhy->io_rf_psram_pad_en_cfg = 0;
    hwp_lpddrPhy->io_rf_psram_drv_cfg = 0;
    hwp_lpddrPhy->io_rf_psram_pull_cfg = 0;
    hwp_lpddrPhy->io_rf_ddr_pad_cfg = 0;
    hwp_lpddrPhy->io_rf_ddr_rpull_cfg = 0x1;
    hwp_lpddrPhy->io_rf_ddr_vref_cfg = 0x2082; // ddr_vref_en_int/1, ddr_sel_int/32, ddr_sel_pad/32
    hwp_lpddrPhy->io_rf_ddr_zq_cfg = 0x20;     // ddr_cal_zq_pd/1
}

static inline void prvRamDmcCfg(void)
{
    hwp_dmcCtrl->format_control = 0x11000101; // 32x16, BURST_2_DDR_BL4, ACC_GRANU_2_DDR_4N, ALIGN_BOUNDARY_2_COL_2BIT
    hwp_dmcCtrl->address_control = 0x20201;   // 9_COL_BITS, 13_ROW_BITS, 2_BANK_BITS_4BK, 0_CHIP_BITS_1CS, 0_CHANNEL_BITS_1MEMIF
    hwp_dmcCtrl->decode_control = 0x22;       // CHIP_BANK_ROW_CHANNEL_COL, PAGE_ADDR_11_10
    hwp_dmcCtrl->t_mrr = 0x2;
    hwp_dmcCtrl->t_mrw = 0x5;
    hwp_dmcCtrl->t_xsr = 0x380038;
    hwp_dmcCtrl->t_rddata_en = 0x6;
    hwp_dmcCtrl->t_phywrlat = 0x103;
    hwp_dmcCtrl->t_rcd = 0x6;
    hwp_dmcCtrl->t_refi = 0x186;
    hwp_dmcCtrl->t_rfc = 0x240024;
    hwp_dmcCtrl->t_ras = 0x11;
    hwp_dmcCtrl->t_rp = 0x8;
    hwp_dmcCtrl->t_rpall = 0x8;
    hwp_dmcCtrl->t_rrd = 0x4;
    hwp_dmcCtrl->t_faw = 0x18;
    hwp_dmcCtrl->read_latency = 0xe;
    hwp_dmcCtrl->t_rtr = 0x2;
    hwp_dmcCtrl->t_rtw = 0x9;
    hwp_dmcCtrl->t_rtp = 0x3;
    hwp_dmcCtrl->write_latency = 0xa;
    hwp_dmcCtrl->t_wr = 0xc;
    hwp_dmcCtrl->t_wtr = 0xa000a;
    hwp_dmcCtrl->t_wtw = 0xc000c;
    hwp_dmcCtrl->t_eckd = 0xb;
    hwp_dmcCtrl->t_xckd = 0xb;
    hwp_dmcCtrl->t_ep = 0x2;
    hwp_dmcCtrl->t_xp = 0x30003;
    hwp_dmcCtrl->t_esr = 0x2;
    hwp_dmcCtrl->t_srckd = 0xb;
    hwp_dmcCtrl->t_cksrd = 0xb;
}

static inline void prvRamBootCfg(void)
{
    prvDelayUS(150);                      // t_PU >= 150 us
    hwp_dmcCtrl->direct_cmd = 0x0;        // NOPE
    hwp_dmcCtrl->direct_cmd = 0x1000003f; // MRS, MR63, reset
    hwp_dmcCtrl->direct_cmd = 0x5000ff0a;
    prvDelayUS(10); // t_RST >= 2 us
    hwp_dmcCtrl->direct_cmd = 0x10009201;
    hwp_dmcCtrl->direct_cmd = 0x10000402; //! MR2, RL=3, WL=1, LPDDR2-400M
    hwp_dmcCtrl->direct_cmd = 0x10000203; // MR3, 0x2: 40 ohm, 0x03: 48 ohm, 0x04: 60 ohm
    prvDelayUS(10);

    hwp_pwrctrl->ddr_hold_ctrl = 0;
    hwp_idle->ddr_latch = 2;

    hwp_dmcCtrl->memc_cmd = 3; // GO
    prvDelayUS(10);
}

static inline void prvRamWakeCfg(void)
{
    hwp_pwrctrl->ddr_hold_ctrl = 0;
    hwp_idle->ddr_latch = 2;

    hwp_dmcCtrl->memc_cmd = 3; // GO
    prvDelayUS(10);
}
