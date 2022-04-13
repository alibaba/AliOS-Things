/**
 * @file aiot_mqtt_api.c
 * @brief MQTT模块实现, 其中包含了连接到物联网平台和收发数据的API接口
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#include "core_mqtt.h"

static int32_t _core_mqtt_sysdep_return(int32_t sysdep_code, int32_t core_code)
{
    if (sysdep_code >= (STATE_PORT_BASE - 0x00FF) && sysdep_code < (STATE_PORT_BASE)) {
        return sysdep_code;
    } else {
        return core_code;
    }
}

static void _core_mqtt_event_notify_process_handler(core_mqtt_handle_t *mqtt_handle, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    core_mqtt_process_data_node_t *node = NULL;

    core_list_for_each_entry(node, &mqtt_handle->process_data_list,
                             linked_node, core_mqtt_process_data_node_t) {
        node->process_data.handler(node->process_data.context, event, core_event);
    }
}

static void _core_mqtt_event_notify(core_mqtt_handle_t *mqtt_handle, aiot_mqtt_event_type_t type)
{
    aiot_mqtt_event_t event;
    memset(&event, 0, sizeof(aiot_mqtt_event_t));
    event.type = type;

    if (mqtt_handle->event_handler) {
        mqtt_handle->event_handler((void *)mqtt_handle, &event, mqtt_handle->userdata);
    }

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->process_handler_mutex);
    _core_mqtt_event_notify_process_handler(mqtt_handle, &event, NULL);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->process_handler_mutex);
}

static void _core_mqtt_connect_event_notify(core_mqtt_handle_t *mqtt_handle)
{
    mqtt_handle->disconnected = 0;
    if (mqtt_handle->has_connected == 0) {
        mqtt_handle->has_connected = 1;
        _core_mqtt_event_notify(mqtt_handle, AIOT_MQTTEVT_CONNECT);
    } else {
        _core_mqtt_event_notify(mqtt_handle, AIOT_MQTTEVT_RECONNECT);
    }
}

static void _core_mqtt_disconnect_event_notify(core_mqtt_handle_t *mqtt_handle,
        aiot_mqtt_disconnect_event_type_t disconnect)
{
    if (mqtt_handle->has_connected == 1 && mqtt_handle->disconnected == 0) {
        aiot_mqtt_event_t event;

        mqtt_handle->disconnected = 1;

        memset(&event, 0, sizeof(aiot_mqtt_event_t));
        event.type = AIOT_MQTTEVT_DISCONNECT;
        event.data.disconnect = disconnect;

        if (mqtt_handle->event_handler) {
            mqtt_handle->event_handler((void *)mqtt_handle, &event, mqtt_handle->userdata);
        }

        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->process_handler_mutex);
        _core_mqtt_event_notify_process_handler(mqtt_handle, &event, NULL);
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->process_handler_mutex);
    }
}

static void _core_mqtt_exec_inc(core_mqtt_handle_t *mqtt_handle)
{
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    mqtt_handle->exec_count++;
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
}

static void _core_mqtt_exec_dec(core_mqtt_handle_t *mqtt_handle)
{
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    mqtt_handle->exec_count--;
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
}

static void _core_mqtt_sign_clean(core_mqtt_handle_t *mqtt_handle)
{
    if (mqtt_handle->username) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->username);
        mqtt_handle->username = NULL;
    }
    if (mqtt_handle->password) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->password);
        mqtt_handle->password = NULL;
    }
    if (mqtt_handle->clientid) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->clientid);
        mqtt_handle->clientid = NULL;
    }
}

static int32_t _core_mqtt_handlerlist_insert(core_mqtt_handle_t *mqtt_handle, core_mqtt_sub_node_t *sub_node,
        aiot_mqtt_recv_handler_t handler, void *userdata)
{
    core_mqtt_sub_handler_node_t *node = NULL;

    core_list_for_each_entry(node, &sub_node->handle_list, linked_node, core_mqtt_sub_handler_node_t) {
        if (node->handler == handler) {
            /* exist handler, replace userdata */
            node->userdata = userdata;
            return STATE_SUCCESS;
        }
    }

    if (&node->linked_node == &sub_node->handle_list) {
        /* new handler */
        node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_sub_handler_node_t), CORE_MQTT_MODULE_NAME);
        if (node == NULL) {
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node, 0, sizeof(core_mqtt_sub_handler_node_t));
        CORE_INIT_LIST_HEAD(&node->linked_node);
        node->handler = handler;
        node->userdata = userdata;

        core_list_add_tail(&node->linked_node, &sub_node->handle_list);
    }

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_sublist_insert(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic,
        aiot_mqtt_recv_handler_t handler, void *userdata)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_sub_node_t *node = NULL;

    core_list_for_each_entry(node, &mqtt_handle->sub_list, linked_node, core_mqtt_sub_node_t) {
        if ((strlen(node->topic) == topic->len) && memcmp(node->topic, topic->buffer, topic->len) == 0) {
            /* exist topic */
            if (handler != NULL) {
                return _core_mqtt_handlerlist_insert(mqtt_handle, node, handler, userdata);
            } else {
                return STATE_SUCCESS;
            }
        }
    }

    if (&node->linked_node == &mqtt_handle->sub_list) {
        /* new topic */
        node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_sub_node_t), CORE_MQTT_MODULE_NAME);
        if (node == NULL) {
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node, 0, sizeof(core_mqtt_sub_node_t));
        CORE_INIT_LIST_HEAD(&node->linked_node);
        CORE_INIT_LIST_HEAD(&node->handle_list);

        node->topic = mqtt_handle->sysdep->core_sysdep_malloc(topic->len + 1, CORE_MQTT_MODULE_NAME);
        if (node->topic == NULL) {
            mqtt_handle->sysdep->core_sysdep_free(node);
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node->topic, 0, topic->len + 1);
        memcpy(node->topic, topic->buffer, topic->len);

        if (handler != NULL) {
            res = _core_mqtt_handlerlist_insert(mqtt_handle, node, handler, userdata);
            if (res < STATE_SUCCESS) {
                mqtt_handle->sysdep->core_sysdep_free(node->topic);
                mqtt_handle->sysdep->core_sysdep_free(node);
                return res;
            }
        }

        core_list_add_tail(&node->linked_node, &mqtt_handle->sub_list);
    }
    return res;
}

static void _core_mqtt_sublist_handlerlist_destroy(core_mqtt_handle_t *mqtt_handle, struct core_list_head *list)
{
    core_mqtt_sub_handler_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, list, linked_node, core_mqtt_sub_handler_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
}

static void _core_mqtt_sublist_remove(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic)
{
    core_mqtt_sub_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->sub_list, linked_node, core_mqtt_sub_node_t) {
        if ((strlen(node->topic) == topic->len) && memcmp(node->topic, topic->buffer, topic->len) == 0) {
            core_list_del(&node->linked_node);
            _core_mqtt_sublist_handlerlist_destroy(mqtt_handle, &node->handle_list);
            mqtt_handle->sysdep->core_sysdep_free(node->topic);
            mqtt_handle->sysdep->core_sysdep_free(node);
        }
    }
}

static void _core_mqtt_sublist_remove_handler(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic, aiot_mqtt_recv_handler_t handler)
{
    core_mqtt_sub_node_t *node = NULL;
    core_mqtt_sub_handler_node_t *handler_node = NULL, *handler_next = NULL;

    core_list_for_each_entry(node, &mqtt_handle->sub_list, linked_node, core_mqtt_sub_node_t) {
        if ((strlen(node->topic) == topic->len) && memcmp(node->topic, topic->buffer, topic->len) == 0) {
            core_list_for_each_entry_safe(handler_node, handler_next, &node->handle_list,
                                          linked_node, core_mqtt_sub_handler_node_t) {
                if (handler_node->handler == handler) {
                    core_list_del(&handler_node->linked_node);
                    mqtt_handle->sysdep->core_sysdep_free(handler_node);
                }
            }
        }
    }
}

static void _core_mqtt_sublist_destroy(core_mqtt_handle_t *mqtt_handle)
{
    core_mqtt_sub_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->sub_list, linked_node, core_mqtt_sub_node_t) {
        core_list_del(&node->linked_node);
        _core_mqtt_sublist_handlerlist_destroy(mqtt_handle, &node->handle_list);
        mqtt_handle->sysdep->core_sysdep_free(node->topic);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
}

static int32_t _core_mqtt_topic_is_valid(char *topic, uint32_t len)
{
    uint32_t idx = 0;

    /* topic should start with '/' */
    if (topic[0] != '/') {
        return STATE_MQTT_TOPIC_INVALID;
    }

    for (idx = 0; idx < len; idx++) {
        if (topic[idx] == '+') {
            /* topic should contain '/+/' in the middle or '/+' in the end */
            if ((topic[idx - 1] != '/') ||
                ((idx + 1 < len) && (topic[idx + 1] != '/'))) {
                return STATE_MQTT_TOPIC_INVALID;
            }
        }
        if (topic[idx] == '#') {
            /* topic should contain '/#' in the end */
            if ((topic[idx - 1] != '/') ||
                (idx + 1 < len)) {
                return STATE_MQTT_TOPIC_INVALID;
            }
        }
    }

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_append_topic_map(core_mqtt_handle_t *mqtt_handle, aiot_mqtt_topic_map_t *map)
{
    int32_t             res = STATE_SUCCESS;
    core_mqtt_buff_t    topic_buff;

    if (map->topic == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (strlen(map->topic) >= CORE_MQTT_TOPIC_MAXLEN) {
        return STATE_MQTT_TOPIC_TOO_LONG;
    }
    if ((res = _core_mqtt_topic_is_valid((char *)map->topic, strlen((char *)map->topic))) < STATE_SUCCESS) {
        return res;
    }

    memset(&topic_buff, 0, sizeof(topic_buff));
    topic_buff.buffer = (uint8_t *)map->topic;
    topic_buff.len = strlen(map->topic);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->sub_mutex);
    res = _core_mqtt_sublist_insert(mqtt_handle, &topic_buff, map->handler, map->userdata);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->sub_mutex);

    return res;
}

