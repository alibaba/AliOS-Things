/**
 * @file aiot_bootstrap_api.c
 * @brief Bootstrap模块的API接口实现, 提供获取阿里云物联网平台连接信息的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "bootstrap_private.h"

static void _bootstrap_exec_inc(bootstrap_handle_t *bootstrap_handle)
{
    bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
    bootstrap_handle->exec_count++;
    bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
}

static void _bootstrap_exec_dec(bootstrap_handle_t *bootstrap_handle)
{
    bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
    bootstrap_handle->exec_count--;
    bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
}

static void _bootstrap_http_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)userdata;

    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            bootstrap_handle->response.code = packet->data.status_code.code;
        }
        break;
        case AIOT_HTTPRECV_HEADER: {
            if ((strlen(packet->data.header.key) == strlen("Content-Length")) &&
                (memcmp(packet->data.header.key, "Content-Length", strlen(packet->data.header.key)) == 0)) {
                core_str2uint(packet->data.header.value, (uint8_t)strlen(packet->data.header.value),
                              &bootstrap_handle->response.content_total_len);
            }
        }
        break;
        case AIOT_HTTPRECV_BODY: {
            uint8_t *content = bootstrap_handle->sysdep->core_sysdep_malloc(bootstrap_handle->response.content_len +
                               packet->data.body.len + 1,
                               CORE_HTTP_MODULE_NAME);
            if (content == NULL) {
                return;
            }
            memset(content, 0, bootstrap_handle->response.content_len + packet->data.body.len + 1);
            if (content != NULL) {
                memcpy(content, bootstrap_handle->response.content, bootstrap_handle->response.content_len);
                bootstrap_handle->sysdep->core_sysdep_free(bootstrap_handle->response.content);
            }
            memcpy(content + bootstrap_handle->response.content_len, packet->data.body.buffer, packet->data.body.len);
            bootstrap_handle->response.content = content;
            bootstrap_handle->response.content_len = bootstrap_handle->response.content_len + packet->data.body.len;
        }
        break;
        default: {

        }
        break;
    }
}

static int32_t _bootstrap_connection_info(bootstrap_handle_t *bootstrap_handle, char **host, uint16_t *port)
{
    int32_t res = STATE_SUCCESS;
    char *tmp_host = NULL, *host_key = "host", *port_key = "port", *ip_key = "ip";
    char *host_value = NULL, *port_value = NULL, *ip_value = NULL;
    uint32_t tmp_port = 0, host_value_len = 0, port_value_len = 0, ip_value_len = 0;

    if (bootstrap_handle->response.code != 200) {
        return STATE_BOOTSTRAP_INVALID_STATUS_CODE;
    }

    if (((res = core_json_value((char *)bootstrap_handle->response.content, bootstrap_handle->response.content_len,
                                host_key, strlen(host_key), &host_value, &host_value_len)) < STATE_SUCCESS) ||
        ((res = core_json_value((char *)bootstrap_handle->response.content, bootstrap_handle->response.content_len,
                                port_key, strlen(port_key), &port_value, &port_value_len)) < STATE_SUCCESS)) {
        return STATE_BOOTSTRAP_INVALID_CONNECTION_INFO;
    }

    tmp_host = bootstrap_handle->sysdep->core_sysdep_malloc(host_value_len + 1, BOOTSTRAP_MODULE_NAME);
    if (tmp_host == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(tmp_host, 0, host_value_len + 1);
    memcpy(tmp_host, host_value, host_value_len);

    core_str2uint(port_value, port_value_len, &tmp_port);

    *host = tmp_host;
    *port = tmp_port;

    /* backup ip address */
    if ((res = core_json_value((char *)bootstrap_handle->response.content, bootstrap_handle->response.content_len,
                                ip_key, strlen(ip_key), &ip_value, &ip_value_len)) >= STATE_SUCCESS &&
        (ip_value_len < 16)) {
        char ip[16] = {0};

        memcpy(ip, ip_value, ip_value_len);
        core_global_set_mqtt_backup_ip(bootstrap_handle->sysdep, ip);
    }

    return STATE_SUCCESS;
}

