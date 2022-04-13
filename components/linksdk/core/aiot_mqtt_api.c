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

static int32_t _core_mqtt_5_feature_is_enabled(core_mqtt_handle_t *mqtt_handle)
{
    return (AIOT_MQTT_VERSION_5_0 == mqtt_handle->protocol_version);
}

static void _core_mqtt_event_notify_process_handler(core_mqtt_handle_t *mqtt_handle, aiot_mqtt_event_t *event,
        core_mqtt_event_t *core_event)
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

static int32_t _core_mqtt_topic_alias_list_insert(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic,
        uint16_t topic_alias, struct core_list_head *list)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_topic_alias_node_t *node = NULL;

    core_list_for_each_entry(node, list, linked_node, core_mqtt_topic_alias_node_t) {
        if ((strlen(node->topic) == topic->len) && memcmp(node->topic, topic->buffer, topic->len) == 0) {
            /* exist topic */
            return STATE_SUCCESS;
        }
    }

    if (&node->linked_node == list) {
        /* new topic */
        node = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(core_mqtt_topic_alias_node_t), CORE_MQTT_MODULE_NAME);
        if (node == NULL) {
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node, 0, sizeof(core_mqtt_topic_alias_node_t));
        CORE_INIT_LIST_HEAD(&node->linked_node);

        node->topic = mqtt_handle->sysdep->core_sysdep_malloc(topic->len + 1, CORE_MQTT_MODULE_NAME);
        if (node->topic == NULL) {
            mqtt_handle->sysdep->core_sysdep_free(node);
            return STATE_SYS_DEPEND_MALLOC_FAILED;
        }
        memset(node->topic, 0, topic->len + 1);
        memcpy(node->topic, topic->buffer, topic->len);
        node->topic_alias = topic_alias;

        core_list_add_tail(&node->linked_node, list);
    }
    return res;
}

static void _core_mqtt_topic_alias_list_remove_all(core_mqtt_handle_t *mqtt_handle)
{
    core_mqtt_topic_alias_node_t *node = NULL, *next = NULL;
    core_list_for_each_entry_safe(node, next, &mqtt_handle->rx_topic_alias_list, linked_node,
                                  core_mqtt_topic_alias_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node->topic);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }

    core_list_for_each_entry_safe(node, next, &mqtt_handle->tx_topic_alias_list, linked_node,
                                  core_mqtt_topic_alias_node_t) {
        core_list_del(&node->linked_node);
        mqtt_handle->sysdep->core_sysdep_free(node->topic);
        mqtt_handle->sysdep->core_sysdep_free(node);
    }
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

static void _core_mqtt_sublist_remove_handler(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic,
        aiot_mqtt_recv_handler_t handler)
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

static int32_t _core_mqtt_general_property_is_valid(general_property_t *general_property)
{
    uint32_t idx = 0;

    if (NULL == general_property) {
        return STATE_SUCCESS;
    }

    /* User Property */
    user_property_t **user_prop_list = general_property->user_property;
    do {
        user_property_t *user_prop = user_prop_list[idx++];
        if (NULL != user_prop) {
            if (user_prop->key.len > CORE_MQTT_USER_PROPERTY_KEY_MAX_LEN
                || user_prop->value.len > CORE_MQTT_USER_PROPERTY_VALUE_MAX_LEN
               ) {
                return STATE_MQTT_INVALID_USER_PERPERTY_LEN;
            }
            if ((user_prop->key.len > 0 && NULL == user_prop->key.value)
                || (user_prop->value.len > 0 && NULL == user_prop->value.value)) {
                return STATE_MQTT_INVALID_USER_PERPERTY_DATA;
            }
        }
    } while (idx < USER_PROPERTY_MAX);

    /* Subscription Identifier */
    if (general_property->subscription_identifier > 0xFFFFFFF) {
        return STATE_MQTT_INVALID_SUBSCRIPTION_IDENTIFIER;
    }

    return STATE_SUCCESS;
}

uint32_t _core_get_general_property_len(general_property_t *general_prop);
void _core_write_general_prop(uint8_t *pos, uint32_t *index, general_property_t *general_property,
                              uint8_t *property_len_array, uint8_t property_len_offset)
{
    uint32_t idx = *index;
    uint16_t topic_alias_max = general_property->topic_alias_max;
    uint16_t topic_alias = general_property->topic_alias;
    uint16_t client_receive_max = general_property->client_receive_max;
    len_value_t *reason_string = general_property->reason_string;
    user_property_t **prop_list = general_property->user_property;

    /* Total Length of All Properties */
    if (NULL != property_len_array) {
        memcpy(&(pos[idx]), property_len_array, property_len_offset);
        idx += property_len_offset;
    }

    /*User Properties */
    if (NULL != prop_list) {
        int iter = 0;
        for (iter = 0; iter < USER_PROPERTY_MAX; iter++) {
            user_property_t *prop = prop_list[iter];
            if (NULL != prop) {
                pos[idx++] = CORE_MQTTPROP_USER_PROPERTY;
                pos[idx++] = prop->key.len << 8;
                pos[idx++] = prop->key.len & 0xff;
                memcpy(&(pos[idx]), prop->key.value, prop->key.len);
                idx += prop->key.len;

                pos[idx++] = prop->value.len << 8;
                pos[idx++] = prop->value.len & 0xff;
                memcpy(&(pos[idx]), prop->value.value, prop->value.len);
                idx += prop->value.len;
            }
        }
    }

    /*User Topic Alias max */
    if (topic_alias_max > 0) {
        pos[idx++] = CORE_MQTTPROP_TOPIC_ALIAS_MAX;
        pos[idx++] = topic_alias_max >> 8;
        pos[idx++] = topic_alias_max & 0xFF;
    }

    /* Topic Alias */
    if (topic_alias > 0) {
        pos[idx++] = CORE_MQTTPROP_TOPIC_ALIAS;
        pos[idx++] = topic_alias >> 8;
        pos[idx++] = topic_alias & 0xFF;
    }

    /*Client Receive Max */
    if (client_receive_max > 0) {
        pos[idx++] = CORE_MQTTPROP_RECEIVE_MAXIMUM;
        pos[idx++] = client_receive_max >> 8;
        pos[idx++] = client_receive_max & 0xFF;
    }

    /* Response Topic */
    if (0 != general_property->response_topic.len) {
        pos[idx++] = CORE_MQTTPROP_RESPONSE_TOPIC;
        uint16_t topic_len = general_property->response_topic.len;
        pos[idx++] = topic_len >> 8;
        pos[idx++] = topic_len & 0xFF;
        memcpy(&(pos[idx]), general_property->response_topic.value, topic_len);
        idx += topic_len;
    }

    /* Correlation Data */
    if (0 != general_property->correlation_data.len) {
        pos[idx++] = CORE_MQTTPROP_CORRELATION_DATA;
        uint16_t data_len = general_property->correlation_data.len;
        pos[idx++] = data_len >> 8;
        pos[idx++] = data_len & 0xFF;
        memcpy(&(pos[idx]), general_property->correlation_data.value, data_len);
        idx += data_len;
    }

    /* Subscription Identifier */
    if (general_property->subscription_identifier > 0) {
        uint32_t subscription_identifer_offset = 0;
        uint8_t subscription_identifier[4] = {0};
        _core_mqtt_remain_len_encode(general_property->subscription_identifier, &subscription_identifier[0],
                                     &subscription_identifer_offset);
        pos[idx++] = CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER;
        memcpy(&pos[idx], subscription_identifier, subscription_identifer_offset);
        idx += subscription_identifer_offset;
    }

    /* Reason String */
    if (NULL != reason_string) {
        pos[idx++] =  CORE_MQTTPROP_REASON_STRING;
        pos[idx++] = (reason_string->len) >> 8 ;
        pos[idx++] =  reason_string->len & 0xFF;
        memcpy(&pos[idx], reason_string->value, reason_string->len);
        idx += reason_string->len ;
    }

    *index = idx;
}

