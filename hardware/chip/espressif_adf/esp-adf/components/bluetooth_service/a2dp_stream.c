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

#include <stdlib.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_peripherals.h"
#include "audio_common.h"
#include "audio_mem.h"
#include "audio_thread.h"
#include "sdkconfig.h"

#include "a2dp_stream.h"

#if CONFIG_BT_ENABLED

static const char *TAG = "A2DP_STREAM";

typedef struct {
    audio_element_handle_t sink_stream;
    audio_element_handle_t source_stream;
    a2dp_stream_user_callback_t user_callback;
    esp_periph_handle_t bt_avrc_periph;
    bool avrcp_conn_state;
    audio_stream_type_t stream_type;
    uint8_t trans_label;

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    audio_hal_handle_t audio_hal;
    bool avrcp_conn_tg_state;
    _lock_t volume_lock;
    int16_t volume;
    bool volume_notify;
#endif

    audio_thread_t a2dp_thread;
    QueueHandle_t a2dp_queue;
} aadp_info_t;

typedef struct {
    enum {
        A2DP_TYPE_SINK ,
        A2DP_TYPE_DESTORY,
    } type;
    void *data;
    size_t size;
} a2dp_data_t;

static aadp_info_t s_aadp_handler = { 0 };

int16_t default_volume = 50;
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void bt_avrc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param);
#endif

static const char *audio_state_str[] = { "Suspended", "Stopped", "Started" };
static void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

static void audio_a2dp_stream_thread(void *pvParameters)
{
    (void) pvParameters;

    a2dp_data_t recv_msg;
    bool _aadp_task_run = true;
    while(_aadp_task_run) {
        if ( pdTRUE ==  xQueueReceive (s_aadp_handler.a2dp_queue, &recv_msg, portMAX_DELAY) ) {
            switch (recv_msg.type) {
            case A2DP_TYPE_SINK:
                    audio_element_output(s_aadp_handler.sink_stream, (char *)recv_msg.data, recv_msg.size);
                    audio_free(recv_msg.data);
                    recv_msg.data = NULL;
                break;
            case A2DP_TYPE_DESTORY:
                _aadp_task_run = false;
                break;
            default:
                break;
            }
        }
    }
    ESP_LOGI(TAG, "Delete the audio_a2dp_stream_thread");
    vTaskDelete(NULL);
}

static void bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    if (s_aadp_handler.user_callback.user_a2d_cb) {
        s_aadp_handler.user_callback.user_a2d_cb(event, param);
    }
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            a2d = (esp_a2d_cb_param_t *)(param);
            uint8_t *bda = a2d->conn_stat.remote_bda;
            ESP_LOGI(TAG, "A2DP bd address:, [%02x:%02x:%02x:%02x:%02x:%02x]",
                     bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                ESP_LOGI(TAG, "A2DP connection state =  DISCONNECTED");
                if (s_aadp_handler.sink_stream) {
                    audio_element_report_status(s_aadp_handler.sink_stream, AEL_STATUS_INPUT_DONE);
                }
                if (s_aadp_handler.bt_avrc_periph) {
                    esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_DISCONNECTED, NULL, 0);
                }
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "A2DP connection state =  CONNECTED");
                if (s_aadp_handler.bt_avrc_periph) {
                    esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_CONNECTED, NULL, 0);
                }
            }
            break;

        case ESP_A2D_AUDIO_STATE_EVT:
            a2d = (esp_a2d_cb_param_t *)(param);
            ESP_LOGD(TAG, "A2DP audio state: %s", audio_state_str[a2d->audio_stat.state]);
            if (s_aadp_handler.bt_avrc_periph == NULL) {
                break;
            }

            if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
                esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_AUDIO_STARTED, NULL, 0);
            } else if (ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND == a2d->audio_stat.state) {
                esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_AUDIO_SUSPENDED, NULL, 0);
            } else if (ESP_A2D_AUDIO_STATE_STOPPED == a2d->audio_stat.state) {
                esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_AUDIO_STOPPED, NULL, 0);
            }
            break;

        case ESP_A2D_AUDIO_CFG_EVT:
            ESP_LOGI(TAG, "A2DP audio stream configuration, codec type %d", param->audio_cfg.mcc.type);
            if (param->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
                int sample_rate = 16000;
                char oct0 = param->audio_cfg.mcc.cie.sbc[0];
                if (oct0 & (0x01 << 6)) {
                    sample_rate = 32000;
                } else if (oct0 & (0x01 << 5)) {
                    sample_rate = 44100;
                } else if (oct0 & (0x01 << 4)) {
                    sample_rate = 48000;
                }
                ESP_LOGI(TAG, "Bluetooth configured, sample rate=%d", sample_rate);
                if (s_aadp_handler.sink_stream == NULL) {
                    break;
                }
                audio_element_set_music_info(s_aadp_handler.sink_stream, sample_rate, 2, 16);
                audio_element_report_info(s_aadp_handler.sink_stream);
            }
            break;
        default:
            ESP_LOGI(TAG, "Unhandled A2DP event: %d", event);
            break;
    }
}

