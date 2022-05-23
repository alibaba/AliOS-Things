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

#ifndef _A2DP_STREAM_H_
#define _A2DP_STREAM_H_

#include "audio_error.h"
#include "audio_element.h"
#include "audio_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_ENABLED
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "bt_keycontrol.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

typedef struct {
    esp_a2d_cb_t                user_a2d_cb;
    esp_a2d_sink_data_cb_t      user_a2d_sink_data_cb;
    esp_a2d_source_data_cb_t    user_a2d_source_data_cb;
} a2dp_stream_user_callback_t;

typedef struct {
    audio_stream_type_t         type;
    a2dp_stream_user_callback_t user_callback;
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    audio_hal_handle_t          audio_hal;
#endif
} a2dp_stream_config_t;

/**
 * a2dp task is only created in a2dp sink mode
 */
#define A2DP_STREAM_TASK_STACK          ( 2 * 1024 )
#define A2DP_STREAM_TASK_CORE           ( 0 )
#define A2DP_STREAM_TASK_PRIO           ( 22 )
#define A2DP_STREAM_TASK_IN_EXT         ( true )
#define A2DP_STREAM_QUEUE_SIZE          ( 20 )

/**
 * @brief      Create a handle to an Audio Element to stream data from A2DP to another Element
 *             or get data from other elements sent to A2DP, depending on the configuration
 *             the stream type, either AUDIO_STREAM_READER or AUDIO_STREAM_WRITER.
 *
 * @param      config  The configuration
 *
 * @return     The Audio Element handle
 */
audio_element_handle_t a2dp_stream_init(a2dp_stream_config_t *config);

/**
 * @brief      Destroy and cleanup A2DP profile.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t a2dp_destroy();

/**
 * @brief      Create Bluetooth peripheral.
 *             The returned bluetooth peripheral compatible with existing peripherals and can be used with the ESP Peripherals
 *
 * @return     The Peripheral handle
 */
esp_periph_handle_t bt_create_periph();

/**
 * @brief      Send the AVRC passthrough command (PLAY) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_play(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (PAUSE) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_pause(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (STOP) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_stop(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (NEXT) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_avrc_next(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (PREV) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_avrc_prev(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (REWIND) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_avrc_rewind(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (FAST FORWARD) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_avrc_fast_forward(esp_periph_handle_t periph);

/**
 * @brief      Start device discovery.
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK : Succeed
 *     - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *     - ESP_ERR_INVALID_ARG: if invalid parameters are provided
 *     - ESP_FAIL: others
 */
esp_err_t periph_bt_discover(esp_periph_handle_t periph);
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
/**
 * @brief      Send the AVRC passthrough command (VOL_UP) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_volume_up(esp_periph_handle_t periph);

/**
 * @brief      Send the AVRC passthrough command (VOL_DOWN) to the Bluetooth device
 *
 * @param[in]  periph  The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_bt_volume_down(esp_periph_handle_t periph);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
