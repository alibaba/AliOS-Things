/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "http_client.h"
#include "http_client_config.h"

httpc_t httpc_sessions[HTTPC_SESSION_NUM];

httpc_handle_t httpc_init(httpc_connection_t *settings)
{
    uint8_t index;
    httpc_param_t param;
    bool ret;
    uint16_t server_name_len = 0;
    uint16_t server_name_offset = 0;
    char *port_ptr;

    for (index = 0; index < HTTPC_SESSION_NUM; index++) {
        if (httpc_sessions[index].socket == -1) {
            break;
        }
    }

    // no space for a new http client session
    if (index == HTTPC_SESSION_NUM) {
        return NULL;
    }

    if (strlen(settings->server_name) > CONFIG_HTTPC_SERVER_NAME_SIZE) {
        httpc_log("%s, server name is too long", __func__);
        return NULL;
    }

    if (settings->socket < 0) {
        httpc_log("%s, invalid socket for httpc connection", __func__);
        return NULL;
    }

    server_name_len = strlen(settings->server_name);
    ret = http_str_search(httpc_sessions[index].server_name, ":", 0,
                          strlen(httpc_sessions[index].server_name), &param);
    if (ret == false) {
        httpc_log("%s, no : in server name", __func__);
        return NULL;
    }

    server_name_offset += param.len;

    // check https or http
    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
        httpc_sessions[index].flags |= HTTP_CLIENT_FLAG_SECURE;
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == true) {
        httpc_log("%s, no http or https in server name", __func__);
        return NULL;
    }

    if (http_str_search(httpc_sessions[index].server_name, "://",
                        server_name_offset, 3, NULL) == false) {
        httpc_log("%s, server name format error", __func__);
        return NULL;
    }
    server_name_offset += 3;

    // get host name
    if (http_str_search(httpc_sessions[index].server_name, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        httpc_sessions[index].server_name[server_name_len] = "/";
        if (http_str_search(httpc_sessions[index].server_name, "/",
                            server_name_offset, server_name_len - server_name_offset, &param) == false) {
            httpc_log("%s, server name format error", __func__);
            return NULL;
        }
    }
    memset(httpc_sessions[index].server_name, 0, sizeof(httpc_sessions[index].server_name));
    strncpy(httpc_sessions[index].server_name, param.param, param.len);

    server_name_offset += param.len;
    if (http_str_search(httpc_sessions[index].server_name, ":", server_name_offset - param.len,
                        param.len, &param) == true) {
        if ((strlen(httpc_sessions[index].server_name) - param.len) < 10) {
            param.param += (param.len + 1);
            port_ptr = param.param;
            while (*port_ptr && port_ptr++) {
                if (*port_ptr == '/') {
                    *port_ptr = 0;
                    httpc_sessions[index].port = (uint16_t)atol(port_ptr);
                    httpc_sessions[index].flags |= HTTP_CLIENT_FLAG_PORT;
                    break;
                }
            }
        } else {
            httpc_log("%s, server name port too large", __func__);
            return NULL;
        }
    }

    httpc_sessions[index].index = index;
    httpc_sessions[index].socket = settings->socket;
    httpc_sessions[index].recv_fn = settings->recv_fn;

    if (settings->keep_alive) {
        httpc_sessions[index].flags |= HTTP_CLIENT_FLAG_KEEP_ALIVE;
    }

    // default no cache
    httpc_sessions[index].flags |= HTTP_CLIENT_FLAG_NO_CACHE;

    return &httpc_sessions[index];
}

int httpc_deinit(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    memset(http_session, 0, sizeof(httpc_t));
    http_session->socket = -1;
    return HTTPC_SUCCESS;
}

