#include "drv_wcn.h"
#include "drv_wcn_ipc.h"
#include "hwregs.h"
#include "hal_chip.h"
#include "osi_profile.h"
#include "stdlib.h"
#include "ddb.h"
#include "drv_md_ipc.h"
#include "bt_drv.h"
#include "bt_app_config.h"

#define _NV_LOAD_
#define PSKEY_NV_SIZE 268

static uint32_t s_bt_sleep_time = 0;
static uint32_t s_bt_sleep_time_ap32k = 0;
static drvWcn_t *gBtWcn;
uint8_t *bt_code_addr; //bt code start address

#define BT_CODE_START_ADDR (CONFIG_RAM_PHY_ADDRESS + CONFIG_APP_BTFW_RAM_OFFSET)

extern const unsigned int psram_midi[];
extern unsigned psram_midi_size[];
static void prvBTLoad(uint8 *boot_addr_ptr);

unsigned int BT_GetPSRAMCode(unsigned int **p_bt_ram_code_ptr)
{
    *p_bt_ram_code_ptr = (unsigned int *)psram_midi;
    return (unsigned)psram_midi_size;
}

unsigned int BT_GetIRAMCode(unsigned char **p_bt_ram_code_ptr)
{
    return 0;
}

static void bt_riscv_assert_handler(void)
{
//    BT_riscv_assert_reset();
}

void bt_disable_deep_sleep(void)
{
    //DISABLE_DEEP_SLEEP
    drvWcnRequest(gBtWcn);
}

/*****************************************************************************/
//  Description:    This function is used by RISC-V to notify AP sleep
//  Author:
//  Note:
/*****************************************************************************/
static void bt_sleep_callback(uint32_t sleeptime)
{
    osiProfileEnter(PROFCODE_WCN_SLEEP);

    s_bt_sleep_time = sleeptime;
    s_bt_sleep_time_ap32k = hwp_idle->idl_32k_ref;

    OSI_LOGI(0, "bt sleep request at 0x%x, period 0x%x", s_bt_sleep_time_ap32k, s_bt_sleep_time);

    //ENABLE_DEEP_SLEEP
    drvWcnRelease(gBtWcn);
}

static uint32 prvBTGetPsramCode(uint32 **p_bt_ram_code_ptr)
{
    return BT_GetPSRAMCode((unsigned int **)p_bt_ram_code_ptr);
}

//here we use function pointer to add another way for npi test mode
void (*pprvBTLoad)(uint8 *addr) = prvBTLoad;

