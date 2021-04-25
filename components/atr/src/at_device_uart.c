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

// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include "osi_log.h"
#include "at_engine_imp.h"
#include "osi_api.h"
#include "drv_uart.h"
#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
#include "srv_2line_wakeup.h"
#endif
#include "quec_proj_config.h"
#include "quec_data_engine.h"

#define UART_RX_BUF_SIZE (4 * 1024)
#define UART_TX_BUF_SIZE (4 * 1024)

#define UART_WAIT_TX_DONE_TIMEOUT (500)

#ifndef CONFIG_QUEC_PROJECT_FEATURE_UART
typedef struct
{
    atDevice_t ops; // API
    drvUart_t *drv;
    atDeviceUartConfig_t config; // UART configuration
    osiWork_t *work;
    uint32_t pending_event;
} atDeviceUart_t;
#endif

static void _destroy(atDevice_t *th);
static bool _open(atDevice_t *th);
static void _close(atDevice_t *th);
static int _write(atDevice_t *th, const void *data, size_t length);
static int _read(atDevice_t *th, void *data, size_t size);

static inline drvUartParity_t _convParity(atDeviceParity_t parity)
{
    switch (parity)
    {
    case AT_DEVICE_PARITY_ODD:
        return DRV_UART_ODD_PARITY;
    case AT_DEVICE_PARITY_EVEN:
        return DRV_UART_EVEN_PARITY;
//8910 ARM UART hardware not support Mark Space
#ifndef CONFIG_SOC_8910
    case AT_DEVICE_PARITY_MARK:
        return DRV_UART_MARK_PARITY;
    case AT_DEVICE_PARITY_SPACE:
        return DRV_UART_SPACE_PARITY;
#endif
    default:
        return DRV_UART_NO_PARITY;
    }
}

static void _convFormat(drvUartCfg_t *cfg, atDeviceFormat_t format, atDeviceParity_t parity)
{
#ifndef CONFIG_SOC_8910
    cfg->data_bits = (format == AT_DEVICE_FORMAT_7N2 ||
                      format == AT_DEVICE_FORMAT_711 ||
                      format == AT_DEVICE_FORMAT_7N1)
                         ? DRV_UART_DATA_BITS_7
                         : DRV_UART_DATA_BITS_8;
    cfg->stop_bits = (format == AT_DEVICE_FORMAT_8N2 ||
                      format == AT_DEVICE_FORMAT_7N2)
                         ? DRV_UART_STOP_BITS_2
                         : DRV_UART_STOP_BITS_1;
    cfg->parity = (format == AT_DEVICE_FORMAT_811 ||
                   format == AT_DEVICE_FORMAT_711)
                      ? _convParity(parity)
                      : DRV_UART_NO_PARITY;
#else
    //8910 ARM UART hardware not support data bits 7
    cfg->data_bits = DRV_UART_DATA_BITS_8;
    cfg->stop_bits = (format == AT_DEVICE_FORMAT_8N2)
                         ? DRV_UART_STOP_BITS_2
                         : DRV_UART_STOP_BITS_1;
    cfg->parity = (format == AT_DEVICE_FORMAT_811)
                      ? _convParity(parity)
                      : DRV_UART_NO_PARITY;
#endif
}

static void _workNotify(void *param)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)param;
    uint32_t critical = osiEnterCritical();
    unsigned event = uart->pending_event;
    uart->pending_event = 0;
    osiExitCritical(critical);

    OSI_LOGI(0x1000526e, "AT %4c callback evt is 0x%08x", uart->config.name, event);	
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	if(event & DRV_UART_EVENT_RX_ARRIVED){
		atDispatch_t  *th = uart->ops.recv;
		if(atDispatchIsDataMode(th)){
			atDataEngine_t *engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(uart->ops.recv, QL_DATA_ENGINE_RX_ARRIVED);
			}else{
				atDispatchReadLater(uart->ops.recv);
			}
		}else{
			atDispatchReadLater(uart->ops.recv);
		}
	}
	if(event & DRV_UART_EVENT_TX_COMPLETE){
		atDispatch_t  *th = uart->ops.recv;
		if(atDispatchIsDataMode(th)){
			atDataEngine_t *engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(uart->ops.recv, QL_DATA_ENGINE_TX_COMPLETE);
			}
		}else{
#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
			srv2LineWakeUpOutSetSleepTimer();
#endif
		}
	}