static int32_t _core_mqtt_remove_topic_map(core_mqtt_handle_t *mqtt_handle, aiot_mqtt_topic_map_t *map)
{
    int32_t             res = STATE_SUCCESS;
    core_mqtt_buff_t    topic_buff;

    if (map->topic == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (strlen(map->topic) >= CORE_MQTT_TOPIC_MAXLEN) {
        return STATE_MQTT_TOPIC_TOO_LONG;
    }
    if ((res = _core_mqtt_topic_is_valid((char *)map->topic, strlen((char *)map->topic))) < STATE_SUCCESS) {
        return res;
    }

    memset(&topic_buff, 0, sizeof(topic_buff));
    topic_buff.buffer = (uint8_t *)map->topic;
    topic_buff.len = strlen(map->topic);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->sub_mutex);
    _core_mqtt_sublist_remove_handler(mqtt_handle, &topic_buff, map->handler);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->sub_mutex);

    return STATE_SUCCESS;
}

static void _core_mqtt_set_utf8_encoded_str(uint8_t *input, uint16_t input_len, uint8_t *output)
{
    uint32_t idx = 0, input_idx = 0;

    /* String Length MSB */
    output[idx++] = (uint8_t)((input_len >> 8) & 0x00FF);

    /* String Length LSB */
    output[idx++] = (uint8_t)((input_len) & 0x00FF);

    /* UTF-8 Encoded Character Data */
    for (input_idx = 0; input_idx < input_len; input_idx++) {
        output[idx++] = input[input_idx];
    }
}

static void _core_mqtt_remain_len_encode(uint32_t input, uint8_t *output, uint32_t *output_idx)
{
    uint8_t encoded_byte = 0, idx = 0;

    do {
        encoded_byte = input % 128;
        input /= 128;
        if (input > 0) {
            encoded_byte |= 128;
        }
        output[idx++] = encoded_byte;
    } while (input > 0);

    *output_idx += idx;
}

static int32_t _core_mqtt_conn_pkt(core_mqtt_handle_t *mqtt_handle, uint8_t **pkt, uint32_t *pkt_len)
{
    uint32_t idx = 0, conn_paylaod_len = 0, conn_remainlen = 0, conn_pkt_len = 0;
    uint8_t *pos = NULL;
    const uint8_t conn_fixed_header = CORE_MQTT_CONN_PKT_TYPE;
    const uint8_t conn_protocol_name[] = {0x00, 0x04, 0x4D, 0x51, 0x54, 0x54};
    const uint8_t conn_protocol_level = 0x04;
    const uint8_t conn_connect_flag = 0xC0 | (mqtt_handle->clean_session << 1);

    /* Payload Length */
    conn_paylaod_len = (uint32_t)(strlen(mqtt_handle->clientid) + strlen(mqtt_handle->username)
                                  + strlen(mqtt_handle->password) + 3 * CORE_MQTT_UTF8_STR_EXTRA_LEN);

    /* Remain-Length Value */
    conn_remainlen = CORE_MQTT_CONN_REMAINLEN_FIXED_LEN + conn_paylaod_len;

    /* Total Packet Length */
    conn_pkt_len = CORE_MQTT_CONN_FIXED_HEADER_TOTAL_LEN + conn_paylaod_len;

    pos = mqtt_handle->sysdep->core_sysdep_malloc(conn_pkt_len, CORE_MQTT_MODULE_NAME);
    if (pos == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(pos, 0, conn_pkt_len);

    /* Fixed Header */
    pos[idx++] = conn_fixed_header;

    /* Remain Length */
    _core_mqtt_remain_len_encode(conn_remainlen, pos + idx, &idx);

    /* Protocol Name */
    memcpy(pos + idx, conn_protocol_name, CORE_MQTT_CONN_PROTOCOL_NAME_LEN);
    idx += CORE_MQTT_CONN_PROTOCOL_NAME_LEN;

    /* Protocol Level */
    pos[idx++] = conn_protocol_level;

    /* Connect Flag */
    pos[idx++] = conn_connect_flag;

    /* Keep Alive MSB */
    pos[idx++] = (uint8_t)((mqtt_handle->keep_alive_s >> 8) & 0x00FF);

    /* Keep Alive LSB */
    pos[idx++] = (uint8_t)((mqtt_handle->keep_alive_s) & 0x00FF);

    /* Payload: clientid, username, password */
    _core_mqtt_set_utf8_encoded_str((uint8_t *)mqtt_handle->clientid, strlen(mqtt_handle->clientid), pos + idx);
    idx += CORE_MQTT_UTF8_STR_EXTRA_LEN + strlen(mqtt_handle->clientid);
    _core_mqtt_set_utf8_encoded_str((uint8_t *)mqtt_handle->username, strlen(mqtt_handle->username), pos + idx);
    idx += CORE_MQTT_UTF8_STR_EXTRA_LEN + strlen(mqtt_handle->username);
    _core_mqtt_set_utf8_encoded_str((uint8_t *)mqtt_handle->password, strlen(mqtt_handle->password), pos + idx);
    idx += CORE_MQTT_UTF8_STR_EXTRA_LEN + strlen(mqtt_handle->password);

    *pkt = pos;
    *pkt_len = idx;

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_connack_handle(core_mqtt_handle_t *mqtt_handle, uint8_t *connack)
{
    int32_t res = STATE_SUCCESS;
    const uint8_t connack_byte1 = CORE_MQTT_CONNACK_PKT_TYPE;
    const uint8_t connack_byte2 = 0x02;
    const uint8_t connack_byte3 = 0x00;

    if (connack[0] != connack_byte1 || connack[1] != connack_byte2 ||
        connack[2] != connack_byte3) {
        return STATE_MQTT_CONNACK_FMT_ERROR;
    }

    if (connack[3] == CORE_MQTT_CONNACK_RCODE_ACCEPTED) {
        res = STATE_SUCCESS;
    } else if (connack[3] == CORE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION) {
        core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT invalid protocol version, disconeect\r\n");
        res = STATE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION;
    } else if (connack[3] == CORE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE) {
        core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT server unavailable, disconnect\r\n");
        res = STATE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE;
    } else if (connack[3] == CORE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD) {
        core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT bad username or password, disconnect\r\n");
        res = STATE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD;
    } else if (connack[3] == CORE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED) {
        core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT authorize fail, disconnect\r\n");
        res = STATE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED;
    } else {
        res = STATE_MQTT_CONNACK_RCODE_UNKNOWN;
    }

    return res;
}

static int32_t _core_mqtt_read(core_mqtt_handle_t *mqtt_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    int32_t res = STATE_SUCCESS;

    if (mqtt_handle->network_handle != NULL) {
        res = mqtt_handle->sysdep->core_sysdep_network_recv(mqtt_handle->network_handle, buffer, len, timeout_ms, NULL);
        if (res < STATE_SUCCESS) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT network error when receving data, disconnect\r\n");
            res = _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_RECV_ERR);
        } else if (res != len) {
            res = STATE_SYS_DEPEND_NWK_READ_LESSDATA;
        }
    } else {
        res = STATE_SYS_DEPEND_NWK_CLOSED;
    }

    return res;
}

static int32_t _core_mqtt_write(core_mqtt_handle_t *mqtt_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    int32_t res = STATE_SUCCESS;

    if (mqtt_handle->network_handle != NULL) {
        res = mqtt_handle->sysdep->core_sysdep_network_send(mqtt_handle->network_handle, buffer, len, timeout_ms, NULL);
        if (res < STATE_SUCCESS) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT network error when sending data, disconnect\r\n");
            res = _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_SEND_ERR);
        } else if (res != len) {
            res = STATE_SYS_DEPEND_NWK_WRITE_LESSDATA;
        }
    } else {
        res = STATE_SYS_DEPEND_NWK_CLOSED;
    }

    return res;
}

static void _core_mqtt_connect_diag(core_mqtt_handle_t *mqtt_handle, uint8_t flag)
{
    uint8_t buf[4] = {0};
    uint32_t idx = 0;

    buf[idx++] = (CORE_MQTT_DIAG_TLV_MQTT_CONNECTION >> 8) & 0x00FF;
    buf[idx++] = (CORE_MQTT_DIAG_TLV_MQTT_CONNECTION) & 0x00FF;
    buf[idx++] = 0x01;
    buf[idx++] = flag;

    core_diag(mqtt_handle->sysdep, STATE_MQTT_BASE, buf, sizeof(buf));
}

static void _core_mqtt_heartbeat_diag(core_mqtt_handle_t *mqtt_handle, uint8_t flag)
{
    uint8_t buf[4] = {0};
    uint32_t idx = 0;

    buf[idx++] = (CORE_MQTT_DIAG_TLV_MQTT_HEARTBEAT >> 8) & 0x00FF;
    buf[idx++] = (CORE_MQTT_DIAG_TLV_MQTT_HEARTBEAT) & 0x00FF;
    buf[idx++] = 0x01;
    buf[idx++] = flag;

    core_diag(mqtt_handle->sysdep, STATE_MQTT_BASE, buf, sizeof(buf));
}

