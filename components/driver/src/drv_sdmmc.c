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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('S', 'D', 'M', 'C')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "drv_sdmmc.h"
#include "drv_sdmmc_imp.h"
#include "osi_log.h"
#include "hal_chip.h"
#include <stdlib.h>
#include <string.h>
#include "hal_iomux.h"

#include "quec_proj_config.h"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
extern bool quec_sdmmc_init_is_sd(void);
extern void quec_sdmmcSendCmdSetEXT_CSD(REG_SDMMC_SDMMC_CONFIG_T *sdmmc_config);
extern void quec_sdmmcSendCmdSetWR_BLK_COUNT(REG_SDMMC_SDMMC_CONFIG_T *sdmmc_config);
extern void quec_sdmmcSendCmdSetWR_BLK_COUNT_SD(REG_SDMMC_SDMMC_CONFIG_T *sdmmc_config);
extern void quec_sdmmcSetClkSet(drvSdmmc_t *d,uint32_t freq,uint32_t *qdiv);
extern void quec_sdmmcCreateSetPower(void);
extern void quec_sdmmcCreateSet(drvSdmmc_t *d);
extern bool quec_sdmmcCheckResp(drvSdmmc_t *d, sdmmcCmd_t cmd);
extern void quec_sdmmcCreateSetPower(void);
extern void quec_sdmmcCreateSet(drvSdmmc_t *d);
extern void quec_sdmmcCmdArg_Clear(drvSdmmc_t *d);
extern void quec_sdmmcReadAdress(drvSdmmc_t *d, uint32_t nr,unsigned *adress);
extern void quec_sdmmcReadMultiAdress(drvSdmmc_t *d,unsigned rcount,unsigned rsize,unsigned *adress);
extern bool quec_sdmmcWriteBlocksSendCmd_SD(drvSdmmc_t *d,uint32_t count);
extern void quec_sdmmcSetCard_is_sdhc(drvSdmmc_t *d);
extern void quec_sdmmcReadCsd(drvSdmmc_t *d);
extern bool quec_sdmmcOpen_sub(drvSdmmc_t *d);
#endif

/**
 * Add support for CONFIG_QUEC_PROJECT_FEATURE_EMMC 
 * Init SD or EMMC
 * add by sum 2021/02/26
 */
 
/**
 * Send command through controller, sdmmc_config may be different for
 * various commands.
 */
static void prvSdmmcSendCmd(drvSdmmc_t *d, sdmmcCmd_t cmd, uint32_t arg)
{
    REG_SDMMC_SDMMC_CONFIG_T sdmmc_config = {};

    switch (cmd)
    {
    case SDMMC_CMD_ALL_SEND_CID:
    case SDMMC_CMD_SEND_CSD:
        sdmmc_config.b.rsp_sel = 2;
        sdmmc_config.b.rsp_en = 1;
        break;

    case SDMMC_CMD_SEND_RELATIVE_ADDR:
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    case SDMMC_CMD_SEND_IF_COND:
#endif
    case SDMMC_CMD_SELECT_CARD:
    case SDMMC_CMD_SEND_STATUS:
    case SDMMC_CMD_SET_BLOCKLEN:
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        break;

    case SDMMC_CMD_SET_DSR:
    case SDMMC_CMD_GO_IDLE_STATE:
    case SDMMC_CMD_STOP_TRANSMISSION:
        break;

    case SDMMC_CMD_READ_SINGLE_BLOCK:
        sdmmc_config.b.rd_wt_sel = 0;
        sdmmc_config.b.rd_wt_en = 1;
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        break;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC		
    case SEND_EXT_CSD:
        quec_sdmmcSendCmdSetEXT_CSD(&sdmmc_config);
        break;
#endif
    case SDMMC_CMD_READ_MULT_BLOCK:
        sdmmc_config.b.s_m_sel = 1;
        sdmmc_config.b.rd_wt_sel = 0;
        sdmmc_config.b.rd_wt_en = 1;
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        sdmmc_config.b.bit_16 = 1;
        break;

    case SDMMC_CMD_WRITE_SINGLE_BLOCK:
        sdmmc_config.b.rd_wt_sel = 1;
        sdmmc_config.b.rd_wt_en = 1;
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        break;

    case SDMMC_CMD_WRITE_MULT_BLOCK:
        sdmmc_config.b.s_m_sel = 1;
        sdmmc_config.b.rd_wt_sel = 1;
        sdmmc_config.b.rd_wt_en = 1;
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        sdmmc_config.b.bit_16 = 1;
        break;

    case SDMMC_CMD_APP_CMD:
    case SDMMC_CMD_SET_BUS_WIDTH:
    case SDMMC_CMD_SWITCH:
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        break;

    case SDMMC_CMD_SEND_NUM_WR_BLOCKS:
        sdmmc_config.b.s_m_sel = 1;
        sdmmc_config.b.rd_wt_sel = 0;
        sdmmc_config.b.rd_wt_en = 1;
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
        sdmmc_config.b.bit_16 = 1;
        break;

    case SDMMC_CMD_SET_WR_BLK_COUNT:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
        quec_sdmmcSendCmdSetWR_BLK_COUNT(&sdmmc_config);
		break;
    case SDMMC_CMD_SET_WR_BLK_COUNT_SD:
        quec_sdmmcSendCmdSetWR_BLK_COUNT_SD(&sdmmc_config);       	
#else	
        sdmmc_config.b.rsp_sel = 0;
        sdmmc_config.b.rsp_en = 1;
#endif
        break;	

    case SDMMC_CMD_MMC_SEND_OP_COND:
    case SDMMC_CMD_SEND_OP_COND:
        sdmmc_config.b.rsp_sel = 1;
        sdmmc_config.b.rsp_en = 1;
        break;

    default:
        break;
    }
    sdmmc_config.b.sdmmc_sendcmd = 1;

    d->hwp->sdmmc_cmd_index = cmd & 0x3f;
    d->hwp->sdmmc_cmd_arg = arg;
    d->hwp->sdmmc_config = sdmmc_config.v;
    OSI_LOGV(0, "%4c cmd/%d arg/%x config/%x", d->name, cmd & 0x3f, arg, sdmmc_config.v);
}