static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len)
{
    if (s_aadp_handler.user_callback.user_a2d_sink_data_cb) {
        s_aadp_handler.user_callback.user_a2d_sink_data_cb(data, len);
    }
    if (s_aadp_handler.sink_stream) {
        if (audio_element_get_state(s_aadp_handler.sink_stream) == AEL_STATE_RUNNING) {
            a2dp_data_t send_msg = {0};
            send_msg.data = (uint8_t *) audio_calloc(1, len);
            if (!send_msg.data) {
                ESP_LOGE(TAG, "No ineffecitive memory to allocate(%d)", __LINE__);
                return;
            }
            memcpy(send_msg.data, data, len);
            send_msg.size = len;
            send_msg.type = A2DP_TYPE_SINK;
            if ((!s_aadp_handler.a2dp_queue) || xQueueSend( s_aadp_handler.a2dp_queue, &send_msg, 0)  != pdPASS ) {
                ESP_LOGW(TAG, "discard a2dp(%p) sink pkt, A2DP_STREAM_QUEUE_SIZE value needs to be expanded", s_aadp_handler.a2dp_queue);
                audio_free(send_msg.data);
                send_msg.data = NULL;
            } 
        }
    }
}

static void bt_a2d_source_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    if (s_aadp_handler.user_callback.user_a2d_cb) {
        s_aadp_handler.user_callback.user_a2d_cb(event, param);
    }
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "a2dp source connected");
                ESP_LOGI(TAG, "a2dp media ready checking ...");
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
                if (s_aadp_handler.bt_avrc_periph) {
                    esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_CONNECTED, NULL, 0);
                }
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                ESP_LOGI(TAG, "a2dp source disconnected");
                esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
                if (s_aadp_handler.bt_avrc_periph) {
                    esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_DISCONNECTED, NULL, 0);
                }
            }
            break;
        case ESP_A2D_MEDIA_CTRL_ACK_EVT:
            if (param->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY &&
                param->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
            }
            break;
        default:
            break;
    }
}

static int32_t bt_a2d_source_data_cb(uint8_t *data, int32_t len)
{
    if (s_aadp_handler.source_stream) {
        if (audio_element_get_state(s_aadp_handler.source_stream) == AEL_STATE_RUNNING) {
            if (len < 0 || data == NULL) {
                return 0;
            }
            len = audio_element_input(s_aadp_handler.source_stream, (char *)data, len);
            if (len == AEL_IO_DONE) {
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
                if (s_aadp_handler.bt_avrc_periph) {
                    esp_periph_send_event(s_aadp_handler.bt_avrc_periph, PERIPH_BLUETOOTH_AUDIO_SUSPENDED, NULL, 0);
                }
            }
            return len;
        }
    }
    return 0;
}

