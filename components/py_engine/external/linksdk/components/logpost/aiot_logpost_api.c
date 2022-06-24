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

    core_log(logpost_handle->sysdep, STATE_LOGPOST_LOG_RECV, "LOGPOST user log config arrived\r\n");

    if ((res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                               LOGPOST_JSON_KEY_MODE, strlen(LOGPOST_JSON_KEY_MODE), &value, &value_len)) < 0 ||
        (res = core_str2uint(value, value_len, &log_switch)) < 0) {

        core_log(logpost_handle->sysdep, SATAE_LOGPOST_LOG_PARSE_MSG_FAILED, "LOGPOST parse log config failed\r\n");
        return;
    }

    /* update log config */
    logpost_handle->user_log_switch = log_switch;

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

int32_t _logpost_send_nwkstats_rtt(logpost_handle_t *handle)
{
    int32_t res = STATE_SUCCESS;
    aiot_logpost_msg_t msg;
    core_mqtt_nwkstats_info_t nwkstats_info;
    char *content_fmt = NWKSTAT_RTT_INFO_FMT;
    char timestamp_str[22] = {0};
    char rtt_str[22] = {0};
    char *content = NULL;
    char *content_src[] = {timestamp_str, rtt_str};

    core_mqtt_get_nwkstats(handle->mqtt_handle, &nwkstats_info);
    core_uint642str(nwkstats_info.rtt, rtt_str, NULL);
    core_uint642str(handle->sysdep->core_sysdep_time(), timestamp_str, NULL);
    res = core_sprintf(handle->sysdep, &content, content_fmt, content_src, 2, LOGPOST_MODULE_NAME);
    if (res < 0) {
        return res;
    }

    memset(&msg, 0, sizeof(aiot_logpost_msg_t));
    msg.timestamp = 0;                          /* 单位为ms的时间戳, 填写0则SDK将使用当前的时间戳 */
    msg.loglevel = AIOT_LOGPOST_LEVEL_INFO;     /* 日志级别 */
    msg.module_name = NWKSTAT_NET_RT;           /* 日志对应的模块 */
    msg.code = 200;                             /* 状态码 */
    msg.msg_id = 0;
    msg.content = content;                      /* 日志内容 */

    res = aiot_logpost_send(handle, &msg);
    handle->sysdep->core_sysdep_free(content);

    return res;
}

int32_t _logpost_send_nwkstats_conn(logpost_handle_t *handle)
{
    int32_t res = STATE_SUCCESS;
    aiot_logpost_msg_t msg;
    core_mqtt_nwkstats_info_t nwkstats_info;
    char *content_fmt = NULL;
    char success_time[22] = {0};
    char conn_time_str[22] = {0};
    char failed_time[22] = {0};
    char conn_code[12] = {0};
    char *content = NULL;
    char *content_src[] = {success_time, NULL, conn_time_str, failed_time, NULL, conn_code};
    uint8_t content_src_cnt = 0;

    core_mqtt_get_nwkstats(handle->mqtt_handle, &nwkstats_info);
    if (nwkstats_info.connect_time_used == 0) {
        return res;
    }
    core_uint642str(nwkstats_info.connect_timestamp, success_time, NULL);
    content_src[1] = (nwkstats_info.network_type == 0) ? "TCP" : "TLS";
    content_src[4] = (nwkstats_info.network_type == 0) ? "TCP" : "TLS";
    core_uint642str(nwkstats_info.connect_time_used, conn_time_str, NULL);
    core_uint642str(nwkstats_info.failed_timestamp, failed_time, NULL);
    core_int2str(nwkstats_info.failed_error_code, conn_code, NULL);

    /* check if connect failure happened */
    if (nwkstats_info.failed_error_code != 0) {
        content_fmt = NWKSTAT_CONN_INFO_FMT2;
        content_src_cnt = sizeof(content_src) / sizeof(content_src[0]);
    } else {
        content_fmt = NWKSTAT_CONN_INFO_FMT;
        content_src_cnt = 3;
    }

    res = core_sprintf(handle->sysdep, &content, content_fmt, content_src, content_src_cnt, LOGPOST_MODULE_NAME);
    if (res < 0) {
        return res;
    }

    memset(&msg, 0, sizeof(aiot_logpost_msg_t));
    msg.timestamp = 0;                          /* 单位为ms的时间戳, 填写0则SDK将使用当前的时间戳 */
    msg.loglevel = AIOT_LOGPOST_LEVEL_INFO;     /* 日志级别 */
    msg.module_name = NWKSTAT_NET_CONN;         /* 日志对应的模块 */
    msg.code = 200;                             /* 状态码 */
    msg.msg_id = 0;
    msg.content = content;                      /* 日志内容 */

    res = aiot_logpost_send(handle, &msg);
    handle->sysdep->core_sysdep_free(content);

    return res;
}

int32_t _should_report_sys_log(logpost_handle_t *logpost_handle, char *module_name)
{
    int result = 0;
    if (0 == logpost_handle->sys_log_switch) {
        return result;
    }
    if (0 == memcmp(NWKSTAT_NET_CONN, module_name, strlen(NWKSTAT_NET_CONN)) ||
        0 == memcmp(NWKSTAT_NET_RT, module_name, strlen(NWKSTAT_NET_RT))) {
        result = 1;
        core_log(logpost_handle->sysdep, STATE_LOGPOST_LOG_RECV,
                 "sys log config is on, toggle it using AIOT_LOGPOSTOPT_SYS_LOG.\r\n");
    }

    return result;
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
        if (logpost_handle->sys_log_switch == 0) {
            return;
        }
        _logpost_send_nwkstats_conn(logpost_handle);

        if ((logpost_handle->sysdep->core_sysdep_time() - logpost_handle->last_post_time) \
            > LOGPOST_NWKSTATS_POST_INTERVAL) {
            logpost_handle->last_post_time = logpost_handle->sysdep->core_sysdep_time();

            _logpost_send_nwkstats_rtt(logpost_handle);
        }

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
    logpost_handle->user_log_switch = LOGPOST_DEFAULT_LOG_ONOFF;
    logpost_handle->sys_log_switch = LOGPOST_DEFAULT_LOG_ONOFF;

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
        case AIOT_LOGPOSTOPT_SYS_LOG: {
            logpost_handle->sys_log_switch = *(uint8_t *)data;
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
    if (1 != _should_report_sys_log(logpost_handle, msg->module_name) && logpost_handle->user_log_switch == 0) {
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
        } else {
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

