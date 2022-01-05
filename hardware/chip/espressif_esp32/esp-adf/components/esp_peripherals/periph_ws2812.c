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

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <driver/rmt.h>
#include "esp_log.h"
#include "audio_mem.h"
#include "audio_sys.h"
#include "periph_ws2812.h"
#include "esp_peripherals.h"
#include "soc/dport_access.h"
#if !defined CONFIG_IDF_TARGET_ESP32C3
#include "soc/dport_reg.h"
#endif
#include "driver/rmt.h"
#include "audio_idf_version.h"

static const char *TAG = "PERIPH_WS2812";

#define DIVIDER                 4
#define DURATION                12.5
#define RMTCHANNEL              0
#define MAX_PULSES              32
#define PULSE_T0H               ((uint32_t)( 350 / (DURATION*DIVIDER)) & 0x00007FFF )
#define PULSE_T1H               ((uint32_t)( 900 / (DURATION*DIVIDER)) & 0x00007FFF )
#define PULSE_T0L               ((uint32_t)( 900 / (DURATION*DIVIDER)) & 0x00007FFF )
#define PULSE_T1L               ((uint32_t)( 350 / (DURATION*DIVIDER)) & 0x00007FFF )
#define PULSE_TRS               ( 50000 / (DURATION*DIVIDER))
#define PULSE_BIT0              (((uint32_t)PULSE_T0L<<16) + (((uint32_t)1)<<15) + (PULSE_T0H))
#define PULSE_BIT1              (((uint32_t)PULSE_T1L<<16) + (((uint32_t)1)<<15) + (PULSE_T1H))

#define FADE_STEP               30
#define INTERVAL_TIME_MS        10

typedef union {
    struct __attribute__ ((packed)) {
        uint8_t r, g, b;
    };
    uint32_t num;
} rgb_value;

typedef struct {
    periph_rgb_value         color;
    periph_ws2812_mode_t     mode;
    uint32_t                 time_on_ms;
    uint32_t                 time_off_ms;
    long long                tick;
    uint32_t                 loop;
    bool                     is_on;
    bool                     is_set;
} periph_ws2812_state_t;

typedef struct {
    uint32_t                 pos;
    uint32_t                 half;
    uint8_t                  *buffer;
} periph_ws2812_process_t;

typedef struct periph_ws2812 {
    periph_rgb_value          *color;
    uint32_t                  led_num;
    TimerHandle_t             timer;
    xSemaphoreHandle          sem;
    intr_handle_t             rmt_intr_handle;
    periph_ws2812_state_t     *state;
    periph_ws2812_process_t   process;
} periph_ws2812_t;

static esp_err_t ws2812_init_rmt_channel(int rmt_channel, int gpio_num)
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = rmt_channel;
    rmt_tx.gpio_num = gpio_num;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = DIVIDER;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_level = 1;
    rmt_tx.tx_config.carrier_en = 0;
    rmt_tx.tx_config.idle_level = 0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);

    rmt_set_tx_thr_intr_en(RMTCHANNEL, true, MAX_PULSES);
    rmt_set_mem_block_num(RMTCHANNEL, 1);
    rmt_set_mem_pd(RMTCHANNEL, false);
    rmt_set_tx_loop_mode(RMTCHANNEL, false);
    rmt_set_source_clk(RMTCHANNEL, RMT_BASECLK_APB);
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 3, 0))
    rmt_set_intr_enable_mask(BIT(0) | BIT(24));
#endif
    return ESP_OK;
}