static int32_t _core_mqtt_connect(core_mqtt_handle_t *mqtt_handle)
{
    int32_t res = 0;
    core_sysdep_socket_type_t socket_type = CORE_SYSDEP_SOCKET_TCP_CLIENT;
    char backup_ip[16] = {0};
    uint8_t *conn_pkt = NULL;
    uint8_t connack_pkt[CORE_MQTT_CONNACK_FIXED_HEADER_TOTAL_LEN] = {0};
    uint32_t conn_pkt_len = 0;
    char *secure_mode = (mqtt_handle->cred == NULL) ? ("3") : ("2");

    if (mqtt_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    if (mqtt_handle->security_mode != NULL) {
        secure_mode = mqtt_handle->security_mode;
    }

    if (mqtt_handle->cred && \
        mqtt_handle->cred->option == AIOT_SYSDEP_NETWORK_CRED_NONE && \
        mqtt_handle->security_mode == NULL) {
        secure_mode = "3";
    }

    if (mqtt_handle->username == NULL || mqtt_handle->password == NULL ||
        mqtt_handle->clientid == NULL) {
        /* no valid username, password or clientid, check pk/dn/ds */
        if (mqtt_handle->product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (mqtt_handle->device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
        if (mqtt_handle->device_secret == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_SECRET;
        }
        _core_mqtt_sign_clean(mqtt_handle);

        if ((res = core_auth_mqtt_username(mqtt_handle->sysdep, &mqtt_handle->username, mqtt_handle->product_key,
                                           mqtt_handle->device_name, CORE_MQTT_MODULE_NAME)) < STATE_SUCCESS ||
            (res = core_auth_mqtt_password(mqtt_handle->sysdep, &mqtt_handle->password, mqtt_handle->product_key,
                                           mqtt_handle->device_name, mqtt_handle->device_secret, CORE_MQTT_MODULE_NAME)) < STATE_SUCCESS ||
            (res = core_auth_mqtt_clientid(mqtt_handle->sysdep, &mqtt_handle->clientid, mqtt_handle->product_key,
                                           mqtt_handle->device_name, secure_mode, mqtt_handle->extend_clientid, CORE_MQTT_MODULE_NAME)) < STATE_SUCCESS) {
            _core_mqtt_sign_clean(mqtt_handle);
            return res;
        }
        core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_USERNAME, "%s\r\n", (void *)mqtt_handle->username);
        core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_PASSWORD, "%s\r\n", (void *)mqtt_handle->password);
        /* core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CLIENTID, "%s\r\n", (void *)mqtt_handle->clientid); */
    }

    if (mqtt_handle->network_handle != NULL) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
    }

    _core_mqtt_connect_diag(mqtt_handle, 0x00);

    mqtt_handle->network_handle = mqtt_handle->sysdep->core_sysdep_network_init();
    if (mqtt_handle->network_handle == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    core_global_get_mqtt_backup_ip(mqtt_handle->sysdep, backup_ip);
    if (strlen(backup_ip) > 0) {
        core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_BACKUP_IP, "%s\r\n", (void *)backup_ip);
    }
    if ((res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_SOCKET_TYPE,
               &socket_type)) < STATE_SUCCESS ||
        (res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_HOST,
                mqtt_handle->host)) < STATE_SUCCESS ||
        (res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_BACKUP_IP,
                backup_ip)) < STATE_SUCCESS ||
        (res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_PORT,
                &mqtt_handle->port)) < STATE_SUCCESS ||
        (res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle,
                CORE_SYSDEP_NETWORK_CONNECT_TIMEOUT_MS,
                &mqtt_handle->connect_timeout_ms)) < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        return _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_INVALID_OPTION);
    }

    if (mqtt_handle->cred != NULL) {
        if ((res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_CRED,
                   mqtt_handle->cred)) < STATE_SUCCESS) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            return _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_INVALID_OPTION);
        }
        if (mqtt_handle->cred->option == AIOT_SYSDEP_NETWORK_CRED_SVRCERT_PSK) {
            char *psk_id = NULL, psk[65] = {0};
            core_sysdep_psk_t sysdep_psk;

            res = core_auth_tls_psk(mqtt_handle->sysdep, &psk_id, psk, mqtt_handle->product_key, mqtt_handle->device_name,
                                    mqtt_handle->device_secret, CORE_MQTT_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                return res;
            }

            memset(&sysdep_psk, 0, sizeof(core_sysdep_psk_t));
            sysdep_psk.psk_id = psk_id;
            sysdep_psk.psk = psk;
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_TLS_PSK, "%s\r\n", sysdep_psk.psk_id);
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_TLS_PSK, "%s\r\n", sysdep_psk.psk);
            res = mqtt_handle->sysdep->core_sysdep_network_setopt(mqtt_handle->network_handle, CORE_SYSDEP_NETWORK_PSK,
                    (void *)&sysdep_psk);
            mqtt_handle->sysdep->core_sysdep_free(psk_id);
            if (res < STATE_SUCCESS) {
                return _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_INVALID_OPTION);
            }
        }
        mqtt_handle->nwkstats_info.network_type = (uint8_t)mqtt_handle->cred->option;
    }

    /* network stats */
    mqtt_handle->nwkstats_info.connect_timestamp = mqtt_handle->sysdep->core_sysdep_time();

    if ((res = mqtt_handle->sysdep->core_sysdep_network_establish(mqtt_handle->network_handle)) < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        mqtt_handle->nwkstats_info.failed_timestamp = mqtt_handle->nwkstats_info.connect_timestamp;
        mqtt_handle->nwkstats_info.failed_error_code = res;
        return _core_mqtt_sysdep_return(res, STATE_SYS_DEPEND_NWK_EST_FAILED);
    }

    mqtt_handle->nwkstats_info.connect_time_used = mqtt_handle->sysdep->core_sysdep_time() \
                                                   - mqtt_handle->nwkstats_info.connect_timestamp;


    /* Get MQTT Connect Packet */
    res = _core_mqtt_conn_pkt(mqtt_handle, &conn_pkt, &conn_pkt_len);
    if (res < STATE_SUCCESS) {
        return res;
    }

    /* Send MQTT Connect Packet */
    res = _core_mqtt_write(mqtt_handle, conn_pkt, conn_pkt_len, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_free(conn_pkt);
    if (res < STATE_SUCCESS) {
        if (res == STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT_TIMEOUT, "MQTT connect packet send timeout: %d\r\n",
                      &mqtt_handle->send_timeout_ms);
        }
        return res;
    }

    /* Receive MQTT Connect ACK Packet */
    res = _core_mqtt_read(mqtt_handle, connack_pkt, CORE_MQTT_CONNACK_FIXED_HEADER_TOTAL_LEN, mqtt_handle->recv_timeout_ms);
    if (res < STATE_SUCCESS) {
        if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT_TIMEOUT, "MQTT connack packet recv timeout: %d\r\n",
                      &mqtt_handle->recv_timeout_ms);
        }
        return res;
    }

    res = _core_mqtt_connack_handle(mqtt_handle, connack_pkt);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        return res;
    }

    _core_mqtt_connect_diag(mqtt_handle, 0x01);

    return STATE_MQTT_CONNECT_SUCCESS;
}

static int32_t _core_mqtt_disconnect(core_mqtt_handle_t *mqtt_handle)
{
    int32_t res = 0;
    uint8_t pingreq_pkt[2] = {CORE_MQTT_DISCONNECT_PKT_TYPE, 0x00};

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(mqtt_handle, pingreq_pkt, 2, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static uint16_t _core_mqtt_packet_id(core_mqtt_handle_t *mqtt_handle)
{
    uint16_t packet_id = 0;

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    if ((uint16_t)(mqtt_handle->packet_id + 1) == 0) {
        mqtt_handle->packet_id = 0;
    }
    packet_id = ++mqtt_handle->packet_id;
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);

    return packet_id;
}

static int32_t _core_mqtt_publist_insert(core_mqtt_handle_t *mqtt_handle, uint8_t *packet, uint32_t len,
        uint16_t packet_id)
{
    core_mqtt_pub_node_t *node = NULL;

    core_list_for_each_entry(node, &mqtt_handle->pub_list, linked_node, core_mqtt_pub_node_t) {
        if (node->packet_id == packet_id) {
            return STATE_MQTT_PUBLIST_PACKET_ID_ROLL;
        }
    }

    node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_pub_node_t), CORE_MQTT_MODULE_NAME);
    if (node == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(node, 0, sizeof(core_mqtt_pub_node_t));
    CORE_INIT_LIST_HEAD(&node->linked_node);
    node->packet_id = packet_id;
    node->packet = mqtt_handle->sysdep->core_sysdep_malloc(len, CORE_MQTT_MODULE_NAME);
    if (node->packet == NULL) {
        mqtt_handle->sysdep->core_sysdep_free(node);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(node->packet, 0, len);
    memcpy(node->packet, packet, len);
    node->len = len;
    node->last_send_time = mqtt_handle->sysdep->core_sysdep_time();

    core_list_add_tail(&node->linked_node, &mqtt_handle->pub_list);

    return STATE_SUCCESS;
}

static void _core_mqtt_publist_remove(core_mqtt_handle_t *mqtt_handle, uint16_t packet_id)
{
    core_mqtt_pub_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->pub_list,
                                  linked_node, core_mqtt_pub_node_t) {
        if (node->packet_id == packet_id) {
            core_list_del(&node->linked_node);
            mqtt_handle->sysdep->core_sysdep_free(node->packet);
            mqtt_handle->sysdep->core_sysdep_free(node);
            return;
        }
    }
}

