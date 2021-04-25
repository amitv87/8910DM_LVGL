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

#include "ats_config.h"
#include "at_command.h"
#include "at_response.h"
#include "at_engine.h"
#include "osi_log.h"
#include "drv_names.h"
#include "drv_uart.h"
#include "drv_serial.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    uint32_t name_pre;
    bool (*create_channel)(uint32_t name);
} atAuxDevDef_t;

static const atAuxDevDef_t kAuxDevDefs[] = {
#ifdef CONFIG_SOC_8910
    {
        .name_pre = OSI_MAKE_TAG('A', 'C', 'M', 0),
        .create_channel = atEngineCreateUSerialChannel,
    },
    {
        .name_pre = OSI_MAKE_TAG('U', 'S', 'L', 0),
        .create_channel = atEngineCreateUSerialChannel,
    },
#endif
#ifdef CONFIG_SOC_8811
    {
        .name_pre = OSI_MAKE_TAG('U', 'R', 'T', 0),
        .create_channel = atEngineCreateUartChannel,
    },
#endif
};

void atCmdUCreateAtChannel(atCommand_t *cmd)
{
    if (cmd->type == AT_CMD_TEST)
    {
        atCmdRespInfoText(cmd->engine, "+UCREATEATCH: \"devname\"");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET)
    {
        bool paramok = true;
        const char *name_ = atParamStr(cmd->params[0], &paramok);
        if (name_ == NULL || strlen(name_) != 4)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_ERROR);

        const uint32_t name = *(const uint32_t *)name_;
        const atAuxDevDef_t *def = NULL;
        for (unsigned i = 0; i < OSI_ARRAY_SIZE(kAuxDevDefs); ++i)
        {
            if ((DRV_NAME_PREFIX_MASK & name) == kAuxDevDefs[i].name_pre)
            {
                def = &kAuxDevDefs[i];
                break;
            }
        }

        if (!def)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (!def->create_channel(name))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}