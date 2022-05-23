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

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "periph_is31fl3216.h"
#include "IS31FL3216.h"
#include "audio_mem.h"
#include "esp_log.h"

#define IS31FL3216_TASK_STACK_SIZE  (2048 + 1024)
#define IS31FL3216_TASK_PRIORITY    3

#define ONE_FRAME_BYTE_SIZE         18
#define DEFAULT_FLASH_STEP          2

static const char *TAG = "PERIPH_IS31";
static const int DESTROY_BIT = BIT0;

#define VALIDATE_IS31FL3216(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_IS31FL3216)) { \
    ESP_LOGE(TAG, "Invalid is31fl3216 periph, at line %d", __LINE__);\
    return ret;\
}

static const uint8_t light_audio_frames[8][ONE_FRAME_BYTE_SIZE] = {
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xff, 0xff},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xff, 0xff, 0xff},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff, 0xff, 0xff},
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xff, 0xFF, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};

typedef enum {
    PERIPH_IS31_CMD_CHG_STATE,
    PERIPH_IS31_CMD_QUIT,
} periph_is31_cmd_t;

typedef struct {
    uint16_t                    max_light_num;              // Maximum light number
    uint16_t                    light_num;                  // Working lights number
    uint16_t                    light_mask;                 // Light bits mask
    int                         interval_time;              // Interval working time
    uint16_t                    act_time;                   // Action times
    uint8_t                     duty_step;                  // Duty step
    periph_is31_shift_mode_t    shift_mode;                 // Shift mode step
} periph_is31_arg_t;

typedef struct {
    periph_is31_arg_t           *arg;
    uint8_t                     duty[IS31FL3216_CH_NUM];    // Duty of lights
    is31fl3216_handle_t         handle;
    periph_is31fl3216_state_t   cur_state;
    QueueHandle_t               evt;
    EventGroupHandle_t          g_event_bit;
} periph_is31fl3216_t;

typedef struct {
    periph_is31_cmd_t   type;
    uint32_t            data;
} periph_is31_msg_t;

static esp_err_t is31_leds_ctrl(is31fl3216_handle_t *handle, uint16_t mask)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < IS31FL3216_CH_NUM; i++) {
        if (mask & (1UL << i)) {
            ret |= is31fl3216_ch_enable(handle, 1UL << i);
        } else {
            ret |= is31fl3216_ch_disable(handle, 1UL << i);
        }
    }
    return ret;
}

static esp_err_t is31_leds_duty(is31fl3216_handle_t *handle, int duty, uint16_t mask)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < IS31FL3216_CH_NUM; i++) {
        if (mask & (1UL << i))
            ret |= is31fl3216_ch_duty_set(handle, 1UL << i, duty);
    }
    return ret;
}

