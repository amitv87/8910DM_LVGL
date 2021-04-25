/** 
* @Time    :   2020/12/24 11:16:21
* @FileName:   screen_manager.h
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: screen管理模块，功能包括创建screen，screen历史管理，关闭screen  
*/

#include "ic_widgets_inc.h"

#ifndef __SCREEN_MANAGER_H__
#define  __SCREEN_MANAGER_H__

#define MAX_SCREEN_HISTORY    (10)

typedef void (*pFUN_screen)(void *obj);

typedef struct {
    pFUN_screen  init;    //创建屏幕回调，需要在回调里面初始化data
    pFUN_screen  deinit;  //删除屏幕回调
    pFUN_screen  entry;   //进入屏幕回调
    pFUN_screen  exit;    //退出屏幕回调
}screen_callback_t;

typedef enum {
    SCREEN_INDEX_START = 1000,
    SCREEN_INDEX_IDLE,
    SCREEN_INDEX_MAINMENU,
    SCREEN_INDEX_HR,
    SCREEN_RECENT_CALLOG,//最近通话记录
    SCREEN_MISSED_CALLLOG,//未接电话
	SCREEN_ALARM_CLOCK,//闹钟
	SCREEN_ALARM_CLOCK_ADD,//闹钟添加
	SCREEN_ALARM_CLOCK_FRE,//闹钟频率
	SCREEN_SELECT_PHOBEBOOK_OR_DIAL,//选择电话簿或是拨号
	SCREEN_DIAL,//拨号界面
	SCREEN_CALLING,//正在拨号界面
	SCREEN_PB_LIST,//通讯录菜单
    SCREEN_INDEX_NUM
}screen_index_enum;

typedef struct {
    void *   data;
    screen_callback_t cb;
    screen_index_enum index;
}ic_screen_t;

struct screen_node{
    ic_screen_t screen;
    struct screen_node *next;
    struct screen_node *prev;
};

typedef struct screen_node screen_list_t;



bool ic_screen_create(screen_index_enum screen_inedx,
        screen_callback_t *cb);

extern bool ic_screen_goback(void);
extern bool ic_screen_close_active(void);
extern bool ic_screen_goto_idle(void);
extern bool ic_screen_entry(screen_index_enum screen_index);
#endif