static esp_err_t a2dp_sink_destory(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "a2dp_sink_destory");

    a2dp_data_t destory_msg = {
        .type = A2DP_TYPE_DESTORY,
    };
    if ( xQueueSend( s_aadp_handler.a2dp_queue, &destory_msg, 0 )  != pdPASS ) {
        ESP_LOGE(TAG, "Destory audio_a2dp_stream_thread failed");
        return ESP_FAIL;
    }
    s_aadp_handler.sink_stream = NULL;
    vQueueDelete(s_aadp_handler.a2dp_queue);
    s_aadp_handler.a2dp_queue = NULL;
    memset(&s_aadp_handler.user_callback, 0, sizeof(a2dp_stream_user_callback_t));
    return ESP_OK;
}

static esp_err_t a2dp_source_destory(audio_element_handle_t self)
{
    s_aadp_handler.source_stream = NULL;
    memset(&s_aadp_handler.user_callback, 0, sizeof(a2dp_stream_user_callback_t));
    return ESP_OK;
}

audio_element_handle_t a2dp_stream_init(a2dp_stream_config_t *config)
{
    audio_element_handle_t el = NULL;
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    if (s_aadp_handler.sink_stream || s_aadp_handler.source_stream) {
        ESP_LOGE(TAG, "a2dp stream already created. please terminate before create.");
        return NULL;
    }

    cfg.task_stack = -1; // No need task
    cfg.tag = "aadp";    

    esp_avrc_ct_init();
    esp_avrc_ct_register_callback(bt_avrc_ct_cb);

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    if (s_aadp_handler.audio_hal == NULL) {
        s_aadp_handler.audio_hal = config->audio_hal;
    }
    esp_avrc_tg_init();
    esp_avrc_tg_register_callback(bt_avrc_tg_cb);
    esp_avrc_rn_evt_cap_mask_t evt_set = {0};
    esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set, ESP_AVRC_RN_VOLUME_CHANGE);
    esp_avrc_tg_set_rn_evt_cap(&evt_set);
#endif

    if (config->type == AUDIO_STREAM_READER) {
        // A2DP sink
        s_aadp_handler.stream_type = AUDIO_STREAM_READER;
        cfg.destroy = a2dp_sink_destory;
        el = s_aadp_handler.sink_stream = audio_element_init(&cfg);

        esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
        esp_a2d_register_callback(bt_a2d_sink_cb);
        esp_a2d_sink_init();
    } else {
        // A2DP source
        s_aadp_handler.stream_type = AUDIO_STREAM_WRITER;
        cfg.destroy = a2dp_source_destory;
        el = s_aadp_handler.source_stream = audio_element_init(&cfg);

        esp_a2d_register_callback(bt_a2d_source_cb);
        esp_a2d_source_register_data_callback(bt_a2d_source_data_cb);
        esp_a2d_source_init();
    }

    AUDIO_MEM_CHECK(TAG, el, return NULL);
    
    memcpy(&s_aadp_handler.user_callback, &config->user_callback, sizeof(a2dp_stream_user_callback_t));

    if ( config->type == AUDIO_STREAM_READER ) {
        s_aadp_handler.a2dp_queue = xQueueCreate(A2DP_STREAM_QUEUE_SIZE, sizeof(a2dp_data_t));
        if (s_aadp_handler.a2dp_queue == 0) {
            ESP_LOGE(TAG, "Create a2dp queue failed(%d)", __LINE__);
            return NULL;
        }
        esp_err_t err = audio_thread_create(&s_aadp_handler.a2dp_thread, "audio_a2dp_stream_thread", audio_a2dp_stream_thread, NULL,
                                A2DP_STREAM_TASK_STACK, A2DP_STREAM_TASK_PRIO, A2DP_STREAM_TASK_IN_EXT, A2DP_STREAM_TASK_CORE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Create audio_a2dp_stream_thread failed(%d)", __LINE__);
            return NULL;
        }
    }
    return el;
}