static int32_t _bootstrap_send_get_request(bootstrap_handle_t *bootstrap_handle)
{
    int32_t res = STATE_SUCCESS;
    char *path = NULL;
    char *path_src[] = { bootstrap_handle->region_id };
    char *path_fmt = BOOTSTRAP_PATH "/regionId/%s";
    core_http_request_t request;

    memset(&request, 0, sizeof(core_http_request_t));
    res = core_sprintf(bootstrap_handle->sysdep, &path, path_fmt, path_src, sizeof(path_src) / sizeof(char *),
                        BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    request.method = "GET";
    request.path = path;
    request.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";
    request.content = NULL;
    request.content_len = 0;

    res = core_http_send(bootstrap_handle->http_handle, &request);
    bootstrap_handle->sysdep->core_sysdep_free(path);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static int32_t _bootstrap_send_post_request(bootstrap_handle_t *bootstrap_handle)
{
    int32_t res = STATE_SUCCESS;
    char *content = NULL;
    char *content_fmt = "productKey=%s&deviceName=%s&clientId=%s.%s&resources=mqtt";
    char *content_src[] = { bootstrap_handle->product_key, bootstrap_handle->device_name, bootstrap_handle->product_key, bootstrap_handle->device_name };
    core_http_request_t request;

    memset(&request, 0, sizeof(core_http_request_t));
    res = core_sprintf(bootstrap_handle->sysdep, &content, content_fmt, content_src, sizeof(content_src) / sizeof(char *),
                        BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }
    request.method = "POST";
    request.path = BOOTSTRAP_PATH;
    request.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n" \
                        "Content-Type: application/x-www-form-urlencoded;charset=utf-8\r\n";
    request.content = (uint8_t *)content;
    request.content_len = (uint32_t)strlen(content);

    res = core_http_send(bootstrap_handle->http_handle, &request);
    bootstrap_handle->sysdep->core_sysdep_free(content);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static void _bootstrap_alink_reply(bootstrap_handle_t *bootstrap_handle, char *id)
{
    int32_t res = STATE_SUCCESS;
    char *topic = NULL, *topic_fmt = BOOTSTRAP_NOTIFY_REPLY_TOPIC_FMT;
    char *topic_src[] = { core_mqtt_get_product_key(bootstrap_handle->mqtt_handle), core_mqtt_get_device_name(bootstrap_handle->mqtt_handle) };
    char *reply = NULL, *reply_fmt = "{\"id\":\"%s\",\"code\":200,\"data\":{}}";
    char *reply_src[] = { id };

    res = core_sprintf(bootstrap_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src)/sizeof(char *), BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return;
    }

    res = core_sprintf(bootstrap_handle->sysdep, &reply, reply_fmt, reply_src, sizeof(reply_src)/sizeof(char *), BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        bootstrap_handle->sysdep->core_sysdep_free(topic);
        return;
    }

    aiot_mqtt_pub(bootstrap_handle->mqtt_handle, topic, (uint8_t *)reply, (uint32_t)strlen(reply), 0);

    bootstrap_handle->sysdep->core_sysdep_free(reply);
    bootstrap_handle->sysdep->core_sysdep_free(topic);
}

static void _bootstrap_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)userdata;

    switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            char *id_key = "id", *cmd_key = "cmd";
            char *id_value = NULL, *cmd_value = NULL;
            uint32_t id_value_len = 0, cmd_value_len = 0;

            if (core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len, id_key, (uint32_t)strlen(id_key),
                                &id_value, &id_value_len) == STATE_SUCCESS &&
                core_json_value((char *)packet->data.pub.payload, packet->data.pub.payload_len, cmd_key, (uint32_t)strlen(cmd_key),
                                &cmd_value, &cmd_value_len) == STATE_SUCCESS) {
                uint32_t cmd = 0;
                if (core_str2uint(cmd_value, cmd_value_len, &cmd) >= STATE_SUCCESS) {
                    if (bootstrap_handle->recv_handler != NULL) {
                        aiot_bootstrap_recv_t recv;

                        memset(&recv, 0, sizeof(aiot_bootstrap_recv_t));
                        recv.type = AIOT_BOOTSTRAPRECV_NOTIFY;
                        recv.data.notify.cmd = cmd;

                        bootstrap_handle->recv_handler(bootstrap_handle, &recv, bootstrap_handle->userdata);
                    }
                    char *id = bootstrap_handle->sysdep->core_sysdep_malloc(id_value_len + 1, BOOTSTRAP_MODULE_NAME);
                    if (id != NULL) {
                        memset(id, 0, id_value_len + 1);
                        memcpy(id, id_value, id_value_len);
                        _bootstrap_alink_reply(bootstrap_handle, id);
                        bootstrap_handle->sysdep->core_sysdep_free(id);
                    }
                } else {
                    if (bootstrap_handle->event_handler != NULL) {
                        aiot_bootstrap_event_t event;

                        memset(&event, 0, sizeof(aiot_bootstrap_event_t));
                        event.type = AIOT_BOOTSTRAPEVT_INVALID_CMD;
                        bootstrap_handle->event_handler(bootstrap_handle, &event, bootstrap_handle->userdata);
                    }
                }
            } else {
                if (bootstrap_handle->event_handler != NULL) {
                    aiot_bootstrap_event_t event;

                    memset(&event, 0, sizeof(aiot_bootstrap_event_t));
                    event.type = AIOT_BOOTSTRAPEVT_INVALID_RESPONSE;
                    bootstrap_handle->event_handler(bootstrap_handle, &event, bootstrap_handle->userdata);
                }
            }
        }
        break;
        default: {

        }
        break;
    }
}