static int32_t _read_variable_byte_interger(uint8_t *input, uint32_t *remainlen, uint8_t *offset);
static int32_t _core_mqtt_conn_pkt(core_mqtt_handle_t *mqtt_handle, uint8_t **pkt, uint32_t *pkt_len,
                                   conn_property_t *conn_prop)
{
    uint32_t idx = 0, conn_paylaod_len = 0, conn_remainlen = 0, conn_pkt_len = 0, property_len = 0, property_total_len = 0;
    uint8_t *pos = NULL;
    const uint8_t conn_fixed_header = CORE_MQTT_CONN_PKT_TYPE;
    const uint8_t conn_protocol_name[] = {0x00, 0x04, 0x4D, 0x51, 0x54, 0x54};
    uint8_t conn_protocol_level = 0x04;
    const uint8_t conn_connect_flag = 0xC0 | (mqtt_handle->clean_session << 1);
    uint32_t property_len_offset = 0;
    uint8_t property_len_array[4] = {0};
    general_property_t general_property = {0};

    /* Property Len */
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        conn_protocol_level = 0x5;
        if (NULL != conn_prop) {
            general_property.topic_alias_max = conn_prop->topic_alias_max;
            memcpy(&(general_property.user_property[0]), &(conn_prop->user_property[0]),
                   USER_PROPERTY_MAX * (sizeof(user_property_t *)));
        }

        if (NULL == mqtt_handle->pre_connect_property) {
            /* Normal Connection Case */
            int res = _core_mqtt_general_property_is_valid(&general_property);
            if (res < STATE_SUCCESS) {
                return res;
            }

            property_len = _core_get_general_property_len(&general_property);
            _core_mqtt_remain_len_encode(property_len, property_len_array, &property_len_offset);
        } else {
            /* Reconnetion Case: Use Connection Properties Stored Previously */
            uint8_t tmp_offset = 0;
            _read_variable_byte_interger(mqtt_handle->pre_connect_property, &property_len, &tmp_offset);
            property_len_offset = tmp_offset;
        }

        property_total_len = property_len_offset + property_len;
    }

    /* Payload Length */
    conn_paylaod_len = (uint32_t)(strlen(mqtt_handle->clientid) + strlen(mqtt_handle->username)
                                  + strlen(mqtt_handle->password) + 3 * CORE_MQTT_UTF8_STR_EXTRA_LEN);

    /* Remain-Length Value */
    conn_remainlen = CORE_MQTT_CONN_REMAINLEN_FIXED_LEN + conn_paylaod_len + property_total_len;

    /* Total Packet Length */
    conn_pkt_len = CORE_MQTT_CONN_FIXED_HEADER_TOTAL_LEN + conn_paylaod_len + property_total_len;

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

    /* property */
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        if (NULL != mqtt_handle->pre_connect_property) {
            /* Normal Connection Case: Use Connection Properties Stored Previously */
            memcpy(&(pos[idx]), mqtt_handle->pre_connect_property, property_total_len);
            idx += property_total_len;
        } else  {
            /* Not Reconnection Case */
            uint8_t *start = &(pos[idx]);
            _core_write_general_prop(pos, &idx, &general_property, property_len_array, property_len_offset);
            mqtt_handle->pre_connect_property = mqtt_handle->sysdep->core_sysdep_malloc(property_total_len, CORE_MQTT_MODULE_NAME);
            if (NULL == mqtt_handle->pre_connect_property) {
                return STATE_SYS_DEPEND_MALLOC_FAILED;
            }
            memset(mqtt_handle->pre_connect_property, 0, property_total_len);
            memcpy(mqtt_handle->pre_connect_property, start, property_total_len);
        }
    }

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

static int32_t _read_variable_byte_interger(uint8_t *input, uint32_t *remainlen, uint8_t *offset)
{
    uint8_t ch = 0;
    uint32_t multiplier = 1;
    uint32_t mqtt_remainlen = 0;
    uint8_t pos = 0;

    do {
        ch = input[pos++];
        mqtt_remainlen += (ch & 127) * multiplier;
        if (multiplier > 128 * 128 * 128) {
            return STATE_MQTT_MALFORMED_REMAINING_LEN;
        }
        multiplier *= 128;
    } while ((ch & 128) != 0 && pos < 4);

    *remainlen = mqtt_remainlen;
    *offset = pos;

    return STATE_SUCCESS;
}

static int32_t _core_mqtt_parse_property_element(core_mqtt_handle_t *mqtt_handle, uint8_t *property, uint32_t *idx,
        type_len_value_t *tlv,
        user_property_t *user_prop, uint32_t *variable_byte_integer)
{
    uint32_t read_len = 0;
    uint8_t type = property[0];
    int32_t res;

    switch (type) {
        /* Bits (Single Byte) */
        case CORE_MQTTPROP_PAYLOAD_FORMAT_INDICATOR:
        case CORE_MQTTPROP_REQUEST_PROBLEM_INFORMATION:
        case CORE_MQTTPROP_REQUEST_RESPONSE_INFORMATION:
        case CORE_MQTTPROP_MAX_QOS:
        case CORE_MQTTPROP_RETAIN_AVAILABLE:
        case CORE_MQTTPROP_WILDCARD_SUBSCRIPTION_AVAILABLE:
        case CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER_AVAILABLE:
        case CORE_MQTTPROP_SHARED_SUBSCRIPTION_AVAILABLE: {
            read_len++;
            tlv->type = type;
            tlv->len = 1;
            read_len += 1;
            tlv->value = property + CORE_MQTT_V5_PROPERTY_ID_LEN ;
        }
        break;

        /* Two Byte Integer */
        case CORE_MQTTPROP_SERVER_KEEP_ALIVE:
        case CORE_MQTTPROP_TOPIC_ALIAS:
        case CORE_MQTTPROP_RECEIVE_MAXIMUM:
        case CORE_MQTTPROP_TOPIC_ALIAS_MAX: {
            read_len++;
            tlv->type = type;
            tlv->len = 2;
            read_len += 2;
            tlv->value = property + CORE_MQTT_V5_PROPERTY_ID_LEN;
        }
        break;

        /* Four Byte Integer */
        case CORE_MQTTPROP_PUBLICATION_EXPIRY_INTERVAL:
        case CORE_MQTTPROP_SESSION_EXPIRY_INTERVAL:
        case CORE_MQTTPROP_WILL_DELAY_INTERVAL:
        case CORE_MQTTPROP_MAX_PACK_SIZE: {
            read_len++;
            tlv->type = type;
            tlv->len = 4;
            read_len += 4;
            tlv->value = property + CORE_MQTT_V5_PROPERTY_ID_LEN;
        }
        break;

        /* Variable Byte Integer */
        case CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER: {
            read_len++;
            uint8_t *remain_len_start = property + read_len;
            uint32_t remain_len = 0;
            uint8_t offset = 0;

            res = _read_variable_byte_interger(remain_len_start, &remain_len, &offset);
            if (res < STATE_SUCCESS) {
                return res;
            }
            read_len += offset;

            tlv->type = type;
            tlv->len = offset;
            *variable_byte_integer = remain_len;
        }
        break;

        /* Binary Data; UTF-8 String */
        case CORE_MQTTPROP_CONTENT_TYPE:
        case CORE_MQTTPROP_RESPONSE_TOPIC:
        case CORE_MQTTPROP_ASSIGNED_CLIENT_IDENTIFIER:
        case CORE_MQTTPROP_AUTHENTICATION_METHOD:
        case CORE_MQTTPROP_RESPONSE_INFORMATION:
        case CORE_MQTTPROP_SERVER_REFERENCE:
        case CORE_MQTTPROP_REASON_STRING:
        case CORE_MQTTPROP_CORRELATION_DATA:
        case CORE_MQTTPROP_AUTHENTICATION_DATA: {
            read_len++;
            tlv->type = type;
            tlv->len = ((*(property + 1)) << 8) + (*(property + 2));
            read_len += 2;
            tlv->value = property + 3;
            read_len += tlv->len;
        }
        break;

        /* UTF-8 String Pairs. especially for User Properties */
        case CORE_MQTTPROP_USER_PROPERTY: {
            read_len++;
            uint8_t *key_start = property + read_len;
            user_prop->key.len = ((*(key_start)) << 8) + (*(key_start + 1));
            user_prop->key.value = key_start + 2;
            read_len += 2 + user_prop->key.len;

            uint8_t *value_start = property + read_len;
            user_prop->value.len = ((*(value_start)) << 8) + (*(value_start + 1));
            user_prop->value.value = value_start + 2;
            read_len += 2 + user_prop->value.len;

            tlv->type = type;
            tlv->len = 2 + user_prop->key.len + 2 + user_prop->value.len;
        }
        break;

        default: {
            tlv->type = CORE_MQTTPROP_UNRESOLVED;
            core_log(mqtt_handle->sysdep, STATE_MQTT_UNKNOWN_PROPERTY_OPTION, "unresolved property option\n");
        }
        break;
    }
    *idx = *idx + read_len;
    return STATE_SUCCESS;
}

