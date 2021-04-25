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
#include "osi_api.h"
#include "osi_chip.h"
#include "osi_internal.h"
#include "hwregs.h"
#include "vfs.h"
#include "pb_util.h"
#include "osi_sysnv.pb.h"
#include <stdlib.h>
#include <sys/time.h>
#define __SYSNV_DEFINE_VAR
#include "osi_sysnv.h"

// ATTENTION: alloca is used for load/save memory.
//      Pay attention to system NV size

#define SYSNV_FNAME CONFIG_FS_AP_NVM_DIR "/sys_osi.nv"

static bool osiSysnvDecode(void *buf, size_t size)
{
    const pb_field_t *fields = PB_SysNvm_fields;
    PB_SysNvm pb;
    PB_SysNvm *pbs = &pb;

    pb_istream_t istrm = pb_istream_from_buffer(buf, size);
    if (!pb_decode(&istrm, fields, &pb))
        return false;

    PB_OPT_DEC_ASSIGN(gSysnvTraceEnabled, gSysnvTraceEnabled);
    PB_OPT_DEC_ASSIGN(gSysnvDeepSleepEnabled, gSysnvDeepSleepEnabled);
    PB_OPT_DEC_ASSIGN(gSysnvPsmSleepEnabled, gSysnvPsmSleepEnabled);
    PB_OPT_DEC_ASSIGN(gSysnvUsbWorkMode, gSysnvUsbWorkMode);
    PB_OPT_DEC_ASSIGN(gSysnvTraceDevice, gSysnvTraceDevice);
    PB_OPT_DEC_ASSIGN(gSysnvDiagDevice, gSysnvDiagDevice);
    PB_OPT_DEC_ASSIGN(gSysnvSimHotPlug, gSysnvSimHotPlug);
    PB_OPT_DEC_ASSIGN(gSysnvSimVoltTrigMode, gSysnvSimVoltTrigMode);
    PB_OPT_DEC_ASSIGN(gSysnvUsbDetMode, gSysnvUsbDetMode);
    PB_OPT_DEC_ASSIGN(gSysnvNATCfg, gSysnvNATCfg);
    PB_OPT_DEC_ASSIGN(gSysnvFstraceMask, gSysnvFstraceMask);
    PB_OPT_DEC_ASSIGN(gSysnvPoffChargeEnable, gSysnvPoffChargeEnable);
    return true;
}

static int osiSysnvEncode(void *buf, size_t size)
{
    const pb_field_t *fields = PB_SysNvm_fields;
    PB_SysNvm pb = {};
    PB_SysNvm *pbs = &pb;

    PB_OPT_ENC_ASSIGN(gSysnvTraceEnabled, gSysnvTraceEnabled);
    PB_OPT_ENC_ASSIGN(gSysnvDeepSleepEnabled, gSysnvDeepSleepEnabled);
    PB_OPT_ENC_ASSIGN(gSysnvPsmSleepEnabled, gSysnvPsmSleepEnabled);
    PB_OPT_ENC_ASSIGN(gSysnvUsbWorkMode, gSysnvUsbWorkMode);
    PB_OPT_ENC_ASSIGN(gSysnvTraceDevice, gSysnvTraceDevice);
    PB_OPT_ENC_ASSIGN(gSysnvDiagDevice, gSysnvDiagDevice);
    PB_OPT_ENC_ASSIGN(gSysnvSimHotPlug, gSysnvSimHotPlug);
    PB_OPT_ENC_ASSIGN(gSysnvSimVoltTrigMode, gSysnvSimVoltTrigMode);
    PB_OPT_ENC_ASSIGN(gSysnvUsbDetMode, gSysnvUsbDetMode);
    PB_OPT_ENC_ASSIGN(gSysnvNATCfg, gSysnvNATCfg);
    PB_OPT_ENC_ASSIGN(gSysnvFstraceMask, gSysnvFstraceMask);
    PB_OPT_ENC_ASSIGN(gSysnvPoffChargeEnable, gSysnvPoffChargeEnable);
    return pbEncodeToMem(fields, pbs, buf, size);
}

bool osiSysnvLoad(void)
{
    vfs_sfile_init(SYSNV_FNAME);

    int file_size = vfs_sfile_size(SYSNV_FNAME);
    if (file_size <= 0)
        return false;

    void *buf = alloca(file_size);
    if (vfs_sfile_read(SYSNV_FNAME, buf, file_size) != file_size ||
        !osiSysnvDecode(buf, file_size))
        return false;

    return true;
}

bool osiSysnvSave(void)
{
    int nv_size = osiSysnvEncode(NULL, 0);
    if (nv_size < 0)
        return false;

    void *buf = alloca(nv_size);
    if (osiSysnvEncode(buf, nv_size) < 0 ||
        vfs_sfile_write(SYSNV_FNAME, buf, nv_size) != nv_size)
        return false;

    return true;
}

void osiSysnvClear(void)
{
    vfs_unlink(SYSNV_FNAME);
}

int osiSysnvSize(void)
{
    return osiSysnvEncode(NULL, 0);
}

int osiSysnvLoadToMem(void *buf, size_t size)
{
    return vfs_sfile_read(SYSNV_FNAME, buf, size);
}

int osiSysnvSaveFromMem(const void *buf, size_t size)
{
    return vfs_sfile_write(SYSNV_FNAME, buf, size);
}
