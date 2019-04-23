/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "http_client.h"
#include "http_wrapper.h"
#include "http_def_config.h"
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

    //http_log("%s, processed %d, len %d", __func__, http_session->rsp.processed, length);

    if (http_session->rsp.body_start == NULL) {
        http_session->rsp.body_start = (uint8_t *)at;
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

    http_session->rsp.headers_complete = 1;

    if (parser->status_code >= 500 && parser->status_code < 600) {
        http_log("%s, status %d, skipping body", parser->status_code);
        return 1;
    }

    if (http_session->req.method == HTTP_OPTIONS && http_session->rsp.content_len > 0) {
        http_log("%s, no body expected", __func__);
        return 1;
    }

    http_log("%s, headers complete", __func__);
    return 0;
}

static int on_header_field(struct http_parser *parser, const char *at, size_t length)
{
    char *content_len = "Content-Length";
    httpc_t *http_session;
    uint16_t len;

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
    http_session->rsp.message_complete = 1;
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
#if CONFIG_HTTP_SECURE
        http_sessions[index].flags |= HTTP_CLIENT_FLAG_SECURE;
        if (settings->ca_cert == NULL) {
            http_log("%s, no ca cert", __func__);
        }
        http_sessions[index].https.ssl.ca_cert_c = settings->ca_cert;
#else
        http_log("%s, https not available", __func__);
        return 0;
#endif
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
    strncpy(http_sessions[index].server_name, param.param, param.len + 1);

    if (http_str_search(http_sessions[index].server_name, ":", 0, param.len, &param) == true) {
        if ((strlen(http_sessions[index].server_name) - param.len) < 10) {
            param.param += (param.len + 1);
            port_ptr = param.param;
            while (*port_ptr && port_ptr++) {
                if (*port_ptr == '/') {
                    *port_ptr = 0;
                    http_sessions[index].port = (uint16_t)atol(param.param);
                    http_sessions[index].flags |= HTTP_CLIENT_FLAG_PORT;
                    break;
                }
            }
        } else {
            http_log("%s, server name port too large", __func__);
            return 0;
        }
        memset(http_sessions[index].server_name + param.len, 0, strlen(http_sessions[index].server_name) - param.len);
    } else {
        memset(http_sessions[index].server_name, 0, sizeof(http_sessions[index].server_name));
        strncpy(http_sessions[index].server_name, param.param, param.len);
    }

    http_sessions[index].index = index;
    http_sessions[index].socket = settings->socket;

    if (settings->keep_alive) {
        http_sessions[index].flags |= HTTP_CLIENT_FLAG_KEEP_ALIVE;
    }

    // default no cache
    http_sessions[index].flags |= HTTP_CLIENT_FLAG_NO_CACHE;

    // request buffer
    http_sessions[index].req.buf = settings->req_buf;
    http_sessions[index].req.buf_size = settings->req_buf_size;

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
        return HTTP_EARG;
    }

#if CONFIG_HTTP_SECURE
    if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
        httpc_wrapper_ssl_destroy((httpc_handle_t)http_session);
    }
#endif
    memset(http_session, 0, sizeof(httpc_t));
    http_session->socket = -1;
    return HTTP_SUCCESS;
}

int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data)
{
    uint16_t hdr_len;
    uint16_t hdr_data_len;
    uint16_t hdr_length;

    if (buf == NULL || buf_size == 0 || name == NULL || data == NULL) {
        return HTTP_EARG;
    }

    hdr_len = strlen(name);
    hdr_data_len = strlen(data);
    hdr_length = hdr_len + hdr_data_len + 4;

    if (hdr_length > buf_size) {
        return HTTP_ENOBUFS;
    }

    memcpy(buf, name, hdr_len);
    buf += hdr_len;
    memcpy(buf, ": ", 2);
    buf += 2;
    memcpy(buf, data, hdr_data_len);
    buf += hdr_data_len;
    memcpy(buf, HTTPC_CRLF, 2);

    return (int32_t)hdr_length;
}

static int8_t httpc_add_request_header(httpc_handle_t httpc, const char *hdr_name, const char *hdr_data)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *buf;
    uint16_t buf_size;
    int32_t hdr_length;

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    buf = (char *)http_session->req.buf + http_session->req.data_len;
    buf_size = http_session->req.buf_size - http_session->req.data_len;
    hdr_length = httpc_construct_header(buf, buf_size, hdr_name, hdr_data);
    if (hdr_length < 0) {
        return hdr_length;
    }
    http_session->req.data_len += hdr_length;

    return HTTP_SUCCESS;
}

static int8_t httpc_add_space(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (free_space < 1) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    *hdr_ptr = ' ';
    http_session->req.data_len += 1;
    return HTTP_SUCCESS;
}