static int32_t _core_mqtt_parse_general_properties(core_mqtt_handle_t *mqtt_handle, uint8_t *property,
        general_property_t *general_prop, uint32_t *parsed_property_len, uint8_t *parsed_offset, uint32_t remain_len)
{
    uint32_t idx = 0, total_prop_len = 0;
    int32_t res;
    user_property_t **user_prop = general_prop->user_property;
    uint32_t property_len = 0;
    uint8_t property_len_offset = 0;
    type_len_value_t tlv = {0};
    user_property_t cur_user_prop;
    uint8_t user_property_count = 0;
    uint32_t variable_byte_integer = 0;

    res = _read_variable_byte_interger(&(property[0]), &property_len,  &property_len_offset);
    if (res < STATE_SUCCESS) {
        return res;
    }

    /* quit if the total len of all properties exceeds the remain_len of mqtt packet */
    if (property_len > remain_len - property_len_offset) {
        return STATE_MQTT_INVALID_PROPERTY_LEN;
    }

    if (NULL != parsed_property_len) {
        *parsed_property_len = property_len;
    }
    if (NULL != parsed_offset) {
        *parsed_offset = property_len_offset;
    }

    total_prop_len = property_len + property_len_offset;
    if (property_len == 0) {
        return STATE_SUCCESS;
    }

    idx += property_len_offset;
    general_prop->max_qos = 2;                            /* by default is 2 */
    general_prop->wildcard_subscription_available = 1;    /* enabled by default */
    general_prop->subscription_identifier_available = 1;  /* enabled by default */
    general_prop->shared_subscription_available = 1;      /* enabled by default */

    while (total_prop_len > idx) {

        memset(&tlv, 0, sizeof(tlv));
        memset(&cur_user_prop, 0, sizeof(cur_user_prop));
        res = _core_mqtt_parse_property_element(mqtt_handle, property + idx, &idx, &tlv, &cur_user_prop,
                                                &variable_byte_integer);
        if (res < STATE_SUCCESS) {
            return res;
        }

        /* quit if the length of a certain property exceeds the remain_len of the whole packet */
        if (idx > remain_len) {
            return STATE_MQTT_INVALID_PROPERTY_LEN;
        }

        switch (tlv.type) {
            case CORE_MQTTPROP_TOPIC_ALIAS: {
                uint16_t topic_alias = 0;
                topic_alias = (uint16_t)(*(tlv.value) << 8) ;
                topic_alias += (uint16_t)(*(tlv.value + 1));
                general_prop->topic_alias = topic_alias;
            }
            break;
            case CORE_MQTTPROP_TOPIC_ALIAS_MAX: {
                uint16_t topic_alias_max = 0;
                topic_alias_max = (uint16_t)(*(tlv.value) << 8) ;
                topic_alias_max += (uint16_t)(*(tlv.value + 1));
                general_prop->topic_alias_max = topic_alias_max;
            }
            break;
            case CORE_MQTTPROP_MAX_QOS: {
                uint8_t max_qos = 0;
                max_qos = (uint8_t)(*(tlv.value));
                general_prop->max_qos = max_qos;
            }
            break;
            case CORE_MQTTPROP_ASSIGNED_CLIENT_IDENTIFIER: {
                void *cid = mqtt_handle->sysdep->core_sysdep_malloc(tlv.len + 1, CORE_MQTT_MODULE_NAME);
                if (NULL == cid) {
                    return STATE_SYS_DEPEND_MALLOC_FAILED;
                }
                memset(cid, 0, tlv.len + 1);
                memcpy(cid, tlv.value, tlv.len);
                general_prop->assigned_clientid = cid;
            }
            break;
            case CORE_MQTTPROP_WILDCARD_SUBSCRIPTION_AVAILABLE: {
                general_prop->wildcard_subscription_available = (uint8_t)(*(tlv.value));
            }
            break;
            case CORE_MQTTPROP_SUBSCRIPTION_IDENTIFIER_AVAILABLE: {
                general_prop->subscription_identifier_available = (uint8_t)(*(tlv.value));
            }
            break;
            case CORE_MQTTPROP_SHARED_SUBSCRIPTION_AVAILABLE: {
                general_prop->shared_subscription_available = (uint8_t)(*(tlv.value));
            }
            break;
            case CORE_MQTTPROP_PUBLICATION_EXPIRY_INTERVAL: {
                uint32_t message_expire = 0;
                message_expire += *(tlv.value) << 24;
                message_expire += *(tlv.value + 1) << 16;
                message_expire += *(tlv.value + 2) << 8;
                message_expire += *(tlv.value + 3) << 0;
                general_prop->message_expire_interval = message_expire;
            }
            break;
            case CORE_MQTTPROP_MAX_PACK_SIZE: {
                uint32_t max_pack_size = 0;
                max_pack_size += *(tlv.value) << 24;
                max_pack_size += *(tlv.value + 1) << 16;
                max_pack_size += *(tlv.value + 2) << 8;
                max_pack_size += *(tlv.value + 3) << 0;
                general_prop->max_packet_size = max_pack_size;
            }
            break;
            case CORE_MQTTPROP_RECEIVE_MAXIMUM: {
                uint16_t server_receive_max = 0;
                server_receive_max = (uint16_t)(*(tlv.value) << 8) ;
                server_receive_max += (uint16_t)(*(tlv.value + 1));
                general_prop->server_receive_max = server_receive_max;
            }
            break;
            case CORE_MQTTPROP_USER_PROPERTY: {
                if (user_property_count < USER_PROPERTY_MAX && cur_user_prop.key.len != 0) {
                    user_prop[user_property_count] = mqtt_handle->sysdep->core_sysdep_malloc(sizeof(user_property_t),
                                                     CORE_MQTT_MODULE_NAME);
                    if (NULL == user_prop[user_property_count]) {
                        return STATE_SYS_DEPEND_MALLOC_FAILED;
                    }
                    memcpy(user_prop[user_property_count], &cur_user_prop, sizeof(user_property_t));
                    user_property_count++;
                }
            }
            break;
            case CORE_MQTTPROP_UNRESOLVED: {
                return STATE_MQTT_UNKNOWN_PROPERTY_OPTION;
            }
            default:
                break;
        }
    }

    return STATE_SUCCESS;
}


static int32_t _core_mqtt_parse_conack_properties(core_mqtt_handle_t *mqtt_handle, uint8_t *property,
        connack_property_t *conack_prop, uint32_t remain_len)
{
    general_property_t general_prop = {0};
    int32_t res = _core_mqtt_parse_general_properties(mqtt_handle, property, &general_prop, NULL, NULL, remain_len);
    if (res != STATE_SUCCESS) {
        return res;
    }

    conack_prop->max_qos = general_prop.max_qos;
    conack_prop->topic_alias_max = general_prop.topic_alias_max;
    memcpy(&(conack_prop->user_property[0]), &(general_prop.user_property[0]),
           USER_PROPERTY_MAX * (sizeof(user_property_t *)));
    conack_prop->assigned_clientid = general_prop.assigned_clientid;
    conack_prop->max_packet_size = general_prop.max_packet_size;
    conack_prop->server_receive_max = general_prop.server_receive_max;
    conack_prop->wildcard_subscription_available = general_prop.wildcard_subscription_available;
    conack_prop->subscription_identifier_available = general_prop.subscription_identifier_available;
    conack_prop->shared_subscription_available = general_prop.shared_subscription_available;
    mqtt_handle->tx_topic_alias_max =
                conack_prop->topic_alias_max;

    return STATE_SUCCESS;
}

