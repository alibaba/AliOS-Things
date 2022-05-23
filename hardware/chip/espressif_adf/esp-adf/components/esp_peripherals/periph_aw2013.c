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

#include "audio_mem.h"
#include "esp_peripherals.h"
#include "aw2013.h"
#include "periph_aw2013.h"

static const char *TAG = "PERIPH_AW2013";

typedef struct {
    aw2013_time_level_t time[5];
    aw2013_brightness_t bright;
    periph_aw2013_mode_t mode;
    uint32_t rgb_value;
    uint8_t rpt_time;
} periph_aw2013_t;

static esp_err_t _aw2013_set_mode(periph_aw2013_mode_t mode)
{
    esp_err_t ret = ESP_OK;
    switch (mode) {
        case AW2013_MODE_LED:
            ret |= aw2013_enable_auto_flash(false);
            ret |= aw2013_enable_fade_mode(false);
            break;
        case AW2013_MODE_FADE:
            ret |= aw2013_enable_auto_flash(false);
            ret |= aw2013_enable_fade_mode(true);
            break;
        case AW2013_MODE_AUTO:
            ret |= aw2013_enable_auto_flash(true);
            ret |= aw2013_enable_fade_mode(false);
            break;
        default:
            ESP_LOGE(TAG, "Invalid mode :%d", mode);
            return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t _aw2013_init(esp_periph_handle_t self)
{
    esp_err_t ret = ESP_OK;
    periph_aw2013_t *aw2013 = esp_periph_get_data(self);

    ret |= aw2013_init();
    for (int i = 0; i <= AW2013_TIME_4; i++) {
        ret |= aw2013_set_time(i, aw2013->time[i]);
    }
    ret |= aw2013_set_repeat_time(aw2013->rpt_time);
    ret |= aw2013_set_brightness(aw2013->bright);
    ret |= _aw2013_set_mode(aw2013->mode);
    ret |= aw2013_set_pwm_value(aw2013->rgb_value);
    return ret;
}

static esp_err_t _aw2013_destroy(esp_periph_handle_t self)
{
    esp_err_t ret = ESP_OK;
    periph_aw2013_t *aw2013 = esp_periph_get_data(self);

    ret |= aw2013_deinit();
    audio_free(aw2013);
    return ret;
}

esp_err_t periph_aw2013_set_brightless(esp_periph_handle_t periph, aw2013_brightness_t bright)
{
    periph_aw2013_t *aw2013 = esp_periph_get_data(periph);
    if (bright < AW2013_BRIGHT_0 || bright > AW2013_BRIGHT_3) {
        ESP_LOGE(TAG, "Fail to set parameters, bright: %d", bright);
        return ESP_FAIL;
    }
    aw2013->bright = bright;
    return aw2013_set_brightness(bright);
}

esp_err_t periph_aw2013_set_rgb_value(esp_periph_handle_t periph, uint32_t value)
{
    periph_aw2013_t *aw2013 = esp_periph_get_data(periph);
    aw2013->rgb_value = value;
    return aw2013_set_pwm_value(value);
}

esp_err_t periph_aw2013_set_repeat_time(esp_periph_handle_t periph, uint8_t cnt)
{
    periph_aw2013_t *aw2013 = esp_periph_get_data(periph);
    aw2013->rpt_time = cnt;
    return aw2013_set_repeat_time(cnt);
}

esp_err_t periph_aw2013_set_mode(esp_periph_handle_t periph, periph_aw2013_mode_t mode)
{
    if (mode < 0 || mode > AW2013_MODE_AUTO) {
        ESP_LOGE(TAG, "Fail to set parameters, mode: %d", mode);
        return ESP_FAIL;
    }
    periph_aw2013_t *aw2013 = esp_periph_get_data(periph);
    aw2013->mode = mode;
    return _aw2013_set_mode(mode);
}

esp_err_t periph_aw2013_set_time(esp_periph_handle_t periph, aw2013_time_t time, aw2013_time_level_t level)
{
    if (time < 0 || time > AW2013_TIME_4 || level < 0 || level > AW2013_TIME_LEVEL_8) {
        ESP_LOGE(TAG, "Fail to set parameters, time: %d, level: %d", time, level);
        return ESP_FAIL;
    }
    periph_aw2013_t *aw2013 = esp_periph_get_data(periph);
    aw2013->time[time] = level;
    return aw2013_set_time(time, level);
}

esp_periph_handle_t periph_aw2013_init(periph_aw2013_cfg_t *aw2013_cfg)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_AW2013, "periph_aw2013");
    AUDIO_NULL_CHECK(TAG, periph, return NULL);

    periph_aw2013_t *aw2013 = audio_calloc(1, sizeof(periph_aw2013_t));
    AUDIO_MEM_CHECK(TAG, aw2013, {
        audio_free(periph);
        return NULL;
    });

    aw2013->mode = aw2013_cfg->mode;
    aw2013->bright = aw2013_cfg->bright;
    aw2013->rgb_value = aw2013_cfg->rgb_value;
    // Default time period
    aw2013->time[0] = AW2013_TIME_LEVEL_1;
    aw2013->time[1] = AW2013_TIME_LEVEL_3;
    aw2013->time[2] = AW2013_TIME_LEVEL_3;
    aw2013->time[3] = AW2013_TIME_LEVEL_3;
    aw2013->time[4] = AW2013_TIME_LEVEL_2;
    // Cycle all the time
    aw2013->rpt_time = 0;

    esp_periph_set_data(periph, aw2013);
    esp_periph_set_function(periph, _aw2013_init, NULL, _aw2013_destroy);
    return periph;
}
