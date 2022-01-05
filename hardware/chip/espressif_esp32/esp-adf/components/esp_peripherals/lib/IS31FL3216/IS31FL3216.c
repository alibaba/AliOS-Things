/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
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

#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "IS31FL3216.h"
#include "i2c_bus.h"
#include "audio_mem.h"

#define IS31FL3216_WRITE_BIT        0x00

#define I2C_MASTER_SCL_IO           23        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           18        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM          I2C_NUM_0     /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000    /*!< I2C master clock frequency */

#define IS31FL3216_ADDRESS          0xE8      /*!< I2C Addr*/

#define IS31_ERROR_CHECK(con) if(!(con)) {ESP_LOGE(TAG,"err line: %d", __LINE__);}
#define IS31_PARAM_CHECK(con) if(!(con)) {ESP_LOGE(TAG,"Parameter error: %d", __LINE__);}
#define IS31_CHECK_I2C_RES(res) if(ret == ESP_FAIL) {ESP_LOGE(TAG, "Is31fl3216[%s]: FAIL\n", __FUNCTION__);} \
                                else if(ret == ESP_ERR_TIMEOUT) {ESP_LOGE(TAG, "Is31fl3216[%s]: TIMEOUT\n", __FUNCTION__);}
typedef struct {
    i2c_bus_handle_t bus;
    uint16_t addr;
} is31fl3216_dev_t;

uint8_t Is31Value[10] = {0x80, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static char *TAG = "IS31";

/**
 * @brief set software shutdown mode
 */
static esp_err_t is31fl3216_write_reg(is31fl3216_handle_t handle, is31fl3216_reg_t regAddr, uint8_t *data, uint8_t data_num)
{
    IS31_PARAM_CHECK(NULL != data);
    is31fl3216_dev_t *dev = (is31fl3216_dev_t *) handle;
    esp_err_t ret = i2c_bus_write_bytes(dev->bus, IS31FL3216_ADDRESS | IS31FL3216_WRITE_BIT, (uint8_t *)&regAddr, 1, data, data_num);
    return ret;
}

/**
 * @brief change channels PWM duty cycle data register
 */
static esp_err_t is31fl3218S_channel_duty_by_bits(is31fl3216_handle_t handle, uint32_t by_bits, uint8_t duty)
{
    for (int i = 0; i < IS31FL3216_CH_NUM_MAX; i++) {
        if ((by_bits >> i) & 0x1) {
            esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_PWM_16 + (IS31FL3216_CH_NUM_MAX - i - 1), &duty, 1);
            if (ret == ESP_OK) {
                //PASS
            } else {
                IS31_CHECK_I2C_RES(ret);
                return ret;
            }
        }
    }
    return ESP_OK;
}

/**
 * @brief Load PWM Register and LED Control Registers data
 */
esp_err_t is31fl3216_update_reg(is31fl3216_handle_t handle)
{
    IS31_PARAM_CHECK(NULL != handle);
    uint8_t m = 0;
    return is31fl3216_write_reg(handle, IS31FL3216_REG_UPDATE, &m, 1);
}

/**
 * @brief set software shutdown mode
 */
esp_err_t is31fl3216_power(is31fl3216_handle_t handle, is31fl3216_pwr_t mode)
{
    IS31_PARAM_CHECK(NULL != handle);
    if (IS31FL3216_PWR_SHUTDOWN == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(1 << 7))) | (1 << 7);
    } else if (IS31FL3216_PWR_NORMAL == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(1 << 7)));
    } else {
        return ESP_FAIL;
    }
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_CONFIG, (uint8_t *) &Is31Value[IS31FL3216_REG_CONFIG], 1);
    return ret;
}

esp_err_t is31fl3216_work_mode_set(is31fl3216_handle_t handle, is31fl3216_work_mode_t mode)
{
    IS31_PARAM_CHECK(NULL != handle);
    if (IS31FL3216_MODE_PWM == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(3 << 5)));
    } else if (IS31FL3216_MODE_AUTO_FRAME == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(3 << 5))) | (1 << 5);
    } else if (IS31FL3216_MODE_FRAME == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(3 << 5))) | (2 << 5);
    } else {
        return ESP_FAIL;
    }
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_CONFIG, (uint8_t *) &Is31Value[IS31FL3216_REG_CONFIG], 1);
    return ret;
}

/**
 * @brief change channels PWM duty cycle data register
 */
