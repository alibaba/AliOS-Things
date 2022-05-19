/*
 *  * ESPRESSIF MIT License
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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "audio_mem.h"
#include "audio_sys.h"
#include "audio_error.h"
#include "audio_element.h"
#include "esp_log.h"
#include "esp_err.h"

#include "soc/ledc_struct.h"
#include "soc/ledc_reg.h"
#include "pwm_stream.h"

static const char *TAG = "PWM_STREAM";

#define BUFFER_MIN_SIZE (256UL)
#define SAMPLE_RATE_MAX (48000)
#define SAMPLE_RATE_MIN (8000)
#define CHANNEL_LEFT_INDEX  (0)
#define CHANNEL_RIGHT_INDEX (1)
#define CHANNEL_LEFT_MASK   (0x01)
#define CHANNEL_RIGHT_MASK  (0x02)
#define AUDIO_PWM_CH_MAX (2)

typedef struct {
    char *buf;                         /**< Original pointer */
    uint32_t volatile head;            /**< ending pointer */
    uint32_t volatile tail;            /**< Read pointer */
    uint32_t size;                     /**< Buffer size */
    uint32_t is_give;                  /**< semaphore give flag */
    SemaphoreHandle_t semaphore;       /**< Semaphore for data */
} data_list_t;
typedef data_list_t *pwm_data_handle_t;

typedef enum {
    AUDIO_PWM_STATUS_UNINIT = 0,    /*!< pwm audio uninitialized */
    AUDIO_PWM_STATUS_IDLE   = 1,    /*!< pwm audio idle */
    AUDIO_PWM_STATUS_BUSY   = 2,    /*!< pwm audio busy */
} audio_pwm_status_t;

typedef struct {
    audio_pwm_config_t    config;                          /**< pwm audio config struct */
    ledc_channel_config_t ledc_channel[AUDIO_PWM_CH_MAX];  /**< ledc channel config */
    ledc_timer_config_t   ledc_timer;                      /**< ledc timer config  */
    timg_dev_t            *timg_dev;                       /**< timer group register pointer */
    pwm_data_handle_t     data;                            /**< audio data pointer */
    uint32_t              channel_mask;                    /**< channel gpio mask */
    uint32_t              channel_set_num;                 /**< channel audio set number */
    int32_t               framerate;                       /*!< frame rates in Hz */
    int32_t               bits_per_sample;                 /*!< bits per sample (16, 32) */
    audio_pwm_status_t    status;
} audio_pwm_t;
typedef audio_pwm_t *audio_pwm_handle_t;

static volatile uint32_t *g_ledc_left_conf0_val  = NULL;
static volatile uint32_t *g_ledc_left_conf1_val  = NULL;
static volatile uint32_t *g_ledc_left_duty_val   = NULL;
static volatile uint32_t *g_ledc_right_conf0_val = NULL;
static volatile uint32_t *g_ledc_right_conf1_val = NULL;
static volatile uint32_t *g_ledc_right_duty_val  = NULL;
static audio_pwm_handle_t g_audio_pwm_handle = NULL;

typedef struct pwm_stream {
    audio_stream_type_t type;
    pwm_stream_cfg_t    config;
    bool                is_open;
    bool                uninstall_drv;
} pwm_stream_t;

static esp_err_t pwm_data_list_destroy(pwm_data_handle_t data)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (data->buf) {
        audio_free(data->buf);
    }
    if (data->semaphore) {
        vSemaphoreDelete(data->semaphore);
    }
    audio_free(data);
    data = NULL;
    return ESP_OK;
}

static pwm_data_handle_t pwm_data_list_create(int size)
{
    if (size < (BUFFER_MIN_SIZE << 2)) {
        ESP_LOGE(TAG, "Invalid buffer size, Minimum = %d", (int32_t)(BUFFER_MIN_SIZE << 2));
        return NULL;
    }

    pwm_data_handle_t data = audio_calloc(1, sizeof(data_list_t));
    AUDIO_NULL_CHECK(TAG, data, goto data_error);

    data->buf = audio_calloc(1, size);
    AUDIO_NULL_CHECK(TAG, data->buf, goto data_error);

    data->semaphore = xSemaphoreCreateBinary();
    AUDIO_NULL_CHECK(TAG, data->semaphore, goto data_error);

    data->is_give = 0;
    data->head = data->tail = 0;
    data->size = size;
    return data;

data_error:
    if (data->semaphore != NULL) {
        vSemaphoreDelete(data->semaphore);
        data->semaphore = NULL;
    }
    if (data->buf) {
        audio_free(data->buf);
        data->buf = NULL;
    }
    if (data) {
        audio_free(data);
        data = NULL;
    }
    return NULL;
}

