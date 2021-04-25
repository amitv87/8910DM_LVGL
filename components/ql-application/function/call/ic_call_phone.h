/******************************************************************************
* @Time    :   2021/4/17 18:11:21
* @FileName:   ic_call_phone.h
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
******************************************************************************/


/******************************************************************************
 * include  head file
 * 
 *
 ******************************************************************************/

#ifndef __IC_CALL_PHONE_H__
#define __IC_CALL_PHONE_H__

#include "ic_widgets_inc.h"
#include "ql_api_rtc.h"

/******************************************************************************
 * defined struct
 * 
 *
 ******************************************************************************/

typedef struct
{
    ql_rtc_time_t start;     //开始时间
    ql_rtc_time_t end; 		 //结束时间
    char name[20]; 			 //姓名
    uint8_t number[20];      //电话
    uint8_t call_type;       //0:已接       1：未接
    uint8_t call_type2;   	 //0:来电       1：去电
    bool flag;               //使用标记
} ic_call_phone_str;

typedef struct List
{
	ic_call_phone_str data;
	struct List *next;
}call_list,*p_list;

/******************************************************************************
 * local function
 * 
 *
 ******************************************************************************/
void ic_voice_call_start(char* dial_num);
void ic_node_del(call_list **pNode,int place);

#endif