esp_err_t is31fl3216_ch_duty_set(is31fl3216_handle_t handle, is31_pwm_channel_t ch_bits, uint8_t duty)
{
    esp_err_t ret = ESP_OK;
    IS31_PARAM_CHECK(NULL != handle);
    ret = is31fl3218S_channel_duty_by_bits(handle, ch_bits, duty);
    if (ret != ESP_OK) {
        IS31_CHECK_I2C_RES(ret);
        return ret;
    }
    ret = is31fl3216_update_reg(handle);
    if (ret != ESP_OK) {
        IS31_CHECK_I2C_RES(ret);
        return ret;
    }
    return ESP_OK;
}

/**
 * @brief change channels PWM duty cycle data register
 */
esp_err_t is31fl3216_ch_enable(is31fl3216_handle_t handle, is31_pwm_channel_t ch_bits)
{
    esp_err_t ret = ESP_OK;
    IS31_PARAM_CHECK(NULL != handle);
    uint16_t value = 0;
    for (int i = 0; i < IS31FL3216_CH_NUM_MAX; ++i) {
        if ((ch_bits >> i) & 0x01) {
            value |= (1 << i);
        }
    }
    Is31Value[IS31FL3216_REG_LED_CTRL_H] |= value >> 8;
    Is31Value[IS31FL3216_REG_LED_CTRL_L] |= value;
    ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_CTRL_H, &Is31Value[IS31FL3216_REG_LED_CTRL_H], 2);
    return ret;
}

/**
 * @brief change channels PWM duty cycle data register
 */
esp_err_t is31fl3216_ch_disable(is31fl3216_handle_t handle, is31_pwm_channel_t ch_bits)
{
    esp_err_t ret = ESP_OK;
    IS31_PARAM_CHECK(NULL != handle);
    uint16_t value = ((uint16_t)Is31Value[IS31FL3216_REG_LED_CTRL_H]) << 8;
    value |= Is31Value[IS31FL3216_REG_LED_CTRL_L];
    for (int i = 0; i < IS31FL3216_CH_NUM_MAX; ++i) {
        if ((ch_bits >> i) & 0x01) {
            value = value & (~(1 << i));
        }
    }
    Is31Value[IS31FL3216_REG_LED_CTRL_H] = value >> 8;
    Is31Value[IS31FL3216_REG_LED_CTRL_L] = value;
    ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_CTRL_H, &Is31Value[IS31FL3216_REG_LED_CTRL_H], 2);
    return ret;
}

esp_err_t is31fl3216_cur_mode_set(is31fl3216_handle_t handle, is31fl3216_cur_mode_t mode)
{
    IS31_PARAM_CHECK(NULL != handle);
    if (IS31FL3216_CUR_MODE_REXT == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(1 << 4)));
    } else if (IS31FL3216_CUR_MODE_AUDIO == mode) {
        Is31Value[IS31FL3216_REG_CONFIG] = (Is31Value[IS31FL3216_REG_CONFIG] & (~(1 << 4))) | (1 << 4);
    } else {
        return ESP_FAIL;
    }
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_CONFIG, (uint8_t *) &Is31Value[IS31FL3216_REG_CONFIG], 1);
    return ret;
}

esp_err_t is31fl3216_cur_value_set(is31fl3216_handle_t handle, is31fl3216_cur_value_t value)
{
    IS31_PARAM_CHECK(NULL != handle);
    Is31Value[IS31FL3216_REG_LED_EFFECT] = (Is31Value[IS31FL3216_REG_LED_EFFECT] & (~(7 << 4))) | value << 4;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_EFFECT, &Is31Value[IS31FL3216_REG_LED_EFFECT], 1);
    return ret;
}

esp_err_t is31fl3216_ags_value_set(is31fl3216_handle_t handle, is31fl3216_ags_value_t value)
{
    IS31_PARAM_CHECK(NULL != handle);
    Is31Value[IS31FL3216_REG_LED_EFFECT] = (Is31Value[IS31FL3216_REG_LED_EFFECT] & (~(7 << 0))) | value << 0;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_EFFECT, &Is31Value[IS31FL3216_REG_LED_EFFECT], 1);
    return ret;
}

