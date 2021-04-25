/** 
* @Time    :   2020/12/24 11:19:25
* @FileName:   mainmenu.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: applewatch5 style mainmenu 
*/
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#include "ic_widgets_inc.h"

typedef enum
{
    ISTYPE_MODE1,
    ISTYPE_MODE2,

    ISTYLE_MODE_NUM,
}istyle_mode_t;

#define STYLE1_RADUIS_L  (42)
#define STYLE1_RADUIS_M  (37)
#define STYLE1_RADUIS_S  (32)

#define ITEM_GAP         (2)
#define COS60VALUE           
#define SIN60VALUE           


/**************************************************
 *  
 *              1     1   
 *        1   8    9    10   1
 *        11     6     4     12 
 *        1   3    1    2   1
 *        13     7     5     14 
 *        1   15    16    17   1
 *              1     1   
 *
 ************************************************/
#define  AREA1_X1       (LV_HOR_RES_MAX/2-STYLE1_RADUIS_L)
#define  AREA1_Y1       (LV_VER_RES_MAX/2-STYLE1_RADUIS_L)
#define  AREA2_X1       (LV_HOR_RES_MAX/2+STYLE1_RADUIS_L+ITEM_GAP) 
static lv_area_t item_area_table[] = 
{
    //one large area   (1)
    { 
        AREA1_X1,
        AREA1_Y1,
        LV_HOR_RES_MAX/2+STYLE1_RADUIS_L, 
        LV_VER_RES_MAX/2+STYLE1_RADUIS_L
    },

    //six meduim area
    //(2)
    { 
        AREA2_X1, 
        LV_VER_RES_MAX/2-STYLE1_RADUIS_M, 
        AREA2_X1+STYLE1_RADUIS_M, 
        LV_VER_RES_MAX/2+STYLE1_RADUIS_M
    },
    //(3)
    { 
        LV_HOR_RES_MAX/2-STYLE1_RADUIS_L-ITEM_GAP-2*STYLE1_RADUIS_M, 
        LV_VER_RES_MAX/2-STYLE1_RADUIS_M, 
        LV_HOR_RES_MAX/2-STYLE1_RADUIS_L-ITEM_GAP, 
        LV_VER_RES_MAX/2+STYLE1_RADUIS_M, 
    },
    //(4)
    { 
        LV_HOR_RES_MAX/2+(AREA2_X1)*COS60VALUE-STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2-(AREA2_X1)*SIN60VALUE-STYLE1_RADUIS_M,
        LV_HOR_RES_MAX/2+(AREA2_X1)*COS60VALUE+STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2-(AREA2_X1)*SIN60VALUE+STYLE1_RADUIS_M,
    },
    //(5)
    { 
        LV_HOR_RES_MAX/2+(AREA2_X1)*COS60VALUE-STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2+(AREA2_X1)*SIN60VALUE-STYLE1_RADUIS_M,
        LV_HOR_RES_MAX/2+(AREA2_X1)*COS60VALUE+STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2+(AREA2_X1)*SIN60VALUE+STYLE1_RADUIS_M,
    },
    //(6)
    { 
        LV_HOR_RES_MAX/2-(AREA2_X1)*COS60VALUE-STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2-(AREA2_X1)*SIN60VALUE-STYLE1_RADUIS_M,
        LV_HOR_RES_MAX/2-(AREA2_X1)*COS60VALUE+STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2-(AREA2_X1)*SIN60VALUE+STYLE1_RADUIS_M,
    },
    //(7)
    { 
        LV_HOR_RES_MAX/2-(AREA2_X1)*COS60VALUE-STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2+(AREA2_X1)*SIN60VALUE-STYLE1_RADUIS_M,
        LV_HOR_RES_MAX/2-(AREA2_X1)*COS60VALUE+STYLE1_RADUIS_M,
        LV_VER_RES_MAX/2+(AREA2_X1)*SIN60VALUE+STYLE1_RADUIS_M,
    },

    //10 small area
    //(9)
    {
    },
};
