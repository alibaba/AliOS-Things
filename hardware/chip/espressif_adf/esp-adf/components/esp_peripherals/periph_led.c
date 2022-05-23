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
#include <math.h>
#include "esp_log.h"
#include "audio_mem.h"
#include "audio_sys.h"
#include "periph_led.h"
#include "esp_peripherals.h"
#include "audio_mutex.h"

#define MAX_LED_CHANNEL (8)

static const char *TAG = "PERIPH_LED";

#define VALIDATE_LED(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_LED)) { \
    ESP_LOGE(TAG, "Invalid LED periph, at line %d", __LINE__);\
    return ret;\
}

typedef struct {
    int index;
    int pin;
    int high_level_ms;
    int low_level_ms;
    long long tick;
    int loop;
    bool is_high_level;
    bool fade;
    bool stop;
    int  level;
} periph_led_channel_t;

typedef struct periph_led {
    ledc_mode_t      led_speed_mode;
    ledc_timer_bit_t led_duty_resolution;
    ledc_timer_t     led_timer_num;
    uint32_t         led_freq_hz;
    QueueHandle_t    led_mutex;
    periph_led_channel_t   channels[MAX_LED_CHANNEL];
} periph_led_t;

static esp_err_t _led_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    return ESP_OK;
}

static esp_err_t _led_init(esp_periph_handle_t self)
{
    VALIDATE_LED(self, ESP_FAIL);
    periph_led_t *periph_led = esp_periph_get_data(self);
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = periph_led->led_duty_resolution, // resolution of PWM duty
        .freq_hz = periph_led->led_freq_hz,                      // frequency of PWM signal
        .speed_mode = periph_led->led_speed_mode,           // timer mode
        .timer_num = periph_led->led_timer_num            // timer index
    };

    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);
    ledc_fade_func_install(0);
    return ESP_OK;
}

static esp_err_t _led_destroy(esp_periph_handle_t self)
{
    periph_led_t *periph_led = esp_periph_get_data(self);
    for (int i = 0; i < MAX_LED_CHANNEL; i++) {
        periph_led_channel_t *ch = &periph_led->channels[i];
        if (ch->index > 0 && ch->pin > 0) {
            ledc_stop(periph_led->led_speed_mode, ch->index, ch->level);
        }
    }
    esp_periph_stop_timer(self);
    ledc_fade_func_uninstall();
    mutex_destroy(periph_led->led_mutex);
    audio_free(periph_led);
    return ESP_OK;
}

esp_periph_handle_t periph_led_init(periph_led_cfg_t *config)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_LED, "periph_led");
    //check periph
    periph_led_t *periph_led = audio_calloc(1, sizeof(periph_led_t));
    //check periph_led
    periph_led->led_speed_mode      = config->led_speed_mode;
    periph_led->led_duty_resolution = config->led_duty_resolution;
    periph_led->led_timer_num       = config->led_timer_num;
    periph_led->led_freq_hz         = config->led_freq_hz;
    periph_led->led_mutex           = mutex_create();
    if (periph_led->led_freq_hz == 0) {
        periph_led->led_freq_hz = 5000;
    }
    memset(&periph_led->channels, -1, sizeof(periph_led->channels));
    esp_periph_set_data(periph, periph_led);
    esp_periph_set_function(periph, _led_init, _led_run, _led_destroy);
    return periph;
}

static periph_led_channel_t *_find_led_channel(periph_led_t *periph_led, int gpio_num)
{
    periph_led_channel_t *ch = NULL;
    for (int i = 0; i < MAX_LED_CHANNEL; i++) {
        if (periph_led->channels[i].pin == gpio_num) {
            ch = &periph_led->channels[i];
            ch->index = i;
            break;
        } else if (periph_led->channels[i].pin == -1) {
            ch = &periph_led->channels[i];
            ch->index = i;
        }
    }
    return ch;
}

