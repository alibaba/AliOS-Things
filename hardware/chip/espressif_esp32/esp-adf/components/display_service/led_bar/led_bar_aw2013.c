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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include "esp_log.h"
#include "periph_service.h"
#include "periph_aw2013.h"
#include "led_bar_aw2013.h"

#define AW2013_QUE_LENGTH 4
#define AW2013_DEFAULT_BLINK_PERIOD  500
#define AW2013_DEFAULT_BLINK_TIME  3

static const char *TAG = "LED_BAR_AW2013";

static const int DESTROY_BIT = BIT0;
static QueueHandle_t aw2013_que;
static bool _task_run;
static EventGroupHandle_t g_event_bit;

typedef struct {
    esp_periph_handle_t instance;
    display_pattern_t pat;
    int blink_time_period;
    uint8_t blink_time;
} led_bar_aw2013_cmd_t;

typedef enum {
    AW2013_COLOR_RED    = 0x000000ff,
    AW2013_COLOR_GREEN  = 0x00ff0000,
    AW2013_COLOR_BLUE   = 0x0000ff00,
    AW2013_COLOR_WHITE  = 0x00ffffff,
    AW2013_COLOR_YELLOW = 0x00ff00ff,
    AW2013_COLOR_CYAN   = 0x00ffff00,
    AW2013_COLOR_PURPLE = 0x0000ffff,
    AW2013_COLOR_BLACK  = 0x0,
} led_bar_aw2013_color_t;

void aw2013_led_bar_task(void *parameters)
{
    display_pattern_t pat = DISPLAY_PATTERN_UNKNOWN;
    led_bar_aw2013_cmd_t recv_cmd = {0};
    _task_run = true;
    int blink_period = AW2013_DEFAULT_BLINK_PERIOD;
    int blink_time = AW2013_DEFAULT_BLINK_TIME;
    xEventGroupClearBits(g_event_bit, DESTROY_BIT);

    while (_task_run) {
        if (aw2013_que) {
            if (xQueueReceive(aw2013_que, &recv_cmd, 0) == pdPASS) {
                pat = recv_cmd.pat;
                if (recv_cmd.blink_time_period != 0) {
                    blink_period = recv_cmd.blink_time_period;
                }
                if (recv_cmd.blink_time != 0) {
                    blink_time = recv_cmd.blink_time;
                }
            }
            switch (pat) {
                case DISPLAY_PATTERN_UNKNOWN: {
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                        break;
                    }
                case DISPLAY_PATTERN_WIFI_SETTING: { // cyan led flash
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_AUTO);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_CYAN);
                        break;
                    }
                case DISPLAY_PATTERN_WIFI_CONNECTTING: { // green led flash
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_AUTO);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_GREEN);
                        break;
                    }
                case DISPLAY_PATTERN_WIFI_CONNECTED: { // green led blink
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        for (int i = 0; i < blink_time; i++) {
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_GREEN);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                        }
                        break;
                    }
                case DISPLAY_PATTERN_WIFI_DISCONNECTED: { // red led blink
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        for (int i = 0; i < blink_time; i++) {
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_RED);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                        }
                        break;
                    }
                case DISPLAY_PATTERN_WIFI_SETTING_FINISHED: {
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                        break;
                    }
                case DISPLAY_PATTERN_BT_CONNECTTING: { // blue led flash
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_AUTO);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLUE);
                        break;
                    }
                case DISPLAY_PATTERN_BT_CONNECTED: { // blue led blink
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        for (int i = 0; i < blink_time; i++) {
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLUE);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                        }
                        break;
                    }
                case DISPLAY_PATTERN_BT_DISCONNECTED: { // red led blink
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        for (int i = 0; i < blink_time; i++) {
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_RED);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                            periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                            vTaskDelay(blink_period / portTICK_PERIOD_MS);
                        }
                        break;
                    }
                case DISPLAY_PATTERN_WAKEUP_ON: { // white led on
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_WHITE);
                        break;
                    }
                case DISPLAY_PATTERN_WAKEUP_FINISHED: {
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                        break;
                    }
                case DISPLAY_PATTERN_TURN_OFF: {
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_BLACK);
                        break;
                    }
                case DISPLAY_PATTERN_BATTERY_LOW: { // red led on
                        periph_aw2013_set_mode(recv_cmd.instance, AW2013_MODE_LED);
                        periph_aw2013_set_rgb_value(recv_cmd.instance, AW2013_COLOR_RED);
                        break;
                    }
                default: {
                        ESP_LOGW(TAG, "Pat %d hasn't been defined, you can defined it by yourself", pat);
                        break;
                    }
            }
            pat = DISPLAY_PATTERN_UNKNOWN;
        } else {
            ESP_LOGE(TAG, "The msg queue is NULL");
            break;
        }
    }

    if (g_event_bit) {
        xEventGroupSetBits(g_event_bit, DESTROY_BIT);
    }
    vTaskDelete(NULL);
}