static void prvBTLoad(uint8 *boot_addr_ptr)
{
#ifdef _NV_LOAD_
    uint32 i;
    bt_pskey_nv_info_t bt_sprd_nv_param;
    BT_ADDRESS bd_addr;
    uint32 ECO_8910_chip_flag = 0;
    uint16 bt_log_set = 0;
#endif

    uint32 *bt_ram_code_ptr;
    uint32 bt_ram_code_size = 0;
    //Copy the BT controller bin into RAM.
    bt_ram_code_size = prvBTGetPsramCode(&bt_ram_code_ptr);

    SCI_TRACE_LOW("btdrv: Loading bt bin addr=0x%x, size=%d", bt_code_addr, bt_ram_code_size);

    bt_code_addr[0] = 0xFF;
    bt_code_addr[1] = 0xFF;
    bt_code_addr[2] = 0xFF;
    bt_code_addr[3] = 0xFF;

    //just for compile
    memcpy((uint8 *)(bt_code_addr + 4), (const void *)bt_ram_code_ptr, bt_ram_code_size);

    osiDCacheClean((uint8 *)(bt_code_addr), bt_ram_code_size);

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    drvWcnIpcInit();
#endif

#ifdef _NV_LOAD_
    /* set SSP debug mode */
//    if (FALSE != BT_GetSSPDebugMode())
    {
        bt_sprd_nv_param.feature_set[6] = 0;
        bt_sprd_nv_param.feature_set[6] &= ~0x08;
    }

    if (NVERR_None != EFS_NvitemRead(NV_BT_SPRD, sizeof(bt_sprd_nv_param), (uint8 *)&bt_sprd_nv_param))
    {
        SCI_TRACE_LOW("btdrv: [BT] Read NV failed");
        return;
    }

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    bt_sprd_nv_param.g_sys_sco_transmit_mode = 1;
#endif

    uint8_t *p;
    uint8_t p_buf[PSKEY_NV_SIZE + 4 + 4] = {0}; //PSKEY_SIZE is NV BT_SPRD size, 4 is uint16 bt_log_set and uint16 bt_calibration_flag
    p = p_buf;

    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_dbg_source_sink_syn_test_data);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_in_standby_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_master_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_slave_supported);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.default_ahb_clk);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.device_class);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.win_ext);

    for (i = 0; i < 6; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aGainValue[i]);
    }

    for (i = 0; i < 5; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aPowerValue[i]);
    }

    for (i = 0; i < 16; i++)
    {
        UINT8_TO_STREAM(p, bt_sprd_nv_param.feature_set[i]);
    }

    //get addr of NV
    DB_GetLocalPublicAddr(&bd_addr);
    
    for (i = 0; i < 6; i++)
    {
        UINT8_TO_STREAM(p, bd_addr.addr[i]);
    }

    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sco_transmit_mode);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_uart0_communication_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.edr_tx_edr_delay);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.edr_rx_edr_delay);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.g_wbs_nv_117);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.is_wdg_supported);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_rx_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_packet_num_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_data_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.g_PrintLevel);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_block_length);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_rx_block_length);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_water_mark);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_timeout_value);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.uart_rx_watermark);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.uart_flow_control_thld);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.comp_id);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.pcm_clk_divd);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.g_nbs_nv_118);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.br_edr_diff_reserved);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.br_edr_powercontrol_flag);

    for (i = 0; i < 8; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.gain_br_channel_power[i]);
    }

    for (i = 0; i < 8; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.gain_edr_channel_power[i]);
    }

    UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aLEPowerControlFlag);
    for (i = 0; i < 8; i++)
    {
        UINT16_TO_STREAM(p, bt_sprd_nv_param.g_aLEChannelpwrvalue[i]);
    }

    for (i = 0; i < 2; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.host_reserved[i]);
    }

    UINT32_TO_STREAM(p, bt_sprd_nv_param.config0);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config1);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config2);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config3);

    //set calibration flag
    UINT16_TO_STREAM(p, 0);

    //The bt_log_set is NOT in NV_BT_SPRD. But it is in the Host and Controller PSK structure.
    //0 -- no log;
    // 1 -- debug host (coolwatcher);
    // 2 -- AP output log;
    bt_log_set = 2;

    UINT16_TO_STREAM(p, bt_log_set);

    bt_sprd_nv_param.reserved1[0] = 0;
    bt_sprd_nv_param.reserved1[1] = 0;
    for (i = 0; i < 4; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.reserved1[i]);
    }

    //The ECO_8910_chip_flag is NOT in NV_BT_SPRD. But it is in the Host and Controller PSK structure.
    ECO_8910_chip_flag = hwp_rfReg->chip_id_reg2;
    if (1 == ECO_8910_chip_flag) //  1 means old 8910 chip
    {
        ECO_8910_chip_flag = 0;
    }
    else // 2 means ECO 8910 chip
    {
        ECO_8910_chip_flag = 1;
    }
    UINT32_TO_STREAM(p, ECO_8910_chip_flag);

    for (i = 0; i < 3; i++)
    {
        UINT16_TO_STREAM(p, bt_sprd_nv_param.reserved2[i]);
    }

    memcpy((uint8 *)(bt_code_addr + 0x200), (uint8 *)p_buf, (PSKEY_NV_SIZE + 4 + 4));

    osiDCacheClean((uint8 *)(bt_code_addr + 0x200), (PSKEY_NV_SIZE + 4 + 4));
#endif
}

