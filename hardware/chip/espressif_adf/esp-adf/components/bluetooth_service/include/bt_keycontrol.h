/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef __BT_KEYCONTROL_H__
#define __BT_KEYCONTROL_H__


#if CONFIG_BT_ENABLED

#include "esp_avrc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *brief      Bluetooth key action parameter
 */
typedef struct {
    uint32_t evt;       /*!< AVRC Controller callback events */
    uint32_t key_code;  /*!< passthrough command code */
    uint8_t key_state;  /*!< 0 for PRESSED, 1 for RELEASED */
    uint8_t tl;         /*!< transaction label, 0 to 15 */
} bt_key_act_param_t;

/**
 *brief      Bluetooth AVRC callback events
 */
#define ESP_AVRC_CT_KEY_STATE_CHG_EVT (ESP_AVRC_CT_REMOTE_FEATURES_EVT + 1) /*!< key-press action is triggered */
#define ESP_AVRC_CT_PT_RSP_TO_EVT (ESP_AVRC_CT_REMOTE_FEATURES_EVT + 2)     /*!< passthrough-command response time-out */

/**
 * brief      Bluetooth peripheral event id
 */
typedef enum {
    PERIPH_BLUETOOTH_UNKNOWN = 0,       /*!< No event */
    PERIPH_BLUETOOTH_CONNECTED,         /*!< A bluetooth device was connected */
    PERIPH_BLUETOOTH_DISCONNECTED,      /*!< Last connection was disconnected */
    PERIPH_BLUETOOTH_AUDIO_STARTED,     /*!< The audio session has been started */
    PERIPH_BLUETOOTH_AUDIO_SUSPENDED,   /*!< The audio session has been suspended */
    PERIPH_BLUETOOTH_AUDIO_STOPPED,     /*!< The audio session has been stopped */
} periph_bluetooth_event_id_t;

/**
 * @brief      Initialize bluetooth key action state machine
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t bt_key_act_sm_init(void);

/**
 * @brief      De-initialize bluetooth key action state machine
 */
void bt_key_act_sm_deinit(void);

/**
 * @brief      Set bluetooth key action state
 *
 * @param[in]  Bluetooth key action parameter
 *
 */
void bt_key_act_state_machine(bt_key_act_param_t *param);

#ifdef __cplusplus
}
#endif

#endif

#endif /* __BT_KEYCONTROL_H__ */
