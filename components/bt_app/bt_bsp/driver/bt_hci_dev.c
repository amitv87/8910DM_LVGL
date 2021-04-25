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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('B', 'T', 'U', 'A')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "bt_hci_dev.h"
#include "bt_app_config.h"
#include "drv_uart.h"
#include "drv_wcn_ipc.h"
#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"

#define BT_UART_WAIT_TXDONE_TIMEOUT (500)
#define BT_UART_RXBUF_SIZE (16384) //8192
#define BT_UART_TXBUF_SIZE (2048)

struct bt_hci_dev
{
#ifdef CONFIG_BT_COMM_USE_INT_UART
    drvUartCfg_t uart_cfg;
    drvUart_t *uart;
#endif
#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    drvWcnIpcUart_t *ipc_uart;
#endif
    bt_hci_dev_rx_avail_cb_t cb;
    void *cb_ctx;
};

static bt_hci_dev_t gHciDrv;

void bt_hci_dev_set_baud(unsigned baud)
{
#ifdef CONFIG_BT_COMM_USE_INT_UART
    gHciDrv.uart_cfg.baud = baud;
#endif
}

static void prv_event_cb(void *param, uint32_t event)
{
    bt_hci_dev_t *d = &gHciDrv;
    if ((event & DRV_UART_EVENT_RX_ARRIVED) && d->cb != NULL)
        d->cb(d, d->cb_ctx);

#ifdef CONFIG_BT_COMM_USE_INT_UART
    if (event & DRV_UART_EVENT_TX_COMPLETE)
    {
        REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_misc1_cfg, REG_SYS_CTRL_CFG_MISC1_CFG_T,
                          wcn_osc_en_ctrl, 1);
    }
#endif

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    if (event & DRV_UART_EVENT_TX_COMPLETE)
    {
        REGT_FIELD_CHANGE(hwp_pwrctrl->wcn_lps, REG_CP_PWRCTRL_WCN_LPS_T,
                          sys2wcn_wakeup, 0);
        REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_misc1_cfg, REG_SYS_CTRL_CFG_MISC1_CFG_T,
                          wcn_osc_en_ctrl, 1);
    }
#endif
}

bt_hci_dev_t *bt_hci_dev_open(bt_hci_dev_rx_avail_cb_t cb, void *cb_ctx)
{
    OSI_LOGI(0, "[BT UART] open");

    bt_hci_dev_t *d = &gHciDrv;
    d->cb = cb;
    d->cb_ctx = cb_ctx;

#ifdef CONFIG_BT_COMM_USE_INT_UART
    d->uart_cfg.data_bits = DRV_UART_DATA_BITS_8;
    d->uart_cfg.stop_bits = DRV_UART_STOP_BITS_1;
    d->uart_cfg.parity = DRV_UART_NO_PARITY;
    d->uart_cfg.rx_buf_size = BT_UART_RXBUF_SIZE;
    d->uart_cfg.tx_buf_size = BT_UART_TXBUF_SIZE;
    d->uart_cfg.cts_enable = true;
    d->uart_cfg.rts_enable = true;
    d->uart_cfg.event_mask = DRV_UART_EVENT_RX_ARRIVED | DRV_UART_EVENT_RX_OVERFLOW | DRV_UART_EVENT_TX_COMPLETE;
    d->uart_cfg.event_cb = prv_event_cb;
    d->uart_cfg.event_cb_ctx = NULL;
    d->uart = drvUartCreate(DRV_NAME_UART2, &d->uart_cfg);
    if (d->uart == NULL)
        return NULL;

    if (!drvUartOpen(d->uart))
    {
        drvUartDestroy(d->uart);
        d->uart = NULL;
        return NULL;
    }

    drvUartSetAutoSleep(d->uart, 500);
    return d;
#endif

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    d->ipc_uart = drvWcnIpcUartOpen(BT_UART_RXBUF_SIZE, prv_event_cb, NULL,
                                    DRV_UART_EVENT_RX_ARRIVED | DRV_UART_EVENT_TX_COMPLETE);

    return (d->ipc_uart == NULL) ? NULL : d;
#endif
}

void bt_hci_dev_close(bt_hci_dev_t *d)
{
    OSI_LOGI(0, "[BT UART] close");

#ifdef CONFIG_BT_COMM_USE_INT_UART
    drvUartClose(d->uart);
    drvUartDestroy(d->uart);
    d->uart = NULL;
#endif

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    drvWcnIpcUartClose(d->ipc_uart);
    d->ipc_uart = NULL;
#endif
}

int bt_hci_dev_read(bt_hci_dev_t *d, void *data, unsigned size)
{
#ifdef CONFIG_BT_COMM_USE_INT_UART
    return drvUartReceive(d->uart, data, size);
#endif
#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    return drvWcnIpcUartRead(d->ipc_uart, data, size);
#endif
}

int bt_hci_dev_write(bt_hci_dev_t *d, const void *data, unsigned size)
{
#ifdef CONFIG_BT_COMM_USE_INT_UART
    // bypass hardware clock gate control.
    REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_misc1_cfg, REG_SYS_CTRL_CFG_MISC1_CFG_T,
                      wcn_osc_en_ctrl, 0);

    // MAYBE a little bit delay is needed. When there is clock switch,
    // clock switch will need time.
    osiDelayUS(50);

    int trans = drvUartSendAll(d->uart, data, size, BT_UART_WAIT_TXDONE_TIMEOUT);
#endif

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    // bypass hardware clock gate control.
    REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_misc1_cfg, REG_SYS_CTRL_CFG_MISC1_CFG_T,
                      wcn_osc_en_ctrl, 0);

    int trans = drvWcnIpcUartWriteAll(d->ipc_uart, data, size, BT_UART_WAIT_TXDONE_TIMEOUT);

    // in case bt is sleep, this will wakeup bt.
    REGT_FIELD_CHANGE(hwp_pwrctrl->wcn_lps, REG_CP_PWRCTRL_WCN_LPS_T,
                      sys2wcn_wakeup, 1);
#endif

    OSI_LOGI(0, "[BT UART] write %u/%d", size, trans);
    if (trans != size)
        OSI_LOGE(0, "[BT UART] write fail %u/%d", size, trans);
    return trans;
}
