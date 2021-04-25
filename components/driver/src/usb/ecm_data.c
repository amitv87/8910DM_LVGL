/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'C', 'M', 'D')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "ecm_data.h"
#include "usb_ether.h"
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "osi_log.h"
#include "osi_api.h"
#include "netdev_interface.h"

typedef struct
{
    usbEp_t *ep_in;
    usbEp_t *ep_out;
    usbEther_t *usbe;
    bool enable;
} ecmPriv_t;

static inline ecmData_t *prvUsbe2Ecm(usbEther_t *usbe)
{
    return (ecmData_t *)usbEtherConfig(usbe)->dev_priv;
}

static inline ecmPriv_t *prvUsbe2Priv(usbEther_t *usbe)
{
    return (ecmPriv_t *)(prvUsbe2Ecm(usbe)->priv);
}

static int prvEcmNetup(usbEther_t *usbe)
{
    ecmPriv_t *p = prvUsbe2Priv(usbe);
    ecmData_t *ecm = prvUsbe2Ecm(usbe);
    OSI_LOGI(0, "ECM netup %d", p->enable);
    if (!p->enable)
        return -ENOTCONN;

    ecm->ecm_open(ecm->func);
    return 0;
}

static void prvEcmNetdown(usbEther_t *usbe)
{
    ecmData_t *ecm = prvUsbe2Ecm(usbe);
    OSI_LOGI(0, "ECM netdown %d", prvUsbe2Priv(usbe)->enable);
    ecm->ecm_close(ecm->func);
}

static usbEther_t *prvUsbeCreate(ecmData_t *ecm, ecmPriv_t *p)
{
    usbEtherConfig_t config = {};
    config.ops.netup_cb = prvEcmNetup;
    config.ops.netdown_cb = prvEcmNetdown;

    config.udc = ecm->func->controller;
    config.tx_ep = p->ep_in;
    config.rx_ep = p->ep_out;
    config.dev_proto_head_len = 0;
    config.payload_max_size = 2048;
    config.dev_priv = ecm;

    config.host_mac = ecm->host_mac;
    config.dev_mac = ecm->dev_mac;

    return usbEtherCreate(&config);
}

ecmPriv_t *prvCreate(ecmData_t *ecm)
{
    ecmPriv_t *p = calloc(1, sizeof(ecmPriv_t));
    if (p == NULL)
        return NULL;

    p->ep_in = udcEpAlloc(ecm->func->controller, ecm->epin_desc);
    if (p->ep_in == NULL)
        goto fail_ep_in;

    p->ep_out = udcEpAlloc(ecm->func->controller, ecm->epout_desc);
    if (p->ep_out == NULL)
        goto fail_ep_out;

    p->usbe = prvUsbeCreate(ecm, p);
    if (p->usbe == NULL)
        goto fail_ether;

    return p;

fail_ether:
    udcEpFree(ecm->func->controller, p->ep_out);
    p->ep_out = NULL;

fail_ep_out:
    udcEpFree(ecm->func->controller, p->ep_in);
    p->ep_in = NULL;

fail_ep_in:
    free(p);

    return NULL;
}

bool ecmDataBind(ecmData_t *ecm)
{
    ecmPriv_t *priv = prvCreate(ecm);
    if (priv == NULL)
    {
        OSI_LOGE(0, "ecm data bind create private data fail");
        return false;
    }

    ecm->epin_desc->bEndpointAddress = priv->ep_in->address;
    ecm->epout_desc->bEndpointAddress = priv->ep_out->address;
    ecm->priv = priv;
    netdevInit(usbEtherGetEther(priv->usbe));
    return true;
}

void ecmDataUnbind(ecmData_t *ecm)
{
    if (ecm == NULL || ecm->priv == NULL)
        return;

    ecmPriv_t *p = (ecmPriv_t *)ecm->priv;
    netdevExit();
    usbEtherDestroy(p->usbe);
    udcEpFree(ecm->func->controller, p->ep_in);
    udcEpFree(ecm->func->controller, p->ep_out);
    ecm->priv = NULL;
    free(p);
}

bool ecmDataEnable(ecmData_t *ecm)
{
    OSI_LOGV(0, "ecm data enable");
    ecmPriv_t *p = (ecmPriv_t *)ecm->priv;
    if (p->enable)
    {
        OSI_LOGW(0, "ecm data enable, already enabled");
        return true;
    }

    udc_t *udc = ecm->func->controller;
    int retval = udcEpEnable(udc, p->ep_in);
    if (retval < 0)
    {
        OSI_LOGE(0, "ecm data enable ep in fail");
        return false;
    }

    retval = udcEpEnable(udc, p->ep_out);
    if (retval < 0)
        goto fail_out;

    p->enable = true;
    usbEtherStart(p->usbe);
    return true;

fail_out:
    udcEpDisable(udc, p->ep_in);
    return false;
}

void ecmDataDisable(ecmData_t *ecm)
{
    OSI_LOGV(0, "ecm data disable");
    uint32_t critical = osiEnterCritical();
    ecmPriv_t *p = (ecmPriv_t *)ecm->priv;
    if (!p->enable)
    {
        osiExitCritical(critical);
        return; // already disable
    }
    p->enable = false;
    osiExitCritical(critical);

    usbEtherStop(p->usbe);
    udc_t *udc = ecm->func->controller;
    udcEpDisable(udc, p->ep_in);
    udcEpDisable(udc, p->ep_out);
}