//prvBTLoad for npi test mode to avoid controller sleep
static void npi_prvBTLoad(uint8 *boot_addr_ptr)
{
#ifdef _NV_LOAD_
    uint32 i;

    bt_pskey_nv_info_t bt_sprd_nv_param;
    bt_nv_param_t bt_nv_param;
    uint32 ECO_8910_chip_flag = 0;
    uint16 bt_log_set = 0;
#endif

    uint32 *bt_ram_code_ptr;
    uint32 bt_ram_code_size = 0;
    //Copy the BT controller bin into RAM.
    bt_ram_code_size = prvBTGetPsramCode(&bt_ram_code_ptr);

    SCI_TRACE_LOW("btdrv: Loading bt bin addr=0x%x, size=%d", bt_code_addr, bt_ram_code_size);

    bt_code_addr[0] = 0xFF;
    bt_code_addr[1] = 0xFF;
    bt_code_addr[2] = 0xFF;
    bt_code_addr[3] = 0xFF;

    //just for compile
    memcpy((uint8 *)(bt_code_addr + 4), (const void *)bt_ram_code_ptr, bt_ram_code_size);

    osiDCacheClean((uint8 *)(bt_code_addr), bt_ram_code_size);

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    drvWcnIpcInit();
#endif

#ifdef _NV_LOAD_
    /* set SSP debug mode */
//    if (FALSE != BT_GetSSPDebugMode())
    {
        bt_sprd_nv_param.feature_set[6] = 0;
        bt_sprd_nv_param.feature_set[6] &= ~0x08;
    }

    if (NVERR_None != EFS_NvitemRead(NV_BT_CONFIG, sizeof(bt_nv_param), (uint8 *)&bt_nv_param))
    {
        SCI_TRACE_LOW("btdrv: [BT] Read NV failed");
        return;
    }
    if (NVERR_None != EFS_NvitemRead(NV_BT_SPRD, sizeof(bt_sprd_nv_param), (uint8 *)&bt_sprd_nv_param))
    {
        SCI_TRACE_LOW("btdrv: [BT] Read NV failed");
        return;
    }

#ifdef CONFIG_BT_COMM_USE_WCN_IPC
    bt_sprd_nv_param.g_sys_sco_transmit_mode = 1;
#endif

    uint8_t *p;
    uint8_t p_buf[PSKEY_NV_SIZE + 4 + 4] = {0}; //PSKEY_SIZE is NV BT_SPRD size, 4 is uint16 bt_log_set and uint16 bt_calibration_flag
    p = p_buf;

    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_dbg_source_sink_syn_test_data);

    //Controller should not SLEEP in the test mode
    bt_sprd_nv_param.g_sys_sleep_in_standby_supported = 0;
    bt_sprd_nv_param.g_sys_sleep_master_supported = 0;
    bt_sprd_nv_param.g_sys_sleep_slave_supported = 0;

    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_in_standby_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_master_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sleep_slave_supported);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.default_ahb_clk);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.device_class);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.win_ext);

    for (i = 0; i < 6; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aGainValue[i]);
    }

    for (i = 0; i < 5; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aPowerValue[i]);
    }

    for (i = 0; i < 16; i++)
    {
        UINT8_TO_STREAM(p, bt_sprd_nv_param.feature_set[i]);
    }

    for (i = 0; i < 6; i++)
    {
        UINT8_TO_STREAM(p, bt_nv_param.bd_addr.addr[i]);
    }

    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_sco_transmit_mode);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.g_sys_uart0_communication_supported);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.edr_tx_edr_delay);
    UINT8_TO_STREAM(p, bt_sprd_nv_param.edr_rx_edr_delay);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.g_wbs_nv_117);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.is_wdg_supported);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_rx_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_packet_num_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_data_base_addr);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.g_PrintLevel);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_block_length);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_rx_block_length);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_water_mark);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.share_memo_tx_timeout_value);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.uart_rx_watermark);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.uart_flow_control_thld);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.comp_id);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.pcm_clk_divd);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.g_nbs_nv_118);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.br_edr_diff_reserved);
    UINT16_TO_STREAM(p, bt_sprd_nv_param.br_edr_powercontrol_flag);

    for (i = 0; i < 8; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.gain_br_channel_power[i]);
    }

    for (i = 0; i < 8; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.gain_edr_channel_power[i]);
    }

    UINT32_TO_STREAM(p, bt_sprd_nv_param.g_aLEPowerControlFlag);
    for (i = 0; i < 8; i++)
    {
        UINT16_TO_STREAM(p, bt_sprd_nv_param.g_aLEChannelpwrvalue[i]);
    }

    for (i = 0; i < 2; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.host_reserved[i]);
    }

    UINT32_TO_STREAM(p, bt_sprd_nv_param.config0);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config1);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config2);
    UINT32_TO_STREAM(p, bt_sprd_nv_param.config3);

    //set calibration flag
    UINT16_TO_STREAM(p, 0);

    //The bt_log_set is NOT in NV_BT_SPRD. But it is in the Host and Controller PSK structure.
    //0 -- no log;
    // 1 -- debug host (coolwatcher);
    // 2 -- AP output log;
    bt_log_set = 2;

    UINT16_TO_STREAM(p, bt_log_set);

    bt_sprd_nv_param.reserved1[0] = 0;
    bt_sprd_nv_param.reserved1[1] = 0;
    for (i = 0; i < 4; i++)
    {
        UINT32_TO_STREAM(p, bt_sprd_nv_param.reserved1[i]);
    }

    //The ECO_8910_chip_flag is NOT in NV_BT_SPRD. But it is in the Host and Controller PSK structure.
    ECO_8910_chip_flag = hwp_rfReg->chip_id_reg2;
    if (1 == ECO_8910_chip_flag) //  1 means old 8910 chip
    {
        ECO_8910_chip_flag = 0;
    }
    else // 2 means ECO 8910 chip
    {
        ECO_8910_chip_flag = 1;
    }
    UINT32_TO_STREAM(p, ECO_8910_chip_flag);

    for (i = 0; i < 3; i++)
    {
        UINT16_TO_STREAM(p, bt_sprd_nv_param.reserved2[i]);
    }

    memcpy((uint8 *)(bt_code_addr + 0x200), (uint8 *)p_buf, (PSKEY_NV_SIZE + 4 + 4));

    osiDCacheClean((uint8 *)(bt_code_addr + 0x200), (PSKEY_NV_SIZE + 4 + 4));
