/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('U', 'S', 'B', 'E')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "usb_ether.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hal_config.h"
#include "drv_config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/errno.h>
#include <machine/endian.h>

#define USBE_DEBUG_RECORD_RESOURCE

#define USBE_TX_QUEUE_COUNT 64
#define USBE_RX_QUEUE_COUNT 16
#define USBE_THREAD_STACK_SIZE 4096

typedef struct usb_ether_request usbEtherReq_t;

typedef TAILQ_ENTRY(usb_ether_request) usbEtherReqIter_t;
typedef TAILQ_HEAD(usb_ether_request_head, usb_ether_request) usbEtherReqHead_t;

struct usb_ether_request
{
    drvEthReq_t req;
    usbEther_t *usbe;
    usbXfer_t *xfer;
    usbEtherReqIter_t iter;
#ifdef USBE_DEBUG_RECORD_RESOURCE
    uint32_t _ra;
#define USBE_REQ_SET_RA(uereq) uereq->_ra = (uint32_t)__builtin_return_address(0)
#define USBE_REQ_GET_RA(uereq) (uereq->_ra)
#define USBE_REQ_CLR_RA(uereq) uereq->_ra = 0xcccceeee
#else
#define USBE_REQ_SET_RA(uereq)
#define USBE_REQ_GET_RA(uereq) (0xeeee0000)
#define USBE_REQ_CLR_RA(uereq)
#endif
};

struct usb_ether
{
    drvEther_t ether;
    usbEtherConfig_t config;
    osiWorkQueue_t *wq;
    osiWork_t *rx_work;
    // start/stop work should be called in order, use `osiWorkEnqueuLast`
    osiWork_t *start_work;
    osiWork_t *stop_work;

    // rx idle, link not start or transfer completed request
    usbEtherReqHead_t rx_idle;
    // rx frames, link the requests carried data
    usbEtherReqHead_t rx_frame;
    // tx idle, link all idle requests can be allocated
    usbEtherReqHead_t tx_idle;
    // tx busy, link the tx request had been allocated
    usbEtherReqHead_t tx_busy;

    drvEthEventCB_t event_cb;
    drvEthULDataCB_t uldata_cb;
    void *event_cb_priv;
    void *uldata_cb_priv;

    // host may set a dynamic mac address
    uint8_t alter_host_mac[ETH_ALEN];

    bool rx_processing;
    bool net_link_on;
    bool dev_online;
};

#define E2UE(e) (usbEther_t *)(e)
#define R2UR(r) (usbEtherReq_t *)(r)

static bool _rxStart(usbEther_t *usbe)
{
    uint32_t critical = osiEnterCritical();
    int retval;
    usbEtherReq_t *r;
    while ((r = TAILQ_FIRST(&usbe->rx_idle)) != NULL)
    {
        TAILQ_REMOVE(&usbe->rx_idle, r, iter);
        osiExitCritical(critical);

        retval = udcEpQueue(usbe->config.udc, usbe->config.rx_ep, r->xfer);
        if (retval < 0)
        {
            OSI_LOGE(0, "rx queue fail, %d", retval);
            critical = osiEnterCritical();
            TAILQ_INSERT_TAIL(&usbe->rx_idle, r, iter);
            osiExitCritical(critical);
            return false;
        }

        critical = osiEnterCritical();
    }
    osiExitCritical(critical);
    return true;
}

static inline void _rxStop(usbEther_t *usbe)
{
    udcEpDequeueAll(usbe->config.udc, usbe->config.rx_ep);
}

static inline void _txCancelAll(usbEther_t *usbe)
{
    udcEpDequeueAll(usbe->config.udc, usbe->config.tx_ep);
}

