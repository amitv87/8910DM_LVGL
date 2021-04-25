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

#ifndef _AIF_H_
#define _AIF_H_

// Auto generated (v1.0-22-ge2f5f70). Don't edit it manually!

#define AIF_RX_FIFO_SIZE (8)
#define AIF_TX_FIFO_SIZE (8)

typedef enum
{
    AIF_8K = 0,
    AIF_11K025 = 1,
    AIF_12K = 2,
    AIF_16K = 3,
    AIF_22K05 = 4,
    AIF_24K = 5,
    AIF_32K = 6,
    AIF_44K1 = 7,
    AIF_48K = 8,
} AIF_SAMPLING_RATE_T;

#define REG_AIF1_BASE (0x0881a000)
#define REG_AIF2_BASE (0x0881b000)

typedef volatile struct
{
    uint32_t data;           // 0x00000000
    uint32_t ctrl;           // 0x00000004
    uint32_t serial_ctrl;    // 0x00000008
    uint32_t tone;           // 0x0000000c
    uint32_t side_tone;      // 0x00000010
    uint32_t rx_load_pos;    // 0x00000014
    uint32_t fm_record_ctrl; // 0x00000018
} HWP_AIF_T;

#define hwp_aif1 ((HWP_AIF_T *)REG_ACCESS_ADDRESS(REG_AIF1_BASE))
#define hwp_aif2 ((HWP_AIF_T *)REG_ACCESS_ADDRESS(REG_AIF2_BASE))

// data
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data0 : 16; // [15:0]
        uint32_t data1 : 16; // [31:16]
    } b;
} REG_AIF_DATA_T;

// ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;           // [0]
        uint32_t __3_1 : 3;            // [3:1]
        uint32_t tx_off : 1;           // [4]
        uint32_t __7_5 : 3;            // [7:5]
        uint32_t parallel_out_set : 1; // [8], write set
        uint32_t parallel_out_clr : 1; // [9], write clear
        uint32_t parallel_in_set : 1;  // [10], write set
        uint32_t parallel_in_clr : 1;  // [11], write clear
        uint32_t tx_stb_mode : 1;      // [12]
        uint32_t __15_13 : 3;          // [15:13]
        uint32_t out_underflow : 1;    // [16], write clear
        uint32_t in_overflow : 1;      // [17], write clear
        uint32_t __30_18 : 13;         // [30:18]
        uint32_t loop_back : 1;        // [31]
    } b;
} REG_AIF_CTRL_T;

// serial_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t serial_mode : 2;           // [1:0]
        uint32_t i2s_in_sel : 2;            // [3:2]
        uint32_t master_mode : 1;           // [4]
        uint32_t lsb : 1;                   // [5]
        uint32_t lrck_pol : 1;              // [6]
        uint32_t lr_justified : 1;          // [7]
        uint32_t rx_dly : 2;                // [9:8]
        uint32_t tx_dly : 1;                // [10]
        uint32_t tx_dly_s : 1;              // [11]
        uint32_t tx_mode : 2;               // [13:12]
        uint32_t rx_mode : 1;               // [14]
        uint32_t __15_15 : 1;               // [15]
        uint32_t bck_lrck : 5;              // [20:16]
        uint32_t __23_21 : 3;               // [23:21]
        uint32_t bck_pol : 1;               // [24]
        uint32_t output_half_cycle_dly : 1; // [25]
        uint32_t input_half_cycle_dly : 1;  // [26]
        uint32_t __27_27 : 1;               // [27]
        uint32_t bckout_gate : 1;           // [28]
        uint32_t __31_29 : 3;               // [31:29]
    } b;
} REG_AIF_SERIAL_CTRL_T;

// tone
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_h : 1;      // [0]
        uint32_t tone_select : 1;   // [1]
        uint32_t __3_2 : 2;         // [3:2]
        uint32_t dtmf_freq_col : 2; // [5:4]
        uint32_t dtmf_freq_row : 2; // [7:6]
        uint32_t comfort_freq : 2;  // [9:8]
        uint32_t __11_10 : 2;       // [11:10]
        uint32_t tone_gain : 2;     // [13:12]
        uint32_t __31_14 : 18;      // [31:14]
    } b;
} REG_AIF_TONE_T;

