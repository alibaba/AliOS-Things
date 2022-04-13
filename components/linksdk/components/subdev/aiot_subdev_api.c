/**
 * @file aiot_subdev_api.c
 * @brief subdev模块的API接口实现, 提供.......的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "subdev_private.h"

/* TODO: 列出对core模块需要包含的头文件 */
#include "core_global.h"
#include "core_string.h"
#include "core_log.h"
#include "core_sha256.h"
#include "core_mqtt.h"

#include "cJSON.h"

static int32_t _subdev_update_alink_id(subdev_handle_t *subdev_handle)
{
    int32_t res = STATE_SUCCESS, alink_id = 0;

    memset(subdev_handle->alink_id, 0, SUBDEV_ALINK_ID_MAX_LEN);
    core_global_alink_id_next(subdev_handle->sysdep, &alink_id);

    res = core_int2str(alink_id, subdev_handle->alink_id, NULL);
    if (res < STATE_SUCCESS) {
        return res;
    }

    return STATE_SUCCESS;
}

static void _subdev_topo_generic_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata, aiot_subdev_recv_type_t type, uint8_t pk_pos)
{
    subdev_handle_t *subdev_handle = (subdev_handle_t *)userdata;

    if (subdev_handle->recv_handler == NULL) {
        return;
    }

    switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            char *id = NULL, *code = NULL, *data = NULL, *data_str = NULL, *message = NULL, *message_str = NULL;
            uint32_t id_len = 0, code_len = 0, data_len = 0, message_len = 0, id_num = 0, code_num = 0;
            aiot_subdev_recv_t recv;

            if (core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "id", strlen("id"), &id, &id_len) < STATE_SUCCESS ||
                core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "code", strlen("code"), &code, &code_len) < STATE_SUCCESS ||
                core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "data", strlen("data"), &data, &data_len) < STATE_SUCCESS ||
                core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "message", strlen("message"), &message, &message_len) < STATE_SUCCESS) {
                aiot_subdev_event_t event;

                memset(&event, 0, sizeof(aiot_subdev_event_t));
                event.type = AIOT_SUBDEVEVT_INVALID_RESPONSE;

                if (subdev_handle->event_handler != NULL) {
                    subdev_handle->event_handler(subdev_handle, &event, subdev_handle->userdata);
                }
            }

            if (core_str2uint(id, id_len, &id_num) < STATE_SUCCESS) {
                aiot_subdev_event_t event;

                memset(&event, 0, sizeof(aiot_subdev_event_t));
                event.type = AIOT_SUBDEVEVT_INVALID_ID;

                if (subdev_handle->event_handler != NULL) {
                    subdev_handle->event_handler(subdev_handle, &event, subdev_handle->userdata);
                }
            }

            if (core_str2uint(code, code_len, &code_num) < STATE_SUCCESS) {
                aiot_subdev_event_t event;

                memset(&event, 0, sizeof(aiot_subdev_event_t));
                event.type = AIOT_SUBDEVEVT_INVALID_CODE;

                if (subdev_handle->event_handler != NULL) {
                    subdev_handle->event_handler(subdev_handle, &event, subdev_handle->userdata);
                }
            }

            data_str = subdev_handle->sysdep->core_sysdep_malloc(data_len + 1, SUBDEV_MODULE_NAME);
            if (data_str == NULL) {
                return;
            }
            memset(data_str, 0, data_len + 1);
            memcpy(data_str, data, data_len);

            message_str = subdev_handle->sysdep->core_sysdep_malloc(message_len + 1, SUBDEV_MODULE_NAME);
            if (message_str == NULL) {
                subdev_handle->sysdep->core_sysdep_free(data_str);
                return;
            }
            memset(message_str, 0, message_len + 1);
            memcpy(message_str, message, message_len);

            memset(&recv, 0, sizeof(aiot_subdev_recv_t));
            recv.type = type;

            recv.data.generic_reply.msg_id = id_num;
            recv.data.generic_reply.code = code_num;
            recv.data.generic_reply.data = data_str;
            recv.data.generic_reply.message = message_str;

            strtok(packet->data.pub.topic, "/");

            while((--pk_pos - 1) > 0) {
                strtok(NULL, "/");
            }
            recv.data.generic_reply.product_key = strtok(NULL, "/");
            recv.data.generic_reply.device_name = strtok(NULL, "/");

            subdev_handle->recv_handler(subdev_handle, &recv, subdev_handle->userdata);

            subdev_handle->sysdep->core_sysdep_free(data_str);
            subdev_handle->sysdep->core_sysdep_free(message_str);
        }
        break;
        default: {
            break;
        }
    }
}