static void _core_reclaim_user_properties_mem(core_mqtt_handle_t *mqtt_handle,  user_property_t **user_property_list)
{
    int iter = 0;
    for (iter = 0; iter < USER_PROPERTY_MAX; iter++) {
        user_property_t *user_prop = user_property_list[iter];
        if (NULL != user_prop) {
            mqtt_handle->sysdep->core_sysdep_free(user_prop);
            user_property_list[iter] = NULL;
        }
    }
}

static int32_t _core_mqtt_connack_handle(core_mqtt_handle_t *mqtt_handle, uint8_t *connack, uint32_t remain_len)
{
    int32_t res = STATE_SUCCESS;
    int32_t  ret = STATE_SUCCESS;

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        connack_property_t connack_prop = {0};
        aiot_mqtt_recv_t packet = {0};

        if (connack[0] != 0x00) {
            return STATE_MQTT_CONNACK_FMT_ERROR;
        }

        /* First Byte is Conack Flag */
        if (connack[1] == CORE_MQTT_CONNACK_RCODE_ACCEPTED) {
            res = STATE_SUCCESS;
        } else if (connack[1] == CORE_MQTT_V5_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT invalid protocol version, disconeect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION;
        } else if (connack[1] == CORE_MQTT_V5_CONNACK_RCODE_SERVER_UNAVAILABLE) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT server unavailable, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE;
        } else if (connack[1] == CORE_MQTT_V5_CONNACK_RCODE_BAD_USERNAME_PASSWORD) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT bad username or password, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD;
        } else if (connack[1] == CORE_MQTT_V5_CONNACK_RCODE_NOT_AUTHORIZED) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT authorize fail, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED;
        } else {
            res = STATE_MQTT_CONNACK_RCODE_UNKNOWN;
        }

        /* Second Byte is Reason Code */
        packet.data.con_ack.reason_code = connack[1];

        /* Properties Starts from 3rd Byte */
        ret = _core_mqtt_parse_conack_properties(mqtt_handle, &connack[2], &connack_prop, remain_len);
        if (ret < STATE_SUCCESS) {
            return ret;
        }

        packet.data.con_ack.prop = connack_prop;
        packet.type = AIOT_MQTTRECV_CON_ACK;

        if (mqtt_handle->recv_handler) {
            mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
        }

        if (NULL != connack_prop.assigned_clientid) {
            mqtt_handle->sysdep->core_sysdep_free(connack_prop.assigned_clientid);
        }

        /* Reclaims Mem of User Properties */
        _core_reclaim_user_properties_mem(mqtt_handle, connack_prop.user_property);

    } else {

        if (connack[1] == CORE_MQTT_CONNACK_RCODE_ACCEPTED) {
            res = STATE_SUCCESS;
        } else if (connack[1] == CORE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT invalid protocol version, disconeect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_UNACCEPTABLE_PROTOCOL_VERSION;
        } else if (connack[1] == CORE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT server unavailable, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_SERVER_UNAVAILABLE;
        } else if (connack[1] == CORE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT bad username or password, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_BAD_USERNAME_PASSWORD;
        } else if (connack[1] == CORE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT authorize fail, disconnect\r\n");
            res = STATE_MQTT_CONNACK_RCODE_NOT_AUTHORIZED;
        } else {
            res = STATE_MQTT_CONNACK_RCODE_UNKNOWN;
        }
    }
    return res;
}

static int32_t _core_mqtt_read(core_mqtt_handle_t *mqtt_handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms)
{
    int32_t res = STATE_SUCCESS;

    if (mqtt_handle->network_handle != NULL) {
        res = mqtt_handle->sysdep->core_sysdep_network_recv(mqtt_handle->network_handle, buffer, len, timeout_ms, NULL);
        if (res < STATE_SUCCESS) {
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

static int32_t _core_mqtt_read_remainlen(core_mqtt_handle_t *mqtt_handle, uint32_t *remainlen);


static int32_t _core_mqtt_connect(core_mqtt_handle_t *mqtt_handle, conn_property_t *conn_prop)
{
    int32_t res = 0;
    core_sysdep_socket_type_t socket_type = CORE_SYSDEP_SOCKET_TCP_CLIENT;
    char backup_ip[16] = {0};
    uint8_t *conn_pkt = NULL;
    uint8_t connack_fixed_header = 0;
    uint8_t *connack_ptr = NULL;
    uint32_t conn_pkt_len = 0;
    char *secure_mode = (mqtt_handle->cred == NULL) ? ("3") : ("2");
    uint8_t use_assigned_clientid = mqtt_handle->use_assigned_clientid;
    uint32_t remain_len = 0;

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
                                           mqtt_handle->device_name, mqtt_handle->device_secret, use_assigned_clientid, CORE_MQTT_MODULE_NAME)) < STATE_SUCCESS ||
            (res = core_auth_mqtt_clientid(mqtt_handle->sysdep, &mqtt_handle->clientid, mqtt_handle->product_key,
                                           mqtt_handle->device_name, secure_mode, mqtt_handle->extend_clientid, use_assigned_clientid,
                                           CORE_MQTT_MODULE_NAME)) < STATE_SUCCESS) {
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

    /* Remove All Topic Alias Relationship */
    _core_mqtt_topic_alias_list_remove_all(mqtt_handle);

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
    res = _core_mqtt_conn_pkt(mqtt_handle, &conn_pkt, &conn_pkt_len, conn_prop);
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
        } else {
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
        }
        return res;
    }

    /* Receive MQTT Connect ACK Fixed Header Byte */
    res = _core_mqtt_read(mqtt_handle, &connack_fixed_header, CORE_MQTT_FIXED_HEADER_LEN, mqtt_handle->recv_timeout_ms);
    if (res < STATE_SUCCESS) {
        if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT_TIMEOUT, "MQTT connack packet recv fixed header timeout: %d\r\n",
                      &mqtt_handle->recv_timeout_ms);
        } else {
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
        }
        return res;
    }

    if (connack_fixed_header != CORE_MQTT_CONNACK_PKT_TYPE) {
        return STATE_MQTT_CONNACK_FMT_ERROR;
    }

    /* Receive MQTT Connect ACK remain len */
    res = _core_mqtt_read_remainlen(mqtt_handle, &remain_len);
    if (res < STATE_SUCCESS) {
        if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT_TIMEOUT, "MQTT connack packet recv remain len timeout: %d\r\n",
                      &mqtt_handle->recv_timeout_ms);
        }
        return res;
    }

    /* Connack Format Error for Mqtt 3.1 */
    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle) && remain_len != 0x2) {
        return STATE_MQTT_CONNACK_FMT_ERROR;
    }

    connack_ptr = mqtt_handle->sysdep->core_sysdep_malloc(remain_len, CORE_MQTT_MODULE_NAME);
    if (NULL == connack_ptr) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(connack_ptr, 0, remain_len);

    /* Receive MQTT Connect ACK Variable Header and Properties */
    res = _core_mqtt_read(mqtt_handle, connack_ptr, remain_len, mqtt_handle->recv_timeout_ms);

    if (res < STATE_SUCCESS) {
        if (res == STATE_SYS_DEPEND_NWK_READ_LESSDATA) {
            core_log1(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT_TIMEOUT,
                      "MQTT connack packet variable header and property recv timeout: %d\r\n",
                      &mqtt_handle->recv_timeout_ms);
        }
        mqtt_handle->sysdep->core_sysdep_free(connack_ptr);
        return res;
    }

    res = _core_mqtt_connack_handle(mqtt_handle, connack_ptr, remain_len);
    mqtt_handle->sysdep->core_sysdep_free(connack_ptr);
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
        if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        }
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
                                   uint8_t pkt_type, general_property_t *general_property)
{
    int32_t res = 0;
    uint16_t packet_id = 0;
    uint8_t *pkt = NULL;
    uint32_t idx = 0, pkt_len = 0, remainlen = 0;
    uint32_t property_total_len = 0;
    uint32_t property_len = 0, property_len_offset = 0;
    uint8_t property_len_array[4] = {0};

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        if (NULL != general_property) {
            res = _core_mqtt_general_property_is_valid(general_property);
            if (res < STATE_SUCCESS) {
                return res;
            }
            property_len = _core_get_general_property_len(general_property);
        }

        /* Property Len */
        _core_mqtt_remain_len_encode(property_len, &property_len_array[0],
                                     &property_len_offset);
        property_total_len = property_len_offset + property_len;
    }

    remainlen = CORE_MQTT_PACKETID_LEN + CORE_MQTT_UTF8_STR_EXTRA_LEN + topic_len + property_total_len;
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
        uint8_t unsub_flag = 0;
        if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
            unsub_flag = CORE_MQTT_UNSUB_PKT_TYPE | CORE_MQTT_UNSUB_PKT_RESERVE;
        } else {
            unsub_flag = CORE_MQTT_UNSUB_PKT_TYPE;
        }
        pkt[idx++] = unsub_flag;
    }

    /* Remaining Length */
    _core_mqtt_remain_len_encode(remainlen, &pkt[idx], &idx);

    /* Packet Id */
    packet_id = _core_mqtt_packet_id(mqtt_handle);
    pkt[idx++] = (uint8_t)((packet_id >> 8) & 0x00FF);
    pkt[idx++] = (uint8_t)((packet_id) & 0x00FF);

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        _core_write_general_prop(pkt, &idx, general_property, property_len_array, property_len_offset);
    }

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
        if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        }
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
        if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        }
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
                if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
                    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
                    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
                    if (mqtt_handle->network_handle != NULL) {
                        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
                    }
                    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
                    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
                }
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
        res = _core_mqtt_connect(mqtt_handle, NULL);
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
        if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
        }
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

