#include "core_http.h"

static void _core_aiot_http_exec_inc(core_http_handle_t *http_handle)
{
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    http_handle->exec_count++;
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
}

static void _core_aiot_http_exec_dec(core_http_handle_t *http_handle)
{
    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    http_handle->exec_count--;
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
}

static void _core_http_auth_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;
    core_http_response_t *response = (core_http_response_t *)userdata;

    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            response->code = packet->data.status_code.code;
        }
        break;
        case AIOT_HTTPRECV_HEADER: {
            if ((strlen(packet->data.header.key) == strlen("Content-Length")) &&
                (memcmp(packet->data.header.key, "Content-Length", strlen(packet->data.header.key)) == 0)) {
                core_str2uint(packet->data.header.value, (uint8_t)strlen(packet->data.header.value), &response->content_total_len);
            }
        }
        break;
        case AIOT_HTTPRECV_BODY: {
            uint8_t *content = http_handle->sysdep->core_sysdep_malloc(response->content_len + packet->data.body.len + 1,
                               CORE_HTTP_MODULE_NAME);
            if (content == NULL) {
                return;
            }
            memset(content, 0, response->content_len + packet->data.body.len + 1);
            if (response->content != NULL) {
                memcpy(content, response->content, response->content_len);
                http_handle->sysdep->core_sysdep_free(response->content);
            }
            memcpy(content + response->content_len, packet->data.body.buffer, packet->data.body.len);
            response->content = content;
            response->content_len = response->content_len + packet->data.body.len;
        }
        break;
        default: {

        }
        break;
    }
}

static void _core_http_recv_handler(void *handle, const aiot_http_recv_t *packet, void *userdata)
{
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;
    core_http_response_t *response = (core_http_response_t *)userdata;

    if (http_handle->recv_handler == NULL) {
        return;
    }

    switch (packet->type) {
        case AIOT_HTTPRECV_STATUS_CODE: {
            http_handle->recv_handler(http_handle, packet, http_handle->userdata);
        }
        break;
        case AIOT_HTTPRECV_HEADER: {
            if ((strlen(packet->data.header.key) == strlen("Content-Length")) &&
                (memcmp(packet->data.header.key, "Content-Length", strlen(packet->data.header.key)) == 0)) {
                core_str2uint(packet->data.header.value, (uint8_t)strlen(packet->data.header.value), &response->content_total_len);
            }
            http_handle->recv_handler(http_handle, packet, http_handle->userdata);
        }
        break;
        case AIOT_HTTPRECV_BODY: {
            uint8_t *content = http_handle->sysdep->core_sysdep_malloc(response->content_len + packet->data.body.len + 1,
                               CORE_HTTP_MODULE_NAME);
            if (content == NULL) {
                return;
            }
            memset(content, 0, response->content_len + packet->data.body.len + 1);
            if (response->content != NULL) {
                memcpy(content, response->content, response->content_len);
                http_handle->sysdep->core_sysdep_free(response->content);
            }
            memcpy(content + response->content_len, packet->data.body.buffer, packet->data.body.len);
            response->content = content;
            response->content_len = response->content_len + packet->data.body.len;
        }
        break;
        default: {

        }
        break;
    }
}

static void _core_aiot_http_token_expired_event(core_http_handle_t *http_handle, core_http_response_t *response)
{
    int32_t res = STATE_SUCCESS;
    char *code_str = NULL;
    uint32_t code_strlen = 0, code = 0;

    res = core_json_value((const char *)response->content, response->content_len, "code", strlen("code"), &code_str,
                          &code_strlen);
    if (res < STATE_SUCCESS) {
        return;
    }

    res = core_str2uint(code_str, code_strlen, &code);
    if (res < STATE_SUCCESS) {
        return;
    }

    if (code == AIOT_HTTP_RSPCODE_TOKEN_EXPIRED ||
        code == AIOT_HTTP_RSPCODE_TOKEN_CHECK_ERROR) {
        if (http_handle->event_handler != NULL) {
            aiot_http_event_t event;
            event.type = AIOT_HTTPEVT_TOKEN_INVALID;

            http_handle->event_handler(http_handle, &event, http_handle->userdata);
        }
    }
}

