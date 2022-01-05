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

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "sys/queue.h"
#include "touch.h"
#include "sdkconfig.h"
#include "audio_mem.h"

#define TOUCHPAD_TRIGGER_THRESHOLD      100
#define TOUCHPAD_FILTER_PERIOD          (30)
#define TOUCHPAD_READ_INTERVAL_MS       (TOUCHPAD_FILTER_PERIOD*4)
#define TOUCHPAD_INTIALIZE_TIME_MS      1000
#define UPDATE_THRESHOLD_PERIOD_MS      200

static const char *TAG = "TOUCH";

typedef struct esp_touch_item {
    int                             touch_num;
    long long                       last_tap_tick;
    long long                       update_threshold_tick;
    long long                       last_read_tick;
    uint16_t                        last_read_value;
    uint16_t                        untouch_value;
    uint16_t                        threshold_value;
    bool                            long_tapped;
    bool                            tapped;
    STAILQ_ENTRY(esp_touch_item)    entry;
} esp_touch_item_t;

struct esp_touch {
    int long_tap_time_ms;
    int touch_mask;
    int tap_threshold_percent;
    touch_intr_handler intr_fn;
    void *intr_context;
    STAILQ_HEAD(esp_touch_list, esp_touch_item) touch_list;
};

#ifdef periph_tick_get
#define tick_get periph_tick_get
#else
static long long tick_get()
{
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}
#endif

static void touch_pad_isr_handler(void* arg)
{
    esp_touch_handle_t touch = (esp_touch_handle_t)arg;
#if CONFIG_IDF_TARGET_ESP32
    touch_pad_clear_status();
#endif
    if (touch->intr_fn) {
        touch->intr_fn(touch->intr_context);
    }
}

esp_touch_handle_t esp_touch_init(touch_config_t *config)
{
    esp_touch_handle_t touch = audio_calloc(1, sizeof(struct esp_touch));
    AUDIO_MEM_CHECK(TAG, touch, return NULL);

    if (config->touch_mask <= 0) {
        ESP_LOGE(TAG, "required at least 1 touch");
        return NULL;
    }

    touch->touch_mask = config->touch_mask;
    touch->long_tap_time_ms = config->long_tap_time_ms;
    touch->tap_threshold_percent = config->tap_threshold_percent;

    if (touch->long_tap_time_ms == 0) {
        touch->long_tap_time_ms = DEFAULT_LONG_TAP_TIME_MS;
    }

    if (touch->tap_threshold_percent == 0) {
        touch->tap_threshold_percent = DEFAULT_TOUCH_THRESHOLD_PERCENT;
    }
    bool _success = (touch_pad_init() == ESP_OK);

    AUDIO_MEM_CHECK(TAG, _success, {
        audio_free(touch);
        return NULL;
    });

    int touch_mask = touch->touch_mask;
    int touch_num = 0;
    int touch_index = 0;
    STAILQ_INIT(&touch->touch_list);
    while (touch_mask) {
        if (touch_mask & 0x01) {
            ESP_LOGD(TAG, "Mask = %x, current_mask = %x, idx=%d", touch->touch_mask, touch_mask, touch_num);
            esp_touch_item_t *new_touch = audio_calloc(1, sizeof(esp_touch_item_t));
            AUDIO_MEM_CHECK(TAG, new_touch, {
                esp_touch_destroy(touch);
                audio_free(touch);
                return NULL;
            });
            new_touch->touch_num = touch_num;
            new_touch->last_read_tick = tick_get() + touch_index * 10;
#if CONFIG_IDF_TARGET_ESP32
            touch_pad_config(touch_num, 0);
#elif CONFIG_IDF_TARGET_ESP32S2
            touch_pad_config(touch_num);
#endif
            if (config->touch_intr_handler) {
                touch_pad_set_thresh(touch_num, TOUCHPAD_TRIGGER_THRESHOLD);
            }
            STAILQ_INSERT_TAIL(&touch->touch_list, new_touch, entry);
            touch_index ++;
        }
        touch_mask >>= 1;
        touch_num ++;
    }
    touch->intr_fn = config->touch_intr_handler;
    touch->intr_context = config->intr_context;

    if (config->touch_intr_handler) {
#if CONFIG_IDF_TARGET_ESP32
        touch_pad_isr_register(touch_pad_isr_handler, touch);
        touch_pad_intr_enable();
#elif CONFIG_IDF_TARGET_ESP32S2
        touch_pad_isr_register(touch_pad_isr_handler, touch, TOUCH_PAD_INTR_MASK_ALL);
        touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ALL);
#endif

    }
#if CONFIG_IDF_TARGET_ESP32
    touch_pad_filter_start(TOUCHPAD_FILTER_PERIOD);
#endif
    return touch;
}