// side_tone
typedef union {
    uint32_t v;
    struct
    {
        uint32_t side_tone_gain : 4; // [3:0]
        uint32_t __31_4 : 28;        // [31:4]
    } b;
} REG_AIF_SIDE_TONE_T;

// rx_load_pos
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rx_load_position : 4; // [3:0]
        uint32_t __31_4 : 28;          // [31:4]
    } b;
} REG_AIF_RX_LOAD_POS_T;

// fm_record_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t record_en : 1; // [0]
        uint32_t __3_1 : 3;     // [3:1]
        uint32_t lr_swap : 1;   // [4]
        uint32_t __31_5 : 27;   // [31:5]
    } b;
} REG_AIF_FM_RECORD_CTRL_T;

// data
#define AIF_DATA0(n) (((n)&0xffff) << 0)
#define AIF_DATA1(n) (((n)&0xffff) << 16)

// ctrl
#define AIF_ENABLE_ENABLE (1 << 0)
#define AIF_ENABLE_DISABLE (0 << 0)
#define AIF_TX_OFF_TX_ON (0 << 4)
#define AIF_TX_OFF_TX_OFF (1 << 4)
#define AIF_PARALLEL_OUT_SET_SERL (0 << 8)
#define AIF_PARALLEL_OUT_SET_PARA (1 << 8)
#define AIF_PARALLEL_OUT_CLR_SERL (0 << 9)
#define AIF_PARALLEL_OUT_CLR_PARA (1 << 9)
#define AIF_PARALLEL_IN_SET_SERL (0 << 10)
#define AIF_PARALLEL_IN_SET_PARA (1 << 10)
#define AIF_PARALLEL_IN_CLR_SERL (0 << 11)
#define AIF_PARALLEL_IN_CLR_PARA (1 << 11)
#define AIF_TX_STB_MODE (1 << 12)
#define AIF_OUT_UNDERFLOW (1 << 16)
#define AIF_IN_OVERFLOW (1 << 17)
#define AIF_LOOP_BACK_NORMAL (0 << 31)
#define AIF_LOOP_BACK_LOOPBACK (1 << 31)

#define AIF_ENABLE_V_ENABLE (1)
#define AIF_ENABLE_V_DISABLE (0)
#define AIF_TX_OFF_V_TX_ON (0)
#define AIF_TX_OFF_V_TX_OFF (1)
#define AIF_PARALLEL_OUT_SET_V_SERL (0)
#define AIF_PARALLEL_OUT_SET_V_PARA (1)
#define AIF_PARALLEL_OUT_CLR_V_SERL (0)
#define AIF_PARALLEL_OUT_CLR_V_PARA (1)
#define AIF_PARALLEL_IN_SET_V_SERL (0)
#define AIF_PARALLEL_IN_SET_V_PARA (1)
#define AIF_PARALLEL_IN_CLR_V_SERL (0)
#define AIF_PARALLEL_IN_CLR_V_PARA (1)
#define AIF_LOOP_BACK_V_NORMAL (0)
#define AIF_LOOP_BACK_V_LOOPBACK (1)