static uint32_t IRAM_ATTR pwm_data_list_get_count(pwm_data_handle_t data)
{
    uint32_t tail = data->tail;
    if (data->head >= tail) {
        return (data->head - tail);
    }
    return (data->size - (tail - data->head));
}

static uint32_t IRAM_ATTR pwm_data_list_get_free(pwm_data_handle_t data)
{
    return (data->size - pwm_data_list_get_count(data) - 1);
}

static esp_err_t pwm_data_list_flush(pwm_data_handle_t data)
{
    data->tail = data->head = 0;
    return ESP_OK;
}

static esp_err_t IRAM_ATTR pwm_data_list_read_byte(pwm_data_handle_t data, uint8_t *outdata)
{
    uint32_t tail = data->tail;
    if ((tail == data->head) || (tail == (data->head + 1))) {
        return ESP_FAIL;
    }
    *outdata = data->buf[tail];
    tail++;
    if (tail == data->size) {
        tail = 0;
    }

    data->tail = tail;
    return ESP_OK;
}

static esp_err_t pwm_data_list_write_byte(pwm_data_handle_t data, const uint8_t indata)
{
    uint32_t next_head = data->head + 1;
    if (next_head == data->size) {
        next_head = 0;
    }
    if (next_head == data->tail) {
        return ESP_FAIL;
    }
    data->buf[data->head] = indata;
    data->head = next_head;
    return ESP_OK;
}

