/**
 * @file aiot_dynregmq_api.c
 * @brief dynregmq模块的API接口实现, 提供获取设备信息的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#include "dynregmq_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_auth.h"


static void _dynregmq_exec_inc(dynregmq_handle_t *dynregmq_handle)
{
    dynregmq_handle->sysdep->core_sysdep_mutex_lock(dynregmq_handle->data_mutex);
    dynregmq_handle->exec_count++;
    dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
}

static void _dynregmq_exec_dec(dynregmq_handle_t *dynregmq_handle)
{
    dynregmq_handle->sysdep->core_sysdep_mutex_lock(dynregmq_handle->data_mutex);
    dynregmq_handle->exec_count--;
    dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
}

static void _dynregmq_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    dynregmq_handle_t *dynregmq_handle = (dynregmq_handle_t *)userdata;

    if (dynregmq_handle->recv_handler == NULL) {
        return;
    }

    switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            char *topic = packet->data.pub.topic;
            uint32_t topic_len = packet->data.pub.topic_len;
            char *payload = (char *)packet->data.pub.payload;
            uint32_t payload_len = packet->data.pub.payload_len;
            const char *topic_register = "/ext/register";
            const char *topic_regnwl = "/ext/regnwl";

            if (strlen(topic_register) == topic_len && !memcmp(topic_register, topic, topic_len)) {
                const char *key_ds = "deviceSecret";
                char *ds = NULL;
                uint32_t ds_len = 0;

                core_json_value(payload, payload_len, key_ds, strlen(key_ds),
                                &ds, &ds_len);

                if (ds == NULL || ds_len == 0) {
                    break;
                }

                dynregmq_handle->flag_completed = 1;

                if (dynregmq_handle->recv_handler) {
                    aiot_dynregmq_recv_t recv_data;
                    memset(&recv_data, 0, sizeof(aiot_dynregmq_recv_t));

                    *(ds + ds_len) = 0;
                    recv_data.type = AIOT_DYNREGMQRECV_DEVICEINFO_WL;
                    recv_data.data.deviceinfo_wl.device_secret = ds;
                    dynregmq_handle->recv_handler(dynregmq_handle, &recv_data, dynregmq_handle->userdata);
                }
            } else if (strlen(topic_regnwl) == topic_len && !memcmp(topic_regnwl, topic, topic_len)) {
                const char *key_clientid = "clientId";
                const char *key_devicetoken = "deviceToken";
                char *client_id = NULL;
                char *device_token = NULL;
                uint32_t client_id_len = 0;
                uint32_t device_token_len = 0;

                core_json_value(payload, payload_len, key_clientid, strlen(key_clientid), &client_id, &client_id_len);
                core_json_value(payload, payload_len, key_devicetoken, strlen(key_devicetoken), &device_token, &device_token_len);

                if (client_id == NULL || device_token == NULL || client_id_len == 0 || device_token_len == 0) {
                    break;
                }

                dynregmq_handle->flag_completed = 1;

                if (dynregmq_handle->recv_handler) {
                    aiot_dynregmq_recv_t recv_data;
                    char *conn_clientid = NULL;
                    char *conn_username = NULL;
                    char *conn_username_fmt[] = { dynregmq_handle->device_name, dynregmq_handle->product_key };

                    *(client_id + client_id_len) = 0;
                    *(device_token + device_token_len) = 0;
                    core_sprintf(dynregmq_handle->sysdep, &conn_clientid, "%s|authType=connwl,securemode=-2,_ss=1,ext=3,_v="CORE_AUTH_SDK_VERSION"|",
                                 &client_id, 1, DYNREGMQ_MODULE_NAME);
                    core_sprintf(dynregmq_handle->sysdep, &conn_username, "%s&%s",
                                 conn_username_fmt, sizeof(conn_username_fmt) / sizeof(char *), DYNREGMQ_MODULE_NAME);

                    memset(&recv_data, 0, sizeof(aiot_dynregmq_recv_t));
                    recv_data.type = AIOT_DYNREGMQRECV_DEVICEINFO_NWL;
                    recv_data.data.deviceinfo_nwl.clientid = conn_clientid;
                    recv_data.data.deviceinfo_nwl.username = conn_username;
                    recv_data.data.deviceinfo_nwl.password = device_token;
                    dynregmq_handle->recv_handler(dynregmq_handle, &recv_data, dynregmq_handle->userdata);

                    dynregmq_handle->sysdep->core_sysdep_free(conn_clientid);
                    dynregmq_handle->sysdep->core_sysdep_free(conn_username);
                }
            }
        }
        break;
        default: break;
    }
}

void *aiot_dynregmq_init(void)
{
    dynregmq_handle_t *dynregmq_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    dynregmq_handle = sysdep->core_sysdep_malloc(sizeof(dynregmq_handle_t), DYNREGMQ_MODULE_NAME);
    if (dynregmq_handle == NULL) {
        return NULL;
    }
    memset(dynregmq_handle, 0, sizeof(dynregmq_handle_t));

    dynregmq_handle->sysdep = sysdep;
    dynregmq_handle->timeout_ms = DYNREGMQ_DEFAULT_TIMEOUT_MS;
    dynregmq_handle->deinit_timeout_ms = DYNREGMQ_DEFAULT_DEINIT_TIMEOUT_MS;
    dynregmq_handle->send_timeout_ms = DYNREGMQ_DEFAULT_SEND_TIMEOUT;
    dynregmq_handle->recv_timeout_ms = DYNREGMQ_DEFAULT_RECV_TIMEOUT;
    dynregmq_handle->data_mutex = dynregmq_handle->sysdep->core_sysdep_mutex_init();

    dynregmq_handle->exec_enabled = 1;

    return dynregmq_handle;
}

int32_t aiot_dynregmq_setopt(void *handle, aiot_dynregmq_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    dynregmq_handle_t *dynregmq_handle = (dynregmq_handle_t *)handle;

    if (dynregmq_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_DYNREGMQOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (dynregmq_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynregmq_exec_inc(dynregmq_handle);

    dynregmq_handle->sysdep->core_sysdep_mutex_lock(dynregmq_handle->data_mutex);
    switch (option) {
        case AIOT_DYNREGMQOPT_NETWORK_CRED: {
            if (dynregmq_handle->cred != NULL) {
                dynregmq_handle->sysdep->core_sysdep_free(dynregmq_handle->cred);
                dynregmq_handle->cred = NULL;
            }
            dynregmq_handle->cred = dynregmq_handle->sysdep->core_sysdep_malloc(sizeof(aiot_sysdep_network_cred_t),
                                    DYNREGMQ_MODULE_NAME);
            if (dynregmq_handle->cred != NULL) {
                memset(dynregmq_handle->cred, 0, sizeof(aiot_sysdep_network_cred_t));
                memcpy(dynregmq_handle->cred, data, sizeof(aiot_sysdep_network_cred_t));
            } else {
                res = STATE_SYS_DEPEND_MALLOC_FAILED;
            }
        }
        break;
        case AIOT_DYNREGMQOPT_HOST: {
            res = core_strdup(dynregmq_handle->sysdep, &dynregmq_handle->host, data, DYNREGMQ_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGMQOPT_PORT: {
            dynregmq_handle->port = *(uint16_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_PRODUCT_KEY: {
            res = core_strdup(dynregmq_handle->sysdep, &dynregmq_handle->product_key, data, DYNREGMQ_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGMQOPT_PRODUCT_SECRET: {
            res = core_strdup(dynregmq_handle->sysdep, &dynregmq_handle->product_secret, data, DYNREGMQ_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGMQOPT_DEVICE_NAME: {
            res = core_strdup(dynregmq_handle->sysdep, &dynregmq_handle->device_name, data, DYNREGMQ_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGMQOPT_SEND_TIMEOUT_MS: {
            dynregmq_handle->send_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_RECV_TIMEOUT_MS: {
            dynregmq_handle->recv_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_RECV_HANDLER: {
            dynregmq_handle->recv_handler = (aiot_dynregmq_recv_handler_t)data;
        }
        break;
        case AIOT_DYNREGMQOPT_USERDATA: {
            dynregmq_handle->userdata = data;
        }
        break;
        case AIOT_DYNREGMQOPT_TIMEOUT_MS: {
            dynregmq_handle->timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_DEINIT_TIMEOUT_MS: {
            dynregmq_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_NO_WHITELIST: {
            dynregmq_handle->flag_nowhitelist = *(uint8_t *)data;
        }
        break;
        case AIOT_DYNREGMQOPT_INSTANCE_ID: {
            res = core_strdup(dynregmq_handle->sysdep, &dynregmq_handle->instance_id, data, DYNREGMQ_MODULE_NAME);
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);

    _dynregmq_exec_dec(dynregmq_handle);

    return res;
}

int32_t aiot_dynregmq_deinit(void **handle)
{
    uint32_t deinit_timeout_ms = 0;
    dynregmq_handle_t *dynregmq_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    dynregmq_handle = *(dynregmq_handle_t **)handle;
    sysdep = dynregmq_handle->sysdep;

    if (dynregmq_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    dynregmq_handle->exec_enabled = 0;
    deinit_timeout_ms = dynregmq_handle->deinit_timeout_ms;
    do {
        if (dynregmq_handle->exec_count == 0) {
            break;
        }
        dynregmq_handle->sysdep->core_sysdep_sleep(DYNREGMQ_DEINIT_INTERVAL_MS);
    } while ((deinit_timeout_ms > DYNREGMQ_DEINIT_INTERVAL_MS) && (deinit_timeout_ms - DYNREGMQ_DEINIT_INTERVAL_MS > 0));

    if (dynregmq_handle->exec_count != 0) {
        return STATE_DYNREGMQ_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    if (dynregmq_handle->mqtt_handle != NULL) {
        aiot_mqtt_deinit(&dynregmq_handle->mqtt_handle);
    }

    if (dynregmq_handle->host != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->host);
    }

    if (dynregmq_handle->product_key != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->product_key);
    }

    if (dynregmq_handle->product_secret != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->product_secret);
    }

    if (dynregmq_handle->device_name != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->device_name);
    }

    if (dynregmq_handle->cred != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->cred);
    }

    if (dynregmq_handle->instance_id != NULL) {
        sysdep->core_sysdep_free(dynregmq_handle->instance_id);
    }

    sysdep->core_sysdep_mutex_deinit(&dynregmq_handle->data_mutex);

    sysdep->core_sysdep_free(dynregmq_handle);

    return STATE_SUCCESS;
}

int32_t aiot_dynregmq_send_request(void *handle)
{
    int32_t res = STATE_SUCCESS;
    dynregmq_handle_t *dynregmq_handle = (dynregmq_handle_t *)handle;
    char *auth_clientid = NULL;
    char *auth_username = NULL;
    char auth_password[65] = {0};
    char *sign_input = NULL;
    uint32_t random_num = 0;
    char random[11] = {0};
    char *auth_type = NULL;
    uint8_t reconnect = 0;

    if (dynregmq_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dynregmq_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    if (dynregmq_handle->product_key == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }

    if (dynregmq_handle->product_secret == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_SECRET;
    }

    if (dynregmq_handle->device_name == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }

    if (dynregmq_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynregmq_exec_inc(dynregmq_handle);

    dynregmq_handle->sysdep->core_sysdep_mutex_lock(dynregmq_handle->data_mutex);

    if (dynregmq_handle->mqtt_handle != NULL) {
        aiot_mqtt_deinit(&dynregmq_handle->mqtt_handle);
    }

    dynregmq_handle->mqtt_handle = aiot_mqtt_init();
    if (dynregmq_handle->mqtt_handle == NULL) {
        dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
        _dynregmq_exec_dec(dynregmq_handle);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    /* setup auth_type */
    auth_type = (dynregmq_handle->flag_nowhitelist) ? "regnwl" : "register";

    /* generate random string */
    dynregmq_handle->sysdep->core_sysdep_rand((uint8_t *)&random_num, 4);
    core_uint2str(random_num, random, NULL);

    /* assamble clientid, username and password */
    {
        uint8_t has_instance_id = (dynregmq_handle->flag_nowhitelist && dynregmq_handle->instance_id != NULL) ? 1 : 0;
        char *client_fmt = (has_instance_id) ?
                           "%s.%s|random=%s,authType=%s,securemode=2,signmethod=hmacsha256,instanceId=%s|" :
                           "%s.%s|random=%s,authType=%s,securemode=2,signmethod=hmacsha256|";
        char *client_src[] = { dynregmq_handle->device_name, dynregmq_handle->product_key,
                               random, auth_type, dynregmq_handle->instance_id
                             };
        char *username_fmt = "%s&%s";
        char *username_src[] = { dynregmq_handle->device_name, dynregmq_handle->product_key };
        char *sign_input_fmt = "deviceName%sproductKey%srandom%s";
        uint8_t sign_output[32] = {0};

        core_sprintf(dynregmq_handle->sysdep, &auth_clientid, client_fmt, client_src,
                     has_instance_id ? 5 : 4, DYNREGMQ_MODULE_NAME);
        core_sprintf(dynregmq_handle->sysdep, &auth_username, username_fmt, username_src,
                     sizeof(username_src) / sizeof(char *), DYNREGMQ_MODULE_NAME);
        core_sprintf(dynregmq_handle->sysdep, &sign_input, sign_input_fmt, client_src,
                     3, DYNREGMQ_MODULE_NAME);
        core_hmac_sha256((const uint8_t *)sign_input, (uint32_t)strlen(sign_input),
                         (const uint8_t *)dynregmq_handle->product_secret,
                         (uint32_t)strlen(dynregmq_handle->product_secret), sign_output);
        core_hex2str(sign_output, sizeof(sign_output), auth_password, 0);
    }

    if (((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_HOST,
                                 (void *)dynregmq_handle->host)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_PORT,
                                 (void *)&dynregmq_handle->port)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_CLIENTID,
                                 (void *)auth_clientid)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_USERNAME,
                                 (void *)auth_username)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_PASSWORD,
                                 (void *)auth_password)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_NETWORK_CRED,
                                 (void *)dynregmq_handle->cred)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_SEND_TIMEOUT_MS,
                                 (void *)&dynregmq_handle->send_timeout_ms)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_RECV_TIMEOUT_MS,
                                 (void *)&dynregmq_handle->recv_timeout_ms)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_RECV_HANDLER,
                                 (void *)_dynregmq_recv_handler)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_USERDATA,
                                 (void *)dynregmq_handle)) < STATE_SUCCESS) ||
        ((res = aiot_mqtt_setopt(dynregmq_handle->mqtt_handle, AIOT_MQTTOPT_RECONN_ENABLED,
                                 (void *)&reconnect)) < STATE_SUCCESS)) {
        aiot_mqtt_deinit(&dynregmq_handle->mqtt_handle);
        dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
        _dynregmq_exec_dec(dynregmq_handle);

        dynregmq_handle->sysdep->core_sysdep_free(auth_clientid);
        dynregmq_handle->sysdep->core_sysdep_free(auth_username);
        dynregmq_handle->sysdep->core_sysdep_free(sign_input);
        return res;
    }

    res = aiot_mqtt_connect(dynregmq_handle->mqtt_handle);
    if (res < STATE_SUCCESS) {
        aiot_mqtt_deinit(&dynregmq_handle->mqtt_handle);
    }

    dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
    _dynregmq_exec_dec(dynregmq_handle);

    dynregmq_handle->sysdep->core_sysdep_free(auth_clientid);
    dynregmq_handle->sysdep->core_sysdep_free(auth_username);
    dynregmq_handle->sysdep->core_sysdep_free(sign_input);
    return res;
}