// serial_ctrl
#define AIF_SERIAL_MODE(n) (((n)&0x3) << 0)
#define AIF_SERIAL_MODE_I2S_PCM (0 << 0)
#define AIF_SERIAL_MODE_VOICE (1 << 0)
#define AIF_SERIAL_MODE_DAI (2 << 0)
#define AIF_I2S_IN_SEL(n) (((n)&0x3) << 2)
#define AIF_I2S_IN_SEL_I2S_IN_0 (0 << 2)
#define AIF_I2S_IN_SEL_I2S_IN_1 (1 << 2)
#define AIF_I2S_IN_SEL_I2S_IN_2 (2 << 2)
#define AIF_MASTER_MODE_SLAVE (0 << 4)
#define AIF_MASTER_MODE_MASTER (1 << 4)
#define AIF_LSB_MSB (0 << 5)
#define AIF_LSB_LSB (1 << 5)
#define AIF_LRCK_POL_LEFT_H_RIGHT_L (0 << 6)
#define AIF_LRCK_POL_LEFT_L_RIGHT_H (1 << 6)
#define AIF_LR_JUSTIFIED (1 << 7)
#define AIF_RX_DLY(n) (((n)&0x3) << 8)
#define AIF_RX_DLY_ALIGN (0 << 8)
#define AIF_RX_DLY_DLY_1 (1 << 8)
#define AIF_RX_DLY_DLY_2 (2 << 8)
#define AIF_RX_DLY_DLY_3 (3 << 8)
#define AIF_TX_DLY_ALIGN (0 << 10)
#define AIF_TX_DLY_DLY_1 (1 << 10)
#define AIF_TX_DLY_S_NO_DLY (0 << 11)
#define AIF_TX_DLY_S_DLY (1 << 11)
#define AIF_TX_MODE(n) (((n)&0x3) << 12)
#define AIF_TX_MODE_STEREO_STEREO (0 << 12)
#define AIF_TX_MODE_MONO_STEREO_CHAN_L (1 << 12)
#define AIF_TX_MODE_MONO_STEREO_DUPLI (2 << 12)
#define AIF_TX_MODE_STEREO_TO_MONO (3 << 12)
#define AIF_RX_MODE_STEREO_STEREO (0 << 14)
#define AIF_RX_MODE_STEREO_MONO_FROM_L (1 << 14)
#define AIF_BCK_LRCK(n) (((n)&0x1f) << 16)
#define AIF_BCK_LRCK_BCK_LRCK_16 (0 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_17 (1 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_18 (2 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_19 (3 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_20 (4 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_21 (5 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_22 (6 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_23 (7 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_24 (8 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_25 (9 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_26 (10 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_27 (11 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_28 (12 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_29 (13 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_30 (14 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_31 (15 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_32 (16 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_33 (17 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_34 (18 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_35 (19 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_36 (20 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_37 (21 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_38 (22 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_39 (23 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_40 (24 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_41 (25 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_42 (26 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_43 (27 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_44 (28 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_45 (29 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_46 (30 << 16)
#define AIF_BCK_LRCK_BCK_LRCK_47 (31 << 16)
#define AIF_BCK_POL_NORMAL (0 << 24)
#define AIF_BCK_POL_INVERT (1 << 24)
#define AIF_OUTPUT_HALF_CYCLE_DLY_NO_DLY (0 << 25)
#define AIF_OUTPUT_HALF_CYCLE_DLY_DLY (1 << 25)
#define AIF_INPUT_HALF_CYCLE_DLY_NO_DLY (0 << 26)
#define AIF_INPUT_HALF_CYCLE_DLY_DLY (1 << 26)
#define AIF_BCKOUT_GATE_NO_GATE (0 << 28)
#define AIF_BCKOUT_GATE_GATED (1 << 28)