static void _subdev_topo_generic_notify_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata, aiot_subdev_recv_type_t type, uint8_t pk_pos)
{
    subdev_handle_t *subdev_handle = (subdev_handle_t *)userdata;

    if (subdev_handle->recv_handler == NULL) {
        return;
    }

    switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            char *id = NULL, *params = NULL, *params_str = NULL;
            uint32_t id_len = 0, params_len = 0, id_num = 0;
            aiot_subdev_recv_t recv;

            if (core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "id", strlen("id"), &id, &id_len) < STATE_SUCCESS ||
                core_json_value((const char *)packet->data.pub.payload, packet->data.pub.payload_len, "params", strlen("params"), &params, &params_len) < STATE_SUCCESS) {
                aiot_subdev_event_t event;

                memset(&event, 0, sizeof(aiot_subdev_event_t));
                event.type = AIOT_SUBDEVEVT_INVALID_RESPONSE;

                if (subdev_handle->event_handler != NULL) {
                    subdev_handle->event_handler(subdev_handle, &event, subdev_handle->userdata);
                }
            }

            if (core_str2uint(id, id_len, &id_num) < STATE_SUCCESS) {
                aiot_subdev_event_t event;

                memset(&event, 0, sizeof(aiot_subdev_event_t));
                event.type = AIOT_SUBDEVEVT_INVALID_ID;

                if (subdev_handle->event_handler != NULL) {
                    subdev_handle->event_handler(subdev_handle, &event, subdev_handle->userdata);
                }
            }

            params_str = subdev_handle->sysdep->core_sysdep_malloc(params_len + 1, SUBDEV_MODULE_NAME);
            if (params_str == NULL) {
                return;
            }
            memset(params_str, 0, params_len + 1);
            memcpy(params_str, params, params_len);

            memset(&recv, 0, sizeof(aiot_subdev_recv_t));
            recv.type = type;

            recv.data.generic_notify.msg_id = id_num;
            recv.data.generic_notify.params = params_str;

            strtok(packet->data.pub.topic, "/");

            while((--pk_pos - 1) > 0) {
                strtok(NULL, "/");
            }
            recv.data.generic_notify.product_key = strtok(NULL, "/");
            recv.data.generic_notify.device_name = strtok(NULL, "/");

            subdev_handle->recv_handler(subdev_handle, &recv, subdev_handle->userdata);

            subdev_handle->sysdep->core_sysdep_free(params_str);
        }
        break;
        default: {
            break;
        }
    }
}

static void _subdev_topo_add_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_TOPO_ADD_REPLY, 2);   
}

static void _subdev_topo_delete_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_TOPO_DELETE_REPLY, 2);
}

static void _subdev_topo_get_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_TOPO_GET_REPLY, 2);
}

static void _subdev_batch_login_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_BATCH_LOGIN_REPLY, 3);
}

static void _subdev_batch_logout_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_BATCH_LOGOUT_REPLY, 3);
}

static void _subdev_sub_register_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_SUB_REGISTER_REPLY, 2);
}

static void _subdev_product_register_reply_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_reply_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_PRODUCT_REGISTER_REPLY, 2);
}

static void _subdev_topo_change_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    _subdev_topo_generic_notify_recv_handler(handle, packet, userdata, AIOT_SUBDEVRECV_TOPO_CHANGE_NOTIFY, 2);
}