/**
 * Whether hardware status is OK for the specified condition
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
bool prvSdmmcCheckStatus(drvSdmmc_t *d, sdmmcCheckStatus_t cond)
{
    REG_SDMMC_SDMMC_STATUS_T sdmmc_status = {d->hwp->sdmmc_status};

    switch (cond)
    {
    case SDMMC_CHECK_TRANSFER_DONE:
        return !sdmmc_status.b.not_sdmmc_over;

    case SDMMC_CHECK_RESPONSE_OK:
        return !(sdmmc_status.b.rsp_error || sdmmc_status.b.no_rsp_error);

    case SDMMC_CHECK_CRC_OK:
        return (sdmmc_status.b.crc_status == 0x02);

    case SDMMC_CHECK_DATA_OK:
        return !sdmmc_status.b.data_error;

    default:
        return false;
    }
}

/**
 * Set hardware block size register
 */
static void prvSdmmcSetBlockSizeReg(drvSdmmc_t *d)
{
    d->hwp->sdmmc_block_size_reg = 31 - __builtin_clz(d->block_size);
}

/**
 * Set hardware clock registers
 */
static void prvSdmmcSetClk(drvSdmmc_t *d, uint32_t freq)
{
    if (freq > d->max_clk)
        freq = d->max_clk;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	uint32_t div = 0;
	quec_sdmmcSetClkSet(d,freq,&div);
#else

    uint32_t div = OSI_DIV_ROUND_UP(CONFIG_DEFAULT_SYSAXI_FREQ / 2, freq) - 1;
    div = OSI_MIN(unsigned, div, 0xff);
    OSI_LOGI(0, "%4c clock/%d div/%d", d->name, freq, div);
#endif

    d->hwp->sdmmc_trans_speed_reg = div & 0xff;
    d->hwp->sdmmc_mclk_adjust_reg = (d->clk_adj & 0x0f) | (d->clk_inv << 4);
}

/**
 * Set hardware data width register
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
void prvSdmmcSetDataWidth(drvSdmmc_t *d)
{
    switch (d->bus_width)
    {
    case SDMMC_DATA_BUS_WIDTH_1:
        d->hwp->sdmmc_data_width_reg = 1;
        break;

    case SDMMC_DATA_BUS_WIDTH_4:
        d->hwp->sdmmc_data_width_reg = 4;
        break;

    default:
        break;
    }
}

/**
 * Get response, 1 word or 4 words.
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
void prvSdmmcGetResp(drvSdmmc_t *d, sdmmcCmd_t cmd)
{
    switch (cmd)
    {
    // If they require a response, it is a 32 bit argument.
    case SDMMC_CMD_SEND_RELATIVE_ADDR:
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    case SEND_EXT_CSD:
	case SDMMC_CMD_SET_WR_BLK_COUNT_SD:
#else
    case SDMMC_CMD_SEND_IF_COND:
#endif
    case SDMMC_CMD_SELECT_CARD:
    case SDMMC_CMD_SEND_STATUS:
    case SDMMC_CMD_SET_BLOCKLEN:
    case SDMMC_CMD_READ_SINGLE_BLOCK:
    case SDMMC_CMD_READ_MULT_BLOCK:
    case SDMMC_CMD_WRITE_SINGLE_BLOCK:
    case SDMMC_CMD_WRITE_MULT_BLOCK:
    case SDMMC_CMD_APP_CMD:
    case SDMMC_CMD_SET_BUS_WIDTH:
    case SDMMC_CMD_SEND_NUM_WR_BLOCKS:
    case SDMMC_CMD_SET_WR_BLK_COUNT:
    case SDMMC_CMD_MMC_SEND_OP_COND:
    case SDMMC_CMD_SEND_OP_COND:
    case SDMMC_CMD_SWITCH:
        d->resp.v[0] = d->hwp->sdmmc_resp_arg3;
        d->resp.v[1] = 0;
        d->resp.v[2] = 0;
        d->resp.v[3] = 0;
        break;

    // Those response arguments are 128 bits
    case SDMMC_CMD_ALL_SEND_CID:
    case SDMMC_CMD_SEND_CSD:
        d->resp.v[0] = d->hwp->sdmmc_resp_arg0;
        d->resp.v[1] = d->hwp->sdmmc_resp_arg1;
        d->resp.v[2] = d->hwp->sdmmc_resp_arg2;
        d->resp.v[3] = d->hwp->sdmmc_resp_arg3;
        break;

    default:
        break;
    }
}

/**
 * Start read (rx ifc) transfer
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
void prvSdmmcStartReadXfer(drvSdmmc_t *d, void *buf, unsigned block_num)
{
    d->hwp->sdmmc_block_cnt_reg = block_num & 0xFFFF;

    /* Configure Bytes reordering */
    REG_SDMMC_APBI_CTRL_SDMMC_T apbi_ctrl_sdmmc = {};
    apbi_ctrl_sdmmc.b.soft_rst_l = 1;
    apbi_ctrl_sdmmc.b.l_endian = 1;
    d->hwp->apbi_ctrl_sdmmc = apbi_ctrl_sdmmc.v;

    unsigned xfersize = block_num * d->block_size;

    uint32_t critical = osiEnterCritical();
    drvIfcFlush(&d->rx_ifc);
    osiDCacheInvalidate(buf, xfersize);
    drvIfcStart(&d->rx_ifc, buf, xfersize);
    osiExitCritical(critical);
}

/**
 * Stop read (rx ifc) transfer
 */
static void prvSdmmcStopReadXfer(drvSdmmc_t *d)
{
    /* Configure Bytes reordering */
    REG_SDMMC_APBI_CTRL_SDMMC_T apbi_ctrl_sdmmc = {};
    apbi_ctrl_sdmmc.b.soft_rst_l = 0;
    apbi_ctrl_sdmmc.b.l_endian = 1;
    d->hwp->apbi_ctrl_sdmmc = apbi_ctrl_sdmmc.v;

    drvIfcFlush(&d->rx_ifc);
}

/**
 * Start write (tx ifc) transfer
 */
