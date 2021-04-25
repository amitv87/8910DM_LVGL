#ifndef __MAINSCREEN_H__
#define __MAINSCREEN_H__
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

#define CLOCK_UPDATE_INTERVAL 1000

typedef struct {
    lv_obj_t* root;
    lv_obj_t* infocard;
    lv_obj_t* control_center;
    lv_obj_t* message_center;
    lv_obj_t* mainmenu;

    lv_obj_t* focused_obj;

    clockface_obj_t clock;

    lv_style_t style;

    lv_task_t* clockupdate_task;

    obj_move_anim_t anim_para;
}mainscreen_obj_t;





/**
	Infocard center
*/
lv_obj_t* ic_infocard_create(lv_obj_t* parent, lv_style_t* style);
void ic_infocard_delete(lv_obj_t* obj);
void ic_infocard_update(lv_obj_t* obj);

/**
   Message center
 */
lv_obj_t* ic_message_center_create(lv_obj_t* parent, lv_style_t* style);
void ic_message_center_delete(lv_obj_t* obj);
void ic_message_center_update(lv_obj_t* obj);

/**
  Control center
*/
lv_obj_t* ic_control_center_create(lv_obj_t* parent, lv_style_t* style);
void ic_control_center_delete(lv_obj_t* obj);
void ic_control_center_update(lv_obj_t* obj);

/**
 Mainmenu
*/
lv_obj_t* ic_mainmenu_create(lv_obj_t* parent, lv_style_t* style);
void ic_mainmenu_delete(lv_obj_t* obj);
void ic_mainmenu_update(lv_obj_t* obj);

#endif