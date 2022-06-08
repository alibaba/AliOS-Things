/**
 * @file aiot_devinfo_api.c
 * @brief devinfo模块的API接口实现, 提供更新和删除设备标签的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "devinfo_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "core_mqtt.h"

static void _core_devinfo_exec_inc(devinfo_handle_t *devinfo_handle)
{
    devinfo_handle->sysdep->core_sysdep_mutex_lock(devinfo_handle->data_mutex);
    devinfo_handle->exec_count++;
    devinfo_handle->sysdep->core_sysdep_mutex_unlock(devinfo_handle->data_mutex);
}

static void _core_devinfo_exec_dec(devinfo_handle_t *devinfo_handle)
{
    devinfo_handle->sysdep->core_sysdep_mutex_lock(devinfo_handle->data_mutex);
    devinfo_handle->exec_count--;
    devinfo_handle->sysdep->core_sysdep_mutex_unlock(devinfo_handle->data_mutex);
}

static void _devinfo_find_pk_dn(devinfo_handle_t *devinfo_handle, char *topic, uint16_t topic_len, char **product_key, char **device_name)
{
    uint16_t idx = 0, prev_slash = 0, slash = 0, pk_len = 0, dn_len = 0;
    char *pk_pos = NULL, *dn_pos = NULL, *tmp_pk = NULL, *tmp_dn = NULL;

    for (idx = 0;idx < topic_len;idx++) {
        if (topic[idx] == '/') {
            slash++;
            if (slash == 2) {
                pk_pos = &topic[idx + 1];
                prev_slash = idx;
            } else if (slash == 3) {
                dn_pos = &topic[idx + 1];
                pk_len = idx - prev_slash - 1;
                prev_slash = idx;
            } else if (slash == 4) {
                dn_len = idx - prev_slash - 1;
                break;
            }
        }
    }

    if (pk_len == 0 || dn_len == 0) {
        return;
    }

    tmp_pk = devinfo_handle->sysdep->core_sysdep_malloc(pk_len + 1, DEVINFO_MODULE_NAME);
    if (tmp_pk == NULL) {
        return;
    }
    memset(tmp_pk, 0, pk_len + 1);
    memcpy(tmp_pk, pk_pos, pk_len);

    tmp_dn = devinfo_handle->sysdep->core_sysdep_malloc(dn_len + 1, DEVINFO_MODULE_NAME);
    if (tmp_dn == NULL) {
        devinfo_handle->sysdep->core_sysdep_free(tmp_pk);
        return;
    }
    memset(tmp_dn, 0, dn_len + 1);
    memcpy(tmp_dn, dn_pos, dn_len);

    *product_key = tmp_pk;
    *device_name = tmp_dn;
}

static void _devinfo_mqtt_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    devinfo_handle_t *devinfo_handle = (devinfo_handle_t *)userdata;
    aiot_devinfo_event_t event;
    char *product_key = NULL, *device_name = NULL;
    char *code_key = "code", *id_key = "id", *data_key = "data", *message_key = "message";
    char *code_value = NULL, *id_value = NULL, *data_value = NULL, *message_value = NULL;
    uint32_t code_value_len = 0, id_value_len = 0, data_value_len = 0, message_value_len = 0;

    if (devinfo_handle->recv_handler == NULL) {
        return;
    }

    _devinfo_find_pk_dn(devinfo_handle, packet->data.pub.topic, packet->data.pub.topic_len, &product_key, &device_name);
    if (product_key == NULL || device_name == NULL) {
        if (devinfo_handle->event_handler != NULL) {
            memset(&event, 0, sizeof(aiot_devinfo_event_t));
            event.type = AIOT_DEVINFOEVT_INVALID_DEVINFO;
            devinfo_handle->event_handler(devinfo_handle, &event, devinfo_handle->userdata);
        }
        return;
    }

    if (core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len,
                    code_key, (uint32_t)strlen(code_key) ,&code_value, &code_value_len) == STATE_SUCCESS &&
        core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len,
                    id_key, (uint32_t)strlen(id_key) ,&id_value, &id_value_len) == STATE_SUCCESS &&
        core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len,
                    data_key, (uint32_t)strlen(data_key) ,&data_value, &data_value_len) == STATE_SUCCESS &&
        core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len,
                    message_key, (uint32_t)strlen(message_key) ,&message_value, &message_value_len) == STATE_SUCCESS) {
        uint32_t code = 0, id = 0;
        if (core_str2uint(code_value, code_value_len, &code) == STATE_SUCCESS &&
            core_str2uint(id_value, id_value_len, &id) == STATE_SUCCESS) {
            aiot_devinfo_recv_t recv;

            memset(&recv, 0, sizeof(aiot_devinfo_recv_t));
            recv.product_key = product_key;
            recv.device_name = device_name;
            recv.type = AIOT_DEVINFORECV_GENERIC_REPLY;
            recv.data.generic_reply.code = code;
            recv.data.generic_reply.msg_id = id;
            recv.data.generic_reply.data = data_value;
            recv.data.generic_reply.data_len = data_value_len;
            recv.data.generic_reply.message = message_value;
            recv.data.generic_reply.message_len = message_value_len;

            devinfo_handle->recv_handler(devinfo_handle, &recv, devinfo_handle->userdata);
        } else {
            if (devinfo_handle->event_handler != NULL) {
                memset(&event, 0, sizeof(aiot_devinfo_event_t));
                event.type = AIOT_DEVINFOEVT_INVALID_RESPONSE_FORMAT;
                devinfo_handle->event_handler(devinfo_handle, &event, devinfo_handle->userdata);
            }
        }
    } else {
        if (devinfo_handle->event_handler != NULL) {
            memset(&event, 0, sizeof(aiot_devinfo_event_t));
            event.type = AIOT_DEVINFOEVT_INVALID_RESPONSE;
            devinfo_handle->event_handler(devinfo_handle, &event, devinfo_handle->userdata);
        }
    }
    devinfo_handle->sysdep->core_sysdep_free(product_key);
    devinfo_handle->sysdep->core_sysdep_free(device_name);
}

static int32_t _devinfo_operate_topic_map(devinfo_handle_t *devinfo_handle, aiot_mqtt_option_t option)
{
    int32_t res = STATE_SUCCESS;
    aiot_mqtt_topic_map_t map;

    if (option == AIOT_MQTTOPT_NETWORK_CRED) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    memset(&map, 0, sizeof(aiot_mqtt_topic_map_t));
    map.topic = DEVINFO_UPDATE_REPLY_TOPIC;
    map.handler = _devinfo_mqtt_recv_handler;
    map.userdata = devinfo_handle;

    res = aiot_mqtt_setopt(devinfo_handle->mqtt_handle, option, &map);
    if (res < STATE_SUCCESS) {
        return res;
    }

    map.topic = DEVINFO_DELETE_REPLY_TOPIC;
    map.handler = _devinfo_mqtt_recv_handler;
    map.userdata = devinfo_handle;

    res = aiot_mqtt_setopt(devinfo_handle->mqtt_handle, option, &map);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static int32_t _devinfo_send(devinfo_handle_t *devinfo_handle, char *product_key, char *device_name, aiot_devinfo_msg_data_t *data, char *topic_fmt)
{
    int32_t res = STATE_SUCCESS, alink_id = 0;
    char *topic = NULL, *payload = NULL;
    char *topic_src[] = { product_key, device_name };
    char alink_id_str[11] = {0}, *payload_src[] = { alink_id_str, data->params };
    char *payload_fmt = "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":%s}";

    res = core_global_alink_id_next(devinfo_handle->sysdep, &alink_id);
    if (res < STATE_SUCCESS) {
        return res;
    }

    res = core_int2str(alink_id, alink_id_str, NULL);
    if (res < STATE_SUCCESS) {
        return res;
    }

    res = core_sprintf(devinfo_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src)/sizeof(char *), DEVINFO_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    res = core_sprintf(devinfo_handle->sysdep, &payload, payload_fmt, payload_src, sizeof(payload_src)/sizeof(char *), DEVINFO_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        devinfo_handle->sysdep->core_sysdep_free(topic);
        return res;
    }

    res = aiot_mqtt_pub(devinfo_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), 0);
    devinfo_handle->sysdep->core_sysdep_free(topic);
    devinfo_handle->sysdep->core_sysdep_free(payload);

    if (res >= STATE_SUCCESS) {
        res = alink_id;
    }

    return res;
}

static void _devinfo_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    devinfo_handle_t *devinfo_handle = (devinfo_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                devinfo_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _devinfo_core_mqtt_operate_process_handler(devinfo_handle_t *devinfo_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _devinfo_core_mqtt_process_handler;
    process_data.context = devinfo_handle;

    return core_mqtt_setopt(devinfo_handle->mqtt_handle, option, &process_data);
}

void *aiot_devinfo_init(void)
{
    int32_t res = STATE_SUCCESS;
    devinfo_handle_t *devinfo_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    res = core_global_init(sysdep);
    if (res < STATE_SUCCESS) {
        return NULL;
    }

    devinfo_handle = sysdep->core_sysdep_malloc(sizeof(devinfo_handle_t), DEVINFO_MODULE_NAME);
    if (devinfo_handle == NULL) {
        core_global_deinit(sysdep);
        return NULL;
    }
    memset(devinfo_handle, 0, sizeof(devinfo_handle_t));

    devinfo_handle->sysdep = sysdep;
    devinfo_handle->deinit_timeout_ms = DEVINFO_DEFAULT_DEINIT_TIMEOUT_MS;

    devinfo_handle->data_mutex = sysdep->core_sysdep_mutex_init();

    devinfo_handle->exec_enabled = 1;

    return devinfo_handle;
}

int32_t aiot_devinfo_setopt(void *handle, aiot_devinfo_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    devinfo_handle_t *devinfo_handle = (devinfo_handle_t *)handle;

    if (handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_DEVINFOOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (devinfo_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_devinfo_exec_inc(devinfo_handle);

    devinfo_handle->sysdep->core_sysdep_mutex_lock(devinfo_handle->data_mutex);
    switch (option) {
        case AIOT_DEVINFOOPT_MQTT_HANDLE: {
            devinfo_handle->mqtt_handle = data;
            devinfo_handle->sysdep->core_sysdep_mutex_unlock(devinfo_handle->data_mutex);
            res = _devinfo_operate_topic_map(devinfo_handle, AIOT_MQTTOPT_APPEND_TOPIC_MAP);
            if (res >= STATE_SUCCESS) {
                res = _devinfo_core_mqtt_operate_process_handler(devinfo_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
            devinfo_handle->sysdep->core_sysdep_mutex_lock(devinfo_handle->data_mutex);
        }
        break;
        case AIOT_DEVINFOOPT_RECV_HANDLER: {
            devinfo_handle->recv_handler = (aiot_devinfo_recv_handler_t)data;
        }
        break;
        case AIOT_DEVINFOOPT_EVENT_HANDLER: {
            devinfo_handle->event_handler = (aiot_devinfo_event_handler_t)data;
        }
        break;
        case AIOT_DEVINFOOPT_USERDATA: {
            devinfo_handle->userdata = data;
        }
        break;
        case AIOT_DEVINFOOPT_DEINIT_TIMEOUT_MS: {
            devinfo_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
    }
    devinfo_handle->sysdep->core_sysdep_mutex_unlock(devinfo_handle->data_mutex);

    _core_devinfo_exec_dec(devinfo_handle);

    return res;
}

int32_t aiot_devinfo_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    devinfo_handle_t *devinfo_handle = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    devinfo_handle = *(devinfo_handle_t **)handle;

    if (devinfo_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    devinfo_handle->exec_enabled = 0;

    _devinfo_core_mqtt_operate_process_handler(devinfo_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);
    _devinfo_operate_topic_map(devinfo_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP);

    deinit_timestart = devinfo_handle->sysdep->core_sysdep_time();
    do {
        if (devinfo_handle->exec_count == 0) {
            break;
        }
        devinfo_handle->sysdep->core_sysdep_sleep(DEVINFO_DEINIT_INTERVAL_MS);
    } while ((devinfo_handle->sysdep->core_sysdep_time() - deinit_timestart) < devinfo_handle->deinit_timeout_ms);

    if (devinfo_handle->exec_count != 0) {
        return STATE_MQTT_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    devinfo_handle->sysdep->core_sysdep_mutex_deinit(&devinfo_handle->data_mutex);

    core_global_deinit(devinfo_handle->sysdep);

    devinfo_handle->sysdep->core_sysdep_free(devinfo_handle);

    return STATE_SUCCESS;
}

int32_t aiot_devinfo_send(void *handle, aiot_devinfo_msg_t *msg)
{
    int32_t res = STATE_SUCCESS;
    devinfo_handle_t *devinfo_handle = (devinfo_handle_t *)handle;

    if (handle == NULL || msg == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (devinfo_handle->mqtt_handle == NULL) {
        return STATE_DEVINFO_MISSING_MQTT_HANDLE;
    }

    if (msg->product_key == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }

    if (msg->device_name == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }

    switch (msg->type) {
        case AIOT_DEVINFO_MSG_UPDATE: {
            if (msg->data.update.params == NULL) {
                res = STATE_USER_INPUT_NULL_POINTER;
            }else{
                res = _devinfo_send(devinfo_handle, msg->product_key, msg->device_name, &msg->data.update, DEVINFO_UPDATE_TOPIC_FMT);
            }
        }
        break;
        case AIOT_DEVINFO_MSG_DELETE: {
            if (msg->data.delete.params == NULL) {
                res = STATE_USER_INPUT_NULL_POINTER;
            }else{
                res = _devinfo_send(devinfo_handle, msg->product_key, msg->device_name, &msg->data.delete, DEVINFO_DELETE_TOPIC_FMT);
            }
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
    }

    return res;
}