#else
    if (event & DRV_UART_EVENT_RX_ARRIVED)
        atDispatchReadLater(uart->ops.recv);

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
    if (event & DRV_UART_EVENT_TX_COMPLETE)
        srv2LineWakeUpOutSetSleepTimer();
#endif
#endif
}

static void _drvCallback(void *param, uint32_t evt)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)param;
    uart->pending_event |= evt;
    osiWorkEnqueue(uart->work, osiSysWorkQueueHighPriority());
}

// =============================================================================
// Set flow control
// =============================================================================
static bool _setFlowCtrl(atDevice_t *th, atDeviceRXFC_t rxfc, atDeviceTXFC_t txfc)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    bool rts_enable = (rxfc == AT_DEVICE_RXFC_HW);
    bool cts_enable = (txfc == AT_DEVICE_TXFC_HW);
    bool result = true;

    if (uart->config.rts_enable != rts_enable ||
        uart->config.cts_enable != cts_enable)
    {
        drvUartCfg_t cfg = *drvUartConfig(uart->drv);
        cfg.rts_enable = rts_enable;
        cfg.cts_enable = cts_enable;

        drvUartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
        drvUartClose(uart->drv);
        result = drvUartReconfig(uart->drv, &cfg);
        drvUartOpen(uart->drv);

        if (result)
        {
            uart->config.rts_enable = rts_enable;
            uart->config.cts_enable = cts_enable;
        }
    }

    return result;
}

static void _setBaud(atDevice_t *th, size_t baud)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    // auto baud or baud change
    if (baud == 0 || uart->config.baud != baud)
    {
        drvUartCfg_t cfg = *drvUartConfig(uart->drv);
        cfg.baud = baud;
        drvUartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
        drvUartClose(uart->drv);
        bool r = drvUartReconfig(uart->drv, &cfg) && drvUartOpen(uart->drv);
        if (r)
        {
            uart->config.baud = baud;
        }
        else
        {
            OSI_LOGE(0, "AT %4c set baud/%d fail", uart->config.name, baud);
        }
    }
}

// =============================================================================
// Set baud, format and parity
// =============================================================================
static void _setFormat(atDevice_t *th, atDeviceFormat_t format, atDeviceParity_t parity)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    drvUartCfg_t cfg = *drvUartConfig(uart->drv);
    _convFormat(&cfg, format, parity);

    if (uart->config.format != format || uart->config.parity != parity)
    {
        _convFormat(&cfg, format, parity);

        drvUartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
        drvUartClose(uart->drv);
        bool r = drvUartReconfig(uart->drv, &cfg);
        drvUartOpen(uart->drv);

        if (r)
        {
            uart->config.format = format;
            uart->config.parity = parity;
        }
    }
}

static void _setAutoSleep(atDevice_t *th, int timeout)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    drvUartSetAutoSleep(uart->drv, timeout);
}

static int _readAvail(atDevice_t *th)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    return drvUartReadAvail(uart->drv);
}

static int _writeAvail(atDevice_t *th)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    return drvUartWriteAvail(uart->drv);
}

// =============================================================================
// atDeviceUartCreate
// =============================================================================
atDevice_t *atDeviceUartCreate(atDeviceUartConfig_t *cfg)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)calloc(1, sizeof(*uart));

    uart->config = *cfg;
    OSI_LOGI(0x1000526f, "AT %4c create: baud/%d", uart->config.name, cfg->baud);

    uart->ops.destroy = _destroy;
    uart->ops.open = _open;
    uart->ops.close = _close;
    uart->ops.write = _write;
    uart->ops.read = _read;
    uart->ops.read_avail = _readAvail;
    uart->ops.write_avail = _writeAvail;
    uart->ops.set_baud = _setBaud;
    uart->ops.set_format = _setFormat;
    uart->ops.set_flow_ctrl = _setFlowCtrl;
    uart->ops.set_auto_sleep = _setAutoSleep;
    uart->ops.isReady = false;

    drvUartCfg_t drvcfg = {};
    drvcfg.baud = cfg->baud;
    drvcfg.auto_baud_lc = 0;
    _convFormat(&drvcfg, cfg->format, cfg->parity);
    drvcfg.rx_buf_size = UART_RX_BUF_SIZE;
    drvcfg.tx_buf_size = UART_TX_BUF_SIZE;
    drvcfg.event_mask = DRV_UART_EVENT_RX_ARRIVED | DRV_UART_EVENT_RX_OVERFLOW | DRV_UART_EVENT_TX_COMPLETE;
    drvcfg.event_cb = _drvCallback;
    drvcfg.event_cb_ctx = uart;
    uart->drv = drvUartCreate(uart->config.name, &drvcfg);
    uart->work = osiWorkCreate(_workNotify, NULL, uart);

    return (atDevice_t *)uart;
}

