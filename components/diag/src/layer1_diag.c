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

#include "diag.h"
#include "hal_chip.h"
#include "drv_md_nvm.h"
#include "osi_log.h"
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int16_t int16;
typedef int32_t int32;
typedef bool BOOLEAN;
typedef uint8_t LOOP_LIST_E;
#define PUBLIC
#define SIGNAL_VARS
#include "cmddef.h"
#include "calibration.h"
#include "nvm.h"

typedef struct
{
    uint16 PA_type;
    uint16 Transceiver_type;
    uint16 Crystal_type;
    uint16 RF_PARA_RESERVED[3];
} RF_SOLUTION_T;

typedef struct calibParam
{
    uint16_t is_nv;
    uint16_t band;
    uint16_t type;
    uint16_t index;
    uint16_t length;
} calibParam_t;

typedef struct diagToolCnf
{
    uint16_t parameter_type;
    uint16_t operation_status;
    uint16_t length;
} diagToolCnf_t;

static void _readCnf(const diagMsgHead_t *cmd,
                     uint16_t parameter_type,
                     uint16_t operation_status,
                     const void *read_data_addr,
                     uint16_t length)
{
    diagMsgHead_t head = *cmd;
    head.subtype = 1; // 1= read

    diagToolCnf_t cnf;
    cnf.parameter_type = parameter_type;
    cnf.operation_status = operation_status;
    cnf.length = length;

    diagOutputPacket3(&head, &cnf, sizeof(cnf), read_data_addr, length);
}

static void _writeCnf(const diagMsgHead_t *cmd,
                      uint16_t parameter_type,
                      uint16_t operation_status,
                      uint16_t length)
{
    diagMsgHead_t head = *cmd;
    head.subtype = 0; // 0= read

    diagToolCnf_t cnf;
    cnf.parameter_type = parameter_type;
    cnf.operation_status = operation_status;
    cnf.length = length;

    diagOutputPacket2(&head, &cnf, sizeof(cnf));
}

static void HandleImeiReadWrite(const diagMsgHead_t *cmd)
{
    const calibParam_t *param = (const calibParam_t *)diagCmdData(cmd);
    bool nv_operate_status = false;
    int len = 0;
    uint16_t imei_len = 8;

    if (1 == cmd->subtype) //1==read
    {
        uint8_t imei[8];
        if (param->type == CALI_IMEI)
        {
            nvmReadItem(NVID_IMEI1, imei, imei_len);
        }
        else if (param->type == CALI_IMEI1)
        {
            nvmReadItem(NVID_IMEI2, imei, imei_len);
        }

        if (len == imei_len)
            nv_operate_status = true;
        //send confirm to CM(TOOL)
        _readCnf(cmd, CALI_IMEI, nv_operate_status, imei, imei_len);
    }
    else //write
    {
        if (param->type == CALI_IMEI)
        {
            len = nvmWriteItem(NVID_IMEI1, (uint8_t *)(param + 1), imei_len);
        }
        else if (param->type == CALI_IMEI1)
        {
            len = nvmWriteItem(NVID_IMEI2, (uint8_t *)(param + 1), imei_len);
        }

        if (len == imei_len)
            nv_operate_status = true;
        //send confirm to CM(TOOL)
        _writeCnf(cmd, CALI_IMEI, nv_operate_status, imei_len);
    }
}

static void HandleReadChipId(const diagMsgHead_t *cmd)
{
    uint32_t chip_id = halGetChipId();
    _readCnf(cmd, CALI_CHIP_ID, true, &chip_id, sizeof(uint32_t));
}

static void HandleReadRfSolution(const diagMsgHead_t *cmd)
{
    RF_SOLUTION_T rf_solution = {};
    _readCnf(cmd, CALI_RF_SOLUTION, true, &rf_solution, sizeof(RF_SOLUTION_T));
}

static bool DiagHandleCalibration(const diagMsgHead_t *cmd, void *ctx)
{
    const calibParam_t *param = (const calibParam_t *)diagCmdData(cmd);

    OSI_LOGD(0, "calib is_nv/%d band/%d type/%d index/%d length/%d",
             param->is_nv, param->band, param->type, param->index, param->length);

    switch (param->type)
    {
    case CALI_IMEI:
    case CALI_IMEI1:
        HandleImeiReadWrite(cmd);
        break;

    case CALI_CHIP_ID:
        HandleReadChipId(cmd);

        break;
    case CALI_RF_SOLUTION:
        HandleReadRfSolution(cmd);
        break;

    case CALI_NOSINGAL_RXTXLOOP:
        return false;

    case CALI_FASTCALI_MODE:
        return false;

    default:              //for other parameters
        if (param->is_nv) //object is nv send to cp TODO
        {
            drvNvmSendCalibCmd(cmd);
        }
        else //object is ram
        {
            drvNvmSendCalibCmd(cmd);
        }
        break;
    }

    return true;
}

static bool DiagHandleLteCalibration(const diagMsgHead_t *cmd, void *ctx)
{
    // tool need to
    if (DIAG_LTE_RF == cmd->type)
        diagOutputPacket2(cmd, NULL, 0);

    drvNvmSendCalibCmd(cmd);
    return true;
}

void Layer1_RegCaliDiagHandle(void)
{
    diagRegisterCmdHandle(DIAG_CALIBRATION, DiagHandleCalibration, NULL);
    diagRegisterCmdHandle(DIAG_LTE_RF, DiagHandleLteCalibration, NULL);
    diagRegisterCmdHandle(DIAG_LTE_NV, DiagHandleLteCalibration, NULL);
}
