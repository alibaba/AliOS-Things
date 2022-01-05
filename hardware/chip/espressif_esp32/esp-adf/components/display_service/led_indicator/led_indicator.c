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

#include "periph_led.h"
#include "esp_log.h"
#include "led_indicator.h"
#include "driver/gpio.h"
#include "audio_mem.h"

static char *TAG = "LED_INDI";

typedef struct led_indicator_impl {
    gpio_num_t              gpio_num;
    esp_periph_handle_t     periph_handle;
} led_indicator_impl_t;

led_indicator_handle_t led_indicator_init(gpio_num_t num)
{
    led_indicator_impl_t *impl =  audio_calloc(1, sizeof(led_indicator_impl_t));
    AUDIO_MEM_CHECK(TAG, impl, return NULL);
    impl->gpio_num = num;
    periph_led_cfg_t led_cfg = {
        .led_speed_mode = LEDC_LOW_SPEED_MODE,
        .led_duty_resolution = LEDC_TIMER_10_BIT,
        .led_timer_num = LEDC_TIMER_0,
        .led_freq_hz = 10000,
    };
    impl->periph_handle  = periph_led_init(&led_cfg);
    esp_periph_init(impl->periph_handle);
    return impl;
}

esp_err_t led_indicator_pattern(void *handle, int pat, int value)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    led_indicator_handle_t h = (led_indicator_handle_t)handle;
    ESP_LOGD(TAG, "pat:%d, gpio:%d", pat, h->gpio_num);
    switch (pat) {
        case DISPLAY_PATTERN_WIFI_SETTING:
            periph_led_blink(h->periph_handle, h->gpio_num, 500, 500, false, -1, value);
            break;
        case DISPLAY_PATTERN_WIFI_CONNECTED:
            periph_led_blink(h->periph_handle, h->gpio_num, 1000, 200, true, 10, value);
            break;
        case DISPLAY_PATTERN_WIFI_DISCONNECTED:
            periph_led_blink(h->periph_handle, h->gpio_num, 200, 500, false, -1, value);
            break;
        case DISPLAY_PATTERN_TURN_ON:
        case DISPLAY_PATTERN_WAKEUP_ON:
            periph_led_blink(h->periph_handle, h->gpio_num, 100, 0, false, -1, value);
            break;
        case DISPLAY_PATTERN_TURN_OFF:
        case DISPLAY_PATTERN_WAKEUP_FINISHED:
        case DISPLAY_PATTERN_SPEECH_OVER:
            periph_led_blink(h->periph_handle, h->gpio_num, 0, 100, false, -1, value);
            break;
        case DISPLAY_PATTERN_SPEECH_BEGIN:
            periph_led_blink(h->periph_handle, h->gpio_num, 500, 500, false, -1, value);
            break;
        default:
            ESP_LOGW(TAG, "The led mode is invalid");
            break;
    }
    return ESP_OK;
}

void led_indicator_deinit(led_indicator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return);
    esp_periph_destroy (handle->periph_handle);
}