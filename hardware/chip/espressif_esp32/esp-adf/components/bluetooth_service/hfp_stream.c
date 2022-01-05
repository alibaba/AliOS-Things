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

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "raw_stream.h"
#include "audio_element.h"
#include "audio_mem.h"
#include "sdkconfig.h"
#include "hfp_stream.h"

#if CONFIG_BT_ENABLED
static const char *TAG = "HFP_STREAM";

#define ESP_HFP_RINGBUF_SIZE     3600
#define ESP_HFP_TASK_SIZE        2048
#define ESP_HFP_TASK_PRIORITY    23
static bool is_get_data = true;
static hfp_stream_user_callback_t  hfp_stream_user_callback;
static audio_element_handle_t hfp_incoming_stream = NULL;
static audio_element_handle_t hfp_outgoing_stream = NULL;

const char *c_hf_evt_str[] = {
    "CONNECTION_STATE_EVT",              /*!< connection state changed event */
    "AUDIO_STATE_EVT",                   /*!< audio connection state change event */
    "VR_STATE_CHANGE_EVT",                /*!< voice recognition state changed */
    "CALL_IND_EVT",                      /*!< call indication event */
    "CALL_SETUP_IND_EVT",                /*!< call setup indication event */
    "CALL_HELD_IND_EVT",                 /*!< call held indicator event */
    "NETWORK_STATE_EVT",                 /*!< network state change event */
    "SIGNAL_STRENGTH_IND_EVT",           /*!< signal strength indication event */
    "ROAMING_STATUS_IND_EVT",            /*!< roaming status indication event */
    "BATTERY_LEVEL_IND_EVT",             /*!< battery level indication event */
    "CURRENT_OPERATOR_EVT",              /*!< current operator name event */
    "RESP_AND_HOLD_EVT",                 /*!< response and hold event */
    "CLIP_EVT",                          /*!< Calling Line Identification notification event */
    "CALL_WAITING_EVT",                  /*!< call waiting notification */
    "CLCC_EVT",                          /*!< listing current calls event */
    "VOLUME_CONTROL_EVT",                /*!< audio volume control event */
    "AT_RESPONSE",                       /*!< audio volume control event */
    "SUBSCRIBER_INFO_EVT",               /*!< subscriber information event */
    "INBAND_RING_TONE_EVT",              /*!< in-band ring tone settings */
    "LAST_VOICE_TAG_NUMBER_EVT",         /*!< requested number from AG event */
    "RING_IND_EVT",                      /*!< ring indication event */
};

// esp_hf_client_connection_state_t
const char *c_connection_state_str[] = {
    "disconnected",
    "connecting",
    "connected",
    "slc_connected",
    "disconnecting",
};

// esp_hf_client_audio_state_t
const char *c_audio_state_str[] = {
    "disconnected",
    "connecting",
    "connected",
    "connected_msbc",
};

/// esp_hf_vr_state_t
const char *c_vr_state_str[] = {
    "disabled",
    "enabled",
};

// esp_hf_service_availability_status_t
const char *c_service_availability_status_str[] = {
    "unavailable",
    "available",
};

// esp_hf_roaming_status_t
const char *c_roaming_status_str[] = {
    "inactive",
    "active",
};

// esp_hf_client_call_state_t
const char *c_call_str[] = {
    "NO call in progress",
    "call in progress",
};

// esp_hf_client_callsetup_t
const char *c_call_setup_str[] = {
    "NONE",
    "INCOMING",
    "OUTGOING_DIALING",
    "OUTGOING_ALERTING"
};

// esp_hf_client_callheld_t
const char *c_call_held_str[] = {
    "NONE held",
    "Held and Active",
    "Held",
};

// esp_hf_response_and_hold_status_t
const char *c_resp_and_hold_str[] = {
    "HELD",
    "HELD ACCEPTED",
    "HELD REJECTED",
};

// esp_hf_client_call_direction_t
const char *c_call_dir_str[] = {
    "outgoing",
    "incoming",
};

// esp_hf_client_call_state_t
const char *c_call_state_str[] = {
    "active",
    "held",
    "dialing",
    "alerting",
    "incoming",
    "waiting",
    "held_by_resp_hold",
};

// esp_hf_current_call_mpty_type_t
const char *c_call_mpty_type_str[] = {
    "single",
    "multi",
};

// esp_hf_volume_control_target_t
const char *c_volume_control_target_str[] = {
    "SPEAKER",
    "MICROPHONE"
};

// esp_hf_at_response_code_t
const char *c_at_response_code_str[] = {
    "OK",
    "ERROR"
    "ERR_NO_CARRIER",
    "ERR_BUSY",
    "ERR_NO_ANSWER",
    "ERR_DELAYED",
    "ERR_BLACKLILSTED",
    "ERR_CME",
};

