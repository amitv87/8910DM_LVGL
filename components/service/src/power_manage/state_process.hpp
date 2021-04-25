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

#ifndef _STATE_PROCESS_HPP_
#define _STATE_PROCESS_HPP_

#include "osi_api.h"

namespace srvpm
{

class SrvPm;

enum PowerkeyState
{
    RELEASE = 0,
    PRESSED,
    LONG_PRESSED,
};

struct StateProcess
{
    explicit StateProcess(SrvPm &p) : pm(p) {}
    virtual ~StateProcess() {}

    virtual bool init() { return true; }
    virtual void setState() {}
    virtual void monitorBattery(uint16_t vol, bool on = true) {}
    virtual void chargePlugInWork(bool on) {}
    virtual void powerkeyStateChangeInWork(enum PowerkeyState state) {}

  protected:
    SrvPm &pm;
};

struct PonProcess final : public StateProcess
{
    explicit PonProcess(SrvPm &p) : StateProcess(p), long_pressed(false) {}

    virtual void setState() override;
    virtual void monitorBattery(uint16_t vol, bool on) override;
    virtual void chargePlugInWork(bool on) override;
    virtual void powerkeyStateChangeInWork(enum PowerkeyState state) override;

  private:
    bool long_pressed;
};

struct PoffChargeProcess final : public StateProcess
{
    explicit PoffChargeProcess(SrvPm &p) : StateProcess(p) {}

    virtual void monitorBattery(uint16_t vol, bool on) override;
    virtual void chargePlugInWork(bool on) override;
    virtual void powerkeyStateChangeInWork(enum PowerkeyState state) override;
};

} // namespace srvpm

#endif