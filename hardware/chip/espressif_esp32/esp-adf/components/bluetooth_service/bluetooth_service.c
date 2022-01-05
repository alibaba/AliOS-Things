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

#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "audio_mem.h"
#include "sdkconfig.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

#if CONFIG_BT_ENABLED
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

#include "bluetooth_service.h"
#include "bt_keycontrol.h"

static const char *TAG = "BLUETOOTH_SERVICE";

#define BT_SOURCE_DEFAULT_REMOTE_NAME "ESP-ADF-SPEAKER"

#define VALIDATE_BT(periph, ret)                                         \
    if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_BLUETOOTH)) { \
        ESP_LOGE(TAG, "Invalid Bluetooth periph, at line %d", __LINE__); \
        return ret;                                                      \
    }

/* A2DP source global state */
typedef enum {
    BT_SOURCE_STATE_IDLE,
    BT_SOURCE_STATE_DISCOVERING,
    BT_SOURCE_STATE_DISCOVERED,
    BT_SOURCE_STATE_UNCONNECTED,
    BT_SOURCE_STATE_CONNECTING,
    BT_SOURCE_STATE_CONNECTED,
    BT_SOURCE_STATE_DISCONNECTING,
} esp_a2d_source_state_t;

typedef uint8_t esp_peer_bdname_t[ESP_BT_GAP_MAX_BDNAME_LEN + 1];

typedef struct bluetooth_service {
    audio_element_handle_t stream;
    esp_periph_handle_t periph;
    audio_stream_type_t stream_type;
    esp_bd_addr_t remote_bda;
    esp_peer_bdname_t peer_bdname;
    esp_a2d_connection_state_t connection_state;
    esp_a2d_source_state_t source_a2d_state;
    esp_a2d_audio_state_t audio_state;
    uint64_t pos;
    uint8_t tl;
    bool avrc_connected;
    int a2dp_sample_rate;
} bluetooth_service_t;

bluetooth_service_t *g_bt_service = NULL;

static const char *conn_state_str[] = { "Disconnected", "Connecting", "Connected", "Disconnecting" };
static const char *audio_state_str[] = { "Suspended", "Stopped", "Started" };

static void bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *p_param)
{
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            a2d = (esp_a2d_cb_param_t *)(p_param);
            uint8_t *bda = a2d->conn_stat.remote_bda;
            ESP_LOGD(TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

            if (g_bt_service->connection_state == ESP_A2D_CONNECTION_STATE_DISCONNECTED
                && a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                memcpy(&g_bt_service->remote_bda, &a2d->conn_stat.remote_bda, sizeof(esp_bd_addr_t));
                g_bt_service->connection_state = a2d->conn_stat.state;
                ESP_LOGD(TAG, "A2DP connection state = CONNECTED");
                if (g_bt_service->periph) {
                    esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_CONNECTED, NULL, 0);
                }
            }
            if (memcmp(&g_bt_service->remote_bda, &a2d->conn_stat.remote_bda, sizeof(esp_bd_addr_t)) == 0
                && g_bt_service->connection_state == ESP_A2D_CONNECTION_STATE_CONNECTED
                && a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                memset(&g_bt_service->remote_bda, 0, sizeof(esp_bd_addr_t));
                g_bt_service->connection_state = ESP_A2D_CONNECTION_STATE_DISCONNECTED;
                ESP_LOGD(TAG, "A2DP connection state =  DISCONNECTED");
                if (g_bt_service->stream) {
                    audio_element_report_status(g_bt_service->stream, AEL_STATUS_INPUT_DONE);
                }
                if (g_bt_service->periph) {
                    esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_DISCONNECTED, NULL, 0);
                }
            }

            break;
        case ESP_A2D_AUDIO_STATE_EVT:
            a2d = (esp_a2d_cb_param_t *)(p_param);
            ESP_LOGD(TAG, "A2DP audio state: %s", audio_state_str[a2d->audio_stat.state]);
            g_bt_service->audio_state = a2d->audio_stat.state;
            if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
                g_bt_service->pos = 0;
            }
            if (g_bt_service->periph == NULL) {
                break;
            }

            if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
                esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_AUDIO_STARTED, NULL, 0);
            } else if (ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND == a2d->audio_stat.state) {
                esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_AUDIO_SUSPENDED, NULL, 0);
            } else if (ESP_A2D_AUDIO_STATE_STOPPED == a2d->audio_stat.state) {
                esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_AUDIO_STOPPED, NULL, 0);
            }

            break;
        case ESP_A2D_AUDIO_CFG_EVT:
            a2d = (esp_a2d_cb_param_t *)(p_param);
            ESP_LOGD(TAG, "A2DP audio stream configuration, codec type %d", a2d->audio_cfg.mcc.type);
            if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
                int sample_rate = 16000;
                char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
                if (oct0 & (0x01 << 6)) {
                    sample_rate = 32000;
                } else if (oct0 & (0x01 << 5)) {
                    sample_rate = 44100;
                } else if (oct0 & (0x01 << 4)) {
                    sample_rate = 48000;
                }
                g_bt_service->a2dp_sample_rate = sample_rate;
                ESP_LOGD(TAG, "Bluetooth configured, sample rate=%d", sample_rate);
                if (g_bt_service->stream == NULL) {
                    break;
                }

                audio_element_set_music_info(g_bt_service->stream, sample_rate, 2, 16);
                audio_element_report_info(g_bt_service->stream);
            }
            break;
        default:
            ESP_LOGD(TAG, "Unhandled A2DP event: %d", event);
            break;
    }
}