esp_err_t a2dp_destroy()
{
    if (s_aadp_handler.stream_type == AUDIO_STREAM_READER) {
        esp_a2d_sink_deinit();
    } else if (s_aadp_handler.stream_type == AUDIO_STREAM_WRITER) {
        esp_a2d_source_deinit();
    }
    return ESP_OK;
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void bt_avrc_volume_set_by_controller(int16_t volume)
{
    ESP_LOGI(TAG, "Volume is set by remote controller %d%%\n", (uint32_t)volume * 100 / 0x7f);
    _lock_acquire(&s_aadp_handler.volume_lock);
    s_aadp_handler.volume = volume;
    _lock_release(&s_aadp_handler.volume_lock);
    if (s_aadp_handler.audio_hal) {
        audio_hal_set_volume(s_aadp_handler.audio_hal, s_aadp_handler.volume);
    }
}

static void bt_avrc_volume_set_by_local(int16_t volume)
{
    ESP_LOGI(TAG, "Volume is set locally to: %d%%", volume );
    _lock_acquire(&s_aadp_handler.volume_lock);
    s_aadp_handler.volume = volume;
    _lock_release(&s_aadp_handler.volume_lock);

    esp_avrc_rn_param_t rn_param;
    rn_param.volume = default_volume;
    esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_CHANGED, &rn_param);
    if (s_aadp_handler.audio_hal) {
        audio_hal_set_volume(s_aadp_handler.audio_hal, s_aadp_handler.volume);
    }
    s_aadp_handler.volume_notify = false;

}
#endif

static void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *p_param)
{
    esp_avrc_ct_cb_param_t *rc = p_param;
    switch (event) {
        case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
                uint8_t *bda = rc->conn_stat.remote_bda;
                s_aadp_handler.avrcp_conn_state = rc->conn_stat.connected;
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
                if (s_aadp_handler.avrcp_conn_state) {
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
                // ESP_LOGD(TAG, "AVRC event notification: %u, param: %u", rc->change_ntf.event_id, rc->change_ntf.event_parameter);
                break;
            }
        case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
                ESP_LOGW(TAG, "AVRC ct remote features %x", rc->rmt_feats.feat_mask);
                break;
            }
        default:
            ESP_LOGE(TAG, "%s unhandled evt %d", __func__, event);
            break;
    }
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void bt_avrc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param)
{
    ESP_LOGD(TAG, "%s evt %d", __func__, event);
    esp_avrc_tg_cb_param_t *rc = (esp_avrc_tg_cb_param_t *)(param);
    switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        s_aadp_handler.avrcp_conn_tg_state = rc->conn_stat.connected;
        break;
    }
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT: {
        ESP_LOGI(TAG, "AVRC passthrough cmd: key_code 0x%x, key_state %d", rc->psth_cmd.key_code, rc->psth_cmd.key_state);
        break;
    }
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
        ESP_LOGI(TAG, "AVRC set absolute volume: %d%%", (int)rc->set_abs_vol.volume * 100/ 0x7f);
        bt_avrc_volume_set_by_controller(rc->set_abs_vol.volume);
        default_volume = rc->set_abs_vol.volume;
        break;
    }
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
        ESP_LOGI(TAG, "AVRC register event notification: %d, param: 0x%x", rc->reg_ntf.event_id, rc->reg_ntf.event_parameter);
        if (rc->reg_ntf.event_id == ESP_AVRC_RN_VOLUME_CHANGE) {
            s_aadp_handler.volume_notify = true;
            esp_avrc_rn_param_t rn_param;
            rn_param.volume = default_volume;
            ESP_LOGI(TAG, "rn_param.volume:%d", rn_param.volume);
            esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_INTERIM, &rn_param);
        }
        break;
    }
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT: {
        ESP_LOGW(TAG, "AVRC tg remote features %x, CT features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.ct_feat_flag);
        break;
    }
    default:
        ESP_LOGE(TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
#endif

static esp_err_t _bt_avrc_periph_init(esp_periph_handle_t periph)
{
    return ESP_OK;
}

static esp_err_t _bt_avrc_periph_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    return ESP_OK;
}

static esp_err_t _bt_avrc_periph_destroy(esp_periph_handle_t periph)
{
    esp_avrc_ct_deinit();
    s_aadp_handler.bt_avrc_periph = NULL;
    return ESP_OK;
}

esp_periph_handle_t bt_create_periph()
{
    if (s_aadp_handler.bt_avrc_periph) {
        ESP_LOGE(TAG, "bt periph have been created");
        return NULL;
    }
    s_aadp_handler.bt_avrc_periph = esp_periph_create(PERIPH_ID_BLUETOOTH, "periph_bt");
    esp_periph_set_function(s_aadp_handler.bt_avrc_periph, _bt_avrc_periph_init, _bt_avrc_periph_run, _bt_avrc_periph_destroy);
    return s_aadp_handler.bt_avrc_periph;
}

static esp_err_t periph_bt_avrc_passthrough_cmd(esp_periph_handle_t periph, uint8_t cmd)
{
    esp_err_t err = ESP_OK;

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    if(s_aadp_handler.avrcp_conn_tg_state) {
        if (cmd == ESP_AVRC_PT_CMD_VOL_DOWN) {
            int16_t volume = (default_volume - 5) < 0 ? 0 : (default_volume - 5);
            if(volume <= 0){
                volume = 0;
            }
            bt_avrc_volume_set_by_local(volume);
            default_volume = volume;
            return err;
        } else if (cmd == ESP_AVRC_PT_CMD_VOL_UP) {
            int16_t volume = (default_volume + 5) > 0x7f ? 0x7f : (default_volume + 5);
            if(volume >= 100){
                volume = 100;
            }
            bt_avrc_volume_set_by_local(volume);
            default_volume = volume;
            return err;
        }
    }
#endif
    if (s_aadp_handler.avrcp_conn_state) {
        bt_key_act_param_t param;
        memset(&param, 0, sizeof(bt_key_act_param_t));
        param.evt = ESP_AVRC_CT_KEY_STATE_CHG_EVT;
        param.key_code = cmd;
        param.key_state = 0;
        param.tl = s_aadp_handler.trans_label & 0x0F;
        s_aadp_handler.trans_label = (s_aadp_handler.trans_label + 2) & 0x0f;
        bt_key_act_state_machine(&param);
    }
    return err;
}

esp_err_t periph_bt_play(esp_periph_handle_t periph)
{
    esp_err_t err = ESP_OK;
    if (s_aadp_handler.stream_type == AUDIO_STREAM_READER) {
        err = periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_PLAY);
    } else {
        err = esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
    }
    return err;
}

