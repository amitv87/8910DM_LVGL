/**
 * @file clockface.h
 * @brief clockface descriptor for lvgl based project 
 * @author Wang Hua (whfyzg@gmail.com))
 * @version V2.0
 * @date 2020-11-13
 */

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#ifndef __CLOCKFACE_H__
#define __CLOCKFACE_H__

#define  CLOCKFACE_MAX_PATH_SIZE   (128)
#define  CLOCK_MAX_SHORTCUT         (4)

typedef enum
{
    CLOCK_ANALOG,   //模拟时钟
    CLOCK_DIGITAL,  //数字时钟
    CLOCK_BOTH,     //表盘上既有数字时间，又有模拟表针（ugly design）
    
    CLOCK_UNKOWN,  
}clock_type_enum;

typedef enum 
{
    CLOCK_COMP_TIME,   //时间组件
    CLOCK_COMP_DATE,   //日期组件
    CLOCK_COMP_WEEK,   //星期组件
    
    CLOCK_COMP_BG,      //背景

    CLOCK_COMP_STEP,   //步数组件
    CLOCK_COMP_DIST,   //距离组件
    CLOCK_COMP_CALORIE, //卡路里组件

    CLOCK_COMP_BATTERY,  //电量组件

    CLOCK_COMP_WEATHER,   //天气组件

    CLOCK_COMP_HEARTRATE,  //心率组件

    CLOCK_COMP_NUM
}clock_comp_enum;

typedef enum
{
    CLOCK_SHORTCUT_PEDOMETER,  //计步器
    CLOCK_SHORTCUT_SPORT,      //运动 
    CLOCK_SHORTCUT_FIT,        //锻炼 
    CLOCK_SHORTCUT_HEARTRATE,  //心率 
    CLOCK_SHORTCUT_DIALER,     //拨号盘 
    CLOCK_SHORTCUT_MUSIC,     
    CLOCK_SHORTCUT_SETTING,   
    CLOCK_SHORTCUT_SIRI,      
    CLOCK_SHORTCUT_SLEEPMONITOR,      
    CLOCK_SHORTCUT_WEATHER,      
    CLOCK_SHORTCUT_ALARM,      
}clock_shortcut_enum;

typedef enum
{
    CLOCK_IMAGE_TYPE_PNG,
    CLOCK_IMAGE_TYPE_GIF,
    CLOCK_IMAGE_TYPE_BMP,
    CLOCK_IMAGE_TYPE_NUM
}clock_image_enum;

typedef enum
{
    CLOCK_ALIGN_LEFT,   //左对齐
    CLOCK_ALIGN_RIGHT,  //右对齐
    CLOCK_ALIGN_CENTER,   //居中  
}clock_align_enum;


typedef struct 
{
    uint16_t x;
    uint16_t y;
}position_t;

typedef enum {
    CF_IMAGE_NONE = -1,
    CF_IMAGE_LV_IMG,           //lv内置图片
    CF_IMAGE_LV_IMG_SEQ,       //lv内置图片序列
    CF_IMAGE_PNG_RAW,          //内置PNG
    CF_IMAGE_PNG_SEQ,          //内置png序列
    CF_IMAGE_GIF_RAW,           //内置GIF

    CF_IMAGE_FS_START,
    CF_IMAGE_FS_IMG = CF_IMAGE_FS_START,          //lv 图片文件
    CF_IMAGE_FS_IMG_SEQ,      //lv 图片文件序列
    CF_IMAGE_FS_PNG,          //PNG 文件
    CF_IMAGE_FS_PNG_SEQ,      //PNG file seq
    CF_IMAGE_FS_GIF,          //GIF file
}clockface_img_enum;

typedef struct
{
    uint32_t rate;
    uint32_t frames;
    void* begin_img;
}clock_img_seq_t;

typedef struct {
    clockface_img_enum type;
    union {
        void* img_src;
        clock_img_seq_t seq;
    };
}lv_image_res_t;

typedef struct
{
    uint8_t exist;     //是否存在
    lv_image_res_t img;
}analog_handle_t;

typedef struct
{
    uint8_t has_position;     //是否指定旋转中心点坐标，默认显示屏中心点
    position_t center;        //中心点位置
    analog_handle_t hour;     //时针
    analog_handle_t minute;   //分针
    analog_handle_t second;   //秒针
    analog_handle_t nail;
}clock_element_analog_t;

//两位数的图片，比如数字时间的小时，分钟，月，日
typedef struct
{
   position_t pos1;       //个位数位置
   position_t pos2;       //十位数位置
   lv_image_res_t img;
   uint8_t name[16];
}clock_2_dital_t;

//多位数的图片，步数，卡路里等
typedef struct
{
   clock_align_enum     align;
   position_t           pos[5];    //五位十进制的数字位置,从低位数开始, pos[0]为个位数位置
   lv_image_res_t img;
   uint8_t name[16];
   uint8_t float_v;                 //是否是浮点数
}clock_dital_t;


typedef struct
{
   position_t pos;       //图片位置
   lv_image_res_t img;
}clock_image_t;

typedef struct
{
    uint8_t exist;
    position_t pos;
    uint8_t resolution;         //进度条分辨率
    lv_image_res_t img;
}clock_process_t;