static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len)
{
    if (g_bt_service->stream) {
        if (audio_element_get_state(g_bt_service->stream) == AEL_STATE_RUNNING) {
            audio_element_output(g_bt_service->stream, (char *)data, len);
        }
    }
}

static int32_t bt_a2d_source_data_cb(uint8_t *data, int32_t len)
{
    if (g_bt_service->stream) {
        if (audio_element_get_state(g_bt_service->stream) == AEL_STATE_RUNNING) {
            if (len < 0 || data == NULL) {
                return 0;
            }

            len = audio_element_input(g_bt_service->stream, (char *)data, len);
            if (len == AEL_IO_DONE) {
                if (g_bt_service->periph) {
                    esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_AUDIO_SUSPENDED, NULL, 0);
                }
            }
            return len;
        }
    }
    return 0;
}

static char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

static bool get_name_from_eir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static void filter_inquiry_scan_result(esp_bt_gap_cb_param_t *param)
{
    char bda_str[18];
    uint32_t cod = 0;
    int32_t rssi = -129; /* invalid value */
    uint8_t *eir = NULL;
    esp_peer_bdname_t peer_bdname;
    esp_bt_gap_dev_prop_t *p;

    ESP_LOGI(TAG, "Scanned device: %s", bda2str(param->disc_res.bda, bda_str, 18));
    for (int i = 0; i < param->disc_res.num_prop; i++) {
        p = param->disc_res.prop + i;
        switch (p->type) {
            case ESP_BT_GAP_DEV_PROP_COD:
                cod = *(uint32_t *)(p->val);
                ESP_LOGI(TAG, "--Class of Device: 0x%x", cod);
                break;
            case ESP_BT_GAP_DEV_PROP_RSSI:
                rssi = *(int8_t *)(p->val);
                ESP_LOGI(TAG, "--RSSI: %d", rssi);
                break;
            case ESP_BT_GAP_DEV_PROP_EIR:
                eir = (uint8_t *)(p->val);
                get_name_from_eir(eir, (uint8_t *)&peer_bdname, NULL);
                ESP_LOGI(TAG, "--Name: %s", peer_bdname);
                break;
            case ESP_BT_GAP_DEV_PROP_BDNAME:
            default:
                break;
        }
    }

    /* search for device with MAJOR service class as "rendering" in COD */
    if (!esp_bt_gap_is_valid_cod(cod) || !(esp_bt_gap_get_cod_srvc(cod) & ESP_BT_COD_SRVC_RENDERING)) {
        return;
    }

    /* search for device named "peer_bdname" in its extended inquiry response */
    if (eir) {
        get_name_from_eir(eir, (uint8_t *)&peer_bdname, NULL);
        if (strcmp((char *)peer_bdname, (char *)&g_bt_service->peer_bdname) != 0) {
            return;
        }

        ESP_LOGI(TAG, "Found a target device, address %s, name %s", bda_str, (uint8_t *)peer_bdname);
        g_bt_service->source_a2d_state = BT_SOURCE_STATE_DISCOVERED;
        memcpy(&g_bt_service->remote_bda, param->disc_res.bda, ESP_BD_ADDR_LEN);
        ESP_LOGI(TAG, "Cancel device discovery ...");
        esp_bt_gap_cancel_discovery();
    }
}