static void prvSdmmcStartWriteXfer(drvSdmmc_t *d, const void *buf, unsigned block_num)
{
    d->hwp->sdmmc_block_cnt_reg = block_num & 0xFFFF;

    /* Configure Bytes reordering */
    REG_SDMMC_APBI_CTRL_SDMMC_T apbi_ctrl_sdmmc = {};
    apbi_ctrl_sdmmc.b.soft_rst_l = 1;
    apbi_ctrl_sdmmc.b.l_endian = 1;
    d->hwp->apbi_ctrl_sdmmc = apbi_ctrl_sdmmc.v;

    unsigned xfersize = block_num * d->block_size;

    uint32_t critical = osiEnterCritical();
    osiDCacheClean(buf, xfersize);
    drvIfcStart(&d->tx_ifc, buf, xfersize);
    osiExitCritical(critical);
}

/**
 * Stop write (tx ifc) transfer
 */
static void prvSdmmcStopWriteXfer(drvSdmmc_t *d)
{
    /* Configure Bytes reordering */
    REG_SDMMC_APBI_CTRL_SDMMC_T apbi_ctrl_sdmmc = {};
    apbi_ctrl_sdmmc.b.soft_rst_l = 0;
    apbi_ctrl_sdmmc.b.l_endian = 1;
    d->hwp->apbi_ctrl_sdmmc = apbi_ctrl_sdmmc.v;

    OSI_LOOP_WAIT(drvIfcIsFifoEmpty(&d->tx_ifc));
}

/**
 * Wait transfer done with timeout
 */
static bool prvSdmmcWaitCmdDone(drvSdmmc_t *d)
{
    return OSI_LOOP_WAIT_TIMEOUT_US(prvSdmmcCheckStatus(d, SDMMC_CHECK_TRANSFER_DONE),
                                    SDMMC_CMD_TIMEOUT_US);
}

/**
 * Wait response ok with timeout
 */
static bool prvSdmmcWaitCmdResp(drvSdmmc_t *d)
{
    return OSI_LOOP_WAIT_TIMEOUT_US(prvSdmmcCheckStatus(d, SDMMC_CHECK_RESPONSE_OK),
                                    SDMMC_RESP_TIMEOUT_US);
}

/**
 * Check response, return false if there are errors.
 */

#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
bool prvSdmmcCheckResp(drvSdmmc_t *d, sdmmcCmd_t cmd)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
     return quec_sdmmcCheckResp(d,cmd);      
#else
    sdmmcCardStatus_t status = {.v = d->resp.v[0]};
    sdmmcCardStatus_t status_exp = {};
    sdmmcCardState_t state;

    switch (cmd)
    {
    case SDMMC_CMD_APP_CMD:
        return (status.b.ready_for_data && status.b.app_cmd);

    case SDMMC_CMD_SET_BLOCKLEN:
    case SDMMC_CMD_SEND_STATUS:
    case SDMMC_CMD_READ_SINGLE_BLOCK:
    case SDMMC_CMD_READ_MULT_BLOCK:
        status_exp.b.ready_for_data = 1;
        status_exp.b.current_state = SDMMC_CARD_STATE_TRAN;
        return (status.v == status_exp.v);

    case SDMMC_CMD_WRITE_SINGLE_BLOCK:
    case SDMMC_CMD_WRITE_MULT_BLOCK:
        state = status.b.current_state;
        if ((status.v & 0xffff0000) != 0 ||
            (state != SDMMC_CARD_STATE_TRAN &&
             state != SDMMC_CARD_STATE_RCV &&
             state != SDMMC_CARD_STATE_PRG))
            return false;
        return true;

    case SDMMC_CMD_SELECT_CARD:
        return status.b.ready_for_data;

    case SDMMC_CMD_SET_BUS_WIDTH:
    case SDMMC_CMD_SET_WR_BLK_COUNT:
        status_exp.b.ready_for_data = 1;
        status_exp.b.app_cmd = 1;
        status_exp.b.current_state = SDMMC_CARD_STATE_TRAN;
        return (status.v == status_exp.v);

    case SDMMC_CMD_SEND_RELATIVE_ADDR:
        d->rca = d->resp.v[0] & 0xffff0000;
        return true;

    case SDMMC_CMD_SEND_IF_COND:
        return ((d->resp.v[0] & 0xff) == SDMMC_CMD8_CHECK_PATTERN);

    case SDMMC_CMD_SEND_OP_COND:
        return status.b.app_cmd;

    default:
        return true;
    }
#endif
}

