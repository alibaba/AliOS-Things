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

#include "esp_log.h"
#include "button.h"
#include "periph_button.h"
#include "audio_mem.h"

static const char* TAG = "PERIPH_BUTTON";

#define VALIDATE_BTN(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_BUTTON)) { \
    ESP_LOGE(TAG, "Invalid BUTTON periph, at line %d", __LINE__);\
    return ret;\
}

typedef struct {
    esp_button_handle_t btn;
    uint64_t gpio_mask;
    int long_press_time_ms;
} periph_button_t;

static void button_send_event(esp_periph_handle_t self, int event_id, uint64_t mask)
{
    int gpio_num = 0;
    while (mask) {
        if (mask & 0x01) {
            esp_periph_send_event(self, event_id, (void*)gpio_num, 0);
        }
        mask >>= 1;
        gpio_num ++;
    }
}

static esp_err_t _button_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    button_result_t result;
    periph_button_t *periph_btn = esp_periph_get_data(self);
    if (button_read(periph_btn->btn, &result)) {
        ESP_LOGD(TAG, "Button event, press_mask %llx, release_mask: %llx, long_press_mask: %llx, long_release_mask: %llx",
                 result.press_mask, result.release_mask, result.long_press_mask, result.long_release_mask);
        button_send_event(self, PERIPH_BUTTON_PRESSED, result.press_mask);
        button_send_event(self, PERIPH_BUTTON_RELEASE, result.release_mask);
        button_send_event(self, PERIPH_BUTTON_LONG_PRESSED, result.long_press_mask);
        button_send_event(self, PERIPH_BUTTON_LONG_RELEASE, result.long_release_mask);
    }
    return ESP_OK;
}

static esp_err_t _button_destroy(esp_periph_handle_t self)
{
    periph_button_t *periph_btn = esp_periph_get_data(self);
    button_destroy(periph_btn->btn);
    audio_free(periph_btn);
    return ESP_OK;
}

static void IRAM_ATTR button_intr_handler(void* param)
{
    esp_periph_handle_t periph = (esp_periph_handle_t)param;
    esp_periph_send_cmd_from_isr(periph, 0, NULL, 0);
}

static void button_timer_handler(xTimerHandle tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t) pvTimerGetTimerID(tmr);
    esp_periph_send_cmd_from_isr(periph, 0, NULL, 0);
}

static esp_err_t _button_init(esp_periph_handle_t self)
{
    int ret = 0;
    VALIDATE_BTN(self, ESP_FAIL);
    periph_button_t *periph_btn = esp_periph_get_data(self);

    button_config_t btn_config = {
        .gpio_mask = periph_btn->gpio_mask,
        .long_press_time_ms = periph_btn->long_press_time_ms,
        .button_intr_handler = button_intr_handler,
        .intr_context = self,
    };
    periph_btn->btn = button_init(&btn_config);

    esp_periph_start_timer(self, 50/portTICK_RATE_MS, button_timer_handler);
    return ret;
}

esp_periph_handle_t periph_button_init(periph_button_cfg_t *config)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_BUTTON, "periph_btn");
    AUDIO_MEM_CHECK(TAG, periph, return NULL);
    periph_button_t *periph_btn = audio_calloc(1, sizeof(periph_button_t));

    AUDIO_MEM_CHECK(TAG, periph_btn, {
        audio_free(periph);
        return NULL;
    });
    periph_btn->gpio_mask = config->gpio_mask;
    periph_btn->long_press_time_ms = config->long_press_time_ms;

    esp_periph_set_data(periph, periph_btn);
    esp_periph_set_function(periph, _button_init, _button_run, _button_destroy);

    return periph;
}

