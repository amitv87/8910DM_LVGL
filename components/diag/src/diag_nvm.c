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
#include "vfs.h"
#include "nvm.h"
#include "hal_shmem_region.h"
#include "cmddef.h"
#include "calclib/crc32.h"
#include <stdlib.h>
#include <string.h>
#include "connectivity_config.h"
#ifdef CONFIG_BLUEU_BT_ENABLE
#include "bt_app.h"
#endif

#define DIAG_CMD_PARAM(cmd, type) const type *param = (const type *)diagCmdData(cmd)

#define NVITEM_MIN_ID 0
#define NVITEM_MAX_ID 0x1500 //TODO
#define RW_MASK (0x80)
#define MN_MAX_IMEI_LENGTH (8)
#define MAX_MSG_SIZE (512)

enum
{
    DIR_ERR_NONE = 0,
    DIR_CRC_ERR = 1,
    DIR_CMD_ERR,
    DIR_SAVE_ERR,
    DIR_READ_ERR
};

enum
{
    RM_CALI_NV_IMEI1 = 0,
    RM_CALI_NV_IMEI2 = 1,
    RM_CALI_NV_BT = 2,
    RM_CALI_NV_GPS = 3,
    RM_CALI_NV_GUID = 4,
    RM_CALI_NV_SN = 5,
    RM_CALI_NV_WIFI = 6,
};

enum
{
    NV_BT_CONFIG = 401,
    NV_MODEM_NV_BASE = 600,
    NV_MODEM_STATIC_NV = NV_MODEM_NV_BASE + 1,
    NV_MODEM_PHY_NV = NV_MODEM_NV_BASE + 2,
    NV_MODEM_DYNAMIC_NV = NV_MODEM_NV_BASE + 3,
    NV_MODEM_STATIC_CARD2_NV = NV_MODEM_NV_BASE + 4,
    NV_MODEM_PHY_CARD2_NV = NV_MODEM_NV_BASE + 5,
    NV_MODEM_DYNAMIC_CARD2_NV = NV_MODEM_NV_BASE + 6,
    NV_MODEM_RF_NV = 620,
    NV_8910_FIXNV_START = 621,
    NV_MODEM_RF_CALIB_GSM = NV_8910_FIXNV_START,
    NV_MODEM_RF_CALIB_LTE = 622,
    NV_MODEM_RM_GOLDEN_BOARD = 623,
    NV_8910_FIXNV_END = NV_MODEM_RM_GOLDEN_BOARD,
    NV_MODEM_CFW_NV = 631,
    NV_MODEM_AUD_CALIB = 632,
};

enum
{
    NVERR_NONE = 0, /* Success */
    NVERR_SYSTEM,   /* System error, e.g. hardware failure */
    NVERR_INVALID_PARAM,
    NVERR_NO_ENOUGH_RESOURCE,
    NVERR_NOT_EXIST,
    NVERR_ACCESS_DENY,
    NVERR_INCOMPATIBLE,
    NVERR_NOT_OPENED
};

typedef struct
{
    uint8_t trans_buf[MAX_MSG_SIZE];
    uint32_t trans_buf_size;
} diagProd_t;

typedef struct
{
    uint16_t id;
} nvitemRead_t;

typedef struct
{
    uint16_t id;
    uint8_t data[0];
} nvitemReadCnf_t;

typedef struct
{
    uint16_t id;
    uint8_t data[0];
} nvitemWrite_t;

typedef struct
{
    uint16_t id;
} nvitemGetlength_t;

typedef struct
{
    uint16_t id;
    uint16_t len;
} nvitemGetlengthCnf_t;

typedef struct
{
    uint16_t from;
    uint16_t to;
} nvitemDelete_t;

typedef struct
{
    uint16_t offset;
    uint16_t len;
} nvitemProductCtrlRead_t;

typedef struct
{
    uint16_t offset;
    uint16_t len;
} nvitemProductCtrlWrite_t;

typedef struct
{
    uint8_t mode;
} nvitemUsbAutoMode_t;