static void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *p_param)
{
    esp_avrc_ct_cb_param_t *rc = p_param;
    switch (event) {
        case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
                uint8_t *bda = rc->conn_stat.remote_bda;
                g_bt_service->avrc_connected = rc->conn_stat.connected;
                if (rc->conn_stat.connected) {
                    ESP_LOGD(TAG, "ESP_AVRC_CT_CONNECTION_STATE_EVT");
                    bt_key_act_sm_init();
                } else if (0 == rc->conn_stat.connected) {
                    bt_key_act_sm_deinit();
                }

                ESP_LOGD(TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                         rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
                break;
            }
        case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
                if (g_bt_service->avrc_connected) {
                    ESP_LOGD(TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
                    bt_key_act_param_t param;
                    memset(&param, 0, sizeof(bt_key_act_param_t));
                    param.evt = event;
                    param.tl = rc->psth_rsp.tl;
                    param.key_code = rc->psth_rsp.key_code;
                    param.key_state = rc->psth_rsp.key_state;
                    bt_key_act_state_machine(&param);
                }
                break;
            }
        case ESP_AVRC_CT_METADATA_RSP_EVT: {
                ESP_LOGD(TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
                // free(rc->meta_rsp.attr_text);
                break;
            }
        case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
                ESP_LOGD(TAG, "AVRC event notification: %d", rc->change_ntf.event_id);
                break;
            }
        case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
                ESP_LOGD(TAG, "AVRC remote features %x", rc->rmt_feats.feat_mask);
                break;
            }

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
        case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
                ESP_LOGD(TAG, "remote rn_cap: count %d, bitmask 0x%x", rc->get_rn_caps_rsp.cap_count,
                         rc->get_rn_caps_rsp.evt_set.bits);
                break;
            }
