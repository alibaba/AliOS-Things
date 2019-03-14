/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "http_client.h"
#include "http_client_wrapper.h"
#include "http_client_config.h"

httpc_t httpc_sessions[CONFIG_HTTPC_SESSION_NUM];

httpc_handle_t httpc_init(httpc_connection_t *settings)
{
    uint8_t index;
    httpc_param_t param;
    bool ret;
    uint16_t server_name_len = 0;
    uint16_t server_name_offset = 0;
    char *port_ptr;

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (httpc_sessions[index].socket == -1) {
            break;
        }
    }

    if (index == CONFIG_HTTPC_SESSION_NUM) {
        httpc_log("%s, no space for a new http session", __func__);
        return 0;
    }

    memset(&httpc_sessions[index], 0, sizeof(httpc_sessions[index]));
    if (settings->server_name == NULL ||
        strlen(settings->server_name) > CONFIG_HTTPC_SERVER_NAME_SIZE) {
        httpc_log("%s, server name is NULL or too long (max %d)", __func__, CONFIG_HTTPC_SERVER_NAME_SIZE);
        return 0;
    }

    if (settings->socket < 0) {
        httpc_log("%s, invalid socket for httpc connection", __func__);
        return 0;
    }

    server_name_len = strlen(settings->server_name);
    ret = http_str_search(settings->server_name, ":", 0, strlen(settings->server_name), &param);
    if (ret == false) {
        httpc_log("%s, no : in server name", __func__);
        return 0;
    }

    server_name_offset += param.len;

    // check https or http
    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
        httpc_sessions[index].flags |= HTTP_CLIENT_FLAG_SECURE;
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == false) {
        httpc_log("%s, no http or https in server name", __func__);
        return 0;
    }

    if (http_str_search(settings->server_name, "://", server_name_offset, 3, NULL) == false) {
        httpc_log("%s, server name format error", __func__);
        return 0;
    }
    server_name_offset += 3;

    // get host name
    if (http_str_search(settings->server_name, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        settings->server_name[server_name_len] = '/';
        if (http_str_search(settings->server_name, "/",
                            server_name_offset, server_name_len - server_name_offset, &param) == false) {
            httpc_log("%s, server name format error", __func__);
            return 0;
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
            return 0;
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

    return (httpc_handle_t)&httpc_sessions[index];
}

int8_t httpc_deinit(httpc_handle_t httpc)
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
    hdr_length = hdr_len + hdr_data_len + 4;

    if ((http_session->header_len + hdr_length) > CONFIG_HTTPC_HEADER_SIZE) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    memcpy(hdr_ptr, hdr_name, hdr_len);
    hdr_ptr += hdr_len;
    memcpy(hdr_ptr, ": ", 2);
    hdr_ptr += 2;
    memcpy(hdr_ptr, hdr_data, hdr_data_len);
    hdr_ptr += hdr_data_len;
    memcpy(hdr_ptr, HTTPC_CRLF, 2);
    hdr_ptr += 2;

    http_session->header_len += hdr_length;

    return HTTPC_SUCCESS;
}

static int8_t httpc_add_space(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->header_len;
    if (free_space < 1) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    *hdr_ptr = ' ';
    http_session->header_len += 1;
    return HTTPC_SUCCESS;
}

static bool is_valid_method(HTTPC_METHOD_TYPE_T method)
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
    int8_t ret;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->header_len;
    hdr_ptr = http_session->header + http_session->header_len;

    switch (method) {
        case GET:
            if (free_space < 3) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "GET", 3);
            http_session->header_len += 3;
            break;
        case PUT:
            if (free_space < 3) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "PUT", 3);
            http_session->header_len += 3;
            break;
        case POST:
            if (free_space < 4) {
                return HTTPC_FAIL;
            }
            strncpy(hdr_ptr, "POST", 4);
            http_session->header_len += 4;
            break;
        default:
            return HTTPC_FAIL;
    }

    ret = httpc_add_space(httpc);
    return ret;
}

