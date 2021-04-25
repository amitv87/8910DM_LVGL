/**
 * @file     npi_test.c
 * @brief
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2020-09-30
 * @license  Copyright (C) 2020 Unisoc Communications Inc
 */
#include "npi_test.h"
#include "bluetooth/bt_vendor.h"
#include "bluetooth/bt_at_npi.h"

BT_TEST_MODE_T bt_test_mode = BT_TESTMODE_NONE;
static const btnpi_interface_t *btapp_npi_int_interface = NULL;
const btnpi_interface_t *get_btnpi_interface(void);

void bt_npi_init(void)
{

    btapp_npi_int_interface = get_btnpi_interface();

}

void npi_stop_bt(void)
{
    if(bt_state_get() && (BT_SUCCESS != bt_stop()))
    {
        while(bt_state_get())
        {
            SCI_Sleep(100);
        }
    }
}

BOOLEAN npi_start_bt(void)
{
    if(!bt_state_get() && (BT_SUCCESS != bt_start()))
    {
        uint32 count = 0;

        while(!bt_state_get() && (count++ < 200))
        {
            SCI_Sleep(100);
        }
    }
    return bt_state_get();
}

void bt_test_mode_set(BT_TEST_MODE_T mode)
{
    SCI_TRACE_LOW("[bt] set test mode to %d", mode);
    bt_test_mode = mode;
}

BT_TEST_MODE_T bt_test_mode_get(void)
{
    return bt_test_mode;
}

BOOLEAN npi_enter_dut_mode(void)
{
    npi_start_bt();
    return BT_SUCCESS;

}

BT_STATUS bt_enter_eut_mode(void)
{
    bt_test_mode_set(BT_TESTMODE_SIG);//set this flag before BT Start
    if(npi_enter_dut_mode())
    {
        return BT_SUCCESS;
    }
    else
    {
        bt_test_mode_set(BT_TESTMODE_NONE);
        return BT_ERROR;
    }
}

BOOLEAN npi_leave_dut_mode(void)
{
    npi_stop_bt();
    bt_test_mode = BT_TESTMODE_NONE;

    return TRUE;
}

BT_STATUS bt_leave_eut_mode(void)//old in bt_dut.c line 106
{
    if(npi_leave_dut_mode())
    {
        bt_test_mode_set(BT_TESTMODE_NONE);
        return BT_SUCCESS;
    }
    else
    {
        return BT_ERROR;
    }
}

//judge the status now is  test mode or not
BOOLEAN is_test_mode(void)
{
    if(BT_TESTMODE_NONE != bt_test_mode_get())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//0-success 1-fail
BOOL bt_set_nonsing_tx_test_mode(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 pac_len,
                                            UINT16 power_type, UINT16 power_value, UINT16 pac_cnt, cmd_callback_t cb)
{
    BOOL status = FALSE;
    bt_status_t bt_status;

    if(btapp_npi_int_interface)
    {
        bt_status = btapp_npi_int_interface->set_nonsing_tx_test_mode(enable, le, pattern, channel, pac_type, pac_len, power_type, power_value, pac_cnt);

        if(bt_status == BT_SUCCESS)
        {
            status = TRUE;
        }

    }

    return status;
}

//0-success 1-fail
BOOL bt_set_nonsing_rx_test_mode(UINT16 enable, UINT16 le, UINT16 pattern, UINT16 channel, UINT16 pac_type, UINT16 rx_gain,
                                            UINT8 *addr, cmd_callback_t cb)
{
    BOOL status = FALSE;
    bt_status_t bt_status;

    if(btapp_npi_int_interface)
    {
        bt_status = btapp_npi_int_interface->set_nonsing_rx_test_mode(enable, le, pattern, channel, pac_type, rx_gain, addr);

        if(bt_status == BT_SUCCESS)
        {
            status = TRUE;
        }
    }

    return status;
}