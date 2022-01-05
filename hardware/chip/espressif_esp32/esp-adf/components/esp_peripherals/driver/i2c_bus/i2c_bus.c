/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c_bus.h"
#include "audio_mutex.h"
#include "audio_mem.h"

#define ESP_INTR_FLG_DEFAULT  (0)
#define ESP_I2C_MASTER_BUF_LEN  (0)
#define I2C_ACK_CHECK_EN 1

#define I2C_BUS_CHECK(a, str, ret)  if(!(a)) {                               \
    ESP_LOGE(TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
    return (ret);                                                            \
}

typedef struct {
    i2c_config_t i2c_conf;   /*!<I2C bus parameters*/
    i2c_port_t i2c_port;     /*!<I2C port number */
} i2c_bus_t;

static const char *TAG = "I2C_BUS";

static i2c_bus_t *i2c_bus[I2C_NUM_MAX];

static xSemaphoreHandle _busLock;

i2c_bus_handle_t i2c_bus_create(i2c_port_t port, i2c_config_t *conf)
{
    I2C_BUS_CHECK(port < I2C_NUM_MAX, "I2C port error", NULL);
    I2C_BUS_CHECK(conf != NULL, "Configuration not initialized", NULL);
    if (i2c_bus[port]) {
        ESP_LOGW(TAG, "%s:%d: I2C bus has been already created, [port:%d]", __FUNCTION__, __LINE__, port);
        return i2c_bus[port];
    }
    i2c_bus[port] = (i2c_bus_t *) audio_calloc(1, sizeof(i2c_bus_t));
    i2c_bus[port]->i2c_conf = *conf;
    i2c_bus[port]->i2c_port = port;
    esp_err_t ret = i2c_param_config(i2c_bus[port]->i2c_port, &i2c_bus[port]->i2c_conf);
    if (ret != ESP_OK) {
        goto error;
    }
    ret = i2c_driver_install(i2c_bus[port]->i2c_port, i2c_bus[port]->i2c_conf.mode, ESP_I2C_MASTER_BUF_LEN, ESP_I2C_MASTER_BUF_LEN, ESP_INTR_FLG_DEFAULT);
    if (ret != ESP_OK) {
        goto error;
    }
    if (_busLock) {
        mutex_destroy(_busLock);
    }
    _busLock = mutex_create();
    return (i2c_bus_handle_t) i2c_bus[port];

error:
    if (i2c_bus[port]) {
        audio_free(i2c_bus[port]);
    }
    return NULL;
}

esp_err_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int regLen, uint8_t *data, int datalen)
{
    I2C_BUS_CHECK(bus != NULL, "Handle error", ESP_FAIL);
    i2c_bus_t *p_bus = (i2c_bus_t *) bus;
    I2C_BUS_CHECK(p_bus->i2c_port < I2C_NUM_MAX, "I2C port error", ESP_FAIL);
    I2C_BUS_CHECK(data != NULL, "Not initialized input data pointer", ESP_FAIL);
    esp_err_t ret = ESP_OK;
    mutex_lock(_busLock);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, addr, 1);
    ret |= i2c_master_write(cmd, reg, regLen, I2C_ACK_CHECK_EN);
    ret |= i2c_master_write(cmd, data, datalen, I2C_ACK_CHECK_EN);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(p_bus->i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    mutex_unlock(_busLock);
    I2C_BUS_CHECK(ret == 0, "I2C Bus WriteReg Error", ESP_FAIL);
    return ret;
}

esp_err_t i2c_bus_write_data(i2c_bus_handle_t bus, int addr, uint8_t *data, int datalen)
{
    I2C_BUS_CHECK(bus != NULL, "Handle error", ESP_FAIL);
    i2c_bus_t *p_bus = (i2c_bus_t *) bus;
    I2C_BUS_CHECK(p_bus->i2c_port < I2C_NUM_MAX, "I2C port error", ESP_FAIL);
    I2C_BUS_CHECK(data != NULL, "Not initialized input data pointer", ESP_FAIL);
    esp_err_t ret = ESP_OK;
    mutex_lock(_busLock);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, addr, 1);
    ret |= i2c_master_write(cmd, data, datalen, I2C_ACK_CHECK_EN);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(p_bus->i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    mutex_unlock(_busLock);
    I2C_BUS_CHECK(ret == 0, "I2C Bus WriteReg Error", ESP_FAIL);
    return ret;
}

esp_err_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int reglen, uint8_t *outdata, int datalen)
{
    I2C_BUS_CHECK(bus != NULL, "Handle error", ESP_FAIL);
    i2c_bus_t *p_bus = (i2c_bus_t *) bus;
    I2C_BUS_CHECK(p_bus->i2c_port < I2C_NUM_MAX, "I2C port error", ESP_FAIL);
    I2C_BUS_CHECK(outdata != NULL, "Not initialized output data buffer pointer", ESP_FAIL);
    esp_err_t ret = ESP_OK;
    mutex_lock(_busLock);
    i2c_cmd_handle_t cmd;
    cmd = i2c_cmd_link_create();
    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, addr, I2C_ACK_CHECK_EN);
    ret |= i2c_master_write(cmd, reg, reglen, I2C_ACK_CHECK_EN);
    ret |= i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(p_bus->i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    ret |= i2c_master_start(cmd);
    ret |= i2c_master_write_byte(cmd, addr | 0x01, I2C_ACK_CHECK_EN);

    for (int i = 0; i < datalen - 1; i++) {
        ret |= i2c_master_read_byte(cmd, &outdata[i], 0);
    }
    ret |= i2c_master_read_byte(cmd, &outdata[datalen - 1], 1);

    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(p_bus->i2c_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    mutex_unlock(_busLock);
    I2C_BUS_CHECK(ret == 0, "I2C Bus ReadReg Error", ESP_FAIL);
    return ret;
}

esp_err_t i2c_bus_delete(i2c_bus_handle_t bus)
{
    I2C_BUS_CHECK(bus != NULL, "Handle error", ESP_FAIL);
    i2c_bus_t *p_bus = (i2c_bus_t *) bus;
    i2c_driver_delete(p_bus->i2c_port);
    i2c_bus[p_bus->i2c_port] = NULL;
    audio_free(p_bus);
    mutex_destroy(_busLock);

    _busLock = NULL;
    return ESP_OK;
}

esp_err_t i2c_bus_cmd_begin(i2c_bus_handle_t bus, i2c_cmd_handle_t cmd, portBASE_TYPE ticks_to_wait)
{
    I2C_BUS_CHECK(bus != NULL, "Handle error", ESP_FAIL);
    I2C_BUS_CHECK(cmd != NULL, "I2C cmd error", ESP_FAIL);
    i2c_bus_t *p_bus = (i2c_bus_t *) bus;

    esp_err_t ret = i2c_master_cmd_begin(p_bus->i2c_port, cmd, ticks_to_wait);
    return ret;
}