static touch_status_t touch_get_state(esp_touch_handle_t touch, esp_touch_item_t *touch_item, long long tick)
{
    if (tick - touch_item->last_read_tick < TOUCHPAD_READ_INTERVAL_MS) {
        return TOUCH_UNCHANGE;
    }

    touch_item->last_read_tick = tick;
    esp_err_t err = ESP_OK;
#if CONFIG_IDF_TARGET_ESP32
    err = touch_pad_read_filtered(touch_item->touch_num, &touch_item->last_read_value);
#elif CONFIG_IDF_TARGET_ESP32S2
    err = ESP_OK;
#endif


    if (err != ESP_OK) {
        return TOUCH_UNCHANGE;
    }
    if (touch_item->untouch_value == 0) {
        touch_item->untouch_value = touch_item->last_read_value;
        int threshold_value = touch_item->untouch_value * touch->tap_threshold_percent / 100;
        touch_item->threshold_value = threshold_value;
    }

    if (!touch_item->tapped && touch_item->last_read_value < touch_item->threshold_value) {
        touch_item->tapped = true;

    } else if (touch_item->tapped && touch_item->last_read_value > touch_item->threshold_value) {
        touch_item->tapped = false;
    }

    // Update touch threshold
    if (tick - touch_item->update_threshold_tick > UPDATE_THRESHOLD_PERIOD_MS && !touch_item->tapped) {
        touch_item->update_threshold_tick = tick;
        touch_item->untouch_value += touch_item->last_read_value;
        touch_item->untouch_value /= 2;
        int threshold_value = touch_item->untouch_value * touch->tap_threshold_percent / 100;
        touch_item->threshold_value = threshold_value;
        // ESP_LOGD(TAG, "UPDATE THRESHOLD[%d]=%d", touch_item->touch_num, threshold_value);
    }


    if (touch_item->last_tap_tick == 0 && touch_item->tapped) {
        touch_item->last_tap_tick = tick_get();
        touch_item->long_tapped = false;
        ESP_LOGD(TAG, "TOUCH_TAPPED[%d] %d, threshold %d",
                 touch_item->touch_num, touch_item->last_read_value, touch_item->threshold_value);
        return TOUCH_TAP;
    }

    if (!touch_item->tapped && touch_item->last_tap_tick && tick_get() - touch_item->last_tap_tick > touch->long_tap_time_ms) {
        touch_item->last_tap_tick = 0;
        touch_item->long_tapped = false;
        ESP_LOGD(TAG, "TOUCH_LONG_RELEASE[%d] %d, threshold %d",
                 touch_item->touch_num, touch_item->last_read_value, touch_item->threshold_value);
        return TOUCH_LONG_RELEASE;
    }

    if (!touch_item->tapped && touch_item->last_tap_tick) {
        touch_item->last_tap_tick = 0;
        touch_item->long_tapped = false;
        ESP_LOGD(TAG, "TOUCH_RELEASE[%d] %d, threshold %d",
                 touch_item->touch_num, touch_item->last_read_value, touch_item->threshold_value);
        return TOUCH_RELEASE;
    }

    if (touch_item->long_tapped == false && touch_item->tapped && tick_get() - touch_item->last_tap_tick > touch->long_tap_time_ms) {
        touch_item->long_tapped = true;
        ESP_LOGD(TAG, "TOUCH_LONG_TAP[%d] %d, threshold %d",
                 touch_item->touch_num, touch_item->last_read_value, touch_item->threshold_value);
        return TOUCH_LONG_TAP;
    }
    return TOUCH_UNCHANGE;
}

bool esp_touch_read(esp_touch_handle_t touch, touch_result_t *result)
{
    esp_touch_item_t *touch_item;
    touch_status_t touch_status;
    bool changed = false;
    memset(result, 0, sizeof(touch_result_t));
    int tmp;
    long long tick = tick_get();
    STAILQ_FOREACH(touch_item, &touch->touch_list, entry) {
        touch_status = touch_get_state(touch, touch_item, tick);
        switch (touch_status) {
            case TOUCH_UNCHANGE:
                break;
            case TOUCH_TAP:
                changed = true;
                tmp = 0x01;
                tmp <<= touch_item->touch_num;
                result->tap_mask |= tmp;
                break;
            case TOUCH_RELEASE:
                changed = true;
                tmp = 0x01;
                tmp <<= touch_item->touch_num;
                result->release_mask |=  tmp;
                break;
            case TOUCH_LONG_RELEASE:
                changed = true;
                tmp = 0x01;
                tmp <<= touch_item->touch_num;
                result->long_release_mask |=  tmp;
                break;
            case TOUCH_LONG_TAP:
                changed = true;
                tmp = 0x01;
                tmp <<= touch_item->touch_num;
                result->long_tap_mask |=  tmp;
                break;
        }
    }
    return changed;
}

esp_err_t esp_touch_destroy(esp_touch_handle_t touch)
{
    esp_touch_item_t *touch_item, *tmp;
#if CONFIG_IDF_TARGET_ESP32
    touch_pad_filter_delete();
    touch_pad_intr_disable();
#elif CONFIG_IDF_TARGET_ESP32S2
    touch_pad_intr_disable(TOUCH_PAD_INTR_MASK_ALL);
#endif

    touch_pad_isr_deregister(touch_pad_isr_handler, touch);
    STAILQ_FOREACH_SAFE(touch_item, &touch->touch_list, entry, tmp) {
        STAILQ_REMOVE(&touch->touch_list, touch_item, esp_touch_item, entry);
        audio_free(touch_item);
    }
    touch_pad_deinit();
    audio_free(touch);
    return ESP_OK;
}