int8_t httpc_add_request_header(httpc_handle_t httpc, char *hdr_name, char *hdr_data)
{
    httpc_t *http_session = (httpc_t *)httpc;
    uint16_t hdr_len;
    uint16_t hdr_data_len;
    char *hdr_ptr;
    uint16_t hdr_length;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    hdr_len = strlen(hdr_name);
    hdr_data_len = strlen(hdr_data);
    hdr_length = hdr_len + hdr_length_len + 4;

    if ((httpc->headr_len + hdr_length) > CONFIG_HTTPC_HEADER_SIZE) {
        return HTTPC_FAIL;
    }

    hdr_ptr = httpc->header + httpc->header_len;
    memcpy(hdr_ptr, hdr_name, hdr_len);
    hdr_ptr += hdr_len;
    memcpy(hdr_ptr, ": ", 2);
    hdr_ptr += 2;
    memcpy(hdr_ptr, hdr_data, hdr_data_len);
    hdr_ptr += hdr_data_len;
    memcpy(hdr_ptr, HTTPC_CRLF, 2);
    hdr_ptr += 2;

    httpc->header += hdr_ptr;
    httpc->header_len += hdr_length;

    return HTTPC_SUCCESS;
}

static int8_t httpc_add_space(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    free_space = CONFIG_HTTPC_HEADER_SIZE - httpc->headr_len;
    if (free_space < 1) {
        return HTTPC_FAIL;
    }

    hdr_ptr = httpc->header + httpc->header_len;
    *hdr_ptr = ' ';
    httpc->header_len += 1;
    return HTTPC_SUCCESS;
}

static is_valid_method(HTTPC_METHOD_TYPE_T method)
{
    if (method == GET) {
        return true;
    }

    return false;
}

static int8_t httpc_add_method(httpc_handle_t httpc, HTTPC_METHOD_TYPE_T method)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - httpc->headr_len;
    hdr_ptr = httpc->header + httpc->header_len;

    switch (method) {
        case GET:
            if (free_space < 4) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "GET", 4);
            httpc->header_len += 4;
            break;
        case PUT:
            if (free_space < 4) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "PUT", 4);
            httpc->header_len += 4;
            break;
        case POST:
            if (free_space < 5) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "POST", 5);
            httpc->header_len += 5;
            break;
        default:
            return HTTPC_FAIL;
    }

    return HTTPC_SUCCESS;
}

static int8_t httpc_add_uri(httpc_handle_t httpc,const char *uri)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->headr_len;
    if (strlen(uri) > free_space) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    strncpy(hdr_ptr, uri, strlen(uri));
    http_session->headr_len += strlen(uri);
    return HTTPC_SUCCESS;
}

static int8_t httpc_add_version(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->headr_len;
    if ((strlen(HTTPC_VERSION) + 2) > free_space) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    strncpy(hdr_ptr, HTTPC_VERSION, strlen(HTTPC_VERSION));
    strncpy(hdr_ptr, HTTPC_CRLF, strlen(HTTPC_CRLF));
    http_session->headr_len += (strlen(HTTPC_VERSION) + strlen(HTTPC_VERSION));
    return HTTPC_SUCCESS;
}

int8_t httpc_send_request(httpc_handle_t httpc, HTTPC_METHOD_TYPE_T method,
                          const char *uri, char *param, uint16_t param_len)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int8_t ret = HTTPC_SUCCESS;
    char *hdr_ptr;

    if (is_valid_method(method) == false) {
        return HTTPC_FAIL;
    }

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    ret = httpc_add_method(httpc, method);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_space(httpc);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_uri(httpc, uri);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_space(httpc);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_version(httpc);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_request_header(httpc, "User-Agent", CONFIG_HTTPC_DEFAULT_CLIENT);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_NO_CACHE) == HTTP_CLIENT_FLAG_NO_CACHE) {
        ret = httpc_add_request_header(httpc, "Cache-Control", "no-cache");
        if (ret != HTTPC_SUCCESS) {
            goto exit;
        }
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_KEEP_ALIVE) == HTTP_CLIENT_FLAG_KEEP_ALIVE) {
        ret = httpc_add_request_header(httpc, "Connection", "Keep-Alive");
    } else {
        ret = httpc_add_request_header(httpc, "Connection", "close");
    }
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

exit:
    return ret;
}

int8_t http_client_intialize(void)
{
    uint8_t index;

    memset(httpc_sessions, 0, sizeof(httpc_sessions));
    for (index = 0; index < HTTPC_SESSION_NUM; index++) {
        httpc_sessions.socket = -1;
    }

    return HTTPC_SUCCESS;
}