static int32_t _bootstrap_operate_topic_map(bootstrap_handle_t *bootstrap_handle, aiot_mqtt_option_t option)
{
    int32_t res = STATE_SUCCESS;
    aiot_mqtt_topic_map_t map;
    char *topic = NULL;
    char *topic_src[] = { core_mqtt_get_product_key(bootstrap_handle->mqtt_handle), core_mqtt_get_device_name(bootstrap_handle->mqtt_handle) };
    char *topic_fmt = BOOTSTRAP_NOTIFY_TOPIC_FMT;

    memset(&map, 0, sizeof(aiot_mqtt_topic_map_t));

    res = core_sprintf(bootstrap_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src) / sizeof(char *),
                       BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    map.topic = topic;
    map.handler = _bootstrap_recv_handler;
    map.userdata = (void *)bootstrap_handle;

    res = aiot_mqtt_setopt(bootstrap_handle->mqtt_handle, option, &map);
    bootstrap_handle->sysdep->core_sysdep_free(topic);

    return res;
}

static void _bootstrap_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                bootstrap_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _bootstrap_core_mqtt_operate_process_handler(bootstrap_handle_t *bootstrap_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _bootstrap_core_mqtt_process_handler;
    process_data.context = bootstrap_handle;

    return core_mqtt_setopt(bootstrap_handle->mqtt_handle, option, &process_data);
}