/**
 * Send command, wait response if needed, and optional check response
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
bool prvSdmmcCommand(drvSdmmc_t *d, sdmmcCmd_t cmd, uint32_t arg,
                            sdmmcRespOption_t resp_opt)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
   if(quec_sdmmc_init_is_sd()){
#endif
    if (cmd & SDMMC_ACMD_SEL)
    {
        // This is an application specific command,
        // we send the CMD55 first, response r1
        prvSdmmcSendCmd(d, SDMMC_CMD_APP_CMD, d->rca);

        // Wait for command over
        if (!prvSdmmcWaitCmdDone(d))
        {
            OSI_LOGE(0, "%4c send CMD55 timeout", d->name);
            return false;
        }

        if (!prvSdmmcWaitCmdResp(d))
        {
            OSI_LOGE(0, "%4c wait CMD55 response timeout", d->name);
            return false;
        }

        // Fetch response
        prvSdmmcGetResp(d, SDMMC_CMD_APP_CMD);
        if (!prvSdmmcCheckResp(d, SDMMC_CMD_APP_CMD))
        {
            OSI_LOGE(0, "%4c CMD55 response error 0x%x", d->name, &d->resp.v[0]);
            return false;
        }
    }
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    }
#endif

    // Send proper command. If it was an ACMD, the CMD55 have just been sent.
    prvSdmmcSendCmd(d, cmd, arg);

    // Wait for command to be sent
    if (!prvSdmmcWaitCmdDone(d))
    {
        OSI_LOGE(0, "%4c CMD %d send timeout", d->name, cmd & 0x3f);
        return false;
    }

    // Wait for response and get its argument
    if (resp_opt == SDMMC_RESP_FETCH || resp_opt == SDMMC_RESP_CHECK)
    {
        if (!prvSdmmcWaitCmdResp(d))
        {
            OSI_LOGE(0, "%4c cmd %d wait resp timeout", d->name, cmd & 0x3f);
            return false;
        }

        // Fetch response
        prvSdmmcGetResp(d, cmd);

        if (resp_opt == SDMMC_RESP_CHECK && !prvSdmmcCheckResp(d, cmd))
        {
            OSI_LOGE(0, "%4c cmd %d response error 0x%x", d->name, cmd & 0x3f, d->resp.v[0]);
            return false;
        }
    }
    return true;
}

/**
 * Read CSD
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
bool prvSdmmcReadCsd(drvSdmmc_t *d)
{
    if (!prvSdmmcCommand(d, SDMMC_CMD_SEND_CSD, d->rca, SDMMC_RESP_FETCH))
        return false;

    memcpy(&d->csd, &d->resp, sizeof(d->csd));

    OSI_LOGI(0, "%4c CSD %08x %08x %08x %08x", d->name,
             d->resp.v[3], d->resp.v[2], d->resp.v[1], d->resp.v[0]);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    quec_sdmmcReadCsd(d);
#else
    OSI_LOGD(0, "CSD: csd_struct = %d", prvCsdStruct(&d->csd));
    OSI_LOGD(0, "CSD: taac = %d", prvCsdTaac(&d->csd));
    OSI_LOGD(0, "CSD: nsac = %d", prvCsdNsac(&d->csd));
    OSI_LOGD(0, "CSD: trans_speed = %d", prvCsdTransSpeed(&d->csd));
    OSI_LOGD(0, "CSD: ccc = 0x%x", prvCsdCcc(&d->csd));
    OSI_LOGD(0, "CSD: read_bl_len = %d", prvCsdReadBlLen(&d->csd));
    OSI_LOGD(0, "CSD: read_bl_partial = %d", prvCsdReadBlPartial(&d->csd));
    OSI_LOGD(0, "CSD: write_blk_misalign = %d", prvCsdWriteBlkMisalign(&d->csd));
    OSI_LOGD(0, "CSD: read_blk_misalign = %d", prvCsdReadBlkMisalign(&d->csd));
    OSI_LOGD(0, "CSD: dsr_imp = %d", prvCsdDsrImp(&d->csd));
    OSI_LOGD(0, "CSD: c_size = %d", prvCsdCSize(&d->csd));
    if (prvCsdStruct(&d->csd) == SDMMC_CSD_VERSION_1)
    {
        OSI_LOGD(0, "CSD: vdd_r_curr_min = %d", prvCsdVddRCurrMin(&d->csd));
        OSI_LOGD(0, "CSD: vdd_r_curr_max = %d", prvCsdVddRCurrMax(&d->csd));
        OSI_LOGD(0, "CSD: vdd_w_curr_min = %d", prvCsdVddWCurrMin(&d->csd));
        OSI_LOGD(0, "CSD: vdd_w_curr_max = %d", prvCsdVddWCurrMax(&d->csd));
        OSI_LOGD(0, "CSD: c_size_mult = %d", prvCsdCSizeMult(&d->csd));
    }
    OSI_LOGD(0, "CSD: erase_blk_en = %d", prvCsdEraseBlkEn(&d->csd));
    OSI_LOGD(0, "CSD: sector_size = %d", prvCsdSectorSize(&d->csd));
    OSI_LOGD(0, "CSD: wr_grp_size = %d", prvCsdWrGrpSize(&d->csd));
    OSI_LOGD(0, "CSD: wr_grp_enable = %d", prvCsdWrGrpEnable(&d->csd));
    OSI_LOGD(0, "CSD: r2w_factor = %d", prvCsdR2wFactor(&d->csd));
    OSI_LOGD(0, "CSD: write_bl_len = %d", prvCsdWriteBlLen(&d->csd));
    OSI_LOGD(0, "CSD: write_bl_partial = %d", prvCsdWriteBlPartial(&d->csd));
    OSI_LOGD(0, "CSD: file_format_grp = %d", prvCsdFileFormatGrp(&d->csd));
    OSI_LOGD(0, "CSD: perm_write_protect = %d", prvCsdPermWriteProtect(&d->csd));
    OSI_LOGD(0, "CSD: tmp_write_protect = %d", prvCsdTmpWriteProtect(&d->csd));
    OSI_LOGD(0, "CSD: file_format = %d", prvCsdFileFormat(&d->csd));
    OSI_LOGD(0, "CSD: crc = 0x%x", prvCsdCrc(&d->csd));

    d->block_size = 512;
    if (d->csd.v1.csd_struct == SDMMC_CSD_VERSION_1)
    {
        int shift = (d->csd.v1.c_size_mult + d->csd.v1.read_bl_len + 2 - 9);
        d->block_count = (d->csd.v1.c_size + 1) << shift;
    }
    else if (d->csd.v1.csd_struct == SDMMC_CSD_VERSION_2)
    {
        d->block_count = (d->csd.v2.c_size + 1) * 1024;
    }
    else if (d->csd.v1.csd_struct == SDMMC_CSD_VERSION_2)
    {
        d->block_count = (d->csd.v3.c_size + 1) * 1024;
    }
#endif
    OSI_LOGI(0, "%4c card block size %d, block number %d",
             d->name, d->block_size, d->block_count);

    prvSdmmcSetBlockSizeReg(d);
    return true;
}

/**
 * Read CID
 */
 #ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif
bool prvSdmmcReadCid(drvSdmmc_t *d)
{
    if (!prvSdmmcCommand(d, SDMMC_CMD_ALL_SEND_CID, 0, SDMMC_RESP_FETCH))
        return false;

    memcpy(&d->cid, &d->resp, sizeof(d->cid));

    OSI_LOGI(0, "%4c CID %08x %08x %08x %08x", d->name,
             d->resp.v[3], d->resp.v[2], d->resp.v[1], d->resp.v[0]);
    return true;
}

/**
 * ISR of sdmmc controller
 */
