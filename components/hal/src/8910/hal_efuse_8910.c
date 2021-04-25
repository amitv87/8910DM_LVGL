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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'F', 'U', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
#include <stdlib.h>
#include <sys/queue.h>
#include "hwregs.h"
#include "osi_log.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "osi_log.h"
#include "hal_efuse.h"
#include "hal_hwspinlock.h"

#define readl(addr) (*(volatile unsigned int *)(addr))
#define writel(val, addr) (*(volatile unsigned int *)(addr) = (val))

#define EFUSE_AP_USE_BIT (1 << 1)
#define EFUSE_USE_MAP hwp_mailbox->sysmail100

static inline bool prvBlockIsValid(int32_t block_index)
{
    return (block_index > RDA_EFUSE_BLOCK_LOCK_7_INDEX && block_index <= RDA_EFUSE_BLOCK_MAX_INDEX);
}

static inline void prvEfuseSetDouble(bool enable)
{
    REG_EFUSE_CTRL_EFUSE_SEC_EN_T sec_en;
    REG_FIELD_CHANGE1(hwp_efuseCtrl->efuse_sec_en, sec_en, double_bit_en_sec, enable ? 1 : 0);
}

static bool prvEfuseRead(uint32_t block_index, uint32_t *val)
{
    *val = readl((uint32_t)&hwp_efuseCtrl->efuse_mem + block_index * 4);
    return (hwp_efuseCtrl->efuse_sec_err_flag == 0);
}

static bool prvEfuseWrite(uint32_t block_index, uint32_t val)
{

    REG_EFUSE_CTRL_EFUSE_PW_SWT_T pw_swt = {};
    pw_swt.b.ns_s_pg_en = 1;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;

    pw_swt.b.efs_enk2_on = 0;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;
    osiDelayUS(1000);

    pw_swt.b.efs_enk1_on = 1;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;
    osiDelayUS(1000);

    writel(val, (uint32_t)&hwp_efuseCtrl->efuse_mem + block_index * 4);

    pw_swt.b.efs_enk1_on = 0;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;
    osiDelayUS(1000);

    pw_swt.b.efs_enk2_on = 1;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;
    osiDelayUS(1000);

    pw_swt.b.ns_s_pg_en = 0;
    hwp_efuseCtrl->efuse_pw_swt = pw_swt.v;

    return (hwp_efuseCtrl->efuse_sec_err_flag == 0);
}

void halEfuseOpen(void)
{
    uint32_t lock = halHwspinlockAcquire(HAL_HWSPINLOCK_ID_EFUSE);
    if (EFUSE_USE_MAP == 0)
    {
        REG_EFUSE_CTRL_EFUSE_SEC_MAGIC_NUMBER_T magic = {};
        magic.b.sec_efuse_magic_number = 0x8910;
        hwp_efuseCtrl->efuse_sec_magic_number = magic.v;

        REG_EFUSE_CTRL_EFUSE_SEC_EN_T sec_en = {hwp_efuseCtrl->efuse_sec_en};
        sec_en.b.sec_vdd_en = 1;
        hwp_efuseCtrl->efuse_sec_en = sec_en.v;
        osiDelayUS(1000);
    }
    EFUSE_USE_MAP |= EFUSE_AP_USE_BIT;
    halHwspinlockRelease(lock, HAL_HWSPINLOCK_ID_EFUSE);
}

void halEfuseClose(void)
{
    uint32_t lock = halHwspinlockAcquire(HAL_HWSPINLOCK_ID_EFUSE);
    EFUSE_USE_MAP &= ~EFUSE_AP_USE_BIT;
    if (EFUSE_USE_MAP == 0)
    {
        REG_EFUSE_CTRL_EFUSE_SEC_EN_T sec_en = {hwp_efuseCtrl->efuse_sec_en};
        sec_en.b.sec_vdd_en = 0;
        hwp_efuseCtrl->efuse_sec_en = sec_en.v;

        hwp_efuseCtrl->efuse_sec_magic_number = 0;
        osiDelayUS(1000);
    }
    halHwspinlockRelease(lock, HAL_HWSPINLOCK_ID_EFUSE);
}

bool halEfuseWrite(uint32_t block_index, uint32_t val)
{
    if (!prvBlockIsValid(block_index))
        return false;

    prvEfuseSetDouble(false);
    return prvEfuseWrite(block_index, val);
}

bool halEfuseDoubleWrite(uint32_t block_index, uint32_t val)
{
    if (!prvBlockIsValid(block_index * 2))
        return false;

    prvEfuseSetDouble(true);
    return prvEfuseWrite(block_index, val);
}

bool halEfuseRead(uint32_t block_index, uint32_t *val)
{
    if (!prvBlockIsValid(block_index))
        return false;

    if (val == NULL)
        return false;

    prvEfuseSetDouble(false);
    return prvEfuseRead(block_index, val);
}

bool halEfuseDoubleRead(uint32_t block_index, uint32_t *val)
{
    if (!prvBlockIsValid(block_index * 2))
        return false;

    if (val == NULL)
        return false;

    prvEfuseSetDouble(true);
    return prvEfuseRead(block_index, val);
}