#endif
}

void npi_btLoad_register(void)
{
    SCI_TRACE_LOW("enter npi_btLoad_register");
    pprvBTLoad = npi_prvBTLoad;
}

void npi_btLoad_deregister(void)
{
    SCI_TRACE_LOW("enter npi_btLoad_deregister");
    pprvBTLoad = prvBTLoad;
}

static bool prvBTBootup(uint8_t *boot_addr_ptr)
{
    //WCN BT CPU clock frequecny from APPLL(1000MHz) /*3 means 1000MHz/8=125MHz */ /*2 means 1000MHz/12=83MHz*/ /*1 means 1000MHz/16=62MHz*/
    //Accoding to ASIC's suggestion, 8910 WCN clock cannot be more than 100MHz.

    gBtWcn = drvWcnOpen(WCN_USER_BT);

    if(gBtWcn == NULL)
    {
        return false;
    }

    //set WCN RF Power to 1700mv

    halPmuSetPowerLevel(HAL_POWER_WCN, WCN_VDD_1700MV);

    //osiDelayUS(100*1000);(100);//100ms
    osiThreadSleep(100);

    //Sleep-Wakeup Setting.
    //We will use AP<->RISC-V software control Sleep-Wakeup, not use OSC_En now.
    hwp_idle->idl_wcn_en = 0;
    hwp_idle->eliminat_jitter = 0x880;

    //WCN boot up
    hwp_wcnSysCtrl->branch_addr = ((((uint32_t)boot_addr_ptr) & 0x7FFFFFFF) | 0x22000000);

    hwp_wcnSysCtrl->soft_reset = 0x8000ffff;

    osiThreadSleep(310); //250ms for Risc-V init and calibration

    return true;
}

static void prvWCNHandler(void *ctx)
{
    osiProfileExit(PROFCODE_WCN_SLEEP);

    uint32 idl_32k = hwp_idle->idl_32k_ref;
    OSI_LOGI(0, "bt wakeup interrupt, 32k_time=0x%x", idl_32k);

    // Force to give the WCN clock to access the WCN register,
    // bypass hardware clock gate control.
    unsigned reg = hwp_sysCtrl->cfg_misc1_cfg;
    reg &= ~SYS_CTRL_WCN_OSC_EN_CTRL;
    hwp_sysCtrl->cfg_misc1_cfg = reg;

    // clear this interrupt
    reg = hwp_wcnSysCtrl->wakeup_ctrl_1;
    reg |= 1;
    hwp_wcnSysCtrl->wakeup_ctrl_1 = reg;

    // close the force.
    reg = hwp_sysCtrl->cfg_misc1_cfg;
    reg |= SYS_CTRL_WCN_OSC_EN_CTRL;
    hwp_sysCtrl->cfg_misc1_cfg = reg;

    s_bt_sleep_time = 0;

    //DISABLE_DEEP_SLEEP
    drvWcnRequest(gBtWcn);
}

