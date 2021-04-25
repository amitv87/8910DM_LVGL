#ifndef __TITLE_BAR_H__
#define __TITLE_BAR_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"
#include "title_bar.h"

/**
 Title Bar
  | Title         Time |
 */
#define  TITLE_BAR_HEIGHT                 (20)
#define  TITLE_BAR_BORDER_WIDTH            (10)

typedef struct {
    uint8_t* title;
    lv_style_t* title_style;

}screen_title_bar_desc_t;

lv_obj_t *ic_watch_create_title_bar(lv_obj_t* parent, screen_title_bar_desc_t* desc);

#endif