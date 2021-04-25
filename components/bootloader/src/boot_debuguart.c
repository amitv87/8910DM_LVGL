/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_debuguart.h"
#include "hwregs.h"
#include "osi_api.h"

// Timeout for TX fifo write. This timeout is to handle the case that
// debughost TX is blocked, and avoid infinite wait. The timeout value
// is the time there are no TX room.
#define TX_FIFO_WRITE_TIMEOUT_US (200)

void bootDebuguartInit(void)
{
    hwp_debugUart->irq_mask = 0;

    REG_DEBUG_UART_TRIGGERS_T triggers = {};
    triggers.b.rx_trigger = 7;
    triggers.b.tx_trigger = 12;
    triggers.b.afc_level = 10;
    hwp_debugUart->triggers = triggers.v;

    REG_DEBUG_UART_CTRL_T ctrl = {};
    ctrl.b.enable = 1;
    ctrl.b.data_bits = DEBUG_UART_DATA_BITS_V_8_BITS;
    ctrl.b.parity_enable = DEBUG_UART_PARITY_ENABLE_V_NO;
    ctrl.b.tx_stop_bits = DEBUG_UART_TX_STOP_BITS_V_1_BIT;
    ctrl.b.tx_break_control = 0;
    ctrl.b.rx_fifo_reset = 1;
    ctrl.b.tx_fifo_reset = 1;
    ctrl.b.dma_mode = 0;
    ctrl.b.swrx_flow_ctrl = 1;
    ctrl.b.swtx_flow_ctrl = 1;
    ctrl.b.backslash_en = 1;
    ctrl.b.tx_finish_n_wait = 0;
    ctrl.b.divisor_mode = 0;
    ctrl.b.irda_enable = 0;
    ctrl.b.rx_rts = 1;
    ctrl.b.auto_flow_control = 1;
    ctrl.b.loop_back_mode = 0;
    ctrl.b.rx_lock_err = 0;
    ctrl.b.hst_txd_oen = DEBUG_UART_HST_TXD_OEN_V_ENABLE;
    ctrl.b.rx_break_length = 11;
    hwp_debugUart->ctrl = ctrl.v;
}

int bootDebuguartWrite(const void *data, size_t size)
{
    REG_DEBUG_UART_STATUS_T status = {hwp_debugUart->status};
    uint32_t tx_fifo_level = status.b.tx_fifo_level;
    if (tx_fifo_level >= DEBUG_UART_TX_FIFO_SIZE)
        return 0;

    int bytes = DEBUG_UART_TX_FIFO_SIZE - tx_fifo_level;
    if (bytes > size)
        bytes = size;

    const uint8_t *data8 = (const uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        hwp_debugUart->rxtx_buffer = *data8++;
    return bytes;
}

void bootDebuguartWriteAll(const void *data, size_t size)
{
    osiElapsedTimer_t timer;
    osiElapsedTimerStart(&timer);

    while (size > 0)
    {
        if (osiElapsedTimeUS(&timer) > TX_FIFO_WRITE_TIMEOUT_US)
            return;

        int wsize = bootDebuguartWrite(data, size);
        if (wsize > 0)
        {
            size -= wsize;
            data = (const char *)data + wsize;
            osiElapsedTimerStart(&timer);
        }
    }
}
