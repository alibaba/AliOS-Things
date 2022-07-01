// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _IOT_SCREEN_INTERFACE_DRIVER_H_
#define _IOT_SCREEN_INTERFACE_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "i2s_lcd_driver.h"
#include "i2c_bus.h"
#include "spi_bus.h"

/**
 * @brief SPI interface configuration
 * 
 */
typedef struct {
    spi_bus_handle_t spi_bus;    /*!< Handle of spi bus */
    int8_t pin_num_cs;           /*!< SPI Chip Select Pin*/
    int8_t pin_num_dc;           /*!< Pin to select Data or Command for LCD */
    int clk_freq;                /*!< SPI clock frequency */
    bool swap_data;              /*!< Whether to swap data */
} scr_interface_spi_config_t;

/**
 * @brief I2C interface configuration
 * 
 */
typedef struct {
    i2c_bus_handle_t i2c_bus;    /*!< Handle of i2c bus */
    uint32_t clk_speed;          /*!< I2C clock frequency for master mode, (no higher than 1MHz for now) */
    uint16_t slave_addr;         /*!< I2C slave address */
} scr_interface_i2c_config_t;

/**
 * @brief Type of screen interface
 * 
 */
typedef enum {
    SCREEN_IFACE_I2C,            /*!< I2C interface */
    SCREEN_IFACE_8080,           /*!< 8080 parallel interface */
    SCREEN_IFACE_SPI,            /*!< SPI interface */
} scr_interface_type_t;

/**
 * @brief Define common function for screen interface driver
 * 
 */
typedef struct {
    scr_interface_type_t type;                                                  /*!< Interface bus type, see scr_interface_type_t struct */
    esp_err_t (*write_cmd)(void *handle, uint16_t cmd);                         /*!< Function to write a command */
    esp_err_t (*write_data)(void *handle, uint16_t data);                       /*!< Function to write a data */
    esp_err_t (*write)(void *handle, const uint8_t *data, uint32_t length);     /*!< Function to write a block data */
    esp_err_t (*read)(void *handle, uint8_t *data, uint32_t length);            /*!< Function to read a block data */
    esp_err_t (*bus_acquire)(void *handle);                                     /*!< Function to acquire interface bus */
    esp_err_t (*bus_release)(void *handle);                                     /*!< Function to release interface bus */
} scr_interface_driver_t;

/**
 * @brief Create screen interface driver
 * 
 * @param type Type of screen interface
 * @param config configuration of interface driver
 * @param out_driver Pointer to a screen interface driver
 * 
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG   Arguments is NULL.
 *      - ESP_FAIL Initialize failed
 *      - ESP_ERR_NO_MEM: Cannot allocate memory.
 */
esp_err_t scr_interface_create(scr_interface_type_t type, void *config, scr_interface_driver_t **out_driver);

/**
 * @brief Delete screen interface driver
 * 
 * @param driver screen interface driver to delete
 * 
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG   Arguments is NULL.
 */
esp_err_t scr_interface_delete(const scr_interface_driver_t *driver);

#ifdef __cplusplus
}
#endif

#endif
