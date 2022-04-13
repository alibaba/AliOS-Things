/**
 * @file aiot_ntp_api.c
 * @brief ntp模块的API接口实现, 提供获取utc时间的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

/* TODO: 对本模块的头文件, 仅需包含ntp_private.h, 不需包含aiot_ntp_api.h */
#include "ntp_private.h"

/* TODO: 列出对core模块需要包含的头文件 */
#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "core_mqtt.h"

static void _core_ntp_exec_inc(ntp_handle_t *ntp_handle)
{
    ntp_handle->sysdep->core_sysdep_mutex_lock(ntp_handle->data_mutex);
    ntp_handle->exec_count++;
    ntp_handle->sysdep->core_sysdep_mutex_unlock(ntp_handle->data_mutex);
}

static void _core_ntp_exec_dec(ntp_handle_t *ntp_handle)
{
    ntp_handle->sysdep->core_sysdep_mutex_lock(ntp_handle->data_mutex);
    ntp_handle->exec_count--;
    ntp_handle->sysdep->core_sysdep_mutex_unlock(ntp_handle->data_mutex);
}

static void _ntp_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    ntp_handle_t *ntp_handle = (ntp_handle_t *)userdata;

    switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            char *dst_key = "deviceSendTime", *srt_key = "serverRecvTime", *sst_key = "serverSendTime";
            char *dst_value = NULL, *srt_value = NULL, *sst_value = NULL;
            uint32_t dst_value_len = 0, srt_value_len = 0, sst_value_len = 0;
            uint64_t dst = 0, srt = 0, sst = 0, utc = 0;

            if (core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len, dst_key, (uint32_t)strlen(dst_key),
                                &dst_value, &dst_value_len) == STATE_SUCCESS &&
                core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len, srt_key, (uint32_t)strlen(srt_key),
                                &srt_value, &srt_value_len) == STATE_SUCCESS &&
                core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len, sst_key, (uint32_t)strlen(sst_key),
                                &sst_value, &sst_value_len) == STATE_SUCCESS) {
                if (core_str2uint64(dst_value, (uint8_t)dst_value_len, &dst) == STATE_SUCCESS &&
                    core_str2uint64(srt_value, (uint8_t)srt_value_len, &srt) == STATE_SUCCESS &&
                    core_str2uint64(sst_value, (uint8_t)sst_value_len, &sst) == STATE_SUCCESS) {
                    core_date_t date;
                    utc = (srt + sst + ntp_handle->sysdep->core_sysdep_time() - dst) / 2;

                    core_log_set_timestamp(ntp_handle->sysdep, utc);

                    memset(&date, 0, sizeof(core_date_t));
                    core_utc2date(utc, ntp_handle->time_zone, &date);
                    if (ntp_handle->recv_handler != NULL) {
                        aiot_ntp_recv_t recv;

                        memset(&recv, 0, sizeof(aiot_ntp_recv_t));
                        recv.type = AIOT_NTPRECV_LOCAL_TIME;
                        recv.data.local_time.timestamp = utc;
                        recv.data.local_time.year = date.year;
                        recv.data.local_time.mon = date.mon;
                        recv.data.local_time.day = date.day;
                        recv.data.local_time.hour = date.hour;
                        recv.data.local_time.min = date.min;
                        recv.data.local_time.sec = date.sec;
                        recv.data.local_time.msec = date.msec;

                        ntp_handle->recv_handler(ntp_handle, &recv, ntp_handle->userdata);
                    }
                } else {
                    if (ntp_handle->event_handler != NULL) {
                        aiot_ntp_event_t event;

                        memset(&event, 0, sizeof(aiot_ntp_event_t));
                        event.type = AIOT_NTPEVT_INVALID_TIME_FORMAT;
                        ntp_handle->event_handler(ntp_handle, &event, ntp_handle->userdata);
                    }
                }
            } else {
                if (ntp_handle->event_handler != NULL) {
                    aiot_ntp_event_t event;

                    memset(&event, 0, sizeof(aiot_ntp_event_t));
                    event.type = AIOT_NTPEVT_INVALID_RESPONSE;
                    ntp_handle->event_handler(ntp_handle, &event, ntp_handle->userdata);
                }
            }
        }
        default: {

        }
        break;
    }
}

static int32_t _ntp_operate_topic_map(ntp_handle_t *ntp_handle, aiot_mqtt_option_t option)
{
    int32_t res = STATE_SUCCESS;
    aiot_mqtt_topic_map_t map;
    char *topic = NULL;
    char *topic_src[] = { core_mqtt_get_product_key(ntp_handle->mqtt_handle), core_mqtt_get_device_name(ntp_handle->mqtt_handle) };
    char *topic_fmt = NTP_RESPONSE_TOPIC_FMT;

    memset(&map, 0, sizeof(aiot_mqtt_topic_map_t));

    res = core_sprintf(ntp_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src) / sizeof(char *),
                       NTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    map.topic = topic;
    map.handler = _ntp_recv_handler;
    map.userdata = (void *)ntp_handle;

    res = aiot_mqtt_setopt(ntp_handle->mqtt_handle, option, &map);
    ntp_handle->sysdep->core_sysdep_free(topic);

    return res;
}