static void prvSdmmcISR(void *param)
{
    drvSdmmc_t *d = (drvSdmmc_t *)param;

    REG_SDMMC_SDMMC_INT_STATUS_T cause = {d->hwp->sdmmc_int_status};
    d->hwp->sdmmc_int_clear = cause.v;

    OSI_LOGD(0, "%4c interrupt cause 0x%08x", d->name, cause.v);

    if (cause.b.dat_over_int)
    {
        // RX and TX can't work simultanuously. It is not needed to save
        // the direction. It is ok to release both semaphore.
        osiSemaphoreRelease(d->tx_done_sema);
        osiSemaphoreRelease(d->rx_done_sema);
    }
}

/**
 * Platform power on for sdmmc
 */
static void prvSdmmcPowerOn(drvSdmmc_t *d)
{
    // It seems card identification is unreliable without the reset
    if (d->name == DRV_NAME_SDMMC1)
    {
        hwp_sysCtrl->clk_sys_axi_enable = SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_SDMMC1; // write set
        hwp_sysCtrl->ap_apb_rst_set = SYS_CTRL_SET_AP_APB_RST_ID_SDMMC1;         // write clear
        osiDelayUS(100);
        hwp_sysCtrl->ap_apb_rst_clr = SYS_CTRL_CLR_AP_APB_RST_ID_SDMMC1; // write set
        osiDelayUS(100);
    }
    else
    {
        hwp_sysCtrl->clk_ap_apb_enable = SYS_CTRL_ENABLE_AP_APB_CLK_ID_SDMMC2; // write set
        hwp_sysCtrl->ap_apb_rst_set = SYS_CTRL_SET_AP_APB_RST_ID_SDMMC2;       // write clear
        osiDelayUS(100);
        hwp_sysCtrl->ap_apb_rst_clr = SYS_CTRL_CLR_AP_APB_RST_ID_SDMMC2; // write set
        osiDelayUS(100);
    }

    // Power off/on cycle can put device to known state
    halPmuSwitchPower(HAL_POWER_SD, false, false);
    osiThreadSleepUS(10000);

    halPmuSwitchPower(HAL_POWER_SD, true, true);
    osiThreadSleepUS(1000);
}

/**
 * Platform power off for sdmmc
 */
static void prvSdmmcPowerOff(drvSdmmc_t *d)
{
    // It seems card identification is unreliable without the reset
    if (d->name == DRV_NAME_SDMMC1)
    {
        hwp_sysCtrl->clk_sys_axi_disable = SYS_CTRL_DISABLE_SYS_AXI_CLK_ID_SDMMC1; // write clear
    }
    else
    {
        hwp_sysCtrl->clk_ap_apb_disable = SYS_CTRL_DISABLE_AP_APB_CLK_ID_SDMMC2; // write set
    }

    halPmuSwitchPower(HAL_POWER_SD, false, false);
}

/**
 * Suspend hook for power manegement
 */
static void prvSdmmcSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvSdmmc_t *d = (drvSdmmc_t *)ctx;
    if (!d->opened)
        return;

    drvIfcReleaseChannel(&d->rx_ifc);
    drvIfcReleaseChannel(&d->tx_ifc);
    d->hwp->sdmmc_int_mask = 0;
    d->pm_ctx.sdmmc_trans_speed_reg = d->hwp->sdmmc_trans_speed_reg;
}

/**
 * Resume hook for power manegement
 */
static void prvSdmmcResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    drvSdmmc_t *d = (drvSdmmc_t *)ctx;
    if (!d->opened)
        return;

    drvIfcRequestChannel(&d->rx_ifc);
    drvIfcRequestChannel(&d->tx_ifc);
    d->hwp->sdmmc_int_mask = SDMMC_DAT_OVER_MK;

    if (source & OSI_RESUME_ABORT)
        return;

    if (d->name == DRV_NAME_SDMMC1)
        hwp_sysCtrl->clk_sys_axi_enable = SYS_CTRL_ENABLE_SYS_AXI_CLK_ID_SDMMC1; // write set
    else
        hwp_sysCtrl->clk_ap_apb_enable = SYS_CTRL_ENABLE_AP_APB_CLK_ID_SDMMC2; // write set

    d->hwp->sdmmc_trans_speed_reg = d->pm_ctx.sdmmc_trans_speed_reg;
    d->hwp->sdmmc_mclk_adjust_reg = (d->clk_adj & 0x0f) | (d->clk_inv << 4);
    prvSdmmcSetDataWidth(d);
    prvSdmmcSetBlockSizeReg(d);
}

/**
 * PM source operations
 */
static const osiPmSourceOps_t gSdmmcPmOps = {
    .suspend = prvSdmmcSuspend,
    .resume = prvSdmmcResume,
};

/**
 * Create a sdmmc driver instance
 */
drvSdmmc_t *drvSdmmcCreate(uint32_t name)
{
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	quec_sdmmcCreateSetPower();
#endif
    if (name != DRV_NAME_SDMMC1 && name != DRV_NAME_SDMMC2)
        return NULL;

    drvSdmmc_t *d = (drvSdmmc_t *)calloc(1, sizeof(drvSdmmc_t));
    if (d == NULL)
        return NULL;

    if (name == DRV_NAME_SDMMC1)
    {
        d->hwp = hwp_sdmmc;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_SDMMC1);
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_SDMMC1);
    }
    else if (name == DRV_NAME_SDMMC2)
    {
        d->hwp = hwp_sdmmc2;
        d->irqn = HAL_SYSIRQ_NUM(SYS_IRQ_ID_SDMMC2);
        osiIrqSetPriority(d->irqn, SYS_IRQ_PRIO_SDMMC2);
    }

    d->name = name;
    d->opened = false;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	quec_sdmmcCreateSet(d);
#else
    d->ocr.v = 0x00ff8000;
    d->dsr = 0x04040000;
    d->rca = 0;
    d->card_is_sdhc = false;
    d->max_clk = 50000000;
    d->master_clk = SDMMC_CLK_FREQ_SD;
    d->bus_width = 1;
