/******************************************************************************
 *  Function    - create clockface_table.c
 * 
 *  Purpose     -   将表盘做成表，为了后续项目扩展
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-10,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/

/******************************************************************************
 *  extern head file
 * 
 *
 ******************************************************************************/
#include "ic_widgets_inc.h"
#include "clockface_struct.h"


/******************************************************************************
 *  Global variable
 * 
 *
 ******************************************************************************/
const clockface_t clock_table[]=
{
	{	    
		.type =CLOCK_DIGITAL,
		.clock_num =0,
		.bg.type =CF_IMAGE_LV_IMG,
		.bg.img.img_src = &IMG_CLOCKFACE_DIGITAL1_BG,

		.digital.hour.pos1.x =59,//小时个位数横位置
		.digital.hour.pos1.y =3,
		.digital.hour.pos2.x =91,
		.digital.hour.pos2.y =3,
		.digital.minute.pos1.x =59,//分钟个位数横位置
		.digital.minute.pos1.y =51,
		.digital.minute.pos2.x =91,
		.digital.minute.pos2.y =51,

		.digital.slash.pos.x =27,//斜杠
		.digital.slash.pos.y =91,
		.digital.hour.img.type=CF_IMAGE_LV_IMG,
		.digital.minute.img.type =CF_IMAGE_LV_IMG,
		.digital.colon.img.type =CF_IMAGE_LV_IMG,

		.holder_num =5,
		.holders[0].type =CLOCKFACE_HOLDER_MONTH,
		.holders[0].pos.x =11,//月十位
		.holders[0].pos.y =91,
		.holders[1].type =CLOCKFACE_HOLDER_MONTH,
		.holders[1].pos.x =21,//月个位
		.holders[1].pos.y =91,
		.holders[2].type =CLOCKFACE_HOLDER_DAY,
		.holders[2].pos.x =33,//日期十位
		.holders[2].pos.y =91,
		.holders[3].type =CLOCKFACE_HOLDER_DAY,
		.holders[3].pos.x =42,//日期个位
		.holders[3].pos.y =91,
		.holders[4].type =CLOCKFACE_HOLDER_WEEK,//星期
		.holders[4].pos.x =17,
		.holders[4].pos.y =108,
		.draw_extra =NULL,
		.update_extra =NULL,
	},
	//第二个表盘
	{

		.type =CLOCK_DIGITAL,
		.clock_num =1,
		.bg.type =CF_IMAGE_LV_IMG,
		.bg.img.img_src =&IMG_CLOCKFACE_DIGITAL2_BG,

		.digital.hour.pos1.x =1,//小时个位数横位置
		.digital.hour.pos1.y =6,
		.digital.hour.pos2.x =26,
		.digital.hour.pos2.y =6,
		.digital.minute.pos1.x =0,//分钟个位数横位置
		.digital.minute.pos1.y =53,
		.digital.minute.pos2.x =25,
		.digital.minute.pos2.y =53,

		.digital.slash.pos.x =73,//斜杠
		.digital.slash.pos.y =9,
		.digital.hour.img.type=CF_IMAGE_LV_IMG,
		.digital.minute.img.type =CF_IMAGE_LV_IMG,
		.digital.colon.img.type =CF_IMAGE_LV_IMG,

		.holder_num =5,
		.holders[0].type =CLOCKFACE_HOLDER_MONTH,
		.holders[0].pos.x =59,//月十位
		.holders[0].pos.y =9,
		.holders[1].type =CLOCKFACE_HOLDER_MONTH,
		.holders[1].pos.x =68,//月个位
		.holders[1].pos.y =9,
		.holders[2].type =CLOCKFACE_HOLDER_DAY,
		.holders[2].pos.x =80,//日期十位
		.holders[2].pos.y =9,
		.holders[3].type =CLOCKFACE_HOLDER_DAY,
		.holders[3].pos.x =89,//日期个位
		.holders[3].pos.y =9,
		.holders[4].type =CLOCKFACE_HOLDER_WEEK,//星期
		.holders[4].pos.x =59,
		.holders[4].pos.y =22,
		.draw_extra =NULL,
		.update_extra =NULL
	}, 
	//第三个表盘	
	{
		.type =CLOCK_DIGITAL,
		.clock_num =2,
		.bg.type =CF_IMAGE_LV_IMG,
		.bg.img.img_src = &IMG_CLOCKFACE_DIGITAL3_BG,

		.digital.hour.pos1.x =26,//小时个位数横位置
		.digital.hour.pos1.y =46,
		.digital.hour.pos2.x =42,
		.digital.hour.pos2.y =46,
		.digital.minute.pos1.x =65,//分钟个位数横位置
		.digital.minute.pos1.y =46,
		.digital.minute.pos2.x =82,
		.digital.minute.pos2.y =46,
		.digital.slash.pos.x =50,//斜杠
		.digital.slash.pos.y =77,
		.digital.colon.pos.x =53,//冒号
		.digital.colon.pos.y =46,

		.digital.hour.img.type=CF_IMAGE_LV_IMG,
		.digital.minute.img.type =CF_IMAGE_LV_IMG,
		.digital.colon.img.type =CF_IMAGE_LV_IMG,
		.digital.slash.img.type =CF_IMAGE_LV_IMG,

		.holder_num =5,
		.holders[0].type =CLOCKFACE_HOLDER_MONTH,
		.holders[0].pos.x =37,//月十位
		.holders[0].pos.y =77,
		.holders[1].type =CLOCKFACE_HOLDER_MONTH,
		.holders[1].pos.x =45,//月个位
		.holders[1].pos.y =77,
		.holders[2].type =CLOCKFACE_HOLDER_DAY,
		.holders[2].pos.x =55,//日期十位
		.holders[2].pos.y =77,
		.holders[3].type =CLOCKFACE_HOLDER_DAY,
		.holders[3].pos.x =63,//日期个位
		.holders[3].pos.y =77,
		.holders[4].type =CLOCKFACE_HOLDER_WEEK,//星期
		.holders[4].pos.x =75,
		.holders[4].pos.y =77,
		.draw_extra =NULL,
		.update_extra =NULL
	}
};