static bool _etherApiNetup(drvEther_t *ether)
{
    OSI_LOGI(0, "usbe netup");
    usbEther_t *usbe = E2UE(ether);
    if (usbe->net_link_on)
    {
        OSI_LOGW(0, "usb ether already up");
        return true;
    }

    if (usbe->dev_online)
    {
        if (TAILQ_EMPTY(&usbe->rx_idle))
        {
            OSI_LOGE(0, "usbe open rx submit empty");
            return false;
        }

        if (!_rxStart(usbe))
        {
            OSI_LOGE(0, "usbe submit rx transfer fail");
            _rxStop(usbe);
            return false;
        }
    }

    usbe->net_link_on = true;
    if (usbe->config.ops.netup_cb)
    {
        int r = usbe->config.ops.netup_cb(usbe);
        if (r < 0)
        {
            OSI_LOGE(0, "usbe netup fail when call callback (%d)", r);
            _rxStop(usbe);
            return false;
        }
    }
    return true;
}

static void _etherApiNetdown(drvEther_t *ether)
{
    OSI_LOGI(0, "usbe netdown");
    uint32_t critical = osiEnterCritical();
    usbEther_t *usbe = E2UE(ether);
    if (!usbe->net_link_on)
    {
        osiExitCritical(critical);
        OSI_LOGW(0, "usbe already net down");
        return;
    }

    usbe->net_link_on = false;
    osiExitCritical(critical);
    _rxStop(usbe);
    _txCancelAll(usbe);

    critical = osiEnterCritical();
    usbEtherReq_t *r;
    while ((r = TAILQ_FIRST(&usbe->rx_frame)) != NULL)
    {
        TAILQ_REMOVE(&usbe->rx_frame, r, iter);
        TAILQ_INSERT_TAIL(&usbe->rx_idle, r, iter);
    }

    while ((r = TAILQ_FIRST(&usbe->tx_busy)) != NULL)
    {
        TAILQ_REMOVE(&usbe->tx_busy, r, iter);
        osiExitCritical(critical);
        OSI_LOGE(0, "usbe netdown, withdraw tx req from %x", USBE_REQ_GET_RA(r));
        USBE_REQ_CLR_RA(r);
        critical = osiEnterCritical();
        TAILQ_INSERT_TAIL(&usbe->tx_idle, r, iter);
    }
    osiExitCritical(critical);
    if (usbe->config.ops.netdown_cb)
        usbe->config.ops.netdown_cb(usbe);
}

static drvEthReq_t *_etherApiAllocTxReq(drvEther_t *ether)
{
    uint32_t critical = osiEnterCritical();
    usbEther_t *usbe = E2UE(ether);
    usbEtherReq_t *req = TAILQ_FIRST(&usbe->tx_idle);
    if (req == NULL)
    {
        usbe->ether.stats.tx_dropped++;
        osiExitCritical(critical);
        OSI_LOGW(0, "usb ether allocate tx request fail");
        return NULL;
    }

    USBE_REQ_SET_RA(req);
    req->req.payload = (drvEthPacket_t *)(req->xfer->buf + usbe->config.dev_proto_head_len);
    req->req.buffer_size = usbe->config.payload_max_size - usbe->config.dev_proto_head_len;

    TAILQ_REMOVE(&usbe->tx_idle, req, iter);
    TAILQ_INSERT_HEAD(&usbe->tx_busy, req, iter);
    osiExitCritical(critical);
    return &req->req;
}

static void _givebackTxReq(usbEther_t *usbe, usbEtherReq_t *req)
{
    uint32_t critical = osiEnterCritical();
    TAILQ_REMOVE(&usbe->tx_busy, req, iter);
    USBE_REQ_CLR_RA(req);
    TAILQ_INSERT_TAIL(&usbe->tx_idle, req, iter);
    osiExitCritical(critical);
}

static void _etherApiFreeTxReq(drvEther_t *ether, drvEthReq_t *req)
{
    usbEther_t *usbe = E2UE(ether);
    usbEtherReq_t *ureq = R2UR(req);
    _givebackTxReq(usbe, ureq);
}

