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
#include "periph_touch.h"
#include "touch.h"
#include "esp_peripherals.h"
#include "audio_mem.h"

static const char *TAG = "PERIPH_TOUCH";

#define VALIDATE_TOUCH(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_TOUCH)) { \
    ESP_LOGE(TAG, "Invalid TOUCH periph, at line %d", __LINE__);\
    return ret;\
}

typedef struct periph_touch {
    esp_touch_handle_t touch;
    int touch_mask;
    int long_tap_time_ms;
    int tap_threshold_percent;
    touch_result_t result;
} periph_touch_t;

static void touch_send_event(esp_periph_handle_t self, int event_id, int mask)
{
    int touch_num = 0;
    while (mask) {
        if (mask & 0x01) {
            esp_periph_send_event(self, event_id, (void *)touch_num, 0);
        }
        mask >>= 1;
        touch_num ++;
    }
}

static esp_err_t _touch_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    periph_touch_t *periph_touch = esp_periph_get_data(self);
    touch_send_event(self, PERIPH_TOUCH_TAP, periph_touch->result.tap_mask);
    touch_send_event(self, PERIPH_TOUCH_RELEASE, periph_touch->result.release_mask);
    touch_send_event(self, PERIPH_TOUCH_LONG_TAP, periph_touch->result.long_tap_mask);
    touch_send_event(self, PERIPH_TOUCH_LONG_RELEASE, periph_touch->result.long_release_mask);
    return ESP_OK;
}

static void touch_timer_handler(xTimerHandle tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t) pvTimerGetTimerID(tmr);

    periph_touch_t *periph_touch = esp_periph_get_data(periph);
    if (esp_touch_read(periph_touch->touch, &periph_touch->result)) {
        ESP_LOGD(TAG, "Touch event, tap %x, release_mask: %x, long_tap_mask: %x, long_tap_mask: %x",
                 periph_touch->result.tap_mask, periph_touch->result.release_mask,
                 periph_touch->result.long_tap_mask, periph_touch->result.long_release_mask);
        esp_periph_send_cmd(periph, 0, NULL, 0);
    }
}

static esp_err_t _touch_init(esp_periph_handle_t self)
{
    VALIDATE_TOUCH(self, ESP_FAIL);
    periph_touch_t *periph_touch = esp_periph_get_data(self);

    touch_config_t touch_config = {
        .touch_mask = periph_touch->touch_mask,
        .long_tap_time_ms = periph_touch->long_tap_time_ms,
        .tap_threshold_percent = periph_touch->tap_threshold_percent,
    };
    periph_touch->touch = esp_touch_init(&touch_config);

    esp_periph_start_timer(self, 150 / portTICK_PERIOD_MS, touch_timer_handler);
    ESP_LOGW(TAG, "_touch_init");
    return ESP_OK;
}

static esp_err_t _touch_destroy(esp_periph_handle_t self)
{
    periph_touch_t *periph_touch = esp_periph_get_data(self);
    esp_touch_destroy(periph_touch->touch);
    audio_free(periph_touch);
    return ESP_OK;
}

esp_periph_handle_t periph_touch_init(periph_touch_cfg_t *config)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_TOUCH, "periph_touch");
    AUDIO_MEM_CHECK(TAG, periph, return NULL);
    periph_touch_t *periph_touch = audio_calloc(1, sizeof(periph_touch_t));
    AUDIO_MEM_CHECK(TAG, periph_touch, {
        audio_free(periph);
        return NULL;
    });
    periph_touch->touch_mask = config->touch_mask;
    periph_touch->long_tap_time_ms = config->long_tap_time_ms;
    periph_touch->tap_threshold_percent = config->tap_threshold_percent;

    esp_periph_set_data(periph, periph_touch);
    esp_periph_set_function(periph, _touch_init, _touch_run, _touch_destroy);

    return periph;
}
