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

#include "diag_config.h"
#include "diag.h"
#include "osi_api.h"
#include "osi_log.h"
#include "drv_uart.h"
#include "cmddef.h"
#include "nvm.h"
#include "calclib/crc32.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <nvm.h>

#define RW_MASK (0x80)
#define MAX_MSG_SIZE (512)
#define MAX_SN_LEN (24)
#define MAX_STATION_NUM (15)
#define MAX_LAST_DESCRIPTION_LEN (32)
#define MAX_STATION_NAME_LEN (10)
enum
{
    DIR_ERR_NONE = 0,
    DIR_CRC_ERR = 1,
    DIR_CMD_ERR,
    DIR_SAVE_ERR,
    DIR_READ_ERR
};

/**
 * @brief diag command subtype bito ~ bit6
*/
enum
{
    RM_CALI_NV_IMEI1 = 0, //operator IMEI1
    RM_CALI_NV_IMEI2 = 1, //operator IMEI2
    RM_CALI_NV_BT = 2,    //operator BT
    RM_CALI_NV_GPS = 3,   //operator GPS
    RM_CALI_NV_GUID = 4,  //operator GUID
    RM_CALI_NV_SN = 5,    //operator SN
    RM_CALI_NV_WIFI = 6,  //operator WIFI
};

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef struct
{
    uint8_t trans_buf[MAX_MSG_SIZE];
    uint32_t trans_buf_size;
} diagProd_t;

static bool _handleNvm(const diagMsgHead_t *cmd, void *ctx);
static uint16_t _diagNvDirectRead(const diagMsgHead_t *cmd, void *buffer, uint16_t size);
static bool _diagNvDirectWrite(const diagMsgHead_t *cmd);
static bool _diagNvDirect(const diagMsgHead_t *cmd, void *ctx);
void diagSwverInit(void);

/*this function is used for read production information,but there is not data at nv ,so out a temp data*/
static bool _handleNvm(const diagMsgHead_t *cmd, void *ctx)
{
    uint32_t Magic;
    diagMsgHead_t head = *cmd;
    switch (cmd->subtype)
    {
    case NVITEM_PRODUCT_CTRL_READ:
        Magic = 0x53503039;
        head.subtype = 0x0;
        diagOutputPacket3(&head, (char *)diagCmdData(cmd), head.len - sizeof(diagMsgHead_t), &Magic, sizeof(uint32_t));
        break;

    default:
        break;
    }
    return true;
}
static uint16_t _diagNvDirectRead(const diagMsgHead_t *cmd, void *buffer, uint16_t size)
{
    uint16_t nv_len = 0;
    uint8_t cmd_mask = cmd->subtype & 0x7f;

    switch (cmd->type)
    {
    case DIAG_DIRECT_NV:
        nv_len = sizeof(refNVDirect_t);
        if (size < nv_len)
            return 0;
        memset(buffer, 0, nv_len);
        refNVDirect_t *nvinfo = (refNVDirect_t *)buffer;

        for (uint8_t i = 0; i < 7; ++i)
        {
            if (cmd_mask & (1 << i))
            {
                if (i == RM_CALI_NV_IMEI1)
                {
                    if (-1 == nvmReadImei2(0, &nvinfo->imei1))
                        return 0;
                }
                else if (i == RM_CALI_NV_IMEI2)
                {
                    if (-1 == nvmReadImei2(1, &nvinfo->imei2))
                        return 0;
                }
            }
        }
        break;

    case DIAG_DIRECT_PHSCHK:
        nv_len = sizeof(phaseCheckHead_t);
        if (size < nv_len)
            return 0;
        memset(buffer, 0, nv_len);
        phaseCheckHead_t *nvInfo = (phaseCheckHead_t *)buffer;
        nvInfo->Magic = 0x53503039; //temp data,there is not magic at nv
        if (!(nvmReadSN(1, nvInfo->SN, MAX_SN_LEN)))
        {
            memset(buffer, 0xff, sizeof(phaseCheckHead_t));
            return 0;
        }
    case DIAG_DIRECT_RDVER:
    case DIAG_DIRECT_IMEINUM:
    default:
        break;
    }
    OSI_LOGD(0, "nv_len = %d", nv_len);
    return nv_len;
}

