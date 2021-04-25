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

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('L', 'Z', 'M', 'A')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG

#include "hal_lzma.h"
#include "hwregs.h"
#include "osi_api.h"
#include "osi_compiler.h"
#include "osi_log.h"
#include <string.h>

#define ISRAM(n) (OSI_IS_IN_REGION(unsigned, n, CONFIG_RAM_PHY_ADDRESS, CONFIG_RAM_SIZE) || \
                  OSI_IS_IN_REGION(unsigned, n, CONFIG_SRAM_PHY_ADDRESS, CONFIG_SRAM_SIZE))

typedef struct
{
    uint32_t data_size;  // size in bytes
    uint16_t block_size; // size in 1KB bytes
    uint8_t dict_size;   // size in 1KB bytes
    uint8_t prop;        // the property byte
} lzmaFileHeader_t;

typedef struct
{
    uint32_t stream_size;
    uint32_t data_crc;
} lzmaBlockHeader_t;

int halLzmaDataSize(const void *stream)
{
    if (stream == NULL)
        return -1;

    lzmaFileHeader_t *fheader = (lzmaFileHeader_t *)stream;
    return fheader->data_size;
}

int halLzmaMaxBlockStreamSize(const void *stream, uint32_t size)
{
    if (stream == NULL)
        return -1;

    int st_size = -1;
    const char *p = (const char *)stream;
    const char *pend = p + size;
    p += sizeof(lzmaFileHeader_t);
    while (p < pend)
    {
        lzmaBlockHeader_t *bh = (lzmaBlockHeader_t *)p;
        uint32_t ss = bh->stream_size;
        if (ss > st_size)
            st_size = ss;
        p += 4 + ss;
    }
    return st_size;
}

int halLzmaDecompressFile(const void *stream, void *dest)
{
    OSI_LOGD(0, "lzma: src/%p dest/%p", stream, dest);

    if (stream == NULL || dest == NULL)
        return -1;

    if (!OSI_IS_ALIGNED(dest, 32) || !OSI_IS_ALIGNED(stream, 8))
        return -1;

    if (!ISRAM(stream) || !ISRAM(dest))
        return -1;

    lzmaFileHeader_t *fheader = (lzmaFileHeader_t *)stream;
    unsigned data_size = fheader->data_size;
    unsigned block_size = fheader->block_size << 10;
    unsigned dict_size = fheader->dict_size << 10;

    unsigned count = (fheader->data_size + block_size - 1) / block_size;
    const char *ps = (const char *)stream + sizeof(lzmaFileHeader_t);
    char *pd = (char *)dest;

    OSI_LOGD(0, "lzma: data_size/%u block_size/%u dict_size/%u count/%u",
             data_size, block_size, dict_size, count);

    unsigned remained = data_size;
    for (int n = 0; n < count; n++)
    {
        lzmaBlockHeader_t *bheader = (lzmaBlockHeader_t *)ps;
        unsigned src_size = bheader->stream_size;
        unsigned dest_size = (remained > block_size) ? block_size : remained;
        uint32_t crc = 0;
        if (!halLzmaDecompressBlock(ps + sizeof(lzmaBlockHeader_t), src_size,
                                    pd, dest_size, dict_size, &crc))
            return -1;

        if (crc != bheader->data_crc)
        {
            OSI_LOGD(0, "lzma: crc mismatch");
            return -1;
        }

        remained -= dest_size;
        pd += dest_size;
        ps += sizeof(lzmaBlockHeader_t) + OSI_ALIGN_UP(src_size, 8);
    }

    return data_size;
}