void *aiot_bootstrap_init(void)
{
    int32_t res = STATE_SUCCESS;
    bootstrap_handle_t *bootstrap_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    res = core_global_init(sysdep);
    if (res < STATE_SUCCESS) {
        return NULL;
    }

    bootstrap_handle = sysdep->core_sysdep_malloc(sizeof(bootstrap_handle_t), BOOTSTRAP_MODULE_NAME);
    if (bootstrap_handle == NULL) {
        core_global_deinit(sysdep);
        return NULL;
    }
    memset(bootstrap_handle, 0, sizeof(bootstrap_handle_t));

    bootstrap_handle->sysdep = sysdep;
    res = core_strdup(sysdep, &bootstrap_handle->host, BOOTSTRAP_DEFAULT_HOST, BOOTSTRAP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        core_global_deinit(sysdep);
        return NULL;
    }
    bootstrap_handle->port = BOOTSTRAP_DEFAULT_PORT;
    bootstrap_handle->response_body_len = BOOTSTRAP_RESPONSE_BODY_LEN;
    bootstrap_handle->send_timeout_ms = BOOTSTRAP_DEFAULT_SEND_TIMEOUT;
    bootstrap_handle->recv_timeout_ms = BOOTSTRAP_DEFAULT_RECV_TIMEOUT;
    bootstrap_handle->timeout_ms = BOOTSTRAP_DEFAULT_TIMEOUT_MS;
    bootstrap_handle->deinit_timeout_ms = BOOTSTRAP_DEFAULT_DEINIT_TIMEOUT_MS;
    bootstrap_handle->data_mutex = bootstrap_handle->sysdep->core_sysdep_mutex_init();

    bootstrap_handle->exec_enabled = 1;

    return bootstrap_handle;
}