static void _core_mqtt_publist_destroy(core_mqtt_handle_t *mqtt_handle)
{
    core_mqtt_pub_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->pub_list,
                                  linked_node, core_mqtt_pub_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node->packet);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
}

static int32_t _core_mqtt_subunsub(core_mqtt_handle_t *mqtt_handle, char *topic, uint16_t topic_len, uint8_t qos,
                                   uint8_t pkt_type)
{
    int32_t res = 0;
    uint16_t packet_id = 0;
    uint8_t *pkt = NULL;
    uint32_t idx = 0, pkt_len = 0, remainlen = 0;

    remainlen = CORE_MQTT_PACKETID_LEN + CORE_MQTT_UTF8_STR_EXTRA_LEN + topic_len;
    if (pkt_type == CORE_MQTT_SUB_PKT_TYPE) {
        remainlen += CORE_MQTT_REQUEST_QOS_LEN;
    }
    pkt_len = CORE_MQTT_FIXED_HEADER_LEN + CORE_MQTT_REMAINLEN_MAXLEN + remainlen;

    pkt = mqtt_handle->sysdep->core_sysdep_malloc(pkt_len, CORE_MQTT_MODULE_NAME);
    if (pkt == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(pkt, 0, pkt_len);

    /* Subscribe/Unsubscribe Packet Type */
    if (pkt_type == CORE_MQTT_SUB_PKT_TYPE) {
        pkt[idx++] = CORE_MQTT_SUB_PKT_TYPE | CORE_MQTT_SUB_PKT_RESERVE;
    } else if (pkt_type == CORE_MQTT_UNSUB_PKT_TYPE) {
        pkt[idx++] = CORE_MQTT_UNSUB_PKT_TYPE;
    }

    /* Remaining Length */
    _core_mqtt_remain_len_encode(remainlen, &pkt[idx], &idx);

    /* Packet Id */
    packet_id = _core_mqtt_packet_id(mqtt_handle);
    pkt[idx++] = (uint8_t)((packet_id >> 8) & 0x00FF);
    pkt[idx++] = (uint8_t)((packet_id) & 0x00FF);

    /* Topic */
    pkt[idx++] = (uint8_t)((topic_len >> 8) & 0x00FF);
    pkt[idx++] = (uint8_t)((topic_len) & 0x00FF);
    memcpy(&pkt[idx], topic, topic_len);
    idx += topic_len;

    /* QOS */
    if (pkt_type == CORE_MQTT_SUB_PKT_TYPE) {
        pkt[idx++] = qos;
    }

    pkt_len = idx;
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(mqtt_handle, pkt, pkt_len, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_free(pkt);
        return res;
    }

    mqtt_handle->sysdep->core_sysdep_free(pkt);

    return packet_id;
}

static int32_t _core_mqtt_heartbeat(core_mqtt_handle_t *mqtt_handle)
{
    int32_t res = 0;
    uint8_t pingreq_pkt[2] = { CORE_MQTT_PINGREQ_PKT_TYPE, 0x00 };

    _core_mqtt_heartbeat_diag(mqtt_handle, 0x00);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(mqtt_handle, pingreq_pkt, 2, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_repub(core_mqtt_handle_t *mqtt_handle)
{
    int32_t res = 0;
    uint64_t time_now = 0;
    core_mqtt_pub_node_t *node = NULL;

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
    core_list_for_each_entry(node, &mqtt_handle->pub_list, linked_node, core_mqtt_pub_node_t) {
        time_now = mqtt_handle->sysdep->core_sysdep_time();
        if (time_now < node->last_send_time) {
            node->last_send_time = time_now;
        }
        if ((time_now - node->last_send_time) >= mqtt_handle->repub_timeout_ms) {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            res = _core_mqtt_write(mqtt_handle, node->packet, node->len, mqtt_handle->send_timeout_ms);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
            if (res < STATE_SUCCESS) {
                mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);
                return res;
            }
        }
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_process_datalist_insert(core_mqtt_handle_t *mqtt_handle,
        core_mqtt_process_data_t *process_data)
{
    core_mqtt_process_data_node_t *node = NULL;

    core_list_for_each_entry(node, &mqtt_handle->process_data_list,
                             linked_node, core_mqtt_process_data_node_t) {
        if (node->process_data.handler == process_data->handler) {
            node->process_data.context = process_data->context;
            return STATE_SUCCESS;
        }
    }

    if (&node->linked_node == &mqtt_handle->process_data_list) {
        node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_process_data_node_t), CORE_MQTT_MODULE_NAME);
        if (node == NULL) {
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node, 0, sizeof(core_mqtt_process_data_node_t));
        CORE_INIT_LIST_HEAD(&node->linked_node);
        memcpy(&node->process_data, process_data, sizeof(core_mqtt_process_data_t));

        core_list_add_tail(&node->linked_node, &mqtt_handle->process_data_list);
    }

    return STATE_SUCCESS;
}

static void _core_mqtt_process_datalist_remove(core_mqtt_handle_t *mqtt_handle,
        core_mqtt_process_data_t *process_data)
{
    core_mqtt_process_data_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->process_data_list,
                                  linked_node, core_mqtt_process_data_node_t) {
        if (node->process_data.handler == process_data->handler) {
            core_list_del(&node->linked_node);
            mqtt_handle->sysdep->core_sysdep_free(node);
            return;
        }
    }
}

static void _core_mqtt_process_datalist_destroy(core_mqtt_handle_t *mqtt_handle)
{
    core_mqtt_process_data_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, &mqtt_handle->process_data_list,
                                  linked_node, core_mqtt_process_data_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
}

static int32_t _core_mqtt_append_process_data(core_mqtt_handle_t *mqtt_handle,
        core_mqtt_process_data_t *process_data)
{
    int32_t res = STATE_SUCCESS;

    if (process_data->handler == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->process_handler_mutex);
    res = _core_mqtt_process_datalist_insert(mqtt_handle, process_data);
    if (res >= STATE_SUCCESS && mqtt_handle->has_connected == 1) {
        aiot_mqtt_event_t event;

        memset(&event, 0, sizeof(aiot_mqtt_event_t));
        event.type = AIOT_MQTTEVT_CONNECT;

        process_data->handler(process_data->context, &event, NULL);
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->process_handler_mutex);

    return res;
}

static int32_t _core_mqtt_remove_process_data(core_mqtt_handle_t *mqtt_handle,
        core_mqtt_process_data_t *process_data)
{
    int32_t res = STATE_SUCCESS;

    if (process_data->handler == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->process_handler_mutex);
    _core_mqtt_process_datalist_remove(mqtt_handle, process_data);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->process_handler_mutex);

    return res;
}

static void _core_mqtt_process_data_process(core_mqtt_handle_t *mqtt_handle, core_mqtt_event_t *core_event)
{
    core_mqtt_process_data_node_t *node = NULL;

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->process_handler_mutex);
    core_list_for_each_entry(node, &mqtt_handle->process_data_list,
                             linked_node, core_mqtt_process_data_node_t) {
        node->process_data.handler(node->process_data.context, NULL, core_event);
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->process_handler_mutex);
}

static int32_t _core_mqtt_reconnect(core_mqtt_handle_t *mqtt_handle)
{
    int32_t res = STATE_SYS_DEPEND_NWK_CLOSED;
    uint64_t time_now = 0;

    if (mqtt_handle->network_handle != NULL) {
        return STATE_SUCCESS;
    }

    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_RECONNECTING, "MQTT network disconnect, try to reconnecting...\r\n");

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
    time_now = mqtt_handle->sysdep->core_sysdep_time();
    if (time_now < mqtt_handle->reconnect_params.last_retry_time) {
        mqtt_handle->reconnect_params.last_retry_time = time_now;
    }
    if (time_now >= (mqtt_handle->reconnect_params.last_retry_time + mqtt_handle->reconnect_params.interval_ms)) {
        mqtt_handle->reconnect_params.last_retry_time = time_now;
        res = _core_mqtt_connect(mqtt_handle);
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);

    return res;
}