static bool _etherApiSubmitTxReq(drvEther_t *ether, drvEthReq_t *req_, size_t size)
{
    int result;
    uint32_t critical;
    usbEther_t *usbe = E2UE(ether);
    usbEtherReq_t *req = R2UR(req_);
    ethHdr_t *ehdr = (ethHdr_t *)(req->xfer->buf + usbe->config.dev_proto_head_len);
    uint8_t *ipdata = (uint8_t *)ehdr + ETH_HLEN;
    if (!(usbe->net_link_on && usbe->dev_online))
        goto fail;

    if ((ipdata[0] & 0xF0) == 0x40)
        ehdr->h_proto = __htons(ETH_P_IP);
    else if ((ipdata[0] & 0xF0) == 0x60)
        ehdr->h_proto = __htons(ETH_P_IPV6);
    else if (ipdata[0] == 0x00 && ipdata[1] == 0x01 &&
             ipdata[2] == 0x08 && ipdata[3] == 0x00)
        ehdr->h_proto = __htons(ETH_P_ARP);
    else
    {
        OSI_LOGE(0, "unknown ip version (%x)", *((uint32_t *)ipdata));
        goto fail;
    }

    req->xfer->status = 0;
    req->xfer->length = size + ETH_HLEN + usbe->config.dev_proto_head_len;
    if (usbe->config.dev_proto_head_len != 0)
    {
        req_->actual_size = size + ETH_HLEN;
        usbe->config.ops.wrap(usbe, req_, req->xfer->buf);
    }

    critical = osiEnterCritical();
    TAILQ_REMOVE(&usbe->tx_busy, req, iter);
    USBE_REQ_CLR_RA(req);

    result = udcEpQueue(usbe->config.udc, usbe->config.tx_ep, req->xfer);
    if (result < 0)
    {
        usbe->ether.stats.tx_dropped++;
        TAILQ_INSERT_TAIL(&usbe->tx_idle, req, iter);
        osiExitCritical(critical);
        OSI_LOGE(0, "usbe submit tx fail, %d", result);
        return false;
    }

    osiExitCritical(critical);
    return true;

fail:
    usbe->ether.stats.tx_dropped++;
    _givebackTxReq(usbe, req);
    return false;
}

static void _etherApiSetEventCb(drvEther_t *ether, drvEthEventCB_t cb, void *priv)
{
    uint32_t critical = osiEnterCritical();
    usbEther_t *usbe = E2UE(ether);
    usbe->event_cb = cb;
    usbe->event_cb_priv = priv;
    osiExitCritical(critical);
}

static void _etherApiSetUldataCb(drvEther_t *ether, drvEthULDataCB_t cb, void *priv)
{
    uint32_t critical = osiEnterCritical();
    usbEther_t *usbe = E2UE(ether);
    usbe->uldata_cb = cb;
    usbe->uldata_cb_priv = priv;
    osiExitCritical(critical);
}

static OSI_UNUSED void _resetTxReqDestMacLocked(usbEther_t *usbe, const uint8_t *mac)
{
    ethHdr_t *ehdr;
    usbEtherReq_t *req;
    TAILQ_FOREACH(req, &usbe->tx_busy, iter)
    {
        ehdr = (ethHdr_t *)((uint8_t *)req->xfer->buf + usbe->config.dev_proto_head_len);
        memcpy(ehdr->h_dest, mac, ETH_ALEN);
    }

    TAILQ_FOREACH(req, &usbe->tx_idle, iter)
    {
        ehdr = (ethHdr_t *)((uint8_t *)req->xfer->buf + usbe->config.dev_proto_head_len);
        memcpy(ehdr->h_dest, mac, ETH_ALEN);
    }
}

static void _etherApiSetHostMac(drvEther_t *ether, uint8_t host_mac[ETH_ALEN])
{
#ifdef CONFIG_USB_ETHER_DYNAMIC_HOST_MAC_SUPPORT
    uint32_t critical = osiEnterCritical();
    if (memcmp(ether->host_mac, host_mac, ETH_ALEN) != 0)
    {
        usbEther_t *usbe = E2UE(ether);
        memcpy(usbe->alter_host_mac, host_mac, ETH_ALEN);
        ether->host_mac = usbe->alter_host_mac;

        _resetTxReqDestMacLocked(usbe, ether->host_mac);
    }
    osiExitCritical(critical);
#endif
}