int32_t aiot_dynregmq_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint64_t timenow_ms = 0;
    dynregmq_handle_t *dynregmq_handle = (dynregmq_handle_t *)handle;

    if (dynregmq_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dynregmq_handle->mqtt_handle == NULL) {
        return STATE_DYNREGMQ_NEED_SEND_REQUEST;
    }

    if (dynregmq_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynregmq_exec_inc(dynregmq_handle);

    dynregmq_handle->sysdep->core_sysdep_mutex_lock(dynregmq_handle->data_mutex);
    timenow_ms = dynregmq_handle->sysdep->core_sysdep_time();
    while (1) {
        if (timenow_ms >= dynregmq_handle->sysdep->core_sysdep_time()) {
            timenow_ms = dynregmq_handle->sysdep->core_sysdep_time();
        }
        if (dynregmq_handle->sysdep->core_sysdep_time() - timenow_ms >= dynregmq_handle->timeout_ms) {
            res = STATE_DYNREGMQ_AUTH_TIMEOUT;
            break;
        }

        res = aiot_mqtt_recv(dynregmq_handle->mqtt_handle);
        if (res < 0) {
            break;
        }

        if (dynregmq_handle->flag_completed == 1) {
            dynregmq_handle->flag_completed = 0;
            res = STATE_SUCCESS;
            break;
        }
    }

    dynregmq_handle->sysdep->core_sysdep_mutex_unlock(dynregmq_handle->data_mutex);
    _dynregmq_exec_dec(dynregmq_handle);
    return res;
}

