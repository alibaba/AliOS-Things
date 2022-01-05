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
#ifndef _IOT_I2C_BUS_H_
#define _IOT_I2C_BUS_H_

#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *i2c_bus_handle_t;

/**
 * @brief Create and init I2C bus and return a I2C bus handle
 *
 * @param port       I2C port number
 * @param conf       Pointer to I2C parameters
 *
 * @return
 *     - I2C bus handle
 */
i2c_bus_handle_t i2c_bus_create(i2c_port_t port, i2c_config_t *conf);

/**
 * @brief Write bytes to I2C bus
 *
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 * @param reg        The register of the device
 * @param regLen     The length of register
 * @param data       The data pointer
 * @param datalen    The length of data
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
esp_err_t i2c_bus_write_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int regLen, uint8_t *data, int datalen);

/**
 * @brief Write data to I2C bus
 *
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 * @param data       The data pointer
 * @param datalen    The length of data
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
esp_err_t i2c_bus_write_data(i2c_bus_handle_t bus, int addr, uint8_t *data, int datalen);

/**
 * @brief Read bytes to I2C bus
 *
 * @param bus        I2C bus handle
 * @param addr       The address of the device
 * @param reg        The register of the device
 * @param regLen     The length of register
 * @param outdata    The outdata pointer
 * @param datalen        The length of outdata
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
esp_err_t i2c_bus_read_bytes(i2c_bus_handle_t bus, int addr, uint8_t *reg, int reglen, uint8_t *outdata, int datalen);

/**
 * @brief Delete and release the I2C bus object
 *
 * @param bus        I2C bus handle
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_delete(i2c_bus_handle_t bus);

/**
 * @brief I2C start sending buffered commands
 *
 * @param bus            I2C bus handle
 * @param cmd            I2C cmd handle
 * @param ticks_to_wait  Maximum blocking time
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_cmd_begin(i2c_bus_handle_t bus, i2c_cmd_handle_t cmd, portBASE_TYPE ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif
