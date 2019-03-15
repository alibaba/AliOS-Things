/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "http_client.h"
#include "http_wrapper.h"
#include "http_config.h"
#include "http_parser.h"

httpc_t http_sessions[CONFIG_HTTPC_SESSION_NUM];

#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

static void print_header_field(int16_t len, const char *str)
{
#if defined(CONFIG_HTTP_DEBUG)
#define MAX_OUTPUT_LEN 128
    char output[MAX_OUTPUT_LEN];

    if ((len + 1) > sizeof(output)) {
        len = sizeof(output) - 1;
    }

    memset(output, 0, sizeof(output));
    strncpy(output, str, len);
    http_log("%s, len %d, %s", __func__, len, output);
#endif
}

static int on_body(struct http_parser *parser, const char *at, size_t length)
{
    httpc_t *http_session = CONTAINER_OF(parser, httpc_t, parser);

    http_session->rsp.body_present = 1;
    http_session->rsp.processed += length;

    http_log("%s, processed %d, len %d", __func__, http_session->rsp.processed, length);

    if (http_session->rsp.body_start == NULL) {
        http_session->rsp.body_start = (uint8_t *)at;
    }

    if (http_session->rsp.recv_fn) {
        http_session->rsp.recv_fn((httpc_handle_t)http_session, http_session->rsp.buf,
                                   http_session->rsp.buf_size, http_session->rsp.data_len, false);
        http_session->rsp.data_len = 0;
    }

    return 0;
}

static int on_chunk_header(struct http_parser *parser)
{
    (void)(parser);
    return 0;
}

static int on_chunk_complete(struct http_parser *parser)
{
    (void)(parser);
    return 0;
}

static int on_headers_complete(struct http_parser *parser)
{
    httpc_t *http_session = CONTAINER_OF(parser, httpc_t, parser);

    if (parser->status_code >= 500 && parser->status_code < 600) {
        http_log("%s, status %d, skipping body", parser->status_code);
        return 1;
    }

    if ((http_session->req.method == HTTP_HEAD || http_session->req.method == HTTP_OPTIONS) &&
        http_session->rsp.content_len > 0) {
        http_log("%s, no body expected", __func__);
        return 1;
    }

    http_log("%s, header complete", __func__);
    return 0;
}

static int on_header_field(struct http_parser *parser, const char *at, size_t length)
{
    char *content_len = "Content-Length";
    httpc_t *http_session;
    u16_t len;

    http_session = CONTAINER_OF(parser, httpc_t, parser);

    len = strlen(content_len);
    if (length >= len && memcmp(at, content_len, len) == 0) {
        http_session->rsp.content_len_present = 1;
    }

    print_header_field(length, at);
    return 0;
}

#define MAX_NUM_DIGITS  16
static int on_header_value(struct http_parser *parser, const char *at, size_t length)
{
    httpc_t *http_session;
    char str[MAX_NUM_DIGITS];

    http_session = CONTAINER_OF(parser, httpc_t, parser);

    if (http_session->rsp.content_len_present) {
        if (length <= MAX_NUM_DIGITS - 1) {
            long int num;

            memcpy(str, at, length);
            str[length] = 0;
            num = strtol(str, NULL, 10);
            if (num == INT32T_MIN || num == INT32T_MAX) {
                return -EINVAL;
            }
            http_session->rsp.content_len = num;
        }

        http_session->rsp.content_len_present = 0;
    }

    print_header_field(length, at);
    return 0;
}

static int on_message_begin(struct http_parser *parser)
{
#ifdef CONFIG_HTTP_DEBUG
    httpc_t *http_session = CONTAINER_OF(parser, httpc_t, parser);

    http_log("%s, HTTP response (headers), method %s", __func__, http_method_str(http_session->req.method));
#else
    (void)(parser);
#endif
    return 0;
}

static int on_message_complete(struct http_parser *parser) 
{
    httpc_t *http_session = CONTAINER_OF(parser, httpc_t, parser);

    http_log("%s, HTTP %s response (complete)", __func__, http_method_str(http_session->req.method));

    if (http_session->rsp.recv_fn) {
        http_session->rsp.recv_fn((httpc_handle_t)http_session, http_session->rsp.buf,
                                   http_session->rsp.buf_size, http_session->rsp.data_len, true);
    }

    return 0;
}

