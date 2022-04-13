/**
 * @file aiot_diag_api.c
 * @brief diag模块的API接口实现, 提供诊断SDK的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "diag_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "core_mqtt.h"
#include "core_diag.h"

static void _diag_mqtt_conn_hb_extra_clean(void *handle, void *extra_data);
static int32_t _diag_mqtt_conn_hb_extra_stop(void *handle, diag_running_state_node_t *node, uint32_t stat_idx, uint32_t stat_number, void *extra_data);
static int32_t _diag_mqtt_conn_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc);
static int32_t _diag_mqtt_hb_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc);
static void _diag_alink_uplink_extra_clean(void *handle, void *extra_data);
static int32_t _diag_alink_uplink_extra_stop(void *handle, diag_running_state_node_t *node, uint32_t stat_idx, uint32_t stat_number, void *extra_data);
static int32_t _diag_alink_uplink_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc);

static diag_config_t g_diag_config[] = {
    {
        DIAG_MQTT_CONNECTION_STAT_INDEX,
        DIAG_MQTT_CONNECTION_NAME_STR,
        DIAG_TLV_MQTT_CONNECTION,
        {
            DIAG_DEFAULT_MQTT_CONN_ENABLED,
            DIAG_DEFAULT_MQTT_CONN_INTERVAL_MS,
            DIAG_DEFAULT_MQTT_CONN_WARNING_THRESHOLD,
            DIAG_DEFAULT_MQTT_CONN_FATAL_THRESHOLD,
        },
        DIAG_DEFAULT_MQTT_CONN_MAX_STAT_NUMBER,
        {
            _diag_mqtt_conn_hb_extra_clean,
            _diag_mqtt_conn_hb_extra_stop,
            _diag_mqtt_conn_report_desc_append
        },
        1
    },
    {
        DIAG_MQTT_HEARTBEAT_STAT_INDEX,
        DIAG_MQTT_HEARTBEAT_NAME_STR,
        DIAG_TLV_MQTT_HEARTBEAT,
        {
            DIAG_DEFAULT_MQTT_HB_ENABLED,
            DIAG_DEFAULT_MQTT_HB_INTERVAL_MS,
            DIAG_DEFAULT_MQTT_HB_WARNING_THRESHOLD,
            DIAG_DEFAULT_MQTT_HB_FATAL_THRESHOLD
        },
        DIAG_DEFAULT_MQTT_HB_MAX_STAT_NUMBER,
        {
            _diag_mqtt_conn_hb_extra_clean,
            _diag_mqtt_conn_hb_extra_stop,
            _diag_mqtt_hb_report_desc_append,
        },
        0
    },
    {
        DIAG_ALINK_UPLINK_STAT_INDEX,
        DIAG_ALINK_UPLINK_NAME_STR,
        DIAG_TLV_ALINK_UPLINK,
        {
            DIAG_DEFAULT_ALINK_UPLINK_ENABLED,
            DIAG_DEFAULT_ALINK_UPLINK_INTERVAL_MS,
            DIAG_DEFAULT_ALINK_UPLINK_WARNING_THRESHOLD,
            DIAG_DEFAULT_ALINK_UPLINK_FATAL_THRESHOLD
        },
        DIAG_DEFAULT_ALINK_UPLINK_MAX_STAT_NUMBER,
        {
            _diag_alink_uplink_extra_clean,
            _diag_alink_uplink_extra_stop,
            _diag_alink_uplink_report_desc_append
        },
        0
    }
};

static void _core_diag_exec_inc(diag_handle_t *diag_handle)
{
    diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
    diag_handle->exec_count++;
    diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);
}

static void _core_diag_exec_dec(diag_handle_t *diag_handle)
{
    diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
    diag_handle->exec_count--;
    diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);
}

static void _diag_desc_list_append(diag_handle_t *diag_handle, diag_stat_t *stat, diag_running_state_node_t *running_state_node, struct core_list_head *desc_list)
{
    int32_t res = STATE_SUCCESS;
    char *desc = NULL;
    diag_desc_node_t *node = NULL;

    res = stat->stat_cb.desc_append_cb(diag_handle, &stat->running_state, running_state_node, &desc);
    if (res < STATE_SUCCESS) {
        return;
    }

    node = diag_handle->sysdep->core_sysdep_malloc(sizeof(diag_desc_node_t), DIAG_MODULE_NAME);
    if (node == NULL) {
        diag_handle->sysdep->core_sysdep_free(desc);
        return;
    }
    memset(node, 0, sizeof(diag_desc_node_t));
    node->timestamp = core_log_get_timestamp(diag_handle->sysdep);
    node->code = stat->running_state.code;
    node->module_name = stat->running_state.name;
    node->level = (stat->running_state.is_reported == 0)?(running_state_node->level):(DIAG_REPORT_LEVEL_WARNING_STR);
    node->desc = desc;
    node->qos = stat->running_state.qos;
    CORE_INIT_LIST_HEAD(&node->linked_node);

    core_list_add_tail(&node->linked_node, desc_list);
}

static void _diag_desc_list_send(diag_handle_t *diag_handle, struct core_list_head *desc_list)
{
    diag_desc_node_t *node = NULL;

    core_list_for_each_entry(node, desc_list, linked_node, diag_desc_node_t) {
        /* local event notify */
        if ((diag_handle->event_handler != NULL) && (diag_handle->local_report_enabled == 1)) {
            aiot_diag_event_t event;

            memset(&event, 0, sizeof(aiot_diag_event_t));
            event.type = AIOT_DIAGEVT_ALERT;
            event.data.alert.module_name = node->module_name;
            event.data.alert.level = node->level;
            event.data.alert.desc = node->desc;

            diag_handle->event_handler(diag_handle, &event, diag_handle->userdata);
        }

        /* cloud event report */
        if (diag_handle->cloud_report_enabled == 1) {
            int32_t res = STATE_SUCCESS;
            char *topic = NULL, *topic_fmt = DIAG_REPORT_TOPIC_FMT;
            char *topic_src[] = { core_mqtt_get_product_key(diag_handle->mqtt_handle), core_mqtt_get_device_name(diag_handle->mqtt_handle) };
            int32_t alink_id = 0;
            char alink_id_str[11] = {0}, utc_time_str[21] = {0}, code_str[11] = {0};
            char *payload = NULL, *payload_fmt = DIAG_REPORT_PAYLOAD_FMT;
            char *payload_src[] = { alink_id_str, utc_time_str, node->level, node->module_name, code_str, alink_id_str, node->desc };

            res = core_sprintf(diag_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src)/sizeof(char *), DIAG_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                continue;
            }

            core_global_alink_id_next(diag_handle->sysdep, &alink_id);
            core_int2str(alink_id, alink_id_str, NULL);
            core_uint642str(node->timestamp, utc_time_str, NULL);
            core_uint2str(node->code, code_str, NULL);
            res = core_sprintf(diag_handle->sysdep, &payload, payload_fmt, payload_src, sizeof(payload_src)/sizeof(char *), DIAG_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                diag_handle->sysdep->core_sysdep_free(topic);
                continue;
            }

            aiot_mqtt_pub(diag_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), node->qos);
            diag_handle->sysdep->core_sysdep_free(topic);
            diag_handle->sysdep->core_sysdep_free(payload);
        }
    }
}

