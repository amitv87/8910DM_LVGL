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

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/reent.h>
#include "hal_config.h"
#include "cmsis_core.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "osi_profile.h"
#include "osi_tick_unit.h"
#include "osi_mem.h"
#include "osi_log.h"
#include "osi_trace.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "hal_mmu.h"
#include "hal_adi_bus.h"
#include "hal_cp_ctrl.h"
#include "hal_iomux.h"
#include "drv_md_ipc.h"
#include "drv_md_nvm.h"
#include "drv_ipc_at.h"
#include "hal_spi_flash.h"
#include "hal_hwspinlock.h"
#include "hal_efuse.h"
#include <string.h>

#if 0
#define SUSPEND_DEBUG
#else
#define SUSPEND_DEBUG hwp_idle->idle_res9 = __LINE__
#endif

#define IDLE_SLEEP_ENABLE_MAGIC 0x49444c45
#define AP_WAKEUP_JUMP_MAGIC 0xD8E5BEAF
#define PMIC_PWR_PROT_MAGIC 0x6e7f

#define AP_WAKEUP_JUMP_MAGIC_REG (hwp_idle->idle_res0)
#define AP_WAKEUP_JUMP_ADDR_REG (hwp_idle->idle_res1)

#define AP_SRAM_BACKUP_START_SYM __sramboottext_start
#define AP_SRAM_BACKUP_END_SYM __srambss_end

#define AP_SRAM_BACKUP_START ((uintptr_t)(&AP_SRAM_BACKUP_START_SYM))
#define AP_SRAM_BACKUP_END ((uintptr_t)(&AP_SRAM_BACKUP_END_SYM))
#define AP_SRAM_BACKUP_SIZE (AP_SRAM_BACKUP_END - AP_SRAM_BACKUP_START)
#define AP_SRAM_BACKUP_ALLOC_SIZE (CONFIG_APP_SRAM_SIZE - CONFIG_IRQ_STACK_SIZE - CONFIG_SVC_STACK_SIZE - CONFIG_BLUE_SCREEN_SIZE)

#define AP_SRAM_SHMEM_START (CONFIG_SRAM_PHY_ADDRESS + CONFIG_APP_SRAM_SHMEM_OFFSET)

typedef struct
{
    osiSuspendMode_t mode;
    uint32_t reg_sys[11]; // r4-r14
    uint32_t cpsr_sys;
    uint32_t ttbr0;
    uint32_t dacr;
    uint32_t sys_ctrl_cfg_misc_cfg;
    uint8_t sram[AP_SRAM_BACKUP_ALLOC_SIZE];
    uint8_t sram_shmem[CONFIG_APP_SRAM_SHMEM_SIZE];
} cpuSuspendContext_t;

static cpuSuspendContext_t gCpuSuspendCtx;

extern void osiWakePm1Entry(void);
extern void osiWakePm2Entry(void);

/**
 * Backup SRAM to PSRAM, due to SRAM will be power off during suspend.
 */
static void prvSramBackup(void)
{
    extern uint32_t AP_SRAM_BACKUP_START_SYM;
    extern uint32_t AP_SRAM_BACKUP_END_SYM;
    memcpy(gCpuSuspendCtx.sram, (void *)AP_SRAM_BACKUP_START, AP_SRAM_BACKUP_SIZE);
    memcpy(gCpuSuspendCtx.sram_shmem, (void *)AP_SRAM_SHMEM_START, CONFIG_APP_SRAM_SHMEM_SIZE);
}

/**
 * Restore SRAM from PSRAM
 */
static void prvSramRestore(void)
{
    extern uint32_t AP_SRAM_BACKUP_START_SYM;
    extern uint32_t AP_SRAM_BACKUP_END_SYM;
    memcpy((void *)AP_SRAM_BACKUP_START, gCpuSuspendCtx.sram, AP_SRAM_BACKUP_SIZE);
    memcpy((void *)AP_SRAM_SHMEM_START, gCpuSuspendCtx.sram_shmem, CONFIG_APP_SRAM_SHMEM_SIZE);
}

/**
 * Save context (r4-r14) to specified memory, and switch stack to
 * SRAM, and call specified function. If \p post returns, this will
 * return true, to indicate suspend fail.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
__attribute__((naked, target("arm"))) static bool prvInvokeSuspendPost(void *ctx, void (*post)(void))
{
    asm volatile("stmia  r0, {r4-r14}\n"
                 "mov    r2, sp\n"
                 "ldr    sp, =__svc_stack_end\n"
                 "push   {r2, lr}\n"
                 "blx    r1\n"
                 "pop    {r2, lr}\n"
                 "mov    sp, r2\n"
                 "mov    r0, #1\n"
                 "bx     lr\n");
}
#pragma GCC diagnostic pop

/**
 * Restore context from specified memory, and return to \p lr in the
 * context, rather then caller of this. The return value for \p lr
 * (not for caller) is false, to indicate resume.
 */
