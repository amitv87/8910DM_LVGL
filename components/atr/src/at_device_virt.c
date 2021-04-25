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
#include "osi_pipe.h"
#include <stdlib.h>
#include <string.h>
#include "quec_proj_config.h"
#include "quec_data_engine.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_VIRT_AT	
extern void ql_virt_at_rx_pipe_callback(void *param, unsigned event);
extern void ql_virt_at_tx_pipe_callback(void *param, unsigned event);
#endif

#define VIRT_WAIT_TX_DONE_TIMEOUT (500)

typedef struct
{
    atDevice_t ops;              // API
    atDeviceVirtConfig_t config; // configuration
} atDeviceVirt_t;

static void prvRxPipeCallback(void *param, unsigned event)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)param;

    OSI_LOGI(0x1000526e, "AT %4c callback evt is 0x%08x", d->config.name, event);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
	if (event & OSI_PIPE_EVENT_RX_ARRIVED)
	{
		atDispatch_t  *th = d->ops.recv;
		if(atDispatchIsDataMode(th)){
			atDataEngine_t *engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(d->ops.recv, QL_DATA_ENGINE_RX_ARRIVED);
			}else{
				atDispatchReadLater(d->ops.recv);
			}
		}else{
			atDispatchReadLater(d->ops.recv);
		}
	}
#else
    if (event & OSI_PIPE_EVENT_RX_ARRIVED)
        atDispatchReadLater(d->ops.recv);
#endif
}

#ifdef CONFIG_QUEC_PROJECT_FEATURE
static void prvTxPipeCallback(void *param, unsigned event)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)param;

    OSI_LOGI(0x1000526e, "AT %4c callback evt is 0x%08x", d->config.name, event);
    if (event & OSI_PIPE_EVENT_TX_COMPLETE){
        atDispatch_t  *th = d->ops.recv;
		if(atDispatchIsDataMode(th)){
			atDataEngine_t *engine = atDispatchGetDataEngine(th);
			if(atDataEngineGetPppSession(engine) == NULL){
				//enter quectel data mode engine
				quec_data_engine_notify(d->ops.recv, QL_DATA_ENGINE_TX_COMPLETE);
			}
		}
    }
}
#endif
static int prvReadAvail(atDevice_t *th)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    return osiPipeReadAvail(d->config.rx_pipe);
}

static int prvWriteAvail(atDevice_t *th)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    return osiPipeWriteAvail(d->config.tx_pipe);
}

static bool prvOpen(atDevice_t *th)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    osiPipeSetReaderCallback(d->config.rx_pipe, OSI_PIPE_EVENT_RX_ARRIVED, prvRxPipeCallback, d);
#ifdef CONFIG_QUEC_PROJECT_FEATURE
    osiPipeSetReaderCallback(d->config.tx_pipe, OSI_PIPE_EVENT_RX_ARRIVED, prvTxPipeCallback, d);
#endif

    //if quectel virtual at is enabled, the following callbacks will override the above callbacks
#ifdef CONFIG_QUEC_PROJECT_FEATURE_VIRT_AT
    osiPipeSetReaderCallback(d->config.rx_pipe, OSI_PIPE_EVENT_RX_ARRIVED, ql_virt_at_rx_pipe_callback, d);
    osiPipeSetReaderCallback(d->config.tx_pipe, OSI_PIPE_EVENT_RX_ARRIVED, ql_virt_at_tx_pipe_callback, d);
#endif
    return true;
}

static void prvClose(atDevice_t *th)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    osiPipeSetReaderCallback(d->config.rx_pipe, 0, NULL, NULL);
}

static int prvWrite(atDevice_t *th, const void *data, size_t length)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    if (data == NULL || length == 0)
        return 0;

    OSI_LOGI(0x10005271, "AT %4c write: len=%d", d->config.name, length);

    int trans = osiPipeWriteAll(d->config.tx_pipe, data, length, VIRT_WAIT_TX_DONE_TIMEOUT);
    if (trans < 0)
        OSI_LOGE(0x10005272, "AT %4c output error %d", d->config.name, trans);
    else if (trans < length)
        OSI_LOGE(0x10005273, "AT %4c output overflow, drop %d bytes", d->config.name, length - trans);

    return trans;
}

static int prvRead(atDevice_t *th, void *data, size_t size)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    if (data == NULL || size == 0)
        return 0;

    int trans = osiPipeRead(d->config.rx_pipe, data, size);
    if (trans < 0)
        OSI_LOGE(0x10005274, "AT %4c read error %d", d->config.name, trans);
    else if (trans > 0)
        OSI_LOGI(0x10005275, "AT %4c read %d, got %d", d->config.name, size, trans);
    return trans;
}

static void prvDestroy(atDevice_t *th)
{
    atDeviceVirt_t *d = (atDeviceVirt_t *)th;
    if (d == NULL)
        return;

    OSI_LOGI(0x10005270, "AT %4c destroy", d->config.name);
    osiPipeSetReaderCallback(d->config.rx_pipe, 0, NULL, NULL);
    free(d);
}

atDevice_t *atDeviceVirtCreate(const atDeviceVirtConfig_t *cfg)
{
    OSI_LOGI(0, "AT %4c create", cfg->name);
    if (cfg->rx_pipe == NULL || cfg->tx_pipe == NULL)
        return NULL;

    atDeviceVirt_t *d = (atDeviceVirt_t *)calloc(1, sizeof(atDeviceVirt_t));
    if (d == NULL)
        return NULL;

    d->config = *cfg;
    d->ops.destroy = prvDestroy;
    d->ops.open = prvOpen;
    d->ops.close = prvClose;
    d->ops.write = prvWrite;
    d->ops.read = prvRead;
    d->ops.read_avail = prvReadAvail;
    d->ops.write_avail = prvWriteAvail;
    d->ops.isReady = false;
    return (atDevice_t *)d;
}
