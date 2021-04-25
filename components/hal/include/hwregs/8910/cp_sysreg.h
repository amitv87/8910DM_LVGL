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

#ifndef _CP_SYSREG_H_
#define _CP_SYSREG_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#if defined(REG_ADDRESS_FOR_GGE)
#define REG_SYSREG_BASE (0x05080000)
#else
#define REG_SYSREG_BASE (0x50080000)
#endif

typedef volatile struct
{
    uint32_t zsp_svtaddr;          // 0x00000000
    uint32_t mem_ema_cfg_zsp;      // 0x00000004
    uint32_t mem_ema_cfg_lte;      // 0x00000008
    uint32_t rom_ema_cfg_lte;      // 0x0000000c
    uint32_t mem_ema_cfg_bbsys;    // 0x00000010
    uint32_t zsp_qos;              // 0x00000014
    uint32_t __24[1];              // 0x00000018
    uint32_t flow_para;            // 0x0000001c
    uint32_t rf_sel;               // 0x00000020
    uint32_t nb_lte_sel;           // 0x00000024
    uint32_t ggenb_sel;            // 0x00000028
    uint32_t rf_ana_26m_ctrl;      // 0x0000002c
    uint32_t rf_ana_26m_ctrl_set;  // 0x00000030
    uint32_t rf_ana_26m_ctrl_clr;  // 0x00000034
    uint32_t ddr_slp_ctrl_enable;  // 0x00000038
    uint32_t ddr_wakeup_force_en;  // 0x0000003c
    uint32_t ddr_wakeup_force_ack; // 0x00000040
    uint32_t ddr_slp_wait_number;  // 0x00000044
    uint32_t lvds_spi_sel;         // 0x00000048
    uint32_t monitor_lte_fint_sel; // 0x0000004c
    uint32_t rfmux_irq_sta;        // 0x00000050
    uint32_t apt_sel;              // 0x00000054
    uint32_t rfspi_mode_sel;       // 0x00000058
    uint32_t __92[1];              // 0x0000005c
    uint32_t ggenb_sys_ctrl;       // 0x00000060
    uint32_t a5_standbywfi_en;     // 0x00000064
} HWP_CP_SYSREG_T;

#define hwp_sysreg ((HWP_CP_SYSREG_T *)REG_ACCESS_ADDRESS(REG_SYSREG_BASE))

// zsp_svtaddr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __7_0 : 8;    // [7:0]
        uint32_t svtaddr : 24; // [31:8]
    } b;
} REG_CP_SYSREG_ZSP_SVTADDR_T;

// mem_ema_cfg_zsp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t radpd_rmea : 1; // [0]
        uint32_t radpd_rma : 4;  // [4:1]
        uint32_t radpd_rmeb : 1; // [5]
        uint32_t radpd_rmb : 4;  // [9:6]
        uint32_t raspu_rme : 1;  // [10]
        uint32_t raspu_rm : 4;   // [14:11]
        uint32_t rfspd_rme : 1;  // [15]
        uint32_t rfspd_rm : 4;   // [19:16]
        uint32_t rftpd_rmea : 1; // [20]
        uint32_t rftpd_rma : 4;  // [24:21]
        uint32_t rftpd_rmeb : 1; // [25]
        uint32_t rftpd_rmb : 4;  // [29:26]
        uint32_t __31_30 : 2;    // [31:30]
    } b;
} REG_CP_SYSREG_MEM_EMA_CFG_ZSP_T;

// mem_ema_cfg_lte
typedef union {
    uint32_t v;
    struct
    {
        uint32_t radpd_rmea : 1; // [0]
        uint32_t radpd_rma : 4;  // [4:1]
        uint32_t radpd_rmeb : 1; // [5]
        uint32_t radpd_rmb : 4;  // [9:6]
        uint32_t raspu_rme : 1;  // [10]
        uint32_t raspu_rm : 4;   // [14:11]
        uint32_t rfspd_rme : 1;  // [15]
        uint32_t rfspd_rm : 4;   // [19:16]
        uint32_t rftpd_rmea : 1; // [20]
        uint32_t rftpd_rma : 4;  // [24:21]
        uint32_t rftpd_rmeb : 1; // [25]
        uint32_t rftpd_rmb : 4;  // [29:26]
        uint32_t __31_30 : 2;    // [31:30]
    } b;
} REG_CP_SYSREG_MEM_EMA_CFG_LTE_T;

