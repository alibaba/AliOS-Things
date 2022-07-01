// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _I2C_BUS_H_
#define _I2C_BUS_H_
#include "driver/i2c.h"

#define NULL_I2C_MEM_ADDR 0xFF /*!< set mem_address to NULL_I2C_MEM_ADDR if i2c device has no internal address during read/write */
#define NULL_I2C_DEV_ADDR 0xFF /*!< invalid i2c device address */
typedef void *i2c_bus_handle_t; /*!< i2c bus handle */
typedef void *i2c_bus_device_handle_t; /*!< i2c device handle */

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************** Public Functions (Application level)*********************************************/

/**
 * @brief Create an I2C bus instance then return a handle if created successfully. Each I2C bus works in a singleton mode,
 * which means for an i2c port only one group parameter works. When i2c_bus_create is called more than one time for the
 * same i2c port, following parameter will override the previous one.
 *
 * @param port I2C port number
 * @param conf Pointer to I2C bus configuration
 * @return i2c_bus_handle_t Return the I2C bus handle if created successfully, return NULL if failed.
 */
i2c_bus_handle_t i2c_bus_create(i2c_port_t port, const i2c_config_t *conf);

/**
 * @brief Delete and release the I2C bus resource.
 *
 * @param p_bus_handle Point to the I2C bus handle, if delete succeed handle will set to NULL.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_delete(i2c_bus_handle_t *p_bus_handle);

/**
 * @brief Scan i2c devices attached on i2c bus
 *
 * @param bus_handle I2C bus handle
 * @param buf Pointer to a buffer to save devices' address, if NULL no address will be saved.
 * @param num Maximum number of addresses to save, invalid if buf set to NULL,
 * higer addresses will be discarded if num less-than the total number found on the I2C bus.
 * @return uint8_t Total number of devices found on the I2C bus
 */
uint8_t i2c_bus_scan(i2c_bus_handle_t bus_handle, uint8_t *buf, uint8_t num);

/**
 * @brief Get current active clock speed.
 * 
 * @param bus_handle I2C bus handle
 * @return uint32_t current clock speed
 */
uint32_t i2c_bus_get_current_clk_speed(i2c_bus_handle_t bus_handle);

/**
 * @brief Get created device number of the bus.
 * 
 * @param bus_handle I2C bus handle
 * @return uint8_t created device number of the bus
 */
uint8_t i2c_bus_get_created_device_num(i2c_bus_handle_t bus_handle);

/**
 * @brief Create an I2C device on specific bus.
 *        Dynamic configuration must be enable to achieve multiple devices with different configs on a single bus.
 *        menuconfig:Bus Options->I2C Bus Options->enable dynamic configuration
 * 
 * @param bus_handle Point to the I2C bus handle
 * @param dev_addr i2c device address
 * @param clk_speed device specified clock frequency the i2c_bus will switch to during each transfer. 0 if use current bus speed.
 * @return i2c_bus_device_handle_t return a device handle if created successfully, return NULL if failed.
 */
i2c_bus_device_handle_t i2c_bus_device_create(i2c_bus_handle_t bus_handle, uint8_t dev_addr, uint32_t clk_speed);

/**
 * @brief Delete and release the I2C device resource, i2c_bus_device_delete should be used in pairs with i2c_bus_device_create.
 *
 * @param p_dev_handle Point to the I2C device handle, if delete succeed handle will set to NULL.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t i2c_bus_device_delete(i2c_bus_device_handle_t *p_dev_handle);

/**
 * @brief Get device's I2C address
 * 
 * @param dev_handle I2C device handle
 * @return uint8_t I2C address, return NULL_I2C_DEV_ADDR if dev_handle is invalid.
 */
uint8_t i2c_bus_device_get_address(i2c_bus_device_handle_t dev_handle);

/**
 * @brief Read single byte from i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to read from, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data Pointer to a buffer to save the data that was read
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_read_byte(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t *data);

/**
 * @brief Read multiple bytes from i2c device with 8-bit internal register/memory address.
 * If internal reg/mem address is 16-bit, please refer i2c_bus_read_reg16
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to read from, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data_len Number of bytes to read
 * @param data Pointer to a buffer to save the data that was read
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_read_bytes(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, uint8_t *data);

/**
 * @brief Read single bit of a byte from i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to read from, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param bit_num The bit number 0 - 7 to read
 * @param data Pointer to a buffer to save the data that was read. *data == 0 -> bit = 0, *data !=0 -> bit = 1.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_read_bit(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_num, uint8_t *data);

/**
 * @brief Read multiple bits of a byte from i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to read from, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param bit_start The bit to start from, 0 - 7, MSB at 0
 * @param length The number of bits to read, 1 - 8
 * @param data Pointer to a buffer to save the data that was read
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_read_bits(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_start, uint8_t length, uint8_t *data);

/**
 * @brief Write single byte to i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to write to, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data The byte to write.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_write_byte(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t data);

/**
 * @brief Write multiple byte to i2c device with 8-bit internal register/memory address
 * If internal reg/mem address is 16-bit, please refer i2c_bus_write_reg16
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to write to, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data_len Number of bytes to write
 * @param data Pointer to the bytes to write.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_write_bytes(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, size_t data_len, const uint8_t *data);

/**
 * @brief Write single bit of a byte to an i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to write to, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param bit_num The bit number 0 - 7 to write
 * @param data The bit to write, data == 0 means set bit = 0, data !=0 means set bit = 1.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_write_bit(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_num, uint8_t data);

/**
 * @brief Write multiple bits of a byte to an i2c device with 8-bit internal register/memory address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal reg/mem address to write to, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param bit_start The bit to start from, 0 - 7, MSB at 0
 * @param length The number of bits to write, 1 - 8
 * @param data The bits to write.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_write_bits(i2c_bus_device_handle_t dev_handle, uint8_t mem_address, uint8_t bit_start, uint8_t length, uint8_t data);

/**************************************** Public Functions (Low level)*********************************************/

/**
 * @brief I2C master send queued commands create by ``i2c_cmd_link_create`` .
 *        This function will trigger sending all queued commands.
 *        The task will be blocked until all the commands have been sent out.
 *        If I2C_BUS_DYNAMIC_CONFIG enable, i2c_bus will dynamically check configs and re-install i2c driver before each transfer,
 *        hence multiple devices with different configs on a single bus can be supported.
 *        @note
 *        Only call this function when ``i2c_bus_read/write_xx`` do not meet the requirements
 * 
 * @param dev_handle I2C device handle
 * @param cmd I2C command handler
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_cmd_begin(i2c_bus_device_handle_t dev_handle, i2c_cmd_handle_t cmd);

/**
 * @brief Write date to an i2c device with 16-bit internal reg/mem address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal 16-bit reg/mem address to write to, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data_len Number of bytes to write
 * @param data Pointer to the bytes to write.
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_write_reg16(i2c_bus_device_handle_t dev_handle, uint16_t mem_address, size_t data_len, const uint8_t *data);

/**
 * @brief Read date from i2c device with 16-bit internal reg/mem address
 *
 * @param dev_handle I2C device handle
 * @param mem_address The internal 16-bit reg/mem address to read from, set to NULL_I2C_MEM_ADDR if no internal address.
 * @param data_len Number of bytes to read
 * @param data Pointer to a buffer to save the data that was read
 * @return esp_err_t 
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_bus_read_reg16(i2c_bus_device_handle_t dev_handle, uint16_t mem_address, size_t data_len, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif
