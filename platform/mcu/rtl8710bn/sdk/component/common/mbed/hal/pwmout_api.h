/** mbed Microcontroller Library
  ******************************************************************************
  * @file    pwmout_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed pwm API
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
  ****************************************************************************** 
  */
#ifndef MBED_PWMOUT_API_H
#define MBED_PWMOUT_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup pwm PWM
 *  @ingroup    hal
 *  @brief      pwm functions
 *  @{
 */

///@name Ameba Common
///@{

typedef struct pwmout_s pwmout_t;
/**
  * @brief  Initializes the PWM function/registers of the specified pin with default parameters.
  * @param  obj: PWM object define in application software.
  * @param  pin: the pinname of specified channel to be set.
  * @retval none
  * @note  
  *             - default period: 1638us
  *             - default pulse width: 102us
  *             - default duty cycle: 6.227%
  */
void pwmout_init(pwmout_t* obj, PinName pin);

/**
  * @brief  Deinitializes the PWM device of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval none
  * @note  If all channels are released, TIM5 will also be disabled.
  */
void pwmout_free(pwmout_t* obj);

/**
  * @brief  Set the duty cycle of the specified channel.
  * @param  obj: PWM object define in application software.
  * @param  percent: The duty cycle value to be set.
  * @retval none
  */
void pwmout_write(pwmout_t* obj, float percent);

/**
  * @brief  Get the duty cycle value of the specified channel.
  * @param  obj: PWM object define in application software.
  * @retval : the duty cycle value of the specified channel.
  */
float pwmout_read(pwmout_t* obj);

/**
  * @brief  Set the period of the specified channel in seconds.
  * @param  obj: PWM object define in application software.
  * @param  seconds: The period value to be set in seconds.
  * @retval none
  */
void pwmout_period(pwmout_t* obj, float seconds);

/**
  * @brief  Set the period of the specified channel in millseconds.
  * @param  obj: PWM object define in application software.
  * @param  ms: The period value to be set in millseconds.
  * @retval none
  */
void pwmout_period_ms(pwmout_t* obj, int ms);

/**
  * @brief  Set the period of the specified channel in microseconds.
  * @param  obj: PWM object define in application software.
  * @param  us: The period value to be set in microseconds.
  * @retval none
  */
void pwmout_period_us(pwmout_t* obj, int us);

/**
  * @brief  Set the pulse width of the specified channel in seconds.
  * @param  obj: PWM object define in application software.
  * @param  seconds: The pulse width value to be set in seconds.
  * @retval none
  */
void pwmout_pulsewidth(pwmout_t* obj, float seconds);

/**
  * @brief  Set the pulse width of the specified channel in milliseconds.
  * @param  obj: PWM object define in application software.
  * @param  ms: The pulse width value to be set in milliseconds.
  * @retval none
  */
void pwmout_pulsewidth_ms(pwmout_t* obj, int ms);

/**
  * @brief  Set the pulse width of the specified channel in microseconds.
  * @param  obj: PWM object define in application software.
  * @param  us: The pulse width value to be set in microseconds.
  * @retval none
  */
void pwmout_pulsewidth_us(pwmout_t* obj, int us);

///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif
