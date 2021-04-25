/** 
* @Time    :   2020/12/24 11:15:38
* @FileName:   iclv_font.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: font management module
*/
#include "ic_widgets_inc.h"

typedef struct {
   char *locale;
   lv_font_t * fonts[IC_FONT_NUM];
}ic_locale_font_t;


//font declauration
LV_FONT_DECLARE(opposans_14);

static const ic_locale_font_t ic_font_list[] = 
{
    //default language
    { 
        "en-US",
        {
            &opposans_14,         //IC_FONT_XXLARGE
            &opposans_14,         //IC_FONT_XLARGE
            &opposans_14,         //IC_FONT_LARGE
            &opposans_14,         //IC_FONT_MEDIUM
            &opposans_14,         //IC_FONT_SMALL
        },
    },

#if defined(IC_LANGUAGE_CHINESE_SM)
    {
        "zh-CN",
        {
            &opposans_14,         //IC_FONT_XXLARGE
            &opposans_14,         //IC_FONT_XLARGE
            &opposans_14,         //IC_FONT_LARGE
            &opposans_14,         //IC_FONT_MEDIUM
            &opposans_14,         //IC_FONT_SMALL
        },
    },
#endif
};

lv_font_t * ic_font_get(ic_font_enum id)
{
    char *locale =  lv_i18n_get_current_locale();
    int i =0;

    for(i = 0; i < sizeof(ic_font_list)/sizeof(ic_locale_font_t); i++){
        if(strcmp(locale, ic_font_list[i].locale) == 0){
            return ic_font_list[i].fonts[id];
        }
    }

    return ic_font_list[0].fonts[id];
}
