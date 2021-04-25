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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "osi_api.h"
#include "osi_log.h"
#include "osi_profile.h"
#include "osi_chip.h"
#include "osi_internal.h"
#include "osi_api_inside.h"
#include "drv_names.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "hal_shmem_region.h"
#include "hal_spi_flash.h"
#include "hal_hwspinlock.h"
#include "vfs.h"
#include <string.h>
#include "drv_usb.h"
#include "drv_axidma.h"
#include "connectivity_config.h"

#include "quec_cust_patch.h"
#include "quec_proj_config.h"
#include "ql_gpio.h"
#include "quec_pin_index.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PSM
#include "quec_power_mg.h"
#endif


// #define SLEEP_DEBUG(n) *(volatile unsigned *)0x802ffc = (n)

#define IDLE_SLEEP_ENABLE_MAGIC 0x49444c45

#define SLEEP32K_CP_CHECK_REG (hwp_sysCtrl->cfg_reserve11)
#define SLEEP32K_CP_WAIT_FLAG (0xec)
#define SLEEP32K_CP_GO_FLAG (0xce)

#define SUSPEND_CP_READY_REG (hwp_sysCtrl->cfg_reserve9)
#define SUSPEND_CP_READY_FLAG (0xda)

#define PSM_DATA_BUF_SIZE (12 * 1024)
#define PSM_CONTEXT_FNAME CONFIG_FS_AP_NVM_DIR "/psm_osi.nv"

#define MIN_TICK32K_TO_CP_WAKE OSI_MS_TO_TICK32K_U32(10) // 10 ms in 32K tick
#define USB_MON_DET_COUNT (3)                            // 3 times for USB resume
#define SLEEP32K_POLL_PLL_START (9)                      // 16K tick, 610us

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_SUSPEND
#if ((CONFIG_SLEEP32K_GPIO != QUEC_GPIO_DNAME_GPIO_7) && (CONFIG_SLEEP32K_GPIO != QUEC_GPIO_DNAME_GPIO_8)) //adjust by kevin.wang, 20210106 add gpio as usb suspend pin 
#error "CONFIG_SLEEP32K_GPIO should be defined as GPIO_7 or GPIO_8." 
#endif
#endif

typedef struct
{
    int64_t psm_set_uptime;  // up time of psm enabled point
    int64_t psm_wake_uptime; // psm expiration in up time
    int64_t uptick;
    uint32_t ref32k;
} osiChipContext_t;

typedef struct
{
    uint8_t owner;
    uint16_t size;
} osiPsmDataHeader_t;

typedef struct
{
    int64_t psm_set_uptime; // up time of psm enabled point
    int64_t uptime;         // uptime at the point of sleep
    int64_t epoch_delta;    // epoch_time - uptime
    int64_t local_delta;    // local_time - uptime
} osiPsmKernelData_t;

typedef struct
{
    uint8_t *buf;      // PSM data buffer
    unsigned buf_size; // PSM data buffer size
    unsigned buf_pos;  // valid PSM data position
} osiPsmContext_t;

static osiChipContext_t gOsiChipCtx;
static osiPsmContext_t gOsiPsmCtx;
static int (*gBtSleepChecker)(void) = NULL;

void osiChipIdleISR(void *param)
{
    REG_CP_IDLE_AP_INT_STA_T ap_int_sta = {hwp_idle->ap_int_sta};
    hwp_idle->ap_int_sta = ap_int_sta.v; // clear IRQ
}

void osiChipPmReorder(void)
{
    osiPmResumeFirst(DRV_NAME_DEBUGUART);
}

static void prvSetIdleTimerForSleep(int64_t sleep_ms)
{
    REG_CP_IDLE_IDL_OSW2_EN_T idl_osw2_en = {0};
    if (sleep_ms != INT64_MAX)
    {
        int64_t ticks = OSI_MS_TO_TICK16K(sleep_ms);
        idl_osw2_en.b.osw2_en = 1;
        idl_osw2_en.b.osw2_time = (ticks >= 0x7fffffff) ? 0x7fffffff : ticks;
    }
    hwp_idle->idl_osw2_en = idl_osw2_en.v;
}

