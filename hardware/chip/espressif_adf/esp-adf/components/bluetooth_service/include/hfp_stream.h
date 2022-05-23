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

#ifndef _HFP_STREAM_H_
#define _HFP_STREAM_H_

#include "audio_element.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BT_ENABLED
#include "esp_bt.h"
#include "esp_hf_client_api.h"

typedef enum {
    HF_DATA_CVSD = 0,
    HF_DATA_MSBC = 1,
} hfp_data_enc_type_t;

typedef enum {
    INCOMING_STREAM = 0,
    OUTGOING_STREAM = 1,
} hfp_stream_type_t;

typedef void (* esp_hf_audio_open_t)(hfp_data_enc_type_t type);
typedef void (* esp_hf_audio_close_t)(void);

typedef struct {
    esp_hf_audio_open_t     user_hfp_open_cb;
    esp_hf_audio_close_t     user_hfp_close_cb;
} hfp_stream_user_callback_t;

typedef struct {
    hfp_stream_type_t         type;
} hfp_stream_config_t;

/**
 * @brief      Register hfp audio open and close event callback function for application.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t hfp_open_and_close_evt_cb_register(esp_hf_audio_open_t open_cb, esp_hf_audio_close_t close_cb);

/**
 * @brief      Initialize and start the hfp service. This function can only be called for one time.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t hfp_service_init(void);

/**
 * @brief      Create hfp stream, it is valid when Bluetooth is enabled.
 *
 * @return     The Audio Element handle
 */
audio_element_handle_t hfp_stream_init(hfp_stream_config_t *config);
#endif

#ifdef __cplusplus
}
#endif

#endif