#endif
    d->clk_adj = 0x01;
    d->clk_inv = 0x01;

    d->lock = osiMutexCreate();
    d->tx_done_sema = osiSemaphoreCreate(1, 0);
    d->rx_done_sema = osiSemaphoreCreate(1, 0);
    d->pm_source = osiPmSourceCreate(name, &gSdmmcPmOps, d);
    osiClockConstrainInit(&d->clk_constrain, name);
    if (d->lock == NULL ||
        d->tx_done_sema == NULL ||
        d->rx_done_sema == NULL ||
        d->pm_source == NULL)
        goto failed;

    d->hwp->sdmmc_int_mask = 0x0;
    drvIfcChannelInit(&d->rx_ifc, d->name, DRV_IFC_RX);
    drvIfcChannelInit(&d->tx_ifc, d->name, DRV_IFC_TX);

    d->rx_enabled = true;
    d->tx_enabled = true;

    drvIfcRequestChannel(&d->tx_ifc);
    drvIfcRequestChannel(&d->rx_ifc);

    osiIrqSetHandler(d->irqn, prvSdmmcISR, d);
    osiIrqEnable(d->irqn);
    return d;

failed:
    drvSdmmcDestroy(d);
    return NULL;
}

/**
 * Wait tranfer done, by CMD13
 */
#ifndef CONFIG_QUEC_PROJECT_FEATURE_EMMC
static 
#endif 
bool prvSdmmcWaitTransDone(drvSdmmc_t *d, uint32_t timeout_us)
{
    osiElapsedTimer_t timer;

    osiElapsedTimerStart(&timer);
    while (1)
    {
        if (!prvSdmmcCommand(d, SDMMC_CMD_SEND_STATUS, d->rca, SDMMC_RESP_FETCH))
            return false;

        if (prvSdmmcCheckResp(d, SDMMC_CMD_SEND_STATUS))
            return true;

        if (osiElapsedTimeUS(&timer) > timeout_us)
        {
            OSI_LOGE(0, "%4c CMD13 timeout, resp/0x%x", d->name, d->resp.v[0]);
            return false;
        }
    }
}

/**
 * Read blocks
 */
static bool prvSdmmcReadBlocks(drvSdmmc_t *d, uint32_t address, void *buf, uint32_t count)
{
    osiSemaphoreTryAcquire(d->rx_done_sema, 0);

    // Command are different for reading one or several blocks of data
    sdmmcCmd_t cmd = (count == 1) ? SDMMC_CMD_READ_SINGLE_BLOCK : SDMMC_CMD_READ_MULT_BLOCK;

    // Initiate data migration through Ifc.
    prvSdmmcStartReadXfer(d, buf, count);

    // Initiate data migration of multiple blocks through SD bus.
    if (!prvSdmmcCommand(d, cmd, address, SDMMC_RESP_CHECK))
        goto fail;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC		
		quec_sdmmcCmdArg_Clear(d);
#endif

    if (!osiSemaphoreTryAcquire(d->rx_done_sema, SDMMC_READ_TIMEOUT_MS))
    {
        OSI_LOGE(0, "%4c read cmd timeout", d->name);
        goto fail;
    }

    if (!prvSdmmcCheckStatus(d, SDMMC_CHECK_DATA_OK))
    {
        OSI_LOGE(0, "%4c read block bad crc", d->name);
        goto fail;
    }

    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c wait trans done timeout", d->name);
        goto fail;
    }

    return true;

fail:
    prvSdmmcStopReadXfer(d);
    return false;
}

/**
 * Read blocks in blue screen mode
 */
static bool prvSdmmcBsReadBlocks(drvSdmmc_t *d, uint32_t address, void *buf, uint32_t count)
{
    // Command are different for reading one or several blocks of data
    sdmmcCmd_t cmd = (count == 1) ? SDMMC_CMD_READ_SINGLE_BLOCK : SDMMC_CMD_READ_MULT_BLOCK;

    // Initiate data migration through Ifc.
    prvSdmmcStartReadXfer(d, buf, count);

    // Clear interrupt status
    d->hwp->sdmmc_int_clear = d->hwp->sdmmc_int_status;

    // Initiate data migration of multiple blocks through SD bus.
    if (!prvSdmmcCommand(d, cmd, address, SDMMC_RESP_CHECK))
        goto fail;

    OSI_LOOP_WAIT(d->hwp->sdmmc_int_status & SDMMC_DAT_OVER_INT);

    if (!prvSdmmcCheckStatus(d, SDMMC_CHECK_DATA_OK))
    {
        OSI_LOGE(0, "%4c read block bad crc", d->name);
        goto fail;
    }

    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c wait trans done timeout", d->name);
        goto fail;
    }

    return true;

fail:
    prvSdmmcStopReadXfer(d);
    return false;
}

/**
 * Read API
 */
bool drvSdmmcRead(drvSdmmc_t *d, uint32_t nr, void *buf, uint32_t size)
{
    if (size == 0)
        return true;
    if (d == NULL || !d->opened || buf == NULL)
        return false;
    if (!HAL_ADDR_IS_SRAM_RAM(buf))
        return false;

    // d->block_size is 2^n
    if (!OSI_IS_ALIGNED(buf, 4) || !OSI_IS_ALIGNED(size, d->block_size))
        return false;

    unsigned count = size / d->block_size;
    if ((nr + count) > d->block_count)
        return false;

    OSI_LOGD(0, "%4c read nr/%d buf/0x%x size/%d", d->name, nr, buf, size);

    osiMutexLock(d->lock);
    osiRequestSysClkActive(&d->clk_constrain);

    // Address are block number for high capacity card, when the capacity is
    // larger than 2GB, sector access mode, or byte access mode
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	unsigned address = 0;
    quec_sdmmcReadAdress(d,nr,&address);
#else
    unsigned address = d->card_is_sdhc ? nr : nr * d->block_size;
#endif
    bool ret = true;
    while (count > 0)
    {
        unsigned rcount = OSI_MIN(unsigned, SDMMC_MAX_BLOCK_PER_XFER, count);
        ret = osiIsPanic()
                  ? prvSdmmcBsReadBlocks(d, address, buf, rcount)
                  : prvSdmmcReadBlocks(d, address, buf, rcount);
        if (!ret)
            break;

        unsigned rsize = rcount * d->block_size;
        buf = (char *)buf + rsize;
        count -= rcount;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    	quec_sdmmcReadMultiAdress(d,rcount,rsize,&address);
#else	
        address += d->card_is_sdhc ? rcount : rsize;
#endif
    }

    osiReleaseClk(&d->clk_constrain);
    osiMutexUnlock(d->lock);
    return ret;
}