static void _ntp_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    ntp_handle_t *ntp_handle = (ntp_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                ntp_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _ntp_core_mqtt_operate_process_handler(ntp_handle_t *ntp_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _ntp_core_mqtt_process_handler;
    process_data.context = ntp_handle;

    return core_mqtt_setopt(ntp_handle->mqtt_handle, option, &process_data);
}

void *aiot_ntp_init(void)
{
    ntp_handle_t *ntp_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    ntp_handle = sysdep->core_sysdep_malloc(sizeof(ntp_handle_t), NTP_MODULE_NAME);
    if (ntp_handle == NULL) {
        return NULL;
    }
    memset(ntp_handle, 0, sizeof(ntp_handle_t));

    ntp_handle->sysdep = sysdep;
    ntp_handle->deinit_timeout_ms = NTP_DEFAULT_DEINIT_TIMEOUT_MS;

    ntp_handle->data_mutex = sysdep->core_sysdep_mutex_init();

    ntp_handle->exec_enabled = 1;

    return ntp_handle;
}

int32_t aiot_ntp_setopt(void *handle, aiot_ntp_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    ntp_handle_t *ntp_handle = (ntp_handle_t *)handle;

    if (handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_NTPOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (ntp_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_ntp_exec_inc(ntp_handle);

    ntp_handle->sysdep->core_sysdep_mutex_lock(ntp_handle->data_mutex);
    switch (option) {
        case AIOT_NTPOPT_MQTT_HANDLE: {
            ntp_handle->mqtt_handle = data;
            ntp_handle->sysdep->core_sysdep_mutex_unlock(ntp_handle->data_mutex);
            res = _ntp_operate_topic_map(ntp_handle, AIOT_MQTTOPT_APPEND_TOPIC_MAP);
            if (res >= STATE_SUCCESS) {
                res = _ntp_core_mqtt_operate_process_handler(ntp_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
            ntp_handle->sysdep->core_sysdep_mutex_lock(ntp_handle->data_mutex);
        }
        break;
        case AIOT_NTPOPT_TIME_ZONE: {
            ntp_handle->time_zone = *(int8_t *)data;
        }
        break;
        case AIOT_NTPOPT_RECV_HANDLER: {
            ntp_handle->recv_handler = (aiot_ntp_recv_handler_t)data;
        }
        break;
        case AIOT_NTPOPT_EVENT_HANDLER: {
            ntp_handle->event_handler = (aiot_ntp_event_handler_t)data;
        }
        break;
        case AIOT_NTPOPT_USERDATA: {
            ntp_handle->userdata = data;
        }
        break;
        case AIOT_NTPOPT_DEINIT_TIMEOUT_MS: {
            ntp_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    ntp_handle->sysdep->core_sysdep_mutex_unlock(ntp_handle->data_mutex);

    _core_ntp_exec_dec(ntp_handle);

    return res;
}

int32_t aiot_ntp_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    ntp_handle_t *ntp_handle = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    ntp_handle = *(ntp_handle_t **)handle;

    if (ntp_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    ntp_handle->exec_enabled = 0;

    _ntp_core_mqtt_operate_process_handler(ntp_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);
    _ntp_operate_topic_map(ntp_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP);

    deinit_timestart = ntp_handle->sysdep->core_sysdep_time();
    do {
        if (ntp_handle->exec_count == 0) {
            break;
        }
        ntp_handle->sysdep->core_sysdep_sleep(NTP_DEINIT_INTERVAL_MS);
    } while ((ntp_handle->sysdep->core_sysdep_time() - deinit_timestart) < ntp_handle->deinit_timeout_ms);

    if (ntp_handle->exec_count != 0) {
        return STATE_MQTT_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    ntp_handle->sysdep->core_sysdep_mutex_deinit(&ntp_handle->data_mutex);

    ntp_handle->sysdep->core_sysdep_free(ntp_handle);

    return STATE_SUCCESS;
}

int32_t aiot_ntp_send_request(void *handle)
{
    int32_t res = STATE_SUCCESS;
    char *topic = NULL, *payload = NULL;
    ntp_handle_t *ntp_handle = (ntp_handle_t *)handle;

    if (handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (ntp_handle->mqtt_handle == NULL) {
        return STATE_NTP_MISSING_MQTT_HANDLE;
    }

    if (ntp_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_ntp_exec_inc(ntp_handle);

    {
        char *topic_src[] = { core_mqtt_get_product_key(ntp_handle->mqtt_handle), core_mqtt_get_device_name(ntp_handle->mqtt_handle) };
        char *topic_fmt = NTP_REQUEST_TOPIC_FMT;
        char time_str[21] = {0};
        char *payload_src[] = { time_str };
        char *payload_fmt = NTP_REQUEST_PAYLOAD_FMT;

        res = core_sprintf(ntp_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src) / sizeof(char *),
                           NTP_MODULE_NAME);
        if (res < STATE_SUCCESS) {
            _core_ntp_exec_dec(ntp_handle);
            return res;
        }

        core_uint642str(ntp_handle->sysdep->core_sysdep_time(), time_str, NULL);
        res = core_sprintf(ntp_handle->sysdep, &payload, payload_fmt, payload_src, sizeof(payload_src) / sizeof(char *),
                           NTP_MODULE_NAME);
        if (res < STATE_SUCCESS) {
            ntp_handle->sysdep->core_sysdep_free(topic);
            _core_ntp_exec_dec(ntp_handle);
            return res;
        }
    }

    res = aiot_mqtt_pub(ntp_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), 0);
    ntp_handle->sysdep->core_sysdep_free(topic);
    ntp_handle->sysdep->core_sysdep_free(payload);
    if (res < STATE_SUCCESS) {
        _core_ntp_exec_dec(ntp_handle);
        return res;
    }

    _core_ntp_exec_dec(ntp_handle);

    return STATE_SUCCESS;
}

