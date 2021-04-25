/** 
* @Time    :   2020/12/24 11:20:39
* @FileName:   clockface.c
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

#include "clockface.h"

static clockface_obj_t *clockface_ctx;


static void clockface_screen_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static uint32_t down_tick;
    static int8_t direct;
    static int8_t direct_x, direct_y;
    static obj_move_desc_t move_desc;
    static ic_obj_message_t message;
    message.sender = clockface_ctx->bg;

   // LOGI("clockface_screen_event_cb,obj:%x, event:%d\n", obj, event);

    switch (event) {
    case LV_EVENT_PRESSED:
        lv_indev_get_point(lv_indev_get_act(), &down_pos);
        LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
        last_pos = down_pos;
        direct = -1;
        down_tick = lv_tick_get();
        direct_x = -1;
        direct_y = -1;
        move_desc.offset.x = 0;
        move_desc.offset.y = 0;


        break;
    case LV_EVENT_PRESSING: {
        lv_indev_get_point(lv_indev_get_act(), &pos);
        LOGI("pressing(%d, %d)\n", pos.x, pos.y);

        int16_t offset_x = pos.x - last_pos.x;
        int16_t offset_y = pos.y - last_pos.y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;

        move_desc.offset.x += offset_x;
        move_desc.offset.y += offset_y;

        if (direct == -1) {
            if (abs(offset_x) > abs(offset_y) && abs(offset_x) > 5) {
                direct = 1;
            }
            else if (abs(offset_y) > abs(offset_x)) {
                if (offset_y > 5) {
                    direct = 2;
                }
                if (offset_y < -5) {
                    direct = 3;
                }
            }
        }

        last_pos = pos;
        message.data = (void*)&move_desc;

        if (direct == 1) {
            message.msg = IC_OBJ_MSG_GRP_MOVE;
            move_desc.direct = direct_x == 0 ? TOUCH_MOVE_DIRECT_RIGHT : TOUCH_MOVE_DIRECT_LEFT;
            lv_event_send(clockface_ctx->bg->parent, LV_EVENT_VALUE_CHANGED, &message);
        }

        if (direct > 1) {
            message.msg = IC_OBJ_MSG_GRP_MOVE;
            move_desc.direct = direct == 2 ? TOUCH_MOVE_DIRECT_DOWN : TOUCH_MOVE_DIRECT_UP;
            lv_event_send(clockface_ctx->bg->parent, LV_EVENT_VALUE_CHANGED, &message);
        }

        break;


    case LV_EVENT_SHORT_CLICKED:
        LOGI("short click\n");
        break;

    case LV_EVENT_CLICKED:
        LOGI("clicked\n");
       // static uint8_t flag = 0;
       // if (flag == 0) {
       //     lv_obj_fade_out(clockface_ctx->bg, 1000, 50);
       //     flag = 1;
      //  }
       // else {
       //     lv_obj_fade_in(clockface_ctx->bg, 1000, 50);
       //     flag = 0;
       // }
        break;

    case LV_EVENT_LONG_PRESSED:
        LOGI("LONG pressed\n");
        break;

    case LV_EVENT_LONG_PRESSED_REPEAT:
        LOGI("long pressed repeat\n");
        break;

    case LV_EVENT_PRESS_LOST:
        LOGI("press lost\n");
    case LV_EVENT_RELEASED: {
        lv_indev_get_point(lv_indev_get_act(), &pos);
        LOGI("release(%d, %d)\n", pos.x, pos.y);

        int16_t offset_x = pos.x - last_pos.x;
        int16_t offset_y = pos.y - last_pos.y;

        move_desc.offset.x += offset_x;
        move_desc.offset.y += offset_y;

        if (offset_x > 0) direct_x = 0;
        if (offset_x < 0) direct_x = 1;

        if (offset_y > 0) direct_y = 0;
        if (offset_y < 0) direct_y = 1;


        if (direct == 1) {
            move_desc.direct = direct_x == 0 ? TOUCH_MOVE_DIRECT_RIGHT : TOUCH_MOVE_DIRECT_LEFT;
            message.data = (void*)&move_desc;
            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
            }
            else {
                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
            }
            lv_event_send(clockface_ctx->bg->parent, LV_EVENT_VALUE_CHANGED, &message);
        }

        if (direct > 1) {
            move_desc.direct = direct == 2 ? TOUCH_MOVE_DIRECT_DOWN : TOUCH_MOVE_DIRECT_UP;
            message.data = (void*)&move_desc;
            if ((lv_tick_get() - down_tick < MAINSCREEN_PULL_FAST_TIME)) {
                message.msg = IC_OBJ_MSG_GRP_ANIM_FAST;
            }
            else {
                message.msg = IC_OBJ_MSG_GRP_ANIM_SLOW;
            }
            lv_event_send(clockface_ctx->bg->parent, LV_EVENT_VALUE_CHANGED, &message);
        }
    }
    break;

    case LV_EVENT_DRAG_BEGIN:
        LOGI("drage begin\n");
        break;

    case LV_EVENT_DRAG_END:
        LOGI("drag end\n");
        break;
    case LV_EVENT_DRAG_THROW_BEGIN:
        LOGI("drag throw begin\n");
        break;
    case LV_EVENT_GESTURE:
        LOGI("gesture\n");
        break;
    case LV_EVENT_KEY:
        LOGI("KEY\n");
        break;
    case LV_EVENT_FOCUSED:
        LOGI("FOCUSED\n");
        break;
    case LV_EVENT_DEFOCUSED:
        LOGI("DEFOCUSED\n");
        break;
    case LV_EVENT_LEAVE:
        LOGI("leave\n");
        break;
    case LV_EVENT_VALUE_CHANGED:
        LOGI("value changed\n");
        break;
    case LV_EVENT_INSERT:
        LOGI("insert\n");
        break;
    case LV_EVENT_REFRESH:
        LOGI("refresh\n");
        break;
    case LV_EVENT_APPLY:
        LOGI("apply\n");
        break;
    case LV_EVENT_CANCEL:
        LOGI("cancel\n");
        break;
    case LV_EVENT_DELETE:
        LOGI("delete\n");
        break;
    }
    }
}

/*****************************************************************************************************
 *
 *  create, draw and update a clock
 *
 *****************************************************************************************************/
