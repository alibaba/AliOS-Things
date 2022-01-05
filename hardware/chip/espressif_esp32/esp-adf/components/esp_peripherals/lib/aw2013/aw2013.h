/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
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

#ifndef _AW2013_H_
#define _AW2013_H_

#include "esp_log.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AW2013_I2C_PORT       I2C_NUM_0

#define AW2013_RESET_VALUE     0x55

#define AW2013_REG_RESET       0x00
#define AW2013_REG_GCR         0x01

#define AW2013_REG_LCTR        0x30

#define AW2013_REG_LCFG0       0x31
#define AW2013_REG_LCFG1       0x32
#define AW2013_REG_LCFG2       0x33

#define AW2013_REG_PWM0        0x34
#define AW2013_REG_PWM1        0x35
#define AW2013_REG_PWM2        0x36

#define AW2013_REG_LED0T1T2      0x37
#define AW2013_REG_LED1T1T2      0x3A
#define AW2013_REG_LED2T1T2      0x3D

#define AW2013_REG_LED0T3T4      0x38
#define AW2013_REG_LED1T3T4      0x3B
#define AW2013_REG_LED2T3T4      0x3E

#define AW2013_REG_LED0T0CNT     0x39
#define AW2013_REG_LED1T0CNT     0x3C
#define AW2013_REG_LED2T0CNT     0x3F

typedef enum {
    AW2013_BRIGHT_0,  // Turn off the lights, the electric current is 0mA
    AW2013_BRIGHT_1,  // 5mA
    AW2013_BRIGHT_2,  // 10mA
    AW2013_BRIGHT_3,  // 15mA
} aw2013_brightness_t;

// Time periods of a auto flash cycle
/*-------------------------------------------*\
|                 __________                  |
|                /|        |\                 |
|               / |        | \                |
|              /  |        |  \               |
|     ________/   |        |   \__________    |
|     |       |   |        |   |        |     |
|     |<--t0->|t1 |<--t2-->|t3 |<--t4-->|     |
\*-------------------------------------------*/
typedef enum {
    AW2013_TIME_0, // T0
    AW2013_TIME_1, // T1
    AW2013_TIME_2, // T2
    AW2013_TIME_3, // T3
    AW2013_TIME_4  // T4
} aw2013_time_t;

typedef enum {             // T1-T4     T0
    AW2013_TIME_LEVEL_0,   // 0.13s  (T0 0s)
    AW2013_TIME_LEVEL_1,   // 0.26s  (T0 0.13s)
    AW2013_TIME_LEVEL_2,   // 0.52s  (T0 0.26s)
    AW2013_TIME_LEVEL_3,   // 1.04s  (T0 0.52s)
    AW2013_TIME_LEVEL_4,   // 2.08s  (T0 1.04s)
    AW2013_TIME_LEVEL_5,   // 4.16s  (T0 2.08s)
    AW2013_TIME_LEVEL_6,   // 8.32s  (T0 4.16s)
    AW2013_TIME_LEVEL_7,   // 16.64s (T0 8.32s)
    AW2013_TIME_LEVEL_8,   //        (T0 16.64s)
} aw2013_time_level_t;

/**
 * @brief Initialize the aw2013 chip
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_init(void);

/**
 * @brief Deinitialize the aw2013 chip
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_deinit(void);

/**
 * @brief Reset the aw2013 chip
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_reset(void);

/**
 * @brief Set rgb value for the aw2013
 *
 * @param value  The value to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_set_pwm_value(uint32_t value);

/**
 * @brief Set repeat times for auto flash
 *
 * @param cnt  Number of repetitions
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_set_repeat_time(uint8_t cnt);

/**
 * @brief Set the time for each time period for auto flash
 *
 * @param time  The time period
 * @param level The time to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_set_time(aw2013_time_t time, aw2013_time_level_t level);

/**
 * @brief Set the brightness
 *
 * @param bright The brightness to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_set_brightness(aw2013_brightness_t bright);

/**
 * @brief Enable the auto flash fuction
 *
 * @param en  Whether to enable
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_enable_auto_flash(bool en);

/**
 * @brief Enable the fade fuction
 *
 * @param en  Whether to enable
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t aw2013_enable_fade_mode(bool en);

#ifdef __cplusplus
}
#endif

#endif