#endif

        default:
            ESP_LOGE(TAG, "%s unhandled evt %d", __func__, event);
            break;
    }
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_BT_GAP_DISC_RES_EVT: {
                filter_inquiry_scan_result(param);
                break;
            }
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
                if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                    if (g_bt_service->source_a2d_state == BT_SOURCE_STATE_DISCOVERED) {
                        g_bt_service->source_a2d_state = BT_SOURCE_STATE_CONNECTING;
                        ESP_LOGI(TAG, "Device discovery stopped.");
                        ESP_LOGI(TAG, "a2dp connecting to peer: %s", g_bt_service->peer_bdname);
                        esp_a2d_source_connect(g_bt_service->remote_bda);
                    } else if (g_bt_service->source_a2d_state != BT_SOURCE_STATE_IDLE) {
                        // not discovered, continue to discover
                        ESP_LOGI(TAG, "Device discovery failed, continue to discover...");
                        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
                    }
                } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
                    ESP_LOGI(TAG, "Discovery started.");
                }
                break;
            }
        case ESP_BT_GAP_RMT_SRVCS_EVT:
        case ESP_BT_GAP_RMT_SRVC_REC_EVT:
            break;
        case ESP_BT_GAP_AUTH_CMPL_EVT: {
                if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                    ESP_LOGI(TAG, "authentication success: %s", param->auth_cmpl.device_name);
                    esp_log_buffer_hex(TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
                } else {
                    ESP_LOGI(TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
                }
                break;
            }
        case ESP_BT_GAP_PIN_REQ_EVT: {
                ESP_LOGI(TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
                if (param->pin_req.min_16_digit) {
                    ESP_LOGI(TAG, "Input pin code: 0000 0000 0000 0000");
                    esp_bt_pin_code_t pin_code = { 0 };
                    esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
                } else {
                    ESP_LOGI(TAG, "Input pin code: 1234");
                    esp_bt_pin_code_t pin_code;
                    pin_code[0] = '1';
                    pin_code[1] = '2';
                    pin_code[2] = '3';
                    pin_code[3] = '4';
                    esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
                }
                break;
            }
        default: {
                ESP_LOGI(TAG, "event: %d", event);
                break;
            }
    }
    return;
}
static void bt_a2d_source_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    ESP_LOGI(TAG, "%s state %d, evt 0x%x", __func__, g_bt_service->source_a2d_state, event);
    switch (g_bt_service->source_a2d_state) {
        case BT_SOURCE_STATE_DISCOVERING:
        case BT_SOURCE_STATE_DISCOVERED:
            break;
        case BT_SOURCE_STATE_UNCONNECTED:
            break;
        case BT_SOURCE_STATE_CONNECTING:
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "a2dp connected");
                g_bt_service->source_a2d_state = BT_SOURCE_STATE_CONNECTED;
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
                esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
#else
                esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_NONE);
#endif
                if (g_bt_service->periph) {
                    esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_CONNECTED, NULL, 0);
                }
                ESP_LOGI(TAG, "a2dp media ready checking ...");
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                g_bt_service->source_a2d_state = BT_SOURCE_STATE_UNCONNECTED;
            }
            break;
        case BT_SOURCE_STATE_CONNECTED:
            if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
                if (param->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY && param->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
                    ESP_LOGI(TAG, "a2dp media ready, starting ...");
                    esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
                } else if (param->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_START && param->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
                    ESP_LOGI(TAG, "a2dp media start successfully.");
                    g_bt_service->audio_state = ESP_A2D_AUDIO_STATE_STARTED;
                } else if (param->media_ctrl_stat.cmd != ESP_A2D_MEDIA_CTRL_SUSPEND) {
                    // not started successfully, transfer to idle state
                    ESP_LOGI(TAG, "a2dp media start failed.");
                    g_bt_service->audio_state = BT_SOURCE_STATE_IDLE;
                }
            } else if (event == ESP_A2D_CONNECTION_STATE_EVT && param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                g_bt_service->source_a2d_state = BT_SOURCE_STATE_UNCONNECTED;
                if (g_bt_service->periph) {
                    esp_periph_send_event(g_bt_service->periph, PERIPH_BLUETOOTH_DISCONNECTED, NULL, 0);
                }
            }
            break;
        case BT_SOURCE_STATE_DISCONNECTING:
            break;
        default:
            ESP_LOGE(TAG, "%s invalid state %d", __func__, g_bt_service->source_a2d_state);
            break;
    }
}