// rom_ema_cfg_lte
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rom_rmea : 1; // [0]
        uint32_t rom_rma : 4;  // [4:1]
        uint32_t __31_5 : 27;  // [31:5]
    } b;
} REG_CP_SYSREG_ROM_EMA_CFG_LTE_T;

// mem_ema_cfg_bbsys
typedef union {
    uint32_t v;
    struct
    {
        uint32_t radpd_rmea : 1; // [0]
        uint32_t radpd_rma : 4;  // [4:1]
        uint32_t radpd_rmeb : 1; // [5]
        uint32_t radpd_rmb : 4;  // [9:6]
        uint32_t raspu_rme : 1;  // [10]
        uint32_t raspu_rm : 4;   // [14:11]
        uint32_t rfspd_rme : 1;  // [15]
        uint32_t rfspd_rm : 4;   // [19:16]
        uint32_t rftpd_rmea : 1; // [20]
        uint32_t rftpd_rma : 4;  // [24:21]
        uint32_t rftpd_rmeb : 1; // [25]
        uint32_t rftpd_rmb : 4;  // [29:26]
        uint32_t __31_30 : 2;    // [31:30]
    } b;
} REG_CP_SYSREG_MEM_EMA_CFG_BBSYS_T;

// zsp_qos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t arqos_zsp_dbus : 4;   // [3:0]
        uint32_t awqos_zsp_dbus : 4;   // [7:4]
        uint32_t arqos_zsp_ibus : 4;   // [11:8]
        uint32_t awqos_zsp_ibus : 4;   // [15:12]
        uint32_t arqos_zsp_axidma : 4; // [19:16]
        uint32_t awqos_zsp_axidma : 4; // [23:20]
        uint32_t __31_24 : 8;          // [31:24]
    } b;
} REG_CP_SYSREG_ZSP_QOS_T;

// rf_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfsel : 1;   // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_RF_SEL_T;

// nb_lte_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nblte_sel : 1; // [0]
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_CP_SYSREG_NB_LTE_SEL_T;

// ggenb_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gge_nb_sel : 1; // [0]
        uint32_t __31_1 : 31;    // [31:1]
    } b;
} REG_CP_SYSREG_GGENB_SEL_T;

// rf_ana_26m_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_clk_26m_usb : 1;    // [0]
        uint32_t enable_clk_26m_dsi : 1;    // [1]
        uint32_t enable_clk_26m_audio : 1;  // [2]
        uint32_t muxsel_clk26m_audio : 1;   // [3]
        uint32_t enable_clk_26m_aud2ad : 1; // [4]
        uint32_t muxsel_clk26m_aud2ad : 1;  // [5]
        uint32_t enable_clk_26m_vad : 1;    // [6]
        uint32_t enable_clk_26m_adi : 1;    // [7]
        uint32_t enable_clk_26m_wcn : 1;    // [8]
        uint32_t __31_9 : 23;               // [31:9]
    } b;
} REG_CP_SYSREG_RF_ANA_26M_CTRL_T;

// rf_ana_26m_ctrl_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set : 9;     // [8:0], write set
        uint32_t __31_9 : 23; // [31:9]
    } b;
} REG_CP_SYSREG_RF_ANA_26M_CTRL_SET_T;

// rf_ana_26m_ctrl_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clear : 9;   // [8:0], write clear
        uint32_t __31_9 : 23; // [31:9]
    } b;
} REG_CP_SYSREG_RF_ANA_26M_CTRL_CLR_T;

// ddr_slp_ctrl_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;  // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_DDR_SLP_CTRL_ENABLE_T;

// ddr_wakeup_force_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;  // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_DDR_WAKEUP_FORCE_EN_T;

// ddr_wakeup_force_ack
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ack : 1;     // [0], read only
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_DDR_WAKEUP_FORCE_ACK_T;

