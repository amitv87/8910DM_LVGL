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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'T', 'H', 'R')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_VERBOSE

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/errno.h>
#include <osi_api.h>
#include <osi_log.h>

#include "rndis_data.h"
#include "drv_names.h"
#include "rndis.h"
#include "netdev_interface.h"

typedef struct
{
    usbEtherConfig_t cfg;
    bool enable; ///< ether port state
} rndisPriv_t;

static inline rndisData_t *prvEth2Rndis(usbEther_t *usbe)
{
    return (rndisData_t *)usbEtherConfig(usbe)->dev_priv;
}

static inline rndisPriv_t *prvRndis2Priv(rndisData_t *rnd)
{
    return (rndisPriv_t *)rnd->priv;
}

static int prvRndisNetup(usbEther_t *usbe)
{
    rndisData_t *rnd = prvEth2Rndis(usbe);
    rndisPriv_t *priv = prvRndis2Priv(rnd);

    if (!priv->enable)
        return -ENODEV;

    rnd->rndis_open(rnd->func);

    return 0;
}

static void prvRndisNetdown(usbEther_t *usbe)
{
    rndisData_t *rnd = prvEth2Rndis(usbe);
    rnd->rndis_close(rnd->func);
}

static void prvRndisAddCommonHead(usbEther_t *usbe, uint8_t *buf)
{
    struct rndis_packet_msg_type *head = (struct rndis_packet_msg_type *)buf;
    memset(head, 0, sizeof(*head));
    head->MessageType = cpu_to_le32(RNDIS_MSG_PACKET);
    head->DataOffset = cpu_to_le32(36);
}

static void prvRndisAddHead(usbEther_t *usbe, const drvEthReq_t *req, uint8_t *buf)
{
    struct rndis_packet_msg_type *head = (struct rndis_packet_msg_type *)buf;
    head->MessageLength = cpu_to_le32(req->actual_size + sizeof(*head));
    head->DataLength = cpu_to_le32(req->actual_size);
}

static bool prvRndisRemoveHead(usbEther_t *usbe, uint8_t *buf, uint32_t size, drvEthReq_t *req)
{
    struct rndis_packet_msg_type *head = (struct rndis_packet_msg_type *)buf;
    if (head->MessageType != cpu_to_le32(RNDIS_MSG_PACKET))
    {
        OSI_LOGE(0, "Invalid RNDIS data packet 0x%x", head->MessageType);
        return false;
    }
    req->payload = (drvEthPacket_t *)(buf + head->DataOffset + 8);
    req->actual_size = size - sizeof(*head);
    return true;
}

static rndisPriv_t *prvRndisPrivCreate(rndisData_t *rnd)
{
    rndisPriv_t *p = calloc(1, sizeof(rndisPriv_t));
    if (p == NULL)
    {
        OSI_LOGE(0, "rndis data fail to allocate rndis priv");
        return NULL;
    }

    p->cfg.host_mac = rnd->host_mac;
    p->cfg.dev_mac = rnd->dev_mac;

    udc_t *udc = rnd->func->controller;
    p->cfg.tx_ep = udcEpAlloc(udc, rnd->epin_desc);
    if (p->cfg.tx_ep == NULL)
        goto fail_tx_ep;

    p->cfg.rx_ep = udcEpAlloc(udc, rnd->epout_desc);
    if (p->cfg.rx_ep == NULL)
        goto fail_rx_ep;

    p->cfg.dev_priv = rnd;
    p->cfg.ops.netup_cb = prvRndisNetup;
    p->cfg.ops.netdown_cb = prvRndisNetdown;
    p->cfg.ops.wrap_pre = prvRndisAddCommonHead;
    p->cfg.ops.wrap = prvRndisAddHead;
    p->cfg.ops.unwrap = prvRndisRemoveHead;
    p->cfg.name = OSI_MAKE_TAG('N', 'D', 'I', 'S');
    p->cfg.udc = udc;
    p->cfg.payload_max_size = 2048;
    p->cfg.dev_proto_head_len = sizeof(struct rndis_packet_msg_type);

    return p;

fail_rx_ep:
    udcEpFree(udc, p->cfg.tx_ep);

fail_tx_ep:
    free(p);
    return NULL;
}

static void prvRndisPrivDestroy(rndisData_t *rnd, rndisPriv_t *p)
{
    udc_t *udc = rnd->func->controller;
    if (p->cfg.tx_ep)
        udcEpFree(udc, p->cfg.tx_ep);
    if (p->cfg.rx_ep)
        udcEpFree(udc, p->cfg.rx_ep);
    free(p);
}

int rndisEtherBind(rndisData_t *rnd)
{
    if (!rnd || !rnd->func || !rnd->func->controller ||
        !rnd->epin_desc || !rnd->epout_desc)
        return -1;

    OSI_LOGI(0, "CDC Ethernet bind\n");
    rndisPriv_t *priv = prvRndisPrivCreate(rnd);
    if (priv == NULL)
    {
        OSI_LOGE(0, "rndis data fail to create priv");
        return -1;
    }

    rnd->usbe = usbEtherCreate(&priv->cfg);
    if (rnd->usbe == NULL)
        goto failed;

    rnd->epin_desc->bEndpointAddress = priv->cfg.tx_ep->address;
    rnd->epout_desc->bEndpointAddress = priv->cfg.rx_ep->address;
    rnd->priv = (unsigned long)priv;
    netdevInit(usbEtherGetEther(rnd->usbe));

    return 0;

failed:
    prvRndisPrivDestroy(rnd, priv);
    return -1;
}

void rndisEtherUnbind(rndisData_t *rnd)
{
    OSI_LOGV(0, "Rndis Ethernet unbind\n");
    if (!rnd || !rnd->func)
        return;

    uint32_t critical = osiEnterCritical();
    rndisPriv_t *priv = prvRndis2Priv(rnd);
    usbEther_t *usbe = rnd->usbe;
    rnd->usbe = NULL;
    rnd->priv = 0;
    osiExitCritical(critical);

    netdevExit();
    usbEtherDestroy(usbe);
    prvRndisPrivDestroy(rnd, priv);
}

int rndisEtherEnable(rndisData_t *rnd)
{
    rndisPriv_t *priv = prvRndis2Priv(rnd);
    OSI_LOGI(0, "rndis ether enable");
    if (priv->enable)
    {
        OSI_LOGW(0, "rndis data enable, already enabled");
        return 0;
    }

    udc_t *udc = rnd->func->controller;
    int retval = udcEpEnable(udc, priv->cfg.tx_ep);
    if (retval < 0)
        return retval;

    retval = udcEpEnable(udc, priv->cfg.rx_ep);
    if (retval < 0)
        goto fail_rx;

    priv->enable = true;
    usbEtherStart(rnd->usbe);
    return 0;

fail_rx:
    udcEpDisable(udc, priv->cfg.tx_ep);

    return retval;
}

void rndisEtherDisable(rndisData_t *rnd)
{
    OSI_LOGI(0, "rndis ether disable");
    rndisPriv_t *priv = prvRndis2Priv(rnd);
    if (priv == NULL)
        return;

    uint32_t critical = osiEnterCritical();
    if (priv->enable == false)
    {
        osiExitCritical(critical);
        return;
    }
    priv->enable = false;
    osiExitCritical(critical);

    usbEtherStop(rnd->usbe);
    udc_t *udc = rnd->func->controller;
    udcEpDisable(udc, priv->cfg.tx_ep);
    udcEpDisable(udc, priv->cfg.rx_ep);
}
