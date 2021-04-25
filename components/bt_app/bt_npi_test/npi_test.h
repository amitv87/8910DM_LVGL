/**
 * @file    npi_test.h
 * @brief
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */

#ifndef NPI_TEST_H
#define NPI_TEST_H

#include "bt_abs.h"
#include "bluetooth/bt_vendor.h"

/***************All enumerations *****************/
typedef enum
{
    BT_TESTMODE_NONE = 0,  // Leave Test mode
    BT_TESTMODE_SIG = 1,   // Enter EUT signal mode
    BT_TESTMODE_NONSIG = 2 // Enter Nonsignal mode
} BT_TEST_MODE_T;

//Test mode
void bt_npi_init(void);

void npi_stop_bt(void);

BOOLEAN npi_start_bt(void);

void bt_test_mode_set(BT_TEST_MODE_T mode);

BT_TEST_MODE_T bt_test_mode_get(void);

BT_STATUS bt_enter_eut_mode(void);

BOOLEAN npi_enter_dut_mode(void);

BT_STATUS bt_leave_eut_mode(void);

BOOLEAN npi_leaver_dut_mode(void);

BOOLEAN is_test_mode(void);

BOOL bt_set_nonsing_tx_test_mode(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 pac_len,
                                            UINT16 power_type, UINT16 power_value, UINT16 pac_cnt, cmd_callback_t cb);

BOOL bt_set_nonsing_rx_test_mode(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 rx_gain,
                                            UINT8 *addr, cmd_callback_t cb);

#endif