static void clockface_get_analog_clock_angle(uint16_t *hour_hand_counter, 
        uint16_t *minute_hand_counter, uint16_t *second_hand_counter)
{
    ic_hal_rtc_t rtc;
    ic_hal_rtc_get_time(&rtc);
    uint8_t m = rtc.min;
    uint8_t s = rtc.sec;

    *minute_hand_counter = m;
    *second_hand_counter = s;

    uint8_t h = rtc.hour;

    h++;
    if (h > 12)
    {
        h -= 12;
    }
    *hour_hand_counter = (h - 1) * 5;
    *hour_hand_counter += m / 12;
    if (*minute_hand_counter >= 60)
    {
        *minute_hand_counter = 0;
    }
    if (*second_hand_counter >= 60)
    {
        *second_hand_counter = 0;
    }
    if ((*minute_hand_counter % 12) == 0)
    {
        if (*hour_hand_counter >= 60)
        {
            *hour_hand_counter = 0;
        }
    }

    *hour_hand_counter      *= 60;
    *minute_hand_counter    *= 60;
    *second_hand_counter    *= 60;

    /* LOGI("%02d:%02d:%02d, %d, %d, %d", rtc.hour, rtc.min, rtc.sec, */ 
    /*            *hour_hand_counter, */
    /*            *minute_hand_counter, */
    /*            *second_hand_counter */
    /*            ); */
}


