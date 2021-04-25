/** 
* @Time    :   2020/12/24 11:20:01
* @FileName:   title_bar.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton:  
*/
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"



lv_obj_t *ic_watch_create_title_bar(lv_obj_t* parent, screen_title_bar_desc_t* desc)
{
    lv_obj_t* bar = lv_obj_create(parent, NULL);
    lv_obj_t* title =  lv_label_create(bar, NULL);
    lv_obj_t* time = lv_label_create(bar, NULL);

   // lv_obj_move_foreground(bar);

    lv_obj_add_style(bar, LV_OBJ_PART_MAIN, desc->title_style);

    lv_obj_set_pos(bar, TITLE_BAR_BORDER_WIDTH, 0);
    lv_obj_set_size(bar, LV_HOR_RES_MAX-2 * TITLE_BAR_BORDER_WIDTH, TITLE_BAR_HEIGHT);

    lv_label_set_text(title, desc->title);
    lv_label_set_align(title, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_pos(title, 0, 0);


    lv_label_set_text(time, _("16:25"));
    lv_label_set_align(time, LV_LABEL_ALIGN_RIGHT);
    lv_obj_align(time, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    return bar;
}