static void _usbeStartWork(void *param)
{
    usbEther_t *usbe = (usbEther_t *)param;
#ifdef CONFIG_USB_ETHER_DYNAMIC_HOST_MAC_SUPPORT
    uint32_t critical = osiEnterCritical();
    if (memcmp(usbe->ether.host_mac, usbe->config.host_mac, ETH_ALEN) != 0)
    {
        usbe->ether.host_mac = usbe->config.host_mac;
        _resetTxReqDestMacLocked(usbe, usbe->ether.host_mac);
    }
    osiExitCritical(critical);
#endif
    usbe->dev_online = true;
    if (usbe->event_cb)
        usbe->event_cb(DRV_ETHER_EVENT_CONNECT, usbe->event_cb_priv);
}

static void _usbeStopWork(void *param)
{
    usbEther_t *usbe = (usbEther_t *)param;
    usbe->dev_online = false;
    if (usbe->event_cb)
        usbe->event_cb(DRV_ETHER_EVENT_DISCONNECT, usbe->event_cb_priv);
}

static void _usbeRxWork(void *param)
{
    usbEther_t *usbe = (usbEther_t *)param;
    bool result = true;
    usbEtherReq_t *req;
    uint32_t critical = osiEnterCritical();
    usbe->rx_processing = true;

critical_rx_continue:
    while ((req = TAILQ_FIRST(&usbe->rx_frame)) != NULL)
    {
        TAILQ_REMOVE(&usbe->rx_frame, req, iter);
        osiExitCritical(critical);
        if (usbe->config.dev_proto_head_len == 0)
        {
            req->req.payload = req->xfer->buf;
            req->req.actual_size = req->xfer->actual;
        }
        else
        {
            result = usbe->config.ops.unwrap(usbe, req->xfer->buf, req->xfer->actual, &req->req);
        }
        critical = osiEnterCritical();
        if (result)
        {
            usbe->ether.stats.rx_packets++;
            usbe->ether.stats.rx_bytes += req->xfer->actual;
            osiExitCritical(critical);

            if (usbe->uldata_cb)
                usbe->uldata_cb(req->req.payload, req->req.actual_size, usbe->uldata_cb_priv);

            critical = osiEnterCritical();
        }
        else
        {
            usbe->ether.stats.rx_errors++;
            usbe->ether.stats.rx_length_errors++;
            OSI_LOGE(0, "unwrap ether packet fail");
        }

        TAILQ_INSERT_TAIL(&usbe->rx_idle, req, iter);
        if (usbe->dev_online && usbe->net_link_on)
        {
            osiExitCritical(critical);
            _rxStart(usbe);
            critical = osiEnterCritical();
        }
    }

    if (!TAILQ_EMPTY(&usbe->rx_frame))
        goto critical_rx_continue;

    if (usbe->dev_online && usbe->net_link_on)
    {
        osiExitCritical(critical);
        _rxStart(usbe);
        critical = osiEnterCritical();

        if (!TAILQ_EMPTY(&usbe->rx_frame))
            goto critical_rx_continue;
    }

    usbe->rx_processing = false;
    osiExitCritical(critical);
}

static void _rxComplete(usbEp_t *ep, usbXfer_t *xfer)
{
    uint32_t critical = osiEnterCritical();
    usbEtherReq_t *req = (usbEtherReq_t *)xfer->param;
    usbEther_t *usbe = req->usbe;

    if (xfer->status == 0)
    {
        TAILQ_INSERT_TAIL(&usbe->rx_frame, req, iter);
        if (!usbe->rx_processing)
            osiWorkEnqueue(usbe->rx_work, usbe->wq);
    }
    else
    {
        TAILQ_INSERT_TAIL(&usbe->rx_idle, req, iter);
        OSI_LOGE(0, "rx complete fail %d", xfer->status);
        if (xfer->status != -ECANCELED)
            usbe->ether.stats.rx_errors++;
    }

    if (xfer->status != -ECANCELED && !TAILQ_EMPTY(&usbe->rx_idle) && usbe->net_link_on)
    {
        osiExitCritical(critical);
        _rxStart(usbe);
        critical = osiEnterCritical();
    }
    osiExitCritical(critical);
}