// esp_hf_subscriber_service_type_t
const char *c_subscriber_service_type_str[] = {
    "unknown",
    "voice",
    "fax",
};

// esp_hf_client_in_band_ring_state_t
const char *c_inband_ring_state_str[] = {
    "NOT provided",
    "Provided",
};

esp_err_t hfp_open_and_close_evt_cb_register(esp_hf_audio_open_t open_cb, esp_hf_audio_close_t close_cb)
{
    if ((open_cb == NULL)||(close_cb == NULL)) {
        return ESP_FAIL;
    }
    hfp_stream_user_callback.user_hfp_open_cb = open_cb;
    hfp_stream_user_callback.user_hfp_close_cb = close_cb;
    return ESP_OK;
}

static uint32_t bt_app_hf_client_outgoing_cb(uint8_t *p_buf, uint32_t sz)
{
    int out_len_bytes = 0;
    if (is_get_data) {
        out_len_bytes = audio_element_input(hfp_outgoing_stream, (char *)p_buf, sz);
    }

    if (out_len_bytes == sz) {
        is_get_data = false;
        return sz;
    } else {
        is_get_data = true;
        return 0;
    }
}

static void bt_app_hf_client_incoming_cb(const uint8_t *buf, uint32_t sz)
{
    if (hfp_incoming_stream) {
        if (audio_element_get_state(hfp_incoming_stream) == AEL_STATE_RUNNING) {
            audio_element_output(hfp_incoming_stream, (char *)buf, sz);
            esp_hf_client_outgoing_data_ready();
        }
    }
}
/* callback for HF_CLIENT */
void bt_hf_client_cb(esp_hf_client_cb_event_t event, esp_hf_client_cb_param_t *param)
{
    if (event <= ESP_HF_CLIENT_RING_IND_EVT) {
        ESP_LOGI(TAG, "APP HFP event: %s", c_hf_evt_str[event]);
    } else {
        ESP_LOGE(TAG, "APP HFP invalid event %d", event);
    }

    switch (event) {
    case ESP_HF_CLIENT_CONNECTION_STATE_EVT:
        ESP_LOGI(TAG, "--Connection state %s, peer feats 0x%x, chld_feats 0x%x",
                 c_connection_state_str[param->conn_stat.state],
                 param->conn_stat.peer_feat,
                 param->conn_stat.chld_feat);
        break;
    case ESP_HF_CLIENT_AUDIO_STATE_EVT:
        ESP_LOGI(TAG, "--Audio state %s",
                 c_audio_state_str[param->audio_stat.state]);
        if ((param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_CONNECTED)
            || (param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_CONNECTED_MSBC)) {
            if(hfp_stream_user_callback.user_hfp_open_cb != NULL) {
                if (param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_CONNECTED) {
                    hfp_stream_user_callback.user_hfp_open_cb(HF_DATA_CVSD);
                } else {
                    hfp_stream_user_callback.user_hfp_open_cb(HF_DATA_MSBC);
                }
            }
            esp_hf_client_register_data_callback(bt_app_hf_client_incoming_cb,
                                                 bt_app_hf_client_outgoing_cb);
        } else if (param->audio_stat.state == ESP_HF_CLIENT_AUDIO_STATE_DISCONNECTED) {
            if (hfp_stream_user_callback.user_hfp_close_cb != NULL) {
                hfp_stream_user_callback.user_hfp_close_cb();
            }
        }
        break;
    case ESP_HF_CLIENT_BVRA_EVT:
        ESP_LOGI(TAG, "--VR state %s",
                 c_vr_state_str[param->bvra.value]);
        break;
    case ESP_HF_CLIENT_CIND_SERVICE_AVAILABILITY_EVT:
        ESP_LOGI(TAG, "--NETWORK state %s",
                 c_service_availability_status_str[param->service_availability.status]);
        break;
    case ESP_HF_CLIENT_CIND_ROAMING_STATUS_EVT:
        ESP_LOGI(TAG, "--ROAMING: %s",
                 c_roaming_status_str[param->roaming.status]);
        break;
    case ESP_HF_CLIENT_CIND_SIGNAL_STRENGTH_EVT:
        ESP_LOGI(TAG, "--Signal strength: %d",
                 param->signal_strength.value);
        break;
    case ESP_HF_CLIENT_CIND_BATTERY_LEVEL_EVT:
        ESP_LOGI(TAG, "--Battery level %d",
                 param->battery_level.value);
        break;
    case ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT:
        ESP_LOGI(TAG, "--Operator name: %s",
                 param->cops.name);
        break;
    case ESP_HF_CLIENT_CIND_CALL_EVT:
        ESP_LOGI(TAG, "--Call indicator %s",
                 c_call_str[param->call.status]);
        break;
    case ESP_HF_CLIENT_CIND_CALL_SETUP_EVT:
        ESP_LOGI(TAG, "--Call setup indicator %s",
                 c_call_setup_str[param->call_setup.status]);
        break;
    case ESP_HF_CLIENT_CIND_CALL_HELD_EVT:
        ESP_LOGI(TAG, "--Call held indicator %s",
                 c_call_held_str[param->call_held.status]);
        break;
    case ESP_HF_CLIENT_BTRH_EVT:
        ESP_LOGI(TAG, "--Response and hold %s",
                 c_resp_and_hold_str[param->btrh.status]);
        break;
    case ESP_HF_CLIENT_CLIP_EVT:
        ESP_LOGI(TAG, "--Clip number %s",
                 (param->clip.number == NULL) ? "NULL" : (param->clip.number));
        break;
    case ESP_HF_CLIENT_CCWA_EVT:
        ESP_LOGI(TAG, "--Call_waiting %s",
                 (param->ccwa.number == NULL) ? "NULL" : (param->ccwa.number));
        break;
    case ESP_HF_CLIENT_CLCC_EVT:
        ESP_LOGI(TAG, "--Current call: idx %d, dir %s, state %s, mpty %s, number %s",
                 param->clcc.idx,
                 c_call_dir_str[param->clcc.dir],
                 c_call_state_str[param->clcc.status],
                 c_call_mpty_type_str[param->clcc.mpty],
                 (param->clcc.number == NULL) ? "NULL" : (param->clcc.number));
        break;
    case ESP_HF_CLIENT_VOLUME_CONTROL_EVT:
        ESP_LOGI(TAG, "--Volume_target: %s, volume %d",
                 c_volume_control_target_str[param->volume_control.type],
                 param->volume_control.volume);
        break;
    case ESP_HF_CLIENT_AT_RESPONSE_EVT:
        ESP_LOGI(TAG, "--AT response event, code %d, cme %d",
                 param->at_response.code, param->at_response.cme);
        break;
    case ESP_HF_CLIENT_CNUM_EVT:
        ESP_LOGI(TAG, "--Subscriber type %s, number %s",
                 c_subscriber_service_type_str[param->cnum.type],
                 (param->cnum.number == NULL) ? "NULL" : param->cnum.number);
        break;
    case ESP_HF_CLIENT_BSIR_EVT:
        ESP_LOGI(TAG, "--Inband ring state %s",
                 c_inband_ring_state_str[param->bsir.state]);
        break;
    case ESP_HF_CLIENT_BINP_EVT:
        ESP_LOGI(TAG, "--Last voice tag number: %s",
                 (param->binp.number == NULL) ? "NULL" : param->binp.number);
        break;
    default:
        ESP_LOGI(TAG, "HF_CLIENT EVT: %d", event);
        break;
    }
}

