#if defined(PLATFORM_EC600)
    #include "lvgl.h"
    #else
    #include "lvgl/lvgl.h"
    #endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL3_HOUR5
#define LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL3_HOUR5
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_IMG_IMG_CLOCKFACE_DIGITAL3_HOUR5 uint8_t IMG_CLOCKFACE_DIGITAL3_HOUR5_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Blue: 2 bit, Green: 3 bit, Red: 3 bit, Alpha 8 bit */
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x23, 0x1f, 0x44, 0x1f, 0x23, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xef, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0x54, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x10, 0x1f, 0x44, 0x1f, 0x44, 0x1f, 0x10, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0x54, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xbb, 0x1f, 0x98, 0x1f, 0x77, 0x1f, 0x67, 0x1f, 0x44, 0x1f, 0x44, 0x1f, 0x23, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xef, 0x1f, 0x88, 0x1f, 0x44, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xbb, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xbb, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xbb, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x1f, 0x10, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0x98, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1b, 0x00, 0x1f, 0x44, 0x1b, 0xff, 0x1f, 0xff, 0x1b, 0xff, 0x1f, 0xff, 0x1b, 0xff, 0x1f, 0xff, 0x1b, 0xff, 0x1f, 0xff, 0x1b, 0xef, 0x1f, 0xab, 0x1b, 0x44, 0x1f, 0x00, 0x1b, 0x00, 0x1f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1b, 0x00, 0x1b, 0x44, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0x88, 0x1b, 0x00, 0x1b, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1b, 0x00, 0x1b, 0x23, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0x77, 0x1b, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1b, 0x00, 0x1b, 0x00, 0x1b, 0x77, 0x1b, 0xbb, 0x1b, 0x98, 0x1b, 0x77, 0x1b, 0x44, 0x1b, 0x44, 0x1b, 0x44, 0x1b, 0x77, 0x1b, 0xdc, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xff, 0x1b, 0xef, 0x1b, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x10, 0x17, 0xcc, 0x17, 0xff, 0x17, 0xff, 0x17, 0xff, 0x17, 0x67, 0x00, 0x00, 
  0x00, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x54, 0x17, 0xff, 0x17, 0xff, 0x17, 0xff, 0x17, 0x88, 0x00, 0x00, 
  0x00, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x00, 0x17, 0x10, 0x17, 0xff, 0x17, 0xff, 0x17, 0xff, 0x17, 0xbb, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x10, 0x13, 0xff, 0x17, 0xff, 0x13, 0xff, 0x17, 0xbb, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x00, 0x13, 0x10, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x00, 0x17, 0x00, 0x13, 0x54, 0x17, 0xff, 0x13, 0xff, 0x17, 0xff, 0x13, 0xab, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x00, 0x13, 0xef, 0x13, 0x67, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0xbb, 0x13, 0xff, 0x13, 0xff, 0x13, 0xff, 0x13, 0x77, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x33, 0x13, 0xff, 0x13, 0xff, 0x13, 0xbb, 0x13, 0x67, 0x13, 0x10, 0x13, 0x00, 0x13, 0x00, 0x13, 0x00, 0x13, 0x23, 0x13, 0xab, 0x13, 0xff, 0x13, 0xff, 0x13, 0xff, 0x13, 0xff, 0x13, 0x33, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x67, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xbb, 0x0f, 0xbb, 0x13, 0xcc, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xbb, 0x0f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x13, 0x98, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xef, 0x13, 0x10, 0x0f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0x00, 0x13, 0x77, 0x0f, 0xef, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xbb, 0x13, 0x23, 0x0f, 0x00, 0x13, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x13, 0x10, 0x0f, 0x67, 0x13, 0xab, 0x0f, 0xef, 0x13, 0xff, 0x0f, 0xff, 0x13, 0xff, 0x0f, 0xdc, 0x13, 0x98, 0x0f, 0x44, 0x13, 0x00, 0x0f, 0x00, 0x13, 0x00, 0x0f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Blue: 5 bit, Green: 6 bit, Red: 5 bit, Alpha 8 bit*/
  0x00, 0x00, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x23, 0xff, 0x07, 0x44, 0xff, 0x07, 0x23, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x07, 0x00, 0xfe, 0x07, 0x00, 0xff, 0x07, 0xef, 0xfe, 0x07, 0xff, 0xff, 0x07, 0xff, 0xfe, 0x07, 0x54, 0xff, 0x07, 0x00, 0xfe, 0x07, 0x00, 0xff, 0x07, 0x10, 0xfe, 0x07, 0x44, 0xff, 0x07, 0x44, 0xfe, 0x07, 0x10, 0xff, 0x07, 0x00, 0xfe, 0x07, 0x00, 0xff, 0x07, 0x00, 0xfe, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0x00, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0x54, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x07, 0x00, 0xfe, 0x07, 0x00, 0xff, 0x07, 0xff, 0xfe, 0x07, 0xff, 0xff, 0x07, 0xff, 0xfe, 0x07, 0xff, 0xff, 0x07, 0xff, 0xfe, 0x07, 0xff, 0xff, 0x07, 0xff, 0xfe, 0x07, 0xbb, 0xff, 0x07, 0x98, 0xfe, 0x07, 0x77, 0xff, 0x07, 0x67, 0xfe, 0x07, 0x44, 0xff, 0x07, 0x44, 0xfe, 0x07, 0x23, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbf, 0x07, 0x00, 0xdf, 0x07, 0x00, 0xbf, 0x07, 0xff, 0xdf, 0x07, 0xff, 0xbf, 0x07, 0xff, 0xdf, 0x07, 0xef, 0xbf, 0x07, 0x88, 0xdf, 0x07, 0x44, 0xbf, 0x07, 0x00, 0xdf, 0x07, 0x00, 0xbf, 0x07, 0x00, 0xdf, 0x07, 0x00, 0xbf, 0x07, 0x00, 0xdf, 0x07, 0x00, 0xbf, 0x07, 0x00, 0xdf, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0xff, 0x9f, 0x07, 0xff, 0x9f, 0x07, 0xff, 0x9f, 0x07, 0xbb, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x9f, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0xff, 0x5f, 0x07, 0xff, 0x5f, 0x07, 0xff, 0x5f, 0x07, 0xbb, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x5f, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0xff, 0x1f, 0x07, 0xff, 0x1f, 0x07, 0xff, 0x1f, 0x07, 0xbb, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x1f, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x10, 0xdf, 0x06, 0xff, 0xdf, 0x06, 0xff, 0xdf, 0x06, 0xff, 0xdf, 0x06, 0x98, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0xdf, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x9f, 0x06, 0x00, 0x9f, 0x06, 0x44, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xff, 0x9f, 0x06, 0xef, 0x9f, 0x06, 0xab, 0x9f, 0x06, 0x44, 0x9f, 0x06, 0x00, 0x9f, 0x06, 0x00, 0x9f, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5f, 0x06, 0x00, 0x5f, 0x06, 0x44, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0xff, 0x5f, 0x06, 0x88, 0x5f, 0x06, 0x00, 0x5f, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0x06, 0x00, 0x1f, 0x06, 0x23, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0xff, 0x1f, 0x06, 0x77, 0x1f, 0x06, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbf, 0x05, 0x00, 0xdf, 0x05, 0x00, 0xbf, 0x05, 0x77, 0xdf, 0x05, 0xbb, 0xbf, 0x05, 0x98, 0xdf, 0x05, 0x77, 0xbf, 0x05, 0x44, 0xdf, 0x05, 0x44, 0xbf, 0x05, 0x44, 0xdf, 0x05, 0x77, 0xbf, 0x05, 0xdc, 0xdf, 0x05, 0xff, 0xbf, 0x05, 0xff, 0xdf, 0x05, 0xff, 0xbf, 0x05, 0xef, 0xdf, 0x05, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x00, 0x7f, 0x05, 0x10, 0x7f, 0x05, 0xcc, 0x7f, 0x05, 0xff, 0x7f, 0x05, 0xff, 0x7f, 0x05, 0xff, 0x7f, 0x05, 0x67, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x00, 0x3f, 0x05, 0x54, 0x3f, 0x05, 0xff, 0x3f, 0x05, 0xff, 0x3f, 0x05, 0xff, 0x3f, 0x05, 0x88, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x00, 0xdf, 0x04, 0x00, 0xff, 0x04, 0x10, 0xdf, 0x04, 0xff, 0xff, 0x04, 0xff, 0xdf, 0x04, 0xff, 0xff, 0x04, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x00, 0xbf, 0x04, 0x00, 0x9f, 0x04, 0x10, 0xbf, 0x04, 0xff, 0x9f, 0x04, 0xff, 0xbf, 0x04, 0xff, 0x9f, 0x04, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x10, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x00, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x00, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x00, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x00, 0x5f, 0x04, 0x00, 0x7f, 0x04, 0x54, 0x5f, 0x04, 0xff, 0x7f, 0x04, 0xff, 0x5f, 0x04, 0xff, 0x7f, 0x04, 0xab, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x04, 0x00, 0x1f, 0x04, 0xef, 0x3f, 0x04, 0x67, 0x1f, 0x04, 0x00, 0x3f, 0x04, 0x00, 0x1f, 0x04, 0x00, 0x3f, 0x04, 0x00, 0x1f, 0x04, 0x00, 0x3f, 0x04, 0x00, 0x1f, 0x04, 0x00, 0x3f, 0x04, 0x00, 0x1f, 0x04, 0xbb, 0x3f, 0x04, 0xff, 0x1f, 0x04, 0xff, 0x3f, 0x04, 0xff, 0x1f, 0x04, 0x77, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xdf, 0x03, 0x33, 0xff, 0x03, 0xff, 0xdf, 0x03, 0xff, 0xff, 0x03, 0xbb, 0xdf, 0x03, 0x67, 0xff, 0x03, 0x10, 0xdf, 0x03, 0x00, 0xff, 0x03, 0x00, 0xdf, 0x03, 0x00, 0xff, 0x03, 0x23, 0xdf, 0x03, 0xab, 0xff, 0x03, 0xff, 0xdf, 0x03, 0xff, 0xff, 0x03, 0xff, 0xdf, 0x03, 0xff, 0xff, 0x03, 0x33, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbf, 0x03, 0x67, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xbb, 0xbf, 0x03, 0xbb, 0xbf, 0x03, 0xcc, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xff, 0xbf, 0x03, 0xbb, 0xbf, 0x03, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x9f, 0x03, 0x98, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xff, 0x9f, 0x03, 0xef, 0x9f, 0x03, 0x10, 0x9f, 0x03, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x77, 0x7f, 0x03, 0xef, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xbb, 0x7f, 0x03, 0x23, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x10, 0x7f, 0x03, 0x67, 0x7f, 0x03, 0xab, 0x7f, 0x03, 0xef, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xff, 0x7f, 0x03, 0xdc, 0x7f, 0x03, 0x98, 0x7f, 0x03, 0x44, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x00, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format:  Blue: 5 bit Green: 6 bit, Red: 5 bit, Alpha 8 bit  BUT the 2  color bytes are swapped*/
  0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x23, 0x07, 0xff, 0x44, 0x07, 0xff, 0x23, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x07, 0xff, 0xef, 0x07, 0xfe, 0xff, 0x07, 0xff, 0xff, 0x07, 0xfe, 0x54, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x07, 0xff, 0x10, 0x07, 0xfe, 0x44, 0x07, 0xff, 0x44, 0x07, 0xfe, 0x10, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x07, 0xff, 0x54, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x07, 0xff, 0xff, 0x07, 0xfe, 0xff, 0x07, 0xff, 0xff, 0x07, 0xfe, 0xff, 0x07, 0xff, 0xff, 0x07, 0xfe, 0xff, 0x07, 0xff, 0xff, 0x07, 0xfe, 0xbb, 0x07, 0xff, 0x98, 0x07, 0xfe, 0x77, 0x07, 0xff, 0x67, 0x07, 0xfe, 0x44, 0x07, 0xff, 0x44, 0x07, 0xfe, 0x23, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xbf, 0x00, 0x07, 0xdf, 0x00, 0x07, 0xbf, 0xff, 0x07, 0xdf, 0xff, 0x07, 0xbf, 0xff, 0x07, 0xdf, 0xef, 0x07, 0xbf, 0x88, 0x07, 0xdf, 0x44, 0x07, 0xbf, 0x00, 0x07, 0xdf, 0x00, 0x07, 0xbf, 0x00, 0x07, 0xdf, 0x00, 0x07, 0xbf, 0x00, 0x07, 0xdf, 0x00, 0x07, 0xbf, 0x00, 0x07, 0xdf, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0xff, 0x07, 0x9f, 0xff, 0x07, 0x9f, 0xff, 0x07, 0x9f, 0xbb, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x07, 0x9f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0xff, 0x07, 0x5f, 0xff, 0x07, 0x5f, 0xff, 0x07, 0x5f, 0xbb, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x07, 0x5f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0xff, 0x07, 0x1f, 0xff, 0x07, 0x1f, 0xff, 0x07, 0x1f, 0xbb, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x07, 0x1f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x10, 0x06, 0xdf, 0xff, 0x06, 0xdf, 0xff, 0x06, 0xdf, 0xff, 0x06, 0xdf, 0x98, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x06, 0xdf, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x9f, 0x00, 0x06, 0x9f, 0x44, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xff, 0x06, 0x9f, 0xef, 0x06, 0x9f, 0xab, 0x06, 0x9f, 0x44, 0x06, 0x9f, 0x00, 0x06, 0x9f, 0x00, 0x06, 0x9f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x5f, 0x00, 0x06, 0x5f, 0x44, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0xff, 0x06, 0x5f, 0x88, 0x06, 0x5f, 0x00, 0x06, 0x5f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x06, 0x1f, 0x00, 0x06, 0x1f, 0x23, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0xff, 0x06, 0x1f, 0x77, 0x06, 0x1f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0xbf, 0x00, 0x05, 0xdf, 0x00, 0x05, 0xbf, 0x77, 0x05, 0xdf, 0xbb, 0x05, 0xbf, 0x98, 0x05, 0xdf, 0x77, 0x05, 0xbf, 0x44, 0x05, 0xdf, 0x44, 0x05, 0xbf, 0x44, 0x05, 0xdf, 0x77, 0x05, 0xbf, 0xdc, 0x05, 0xdf, 0xff, 0x05, 0xbf, 0xff, 0x05, 0xdf, 0xff, 0x05, 0xbf, 0xef, 0x05, 0xdf, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x00, 0x05, 0x7f, 0x10, 0x05, 0x7f, 0xcc, 0x05, 0x7f, 0xff, 0x05, 0x7f, 0xff, 0x05, 0x7f, 0xff, 0x05, 0x7f, 0x67, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x00, 0x05, 0x3f, 0x54, 0x05, 0x3f, 0xff, 0x05, 0x3f, 0xff, 0x05, 0x3f, 0xff, 0x05, 0x3f, 0x88, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x00, 0x04, 0xdf, 0x00, 0x04, 0xff, 0x10, 0x04, 0xdf, 0xff, 0x04, 0xff, 0xff, 0x04, 0xdf, 0xff, 0x04, 0xff, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x00, 0x04, 0xbf, 0x00, 0x04, 0x9f, 0x10, 0x04, 0xbf, 0xff, 0x04, 0x9f, 0xff, 0x04, 0xbf, 0xff, 0x04, 0x9f, 0xbb, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x10, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x00, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x00, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x00, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x00, 0x04, 0x5f, 0x00, 0x04, 0x7f, 0x54, 0x04, 0x5f, 0xff, 0x04, 0x7f, 0xff, 0x04, 0x5f, 0xff, 0x04, 0x7f, 0xab, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0x3f, 0x00, 0x04, 0x1f, 0xef, 0x04, 0x3f, 0x67, 0x04, 0x1f, 0x00, 0x04, 0x3f, 0x00, 0x04, 0x1f, 0x00, 0x04, 0x3f, 0x00, 0x04, 0x1f, 0x00, 0x04, 0x3f, 0x00, 0x04, 0x1f, 0x00, 0x04, 0x3f, 0x00, 0x04, 0x1f, 0xbb, 0x04, 0x3f, 0xff, 0x04, 0x1f, 0xff, 0x04, 0x3f, 0xff, 0x04, 0x1f, 0x77, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xdf, 0x33, 0x03, 0xff, 0xff, 0x03, 0xdf, 0xff, 0x03, 0xff, 0xbb, 0x03, 0xdf, 0x67, 0x03, 0xff, 0x10, 0x03, 0xdf, 0x00, 0x03, 0xff, 0x00, 0x03, 0xdf, 0x00, 0x03, 0xff, 0x23, 0x03, 0xdf, 0xab, 0x03, 0xff, 0xff, 0x03, 0xdf, 0xff, 0x03, 0xff, 0xff, 0x03, 0xdf, 0xff, 0x03, 0xff, 0x33, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xbf, 0x67, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xbb, 0x03, 0xbf, 0xbb, 0x03, 0xbf, 0xcc, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xff, 0x03, 0xbf, 0xbb, 0x03, 0xbf, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0x9f, 0x98, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xff, 0x03, 0x9f, 0xef, 0x03, 0x9f, 0x10, 0x03, 0x9f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x77, 0x03, 0x7f, 0xef, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xbb, 0x03, 0x7f, 0x23, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x10, 0x03, 0x7f, 0x67, 0x03, 0x7f, 0xab, 0x03, 0x7f, 0xef, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xff, 0x03, 0x7f, 0xdc, 0x03, 0x7f, 0x98, 0x03, 0x7f, 0x44, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x00, 0x03, 0x7f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format:  Blue: 8 bit, Green: 8 bit, Red: 8 bit, Alpha: 8 bit*/
  0x00, 0x00, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x23, 0xf6, 0xff, 0x00, 0x44, 0xf6, 0xff, 0x00, 0x23, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0xef, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0x54, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x10, 0xf6, 0xff, 0x00, 0x44, 0xf6, 0xff, 0x00, 0x44, 0xf6, 0xff, 0x00, 0x10, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0x00, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0xff, 0xf6, 0xff, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf6, 0xfb, 0x00, 0x00, 0xf6, 0xfb, 0x00, 0x00, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xff, 0xf6, 0xfb, 0x00, 0xbb, 0xf6, 0xfb, 0x00, 0x98, 0xf6, 0xfb, 0x00, 0x77, 0xf6, 0xfb, 0x00, 0x67, 0xf6, 0xfb, 0x00, 0x44, 0xf6, 0xfb, 0x00, 0x44, 0xf6, 0xfb, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0xff, 0xf7, 0xf6, 0x00, 0xff, 0xf7, 0xf6, 0x00, 0xff, 0xf7, 0xf6, 0x00, 0xef, 0xf7, 0xf6, 0x00, 0x88, 0xf7, 0xf6, 0x00, 0x44, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0xf7, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0xff, 0xf7, 0xf0, 0x00, 0xff, 0xf7, 0xf0, 0x00, 0xff, 0xf7, 0xf0, 0x00, 0xbb, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0xf7, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0xff, 0xf7, 0xe9, 0x00, 0xff, 0xf7, 0xe9, 0x00, 0xff, 0xf7, 0xe9, 0x00, 0xbb, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0xf7, 0xe9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0xff, 0xf8, 0xe1, 0x00, 0xff, 0xf8, 0xe1, 0x00, 0xff, 0xf8, 0xe1, 0x00, 0xbb, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0xf8, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x10, 0xf8, 0xd9, 0x00, 0xff, 0xf8, 0xd9, 0x00, 0xff, 0xf8, 0xd9, 0x00, 0xff, 0xf8, 0xd9, 0x00, 0x98, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0xf8, 0xd9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf9, 0xd1, 0x00, 0x00, 0xf9, 0xd1, 0x00, 0x44, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xff, 0xf9, 0xd1, 0x00, 0xef, 0xf9, 0xd1, 0x00, 0xab, 0xf9, 0xd1, 0x00, 0x44, 0xf9, 0xd1, 0x00, 0x00, 0xf9, 0xd1, 0x00, 0x00, 0xf9, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xf9, 0xc8, 0x00, 0x00, 0xf9, 0xc8, 0x00, 0x44, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0xff, 0xf9, 0xc8, 0x00, 0x88, 0xf9, 0xc8, 0x00, 0x00, 0xf9, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfa, 0xbf, 0x00, 0x00, 0xfa, 0xbf, 0x00, 0x23, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0xff, 0xfa, 0xbf, 0x00, 0x77, 0xfa, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfa, 0xb6, 0x00, 0x00, 0xfa, 0xb6, 0x00, 0x00, 0xfa, 0xb6, 0x00, 0x77, 0xfa, 0xb6, 0x00, 0xbb, 0xfa, 0xb6, 0x00, 0x98, 0xfa, 0xb6, 0x00, 0x77, 0xfa, 0xb6, 0x00, 0x44, 0xfa, 0xb6, 0x00, 0x44, 0xfa, 0xb6, 0x00, 0x44, 0xfa, 0xb6, 0x00, 0x77, 0xfa, 0xb6, 0x00, 0xdc, 0xfa, 0xb6, 0x00, 0xff, 0xfa, 0xb6, 0x00, 0xff, 0xfa, 0xb6, 0x00, 0xff, 0xfa, 0xb6, 0x00, 0xef, 0xfa, 0xb6, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x00, 0xfb, 0xac, 0x00, 0x10, 0xfb, 0xac, 0x00, 0xcc, 0xfb, 0xac, 0x00, 0xff, 0xfb, 0xac, 0x00, 0xff, 0xfb, 0xac, 0x00, 0xff, 0xfb, 0xac, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x00, 0xfc, 0xa3, 0x00, 0x54, 0xfc, 0xa3, 0x00, 0xff, 0xfc, 0xa3, 0x00, 0xff, 0xfc, 0xa3, 0x00, 0xff, 0xfc, 0xa3, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x00, 0xfc, 0x9a, 0x00, 0x10, 0xfc, 0x9a, 0x00, 0xff, 0xfc, 0x9a, 0x00, 0xff, 0xfc, 0x9a, 0x00, 0xff, 0xfc, 0x9a, 0x00, 0xbb, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x00, 0xfd, 0x92, 0x00, 0x10, 0xfd, 0x92, 0x00, 0xff, 0xfd, 0x92, 0x00, 0xff, 0xfd, 0x92, 0x00, 0xff, 0xfd, 0x92, 0x00, 0xbb, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x10, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x00, 0xfd, 0x8a, 0x00, 0x54, 0xfd, 0x8a, 0x00, 0xff, 0xfd, 0x8a, 0x00, 0xff, 0xfd, 0x8a, 0x00, 0xff, 0xfd, 0x8a, 0x00, 0xab, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0xef, 0xfe, 0x82, 0x00, 0x67, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0x00, 0xfe, 0x82, 0x00, 0xbb, 0xfe, 0x82, 0x00, 0xff, 0xfe, 0x82, 0x00, 0xff, 0xfe, 0x82, 0x00, 0xff, 0xfe, 0x82, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0x00, 0x33, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0xbb, 0xfe, 0x7b, 0x00, 0x67, 0xfe, 0x7b, 0x00, 0x10, 0xfe, 0x7b, 0x00, 0x00, 0xfe, 0x7b, 0x00, 0x00, 0xfe, 0x7b, 0x00, 0x00, 0xfe, 0x7b, 0x00, 0x23, 0xfe, 0x7b, 0x00, 0xab, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0xff, 0xfe, 0x7b, 0x00, 0x33, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x75, 0x00, 0x67, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xbb, 0xfe, 0x75, 0x00, 0xbb, 0xfe, 0x75, 0x00, 0xcc, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xff, 0xfe, 0x75, 0x00, 0xbb, 0xfe, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0x70, 0x00, 0x98, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xff, 0xff, 0x70, 0x00, 0xef, 0xff, 0x70, 0x00, 0x10, 0xff, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x77, 0xff, 0x6c, 0x00, 0xef, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xbb, 0xff, 0x6c, 0x00, 0x23, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x10, 0xff, 0x6c, 0x00, 0x67, 0xff, 0x6c, 0x00, 0xab, 0xff, 0x6c, 0x00, 0xef, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xff, 0xff, 0x6c, 0x00, 0xdc, 0xff, 0x6c, 0x00, 0x98, 0xff, 0x6c, 0x00, 0x44, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0xff, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
};

const lv_img_dsc_t IMG_CLOCKFACE_DIGITAL3_HOUR5 = {
  .header.always_zero = 0,
  .header.w = 18,
  .header.h = 25,
  .data_size = 450 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = IMG_CLOCKFACE_DIGITAL3_HOUR5_map,
};