static esp_err_t pwm_data_list_wait_semaphore(pwm_data_handle_t data, TickType_t ticks_to_wait)
{
    data->is_give = 0;
    if (xSemaphoreTake(data->semaphore, ticks_to_wait) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

static inline void ledc_set_left_duty_fast(uint32_t duty_val)
{
    *g_ledc_left_duty_val = (duty_val) << 4;
    *g_ledc_left_conf0_val |= 0x00000014;
    *g_ledc_left_conf1_val |= 0x80000000;
}

static inline void ledc_set_right_duty_fast(uint32_t duty_val)
{
    *g_ledc_right_duty_val = (duty_val) << 4;
    *g_ledc_right_conf0_val |= 0x00000014;
    *g_ledc_right_conf1_val |= 0x80000000;
}

static void IRAM_ATTR timer_group_isr(void *para)
{
    audio_pwm_handle_t handle = g_audio_pwm_handle;

    if (handle == NULL) {
        return;
    }

    if (handle->timg_dev->int_st_timers.val & BIT(handle->config.timer_num)) {
        handle->timg_dev->int_clr_timers.val |= (1UL << handle->config.timer_num);
    }
#ifdef CONFIG_IDF_TARGET_ESP32S2
    handle->timg_dev->hw_timer[handle->config.timer_num].config.tx_alarm_en = TIMER_ALARM_EN;
#elif CONFIG_IDF_TARGET_ESP32
    handle->timg_dev->hw_timer[handle->config.timer_num].config.alarm_en = TIMER_ALARM_EN;
#elif CONFIG_IDF_TARGET_ESP32S3
    handle->timg_dev->hw_timer[handle->config.timer_num].config.tn_alarm_en = TIMER_ALARM_EN;
#endif

    static uint8_t wave_h, wave_l;
    static uint16_t value;

    if (handle->channel_mask & CHANNEL_LEFT_MASK) {
        if (handle->config.duty_resolution > 8) {
            pwm_data_list_read_byte(handle->data, &wave_l);
            if (ESP_OK == pwm_data_list_read_byte(handle->data, &wave_h)) {
                value = ((wave_h << 8) | wave_l);
                ledc_set_left_duty_fast(value);
            }
        } else {
            if (ESP_OK == pwm_data_list_read_byte(handle->data, &wave_h)) {
                ledc_set_left_duty_fast(wave_h);
            }
        }
    }

    if (handle->channel_mask & CHANNEL_RIGHT_MASK) {
        if (handle->channel_set_num == 1) {
            if (handle->config.duty_resolution > 8) {
                ledc_set_right_duty_fast(value);
            } else {
                ledc_set_right_duty_fast(wave_h);
            }
        } else {
            if (handle->config.duty_resolution > 8) {
                pwm_data_list_read_byte(handle->data, &wave_l);
                if (ESP_OK == pwm_data_list_read_byte(handle->data, &wave_h)) {
                    value = ((wave_h << 8) | wave_l);
                    ledc_set_right_duty_fast(value);
                }
            } else {
                if (ESP_OK == pwm_data_list_read_byte(handle->data, &wave_h)) {
                    ledc_set_right_duty_fast(wave_h);
                }
            }
        }
    } else {
        if (handle->channel_set_num == 2) {
            if (handle->config.duty_resolution > 8) {
                pwm_data_list_read_byte(handle->data, &wave_h);
                pwm_data_list_read_byte(handle->data, &wave_h);
            } else {
                pwm_data_list_read_byte(handle->data, &wave_h);
            }
            pwm_data_list_read_byte(handle->data, &wave_l);
        }
    }

    if (0 == handle->data->is_give && pwm_data_list_get_free(handle->data) > BUFFER_MIN_SIZE) {
        handle->data->is_give = 1;
        BaseType_t xHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR(handle->data->semaphore, &xHigherPriorityTaskWoken);
        if (pdFALSE != xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

static esp_err_t audio_pwm_init(const audio_pwm_config_t *cfg)
{
    esp_err_t res = ESP_OK;
    AUDIO_NULL_CHECK(TAG, cfg, return ESP_ERR_INVALID_ARG);
    if (!(cfg->tg_num < TIMER_GROUP_MAX)) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM TIMER GROUP NUMBER IS %d AND SHOULD BE 0 OR 1", __FILENAME__, __LINE__, __FUNCTION__, cfg->tg_num);
    }
    if (!(cfg->timer_num < TIMER_MAX)) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM TIMER NUMBER IS %d AND SHOULD BE 0 OR 1", __FILENAME__, __LINE__, __FUNCTION__, cfg->timer_num);
    }

    if (cfg->duty_resolution < 8 || cfg->duty_resolution > 10) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM RESOLUTION IS %d AND SHOULD BE 8, 9, 10", __FILENAME__, __LINE__, __FUNCTION__, cfg->duty_resolution);
    }

    audio_pwm_handle_t handle = NULL;
    handle = audio_calloc(1, sizeof(audio_pwm_t));
    AUDIO_NULL_CHECK(TAG, handle, goto init_error);

    handle->data = pwm_data_list_create(cfg->data_len);
    AUDIO_NULL_CHECK(TAG, handle->data, goto init_error);

    handle->config = *cfg;
    g_audio_pwm_handle = handle;
    if (cfg->tg_num == TIMER_GROUP_0) {
        handle->timg_dev = &TIMERG0;
    } else {
        handle->timg_dev = &TIMERG1;
    }
    handle->channel_mask = 0;
    if (handle->config.gpio_num_left >= 0) {
        handle->ledc_channel[CHANNEL_LEFT_INDEX].channel = handle->config.ledc_channel_left;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].duty = 0;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].gpio_num = handle->config.gpio_num_left;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].speed_mode = LEDC_LOW_SPEED_MODE;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].hpoint = 0;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].timer_sel = handle->config.ledc_timer_sel;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].intr_type = LEDC_INTR_DISABLE;
        res = ledc_channel_config(&handle->ledc_channel[CHANNEL_LEFT_INDEX]);
        AUDIO_CHECK(TAG, ESP_OK == res, goto init_error, "AUDIO PWM LEFT CHANNEL CONFIG ERROR");
        handle->channel_mask |= CHANNEL_LEFT_MASK;
    }
    if (handle->config.gpio_num_right >= 0) {
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel = handle->config.ledc_channel_right;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].duty = 0;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].gpio_num = handle->config.gpio_num_right;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].speed_mode = LEDC_LOW_SPEED_MODE;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].hpoint = 0;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].timer_sel = handle->config.ledc_timer_sel;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].intr_type = LEDC_INTR_DISABLE;
        res = ledc_channel_config(&handle->ledc_channel[CHANNEL_RIGHT_INDEX]);
        AUDIO_CHECK(TAG, ESP_OK == res, goto init_error, "AUDIO PWM RIGHT CHANNEL CONFIG ERROR");
        handle->channel_mask |= CHANNEL_RIGHT_MASK;
    }
    AUDIO_CHECK(TAG, 0 != handle->channel_mask, goto init_error, "AUDIO PWM CHANNEL MASK IS 0");

#ifdef CONFIG_IDF_TARGET_ESP32S2
    handle->ledc_timer.clk_cfg = LEDC_USE_APB_CLK;
