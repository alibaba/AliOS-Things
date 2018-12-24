/**
 ******************************************************************************
 * @file    hsb2rgb.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   HSB value to RGB value converter functions
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */


#include <math.h>

#include "mx_common.h"
#include "color_led.h"

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

#define H2R_MAX_RGB_val 255.0f

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

static float constrain(float value, float min, float max){
  if(value >= max)
    return max;
  if(value <=min )
    return min;
  return value;
}

//float Percent(float value){
//  return value = (((float)value / 255.0) * 100.0);
//}

static void H2R_HSBtoRGB(float hue, float sat, float bright, float *color) {
  // constrain all input variables to expected range
  hue = constrain(hue, 0, 360);
  sat = constrain(sat, 0, 100);
  bright = constrain(bright, 0, 100);
  
  // define maximum value for RGB array elements
  float max_rgb_val = H2R_MAX_RGB_val;
  // convert saturation and brightness value to decimals and init r, g, b variables
  float sat_f = (float)sat / 100.0f;
  float bright_f = (float)bright / 100.0f;
  float r=0, g=0, b=0;
  // If brightness is 0 then color is black (achromatic)
  // therefore, R, G and B values will all equal to 0
  if (bright <= 0) {
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
  }
  // If saturation is 0 then color is gray (achromatic)
  // therefore, R, G and B values will all equal the current brightness
  if (sat <= 0) {
    color[0] = bright_f * max_rgb_val;
    color[1] = bright_f * max_rgb_val;
    color[2] = bright_f * max_rgb_val;
  }
  // if saturation and brightness are greater than 0 then calculate
  // R, G and B values based on the current hue and brightness
  else {
    if (hue >= 0 && hue < 120) {
      float hue_primary = 1.0f - ((float)hue / 120.0f);
      float hue_secondary = (float)hue / 120.0f;
      float sat_primary = (1.0f - hue_primary) * (1.0f - sat_f);
      float sat_secondary = (1.0f - hue_secondary) * (1.0f - sat_f);
      float sat_tertiary = 1.0f - sat_f;
      r = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
      g = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
      b = (bright_f * max_rgb_val) * sat_tertiary;
    }
    else if (hue >= 120 && hue < 240) {
      float hue_primary = 1.0f - (((float)hue-120.0f) / 120.0f);
      float hue_secondary = ((float)hue-120.0f) / 120.0f;
      float sat_primary = (1.0f - hue_primary) * (1.0f - sat_f);
      float sat_secondary = (1.0f - hue_secondary) * (1.0f - sat_f);
      float sat_tertiary = 1.0f - sat_f;
      r = (bright_f * max_rgb_val) * sat_tertiary;
      g = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
      b = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
    }
    else if (hue >= 240 && hue <= 360) {
      float hue_primary = 1.0f - (((float)hue-240.0f) / 120.0f);
      float hue_secondary = ((float)hue-240.0f) / 120.0f;
      float sat_primary = (1.0f - hue_primary) * (1.0f - sat_f);
      float sat_secondary = (1.0f - hue_secondary) * (1.0f - sat_f);
      float sat_tertiary = 1.0f - sat_f;
      r = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
      g = (bright_f * max_rgb_val) * sat_tertiary;
      b = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
    }
    color[0] = r;
    color[1] = g;
    color[2] = b;
  }
  //  color[0] = Percent(color[0]);
  //  color[1] = Percent(color[1]);
  //  color[2] = Percent(color[2]);
}



void color_led_open_hsb(float hues, float saturation, float brightness)
{
	float color[3] = {0};
		
	//saturation = saturation;//*pow(saturation/100.0, 0.2);
	saturation = saturation*pow(saturation/100.0, 0.1);
	brightness = brightness*pow(brightness/100.0, 2);

	H2R_HSBtoRGB(hues, saturation, brightness, color);
	
	color_led_init();
	color_led_open_rgb((uint8_t)(color[0]), (uint8_t)(color[1]), (uint8_t)(color[2]));
}