static void prvSetUartBTUse(uint32_t use)
{
    uint32 critical = osiEnterCritical();

#ifdef CONFIG_BT_COMM_USE_INT_UART
    if (use == 1)
    {

        hwp_sysCtrl->cfg_misc_cfg &= ~(SYS_CTRL_WCN_UART_OUT_SEL | SYS_CTRL_AP_UART_OUT_SEL);
    }
    else
    {
        hwp_sysCtrl->cfg_misc_cfg |= (SYS_CTRL_AP_UART_OUT_SEL);
    }
#endif

    osiExitCritical(critical);
}

static void prvEnableBTUart(void)
{
    SCI_TRACE_LOW("btdrv: prvEnableBTUart");
    prvSetUartBTUse(1);

    //  UartDrv_Configure(3250000); //In fact, 8910's BT_BAUD_3250000 is 3000000.
    UartDrv_Configure(3000000); //In fact, 8910's BT_BAUD_3250000 is 3000000.
    UartDrv_Start();
}

static void prvDisableBTUart(void)
{
    prvSetUartBTUse(0);
    UartDrv_Stop();
}

/*****************************************************************************/
//  Description:    This function is used to start up BT chip
//  Author:
//  Note:
/*****************************************************************************/
bool bt_chip_startup(void)
{
    SCI_TRACE_LOW("btdrv: [BT] enter BTI_StartupChip");
    osiDebugEvent(0xb700b007);

    SCI_TRACE_LOW("btdrv: start up the BT chip");

    osiIrqSetPriority(HAL_SYSIRQ_NUM(SYS_IRQ_ID_WCN_AWAKE), SYS_IRQ_PRIO_WCN_AWAKE);
    osiIrqSetHandler(HAL_SYSIRQ_NUM(SYS_IRQ_ID_WCN_AWAKE), prvWCNHandler, (void *)0);
    osiIrqEnable(HAL_SYSIRQ_NUM(SYS_IRQ_ID_WCN_AWAKE)); //

    ipc_register_bt_assert_notify(bt_riscv_assert_handler);
    ipc_register_bt_sleep_notify(bt_sleep_callback);

    bt_code_addr = (uint8_t *)BT_CODE_START_ADDR;

    //Copy RAM, and bootup RISC-V
//    prvBTLoad(bt_code_addr);
    pprvBTLoad(bt_code_addr);
    //power on , init RF , reset risc-v etc.
    if(prvBTBootup(bt_code_addr + 4) == false)
    {
        OSI_LOGW(0, "Warning: WIFI and BT can not coexist!!");
        return false;
    }

    //Shake-hand with RISC-V, if we need.

    prvEnableBTUart();

    SCI_TRACE_LOW("btdrv: [BT] exit BTI_StartupChip");

    return true;
}

void bt_chip_shutdown(void)
{
    SCI_TRACE_LOW("btdrv: [BT] enter BTI_ShutdownChip");
    prvDisableBTUart();

    osiThreadSleep(100);

    drvWcnClose(gBtWcn);

    SCI_TRACE_LOW("btdrv: [BT] exit BTI_ShutdownChip");
}

bool bt_chip_enter_sleep_check(void)
{
    if (s_bt_sleep_time == 0)
        return false;

    if ((hwp_pwrctrl->wcn_lps & CP_PWRCTRL_WCN2SYS_OSC_EN) != 0)
        return false;

    // Not to sleep when less than 6.9ms (230/32768)
    // TODO: It is not clear the calculation method of this threshold
    unsigned sleep_dest = s_bt_sleep_time_ap32k + s_bt_sleep_time;
    int delta = sleep_dest - hwp_idle->idl_32k_ref;
    if (delta < 230)
        return false;

    return true;
}