/**
 * Write blocks
 */
static bool prvSdmmcWriteBlocks(drvSdmmc_t *d, uint32_t address, const void *buf, uint32_t count)
{
    osiSemaphoreTryAcquire(d->tx_done_sema, 0);

    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c pre wait trans done timeout", d->name);
        return false;
    }

    // PreErasing, to accelerate the multi-block write operations
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
    if(!quec_sdmmcWriteBlocksSendCmd_SD(d,count))
	{
	   return false;
	}
#else
    if (count > 1)
    {
        if (!prvSdmmcCommand(d, SDMMC_CMD_SET_WR_BLK_COUNT, count, SDMMC_RESP_CHECK))
            return false;
    }
#endif

    // Initiate data migration through Ifc.
    prvSdmmcStartWriteXfer(d, buf, count);

    // Initiate data migration of multiple blocks through SD bus.
    sdmmcCmd_t cmd = (count == 1) ? SDMMC_CMD_WRITE_SINGLE_BLOCK : SDMMC_CMD_WRITE_MULT_BLOCK;
    if (!prvSdmmcCommand(d, cmd, address, SDMMC_RESP_CHECK))
        goto fail;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
      quec_sdmmcCmdArg_Clear(d);
#endif

    if (!osiSemaphoreTryAcquire(d->tx_done_sema, SDMMC_WRITE_TIMEOUT_MS))
    {
        OSI_LOGE(0, "%4c write cmd timeout", d->name);
        goto fail;
    }

    // Nota: CMD12 (stop transfer) is automatically
    // sent by the SDMMC controller.

    if (!prvSdmmcCheckStatus(d, SDMMC_CHECK_CRC_OK))
    {
        OSI_LOGE(0, "%4c write block bad crc, status 0x%x",
                 d->name, d->hwp->sdmmc_status);
        goto fail;
    }

    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c post wait trans done timeout", d->name);
        goto fail;
    }
    return true;

fail:
    prvSdmmcStopWriteXfer(d);
    return false;
}

/**
 * Write blocks in blue screen mode
 */
static bool prvSdmmcBsWriteBlocks(drvSdmmc_t *d, uint32_t address, const void *buf, uint32_t count)
{
    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c pre wait trans done timeout", d->name);
        return false;
    }

    // PreErasing, to accelerate the multi-block write operations
    if (count > 1)
    {
        if (!prvSdmmcCommand(d, SDMMC_CMD_SET_WR_BLK_COUNT, count, SDMMC_RESP_CHECK))
            return false;
    }

    // Initiate data migration through Ifc.
    prvSdmmcStartWriteXfer(d, buf, count);

    // Clear interrupt status
    d->hwp->sdmmc_int_clear = d->hwp->sdmmc_int_status;

    // Initiate data migration of multiple blocks through SD bus.
    sdmmcCmd_t cmd = (count == 1) ? SDMMC_CMD_WRITE_SINGLE_BLOCK : SDMMC_CMD_WRITE_MULT_BLOCK;
    if (!prvSdmmcCommand(d, cmd, address, SDMMC_RESP_CHECK))
        goto fail;

    OSI_LOOP_WAIT(d->hwp->sdmmc_int_status & SDMMC_DAT_OVER_INT);

    // Nota: CMD12 (stop transfer) is automatically
    // sent by the SDMMC controller.

    if (!prvSdmmcCheckStatus(d, SDMMC_CHECK_CRC_OK))
    {
        OSI_LOGE(0, "%4c write block bad crc, status 0x%x",
                 d->name, d->hwp->sdmmc_status);
        goto fail;
    }

    // Check that the card is in tran (Transmission) state.
    if (!prvSdmmcWaitTransDone(d, SDMMC_TRAN_TIMEOUT_US))
    {
        OSI_LOGE(0, "%4c post wait trans done timeout", d->name);
        goto fail;
    }
    return true;

fail:
    prvSdmmcStopWriteXfer(d);
    return false;
}

/**
 * Write API
 */
bool drvSdmmcWrite(drvSdmmc_t *d, uint32_t nr, const void *buf, uint32_t size)
{
    if (size == 0)
        return true;
    if (d == NULL || !d->opened || buf == NULL)
        return false;
    if (!HAL_ADDR_IS_SRAM_RAM(buf))
        return false;

    // d->block_size is 2^n
    if (!OSI_IS_ALIGNED(buf, 4) || !OSI_IS_ALIGNED(size, d->block_size))
        return false;

    unsigned count = size / d->block_size;
    if ((nr + count) > d->block_count)
        return false;

    OSI_LOGD(0, "%4c write nr/%d buf/0x%x size/%d", d->name, nr, buf, size);

    osiMutexLock(d->lock);
    osiRequestSysClkActive(&d->clk_constrain);

    // Addresses are block number for high capacity card
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	quec_sdmmcSetCard_is_sdhc(d);
#endif
    unsigned address = d->card_is_sdhc ? nr : nr * d->block_size;
    bool ret = true;
    while (count > 0)
    {
        unsigned wcount = OSI_MIN(unsigned, SDMMC_MAX_BLOCK_PER_XFER, count);
        ret = osiIsPanic()
                  ? prvSdmmcBsWriteBlocks(d, address, buf, wcount)
                  : prvSdmmcWriteBlocks(d, address, buf, wcount);
        if (!ret)
            break;

        unsigned wsize = wcount * d->block_size;
        buf = (char *)buf + wsize;
        count -= wcount;
        address += d->card_is_sdhc ? wcount : wsize;
    }

    osiReleaseClk(&d->clk_constrain);
    osiMutexUnlock(d->lock);
    return ret;
}

/**
 * Open sdmmc driver
 */