// =============================================================================
// _destroy
// =============================================================================
static void _destroy(atDevice_t *th)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;

    if (uart == NULL)
        return;

    OSI_LOGI(0x10005270, "AT %4c destroy", uart->config.name);
    drvUartDestroy(uart->drv);
    osiWorkDelete(uart->work);
    free(uart);
}

// =============================================================================
// Write
// =============================================================================
static int _write(atDevice_t *th, const void *data, size_t length)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;

    if (data == NULL || length == 0)
        return 0;

    OSI_LOGI(0x10005271, "AT %4c write: len=%d", uart->config.name, length);

#ifdef CONFIG_TWOLINE_WAKEUP_ENABLE
    srv2LineWakeUpCheckWakeUpOutState();
#endif
    int trans = drvUartSendAll(uart->drv, data, length, UART_WAIT_TX_DONE_TIMEOUT);
    if (trans < 0)
        OSI_LOGE(0x10005272, "AT %4c output error %d", uart->config.name, trans);
    else if (trans < length)
        OSI_LOGE(0x10005273, "AT %4c output overflow, drop %d bytes", uart->config.name, length - trans);

    return trans;
}

// =============================================================================
// Read
// =============================================================================
static int _read(atDevice_t *th, void *data, size_t size)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;
    if (data == NULL || size == 0)
        return 0;

    int trans = drvUartReceive(uart->drv, data, size);
    if (trans < 0)
        OSI_LOGE(0x10005274, "AT %4c read error %d", uart->config.name, trans);
    else if (trans > 0)
        OSI_LOGI(0x10005275, "AT %4c read %d, got %d", uart->config.name, size, trans);
    return trans;
}

// =============================================================================
// Open
// =============================================================================
static bool _open(atDevice_t *th)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;

    OSI_LOGI(0x10005276, "AT %4c open", uart->config.name);
    uart->pending_event = 0;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
	bool ret = drvUartOpen(uart->drv);
	return ret;
#else
    drvUartOpen(uart->drv);
    return true;
#endif
}

// =============================================================================
// Close
// =============================================================================
static void _close(atDevice_t *th)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)th;

    OSI_LOGI(0x10005277, "AT %4c close", uart->config.name);
    drvUartWaitTxFinish(uart->drv, UART_WAIT_TX_DONE_TIMEOUT);
    drvUartClose(uart->drv);
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART
atDevice_t *ql_device_uart_create(atDeviceUartConfig_t *cfg, osiCallback_t work_notify, size_t rx_buf_size, size_t tx_buf_size)
{
    atDeviceUart_t *uart = (atDeviceUart_t *)calloc(1, sizeof(*uart));

    uart->config = *cfg;
    OSI_LOGI(0, "ql open create %4c, baud:%d", uart->config.name, cfg->baud);

    uart->ops.destroy = _destroy;
    uart->ops.open = _open;
    uart->ops.close = _close;
    uart->ops.write = _write;
    uart->ops.read = _read;
    uart->ops.read_avail = _readAvail;
    uart->ops.write_avail = _writeAvail;
    uart->ops.set_baud = _setBaud;
    uart->ops.set_format = _setFormat;
    uart->ops.set_flow_ctrl = _setFlowCtrl;
    uart->ops.set_auto_sleep = _setAutoSleep;
    uart->ops.isReady = false;

    drvUartCfg_t drvcfg = {};
    drvcfg.baud = cfg->baud;
    drvcfg.auto_baud_lc = 0;
    _convFormat(&drvcfg, cfg->format, cfg->parity);
    drvcfg.rx_buf_size = rx_buf_size;
    drvcfg.tx_buf_size = tx_buf_size;
    drvcfg.event_mask = DRV_UART_EVENT_RX_ARRIVED | DRV_UART_EVENT_RX_OVERFLOW;
    drvcfg.event_cb = _drvCallback;
    drvcfg.event_cb_ctx = uart;
    uart->drv = drvUartCreate(uart->config.name, &drvcfg);
    uart->work = osiWorkCreate(work_notify, NULL, uart);

    return (atDevice_t *)uart;
}
#endif

