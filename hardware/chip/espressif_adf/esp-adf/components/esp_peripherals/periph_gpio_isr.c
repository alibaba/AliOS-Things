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
#include "esp_log.h"
#include "driver/gpio.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "sys/queue.h"
#include "esp_peripherals.h"
#include "periph_gpio_isr.h"

static const char *TAG = "PERIPH_GPIO_ISR";

typedef struct gpio_info_node {
    gpio_isr_info_t gpio_info;
    STAILQ_ENTRY(gpio_info_node) entries;
} gpio_isr_node_t;

static esp_periph_handle_t g_handle = NULL;
static STAILQ_HEAD(gpio_isr_list, gpio_info_node) gpio_isr_info_list;

static void IRAM_ATTR gpio_isr_handler(void *param)
{
    int gpio_num = (int)param;
    esp_periph_send_cmd_from_isr(g_handle, gpio_num, NULL, 0);
}

static esp_err_t _gpio_isr_init(esp_periph_handle_t self)
{
    esp_err_t ret = ESP_OK;
    gpio_isr_node_t *tmp_node = NULL;
    gpio_isr_info_t *tmp_info = NULL;

    STAILQ_FOREACH(tmp_node, &gpio_isr_info_list, entries) {
        tmp_info = &tmp_node->gpio_info;
        ret |= gpio_isr_init(tmp_info->gpio_num, tmp_info->type, gpio_isr_handler, (void *)tmp_info->gpio_num);
    }

    return ret;
}

static esp_err_t _gpio_isr_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    esp_err_t ret = ESP_OK;
    if (msg->cmd >= 0) {
        ret = esp_periph_send_event(self, 0, (void *)msg->cmd, 0);
        return ret;
    }

    return ESP_FAIL;
}

static esp_err_t _gpio_isr_destory(esp_periph_handle_t self)
{
    esp_err_t ret = ESP_OK;
    gpio_isr_node_t *tmp, *item;
    gpio_isr_info_t *tmp_info = NULL;

    STAILQ_FOREACH_SAFE(item, &gpio_isr_info_list, entries, tmp) {
        tmp_info = &item->gpio_info;
        ret |= gpio_isr_deinit(tmp_info->gpio_num);
        STAILQ_REMOVE(&gpio_isr_info_list, item, gpio_info_node, entries);
        audio_free(item);
    }

    return ret;
}

esp_err_t periph_gpio_isr_add(gpio_isr_info_t *gpio_info)
{
    AUDIO_NULL_CHECK(TAG, gpio_info, return ESP_FAIL);
    gpio_isr_node_t *tmp_node = NULL;
    gpio_isr_info_t *tmp_info = NULL;

    STAILQ_FOREACH(tmp_node, &gpio_isr_info_list, entries) {
        tmp_info = &tmp_node->gpio_info;
        if (tmp_info->gpio_num == gpio_info->gpio_num) {
            ESP_LOGW(TAG, "The gpio has already registered isr");
            return ESP_FAIL;
        }
    }

    gpio_isr_node_t *gpio_isr_node = (gpio_isr_node_t *)audio_calloc(1, sizeof(gpio_isr_node_t));
    AUDIO_NULL_CHECK(TAG, gpio_isr_node, return ESP_FAIL);
    memcpy(gpio_isr_node, gpio_info, sizeof(gpio_isr_info_t));
    STAILQ_INSERT_TAIL(&gpio_isr_info_list, gpio_isr_node, entries);
    return gpio_isr_init(gpio_info->gpio_num, gpio_info->type, gpio_isr_handler, (void *)gpio_info->gpio_num);
}

esp_err_t periph_gpio_isr_delete(int gpio_num)
{
    esp_err_t ret = ESP_OK;
    if (gpio_num < 0) {
        ESP_LOGW(TAG, "The gpio number should be greater than 0");
    }

    gpio_isr_node_t *tmp_node = NULL;
    gpio_isr_info_t *tmp_info = NULL;
    STAILQ_FOREACH(tmp_node, &gpio_isr_info_list, entries) {
        tmp_info = &tmp_node->gpio_info;
        if (tmp_info->gpio_num == gpio_num) {
            STAILQ_REMOVE(&gpio_isr_info_list, tmp_node, gpio_info_node, entries);
            ret |= gpio_isr_deinit(tmp_info->gpio_num);
            audio_free(tmp_node);
            return ret;
        }
    }
    ESP_LOGW(TAG, "The gpio %d hasn't been registered", gpio_num);
    return ESP_FAIL;
}

esp_periph_handle_t periph_gpio_isr_init(periph_gpio_isr_cfg_t *isr_config)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_GPIO_ISR, "periph_gpio_isr");
    AUDIO_NULL_CHECK(TAG, periph, return NULL);

    STAILQ_INIT(&gpio_isr_info_list);

    if (isr_config) {
        for (int i = 0; i < isr_config->info_size; i++) {
            gpio_isr_node_t *gpio_isr_node = (gpio_isr_node_t *)audio_calloc(1, sizeof(gpio_isr_node_t));
            AUDIO_NULL_CHECK(TAG, gpio_isr_node, {
                audio_free(periph);
                return NULL;
            });
            memcpy(gpio_isr_node, &isr_config->gpio_isr_info[i], sizeof(gpio_isr_info_t));
            STAILQ_INSERT_TAIL(&gpio_isr_info_list, gpio_isr_node, entries);
        }
    }

    esp_periph_set_data(periph, &gpio_isr_info_list);
    esp_periph_set_function(periph, _gpio_isr_init, _gpio_isr_run, _gpio_isr_destory);
    g_handle = periph;
    return periph;
}