bool drvSdmmcOpen(drvSdmmc_t *d)
{
    if (d == NULL)
        return false;
    if (d->opened)
        return true;

    osiMutexLock(d->lock);
    osiRequestSysClkActive(&d->clk_constrain);

    OSI_LOGI(0, "%4c open ...", d->name);

    prvSdmmcPowerOn(d);
    prvSdmmcSetClk(d, SDMMC_CLK_FREQ_IDENTIFY);
    osiDelayUS(1000);

    d->hwp->sdmmc_int_mask = SDMMC_DAT_OVER_MK;
#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
	if(!quec_sdmmcOpen_sub(d))
	{
		 goto fail;
	}
#else
	osiElapsedTimer_t timer;
    // Send power on command
    prvSdmmcCommand(d, SDMMC_CMD_GO_IDLE_STATE, 0, SDMMC_RESP_NONE);
    osiDelayUS(1000);

    // Check if the card is a spec vers.2 one, response is r7
    unsigned cmd8_arg = (SDMMC_CMD8_VOLTAGE_SEL | SDMMC_CMD8_CHECK_PATTERN);
    if (prvSdmmcCommand(d, SDMMC_CMD_SEND_IF_COND, cmd8_arg, SDMMC_RESP_FETCH))
    {
        OSI_LOGI(0, "%4c CMD8 done (support spec v2.0) resp/0x%x", d->name, d->resp.v[0]);
        if (!prvSdmmcCheckResp(d, SDMMC_CMD_SEND_IF_COND))
        {
            OSI_LOGE(0, "%4c CMD8 status error", d->name);
            goto fail;
        }

        d->ocr.b.ccs = 1;
    }

    // Send OCR, as long as the card return busy
    osiElapsedTimerStart(&timer);
    for (;;)
    {
        osiDelayUS(1000);
        if (!prvSdmmcCommand(d, SDMMC_CMD_SEND_OP_COND, d->ocr.v, SDMMC_RESP_FETCH))
            goto fail;

        // Bit 31 is power up busy status bit (pdf spec p. 109)
        sdmmcOcr_t ocr = {d->resp.v[0]};
        if (ocr.b.power_on_finish)
        {
            OSI_LOGI(0, "%4c ACMD41 done, resp/0x%x", d->name, d->resp.v[0]);

            d->ocr.v = ocr.v;
            d->ocr.b.power_on_finish = 0;

            if (d->ocr.b.ccs)
            {
                d->card_is_sdhc = true;
                d->master_clk = SDMMC_CLK_FREQ_SDHC;
                OSI_LOGI(0, "%4c card is SDHC", d->name);
            }
            else
            {
                d->card_is_sdhc = false;
                d->master_clk = SDMMC_CLK_FREQ_SD;
                OSI_LOGI(0, "%4c card is V2, but NOT SDHC", d->name);
            }
            break;
        }

        if (osiElapsedTimeUS(&timer) > SDMMC_SDMMC_OCR_TIMEOUT_US)
        {
            OSI_LOGI(0, "%4c ACMD41 retry timeout", d->name);
            goto fail;
        }
    }

    if (!prvSdmmcReadCid(d))
        goto fail;

    if (!prvSdmmcCommand(d, SDMMC_CMD_SEND_RELATIVE_ADDR, d->rca, SDMMC_RESP_CHECK))
        goto fail;

    d->rca = d->resp.v[0] & 0xffff0000;
    OSI_LOGI(0, "%4c CMD3 done, resp/0x%x, rca/0x%x", d->name, d->resp.v[0], d->rca);

    if (!prvSdmmcReadCsd(d))
        goto fail;

    // Select the card
    if (!prvSdmmcCommand(d, SDMMC_CMD_SELECT_CARD, d->rca, SDMMC_RESP_CHECK))
        goto fail;

    // Set the bus width (use 4 data lines for SD cards only)
    d->bus_width = SDMMC_DATA_BUS_WIDTH_4;
    if (!prvSdmmcCommand(d, SDMMC_CMD_SET_BUS_WIDTH, d->bus_width, SDMMC_RESP_CHECK))
        goto fail;

    prvSdmmcSetDataWidth(d);

    // Configure the block lenght
    if (!prvSdmmcCommand(d, SDMMC_CMD_SET_BLOCKLEN, d->block_size, SDMMC_RESP_CHECK))
        goto fail;

#endif
    /* For cards, fpp = 25MHz for sd, fpp = 50MHz, for sdhc */
    prvSdmmcSetClk(d, d->master_clk);

    OSI_LOGI(0, "%4c open done", d->name);
    d->opened = true;
    osiReleaseClk(&d->clk_constrain);
    osiMutexUnlock(d->lock);
    return true;

fail:
    d->opened = false;
    d->hwp->sdmmc_int_mask = 0;
    prvSdmmcPowerOff(d);
    osiReleaseClk(&d->clk_constrain);
    osiMutexUnlock(d->lock);
    return false;
}

/**
 * Close sdmmc driver
 */
void drvSdmmcClose(drvSdmmc_t *d)
{
    if (d == NULL || !d->opened)
        return;

    osiMutexLock(d->lock);
    d->opened = false;
    d->hwp->sdmmc_int_mask = 0;
    prvSdmmcPowerOff(d);
    osiMutexUnlock(d->lock);
}

/**
 * Delete sdmmc driver instance
 */
void drvSdmmcDestroy(drvSdmmc_t *d)
{
    if (d == NULL)
        return;

    drvSdmmcClose(d);
    drvIfcReleaseChannel(&d->rx_ifc);
    drvIfcReleaseChannel(&d->tx_ifc);
    osiSemaphoreDelete(d->tx_done_sema);
    osiSemaphoreDelete(d->rx_done_sema);
    osiMutexDelete(d->lock);
    osiPmSourceDelete(d->pm_source);
    osiReleaseClk(&d->clk_constrain);
    osiIrqDisable(d->irqn);
    osiIrqSetHandler(d->irqn, NULL, NULL);
    free(d);
}

/**
 * Get block number
 */
uint32_t drvSdmmcGetBlockNum(drvSdmmc_t *d)
{
    if (d == NULL)
        return 0;
    return d->block_count;
}