static void prvSaveTickRef32K(void)
{
    gOsiChipCtx.ref32k = hwp_idle->idl_32k_ref;
    gOsiChipCtx.uptick = osiUpHWTick();
}

static void prvRestoreTickRef32K(void)
{
    uint32_t sleep_32k = hwp_idle->idl_32k_ref - gOsiChipCtx.ref32k;
    osiSetUpHWTick(gOsiChipCtx.uptick + OSI_32K_TO_HWTICK(sleep_32k));
}

void osiChipSuspend(osiSuspendMode_t mode)
{
    prvSaveTickRef32K();
    osiIrqSuspend(mode);
}

void osiChipResume(osiSuspendMode_t mode, uint32_t source)
{
    bool aborted = (source & OSI_RESUME_ABORT);
    osiIrqResume(mode, aborted);

    if (aborted)
        return;

    for (uintptr_t address = (uintptr_t)&hwp_mailbox->sysmail0;
         address <= (uintptr_t)&hwp_mailbox->sysmail191; address += 4)
        *(volatile unsigned *)address = 0;

    drvAxidmaResume(mode, source);

    if (hwp_sysCtrl->cpcpu_rst_clr != 0)
        osiPanic();

    hwp_sysCtrl->cpcpu_rst_clr = 0xF; // all '1'

    REG_TIMER_AP_TIMER_IRQ_MASK_SET_T timer_irq_mask_set;
    REG_FIELD_WRITE1(hwp_timer4->timer_irq_mask_set, timer_irq_mask_set, ostimer_mask, 1);

    prvRestoreTickRef32K();

    // wait CP is ready
    OSI_LOOP_WAIT(SUSPEND_CP_READY_REG == SUSPEND_CP_READY_FLAG);
}

void osiChipLightSleepEnter(void)
{
    halSysWdtStop();

    if (osiIsSlowSysClkAllowed())
    {
        REG_CP_IDLE_SLOW_SYS_CLK_SEL_HWEN_T clk_sel_hwen = {hwp_idle->slow_sys_clk_sel_hwen};
        clk_sel_hwen.b.hwen = 0;
        hwp_idle->slow_sys_clk_sel_hwen = clk_sel_hwen.v;
    }
}

void osiChipLightSleepExit(void)
{
    REG_CP_IDLE_SLOW_SYS_CLK_SEL_HWEN_T clk_sel_hwen = {hwp_idle->slow_sys_clk_sel_hwen};
    if (clk_sel_hwen.b.hwen == 0)
    {
        clk_sel_hwen.b.hwen = 1;
        hwp_idle->slow_sys_clk_sel_hwen = clk_sel_hwen.v;
    }

    halSysWdtStart();
}

void osiChipSetBtSleepChecker(int (*checker)(void))
{
    gBtSleepChecker = checker;
}

bool osiChipSuspendPermitted(void)
{
    if ((gBtSleepChecker != NULL) && !gBtSleepChecker())
        return false;

    REG_CP_IDLE_IDL_CTRL_TIMER_T idl_ctrl_timer = {hwp_idle->idl_ctrl_timer};
    if (hwp_idle->idl_ctrl_sys1 && idl_ctrl_timer.b.idct_ctrl_timer)
    {
        uint32_t curr_tick = hwp_idle->idl_32k_ref;
        uint32_t wake_tick = hwp_idle->idl_m_timer;
        int sleep_tick = wake_tick - curr_tick;
        if (sleep_tick > MIN_TICK32K_TO_CP_WAKE)
            return true;
    }

    return false;
}

void osiSetPsmWakeUpTime(int64_t uptime)
{
    gOsiChipCtx.psm_set_uptime = osiUpTime();
    gOsiChipCtx.psm_wake_uptime = uptime;

    OSI_LOGD(0, "psm: set wake time %u at %u", (unsigned)uptime,
             (unsigned)gOsiChipCtx.psm_set_uptime);
}

void osiSetPsmSleepTime(int64_t ms)
{
    gOsiChipCtx.psm_set_uptime = osiUpTime();
    if (ms == INT64_MAX)
        gOsiChipCtx.psm_wake_uptime = INT64_MAX;
    else
        gOsiChipCtx.psm_wake_uptime = gOsiChipCtx.psm_set_uptime + ms;

    OSI_LOGD(0, "psm: set sleep time %u at %u", (unsigned)ms,
             (unsigned)gOsiChipCtx.psm_set_uptime);
}

