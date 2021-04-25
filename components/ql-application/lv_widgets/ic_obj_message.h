#ifndef __IC_OBJ_MESSAGE_H__
#define __IC_OBJ_MESSAGE_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

typedef enum {
	IC_OBJ_MSG_GRP_MOVE,
	IC_OBJ_MSG_GRP_ANIM_FAST,
	IC_OBJ_MSG_GRP_ANIM_SLOW,

	IC_OBJ_MSG_GRP_MOVE_X,
	IC_OBJ_MSG_GRP_MOVE_Y,
	IC_OBJ_MSG_GRP_ANIM_FAST_X,
	IC_OBJ_MSG_GRP_ANIM_FAST_Y,
	IC_OBJ_MSG_GRP_ANIM_SLOW_X,
	IC_OBJ_MSG_GRP_ANIM_SLOW_Y,
}ic_obj_message_enum;

typedef struct {
	lv_obj_t* sender;

	ic_obj_message_enum   msg;

	void* data;
}ic_obj_message_t;

#endif