static int32_t _core_mqtt_read_remainlen(core_mqtt_handle_t *mqtt_handle, uint32_t *remainlen)
{
    int32_t res = 0;
    uint8_t ch = 0;
    uint32_t multiplier = 1;
    uint32_t mqtt_remainlen = 0;

    do {
        res = _core_mqtt_read(mqtt_handle, &ch, 1, mqtt_handle->recv_timeout_ms);
        if (res < STATE_SUCCESS) {
            return res;
        }
        mqtt_remainlen += (ch & 127) * multiplier;
        if (multiplier > 128 * 128 * 128) {
            return STATE_MQTT_MALFORMED_REMAINING_LEN;
        }
        multiplier *= 128;
    } while ((ch & 128) != 0);

    *remainlen = mqtt_remainlen;

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_read_remainbytes(core_mqtt_handle_t *mqtt_handle, uint32_t remainlen, uint8_t **output)
{
    int32_t res = 0;
    uint8_t *remain = NULL;

    if (remainlen > 0) {
        remain = mqtt_handle->sysdep->core_sysdep_malloc(remainlen, CORE_MQTT_MODULE_NAME);
        if (remain == NULL) {
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(remain, 0, remainlen);

        res = _core_mqtt_read(mqtt_handle, remain, remainlen, mqtt_handle->recv_timeout_ms);
        if (res < STATE_SUCCESS) {
            mqtt_handle->sysdep->core_sysdep_free(remain);
            if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
                return STATE_MQTT_MALFORMED_REMAINING_BYTES;
            } else {
                return res;
            }
        }
    }
    *output = remain;

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_pingresp_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len)
{
    aiot_mqtt_recv_t packet;
    uint64_t rtt = mqtt_handle->sysdep->core_sysdep_time()  \
                   - mqtt_handle->heartbeat_params.last_send_time;

    if (len != 0) {
        return STATE_MQTT_RECV_INVALID_PINRESP_PACKET;
    }

    /* heartbreat rtt stats */
    if (rtt < CORE_MQTT_NWKSTATS_RTT_THRESHOLD) {
        mqtt_handle->nwkstats_info.rtt = (mqtt_handle->nwkstats_info.rtt + rtt) / 2;
    }

    memset(&packet, 0, sizeof(aiot_mqtt_recv_t));

    if (mqtt_handle->recv_handler != NULL) {
        packet.type = AIOT_MQTTRECV_HEARTBEAT_RESPONSE;
        mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
    }

    _core_mqtt_heartbeat_diag(mqtt_handle, 0x01);

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_puback_send(core_mqtt_handle_t *mqtt_handle, uint16_t packet_id)
{
    int32_t res = 0;
    uint8_t pkt[4] = {0};

    /* Packet Type */
    pkt[0] = CORE_MQTT_PUBACK_PKT_TYPE;

    /* Remaining Lengh */
    pkt[1] = 2;

    /* Packet Id */
    pkt[2] = (uint16_t)((packet_id >> 8) & 0x00FF);
    pkt[3] = (uint16_t)((packet_id) & 0x00FF);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(mqtt_handle, pkt, 4, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_topic_compare(char *topic, uint32_t topic_len, char *cmp_topic, uint32_t cmp_topic_len)
{
    uint32_t idx = 0, cmp_idx = 0;

    for (idx = 0, cmp_idx = 0; idx < topic_len; idx++) {
        /* compare topic alreay out of bounds */
        if (cmp_idx >= cmp_topic_len) {
            /* compare success only in case of the left string of topic is "/#" */
            if ((topic_len - idx == 2) && (memcmp(&topic[idx], "/#", 2) == 0)) {
                return STATE_SUCCESS;
            } else {
                return STATE_MQTT_TOPIC_COMPARE_FAILED;
            }
        }

        /* if topic reach the '#', compare success */
        if (topic[idx] == '#') {
            return STATE_SUCCESS;
        }

        if (topic[idx] == '+') {
            /* wildcard + exist */
            for (; cmp_idx < cmp_topic_len; cmp_idx++) {
                if (cmp_topic[cmp_idx] == '/') {
                    /* if topic already reach the bound, compare topic should not contain '/' */
                    if (idx + 1 == topic_len) {
                        return STATE_MQTT_TOPIC_COMPARE_FAILED;
                    } else {
                        break;
                    }
                }
            }
        } else {
            /* compare each character */
            if (topic[idx] != cmp_topic[cmp_idx]) {
                return STATE_MQTT_TOPIC_COMPARE_FAILED;
            }
            cmp_idx++;
        }
    }

    /* compare topic should be reach the end */
    if (cmp_idx < cmp_topic_len) {
        return STATE_MQTT_TOPIC_COMPARE_FAILED;
    }
    return STATE_SUCCESS;
}

static void _core_mqtt_handlerlist_append(core_mqtt_handle_t *mqtt_handle, struct core_list_head *dest,
        struct core_list_head *src, uint8_t *found)
{
    core_mqtt_sub_handler_node_t *node = NULL, *copy_node = NULL;

    core_list_for_each_entry(node, src, linked_node, core_mqtt_sub_handler_node_t) {
        copy_node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_sub_handler_node_t), CORE_MQTT_MODULE_NAME);
        if (copy_node == NULL) {
            continue;
        }
        memset(copy_node, 0, sizeof(core_mqtt_sub_handler_node_t));
        CORE_INIT_LIST_HEAD(&copy_node->linked_node);
        copy_node->handler = node->handler;
        copy_node->userdata = node->userdata;

        core_list_add_tail(&copy_node->linked_node, dest);
        *found = 1;
    }
}

static void _core_mqtt_handlerlist_destroy(core_mqtt_handle_t *mqtt_handle, struct core_list_head *list)
{
    core_mqtt_sub_handler_node_t *node = NULL, *next = NULL;

    core_list_for_each_entry_safe(node, next, list, linked_node, core_mqtt_sub_handler_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
}

static int32_t _core_mqtt_pub_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len, uint8_t qos)
{
    uint8_t sub_found = 0;
    uint32_t idx = 0;
    uint16_t packet_id = 0;
    uint16_t utf8_strlen = 0;
    aiot_mqtt_recv_t packet;
    uint32_t topic_len = 0;
    void *userdata;
    core_mqtt_sub_node_t *sub_node = NULL;
    core_mqtt_sub_handler_node_t *handler_node = NULL;
    struct core_list_head handler_list_copy;

    if (input == NULL || len == 0 || qos > CORE_MQTT_QOS1) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }

    memset(&packet, 0, sizeof(aiot_mqtt_recv_t));
    packet.type = AIOT_MQTTRECV_PUB;

    /* Topic Length */
    if (len < 2) { return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET; }
    utf8_strlen = input[idx++] << 8;
    utf8_strlen |= input[idx++];

    packet.data.pub.topic = (char *)&input[idx];
    packet.data.pub.topic_len = utf8_strlen;
    idx += utf8_strlen;

    /* Packet Id For QOS1 */
    if (len < idx) { return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET; }
    if (qos == CORE_MQTT_QOS1) {
        if (len < idx + 2) { return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET; }
        packet_id = input[idx++] << 8;
        packet_id |= input[idx++];
    }

    /* Payload Len */
    if ((int64_t)len - CORE_MQTT_PUBLISH_TOPICLEN_LEN - (int64_t)packet.data.pub.topic_len < 0) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }
    packet.data.pub.payload = &input[idx];
    packet.data.pub.payload_len = len - CORE_MQTT_PUBLISH_TOPICLEN_LEN - packet.data.pub.topic_len;
    if (qos == CORE_MQTT_QOS1) {
        packet.data.pub.payload_len -= CORE_MQTT_PACKETID_LEN;
    }

    /* Publish Ack For QOS1 */
    if (qos == CORE_MQTT_QOS1) {
        _core_mqtt_puback_send(mqtt_handle, packet_id);
    }

    /* debug */
    topic_len = (uint32_t)packet.data.pub.topic_len;
    core_log2(mqtt_handle->sysdep, STATE_MQTT_LOG_TOPIC, "pub: %.*s\r\n", &topic_len,
              packet.data.pub.topic);
    core_log_hexdump(STATE_MQTT_LOG_HEXDUMP, '<', packet.data.pub.payload, packet.data.pub.payload_len);

    /* Search Packet Handler In sublist */
    CORE_INIT_LIST_HEAD(&handler_list_copy);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->sub_mutex);
    core_list_for_each_entry(sub_node, &mqtt_handle->sub_list, linked_node, core_mqtt_sub_node_t) {
        if (_core_mqtt_topic_compare(sub_node->topic, (uint32_t)(strlen(sub_node->topic)), packet.data.pub.topic,
                                     packet.data.pub.topic_len) == STATE_SUCCESS) {
            _core_mqtt_handlerlist_append(mqtt_handle, &handler_list_copy, &sub_node->handle_list, &sub_found);
        }
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->sub_mutex);

    core_list_for_each_entry(handler_node, &handler_list_copy,
                             linked_node, core_mqtt_sub_handler_node_t) {
        userdata = (handler_node->userdata == NULL) ? (mqtt_handle->userdata) : (handler_node->userdata);
        handler_node->handler(mqtt_handle, &packet, userdata);
    }

    _core_mqtt_handlerlist_destroy(mqtt_handle, &handler_list_copy);

    /* User Data Default Packet Handler */
    if (mqtt_handle->recv_handler && sub_found == 0) {
        mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
    }

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_puback_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len)
{
    aiot_mqtt_recv_t packet;

    if (input == NULL || len != 2) {
        return STATE_MQTT_RECV_INVALID_PUBACK_PACKET;
    }

    memset(&packet, 0, sizeof(aiot_mqtt_recv_t));
    packet.type = AIOT_MQTTRECV_PUB_ACK;

    /* Packet Id */
    packet.data.pub_ack.packet_id = input[0] << 8;
    packet.data.pub_ack.packet_id |= input[1];

    /* Remove Packet From republist */
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
    _core_mqtt_publist_remove(mqtt_handle, packet.data.pub_ack.packet_id);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);

    /* User Ctrl Packet Handler */
    if (mqtt_handle->recv_handler) {
        mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
    }

    return STATE_SUCCESS;
}