static void is31_evt_send(void *que, periph_is31_cmd_t type, uint32_t data, int dir)
{
    periph_is31_msg_t evt = {0};
    evt.type = type;
    evt.data = data;
    if (dir) {
        xQueueSendToFront(que, &evt, 0) ;
    } else {
        xQueueSend(que, &evt, 0);
    }
}
static esp_err_t is31_change_state(periph_is31fl3216_t *is31, int state, periph_is31_arg_t *arg)
{
    esp_err_t ret = ESP_OK;
    switch (state) {
        case IS31FL3216_STATE_OFF:
            ret |= is31fl3216_ch_disable(is31->handle, arg->light_mask);
            arg->interval_time = portMAX_DELAY;
            is31->cur_state = IS31FL3216_STATE_OFF;
            break;
        case IS31FL3216_STATE_ON:
            if (is31->cur_state == IS31FL3216_STATE_BY_AUDIO) {
                ret |= is31fl3216_work_mode_set(is31->handle, IS31FL3216_MODE_PWM);
                is31_leds_duty(is31->handle, IS31FL3216_DUTY_MAX, arg->light_mask);
            }
            is31_leds_ctrl(is31->handle, arg->light_mask);
            arg->interval_time = portMAX_DELAY;
            is31->cur_state = IS31FL3216_STATE_ON;
            break;
        case IS31FL3216_STATE_FLASH:
            if (is31->cur_state == IS31FL3216_STATE_BY_AUDIO) {
                ret |= is31fl3216_work_mode_set(is31->handle, IS31FL3216_MODE_PWM);
            }
            is31->cur_state = IS31FL3216_STATE_FLASH;
            break;
        case IS31FL3216_STATE_SHIFT:
            if (is31->cur_state == IS31FL3216_STATE_BY_AUDIO) {
                ret |= is31fl3216_work_mode_set(is31->handle, IS31FL3216_MODE_PWM);
            }
            is31->cur_state = IS31FL3216_STATE_SHIFT;
            break;
        case IS31FL3216_STATE_BY_AUDIO:
            is31fl3216_reset(is31->handle);
            is31fl3216_work_mode_set(is31->handle, IS31FL3216_MODE_FRAME);
            is31fl3216_sample_rate_set(is31->handle, 0xB4); // Set adc sample rate
            is31fl3216_frame_value_set(is31->handle, 1, (uint8_t *)&light_audio_frames, sizeof(light_audio_frames));
            is31fl3216_first_frame_set(is31->handle, 0);
            is31->cur_state = IS31FL3216_STATE_BY_AUDIO;
            arg->interval_time = portMAX_DELAY;
            break;
        default:
            ESP_LOGE(TAG, "State %d is not supported", state);
            break;
    }
    return ret;
}

static void is31fl3216_run_task(void *Para)
{
    esp_periph_handle_t periph = (esp_periph_handle_t) Para;
    periph_is31fl3216_t *is31 = esp_periph_get_data(periph);
    periph_is31_arg_t is31_arg = {
        .max_light_num = IS31FL3216_CH_NUM,
        .light_num = 1,
        .light_mask = 1,
        .interval_time = 1000,
        .act_time = 0,
        .duty_step = DEFAULT_FLASH_STEP,
        .shift_mode = 0,
    };
    periph_is31_msg_t msg = {0};
    int wait_time_ms = portMAX_DELAY;
    bool task_run = true;
    xEventGroupClearBits(is31->g_event_bit, DESTROY_BIT);
    int cur_duty = 0;
    int sig = 2;
    int cur_bits_mask = 0;
    int i = 0;
    uint16_t act_times = 0;
    while (task_run) {
        if (xQueueReceive(is31->evt, &msg, (wait_time_ms / portTICK_PERIOD_MS))) {
            ESP_LOGD(TAG, "cmd:%d, data:%d", msg.type, msg.data);
            switch (msg.type) {
                case PERIPH_IS31_CMD_CHG_STATE:
                    memcpy(&is31_arg, is31->arg, sizeof(periph_is31_arg_t));
                    wait_time_ms = is31->arg->interval_time;

                    memset(is31->arg, 0, sizeof(periph_is31_arg_t));
                    is31->arg->interval_time = portMAX_DELAY;
                    is31->arg->max_light_num = IS31FL3216_CH_NUM;
                    is31->arg->duty_step = DEFAULT_FLASH_STEP;
                    is31_change_state(is31, msg.data, &is31_arg);

                    if (IS31FL3216_STATE_FLASH == msg.data) {
                        sig = is31_arg.duty_step;
                    }
                    if (is31_arg.act_time && wait_time_ms) {
                        act_times = is31_arg.act_time / wait_time_ms;
                    } else {
                        act_times = 0;
                    }
                    break;

                case PERIPH_IS31_CMD_QUIT:
                    task_run = false;
                    if (is31->g_event_bit) {
                        xEventGroupSetBits(is31->g_event_bit, DESTROY_BIT);
                    }
                    break;
                default:
                    break;
            }
            if (task_run == false) {
                ESP_LOGW(TAG, "Quit is31fl3216 task ...");
                break;
            }
        }
        switch (is31->cur_state) {
            case IS31FL3216_STATE_FLASH: {
                    is31_leds_duty(is31->handle, cur_duty, is31_arg.light_mask);
                    is31_leds_ctrl(is31->handle, is31_arg.light_mask);
                    cur_duty += sig;
                    if (cur_duty > IS31FL3216_DUTY_MAX) {
                        cur_duty = IS31FL3216_DUTY_MAX;
                        sig = -(is31_arg.duty_step);
                    }
                    if (cur_duty < 0) {
                        cur_duty = 0;
                        sig = (is31_arg.duty_step);
                    }
                }
                if (is31_arg.act_time == 0) {
                    act_times = 0;
                    break;
                }
                act_times--;
                if (act_times == 0) {
                    wait_time_ms = portMAX_DELAY;
                    is31->cur_state = IS31FL3216_STATE_UNKNOWN;
                    is31_leds_ctrl(is31->handle, 0);
                }
                break;
            case IS31FL3216_STATE_SHIFT:
                if (is31_arg.shift_mode == PERIPH_IS31_SHIFT_MODE_SINGLE) {
                    cur_bits_mask = ((1UL << is31_arg.light_num) - 1) << (i++);
                    if (i == (is31_arg.max_light_num - is31_arg.light_num + 1)) {
                        i = 0;
                    }
                } else if (is31_arg.shift_mode == PERIPH_IS31_SHIFT_MODE_ACC) {
                    cur_bits_mask = (1UL << (is31_arg.light_num * ((i++) + 1))) - 1;
                    if ((cur_bits_mask >> is31_arg.max_light_num) & 0x01) {
                        cur_bits_mask = 0;
                        i = 0;
                    }
                }
                is31_leds_duty(is31->handle, IS31FL3216_DUTY_MAX, cur_bits_mask);
                is31_leds_ctrl(is31->handle, cur_bits_mask);

                ESP_LOGD(TAG, "Mask:%08x, %d", cur_bits_mask, wait_time_ms);
                if (is31_arg.act_time == 0) {
                    act_times = 0;
                    break;
                }
                act_times--;
                if (act_times == 0) {
                    wait_time_ms = portMAX_DELAY;
                    is31->cur_state = IS31FL3216_STATE_UNKNOWN;
                    is31_leds_ctrl(is31->handle, 0);
                }

                break;
            default:
                break;
        }
    }
    vTaskDelete(NULL);
}