static int32_t _core_http_send_auth(core_http_handle_t *http_handle)
{
    int32_t res = STATE_SUCCESS;
    char *content = NULL, *path = NULL;
    char *path_fmt = "/auth?_v=%s&%s";
    char *path_src[] = { CORE_AUTH_SDK_VERSION, http_handle->extend_devinfo };
    core_http_request_t request;

    res = core_auth_http_body(http_handle->sysdep, &content, http_handle->product_key, http_handle->device_name,
                              http_handle->device_secret, CORE_HTTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    res = core_sprintf(http_handle->sysdep, &path, path_fmt, path_src, sizeof(path_src) / sizeof(char *),
                       CORE_HTTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        http_handle->sysdep->core_sysdep_free(content);
        return res;
    }

    memset(&request, 0, sizeof(core_http_request_t));
    request.method = "POST";
    request.path = path;
    request.header = (http_handle->long_connection == 0) ? ("Content-Type: application/json\r\nConnection: close\r\n") :
                     ("Content-Type: application/json\r\n");
    request.content = (uint8_t *)content;
    request.content_len = (uint32_t)strlen(content);

    res = core_http_send(http_handle, &request);

    http_handle->sysdep->core_sysdep_free(path);
    http_handle->sysdep->core_sysdep_free(content);

    return res;
}

static int32_t _core_http_recv_auth(core_http_handle_t *http_handle, core_http_response_t *response)
{
    int32_t res = STATE_SUCCESS;
    char *token = NULL;
    uint32_t token_len = 0;
    uint64_t timenow_ms = http_handle->sysdep->core_sysdep_time();

    while (1) {
        if (timenow_ms >= http_handle->sysdep->core_sysdep_time()) {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->auth_timeout_ms) {
            break;
        }

        res = core_http_recv(http_handle);
        if (res < STATE_SUCCESS) {
            break;
        }
    }

    if (res < STATE_SUCCESS) {
        if (res != STATE_HTTP_READ_BODY_FINISHED) {
            return res;
        } else {
            res = STATE_SUCCESS;
        }
    } else {
        return STATE_HTTP_AUTH_NOT_FINISHED;
    }

    if (response->code != 200) {
        return STATE_HTTP_AUTH_CODE_FAILED;
    }
    if (response->content == NULL || response->content_len != response->content_total_len) {
        return STATE_HTTP_AUTH_NOT_EXPECTED;
    }

    core_log2(http_handle->sysdep, STATE_HTTP_LOG_AUTH, "%.*s\r\n", &response->content_len, response->content);

    res = core_json_value((const char *)response->content, response->content_len, "token", strlen("token"), &token,
                          &token_len);
    if (res < STATE_SUCCESS) {
        return STATE_HTTP_AUTH_TOKEN_FAILED;
    }

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    if (http_handle->token != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->token);
        http_handle->token = NULL;
    }
    http_handle->token = http_handle->sysdep->core_sysdep_malloc(token_len + 1, CORE_HTTP_MODULE_NAME);
    if (http_handle->token == NULL) {
        http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
        return STATE_SYS_DEPEND_MALLOC_FAILED;
    }
    memset(http_handle->token, 0, token_len + 1);
    memcpy(http_handle->token, token, token_len);
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);

    return STATE_SUCCESS;
}

void *aiot_http_init(void)
{
    core_http_handle_t *http_handle = NULL;

    http_handle = core_http_init();
    if (http_handle == NULL) {
        return NULL;
    }

    http_handle->auth_timeout_ms = CORE_HTTP_DEFAULT_AUTH_TIMEOUT_MS;
    http_handle->long_connection = 1;

    http_handle->exec_enabled = 1;

    return http_handle;
}

