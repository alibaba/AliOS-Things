/**
 * @file aiot_shadow_api.c
 * @brief shadow模块的API接口实现, 提供更新, 删除, 获取设备影子的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "shadow_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "core_mqtt.h"


static int32_t _shadow_get_pk_dn(aiot_sysdep_portfile_t *sysdep, char *topic, uint16_t topic_len,
                                 char **product_key, char **device_name)
{
    uint16_t pk_offset = 12;    /* length of "/shadow/get/" */
    uint16_t idx = 0;
    uint16_t pk_len = 0, dn_len = 0;
    char *pk_pos = topic + pk_offset;
    char *dn_pos = NULL;
    char *tmp_pk = NULL, *tmp_dn = NULL;

    for (idx = pk_offset; idx < topic_len - 1; idx++) {
        if (topic[idx] == '/') {
            dn_pos = topic + idx + 1;
            pk_len = dn_pos - pk_pos - 1;
            dn_len = topic_len - idx - 1;
        }
    }

    if (pk_len == 0 || dn_len == 0) {
        return STATE_SHADOW_INTERNAL_TOPIC_ERROR;
    }

    tmp_pk = sysdep->core_sysdep_malloc(pk_len + 1, SHADOW_MODULE_NAME);
    if (tmp_pk == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(tmp_pk, 0, pk_len + 1);
    memcpy(tmp_pk, pk_pos, pk_len);

    tmp_dn = sysdep->core_sysdep_malloc(dn_len + 1, SHADOW_MODULE_NAME);
    if (tmp_dn == NULL) {
        sysdep->core_sysdep_free(tmp_pk);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(tmp_dn, 0, dn_len + 1);
    memcpy(tmp_dn, dn_pos, dn_len);

    *product_key = tmp_pk;
    *device_name = tmp_dn;
    return STATE_SUCCESS;
}

int32_t _shadow_int642str(int64_t input, char *output, uint8_t *output_len)
{
    uint64_t temp = 0;
    uint8_t len = 0;

    if (input < 0) {
        *output = '-';
        temp = -input;
        core_uint642str(temp, output + 1, &len);
        if (output_len != NULL) {
            *output_len = len + 1;
        }
    } else {
        temp = input;
        core_uint642str(temp, output, output_len);
    }

    return STATE_SUCCESS;
}

static void _shadow_recv_message_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata)
{
    shadow_handle_t *shadow_handle = (shadow_handle_t *)userdata;
    aiot_shadow_recv_t recv;
    char *value = NULL;
    uint32_t value_len = 0;
    char *pk = NULL;
    char *dn = NULL;
    int32_t res = STATE_SUCCESS;

    char *method = NULL;
    uint32_t method_len = 0;
    char *payload = NULL;
    uint32_t payload_len = 0;
    uint64_t timestamp = 0;
    uint64_t version = 0;
    char status[10] = { 0 };
    char *version_str = NULL;
    uint32_t version_strlen = 0;

    if (NULL == shadow_handle->recv_handler) {
        return;
    }

    memset(&recv, 0, sizeof(aiot_shadow_recv_t));

    do {
        /* get pk & dn */
        if ((res = _shadow_get_pk_dn(shadow_handle->sysdep, msg->data.pub.topic, msg->data.pub.topic_len, &pk, &dn)) < 0) {
            break;
        }
        recv.product_key = pk;
        recv.device_name = dn;

        /* parse the message */
        if ((res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                   SHADOW_JSON_KEY_METHOD, strlen(SHADOW_JSON_KEY_METHOD), &method, &method_len) < 0) ||
            (res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                   SHADOW_JSON_KEY_TIMESTAMP, strlen(SHADOW_JSON_KEY_TIMESTAMP), &value, &value_len) < 0) ||
            (res = core_str2uint64(value, value_len, &timestamp) < 0) ||
            (res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                   SHADOW_JSON_KEY_PAYLOAD, strlen(SHADOW_JSON_KEY_PAYLOAD), &payload, &payload_len) < 0)) {
            break;
        }

        if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                            SHADOW_JSON_KEY_VERSION, strlen(SHADOW_JSON_KEY_VERSION), &version_str, &version_strlen) == STATE_SUCCESS) {
            core_str2uint64(version_str, version_strlen, &version);
        }

        /* control message */
        if (method_len == strlen("control") && !memcmp(method, "control", method_len)) {
            /* get version */
            if (version_str == NULL) {
                break;
            }

            core_log(shadow_handle->sysdep, STATE_SHADOW_LOG_RECV, "SHADOW recv control message\r\n");
            recv.type = AIOT_SHADOWRECV_CONTROL;
            recv.data.control.payload = payload;
            recv.data.control.payload_len = payload_len;
            recv.data.control.version = version;
        } /* reply message */
        else if (method_len == strlen("reply") && !memcmp(method, "reply", method_len)) {
            /* get_reply */
            if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                SHADOW_JSON_KEY_STATE, strlen(SHADOW_JSON_KEY_STATE), &value, &value_len) < 0) {

                if ((res = core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                           SHADOW_JSON_KEY_STATUS, strlen(SHADOW_JSON_KEY_STATUS), &value, &value_len) < 0) ||
                    (value_len >= sizeof(status))) {
                    break;
                }

                core_log(shadow_handle->sysdep, STATE_SHADOW_LOG_RECV, "SHADOW recv get_reply message\r\n");
                memcpy(status, value, value_len);
                recv.type = AIOT_SHADOWRECV_GENERIC_REPLY;
                recv.data.generic_reply.payload = payload;
                recv.data.generic_reply.payload_len = payload_len;
                recv.data.generic_reply.status = status;
                recv.data.generic_reply.timestamp = timestamp;
            } /* get_reply */
            else {
                if (version_str == NULL) {
                    break;
                }

                core_log(shadow_handle->sysdep, STATE_SHADOW_LOG_RECV, "SHADOW recv generic_reply message\r\n");
                recv.type = AIOT_SHADOWRECV_GET_REPLY;
                recv.data.get_reply.payload = payload;
                recv.data.get_reply.payload_len = payload_len;
                recv.data.get_reply.version = version;
            }
        } else {
            break;
        }

        shadow_handle->recv_handler(shadow_handle, &recv, shadow_handle->userdata);
        shadow_handle->sysdep->core_sysdep_free(pk);
        shadow_handle->sysdep->core_sysdep_free(dn);
        return;
    } while (0);

    /* invalid message log */
    core_log(shadow_handle->sysdep, SATAE_SHADOW_LOG_PARSE_RECV_MSG_FAILED, "SHADOW parse recv message failed\r\n");

    if (pk != NULL) {
        shadow_handle->sysdep->core_sysdep_free(pk);
    }
    if (dn != NULL) {
        shadow_handle->sysdep->core_sysdep_free(dn);
    }
}