static int32_t _core_mqtt_parse_pub_properties(core_mqtt_handle_t *mqtt_handle, uint8_t *property,
        pub_property_t *pub_prop, uint32_t *parsed_property_len, uint8_t *parsed_offset, uint32_t remain_len)
{
    general_property_t general_prop = {0};
    int32_t res = _core_mqtt_parse_general_properties(mqtt_handle, property, &general_prop, parsed_property_len,
                  parsed_offset, remain_len);
    if (res != STATE_SUCCESS) {
        return res;
    }

    pub_prop->message_expire_interval = general_prop.message_expire_interval;
    pub_prop->topic_alias = general_prop.topic_alias;
    memcpy(&(pub_prop->user_property[0]), &(general_prop.user_property[0]),
           USER_PROPERTY_MAX * (sizeof(user_property_t *)));
    pub_prop->subscription_identifier = general_prop.subscription_identifier;
    pub_prop->response_topic = general_prop.response_topic;
    pub_prop->correlation_data = general_prop.correlation_data;
    return STATE_SUCCESS;
}

uint8_t _core_mqtt_process_topic_alias(core_mqtt_handle_t *mqtt_handle, pub_property_t *pub_prop, uint32_t topic_len,
                                       char *topic,
                                       core_mqtt_topic_alias_node_t *alias_node)
{
    core_mqtt_topic_alias_node_t *topic_alias_node = NULL;

    uint8_t use_alias_topic = 0;
    if (NULL != pub_prop && pub_prop->topic_alias > 0) {
        if (topic_len > 0) {
            /* A topic appears first time during this connection. */
            core_mqtt_buff_t alias_topic = {
                .buffer = (uint8_t *)topic,
                .len = topic_len,
            };
            /* Insert it into topic alias list */
            _core_mqtt_topic_alias_list_insert(mqtt_handle, &alias_topic, pub_prop->topic_alias,
                                               &(mqtt_handle->rx_topic_alias_list));
        } else {
            /* topic len is 0. User previos topic alias */
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->topic_alias_mutex);
            core_list_for_each_entry(topic_alias_node, &mqtt_handle->rx_topic_alias_list, linked_node,
                                     core_mqtt_topic_alias_node_t) {
                if (topic_alias_node->topic_alias == pub_prop->topic_alias) {
                    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "found a rx topic alias\n");
                    use_alias_topic = 1;
                    memcpy(alias_node, topic_alias_node, sizeof(core_mqtt_topic_alias_node_t));
                    break;
                }
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->topic_alias_mutex);
        }
    }
    return use_alias_topic;
}

static int32_t _core_mqtt_pub_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len, uint8_t qos,
                                      uint32_t remain_len)
{
    uint8_t sub_found = 0;
    uint32_t idx = 0;
    uint16_t packet_id = 0;
    uint16_t utf8_strlen = 0;
    aiot_mqtt_recv_t packet;
    uint32_t topic_len = 0;
    void *userdata;
    core_mqtt_sub_node_t *sub_node = NULL;
    core_mqtt_topic_alias_node_t topic_alias_node = {0};

    core_mqtt_sub_handler_node_t *handler_node = NULL;
    struct core_list_head handler_list_copy;
    uint8_t use_alias_topic  = 0;
    pub_property_t pub_prop = {0};
    uint32_t total_prop_len = 0;

    if (input == NULL || len == 0 || qos > CORE_MQTT_QOS1) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }

    memset(&packet, 0, sizeof(aiot_mqtt_recv_t));

    packet.data.pub.qos = qos;
    packet.type = AIOT_MQTTRECV_PUB;

    /* Topic Length */
    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle) && len < 2) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }

    utf8_strlen = input[idx++] << 8;
    utf8_strlen |= input[idx++];

    packet.data.pub.topic = (char *)&input[idx];
    packet.data.pub.topic_len = utf8_strlen;
    idx += utf8_strlen;

    /* Packet Id For QOS1 */
    if (len < idx) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }
    if (qos == CORE_MQTT_QOS1) {
        if (len < idx + 2) {
            return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
        }
        packet_id = input[idx++] << 8;
        packet_id |= input[idx++];

    }

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        uint32_t property_len = 0;
        uint8_t offset = 0;

        int32_t res = _core_mqtt_parse_pub_properties(mqtt_handle, &input[idx], &pub_prop, &property_len, &offset, remain_len);
        if (res < STATE_SUCCESS) {
            return res;
        }

        total_prop_len = property_len + offset ;
        idx += total_prop_len;

        packet.data.pub.pub_prop = &pub_prop;
        use_alias_topic = _core_mqtt_process_topic_alias(mqtt_handle, &pub_prop, packet.data.pub.topic_len,
                          packet.data.pub.topic, &topic_alias_node);
    }

    /* Payload Len */
    if ((int64_t)len - CORE_MQTT_PUBLISH_TOPICLEN_LEN - (int64_t)packet.data.pub.topic_len < 0) {
        return STATE_MQTT_RECV_INVALID_PUBLISH_PACKET;
    }
    packet.data.pub.payload = &input[idx];
    packet.data.pub.payload_len = len - CORE_MQTT_PUBLISH_TOPICLEN_LEN - packet.data.pub.topic_len - total_prop_len;

    if (qos == CORE_MQTT_QOS1) {
        packet.data.pub.payload_len -= CORE_MQTT_PACKETID_LEN;
    }

    /* Publish Ack For QOS1 */
    if (qos == CORE_MQTT_QOS1) {
        _core_mqtt_puback_send(mqtt_handle, packet_id);
    }

    /* User previos topic alias */
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle) && 1 == use_alias_topic) {
        packet.data.pub.topic = topic_alias_node.topic;
        packet.data.pub.topic_len = strlen(packet.data.pub.topic);
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

    /* Reclaims Mem of User Properties */
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        _core_reclaim_user_properties_mem(mqtt_handle, pub_prop.user_property);
    }

    return STATE_SUCCESS;
}


void _core_mqtt_flow_control_inc(core_mqtt_handle_t *mqtt_handle)
{
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle) && mqtt_handle->flow_control_enabled) {
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
        mqtt_handle->server_receive_max++;
        mqtt_handle->server_receive_max = (mqtt_handle->server_receive_max > CORE_DEFAULT_SERVER_RECEIVE_MAX) ?
                                          CORE_DEFAULT_SERVER_RECEIVE_MAX : mqtt_handle->server_receive_max;
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);
    }
}