esp_periph_handle_t led_bar_aw2013_init(void)
{
    aw2013_que = xQueueCreate(AW2013_QUE_LENGTH, sizeof(led_bar_aw2013_cmd_t));
    AUDIO_NULL_CHECK(TAG, aw2013_que, {
        ESP_LOGE(TAG, "Fail to create queue");
        return NULL;
    });

    g_event_bit = xEventGroupCreate();
    AUDIO_NULL_CHECK(TAG, g_event_bit, {
        ESP_LOGE(TAG, "Fail to create a event group");
        vQueueDelete(aw2013_que);
        aw2013_que = NULL;
        return NULL;
    });

    periph_aw2013_cfg_t aw2013_cfg = {
        .mode = AW2013_MODE_LED,
        .bright = AW2013_BRIGHT_2,
        .rgb_value = AW2013_COLOR_BLACK
    };
    esp_periph_handle_t aw2013_handle = periph_aw2013_init(&aw2013_cfg);
    AUDIO_NULL_CHECK(TAG, aw2013_handle, {
        ESP_LOGE(TAG, "Fail to initialize the aw2013 chip");
        vQueueDelete(aw2013_que);
        aw2013_que = NULL;
        vEventGroupDelete(g_event_bit);
        g_event_bit = NULL;
        return NULL;
    });

    esp_periph_init(aw2013_handle);
    return aw2013_handle;
}

esp_err_t led_bar_aw2013_pattern(void *handle, int pat, int value)
{
    if (handle == NULL || aw2013_que == NULL) {
        ESP_LOGE(TAG, "The handle: %p or queue: %p of aw2013 is NULL", handle, aw2013_que);
        return ESP_FAIL;
    }

    led_bar_aw2013_cmd_t cmd = {
        .instance = handle,
        .pat = pat,
    };
    if (xQueueSend(aw2013_que, &cmd, portMAX_DELAY) != pdPASS) {
        ESP_LOGE(TAG, "Fail to set aw2013's pattern");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t led_bar_aw2013_set_blink_time(void *handle, uint8_t time, int period)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);

    led_bar_aw2013_cmd_t cmd = {
        .instance = handle,
        .blink_time = time,
        .blink_time_period = period
    };
    if (xQueueSend(aw2013_que, &cmd, portMAX_DELAY) != pdPASS) {
        ESP_LOGE(TAG, "Fail to set blink time");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void led_bar_aw2013_deinit(esp_periph_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return);
    _task_run = false;
    if (g_event_bit) {
        xEventGroupWaitBits(g_event_bit, DESTROY_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        vEventGroupDelete(g_event_bit);
        g_event_bit = NULL;
    }
    if (aw2013_que) {
        vQueueDelete(aw2013_que);
        aw2013_que = NULL;
    }
    esp_periph_stop(handle);
    esp_periph_destroy(handle);
}
