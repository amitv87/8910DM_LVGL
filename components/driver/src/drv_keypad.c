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

#include "drv_keypad.h"
#include "drv_pmic_intr.h"
#include "osi_log.h"
#include "osi_api.h"
#include "hwregs.h"
#include <stdlib.h>

#define SCAN_ROW_COL(row, col) ((row)*8 + (col) + 1)
#define BBATKEYCODE_NUM 36

typedef struct
{
    uint8_t scan;
    uint8_t key_code;
} drvKeypadScanMap_t;
#include "drv_keypad_def.h"

typedef struct
{
    uint8_t debunceTime;
    uint8_t kpItvTime;
    uint8_t kp_in_mask;
    uint8_t kp_out_mask;
} drvKeyPadCfg;

typedef struct
{
    uint8_t row;
    uint8_t col;
} coordinate_t;

typedef struct key_condition
{
    bool pressed;
    struct
    {
        uint32_t mask;
        void *ctx;
    } handle;
} keyCond_t;

typedef struct keypad_context
{
    keyCond_t cond[KEY_MAP_MAX_COUNT];
    keyEventCb_t cb;
} keypadCtx_t;

static keypadCtx_t *gDrvKeyPad = NULL;
static uint32_t g_previousKeys[2];

/**
 * Get key code from scan (row, col), return -1 for invalid
 */
#ifdef CONFIG_QUEC_PROJECT_FEATURE_KEYPAD

#define KEYPAD_MAXROW  5
#define KEYPAD_MAXCOL  5
static int prvGetKeyCode(uint8_t row, uint8_t col)
{
    if(row > KEYPAD_MAXROW || col > KEYPAD_MAXCOL || col ==0)
    {
        return -1;
    }
    else 
    {
        return row * KEYPAD_MAXCOL + col;
    }
}
#else
static int prvGetKeyCode(uint8_t row, uint8_t col)
{
    unsigned scan = SCAN_ROW_COL(row, col);
    for (unsigned n = 0; n < OSI_ARRAY_SIZE(gKeyMatrix); n++)
    {
        if (gKeyMatrix[n].scan == scan)
            return gKeyMatrix[n].key_code;
    }
    return -1;
}
#endif

static void _keyCallback(keyMap_t id, uint32_t event, bool status)
{
    keypadCtx_t *key = gDrvKeyPad;
    keyCond_t *cond = NULL;
    if (key != NULL)
    {
        cond = &key->cond[id];
        cond->pressed = status;
        if (cond->handle.mask)
            event &= cond->handle.mask;
        if (event && key->cb)
            key->cb(id, event, cond->handle.ctx);
        else
        {
            OSI_LOGI(0, "mask or event set error");
        }
    }
}

/**
 * Get all press keys code from register,values can not more than 10 .
 * bbat test use api
 */

int bbatGetAllScanKeyId(uint8_t *key, int *count)
{
    uint8_t i;
    uint8_t mask = 0;
    uint32_t kp_data[2];
    int id = 0;
    kp_data[0] = hwp_keypad->kp_data_l;
    kp_data[1] = hwp_keypad->kp_data_h;
    if (kp_data[0] == 0 && kp_data[1] == 0)
    {
        return -1;
    }
    for (i = 0; i < LOW_KEY_NB; i++)
    {
        if ((kp_data[0] & (1 << i)) != 0)
        {
            mask = i;
            id = prvGetKeyCode(mask / 8, mask % 8);
            if (id < 0)
                continue;
            key[*count] = id;
            *count = *count + 1;
            if (*count >= BBATKEYCODE_NUM)
                return -1;
        }
    }
    for (i = 0; i < HIGH_KEY_NB; i++)
    {
        if ((kp_data[1] & (1 << i)) != 0)
        {
            mask = i + LOW_KEY_NB;
            id = prvGetKeyCode(mask / 8, mask % 8);
            if (id < 0)
                continue;
            key[*count] = id;
            *count = *count + 1;
            if (*count >= BBATKEYCODE_NUM)
                return -1;
        }
    }
    return 1;
}

static void _keypadSearch(uint32_t *keys, uint32_t event, bool status)
{
    uint8_t i;
    uint8_t mask = 0;
    int id = KEY_MAP_MAX_COUNT;

    for (i = 0; i < LOW_KEY_NB; i++)
    {
        if ((keys[0] & (1 << i)) != 0)
        {
            mask = i;
            id = prvGetKeyCode(mask / 8, mask % 8);
            if (id < 0)
                continue;
            _keyCallback(id, event, status);
        }
    }
    // high keys
    for (i = 0; i < HIGH_KEY_NB; i++)
    {
        if ((keys[1] & (1 << i)) != 0)
        {
            mask = i + LOW_KEY_NB;
            id = prvGetKeyCode(mask / 8, mask % 8);
            if (id < 0)
                continue;
            _keyCallback(id, event, status);
        }
    }
}