static subdev_topic_map_t g_subdev_topic_map[] = {
    {SUBDEV_TOPIC_TOPO_ADD_REPLY,         _subdev_topo_add_reply_recv_handler         },
    {SUBDEV_TOPIC_TOPO_DELETE_REPLY,      _subdev_topo_delete_reply_recv_handler      },
    {SUBDEV_TOPIC_TOPO_GET_REPLY,         _subdev_topo_get_reply_recv_handler         },
    {SUBDEV_TOPIC_BATCH_LOGIN_REPLY,      _subdev_batch_login_reply_recv_handler      },
    {SUBDEV_TOPIC_BATCH_LOGOUT_REPLY,     _subdev_batch_logout_reply_recv_handler     },
    {SUBDEV_TOPIC_SUB_REGISTER_REPLY,     _subdev_sub_register_reply_recv_handler     },
    {SUBDEV_TOPIC_PRODUCT_REGISTER_REPLY, _subdev_product_register_reply_recv_handler },
    {SUBDEV_TOPIC_TOPO_CHANGE_NOTIFY,     _subdev_topo_change_recv_handler            }
};

static int32_t _subdev_operate_topic_map(subdev_handle_t *subdev_handle, aiot_mqtt_option_t option)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    aiot_mqtt_topic_map_t map;

    for (idx = 0;idx < sizeof(g_subdev_topic_map)/sizeof(subdev_topic_map_t);idx++) {
        memset(&map, 0, sizeof(aiot_mqtt_topic_map_t));
        map.topic = g_subdev_topic_map[idx].topic;
        map.handler = g_subdev_topic_map[idx].handler;
        map.userdata = (void *)subdev_handle;

        res = aiot_mqtt_setopt(subdev_handle->mqtt_handle, option, &map);
        if (res < STATE_SUCCESS) {
            return res;
        }
    }

    return res;
}