int64_t osiGetPsmWakeUpTime(void)
{
    if (gOsiChipCtx.psm_wake_uptime == 0)
        gOsiChipCtx.psm_wake_uptime = INT64_MAX;
    return gOsiChipCtx.psm_wake_uptime;
}

int64_t osiGetPsmElapsedTime(void)
{
    return osiUpTime() - gOsiChipCtx.psm_set_uptime;
}

static osiPsmDataHeader_t *_findPsmData(osiPsmDataOwner_t owner)
{
    if (gOsiPsmCtx.buf == NULL)
        return NULL;

    unsigned pos = 0;
    while (pos < gOsiPsmCtx.buf_pos)
    {
        osiPsmDataHeader_t *header = (osiPsmDataHeader_t *)(gOsiPsmCtx.buf + pos);
        if (header->owner == owner)
            return header;

        pos += OSI_ALIGN_UP(header->size, 4) + sizeof(osiPsmDataHeader_t);
    }
    return NULL;
}

static void _psmRestoreKernel(void)
{
    osiPsmKernelData_t kdata;
    if (osiPsmDataRestore(OSI_PSMDATA_OWNER_KERNEL, &kdata, sizeof(kdata)) <= 0)
        return;

    gOsiUpTimeOffset += kdata.uptime;
    gOsiEpochOffset = gOsiUpTimeOffset + kdata.epoch_delta;
    gOsiLocalOffset = gOsiUpTimeOffset + kdata.local_delta;
    gOsiChipCtx.psm_set_uptime = kdata.psm_set_uptime;

    OSI_LOGD(0, "kernel psm restore up/%u -> up/%u",
             OSI_HWTICK_TO_MS_U32(kdata.uptime),
             (unsigned)osiUpTime());

    const halShmemRegion_t *psm_shmem = halShmemGetRegion(MEM_PSM_NV_NAME);
    if (psm_shmem == NULL)
        OSI_LOGE(0, "shmem " MEM_PSM_NV_NAME " is not found");
    else
        osiPsmDataRestore(OSI_PSMDATA_OWNER_STACK, (void *)psm_shmem->address, psm_shmem->size);
}

static void _psmSaveKernel(void)
{
    osiPsmKernelData_t kdata;
    kdata.uptime = osiUpHWTick();
    kdata.epoch_delta = gOsiEpochOffset - gOsiUpTimeOffset;
    kdata.local_delta = gOsiLocalOffset - gOsiUpTimeOffset;
    kdata.psm_set_uptime = gOsiChipCtx.psm_set_uptime;

    OSI_LOGD(0, "kernel psm save up/%u", OSI_HWTICK_TO_MS_U32(kdata.uptime));

    halPmuPsmPrepare();
    osiPsmDataSave(OSI_PSMDATA_OWNER_KERNEL, &kdata, sizeof(kdata));

    const halShmemRegion_t *psm_shmem = halShmemGetRegion(MEM_PSM_NV_NAME);
    if (psm_shmem != NULL)
        osiPsmDataSave(OSI_PSMDATA_OWNER_STACK, (void *)psm_shmem->address, psm_shmem->size);
}

void osiPsmSavePrepare(osiShutdownMode_t mode)
{
    osiPsmContext_t *d = (osiPsmContext_t *)&gOsiPsmCtx;
    if (d->buf == NULL)
        d->buf = (uint8_t *)malloc(PSM_DATA_BUF_SIZE);

    d->buf_size = PSM_DATA_BUF_SIZE;
    d->buf_pos = 0;
}

void osiPsmSave(osiShutdownMode_t mode)
{
    osiPsmContext_t *d = (osiPsmContext_t *)&gOsiPsmCtx;

    _psmSaveKernel();

    ssize_t wsize = vfs_file_write(PSM_CONTEXT_FNAME, d->buf, d->buf_pos);
    OSI_LOGD(0, "save PSM data size/%d written/%d", d->buf_pos, wsize);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_PSM
	quec_psm_enter_handler();
#endif
}

