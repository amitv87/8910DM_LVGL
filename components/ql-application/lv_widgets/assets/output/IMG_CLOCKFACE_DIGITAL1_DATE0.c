#if defined(PLATFORM_EC600)
    #include "lvgl.h"
    #else
    #include "lvgl/lvgl.h"
    #endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL1_DATE0
#define LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL1_DATE0
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL1_DATE0 uint8_t IMG_CLOCKFACE_DIGITAL1_DATE0_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Blue: 2 bit, Green: 3 bit, Red: 3 bit, Alpha 8 bit */
  0x00, 0x00, 0x00, 0x00, 0xff, 0x67, 0xff, 0xcc, 0xff, 0xcc, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0x54, 0xff, 0xff, 0xff, 0xef, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x98, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xab, 0xff, 0xff, 0xff, 0x54, 0x25, 0x00, 0xff, 0xdc, 0xff, 0xff, 0xff, 0x23, 
  0x00, 0x00, 0xff, 0xdc, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0xff, 0x98, 0xff, 0xff, 0xff, 0x54, 
  0xff, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x25, 0x00, 0xff, 0x88, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0xff, 0xef, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0xff, 0x88, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0xff, 0xbb, 0xff, 0xff, 0xff, 0x33, 0x25, 0x00, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x33, 
  0x00, 0x00, 0xff, 0x77, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xab, 0xff, 0xff, 0xff, 0xbb, 0x25, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0x98, 0xff, 0xef, 0xff, 0xff, 0xff, 0xbb, 0xff, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x10, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Blue: 5 bit, Green: 6 bit, Red: 5 bit, Alpha 8 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x67, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xcc, 0xbe, 0xff, 0x88, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x98, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 0x21, 0x08, 0x00, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x98, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0xff, 0xff, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0x01, 0x08, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x98, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format:  Blue: 5 bit Green: 6 bit, Red: 5 bit, Alpha 8 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x67, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xcc, 0xff, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x98, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 0x08, 0x21, 0x00, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x98, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0xff, 0xff, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0x08, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x98, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0x08, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format:  Blue: 8 bit, Green: 8 bit, Red: 8 bit, Alpha: 8 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x67, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xcc, 0xfc, 0xfe, 0xfc, 0x88, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x98, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x98, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0xff, 0xff, 0xff, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x54, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x98, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
};

const lv_img_dsc_t IMG_CLOCKFACE_DIGITAL1_DATE0 = {
  .header.always_zero = 0,
  .header.w = 8,
  .header.h = 10,
  .data_size = 80 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = IMG_CLOCKFACE_DIGITAL1_DATE0_map,
};