#define AIF_SERIAL_MODE_V_I2S_PCM (0)
#define AIF_SERIAL_MODE_V_VOICE (1)
#define AIF_SERIAL_MODE_V_DAI (2)
#define AIF_I2S_IN_SEL_V_I2S_IN_0 (0)
#define AIF_I2S_IN_SEL_V_I2S_IN_1 (1)
#define AIF_I2S_IN_SEL_V_I2S_IN_2 (2)
#define AIF_MASTER_MODE_V_SLAVE (0)
#define AIF_MASTER_MODE_V_MASTER (1)
#define AIF_LSB_V_MSB (0)
#define AIF_LSB_V_LSB (1)
#define AIF_LRCK_POL_V_LEFT_H_RIGHT_L (0)
#define AIF_LRCK_POL_V_LEFT_L_RIGHT_H (1)
#define AIF_RX_DLY_V_ALIGN (0)
#define AIF_RX_DLY_V_DLY_1 (1)
#define AIF_RX_DLY_V_DLY_2 (2)
#define AIF_RX_DLY_V_DLY_3 (3)
#define AIF_TX_DLY_V_ALIGN (0)
#define AIF_TX_DLY_V_DLY_1 (1)
#define AIF_TX_DLY_S_V_NO_DLY (0)
#define AIF_TX_DLY_S_V_DLY (1)
#define AIF_TX_MODE_V_STEREO_STEREO (0)
#define AIF_TX_MODE_V_MONO_STEREO_CHAN_L (1)
#define AIF_TX_MODE_V_MONO_STEREO_DUPLI (2)
#define AIF_TX_MODE_V_STEREO_TO_MONO (3)
#define AIF_RX_MODE_V_STEREO_STEREO (0)
#define AIF_RX_MODE_V_STEREO_MONO_FROM_L (1)
#define AIF_BCK_LRCK_V_BCK_LRCK_16 (0)
#define AIF_BCK_LRCK_V_BCK_LRCK_17 (1)
#define AIF_BCK_LRCK_V_BCK_LRCK_18 (2)
#define AIF_BCK_LRCK_V_BCK_LRCK_19 (3)
#define AIF_BCK_LRCK_V_BCK_LRCK_20 (4)
#define AIF_BCK_LRCK_V_BCK_LRCK_21 (5)
#define AIF_BCK_LRCK_V_BCK_LRCK_22 (6)
#define AIF_BCK_LRCK_V_BCK_LRCK_23 (7)
#define AIF_BCK_LRCK_V_BCK_LRCK_24 (8)
#define AIF_BCK_LRCK_V_BCK_LRCK_25 (9)
#define AIF_BCK_LRCK_V_BCK_LRCK_26 (10)
#define AIF_BCK_LRCK_V_BCK_LRCK_27 (11)
#define AIF_BCK_LRCK_V_BCK_LRCK_28 (12)
#define AIF_BCK_LRCK_V_BCK_LRCK_29 (13)
#define AIF_BCK_LRCK_V_BCK_LRCK_30 (14)
#define AIF_BCK_LRCK_V_BCK_LRCK_31 (15)
#define AIF_BCK_LRCK_V_BCK_LRCK_32 (16)
#define AIF_BCK_LRCK_V_BCK_LRCK_33 (17)
#define AIF_BCK_LRCK_V_BCK_LRCK_34 (18)
#define AIF_BCK_LRCK_V_BCK_LRCK_35 (19)
#define AIF_BCK_LRCK_V_BCK_LRCK_36 (20)
#define AIF_BCK_LRCK_V_BCK_LRCK_37 (21)
#define AIF_BCK_LRCK_V_BCK_LRCK_38 (22)
#define AIF_BCK_LRCK_V_BCK_LRCK_39 (23)
#define AIF_BCK_LRCK_V_BCK_LRCK_40 (24)
#define AIF_BCK_LRCK_V_BCK_LRCK_41 (25)
#define AIF_BCK_LRCK_V_BCK_LRCK_42 (26)
#define AIF_BCK_LRCK_V_BCK_LRCK_43 (27)
#define AIF_BCK_LRCK_V_BCK_LRCK_44 (28)
#define AIF_BCK_LRCK_V_BCK_LRCK_45 (29)
#define AIF_BCK_LRCK_V_BCK_LRCK_46 (30)
#define AIF_BCK_LRCK_V_BCK_LRCK_47 (31)
#define AIF_BCK_POL_V_NORMAL (0)
#define AIF_BCK_POL_V_INVERT (1)
#define AIF_OUTPUT_HALF_CYCLE_DLY_V_NO_DLY (0)
#define AIF_OUTPUT_HALF_CYCLE_DLY_V_DLY (1)
#define AIF_INPUT_HALF_CYCLE_DLY_V_NO_DLY (0)
#define AIF_INPUT_HALF_CYCLE_DLY_V_DLY (1)
#define AIF_BCKOUT_GATE_V_NO_GATE (0)
#define AIF_BCKOUT_GATE_V_GATED (1)