__attribute__((naked, target("arm"))) static void prvRestoreContext(void *ctx)
{
    asm volatile("ldmia   r0, {r4-r14}\n"
                 "mov     r0, #0\n"
                 "bx      lr");
}

/**
 * MMU initialization for cold boot
 */
static inline void prvMmuInit(void)
{
    // Invalidate entire Unified TLB
    __set_TLBIALL(0);

    // Invalidate entire branch predictor array
    __set_BPIALL(0);
    __DSB();
    __ISB();

    //  Invalidate instruction cache and flush branch target cache
    __set_ICIALLU(0);
    __DSB();
    __ISB();

    //  Invalidate data cache
    L1C_InvalidateDCacheAll();
    L1C_InvalidateICacheAll();

    // Create Translation Table
    halMmuCreateTable();

    // Enable MMU
    MMU_Enable();

    // Enable Caches
    L1C_EnableCaches();
    L1C_EnableBTAC();
}

/**
 * MMU initialization for wakeup. TTB is restored from RAM.
 */
static void prvMmuWakeInit(void)
{
    // Invalidate entire Unified TLB
    __set_TLBIALL(0);

    // Invalidate entire branch predictor array
    __set_BPIALL(0);
    __DSB();
    __ISB();

    //  Invalidate instruction cache and flush branch target cache
    __set_ICIALLU(0);
    __DSB();
    __ISB();

    // Invalidate data cache
    L1C_InvalidateDCacheAll();

    // Restore Translation Table
    __set_TTBR0(gCpuSuspendCtx.ttbr0);
    __ISB();
    __set_DACR(gCpuSuspendCtx.dacr);
    __ISB();

    // Enable MMU
    MMU_Enable();

    // Enable Caches
    L1C_EnableCaches();
    L1C_EnableBTAC();
}

/**
 * Initialize heap
 */
static void prvHeapInit(void)
{
    extern uint32_t __heap_start;
    extern uint32_t __heap_end;

    void *heap_start = (void *)&__heap_start;
    uint32_t heap_size = (uint32_t)&__heap_end - (uint32_t)&__heap_start;

    osiMemPool_t *ram_heap = osiBlockPoolInit(heap_start, heap_size, 0);
    osiPoolSetDefault(ram_heap);

    OSI_LOGI(0, "ram heap start/%p size/%d pool/%p", heap_start, heap_size, ram_heap);
}

/**
 * Cold boot, called from \p Reset_Handler.
 */
OSI_NO_RETURN void osiBootStart(uint32_t param)
{
    // I cache is enabled, and D cache is disabled
    OSI_LOAD_SECTION(sramboottext);
    OSI_LOAD_SECTION(sramtext);
    OSI_LOAD_SECTION(sramdata);
    OSI_CLEAR_SECTION(srambss);

    // sync cache after code copy
    L1C_InvalidateICacheAll();

    halClockInit();
    halRamInit();
    prvMmuInit();

    OSI_LOAD_SECTION(ramtext);
    OSI_LOAD_SECTION(data);
    OSI_CLEAR_SECTION(bss);

    // sync cache after code copy
    L1C_CleanDCacheAll();
    __DSB();
    __ISB();

    __FPU_Enable();
    AP_WAKEUP_JUMP_MAGIC_REG = 0;
    _impure_ptr = _GLOBAL_REENT;

    osiTraceBufInit();
    osiProfileInit();
    osiPmInit();
    prvHeapInit();

    halSysWdtStop();
    halSysWdtConfig(CONFIG_SYS_WDT_TIMEOUT);

    halIomuxInit();
    halAdiBusInit();
    halEfuseOpen();
    halEfuseClose();

    halPmuInit();
    halPmuExtFlashPowerOn();
    osiKernelStart();
}

/**
 * PM1 wakeup, it is called from \p osiWakePm1Entry. Clock settings
 * are kept in PM1.
 */
OSI_NO_RETURN void osiWakePm1(void)
{
    // now: I cache is enabled, D cache is disabled, only function
    //      on flash can be executed.

    halRamWakeInit();
    prvMmuWakeInit();
    __FPU_Enable();

    prvSramRestore();

    // sync cache after code copy
    L1C_CleanDCacheAll();
    __DSB();
    __ISB();

    prvRestoreContext(gCpuSuspendCtx.reg_sys);

    OSI_DEAD_LOOP; // shouldn't come here
}

/**
 * PM1 wakeup, it is called from \p osiWakePm2Entry. Clock settings
 * are lost in PM2.
 */