int32_t aiot_bootstrap_setopt(void *handle, aiot_bootstrap_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)handle;

    if (bootstrap_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_BOOTSTRAPOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (bootstrap_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _bootstrap_exec_inc(bootstrap_handle);

    bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
    switch (option) {
        case AIOT_BOOTSTRAPOPT_MQTT_HANDLE: {
            bootstrap_handle->mqtt_handle = data;
            bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
            res = _bootstrap_operate_topic_map(bootstrap_handle, AIOT_MQTTOPT_APPEND_TOPIC_MAP);
            if (res >= STATE_SUCCESS) {
                res = _bootstrap_core_mqtt_operate_process_handler(bootstrap_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
            bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
        }
        break;
        case AIOT_BOOTSTRAPOPT_NETWORK_CRED: {
            if (bootstrap_handle->cred != NULL) {
                bootstrap_handle->sysdep->core_sysdep_free(bootstrap_handle->cred);
                bootstrap_handle->cred = NULL;
            }
            bootstrap_handle->cred = bootstrap_handle->sysdep->core_sysdep_malloc(sizeof(aiot_sysdep_network_cred_t),
                                     BOOTSTRAP_MODULE_NAME);
            if (bootstrap_handle->cred != NULL) {
                memset(bootstrap_handle->cred, 0, sizeof(aiot_sysdep_network_cred_t));
                memcpy(bootstrap_handle->cred, data, sizeof(aiot_sysdep_network_cred_t));
            } else {
                res = STATE_SYS_DEPEND_MALLOC_FAILED;
            }
        }
        break;
        case AIOT_BOOTSTRAPOPT_HOST: {
            res = core_strdup(bootstrap_handle->sysdep, &bootstrap_handle->host, data, BOOTSTRAP_MODULE_NAME);
        }
        break;
        case AIOT_BOOTSTRAPOPT_PORT: {
            bootstrap_handle->port = *(uint16_t *)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_PRODUCT_KEY: {
            res = core_strdup(bootstrap_handle->sysdep, &bootstrap_handle->product_key, data, BOOTSTRAP_MODULE_NAME);
        }
        break;
        case AIOT_BOOTSTRAPOPT_DEVICE_NAME: {
            res = core_strdup(bootstrap_handle->sysdep, &bootstrap_handle->device_name, data, BOOTSTRAP_MODULE_NAME);
        }
        break;
        case AIOT_BOOTSTRAPOPT_REGIONID: {
            res = core_strdup(bootstrap_handle->sysdep, &bootstrap_handle->region_id, data, BOOTSTRAP_MODULE_NAME);
        }
        break;
        case AIOT_BOOTSTRAPOPT_SEND_TIMEOUT_MS: {
            bootstrap_handle->send_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_RECV_TIMEOUT_MS: {
            bootstrap_handle->recv_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_RECV_HANDLER: {
            bootstrap_handle->recv_handler = (aiot_bootstrap_recv_handler_t)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_EVENT_HANDLER: {
            bootstrap_handle->event_handler = (aiot_bootstrap_event_handler_t)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_USERDATA: {
            bootstrap_handle->userdata = data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_TIMEOUT_MS: {
            bootstrap_handle->timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_BOOTSTRAPOPT_DEINIT_TIMEOUT_MS: {
            bootstrap_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);

    _bootstrap_exec_dec(bootstrap_handle);

    return res;
}

int32_t aiot_bootstrap_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    bootstrap_handle_t *bootstrap_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    bootstrap_handle = *(bootstrap_handle_t **)handle;
    sysdep = bootstrap_handle->sysdep;

    if (bootstrap_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    bootstrap_handle->exec_enabled = 0;

    _bootstrap_core_mqtt_operate_process_handler(bootstrap_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);
    _bootstrap_operate_topic_map(bootstrap_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP);

    deinit_timestart = bootstrap_handle->sysdep->core_sysdep_time();
    do {
        if (bootstrap_handle->exec_count == 0) {
            break;
        }
        bootstrap_handle->sysdep->core_sysdep_sleep(BOOTSTRAP_DEINIT_INTERVAL_MS);
    } while ((bootstrap_handle->sysdep->core_sysdep_time() - deinit_timestart) < bootstrap_handle->deinit_timeout_ms);

    if (bootstrap_handle->exec_count != 0) {
        return STATE_BOOTSTRAP_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    if (bootstrap_handle->response.content != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->response.content);
    }
    memset(&bootstrap_handle->response, 0, sizeof(core_http_response_t));

    if (bootstrap_handle->http_handle != NULL) {
        core_http_deinit(&bootstrap_handle->http_handle);
    }

    if (bootstrap_handle->host != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->host);
    }

    if (bootstrap_handle->product_key != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->product_key);
    }

    if (bootstrap_handle->device_name != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->device_name);
    }

    if (bootstrap_handle->region_id != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->region_id);
    }

    if (bootstrap_handle->cred != NULL) {
        sysdep->core_sysdep_free(bootstrap_handle->cred);
    }

    sysdep->core_sysdep_mutex_deinit(&bootstrap_handle->data_mutex);

    core_global_deinit(sysdep);

    sysdep->core_sysdep_free(bootstrap_handle);

    return STATE_SUCCESS;
}

int32_t aiot_bootstrap_send_request(void *handle)
{
    int32_t res = STATE_SUCCESS;
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)handle;

    if (bootstrap_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (bootstrap_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    if (bootstrap_handle->product_key == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }

    if (bootstrap_handle->device_name == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }

    if (bootstrap_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _bootstrap_exec_inc(bootstrap_handle);

    bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
    if (bootstrap_handle->response.content != NULL) {
        bootstrap_handle->sysdep->core_sysdep_free(bootstrap_handle->response.content);
    }
    memset(&bootstrap_handle->response, 0, sizeof(core_http_response_t));

    if (bootstrap_handle->http_handle != NULL) {
        core_http_deinit(&bootstrap_handle->http_handle);
    }

    bootstrap_handle->http_handle = core_http_init();
    if (bootstrap_handle->http_handle == NULL) {
        bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
        _bootstrap_exec_dec(bootstrap_handle);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    if (((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_HOST,
                                 (void *)bootstrap_handle->host)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_PORT,
                                 (void *)&bootstrap_handle->port)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_NETWORK_CRED,
                                 (void *)bootstrap_handle->cred)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_SEND_TIMEOUT_MS,
                                 (void *)&bootstrap_handle->send_timeout_ms)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_RECV_TIMEOUT_MS,
                                 (void *)&bootstrap_handle->recv_timeout_ms)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_BODY_BUFFER_MAX_LEN,
                                 (void *)&bootstrap_handle->response_body_len)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_RECV_HANDLER,
                                 (void *)_bootstrap_http_recv_handler)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(bootstrap_handle->http_handle, CORE_HTTPOPT_USERDATA,
                                 (void *)bootstrap_handle)) < STATE_SUCCESS)) {
        core_http_deinit(&bootstrap_handle->http_handle);
        bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
        _bootstrap_exec_dec(bootstrap_handle);
        return res;
    }

    res = core_http_connect(bootstrap_handle->http_handle);
    if (res < STATE_SUCCESS) {
        core_http_deinit(&bootstrap_handle->http_handle);
        bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
        _bootstrap_exec_dec(bootstrap_handle);
        return res;
    }

    {
        if (bootstrap_handle->region_id != NULL) {
            res = _bootstrap_send_get_request(bootstrap_handle);
        }else{
            res = _bootstrap_send_post_request(bootstrap_handle);
        }
        if (res < STATE_SUCCESS) {
            core_http_deinit(&bootstrap_handle->http_handle);
            bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
            _bootstrap_exec_dec(bootstrap_handle);
            return res;
        }
    }
    bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
    _bootstrap_exec_dec(bootstrap_handle);

    return res;
}

int32_t aiot_bootstrap_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint64_t timenow_ms = 0;
    bootstrap_handle_t *bootstrap_handle = (bootstrap_handle_t *)handle;
    char *host = NULL;
    uint16_t port = 0;
    aiot_bootstrap_recv_t packet;

    if (bootstrap_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (bootstrap_handle->http_handle == NULL) {
        return STATE_BOOTSTRAP_NEED_SEND_REQUEST;
    }

    if (bootstrap_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _bootstrap_exec_inc(bootstrap_handle);

    bootstrap_handle->sysdep->core_sysdep_mutex_lock(bootstrap_handle->data_mutex);
    timenow_ms = bootstrap_handle->sysdep->core_sysdep_time();
    while (1) {
        if (timenow_ms >= bootstrap_handle->sysdep->core_sysdep_time()) {
            timenow_ms = bootstrap_handle->sysdep->core_sysdep_time();
        }
        if (bootstrap_handle->sysdep->core_sysdep_time() - timenow_ms >= bootstrap_handle->timeout_ms) {
            break;
        }

        res = core_http_recv(bootstrap_handle->http_handle);
        if (res < STATE_SUCCESS) {
            break;
        }
    }

    if (res < STATE_SUCCESS) {
        if (res != STATE_HTTP_READ_BODY_FINISHED) {
            if (bootstrap_handle->response.content != NULL) {
                bootstrap_handle->sysdep->core_sysdep_free(bootstrap_handle->response.content);
                memset(&bootstrap_handle->response, 0, sizeof(core_http_response_t));
            }
            bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
            _bootstrap_exec_dec(bootstrap_handle);
            return res;
        } else {
            res = STATE_SUCCESS;
        }
    } else {
        bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
        _bootstrap_exec_dec(bootstrap_handle);
        return STATE_HTTP_RECV_NOT_FINISHED;
    }

    if (bootstrap_handle->recv_handler != NULL) {
        packet.type = AIOT_BOOTSTRAPRECV_STATUS_CODE;
        packet.data.status_code.code = bootstrap_handle->response.code;

        bootstrap_handle->recv_handler(bootstrap_handle, &packet, bootstrap_handle->userdata);
    }

    res = _bootstrap_connection_info(bootstrap_handle, &host, &port);
    bootstrap_handle->sysdep->core_sysdep_mutex_unlock(bootstrap_handle->data_mutex);
    if (res < STATE_SUCCESS) {
        _bootstrap_exec_dec(bootstrap_handle);
        return res;
    }

    memset(&packet, 0, sizeof(aiot_bootstrap_recv_t));
    if (bootstrap_handle->recv_handler != NULL) {
        packet.type = AIOT_BOOTSTRAPRECV_CONNECTION_INFO;
        packet.data.connection_info.host = host;
        packet.data.connection_info.port = (uint16_t)port;

        bootstrap_handle->recv_handler(bootstrap_handle, &packet, bootstrap_handle->userdata);
    }
    bootstrap_handle->sysdep->core_sysdep_free(host);

    _bootstrap_exec_dec(bootstrap_handle);

    return STATE_SUCCESS;
}