static void led_timer_handler(xTimerHandle tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t) pvTimerGetTimerID(tmr);

    periph_led_t *periph_led = esp_periph_get_data(periph);
    mutex_lock(periph_led->led_mutex);
    for (int i = 0; i < MAX_LED_CHANNEL; i++) {
        periph_led_channel_t *ch = &periph_led->channels[i];
        if (ch->pin < 0 || ch->stop == true) {
            continue;
        }

        if (ch->loop == 0) {
            ledc_stop(periph_led->led_speed_mode, ch->index, ch->level);
            esp_periph_send_event(periph, PERIPH_LED_BLINK_FINISH, (void *)ch->pin, 0);
            ch->stop = true;
            continue;
        }

        if (!ch->is_high_level && audio_sys_get_time_ms() - ch->tick > ch->low_level_ms) {
            if (ch->loop > 0) {
                ch->loop --;
            }
            // now, switch on
            if (ch->fade) {
                ledc_set_fade_with_time(periph_led->led_speed_mode, ch->index, pow(2, periph_led->led_duty_resolution) - 1, ch->high_level_ms);
                ledc_fade_start(periph_led->led_speed_mode, ch->index, LEDC_FADE_NO_WAIT);
            } else {
                ledc_set_duty(periph_led->led_speed_mode, ch->index, pow(2, periph_led->led_duty_resolution) - 1);
                ledc_update_duty(periph_led->led_speed_mode, ch->index);
            }
            if (ch->low_level_ms > 0) {
                ch->is_high_level = true;
            }
            ch->tick = audio_sys_get_time_ms();
        } else if (ch->is_high_level && audio_sys_get_time_ms() - ch->tick > ch->high_level_ms) {
            if (ch->loop > 0) {
                ch->loop --;
            }
            // switch off
            if (ch->fade) {
                ledc_set_fade_with_time(periph_led->led_speed_mode, ch->index, 0, ch->low_level_ms);
                ledc_fade_start(periph_led->led_speed_mode, ch->index, LEDC_FADE_NO_WAIT);
            } else {
                ledc_set_duty(periph_led->led_speed_mode, ch->index, 0);
                ledc_update_duty(periph_led->led_speed_mode, ch->index);
            }
            if (ch->high_level_ms > 0) {
                ch->is_high_level = false;
            }
            ch->tick = audio_sys_get_time_ms();
        }
    }
    mutex_unlock(periph_led->led_mutex);
}

esp_err_t periph_led_blink(esp_periph_handle_t periph, int gpio_num, int time_on_ms, int time_off_ms, bool fade, int loop, periph_led_idle_level_t level)
{
    periph_led_t *periph_led = esp_periph_get_data(periph);
    periph_led_channel_t *ch = _find_led_channel(periph_led, gpio_num);
    if (ch == NULL) {
        return ESP_FAIL;
    }
    ledc_channel_config_t ledc_channel_cfg = {
        .channel    = ch->index,
        .duty       = 0,
        .gpio_num   = gpio_num,
        .speed_mode = periph_led->led_speed_mode,
        .timer_sel  = periph_led->led_timer_num,
    };
    ledc_channel_config(&ledc_channel_cfg);
    ch->pin = gpio_num;
    ch->tick = audio_sys_get_time_ms();
    ch->loop = loop;
    ch->fade = fade;
    if (level == PERIPH_LED_IDLE_LEVEL_LOW) {
        ch->is_high_level = false;
        ch->high_level_ms = time_on_ms;
        ch->low_level_ms = time_off_ms;
    } else {
        ch->is_high_level = true;
        ch->high_level_ms = time_off_ms;
        ch->low_level_ms = time_on_ms;
    }
    ch->stop = false;
    ch->level = level;
    esp_periph_start_timer(periph, portTICK_RATE_MS, led_timer_handler);
    return ESP_OK;
}

esp_err_t periph_led_stop(esp_periph_handle_t periph, int gpio_num)
{
    periph_led_t *periph_led = esp_periph_get_data(periph);
    periph_led_channel_t *ch = _find_led_channel(periph_led, gpio_num);
    if (ch && (ch->pin < 0 || ch->index < 0)) {
        return ESP_OK;
    }

    mutex_lock(periph_led->led_mutex);
    ch->stop = true;
    ledc_stop(periph_led->led_speed_mode, ch->index, ch->level);
    mutex_unlock(periph_led->led_mutex);

    return ESP_OK;
}
