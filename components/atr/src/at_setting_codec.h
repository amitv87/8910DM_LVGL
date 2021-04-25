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

#ifndef _AT_SETTING_CODEC_H_
#define _AT_SETTING_CODEC_H_

#include "at_cfg.h"

int atCfgTcpipEncode(const at_tcpip_setting_t *p, void *buf, size_t size);
bool atCfgTcpipDecode(at_tcpip_setting_t *p, const void *buf, size_t size);

int atCfgGlobalEncode(const atSetting_t *p, void *buf, size_t size);
bool atCfgGlobalDecode(atSetting_t *p, const void *buf, size_t size);

int atCfgProfileEncode(const atSetting_t *p, void *buf, size_t size);
bool atCfgProfileDecode(atSetting_t *p, const void *buf, size_t size);

int atCfgAutoSaveEncode(const atSetting_t *p, void *buf, size_t size);
bool atCfgAutoSaveDecode(atSetting_t *p, const void *buf, size_t size);

#endif
