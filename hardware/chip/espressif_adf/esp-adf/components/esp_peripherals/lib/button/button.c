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
#include "sys/queue.h"
#include "button.h"
#include "audio_mem.h"

#ifdef periph_tick_get
#define tick_get periph_tick_get
#else
static long long tick_get()
{
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
    return milliseconds;
}
#endif

static const char *TAG = "BUTTON";

typedef struct esp_button_item {
    int                         gpio_num;
    long long                   last_press_tick;
    bool                        long_pressed;
    STAILQ_ENTRY(esp_button_item)    entry;
} esp_button_item_t;

struct esp_button {
    int long_press_time_ms;
    uint64_t gpio_mask;
    STAILQ_HEAD(esp_button_list, esp_button_item) btn_list;
};

static button_status_t button_get_state(esp_button_handle_t button, esp_button_item_t *btn_item)
{
    int level = gpio_get_level(btn_item->gpio_num);
    int active_level = 0;
    int deactive_level = 1;

    if (btn_item->last_press_tick == 0 && level == active_level) {
        btn_item->last_press_tick = tick_get();
        btn_item->long_pressed = false;
        return BTN_PRESSED;
    }

    if (level == deactive_level && btn_item->last_press_tick && tick_get() - btn_item->last_press_tick > button->long_press_time_ms) {
        btn_item->last_press_tick = 0;
        btn_item->long_pressed = false;
        return BTN_LONG_RELEASE;
    }

    if (level == deactive_level && btn_item->last_press_tick) {
        btn_item->last_press_tick = 0;
        btn_item->long_pressed = false;
        return BTN_RELEASE;
    }

    if (btn_item->long_pressed == false && level == active_level && tick_get() - btn_item->last_press_tick > button->long_press_time_ms) {
        btn_item->long_pressed = true;
        return BTN_LONG_PRESS;
    }
    return BTN_UNCHANGE;
}

esp_button_handle_t button_init(button_config_t *config)
{
    esp_button_handle_t btn = audio_calloc(1, sizeof(struct esp_button));
    AUDIO_MEM_CHECK(TAG, btn, return NULL);
    if (config->gpio_mask <= 0) {
        ESP_LOGE(TAG, "required at least 1 gpio");
        return NULL;
    }
    btn->gpio_mask = config->gpio_mask;
    btn->long_press_time_ms = config->long_press_time_ms;

    if (btn->long_press_time_ms == 0) {
        btn->long_press_time_ms = DEFAULT_LONG_PRESS_TIME_MS;
    }

    gpio_config_t gpiocfg = {
        .pin_bit_mask = btn->gpio_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&gpiocfg);

    uint64_t gpio_mask = btn->gpio_mask;
    int gpio_num = 0;

    STAILQ_INIT(&btn->btn_list);
    while (gpio_mask) {
        if (gpio_mask & 0x01) {
            ESP_LOGD(TAG, "Mask = %llx, current_mask = %llx, idx=%d", btn->gpio_mask, gpio_mask, gpio_num);
            esp_button_item_t *new_btn = audio_calloc(1, sizeof(esp_button_item_t));
            AUDIO_MEM_CHECK(TAG, new_btn, {
                button_destroy(btn);
                return NULL;
            });
            new_btn->gpio_num = gpio_num;
            if (config->button_intr_handler) {
                gpio_set_intr_type(gpio_num, GPIO_INTR_ANYEDGE);
                gpio_isr_handler_add(gpio_num, config->button_intr_handler, config->intr_context);
                gpio_intr_enable(gpio_num);
            }
            STAILQ_INSERT_TAIL(&btn->btn_list, new_btn, entry);
        }
        gpio_mask >>= 1;
        gpio_num ++;
    }
    return btn;
}

bool button_read(esp_button_handle_t button, button_result_t *result)
{
    esp_button_item_t *btn_item;
    button_status_t btn_status;
    bool changed = false;
    memset(result, 0, sizeof(button_result_t));
    uint64_t tmp;
    STAILQ_FOREACH(btn_item, &button->btn_list, entry) {
        btn_status = button_get_state(button, btn_item);
        switch (btn_status) {
            case BTN_UNCHANGE:
                break;
            case BTN_PRESSED:
                changed = true;
                tmp = 0x01;
                tmp <<= btn_item->gpio_num;
                result->press_mask |= tmp;
                break;
            case BTN_RELEASE:
                changed = true;
                tmp = 0x01;
                tmp <<= btn_item->gpio_num;
                result->release_mask |=  tmp;
                break;
            case BTN_LONG_RELEASE:
                changed = true;
                tmp = 0x01;
                tmp <<= btn_item->gpio_num;
                result->long_release_mask |=  tmp;
                break;
            case BTN_LONG_PRESS:
                changed = true;
                tmp = 0x01;
                tmp <<= btn_item->gpio_num;
                result->long_press_mask |=  tmp;
                break;
        }
    }
    return changed;
}

esp_err_t button_destroy(esp_button_handle_t button)
{
    esp_button_item_t *btn_item, *tmp;
    STAILQ_FOREACH_SAFE(btn_item, &button->btn_list, entry, tmp) {
        gpio_intr_disable(btn_item->gpio_num);
        gpio_isr_handler_remove(btn_item->gpio_num);
        STAILQ_REMOVE(&button->btn_list, btn_item, esp_button_item, entry);
        audio_free(btn_item);
    }
    audio_free(button);
    return ESP_OK;
}
