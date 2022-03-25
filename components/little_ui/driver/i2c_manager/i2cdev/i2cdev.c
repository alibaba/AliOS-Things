/*

SPDX-License-Identifier: MIT

MIT License

Copyright (c) 2021 Rop Gonggrijp.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <i2c_manager.h>
#include "i2cdev.h"
#include <esp_log.h>

static const char *TAG = I2C_TAG;


esp_err_t i2cdev_init() {
    return ESP_OK;
}

esp_err_t i2cdev_done() {
    return ESP_OK;
}

esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev) {
    return ESP_OK;
}

esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev) {
    return ESP_OK;
}

esp_err_t i2c_dev_take_mutex(i2c_dev_t *dev) {
    return i2c_manager_lock(dev->port);
}

esp_err_t i2c_dev_give_mutex(i2c_dev_t *dev) {
    return i2c_manager_unlock(dev->port);
}

esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, void *in_data, size_t in_size) {
    if (!dev || !in_data || !in_size) return ESP_ERR_INVALID_ARG;
    uint32_t reg;
    switch(out_size) {
        case 0:
            reg = I2C_NO_REG;
            break;
        case 1:
            reg = *(uint8_t*)out_data;
            break;
        case 2:
            reg = *(uint16_t*)out_data | I2C_REG_16;
            break;
        default:
            ESP_LOGE(TAG, "i2cdev emulation only supports 0, 8 and 16-bit registers");
            return ESP_ERR_INVALID_ARG;
    }
    return i2c_manager_read(dev->port, dev->addr, reg, in_data, (uint16_t)in_size);
}

esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size) {
    if (!dev || !out_data || !out_size) return ESP_ERR_INVALID_ARG;
    uint32_t reg;
    switch(out_reg_size) {
        case 0:
            reg = I2C_NO_REG;
            break;
        case 1:
            reg = *(uint8_t*)out_reg;
            break;
        case 2:
            reg = *(uint16_t*)out_reg | I2C_REG_16;
            break;
        default:
            ESP_LOGE(TAG, "i2cdev emulation only supports 0, 8 and 16-bit registers");
            return ESP_ERR_INVALID_ARG;
    }
    return i2c_manager_write(dev->port, dev->addr, reg, out_data, (uint16_t)out_size);
}

esp_err_t i2c_dev_read_reg(const i2c_dev_t *dev, uint8_t reg, void *in_data, size_t in_size) {
    return i2c_manager_read(dev->port, dev->addr, reg, in_data, (uint16_t)in_size);
}

esp_err_t i2c_dev_write_reg(const i2c_dev_t *dev, uint8_t reg, const void *out_data, size_t out_size) {
    return i2c_manager_write(dev->port, dev->addr, reg, out_data, (uint16_t)out_size);
}