//////////带值和进度调的组件
typedef struct
{
    uint8_t exist;
    clock_dital_t value;    //值（步数，卡路里，距离，电量等）
    clock_process_t process;  //进度条
}clock_com_target_t;

//////////带值组件
typedef struct
{
    uint8_t exist;
    clock_dital_t value;    //值
}clock_com_value_t;


typedef struct 
{
    uint8_t exist;
    lv_image_res_t img;
}weather_icon_t;

typedef struct 
{
    uint8_t exist;
    clock_dital_t value;

    position_t unit_pos;
    lv_image_res_t img;
}temperature_icon_t;

typedef struct 
{
    weather_icon_t weather;
    temperature_icon_t temp;
}clock_com_weather_t;



typedef struct
{
    clockface_img_enum type;
    position_t pos;
    union{
        lv_image_res_t img;
        clock_img_seq_t seq;
    };
}clock_background_t;


typedef struct
{
    clock_2_dital_t hour;
    clock_2_dital_t minute;
    
    clock_image_t colon;//冒号

    clock_image_t slash;//斜杠
}clock_element_digital_t;

typedef enum
{
    CLOCK_MONTH_TYPE_NUMBER,     //数字显示月份， 01 ，06， 11 等
    CLOCK_MONTH_TYPE_STRING,     //字符串显示月份，如sep，oct等
}clock_month_type_enum;

typedef struct
{
    clock_month_type_enum type;   //number or image
    union {
        clock_2_dital_t n_mon;
        clock_image_t   p_mon;
    }month;
}clock_month_t;

typedef enum {
    CLOCKFACE_DOMAIN_INTER,             //固件内置表盘，图片用ID
    CLOCKFACE_DOMAIN_ICC,               //ICC 表盘，从app推送过来 
    CLOCKFACE_DIMAIN_SELFEDITOR,        //自定义表盘，app编辑背景发送过来，并指定时间组件
}clockface_domain_enum;

typedef void (*draw_func)(void* obj);
#define CLOCKFACE_MAX_HOLDER_NUM      (8)

typedef enum {
    CLOCKFACE_HOLDER_NONE = -1,

    CLOCKFACE_HOLDER_IMG,

    CLOCKFACE_HOLDER_MONTH,
    CLOCKFACE_HOLDER_DAY,
    CLOCKFACE_HOLDER_WEEK,

    CLOCKFACE_HOLDER_STEP,
    CLOCKFACE_HOLDER_DIST,
    CLOCKFACE_HOLDER_CALORIE,
    CLOCKFACE_HOLDER_STAND,
    CLOCKFACE_HOLDER_WALK,
    CLOCKFACE_HOLDER_ACTIVITY,

    CLOCKFACE_HOLDER_PAI,
    CLOCKFACE_HOLDER_HR,
    CLOCKFACE_HOLDER_BP,
    CLOCKFACE_HOLDER_SPO,
    CLOCKFACE_HOLDER_ECG,
    CLOCKFACE_HOLDER_STRESS,
    CLOCKFACE_HOLDER_BREATH,
    CLOCKFACE_HOLDER_O2MAX,

    CLOCKFACE_HOLDER_CALENDOR,
    CLOCKFACE_HOLDER_ALARM,

    CLOCKFACE_HOLDER_BATTERY,
    CLOCKFACE_HOLDER_WEATHER,

    CLOCKFACE_HOLDER_SIRI,
    CLOCKFACE_HOLDER_DIALER,
    CLOCKFACE_HOLDER_SPORT,

    CLOCFACE_HOLDER_TYPE_NUM
}clockface_holder_type_enum;

typedef struct {
    clockface_holder_type_enum  type;
    bool                        clickable;                   //是否可以点击，如快捷方式是需要可以点击跳转的

    position_t                  pos;                         //挂件的左上角位置

    union {
        lv_image_res_t          img;
        clock_2_dital_t         day; //日
        clock_month_t           mon; //月
        clock_image_t           week;
        clock_com_target_t      step;   //计步器
        clock_com_target_t      dist;   //距离
        clock_com_target_t      calorie;   //卡路里
        clock_com_target_t      battery;   //电量
        clock_com_target_t      hr;     //版本2的心率对象
        clock_com_weather_t     weather;     //天气
    };

}clock_holder_t;

typedef struct
{
    clockface_domain_enum   domain;   //内部预设表盘，打包表盘，自定义表盘

    //clockface version
    uint8_t version;
    uint8_t clock_num;   //表盘顺序

    clock_type_enum type;     //表盘类型（模拟/数字）,数字加模拟

    //背景
    clock_background_t bg;

    //预览
    clock_image_t   preview;

    //时间，表针或者数字时间
    clock_element_analog_t analog;
    clock_element_digital_t digital;

    //挂件
    uint8_t holder_num;
    clock_holder_t holders[CLOCKFACE_MAX_HOLDER_NUM];


    //额外的未定义的信息回调， 可以在这个回调里面显示本结构体未定义的内容
    draw_func draw_extra;       //初始化
    draw_func update_extra;     //刷新
}clockface_t;
#endif