void osiPsmRestore(void)
{
    uint32_t boot_causes = osiGetBootCauses();
    if (!(boot_causes & OSI_BOOTCAUSE_PSM_WAKEUP))
        return;

    int file_size = vfs_file_size(PSM_CONTEXT_FNAME);
    if (file_size <= 0)
    {
        osiClearBootCause(OSI_BOOTCAUSE_PSM_WAKEUP);
        return;
    }

    osiPsmContext_t *d = (osiPsmContext_t *)&gOsiPsmCtx;
    if (d->buf == NULL)
        d->buf = (uint8_t *)malloc(PSM_DATA_BUF_SIZE);

    d->buf_size = PSM_DATA_BUF_SIZE;
    d->buf_pos = 0;

    if (vfs_file_read(PSM_CONTEXT_FNAME, d->buf, file_size) != file_size)
    {
        OSI_LOGE(0, "read psm data failed");
        osiClearBootCause(OSI_BOOTCAUSE_PSM_WAKEUP);
        return;
    }

    d->buf_pos = file_size;
    _psmRestoreKernel();
}

bool osiPsmDataSave(osiPsmDataOwner_t owner, const void *buf, uint32_t size)
{
    osiPsmContext_t *d = (osiPsmContext_t *)&gOsiPsmCtx;

    OSI_LOGD(0, "save PSM data pos/%d owner/%d buf/%p, size/%d",
             d->buf_pos, owner, buf, size);

    if (buf == NULL && size > 0)
        return false;

    if (_findPsmData(owner) != NULL)
    {
        OSI_LOGE(0, "save PSM data failed, owner/%d already saved", owner);
        return false;
    }

    if (d->buf_pos + size + sizeof(osiPsmDataHeader_t) > d->buf_size)
    {
        OSI_LOGE(0, "save PSM data failed, overflow");
        return false;
    }

    osiPsmDataHeader_t *header = (osiPsmDataHeader_t *)(d->buf + d->buf_pos);
    header->owner = owner;
    header->size = size;

    if (size > 0)
        memcpy(&header[1], buf, size);
    d->buf_pos += OSI_ALIGN_UP(size, 4) + sizeof(osiPsmDataHeader_t);
    return true;
}

int osiPsmDataRestore(osiPsmDataOwner_t owner, void *buf, uint32_t size)
{
    osiPsmDataHeader_t *header = _findPsmData(owner);
    if (header == NULL)
    {
        OSI_LOGE(0, "restore PSM data failed, owner %d not exist", owner);
        return 0;
    }

    if (buf == NULL)
        return header->size;

    if (size < header->size)
    {
        OSI_LOGE(0, "restore PSM data failed owner/%d size/%d is smaller than %d",
                 owner, size, header->size);
        return -1;
    }

    if (header->size > 0)
        memcpy(buf, &header[1], header->size);
    return header->size;
}

OSI_SECTION_SRAM_TEXT static uint32_t prv32KSleepSram(void)
{
    uint32_t source = 0;

    // Check whether cp is ready for sleep, and acquire the spinlock to
    // prevent cp go forward after WFI by unexpected reasons.
    halHwspinlockAcquireInCritical(HAL_HWSPINLOCK_ID_CPSLEEP);

    osiDCacheCleanAll();

    // set usb_det_en
    REGT_FIELD_CHANGE(hwp_analogReg->usb_reg3,
                      REG_ANALOG_REG_USB_REG3_T,
                      usb_det_en, 1);
    hwp_analogReg->usb_reserved |= 0x20;

    // keep power on in deep sleep: AP, AON_LP
    REGT_FIELD_CHANGE(hwp_pwrctrl->pwr_hwen,
                      REG_CP_PWRCTRL_PWR_HWEN_T,
                      ap_pwr_en, 0,
                      aon_lp_pon_en, 0);

    // disable branch prediction
    __set_SCTLR(__get_SCTLR() & (~SCTLR_Z_Msk));
    __ISB();

    // disable MMU, due to TTB is on PSRAM
    __set_SCTLR(__get_SCTLR() & ~1);
    __ISB();

    // force PSRAM into low power mode
    // no PSRAM access from here
    REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_force_lp_mode_lp,
                      REG_SYS_CTRL_CFG_FORCE_LP_MODE_LP_T,
                      cfg_force_lp_psram, 1);

    // force PSRAM into self refresh mode
    REGT_FIELD_CHANGE(hwp_pwrctrl->ddr_slp_req_hwen,
                      REG_CP_PWRCTRL_DDR_SLP_REQ_HWEN_T,
                      ddrslpreq_hwen, 0);
    REGT_FIELD_CHANGE(hwp_pwrctrl->ddr_slp_req_sw,
                      REG_CP_PWRCTRL_DDR_SLP_REQ_SW_T,
                      ddrslpreq, 1);
    REGT_WAIT_FIELD_NEZ(hwp_pwrctrl->ddr_slp_ack,
                        REG_CP_PWRCTRL_DDR_SLP_ACK_T,
                        ddrslpack);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_SUSPEND
    // set GPIO 8, input, high level interrupt, GPIO wake shoule be enabled
