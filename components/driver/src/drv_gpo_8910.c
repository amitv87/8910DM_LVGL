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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('G', 'P', 'I', 'O')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_gpo.h"
#include "osi_api.h"
#include "osi_log.h"
#include "hwregs.h"
#include "hal_iomux.h"
#include <string.h>
#include <stdlib.h>

struct drvGpo
{
    uint32_t id;
    HWP_GPIO_T *hwp;
};

#define GPO_COUNT (8)

drvGpo_t *drvGpoOpen(uint32_t id)
{
    uint32_t critical = osiEnterCritical();
    if (id >= GPO_COUNT)
    {
        osiExitCritical(critical);
        return NULL;
    }
    drvGpo_t *d = (drvGpo_t *)malloc(sizeof(drvGpo_t));
    if (d == NULL)
    {
        osiExitCritical(critical);
        return NULL;
    }
    d->id = id;
    d->hwp = hwp_gpio1;

    osiExitCritical(critical);
    return d;
}

void drvGpoClose(drvGpo_t *d)
{
    uint32_t critical = osiEnterCritical();

    if (d == NULL)
    {
        osiExitCritical(critical);
        return;
    }
    d->hwp->gpo_clr_reg = 1 << d->id;
    d->hwp->gpo_set_reg = ((1 << d->id) & 0xAA);

    free(d);
    osiExitCritical(critical);
}

drvGpoLevel drvGpoRead(drvGpo_t *d)
{
    uint32_t critical = osiEnterCritical();

    if (d == NULL)
    {
        osiExitCritical(critical);
        return GPO_LEVEL_INVALID;
    }

    uint32_t lid = d->id;
    drvGpoLevel res = (d->hwp->gpo_set_reg >> lid) & 1;
    osiExitCritical(critical);
    return res;
}

void drvGpoWrite(drvGpo_t *d, drvGpoLevel level)
{
    uint32_t critical = osiEnterCritical();

    if (d == NULL)
    {
        osiExitCritical(critical);
        return;
    }
    uint32_t lid = d->id;
    if (level == GPO_LEVEL_HIGH)
        d->hwp->gpo_set_reg = (1 << lid);
    else
        d->hwp->gpo_clr_reg = (1 << lid);
    osiExitCritical(critical);
}