static int on_status(struct http_parser *parser, const char *at, size_t length)
{
    httpc_t *http_session = CONTAINER_OF(parser, httpc_t, parser);
    uint16_t len;

    len = (length < (sizeof(http_session->rsp.http_status) - 1))? \
          length: (sizeof(http_session->rsp.http_status) - 1);
    memcpy(http_session->rsp.http_status, at, len);
    http_session->rsp.http_status[len] = 0;

    http_log("%s, HTTP response status %s", __func__, http_session->rsp.http_status);
    return 0;
}

static int on_url(struct http_parser *parser, const char *at, size_t length)
{
    (void)(parser);
    http_log("%s, len %d", __func__, length);
    print_header_field(length, at);
    return 0;
}

httpc_handle_t httpc_init(httpc_connection_t *settings)
{
    uint8_t index;
    httpc_param_t param;
    bool ret;
    uint16_t server_name_len = 0;
    uint16_t server_name_offset = 0;
    char *port_ptr;

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (http_sessions[index].socket == -1) {
            break;
        }
    }

    if (index == CONFIG_HTTPC_SESSION_NUM) {
        http_log("%s, no space for a new http session", __func__);
        return 0;
    }

    memset(&http_sessions[index], 0, sizeof(http_sessions[index]));
    if (settings->server_name == NULL ||
        strlen(settings->server_name) > CONFIG_HTTPC_SERVER_NAME_SIZE) {
        http_log("%s, server name is NULL or too long (max %d)", __func__, CONFIG_HTTPC_SERVER_NAME_SIZE);
        return 0;
    }

    if (settings->socket < 0) {
        http_log("%s, invalid socket for httpc connection", __func__);
        return 0;
    }

    server_name_len = strlen(settings->server_name);
    ret = http_str_search(settings->server_name, ":", 0, strlen(settings->server_name), &param);
    if (ret == false) {
        http_log("%s, no : in server name", __func__);
        return 0;
    }

    server_name_offset += param.len;

    // check https or http
    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
        http_sessions[index].flags |= HTTP_CLIENT_FLAG_SECURE;
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == false) {
        http_log("%s, no http or https in server name", __func__);
        return 0;
    }

    if (http_str_search(settings->server_name, "://", server_name_offset, 3, NULL) == false) {
        http_log("%s, server name format error", __func__);
        return 0;
    }
    server_name_offset += 3;

    // get host name
    if (http_str_search(settings->server_name, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        settings->server_name[server_name_len] = '/';
        if (http_str_search(settings->server_name, "/",
                            server_name_offset, server_name_len - server_name_offset, &param) == false) {
            http_log("%s, server name format error", __func__);
            return 0;
        }
    }
    memset(http_sessions[index].server_name, 0, sizeof(http_sessions[index].server_name));
    strncpy(http_sessions[index].server_name, param.param, param.len);

    server_name_offset += param.len;
    if (http_str_search(http_sessions[index].server_name, ":", server_name_offset - param.len,
                        param.len, &param) == true) {
        if ((strlen(http_sessions[index].server_name) - param.len) < 10) {
            param.param += (param.len + 1);
            port_ptr = param.param;
            while (*port_ptr && port_ptr++) {
                if (*port_ptr == '/') {
                    *port_ptr = 0;
                    http_sessions[index].port = (uint16_t)atol(port_ptr);
                    http_sessions[index].flags |= HTTP_CLIENT_FLAG_PORT;
                    break;
                }
            }
        } else {
            http_log("%s, server name port too large", __func__);
            return 0;
        }
    }

    http_sessions[index].index = index;
    http_sessions[index].socket = settings->socket;
    http_sessions[index].rsp.recv_fn = settings->recv_fn;

    if (settings->keep_alive) {
        http_sessions[index].flags |= HTTP_CLIENT_FLAG_KEEP_ALIVE;
    }

    // default no cache
    http_sessions[index].flags |= HTTP_CLIENT_FLAG_NO_CACHE;

    // response buffer
    http_sessions[index].rsp.buf = settings->rsp_buf;
    http_sessions[index].rsp.buf_size = settings->rsp_buf_size;

    http_sessions[index].parser_settings.on_body = on_body;
    http_sessions[index].parser_settings.on_chunk_complete = on_chunk_complete;
    http_sessions[index].parser_settings.on_chunk_header = on_chunk_header;
    http_sessions[index].parser_settings.on_headers_complete = on_headers_complete;
    http_sessions[index].parser_settings.on_header_field = on_header_field;
    http_sessions[index].parser_settings.on_header_value = on_header_value;
    http_sessions[index].parser_settings.on_message_begin = on_message_begin;
    http_sessions[index].parser_settings.on_message_complete = on_message_complete;
    http_sessions[index].parser_settings.on_status = on_status;
    http_sessions[index].parser_settings.on_url = on_url;

    return (httpc_handle_t)&http_sessions[index];
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

static bool is_valid_method(int method)
{
    if (method == HTTP_GET) {
        return true;
    }

    return false;
}

static int8_t httpc_add_method(httpc_handle_t httpc, int method)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;
    const char *method_str;

    if (http_session == NULL) {
        return HTTPC_FAIL;
    }

    free_space = CONFIG_HTTPC_HEADER_SIZE - http_session->header_len;
    hdr_ptr = http_session->header + http_session->header_len;

    method_str = http_method_str(method);

    if (strcmp(method_str, "<unknown>") == 0) {
        return HTTPC_FAIL;
    }

    if (free_space < strlen(method_str)) {
        return HTTPC_FAIL;
    }

    strncpy(hdr_ptr, method_str, strlen(method_str));
    return httpc_add_space(httpc);
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

static int8_t httpc_reset(httpc_t *http_session)
{
    http_parser_init(&http_session->parser, HTTP_RESPONSE);

    memset(http_session->header, 0, sizeof(http_session->header));
    http_session->header_len = 0;

    http_session->rsp.content_len_present = 0;
    http_session->rsp.content_len = 0;
    http_session->rsp.body_start = NULL;
    http_session->rsp.processed = 0;

    memset(http_session->rsp.buf, 0, http_session->rsp.buf_size);
    http_session->rsp.data_len = 0;

    return HTTPC_SUCCESS;
}

int8_t httpc_send_request(httpc_handle_t httpc, int method,
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

    if (httpc_reset(http_session) != HTTPC_SUCCESS) {
        return HTTPC_FAIL;
    }

    ret = httpc_add_method(httpc, method);
    if (ret != HTTPC_SUCCESS) {
        goto exit;
    }
    http_session->req.method = method;

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

    http_log("%s, send request header %s, socket %d, strlen %d, len %d",
              __func__, http_session->header, http_session->socket, strlen(http_session->header), http_session->header_len);

    if (http_session->connection == false) {
        struct hostent *host_entry;
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        host_entry = httpc_wrapper_gethostbyname(http_session->server_name);
        if (host_entry == NULL) {
            http_log("%s, get host name fail", __func__);
            ret = HTTPC_FAIL;
            goto exit;
        }
        addr.sin_family = AF_INET;
        // addr.sin_port = htons(http_session->port);
        addr.sin_port = htons(80);
        addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
        if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
            http_log("not support https");
        } else {
            socket_res = httpc_wrapper_connect(http_session->socket, (const struct sockaddr *)&addr, sizeof(addr));
        }
        http_log("%s, connect %d", __func__, socket_res);
        if (socket_res == 0) {
            http_session->connection = true;
        } else {
            ret = HTTPC_FAIL;
            goto exit;
        }
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
        http_log("not support https");
    } else {
        socket_res = httpc_wrapper_send(http_session->socket, http_session->header, strlen(http_session->header), 0);
    }

    if (socket_res < 0) {
        ret = HTTPC_FAIL;
    }

    http_log("%s, send %d, send res %d", __func__, strlen(http_session->header), socket_res);

exit:
    return ret;
}

static int http_client_recv(httpc_t *http_session, void *data, int32_t len)
{
    int32_t copy_len = len;
    int32_t start = http_session->rsp.data_len;
    int32_t free_space;

    free_space = http_session->rsp.buf_size - http_session->rsp.data_len;
    if (copy_len > free_space) {
        copy_len = free_space;
    }

    memcpy(http_session->rsp.buf + start, data, copy_len);
    http_session->rsp.data_len = len;
    http_parser_execute(&http_session->parser, &http_session->parser_settings,
                        (const char *)http_session->rsp.buf + start, copy_len);
    http_session->rsp.data_len = 0;
    return 0;
}

int8_t http_client_intialize(void)
{
    uint8_t index;

    memset(http_sessions, 0, sizeof(http_sessions));
    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        http_sessions[index].socket = -1;
    }

    httpc_wrapper_register_recv(http_sessions, http_client_recv);

    return HTTPC_SUCCESS;
}