esp_err_t periph_is31fl3216_set_state(esp_periph_handle_t periph, periph_is31fl3216_state_t state)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31_evt_send(is31fl3216->evt, PERIPH_IS31_CMD_CHG_STATE, state, 0);
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_blink_pattern(esp_periph_handle_t periph, uint16_t blink_pattern)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->light_mask = blink_pattern;
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_duty(esp_periph_handle_t periph, uint8_t index, uint8_t value)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->duty[index] = value;
    is31fl3216_ch_duty_set(is31fl3216->handle, 1UL << index, is31fl3216->duty[index]);
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_duty_step(esp_periph_handle_t periph, uint8_t step)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->duty_step = step;
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_interval(esp_periph_handle_t periph, uint16_t interval_ms)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->interval_time = interval_ms;
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_shift_mode(esp_periph_handle_t periph, periph_is31_shift_mode_t mode)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->shift_mode = mode;
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_light_on_num(esp_periph_handle_t periph, uint16_t light_on_num, uint16_t max_light_num)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->max_light_num = max_light_num;
    is31fl3216->arg->light_num = light_on_num;
    return ESP_OK;
}

esp_err_t periph_is31fl3216_set_act_time(esp_periph_handle_t periph, uint16_t act_ms)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(periph);
    is31fl3216->arg->act_time = act_ms;
    return ESP_OK;
}