// tone
#define AIF_ENABLE_H_DISABLE (0 << 0)
#define AIF_ENABLE_H_ENABLE (1 << 0)
#define AIF_TONE_SELECT_DTMF (0 << 1)
#define AIF_TONE_SELECT_COMFORT_TONE (1 << 1)
#define AIF_DTMF_FREQ_COL(n) (((n)&0x3) << 4)
#define AIF_DTMF_FREQ_COL_1209_HZ (0 << 4)
#define AIF_DTMF_FREQ_COL_1336_HZ (1 << 4)
#define AIF_DTMF_FREQ_COL_1477_HZ (2 << 4)
#define AIF_DTMF_FREQ_COL_1633_HZ (3 << 4)
#define AIF_DTMF_FREQ_ROW(n) (((n)&0x3) << 6)
#define AIF_DTMF_FREQ_ROW_697_HZ (0 << 6)
#define AIF_DTMF_FREQ_ROW_770_HZ (1 << 6)
#define AIF_DTMF_FREQ_ROW_852_HZ (2 << 6)
#define AIF_DTMF_FREQ_ROW_941_HZ (3 << 6)
#define AIF_COMFORT_FREQ(n) (((n)&0x3) << 8)
#define AIF_COMFORT_FREQ_425_HZ (0 << 8)
#define AIF_COMFORT_FREQ_950_HZ (1 << 8)
#define AIF_COMFORT_FREQ_1400_HZ (2 << 8)
#define AIF_COMFORT_FREQ_1800_HZ (3 << 8)
#define AIF_TONE_GAIN(n) (((n)&0x3) << 12)
#define AIF_TONE_GAIN_0_DB (0 << 12)
#define AIF_TONE_GAIN_M3_DB (1 << 12)
#define AIF_TONE_GAIN_M9_DB (2 << 12)
#define AIF_TONE_GAIN_M15_DB (3 << 12)

#define AIF_ENABLE_H_V_DISABLE (0)
#define AIF_ENABLE_H_V_ENABLE (1)
#define AIF_TONE_SELECT_V_DTMF (0)
#define AIF_TONE_SELECT_V_COMFORT_TONE (1)
#define AIF_DTMF_FREQ_COL_V_1209_HZ (0)
#define AIF_DTMF_FREQ_COL_V_1336_HZ (1)
#define AIF_DTMF_FREQ_COL_V_1477_HZ (2)
#define AIF_DTMF_FREQ_COL_V_1633_HZ (3)
#define AIF_DTMF_FREQ_ROW_V_697_HZ (0)
#define AIF_DTMF_FREQ_ROW_V_770_HZ (1)
#define AIF_DTMF_FREQ_ROW_V_852_HZ (2)
#define AIF_DTMF_FREQ_ROW_V_941_HZ (3)
#define AIF_COMFORT_FREQ_V_425_HZ (0)
#define AIF_COMFORT_FREQ_V_950_HZ (1)
#define AIF_COMFORT_FREQ_V_1400_HZ (2)
#define AIF_COMFORT_FREQ_V_1800_HZ (3)
#define AIF_TONE_GAIN_V_0_DB (0)
#define AIF_TONE_GAIN_V_M3_DB (1)
#define AIF_TONE_GAIN_V_M9_DB (2)
#define AIF_TONE_GAIN_V_M15_DB (3)

// side_tone
#define AIF_SIDE_TONE_GAIN(n) (((n)&0xf) << 0)

// rx_load_pos
#define AIF_RX_LOAD_POSITION(n) (((n)&0xf) << 0)

// fm_record_ctrl
#define AIF_RECORD_EN (1 << 0)
#define AIF_LR_SWAP (1 << 4)

#endif // _AIF_H_
