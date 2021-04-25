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

#include "srv_keypad.h"
#include "osi_api.h"
#include "osi_log.h"
#include "string.h"
#include "srv_config.h"

namespace srvkpd
{

// TODO long pressed ...
struct KeyMap
{
    kpdHook_t hook_func;
    void *hook_ctx;
    bool pressed;
    int64_t timestamp;
};

class SrvKpd
{
  public:
    explicit SrvKpd() {}
    ~SrvKpd();

    bool init();
    void keypadEvent(keyMap_t id, keyState_t event);
    bool addHook(keyMap_t id, kpdHook_t hook_func, void *ctx, bool *cur_pressed);
    void removeHook(keyMap_t id);

  private:
    KeyMap keymap_[KEY_MAP_MAX_COUNT];
};

SrvKpd::~SrvKpd()
{
    drvKeypadSetCB(nullptr, 0, nullptr);
}

bool SrvKpd::init()
{
    drvKeypadInit();
    for (auto i = 0; i < KEY_MAP_MAX_COUNT; ++i)
    {
        keymap_[i].hook_func = nullptr;
        keymap_[i].hook_ctx = nullptr;
        keymap_[i].pressed = drvKeypadState((keyMap_t)i);
        keymap_[i].timestamp = osiUpTime();
    }

    auto kp_mask = KEY_STATE_PRESS | KEY_STATE_RELEASE;
    drvKeypadSetCB(
        [](keyMap_t id, keyState_t event, void *p) {
            auto kp = (SrvKpd *)p;
            kp->keypadEvent(id, event);
        },
        kp_mask, this);
    return true;
}

bool SrvKpd::addHook(keyMap_t id, kpdHook_t hook_func, void *ctx, bool *cur_pressed)
{
    if (id >= KEY_MAP_MAX_COUNT)
        return false;

    auto critical = osiEnterCritical();
    auto &k = keymap_[id];
    k.hook_func = hook_func;
    k.hook_ctx = ctx;
    if (cur_pressed)
        *cur_pressed = k.pressed;
    osiExitCritical(critical);
    return true;
}

void SrvKpd::keypadEvent(keyMap_t id, keyState_t event)
{
    if (id >= KEY_MAP_MAX_COUNT)
        return;

    auto &k = keymap_[id];
    int64_t ts = osiUpTime();
    uint32_t delta = ts - k.timestamp;
    k.timestamp = ts;

    bool pressed = (event == KEY_STATE_PRESS);
    if (pressed == k.pressed)
        return;
    k.pressed = pressed;

    OSI_LOGI(0, "key id %d/%p status %d, %u", id, k.hook_func, pressed, delta);
    if (k.hook_func)
    {
        k.hook_func(id, event, k.hook_ctx);
        return;
    }
}

} // namespace srvkpd

static srvkpd::SrvKpd *gSrvKpd = nullptr;

bool srvKeypadAddHook(keyMap_t id, kpdHook_t hook_func, void *ctx, bool *cur_pressed)
{
    auto kp = gSrvKpd;
    return kp->addHook(id, hook_func, ctx, cur_pressed);
}

void srvKeypadInit()
{
    auto kp = new srvkpd::SrvKpd();
    if (kp == nullptr)
    {
        OSI_LOGE(0, "fail create kp");
        osiPanic();
    }

    if (!kp->init())
    {
        OSI_LOGE(0, "fail init kp");
        delete kp;
        osiPanic();
    }

    gSrvKpd = kp;
}