esp_err_t bluetooth_service_start(bluetooth_service_cfg_t *config)
{
    if (g_bt_service) {
        ESP_LOGE(TAG, "Bluetooth service have been initialized");
        return ESP_FAIL;
    }

    g_bt_service = audio_calloc(1, sizeof(bluetooth_service_t));
    AUDIO_MEM_CHECK(TAG, g_bt_service, return ESP_ERR_NO_MEM);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        AUDIO_ERROR(TAG, "initialize controller failed");
        return ESP_FAIL;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_BTDM) != ESP_OK) {
        AUDIO_ERROR(TAG, "enable controller failed");
        return ESP_FAIL;
    }

    if (esp_bluedroid_init() != ESP_OK) {
        AUDIO_ERROR(TAG, "initialize bluedroid failed");
        return ESP_FAIL;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
        AUDIO_ERROR(TAG, "enable bluedroid failed");
        return ESP_FAIL;
    }

    if (config->device_name) {
        esp_bt_dev_set_device_name(config->device_name);
    } else {
        if (config->mode == BLUETOOTH_A2DP_SINK) {
            esp_bt_dev_set_device_name("ESP-ADF-SPEAKER");
        } else {
            esp_bt_dev_set_device_name("ESP-ADF-SOURCE");
        }
    }

    esp_avrc_ct_init();
    esp_avrc_ct_register_callback(bt_avrc_ct_cb);

    if (config->mode == BLUETOOTH_A2DP_SINK) {
        esp_a2d_sink_init();
        esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
        esp_a2d_register_callback(bt_a2d_sink_cb);
        // TODO: Use this function for IDF version higher than v3.0
        // esp_a2d_sink_register_data_callback(bt_a2d_data_cb);
        g_bt_service->stream_type = AUDIO_STREAM_READER;
    } else {
        /*
        * Set default parameters for Legacy Pairing
        * Use variable pin, input pin code when pairing
        */
        esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
        esp_bt_pin_code_t pin_code;
        esp_bt_gap_set_pin(pin_type, 0, pin_code);
        esp_a2d_register_callback(bt_a2d_source_cb);
        esp_bt_gap_register_callback(bt_app_gap_cb);
        esp_a2d_source_register_data_callback(bt_a2d_source_data_cb);
        esp_a2d_source_init();

        /* start device discovery */
        ESP_LOGI(TAG, "Starting device discovery...");
        if (config->remote_name) {
            memcpy(&g_bt_service->peer_bdname, config->remote_name, strlen(config->remote_name) + 1);
        } else {
            memcpy(&g_bt_service->peer_bdname, BT_SOURCE_DEFAULT_REMOTE_NAME, strlen(BT_SOURCE_DEFAULT_REMOTE_NAME) + 1);
        }

        g_bt_service->source_a2d_state = BT_SOURCE_STATE_DISCOVERING;
        g_bt_service->stream_type = AUDIO_STREAM_WRITER;
        esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
    }

    /* set discoverable and connectable mode */
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
#else
    esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
#endif

    g_bt_service->a2dp_sample_rate = 44100;
    return ESP_OK;
}

esp_err_t bluetooth_service_destroy()
{
    if (g_bt_service && (g_bt_service->stream || g_bt_service->periph)) {

        AUDIO_ERROR(TAG, "Stream and periph need to stop first");
        return ESP_FAIL;
    }
    if (g_bt_service) {
        esp_avrc_ct_deinit();
        if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
            esp_a2d_sink_deinit();
        } else {
            esp_a2d_source_deinit();
        }
        esp_bluedroid_disable();
        esp_bluedroid_deinit();
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
        esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
        audio_free(g_bt_service);
        g_bt_service = NULL;
    }
    return ESP_OK;
}

static esp_err_t _bt_stream_destroy(audio_element_handle_t self)
{
    g_bt_service->stream = NULL;
    return ESP_OK;
}

audio_element_handle_t bluetooth_service_create_stream()
{
    if (g_bt_service && g_bt_service->stream) {
        ESP_LOGE(TAG, "Bluetooth stream have been created");
        return NULL;
    }

    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();
    cfg.task_stack = -1; // No need task
    cfg.destroy = _bt_stream_destroy;
    cfg.tag = "bt";
    g_bt_service->stream = audio_element_init(&cfg);

    AUDIO_MEM_CHECK(TAG, g_bt_service->stream, return NULL);

    audio_element_setdata(g_bt_service->stream, g_bt_service);

    return g_bt_service->stream;
}

static esp_err_t _bt_periph_init(esp_periph_handle_t periph)
{
    return ESP_OK;
}

static esp_err_t _bt_periph_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    return ESP_OK;
}

static esp_err_t _bt_periph_destroy(esp_periph_handle_t periph)
{
    g_bt_service->periph = NULL;
    return ESP_OK;
}