static esp_err_t ws2812_data_copy(periph_ws2812_t *ws)
{
    unsigned int i, j, len, bit;

    len = ws->led_num * 3;;// - ws->process.pos;
    rmt_item32_t *rmt_data  = malloc(sizeof(rmt_item32_t) * len * 8);

    for (i = 0; i < len; i++) {
        bit = ws->process.buffer[i];
        for (j = 0; j < 8; j++, bit <<= 1) {
            if ((bit >> 7) & 0x01) {
                rmt_data[j + i * 8].val = PULSE_BIT1;
            } else {
                rmt_data[j + i * 8].val = PULSE_BIT0;
            }
        }
        if (i + ws->process.pos == ws->led_num * 3 - 1) {
            rmt_data[7 + i * 8].duration1 = PULSE_TRS;
        }
    }
    
    rmt_write_items(RMTCHANNEL, rmt_data, len * 8, portMAX_DELAY);
    
    if (rmt_data) {
        free(rmt_data);
        rmt_data = NULL;
    }
    return ESP_OK;
}

static void rmt_handle_tx_end(rmt_channel_t channel, void *arg)
{
    portBASE_TYPE taskAwoken = 0;
    periph_ws2812_t *ws = (periph_ws2812_t *)(arg);
    xSemaphoreGiveFromISR(ws->sem, &taskAwoken);
}

static esp_err_t ws2812_set_colors(periph_ws2812_t *ws)
{
    AUDIO_NULL_CHECK(TAG, ws, return ESP_FAIL);

    ws->process.buffer = audio_malloc(ws->led_num * 3 * sizeof(uint8_t));
    AUDIO_NULL_CHECK(TAG, ws->process.buffer, return ESP_FAIL);

    for (int i = 0; i < ws->led_num; i++) {
        rgb_value rgb = {
            .num = ws->color[i]
        };
        ws->process.buffer[0 + i * 3] = rgb.g;
        ws->process.buffer[1 + i * 3] = rgb.r;
        ws->process.buffer[2 + i * 3] = rgb.b;
    }
    ws->process.pos = 0;
    ws->process.half = 0;

    ws2812_data_copy(ws);
    xSemaphoreTake(ws->sem, portMAX_DELAY);
    if (ws->process.buffer) {
        audio_free(ws->process.buffer);
        ws->process.buffer = NULL;
    }

    return ESP_OK;
}

