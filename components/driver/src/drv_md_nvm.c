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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'V', 'M', 'M')

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osi_api.h"
#include "osi_log.h"
#include "drv_md_ipc.h"
#include "drv_md_nvm.h"
#include "vfs.h"
#include "hal_shmem_region.h"
#include "hal_config.h"
#include "diag.h"
#include "fs_mount.h"
#include "nvm.h"
#include "calclib/crc32.h"
#include <errno.h>

#define MD_NVM_THREAD_PRIORITY (OSI_PRIORITY_NORMAL)
#define MD_NVM_STACK_SIZE (2048 * 4)
#define MD_NVM_EVENT_QUEUE_SIZE (32)

// NV cmd id
#define NV_CMD_CREATE 1
#define NV_CMD_OPEN 2
#define NV_CMD_READ 3
#define NV_CMD_WRITE 4
#define NV_CMD_CALIB 5
#define NV_CMD_IWRITE 6

// NV process result
#define NV_CMD_SUCCESS 0
#define NV_CMD_FAILURE 1

typedef struct
{
    osiThread_t *thread;
    struct smd_ch *chan;
    osiMutex_t *lock;
    diagMsgHead_t pending_diag_head;
} drvNvmContext_t;

static drvNvmContext_t gNvmCtx;

static void _nvmResponse(struct ipc_cmd *cmd, bool result)
{
    drvNvmContext_t *nvm = &gNvmCtx;
    int ipc_ret;

    cmd->para1 = result ? NV_CMD_SUCCESS : NV_CMD_FAILURE;

    osiMutexLock(nvm->lock);
    if ((ipc_ret = ipc_ch_write(gNvmCtx.chan, cmd, 1)) < 0)
        OSI_LOGE(0, "NVM: write ack faild err/%d", ipc_ret);
    osiMutexUnlock(nvm->lock);
}

static void _nvmCommand(struct ipc_cmd *cmd)
{
    drvNvmContext_t *nvm = &gNvmCtx;
    unsigned nv_id = cmd->id;
    unsigned nv_ops = cmd->para0;
    unsigned nv_paddr = cmd->para1;
    unsigned nv_len = cmd->para2;

    OSI_LOGI(0, "NVM id/%d ops/%d paddr/%p len/%d", nv_id, nv_ops, nv_paddr, nv_len);

    if (nv_ops == NV_CMD_CALIB)
    {
        osiMutexLock(nvm->lock);
        diagMsgHead_t head = nvm->pending_diag_head;
        osiMutexUnlock(nvm->lock);

        diagOutputPacket2(&head, (void *)nv_paddr, nv_len);
        return;
    }

    if (nv_ops == NV_CMD_READ || nv_ops == NV_CMD_WRITE)
    {
        const halShmemRegion_t *region = halShmemGetRegionByAddr(nv_paddr);
        if (region == NULL)
        {
            OSI_LOGE(0, "NVM: failed to get region for address 0x%x", nv_paddr);
            _nvmResponse(cmd, false);
            return;
        }

        int nvid = nvmGetIdFromFileName(region->name);
        if (nvid < 0)
        {
            OSI_LOGXE(OSI_LOGPAR_S, 0, "NVM: failed to get nvid for %s", region->name);
            _nvmResponse(cmd, false);
            return;
        }

        if (nv_ops == NV_CMD_READ)
        {
            if (nvmReadItem(nvid, (void *)nv_paddr, nv_len) < 0)
            {
                OSI_LOGXE(OSI_LOGPAR_SIII, 0, "NVM: read failed, name/%s address/0x%x size/0x%x nvid/0x%x",
                          region->name, nv_paddr, nv_len, nvid);
                _nvmResponse(cmd, false);
                return;
            }
        }
        else // NV_CMD_WRITE
        {
            if (nvmWriteItem(nvid, (void *)nv_paddr, nv_len) < 0)
            {
                OSI_LOGXE(OSI_LOGPAR_SIII, 0, "NVM: write failed, name/%s address/0x%x size/0x%x nvid/0x%x",
                          region->name, nv_paddr, nv_len, nvid);
                _nvmResponse(cmd, false);
                return;
            }
        }

        _nvmResponse(cmd, true);
        return;
    }

    if (nv_ops == NV_CMD_IWRITE)
    {
        // write ims nv, needn't response when sucess
        // sharemem ap_cp_sm, nvitemid offset size
        const halShmemRegion_t *region = halShmemGetRegion(MEM_AP_CP_SM_NAME);
        if (nv_paddr > region->size)
        {
            OSI_LOGE(0, "NVM: failed to get region for offset 0x%x", nv_paddr);
            _nvmResponse(cmd, false);
            return;
        }
        unsigned ims_nv_addr = region->address + nv_paddr;
        if (nvmWriteItem(nv_id, (void *)ims_nv_addr, nv_len) < 0)
        {
            OSI_LOGXE(OSI_LOGPAR_SIII, 0, "NVM: write failed, name/%s address/0x%x size/0x%x nvid/0x%x",
                      region->name, ims_nv_addr, nv_len, nv_id);
            _nvmResponse(cmd, false);
            return;
        }

        // At success, it is not needed and shouldn't response to CP.
    }

    OSI_LOGE(0, "NVM: invalid ops");
}