esp_periph_handle_t bluetooth_service_create_periph()
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return NULL);
    if (g_bt_service && g_bt_service->periph) {
        ESP_LOGE(TAG, "Bluetooth periph have been created");
        return NULL;
    }

    g_bt_service->periph = esp_periph_create(PERIPH_ID_BLUETOOTH, "periph_bt");
    esp_periph_set_function(g_bt_service->periph, _bt_periph_init, _bt_periph_run, _bt_periph_destroy);
    return g_bt_service->periph;
}

static esp_err_t periph_bluetooth_passthrough_cmd(esp_periph_handle_t periph, uint8_t cmd)
{
    VALIDATE_BT(periph, ESP_FAIL);
    if (g_bt_service->audio_state != ESP_A2D_AUDIO_STATE_STARTED) {
        //return ESP_FAIL;
    }
    esp_err_t err = ESP_OK;

    if (g_bt_service->avrc_connected) {
        bt_key_act_param_t param;
        memset(&param, 0, sizeof(bt_key_act_param_t));
        param.evt = ESP_AVRC_CT_KEY_STATE_CHG_EVT;
        param.key_code = cmd;
        param.key_state = 0;
        param.tl = (g_bt_service->tl) & 0x0F;
        g_bt_service->tl = (g_bt_service->tl + 2) & 0x0f;
        bt_key_act_state_machine(&param);
    }

    return err;
}

esp_err_t periph_bluetooth_play(esp_periph_handle_t periph)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    esp_err_t err = ESP_OK;
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        err = periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_PLAY);
    } else {
        err = esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
    }
    return err;
}

esp_err_t periph_bluetooth_pause(esp_periph_handle_t periph)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    esp_err_t err = ESP_OK;
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        err = periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_PAUSE);
    } else {
        err = esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_SUSPEND);
    }
    return err;
}

esp_err_t periph_bluetooth_stop(esp_periph_handle_t periph)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    esp_err_t err = ESP_OK;
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        err = periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_STOP);
    } else {
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
        g_bt_service->audio_state = ESP_A2D_AUDIO_STATE_STOPPED;
    }
    return err;
}

esp_err_t periph_bluetooth_next(esp_periph_handle_t periph)
{
    return periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_FORWARD);
}

esp_err_t periph_bluetooth_prev(esp_periph_handle_t periph)
{
    return periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_BACKWARD);
}

esp_err_t periph_bluetooth_rewind(esp_periph_handle_t periph)
{
    return periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_REWIND);
}

esp_err_t periph_bluetooth_fast_forward(esp_periph_handle_t periph)
{
    return periph_bluetooth_passthrough_cmd(periph, ESP_AVRC_PT_CMD_FAST_FORWARD);
}

esp_err_t periph_bluetooth_discover(esp_periph_handle_t periph)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        return esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
    }
    return ESP_OK;
}

esp_err_t periph_bluetooth_cancel_discover(esp_periph_handle_t periph)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        g_bt_service->source_a2d_state = BT_SOURCE_STATE_IDLE;
        return esp_bt_gap_cancel_discovery();
    }
    return ESP_OK;
}

esp_err_t periph_bluetooth_connect(esp_periph_handle_t periph, bluetooth_addr_t remote_bda)
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return ESP_FAIL);
    if (g_bt_service->stream_type == AUDIO_STREAM_READER) {
        g_bt_service->source_a2d_state = BT_SOURCE_STATE_DISCOVERED;
        memcpy(&g_bt_service->remote_bda, remote_bda, BLUETOOTH_ADDR_LEN);
        ESP_LOGI(TAG, "Cancel device discovery and connect remote device.");
        return esp_bt_gap_cancel_discovery();
    }
    return ESP_OK;
}

int periph_bluetooth_get_a2dp_sample_rate()
{
    AUDIO_NULL_CHECK(TAG, g_bt_service, return -1);
    return g_bt_service->a2dp_sample_rate;
}

#endif