OSI_NO_RETURN void osiWakePm2(void)
{
    // now: I cache is enabled, D cache is disabled
    OSI_LOAD_SECTION(sramboottext);

    // sync cache after code copy
    L1C_InvalidateICacheAll();

    halClockInit();
    halRamWakeInit();
    prvMmuWakeInit();
    __FPU_Enable();

    prvSramRestore();

    // sync cache after code copy
    L1C_CleanDCacheAll();
    __DSB();
    __ISB();

    prvRestoreContext(gCpuSuspendCtx.reg_sys);

    OSI_DEAD_LOOP; // shouldn't come here
}

/**
 * Get wake source, and clear wake source
 */
static uint32_t prvGetClearWakeSource(bool aborted)
{
    REG_CP_IDLE_IDL_AWK_ST_T awk_st = {hwp_idle->idl_awk_st};
    REG_CP_IDLE_IDL_AWK_ST_T awk_st_clr = {
        .b.awk0_awk_stat = 1, // pmic
        .b.awk1_awk_stat = 0, // vad_int
        .b.awk2_awk_stat = 1, // key
        .b.awk3_awk_stat = 1, // gpio1
        .b.awk4_awk_stat = 1, // uart1
        .b.awk5_awk_stat = 1, // pad uart1_rxd
        .b.awk6_awk_stat = 1, // wcn2sys
        .b.awk7_awk_stat = 1, // pad wcn_osc_en
        .b.awk_osw2_stat = 1};
    hwp_idle->idl_awk_st = awk_st_clr.v;

    uint32_t source = 0;
    if (awk_st.b.awk0_awk_stat)
        source |= HAL_RESUME_SRC_PMIC;
    if (awk_st.b.awk1_awk_stat)
        source |= HAL_RESUME_SRC_VAD;
    if (awk_st.b.awk2_awk_stat)
        source |= HAL_RESUME_SRC_KEY;
    if (awk_st.b.awk3_awk_stat)
        source |= HAL_RESUME_SRC_GPIO1;
    if (awk_st.b.awk4_awk_stat)
        source |= HAL_RESUME_SRC_UART1;
    if (awk_st.b.awk5_awk_stat)
        source |= HAL_RESUME_SRC_UART1_RXD;
    if (awk_st.b.awk6_awk_stat)
        source |= HAL_RESUME_SRC_WCN2SYS;
    if (awk_st.b.awk7_awk_stat)
        source |= HAL_RESUME_SRC_WCN_OSC;
    if (awk_st.b.awk_osw1_stat)
        source |= HAL_RESUME_SRC_IDLE_TIMER1;
    if (awk_st.b.awk_osw2_stat)
        source |= HAL_RESUME_SRC_IDLE_TIMER2;
    if (awk_st.b.awk_self_stat)
        source |= HAL_RESUME_SRC_SELF;
    return source;
}

/**
 * Suspend after stack is switched to SRAM
 */
static void prvSuspendPost(void)
{
    osiSuspendMode_t mode = gCpuSuspendCtx.mode;

    if (mode == OSI_SUSPEND_PM1)
    {
        halPmuEnterPm1();
        AP_WAKEUP_JUMP_ADDR_REG = (uint32_t)osiWakePm1Entry;
    }
    else
    {
        halPmuEnterPm2();
        AP_WAKEUP_JUMP_ADDR_REG = (uint32_t)osiWakePm2Entry;
    }

    prvSramBackup();

    // go to sleep, really
    L1C_CleanInvalidateDCacheAll();
    __DSB();

#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    // When half sleep is enabled, PSRAM can't be accessed from here.
    // CP shouldn't access PSRAM also. It can't work by just disable
    // CP clock.

    unsigned sctlr = __get_SCTLR();
    sctlr &= ~SCTLR_Z_Msk; // disable branch prediction
    sctlr &= ~SCTLR_M_Msk; // disable MMU, due to TTB is on PSRAM
    __set_SCTLR(sctlr);
    __ISB();

    halRamSuspend();
#endif

    hwp_idle->idl_ctrl_sys2 = IDLE_SLEEP_ENABLE_MAGIC;
    AP_WAKEUP_JUMP_MAGIC_REG = AP_WAKEUP_JUMP_MAGIC;

    __DSB();
    __ISB();
    __WFI();

    // Delay a while. There are no clear reason. Not sure whether it will
    // happen: CPU exit WFI, and then system enter PM1.
    osiDelayLoops(50);

    // come here when suspend failed.
    hwp_idle->idl_awk_self = 1;
    OSI_POLL_WAIT(hwp_idle->idl_ctrl_sys2 == 0);

    hwp_idle->idl_awk_self = 0;
    AP_WAKEUP_JUMP_MAGIC_REG = 0;

#if defined(CONFIG_USE_PSRAM) && defined(CONFIG_PSRAM_LP_HALF_SLEEP)
    halRamSuspendAbort();

    sctlr = __get_SCTLR();
    sctlr |= SCTLR_Z_Msk; // enable branch prediction
    sctlr |= SCTLR_M_Msk; // enable MMU
    sctlr |= SCTLR_AFE_Msk;
    sctlr &= ~SCTLR_TRE_Msk;
    __set_SCTLR(sctlr);
    __ISB();
#endif
}