static void _KeypadIsrCB(void *p)
{
    uint32_t scannedKeys[2];
    uint32_t downKeys[2];
    uint32_t upKeys[2];

    REG_KEYPAD_KP_IRQ_CAUSE_T cause = {hwp_keypad->kp_irq_cause};
    hwp_keypad->kp_irq_clr = 1; // Clear IRQ
    if (cause.b.kp_evt1_irq_cause)
    {
        _keypadSearch(g_previousKeys, KEY_STATE_RELEASE, false);
        g_previousKeys[0] = 0;
        g_previousKeys[1] = 0;
    }
    else if (cause.b.kp_evt0_irq_cause)
    {
        scannedKeys[0] = hwp_keypad->kp_data_l;
        scannedKeys[1] = hwp_keypad->kp_data_h;
        if (scannedKeys[0] != g_previousKeys[0] || scannedKeys[1] != g_previousKeys[1])
        {

            downKeys[0] = scannedKeys[0] & ~g_previousKeys[0];
            downKeys[1] = scannedKeys[1] & ~g_previousKeys[1];

            upKeys[0] = ~scannedKeys[0] & g_previousKeys[0];
            upKeys[1] = ~scannedKeys[1] & g_previousKeys[1];

            if (downKeys[0] != 0 || downKeys[1] != 0)
            {
                _keypadSearch(downKeys, KEY_STATE_PRESS, true);
            }
            else if (upKeys[0] != 0 || upKeys[1] != 0)
            {
                _keypadSearch(upKeys, KEY_STATE_RELEASE, false);
            }
            g_previousKeys[0] = scannedKeys[0];
            g_previousKeys[1] = scannedKeys[1];
        }
    }
}

void drvKeypadSetCB(keyEventCb_t cb, uint32_t mask, void *ctx)
{
    uint8_t i;
    if (gDrvKeyPad == NULL)
        return;
    keypadCtx_t *key = gDrvKeyPad;
    key->cb = cb;
    for (i = 0; i < KEY_MAP_MAX_COUNT; i++)
    {
        keyCond_t *cond = &key->cond[i];
        cond->handle.mask = mask;
        cond->handle.ctx = ctx;
    }
}

int drvKeypadState(keyMap_t id)
{
    if (id < 0 || id >= KEY_MAP_MAX_COUNT)
        return -1;

    if (gDrvKeyPad == NULL)
        return -1;

    keypadCtx_t *key = gDrvKeyPad;
    return key->cond[id].pressed ? 1 : 0;
}

void drvKeypadOpen(drvKeyPadCfg *cfg)
{
    REG_KEYPAD_KP_CTRL_T config_keypad;
    REG_KEYPAD_KP_IRQ_MASK_T config_mask;
    REG_KEYPAD_KP_IRQ_CLR_T config_clr;

    //config keypad ctrl
    config_keypad.b.kp_en = 1;
    config_keypad.b.kp_dbn_time = cfg->debunceTime;
    config_keypad.b.kp_itv_time = cfg->kpItvTime;
    config_keypad.b.kp_in_mask = cfg->kp_in_mask;
    config_keypad.b.kp_out_mask = cfg->kp_out_mask;
    hwp_keypad->kp_ctrl = config_keypad.v;
    //config keypad mask
    config_mask.b.kp_evt0_irq_mask = 1;
    config_mask.b.kp_evt1_irq_mask = 1;
    config_mask.b.kp_itv_irq_mask = 0;
    hwp_keypad->kp_irq_mask = config_mask.v;
    //config keypad clr
    config_clr.b.kp_irq_clr = 1;
    hwp_keypad->kp_irq_clr = config_clr.v;
    g_previousKeys[0] = 0;
    g_previousKeys[1] = 0;
}

void drvKeypadInit()
{
    if (gDrvKeyPad != NULL)
        return;
    keypadCtx_t *key = (keypadCtx_t *)calloc(1, sizeof(keypadCtx_t));
    if (key == NULL)
        return;
    gDrvKeyPad = key;

    drvKeyPadCfg cfg;
    cfg.debunceTime = 8;
    cfg.kpItvTime = 16;
    cfg.kp_in_mask = 0x3f;
    cfg.kp_out_mask = 0x3f;
    drvKeypadOpen(&cfg);

    //config keypad interupt
    uint32_t irq_keypad = HAL_SYSIRQ_NUM(SYS_IRQ_ID_KEYPAD);
    osiIrqDisable(irq_keypad);
    osiIrqSetHandler(irq_keypad, _KeypadIsrCB, NULL);
    osiIrqSetPriority(irq_keypad, SYS_IRQ_PRIO_KEYPAD);
    osiIrqEnable(irq_keypad);

    for (unsigned i = 0; i < KEY_MAP_MAX_COUNT; ++i)
    {
        keyCond_t *cond = &key->cond[i];
        cond->pressed = false;
    }
}