static int32_t _subdev_send_message(subdev_handle_t *subdev_handle, char *topic_fmt, cJSON *root)
{
    int32_t res = STATE_SUCCESS;
    char *topic = NULL, *payload = NULL;
    char *topic_src[] = { core_mqtt_get_product_key(subdev_handle->mqtt_handle), core_mqtt_get_device_name(subdev_handle->mqtt_handle) };

    res = core_sprintf(subdev_handle->sysdep, &topic, topic_fmt, topic_src, sizeof(topic_src)/sizeof(char *), SUBDEV_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    payload = cJSON_PrintUnformatted(root);
    if (payload == NULL) {
        subdev_handle->sysdep->core_sysdep_free(topic);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    res = aiot_mqtt_pub(subdev_handle->mqtt_handle, topic, (uint8_t *)payload, (uint32_t)strlen(payload), 0);
    subdev_handle->sysdep->core_sysdep_free(topic);
    subdev_handle->sysdep->core_sysdep_free(payload);

    return res;
}

static int32_t _subdev_calculate_sign(subdev_handle_t *subdev_handle, aiot_subdev_dev_t *dev, char *timestamp, char sign_str[65])
{
    int32_t res = STATE_SUCCESS;
    uint8_t sign[32] = {0};
    char *plain_text = NULL;
    char *plain_text_src[] = { dev->product_key, dev->device_name, dev->device_name, dev->product_key, timestamp };
    char *plain_text_fmt = "clientId%s.%sdeviceName%sproductKey%stimestamp%s";

    res = core_sprintf(subdev_handle->sysdep, &plain_text, plain_text_fmt, plain_text_src, sizeof(plain_text_src)/sizeof(char *), SUBDEV_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    core_hmac_sha256((uint8_t *)plain_text, (uint32_t)strlen(plain_text), (uint8_t *)dev->device_secret, (uint32_t)strlen(dev->device_secret), sign);
    core_hex2str(sign, 32, sign_str, 0);

    subdev_handle->sysdep->core_sysdep_free(plain_text);

    return STATE_SUCCESS;
}

static int32_t _subdev_calculate_product_register_sign(subdev_handle_t *subdev_handle, aiot_subdev_dev_t *dev, char *random, char sign_str[65])
{
    int32_t res = STATE_SUCCESS;
    uint8_t sign[32] = {0};
    char *plain_text = NULL;
    char *plain_text_src[] = { dev->device_name, dev->product_key, random };
    char *plain_text_fmt = "deviceName%sproductKey%srandom%s";

    res = core_sprintf(subdev_handle->sysdep, &plain_text, plain_text_fmt, plain_text_src, sizeof(plain_text_src)/sizeof(char *), SUBDEV_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    core_hmac_sha256((uint8_t *)plain_text, (uint32_t)strlen(plain_text), (uint8_t *)dev->product_secret, (uint32_t)strlen(dev->product_secret), sign);
    core_hex2str(sign, 32, sign_str, 0);

    subdev_handle->sysdep->core_sysdep_free(plain_text);

    return STATE_SUCCESS;
}

static int32_t _subdev_topo_add_append_params(subdev_handle_t *subdev_handle, cJSON *params, aiot_subdev_dev_t *dev, char *timestamp)
{
    int32_t res = STATE_SUCCESS;
    cJSON *param = NULL;
    char *client_id = NULL, sign_str[65] = {0};

    param = cJSON_CreateObject();
    if (param == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    client_id = subdev_handle->sysdep->core_sysdep_malloc(strlen(dev->product_key) + strlen(dev->device_name) + 2, SUBDEV_MODULE_NAME);
    if (client_id == NULL) {
        cJSON_Delete(param);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(client_id, 0, strlen(dev->product_key) + strlen(dev->device_name) + 2);
    memcpy(client_id, dev->product_key, strlen(dev->product_key));
    memcpy(client_id + strlen(client_id), ".", strlen("."));
    memcpy(client_id + strlen(client_id), dev->device_name, strlen(dev->device_name));

    cJSON_AddStringToObject(param, "productKey", dev->product_key);
    cJSON_AddStringToObject(param, "deviceName", dev->device_name);
    cJSON_AddStringToObject(param, "clientId", client_id);
    cJSON_AddStringToObject(param, "timestamp", timestamp);
    cJSON_AddStringToObject(param, "signmethod", "hmacSha256");

    subdev_handle->sysdep->core_sysdep_free(client_id);

    res = _subdev_calculate_sign(subdev_handle, dev, timestamp, sign_str);
    if (res < STATE_SUCCESS) {
        cJSON_Delete(param);
        return res;
    }

    cJSON_AddStringToObject(param, "sign", sign_str);

    cJSON_AddItemToArray(params, param);

    return STATE_SUCCESS;
}

static int32_t _subdev_append_pk_dn_to_params(subdev_handle_t *subdev_handle, cJSON *params, aiot_subdev_dev_t *dev)
{
    cJSON *param = NULL;

    param = cJSON_CreateObject();
    if (param == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(param, "productKey", dev->product_key);
    cJSON_AddStringToObject(param, "deviceName", dev->device_name);

    cJSON_AddItemToArray(params, param);

    return STATE_SUCCESS;
}

static int32_t _subdev_topo_delete_append_params(subdev_handle_t *subdev_handle, cJSON *params, aiot_subdev_dev_t *dev)
{
    return _subdev_append_pk_dn_to_params(subdev_handle, params, dev);
}

static int32_t _subdev_batch_login_append_device_list(subdev_handle_t *subdev_handle, cJSON *device_list, aiot_subdev_dev_t *dev, char *timestamp)
{
    int32_t res = STATE_SUCCESS;
    cJSON *param = NULL;
    char *client_id = NULL, sign_str[65] = {0};

    param = cJSON_CreateObject();
    if (param == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    client_id = subdev_handle->sysdep->core_sysdep_malloc(strlen(dev->product_key) + strlen(dev->device_name) + 2, SUBDEV_MODULE_NAME);
    if (client_id == NULL) {
        cJSON_Delete(param);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(client_id, 0, strlen(dev->product_key) + strlen(dev->device_name) + 2);
    memcpy(client_id, dev->product_key, strlen(dev->product_key));
    memcpy(client_id + strlen(client_id), ".", strlen("."));
    memcpy(client_id + strlen(client_id), dev->device_name, strlen(dev->device_name));

    cJSON_AddStringToObject(param, "productKey", dev->product_key);
    cJSON_AddStringToObject(param, "deviceName", dev->device_name);
    cJSON_AddStringToObject(param, "clientId", client_id);
    cJSON_AddStringToObject(param, "timestamp", timestamp);
    cJSON_AddStringToObject(param, "cleanSession", "false");

    subdev_handle->sysdep->core_sysdep_free(client_id);

    res = _subdev_calculate_sign(subdev_handle, dev, timestamp, sign_str);
    if (res < STATE_SUCCESS) {
        cJSON_Delete(param);
        return res;
    }

    cJSON_AddStringToObject(param, "sign", sign_str);

    cJSON_AddItemToArray(device_list, param);

    return STATE_SUCCESS;
}

static int32_t _subdev_batch_logout_append_params(subdev_handle_t *subdev_handle, cJSON *params, aiot_subdev_dev_t *dev)
{
    return _subdev_append_pk_dn_to_params(subdev_handle, params, dev);
}

static int32_t _subdev_sub_register_append_params(subdev_handle_t *subdev_handle, cJSON *params, aiot_subdev_dev_t *dev)
{
    return _subdev_append_pk_dn_to_params(subdev_handle, params, dev);
}

static int32_t _subdev_product_register_append_params(subdev_handle_t *subdev_handle, cJSON *proxieds, aiot_subdev_dev_t *dev)
{
    int32_t res = STATE_SUCCESS;
    cJSON *param = NULL;
    uint8_t random[10] = {0};
    char random_str[21] = {0}, sign_str[65] = {0};

    param = cJSON_CreateObject();
    if (param == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    subdev_handle->sysdep->core_sysdep_rand(random, 10);
    core_hex2str(random, 10, random_str, 0);

    cJSON_AddStringToObject(param, "productKey", dev->product_key);
    cJSON_AddStringToObject(param, "deviceName", dev->device_name);
    cJSON_AddStringToObject(param, "random", random_str);
    cJSON_AddStringToObject(param, "signMethod", "hmacSha256");


    res = _subdev_calculate_product_register_sign(subdev_handle, dev, random_str, sign_str);
    if (res < STATE_SUCCESS) {
        cJSON_Delete(param);
        return res;
    }

    cJSON_AddStringToObject(param, "sign", sign_str);

    cJSON_AddItemToArray(proxieds, param);

    return STATE_SUCCESS;
}

void *aiot_subdev_init(void)
{
    int32_t res = STATE_SUCCESS;
    subdev_handle_t *subdev_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    res = core_global_init(sysdep);
    if (res < STATE_SUCCESS) {
        return NULL;
    }

    subdev_handle = sysdep->core_sysdep_malloc(sizeof(subdev_handle_t), SUBDEV_MODULE_NAME);
    if (subdev_handle == NULL) {
        return NULL;
    }
    memset(subdev_handle, 0, sizeof(subdev_handle_t));

    subdev_handle->sysdep = sysdep;
    subdev_handle->data_mutex = sysdep->core_sysdep_mutex_init();

    return subdev_handle;
}

int32_t aiot_subdev_setopt(void *handle, aiot_subdev_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_SUBDEVOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    subdev_handle->sysdep->core_sysdep_mutex_lock(subdev_handle->data_mutex);
    switch (option) {
        case AIOT_SUBDEVOPT_MQTT_HANDLE: {
            subdev_handle->mqtt_handle = data;
            res = _subdev_operate_topic_map(subdev_handle, AIOT_MQTTOPT_APPEND_TOPIC_MAP);
        }
        break;
        case AIOT_SUBDEVOPT_RECV_HANDLER: {
            subdev_handle->recv_handler = (aiot_subdev_recv_handler_t)data;
        }
        break;
        case AIOT_SUBDEVOPT_EVENT_HANDLER: {
            subdev_handle->event_handler = (aiot_subdev_event_handler_t)data;
        }
        break;
        case AIOT_SUBDEVOPT_USERDATA: {
            subdev_handle->userdata = data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    subdev_handle->sysdep->core_sysdep_mutex_unlock(subdev_handle->data_mutex);

    return res;
}

int32_t aiot_subdev_deinit(void **handle)
{
    subdev_handle_t *subdev_handle = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    subdev_handle = *(subdev_handle_t **)handle;

    *handle = NULL;

    core_global_deinit(subdev_handle->sysdep);

    subdev_handle->sysdep->core_sysdep_mutex_deinit(&subdev_handle->data_mutex);

    subdev_handle->sysdep->core_sysdep_free(subdev_handle);

    return STATE_SUCCESS;
}

int32_t aiot_subdev_send_topo_add(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    char timestamp[21] = {0};
    cJSON *root = NULL, *params = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
        if (dev[idx].device_secret == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_SECRET;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    /* timestamp */
    core_uint642str(subdev_handle->sysdep->core_sysdep_time(), timestamp, NULL);

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateArray();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_topo_add_append_params(subdev_handle, params, &dev[idx], timestamp);
    }

    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/sys/%s/%s/thing/topo/add", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_topo_delete(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    cJSON *root = NULL, *params = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateArray();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_topo_delete_append_params(subdev_handle, params, &dev[idx]);
    }

    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/sys/%s/%s/thing/topo/delete", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_topo_get(void *handle)
{
    int32_t res = STATE_SUCCESS;
    cJSON *root = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON_AddStringToObject(root, "params", "{}");

    res = _subdev_send_message(subdev_handle, "/sys/%s/%s/thing/topo/get", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_batch_login(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    char timestamp[21] = {0};
    cJSON *root = NULL, *params = NULL, *device_list = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
        if (dev[idx].device_secret == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_SECRET;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    /* timestamp */
    core_uint642str(subdev_handle->sysdep->core_sysdep_time(), timestamp, NULL);

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateObject();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    device_list = cJSON_CreateArray();
    if (device_list == NULL) {
        cJSON_Delete(params);
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    cJSON_AddStringToObject(params, "signMethod", "hmacSha256");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_batch_login_append_device_list(subdev_handle, device_list, &dev[idx], timestamp);
    }

    cJSON_AddItemToObject(params, "deviceList", device_list);
    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/ext/session/%s/%s/combine/batch_login", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_batch_logout(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    cJSON *root = NULL, *params = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateArray();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_batch_logout_append_params(subdev_handle, params, &dev[idx]);
    }

    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/ext/session/%s/%s/combine/batch_logout", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_sub_register(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    cJSON *root = NULL, *params = NULL;
    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateArray();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_sub_register_append_params(subdev_handle, params, &dev[idx]);
    }

    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/sys/%s/%s/thing/sub/register", root);
    cJSON_Delete(root);

    return res;
}

int32_t aiot_subdev_send_product_register(void *handle, aiot_subdev_dev_t dev[], uint32_t dev_num)
{
    int32_t res = STATE_SUCCESS;
    uint32_t idx = 0;
    cJSON *root = NULL, *params = NULL, *proxieds = NULL;

    subdev_handle_t *subdev_handle = (subdev_handle_t *)handle;

    if (handle == NULL || dev == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dev_num == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    for (idx = 0;idx < dev_num;idx++) {
        if (dev[idx].product_key == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
        }
        if (dev[idx].device_name == NULL) {
            return STATE_USER_INPUT_MISSING_DEVICE_NAME;
        }
        if (dev[idx].product_secret == NULL) {
            return STATE_USER_INPUT_MISSING_PRODUCT_SECRET;
        }
    }

    /* alink id */
    res = _subdev_update_alink_id(subdev_handle);
    if (res < STATE_SUCCESS) {
        return res;
    }

    root = cJSON_CreateObject();
    if (root == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    params = cJSON_CreateObject();
    if (params == NULL) {
        cJSON_Delete(root);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    proxieds = cJSON_CreateArray();
    if (proxieds == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(params);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    cJSON_AddStringToObject(root, "id", subdev_handle->alink_id);
    cJSON_AddStringToObject(root, "version", "1.0");

    for (idx = 0;idx < dev_num;idx++) {
        _subdev_product_register_append_params(subdev_handle, proxieds, &dev[idx]);
    }

    cJSON_AddItemToObject(params, "proxieds", proxieds);
    cJSON_AddItemToObject(root, "params", params);

    res = _subdev_send_message(subdev_handle, "/sys/%s/%s/thing/proxy/provisioning/product_register", root);
    cJSON_Delete(root);

    return res;
}