int32_t aiot_http_setopt(void *handle, aiot_http_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_HTTPOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (http_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_aiot_http_exec_inc(http_handle);

    http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
    switch (option) {
        case AIOT_HTTPOPT_HOST:
        case AIOT_HTTPOPT_PORT:
        case AIOT_HTTPOPT_NETWORK_CRED:
        case AIOT_HTTPOPT_CONNECT_TIMEOUT_MS:
        case AIOT_HTTPOPT_SEND_TIMEOUT_MS:
        case AIOT_HTTPOPT_RECV_TIMEOUT_MS:
        case AIOT_HTTPOPT_DEINIT_TIMEOUT_MS:
        case AIOT_HTTPOPT_HEADER_BUFFER_LEN:
        case AIOT_HTTPOPT_BODY_BUFFER_LEN:
        case AIOT_HTTPOPT_EVENT_HANDLER: {
            http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);
            res = core_http_setopt(handle, (core_http_option_t)option, data);
            http_handle->sysdep->core_sysdep_mutex_lock(http_handle->data_mutex);
        }
        break;
        case AIOT_HTTPOPT_USERDATA: {
            http_handle->userdata = data;
        }
        break;
        case AIOT_HTTPOPT_RECV_HANDLER: {
            http_handle->recv_handler = (aiot_http_recv_handler_t)data;
        }
        break;
        case AIOT_HTTPOPT_PRODUCT_KEY: {
            res = core_strdup(http_handle->sysdep, &http_handle->product_key, (char *)data, CORE_HTTP_MODULE_NAME);
        }
        break;
        case AIOT_HTTPOPT_DEVICE_NAME: {
            res = core_strdup(http_handle->sysdep, &http_handle->device_name, (char *)data, CORE_HTTP_MODULE_NAME);
        }
        break;
        case AIOT_HTTPOPT_DEVICE_SECRET: {
            res = core_strdup(http_handle->sysdep, &http_handle->device_secret, (char *)data, CORE_HTTP_MODULE_NAME);
        }
        break;
        case AIOT_HTTPOPT_EXTEND_DEVINFO: {
            res = core_strdup(http_handle->sysdep, &http_handle->extend_devinfo, (char *)data, CORE_HTTP_MODULE_NAME);
        }
        break;
        case AIOT_HTTPOPT_AUTH_TIMEOUT_MS: {
            http_handle->auth_timeout_ms = *(uint32_t *)data;
        }
        break;
        case AIOT_HTTPOPT_LONG_CONNECTION: {
            http_handle->long_connection = *(uint8_t *)data;
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    http_handle->sysdep->core_sysdep_mutex_unlock(http_handle->data_mutex);

    _core_aiot_http_exec_dec(http_handle);

    return res;
}

int32_t aiot_http_auth(void *handle)
{
    int32_t res = STATE_SUCCESS;
    core_http_response_t response;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (http_handle->product_key == NULL) {
        return STATE_USER_INPUT_MISSING_PRODUCT_KEY;
    }

    if (http_handle->device_name == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_NAME;
    }

    if (http_handle->device_secret == NULL) {
        return STATE_USER_INPUT_MISSING_DEVICE_SECRET;
    }

    if (http_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_aiot_http_exec_inc(http_handle);

    memset(&response, 0, sizeof(core_http_response_t));

    if ((res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_HANDLER,
                                (void *)_core_http_auth_recv_handler)) < STATE_SUCCESS ||
        (res = core_http_setopt(http_handle, CORE_HTTPOPT_USERDATA, (void *)&response)) < STATE_SUCCESS) {
        return res;
    }
    if (http_handle->network_handle == NULL ||
        (http_handle->network_handle != NULL && http_handle->long_connection == 0)) {
        if ((res = core_http_connect(http_handle)) < STATE_SUCCESS) {
            _core_aiot_http_exec_dec(http_handle);
            return res;
        }
    }

    /* send auth request */
    res = _core_http_send_auth(http_handle);
    if (res < STATE_SUCCESS) {
        _core_aiot_http_exec_dec(http_handle);
        return res;
    }

    /* recv auth response */
    res = _core_http_recv_auth(http_handle, &response);
    if (response.content != NULL) {
        http_handle->sysdep->core_sysdep_free(response.content);
    }

    _core_aiot_http_exec_dec(http_handle);

    return res;
}

int32_t aiot_http_send(void *handle, char *topic, uint8_t *payload, uint32_t payload_len)
{
    int32_t res = STATE_SUCCESS;
    char *path = NULL, *header = NULL;
    char *header_src[] = { NULL, NULL };
    core_http_request_t request;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL || topic == NULL || payload == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (payload_len == 0) {
        return STATE_USER_INPUT_OUT_RANGE;
    }
    if (http_handle->token == NULL) {
        return STATE_HTTP_NEED_AUTH;
    }
    if (http_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_aiot_http_exec_inc(http_handle);

    if (http_handle->network_handle == NULL ||
        (http_handle->network_handle != NULL && http_handle->long_connection == 0)) {
        if ((res = core_http_connect(http_handle)) < STATE_SUCCESS) {
            _core_aiot_http_exec_dec(http_handle);
            return res;
        }
    }

    /* path */
    res = core_sprintf(http_handle->sysdep, &path, "/topic%s", (char **)&topic, 1, CORE_HTTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        _core_aiot_http_exec_dec(http_handle);
        return res;
    }

    /* header */
    header_src[0] = http_handle->token;
    if (http_handle->long_connection == 0) {
        header_src[1] = "Connection: close\r\n";
    }
    res = core_sprintf(http_handle->sysdep, &header, "Content-Type: application/octet-stream\r\nPassword: %s\r\n%s",
                       header_src, sizeof(header_src) / sizeof(char *), CORE_HTTP_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        http_handle->sysdep->core_sysdep_free(path);
        _core_aiot_http_exec_dec(http_handle);
        return res;
    }

    memset(&request, 0, sizeof(core_http_request_t));
    request.method = "POST";
    request.path = path;
    request.header = header;
    request.content = (uint8_t *)payload;
    request.content_len = payload_len;

    res = core_http_send(http_handle, &request);
    http_handle->sysdep->core_sysdep_free(path);
    http_handle->sysdep->core_sysdep_free(header);

    _core_aiot_http_exec_dec(http_handle);

    return res;
}

int32_t aiot_http_recv(void *handle)
{
    int32_t res = STATE_SUCCESS;
    uint64_t timenow_ms = 0;
    core_http_response_t response;
    core_http_handle_t *http_handle = (core_http_handle_t *)handle;

    if (http_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    if (http_handle->network_handle == NULL) {
        return STATE_SYS_DEPEND_NWK_CLOSED;
    }
    if (http_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    _core_aiot_http_exec_inc(http_handle);

    memset(&response, 0, sizeof(core_http_response_t));
    if ((res = core_http_setopt(http_handle, CORE_HTTPOPT_RECV_HANDLER, (void *)_core_http_recv_handler)) < STATE_SUCCESS ||
        (res = core_http_setopt(http_handle, CORE_HTTPOPT_USERDATA, (void *)&response)) < STATE_SUCCESS) {
        return res;
    }
    timenow_ms = http_handle->sysdep->core_sysdep_time();
    while (1) {
        if (timenow_ms >= http_handle->sysdep->core_sysdep_time()) {
            timenow_ms = http_handle->sysdep->core_sysdep_time();
        }
        if (http_handle->sysdep->core_sysdep_time() - timenow_ms >= http_handle->recv_timeout_ms) {
            break;
        }

        res = core_http_recv(http_handle);
        if (res < STATE_SUCCESS) {
            break;
        }
    }

    if (res < STATE_SUCCESS) {
        if (res != STATE_HTTP_READ_BODY_FINISHED) {
            _core_aiot_http_exec_dec(http_handle);
            if (response.content != NULL) {
                http_handle->sysdep->core_sysdep_free(response.content);
            }
            return res;
        } else {
            res = STATE_SUCCESS;
        }
    } else {
        return STATE_HTTP_RECV_NOT_FINISHED;
    }

    _core_aiot_http_token_expired_event(http_handle, &response);

    if (http_handle->recv_handler != NULL) {
        aiot_http_recv_t packet;

        packet.type = AIOT_HTTPRECV_BODY;
        packet.data.body.buffer = response.content;
        packet.data.body.len = response.content_len;

        http_handle->recv_handler(http_handle, &packet, http_handle->userdata);
    }
    if (response.content != NULL) {
        http_handle->sysdep->core_sysdep_free(response.content);
    }

    _core_aiot_http_exec_dec(http_handle);

    return res;
}

int32_t aiot_http_deinit(void **p_handle)
{
    uint64_t deinit_timestart = 0;
    core_http_handle_t *http_handle = NULL;

    if (p_handle == NULL || *p_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    http_handle = *(core_http_handle_t **)p_handle;

    if (http_handle->exec_enabled == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    http_handle->exec_enabled = 0;
    deinit_timestart = http_handle->sysdep->core_sysdep_time();
    do {
        if (http_handle->exec_count == 0) {
            break;
        }
        http_handle->sysdep->core_sysdep_sleep(CORE_HTTP_DEINIT_INTERVAL_MS);
    } while ((http_handle->sysdep->core_sysdep_time() - deinit_timestart) < http_handle->deinit_timeout_ms);

    if (http_handle->exec_count != 0) {
        return STATE_HTTP_DEINIT_TIMEOUT;
    }

    if (http_handle->product_key != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->product_key);
    }
    if (http_handle->device_name != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->device_name);
    }
    if (http_handle->device_secret != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->device_secret);
    }
    if (http_handle->extend_devinfo != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->extend_devinfo);
    }
    if (http_handle->token != NULL) {
        http_handle->sysdep->core_sysdep_free(http_handle->token);
    }

    core_http_deinit(p_handle);

    return STATE_SUCCESS;
}