static bool is_valid_method(int method)
{
    if (method == HTTP_GET || method == HTTP_POST || method == HTTP_PUT || method == HTTP_HEAD) {
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
        return HTTP_EARG;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    hdr_ptr = http_session->req.buf + http_session->req.data_len;

    method_str = http_method_str(method);

    if (strcmp(method_str, "<unknown>") == 0) {
        return HTTP_EARG;
    }

    if (free_space < strlen(method_str)) {
        return HTTP_ENOBUFS;
    }

    strncpy(hdr_ptr, method_str, strlen(method_str));
    http_session->req.data_len += strlen(method_str);
    return httpc_add_space(httpc);
}

static int8_t httpc_add_uri(httpc_handle_t httpc, const char *uri)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;
    int8_t ret;

    if (uri == NULL) {
        return HTTP_SUCCESS;
    }

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (strlen(uri) > free_space) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    strncpy(hdr_ptr, uri, strlen(uri));
    http_session->req.data_len += strlen(uri);
    ret = httpc_add_space(httpc);
    return ret;
}

static int8_t httpc_add_version(httpc_handle_t httpc)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if ((strlen(HTTPC_VERSION) + strlen(HTTPC_CRLF)) > free_space) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = http_session->req.buf + http_session->req.data_len;
    strncpy(hdr_ptr, HTTPC_VERSION, strlen(HTTPC_VERSION));
    hdr_ptr += strlen(HTTPC_VERSION);
    strncpy(hdr_ptr, HTTPC_CRLF, strlen(HTTPC_CRLF));
    http_session->req.data_len += (strlen(HTTPC_VERSION) + strlen(HTTPC_CRLF));
    return HTTP_SUCCESS;
}

static int8_t httpc_add_header_field(httpc_handle_t httpc, const char *hdr, uint16_t size)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (size > free_space) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    strncpy(hdr_ptr, hdr, size);
    hdr_ptr += size;
    http_session->req.data_len += size;
    return HTTP_SUCCESS;
}

static int8_t httpc_add_payload(httpc_handle_t httpc, const char *param, uint16_t param_len)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;
    int8_t ret = HTTP_SUCCESS;
    char content_len_str[HTTP_CONTENT_LEN_SIZE];

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    memset(content_len_str, 0, HTTP_CONTENT_LEN_SIZE);
    snprintf(content_len_str, HTTP_CONTENT_LEN_SIZE, "%d", param_len);
    ret = httpc_add_request_header(httpc, "Content-Length", content_len_str);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (2 > free_space) {
        return HTTP_ENOBUFS;
    }
    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    memcpy(hdr_ptr, HTTPC_CRLF, 2);
    http_session->req.data_len += 2;

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (param_len > free_space) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    strncpy(hdr_ptr, param, param_len);
    http_session->req.data_len += param_len;
    return HTTP_SUCCESS;
}

static int8_t httpc_reset(httpc_t *http_session)
{
    http_parser_init(&http_session->parser, HTTP_RESPONSE);

    http_session->rsp.content_len_present = 0;
    http_session->rsp.content_len = 0;
    http_session->rsp.body_start = NULL;
    http_session->rsp.processed = 0;
    http_session->rsp.body_present = 0;
    http_session->rsp.message_complete = 0;
    http_session->rsp.headers_complete = 0;

    memset(http_session->req.buf, 0, http_session->req.buf_size);
    http_session->req.data_len = 0;

    return HTTP_SUCCESS;
}

