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

#ifndef _PERIPH_AW2013_H_
#define _PERIPH_AW2013_H_

#include "esp_peripherals.h"
#include "aw2013.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AW2013_MODE_LED,
    AW2013_MODE_FADE,
    AW2013_MODE_AUTO
} periph_aw2013_mode_t;

/**
 * @brief Configuration of aw2013
 */
typedef struct {
    periph_aw2013_mode_t mode;          /*!< Work mode of aw2013 */
    aw2013_brightness_t bright;  /*!< The brightness of aw2013 */
    uint32_t rgb_value;                 /*!< rgb value to be set */
} periph_aw2013_cfg_t;

/**
 * @brief Initializate aw2013
 *
 * @param aw2013_cfg  Parameters of aw2013
 *
 * @return
 *     - NULL   Error
 *     - others Success
 */
esp_periph_handle_t periph_aw2013_init(periph_aw2013_cfg_t *aw2013_cfg);

/**
 * @brief Set the brightness of aw2013
 *
 * @param periph  The aw2013's handle
 * @param bright  The brightness to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Error
 */
esp_err_t periph_aw2013_set_brightless(esp_periph_handle_t periph, aw2013_brightness_t bright);

/**
 * @brief Set the time periods of aw2013
 *
 * @param periph  The aw2013's handle
 * @param time    The time period to be set
 * @param level   The time value to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Error
 */
esp_err_t periph_aw2013_set_time(esp_periph_handle_t periph, aw2013_time_t time, aw2013_time_level_t level);

/**
 * @brief Set the work mode of aw2013
 *
 * @param periph  The aw2013's handle
 * @param mode    The work mode to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Error
 */
esp_err_t periph_aw2013_set_mode(esp_periph_handle_t periph, periph_aw2013_mode_t mode);

/**
 * @brief Set the rgb value of aw2013
 *
 * @param periph  The aw2013's handle
 * @param value   The value for rgb to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Error
 */
esp_err_t periph_aw2013_set_rgb_value(esp_periph_handle_t periph, uint32_t value);

/**
 * @brief Set the repeat time in auto flash mode
 *
 * @param periph  The aw2013's handle
 * @param cnt     Cycle times to be set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Error
 */
esp_err_t periph_aw2013_set_repeat_time(esp_periph_handle_t periph, uint8_t cnt);

#ifdef __cplusplus
}
#endif

#endif
