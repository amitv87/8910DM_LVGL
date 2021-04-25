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

#include "osi_log.h"
#include "drv_serial.h"
#include "at_engine_imp.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "quec_proj_config.h"
#include "quec_data_engine.h"
#include "drv_names.h"
#include "quec_log.h"

#ifndef CONFIG_QUEC_PROJECT_FEATURE_UART
typedef struct
{
    atDevice_t ops; // API
    uint32_t name;
    drvSerial_t *serial;
} atDeviceUsrl_t;
#endif

#define USERIAL_RX_BUF_SIZE (256 * 1024)
#define USERIAL_TX_BUF_SIZE (128 * 1024)
#define USERIAL_RX_DMA_SIZE (2048)
#define USERIAL_TX_DMA_SIZE (2048)
#define TX_WAIT_TIMEOUT (50)

static void _destroy(atDevice_t *th)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    drvSerialRelease(usrl->serial);
    free(usrl);
}

static bool _open(atDevice_t *th)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    return drvSerialOpen(usrl->serial);
}

static void _close(atDevice_t *th)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    drvSerialWaitWriteFinish(usrl->serial, TX_WAIT_TIMEOUT);
    drvSerialClose(usrl->serial);
}

static int _write(atDevice_t *th, const void *data, size_t length)
{
    if (data == NULL || length == 0)
        return 0;

    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    int send = 0;

    for (;;)
    {
        int trans = drvSerialWrite(usrl->serial, data, length);
        if (trans < 0)
        {
            OSI_LOGE(0x10005272, "AT %4c output error %d", usrl->name, trans);
            return trans;
        }

        send += trans;
        length -= trans;
        data = (const char *)data + trans;
        if (length == 0)
            break;

        if (!drvSerialWaitWriteAvail(usrl->serial, TX_WAIT_TIMEOUT))
            break;
    }

    if (length == 0)
        OSI_LOGI(0x10005278, "AT %4c write %d", usrl->name, send);
    else
        OSI_LOGE(0x10005279, "AT %4c write %d timeout, remain %d", usrl->name, length + send, length);
    return send;
}

static int _read(atDevice_t *th, void *buf, size_t size)
{
    if (buf == NULL || size == 0)
        return 0;

    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    int len = drvSerialRead(usrl->serial, buf, size);
    if (len < 0)
        OSI_LOGE(0x10005274, "AT %4c read error %d", usrl->name, len);
    else if (len > 0)
        OSI_LOGI(0x10005275, "AT %4c read %d, got %d", usrl->name, size, len);
    return len;
}

static int _readAvail(atDevice_t *th)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    return drvSerialReadAvail(usrl->serial);
}

static int _writeAvail(atDevice_t *th)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)th;
    return drvSerialWriteAvail(usrl->serial);
}

static void _atUsrlCb(drvSerialEvent_t evt, unsigned long param)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)param;
    atDevice_t *th = &usrl->ops;

    if (th->recv == NULL)
    {
        OSI_LOGW(0, "AT %4c callback no dispatch", usrl->name);
        return;
    }

    if (evt & DRV_SERIAL_EVENT_READY)
        atDispatchSetDeviceReadyStatus(usrl->ops.recv, true);

    if (evt & DRV_SERIAL_EVENT_BROKEN)
        atDispatchDeviceBroken(usrl->ops.recv);

#ifdef CONFIG_QUEC_PROJECT_FEATURE
	 if (evt & DRV_SERIAL_EVENT_RX_ARRIVED){
		atDispatch_t  *th = usrl->ops.recv;
		atDataEngine_t *engine = NULL;
		if(atDispatchIsDataMode(th)){
			engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(usrl->ops.recv, QL_DATA_ENGINE_RX_ARRIVED);
			}else{
				atDispatchReadLater(usrl->ops.recv);
			}
		}else{
			atDispatchReadLater(usrl->ops.recv);
		}
	 }
	 if(evt & DRV_SERIAL_EVENT_TX_COMPLETE){
		atDispatch_t  *th = usrl->ops.recv;
		if(atDispatchIsDataMode(th)){
			atDataEngine_t *engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(usrl->ops.recv, QL_DATA_ENGINE_TX_COMPLETE);
			}
		}
	 }
#else
    if (evt & DRV_SERIAL_EVENT_RX_ARRIVED)
        atDispatchReadLater(usrl->ops.recv);
#endif	
}

atDevice_t *atDeviceUserialCreate(uint32_t name)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)calloc(1, sizeof(atDeviceUsrl_t));
    if (!usrl)
        return NULL;

    uint32_t event = DRV_SERIAL_EVENT_RX_ARRIVED |
                     DRV_SERIAL_EVENT_RX_OVERFLOW |
                     DRV_SERIAL_EVENT_BROKEN |
                     DRV_SERIAL_EVENT_READY;

    drvSerialCfg_t cfg = {};
    cfg.tx_buf_size = USERIAL_TX_BUF_SIZE;
    cfg.rx_buf_size = USERIAL_RX_BUF_SIZE;
    cfg.rx_dma_size = USERIAL_RX_DMA_SIZE;
    cfg.tx_dma_size = USERIAL_TX_DMA_SIZE;
    cfg.event_mask = event;
    cfg.param = (unsigned long)usrl;
    cfg.event_cb = _atUsrlCb;

    drvSerial_t *serial = drvSerialAcquire(name, &cfg);
    if (!serial)
    {
        free(usrl);
        return NULL;
    }

    usrl->name = name;
    usrl->serial = serial;
    usrl->ops.destroy = _destroy;
    usrl->ops.open = _open;
    usrl->ops.close = _close;
    usrl->ops.write = _write;
    usrl->ops.read = _read;
    usrl->ops.read_avail = _readAvail;
    usrl->ops.write_avail = _writeAvail;
    usrl->ops.isReady = false;
	
    return (atDevice_t *)usrl;
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE_UART

atDevice_t *ql_device_userial_create(uint32_t name, drvSerialEventCB_t notify_cb)
{
    atDeviceUsrl_t *usrl = (atDeviceUsrl_t *)calloc(1, sizeof(atDeviceUsrl_t));
    if (!usrl)
        return NULL;

    uint32_t event = DRV_SERIAL_EVENT_RX_ARRIVED |DRV_SERIAL_EVENT_RX_OVERFLOW;

    drvSerialCfg_t cfg = {};
    cfg.tx_buf_size = USERIAL_TX_BUF_SIZE;
    cfg.rx_buf_size = USERIAL_RX_BUF_SIZE;
    cfg.rx_dma_size = USERIAL_RX_DMA_SIZE;
    cfg.tx_dma_size = USERIAL_TX_DMA_SIZE;
    cfg.event_mask = event;
    cfg.param = (unsigned long)usrl;
    cfg.event_cb = notify_cb;

    drvSerial_t *serial = drvSerialAcquire(name, &cfg);
    if (!serial)
    {
        free(usrl);
        return NULL;
    }

    usrl->name = name;
    usrl->serial = serial;
    usrl->ops.destroy = _destroy;
    usrl->ops.open = _open;
    usrl->ops.close = _close;
    usrl->ops.write = _write;
    usrl->ops.read = _read;
    usrl->ops.read_avail = _readAvail;
    usrl->ops.write_avail = _writeAvail;
    usrl->ops.isReady = false;
	
    return (atDevice_t *)usrl;
}

#endif