esp_err_t hfp_service_init()
{
    esp_hf_client_register_callback(bt_hf_client_cb);
    esp_hf_client_init();

    return ESP_OK;
}

static esp_err_t _hfp_stream_destroy(audio_element_handle_t self)
{
    hfp_stream_config_t *hfp = (hfp_stream_config_t *)audio_element_getdata(self);
    audio_free(hfp);
    return ESP_OK;
}

audio_element_handle_t hfp_stream_init(hfp_stream_config_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);
    audio_element_handle_t el = NULL;
    hfp_stream_config_t *hfp = audio_calloc(1, sizeof(hfp_stream_config_t));
    AUDIO_MEM_CHECK(TAG, hfp, return NULL);
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.task_stack = -1; // No need task
    cfg.destroy = _hfp_stream_destroy;
    hfp->type = config->type;
    if (config->type == INCOMING_STREAM) {
        ESP_LOGI(TAG, "incoming stream init");
        cfg.tag = "hfp";
        el = audio_element_init(&cfg);
        hfp_incoming_stream = el;
        AUDIO_MEM_CHECK(TAG, el, {
            audio_free(hfp);
            return NULL;
        });
        audio_element_setdata(el, hfp);
    } else if (config->type == OUTGOING_STREAM) {
        ESP_LOGI(TAG, "outgoing stream init");
        cfg.tag = "hfp_outgoing";
        el = audio_element_init(&cfg);
        hfp_outgoing_stream = el;
        AUDIO_MEM_CHECK(TAG, el, {
            audio_free(hfp);
            return NULL;
        });
        audio_element_setdata(el, hfp);
    } else {
        ESP_LOGE(TAG, "error stream type");
    }
    return el;
}
#endif