#endif

    handle->ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    handle->ledc_timer.duty_resolution = handle->config.duty_resolution;
    handle->ledc_timer.timer_num = handle->config.ledc_timer_sel;
    uint32_t freq = (APB_CLK_FREQ / (1 << handle->ledc_timer.duty_resolution));
    handle->ledc_timer.freq_hz = freq - (freq % 1000);
    res = ledc_timer_config(&handle->ledc_timer);
    AUDIO_CHECK(TAG, ESP_OK == res, goto init_error, "AUDIO PWM TIMER ERROR");

    g_ledc_left_duty_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].duty.val;
    g_ledc_left_conf0_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].conf0.val;
    g_ledc_left_conf1_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].conf1.val;
    g_ledc_right_duty_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].duty.val;
    g_ledc_right_conf0_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].conf0.val;
    g_ledc_right_conf1_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].conf1.val;

    handle->status = AUDIO_PWM_STATUS_IDLE;
    return res;

init_error:
    if (handle->data) {
        audio_free(handle->data);
        handle->data = NULL;
    }
    if (handle) {
        audio_free(handle);
        handle = NULL;
    }
    return ESP_FAIL;
}

esp_err_t audio_pwm_set_param(int rate, ledc_timer_bit_t bits, int ch)
{
    esp_err_t res = ESP_OK;
    AUDIO_CHECK(TAG, g_audio_pwm_handle->status != AUDIO_PWM_STATUS_BUSY, return ESP_FAIL, "AUDIO PWM CAN NOT SET PARAM, WHEN AUDIO PWM STATUS IS BUSY");
    if (rate > SAMPLE_RATE_MAX || rate < SAMPLE_RATE_MIN) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM SAMPLE IS %d AND SHOULD BE BETWEEN 8000 AND 48000", __FILENAME__, __LINE__, __FUNCTION__, rate);
    }
    if (!(bits == 32 || bits == 16 || bits == 8)) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM BITS IS %d AND SHOULD BE 8, 16, 32", __FILENAME__, __LINE__, __FUNCTION__, bits);
    }
    if (!(ch == 2 || ch == 1)) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM CH IS %d AND SHOULD BE 1 OR 2", __FILENAME__, __LINE__, __FUNCTION__, ch);
    }

    audio_pwm_handle_t handle = g_audio_pwm_handle;
    handle->framerate = rate;
    handle->bits_per_sample = bits;
    handle->channel_set_num = ch;
    timer_config_t config = {0};
    config.divider = 16;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = 1;

#ifdef TIMER_GROUP_SUPPORTS_XTAL_CLOCK
    config.clk_src = TIMER_SRC_CLK_APB;
#endif

    timer_init(handle->config.tg_num, handle->config.timer_num, &config);
    timer_set_counter_value(handle->config.tg_num, handle->config.timer_num, 0x00000000ULL);
    timer_set_alarm_value(handle->config.tg_num, handle->config.timer_num, (TIMER_BASE_CLK / config.divider) / handle->framerate);
    timer_enable_intr(handle->config.tg_num, handle->config.timer_num);
    timer_isr_register(handle->config.tg_num, handle->config.timer_num, timer_group_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    return res;
}

esp_err_t audio_pwm_set_sample_rate(int rate)
{
    esp_err_t res;
    AUDIO_CHECK(TAG, g_audio_pwm_handle->status != AUDIO_PWM_STATUS_BUSY, return ESP_FAIL, "AUDIO PWM CAN NOT SET PARAM, WHEN AUDIO PWM STATUS IS BUSY");
    if (rate > SAMPLE_RATE_MAX || rate < SAMPLE_RATE_MIN) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM SAMPLE IS %d AND SHOULD BE BETWEEN 8000 AND 48000", __FILENAME__, __LINE__, __FUNCTION__, rate);
    }

    audio_pwm_handle_t handle = g_audio_pwm_handle;
    handle->framerate = rate;
    uint16_t div = 1;
#ifdef CONFIG_IDF_TARGET_ESP32S2
    div = (uint16_t)handle->timg_dev->hw_timer[handle->config.timer_num].config.tx_divider;
#elif CONFIG_IDF_TARGET_ESP32
    div = (uint16_t)handle->timg_dev->hw_timer[handle->config.timer_num].config.divider;
#elif CONFIG_IDF_TARGET_ESP32S3
    div = (uint16_t)handle->timg_dev->hw_timer[handle->config.timer_num].config.tn_divider;
