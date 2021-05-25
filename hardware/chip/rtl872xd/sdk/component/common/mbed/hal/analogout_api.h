/** mbed Microcontroller Library
 ******************************************************************************
 * @file    analogout_api.h
 * @author 
 * @version V1.0.0
 * @brief   This file provides following mbed Analog_out API
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2006-2013 ARM Limited
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
 */
#ifndef MBED_ANALOGOUT_API_H
#define MBED_ANALOGOUT_API_H

#include "device.h"



#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup analog_out ANALOG_OUT
 *  @ingroup    hal
 *  @brief      analog_out functions
 *  @{
 */


#if defined(CONFIG_PLATFORM_8195A) && (CONFIG_PLATFORM_8195A == 1)
///@name Ameba1 Only 
///@{

typedef struct dac_s dac_t;


/** 
 * @brief  Initialize DAC 
 * @param  obj: dac object define in application software.
 * @param  pin: dac PinName according to pinmux spec.
 * @retval none  
 */
void analogout_init(dac_t *obj, PinName pin);

/** 
 * @brief  Free DAC
 * @param  obj: dac object define in application software.
 * @retval none  
 */
void analogout_free(dac_t *obj);

/** 
 * @brief Execute analog output   
 * @para obj: dac object define in application software.
 * @para value: analog ratio value, should be transfered to register value.
 * @retval none  
 * @note This function is mainly to execute analog output and the value is a ratio.
 *       The upper/lower bound of DAC register input value is defined by 
 *       DAC_XXXXX_FULL_SCALE. The parameter "value" of this function should be 
 *       transfered to register value.
 */
void analogout_write(dac_t *obj, float value);

/** 
 * @brief Execute analog output 16bit
 * @para obj: dac object define in application software.
 * @para value: analog ratio value, should be transfered to register value.
 * @retval none  
 * @note The register value of DAC input is a format of 2's complement.
 *       The most maximum value of positive value drives DAC to output a voltage about 3.3V.
 *       The most mimimum value of negative value drives DAC to output a voltage about 0.
 *       And the middle value of 0x000 will drive DAC to output a voltage of half of max voltage.
 */
void analogout_write_u16(dac_t *obj, uint16_t value);

///@}
#endif //CONFIG_PLATFORM_8195A

/**@}*/

#ifdef __cplusplus
}
#endif

#endif//MBED_ANALOGOUT_API_H