static void _diag_desc_list_destroy(diag_handle_t *diag_handle, struct core_list_head *desc_list)
{
    diag_desc_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, desc_list, linked_node, diag_desc_node_t) {
        core_list_del(&node->linked_node);
        diag_handle->sysdep->core_sysdep_free(node->desc);
        diag_handle->sysdep->core_sysdep_free(node);
    }
}

static void _diag_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    uint32_t stat_idx = 0;
    diag_handle_t *diag_handle = (diag_handle_t *)context;
    uint64_t timenow_ms = core_log_get_timestamp(diag_handle->sysdep);

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                diag_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }

    if (diag_handle->mqtt_process.last_check_time > timenow_ms) {
        diag_handle->mqtt_process.last_check_time = timenow_ms;
    }

    if (timenow_ms - diag_handle->mqtt_process.last_check_time >= DIAG_MQTT_PROCESS_CHECK_INTERVAL_MS) {
        diag_running_state_node_t *node = NULL;
        struct core_list_head desc_list;

        CORE_INIT_LIST_HEAD(&desc_list);

        for (stat_idx = 0;stat_idx < DIAG_STAT_ITEM_NUMBER;stat_idx++) {
            diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->diag_stat[stat_idx].running_state.mutex);
            diag_handle->diag_stat[stat_idx].running_state.alert_counts = 0;
            core_list_for_each_entry(node, &diag_handle->diag_stat[stat_idx].running_state.linked_list,
                                     linked_node, diag_running_state_node_t) {
                if ((node->stop_time != 0) && (node->start_time > node->stop_time)) {
                    node->start_time = node->stop_time;
                }
                if (node->is_diag == 1) {
                    if (node->start_time > timenow_ms) {
                        node->start_time = timenow_ms;
                    }
                    if (node->stop_time == 0 && (timenow_ms - node->start_time >= diag_handle->diag_stat[stat_idx].config.fatal_threshold)) {
                        node->stop_time = node->start_time + diag_handle->diag_stat[stat_idx].config.fatal_threshold;
                    }
                    if (node->stop_time != 0) {
                        if ((node->stop_time - node->start_time >= diag_handle->diag_stat[stat_idx].config.warning_threashold) &&
                            (node->stop_time - node->start_time < diag_handle->diag_stat[stat_idx].config.fatal_threshold)) {
                            node->level = DIAG_REPORT_LEVEL_WARNING_STR;
                        } else if (node->stop_time - node->start_time >= diag_handle->diag_stat[stat_idx].config.fatal_threshold) {
                            node->level = DIAG_REPORT_LEVEL_FATAL_STR;
                        }
                        node->is_diag = 0;
                    }
                    if ((diag_handle->diag_stat[stat_idx].running_state.is_reported == 0) && (node->level != NULL)) {
                        /* report first in current period*/
                        _diag_desc_list_append(diag_handle, &diag_handle->diag_stat[stat_idx], node, &desc_list);
                        diag_handle->diag_stat[stat_idx].running_state.is_reported = 1;
                        diag_handle->diag_stat[stat_idx].running_state.report_start_time = timenow_ms;
                    }
                }
                if ((node->start_time >= diag_handle->diag_stat[stat_idx].running_state.report_start_time) && node->level != NULL) {
                    diag_handle->diag_stat[stat_idx].running_state.alert_counts++;
                }
                if (diag_handle->diag_stat[stat_idx].running_state.report_start_time > timenow_ms) {
                    diag_handle->diag_stat[stat_idx].running_state.report_start_time = timenow_ms;
                }
                if ((diag_handle->diag_stat[stat_idx].running_state.is_reported == 1) &&
                    (timenow_ms - diag_handle->diag_stat[stat_idx].running_state.report_start_time >= diag_handle->diag_stat[stat_idx].config.interval_ms) &&
                    (node->linked_node.next == &diag_handle->diag_stat[stat_idx].running_state.linked_list)) {
                    /* report alert counts in this period */
                    if (diag_handle->diag_stat[stat_idx].running_state.alert_counts > 0) {
                        _diag_desc_list_append(diag_handle, &diag_handle->diag_stat[stat_idx], node, &desc_list);
                    }
                    diag_handle->diag_stat[stat_idx].running_state.is_reported = 0;
                    diag_handle->diag_stat[stat_idx].running_state.report_start_time = timenow_ms;
                }
            }
            diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->diag_stat[stat_idx].running_state.mutex);
        }

        _diag_desc_list_send(diag_handle, &desc_list);
        _diag_desc_list_destroy(diag_handle, &desc_list);

        diag_handle->mqtt_process.last_check_time = timenow_ms;
    }
}

