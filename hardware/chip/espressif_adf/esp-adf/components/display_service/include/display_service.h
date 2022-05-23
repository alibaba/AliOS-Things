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

#ifndef _DISPLAY_SERVICE_H_
#define _DISPLAY_SERVICE_H_

#include "periph_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DISPLAY_PATTERN_UNKNOWN                 = 0,
    DISPLAY_PATTERN_WIFI_SETTING            = 1,
    DISPLAY_PATTERN_WIFI_CONNECTTING        = 2,
    DISPLAY_PATTERN_WIFI_CONNECTED          = 3,
    DISPLAY_PATTERN_WIFI_DISCONNECTED       = 4,
    DISPLAY_PATTERN_WIFI_SETTING_FINISHED   = 5,

    DISPLAY_PATTERN_BT_CONNECTTING          = 6,
    DISPLAY_PATTERN_BT_CONNECTED            = 7,
    DISPLAY_PATTERN_BT_DISCONNECTED         = 8,

    DISPLAY_PATTERN_RECORDING_START         = 9,
    DISPLAY_PATTERN_RECORDING_STOP          = 10,
    DISPLAY_PATTERN_RECOGNITION_START       = 11,
    DISPLAY_PATTERN_RECOGNITION_STOP        = 12,

    DISPLAY_PATTERN_WAKEUP_ON               = 13,
    DISPLAY_PATTERN_WAKEUP_FINISHED         = 14,
    DISPLAY_PATTERN_MUSIC_ON                = 15,
    DISPLAY_PATTERN_MUSIC_FINISHED          = 16,

    DISPLAY_PATTERN_VOLUME                  = 17,
    DISPLAY_PATTERN_MUTE_ON                 = 18,
    DISPLAY_PATTERN_MUTE_OFF                = 19,
    DISPLAY_PATTERN_TURN_ON                 = 20,
    DISPLAY_PATTERN_TURN_OFF                = 21,
    DISPLAY_PATTERN_BATTERY_LOW             = 22,
    DISPLAY_PATTERN_BATTERY_CHARGING        = 23,
    DISPLAY_PATTERN_BATTERY_FULL            = 24,

    DISPLAY_PATTERN_POWERON_INIT            = 25,
    DISPLAY_PATTERN_WIFI_NO_CFG             = 26,
    DISPLAY_PATTERN_SPEECH_BEGIN            = 27,
    DISPLAY_PATTERN_SPEECH_OVER             = 28,

    DISPLAY_PATTERN_MAX,
} display_pattern_t;

typedef struct display_service_impl *display_service_handle_t;

/**
 * @brief Display service configurations
 */
typedef struct {
    periph_service_config_t          based_cfg;     /*!< Peripheral service configuration */
    void                             *instance;     /*!< Sub-instance */
} display_service_config_t;

/*
 * @brief Create the display service
 *
 * @param cfg   A pointer to display_service_config_t
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
display_service_handle_t display_service_create(display_service_config_t *cfg);

/*
 * @brief Set service display pattern
 *
 * @param handle            A pointer to display_service_handle_t
 * @param disp_pattern      Enumeration of display_pattern_t
 * @param value             Data of the disp_pattern
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t display_service_set_pattern(void *handle, int disp_pattern, int value);

/*
 * @brief Destroy display service
 *
 * @param handle            A pointer to display_service_handle_t
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t display_destroy(display_service_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