static void ws2812_timer_handler(TimerHandle_t tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t)pvTimerGetTimerID(tmr);
    periph_ws2812_t *periph_ws2812 = esp_periph_get_data(periph);
    periph_ws2812_state_t *st = periph_ws2812->state;
    for ( int i = 0; i < periph_ws2812->led_num; i++ ) {
        switch (st[i].mode) {
            case PERIPH_WS2812_ONE:
                if (st[i].is_on) {
                    periph_ws2812->color[i] = st[i].color;
                    ws2812_set_colors(periph_ws2812);
                    st[i].is_on = false;
                    st[i].loop = 0;
                }
                break;

            case PERIPH_WS2812_BLINK:
                if (st[i].is_set == false) {
                    continue;
                }
                if (st[i].loop == 0) {
                    periph_ws2812->color[i] = LED2812_COLOR_BLACK;
                    ws2812_set_colors(periph_ws2812);
                    st[i].is_set = false;
                }

                if (st[i].is_on && audio_sys_get_time_ms() - st[i].tick > st[i].time_off_ms) {
                    if (st[i].loop > 0) {
                        st[i].loop--;
                    } else {
                        continue;
                    }
                    st[i].is_on = false;
                    st[i].tick = audio_sys_get_time_ms();
                    periph_ws2812->color[i] = st[i].color;
                    ws2812_set_colors(periph_ws2812);
                } else if (!st[i].is_on && audio_sys_get_time_ms() - st[i].tick > st[i].time_on_ms) {
                    st[i].is_on = true;
                    st[i].tick = audio_sys_get_time_ms();
                    periph_ws2812->color[i] = LED2812_COLOR_BLACK;
                    ws2812_set_colors(periph_ws2812);
                }
                break;

            case PERIPH_WS2812_FADE:
                if (st[i].is_set == false) {
                    continue;
                }
                if (st[i].loop == 0) {
                    periph_ws2812->color[i] = LED2812_COLOR_BLACK;
                    ws2812_set_colors(periph_ws2812);
                    st[i].is_set = false;
                    continue;
                }

                if (st[i].is_on && (audio_sys_get_time_ms() - st[i].tick > ((st[i].time_on_ms / FADE_STEP)))) {
                    st[i].tick = audio_sys_get_time_ms();
                    rgb_value rgb = {
                        .num = st[i].color
                    };
                    rgb_value rgb1 = {
                        .num =  periph_ws2812->color[i]
                    };
                    rgb1.r -= (uint8_t)rgb.r / FADE_STEP;
                    rgb1.g -= (uint8_t)rgb.g / FADE_STEP;
                    rgb1.b -= (uint8_t)rgb.b / FADE_STEP;
                    ws2812_set_colors(periph_ws2812);
                    periph_ws2812->color[i] = rgb1.num;
                    if ((rgb1.r <= (uint8_t)rgb.r / FADE_STEP)
                        && (rgb1.g <= (uint8_t)rgb.g / FADE_STEP)
                        && (rgb1.b <= (uint8_t)rgb.b / FADE_STEP)) {
                        st[i].is_on = false;
                        st[i].loop--;
                    }
                } else if ((st[i].is_on == false) && (audio_sys_get_time_ms() - st[i].tick > ((st[i].time_off_ms / FADE_STEP)))) {
                    st[i].tick = audio_sys_get_time_ms();
                    rgb_value rgb = {
                        .num = st[i].color
                    };
                    rgb_value rgb1 = {
                        .num =  periph_ws2812->color[i]
                    };
                    rgb1.r += (uint8_t)rgb.r / FADE_STEP;
                    rgb1.g += (uint8_t)rgb.g / FADE_STEP;
                    rgb1.b += (uint8_t)rgb.b / FADE_STEP;
                    ws2812_set_colors(periph_ws2812);
                    periph_ws2812->color[i] = rgb1.num;
                    if ((((uint8_t)rgb.r - rgb1.r) <= (uint8_t)rgb.r / FADE_STEP)
                        && (((uint8_t)rgb.g - rgb1.g) <= (uint8_t)rgb.g / FADE_STEP)
                        && (((uint8_t)rgb.b - rgb1.b) <= (uint8_t)rgb.b / FADE_STEP))  {
                        st[i].is_on = true;
                    }
                }
                break;
            default:
                ESP_LOGW(TAG, "The ws2812 mode[%d] is invalid", st[i].mode);
                break;
        }
    }

}

static esp_err_t _ws2812_run(esp_periph_handle_t periph, audio_event_iface_msg_t *msg)
{
    return ESP_OK;
}

static esp_err_t _ws2812_init(esp_periph_handle_t periph)
{
    return ESP_OK;
}

static esp_err_t _ws2812_destroy(esp_periph_handle_t periph)
{
    periph_ws2812_t *periph_ws2812 = esp_periph_get_data(periph);
    AUDIO_NULL_CHECK(TAG, periph_ws2812, return ESP_FAIL);

    if (periph_ws2812) {
        periph_ws2812_state_t *st = periph_ws2812->state;
        for (int i = 0; i < periph_ws2812->led_num; i++) {
            st[i].color = LED2812_COLOR_BLACK;
            st[i].is_on = true;
            st[i].mode = PERIPH_WS2812_ONE;
        }
        ws2812_set_colors(periph_ws2812);

        if (periph_ws2812->color) {
            audio_free(periph_ws2812->color);
            periph_ws2812->color = NULL;
        }

        if (periph_ws2812->state) {
            audio_free(periph_ws2812->state);
            periph_ws2812->state = NULL;
        }

        esp_periph_stop_timer(periph);
        rmt_tx_stop(RMTCHANNEL);
        rmt_driver_uninstall(RMTCHANNEL);
        vSemaphoreDelete(periph_ws2812->sem);

        audio_free(periph_ws2812);
        periph_ws2812 = NULL;
    }

    return ESP_OK;
}

