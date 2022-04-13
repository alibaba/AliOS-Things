/**
 * @file aiot_logpost_api.c
 * @brief logpost模块的API接口实现, 提供设备端日志上云的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "logpost_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_mqtt.h"
#include "core_global.h"
#include "aiot_mqtt_api.h"


#define LOGPOST_JSON_KEY_MODE           "mode"

const char *logpost_loglevel[] = {
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
};

static void _logpost_config_data_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata)
{
    logpost_handle_t *logpost_handle = (logpost_handle_t *)userdata;
    aiot_logpost_event_t event;
    char *value = NULL;
    uint32_t value_len = 0;
    uint32_t log_switch = 0;
    int32_t res = STATE_SUCCESS;

    /* construct event message */
    memset(&event, 0, sizeof(aiot_logpost_event_t));
    event.type = AIOT_LOGPOSTEVT_CONFIG_DATA;

    core_log(logpost_handle->sysdep, STATE_LOGPOST_LOG_RECV, "LOGPOST log config arrived\r\n");

    if ((res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                            LOGPOST_JSON_KEY_MODE, strlen(LOGPOST_JSON_KEY_MODE), &value, &value_len)) < 0 ||
        (res = core_str2uint(value, value_len, &log_switch)) < 0) {

        core_log(logpost_handle->sysdep, SATAE_LOGPOST_LOG_PARSE_MSG_FAILED, "LOGPOST parse log config failed\r\n");
        return;
    }

    /* update log config */
    logpost_handle->on_off = log_switch;

    /* invoke user callback */
    if (logpost_handle->event_handler != NULL) {
        event.data.config_data.on_off = log_switch;
        logpost_handle->event_handler(logpost_handle, &event, logpost_handle->userdata);
    }
}

static void _logpost_get_config(logpost_handle_t *logpost_handle)
{
    char *topic = NULL;
    char *payload = NULL;
    char *fmt = "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":{\"getType\":\"content\",\"configScope\":\"device\"}}";
    int32_t res = STATE_SUCCESS;
    int32_t id = 0;
    char *pk = NULL, *dn = NULL;
    char id_string[11] = { 0 };

    if (NULL == logpost_handle || NULL == logpost_handle->mqtt_handle) {
        return;
    }
    if (NULL == (pk = core_mqtt_get_product_key(logpost_handle->mqtt_handle))) {
        return;
    }
    if (NULL == (dn = core_mqtt_get_device_name(logpost_handle->mqtt_handle))) {
        return;
    }

    core_global_alink_id_next(logpost_handle->sysdep, &id);
    core_int2str(id, id_string, NULL);

    /* construct topic */
    {
        char *src[2] = { pk, dn };
        res = core_sprintf(logpost_handle->sysdep, &topic, LOGPOST_CONFIG_GET_TOPIC_FMT, src, sizeof(src) / sizeof(char *),
                           LOGPOST_MODULE_NAME);
        if (NULL == topic) {
            return;
        }
    }

    /* construct payload */
    {
        char *src[1] = { id_string };
        res = core_sprintf(logpost_handle->sysdep, &payload, fmt,
                           src, sizeof(src) / sizeof(char *), LOGPOST_MODULE_NAME);
        if (res < STATE_SUCCESS) {
            logpost_handle->sysdep->core_sysdep_free(topic);
            return;
        }
    }

    res = aiot_mqtt_pub(logpost_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), 0);
    logpost_handle->sysdep->core_sysdep_free(topic);
    logpost_handle->sysdep->core_sysdep_free(payload);
}

void _logpost_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    logpost_handle_t *logpost_handle = (logpost_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                logpost_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }

    if (NULL == context || NULL == event) {
        return;
    }

    if (event->type == AIOT_MQTTEVT_CONNECT) {
        _logpost_get_config(logpost_handle);
    }
}

void *aiot_logpost_init(void)
{
    aiot_sysdep_portfile_t *sysdep = aiot_sysdep_get_portfile();
    logpost_handle_t *logpost_handle = NULL;

    if (NULL == sysdep) {
        return NULL;
    }

    logpost_handle = sysdep->core_sysdep_malloc(sizeof(logpost_handle_t), LOGPOST_MODULE_NAME);
    if (NULL == logpost_handle) {
        return NULL;
    }

    memset(logpost_handle, 0, sizeof(logpost_handle_t));
    logpost_handle->sysdep = sysdep;
    logpost_handle->on_off = LOGPOST_DEFAULT_LOG_ONOFF;

    core_global_init(sysdep);
    return logpost_handle;
}