static int8_t httpc_add_uri(httpc_handle_t httpc, const char *uri)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;
    int8_t ret;

    if (uri == NULL) {
        return HTTPC_SUCCESS;
    }

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->header_len;
    if (strlen(uri) > free_space) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    strncpy(hdr_ptr, uri, strlen(uri));
    http_session->header_len += strlen(uri);
    ret = httpc_add_space(httpc);
    return ret;
}

static int8_t httpc_add_version(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->header_len;
    if ((strlen(HTTPC_VERSION) + strlen(HTTPC_CRLF)) > free_space) {
        return HTTPC_FAIL;
    }

    hdr_ptr = http_session->header + http_session->header_len;
    strncpy(hdr_ptr, HTTPC_VERSION, strlen(HTTPC_VERSION));
    hdr_ptr += strlen(HTTPC_VERSION);
    strncpy(hdr_ptr, HTTPC_CRLF, strlen(HTTPC_CRLF));
    http_session->header_len += (strlen(HTTPC_VERSION) + strlen(HTTPC_CRLF));
    return HTTPC_SUCCESS;
}

int8_t httpc_send_request(httpc_handle_t httpc, HTTPC_METHOD_TYPE_T method,
                          const char *uri, char *param, uint16_t param_len)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int8_t ret = HTTPC_SUCCESS;
    int socket_res;
    char *hdr_ptr;

    if (is_valid_method(method) == false) {
        return HTTPC_FAIL;
    }

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    memset(http_session->header, 0, sizeof(http_session->header));
    http_session->header_len = 0;
    ret = httpc_add_method(httpc, method);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_uri(httpc, uri);
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

    ret = httpc_add_request_header(httpc, "Host", http_session->server_name);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_KEEP_ALIVE) == HTTP_CLIENT_FLAG_KEEP_ALIVE) {
        ret = httpc_add_request_header(httpc, "Connection", "Keep-Alive");
    } else {
        ret = httpc_add_request_header(httpc, "Connection", "close");
    }

    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }

    httpc_log("%s, send request header %s, socket %d, strlen %d, len %d\r\n",
              __func__, http_session->header, http_session->socket, strlen(http_session->header), http_session->header_len);

    if (http_session->connection == false) {
        struct hostent *host_entry;
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        host_entry = httpc_wrapper_gethostbyname(http_session->server_name);
        if (host_entry == NULL) {
            httpc_log("%s, get host name fail\r\n", __func__);
            ret = HTTPC_FAIL;
            goto exit;
        }
        addr.sin_family = AF_INET;
        // addr.sin_port = htons(http_session->port);
        addr.sin_port = htons(80);
        addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
        if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
            httpc_log("not support https\r\n");
        } else {
            socket_res = httpc_wrapper_connect(http_session->socket, (const struct sockaddr *)&addr, sizeof(addr));
        }
        httpc_log("%s, connect %d\r\n", __func__, socket_res);
        if (socket_res == 0) {
            http_session->connection = true;
        } else {
            ret = HTTPC_FAIL;
            goto exit;
        }
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
        httpc_log("not support https\r\n");
    } else {
        socket_res = httpc_wrapper_send(http_session->socket, http_session->header, strlen(http_session->header), 0);
    }

    if (socket_res < 0) {
        ret = HTTPC_FAIL;
    }

    httpc_log("%s, send %d, send res %d\r\n", __func__, strlen(http_session->header), socket_res);

exit:
    return ret;
}

int32_t http_parser(httpc_t *http_session, void *data, uint16_t len)
{
    int32_t parsed = 0;

    //http_session->recv_fn(http_session->socket, data, len, HTTPC_SUCCESS);
    return parsed;
}

static int http_client_recv(httpc_t *http_session, void *data, uint16_t len)
{
    int32_t parsed;

    httpc_log("%s, http session %x recv %d bytes data",
              __func__, (httpc_handle_t)http_session, len);
    httpc_log("%s", data);

    http_parser(http_session, data, len);

}

int8_t http_client_intialize(void)
{
    uint8_t index;

    memset(httpc_sessions, 0, sizeof(httpc_sessions));
    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        httpc_sessions[index].socket = -1;
    }

    httpc_wrapper_register_recv(httpc_sessions, http_client_recv);

    return HTTPC_SUCCESS;
}
