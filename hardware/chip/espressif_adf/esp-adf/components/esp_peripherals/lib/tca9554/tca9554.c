/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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
#include "driver/gpio.h"
#include "i2c_bus.h"
#include "board.h"
#include "esp_log.h"
#include "tca9554.h"

#define TCA9554_ADDR   0x40

static char *TAG = "TCA9554";
static i2c_bus_handle_t i2c_handle;

#define SET_BITS(_m, _s, _v)  ((_v) ? (_m)|((_s)) : (_m)&~((_s)))
#define GET_BITS(_m, _s)      (((_m) & (_s)) ? true : false)


#define TCA9554_INPUT_PORT              0x00
#define TCA9554_OUTPUT_PORT             0x01
#define TCA9554_POLARITY_INVERSION_PORT 0x02
#define TCA9554_CONFIGURATION_PORT      0x03


static esp_err_t tca9554_write_reg(uint8_t reg_addr, uint8_t data)
{
    return i2c_bus_write_bytes(i2c_handle, TCA9554_ADDR, &reg_addr, sizeof(reg_addr), &data, sizeof(data));
}

static char tca9554_read_reg(uint8_t reg_addr)
{
    uint8_t data;
    i2c_bus_read_bytes(i2c_handle, TCA9554_ADDR, &reg_addr, sizeof(reg_addr), &data, sizeof(data));
    return data;
}

static int i2c_init(gpio_num_t clk, gpio_num_t sda)
{
    int res = 0;
    i2c_config_t es_i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .scl_io_num = clk,
        .sda_io_num = sda,
    };
    i2c_handle = i2c_bus_create(I2C_NUM_0, &es_i2c_cfg);
    return res;
}

esp_tca9554_io_level_t tca9554_get_input_state(esp_tca9554_gpio_num_t gpio_num)
{
    char data = 0;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_INPUT_PORT);
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return TCA9554_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_tca9554_io_level_t tca9554_get_output_state(esp_tca9554_gpio_num_t gpio_num)
{
    char data = 0;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_OUTPUT_PORT);
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return TCA9554_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_err_t tca9554_set_output_state(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_level_t level)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_OUTPUT_PORT);
        res = tca9554_write_reg(TCA9554_OUTPUT_PORT, SET_BITS(data, gpio_num, level));
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
    }
    return res;
}

esp_err_t tca9554_set_polarity_inversion(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_polarity_t polarity)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_POLARITY_INVERSION_PORT);
        res = tca9554_write_reg(TCA9554_POLARITY_INVERSION_PORT, SET_BITS(data, gpio_num, polarity));
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
    }
    return res;
}

esp_tca9554_io_config_t tca9554_get_io_config(esp_tca9554_gpio_num_t gpio_num)
{
    char data = 0;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_CONFIGURATION_PORT);
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
        return TCA9554_LEVEL_ERROR;
    }

    return GET_BITS(data, gpio_num);
}

esp_err_t tca9554_set_io_config(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_config_t io_config)
{
    char data;
    esp_err_t res = ESP_FAIL;
    if (gpio_num < TCA9554_GPIO_NUM_MAX) {
        data = tca9554_read_reg(TCA9554_CONFIGURATION_PORT);
        res = tca9554_write_reg(TCA9554_CONFIGURATION_PORT, SET_BITS(data, gpio_num, io_config));
    } else {
        ESP_LOGE(TAG, "gpio num is error, current gpio: %d", gpio_num);
    }
    return res;
}

void tca9554_read_all()
{
    for (int i = 0; i < 0x04; i++) {
        uint8_t reg = tca9554_read_reg(i);
        ets_printf("REG:%02x, %02x\n", i, reg);
    }
}

esp_err_t tca9554_init(esp_tca9554_config_t *cfg)
{
    esp_err_t ret = ESP_OK;
    i2c_init(cfg->i2c_scl, cfg->i2c_sda);

    return ret;
}

esp_err_t tca9554_deinit()
{
    i2c_bus_delete(i2c_handle);
    return ESP_OK;
}