// ddr_slp_wait_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t n : 16;       // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_CP_SYSREG_DDR_SLP_WAIT_NUMBER_T;

// lvds_spi_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel : 1;     // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_LVDS_SPI_SEL_T;

// monitor_lte_fint_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel : 1;     // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_MONITOR_LTE_FINT_SEL_T;

// rfmux_irq_sta
typedef union {
    uint32_t v;
    struct
    {
        uint32_t txfifo_full_irq : 1;    // [0], write clear
        uint32_t rfspi_conflict_irq : 1; // [1], write clear
        uint32_t __31_2 : 30;            // [31:2]
    } b;
} REG_CP_SYSREG_RFMUX_IRQ_STA_T;

// apt_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel : 1;     // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_APT_SEL_T;

// rfspi_mode_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel : 1;     // [0]
        uint32_t __31_1 : 31; // [31:1]
    } b;
} REG_CP_SYSREG_RFSPI_MODE_SEL_T;

// ggenb_sys_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ggenb_ctrl : 16; // [15:0]
        uint32_t __31_16 : 16;    // [31:16]
    } b;
} REG_CP_SYSREG_GGENB_SYS_CTRL_T;

// a5_standbywfi_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_a5_standby_en : 1; // [0]
        uint32_t cp_a5_standby_en : 1; // [1]
        uint32_t __31_2 : 30;          // [31:2]
    } b;
} REG_CP_SYSREG_A5_STANDBYWFI_EN_T;

// zsp_svtaddr
#define CP_SYSREG_SVTADDR(n) (((n)&0xffffff) << 8)

// mem_ema_cfg_zsp
#define CP_SYSREG_RADPD_RMEA (1 << 0)
#define CP_SYSREG_RADPD_RMA(n) (((n)&0xf) << 1)
#define CP_SYSREG_RADPD_RMEB (1 << 5)
#define CP_SYSREG_RADPD_RMB(n) (((n)&0xf) << 6)
#define CP_SYSREG_RASPU_RME (1 << 10)
#define CP_SYSREG_RASPU_RM(n) (((n)&0xf) << 11)
#define CP_SYSREG_RFSPD_RME (1 << 15)
#define CP_SYSREG_RFSPD_RM(n) (((n)&0xf) << 16)
#define CP_SYSREG_RFTPD_RMEA (1 << 20)
#define CP_SYSREG_RFTPD_RMA(n) (((n)&0xf) << 21)
#define CP_SYSREG_RFTPD_RMEB (1 << 25)
#define CP_SYSREG_RFTPD_RMB(n) (((n)&0xf) << 26)

// mem_ema_cfg_lte
#define CP_SYSREG_RADPD_RMEA (1 << 0)
#define CP_SYSREG_RADPD_RMA(n) (((n)&0xf) << 1)
#define CP_SYSREG_RADPD_RMEB (1 << 5)
#define CP_SYSREG_RADPD_RMB(n) (((n)&0xf) << 6)
#define CP_SYSREG_RASPU_RME (1 << 10)
#define CP_SYSREG_RASPU_RM(n) (((n)&0xf) << 11)
#define CP_SYSREG_RFSPD_RME (1 << 15)
#define CP_SYSREG_RFSPD_RM(n) (((n)&0xf) << 16)
#define CP_SYSREG_RFTPD_RMEA (1 << 20)
#define CP_SYSREG_RFTPD_RMA(n) (((n)&0xf) << 21)
#define CP_SYSREG_RFTPD_RMEB (1 << 25)
#define CP_SYSREG_RFTPD_RMB(n) (((n)&0xf) << 26)

// rom_ema_cfg_lte
#define CP_SYSREG_ROM_RMEA (1 << 0)
#define CP_SYSREG_ROM_RMA(n) (((n)&0xf) << 1)

