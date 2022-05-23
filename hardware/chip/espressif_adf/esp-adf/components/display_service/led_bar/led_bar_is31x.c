/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include <string.h>
#include "esp_types.h"
#include "esp_log.h"
#include "led_bar_is31x.h"
#include "periph_is31fl3216.h"


static char *TAG = "LED_BAR_IS31X";

esp_periph_handle_t led_bar_is31x_init(void)
{
    periph_is31fl3216_cfg_t is31fl3216_cfg = { 0 };
    is31fl3216_cfg.state = IS31FL3216_STATE_ON;
    esp_periph_handle_t handle = periph_is31fl3216_init(&is31fl3216_cfg);
    if (handle == NULL) {
        return NULL;
    }
    esp_periph_init(handle);
    for (int i = 0; i < BLUE_LED_MAX_NUM; i++) {
        periph_is31fl3216_set_duty(handle, i, 255);
    }
    return handle;
}

esp_err_t led_bar_is31x_pattern(void *handle, int pat, int value)
{
    esp_err_t ret =  ESP_OK;
    if (handle == NULL) {
        ESP_LOGE(TAG, "led_bar_is31x_pattern instance has not initialized");
        return ESP_FAIL;
    }
    esp_periph_handle_t h = (esp_periph_handle_t)handle;

    switch (pat) {
        case DISPLAY_PATTERN_WIFI_SETTING: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_duty_step(h, 20);
                periph_is31fl3216_set_interval(h, 10);
                periph_is31fl3216_set_act_time(h, 0);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_FLASH);
                break;
            }
        case DISPLAY_PATTERN_WIFI_CONNECTTING: {
                periph_is31fl3216_set_light_on_num(h, 1, BLUE_LED_MAX_NUM);
                periph_is31fl3216_set_interval(h, 100);
                periph_is31fl3216_set_shift_mode(h, PERIPH_IS31_SHIFT_MODE_ACC);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_SHIFT);
                break;
            }
        case DISPLAY_PATTERN_WIFI_CONNECTED: {
                periph_is31fl3216_set_light_on_num(h, 2, BLUE_LED_MAX_NUM);
                periph_is31fl3216_set_interval(h, 150);
                periph_is31fl3216_set_shift_mode(h, PERIPH_IS31_SHIFT_MODE_SINGLE);
                periph_is31fl3216_set_act_time(h, 2500);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_SHIFT);
                break;
            }
        case DISPLAY_PATTERN_WIFI_DISCONNECTED: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_duty_step(h, 20);
                periph_is31fl3216_set_interval(h, 10);
                periph_is31fl3216_set_act_time(h, 0);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_FLASH);
                break;
            }
            break;
        case DISPLAY_PATTERN_WIFI_SETTING_FINISHED: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }

        case DISPLAY_PATTERN_BT_CONNECTTING:
            break;
        case DISPLAY_PATTERN_BT_CONNECTED:
            break;
        case DISPLAY_PATTERN_BT_DISCONNECTED:
            break;
        case DISPLAY_PATTERN_RECORDING_START: {

                break;
            }
        case DISPLAY_PATTERN_RECORDING_STOP: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }

        case DISPLAY_PATTERN_RECOGNITION_START: {
                periph_is31fl3216_set_light_on_num(h, 1, BLUE_LED_MAX_NUM);
                periph_is31fl3216_set_interval(h, 150);
                periph_is31fl3216_set_shift_mode(h, PERIPH_IS31_SHIFT_MODE_SINGLE);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_SHIFT);
                break;
            }
        case DISPLAY_PATTERN_RECOGNITION_STOP: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }
        case DISPLAY_PATTERN_WAKEUP_ON: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_ON);
                break;
            }
            break;
        case DISPLAY_PATTERN_WAKEUP_FINISHED: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }
        case DISPLAY_PATTERN_MUSIC_ON: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_BY_AUDIO);
                break;
            }
        case DISPLAY_PATTERN_MUSIC_FINISHED: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }

        case DISPLAY_PATTERN_VOLUME:
            break;

        case DISPLAY_PATTERN_TURN_ON: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_ON);
                break;
            }
            break;
        case DISPLAY_PATTERN_TURN_OFF: {
                int bits_mask = ((1 << BLUE_LED_MAX_NUM) - 1);
                periph_is31fl3216_set_blink_pattern(h, bits_mask);
                periph_is31fl3216_set_state(h, IS31FL3216_STATE_OFF);
                break;
            }
            break;
        default:
            ESP_LOGW(TAG, "The mode is invalid");
            break;
    }

    return ret;
}

void led_bar_is31x_deinit(esp_periph_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return);
    esp_periph_destroy(handle);
}
