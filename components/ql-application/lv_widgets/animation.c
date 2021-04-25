/** 
* @Time    :   2020/12/24 11:16:21
* @FileName:   animation.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
*/

#include "ic_widgets_inc.h"

#include "animation.h"



//obj move task
static void iclv_obj_move_task(lv_task_t* task)
{
    obj_move_anim_t* anim = (obj_move_anim_t*)task->user_data;
    static lv_point_t offset = {0};
    int i = 0;

    //LOGI("iclv_obj_move_task, direct:%d, action:%d\n",
    //     anim->direct, anim->action);

    switch (anim->direct) {
    case TOUCH_MOVE_DIRECT_DOWN:
        if (offset.y == 0) {
            offset.y = anim->offset.y;
        }

        if (anim->action == TOUCH_ANIM_ACTION_BACKWARD) {
            offset.y -= anim->v_step;
            if (offset.y < 0) offset.y = 0;

            if (offset.y == 0) {                
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        else if (anim->action == TOUCH_ANIM_ACTION_FORWARD) {
            offset.y += anim->v_step;
            if (offset.y > anim->distance) offset.y = anim->distance;

            if (offset.y == anim->distance) {              
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }

        break;

    case TOUCH_MOVE_DIRECT_UP:
        if (offset.y == 0) {
            offset.y = anim->offset.y;
        }

        if (anim->action == TOUCH_ANIM_ACTION_BACKWARD) {
            offset.y += anim->v_step;
            if (offset.y > 0) offset.y = 0;

            if (offset.y == 0) {             
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        else if (anim->action == TOUCH_ANIM_ACTION_FORWARD) {
            offset.y -= anim->v_step;
            if (offset.y < -anim->distance) offset.y = -anim->distance;
            
            if (offset.y == -anim->distance) {             
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        break;

    case TOUCH_MOVE_DIRECT_LEFT:
        if (offset.x == 0) {
            offset.x = anim->offset.x;
        }

        if (anim->action == TOUCH_ANIM_ACTION_BACKWARD) {
            offset.x += anim->h_step;
            if (offset.x > 0) offset.x = 0;

            if (offset.x == 0) {
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        else if (anim->action == TOUCH_ANIM_ACTION_FORWARD) {
            offset.x -= anim->h_step;
            if (offset.x < -anim->distance) offset.x = -anim->distance;

            if (offset.x == -anim->distance) {             
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        break;

    case TOUCH_MOVE_DIRECT_RIGHT:
        if (offset.x == 0) {
            offset.x = anim->offset.x;
        }

        if (anim->action == TOUCH_ANIM_ACTION_BACKWARD) {
            offset.x -= anim->h_step;
            if (offset.x < 0) offset.x = 0;

            if (offset.x == 0) {                
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        else if (anim->action == TOUCH_ANIM_ACTION_FORWARD) {
            offset.x += anim->h_step;
            if (offset.x > anim->distance) offset.x = anim->distance;

            if (offset.x == anim->distance) {     
                lv_task_del(task);
                if(anim->finish_cb) anim->finish_cb();
            }
        }
        break;
    default:
        break;
    }

    LOGI("offset.x:%d, offset.y:%d\n", offset.x, offset.y);

    for (i = 0; i < anim->objgrp.num; i++) {
        lv_obj_set_pos(anim->objgrp.objs[i].obj, anim->objgrp.objs[i].origin_pos.x + offset.x, anim->objgrp.objs[i].origin_pos.y + offset.y);
    }

    if (offset.x == -anim->distance || offset.x == anim->distance) {
        offset.x = 0;
    }

    if (offset.y == -anim->distance || offset.y == anim->distance) {
        offset.y = 0;
    }
   
}

void ic_start_move_objs(obj_move_anim_t *anim)
{
    if (!anim) return;

    lv_task_t *_task = lv_task_create(iclv_obj_move_task, anim->interval, LV_TASK_PRIO_HIGH, anim);
    anim->is_moving = 1;
}