static bool _diagNvDirectWrite(const diagMsgHead_t *cmd)
{
    uint8_t cmd_mask = cmd->subtype & 0x7f;
    uint16_t datalen = diagCmdDataSize(cmd);
    if (datalen < sizeof(uint16_t))
        return false;

    datalen -= sizeof(uint16_t);
    switch (cmd->type)
    {
    case DIAG_DIRECT_NV:
        if (datalen != sizeof(refNVDirect_t))
            return false;

        refNVDirect_t *nv_info = (refNVDirect_t *)diagCmdData(cmd);
        for (uint8_t i = 0; i < 7; ++i)
        {
            if (cmd_mask & (1 << i))
            {
                if (i == RM_CALI_NV_IMEI1)
                {
                    if (!nvmWriteImei2(0, &nv_info->imei1))
                        return false;
                }
                else if (i == RM_CALI_NV_IMEI2)
                {
                    if (!nvmWriteImei2(1, &nv_info->imei2))
                        return false;
                }
            }
        }
        return true;
    case DIAG_DIRECT_PHSCHK:
        if (datalen != sizeof(phaseCheckHead_t))
            return false;

        phaseCheckHead_t *nv_Info = (phaseCheckHead_t *)diagCmdData(cmd);
        nv_Info->Magic = 0x11223344; //there is not magic at nv,it is a temp data
        if (nvmWriteSN(1, nv_Info->SN, MAX_SN_LEN))
            return true;

    case DIAG_DIRECT_IMEINUM:
    case DIAG_DIRECT_RDVER:
    default:
        break;
    }
    return false;
}

static bool _diagNvDirect(const diagMsgHead_t *cmd, void *ctx)
{
    diagProd_t *prod = (diagProd_t *)ctx;
    diagMsgHead_t head = *cmd;
    uint8_t *subtype = &head.subtype;
    uint16_t buf_data = 0;
    uint16_t crc_data = 0;
    uint16_t recv_crc = *(uint16_t *)((uint8_t *)cmd + cmd->len - sizeof(uint16_t));
    uint16_t crc = crc16NvCalc(diagCmdData(cmd), diagCmdDataSize(cmd) - sizeof(uint16_t));
    if (recv_crc != crc)
    {
        *subtype = 0;
        crc_data = DIR_CRC_ERR;
        head.len = sizeof(diagMsgHead_t) + sizeof(uint16_t);
        diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        return false;
    }

    uint8_t read_mode = *subtype & RW_MASK;
    if (read_mode)
    {
        uint16_t result_len = _diagNvDirectRead(cmd, prod->trans_buf, prod->trans_buf_size);
        if (result_len)
        {
            *subtype = 1;
            crc_data = crc16NvCalc(prod, result_len);
            diagOutputPacket3(&head, prod->trans_buf, result_len, &crc_data, sizeof(uint16_t));
        }
        else
        {
            *subtype = 0;
            buf_data = 0;
            diagOutputPacket2(&head, &buf_data, sizeof(uint16_t));
        }
    }
    else
    {
        bool result = _diagNvDirectWrite(cmd);
        if (result)
        {
            *subtype = 1;
            crc_data = 0;
            diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        }
        else
        {
            *subtype = 0;
            buf_data = 0;
            crc_data = DIR_SAVE_ERR;
            diagOutputPacket3(&head, &crc_data, sizeof(uint16_t), &buf_data, sizeof(uint16_t));
        }
    }
    return true;
}

void diagNvInit(void)
{
    static diagProd_t _prod;
    _prod.trans_buf_size = sizeof(_prod.trans_buf);

    diagRegisterCmdHandle(DIAG_NVITEM_F, _handleNvm, NULL);
    diagRegisterCmdHandle(DIAG_DIRECT_NV, _diagNvDirect, &_prod);
    diagRegisterCmdHandle(DIAG_DIRECT_PHSCHK, _diagNvDirect, &_prod);
}
