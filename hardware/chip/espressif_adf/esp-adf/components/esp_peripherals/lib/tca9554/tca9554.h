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

#ifndef _TCA9554_H
#define _TCA9554_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TCA9554_GPIO_NUM_0 = BIT(0),
    TCA9554_GPIO_NUM_1 = BIT(1),
    TCA9554_GPIO_NUM_2 = BIT(2),
    TCA9554_GPIO_NUM_3 = BIT(3),
    TCA9554_GPIO_NUM_4 = BIT(4),
    TCA9554_GPIO_NUM_5 = BIT(5),
    TCA9554_GPIO_NUM_6 = BIT(6),
    TCA9554_GPIO_NUM_7 = BIT(7),
    TCA9554_GPIO_NUM_MAX
} esp_tca9554_gpio_num_t;

typedef enum {
    TCA9554_IO_LOW,
    TCA9554_IO_HIGH,
    TCA9554_LEVEL_ERROR
} esp_tca9554_io_level_t;

typedef enum {
    TCA9554_IO_RETAINED,
    TCA9554_IO_INVERTED
} esp_tca9554_io_polarity_t;

typedef enum {
    TCA9554_IO_OUTPUT,
    TCA9554_IO_INPUT
} esp_tca9554_io_config_t;

typedef struct {
    gpio_num_t i2c_sda;
    gpio_num_t i2c_scl;
    gpio_num_t interrupt_output;
} esp_tca9554_config_t;

/*
 * @brief Initialize TCA9554 chip
 *
 * @param codec_cfg  configuration of TCA9554
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t tca9554_init(esp_tca9554_config_t *cfg);

/**
 * @brief Deinitialize TCA9554 chip
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t tca9554_deinit(void);

/*
 * @brief Get TCA9554 input level
 *
 * @param gpio_num  GPIO of TCA9554
 *
 * @return
 *      - esp_tca9554_io_level_t
 */
esp_tca9554_io_level_t tca9554_get_input_state(esp_tca9554_gpio_num_t gpio_num);

/*
 * @brief Get PCA95xian39 output level
 *
 * @param gpio_num  GPIO of TCA9554
 *
 * @return
 *      - esp_tca9554_io_level_t
 */
esp_tca9554_io_level_t tca9554_get_output_state(esp_tca9554_gpio_num_t gpio_num);

/*
 * @brief Get TCA9554 output state
 *
 * @param gpio_num  GPIO of TCA9554
 *
 * @return
 *      - esp_tca9554_io_level_t
 */
esp_err_t tca9554_set_output_state(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_level_t level);

/*
 * @brief Set TCA9554 polarity
 *
 * @param gpio_num  GPIO of TCA9554
 *        polarity  polarity
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t tca9554_set_polarity_inversion(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_polarity_t polarity);

/*
 * @brief Get TCA9554 output level
 *
 * @param gpio_num  GPIO of TCA9554
 *
 * @return
 *      - esp_tca9554_io_level_t
 */
esp_tca9554_io_config_t tca9554_get_io_config(esp_tca9554_gpio_num_t gpio_num);

/*
 * @brief Set TCA9554 io config
 *
 * @param gpio_num  GPIO of TCA9554
 *        io_config  io config
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t tca9554_set_io_config(esp_tca9554_gpio_num_t gpio_num, esp_tca9554_io_config_t io_config);

/**
 * @brief Print all TCA9554 registers
 *
 * @return
 *     - void
 */
void tca9554_read_all();

#ifdef __cplusplus
}
#endif

#endif
