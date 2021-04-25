/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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
#ifdef CONFIG_AT_CISSDK_MIPL_SUPPORT

#ifndef __AT_CMD_MIPL_H__
#define __AT_CMD_MIPL_H__
#include "ats_config.h"
#include "at_engine.h"
#include "at_command.h"

void AT_CmdFunc_MIPLCREATE(atCommand_t *pParam);
void AT_CmdFunc_MIPLDELETE(atCommand_t *pParam);
void AT_CmdFunc_MIPLOPEN(atCommand_t *pParam);
void AT_CmdFunc_MIPLCLOSE(atCommand_t *pParam);
void AT_CmdFunc_MIPLADDOBJ(atCommand_t *pParam);
void AT_CmdFunc_MIPLDELOBJ(atCommand_t *pParam);
void AT_CmdFunc_MIPLNOTIFY(atCommand_t *pParam);
void AT_CmdFunc_MIPLREADRSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLWRITERSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLEXECUTERSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLOBSERVERSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLDISCOVERRSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLPARAMETERRSP(atCommand_t *pParam);
void AT_CmdFunc_MIPLUPDATE(atCommand_t *pParam);
void AT_CmdFunc_MIPLVER(atCommand_t *pParam);
void AT_CmdFunc_MIPLCLEARSTATE(atCommand_t *pParam);

//void AT_CmdFunc_MIPLNMI(atCommand_t *pParam);

#endif
#endif
