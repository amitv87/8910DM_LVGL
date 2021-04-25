#ifndef    _FW_UPDATE_H_
#define    _FW_UPDATE_H_
#include "bl_chip_common.h"

extern int bl_get_chip_id(unsigned char *buf);
#if defined(BL_AUTO_UPDATE_FARMWARE)
extern int bl_auto_update_fw(void);
#endif

#endif