static int32_t _core_mqtt_puback_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len)
{
    aiot_mqtt_recv_t packet;

    /* even maltform puback received, flow control should be re-calculated as well */
    _core_mqtt_flow_control_inc(mqtt_handle);

    if (input == NULL) {
        return STATE_MQTT_RECV_INVALID_PUBACK_PACKET;
    }

    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle) && len != 2) {
        return STATE_MQTT_RECV_INVALID_PUBACK_PACKET;
    } else if (_core_mqtt_5_feature_is_enabled(mqtt_handle) && len < 2) {
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

static int32_t _core_mqtt_server_disconnect_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len)
{
    aiot_mqtt_recv_t packet;
    uint8_t reason_code;

    if (input == NULL) {
        return STATE_MQTT_RECV_INVALID_SERVER_DISCONNECT_PACKET;
    }

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle) && len < 2) {
        return STATE_MQTT_RECV_INVALID_SERVER_DISCONNECT_PACKET;
    }

    memset(&packet, 0, sizeof(aiot_mqtt_recv_t));
    packet.type = AIOT_MQTTRECV_DISCONNECT;

    reason_code = input[0];
    packet.data.server_disconnect.reason_code = reason_code;

    /* User Ctrl Packet Handler */
    if (mqtt_handle->recv_handler) {
        mqtt_handle->recv_handler((void *)mqtt_handle, &packet, mqtt_handle->userdata);
    }

    /* close socket connect with mqtt broker */
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
    if (mqtt_handle->network_handle != NULL) {
        mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);

    _core_mqtt_disconnect_event_notify(mqtt_handle, AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT);
    core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_DISCONNECT, "MQTT receives server disconnect, disconnect\r\n");

    return STATE_SUCCESS;
}

static void _core_mqtt_subunsuback_handler(core_mqtt_handle_t *mqtt_handle, uint8_t *input, uint32_t len,
        uint8_t packet_type)
{
    uint32_t idx = 0;
    aiot_mqtt_recv_t packet;
    int32_t res = STATE_SUCCESS;

    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle)) {
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
    } else {
        uint32_t idx = 0;
        uint32_t property_len = 0;
        uint8_t offset = 0;

        if (input == NULL || len == 0) {
            return;
        }

        /* Packet ID */
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
        idx += 2;

        /* Properties */
        res = _read_variable_byte_interger(&(input[idx]), &property_len,  &offset);
        if (res < STATE_SUCCESS) {
            return;
        }
        idx = idx + offset + property_len;

        /* Suback Flags */
        if (packet_type == CORE_MQTT_SUBACK_PKT_TYPE) {
            if (input[idx] == CORE_MQTT_SUBACK_RCODE_MAXQOS0 ||
                input[idx] == CORE_MQTT_SUBACK_RCODE_MAXQOS1 ||
                input[idx] == CORE_MQTT_SUBACK_RCODE_MAXQOS2) {
                packet.data.sub_ack.res = STATE_SUCCESS;
                packet.data.sub_ack.max_qos = input[idx];
            } else if (input[idx] == CORE_MQTT_SUBACK_RCODE_FAILURE) {
                packet.data.sub_ack.res = STATE_MQTT_SUBACK_RCODE_FAILURE;
            } else {
                packet.data.sub_ack.res = STATE_MQTT_SUBACK_RCODE_UNKNOWN;
            }
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
    mqtt_handle->topic_alias_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->process_handler_mutex = sysdep->core_sysdep_mutex_init();
    mqtt_handle->protocol_version = AIOT_MQTT_VERSION_3_1;
    mqtt_handle->tx_packet_max_size = CORE_TX_PKT_MAX_LENGTH;

    CORE_INIT_LIST_HEAD(&mqtt_handle->sub_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->pub_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->process_data_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->rx_topic_alias_list);
    CORE_INIT_LIST_HEAD(&mqtt_handle->tx_topic_alias_list);

    mqtt_handle->exec_enabled = 1;
    mqtt_handle->server_receive_max = CORE_DEFAULT_SERVER_RECEIVE_MAX;

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
        case AIOT_MQTTOPT_VERSION: {
            uint8_t version = *(uint8_t *)data;
            mqtt_handle->protocol_version = version;
        }
        break;
        case  AIOT_MQTTOPT_ASSIGNED_CLIENTID: {
            uint8_t use_assigned_clientid = *(uint8_t *)data;
            mqtt_handle->use_assigned_clientid = use_assigned_clientid;
        }
        break;
        case AIOT_MQTTOPT_FLOW_CONTROL_ENABLED: {
            mqtt_handle->flow_control_enabled = *(uint8_t *)data;
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

    _core_mqtt_topic_alias_list_remove_all(mqtt_handle);

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
    if (mqtt_handle->pre_connect_property != NULL) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->pre_connect_property);
    }

    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->data_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->recv_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->sub_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->topic_alias_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->pub_mutex);
    mqtt_handle->sysdep->core_sysdep_mutex_deinit(&mqtt_handle->process_handler_mutex);

    _core_mqtt_sublist_destroy(mqtt_handle);
    _core_mqtt_publist_destroy(mqtt_handle);
    _core_mqtt_process_datalist_destroy(mqtt_handle);

    core_global_deinit(mqtt_handle->sysdep);

    mqtt_handle->sysdep->core_sysdep_free(mqtt_handle);

    return STATE_SUCCESS;
}

static int32_t _mqtt_connect_with_prop(void *handle, conn_property_t *connect_property)
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

    res = _core_mqtt_connect(mqtt_handle, connect_property);

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

int32_t aiot_mqtt_connect(void *handle)
{
    return _mqtt_connect_with_prop(handle, NULL);
}

int32_t aiot_mqtt_connect_with_prop(void *handle, conn_property_t *connect_property)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        return STATE_MQTT_INVALID_PROTOCOL_VERSION;
    }

    /* clear previous connection properties */
    if (NULL != mqtt_handle->pre_connect_property) {
        mqtt_handle->sysdep->core_sysdep_free(mqtt_handle->pre_connect_property);
        mqtt_handle->pre_connect_property = NULL;
    }
    return _mqtt_connect_with_prop(handle, connect_property);
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

uint32_t _core_get_general_property_len(general_property_t *general_prop)
{
    uint32_t property_len = 0;
    uint8_t subscription_identifier[4] = {0};
    uint32_t subscription_identifer_offset = 0;
    user_property_t **list = general_prop->user_property;
    uint8_t iter = 0;

    if (general_prop->topic_alias > 0) {
        /* Topic Alias */
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_TOPIC_ALIAS_LEN;
    }

    if (general_prop->topic_alias_max > 0) {
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_TOPIC_ALIAS_MAX_LEN;
    }

    /* receive max */
    if (general_prop->client_receive_max > 0) {
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_RECEIVE_MAX_LEN;
    }

    /* User Properties */
    for (iter = 0; iter < USER_PROPERTY_MAX; iter++) {
        user_property_t *user_prop = list[iter];
        if (NULL != user_prop) {
            property_len += CORE_MQTT_V5_PROPERTY_ID_LEN;         /* len of user property ID */
            property_len += CORE_MQTT_V5_USER_PROPERTY_KEY_LEN;   /* len of user property key's len */
            property_len += user_prop->key.len;
            property_len += CORE_MQTT_V5_USER_PROPERTY_VALUE_LEN; /* len of user property value's len */
            property_len += user_prop->value.len;
        }
    }
    /* Response Topic */
    if (0 != general_prop->response_topic.len) {
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_RESPONSE_TOPIC_LEN + general_prop->response_topic.len;
    }

    /* Corelation Data */
    if (0 != general_prop->correlation_data.len) {
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_CORELATION_DATA_LEN + general_prop->correlation_data.len;
    }

    /* Subscription Identifier */
    if (general_prop->subscription_identifier > 0) {
        _core_mqtt_remain_len_encode(general_prop->subscription_identifier, &subscription_identifier[0],
                                     &subscription_identifer_offset);
        property_len += CORE_MQTT_V5_PROPERTY_ID_LEN + subscription_identifer_offset;
    }

    len_value_t *reason_string = general_prop->reason_string;

    if (NULL != reason_string) {
        property_len += reason_string->len + CORE_MQTT_V5_PROPERTY_ID_LEN + CORE_MQTT_V5_REASON_STRING_LEN;
    }

    return property_len;
}