static void _txComplete(usbEp_t *ep, usbXfer_t *xfer)
{
    uint32_t critical = osiEnterCritical();
    usbEtherReq_t *req = (usbEtherReq_t *)xfer->param;
    usbEther_t *usbe = req->usbe;
    TAILQ_INSERT_TAIL(&usbe->tx_idle, req, iter);
    usbe->ether.stats.tx_packets++;
    if (xfer->status == 0)
    {
        usbe->ether.stats.tx_bytes += xfer->actual;
    }
    else
    {
        usbe->ether.stats.tx_errors++;
        OSI_LOGE(0, "usb ether tx done fail. %d/%u", xfer->status, usbe->ether.stats.tx_errors);
    }
    osiExitCritical(critical);
}

static bool _usbeReqInit(usbEther_t *usbe, uintptr_t *mem, usbEtherReq_t *r, const usbEtherConfig_t *config)
{
    r->xfer = udcXferAlloc(config->udc);
    if (r->xfer == NULL)
        return false;
    r->xfer->buf = (void *)OSI_PTR_INCR_POST(*mem, config->payload_max_size);
    r->xfer->param = (void *)r;
    r->usbe = usbe;
    return true;
}

static void _usbeFreeReqList(usbEtherReqHead_t *h, udc_t *udc)
{
    usbEtherReq_t *r;
    uint32_t critical = osiEnterCritical();
    while ((r = TAILQ_FIRST(h)) != NULL)
    {
        TAILQ_REMOVE(h, r, iter);
        osiExitCritical(critical);
        udcXferFree(udc, r->xfer);
        critical = osiEnterCritical();
    }
    osiExitCritical(critical);
}