// mem_ema_cfg_bbsys
#define CP_SYSREG_RADPD_RMEA (1 << 0)
#define CP_SYSREG_RADPD_RMA(n) (((n)&0xf) << 1)
#define CP_SYSREG_RADPD_RMEB (1 << 5)
#define CP_SYSREG_RADPD_RMB(n) (((n)&0xf) << 6)
#define CP_SYSREG_RASPU_RME (1 << 10)
#define CP_SYSREG_RASPU_RM(n) (((n)&0xf) << 11)
#define CP_SYSREG_RFSPD_RME (1 << 15)
#define CP_SYSREG_RFSPD_RM(n) (((n)&0xf) << 16)
#define CP_SYSREG_RFTPD_RMEA (1 << 20)
#define CP_SYSREG_RFTPD_RMA(n) (((n)&0xf) << 21)
#define CP_SYSREG_RFTPD_RMEB (1 << 25)
#define CP_SYSREG_RFTPD_RMB(n) (((n)&0xf) << 26)

// zsp_qos
#define CP_SYSREG_ARQOS_ZSP_DBUS(n) (((n)&0xf) << 0)
#define CP_SYSREG_AWQOS_ZSP_DBUS(n) (((n)&0xf) << 4)
#define CP_SYSREG_ARQOS_ZSP_IBUS(n) (((n)&0xf) << 8)
#define CP_SYSREG_AWQOS_ZSP_IBUS(n) (((n)&0xf) << 12)
#define CP_SYSREG_ARQOS_ZSP_AXIDMA(n) (((n)&0xf) << 16)
#define CP_SYSREG_AWQOS_ZSP_AXIDMA(n) (((n)&0xf) << 20)

// flow_para
#define CP_SYSREG_FLOWPARA(n) (((n)&0xffffffff) << 0)

// rf_sel
#define CP_SYSREG_RFSEL (1 << 0)

// nb_lte_sel
#define CP_SYSREG_NBLTE_SEL (1 << 0)

// ggenb_sel
#define CP_SYSREG_GGE_NB_SEL (1 << 0)

// rf_ana_26m_ctrl
#define CP_SYSREG_ENABLE_CLK_26M_USB (1 << 0)
#define CP_SYSREG_ENABLE_CLK_26M_DSI (1 << 1)
#define CP_SYSREG_ENABLE_CLK_26M_AUDIO (1 << 2)
#define CP_SYSREG_MUXSEL_CLK26M_AUDIO (1 << 3)
#define CP_SYSREG_ENABLE_CLK_26M_AUD2AD (1 << 4)
#define CP_SYSREG_MUXSEL_CLK26M_AUD2AD (1 << 5)
#define CP_SYSREG_ENABLE_CLK_26M_VAD (1 << 6)
#define CP_SYSREG_ENABLE_CLK_26M_ADI (1 << 7)
#define CP_SYSREG_ENABLE_CLK_26M_WCN (1 << 8)

// rf_ana_26m_ctrl_set
#define CP_SYSREG_SET(n) (((n)&0x1ff) << 0)

// rf_ana_26m_ctrl_clr
#define CP_SYSREG_CLEAR(n) (((n)&0x1ff) << 0)

// ddr_slp_ctrl_enable
#define CP_SYSREG_ENABLE (1 << 0)

// ddr_wakeup_force_en
#define CP_SYSREG_ENABLE (1 << 0)

// ddr_wakeup_force_ack
#define CP_SYSREG_ACK (1 << 0)

// ddr_slp_wait_number
#define CP_SYSREG_N(n) (((n)&0xffff) << 0)

// lvds_spi_sel
#define CP_SYSREG_SEL (1 << 0)

// monitor_lte_fint_sel
#define CP_SYSREG_SEL (1 << 0)

// rfmux_irq_sta
#define CP_SYSREG_TXFIFO_FULL_IRQ (1 << 0)
#define CP_SYSREG_RFSPI_CONFLICT_IRQ (1 << 1)

// apt_sel
#define CP_SYSREG_SEL (1 << 0)

// rfspi_mode_sel
#define CP_SYSREG_SEL (1 << 0)

// ggenb_sys_ctrl
#define CP_SYSREG_GGENB_CTRL(n) (((n)&0xffff) << 0)

// a5_standbywfi_en
#define CP_SYSREG_AP_A5_STANDBY_EN (1 << 0)
#define CP_SYSREG_CP_A5_STANDBY_EN (1 << 1)

#endif // _CP_SYSREG_H_