static int32_t _core_mqtt_tx_topic_alias_process(core_mqtt_handle_t *mqtt_handle, core_mqtt_buff_t *topic,
        uint16_t *alias_id_prt
                                         )
{
    core_mqtt_topic_alias_node_t *topic_alias_node = NULL;
    uint16_t alias_id = 0;
    int32_t replace_topic_with_alias = 0;

    /* Topic Alias */
    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->topic_alias_mutex);
    core_list_for_each_entry(topic_alias_node, &mqtt_handle->tx_topic_alias_list, linked_node,
                             core_mqtt_topic_alias_node_t) {
        if (memcmp(topic_alias_node->topic, topic->buffer, topic->len) == 0) {
            core_log(mqtt_handle->sysdep, STATE_MQTT_LOG_CONNECT, "tx use topic alias\n");
            alias_id = topic_alias_node->topic_alias;
            replace_topic_with_alias = 1;
            break;
        }
    }
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->topic_alias_mutex);

    /* save the tx topic in tx_topic_alias_list */
    if (0 == replace_topic_with_alias) {
        alias_id = mqtt_handle->tx_topic_alias;
        alias_id++;
        /* make sure that it does not exceed tx_topic_alias_max */
        if (alias_id <= mqtt_handle->tx_topic_alias_max) {
            mqtt_handle->tx_topic_alias = alias_id;
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->topic_alias_mutex);
            _core_mqtt_topic_alias_list_insert(mqtt_handle, topic, alias_id, &(mqtt_handle->tx_topic_alias_list));
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->topic_alias_mutex);
        } else {
            /* exceed tx_topic_alias_max, no more alias could be used */
            alias_id = 0;
        }
    }
    *alias_id_prt = alias_id;
    return replace_topic_with_alias;
}

static int32_t _core_mqtt_check_flow_control(core_mqtt_handle_t *mqtt_handle, uint8_t qos)
{
    if (mqtt_handle->flow_control_enabled && qos == CORE_MQTT_QOS1) {
        uint16_t server_receive_max = 0;
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
        server_receive_max = mqtt_handle->server_receive_max;
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);

        if (server_receive_max <= 0) {
            return STATE_MQTT_RECEIVE_MAX_EXCEEDED;
        }
    }
    return STATE_SUCCESS;
}

static void _core_mqtt_check_flow_dec(core_mqtt_handle_t *mqtt_handle)
{
    if (_core_mqtt_5_feature_is_enabled(mqtt_handle) && mqtt_handle->flow_control_enabled
        && mqtt_handle->server_receive_max > 0) {
        mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->pub_mutex);
        mqtt_handle->server_receive_max--;
        mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->pub_mutex);
    }
}

static int32_t _core_mqtt_check_tx_payload_len(uint32_t pkt_len, core_mqtt_handle_t *mqtt_handle)
{
    if (pkt_len >= mqtt_handle->tx_packet_max_size) {
        _core_mqtt_exec_dec(mqtt_handle);
        return STATE_MQTT_INVALID_TX_PACK_SIZE;
    }
    return STATE_SUCCESS;
}

