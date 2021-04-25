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

//#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_ERROR

#include "drv_powerkey_8910.h"
#include "drv_pmic_intr.h"
#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"
#include <stdlib.h>


#define POWERKEY_PRESS_DEBOUNCE (50)
#define POWERKEY_RELEASE_DEBOUBCE (10)

typedef struct
{
    uint8_t pressed;
    uint32_t mask;
    powerkeyCb_t cb;
    void *ctx;
} drvpwrkey_t;

static drvpwrkey_t *gDrvPwrKey = NULL;

static void _pwrKeyIsrCB(void *p)
{

    drvpwrkey_t *key = (drvpwrkey_t *)p;
    uint32_t event;

    if (key->pressed)
    {
        key->pressed = false;
        event = PWR_STATE_RELEASE;
        drvPmicEicTrigger(DRV_PMIC_EIC_PBINT, POWERKEY_PRESS_DEBOUNCE, false);
    }
    else
    {
        key->pressed = true;
        event = PWR_STATE_PRESS;
        drvPmicEicTrigger(DRV_PMIC_EIC_PBINT, POWERKEY_RELEASE_DEBOUBCE, true);
    }
    event &= key->mask;
    if (event && key->cb)
        key->cb(event, key->ctx);
}

void drvPwrKeySetCB(powerkeyCb_t cb, uint32_t mask, void *ctx)
{
    if (gDrvPwrKey == NULL)
        return;

    drvpwrkey_t *key = gDrvPwrKey;
    key->cb = cb;
    key->mask = mask;
    key->ctx = ctx;
}

void drvPowerkeyInit()
{
    if (gDrvPwrKey != NULL)
        return;

    drvpwrkey_t *key = (drvpwrkey_t *)calloc(1, sizeof(drvpwrkey_t));
    if (key == NULL)
        return;

    gDrvPwrKey = key;
    key->pressed = false;
    key->mask = PWR_STATE_RELEASE;

    drvPmicEicSetCB(DRV_PMIC_EIC_PBINT, _pwrKeyIsrCB, key);
    drvPmicEicTrigger(DRV_PMIC_EIC_PBINT, POWERKEY_PRESS_DEBOUNCE, false);
}
