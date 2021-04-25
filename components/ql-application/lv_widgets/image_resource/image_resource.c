
/** 
* @Time    :   2021/04/07 11:15:38
* @FileName:   image_resource.c
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: time image select
*/

#include "ic_widgets_inc.h"
#include "ql_api_rtc.h"
/******************************************************************************
 *  Function    -  iclv_time_get_image_id
 * 
 *  Purpose     -  返回图片ID
 * 
 *  Description -   TYPE   0:小时的十位        1:小时的个位
 *						   2:分钟的十位        3:分钟的个位
 *						   4:月份的十位        5:月份的个位
 *						   6:日期的十位        7:日期的个位
 *						   8:星期
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-08,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
image_id_enum iclv_time_get_image_id(uint8_t type,uint8_t num)
{
    ql_rtc_time_t tm = {0};

#if defined(PLATFORM_EC600)//模拟器不跑		
	ql_rtc_get_time(&tm);
#endif
	//LOGI("%d-%d-%d-%d",tm.tm_year,tm.tm_mon,tm.tm_mday,tm.tm_hour);
	
	tm.tm_hour += 8;

	if(type == 0)//小时的十位
	{
		uint8_t h = tm.tm_hour/10;

		if(h == 2)
		{	
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR2_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR2_ID;	
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR2_ID;	
			}
		}
		else if(h == 1)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR1_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR1_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR1_ID;
			}
		}
		else
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR0_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR0_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR0_ID;
			}
		}

	}
	else if(type == 1)//小时的个位
	{
		uint8_t h = tm.tm_hour%10;

		if(h == 0)
		{	
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR0_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR0_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR0_ID;
			}
		}
		else if(h == 1)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR1_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR1_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR1_ID;
			}
		}
		else if(h == 2)
		{	
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR2_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR2_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR2_ID;
			}
		}
		else if(h == 3)
		{	
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR3_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR3_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR3_ID;
			}
		}
		else if(h == 4)
		{		
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR4_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR4_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR4_ID;
			}
		}
		else if(h == 5)
		{	
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR5_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR5_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR5_ID;
			}
		}
		else if(h == 6)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR6_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR6_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR6_ID;
			}
		}
		else if(h == 7)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR7_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR7_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR7_ID;
			}
		}
		else if(h == 8)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR8_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR8_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR8_ID;
			}
		}
		else if(h == 9)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR9_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_HOUR9_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR9_ID;
			}
		}
	}
	else if(type == 2)//分钟的十位
	{
		uint8_t m = tm.tm_min/10;
		
		if(m == 0)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR0_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN0_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR0_ID;
			}
		}
		else if(m == 1)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR1_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN1_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR1_ID;
			}
		}
		else if(m == 2)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR2_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN2_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR2_ID;
			}
		}
		else if(m == 3)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR3_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN3_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR3_ID;
			}
		}
		else if(m == 4)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR4_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN4_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR4_ID;
			}
		}
		else if(m == 5)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR5_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN5_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR5_ID;
			}
		}
	}
	else if(type == 3)//分钟的个位
	{
		uint8_t m = tm.tm_min%10;
		
		if(m == 0)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR0_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN0_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR0_ID;
			}
		}
		else if(m == 1)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR1_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN1_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR1_ID;
			}
		}
		else if(m == 2)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR2_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN2_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR2_ID;
			}
		}
		else if(m == 3)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR3_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN3_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR3_ID;
			}
		}
		else if(m == 4)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR4_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN4_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR4_ID;
			}
		}
		else if(m == 5)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR5_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN5_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR5_ID;
			}
		}
		else if(m == 6)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR6_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN6_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR6_ID;
			}
		}
		else if(m == 7)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR7_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN7_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR7_ID;
			}
		}
		else if(m == 8)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR8_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN8_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR8_ID;
			}
		}
		else if(m == 9)
		{
			if(num == 0)
			{
				return IMG_CLOCKFACE_DIGITAL1_HOUR9_ID;
			}
			else if(num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL2_MIN9_ID;
			}
			else 
			{
				return IMG_CLOCKFACE_DIGITAL3_HOUR9_ID;
			}
		}
	}
	else if(type == 4)//月份十位
	{
		uint8_t mon = tm.tm_mon/10;

		if(mon == 0)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE0_ID;
		}
		else
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE1_ID;
		}
	}
	else if(type == 5)//月份个位
	{
		uint8_t mon = tm.tm_mon%10;

		if(mon == 0)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE0_ID;
		}
		else if(mon == 1)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE1_ID;
		}
		else if(mon == 2)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE2_ID;
		}
		else if(mon == 3)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE3_ID;
		}
		else if(mon == 4)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE4_ID;
		}
		else if(mon == 5)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE5_ID;
		}
		else if(mon == 6)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE6_ID;
		}
		else if(mon == 7)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE7_ID;
		}
		else if(mon == 8)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE8_ID;
		}
		else
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE9_ID;
		}
	}
	else if(type == 6)//日期十位
	{
		uint8_t day = tm.tm_mday/10;

		if(day == 0)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE0_ID;
		}
		else if(day == 1)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE1_ID;
		}
		else if(day == 2)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE2_ID;
		}
		else 
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE3_ID;
		}		
	}
	else if(type == 7)//日期个位
	{
		uint8_t day = tm.tm_mday%10;

		if(day == 0)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE0_ID;
		}
		else if(day == 1)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE1_ID;
		}
		else if(day == 2)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE2_ID;
		}
		else if(day == 3)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE3_ID;
		}
		else if(day == 4)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE4_ID;
		}
		else if(day == 5)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE5_ID;
		}
		else if(day == 6)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE6_ID;
		}
		else if(day == 7)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE7_ID;
		}
		else if(day == 8)
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE8_ID;
		}
		else 
		{
			return IMG_CLOCKFACE_DIGITAL1_DATE9_ID;
		}
	}
	else if(type == 8)//星期
	{
		uint8_t weekday = tm.tm_wday;

		if(weekday == 0)
		{
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_SUN_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_SUN_ID;
			}
		}
		else if(weekday == 1)
		{
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_MON_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_MON_ID;
			}
		}
		else if(weekday == 2)
		{	
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_TUE_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_TUE_ID;
			}
		}
		else if(weekday == 3)
		{	
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_WED_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_WED_ID;
			}
		}
		else if(weekday == 4)
		{
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_THU_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_THU_ID;
			}
		}
		else if(weekday == 5)
		{
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_FRI_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_FRI_ID;
			}
		}
		else
		{
			if(num == 0 || num == 1)
			{
				return IMG_CLOCKFACE_DIGITAL1_SAT_ID;
			}
			else
			{
				return IMG_CLOCKFACE_DIGITAL3_SAT_ID;
			}
		}
	}

	//LOGI("111111111 = %d", rtc.week);
}