/**
 * Process after suspend fail (must be no-inline), return wake source.
 */
OSI_NO_INLINE static uint32_t prvSuspendFailed(void)
{
    // come here when suspend failed.
    uint32_t source = prvGetClearWakeSource(true);
    source |= OSI_RESUME_ABORT;

    if (gCpuSuspendCtx.mode == OSI_SUSPEND_PM1)
        halPmuAbortPm1();
    else
        halPmuAbortPm2();

    // At suspend fail, it is possible that AON_LP is already power off and
    // power on. And sysmail is needed to be cleared at power on.
    OSI_LOOP_WAIT((hwp_pwrctrl->aon_lp_pwr_stat & 0x3) == 0x3);
    for (uintptr_t address = (uintptr_t)&hwp_mailbox->sysmail0;
         address <= (uintptr_t)&hwp_mailbox->sysmail191; address += 4)
        *(volatile unsigned *)address = 0;

    return source;
}

/**
 * Process after wakeup (must be no-inline), return wake source.
 */
OSI_NO_INLINE static uint32_t prvSuspendResume(void)
{
    // come here from wakeup handler
    __set_CPSR(gCpuSuspendCtx.cpsr_sys);

    hwp_idle->idl_osw2_en = 0; // disable it

    if (gCpuSuspendCtx.mode == OSI_SUSPEND_PM1)
        halPmuExitPm1();
    else
        halPmuExitPm2();

    if (gCpuSuspendCtx.mode == OSI_SUSPEND_PM2)
    {
        hwp_sysCtrl->cfg_misc_cfg = gCpuSuspendCtx.sys_ctrl_cfg_misc_cfg;
    }

    extern uint32_t __svc_stack_start;
    extern uint32_t __svc_stack_end;
    extern uint32_t __irq_stack_start;
    extern uint32_t __irq_stack_end;

    for (uint32_t *p = &__svc_stack_start; p < &__svc_stack_end; ++p)
        *p = 0xA5A5A5A5;

    for (uint32_t *p = &__irq_stack_start; p < &__irq_stack_end; ++p)
        *p = 0xA5A5A5A5;

    halSysWdtConfig(CONFIG_SYS_WDT_TIMEOUT);
    halSysWdtStart();

    // wait cp sleep request again after resume
    ipc_cp_wake_lock();
    return prvGetClearWakeSource(false);
}

/**
 * CPU suspend. It may return either from suspend fail, or from resume.
 */
uint32_t osiPmCpuSuspend(osiSuspendMode_t mode, int64_t sleep_ms)
{
    // Check whether cp is ready for sleep, and acquire the spinlock to
    // prevent cp go forward after WFI by unexpected reasons.
    halHwspinlockAcquireInCritical(HAL_HWSPINLOCK_ID_CPSLEEP);

    gCpuSuspendCtx.mode = mode;
    gCpuSuspendCtx.cpsr_sys = __get_CPSR();
    gCpuSuspendCtx.ttbr0 = __get_TTBR0();
    gCpuSuspendCtx.dacr = __get_DACR();
    gCpuSuspendCtx.sys_ctrl_cfg_misc_cfg = hwp_sysCtrl->cfg_misc_cfg;

    REG_CP_IDLE_IDL_OSW2_EN_T idl_osw2_en = {0};
    if (sleep_ms != INT64_MAX)
    {
        int64_t ticks = OSI_MS_TO_TICK16K(sleep_ms);

        idl_osw2_en.b.osw2_en = 1;
        idl_osw2_en.b.osw2_time = (ticks >= 0x7fffffff) ? 0x7fffffff : ticks;
    }
    hwp_idle->idl_osw2_en = idl_osw2_en.v;

    bool failed = prvInvokeSuspendPost(gCpuSuspendCtx.reg_sys, prvSuspendPost);

    // We come here either from suspend fail, or from wakeup. To avoid
    // unwanted compiler optimization, no-inline functions are used.
    OSI_BARRIER();
    unsigned source = failed ? prvSuspendFailed() : prvSuspendResume();

    // Release spinlock, and cp can go forward after WFI.
    halHwspinlockReleaseInCritical(HAL_HWSPINLOCK_ID_CPSLEEP);
    return source;
}

uint64_t osiCpDeepSleepTime()
{
    int64_t sleep_ms;
    int64_t tick;

    REG_CP_IDLE_IDL_OSW1_EN_T idl_osw1_en = {hwp_idle->idl_osw1_en};
    tick = idl_osw1_en.b.osw1_time;
    sleep_ms = OSI_TICK16K_TO_MS_U32(tick);
    return sleep_ms;
}
