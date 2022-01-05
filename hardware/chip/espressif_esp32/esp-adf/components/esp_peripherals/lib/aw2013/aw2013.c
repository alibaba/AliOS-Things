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
#include "i2c_bus.h"
#include "board.h"
#include "aw2013.h"
#include "esp_log.h"

#define AW2013_ADDR 0x8a
#define AW2013_MAX_LED_NUM 3
#define AW2013_MAX_REPEAT_TIME 15

static const char *TAG = "AW2013";

static i2c_bus_handle_t i2c_handle;

esp_err_t aw2013_set_repeat_time(uint8_t cnt)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    cnt %= AW2013_MAX_REPEAT_TIME;

    for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
        reg_addr = AW2013_REG_LED0T0CNT + AW2013_MAX_LED_NUM * i;
        ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        reg_val &= 0xf0;
        reg_val |= cnt;
        ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
    }
    return ret;
}

esp_err_t aw2013_set_time(aw2013_time_t time, aw2013_time_level_t level)
{
    if (level > AW2013_TIME_LEVEL_8 || time > AW2013_TIME_4) {
        ESP_LOGE(TAG, "Invalid parameters, time: %d, level: %d", time, level);
        return ESP_FAIL;
    }
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);

    for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
        switch (time) {
            case AW2013_TIME_0: {
                    reg_addr = AW2013_REG_LED0T0CNT + AW2013_MAX_LED_NUM * i;
                    ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    reg_val &= 0x0f;
                    reg_val |= (level << 4);
                    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    break;
                }
            case AW2013_TIME_1: {
                    if (level > AW2013_TIME_LEVEL_7) {
                        level = AW2013_TIME_LEVEL_7;
                    }
                    reg_addr = AW2013_REG_LED0T1T2 + AW2013_MAX_LED_NUM * i;
                    ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    reg_val &= 0x8f;
                    reg_val |= (level << 4);
                    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    break;
                }
            case AW2013_TIME_2: {
                    if (level > AW2013_TIME_LEVEL_5) {
                        level = AW2013_TIME_LEVEL_5;
                    }
                    reg_addr = AW2013_REG_LED0T1T2 + AW2013_MAX_LED_NUM * i;
                    ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    reg_val &= 0xf8;
                    reg_val |= level;
                    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    break;
                }
            case AW2013_TIME_3: {
                    if (level > 7) {
                        level = 7;
                    }
                    reg_addr = AW2013_REG_LED0T3T4 + AW2013_MAX_LED_NUM * i;
                    ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    reg_val &= 0x8f;
                    reg_val |= (level << 4);
                    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    break;
                }
            case AW2013_TIME_4: {
                    if (level > 7) {
                        level = 7;
                    }
                    reg_addr = AW2013_REG_LED0T3T4 + AW2013_MAX_LED_NUM * i;
                    ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    reg_val &= 0xf8;
                    reg_val |= level;
                    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
                    break;
                }
            default: {
                    return ESP_FAIL;
                }
        }
    }
    return ret;
}

esp_err_t aw2013_enable_fade_mode(bool en)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);

    if (en) {
        for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
            reg_addr = AW2013_REG_LCFG0 + i;
            ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
            reg_val &= ~(0x01 << 4);
            reg_val |=  (0x03 << 5);
            ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        }
    } else {
        for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
            reg_addr = AW2013_REG_LCFG0 + i;
            ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
            reg_val &= ~(0x03 << 5);
            ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        }
    }
    return ret;
}

esp_err_t aw2013_enable_auto_flash(bool en)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);

    if (en) {
        for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
            reg_addr = AW2013_REG_LCFG0 + i;
            ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
            reg_val |= (0x01 << 4);
            ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        }
    } else {
        for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
            reg_addr = AW2013_REG_LCFG0 + i;
            ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
            reg_val &= ~(0x01 << 4);
            ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        }
    }
    return ret;
}

esp_err_t aw2013_set_pwm_value(uint32_t value)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);

    reg_addr = AW2013_REG_PWM0;
    reg_val = (value >> 16) & 0xff;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);

    reg_addr = AW2013_REG_PWM1;
    reg_val = (value >> 8) & 0xff;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);

    reg_addr = AW2013_REG_PWM2;
    reg_val = value & 0xff;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
    return ret;
}

esp_err_t aw2013_set_brightness(aw2013_brightness_t bright)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);
    if (bright > AW2013_BRIGHT_3) {
        bright = AW2013_BRIGHT_3;
    }
    for (int i = 0; i < AW2013_MAX_LED_NUM; i++) {
        reg_addr = AW2013_REG_LCFG0 + i;
        ret |= i2c_bus_read_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
        reg_val &= 0xfc;
        reg_val |= bright;
        ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
    }
    return ret;
}

esp_err_t aw2013_reset(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0x0;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);

    reg_addr = AW2013_REG_RESET;
    reg_addr = AW2013_RESET_VALUE;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);

    reg_addr = AW2013_REG_GCR;
    reg_val = 0x01;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);

    reg_addr = AW2013_REG_LCTR;
    reg_val = 0x07;
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
    ret |= aw2013_set_brightness(1);
    return ret;
}

esp_err_t aw2013_init(void)
{
    esp_err_t ret = ESP_OK;
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .master.clk_speed = 100000
    };
    ret |= get_i2c_pins(AW2013_I2C_PORT, &config);
    i2c_handle = i2c_bus_create(AW2013_I2C_PORT, &config);
    ret |= aw2013_reset();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to init aw2013");
    }
    return ret;
}

esp_err_t aw2013_deinit(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_addr = AW2013_REG_RESET;
    uint8_t reg_val = AW2013_RESET_VALUE;
    AUDIO_NULL_CHECK(TAG, i2c_handle, return ESP_FAIL);
    ret |= i2c_bus_write_bytes(i2c_handle, AW2013_ADDR, &reg_addr, 1, &reg_val, 1);
    ret |= i2c_bus_delete(i2c_handle);
    return ret;
}