static esp_err_t _is31fl3216_init(esp_periph_handle_t self)
{
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(self);
    esp_err_t ret = ESP_OK;
    is31fl3216_ch_disable(is31fl3216->handle, IS31FL3216_CH_ALL);
    is31_leds_duty(is31fl3216->handle, 0, IS31FL3216_CH_ALL);
    xTaskCreate(is31fl3216_run_task, "is31fl3216_run_task", IS31FL3216_TASK_STACK_SIZE, (void *)self, IS31FL3216_TASK_PRIORITY, NULL);
    if (ret) {
        ESP_LOGE(TAG, "Failed to initialize is31fl3216");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t _is31fl3216_destroy(esp_periph_handle_t self)
{
    VALIDATE_IS31FL3216(self, ESP_FAIL);
    periph_is31fl3216_t *is31fl3216 = esp_periph_get_data(self);
    is31_evt_send(is31fl3216->evt, PERIPH_IS31_CMD_QUIT, 0, 0);
    if (is31fl3216->g_event_bit) {
        xEventGroupWaitBits(is31fl3216->g_event_bit, DESTROY_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        vEventGroupDelete(is31fl3216->g_event_bit);
        is31fl3216->g_event_bit = NULL;
    }
    esp_err_t ret = ESP_OK;
    ret |= is31fl3216_ch_disable(is31fl3216->handle, IS31FL3216_CH_ALL);
    ret |= is31fl3216_deinit(is31fl3216->handle);
    audio_free(is31fl3216->arg);
    vQueueDelete(is31fl3216->evt);
    audio_free(is31fl3216);
    if (ret) {
        ESP_LOGE(TAG, "Error occurred when stopping the is31fl3216");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_periph_handle_t periph_is31fl3216_init(periph_is31fl3216_cfg_t *is31fl3216_config)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_IS31FL3216, "periph_is31fl3216");
    AUDIO_MEM_CHECK(TAG, periph, return NULL);

    periph_is31fl3216_t *is31fl3216 = audio_calloc(1, sizeof(periph_is31fl3216_t));
    AUDIO_MEM_CHECK(TAG, is31fl3216, {
        audio_free(periph);
        return NULL;
    });

    is31fl3216->g_event_bit = xEventGroupCreate();
    AUDIO_NULL_CHECK(TAG, is31fl3216->g_event_bit, {
        audio_free(periph);
        audio_free(is31fl3216);
    });

    is31fl3216->evt = xQueueCreate(2, sizeof(periph_is31_msg_t));
    AUDIO_MEM_CHECK(TAG, is31fl3216->evt, {
        audio_free(periph);
        vEventGroupDelete(is31fl3216->g_event_bit);
        audio_free(is31fl3216);
        return NULL;
    });
    is31fl3216->arg = audio_calloc(1, sizeof(periph_is31_arg_t));
    AUDIO_MEM_CHECK(TAG, is31fl3216->arg, {
        vQueueDelete(is31fl3216->evt);
        vEventGroupDelete(is31fl3216->g_event_bit);
        audio_free(periph);
        audio_free(is31fl3216);
        return NULL;
    });
    is31fl3216->arg->max_light_num = IS31FL3216_CH_NUM;
    is31fl3216->arg->light_num = 0;
    is31fl3216->arg->light_mask = 0;
    is31fl3216->arg->interval_time = 1000;
    is31fl3216->arg->act_time = 0;
    is31fl3216->arg->duty_step = DEFAULT_FLASH_STEP;
    is31fl3216->arg->shift_mode = PERIPH_IS31_SHIFT_MODE_ACC;

    if (is31fl3216_config->duty == NULL) {
        ESP_LOGW(TAG, "The duty array is NULL");
    } else {
        for (int i = 0; i < IS31FL3216_CH_NUM; i++) {
            is31fl3216->duty[i] = is31fl3216_config->duty[i];
        }
    }
    is31fl3216->handle = is31fl3216_init();
    AUDIO_MEM_CHECK(TAG, is31fl3216, {
        audio_free(is31fl3216->arg);
        vQueueDelete(is31fl3216->evt);
        audio_free(periph);
        vEventGroupDelete(is31fl3216->g_event_bit);
        audio_free(is31fl3216);
        return NULL;
    });

    esp_periph_set_data(periph, is31fl3216);
    esp_periph_set_function(periph, _is31fl3216_init, NULL, _is31fl3216_destroy);
    return periph;
}