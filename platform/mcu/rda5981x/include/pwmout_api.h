/* mbed Microcontroller Library
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
#ifndef MBED_PWMOUT_API_H
#define MBED_PWMOUT_API_H

#include "device.h"


#ifdef __cplusplus
extern "C" {
#endif

/** Pwmout hal structure. pwmout_s is declared in the target's hal
 */
typedef struct pwmout_s pwmout_t;

/**
 * \defgroup hal_pwmout Pwmout hal functions
 * @{
 */

/** Set the pwm out clock, PWM0~4 share the same clock source, and
 *  the period equals (1 / (frequency / divider)) * ticks, there are tables:
 *
 * =================================================
 * | PWM \ ticks  | max      | 1%       | min      |
 * |-----------------------------------------------|
 * | PWM0~3       | 0x20000  | 0x64     | 0x2      |
 * |-----------------------------------------------|
 * | PWM4         | 0x1FF8   | 0x320    | 0x8      |
 * =================================================
 *
 * ==============================================================================================================
 * | div \ freq |                   16.384KHz                   |                    10MHz                      |
 * ==============================================================================================================
 * |            | PWM \ period | max      | 1%       | min      | PWM \ period | max      | 1%       | min      |
 * |            |-----------------------------------------------|-----------------------------------------------|
 * | 0          | PWM0~3       | 8s       | 6.1ms    | 122us    | PWM0~3       | 13.1ms   | 10us     | 200ns    |
 * |            |-----------------------------------------------|-----------------------------------------------|
 * |            | PWM4         | 499.5ms  | 48.8ms   | 488.3us  | PWM4         | 818.4us  | 80us     | 800ns    |
 * ==============================================================================================================
 * |            | PWM \ period | max      | 1%       | min      | PWM \ period | max      | 1%       | min      |
 * |            |-----------------------------------------------|-----------------------------------------------|
 * | 128        | PWM0~3       | 1024s    | 781ms    | 15.6ms   | PWM0~3       | 1677.7ms | 1.28ms   | 25.6us   |
 * |            |-----------------------------------------------|-----------------------------------------------|
 * |            | PWM4         | 63.9s    | 6.25s    | 62.5ms   | PWM4         | 104.8ms  | 10.24ms  | 102.4us  |
 * ==============================================================================================================
 *
 * @param obj The pwmout object
 * @param src The pwmout clock source, 0 represents 16KHz, 1 represents 10MHz
 * @param div The pwmout clock divider, 0 represents no divider, 2/4/6/8/.../126/128 is valid
 */
void pwmout_clk_set(pwmout_t *obj, int src, int div);

void pwmout_diff_write(pwmout_t *obj, float percent);

#endif /* TARGET_UNO_91H */

/** Initialize the pwm out peripheral and configure the pin
 *
 * @param obj The pwmout object to initialize
 * @param pin The pwmout pin to initialize
 */
void pwmout_init(pwmout_t *obj, PinName pin);

/** Deinitialize the pwmout object
 *
 * @param obj The pwmout object
 */
void pwmout_free(pwmout_t *obj);

/** Set the output duty-cycle in range <0.0f, 1.0f>
 *
 * Value 0.0f represents 0 percentage, 1.0f represents 100 percent.
 * @param obj     The pwmout object
 * @param percent The floating-point percentage number
 */
void pwmout_write(pwmout_t *obj, float percent);

/** Read the current float-point output duty-cycle
 *
 * @param obj The pwmout object
 * @return A floating-point output duty-cycle
 */
float pwmout_read(pwmout_t *obj);

/** Set the PWM period specified in seconds, keeping the duty cycle the same
 *
 * Periods smaller than microseconds (the lowest resolution) are set to zero.
 * @param obj     The pwmout object
 * @param seconds The floating-point seconds period
 */
void pwmout_period(pwmout_t *obj, float seconds);

/** Set the PWM period specified in miliseconds, keeping the duty cycle the same
 *
 * @param obj The pwmout object
 * @param ms  The milisecond period
 */
void pwmout_period_ms(pwmout_t *obj, int ms);

/** Set the PWM period specified in microseconds, keeping the duty cycle the same
 *
 * @param obj The pwmout object
 * @param us  The microsecond period
 */
void pwmout_period_us(pwmout_t *obj, int us);

/** Set the PWM pulsewidth specified in seconds, keeping the period the same.
 *
 * @param obj     The pwmout object
 * @param seconds The floating-point pulsewidth in seconds
 */
void pwmout_pulsewidth(pwmout_t *obj, float seconds);

/** Set the PWM pulsewidth specified in miliseconds, keeping the period the same.
 *
 * @param obj The pwmout object
 * @param ms  The floating-point pulsewidth in miliseconds
 */
void pwmout_pulsewidth_ms(pwmout_t *obj, int ms);

/** Set the PWM pulsewidth specified in microseconds, keeping the period the same.
 *
 * @param obj The pwmout object
 * @param us  The floating-point pulsewidth in microseconds
 */
void pwmout_pulsewidth_us(pwmout_t *obj, int us);

void pwmout_sync_write(pwmout_t *obj, float value, uint8_t duty_sel);


/**@}*/

#ifdef __cplusplus
}
#endif