static void _shadow_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    shadow_handle_t *shadow_handle = (shadow_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                shadow_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _shadow_core_mqtt_operate_process_handler(shadow_handle_t *shadow_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _shadow_core_mqtt_process_handler;
    process_data.context = shadow_handle;

    return core_mqtt_setopt(shadow_handle->mqtt_handle, option, &process_data);
}

void *aiot_shadow_init(void)
{
    aiot_sysdep_portfile_t *sysdep = aiot_sysdep_get_portfile();
    shadow_handle_t *shadow_handle = NULL;

    if (NULL == sysdep) {
        return NULL;
    }

    shadow_handle = sysdep->core_sysdep_malloc(sizeof(shadow_handle_t), SHADOW_MODULE_NAME);
    if (NULL == shadow_handle) {
        return NULL;
    }

    memset(shadow_handle, 0, sizeof(shadow_handle_t));
    shadow_handle->sysdep = sysdep;

    return shadow_handle;
}

int32_t aiot_shadow_setopt(void *handle, aiot_shadow_option_t option, void *data)
{
    shadow_handle_t *shadow_handle;
    int32_t res = STATE_SUCCESS;

    if (NULL == handle || NULL == data) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (option >= AIOT_SHADOWOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    shadow_handle = (shadow_handle_t *)handle;

    switch (option) {
        case AIOT_SHADOWOPT_MQTT_HANDLE: {
            aiot_mqtt_topic_map_t topic_mapping;
            shadow_handle->mqtt_handle = data;

            /* setup mqtt topic mapping */
            topic_mapping.topic = SHADOW_GET_TOPIC;
            topic_mapping.handler = _shadow_recv_message_handler;
            topic_mapping.userdata = handle;

            res = aiot_mqtt_setopt(data, AIOT_MQTTOPT_APPEND_TOPIC_MAP, &topic_mapping);
            if (res >= STATE_SUCCESS) {
                res = _shadow_core_mqtt_operate_process_handler(shadow_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
        }
        break;
        case AIOT_SHADOWOPT_RECV_HANDLER: {
            shadow_handle->recv_handler = (aiot_shadow_recv_handler_t)data;
        }
        break;
        case AIOT_SHADOWOPT_USERDATA: {
            shadow_handle->userdata = data;
        }
        break;
        default:
            break;
    }

    return res;
}

int32_t aiot_shadow_send(void *handle, aiot_shadow_msg_t *msg)
{
    shadow_handle_t *shadow_handle = NULL;
    char *topic = NULL;
    char *payload = NULL;
    int32_t res = STATE_SUCCESS;

    if (NULL == handle || NULL == msg) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (msg->type >= AIOT_SHADOWMSG_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    shadow_handle = (shadow_handle_t *)handle;
    if (NULL == shadow_handle->mqtt_handle) {
        return STATE_SHADOW_MQTT_HANDLE_IS_NULL;
    }

    /* construct mqtt topic */
    {
        char *src[2];
        char *pk = NULL, *dn = NULL;

        pk = core_mqtt_get_product_key(shadow_handle->mqtt_handle);
        dn = core_mqtt_get_device_name(shadow_handle->mqtt_handle);

        if (NULL == msg->product_key && NULL == pk) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (NULL == msg->device_name && NULL == dn) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }

        src[0] = (msg->product_key != NULL) ? msg->product_key : pk;
        src[1] = (msg->device_name != NULL) ? msg->device_name : dn;
        res = core_sprintf(shadow_handle->sysdep, &topic, SHADOW_UPDATE_TOPIC_FMT, src, sizeof(src) / sizeof(char *),
                           SHADOW_MODULE_NAME);
        if (NULL == topic) {
            return res;
        }
    }

    /* construct payload */
    switch (msg->type) {
        case AIOT_SHADOWMSG_UPDATE: {
            char version_string[21] = { 0 };
            char *src[3] = { NULL };

            if (NULL == msg->data.update.reported) {
                res = STATE_SHADOW_REPORTED_DATA_IS_NULL;
                break;
            }

            _shadow_int642str(msg->data.update.version, version_string, NULL);
            src[0] = "update";
            src[1] = msg->data.update.reported;
            src[2] = version_string;

            res = core_sprintf(shadow_handle->sysdep, &payload, SHADOW_PAYLOAD_REQ_FMT,
                               src, sizeof(src) / sizeof(char *), SHADOW_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                break;
            }
        }
        break;
        case AIOT_SHADOWMSG_CLEAN_DESIRED: {
            char version_string[21] = { 0 };
            char *src[1] = { NULL };

            _shadow_int642str(msg->data.clean_desired.version, version_string, NULL);
            src[0] = version_string;

            res = core_sprintf(shadow_handle->sysdep, &payload, SHADOW_PAYLOAD_CLEAN_FMT,
                               src, sizeof(src) / sizeof(char *), SHADOW_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                break;
            }
        }
        break;
        case AIOT_SHADOWMSG_GET: {
            payload = SHADOW_PAYLOAD_GET;
        }
        break;
        case AIOT_SHADOWMSG_DELETE_REPORTED: {
            char version_string[21] = { 0 };
            char *src[3] = { NULL };

            if (NULL == msg->data.delete_reporte.reported) {
                res = STATE_SHADOW_REPORTED_DATA_IS_NULL;
                break;
            }

            _shadow_int642str(msg->data.delete_reporte.version, version_string, NULL);
            src[0] = "delete";
            src[1] = msg->data.update.reported;
            src[2] = version_string;

            res = core_sprintf(shadow_handle->sysdep, &payload, SHADOW_PAYLOAD_REQ_FMT,
                               src, sizeof(src) / sizeof(char *), SHADOW_MODULE_NAME);
            if (res < STATE_SUCCESS) {
                break;
            }
        }
        break;
        default: {
            res = STATE_USER_INPUT_OUT_RANGE;
        }
        break;
    }

    if (NULL == payload) {
        shadow_handle->sysdep->core_sysdep_free(topic);
        return res;
    }

    res = aiot_mqtt_pub(shadow_handle->mqtt_handle, topic, (uint8_t *)payload, strlen(payload), 0);

    shadow_handle->sysdep->core_sysdep_free(topic);
    if (msg->type != AIOT_SHADOWMSG_GET) {
        shadow_handle->sysdep->core_sysdep_free(payload);
    }
    return res;
}

int32_t aiot_shadow_deinit(void **p_handle)
{
    shadow_handle_t *shadow_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    aiot_mqtt_topic_map_t topic_mapping;

    if (NULL == p_handle || NULL == *p_handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    shadow_handle = *p_handle;
    sysdep = shadow_handle->sysdep;
    *p_handle = NULL;

    _shadow_core_mqtt_operate_process_handler(shadow_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);

    /* remove mqtt topic mapping */
    memset(&topic_mapping, 0, sizeof(aiot_mqtt_topic_map_t));
    topic_mapping.topic = SHADOW_GET_TOPIC;
    topic_mapping.handler = _shadow_recv_message_handler;
    aiot_mqtt_setopt(shadow_handle->mqtt_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP, &topic_mapping);

    sysdep->core_sysdep_free(shadow_handle);
    return STATE_SUCCESS;
}

