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

#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include "tts_config.h"
#ifdef CONFIG_TTS_SUPPORT

#include "at_cfw.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "ats_config.h"
#include "osi_log.h"
#include "cfw_chset.h"
#include "vfs.h"
#include "tts_player.h"
#include "ml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CTTS_STRING_ENCODE_TYPE_UTF8 (0)
#define CTTS_STRING_ENCODE_TYPE_GB2312 (1)
#define CTTS_TEXT_MAX_LEN (512)

void atCmdHandleCTTS(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        // STOP:    AT+CTTS=0
        // START:   AT+CTTS=1,<text>[,<encoding>]
        bool paramok = true;
        uint8_t mode = atParamUintInRange(cmd->params[0], 0, 1, &paramok);
        if (!paramok)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        if (mode == 0) // stop
        {
            if (cmd->param_count > 1)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            ttsStop();
            RETURN_OK(cmd->engine);
        }

        if (mode == 1) // start
        {
            const char *str = atParamStr(cmd->params[1], &paramok);
            unsigned encoding = atParamDefUintInRange(cmd->params[2], 0, 0, 1, &paramok);
            if (!paramok || cmd->param_count > 3)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            if (ttsIsPlaying())
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            unsigned len = strlen(str);
            if (len == 0 || len > CTTS_TEXT_MAX_LEN)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

            unsigned ml_encoding = ML_UTF8;
            if (encoding == CTTS_STRING_ENCODE_TYPE_GB2312)
                ml_encoding = ML_CP936;

            if (!ttsPlayText(str, len, ml_encoding))
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);

            RETURN_OK(cmd->engine);
        }

        // should be impossible, due to mode is already checked
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char rsp[48];
        sprintf(rsp, "%s: %d", cmd->desc->name, ttsIsPlaying());
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_TEST == cmd->type)
    {
        char rsp[48];
        sprintf(rsp, "%s: (0,1),<text>,(0,1)", cmd->desc->name);
        atCmdRespInfoText(cmd->engine, rsp);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#endif