static int32_t _core_mqtt_pub(void *handle, core_mqtt_buff_t *topic, core_mqtt_buff_t *payload, uint8_t qos,
                              pub_property_t *pub_prop)
{
    int32_t res = STATE_SUCCESS;
    uint16_t packet_id = 0;
    uint8_t *pkt = NULL;
    uint32_t idx = 0, remainlen = 0, pkt_len = 0;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;
    uint16_t alias_id = 0;
    uint8_t replace_topic_with_alias = 0;
    uint32_t property_len = 0;
    uint8_t property_len_array[4] = {0};
    uint32_t property_len_offset = 0;
    general_property_t general_prop = {0};


    if (mqtt_handle == NULL ||
        payload == NULL || payload->buffer == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (topic->len == 0 || qos > CORE_MQTT_QOS_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {

        if (NULL != pub_prop) {
            memcpy(&(general_prop.user_property[0]), &(pub_prop->user_property[0]),
                   USER_PROPERTY_MAX * (sizeof(user_property_t *)));
            general_prop.response_topic = pub_prop->response_topic;
            general_prop.correlation_data = pub_prop->correlation_data;
            general_prop.subscription_identifier = pub_prop->subscription_identifier;
        }

        res = _core_mqtt_general_property_is_valid(&general_prop);
        if (res < STATE_SUCCESS) {
            return res;
        }

        /* Flow Control Check*/
        res = _core_mqtt_check_flow_control(mqtt_handle, qos);
        if (res < STATE_SUCCESS) {
            return res;
        }

        /* Topic Alias*/
        replace_topic_with_alias = _core_mqtt_tx_topic_alias_process(mqtt_handle, topic, &alias_id);
        general_prop.topic_alias = alias_id;
    }

    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    core_log2(mqtt_handle->sysdep, STATE_MQTT_LOG_TOPIC, "pub: %.*s\r\n", &topic->len, topic->buffer);
    core_log_hexdump(STATE_MQTT_LOG_HEXDUMP, '>', payload->buffer, payload->len);

    if (0 == replace_topic_with_alias) {
        remainlen = topic->len + payload->len + CORE_MQTT_UTF8_STR_EXTRA_LEN;
    } else {
        remainlen = payload->len + CORE_MQTT_UTF8_STR_EXTRA_LEN;
    }

    if (qos == CORE_MQTT_QOS1) {
        remainlen += CORE_MQTT_PACKETID_LEN;
    }

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        property_len = _core_get_general_property_len(&general_prop);
        _core_mqtt_remain_len_encode(property_len, &property_len_array[0],
                                     &property_len_offset);
        remainlen += property_len + property_len_offset;
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

    /* Topic Length */
    if (replace_topic_with_alias == 0) {
        /* topic len is > 0 when alias DOES NOT replaces topic */
        _core_mqtt_set_utf8_encoded_str((uint8_t *)topic->buffer, topic->len, &pkt[idx]);
        idx += CORE_MQTT_UTF8_STR_EXTRA_LEN + topic->len;
    } else {
        /* topic len is 0 when alias replaces topic */
        pkt[idx++] = 0;
        pkt[idx++] = 0;
    }

    /* Packet Id For QOS 1*/
    if (qos == CORE_MQTT_QOS1) {
        packet_id = _core_mqtt_packet_id(handle);
        pkt[idx++] = (uint8_t)((packet_id >> 8) & 0x00FF);
        pkt[idx++] = (uint8_t)((packet_id) & 0x00FF);
    }

    if (_core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        _core_write_general_prop(pkt, &idx,  &general_prop, property_len_array, property_len_offset);
    }

    /* Payload */
    memcpy(&pkt[idx], payload->buffer, payload->len);
    idx += payload->len;

    pkt_len = idx;

    /* Ensure tx pack size does not overflow  */
    res = _core_mqtt_check_tx_payload_len(pkt_len, mqtt_handle);
    if (res < STATE_SUCCESS) {
        mqtt_handle->sysdep->core_sysdep_free(pkt);
        return res;
    }

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
        if (res != STATE_SYS_DEPEND_NWK_WRITE_LESSDATA) {
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

    mqtt_handle->sysdep->core_sysdep_free(pkt);

    if (qos == CORE_MQTT_QOS1) {
        _core_mqtt_exec_dec(mqtt_handle);
        _core_mqtt_check_flow_dec(mqtt_handle);
        return (int32_t)packet_id;
    }

    _core_mqtt_exec_dec(mqtt_handle);

    return STATE_SUCCESS;
}

int32_t _core_mqtt_disconnect_with_prop(void *handle,  uint8_t reason_code, disconn_property_t *discon_property)
{
    int32_t res = 0;
    uint32_t pkt_len = 0;
    uint8_t *pkt = NULL;
    uint32_t idx = 0;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;
    uint32_t property_len = 0;
    uint32_t property_len_offset = 0;
    uint8_t property_len_array[4] = {0};
    uint32_t pkt_len_offset = 0;
    uint8_t  pkt_len_array[4] = {0};
    uint32_t property_total_len = 0;
    uint32_t pkt_remain_len = 0;
    general_property_t general_property = {0};

    if (discon_property != NULL) {
        memcpy(&(general_property.user_property[0]), &(discon_property->user_property[0]),
               USER_PROPERTY_MAX * (sizeof(user_property_t *)));
        general_property.reason_string = discon_property->reason_string;
        int res = _core_mqtt_general_property_is_valid(&general_property);
        if (res < STATE_SUCCESS) {
            return res;
        }
        property_len = _core_get_general_property_len(&general_property);
    }

    _core_mqtt_remain_len_encode(property_len, property_len_array, &property_len_offset);
    property_total_len = property_len_offset + property_len;

    pkt_remain_len = property_total_len + CORE_MQTT_V5_DISCONNECT_REASON_CODE_LEN;
    _core_mqtt_remain_len_encode(pkt_remain_len, pkt_len_array, &pkt_len_offset);

    pkt_len =  CORE_MQTT_FIXED_HEADER_LEN + pkt_len_offset + pkt_remain_len ;
    pkt = mqtt_handle->sysdep->core_sysdep_malloc(pkt_len, CORE_MQTT_MODULE_NAME);
    if (NULL == pkt) {
        return STATE_PORT_MALLOC_FAILED;
    }
    memset(pkt, 0, pkt_len);

    pkt[idx++] = CORE_MQTT_DISCONNECT_PKT_TYPE;
    memcpy(&pkt[idx], pkt_len_array, pkt_len_offset);
    idx += pkt_len_offset;
    pkt[idx++] = reason_code;
    _core_write_general_prop(&pkt[0], &idx, &general_property, property_len_array, property_len_offset);

    mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
    res = _core_mqtt_write(handle, pkt, idx, mqtt_handle->send_timeout_ms);
    mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
    mqtt_handle->sysdep->core_sysdep_free(pkt);

    return res;
}

int32_t aiot_mqtt_disconnect_with_prop(void *handle, uint8_t reason_code, disconn_property_t *discon_property)
{
    int32_t res = STATE_SUCCESS;
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (0 == (_core_mqtt_5_feature_is_enabled(mqtt_handle))) {
        return STATE_MQTT_INVALID_PROTOCOL_VERSION;
    }

    if (mqtt_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_mqtt_exec_inc(mqtt_handle);

    mqtt_handle->disconnect_api_called = 1;

    /* send mqtt disconnect packet to mqtt broker first */
    res = _core_mqtt_disconnect_with_prop(handle, reason_code, discon_property);
    if (res <= STATE_SUCCESS) {
        _core_mqtt_exec_dec(mqtt_handle);
        return res;
    }
    _core_mqtt_topic_alias_list_remove_all(mqtt_handle);

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
    if (res > STATE_SUCCESS) {
        res = STATE_SUCCESS;
    }

    return res;
}

static int32_t _mqtt_pub_with_prop(void *handle, char *topic, uint8_t *payload, uint32_t payload_len, uint8_t qos,
                                   pub_property_t *pub_prop)
{
    core_mqtt_buff_t    topic_buff = {0};
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
    } else {
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

    res = _core_mqtt_pub(handle, &topic_buff, &payload_buff, qos, pub_prop);
    if (append_rid != 0) {
        mqtt_handle->sysdep->core_sysdep_free(topic_buff.buffer);
    }
    return res;
}

int32_t aiot_mqtt_pub_with_prop(void *handle, char *topic, uint8_t *payload, uint32_t payload_len, uint8_t qos,
                                pub_property_t *pub_prop)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (NULL == handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (0 == (_core_mqtt_5_feature_is_enabled(mqtt_handle))) {
        return STATE_MQTT_INVALID_PROTOCOL_VERSION;
    }
    return  _mqtt_pub_with_prop(handle, topic, payload, payload_len, qos, pub_prop);
}

int32_t aiot_mqtt_pub(void *handle, char *topic, uint8_t *payload, uint32_t payload_len, uint8_t qos)
{
    return  _mqtt_pub_with_prop(handle, topic, payload, payload_len, qos, NULL);
}

static int32_t _core_mqtt_sub(void *handle, core_mqtt_buff_t *topic, aiot_mqtt_recv_handler_t handler,
                              uint8_t qos,
                              void *userdata, sub_property_t *sub_prop)
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
    general_property_t general_property = {0};
    if (NULL != sub_prop) {
        memcpy(&(general_property.user_property[0]), &(sub_prop->user_property[0]),
               USER_PROPERTY_MAX * (sizeof(user_property_t *)));
    }

    /* send subscribe packet */
    res = _core_mqtt_subunsub(mqtt_handle, (char *)topic->buffer, topic->len, qos, CORE_MQTT_SUB_PKT_TYPE,
                              &general_property);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

static int32_t _mqtt_sub_with_prop(void *handle, char *topic, aiot_mqtt_recv_handler_t handler, uint8_t qos,
                                   void *userdata, sub_property_t *sub_prop)
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

    return _core_mqtt_sub(handle, &topic_buff, handler, qos, userdata, sub_prop);
}

int32_t aiot_mqtt_sub(void *handle, char *topic, aiot_mqtt_recv_handler_t handler, uint8_t qos, void *userdata)
{
    return _mqtt_sub_with_prop(handle, topic, handler, qos, userdata, NULL);
}

int32_t aiot_mqtt_sub_with_prop(void *handle, char *topic, aiot_mqtt_recv_handler_t handler, uint8_t qos,
                                void *userdata, sub_property_t *sub_prop)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        return STATE_MQTT_INVALID_PROTOCOL_VERSION;
    }

    return _mqtt_sub_with_prop(mqtt_handle, topic, handler, qos, userdata, sub_prop);
}

static int32_t _core_mqtt_unsub(void *handle, core_mqtt_buff_t *topic, unsub_property_t *unsub_prop)
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

    general_property_t general_property = {0};
    if (NULL != unsub_prop) {
        memcpy(&(general_property.user_property[0]), &(unsub_prop->user_property[0]),
               USER_PROPERTY_MAX * (sizeof(user_property_t *)));
    }

    res = _core_mqtt_subunsub(mqtt_handle, (char *)topic->buffer, topic->len, 0, CORE_MQTT_UNSUB_PKT_TYPE,
                              &general_property);

    _core_mqtt_exec_dec(mqtt_handle);

    return res;
}

static int32_t _mqtt_unsub(void *handle, char *topic, unsub_property_t *unsub_prop)
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

    return _core_mqtt_unsub(handle, &topic_buff, unsub_prop);
}


int32_t aiot_mqtt_unsub(void *handle, char *topic)
{
    return _mqtt_unsub(handle, topic, NULL);
}

int32_t aiot_mqtt_unsub_with_prop(void *handle, char *topic, unsub_property_t *unsub_prop)
{
    core_mqtt_handle_t *mqtt_handle = (core_mqtt_handle_t *)handle;

    if (mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (0 == _core_mqtt_5_feature_is_enabled(mqtt_handle)) {
        return STATE_MQTT_INVALID_PROTOCOL_VERSION;
    }

    return _mqtt_unsub(handle, topic, unsub_prop);
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
            res = _core_mqtt_connect(mqtt_handle, NULL);
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
        } else {
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->send_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_lock(mqtt_handle->recv_mutex);
            if (mqtt_handle->network_handle != NULL) {
                mqtt_handle->sysdep->core_sysdep_network_deinit(&mqtt_handle->network_handle);
            }
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->recv_mutex);
            mqtt_handle->sysdep->core_sysdep_mutex_unlock(mqtt_handle->send_mutex);
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
            res = _core_mqtt_pub_handler(handle, remain, mqtt_remainlen, ((mqtt_pkt_reserved >> 1) & 0x03), mqtt_remainlen);
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
        case CORE_MQTT_SERVER_DISCONNECT_PKT_TYPE: {
            _core_mqtt_server_disconnect_handler(handle, remain, mqtt_remainlen);
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

