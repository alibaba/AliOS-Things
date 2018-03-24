/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup PWM_Driver_APIs PWM Driver APIs
  * @brief PWM driver APIs
  */

/** @addtogroup PWM_Driver_APIs
  * @{
  */

struct pwm_param {
    uint32 period;      /**< PWM period */
    uint32 freq;        /**< PWM frequency */
    uint32 duty[8];     /**< PWM duty */
};

#define PWM_DEPTH 1023

/**
  * @brief     PWM function initialization, including GPIO, frequency and duty cycle.
  *
  * @attention This API can be called only once.
  *
  * @param     uint32 period : pwm frequency
  * @param     uint32 *duty : duty cycle
  * @param     uint32 pwm_channel_num : PWM channel number
  * @param     uint32 (*pin_info_list)[3] : GPIO parameter of PWM channel, it is a pointer
  *                                         of n x 3 array which defines GPIO register, IO
  *                                         reuse of corresponding pin and GPIO number.
  *
  * @return    null
  */
void pwm_init(uint32 period, uint32 *duty, uint32 pwm_channel_num, uint32(*pin_info_list)[3]);

/**
  * @brief     Set the duty cycle of a PWM channel.
  *
  *            Set the time that high level signal will last, duty depends on period,
  *            the maximum value can be 1023.
  *           
  *
  * @attention After set configuration, pwm_start needs to be called to take effect.
  *
  * @param     uint32 duty : duty cycle
  * @param     uint8 channel : PWM channel number
  *
  * @return    null
  */
void pwm_set_duty(uint32 duty, uint8 channel);

/**
  * @brief  Get the duty cycle of a PWM channel.
  *
  * @param  uint8 channel : PWM channel number
  *
  * @return Duty cycle of PWM output.
  */
uint32 pwm_get_duty(uint8 channel);

/**
  * @brief     Set PWM period, unit : us.
  *
  *            For example, for 1KHz PWM, period is 1000 us.
  *
  * @attention After set configuration, pwm_start needs to be called to take effect.
  *
  * @param     uint32 period : PWM period, unit : us.
  *
  * @return    null
  */
void pwm_set_period(uint32 period);

/**
  * @brief  Get PWM period, unit : us.
  *
  * @param  null
  *
  * @return PWM period, unit : us.
  */
uint32 pwm_get_period(void);

/**
  * @brief  Starts PWM. 
  *
  * @attention This function needs to be called after PWM configuration is changed.
  *
  * @param  null
  *
  * @return null
  */
void pwm_start(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