esp_err_t is31fl3216_agc_cfg(is31fl3216_handle_t handle, uint32_t en)
{
    IS31_PARAM_CHECK(NULL != handle);
    Is31Value[IS31FL3216_REG_LED_EFFECT] = (Is31Value[IS31FL3216_REG_LED_EFFECT] & (~(1 << 3))) | en << 3;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_EFFECT, &Is31Value[IS31FL3216_REG_LED_EFFECT], 1);
    return ret;
}

esp_err_t is31fl3216_cascade_mode_set(is31fl3216_handle_t handle, is31fl3216_cascade_mode_t mode)
{
    IS31_PARAM_CHECK(NULL != handle);
    Is31Value[IS31FL3216_REG_LED_EFFECT] = (Is31Value[IS31FL3216_REG_LED_EFFECT] & (~(1 << 7))) | mode << 7;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_EFFECT, &Is31Value[IS31FL3216_REG_LED_EFFECT], 1);
    return ret;
}

esp_err_t is31fl3216_sample_rate_set(is31fl3216_handle_t handle, uint32_t value)
{
    IS31_PARAM_CHECK(NULL != handle);
    uint8_t dat = value;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_ADC_RATE, &dat, 1);
    return ret;
}

esp_err_t is31fl3216_frame_time_set(is31fl3216_handle_t handle, is31fl3216_delay_time_t time)
{
    IS31_PARAM_CHECK(NULL != handle);
    uint8_t dat = time << 5;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_FRAME_DELAY, &dat, 1);
    return ret;
}

esp_err_t is31fl3216_first_frame_set(is31fl3216_handle_t handle, uint32_t frame)
{
    IS31_PARAM_CHECK(NULL != handle);
    uint8_t dat = frame << 5;
    esp_err_t ret = is31fl3216_write_reg(handle, IS31FL3216_REG_FRAME_START, &dat, 1);
    return ret;
}

esp_err_t is31fl3216_frame_value_set(is31fl3216_handle_t handle, uint32_t num, uint8_t *data, uint32_t len)
{
    IS31_PARAM_CHECK(NULL != handle);
    IS31_PARAM_CHECK(NULL != data);
    uint8_t startAddr = IS31FL3216_REG_FRAME1_CTRL + (num - 1) * 18;
    esp_err_t ret = is31fl3216_write_reg(handle, startAddr, data, len);
    return ret;
}

esp_err_t is31fl3216_reset(is31fl3216_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    uint8_t dat = 0x00;
    IS31_PARAM_CHECK(NULL != handle);
    ret = is31fl3216_power(handle, IS31FL3216_PWR_NORMAL);
    if (ret) {
        return ret;
    }
    for (int i = 0; i < IS31FL3216_CH_NUM_MAX; ++i) {
        ret = is31fl3216_ch_duty_set(handle, 1 << i, 0);
        if (ret) {
            return ret;
        }
    }
    ret = is31fl3216_ch_enable(handle, IS31FL3216_CH_ALL);
    if (ret) {
        return ret;
    }

    ret = is31fl3216_write_reg(handle, IS31FL3216_REG_LED_EFFECT, &dat, 1);
    if (ret) {
        return ret;
    }
    ret = is31fl3216_write_reg(handle, IS31FL3216_REG_CH_CONFIG, &dat, 1);
    return ret;
}

/**
 * @brief i2c master initialization
 */

is31fl3216_handle_t is31fl3216_init(void)
{
    i2c_config_t conf = {0};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    is31fl3216_dev_t *led = (is31fl3216_dev_t *) audio_calloc(1, sizeof(is31fl3216_dev_t));
    led->bus = i2c_bus_create(I2C_MASTER_NUM, &conf);
    led->addr = IS31FL3216_ADDRESS;
    IS31_ERROR_CHECK(ESP_OK == is31fl3216_power(led, IS31FL3216_PWR_NORMAL));
    IS31_ERROR_CHECK(ESP_OK == is31fl3216_cur_mode_set(led, IS31FL3216_CUR_MODE_REXT));
    IS31_ERROR_CHECK(ESP_OK == is31fl3216_cur_value_set(led, IS31FL3216_CUR_0_75));
    return (is31fl3216_handle_t) led;
}

esp_err_t is31fl3216_deinit(is31fl3216_handle_t handle)
{
    is31fl3216_dev_t *dev = (is31fl3216_dev_t *) handle;
    if (dev->bus) {
        i2c_bus_delete(dev->bus);
        dev->bus = NULL;
    }
    audio_free(dev);
    return ESP_OK;
}