esp_periph_handle_t periph_ws2812_init(periph_ws2812_cfg_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);

    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_WS2812, "periph_ws2812");
    periph_ws2812_t *periph_ws2812 = audio_calloc(1, sizeof(periph_ws2812_t));
    AUDIO_NULL_CHECK(TAG, periph_ws2812, goto ws2812_init_err);

    periph_ws2812->led_num = config->led_num;
    periph_ws2812->timer = NULL;
    periph_ws2812->sem = xSemaphoreCreateBinary();
    periph_ws2812->rmt_intr_handle = NULL;

    periph_ws2812->color = audio_malloc(sizeof(periph_rgb_value) * periph_ws2812->led_num);
    AUDIO_NULL_CHECK(TAG, periph_ws2812->color, goto ws2812_init_err);

    for (int i = 0; i < periph_ws2812->led_num; i++) {
        periph_ws2812->color[i] = LED2812_COLOR_BLACK;
    }

    periph_ws2812->state = audio_malloc(sizeof(periph_ws2812_state_t) * (periph_ws2812->led_num));
    AUDIO_NULL_CHECK(TAG, periph_ws2812->state, goto ws2812_init_err);

    ws2812_init_rmt_channel(RMTCHANNEL, (gpio_num_t)config->gpio_num);
    esp_periph_set_data(periph, periph_ws2812);
    rmt_register_tx_end_callback(rmt_handle_tx_end, (void *)periph_ws2812);

    esp_periph_set_function(periph, _ws2812_init, _ws2812_run, _ws2812_destroy);
    ws2812_set_colors(periph_ws2812);
    ESP_LOGD(TAG, "periph ws2812 init");
    return periph;

ws2812_init_err:
    if (periph_ws2812->sem) {
        vSemaphoreDelete(periph_ws2812->sem);
        periph_ws2812->sem = NULL;
    }
    if (periph_ws2812->color) {
        audio_free(periph_ws2812->color);
        periph_ws2812->color = NULL;
    }
    if (periph_ws2812->state) {
        audio_free(periph_ws2812->state);
        periph_ws2812->state = NULL;
    }
    if (periph_ws2812) {
        audio_free(periph_ws2812);
        periph_ws2812 = NULL;
    }
    if (periph) {
        audio_free(periph);
    }
    periph = NULL;
    return periph;
}

esp_err_t periph_ws2812_control(esp_periph_handle_t periph, periph_ws2812_ctrl_cfg_t *control_cfg, void *ctx)
{
    periph_ws2812_t *periph_ws2812 = esp_periph_get_data(periph);

    AUDIO_NULL_CHECK(TAG, periph_ws2812, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, control_cfg, return ESP_FAIL);

    for (int i = 0; i < periph_ws2812->led_num; i++) {
        periph_ws2812->state[i].color = control_cfg[i].color;
        periph_ws2812->color[i] = control_cfg[i].color;
        periph_ws2812->state[i].time_on_ms = control_cfg[i].time_on_ms;
        periph_ws2812->state[i].time_off_ms = control_cfg[i].time_off_ms;
        periph_ws2812->state[i].tick = audio_sys_get_time_ms();
        periph_ws2812->state[i].loop = control_cfg[i].loop;
        periph_ws2812->state[i].is_on = true;
        periph_ws2812->state[i].is_set = true;
        periph_ws2812->state[i].mode = control_cfg[i].mode;
    }

    esp_periph_start_timer(periph, INTERVAL_TIME_MS / portTICK_RATE_MS, ws2812_timer_handler);

    return ESP_OK;
}

esp_err_t periph_ws2812_stop(esp_periph_handle_t periph)
{
    periph_ws2812_t *periph_ws2812 = esp_periph_get_data(periph);

    AUDIO_NULL_CHECK(TAG, periph_ws2812, return ESP_FAIL);

    periph_ws2812_state_t *st = periph_ws2812->state;
    for (int i = 0; i < periph_ws2812->led_num; i++) {
        st[i].color = LED2812_COLOR_BLACK;
        st[i].is_on = true;
        st[i].mode = PERIPH_WS2812_ONE;
    }
    ws2812_set_colors(periph_ws2812);
    return ESP_OK;
}