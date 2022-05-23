/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef _BOARD_PINS_CONFIG_H_
#define _BOARD_PINS_CONFIG_H_

#include "driver/i2c.h"
#include "driver/i2s.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                  Get i2c pins configuration
 *
 * @param      port        i2c port number to get configuration
 * @param      i2c_config  i2c configuration parameters
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t get_i2c_pins(i2c_port_t port, i2c_config_t *i2c_config);

/**
 * @brief                  Get i2s pins configuration
 *
 * @param      port        i2s port number to get configuration
 * @param      i2s_config  i2s configuration parameters
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t get_i2s_pins(i2s_port_t port, i2s_pin_config_t *i2s_config);

/**
 * @brief                  Get spi pins configuration
 *
 * @param      spi_config                   spi bus configuration parameters
 * @param      spi_device_interface_config  spi device configuration parameters
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t get_spi_pins(spi_bus_config_t *spi_config, spi_device_interface_config_t *spi_device_interface_config);

/**
 * @brief Set i2s mclk output pin
 *
 * @note GPIO1 and GPIO3 default are UART pins.
 *
 * @param i2s_num       i2s port index
 * @param gpio_num      gpio number index, only support GPIO0, GPIO1 and GPIO3.

 * @return
 *     - ESP_OK                     Success
 *     - ESP_ERR_INVALID_ARG        Parameter error
 *     - ESP_ERR_INVALID_STATE      Driver state error
 *     - ESP_ERR_ADF_NOT_SUPPORT    Not support
 */
esp_err_t i2s_mclk_gpio_select(i2s_port_t i2s_num, gpio_num_t gpio_num);

/**
 * @brief  Get the gpio number for sdcard interrupt
 *
 * @return  -1      non-existent
 *          Others  sdcard interrupt gpio number
 */
int8_t get_sdcard_intr_gpio(void);

/**
 * @brief  Get sdcard maximum number of open files
 *
 * @return  -1      error
 *          Others  max num
 */
int8_t get_sdcard_open_file_num_max(void);

/**
 * @brief  Get the gpio number for auxin detection
 *
 * @return  -1      non-existent
 *          Others  gpio number
 */
int8_t get_auxin_detect_gpio(void);

/**
 * @brief  Get the gpio number for headphone detection
 *
 * @return  -1      non-existent
 *          Others  gpio number
 */
int8_t get_headphone_detect_gpio(void);

/**
 * @brief  Get the gpio number for PA enable
 *
 * @return  -1      non-existent
 *          Others  gpio number
 */
int8_t get_pa_enable_gpio(void);

/**
 * @brief  Get the gpio number for adc detection
 *
 * @return  -1      non-existent
 *          Others  gpio number
 */
int8_t get_adc_detect_gpio(void);

/**
 * @brief  Get the mclk gpio number of es7243
 *
 * @return  -1      non-existent
 *          Others  gpio number
 */
int8_t get_es7243_mclk_gpio(void);

/**
 * @brief  Get the record-button id for adc-button
 *
 * @return  -1      non-existent
 *          Others  button id
 */
int8_t get_input_rec_id(void);

/**
 * @brief  Get the number for mode-button
 *
 * @return  -1      non-existent
 *          Others  number
 */
int8_t get_input_mode_id(void);

/**
 * @brief Get number for set function
 *
 * @return -1       non-existent
 *         Others   number
 */
int8_t get_input_set_id(void);

/**
 * @brief Get number for play function
 *
 * @return -1       non-existent
 *         Others   number
 */
int8_t get_input_play_id(void);

/**
 * @brief number for volume up function
 *
 * @return -1       non-existent
 *         Others   number
 */
int8_t get_input_volup_id(void);

/**
 * @brief Get number for volume down function
 *
 * @return -1       non-existent
 *         Others   number
 */
int8_t get_input_voldown_id(void);

/**
 * @brief Get green led gpio number
 *
 * @return -1       non-existent
 *        Others    gpio number
 */
int8_t get_reset_codec_gpio(void);

/**
 * @brief Get DSP reset gpio number
 *
 * @return -1       non-existent
 *         Others   gpio number
 */
int8_t get_reset_board_gpio(void);

/**
 * @brief Get DSP reset gpio number
 *
 * @return -1       non-existent
 *         Others   gpio number
 */
int8_t get_green_led_gpio(void);

/**
 * @brief Get green led gpio number
 *
 * @return -1       non-existent
 *         Others   gpio number
 */
int8_t get_blue_led_gpio(void);

#ifdef __cplusplus
}
#endif

#endif
