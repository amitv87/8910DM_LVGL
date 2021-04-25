/**
 * @file    bt_npi.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_NPI_H__
#define __BT_NPI_H__

#include "bluetooth/bt_types.h"
#include "bluetooth/bt_vendor.h"

extern hci_data_t *hci_buf_alloc(UINT32 size);

typedef struct
{
    UINT8 (*dut_mode_configure_callback) (UINT8 enable, UINT8 status);
    UINT8 (*dut_set_txpwr_callback) (UINT8 status);
    UINT8 (*dut_set_rxgain_callback) (UINT8 status);
    UINT8 (*set_nonsing_tx_test_mode_callback) (UINT8 enable, UINT8 status);
    UINT8 (*set_nonsing_rx_test_mode_callback) (UINT8 enable, UINT8 status);
    void (*get_nonsing_rx_data_callback) (UINT8 status, UINT32 rx_total_pkts, UINT32 rx_error_pkts, UINT32 rx_total_bits, UINT32 rx_error_bits, UINT8 rssi);
    void (*get_nonsing_le_rx_data_callback) (UINT8 status, UINT32 pkt_cnt, UINT32 pkt_err_cnt, UINT32 bit_cnt, UINT32 bit_err_cnt, UINT8 rssi);
    UINT8 (*le_receiver_test_v1_callback) (UINT8 status);
    UINT8 (*le_receiver_test_v2_callback) (UINT8 status);
    UINT8 (*le_receiver_test_v3_callback) (UINT8 status);
    UINT8 (*le_transmitter_test_v1_callback) (UINT8 status);
    UINT8 (*le_transmitter_test_v2_callback) (UINT8 status);
    UINT8 (*le_transmitter_test_v3_callback) (UINT8 status);
    UINT8 (*le_transmitter_test_v4_callback) (UINT8 status);
    UINT8 (*le_test_end_callback) (UINT8 status);
    UINT8 (*set_rf_path_callback) (UINT8 status);
    void (*get_rf_path_callback) (UINT8 status, UINT8 rf_path);
} btnpi_callbacks_t;


typedef struct
{
    bt_status_t (*init) (btnpi_callbacks_t *callbacks);
    bt_status_t (*dut_mode_configure)(UINT8 enable);
    bt_status_t (*dut_set_txpwr)(UINT8 txpwr_type, UINT16 value);
    bt_status_t (*dut_set_rxgain)(UINT8 value);
    bt_status_t (*set_nonsing_tx_test_mode)(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 pac_len,
                                            UINT16 power_type, UINT16 power_value, UINT16 pac_cnt);
    bt_status_t (*set_nonsing_rx_test_mode)(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 rx_gain,
                                            UINT8 *addr);
    bt_status_t (*get_nonsing_rx_data)(void);
    bt_status_t (*get_nonsing_le_rx_data)(void);
    bt_status_t (*le_receiver_test_v1)(UINT8 channel);
    bt_status_t (*le_receiver_test_v2)(UINT8 channel, UINT8 phy, UINT8 modulation_index);
    bt_status_t (*le_receiver_test_v3)(UINT8 channel, UINT8 phy, UINT8 modulation_index, UINT8 cte_len, UINT8 cte_type,
                                       UINT8 slot_durations, UINT8 switch_pattern_len, UINT8 *antenna_ids);
    bt_status_t (*le_transmitter_test_v1)(UINT8 channel, UINT8 length, UINT8 payload);
    bt_status_t (*le_transmitter_test_v2)(UINT8 channel, UINT8 length, UINT8 payload, UINT8 phy);
    bt_status_t (*le_transmitter_test_v3)(UINT8 channel, UINT8 length, UINT8 payload, UINT8 phy, UINT8 cte_len, UINT8 cte_type,
                                          UINT8 switch_pattern_len, UINT8 *antenna_ids);
    bt_status_t (*le_transmitter_test_v4)(UINT8 channel, UINT8 length, UINT8 payload, UINT8 phy, UINT8 cte_len, UINT8 cte_type,
                                          UINT8 switch_pattern_len, UINT8 *antenna_ids, UINT8 power_level);

    bt_status_t (*le_test_end)(void);
    bt_status_t (*set_rf_path)(UINT8 path);
    bt_status_t (*get_rf_path)();
    void (*dut_test_enable)(void);
} btnpi_interface_t;

#endif