static int32_t _diag_core_mqtt_operate_process_handler(diag_handle_t *diag_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _diag_core_mqtt_process_handler;
    process_data.context = diag_handle;

    return core_mqtt_setopt(diag_handle->mqtt_handle, option, &process_data);
}

static void _diag_mqtt_conn_hb_extra_clean(void *handle, void *extra_data)
{

}

static int32_t _diag_mqtt_conn_hb_extra_stop(void *handle, diag_running_state_node_t *node, uint32_t stat_idx, uint32_t stat_number, void *extra_data)
{
    if (node->stop_time != 0) {
        return STATE_DIAG_STOP_NODE_NOT_MATCH;
    }
    return STATE_SUCCESS;
}

static int32_t _diag_mqtt_conn_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc)
{
    char *tmp_desc = NULL;
    diag_handle_t *diag_handle = (diag_handle_t *)handle;
    uint64_t timenow_ms = core_log_get_timestamp(diag_handle->sysdep);

    if (running_state->is_reported == 0) {
        /* report first time */
        uint64_t time_elapsed = node->stop_time - node->start_time;
        char time_elapsed_str[21] = {0};
        char *desc_fmt = "MQTT connection establish time %s ms";
        char *desc_src[] = { time_elapsed_str };

        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    } else if (running_state->is_reported == 1) {
        /* report period stat data */
        uint64_t time_elapsed = timenow_ms - running_state->report_start_time;
        char time_elapsed_str[21] = {0};
        char alert_counts_str[11] = {0};
        char *desc_fmt = "MQTT connection has been alert extra %s times in past %s ms";
        char *desc_src[] = { alert_counts_str, time_elapsed_str };

        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_uint2str(running_state->alert_counts, alert_counts_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    }

    if (tmp_desc == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    *desc = tmp_desc;

    return STATE_SUCCESS;
}

static int32_t _diag_mqtt_hb_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc)
{
    char *tmp_desc = NULL;
    diag_handle_t *diag_handle = (diag_handle_t *)handle;
    uint64_t timenow_ms = core_log_get_timestamp(diag_handle->sysdep);

    if (running_state->is_reported == 0) {
        /* report first time */
        uint64_t time_elapsed = node->stop_time - node->start_time;
        char time_elapsed_str[21] = {0};
        char *desc_fmt = "MQTT lost heartbeat 1 times in %s ms";
        char *desc_src[] = { time_elapsed_str };

        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    } else if (running_state->is_reported == 1) {
        /* report period stat data */
        uint64_t time_elapsed = timenow_ms - running_state->report_start_time;
        char time_elapsed_str[21] = {0};
        char alert_counts_str[11] = {0};
        char *desc_fmt = "MQTT lost heartbeat has been alert extra %s times in past %s ms";
        char *desc_src[] = { alert_counts_str, time_elapsed_str };

        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_uint2str(running_state->alert_counts, alert_counts_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    }

    if (tmp_desc == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    *desc = tmp_desc;

    return STATE_SUCCESS;
}

static void _diag_alink_uplink_extra_clean(void *handle, void *extra_data)
{
    diag_handle_t *diag_handle = (diag_handle_t *)handle;

    diag_handle->sysdep->core_sysdep_free(extra_data);
}

static int32_t _diag_alink_uplink_extra_stop(void *handle, diag_running_state_node_t *node, uint32_t stat_idx, uint32_t stat_number, void *extra_data)
{
    if (*(uint32_t *)node->extra_data != *(uint32_t *)extra_data) {
        return STATE_DIAG_STOP_NODE_NOT_MATCH;
    }
    return STATE_SUCCESS;
}

static int32_t _diag_alink_uplink_report_desc_append(void *handle, diag_running_state_t *running_state, diag_running_state_node_t *node, char **desc)
{
    char *tmp_desc = NULL;
    diag_handle_t *diag_handle = (diag_handle_t *)handle;
    uint64_t timenow_ms = core_log_get_timestamp(diag_handle->sysdep);

    if (running_state->is_reported == 0) {
        /* report first time */
        char alink_id_str[11] = {0};
        uint64_t time_elapsed = node->stop_time - node->start_time;
        char time_elapsed_str[21] = {0};
        char *desc_fmt = "Alink message %s waiting for reply has already exceed %s ms";
        char *desc_src[] = { alink_id_str, time_elapsed_str };

        core_uint2str(*(uint32_t *)node->extra_data, alink_id_str, NULL);
        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    } else if (running_state->is_reported == 1) {
        /* report period stat data */
        uint64_t time_elapsed = timenow_ms - running_state->report_start_time;
        char time_elapsed_str[21] = {0};
        char alert_counts_str[11] = {0};
        char *desc_fmt = "Alink message reply too slow has been alert extra %s times in past %s ms";
        char *desc_src[] = { alert_counts_str, time_elapsed_str };

        core_uint642str(time_elapsed, time_elapsed_str, NULL);
        core_uint2str(running_state->alert_counts, alert_counts_str, NULL);
        core_sprintf(diag_handle->sysdep, &tmp_desc, desc_fmt, desc_src, sizeof(desc_src)/sizeof(char *), DIAG_MODULE_NAME);
    }

    if (tmp_desc == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    *desc = tmp_desc;

    return STATE_SUCCESS;
}

static void _diag_running_state_start(diag_handle_t *diag_handle, diag_stat_t *stat, uint64_t timestamp, void *extra_data)
{
    diag_running_state_node_t *node = NULL;

    diag_handle->sysdep->core_sysdep_mutex_lock(stat->running_state.mutex);
    if (stat->running_state.stat_number == stat->running_state.max_stat_number) {
        node = core_list_entry(stat->running_state.linked_list.next, diag_running_state_node_t, linked_node);
        core_list_del(&node->linked_node);
        stat->running_state.stat_number--;
        stat->stat_cb.extra_clean_cb(diag_handle, node->extra_data);
        diag_handle->sysdep->core_sysdep_free(node);
    }

    node = diag_handle->sysdep->core_sysdep_malloc(sizeof(diag_running_state_node_t), DIAG_MODULE_NAME);
    if (node == NULL) {
        diag_handle->sysdep->core_sysdep_mutex_unlock(stat->running_state.mutex);
        return;
    }
    memset(node, 0, sizeof(diag_running_state_node_t));
    node->is_diag = 1;
    node->start_time = timestamp;
    node->extra_data = extra_data;
    CORE_INIT_LIST_HEAD(&node->linked_node);

    core_list_add_tail(&node->linked_node, &stat->running_state.linked_list);
    stat->running_state.stat_number++;
    diag_handle->sysdep->core_sysdep_mutex_unlock(stat->running_state.mutex);
}

static void _diag_running_state_stop(diag_handle_t *diag_handle, diag_stat_t *stat, uint64_t timestamp, void *extra_data)
{
    uint32_t stat_idx = 0;
    diag_running_state_node_t *node = NULL;

    diag_handle->sysdep->core_sysdep_mutex_lock(stat->running_state.mutex);
    core_list_for_each_entry(node, &stat->running_state.linked_list,
                             linked_node, diag_running_state_node_t) {
        if (stat->stat_cb.extra_stop_cb(diag_handle, node,
            stat_idx, stat->running_state.stat_number, extra_data) >= STATE_SUCCESS) {
            node->stop_time = timestamp;
            break;
        }
        stat_idx++;
    }
    stat->stat_cb.extra_clean_cb(diag_handle, extra_data);
    diag_handle->sysdep->core_sysdep_mutex_unlock(stat->running_state.mutex);
}

static int32_t _diag_get_extra_data(diag_handle_t *diag_handle, diag_raw_data_t *raw_data, uint32_t code, void **out_extra_data)
{
    if (code == DIAG_TLV_ALINK_UPLINK) {
        uint32_t tlv_sub_type1 = (raw_data->data[4] << 8) | (raw_data->data[5]);
        if (tlv_sub_type1 == DIAG_TLV_ALINK_MSGID) {
            uint32_t *extra_data = NULL;
            extra_data = diag_handle->sysdep->core_sysdep_malloc(sizeof(uint32_t), DIAG_MODULE_NAME);
            if (extra_data == NULL) {
                return STATE_SYS_DEPEND_MALLOC_FAILED;
            }
            memset(extra_data, 0, sizeof(uint32_t));
            *extra_data = ((raw_data->data[7] << 24) | (raw_data->data[8] << 16) | (raw_data->data[9] << 8) | (raw_data->data[10]));
            *(uint32_t **)out_extra_data = extra_data;
        }
    }

    return STATE_SUCCESS;
}

static void _diag_raw_data(diag_handle_t *diag_handle, diag_raw_data_t *raw_data)
{
    uint32_t code = (raw_data->data[0] << 8) | (raw_data->data[1]);
    uint32_t stat_idx = 0;
    void *extra_data = NULL;

    for (stat_idx = 0;stat_idx < DIAG_STAT_ITEM_NUMBER;stat_idx++) {
        if (g_diag_config[stat_idx].code == code) {
            if (_diag_get_extra_data(diag_handle, raw_data, code, &extra_data) < STATE_SUCCESS) {
                return;
            }
            if (raw_data->data[3] == 0x00) {
                _diag_running_state_start(diag_handle, &diag_handle->diag_stat[stat_idx], raw_data->timestamp, extra_data);
            } else if (raw_data->data[3] == 0x01) {
                _diag_running_state_stop(diag_handle, &diag_handle->diag_stat[stat_idx], raw_data->timestamp, extra_data);
            }
        }
    }
}

static void _diag_core_diag_callback(void *handle, uint64_t timestamp, int32_t code, uint8_t *data, uint32_t data_len)
{
    diag_raw_data_t raw_data;

    memset(&raw_data, 0, sizeof(diag_raw_data_t));
    raw_data.timestamp = timestamp;
    raw_data.code = code;
    raw_data.data = data;
    raw_data.data_len = data_len;

    _diag_raw_data((diag_handle_t *)handle, &raw_data);
}

static void _diag_running_state_clean(diag_handle_t *diag_handle)
{
    uint32_t stat_idx = 0;
    diag_running_state_node_t *node = NULL, *next = NULL;

    for (stat_idx = 0;stat_idx < DIAG_STAT_ITEM_NUMBER;stat_idx++) {
        diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->diag_stat[stat_idx].running_state.mutex);
        core_list_for_each_entry_safe(node, next, &diag_handle->diag_stat[stat_idx].running_state.linked_list,
                                      linked_node, diag_running_state_node_t) {
            core_list_del(&node->linked_node);
            diag_handle->diag_stat[stat_idx].stat_cb.extra_clean_cb(diag_handle, node->extra_data);
            diag_handle->sysdep->core_sysdep_free(node);
        }
        diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->diag_stat[stat_idx].running_state.mutex);
        diag_handle->sysdep->core_sysdep_mutex_deinit(&diag_handle->diag_stat[stat_idx].running_state.mutex);
    }
}

void *aiot_diag_init(void)
{
    int32_t res = STATE_SUCCESS;
    uint32_t stat_idx = 0;
    diag_handle_t *diag_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    res = core_global_init(sysdep);
    if (res < STATE_SUCCESS) {
        return NULL;
    }

    diag_handle = sysdep->core_sysdep_malloc(sizeof(diag_handle_t), DIAG_MODULE_NAME);
    if (diag_handle == NULL) {
        return NULL;
    }
    memset(diag_handle, 0, sizeof(diag_handle_t));

    diag_handle->sysdep = sysdep;
    diag_handle->local_report_enabled = DIAG_DAFAULT_LOCAL_REPORT_ENABLED;
    diag_handle->cloud_report_enabled = DIAG_DAFAULT_CLOUD_REPORT_ENABLED;

    for (stat_idx = 0; stat_idx < DIAG_STAT_ITEM_NUMBER; stat_idx++) {
        diag_handle->diag_stat[stat_idx].running_state.code = g_diag_config[stat_idx].code;
        diag_handle->diag_stat[stat_idx].running_state.name = g_diag_config[stat_idx].name;
        memcpy(&diag_handle->diag_stat[stat_idx].config, &g_diag_config[stat_idx].def_config, sizeof(aiot_diag_config_t));
        diag_handle->diag_stat[stat_idx].running_state.max_stat_number = g_diag_config[stat_idx].def_max_stat_number;
        memcpy(&diag_handle->diag_stat[stat_idx].stat_cb, &g_diag_config[stat_idx].def_stat_cb, sizeof(diag_stat_callback_t));
        diag_handle->diag_stat[stat_idx].running_state.qos = g_diag_config[stat_idx].qos;
        CORE_INIT_LIST_HEAD(&diag_handle->diag_stat[stat_idx].running_state.linked_list);
        diag_handle->diag_stat[stat_idx].running_state.mutex = diag_handle->sysdep->core_sysdep_mutex_init();
    }

    diag_handle->deinit_timeout_ms = DIAG_DEFAULT_DEINIT_TIMEOUT_MS;

    diag_handle->data_mutex = sysdep->core_sysdep_mutex_init();

    diag_handle->exec_enabled = 1;

    core_diag_set_cb(diag_handle, _diag_core_diag_callback);

    return diag_handle;
}

int32_t aiot_diag_setopt(void *handle, aiot_diag_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    diag_handle_t *diag_handle = (diag_handle_t *)handle;

    if (diag_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_DIAGOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (diag_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_diag_exec_inc(diag_handle);

    diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
    switch (option) {
        case AIOT_DIAGOPT_MQTT_HANDLE: {
            diag_handle->mqtt_handle = data;
            diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);
            res = _diag_core_mqtt_operate_process_handler(diag_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
        }
        break;
        case AIOT_DIAGOPT_LOCAL_REPORT_ENABLED: {
            diag_handle->local_report_enabled = *(uint8_t *)data;
        }
        break;
        case AIOT_DIAGOPT_CLOUD_REPORT_ENABLED: {
            diag_handle->cloud_report_enabled = *(uint8_t *)data;
        }
        break;
        case AIOT_DIAGOPT_MQTT_CONNECTION: {
            memcpy(&diag_handle->diag_stat[DIAG_MQTT_CONNECTION_STAT_INDEX].config, (aiot_diag_config_t *)data, sizeof(aiot_diag_config_t));
        }
        break;
        case AIOT_DIAGOPT_MQTT_HEARTBEAT: {
            memcpy(&diag_handle->diag_stat[DIAG_MQTT_HEARTBEAT_STAT_INDEX].config, (aiot_diag_config_t *)data, sizeof(aiot_diag_config_t));
        }
        break;
        case AIOT_DIAGOPT_ALINK_UPLINK: {
            memcpy(&diag_handle->diag_stat[DIAG_ALINK_UPLINK_STAT_INDEX].config, (aiot_diag_config_t *)data, sizeof(aiot_diag_config_t));
        }
        break;
        case AIOT_DIAGOPT_RECV_HANDLER: {
            diag_handle->recv_handler = (aiot_diag_recv_handler_t)data;
        }
        break;
        case AIOT_DIAGOPT_EVENT_HANDLER: {
            diag_handle->event_handler = (aiot_diag_event_handler_t)data;
        }
        break;
        case AIOT_DIAGOPT_USERDATA: {
            diag_handle->userdata = data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);

    _core_diag_exec_dec(diag_handle);

    return res;
}

int32_t aiot_diag_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    diag_handle_t *diag_handle = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    diag_handle = *(diag_handle_t **)handle;

    if (diag_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    core_diag_set_cb(diag_handle, NULL);

    _diag_core_mqtt_operate_process_handler(diag_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);

    diag_handle->exec_enabled = 0;
    deinit_timestart = diag_handle->sysdep->core_sysdep_time();
    do {
        if (diag_handle->exec_count == 0) {
            break;
        }
        diag_handle->sysdep->core_sysdep_sleep(DIAG_DEINIT_INTERVAL_MS);
    } while ((diag_handle->sysdep->core_sysdep_time() - deinit_timestart) < diag_handle->deinit_timeout_ms);

    if (diag_handle->exec_count != 0) {
        return STATE_MQTT_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    _diag_running_state_clean(diag_handle);

    diag_handle->sysdep->core_sysdep_mutex_deinit(&diag_handle->data_mutex);

    core_global_deinit(diag_handle->sysdep);

    diag_handle->sysdep->core_sysdep_free(diag_handle);

    return STATE_SUCCESS;
}

int32_t aiot_diag_start(void *handle)
{
    diag_handle_t *diag_handle = (diag_handle_t *)handle;

    if (diag_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
    diag_handle->diag_status = 1;
    diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);

    return STATE_SUCCESS;
}

int32_t aiot_diag_stop(void *handle)
{
    diag_handle_t *diag_handle = (diag_handle_t *)handle;

    if (diag_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    diag_handle->sysdep->core_sysdep_mutex_lock(diag_handle->data_mutex);
    diag_handle->diag_status = 0;
    diag_handle->sysdep->core_sysdep_mutex_unlock(diag_handle->data_mutex);

    return STATE_SUCCESS;
}