void ic_clockface_create(clockface_obj_t* clockface, lv_obj_t* parent)
{
    //assert(parent);
    //assert(clockface);

    clockface_ctx = clockface;

#if 0
    lv_obj_t* bg = NULL;

    bg = lv_obj_create(parent, NULL);
    lv_obj_set_size(bg, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    static lv_style_t bg_style;
    lv_style_init(&bg_style);

    lv_style_set_bg_color(&bg_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&bg_style, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(bg, LV_OBJ_PART_MAIN, &bg_style);


    clockface->bg = bg;



    lv_obj_set_event_cb(clockface->bg, clockface_screen_event_cb);
#else
    //bg
    lv_obj_t* bg = NULL;
    if (clockface->desc->bg.type == CF_IMAGE_NONE) { //�ձ���
        bg = lv_obj_create(parent, NULL);
        lv_obj_set_size(bg, LV_HOR_RES_MAX, LV_VER_RES_MAX);
        
        
        //set obj bg color as black, remove boarder
        static lv_style_t bg_style;
        lv_style_init(&bg_style);
        lv_style_set_bg_color(&bg_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        //lv_style_set_border_color(&bg_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        lv_style_set_border_width(&bg_style, LV_STATE_DEFAULT, 0);
        //lv_style_set_border_opa(&bg_style, LV_STATE_DEFAULT, LV_OPA_100);
        //lv_style_set_border_side(&bg_style, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);

        lv_obj_add_style(bg, LV_OBJ_PART_MAIN, &bg_style);
    }
    else { //ͼƬ����
        bg = lv_img_create(parent, NULL);
        lv_obj_set_click(bg, true);
    }
    
    //assert(bg);
    clockface->bg = bg;
    lv_obj_set_pos(bg, clockface->desc->bg.pos.x, clockface->desc->bg.pos.y);

    if (clockface->desc->bg.type == CF_IMAGE_LV_IMG) {  //internal image
        lv_img_set_src(bg, clockface->desc->bg.img.img_src);
    }
    else if (clockface->desc->bg.type == CF_IMAGE_LV_IMG_SEQ) { // lv internal seq
        lv_img_set_src(bg, clockface->desc->bg.img.seq.begin_img);

        //start lv animator to show image sequence

    }else if (clockface->desc->bg.type == CF_IMAGE_PNG_RAW) { // internal png
        //invoke png decoder
    }
    else if (clockface->desc->bg.type == CF_IMAGE_PNG_SEQ) { // internal png seq
        //start lv animator to show png file sequence
    }
    else if (clockface->desc->bg.type == CF_IMAGE_GIF_RAW) { // internal gif
        //invoke gif decoder
    }
    else if (clockface->desc->bg.type == CF_IMAGE_FS_IMG) { // lv img file
        //show image from file lv_img_set_src(img, "S:folder1/my_img.bin")

    }
    else if (clockface->desc->bg.type == CF_IMAGE_FS_IMG_SEQ) { // lv img file seq
        //animator
    }
    else if (clockface->desc->bg.type == CF_IMAGE_FS_PNG) { // png file
        //decode png from file 
    }
    else if (clockface->desc->bg.type == CF_IMAGE_FS_PNG_SEQ) { // png file seq
        //start lv animator to decode png sequence from file
    }
    else if (clockface->desc->bg.type == CF_IMAGE_FS_GIF) { // gif file
        //decode gif from file
    }

    /**
    *  TIME section, create time component, include digital time and analog handles
    */
    if (clockface->desc->type == CLOCK_ANALOG || clockface->desc->type == CLOCK_BOTH) {  //analog time
        uint16_t hour_angle = 0, min_angle, sec_angle = 0;
        clockface_get_analog_clock_angle(&hour_angle, &min_angle, &sec_angle);

        lv_obj_t* hour = lv_img_create(bg, NULL);
        lv_obj_t* min = lv_img_create(bg, NULL);
        lv_obj_t* sec = NULL;
		lv_obj_t* hour_l = NULL;
		lv_obj_t* min_l = NULL;
		lv_obj_t* mon = NULL;
		lv_obj_t* mon_l = NULL;
		lv_obj_t* day = NULL;
		lv_obj_t* day_l = NULL;
		lv_obj_t* weekday = NULL;
		
        if (clockface->desc->analog.second.exist) {
            sec = lv_img_create(bg, NULL);
        }

        uint16_t center_x = LV_HOR_RES_MAX / 2;
        uint16_t center_y = LV_VER_RES_MAX / 2;

        clockface->hour = hour;
        clockface->min = min;
        clockface->sec = sec;
		clockface->hour_l = hour_l;
		clockface->min_l = min_l;
		clockface->mon = mon;
		clockface->mon_l = mon_l;
		clockface->day = day;
		clockface->day_l = day_l;
		clockface->weekday = weekday;

        //TODO: img src from fs, png raw file decoder
        if (clockface->desc->analog.hour.img.type == CF_IMAGE_LV_IMG) {
            lv_img_set_src(hour, clockface->desc->analog.hour.img.img_src);
            lv_img_set_src(min, clockface->desc->analog.minute.img.img_src);

            if (clockface->desc->analog.second.exist) {
                lv_img_set_src(sec, clockface->desc->analog.second.img.img_src);
            }
        }
        else if (clockface->desc->analog.hour.img.type == CF_IMAGE_FS_IMG) { //image in filssystem

        }

        //center point
        if (clockface->desc->analog.has_position) {
            center_x = clockface->desc->analog.center.x;
            center_y = clockface->desc->analog.center.y;
        }
       
         //hour
            uint16_t width = lv_obj_get_width(hour);
            uint16_t height = lv_obj_get_height(hour);
            lv_obj_set_pos(hour, center_x - width / 2, center_y - height / 2);

            //min
            width = lv_obj_get_width(min);
            height = lv_obj_get_height(min);
            lv_obj_set_pos(min, center_x - width / 2, center_y - height / 2);

            //sec
            if (clockface->desc->analog.second.exist) {
                width = lv_obj_get_width(sec);
                height = lv_obj_get_height(sec);
                lv_obj_set_pos(sec, center_x - width / 2, center_y - height / 2);
            }
       


        //these apis will cost too much cpu time, the performance is largely down      
        //this badly slowdown the obj moving, consider use canvs and just draw last snapshot
        //of this obj when it`s moving
        lv_img_set_angle(hour, hour_angle);
        lv_img_set_antialias(hour, true);

        lv_img_set_angle(min, min_angle);
        lv_img_set_antialias(min, true);

        if (clockface->desc->analog.second.exist) {
            lv_img_set_angle(sec, sec_angle);
            lv_img_set_antialias(sec, true);
        }

    }
    else if(clockface->desc->type == CLOCK_DIGITAL || clockface->desc->type == CLOCK_BOTH)
	{ 
		//digital time 
		lv_obj_t* hour_h = lv_img_create(bg, NULL);//小时十位
		lv_obj_t* hour_l = lv_img_create(bg, NULL);
        lv_obj_t* min_h = lv_img_create(bg, NULL);
		lv_obj_t* min_l = lv_img_create(bg, NULL);
		lv_obj_t* sec = NULL;

		clockface->hour = hour_h;
		clockface->hour_l = hour_l;
		clockface->min = min_h;
		clockface->min_l = min_l;
		clockface->sec = sec;
		
		//小时的显示
		if(clockface->desc->clock_num == 0)//第一个表盘
		{
			lv_obj_set_pos(hour_h, clockface->desc->digital.hour.pos1.x,clockface->desc->digital.hour.pos1.y);
			lv_img_set_src(hour_h, iclv_get_image_by_id(iclv_time_get_image_id(0,0)));
			lv_obj_set_pos(hour_l, clockface->desc->digital.hour.pos2.x,clockface->desc->digital.hour.pos2.y);
			lv_img_set_src(hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,0)));

			//分钟的显示
			lv_obj_set_pos(min_h, clockface->desc->digital.minute.pos1.x,clockface->desc->digital.minute.pos1.y);
			lv_img_set_src(min_h, iclv_get_image_by_id(iclv_time_get_image_id(2,0)));
			lv_obj_set_pos(min_l, clockface->desc->digital.minute.pos2.x,clockface->desc->digital.minute.pos2.y);
			lv_img_set_src(min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,0)));

			//斜杠
			lv_obj_t* slash = lv_img_create(bg, NULL);
			lv_obj_set_pos(slash, clockface->desc->digital.slash.pos.x,clockface->desc->digital.slash.pos.y);
			lv_img_set_src(slash, iclv_get_image_by_id(IMG_CLOCKFACE_DIGITAL1_SLASH_ID));
		}
		else if(clockface->desc->clock_num == 1)//第二个表盘
		{
			lv_obj_set_pos(hour_h, clockface->desc->digital.hour.pos1.x,clockface->desc->digital.hour.pos1.y);
			lv_img_set_src(hour_h, iclv_get_image_by_id(iclv_time_get_image_id(0,1)));
			lv_obj_set_pos(hour_l, clockface->desc->digital.hour.pos2.x,clockface->desc->digital.hour.pos2.y);
			lv_img_set_src(hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,1)));

			//分钟的显示
			lv_obj_set_pos(min_h, clockface->desc->digital.minute.pos1.x,clockface->desc->digital.minute.pos1.y);
			lv_img_set_src(min_h, iclv_get_image_by_id(iclv_time_get_image_id(2,1)));
			lv_obj_set_pos(min_l, clockface->desc->digital.minute.pos2.x,clockface->desc->digital.minute.pos2.y);
			lv_img_set_src(min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,1)));

			//斜杠
			lv_obj_t* slash = lv_img_create(bg, NULL);
			lv_obj_set_pos(slash, clockface->desc->digital.slash.pos.x,clockface->desc->digital.slash.pos.y);
			lv_img_set_src(slash, iclv_get_image_by_id(IMG_CLOCKFACE_DIGITAL1_SLASH_ID));
		}
		else //第三个表盘
		{
			lv_obj_set_pos(hour_h, clockface->desc->digital.hour.pos1.x,clockface->desc->digital.hour.pos1.y);
			lv_img_set_src(hour_h, iclv_get_image_by_id(iclv_time_get_image_id(0,2)));
			lv_obj_set_pos(hour_l, clockface->desc->digital.hour.pos2.x,clockface->desc->digital.hour.pos2.y);
			lv_img_set_src(hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,2)));

			//分钟的显示
			lv_obj_set_pos(min_h, clockface->desc->digital.minute.pos1.x,clockface->desc->digital.minute.pos1.y);
			lv_img_set_src(min_h, iclv_get_image_by_id(iclv_time_get_image_id(2,2)));
			lv_obj_set_pos(min_l, clockface->desc->digital.minute.pos2.x,clockface->desc->digital.minute.pos2.y);
			lv_img_set_src(min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,2)));

			//斜杠和冒号
			lv_obj_t* colon = lv_img_create(bg, NULL);//冒号
			lv_obj_t* slash = lv_img_create(bg, NULL);
			lv_obj_set_pos(colon, clockface->desc->digital.colon.pos.x,clockface->desc->digital.colon.pos.y);
			lv_img_set_src(colon, iclv_get_image_by_id(IMG_CLOCKFACE_DIGITAL3_COLON_ID));
			lv_obj_set_pos(slash, clockface->desc->digital.slash.pos.x,clockface->desc->digital.slash.pos.y);
			lv_img_set_src(slash, iclv_get_image_by_id(IMG_CLOCKFACE_DIGITAL1_SLASH_ID));
		}

			
    }

    /**
    *   HOLDERS: create holders component, include: date, week, pedometer info, weather, biosensor infos, activity info, shortcut....
    *           these component will be displayed according to holders member of clockface_t
    */
    int i;

    for (i = 0; i < clockface->desc->holder_num; i++) 
	{
        switch (clockface->desc->holders[i].type) 
		{
	        case CLOCKFACE_HOLDER_IMG:  //image or animation
	            break;
			
	        case CLOCKFACE_HOLDER_MONTH:
				if(clockface->desc->type == CLOCK_DIGITAL)
				{
					lv_obj_t* month_h = lv_img_create(bg, NULL);//月份十位
					lv_obj_t* month_l = lv_img_create(bg, NULL);

					clockface->mon = month_h;
					clockface->mon_l = month_l;
					if(clockface->desc->clock_num == 0)
					{
						lv_obj_set_pos(month_h, clockface->desc->holders[0].pos.x,clockface->desc->holders[0].pos.y);
						lv_img_set_src(month_h, iclv_get_image_by_id(iclv_time_get_image_id(4,0)));
						lv_obj_set_pos(month_l, clockface->desc->holders[1].pos.x,clockface->desc->holders[1].pos.y);
						lv_img_set_src(month_l, iclv_get_image_by_id(iclv_time_get_image_id(5,0)));
					}
					else if(clockface->desc->clock_num == 1)
					{
						lv_obj_set_pos(month_h, clockface->desc->holders[0].pos.x,clockface->desc->holders[0].pos.y);
						lv_img_set_src(month_h, iclv_get_image_by_id(iclv_time_get_image_id(4,1)));
						lv_obj_set_pos(month_l, clockface->desc->holders[1].pos.x,clockface->desc->holders[1].pos.y);
						lv_img_set_src(month_l, iclv_get_image_by_id(iclv_time_get_image_id(5,1)));
					}
					else
					{
						lv_obj_set_pos(month_h, clockface->desc->holders[0].pos.x,clockface->desc->holders[0].pos.y);
						lv_img_set_src(month_h, iclv_get_image_by_id(iclv_time_get_image_id(4,2)));
						lv_obj_set_pos(month_l, clockface->desc->holders[1].pos.x,clockface->desc->holders[1].pos.y);
						lv_img_set_src(month_l, iclv_get_image_by_id(iclv_time_get_image_id(5,2)));
					}

				}
	            break;
				
	        case CLOCKFACE_HOLDER_DAY:
				if(clockface->desc->type == CLOCK_DIGITAL)
				{
					lv_obj_t* day_h = lv_img_create(bg, NULL);//日期十位
					lv_obj_t* day_l = lv_img_create(bg, NULL);

					clockface->day = day_h;
					clockface->day_l = day_l;
					if(clockface->desc->clock_num == 0)
					{
						lv_obj_set_pos(day_h, clockface->desc->holders[2].pos.x,clockface->desc->holders[2].pos.y);
						lv_img_set_src(day_h, iclv_get_image_by_id(iclv_time_get_image_id(6,0)));
						lv_obj_set_pos(day_l, clockface->desc->holders[3].pos.x,clockface->desc->holders[3].pos.y);
						lv_img_set_src(day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,0)));
					}
					else if(clockface->desc->clock_num == 1)
					{
						lv_obj_set_pos(day_h, clockface->desc->holders[2].pos.x,clockface->desc->holders[2].pos.y);
						lv_img_set_src(day_h, iclv_get_image_by_id(iclv_time_get_image_id(6,1)));
						lv_obj_set_pos(day_l, clockface->desc->holders[3].pos.x,clockface->desc->holders[3].pos.y);
						lv_img_set_src(day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,1)));
					}
					else 
					{
						lv_obj_set_pos(day_h, clockface->desc->holders[2].pos.x,clockface->desc->holders[2].pos.y);
						lv_img_set_src(day_h, iclv_get_image_by_id(iclv_time_get_image_id(6,2)));
						lv_obj_set_pos(day_l, clockface->desc->holders[3].pos.x,clockface->desc->holders[3].pos.y);
						lv_img_set_src(day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,2)));
					}
				}
	            break;
		
	        case CLOCKFACE_HOLDER_WEEK:
				if(clockface->desc->type == CLOCK_DIGITAL)
				{
					lv_obj_t* weekday = lv_img_create(bg, NULL);//星期

					clockface->weekday = weekday;
					lv_obj_set_pos(weekday, clockface->desc->holders[4].pos.x,clockface->desc->holders[4].pos.y);
					if(clockface->desc->clock_num == 0)
					{
						lv_img_set_src(weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,0)));
					}
					else if(clockface->desc->clock_num == 1)
					{
						lv_img_set_src(weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,1)));
					}
					else 
					{
						lv_img_set_src(weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,2)));
					}	
				}
	            break;
	        
	        case CLOCKFACE_HOLDER_STEP:
	            break;
	        case CLOCKFACE_HOLDER_DIST:  //distance
	            break;
	        case CLOCKFACE_HOLDER_CALORIE:
	            break;
	        case CLOCKFACE_HOLDER_STAND:  //stand time
	            break;
	        case CLOCKFACE_HOLDER_WALK:  //walk time
	            break;
	        case CLOCKFACE_HOLDER_ACTIVITY:   //activity time
	            break;
	        
	        case CLOCKFACE_HOLDER_PAI:
	            break;
	        case CLOCKFACE_HOLDER_HR:
	            break;
	        case CLOCKFACE_HOLDER_BP:
	            break;
	        case CLOCKFACE_HOLDER_SPO:
	            break;
	        case CLOCKFACE_HOLDER_ECG:
	            break;
	        case CLOCKFACE_HOLDER_BREATH:
	            break;
	        case CLOCKFACE_HOLDER_O2MAX:
	            break;
	        case CLOCKFACE_HOLDER_STRESS:
	            break;

	        case CLOCKFACE_HOLDER_CALENDOR:
	            break;
	        case CLOCKFACE_HOLDER_ALARM:
	            break;
				
	        case CLOCKFACE_HOLDER_BATTERY:
				if(clockface->desc->type == CLOCK_DIGITAL)
				{
					lv_obj_t* battery = lv_img_create(bg, NULL);//星期

					clockface->battery = battery;
					lv_obj_set_pos(battery, clockface->desc->holders[5].pos.x,clockface->desc->holders[5].pos.y);
					lv_img_set_src(battery, iclv_get_image_by_id(IMG_IDLE_BATTERY0_ID));
				}
	            break;
				
	        case CLOCKFACE_HOLDER_WEATHER:
	            break;
	        case CLOCKFACE_HOLDER_SIRI:
	            break;
	        case CLOCKFACE_HOLDER_DIALER:
	            break;
	        case CLOCKFACE_HOLDER_SPORT:
	            break;

	        default:
            	LOGI("unknown holder type:%d", clockface->desc->holders[i].type); 
				break;
        }

    }

    //draw extra info
    if(clockface->desc->draw_extra)
        clockface->desc->draw_extra(bg);

	if (clockface->desc->type == CLOCK_ANALOG || clockface->desc->type == CLOCK_BOTH)
	{
		lv_obj_set_event_cb(clockface->bg, clockface_screen_event_cb);
		lv_obj_set_event_cb(clockface->hour, clockface_screen_event_cb);
		lv_obj_set_event_cb(clockface->min, clockface_screen_event_cb);
		if (clockface->desc->analog.second.exist) 
		{
			lv_obj_set_event_cb(clockface->sec, clockface_screen_event_cb);
		}
	}
	else if(clockface->desc->type == CLOCK_DIGITAL || clockface->desc->type == CLOCK_BOTH)
	{
		lv_obj_set_event_cb(clockface->bg, clockface_screen_event_cb);
	}


