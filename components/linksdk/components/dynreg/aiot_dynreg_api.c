/**
 * @file aiot_dynreg_api.c
 * @brief dynreg模块的API接口实现, 提供获取设备信息的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#include "dynreg_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_auth.h"

static void _dynreg_exec_inc(dynreg_handle_t *dynreg_handle)
{
    dynreg_handle->sysdep->core_sysdep_mutex_lock(dynreg_handle->data_mutex);
    dynreg_handle->exec_count++;
    dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
}

static void _dynreg_exec_dec(dynreg_handle_t *dynreg_handle)
{
    dynreg_handle->sysdep->core_sysdep_mutex_lock(dynreg_handle->data_mutex);
    dynreg_handle->exec_count--;
    dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
}

static int32_t _dynreg_sign(dynreg_handle_t *dynreg_handle, char *random, char sign_str[65])
{
    int32_t res = STATE_SUCCESS;
    uint8_t sign_hex[32] = {0};
    char *src_fmt = "deviceName%sproductKey%srandom%s";
    char *src[] = {dynreg_handle->device_name, dynreg_handle->product_key, random};
    char *plain_text = NULL;

    res = core_sprintf(dynreg_handle->sysdep, &plain_text, src_fmt, src, sizeof(src) / sizeof(char *), DYNREG_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    core_hmac_sha256((const uint8_t *)plain_text, (uint32_t)strlen(plain_text),
                     (const uint8_t *)dynreg_handle->product_secret, (uint32_t)strlen(dynreg_handle->product_secret), sign_hex);
    core_hex2str(sign_hex, 32, sign_str, 0);

    dynreg_handle->sysdep->core_sysdep_free(plain_text);

    return STATE_SUCCESS;
}

static void _dynreg_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    dynreg_handle_t *dynreg_handle = (dynreg_handle_t *)userdata;

    if (dynreg_handle->recv_handler == NULL) {
        return;
    }

    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            dynreg_handle->response.code = packet->data.status_code.code;
        }
        break;
        case AIOT_HTTPRECV_HEADER: {
            if ((strlen(packet->data.header.key) == strlen("Content-Length")) &&
                (memcmp(packet->data.header.key, "Content-Length", strlen(packet->data.header.key)) == 0)) {
                core_str2uint(packet->data.header.value, (uint8_t)strlen(packet->data.header.value),
                              &dynreg_handle->response.content_total_len);
            }
        }
        break;
        case AIOT_HTTPRECV_BODY: {
            uint8_t *content = dynreg_handle->sysdep->core_sysdep_malloc(dynreg_handle->response.content_len + packet->data.body.len
                               + 1,
                               CORE_HTTP_MODULE_NAME);
            if (content == NULL) {
                return;
            }
            memset(content, 0, dynreg_handle->response.content_len + packet->data.body.len + 1);
            if (content != NULL) {
                memcpy(content, dynreg_handle->response.content, dynreg_handle->response.content_len);
                dynreg_handle->sysdep->core_sysdep_free(dynreg_handle->response.content);
            }
            memcpy(content + dynreg_handle->response.content_len, packet->data.body.buffer, packet->data.body.len);
            dynreg_handle->response.content = content;
            dynreg_handle->response.content_len = dynreg_handle->response.content_len + packet->data.body.len;
        }
        break;
        default: {

        }
        break;
    }
}

static int32_t _dynreg_device_info(dynreg_handle_t *dynreg_handle, char **device_secret)
{
    int32_t res = STATE_SUCCESS;
    char *tmp_ds = NULL, *ds_key = "deviceSecret";
    char *ds_value = NULL;
    uint32_t ds_value_len = 0;

    if (dynreg_handle->response.code != 200) {
        return STATE_DYNREG_INVALID_STATUS_CODE;
    }

    if ((res = core_json_value((char *)dynreg_handle->response.content, dynreg_handle->response.content_len, ds_key,
                               strlen(ds_key), &ds_value, &ds_value_len)) < STATE_SUCCESS) {
        return STATE_DYNREG_INVALID_DEVICE_SECRET;
    }

    tmp_ds = dynreg_handle->sysdep->core_sysdep_malloc(ds_value_len + 1, DYNREG_MODULE_NAME);
    if (tmp_ds == NULL) {
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(tmp_ds, 0, ds_value_len + 1);
    memcpy(tmp_ds, ds_value, ds_value_len);

    *device_secret = tmp_ds;

    return STATE_SUCCESS;
}

void *aiot_dynreg_init(void)
{
    dynreg_handle_t *dynreg_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    dynreg_handle = sysdep->core_sysdep_malloc(sizeof(dynreg_handle_t), DYNREG_MODULE_NAME);
    if (dynreg_handle == NULL) {
        return NULL;
    }
    memset(dynreg_handle, 0, sizeof(dynreg_handle_t));

    dynreg_handle->sysdep = sysdep;
    dynreg_handle->response_body_len = DYNREG_RESPONSE_BODY_LEN;
    dynreg_handle->timeout_ms = DYNREG_DEFAULT_TIMEOUT_MS;
    dynreg_handle->deinit_timeout_ms = DYNREG_DEFAULT_DEINIT_TIMEOUT_MS;
    dynreg_handle->send_timeout_ms = DYNREG_DEFAULT_SEND_TIMEOUT;
    dynreg_handle->recv_timeout_ms = DYNREG_DEFAULT_RECV_TIMEOUT;
    dynreg_handle->data_mutex = dynreg_handle->sysdep->core_sysdep_mutex_init();

    dynreg_handle->exec_enabled = 1;

    return dynreg_handle;
}

int32_t aiot_dynreg_setopt(void *handle, aiot_dynreg_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    dynreg_handle_t *dynreg_handle = (dynreg_handle_t *)handle;

    if (dynreg_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_DYNREGOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (dynreg_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynreg_exec_inc(dynreg_handle);

    dynreg_handle->sysdep->core_sysdep_mutex_lock(dynreg_handle->data_mutex);
    switch (option) {
        case AIOT_DYNREGOPT_NETWORK_CRED: {
            if (dynreg_handle->cred != NULL) {
                dynreg_handle->sysdep->core_sysdep_free(dynreg_handle->cred);
                dynreg_handle->cred = NULL;
            }
            dynreg_handle->cred = dynreg_handle->sysdep->core_sysdep_malloc(sizeof(aiot_sysdep_network_cred_t), DYNREG_MODULE_NAME);
            if (dynreg_handle->cred != NULL) {
                memset(dynreg_handle->cred, 0, sizeof(aiot_sysdep_network_cred_t));
                memcpy(dynreg_handle->cred, data, sizeof(aiot_sysdep_network_cred_t));
            } else {
                res = STATE_SYS_DEPEND_MALLOC_FAILED;
            }
        }
        break;
        case AIOT_DYNREGOPT_HOST: {
            res = core_strdup(dynreg_handle->sysdep, &dynreg_handle->host, data, DYNREG_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGOPT_PORT: {
            dynreg_handle->port = *(uint16_t *)data;
        }
        break;
        case AIOT_DYNREGOPT_PRODUCT_KEY: {
            res = core_strdup(dynreg_handle->sysdep, &dynreg_handle->product_key, data, DYNREG_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGOPT_PRODUCT_SECRET: {
            res = core_strdup(dynreg_handle->sysdep, &dynreg_handle->product_secret, data, DYNREG_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGOPT_DEVICE_NAME: {
            res = core_strdup(dynreg_handle->sysdep, &dynreg_handle->device_name, data, DYNREG_MODULE_NAME);
        }
        break;
        case AIOT_DYNREGOPT_SEND_TIMEOUT_MS: {
            dynreg_handle->send_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGOPT_RECV_TIMEOUT_MS: {
            dynreg_handle->recv_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGOPT_RECV_HANDLER: {
            dynreg_handle->recv_handler = (aiot_dynreg_recv_handler_t)data;
        }
        break;
        case AIOT_DYNREGOPT_USERDATA: {
            dynreg_handle->userdata = data;
        }
        break;
        case AIOT_DYNREGOPT_TIMEOUT_MS: {
            dynreg_handle->timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_DYNREGOPT_DEINIT_TIMEOUT_MS: {
            dynreg_handle->deinit_timeout_ms = *(uint32_t *)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);

    _dynreg_exec_dec(dynreg_handle);

    return res;
}

int32_t aiot_dynreg_deinit(void **handle)
{
    uint64_t deinit_timestart = 0;
    dynreg_handle_t *dynreg_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    dynreg_handle = *(dynreg_handle_t **)handle;
    sysdep = dynreg_handle->sysdep;

    if (dynreg_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    dynreg_handle->exec_enabled = 0;
    deinit_timestart = dynreg_handle->sysdep->core_sysdep_time();
    do {
        if (dynreg_handle->exec_count == 0) {
            break;
        }
        dynreg_handle->sysdep->core_sysdep_sleep(DYNREG_DEINIT_INTERVAL_MS);
    } while ((dynreg_handle->sysdep->core_sysdep_time() - deinit_timestart) < dynreg_handle->deinit_timeout_ms);

    if (dynreg_handle->exec_count != 0) {
        return STATE_DYNREG_DEINIT_TIMEOUT;
    }

    *handle = NULL;

    if (dynreg_handle->response.content != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->response.content);
    }
    memset(&dynreg_handle->response, 0, sizeof(core_http_response_t));

    if (dynreg_handle->http_handle != NULL) {
        core_http_deinit(&dynreg_handle->http_handle);
    }

    if (dynreg_handle->host != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->host);
    }

    if (dynreg_handle->product_key != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->product_key);
    }

    if (dynreg_handle->product_secret != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->product_secret);
    }

    if (dynreg_handle->device_name != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->device_name);
    }

    if (dynreg_handle->cred != NULL) {
        sysdep->core_sysdep_free(dynreg_handle->cred);
    }

    sysdep->core_sysdep_mutex_deinit(&dynreg_handle->data_mutex);

    sysdep->core_sysdep_free(dynreg_handle);

    return STATE_SUCCESS;
}

int32_t aiot_dynreg_send_request(void *handle)
{
    int32_t res = STATE_SUCCESS;
    dynreg_handle_t *dynreg_handle = (dynreg_handle_t *)handle;

    if (dynreg_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dynreg_handle->host == NULL) {
        return STATE_USER_INPUT_MISSING_HOST;
    }

    if (dynreg_handle->product_key == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }

    if (dynreg_handle->product_secret == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_SECRET;
    }

    if (dynreg_handle->device_name == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }

    if (dynreg_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynreg_exec_inc(dynreg_handle);

    dynreg_handle->sysdep->core_sysdep_mutex_lock(dynreg_handle->data_mutex);
    if (dynreg_handle->response.content != NULL) {
        dynreg_handle->sysdep->core_sysdep_free(dynreg_handle->response.content);
    }
    memset(&dynreg_handle->response, 0, sizeof(core_http_response_t));

    if (dynreg_handle->http_handle != NULL) {
        core_http_deinit(&dynreg_handle->http_handle);
    }

    dynreg_handle->http_handle = core_http_init();
    if (dynreg_handle->http_handle == NULL) {
        dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
        _dynreg_exec_dec(dynreg_handle);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }

    if (((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_HOST,
                                 (void *)dynreg_handle->host)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_PORT, (void *)&dynreg_handle->port)) < STATE_SUCCESS)
        ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_NETWORK_CRED,
                                 (void *)dynreg_handle->cred)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_SEND_TIMEOUT_MS,
                                 (void *)&dynreg_handle->send_timeout_ms)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_RECV_TIMEOUT_MS,
                                 (void *)&dynreg_handle->recv_timeout_ms)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_BODY_BUFFER_MAX_LEN,
                                 (void *)&dynreg_handle->response_body_len)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_RECV_HANDLER,
                                 (void *)_dynreg_recv_handler)) < STATE_SUCCESS) ||
        ((res = core_http_setopt(dynreg_handle->http_handle, CORE_HTTPOPT_USERDATA, (void *)dynreg_handle)) < STATE_SUCCESS)) {
        core_http_deinit(&dynreg_handle->http_handle);
        dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
        _dynreg_exec_dec(dynreg_handle);
        return res;
    }

    res = core_http_connect(dynreg_handle->http_handle);
    if (res < STATE_SUCCESS) {
        core_http_deinit(&dynreg_handle->http_handle);
        dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
        _dynreg_exec_dec(dynreg_handle);
        return res;
    }

    {
        uint32_t random_num = 0;
        char random[11] = {0};
        char sign_str[65] = {0};
        char *content = NULL;
        char *content_fmt = "productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=hmacsha256";
        char *content_src[] = { dynreg_handle->product_key, dynreg_handle->device_name, (char *)random, sign_str };
        core_http_request_t request;

        dynreg_handle->sysdep->core_sysdep_rand((uint8_t *)&random_num, 4);
        core_uint2str(random_num, random, NULL);

        res = _dynreg_sign(dynreg_handle, (char *)random, sign_str);
        if (res < STATE_SUCCESS) {
            core_http_deinit(&dynreg_handle->http_handle);
            dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
            _dynreg_exec_dec(dynreg_handle);
            return res;
        }


        memset(&request, 0, sizeof(core_http_request_t));
        res = core_sprintf(dynreg_handle->sysdep, &content, content_fmt, content_src, sizeof(content_src) / sizeof(char *),
                           DYNREG_MODULE_NAME);
        if (res < STATE_SUCCESS) {
            core_http_deinit(&dynreg_handle->http_handle);
            dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
            _dynreg_exec_dec(dynreg_handle);
            return res;
        }
        request.method = "POST";
        request.path = DYNREG_PATH;
        request.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n" \
                         "Content-Type: application/x-www-form-urlencoded\r\n";
        request.content = (uint8_t *)content;
        request.content_len = (uint32_t)strlen(content);

        res = core_http_send(dynreg_handle->http_handle, &request);
        dynreg_handle->sysdep->core_sysdep_free(content);
        if (res < STATE_SUCCESS) {
            core_http_deinit(&dynreg_handle->http_handle);
            dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
            _dynreg_exec_dec(dynreg_handle);
            return res;
        }
    }
    dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
    _dynreg_exec_dec(dynreg_handle);

    return res;
}

int32_t aiot_dynreg_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint64_t timenow_ms = 0;
    dynreg_handle_t *dynreg_handle = (dynreg_handle_t *)handle;
    char *device_secret = NULL;
    aiot_dynreg_recv_t packet;

    if (dynreg_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (dynreg_handle->http_handle == NULL) {
        return STATE_DYNREG_NEED_SEND_REQUEST;
    }

    if (dynreg_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _dynreg_exec_inc(dynreg_handle);

    dynreg_handle->sysdep->core_sysdep_mutex_lock(dynreg_handle->data_mutex);
    timenow_ms = dynreg_handle->sysdep->core_sysdep_time();
    while (1) {
        if (timenow_ms >= dynreg_handle->sysdep->core_sysdep_time()) {
            timenow_ms = dynreg_handle->sysdep->core_sysdep_time();
        }
        if (dynreg_handle->sysdep->core_sysdep_time() - timenow_ms >= dynreg_handle->timeout_ms) {
            break;
        }

        res = core_http_recv(dynreg_handle->http_handle);
        if (res < STATE_SUCCESS) {
            break;
        }
    }

    if (res < STATE_SUCCESS) {
        if (res != STATE_HTTP_READ_BODY_FINISHED) {
            if (dynreg_handle->response.content != NULL) {
                dynreg_handle->sysdep->core_sysdep_free(dynreg_handle->response.content);
                memset(&dynreg_handle->response, 0, sizeof(core_http_response_t));
            }
            dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
            _dynreg_exec_dec(dynreg_handle);
            return res;
        } else {
            res = STATE_SUCCESS;
        }
    } else {
        dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
        _dynreg_exec_dec(dynreg_handle);
        return STATE_HTTP_RECV_NOT_FINISHED;
    }

    if (dynreg_handle->recv_handler != NULL) {
        packet.type = AIOT_DYNREGRECV_STATUS_CODE;
        packet.data.status_code.code = dynreg_handle->response.code;

        dynreg_handle->recv_handler(dynreg_handle, &packet, dynreg_handle->userdata);
    }

    res = _dynreg_device_info(dynreg_handle, &device_secret);
    dynreg_handle->sysdep->core_sysdep_mutex_unlock(dynreg_handle->data_mutex);
    if (res < STATE_SUCCESS) {
        _dynreg_exec_dec(dynreg_handle);
        return res;
    }

    memset(&packet, 0, sizeof(aiot_dynreg_recv_t));
    if (dynreg_handle->recv_handler != NULL) {
        packet.type = AIOT_DYNREGRECV_DEVICE_INFO;
        packet.data.device_info.device_secret = device_secret;

        dynreg_handle->recv_handler(dynreg_handle, &packet, dynreg_handle->userdata);
    }
    dynreg_handle->sysdep->core_sysdep_free(device_secret);

    _dynreg_exec_dec(dynreg_handle);
    return STATE_SUCCESS;
}

