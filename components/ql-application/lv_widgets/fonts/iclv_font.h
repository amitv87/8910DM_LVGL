#ifndef __ICLV_FONT_H__
#define __ICLV_FONT_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"


#include "ic_widgets_inc.h"

typedef enum{
    IC_FONT_XXLARGE,
    IC_FONT_XLARGE,
    IC_FONT_LARGE,
    IC_FONT_MEDUIM,
    IC_FONT_SMALL,

    IC_FONT_NUM
}ic_font_enum;

extern lv_font_t * ic_font_get(ic_font_enum id);
#endif
