#ifndef __CLCOKFACE_H__
#define __CLCOKFACE_H__

#ifdef PLATFORM_EC600
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "clockface_struct.h"

typedef struct
{
    uint32_t total_ms;

    lv_obj_t *bg;
    lv_obj_t *hour;
	lv_obj_t *hour_l;
    lv_obj_t *min;
	lv_obj_t *min_l;
    lv_obj_t *sec;
	lv_obj_t *mon;
	lv_obj_t *mon_l;
	lv_obj_t *day;
	lv_obj_t *day_l;
	lv_obj_t *weekday;
	lv_obj_t *battery;
    clockface_t *desc;
}clockface_obj_t;

extern void ic_clockface_create(clockface_obj_t* clockface, lv_obj_t* parent);
extern void ic_clockface_delete(clockface_obj_t *clock);
extern void ic_clockface_update(clockface_obj_t *clock);

#endif