static void _core_mqtt_subunsuback_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len,
        uint8_t packet_type)
{
    uint32_t idx = 0;
    aiot_mqtt_recv_t packet;

    if (input == NULL || len == 0 ||
        (packet_type == CORE_MQTT_SUBACK_PKT_TYPE && len % 3 != 0) ||
        (packet_type == CORE_MQTT_UNSUBACK_PKT_TYPE && len % 2 != 0)) {
        return;
    }

    for (idx = 0; idx < len;) {
        memset(&packet, 0, sizeof(aiot_mqtt_recv_t));
        if (packet_type == CORE_MQTT_SUBACK_PKT_TYPE) {
            packet.type = AIOT_MQTTRECV_SUB_ACK;
            packet.data.sub_ack.packet_id = input[idx] << 8;
            packet.data.sub_ack.packet_id |= input[idx + 1];
        } else {
            packet.type = AIOT_MQTTRECV_UNSUB_ACK;
            packet.data.unsub_ack.packet_id = input[idx] << 8;
            packet.data.unsub_ack.packet_id |= input[idx + 1];
        }

        if (packet_type == CORE_MQTT_SUBACK_PKT_TYPE) {
            if (input[idx + 2] == CORE_MQTT_SUBACK_RCODE_MAXQOS0 ||
                input[idx + 2] == CORE_MQTT_SUBACK_RCODE_MAXQOS1 ||
                input[idx + 2] == CORE_MQTT_SUBACK_RCODE_MAXQOS2) {
                packet.data.sub_ack.res = STATE_SUCCESS;
                packet.data.sub_ack.max_qos = input[idx + 2];
            } else if (input[idx + 2] == CORE_MQTT_SUBACK_RCODE_FAILURE) {
                packet.data.sub_ack.res = STATE_MQTT_SUBACK_RCODE_FAILURE;
            } else {
                packet.data.sub_ack.res = STATE_MQTT_SUBACK_RCODE_UNKNOWN;
            }
            idx += 3;
        } else {
            idx += 2;
        }

        if (mqtt_handle->recv_handler) {
            mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
        }
    }
}

int32_t core_mqtt_setopt(void *handle, core_mqtt_option_t option, void *data)
{
    int32_t res = 0;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (option >= CORE_MQTTOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    switch (option) {
        case CORE_MQTTOPT_APPEND_PROCESS_HANDLER: {
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
            res = _core_mqtt_append_process_data(mqtt_handle, (core_mqtt_process_data_t *)data);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
        }
        break;
        case CORE_MQTTOPT_REMOVE_PROCESS_HANDLER: {
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
            res = _core_mqtt_remove_process_data(mqtt_handle, (core_mqtt_process_data_t *)data);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);

    return res;
}

void *aiot_mqtt_init(void)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    res = core_global_init(sysdep);
    if (res < STATE_SUCCESS) {
        return NULL;
    }

    mqtt_handle = sysdep->core_sysdep_malloc(sizeof(core_mqtt_handle_t), CORE_MQTT_MODULE_NAME);
    if (mqtt_handle == NULL) {
        core_global_deinit(sysdep);
        return NULL;
    }
    memset(mqtt_handle, 0, sizeof(core_mqtt_handle_t));

    mqtt_handle->sysdep = sysdep;
    mqtt_handle->keep_alive_s = CORE_MQTT_DEFAULT_KEEPALIVE_S;
    mqtt_handle->clean_session = CORE_MQTT_DEFAULT_CLEAN_SESSION;
    mqtt_handle->connect_timeout_ms = CORE_MQTT_DEFAULT_CONNECT_TIMEOUT_MS;
    mqtt_handle->heartbeat_params.interval_ms = CORE_MQTT_DEFAULT_HEARTBEAT_INTERVAL_MS;
    mqtt_handle->heartbeat_params.max_lost_times = CORE_MQTT_DEFAULT_HEARTBEAT_MAX_LOST_TIMES;
    mqtt_handle->reconnect_params.enabled = CORE_MQTT_DEFAULT_RECONN_ENABLED;
    mqtt_handle->reconnect_params.interval_ms = CORE_MQTT_DEFAULT_RECONN_INTERVAL_MS;
    mqtt_handle->send_timeout_ms = CORE_MQTT_DEFAULT_SEND_TIMEOUT_MS;
    mqtt_handle->recv_timeout_ms = CORE_MQTT_DEFAULT_RECV_TIMEOUT_MS;
    mqtt_handle->repub_timeout_ms = CORE_MQTT_DEFAULT_REPUB_TIMEOUT_MS;
    mqtt_handle->deinit_timeout_ms = CORE_MQTT_DEFAULT_DEINIT_TIMEOUT_MS;

    mqtt_handle->data_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->send_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->recv_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->sub_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->pub_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->process_handler_mutex = sysdep->core_sysdep_mutex_init();

    CORE_INIT_LIST_HEAD(&mqtt_handle->sub_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->pub_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->process_data_list);

    mqtt_handle->exec_enabled = 1;

    return mqtt_handle;
}

int32_t aiot_mqtt_setopt(void *handle, aiot_mqtt_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (option >= AIOT_MQTTOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
    switch (option) {
        case AIOT_MQTTOPT_HOST: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->host, (char *)data, CORE_MQTT_MODULE_NAME);
        }
        break;
        case AIOT_MQTTOPT_PORT: {
            mqtt_handle->port = *(uint16_t *)data;
        }
        break;
        case AIOT_MQTTOPT_PRODUCT_KEY: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->product_key, (char *)data, CORE_MQTT_MODULE_NAME);
            _core_mqtt_sign_clean(mqtt_handle);
        }
        break;
        case AIOT_MQTTOPT_DEVICE_NAME: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->device_name, (char *)data, CORE_MQTT_MODULE_NAME);
            _core_mqtt_sign_clean(mqtt_handle);
        }
        break;
        case AIOT_MQTTOPT_DEVICE_SECRET: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->device_secret, (char *)data, CORE_MQTT_MODULE_NAME);
            _core_mqtt_sign_clean(mqtt_handle);
        }
        break;
        case AIOT_MQTTOPT_EXTEND_CLIENTID: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->extend_clientid, (char *)data, CORE_MQTT_MODULE_NAME);
            _core_mqtt_sign_clean(mqtt_handle);
        }
        break;
        case AIOT_MQTTOPT_SECURITY_MODE: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->security_mode, (char *)data, CORE_MQTT_MODULE_NAME);
            _core_mqtt_sign_clean(mqtt_handle);
        }
        break;
        case AIOT_MQTTOPT_USERNAME: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->username, (char *)data, CORE_MQTT_MODULE_NAME);
        }
        break;
        case AIOT_MQTTOPT_PASSWORD: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->password, (char *)data, CORE_MQTT_MODULE_NAME);
        }
        break;
        case AIOT_MQTTOPT_CLIENTID: {
            res = core_strdup(mqtt_handle->sysdep, &mqtt_handle->clientid, (char *)data, CORE_MQTT_MODULE_NAME);
        }
        break;
        case AIOT_MQTTOPT_KEEPALIVE_SEC: {
            mqtt_handle->keep_alive_s = *(uint16_t *)data;
        }
        break;
        case AIOT_MQTTOPT_CLEAN_SESSION: {
            if (*(uint8_t *)data != 0 && *(uint8_t *)data != 1) {
                res = STATE_USER_INPUT_OUT_RANGE;
            }
            mqtt_handle->clean_session = *(uint8_t *)data;
        }
        break;
        case AIOT_MQTTOPT_NETWORK_CRED: {
            if (mqtt_handle->cred != NULL) {
                mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->cred);
                mqtt_handle->cred = NULL;
            }
            mqtt_handle->cred = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(aiot_sysdep_network_cred_t), CORE_MQTT_MODULE_NAME);
            if (mqtt_handle->cred != NULL) {
                memset(mqtt_handle->cred, 0, sizeof(aiot_sysdep_network_cred_t));
                memcpy(mqtt_handle->cred, data, sizeof(aiot_sysdep_network_cred_t));
            } else {
                res = STATE_SYS_DEPEND_MALLOC_FAILED;
            }
        }
        break;
        case AIOT_MQTTOPT_CONNECT_TIMEOUT_MS: {
            mqtt_handle->connect_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_HEARTBEAT_INTERVAL_MS: {
            mqtt_handle->heartbeat_params.interval_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_HEARTBEAT_MAX_LOST: {
            mqtt_handle->heartbeat_params.max_lost_times = *(uint8_t *)data;
        }
        break;
        case AIOT_MQTTOPT_RECONN_ENABLED: {
            if (*(uint8_t *)data != 0 && *(uint8_t *)data != 1) {
                res = STATE_USER_INPUT_OUT_RANGE;
            }
            mqtt_handle->reconnect_params.enabled = *(uint8_t *)data;
        }
        break;
        case AIOT_MQTTOPT_RECONN_INTERVAL_MS: {
            mqtt_handle->reconnect_params.interval_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_SEND_TIMEOUT_MS: {
            mqtt_handle->send_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_RECV_TIMEOUT_MS: {
            mqtt_handle->recv_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_REPUB_TIMEOUT_MS: {
            mqtt_handle->repub_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_DEINIT_TIMEOUT_MS: {
            mqtt_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTTOPT_RECV_HANDLER: {
            mqtt_handle->recv_handler = (aiot_mqtt_recv_handler_t)data;
        }
        break;
        case AIOT_MQTTOPT_EVENT_HANDLER: {
            mqtt_handle->event_handler = (aiot_mqtt_event_handler_t)data;
        }
        break;
        case AIOT_MQTTOPT_APPEND_TOPIC_MAP: {
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
            res = _core_mqtt_append_topic_map(mqtt_handle, (aiot_mqtt_topic_map_t *)data);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
        }
        break;
        case AIOT_MQTTOPT_REMOVE_TOPIC_MAP: {
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);
            res = _core_mqtt_remove_topic_map(mqtt_handle, (aiot_mqtt_topic_map_t *)data);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->data_mutex);
        }
        break;
        case AIOT_MQTTOPT_APPEND_REQUESTID: {
            mqtt_handle->append_requestid = *(uint8_t *)data;
        }
        break;
        case AIOT_MQTTOPT_USERDATA: {
            mqtt_handle->userdata = data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->data_mutex);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

int32_t aiot_mqtt_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    core_mqtt_handle_t *mqtt_handle = NULL;
    core_mqtt_event_t core_event;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    mqtt_handle = *(core_mqtt_handle_t **)handle;

    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    mqtt_handle->exec_enabled = 0;
    deinit_timestart = mqtt_handle->sysdep->core_sysdep_time();
    do {
        if (mqtt_handle->exec_count == 0) {
            break;
        }
        mqtt_handle->sysdep->core_sysdep_sleep(CORE_MQTT_DEINIT_INTERVAL_MS);
    } while ((mqtt_handle->sysdep->core_sysdep_time() - deinit_timestart) < mqtt_handle->deinit_timeout_ms);

    if (mqtt_handle->exec_count != 0) {
        return STATE_MQTT_DEINIT_TIMEOUT;
    }

    memset(&core_event, 0, sizeof(core_mqtt_event_t));
    core_event.type = CORE_MQTTEVT_DEINIT;
    _core_mqtt_process_data_process(mqtt_handle, &core_event);

    *handle = NULL;

    if (mqtt_handle->network_handle != NULL) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
    }
    if (mqtt_handle->host != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->host);
    }
    if (mqtt_handle->product_key != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->product_key);
    }
    if (mqtt_handle->device_name != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->device_name);
    }
    if (mqtt_handle->device_secret != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->device_secret);
    }
    if (mqtt_handle->username != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->username);
    }
    if (mqtt_handle->password != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->password);
    }
    if (mqtt_handle->clientid != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->clientid);
    }
    if (mqtt_handle->extend_clientid != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->extend_clientid);
    }
    if (mqtt_handle->security_mode != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->security_mode);
    }
    if (mqtt_handle->cred != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->cred);
    }

    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->data_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->sub_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->pub_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->process_handler_mutex);

    _core_mqtt_sublist_destroy(mqtt_handle);
    _core_mqtt_publist_destroy(mqtt_handle);
    _core_mqtt_process_datalist_destroy(mqtt_handle);

    core_global_deinit(mqtt_handle->sysdep);

    mqtt_handle->sysdep->core_sysdep_free(mqtt_handle);

    return STATE_SUCCESS;
}