#endif
}

void ic_clockface_delete(clockface_obj_t *clock)
{
    //delete lv obj first
    lv_obj_del(clock->bg);

    //free clockface_obj_t
   // lv_mem_free(clock);
}

void ic_clockface_update(clockface_obj_t *clock)
{
#if 1
    //update time
    if (clock->desc->type == CLOCK_ANALOG || clock->desc->type == CLOCK_BOTH) 
	{  //analog time
        uint16_t hour_angle = 0, min_angle, sec_angle = 0;
        clockface_get_analog_clock_angle(&hour_angle, &min_angle, &sec_angle);
        lv_img_set_angle(clock->hour, hour_angle);
        lv_img_set_angle(clock->min, min_angle);
        if (clock->desc->analog.second.exist) 
		{
            lv_img_set_angle(clock->sec, sec_angle);
        }
    }
    else if (clock->desc->type == CLOCK_DIGITAL || clock->desc->type == CLOCK_BOTH)//digital time 
	{ 
		//小时的显示
		if(clock->desc->clock_num == 0)
		{
			lv_img_set_src(clock->hour, iclv_get_image_by_id(iclv_time_get_image_id(0,0)));
			lv_img_set_src(clock->hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,0)));

			//分钟的显示
			lv_img_set_src(clock->min, iclv_get_image_by_id(iclv_time_get_image_id(2,0)));
			lv_img_set_src(clock->min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,0)));
		}
		else if(clock->desc->clock_num == 1)
		{
			lv_img_set_src(clock->hour, iclv_get_image_by_id(iclv_time_get_image_id(0,1)));
			lv_img_set_src(clock->hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,1)));

			//分钟的显示
			lv_img_set_src(clock->min, iclv_get_image_by_id(iclv_time_get_image_id(2,1)));
			lv_img_set_src(clock->min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,1)));
		}
		else 
		{
			lv_img_set_src(clock->hour, iclv_get_image_by_id(iclv_time_get_image_id(0,2)));
			lv_img_set_src(clock->hour_l, iclv_get_image_by_id(iclv_time_get_image_id(1,2)));

			//分钟的显示
			lv_img_set_src(clock->min, iclv_get_image_by_id(iclv_time_get_image_id(2,2)));
			lv_img_set_src(clock->min_l, iclv_get_image_by_id(iclv_time_get_image_id(3,2)));
		}
    }

    //update holders
    int i;

    for (i = 0; i < clock->desc->holder_num; i++) {
        switch (clock->desc->holders[i].type) {
        case CLOCKFACE_HOLDER_IMG:  //image or animation
            break;
        case CLOCKFACE_HOLDER_MONTH:
			if(clock->desc->type == CLOCK_DIGITAL)
			{
				if(clock->desc->clock_num == 0)
				{
					lv_img_set_src(clock->mon, iclv_get_image_by_id(iclv_time_get_image_id(4,0)));
					lv_img_set_src(clock->mon_l, iclv_get_image_by_id(iclv_time_get_image_id(5,0)));
				}
				else if(clock->desc->clock_num == 1)
				{
					lv_img_set_src(clock->mon, iclv_get_image_by_id(iclv_time_get_image_id(4,1)));
					lv_img_set_src(clock->mon_l, iclv_get_image_by_id(iclv_time_get_image_id(5,1)));
				}
				else
				{
					lv_img_set_src(clock->mon, iclv_get_image_by_id(iclv_time_get_image_id(4,2)));
					lv_img_set_src(clock->mon_l, iclv_get_image_by_id(iclv_time_get_image_id(5,2)));
				}
			}
		
            break;
							
        case CLOCKFACE_HOLDER_DAY:
			if(clock->desc->type == CLOCK_DIGITAL)
			{
				if(clock->desc->clock_num == 0)
				{
					lv_img_set_src(clock->day, iclv_get_image_by_id(iclv_time_get_image_id(6,0)));
					lv_img_set_src(clock->day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,0)));
				}
				else if(clock->desc->clock_num == 1)
				{
					lv_img_set_src(clock->day, iclv_get_image_by_id(iclv_time_get_image_id(6,1)));
					lv_img_set_src(clock->day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,1)));
				}
				else 
				{
					lv_img_set_src(clock->day, iclv_get_image_by_id(iclv_time_get_image_id(6,2)));
					lv_img_set_src(clock->day_l, iclv_get_image_by_id(iclv_time_get_image_id(7,2)));
				}
			}
            break;

		
        case CLOCKFACE_HOLDER_WEEK:
			if(clock->desc->type == CLOCK_DIGITAL)
			{
				if(clock->desc->clock_num == 0)
				{
					lv_img_set_src(clock->weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,0)));
				}
				else if(clock->desc->clock_num == 1)
				{
					lv_img_set_src(clock->weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,1)));
				}
				else 
				{
					lv_img_set_src(clock->weekday, iclv_get_image_by_id(iclv_time_get_image_id(8,2)));
				}			
			}
            break;

        case CLOCKFACE_HOLDER_STEP:
            break;
        case CLOCKFACE_HOLDER_DIST:  //distance
            break;
        case CLOCKFACE_HOLDER_CALORIE:
            break;
        case CLOCKFACE_HOLDER_STAND:  //stand time
            break;
        case CLOCKFACE_HOLDER_WALK:  //walk time
            break;
        case CLOCKFACE_HOLDER_ACTIVITY:   //activity time
            break;

        case CLOCKFACE_HOLDER_PAI:
            break;
        case CLOCKFACE_HOLDER_HR:
            break;
        case CLOCKFACE_HOLDER_BP:
            break;
        case CLOCKFACE_HOLDER_SPO:
            break;
        case CLOCKFACE_HOLDER_ECG:
            break;
        case CLOCKFACE_HOLDER_BREATH:
            break;
        case CLOCKFACE_HOLDER_O2MAX:
            break;
        case CLOCKFACE_HOLDER_STRESS:
            break;

        case CLOCKFACE_HOLDER_CALENDOR:
            break;
        case CLOCKFACE_HOLDER_ALARM:
            break;
        case CLOCKFACE_HOLDER_BATTERY:
            break;
        case CLOCKFACE_HOLDER_WEATHER:
            break;
        case CLOCKFACE_HOLDER_SIRI:
            break;
        case CLOCKFACE_HOLDER_DIALER:
            break;
        case CLOCKFACE_HOLDER_SPORT:
            break;

        default:
            LOGI("unknown holder type:%d", clock->desc->holders[i].type);
			break;
        }

    }

    //draw extra info
    if (clock->desc->update_extra)
        clock->desc->update_extra(clock->bg);
#endif
}