static bool _handleNvm(const diagMsgHead_t *cmd, void *ctx)
{
    diagMsgHead_t head = *cmd;
    switch (cmd->subtype)
    {
    case NVITEM_READ:
    {
        DIAG_CMD_PARAM(cmd, nvitemRead_t);

        int nvsize = nvmReadItem(param->id, NULL, 0);
        if (nvsize <= 0)
        {
            // No this ID or No content of it.
            head.subtype = NVERR_NOT_EXIST;
            diagOutputPacket2(&head, NULL, 0);
        }
        else
        {
            nvitemReadCnf_t *cnf = (nvitemReadCnf_t *)malloc(sizeof(nvitemReadCnf_t) + nvsize);
            cnf->id = param->id;

            nvmReadItem(param->id, cnf->data, nvsize);
            head.subtype = NVERR_NONE;
            diagOutputPacket2(&head, cnf, sizeof(nvitemReadCnf_t) + nvsize);
            free(cnf);
        }
        break;
    }
    case NVITEM_WRITE:
    {
        DIAG_CMD_PARAM(cmd, nvitemWrite_t);

        int data_size = cmd->len - sizeof(diagMsgHead_t) - sizeof(nvitemWrite_t);
        if (nvmWriteItem(param->id, param->data, data_size) < 0)
        {
            OSI_LOGD(0, "NVITEM: write itemID %x fail, len %d", param->id, data_size);
            head.subtype = NVERR_NOT_EXIST;
        }
        else
        {
            head.subtype = NVERR_NONE;
        }
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    case NVITEM_GETLENGTH:
    {
        DIAG_CMD_PARAM(cmd, nvitemGetlength_t);

        int nvsize = nvmReadItem(param->id, NULL, 0);
        if (nvsize <= 0)
        {
            OSI_LOGD(0, "NVITEM: can't find itemID %x file", param->id);
            head.subtype = NVERR_NOT_EXIST;
            diagOutputPacket2(&head, NULL, 0);
        }
        else
        {
            nvitemGetlengthCnf_t cnf = {param->id, nvsize};
            head.subtype = NVERR_NONE;
            diagOutputPacket2(&head, &cnf, sizeof(cnf));
        }
        break;
    }
    case NVITEM_DELETE:
    {
        // DIAG_CMD_PARAM(cmd, nvitemDelete_t);
        // TODO
        head.subtype = NVERR_NONE;
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    case NVITEM_PRODUCT_CTRL_READ:
    {
        DIAG_CMD_PARAM(cmd, nvitemProductCtrlRead_t);

        phaseCheckHead_t phase_check;
        if (!nvmReadPhasecheck(&phase_check))
        {
            // We will always return OK here, and the magic word to make peer happy.
            memset(&phase_check, 0, sizeof(phaseCheckHead_t));
            *(uint32_t *)&phase_check = 0x53503039; // "SP09"
        }
        head.subtype = NVERR_NONE;
        diagOutputPacket3(&head, param, sizeof(nvitemProductCtrlRead_t), &phase_check, param->len);
        break;
    }
    case NVITEM_PRODUCT_CTRL_WRITE:
    {
        // DIAG_CMD_PARAM(cmd, nvitemProductCtrlWrite_t);
        // TODO: write NV
        head.subtype = NVERR_NONE;
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    case NVITEM_PRODUCT_CTRL_ERASE:
    {
        //We do nothing while receiving NVITEM_PRODUCT_CTRL_ERASE
        head.subtype = NVERR_NONE;
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    case NVITEM_USB_AUTO_MODE:
    {
        // DIAG_CMD_PARAM(cmd, nvitemUsbAutoMode_t);
        // TODO
        head.subtype = NVERR_NONE;
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    default:
        head.subtype = NVERR_SYSTEM;
        diagOutputPacket2(&head, NULL, 0);
        break;
    }
    return true;
}

static uint16_t _diagNvDirectRead(const diagMsgHead_t *cmd, void *buffer, uint16_t size)
{
    uint16_t nv_len = 0;
    uint16_t imei_len = 8;
    uint8_t cmd_mask = cmd->subtype & 0x7f;

    switch (cmd->type)
    {
    case DIAG_DIRECT_NV:
        nv_len = sizeof(refNVDirect_t);
        if (size < nv_len)
            return 0;
        memset(buffer, 0, nv_len);

        refNVDirect_t *nvInfo = (refNVDirect_t *)buffer;
        for (uint8_t i = 0; i < 7; ++i)
        {
            if (cmd_mask & (1 << i))
            {
                if (i == RM_CALI_NV_IMEI1)
                {
                    nvmReadItem(NVID_IMEI1, nvInfo->imei1, imei_len);
                }
                else if (i == RM_CALI_NV_IMEI2)
                {
                    nvmReadItem(NVID_IMEI2, nvInfo->imei2, imei_len);
                }
#ifdef CONFIG_BLUEU_BT_ENABLE
                else if (i == RM_CALI_NV_BT)
                {
                    bt_nv_param_t bt_nv_param;

                    if (nvmReadItem(NV_BT_CONFIG, (uint8 *)&bt_nv_param, sizeof(bt_nv_param)) < 0)
                        return false;
                    memcpy(nvInfo->bt_add.addr, bt_nv_param.bd_addr.addr, sizeof(btAddress_t));
                }
#endif
                // TODO else
            }
        }
        break;

    case DIAG_DIRECT_PHSCHK:
        nv_len = sizeof(phaseCheckHead_t);
        if (size < nv_len)
            return 0;
        memset(buffer, 0, nv_len);
        if (!nvmReadPhasecheck((phaseCheckHead_t *)buffer))
            memset(buffer, 0xff, sizeof(phaseCheckHead_t));
        break;

    case DIAG_DIRECT_RDVER:
    case DIAG_DIRECT_IMEINUM:
    default:
        break;
    }

    return nv_len;
}

static bool _diagNvDirectWrite(const diagMsgHead_t *cmd)
{
    uint8_t cmd_mask = cmd->subtype & 0x7f;
    uint16_t imei_len = 8;
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
                    if (nvmWriteItem(NVID_IMEI1, nv_info->imei1, imei_len) < 0)
                        return false;
                }
                else if (i == RM_CALI_NV_IMEI2)
                {
                    if (nvmWriteItem(NVID_IMEI2, nv_info->imei2, imei_len) < 0)
                        return false;
                }
#ifdef CONFIG_BLUEU_BT_ENABLE
                else if (i == RM_CALI_NV_BT)
                {
                    bt_nv_param_t bt_nv_param;

                    if (nvmReadItem(NV_BT_CONFIG, (uint8 *)&bt_nv_param, sizeof(bt_nv_param)) < 0)
                        return false;

                    memcpy((uint8 *)&bt_nv_param.bd_addr, &(nv_info->bt_add), sizeof(btAddress_t));

                    if (nvmWriteItem(NV_BT_CONFIG, (uint8 *)&bt_nv_param, sizeof(bt_nv_param)) < 0)
                        return false;
                }
#endif
                // TODO else
            }
        }
        break;

    case DIAG_DIRECT_PHSCHK:
        if (datalen != sizeof(phaseCheckHead_t))
            return false;
        if (nvmWritePhaseCheck((const phaseCheckHead_t *)diagCmdData(cmd)))
            return false;
        break;

    case DIAG_DIRECT_IMEINUM:
    case DIAG_DIRECT_RDVER:
    default:
        break;
    }

    return true;
}

static bool _diagNvDirectProcess(const diagMsgHead_t *cmd, void *ctx)
{
    diagProd_t *prod = (diagProd_t *)ctx;
    diagMsgHead_t head = *cmd;
    uint16_t crc_data = 0;
    uint16_t recv_crc = *(uint16_t *)((uint8_t *)cmd + cmd->len - sizeof(uint16_t));
    uint16_t crc = crc16NvCalc(diagCmdData(cmd), diagCmdDataSize(cmd) - sizeof(uint16_t));
    if (recv_crc != crc)
    {
        crc_data = DIR_CRC_ERR;
        head.len = sizeof(diagMsgHead_t) + sizeof(uint16_t);
        diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        return false;
    }

    uint8_t read_mode = cmd->subtype & RW_MASK;
    if (read_mode)
    {
        uint16_t result_len = _diagNvDirectRead(cmd, prod->trans_buf, prod->trans_buf_size);
        if (result_len)
        {
            head.len = sizeof(diagMsgHead_t) + result_len + sizeof(uint16_t);
            head.subtype = 1;
            crc_data = crc16NvCalc(prod->trans_buf, result_len);
            diagOutputPacket3(&head, prod->trans_buf, result_len, &crc_data, sizeof(uint16_t));
        }
        else
        {
            head.len = sizeof(diagMsgHead_t) + sizeof(uint16_t);
            crc_data = DIR_READ_ERR;
            diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        }
    }
    else
    {
        bool result = _diagNvDirectWrite(cmd);
        if (result)
        {
            head.len = sizeof(diagMsgHead_t) + sizeof(uint16_t);
            head.subtype = 1;
            crc_data = 0;
            diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        }
        else
        {
            head.len = sizeof(diagMsgHead_t) + sizeof(uint16_t);
            crc_data = DIR_SAVE_ERR;
            diagOutputPacket2(&head, &crc_data, sizeof(uint16_t));
        }
    }

    return true;
}

void diagNvmInit(void)
{
    static diagProd_t _prod;
    _prod.trans_buf_size = sizeof(_prod.trans_buf);

    diagRegisterCmdHandle(DIAG_NVITEM_F, _handleNvm, NULL);
    diagRegisterCmdHandle(DIAG_DIRECT_NV, _diagNvDirectProcess, &_prod);
    diagRegisterCmdHandle(DIAG_DIRECT_PHSCHK, _diagNvDirectProcess, &_prod);
}
