#ifndef __IC_ANIMATION_H__
#define __IC_ANIMATION_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

#define    ANIM_OBJGRP_MAX_NUM     (5)
#define MAINSCREEN_PULL_FAST_TIME  (300)   //ms

typedef enum {
    TOUCH_MOVE_DIRECT_UP,
    TOUCH_MOVE_DIRECT_DOWN,
    TOUCH_MOVE_DIRECT_LEFT,
    TOUCH_MOVE_DIRECT_RIGHT,
}touch_direct_enum;

typedef enum {
    TOUCH_ANIM_ACTION_NONE,
    TOUCH_ANIM_ACTION_FORWARD,
    TOUCH_ANIM_ACTION_BACKWARD,
}touch_anim_action_enum;

typedef struct {
    lv_point_t              offset;
    touch_direct_enum       direct;
}obj_move_desc_t;

typedef struct {
    lv_obj_t* obj;
    lv_point_t origin_pos;
}anim_obj_t;

typedef struct {
    uint8_t num;
    anim_obj_t objs[ANIM_OBJGRP_MAX_NUM];
}obj_group_t;

typedef struct {
    touch_direct_enum           direct;
    touch_anim_action_enum      action;
    lv_point_t                  offset;
    uint16_t                    h_step;
    uint16_t                    v_step;
    uint16_t                    distance;    //move distance

    uint16_t                   interval;  //step inteval ms

    obj_group_t objgrp;

    void (*finish_cb)(void);

    uint8_t is_moving;

}obj_move_anim_t;

void ic_start_move_objs(obj_move_anim_t* anim);

#endif