int32_t aiot_logpost_setopt(void *handle, aiot_logpost_option_t option, void *data)
{
    logpost_handle_t *logpost_handle;
    int32_t res = STATE_SUCCESS;

    if (NULL == handle || NULL == data) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (option >= AIOT_LOGPOSTOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    logpost_handle = (logpost_handle_t *)handle;

    switch (option) {
        case AIOT_LOGPOSTOPT_MQTT_HANDLE: {
            aiot_mqtt_topic_map_t topic_mapping;
            logpost_handle->mqtt_handle = data;
            core_mqtt_process_data_t process_data;

            /* setup mqtt topic mapping */
            memset(&topic_mapping, 0, sizeof(aiot_mqtt_topic_map_t));
            topic_mapping.topic = LOGPOST_CONFIG_PUSH_TOPIC;
            topic_mapping.handler = _logpost_config_data_handler;
            topic_mapping.userdata = handle;
            res = aiot_mqtt_setopt(data, AIOT_MQTTOPT_APPEND_TOPIC_MAP, &topic_mapping);
            if (res < 0) {
                break;
            }

            topic_mapping.topic = LOGPOST_CONFIG_GET_REPLY_TOPIC;
            res = aiot_mqtt_setopt(data, AIOT_MQTTOPT_APPEND_TOPIC_MAP, &topic_mapping);
            if (res < 0) {
                break;
            }

            /* setup mqtt process handler */
            memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
            process_data.handler = _logpost_process_handler;
            process_data.context = logpost_handle;
            res = core_mqtt_setopt(data, CORE_MQTTOPT_APPEND_PROCESS_HANDLER, &process_data);
        }
        break;
        case AIOT_LOGPOSTOPT_EVENT_HANDLER: {
            logpost_handle->event_handler = (aiot_logpost_event_handler_t)data;
        }
        break;
        case AIOT_LOGPOSTOPT_USERDATA: {
            logpost_handle->userdata = data;
        }
        break;
        default:
            break;
    }

    return res;
}

int32_t aiot_logpost_send(void *handle, aiot_logpost_msg_t *msg)
{
    logpost_handle_t *logpost_handle = NULL;
    char *topic = NULL;
    char *payload = NULL;
    char *fmt = "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":" \
                "[{\"utcTime\":\"%s\",\"logLevel\":\"%s\",\"module\":\"%s\",\"code\":\"%s\",\"traceContext\":\"%s\",\"logContent\":\"%s\"}]}";
    char *pk = NULL, *dn = NULL;
    int32_t id = 0;
    int32_t res = STATE_SUCCESS;

    if (NULL == handle || NULL == msg) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (msg->loglevel > AIOT_LOGPOST_LEVEL_DEBUG) {
        return STATE_LOGPOST_LOGLEVEL_ERROR;
    }
    if (NULL == msg->module_name) {
        return STATE_LOGPOST_LOG_MODULE_NAME_IS_NULL;
    }
    if (NULL == msg->content) {
        return STATE_LOGPOST_LOG_CONTENT_IS_NULL;
    }
    if (strlen(msg->content) > LOGPOST_CONTENT_MAXIMUM_LEN) {
        return STATE_LOGPOST_LOG_CONTENT_TOO_LONG;
    }
    if (msg->timestamp > 100000000000000) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    logpost_handle = (logpost_handle_t *)handle;
    if (NULL == logpost_handle->mqtt_handle) {
        return STATE_LOGPOST_MQTT_HANDLE_IS_NULL;
    }
    if (NULL == (pk = core_mqtt_get_product_key(logpost_handle->mqtt_handle))) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }
    if (NULL == (dn = core_mqtt_get_device_name(logpost_handle->mqtt_handle))) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }
    if (logpost_handle->on_off == 0) {
        return STATE_LOGPOST_POST_TURN_OFF;
    }

    /* construct topic */
    {
        char *src[2] = {pk, dn};
        res = core_sprintf(logpost_handle->sysdep, &topic, LOGPOST_POST_TOPIC_FMT, src, sizeof(src) / sizeof(char *),
                           LOGPOST_MODULE_NAME);
        if (NULL == topic) {
            return res;
        }
    }

    /* construct payload */
    {
        char id_string[11] = { 0 };
        char utc[32] = { 0 };
        char code[11] = { 0 };
        char msg_id[22] = { 0 };
        const char *src[] = { id_string, utc, logpost_loglevel[msg->loglevel], msg->module_name, code, msg_id, msg->content };

        core_global_alink_id_next(logpost_handle->sysdep, &id);
        core_int2str(id, id_string, NULL);

        if (msg->timestamp == 0) {
            _core_log_append_date(logpost_handle->sysdep, core_log_get_timestamp(logpost_handle->sysdep), utc);
        }
        else {
            _core_log_append_date(logpost_handle->sysdep, msg->timestamp, utc);
        }
        core_int2str(msg->code, code, NULL);
        core_uint642str(msg->msg_id, msg_id, NULL);

        res = core_sprintf(logpost_handle->sysdep, &payload, fmt,
                           (char **)src, sizeof(src) / sizeof(char *), LOGPOST_MODULE_NAME);
        if (res < STATE_SUCCESS) {
            logpost_handle->sysdep->core_sysdep_free(topic);
            return res;
        }
    }

    res = aiot_mqtt_pub(logpost_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), 0);
    logpost_handle->sysdep->core_sysdep_free(topic);
    logpost_handle->sysdep->core_sysdep_free(payload);

    if (res >= STATE_SUCCESS) {
        res = id;
    }
    return res;
}

int32_t aiot_logpost_deinit(void **p_handle)
{
    logpost_handle_t *logpost_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    aiot_mqtt_topic_map_t topic_mapping;
    core_mqtt_process_data_t process_data;

    if (NULL == p_handle || NULL == *p_handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    logpost_handle = *p_handle;
    sysdep = logpost_handle->sysdep;
    *p_handle = NULL;

    /* remove mqtt precess handler */
    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _logpost_process_handler;
    core_mqtt_setopt(logpost_handle->mqtt_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER, &process_data);

    /* remove mqtt topic mapping */
    memset(&topic_mapping, 0, sizeof(aiot_mqtt_topic_map_t));
    topic_mapping.topic = LOGPOST_CONFIG_PUSH_TOPIC;
    topic_mapping.handler = _logpost_config_data_handler;
    aiot_mqtt_setopt(logpost_handle->mqtt_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP, &topic_mapping);
    topic_mapping.topic = LOGPOST_CONFIG_GET_REPLY_TOPIC;
    aiot_mqtt_setopt(logpost_handle->mqtt_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP, &topic_mapping);

    sysdep->core_sysdep_free(logpost_handle);

    core_global_deinit(sysdep);
    return STATE_SUCCESS;
}