esp_err_t periph_bt_pause(esp_periph_handle_t periph)
{
    esp_err_t err = ESP_OK;
    if (s_aadp_handler.stream_type == AUDIO_STREAM_READER) {
        err = periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_PAUSE);
    } else {
        err = esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_SUSPEND);
    }
    return err;
}

esp_err_t periph_bt_stop(esp_periph_handle_t periph)
{
    esp_err_t err = ESP_OK;
    if (s_aadp_handler.stream_type == AUDIO_STREAM_READER) {
        err = periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_STOP);
    } else {
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);
    }
    return err;
}

esp_err_t periph_bt_avrc_next(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_FORWARD);
}

esp_err_t periph_bt_avrc_prev(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_BACKWARD);
}

esp_err_t periph_bt_avrc_rewind(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_REWIND);
}

esp_err_t periph_bt_avrc_fast_forward(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_FAST_FORWARD);
}

esp_err_t periph_bt_discover(esp_periph_handle_t periph)
{
    if (s_aadp_handler.stream_type == AUDIO_STREAM_READER) {
        return esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
    }
    return ESP_OK;
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
esp_err_t periph_bt_volume_up(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_VOL_UP);
}

esp_err_t periph_bt_volume_down(esp_periph_handle_t periph)
{
    return periph_bt_avrc_passthrough_cmd(periph, ESP_AVRC_PT_CMD_VOL_DOWN);
}
#endif

#endif