int32_t aiot_mqtt_connect(void *handle)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;
    uint64_t time_ent_ms = 0;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    time_ent_ms = mqtt_handle->sysdep->core_sysdep_time();

    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    mqtt_handle->disconnect_api_called = 0;

    /* if network connection exist, close first */
    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "MQTT user calls aiot_mqtt_connect api, connect\r\n");
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
    res = _core_mqtt_connect(mqtt_handle);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);

    if (res == STATE_MQTT_CONNECT_SUCCESS) {
        uint64_t time_ms = mqtt_handle->sysdep->core_sysdep_time();
        uint32_t time_delta = (uint32_t)(time_ms - time_ent_ms);

        core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "MQTT connect success in %d ms\r\n", (void *)&time_delta);
        _core_mqtt_connect_event_notify(mqtt_handle);
        res = STATE_SUCCESS;
    } else {
        _core_mqtt_disconnect_event_notify(mqtt_handle, AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT);
    }

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

int32_t aiot_mqtt_disconnect(void *handle)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    mqtt_handle->disconnect_api_called = 1;

    /* send mqtt disconnect packet to mqtt broker first */
    _core_mqtt_disconnect(handle);

    /* close socket connect with mqtt broker */
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
    if (mqtt_handle->network_handle != NULL) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);

    _core_mqtt_disconnect_event_notify(mqtt_handle, AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT);
    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT user calls aiot_mqtt_disconnect api, disconnect\r\n");

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

int32_t aiot_mqtt_heartbeat(void *handle)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    return _core_mqtt_heartbeat(mqtt_handle);
}