#endif

    res = timer_set_alarm_value(handle->config.tg_num, handle->config.timer_num, (TIMER_BASE_CLK / div) / handle->framerate);
    return res;
}

static esp_err_t pwm_data_convert(pwm_data_handle_t data, uint8_t *inbuf, int32_t bits_per, int32_t duty, uint32_t bytes)
{
    int8_t shift = bits_per - duty;
    uint32_t len = bytes;

    if (bits_per == 16) {
        len >>= 1;
        uint16_t *buf_16b = (uint16_t *)inbuf;
        uint16_t value;
        int16_t temp;
        if (duty > 8) {
            for (size_t i = 0; i < len; i++) {
                temp = buf_16b[i];
                value = temp + 0x7fff;
                value >>= shift;
                pwm_data_list_write_byte(data, value);
                pwm_data_list_write_byte(data, value >> 8);
            }
        } else {
            for (size_t i = 0; i < len; i++) {
                temp = buf_16b[i];
                value = temp + 0x7fff;
                value >>= shift;
                pwm_data_list_write_byte(data, value);
            }
        }
    } else if (bits_per == 32) {
        len >>= 2;
        uint32_t *buf_32b = (uint32_t *)inbuf;
        uint32_t value;
        int32_t temp;
        if (duty > 8) {
            for (size_t i = 0; i < len; i++) {
                temp = buf_32b[i];
                value = temp + 0x7fffffff;
                value >>= shift;
                pwm_data_list_write_byte(data, value);
                pwm_data_list_write_byte(data, value >> 8);
            }
        } else {
            for (size_t i = 0; i < len; i++) {
                temp = buf_32b[i];
                value = temp + 0x7fffffff;
                value >>= shift;
                pwm_data_list_write_byte(data, value);
            }
        }
    } else {
        ESP_LOGE(TAG, "Only support bits (16 or 32), now bits_per is %d", bits_per);
    }
    return ESP_OK;
}


esp_err_t audio_pwm_write(uint8_t *inbuf, size_t inbuf_len, size_t *bytes_written, TickType_t ticks_to_wait)
{
    esp_err_t res = ESP_OK;
    audio_pwm_handle_t handle = g_audio_pwm_handle;
    AUDIO_NULL_CHECK(TAG, inbuf, return ESP_FAIL);

    *bytes_written = 0;
    pwm_data_handle_t data = handle->data;
    while (inbuf_len) {
        if (ESP_OK == pwm_data_list_wait_semaphore(data, ticks_to_wait)) {
            uint32_t free = pwm_data_list_get_free(data);
            uint32_t bytes_can_write = inbuf_len;
            if (inbuf_len > free) {
                bytes_can_write = free;
            }
            bytes_can_write &= 0xfffffffc;
            if (0 == bytes_can_write) {
                *bytes_written += inbuf_len;
                return ESP_OK;
            }
            pwm_data_convert(data, inbuf, handle->bits_per_sample, handle->config.duty_resolution, bytes_can_write);
            inbuf += bytes_can_write;
            inbuf_len -= bytes_can_write;
            *bytes_written += bytes_can_write;
        } else {
            res = ESP_FAIL;
        }
    }
    return res;
}

static esp_err_t audio_pwm_start(void)
{
    esp_err_t res;
    audio_pwm_handle_t handle = g_audio_pwm_handle;

    if (handle->status != AUDIO_PWM_STATUS_IDLE) {
        ESP_LOGE(TAG, "%s:%d (%s): AUDIO PWM STATE IS %d, AND SHOULD BE IDLE WHEN PWM START", __FILENAME__, __LINE__, __FUNCTION__, handle->status);
    }

    handle->status = AUDIO_PWM_STATUS_BUSY;
    timer_enable_intr(handle->config.tg_num, handle->config.timer_num);
    res = timer_start(handle->config.tg_num, handle->config.timer_num);
    return res;
}

static esp_err_t audio_pwm_stop(void)
{
    audio_pwm_handle_t handle = g_audio_pwm_handle;
    timer_pause(handle->config.tg_num, handle->config.timer_num);
    timer_disable_intr(handle->config.tg_num, handle->config.timer_num);
    pwm_data_list_flush(handle->data);
    handle->status = AUDIO_PWM_STATUS_IDLE;
    return ESP_OK;
}