static void _nvmHandler(void *param)
{
    drvNvmContext_t *nvm = &gNvmCtx;
    struct ipc_cmd cmd;
    int ipc_ret;

    for (;;)
    {
        // read only occurs in one thread, lock is needed.
        int32_t avail = ipc_ch_read_avail(nvm->chan);
        if (avail <= 0)
            break;

        if ((ipc_ret = ipc_ch_read(nvm->chan, &cmd, 1)) < 0)
        {
            OSI_LOGE(0, "NVM: read ipc cmd faild err/%d", ipc_ret);
            break;
        }

        _nvmCommand(&cmd);
    }
}

static void _nvmNotify(void *priv, uint32_t event)
{
    osiThreadCallback(gNvmCtx.thread, _nvmHandler, (void *)event);
}

static void _threadEntry(void *args)
{
    gNvmCtx.thread = osiThreadCurrent();

    for (;;)
    {
        osiEvent_t event;
        osiEventWait(gNvmCtx.thread, &event);
    }
}

void drvNvmIpcInit(void)
{
    OSI_LOGI(0, "NVM init");

    if (ipc_ch_open(SMD_CH_A2C_CTRL, &gNvmCtx.chan, NULL, _nvmNotify) < 0)
    {
        OSI_LOGE(0, "NVM open channel failed");
        osiPanic();
    }

    gNvmCtx.lock = osiMutexCreate();
    gNvmCtx.thread = osiThreadCreate("md_nvm", _threadEntry, NULL,
                                     MD_NVM_THREAD_PRIORITY, MD_NVM_STACK_SIZE,
                                     MD_NVM_EVENT_QUEUE_SIZE);
}

void drvNvmSendCalibCmd(const diagMsgHead_t *cmd)
{
    drvNvmContext_t *nvm = &gNvmCtx;

    const void *param = diagCmdData(cmd);
    unsigned size = diagCmdDataSize(cmd);

    // use aud calib buffer to transform calib cmd and data
    const halShmemRegion_t *r = halShmemGetRegion(MEM_AUDIO_CALIB_NAME);
    if (r == NULL || r->size < size)
        osiPanic();

    uint32_t data_crc = crc32Calc(param, size);

    // copy the calib command parameter to shared memory
    memcpy((void *)r->address, param, size);

    struct ipc_cmd ipc_cmd;
    ipc_cmd.id = cmd->type | (cmd->subtype << 8);
    ipc_cmd.para0 = NV_CMD_CALIB;
    ipc_cmd.para1 = r->address;
    ipc_cmd.para2 = size;

    osiMutexLock(nvm->lock);
    nvm->pending_diag_head = *cmd;

    OSI_LOGI(0, "NVM write id/%d ops/%d paddr/%p len/%d, crc 0x%x", ipc_cmd.id, ipc_cmd.para0, ipc_cmd.para1, ipc_cmd.para2, data_crc);
    int result = ipc_ch_write(gNvmCtx.chan, &ipc_cmd, 1);
    if (result < 0)
        OSI_LOGE(0, "NVM: write ack faild err/%d", result);

    osiMutexUnlock(nvm->lock);
}