#if (CONFIG_SLEEP32K_GPIO == QUEC_GPIO_DNAME_GPIO_8)  //adjust by kevin.wang, 20210106 add gpio as usb suspend pin QUEC_GPIO_DNAME_GPIO_8
    REG_IOMUX_PAD_GPIO_8_CFG_REG_T gpio_8_cfg = {};
    gpio_8_cfg.b.pad_gpio_8_sel = 0;
    hwp_iomux->pad_gpio_8_cfg_reg = gpio_8_cfg.v;
#else
    REG_IOMUX_PAD_GPIO_7_CFG_REG_T gpio_7_cfg = {0};
    gpio_7_cfg.b.pad_gpio_7_sel = 0;
    hwp_iomux->pad_gpio_7_cfg_reg = gpio_7_cfg.v;
#endif
    hwp_gpio1->gpio_oen_set_in = (1 << CONFIG_SLEEP32K_GPIO);
    hwp_gpio1->gpint_mode_set_reg = (1 << CONFIG_SLEEP32K_GPIO);
    hwp_gpio1->gpint_ctrl_r_clr_reg = (1 << CONFIG_SLEEP32K_GPIO);
    hwp_gpio1->gpint_ctrl_f_clr_reg = (1 << CONFIG_SLEEP32K_GPIO);
    hwp_gpio1->gpint_ctrl_r_set_reg = (1 << CONFIG_SLEEP32K_GPIO);
#endif

    // IDLE enter deep sleep
    __DSB();
    __ISB();
    hwp_idle->idl_ctrl_sys2 = IDLE_SLEEP_ENABLE_MAGIC;

    REG_ANALOG_REG_USB_MON_T usb_mon = {0};
    unsigned tick16k = hwp_timer2->hwtimer_curval;
    for (int count = 0;;)
    {
        // exit polling when there are pending IRQ
        uint32_t isr = __get_ISR();
        if ((isr & (CPSR_I_Msk | CPSR_F_Msk | CPSR_A_Msk)) != 0)
            break;

        // when apll_locked_h is 1, IDLE should be in normal mode
        if ((unsigned)(hwp_timer2->hwtimer_curval - tick16k) > SLEEP32K_POLL_PLL_START)
        {
            REG_SYS_CTRL_SEL_CLOCK_T sel_clock = {hwp_sysCtrl->sel_clock};
            if (sel_clock.b.apll_locked_h)
                break;
        }

        // exit polling when USB resume is detected
        usb_mon.v = hwp_analogReg->usb_mon;
        count = (usb_mon.b.usb_dm_chr == 0 && usb_mon.b.usb_dp_chr == 1) ? count + 1 : 0;
        if (count >= USB_MON_DET_COUNT)
        {
            source = HAL_RESUME_SRC_USB_MON;
            break;
        }
    }

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_SUSPEND
#if (CONFIG_SLEEP32K_GPIO == QUEC_GPIO_DNAME_GPIO_8)  //adjust by kevin.wang, 20210106 add gpio as usb suspend pin 
	// force GPIO 8 output high
	gpio_8_cfg.b.pad_gpio_8_oen_frc = 1;
	gpio_8_cfg.b.pad_gpio_8_out_frc = 1;
	gpio_8_cfg.b.pad_gpio_8_out_reg = 1;
	hwp_iomux->pad_gpio_8_cfg_reg = gpio_8_cfg.v;
