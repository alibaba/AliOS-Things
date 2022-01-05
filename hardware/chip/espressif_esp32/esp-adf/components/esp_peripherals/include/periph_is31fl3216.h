/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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
#ifndef __PERIPH_IS31FL3216_H__
#define __PERIPH_IS31FL3216_H__

#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS31FL3216_CH_NUM 16   //Should be less than or equal to 16
#define BLUE_LED_MAX_NUM  12

typedef enum {
    IS31FL3216_STATE_UNKNOWN,
    IS31FL3216_STATE_OFF,
    IS31FL3216_STATE_ON,
    IS31FL3216_STATE_FLASH,
    IS31FL3216_STATE_BY_AUDIO,
    IS31FL3216_STATE_SHIFT,
} periph_is31fl3216_state_t;

typedef enum {
    PERIPH_IS31_SHIFT_MODE_UNKNOWN,
    PERIPH_IS31_SHIFT_MODE_ACC,     /*!< accumulation mode */
    PERIPH_IS31_SHIFT_MODE_SINGLE,
} periph_is31_shift_mode_t;

/**
 * @brief      The configuration of is31fl3216
 */
typedef struct {
    uint32_t                    duty[IS31FL3216_CH_NUM];    /*!<An array of the is31fl3216's duty*/
    uint16_t                    is31fl3216_pattern;         /*!<Current enable channel*/
    periph_is31fl3216_state_t   state;                      /*!<The state of all the channels*/
} periph_is31fl3216_cfg_t;

/**
 * @brief      Initializate the is31fl3216
 *
 * @param      is31fl3216_config
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_periph_handle_t periph_is31fl3216_init(periph_is31fl3216_cfg_t *is31fl3216_config);

/**
 * @brief      Set the state of all the channels
 *
 * @param      periph               The is31fl3216 handle
 * @param      state                The state of all channels
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_state(esp_periph_handle_t periph, periph_is31fl3216_state_t state);

/**
 * @brief      Set the current enable channels
 *
 * @param      periph                The is31fl3216 handle
 * @param      blink_pattern         The bit pattern of enabled channels
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_blink_pattern(esp_periph_handle_t periph, uint16_t blink_pattern);

/**
 * @brief      Set the duty of the channel
 *
 * @param      periph                The is31fl3216 handle
 * @param      index                 The channel number
 * @param      value                 The value of the channel's duty to be set
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_duty(esp_periph_handle_t periph, uint8_t index, uint8_t value);

/**
 * @brief      Set the duty step of flash
 *
 * @param      periph              The is31fl3216 handle
 * @param      step                The step of flash
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_duty_step(esp_periph_handle_t periph, uint8_t step);

/**
 * @brief      Set the internval time
 *
 * @param      periph                The is31fl3216 handle
 * @param      interval_ms           Time of interval
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_interval(esp_periph_handle_t periph, uint16_t interval_ms);

/**
 * @brief      Set the shift mode
 *
 * @param      periph                The is31fl3216 handle
 * @param      mode                  Mode of periph_is31_shift_mode_t
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_shift_mode(esp_periph_handle_t periph, periph_is31_shift_mode_t mode);

/**
 * @brief      Set the light on numbers
 *
 * @param      periph                The is31fl3216 handle
 * @param      light_on_num          Enabled led number
 * @param      max_light_num         Maximum led number
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_light_on_num(esp_periph_handle_t periph, uint16_t light_on_num, uint16_t max_light_num);

/**
 * @brief      Set the action time
 *
 * @param      periph                The is31fl3216 handle
 * @param      act_ms                Action time, unit is millisecond, 0 is infinite
 *
 * @return
 *     - ESP_OK    Success
 *     - ESP_FAIL  Fail
 */
esp_err_t periph_is31fl3216_set_act_time(esp_periph_handle_t periph, uint16_t act_ms);


#ifdef __cplusplus
}
#endif

#endif