bool halLzmaDecompressBlock(const void *src, uint32_t src_size,
                            void *dest, uint32_t dest_size,
                            uint32_t dict_size, uint32_t *crc)
{
    OSI_LOGD(0, "lzma: src/%p size/%u dest/%p size/%u dict/%u",
             src, src_size, dest, dest_size, dict_size);

    if (src == NULL || dest == NULL)
        return -1;

    if (!OSI_IS_ALIGNED(dest, 32) || !OSI_IS_ALIGNED(src, 8))
        return -1;

    if (!ISRAM(src) || !ISRAM(dest))
        return -1;

    halLzmaEnable();

    hwp_lzma->lzma_cmd_reg = 0;
    hwp_lzma->lzma_status_reg = 0;
    REG_WAIT_COND(hwp_lzma->lzma_cmd_reg == 0);
    REG_WAIT_COND(hwp_lzma->lzma_status_reg == 0);

    hwp_lzma->lzma_dma_raddr_reg = (uint32_t)src;
    hwp_lzma->lzma_dma_waddr_reg = (uint32_t)dest;

    REG_LZMA_LZMA_CONFIG_REG1_T config_reg1 = {
        .b.reg_dict_size = dict_size,
        .b.reg_block_size = dest_size,
    };
    hwp_lzma->lzma_config_reg1 = config_reg1.v;

    REG_LZMA_LZMA_CONFIG_REG2_T config_reg2 = {
        .b.reg_stream_len = src_size,
    };
    hwp_lzma->lzma_config_reg2 = config_reg2.v;

    REG_LZMA_LZMA_CONFIG_REG3_T config_reg3 = {
        .b.reg_refbyte_en = 0,
        .b.reg_cabac_movebits = 1,
        .b.reg_cabac_totalbits = 0,
    };
    hwp_lzma->lzma_config_reg3 = config_reg3.v;

    osiDCacheClean(src, src_size);
    osiDCacheInvalidate(dest, dest_size);

    hwp_lzma->lzma_cmd_reg = 1;

    REG_WAIT_COND(hwp_lzma->lzma_status_reg != 0);
    REG_LZMA_LZMA_STATUS_REG_T status = {hwp_lzma->lzma_status_reg};

    OSI_LOGD(0, "lzma: status/0x%x/0x%x/0x%x/0x%x", status.v,
             hwp_lzma->lzma_status_reg2, hwp_lzma->lzma_status_reg3,
             hwp_lzma->lzma_cmd_reg);

    if (status.v == 1) // dec_done without error
    {
        if (crc != NULL)
            *crc = hwp_lzma->lzma_output_crc;

        hwp_lzma->lzma_cmd_reg = 0;
        hwp_lzma->lzma_status_reg = 0;
        halLzmaDisable();
        osiDCacheInvalidate(dest, dest_size);
        return true;
    }

    halLzmaReset();
    halLzmaDisable();
    return false;
}

void halLzmaReset(void)
{
    REG_SYS_CTRL_SYS_AXI_RST_SET_T axi_set;
    REG_SYS_CTRL_SYS_AXI_RST_CLR_T axi_clr;
    REG_SYS_CTRL_SYS_AHB_RST_SET_T ahb_set;
    REG_SYS_CTRL_SYS_AHB_RST_CLR_T ahb_clr;
    REG_SYS_CTRL_AP_APB_RST_SET_T apb_set;
    REG_SYS_CTRL_AP_APB_RST_CLR_T apb_clr;

    REG_FIELD_WRITE1(hwp_sysCtrl->sys_axi_rst_set, axi_set, set_sys_axi_rst_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->sys_ahb_rst_set, ahb_set, set_sys_ahb_rst_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->ap_apb_rst_set, apb_set, set_ap_apb_rst_id_lzma, 1);
    osiDelayUS(10);

    REG_FIELD_WRITE1(hwp_sysCtrl->sys_axi_rst_clr, axi_clr, clr_sys_axi_rst_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->sys_ahb_rst_clr, ahb_clr, clr_sys_ahb_rst_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->ap_apb_rst_clr, apb_clr, clr_ap_apb_rst_id_lzma, 1);
    osiDelayUS(10);
}

void halLzmaEnable(void)
{
    REG_SYS_CTRL_CLK_SYS_AXI_ENABLE_T clk_sys_axi_enable;
    REG_SYS_CTRL_CLK_SYS_AHB_ENABLE_T clk_sys_ahb_enable;
    REG_SYS_CTRL_CLK_AP_APB_ENABLE_T clk_ap_apb_enable;

    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_axi_enable, clk_sys_axi_enable,
                     enable_sys_axi_clk_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_ahb_enable, clk_sys_ahb_enable,
                     enable_sys_ahb_clk_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_ap_apb_enable, clk_ap_apb_enable,
                     enable_ap_apb_clk_id_lzma, 1);
}

void halLzmaDisable(void)
{
    REG_SYS_CTRL_CLK_SYS_AXI_DISABLE_T clk_sys_axi_disable;
    REG_SYS_CTRL_CLK_SYS_AHB_DISABLE_T clk_sys_ahb_disable;
    REG_SYS_CTRL_CLK_AP_APB_DISABLE_T clk_ap_apb_disable;

    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_axi_disable, clk_sys_axi_disable,
                     disable_sys_axi_clk_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_sys_ahb_disable, clk_sys_ahb_disable,
                     disable_sys_ahb_clk_id_lzma, 1);
    REG_FIELD_WRITE1(hwp_sysCtrl->clk_ap_apb_disable, clk_ap_apb_disable,
                     disable_ap_apb_clk_id_lzma, 1);
}
