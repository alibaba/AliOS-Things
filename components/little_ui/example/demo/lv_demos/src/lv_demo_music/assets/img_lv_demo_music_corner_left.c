#include "../lv_demo_music.h"
#if LV_USE_DEMO_MUSIC  && !LV_DEMO_MUSIC_LARGE

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

const LV_ATTRIBUTE_MEM_ALIGN uint8_t img_lv_demo_music_corner_left_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Alpha 8 bit, Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x49, 0x07, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0x1b, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0x3c, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0x73, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xb8, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xf8, 0x49, 0x13, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0x5f, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xbf, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0x3f, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xdb, 0x49, 0x0f, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0x8b, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xfb, 0x49, 0x53, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xfc, 0x49, 0x63, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xfc, 0x49, 0x70, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xa0, 0x49, 0x13, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xef, 0x49, 0x57, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xcc, 0x49, 0x6b, 0x49, 0x18, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0xff, 0x00, 0xff, 0x00, 
  0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xff, 0x49, 0xc4, 0x49, 0x7b, 0x49, 0x48, 0x49, 0x1b, 0x49, 0x07, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x89, 0x31, 0x07, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0x1b, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0x3c, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0x73, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xb8, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xf8, 0x89, 0x31, 0x13, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0x5f, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xbf, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0x3f, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xdb, 0x89, 0x31, 0x0f, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0x8b, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xfb, 0x89, 0x31, 0x53, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xfc, 0x89, 0x31, 0x63, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xfc, 0x89, 0x31, 0x70, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xa0, 0x89, 0x31, 0x13, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xef, 0x89, 0x31, 0x57, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xcc, 0x89, 0x31, 0x6b, 0x89, 0x31, 0x18, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0x89, 0x31, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xff, 0x89, 0x31, 0xc4, 0x89, 0x31, 0x7b, 0x89, 0x31, 0x48, 0x89, 0x31, 0x1b, 0x89, 0x31, 0x07, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Alpha 8 bit, Red: 5 bit, Green: 6 bit, Blue: 5 bit  BUT the 2  color bytes are swapped*/
  0x31, 0x89, 0x07, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0x1b, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0x3c, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0x73, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xb8, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xf8, 0x31, 0x89, 0x13, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0x5f, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xbf, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0x3f, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xdb, 0x31, 0x89, 0x0f, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0x8b, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xfb, 0x31, 0x89, 0x53, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xfc, 0x31, 0x89, 0x63, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xfc, 0x31, 0x89, 0x70, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xa0, 0x31, 0x89, 0x13, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xef, 0x31, 0x89, 0x57, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xcc, 0x31, 0x89, 0x6b, 0x31, 0x89, 0x18, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0x31, 0x89, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 
  0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xff, 0x31, 0x89, 0xc4, 0x31, 0x89, 0x7b, 0x31, 0x89, 0x48, 0x31, 0x89, 0x1b, 0x31, 0x89, 0x07, 
#endif
#if LV_COLOR_DEPTH == 32
  0x47, 0x32, 0x34, 0x07, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0x1b, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0x3c, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0x73, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xb8, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xf8, 0x47, 0x32, 0x34, 0x13, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0x5f, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xbf, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0x3f, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xdb, 0x47, 0x32, 0x34, 0x0f, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0x8b, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xfb, 0x47, 0x32, 0x34, 0x53, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xfc, 0x47, 0x32, 0x34, 0x63, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xfc, 0x47, 0x32, 0x34, 0x70, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xa0, 0x47, 0x32, 0x34, 0x13, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xef, 0x47, 0x32, 0x34, 0x57, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xcc, 0x47, 0x32, 0x34, 0x6b, 0x47, 0x32, 0x34, 0x18, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0x47, 0x32, 0x34, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
  0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xff, 0x47, 0x32, 0x34, 0xc4, 0x47, 0x32, 0x34, 0x7b, 0x47, 0x32, 0x34, 0x48, 0x47, 0x32, 0x34, 0x1b, 0x47, 0x32, 0x34, 0x07, 
#endif
};

lv_img_dsc_t img_lv_demo_music_corner_left = {
  .header.always_zero = 0,
  .header.w = 18,
  .header.h = 18,
  .data_size = 324 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = img_lv_demo_music_corner_left_map,
};

#endif /*LV_USE_DEMO_MUSIC*/


