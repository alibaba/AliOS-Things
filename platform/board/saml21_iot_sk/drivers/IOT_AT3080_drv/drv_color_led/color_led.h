/**
 ******************************************************************************
 * @file    color_led.h
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   RGB color led driver APIs
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

#ifndef __COLOR_LED_H_
#define __COLOR_LED_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup drivers */
/** @{*/

/** \addtogroup color_led */
/** @{*/

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/

/**
 * @brief Initialize RGB LED device.
 *
 * @return none
 */
void color_led_init( void );


/**
 * @brief Set light parameters for RGB LED
 *
 * @param red:    Red light parameter
 * @param green:  Green light parameter
 * @param blue:   Blue light parameter
 *
 * @return none
 */
void color_led_open_rgb(uint8_t red, uint8_t green, uint8_t blue);


/**
 * @brief Set light parameters for RGB LED
 *
 * @param hues:         hues data of RGB LED
 * @param saturation:   saturation data of RGB LED
 * @param brightness:   brightness data of RGB LED
 *
 * @return none
 */
void color_led_open_hsb(float hues, float saturation, float brightness);


/**
 * @brief Close RGB LED
 *
 * @return none
 */
void color_led_close(void);

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif  // __COLOR_LED_H_