static esp_err_t audio_pwm_deinit(void)
{
    audio_pwm_handle_t handle = g_audio_pwm_handle;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    handle->status = AUDIO_PWM_STATUS_UNINIT;
    audio_pwm_stop();
    for (size_t i = 0; i < AUDIO_PWM_CH_MAX; i++) {
        if (handle->ledc_channel[i].gpio_num >= 0) {
            ledc_stop(handle->ledc_channel[i].speed_mode, handle->ledc_channel[i].channel, 0);
        }
    }

    for (size_t i = 0; i < AUDIO_PWM_CH_MAX; i++) {
        if (handle->ledc_channel[i].gpio_num >= 0) {
            gpio_set_direction(handle->ledc_channel[i].gpio_num, GPIO_MODE_INPUT);
        }
    }

    pwm_data_list_destroy(handle->data);
    audio_free(handle);
    return ESP_OK;
}

static int _pwm_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait, void *context)
{
    size_t bytes_written = 0;
    audio_pwm_write((uint8_t *)buffer, len, &bytes_written, ticks_to_wait);
    return bytes_written;
}

static esp_err_t _pwm_destroy(audio_element_handle_t self)
{
    esp_err_t res = ESP_OK;
    pwm_stream_t *pwm = (pwm_stream_t *)audio_element_getdata(self);
    if (pwm->uninstall_drv) {
        res = audio_pwm_deinit();
    }
    audio_free(pwm);
    return res;
}

static esp_err_t _pwm_open(audio_element_handle_t self)
{
    esp_err_t res = ESP_OK;
    pwm_stream_t *pwm = (pwm_stream_t *)audio_element_getdata(self);
    if (pwm->is_open) {
        return ESP_OK;
    }
    res = audio_element_set_input_timeout(self, 2000 / portTICK_RATE_MS);
    pwm->is_open = true;
    return res;
}

static esp_err_t _pwm_close(audio_element_handle_t self)
{
    esp_err_t res = ESP_OK;
    pwm_stream_t *pwm = (pwm_stream_t *)audio_element_getdata(self);
    pwm->is_open = false;
    if (AEL_STATE_PAUSED != audio_element_get_state(self)) {
        audio_element_report_pos(self);
        audio_element_set_byte_pos(self, 0);
        res = audio_pwm_stop();
    }
    return res;
}

static int _pwm_process(audio_element_handle_t self, char *in_buffer, int in_len)
{
    int r_size = audio_element_input(self, in_buffer, in_len);
    int w_size = 0;
    if (r_size == AEL_IO_TIMEOUT) {
        memset(in_buffer, 0x00, in_len);
        r_size = in_len;
    }
    if (r_size > 0) {
        w_size = audio_element_output(self, in_buffer, r_size);
        audio_element_update_byte_pos(self, w_size);
    } else {
        w_size = r_size;
    }
    return w_size;
}

audio_element_handle_t pwm_stream_init(pwm_stream_cfg_t *config)
{
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    audio_element_handle_t el;

    audio_pwm_init(&config->pwm_config);
    cfg.open = _pwm_open;
    cfg.close = _pwm_close;
    cfg.process = _pwm_process;
    cfg.destroy = _pwm_destroy;
    cfg.tag = "pwm";
    cfg.out_rb_size = config->out_rb_size;
    cfg.task_stack = config->task_stack;
    cfg.task_prio = config->task_prio;
    cfg.task_core = config->task_core;
    cfg.buffer_len = config->buffer_len;
    cfg.stack_in_ext = config->ext_stack;
    if (config->type == AUDIO_STREAM_WRITER) {
        cfg.write = _pwm_write;
    } else {
        ESP_LOGE(TAG, "PWM stream only support AUDIO_STREAM_WRITER mode, not support %d", config->type);
        return NULL;
    }

    pwm_stream_t *pwm = audio_calloc(1, sizeof(pwm_stream_t));
    AUDIO_NULL_CHECK(TAG, pwm, return NULL);
    memcpy(&pwm->config, config, sizeof(pwm_stream_cfg_t));

    pwm->type = AUDIO_STREAM_WRITER;
    pwm->uninstall_drv = true;

    el = audio_element_init(&cfg);
    audio_element_setdata(el, pwm);

    ESP_LOGD(TAG, "stream init,el:%p", el);
    return el;
}

esp_err_t pwm_stream_set_clk(audio_element_handle_t pwm_stream, int rate, int bits, int ch)
{
    esp_err_t res = ESP_OK;
    res |= audio_pwm_set_param(rate, bits, ch);
    res |= audio_pwm_start();
    return res;
}
