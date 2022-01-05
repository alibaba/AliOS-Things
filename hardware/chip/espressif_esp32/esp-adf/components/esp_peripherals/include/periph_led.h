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

#ifndef _PERIPH_LED_H_
#define _PERIPH_LED_H_

#include "sys/queue.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "audio_common.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Peripheral LED event id
 */
typedef enum {
    PERIPH_LED_UNCHANGE = 0,  /*!< No event */
    PERIPH_LED_BLINK_FINISH,  /*!< When LED blink is finished */
} periph_led_event_id_t;

/**
 * @brief      Peripheral LED idle output level
 */
typedef enum {
	PERIPH_LED_IDLE_LEVEL_LOW,  /*!< Low level output */
	PERIPH_LED_IDLE_LEVEL_HIGH  /*!< High level output */
} periph_led_idle_level_t;

/**
 * @brief   The LED peripheral configuration
 */
typedef struct {
    ledc_mode_t      led_speed_mode;        /*!< LEDC speed speed_mode, high-speed mode or low-speed mode */
    ledc_timer_bit_t led_duty_resolution;   /*!< LEDC channel duty resolution */
    ledc_timer_t     led_timer_num;         /*!< Select the timer source of channel (0 - 3) */
    uint32_t         led_freq_hz;           /*!< LEDC timer frequency (Hz) */
    int              gpio_num;              /*!< Optional, < 0 invalid gpio number */
} periph_led_cfg_t;

/**
 * @brief      Create the LED peripheral handle for esp_peripherals
 *
 * @note       The handle was created by this function automatically destroy when `esp_periph_destroy` is called
 *
 * @param      config  The configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_led_init(periph_led_cfg_t* config);

/**
 * @brief      Bink LED Peripheral, this function will automatically configure the gpio_num to control the LED,
 *             with `time_on_ms` as the time (in milliseconds) switch from OFF to ON (or ON if fade is disabled),
 *             and `time_off_ms` as the time (in milliseconds) switch from ON to OFF (or OFF if fade is disabled).
 *             When switching from ON -> OFF and vice versa, the loop decreases once, and will turn off the effect when the loop is 0.
 *             With a loop value less than 0, the LED effect will loop endlessly.
 *             PERIPH_LED_BLINK_FINISH events will be sent at each end of loop
 *
 * @param[in]  periph       The LED periph
 * @param[in]  gpio_num     The gpio number
 * @param[in]  time_on_ms   The time on milliseconds
 * @param[in]  time_off_ms  The time off milliseconds
 * @param[in]  fade         Fading enabled
 * @param[in]  loop         Loop
 * @param[in]  level        idle level
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_led_blink(esp_periph_handle_t periph, int gpio_num, int time_on_ms, int time_off_ms, bool fade, int loop, periph_led_idle_level_t level);

/**
 * @brief      Stop Blink the LED
 *
 * @param[in]  periph    The periph
 * @param[in]  gpio_num  The gpio number
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_led_stop(esp_periph_handle_t periph, int gpio_num);

#ifdef __cplusplus
}
#endif

#endif