int32_t aiot_mqtt_process(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint64_t time_now = 0;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    /* mqtt PINREQ packet */
    time_now = mqtt_handle->sysdep->core_sysdep_time();
    if (time_now < mqtt_handle->heartbeat_params.last_send_time) {
        mqtt_handle->heartbeat_params.last_send_time = time_now;
    }
    if ((time_now - mqtt_handle->heartbeat_params.last_send_time) >= mqtt_handle->heartbeat_params.interval_ms) {
        res = _core_mqtt_heartbeat(mqtt_handle);
        if (res == STATE_SUCCESS) {
            mqtt_handle->heartbeat_params.last_send_time = time_now;
            mqtt_handle->heartbeat_params.lost_times++;
        }
    }

    /* mqtt QoS1 packet republish */
    _core_mqtt_repub(mqtt_handle);

    /* mqtt process handler process */
    _core_mqtt_process_data_process(mqtt_handle, NULL);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

static int32_t _core_mqtt_pub(void *handle, core_mqtt_buff_t *topic, core_mqtt_buff_t *payload, uint8_t qos)
{
    int32_t res = STATE_SUCCESS;
    uint16_t packet_id = 0;
    uint8_t *pkt = NULL;
    uint32_t idx = 0, remainlen = 0, pkt_len = 0;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL || topic == NULL || topic->buffer == NULL ||
        payload == NULL || payload->buffer == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (topic->len == 0 || qos > CORE_MQTT_QOS_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    core_log2(mqtt_handle->sysdep, STATE_MQTT_LOG_TOPIC, "pub: %.*s\r\n", &topic->len, topic->buffer);
    core_log_hexdump(STATE_MQTT_LOG_HEXDUMP, '>', payload->buffer, payload->len);

    remainlen = topic->len + payload->len + CORE_MQTT_UTF8_STR_EXTRA_LEN;
    if (qos == CORE_MQTT_QOS1) {
        remainlen += CORE_MQTT_PACKETID_LEN;
    }
    pkt_len = CORE_MQTT_FIXED_HEADER_LEN + CORE_MQTT_REMAINLEN_MAXLEN + remainlen;

    pkt = mqtt_handle->sysdep->core_sysdep_malloc(pkt_len, CORE_MQTT_MODULE_NAME);
    if (pkt == NULL) {
        _core_mqtt_exec_dec(mqtt_handle);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(pkt, 0, pkt_len);

    /* Publish Packet Type */
    pkt[idx++] = CORE_MQTT_PUBLISH_PKT_TYPE | (qos << 1);

    /* Remaining Length */
    _core_mqtt_remain_len_encode(remainlen, &pkt[idx], &idx);

    /* Topic */
    _core_mqtt_set_utf8_encoded_str((uint8_t *)topic->buffer, topic->len, &pkt[idx]);
    idx += CORE_MQTT_UTF8_STR_EXTRA_LEN + topic->len;

    /* Packet Id For QOS 1*/
    if (qos == CORE_MQTT_QOS1) {
        packet_id = _core_mqtt_packet_id(handle);
        pkt[idx++] = (uint8_t)((packet_id >> 8) & 0x00FF);
        pkt[idx++] = (uint8_t)((packet_id) & 0x00FF);
    }

    /* Payload */
    memcpy(&pkt[idx], payload->buffer, payload->len);
    idx += payload->len;

    pkt_len = idx;

    if (qos == CORE_MQTT_QOS1) {
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
        res = _core_mqtt_publist_insert(mqtt_handle, pkt, pkt_len, packet_id);
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);
        if (res < STATE_SUCCESS) {
            mqtt_handle->sysdep->core_sysdep_free(pkt);
            _core_mqtt_exec_dec(mqtt_handle);
            return res;
        }
    }

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(handle, pkt, pkt_len, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_free(pkt);
        _core_mqtt_exec_dec(mqtt_handle);
        return res;
    }

    mqtt_handle->sysdep->core_sysdep_free(pkt);

    if (qos == CORE_MQTT_QOS1) {
        _core_mqtt_exec_dec(mqtt_handle);
        return (int32_t)packet_id;
    }

    _core_mqtt_exec_dec(mqtt_handle);

    return STATE_SUCCESS;
}

int32_t aiot_mqtt_pub(void *handle, char *topic, uint8_t *payload, uint32_t payload_len, uint8_t qos)
{
    core_mqtt_buff_t    topic_buff;
    core_mqtt_buff_t    payload_buff;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;
    uint8_t append_rid = 0;
    int32_t res = STATE_SUCCESS;

    if (NULL == topic || NULL == handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (strlen(topic) >= CORE_MQTT_TOPIC_MAXLEN) {
        return STATE_MQTT_TOPIC_TOO_LONG;
    }

    if (payload_len >= CORE_MQTT_PAYLOAD_MAXLEN) {
        return STATE_MQTT_PUB_PAYLOAD_TOO_LONG;
    }

    memset(&topic_buff, 0, sizeof(topic_buff));
    memset(&payload_buff, 0, sizeof(payload_buff));

    append_rid = mqtt_handle->append_requestid;
    if (0 == append_rid) {
        topic_buff.buffer = (uint8_t *)topic;
        topic_buff.len = strlen(topic);
    }
    else {
        char *rid_prefix = "?_rid=";
        uint64_t timestamp = core_log_get_timestamp(mqtt_handle->sysdep);
        uint32_t rand = 0;
        char *buffer = NULL;
        uint32_t buffer_len = strlen(topic) + strlen(rid_prefix) + 32;

        buffer = mqtt_handle->sysdep->core_sysdep_malloc(buffer_len, CORE_MQTT_MODULE_NAME);
        if (NULL == buffer) {
            return STATE_PORT_MALLOC_FAILED;
        }
        memset(buffer, 0, buffer_len);

        memcpy(buffer, topic, strlen(topic));
        memcpy(buffer + strlen(buffer), rid_prefix, strlen(rid_prefix));
        core_uint642str(timestamp, buffer + strlen(buffer), NULL);
        mqtt_handle->sysdep->core_sysdep_rand((uint8_t *)&rand, sizeof(rand));
        core_uint2str(rand, buffer + strlen(buffer), NULL);

        topic_buff.buffer = (uint8_t *)buffer;
        topic_buff.len = strlen(buffer);
    }

    payload_buff.buffer = payload;
    payload_buff.len = payload_len;

    res = _core_mqtt_pub(handle, &topic_buff, &payload_buff, qos);
    if (append_rid != 0) {
        mqtt_handle->sysdep->core_sysdep_free(topic_buff.buffer);
    }
    return res;
}

static int32_t _core_mqtt_sub(void *handle, core_mqtt_buff_t *topic, aiot_mqtt_recv_handler_t handler,
                              uint8_t qos,
                              void *userdata)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL || topic == NULL || topic->buffer == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (topic->len == 0 || qos > CORE_MQTT_QOS_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }
    if ((res = _core_mqtt_topic_is_valid((char *)topic->buffer, topic->len)) < STATE_SUCCESS) {
        return STATE_MQTT_TOPIC_INVALID;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    core_log2(mqtt_handle->sysdep, STATE_MQTT_LOG_TOPIC, "sub: %.*s\r\n", &topic->len, topic->buffer);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->sub_mutex);
    res = _core_mqtt_sublist_insert(mqtt_handle, topic, handler, userdata);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->sub_mutex);

    if (res < STATE_SUCCESS) {
        return res;
    }

    /* send subscribe packet */
    res = _core_mqtt_subunsub(mqtt_handle, (char *)topic->buffer, topic->len, qos, CORE_MQTT_SUB_PKT_TYPE);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

int32_t aiot_mqtt_sub(void *handle, char *topic, aiot_mqtt_recv_handler_t handler, uint8_t qos, void *userdata)
{
    core_mqtt_buff_t    topic_buff;

    if (NULL == topic) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (strlen(topic) >= CORE_MQTT_TOPIC_MAXLEN) {
        return STATE_MQTT_TOPIC_TOO_LONG;
    }

    memset(&topic_buff, 0, sizeof(topic_buff));

    topic_buff.buffer = (uint8_t *)topic;
    topic_buff.len = strlen(topic);

    return _core_mqtt_sub(handle, &topic_buff, handler, qos, userdata);
}

static int32_t _core_mqtt_unsub(void *handle, core_mqtt_buff_t *topic)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL || topic == NULL || topic->buffer == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (topic->len == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }
    if ((res = _core_mqtt_topic_is_valid((char *)topic->buffer, topic->len)) < STATE_SUCCESS) {
        return STATE_MQTT_TOPIC_INVALID;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    core_log2(mqtt_handle->sysdep, STATE_MQTT_LOG_TOPIC, "unsub: %.*s\r\n", &topic->len, topic->buffer);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->sub_mutex);
    _core_mqtt_sublist_remove(mqtt_handle, topic);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->sub_mutex);

    res = _core_mqtt_subunsub(mqtt_handle, (char *)topic->buffer, topic->len, 0, CORE_MQTT_UNSUB_PKT_TYPE);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

int32_t aiot_mqtt_unsub(void *handle, char *topic)
{
    core_mqtt_buff_t    topic_buff;

    if (NULL == topic) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (strlen(topic) >= CORE_MQTT_TOPIC_MAXLEN) {
        return STATE_MQTT_TOPIC_TOO_LONG;
    }

    memset(&topic_buff, 0, sizeof(topic_buff));

    topic_buff.buffer = (uint8_t *)topic;
    topic_buff.len = strlen(topic);

    return _core_mqtt_unsub(handle, &topic_buff);
}

int32_t aiot_mqtt_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint32_t mqtt_remainlen = 0;
    uint8_t mqtt_fixed_header[CORE_MQTT_FIXED_HEADER_LEN] = {0};
    uint8_t mqtt_pkt_type = 0, mqtt_pkt_reserved = 0;
    uint8_t *remain = NULL;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    /* network error reconnect */
    if (mqtt_handle->network_handle == NULL) {
        _core_mqtt_disconnect_event_notify(mqtt_handle, AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT);
    }
    if (mqtt_handle->reconnect_params.enabled == 1 && mqtt_handle->disconnect_api_called == 0) {
        res = _core_mqtt_reconnect(mqtt_handle);
        if (res < STATE_SUCCESS) {
            if (res == STATE_MQTT_CONNECT_SUCCESS) {
                mqtt_handle->heartbeat_params.lost_times = 0;
                core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "MQTT network reconnect success\r\n");
                _core_mqtt_connect_event_notify(mqtt_handle);
                res = STATE_SUCCESS;
            } else {
                _core_mqtt_exec_dec(mqtt_handle);
                return res;
            }
        }
    }

    /* heartbeat missing reconnect */
    if (mqtt_handle->heartbeat_params.lost_times > mqtt_handle->heartbeat_params.max_lost_times) {
        _core_mqtt_disconnect_event_notify(mqtt_handle, AIOT_MQTTDISCONNEVT_HEARTBEAT_DISCONNECT);
        if (mqtt_handle->reconnect_params.enabled == 1 && mqtt_handle->disconnect_api_called == 0) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_RECONNECTING, "MQTT heartbeat lost %d times, try to reconnecting...\r\n",
                      &mqtt_handle->heartbeat_params.lost_times);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            res = _core_mqtt_connect(mqtt_handle);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
            if (res < STATE_SUCCESS) {
                if (res == STATE_MQTT_CONNECT_SUCCESS) {
                    mqtt_handle->heartbeat_params.lost_times = 0;
                    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "MQTT network reconnect success\r\n");
                    _core_mqtt_connect_event_notify(mqtt_handle);
                    res = STATE_SUCCESS;
                } else {
                    _core_mqtt_exec_dec(mqtt_handle);
                    return res;
                }
            }
        }
    }

    /* Read Packet Type Of MQTT Fixed Header, Get  And Remaining Packet Length */
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
    res = _core_mqtt_read(handle, mqtt_fixed_header, CORE_MQTT_FIXED_HEADER_LEN, mqtt_handle->recv_timeout_ms);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        _core_mqtt_exec_dec(mqtt_handle);
        if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
            res = STATE_SUCCESS;
        }
        return res;
    }

    /* Read Remaining Packet Length Of MQTT Fixed Header */
    res = _core_mqtt_read_remainlen(mqtt_handle, &mqtt_remainlen);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
        if (mqtt_handle->network_handle != NULL) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        }
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        _core_mqtt_exec_dec(mqtt_handle);
        return res;
    }

    /* Read Remaining Bytes */
    res = _core_mqtt_read_remainbytes(handle, mqtt_remainlen, &remain);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
        if (mqtt_handle->network_handle != NULL) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        }
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        _core_mqtt_exec_dec(mqtt_handle);
        return res;
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);

    /* Get Packet Type */
    mqtt_pkt_type = mqtt_fixed_header[0] & 0xF0;
    mqtt_pkt_reserved = mqtt_fixed_header[0] & 0x0F;

    /* reset ping response missing times */
    mqtt_handle->heartbeat_params.lost_times = 0;

    switch (mqtt_pkt_type) {
        case CORE_MQTT_PINGRESP_PKT_TYPE: {
            res = _core_mqtt_pingresp_handler(mqtt_handle, remain, mqtt_remainlen);
        }
        break;
        case CORE_MQTT_PUBLISH_PKT_TYPE: {
            res = _core_mqtt_pub_handler(handle, remain, mqtt_remainlen, ((mqtt_pkt_reserved >> 1) & 0x03));
        }
        break;
        case CORE_MQTT_PUBACK_PKT_TYPE: {
            res = _core_mqtt_puback_handler(handle, remain, mqtt_remainlen);
        }
        break;
        case CORE_MQTT_SUBACK_PKT_TYPE: {
            _core_mqtt_subunsuback_handler(handle, remain, mqtt_remainlen, CORE_MQTT_SUBACK_PKT_TYPE);
        }
        break;
        case CORE_MQTT_UNSUBACK_PKT_TYPE: {
            _core_mqtt_subunsuback_handler(handle, remain, mqtt_remainlen, CORE_MQTT_UNSUBACK_PKT_TYPE);
        }
        break;
        case CORE_MQTT_PUBREC_PKT_TYPE:
        case CORE_MQTT_PUBREL_PKT_TYPE:
        case CORE_MQTT_PUBCOMP_PKT_TYPE: {

        }
        break;
        default: {
            res = STATE_MQTT_PACKET_TYPE_UNKNOWN;
        }
    }

    if (remain) {
        mqtt_handle->sysdep->core_sysdep_free(remain);
    }

    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
        if (mqtt_handle->network_handle != NULL) {
            mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
        }
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    }

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

char *core_mqtt_get_product_key(void *handle)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (handle == NULL) {
        return NULL;
    }

    return mqtt_handle->product_key;
}

char *core_mqtt_get_device_name(void *handle)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (handle == NULL) {
        return NULL;
    }

    return mqtt_handle->device_name;
}

uint16_t core_mqtt_get_port(void *handle)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (handle == NULL) {
        return 0;
    }

    return mqtt_handle->port;
}

int32_t core_mqtt_get_nwkstats(void *handle, core_mqtt_nwkstats_info_t *nwk_stats_info)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;
    if (handle == NULL) {
        return 0;
    }

    memcpy(nwk_stats_info, &mqtt_handle->nwkstats_info, sizeof(core_mqtt_nwkstats_info_t));
    mqtt_handle->nwkstats_info.failed_error_code = 0;
    mqtt_handle->nwkstats_info.connect_time_used = 0;
    return STATE_SUCCESS;
}