usbEther_t *usbEtherCreate(const usbEtherConfig_t *config)
{
    OSI_DEBUG_ASSERT(config->dev_proto_head_len == 0 || (config->ops.unwrap && config->ops.wrap));
    OSI_DEBUG_ASSERT(config->payload_max_size > config->dev_proto_head_len &&
                     OSI_IS_ALIGNED(config->payload_max_size, CONFIG_CACHE_LINE_SIZE));

    const unsigned req_cnt = USBE_RX_QUEUE_COUNT + USBE_TX_QUEUE_COUNT;
    const unsigned alloc_size = sizeof(usbEther_t) + CONFIG_CACHE_LINE_SIZE +
                                req_cnt * (sizeof(usbEtherReq_t) + config->payload_max_size);
    uintptr_t mem = (uintptr_t)malloc(alloc_size);
    if (mem == (uintptr_t)NULL)
    {
        OSI_LOGE(0, "usb ether create not enough memory");
        return NULL;
    }

    usbEther_t *usbe = (usbEther_t *)OSI_PTR_INCR_POST(mem, sizeof(usbEther_t));
    memset(usbe, 0, sizeof(usbEther_t));

    bool init_result = false;
    do
    {
        char name[32];
        const char *fourcc = (char *)&config->name;
        snprintf(name, 32, "usb-ether(%c%c%c%c)", fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
        usbe->wq = osiWorkQueueCreate(name, 1, OSI_PRIORITY_HIGH, USBE_THREAD_STACK_SIZE);
        if (usbe->wq == NULL)
            break;

        usbe->rx_work = osiWorkCreate(_usbeRxWork, NULL, usbe);
        if (usbe->rx_work == NULL)
            break;

        usbe->start_work = osiWorkCreate(_usbeStartWork, NULL, usbe);
        if (usbe->start_work == NULL)
            break;

        usbe->stop_work = osiWorkCreate(_usbeStopWork, NULL, usbe);
        if (usbe->stop_work == NULL)
            break;

        init_result = true;
    } while (0);

    if (!init_result)
        goto init_param_fail;

    TAILQ_INIT(&usbe->rx_idle);
    TAILQ_INIT(&usbe->rx_frame);
    TAILQ_INIT(&usbe->tx_idle);
    TAILQ_INIT(&usbe->tx_busy);

    init_result = false;
    usbEtherReq_t *reqs = (usbEtherReq_t *)OSI_PTR_INCR_POST(mem, req_cnt * sizeof(usbEtherReq_t));
    mem = OSI_ALIGN_UP(mem, CONFIG_CACHE_LINE_SIZE);
    do
    {
        usbEtherReq_t *r;
        ethHdr_t *ehdr;
        for (unsigned i = 0; i < USBE_RX_QUEUE_COUNT; ++i)
        {
            r = reqs++;
            if (!_usbeReqInit(usbe, &mem, r, config))
                goto req_alloc_end;
            r->xfer->length = config->payload_max_size;
            r->xfer->complete = _rxComplete;
            TAILQ_INSERT_TAIL(&usbe->rx_idle, r, iter);
        }

        for (unsigned i = 0; i < USBE_TX_QUEUE_COUNT; ++i)
        {
            r = reqs++;
            if (!_usbeReqInit(usbe, &mem, r, config))
                goto req_alloc_end;
            r->xfer->zlp = 1;
            r->xfer->complete = _txComplete;
            ehdr = (ethHdr_t *)((uint8_t *)r->xfer->buf + config->dev_proto_head_len);
            memcpy(ehdr->h_dest, config->host_mac, ETH_ALEN);
            memcpy(ehdr->h_source, config->dev_mac, ETH_ALEN);
            if (config->ops.wrap_pre != NULL)
                config->ops.wrap_pre(usbe, r->xfer->buf);
            TAILQ_INSERT_TAIL(&usbe->tx_idle, r, iter);
        }

        init_result = true;

    } while (0);

req_alloc_end:
    if (!init_result)
        goto init_req_fail;

    usbe->config = *config;
    usbe->ether.host_mac = usbe->config.host_mac;
    usbe->ether.dev_mac = usbe->config.dev_mac;
    usbe->ether.impl.netup = _etherApiNetup;
    usbe->ether.impl.netdown = _etherApiNetdown;
    usbe->ether.impl.tx_req_alloc = _etherApiAllocTxReq;
    usbe->ether.impl.tx_req_free = _etherApiFreeTxReq;
    usbe->ether.impl.tx_req_submit = _etherApiSubmitTxReq;
    usbe->ether.impl.set_event_cb = _etherApiSetEventCb;
    usbe->ether.impl.set_uldata_cb = _etherApiSetUldataCb;
    usbe->ether.impl.set_host_mac = _etherApiSetHostMac;
    return usbe;

init_req_fail:
    _usbeFreeReqList(&usbe->rx_idle, config->udc);
    _usbeFreeReqList(&usbe->tx_idle, config->udc);

init_param_fail:
    osiWorkDelete(usbe->start_work);
    osiWorkDelete(usbe->stop_work);
    osiWorkDelete(usbe->rx_work);
    osiWorkQueueDelete(usbe->wq);
    free(usbe);

    return NULL;
}

drvEther_t *usbEtherGetEther(usbEther_t *usbe)
{
    return usbe ? &usbe->ether : NULL;
}

const usbEtherConfig_t *usbEtherConfig(const usbEther_t *usbe)
{
    return usbe ? &usbe->config : NULL;
}

bool usbEtherStart(usbEther_t *usbe)
{
    if (usbe == NULL)
        return false;
    return osiWorkEnqueueLast(usbe->start_work, usbe->wq);
}

void usbEtherStop(usbEther_t *usbe)
{
    if (usbe == NULL)
        return;
    osiWorkEnqueueLast(usbe->stop_work, usbe->wq);
}

void usbEtherDestroy(usbEther_t *usbe)
{
    if (usbe == NULL)
        return;

    usbEtherStop(usbe);
    _usbeFreeReqList(&usbe->rx_idle, usbe->config.udc);
    _usbeFreeReqList(&usbe->rx_frame, usbe->config.udc);
    _usbeFreeReqList(&usbe->tx_idle, usbe->config.udc);
    _usbeFreeReqList(&usbe->tx_busy, usbe->config.udc);

    osiWorkDelete(usbe->start_work);
    osiWorkDelete(usbe->stop_work);
    osiWorkDelete(usbe->rx_work);
    osiWorkQueueDelete(usbe->wq);

    free(usbe);
}