#else
    // force GPIO 7 output high
    gpio_7_cfg.b.pad_gpio_7_oen_frc = 1;
    gpio_7_cfg.b.pad_gpio_7_out_frc = 1;
    gpio_7_cfg.b.pad_gpio_7_out_reg = 1;
    hwp_iomux->pad_gpio_7_cfg_reg = gpio_7_cfg.v;
#endif
#endif

    // wait IDLE status
    hwp_idle->idl_awk_self = 1;
    REGT_WAIT_FIELD_EQZ(hwp_idle->idl_sta, REG_CP_IDLE_IDL_STA_T, idle_sys_stat);
    hwp_idle->idl_awk_self = 0;

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_SUSPEND
#if (CONFIG_SLEEP32K_GPIO == QUEC_GPIO_DNAME_GPIO_8)  //adjust by kevin.wang, 20210106 add gpio as usb suspend pin 
    gpio_8_cfg.b.pad_gpio_8_oen_frc = 0;
    gpio_8_cfg.b.pad_gpio_8_out_frc = 0;
    gpio_8_cfg.b.pad_gpio_8_out_reg = 0;
	hwp_iomux->pad_gpio_8_cfg_reg = gpio_8_cfg.v;
#else
    // release GPIO 7 force
    gpio_7_cfg.b.pad_gpio_7_oen_frc = 0;
    gpio_7_cfg.b.pad_gpio_7_out_frc = 0;
    gpio_7_cfg.b.pad_gpio_7_out_reg = 0;
    hwp_iomux->pad_gpio_7_cfg_reg = gpio_7_cfg.v;
#endif

    hwp_gpio1->gpint_ctrl_r_clr_reg = (1 << CONFIG_SLEEP32K_GPIO);
    hwp_gpio1->gpint_ctrl_f_clr_reg = (1 << CONFIG_SLEEP32K_GPIO);
#endif

    // disable usb_det_en
    REGT_FIELD_CHANGE(hwp_analogReg->usb_reg3,
                      REG_ANALOG_REG_USB_REG3_T,
                      usb_det_en, 0);
    hwp_analogReg->usb_reserved &= ~0x20;

    // PSRAM exit self refresh
    REGT_FIELD_CHANGE(hwp_pwrctrl->ddr_slp_req_sw,
                      REG_CP_PWRCTRL_DDR_SLP_REQ_SW_T,
                      ddrslpreq, 0);
    REGT_FIELD_CHANGE(hwp_pwrctrl->ddr_slp_req_hwen,
                      REG_CP_PWRCTRL_DDR_SLP_REQ_HWEN_T,
                      ddrslpreq_hwen, 1);

    // release PSRAM low power mode
    // PSRAM access is allowed after it
    REGT_FIELD_CHANGE(hwp_sysCtrl->cfg_force_lp_mode_lp,
                      REG_SYS_CTRL_CFG_FORCE_LP_MODE_LP_T,
                      cfg_force_lp_psram, 0);
    osiDelayUS(10); // fast clock should be ready now

    // enable MMU
    __set_SCTLR((__get_SCTLR() & ~(1 << 28) & ~(1 << 1)) | 1 | (1 << 29));
    __ISB();

    // clear wake state, cared by AP
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

    // Release spinlock, and cp can go forward after WFI.
    halHwspinlockReleaseInCritical(HAL_HWSPINLOCK_ID_CPSLEEP);

    // enable branch prediction
    __set_SCTLR(__get_SCTLR() | SCTLR_Z_Msk);
    __ISB();

    REGT_FIELD_CHANGE(hwp_pwrctrl->pwr_hwen,
                      REG_CP_PWRCTRL_PWR_HWEN_T,
                      ap_pwr_en, 1,
                      aon_lp_pon_en, 1);

    return source;
}

uint32_t osiChip32KSleep(int64_t sleep_ms)
{
    osiChipTimerDisable();

    prvSetIdleTimerForSleep(sleep_ms);
    prvSaveTickRef32K();

    extern char __svc_stack_end[];
    uint32_t source = osiCallWithStack(__svc_stack_end, prv32KSleepSram, 0, 0);

    prvRestoreTickRef32K();
    osiTimerWakeupProcess();

    return source;
}

