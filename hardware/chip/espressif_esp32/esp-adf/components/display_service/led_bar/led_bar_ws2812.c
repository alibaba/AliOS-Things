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

#include "driver/gpio.h"
#include "esp_log.h"
#include "board.h"
#include "led_bar_ws2812.h"
#include "audio_mem.h"
#include "periph_ws2812.h"
#include "string.h"

static const char *TAG = "LED_BAR_WS2812";

typedef struct led_bar_ws2812_impl {
    esp_periph_handle_t             periph_handle;
    periph_ws2812_ctrl_cfg_t        *control_cfg;
    uint32_t                        led_num;
} led_bar_ws2812_impl_t;

/**
 * This function is weak-linked, so you can implement your own version to custom display pattern.
 */
void __attribute__((weak)) ws2812_pattern_copy(struct periph_ws2812_ctrl_cfg *p)
{
    ESP_LOGW(TAG, "Default %s has been called, %s", __func__, __FILE__);
}

led_bar_ws2812_handle_t led_bar_ws2812_init(gpio_num_t gpio_num, int led_num)
{
    led_bar_ws2812_impl_t *impl = audio_calloc(1, sizeof(led_bar_ws2812_impl_t));
    AUDIO_MEM_CHECK(TAG, impl, return NULL);

    impl->led_num = led_num;
    periph_ws2812_cfg_t ws2812_cfg = {
        .gpio_num = gpio_num,
        .led_num = led_num,
    };
    impl->control_cfg = audio_malloc(sizeof(periph_ws2812_ctrl_cfg_t[impl->led_num]) * DISPLAY_PATTERN_MAX);
    AUDIO_MEM_CHECK(TAG, impl->control_cfg, return NULL);
    ws2812_pattern_copy(impl->control_cfg);

    impl->periph_handle = periph_ws2812_init(&ws2812_cfg);

    esp_periph_init(impl->periph_handle);
    return impl;
}

esp_err_t led_bar_ws2812_pattern(void *handle, int pat, int value)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    led_bar_ws2812_handle_t h = (led_bar_ws2812_handle_t)handle;
    ESP_LOGD(TAG, "pat:%d", pat);
    if (pat == DISPLAY_PATTERN_VOLUME) {
        periph_ws2812_ctrl_cfg_t *cfg = (h->control_cfg + h->led_num * pat);
        int on_n = (value * h->led_num) / 100; // volume range is 0-100
        for (int i = 0; i < on_n; ++i) {
            cfg->color = LED2812_COLOR_WHITE;
            cfg ++;
        }
    }
    periph_ws2812_control(h->periph_handle, (h->control_cfg + h->led_num * pat), NULL);

    return ESP_OK;
}

esp_err_t led_bar_ws2812_deinit(led_bar_ws2812_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    esp_periph_destroy(handle->periph_handle);
    if (handle->control_cfg) {
        audio_free(handle->control_cfg);
        handle->control_cfg = NULL;
    }
    audio_free(handle);
    return ESP_OK;
}