int32_t httpc_send_request(httpc_handle_t httpc, int32_t method, const char *uri,
                           const char *hdr, const char *content_type, const char *param, uint16_t param_len)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int8_t ret = HTTP_SUCCESS;
    int socket_res;
    char *hdr_ptr;

    if (is_valid_method(method) == false) {
        return HTTP_EARG;
    }

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    httpc_reset(http_session);
    ret = httpc_add_method(httpc, method);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }
    http_session->req.method = method;

    ret = httpc_add_uri(httpc, uri);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_version(httpc);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_request_header(httpc, "User-Agent", CONFIG_HTTPC_DEFAULT_CLIENT);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_NO_CACHE) == HTTP_CLIENT_FLAG_NO_CACHE) {
        ret = httpc_add_request_header(httpc, "Cache-Control", "no-cache");
        if (ret != HTTP_SUCCESS) {
            goto exit;
        }
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_KEEP_ALIVE) == HTTP_CLIENT_FLAG_KEEP_ALIVE) {
        ret = httpc_add_request_header(httpc, "Connection", "Keep-Alive");
    } else {
        ret = httpc_add_request_header(httpc, "Connection", "close");
    }

    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = httpc_add_request_header(httpc, "Host", http_session->server_name);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    if (hdr) {
        ret = httpc_add_header_field(httpc, hdr, strlen(hdr));
        if (ret != HTTP_SUCCESS) {
            goto exit;
        }
    }

    if (content_type) {
        ret = httpc_add_request_header(httpc, "Content-Type", content_type);
        if (ret != HTTP_SUCCESS) {
            goto exit;
        }
    }

    if (param && param_len > 0) {
        ret = httpc_add_payload(httpc, param, param_len);
        if (ret != HTTP_SUCCESS) {
            goto exit;
        }
    } else {
        hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
        memcpy(hdr_ptr, HTTPC_CRLF, 2);
        http_session->req.data_len += 2;
    }

    http_log("%s, send request header\r\n%s\r\nsocket %d, strlen %d, len %d",
              __func__, http_session->req.buf, http_session->socket,
              strlen((const char *)http_session->req.buf), http_session->req.data_len);

    if (http_session->connection == false) {
        struct hostent *host_entry;
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        host_entry = httpc_wrapper_gethostbyname(http_session->server_name);
        if (host_entry == NULL) {
            http_log("%s, get host name fail", __func__);
            ret = HTTP_EDNS;
            goto exit;
        }
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = *(uint32_t *)(host_entry->h_addr);
        if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
#if CONFIG_HTTP_SECURE
            if ((http_session->flags & HTTP_CLIENT_FLAG_PORT) == HTTP_CLIENT_FLAG_PORT) {
                addr.sin_port = htons(http_session->port);
            } else {
                addr.sin_port = htons(443);
            }
            socket_res = httpc_wrapper_ssl_connect(httpc, (const struct sockaddr *)&addr, sizeof(addr));
#else
            http_log("%s, https not available", __func__);
            ret = HTTP_ENOTSUPP;
            goto exit;
#endif
        } else {
            if ((http_session->flags & HTTP_CLIENT_FLAG_PORT) == HTTP_CLIENT_FLAG_PORT) {
                addr.sin_port = htons(http_session->port);
            } else {
                addr.sin_port = htons(80);
            }
            socket_res = httpc_wrapper_connect(http_session->socket, (const struct sockaddr *)&addr, sizeof(addr));
        }
        http_log("%s, connect %d", __func__, socket_res);
        if (socket_res == 0) {
            http_session->connection = true;
        } else {
            ret = HTTP_ECONN;
            goto exit;
        }
    }

    if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
#if CONFIG_HTTP_SECURE
        socket_res = httpc_wrapper_ssl_send((httpc_handle_t)http_session, http_session->req.buf,
                                            strlen((const char *)http_session->req.buf), 0);
#else
        socket_res = -1;
#endif
    } else {
        socket_res = httpc_wrapper_send(http_session->socket, http_session->req.buf, strlen((const char *)http_session->req.buf), 0);
    }

    if (socket_res < 0) {
        ret = HTTP_ESEND;
    }

    http_log("%s, send %d, send res %d", __func__, strlen((const char *)http_session->req.buf), socket_res);

exit:
    return ret;
}

int32_t httpc_recv_response(httpc_handle_t httpc, uint8_t *rsp, uint32_t rsp_size,
                            http_rsp_info_t *info, uint32_t timeout)
{
    httpc_t *http_session = (httpc_t *)httpc;
    int32_t ret;

    if (http_session == NULL || rsp == NULL || rsp_size == 0) {
        return HTTP_EARG;
    }

#if CONFIG_HTTP_SECURE
    if ((http_session->flags & HTTP_CLIENT_FLAG_SECURE) == HTTP_CLIENT_FLAG_SECURE) {
        ret = httpc_wrapper_ssl_recv((httpc_handle_t)http_session, rsp, rsp_size, timeout);
    } else
#endif
    {
        ret = httpc_wrapper_recv(http_session->socket, rsp, rsp_size, timeout);
    }

    if (ret < 0) {
        http_log("%s, recv %d, recv res %d", __func__, rsp_size, ret);
        return ret;
    }

    http_parser_execute(&http_session->parser, &http_session->parser_settings, (const char *)rsp, ret);

    if (info) {
        info->rsp_len = ret;
        info->content_len_present = http_session->rsp.content_len_present;
        info->body_present = http_session->rsp.body_present;
        info->body_start = http_session->rsp.body_start;
        info->message_complete = http_session->rsp.message_complete;
        info->headers_complete = http_session->rsp.headers_complete;
    }

    return HTTP_SUCCESS;
}

int8_t http_client_initialize(void)
{
    uint8_t index;

    memset(http_sessions, 0, sizeof(http_sessions));
    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        http_sessions[index].socket = -1;
    }

    return HTTP_SUCCESS;
}
