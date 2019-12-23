/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#if CONFIG_HTTP_SECURE
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif /* !defined(MBEDTLS_CONFIG_FILE) */
#endif /* CONFIG_HTTP_SECURE */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <http_def_config.h>
#include <http_parser.h>

#include "http_client.h"
#include "http_wrapper.h"
#include "network/network.h"

#if CONFIG_HTTP_SECURE
#include "mbedtls/debug.h"
#endif

#include "ulog/ulog.h"
#define HTTPCLIENT_DEBUG 0

#define TAG "httpclient"

static httpc_t http_sessions[CONFIG_HTTPC_SESSION_NUM];

#define HTTP_SERVER_PORT_SIZE 7
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#ifndef MIN
#define MIN(x,y) (((x)<(y))?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#define HTTPCLIENT_AUTHB_SIZE     128

#define HTTPCLIENT_CHUNK_SIZE     1024
#define HTTPCLIENT_SEND_BUF_SIZE  512

#define HTTPCLIENT_MAX_HOST_LEN   64
#define HTTPCLIENT_MAX_URL_LEN    512

#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 2
#endif

#define HTTP_DATA_SIZE    1500

#define FORM_DATA_MAXLEN 32
typedef struct formdata_node_t formdata_node_t;
struct formdata_node_t
{
    formdata_node_t *next;
    int   is_file;
    char  file_path[FORM_DATA_MAXLEN];
    char  *data;
    int   data_len;
};

typedef struct {
    int                is_used;
    formdata_node_t    *form_data;
    httpclient_data_t  *client_data;
} formdata_info_t;

#define CLIENT_FORM_DATA_NUM  1
static formdata_info_t formdata_info[CLIENT_FORM_DATA_NUM] = {0};

static int httpclient_parse_host(char *url, char *host, size_t maxhost_len);
static int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len);
static int httpclient_tcp_send_all(int sock_fd, char *data, int length);
static int httpclient_conn(httpclient_t *client, char *host);
static int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len);
static int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
static int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
#if CONFIG_HTTP_SECURE
static int httpclient_ssl_conn(httpclient_t *client, char *host);
static int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length);
static int httpclient_ssl_nonblock_recv(void *ctx, unsigned char *buf, size_t len);
static int httpclient_ssl_close(httpclient_t *client);
#endif

#ifndef strncasecmp
static const unsigned char charmap[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strncasecmp(const char *s1, const char *s2, register size_t n)
{
    register unsigned char u1, u2;
    for (; n != 0; --n) {
        u1 = (unsigned char) *s1++;
        u2 = (unsigned char) *s2++;
        if (charmap[u1] != charmap[u2]) {
            return charmap[u1] - charmap[u2];
        }
        if (u1 == '\0') {
            return 0;
        }
    }
    return 0;
}
#endif

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

    /* http_log("%s, processed %d, len %d", __func__, http_session->rsp.processed, length); */

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

    httpc_wrapper_lock_mutex();

    for (index = 0; index < CONFIG_HTTPC_SESSION_NUM; index++) {
        if (http_sessions[index].socket == -1) {
            break;
        }
    }

    if (index == CONFIG_HTTPC_SESSION_NUM) {
        http_log("%s, no space for a new http session", __func__);
        goto exit;
    }

    memset(&http_sessions[index], 0, sizeof(http_sessions[index]));
    http_sessions[index].socket = -1;
    if (settings->server_name == NULL ||
        strlen(settings->server_name) > CONFIG_HTTPC_SERVER_NAME_SIZE) {
        http_log("%s, server name is NULL or too long (max %d)", __func__, CONFIG_HTTPC_SERVER_NAME_SIZE);
        goto exit;
    }

    if (settings->socket < 0) {
        http_log("%s, invalid socket for httpc connection", __func__);
        goto exit;
    }

    server_name_len = strlen(settings->server_name);
    ret = http_str_search(settings->server_name, ":", 0, strlen(settings->server_name), &param);
    if (ret == false) {
        http_log("%s, no : in server name", __func__);
        goto exit;
    }

    server_name_offset += param.len;

    /* check https or http */
#if CONFIG_HTTP_SECURE
    if ((settings->flags & HTTP_ALWAYS_HTTP_FLAG) == HTTP_ALWAYS_HTTP_FLAG) {
        http_sessions[index].flags &= (~HTTP_CLIENT_FLAG_SECURE);
        http_sessions[index].https.ssl.ca_cert_c = NULL;
    } else
#endif
    if (http_str_insensitive_cmp(param.param, "https", param.len) == true) {
#if CONFIG_HTTP_SECURE
        http_sessions[index].flags |= HTTP_CLIENT_FLAG_SECURE;
        if (settings->ca_cert == NULL) {
            http_log("%s, no ca cert", __func__);
            goto exit;
        }
        http_sessions[index].https.ssl.ca_cert_c = settings->ca_cert;
#else
        http_log("%s, https not available", __func__);
        goto exit;
#endif
    } else if (http_str_insensitive_cmp(param.param, "http", param.len) == false) {
        http_log("%s, no http or https in server name", __func__);
        goto exit;
    }

    if (http_str_search(settings->server_name, "://", server_name_offset, 3, NULL) == false) {
        http_log("%s, server name format error", __func__);
        goto exit;
    }
    server_name_offset += 3;

    /* get host name */
    if (http_str_search(settings->server_name, "/",
                        server_name_offset, server_name_len - server_name_offset, &param) == false) {
        if ((server_name_len + 1) >= CONFIG_HTTPC_SERVER_NAME_SIZE) {
            http_log("%s, server name no buffer space for padding /", __func__);
            goto exit;
        }
        settings->server_name[server_name_len] = '/';
        if (http_str_search(settings->server_name, "/",
                            server_name_offset, server_name_len + 1 - server_name_offset, &param) == false) {
            http_log("%s, server name format error", __func__);
            goto exit;
        }
    }
    memset(http_sessions[index].server_name, 0, sizeof(http_sessions[index].server_name));
    strncpy(http_sessions[index].server_name, param.param, param.len + 1);

    if (http_str_search(http_sessions[index].server_name, ":", 0, param.len, &param) == true) {
        if ((strlen(http_sessions[index].server_name) - param.len) <= HTTP_SERVER_PORT_SIZE) {
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
            goto exit;
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

    /* default no cache */
    http_sessions[index].flags |= HTTP_CLIENT_FLAG_NO_CACHE;

    /* request buffer */
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

    httpc_wrapper_unlock_mutex();
    return (httpc_handle_t)&http_sessions[index];

exit:
    httpc_wrapper_unlock_mutex();
    return 0;
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
    httpc_wrapper_lock_mutex();
    memset(http_session, 0, sizeof(httpc_t));
    http_session->socket = -1;
    httpc_wrapper_unlock_mutex();
    return HTTP_SUCCESS;
}

int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data)
{
    uint32_t hdr_len;
    uint32_t hdr_data_len;
    int32_t hdr_length;

    if (buf == NULL || buf_size == 0 || name == NULL || data == NULL) {
        return HTTP_EARG;
    }

    hdr_len = strlen(name);
    hdr_data_len = strlen(data);
    hdr_length = hdr_len + hdr_data_len + 4;

    if (hdr_length < 0 || hdr_length > buf_size) {
        return HTTP_ENOBUFS;
    }

    memcpy(buf, name, hdr_len);
    buf += hdr_len;
    memcpy(buf, ": ", 2);
    buf += 2;
    memcpy(buf, data, hdr_data_len);
    buf += hdr_data_len;
    memcpy(buf, HTTPC_CRLF, 2);

    return hdr_length;
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
    hdr_ptr = (char *)(http_session->req.buf + http_session->req.data_len);

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

static int8_t httpc_add_uri(httpc_handle_t httpc, char *uri)
{
    httpc_t *http_session = (httpc_t *)httpc;
    char *hdr_ptr;
    int16_t free_space;
    int8_t ret;
    bool add_slash = false;

    if (uri == NULL) {
        return HTTP_SUCCESS;
    }

    if (http_session == NULL) {
        return HTTP_EARG;
    }

    if (http_str_search(uri, "/", 0, 1, NULL) == false) {
        add_slash = true;
    }

    free_space = http_session->req.buf_size - http_session->req.data_len;
    if (add_slash) {
        --free_space;
    }
    if (strlen(uri) > free_space) {
        return HTTP_ENOBUFS;
    }

    hdr_ptr = (char *)http_session->req.buf + http_session->req.data_len;
    if (add_slash) {
        *(hdr_ptr++) = '/';
        ++http_session->req.data_len;
    }
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

    hdr_ptr = (char *)(http_session->req.buf + http_session->req.data_len);
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
    memcpy(hdr_ptr, param, param_len);
    http_session->req.data_len += param_len;
    return HTTP_SUCCESS;
}

static int8_t httpclient_reset(httpc_t *http_session)
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

int8_t httpc_send_request(httpc_handle_t httpc, int8_t method, char *uri,
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

    httpclient_reset(http_session);
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

    http_log("%s, send request header\r\n%s\r\nsocket %d, len %d",
              __func__, http_session->req.buf, http_session->socket, http_session->req.data_len);

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
                                            http_session->req.data_len, 0);
#else
        socket_res = HTTP_ENOTSUPP;
#endif
    } else {
        socket_res = httpc_wrapper_send(http_session->socket, http_session->req.buf,
                                        http_session->req.data_len, 0);
    }

    if (socket_res > 0) {
        ret = HTTP_SUCCESS;
    } else {
        ret = (int8_t)socket_res;
    }

    http_log("%s, send %d, ret %d", __func__, http_session->req.data_len, ret);

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

    httpc_wrapper_init_mutex();

    return HTTP_SUCCESS;
}

static void httpclient_base64enc(char *out, const char *in)
{
    const char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=" ;
    int i = 0, x = 0, l = 0;

    for (; *in; in++) {
        x = x << 8 | *in;
        for (l += 8; l >= 6; l -= 6) {
            out[i++] = code[(x >> (l - 6)) & 0x3f];
        }
    }
    if (l > 0) {
        x <<= 6 - l;
        out[i++] = code[x & 0x3f];
    }
    for (; i % 4;) {
        out[i++] = '=';
    }
    out[i] = '\0' ;
}

static int httpclient_conn(httpclient_t *client, char *host)
{
    struct addrinfo hints, *addr_list, *cur;
    int ret = 0;
    char port[10] = {0};

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ( getaddrinfo( host, port , &hints, &addr_list ) != 0 ) {
        http_log("getaddrinfo != 0, return EDNS");
        return HTTP_EDNS;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = HTTP_EDNS;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        client->socket = (int) socket( cur->ai_family, cur->ai_socktype,
                                        cur->ai_protocol );
        if ( client->socket < 0 ) {
            ret = HTTP_ECONN;
            continue;
        }

        if ( connect( client->socket, cur->ai_addr, (int)cur->ai_addrlen ) == 0 ) {
            ret = 0;
            break;
        }

        close(client->socket);
        ret = HTTP_ECONN;
    }

    freeaddrinfo( addr_list );

    return ret;
}

static int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = NULL;
    size_t host_len = 0;
    size_t path_len;
    char *port_ptr;
    char *path_ptr;
    char *fragment_ptr;

    if (url == NULL) {
        http_log("Could not find url");
        return HTTP_EPARSE;
    }

    host_ptr = (char *) strstr(url, "://");

    if (host_ptr == NULL) {
        http_log("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }

    if ( max_scheme_len < host_ptr - scheme_ptr + 1 ) { /* including NULL-terminating char */
        http_log("Scheme str is too small (%d >= %d)", max_scheme_len, host_ptr - scheme_ptr + 1);
        return HTTP_EPARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            http_log("Could not find port");
            return HTTP_EPARSE;
        }
        *port = (int)tport;
    } else {
        *port = 0;
    }
    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        http_log("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTP_EPARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    fragment_ptr = strchr(host_ptr, '#');
    if (fragment_ptr != NULL) {
        path_len = fragment_ptr - path_ptr;
    } else {
        path_len = strlen(path_ptr);
    }

    if ( max_path_len < path_len + 1 ) { /* including NULL-terminating char */
        http_log("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return HTTP_EPARSE;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return HTTP_SUCCESS;
}

static int httpclient_parse_host(char *url, char *host, size_t maxhost_len)
{
    char *host_ptr = (char *) strstr(url, "://");
    size_t host_len = 0;
    char *port_ptr;
    char *path_ptr;

    if (host_ptr == NULL) {
        http_log("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }
    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            http_log("Could not find port");
            return HTTP_EPARSE;
        }
    }

    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        http_log("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTP_EPARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    return HTTP_SUCCESS;
}

static int httpclient_get_info(httpclient_t *client, char *send_buf, int *send_idx, char *buf, size_t len)   /* 0 on success, err code on failure */
{
    int ret ;
    int cp_len ;
    int idx = *send_idx;

    if (len == 0) {
        len = strlen(buf);
    }

    do {
        if ((HTTPCLIENT_SEND_BUF_SIZE - idx) >= len) {
            cp_len = len ;
        } else {
            cp_len = HTTPCLIENT_SEND_BUF_SIZE - idx ;
        }

        memcpy(send_buf + idx, buf, cp_len) ;
        idx += cp_len ;
        len -= cp_len ;

        if (idx == HTTPCLIENT_SEND_BUF_SIZE) {
            if (client->is_http == false) {
                http_log("send buffer overflow");
                return HTTP_EUNKOWN;
            }
            ret = httpclient_tcp_send_all(client->socket, send_buf, HTTPCLIENT_SEND_BUF_SIZE) ;
            if (ret) {
                return (ret) ;
            }
        }
    } while (len) ;

    *send_idx = idx;
    return HTTP_SUCCESS;
}

void httpc_set_custom_header(httpclient_t *client, char *header)
{
    client->header = header ;
}

static int httpclient_basic_auth(httpclient_t *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_AUTHB_SIZE) {
        return HTTP_EUNKOWN;
    }
    client->auth_user = user;
    client->auth_password = password;
    return HTTP_SUCCESS;
}

static int httpclient_send_auth(httpclient_t *client, char *send_buf, int *send_idx)
{
    char b_auth[(int)((HTTPCLIENT_AUTHB_SIZE + 3) * 4 / 3 + 3)] ;
    char base64buff[HTTPCLIENT_AUTHB_SIZE + 3] ;

    httpclient_get_info(client, send_buf, send_idx, "Authorization: Basic ", 0) ;
    sprintf(base64buff, "%s:%s", client->auth_user, client->auth_password) ;
    http_log("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff) ;
    b_auth[strlen(b_auth) + 2] = '\0' ;
    b_auth[strlen(b_auth) + 1] = '\n' ;
    b_auth[strlen(b_auth)] = '\r' ;
    http_log("b_auth:%s", b_auth) ;
    httpclient_get_info(client, send_buf, send_idx, b_auth, 0) ;
    return HTTP_SUCCESS;
}


static int httpclient_tcp_send_all(int sock_fd, char *data, int length)
{
    int written_len = 0;

    while (written_len < length) {
        int ret = send(sock_fd, data + written_len, length - written_len, 0);
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            http_log("Connection err ret=%d errno=%d\n", ret, errno);
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

static formdata_info_t* found_formdata_info(httpclient_data_t * client_data) {
    int i;

    for(i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if((formdata_info[i].client_data == client_data)
           && (formdata_info[i].is_used == 1)) {
            break;
        }
    }

    if(i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &formdata_info[i];
}

static formdata_info_t* found_empty_formdata_info() {
    int i;

    for(i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if(formdata_info[i].is_used == 0) {
            break;
        }
    }

    if(i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &formdata_info[i];
}

#define TEXT_FORMAT              "\r\nContent-Disposition: %s; name=\"%s\"\r\n\r\n%s\r\n"
#define TEXT_CONTENT_TYPE_FORMAT "\r\nContent-Disposition :%s; name=\"%s\"\r\nContent-Type:%s\r\n\r\n%s\r\n"
int httpc_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len)
{
    int buf_len;
    formdata_info_t* data_info;
    formdata_node_t* previous;
    formdata_node_t* current;

    if((content_disposition == NULL) || (name == NULL) || (data == NULL) || (data_len == 0)) {
        http_log("%s:%d invalid params", __func__, __LINE__);
        return -1;
    }

    if(strlen(data) > data_len) {
        http_log("%s:%d invalid data_len %d strlen data %d", __func__, __LINE__, data_len, strlen(data));
        return -1;
    }

    if((data_info = found_formdata_info(client_data)) == NULL) {
        if((data_info = found_empty_formdata_info()) == NULL) {
            http_log("%s:%d found no client_data info", __func__, __LINE__);
            return -1;
        }
    }

    if(data_info->is_used == 0) {
        data_info->is_used = 1;
        data_info->client_data = client_data;
        data_info->form_data = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(data_info->form_data == NULL) {
            data_info->is_used = 0;
            http_log("%s:%d form data malloc failed", __func__, __LINE__);
            return -1;
        }
        previous = data_info->form_data;
        current = data_info->form_data;
    }
    else {
        current = data_info->form_data;

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(current->next == NULL) {
            http_log("%s:%d form data malloc failed", __func__, __LINE__);
            return -1;
        }
        previous = current;
        current = current->next;
    }

    memset(current, 0, sizeof(formdata_node_t));
    if(content_type == NULL) {
        buf_len = strlen(TEXT_FORMAT) - 6 + strlen(content_disposition) + strlen(name) + strlen(data) + 1;
    }
    else {
        buf_len = strlen(TEXT_CONTENT_TYPE_FORMAT) - 8 + strlen(content_disposition) + strlen(name) + strlen(data) + strlen(content_type) + 1;
    }
    current->data = (char*)malloc(buf_len+1);
    if( current->data == NULL) {
        if(previous == current ) {
            free(current);
            memset(data_info, 0, sizeof(formdata_info_t));
        }
        else {
            free(current);
            previous->next = NULL;
        }
        http_log("%s:%d data malloc failed", __func__, __LINE__);
        return -1;
    }
    memset(current->data, 0, sizeof(buf_len));
    if(content_type == NULL) {
        snprintf(current->data, buf_len, TEXT_FORMAT, content_disposition, name, data);
        current->data_len = strlen(current->data);
    }
    else {
        snprintf(current->data, buf_len, TEXT_CONTENT_TYPE_FORMAT, content_disposition, name, content_type, data);
        current->data_len = strlen(current->data);
    }
    return 0;
}

static int get_url_file_name(char* url)
{
    char * ptr = url;
    int offset = 0;
    int i = 0;

    while(*ptr != '\0')
    {
        i++;
        if(*ptr == '/') {
            offset = i;
        }
        ptr++;
    }
    return offset;
}

#define FILE_FORMAT_START                "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"%s\"\r\n"
#define FILE_FORMAT_END                  "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"\"\r\n"
#define FILE_FORMAT_CONTENT_TYPE_START   "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"
int httpc_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path)
{
    int buf_len;
    formdata_info_t* data_info;
    formdata_node_t* previous;
    formdata_node_t* current;

    if((content_disposition == NULL) || (name == NULL) || (file_path == NULL)) {
        http_log("%s:%d invalid params", __func__, __LINE__);
        return -1;
    }

    if((data_info = found_formdata_info(client_data)) == NULL) {
        if((data_info = found_empty_formdata_info()) == NULL) {
            http_log("%s:%d found no client_data info", __func__, __LINE__);
            return -1;
        }
    }

    if(data_info->is_used == 0) {
        data_info->is_used = 1;
        data_info->client_data = client_data;
        data_info->form_data = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(data_info->form_data == NULL) {
            data_info->is_used = 0;
            http_log("%s:%d data malloc failed", __func__, __LINE__);
            return -1;
        }

        previous = data_info->form_data;
        current = data_info->form_data;
    }
    else {
        current = data_info->form_data;

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(current->next == NULL) {
            http_log("%s:%d data malloc failed", __func__, __LINE__);
            return -1;
        }
        previous = current;
        current = current->next;
    }

    memset(current, 0, sizeof(formdata_node_t));
    if(content_type == NULL) {
        buf_len = strlen(FILE_FORMAT_START) - 6 + strlen(content_disposition) + strlen(name) + strlen(file_path) - get_url_file_name(file_path) + 1;
    }
    else {
        buf_len = strlen(FILE_FORMAT_CONTENT_TYPE_START) - 8 + strlen(content_disposition) + strlen(name) + strlen(file_path) - get_url_file_name(file_path) + strlen(content_type) + 1;
    }
    current->data = (char*)malloc(buf_len + 1);
    if( current->data == NULL) {
        if(previous == current ) {
            free(current);
            data_info->form_data = NULL;
        }
        else {
            free(current);
            previous->next = NULL;
        }
        http_log("%s:%d data malloc failed", __func__, __LINE__);
        return -1;
    }
    memset(current->data, 0, sizeof(buf_len));

    current->is_file = 1;

    memcpy(current->file_path, file_path, strlen(file_path));
    if(content_type == NULL) {
        snprintf(current->data, buf_len, FILE_FORMAT_START, content_disposition, name, file_path + get_url_file_name(file_path));
    }
    else {
        snprintf(current->data, buf_len, FILE_FORMAT_CONTENT_TYPE_START, content_disposition, name, file_path + get_url_file_name(file_path), content_type);
    }
    current->data_len = strlen(current->data);
    return 0;
}

void form_data_clear(formdata_node_t* form_data) {
    if(form_data != NULL) {
        form_data_clear(form_data->next);
        form_data->next = NULL;
        if(form_data->data != NULL) {
            free(form_data->data);
        }
        free(form_data);
    }
}

static void httpclient_clear_form_data(httpclient_data_t * client_data)
{
    formdata_info_t * data_info;
    formdata_node_t * current;

    data_info = found_formdata_info(client_data);

    if(data_info == NULL) {
        http_log("No form data info found");
        return;
    }

    current = data_info->form_data;
    if(current != NULL) {
        form_data_clear(current);
        memset(current, 0, sizeof(formdata_node_t));
    }
    else {
        http_log("No form data in form data info");
    }

    memset(data_info, 0, sizeof(formdata_info_t));
}

static const char *boundary = "----WebKitFormBoundarypNjgoVtFRlzPquKE";
static int httpclient_send_header(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = {0};
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};
    int len;
    int total_len = 0;
    char send_buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char *meth = (method == HTTP_GET) ? "GET" : (method == HTTP_POST) ? "POST" : (method == HTTP_PUT) ? "PUT" : (method == HTTP_DELETE) ? "DELETE" : (method == HTTP_HEAD) ? "HEAD" : "";
    int ret, port;
    formdata_info_t* data_info;

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(port), path, sizeof(path));
    if (res != HTTP_SUCCESS) {
        http_log("httpclient_parse_url returned %d", res);
        return res;
    }

    /* Send request */
    memset(send_buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    len = 0 ; /* Reset send buffer */

    snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nUser-Agent: AliOS-HTTP-Client/2.1\r\nCache-Control: no-cache\r\nConnection: close\r\nHost: %s\r\n", meth, path, host); /* Write request */
    ret = httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    if (ret) {
        http_log("Could not write request");
        return HTTP_ECONN;
    }

    /* Send all headers */
    if (client->auth_user) {
        httpclient_send_auth(client, send_buf, &len) ; /* send out Basic Auth header */
    }

    /* Add user header information */
    if (client->header) {
        httpclient_get_info(client, send_buf, &len, (char *)client->header, strlen(client->header));
    }

    if(((data_info = found_formdata_info(client_data)) != NULL) && (data_info->is_used == 1)) {
        formdata_node_t * current;
        current = data_info->form_data;
        /* calculate content-length*/
        do {
            if(current->is_file == 1) {
                FILE* fd;
                long size;

                fd = fopen(current->file_path, "rb");
                if(fd == NULL) {
                    http_log("%s: open file(%s) failed errno=%d", __func__, current->file_path, errno);
                    return -1;
                }

                fseek(fd,0,SEEK_END);
                size=ftell(fd);
                fseek(fd,0,SEEK_SET);
                fclose(fd);
                total_len += size;
            }

            total_len += current->data_len;

            total_len += strlen(boundary) + 4;

            current = current->next;
        }
        while(current != NULL);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "Accept: */*\r\n");
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL)  {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
        else {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }

        total_len += strlen(boundary) + 8;
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", total_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

    }
    else if ( client_data->post_buf != NULL ) {
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", client_data->post_buf_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL)  {
            snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
    }
    else {
        http_log("Do nothing");
    }

    /* Close headers */
    httpclient_get_info(client, send_buf, &len, "\r\n", 0);

    http_log("Trying to write %d bytes http header:%s", len, send_buf);

#if CONFIG_HTTP_SECURE
    if (client->is_http == false) {
        http_log("Enter SSL_write");
        httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
        if (httpclient_ssl_send_all(&ssl->ssl_ctx, send_buf, len) != len) {
            http_log("SSL_write failed");
            return HTTP_EUNKOWN;
        }
        return HTTP_SUCCESS;
    }
#endif

    ret = httpclient_tcp_send_all(client->socket, send_buf, len);
    if (ret > 0) {
        http_log("Written %d bytes, socket = %d", ret, client->socket);
    } else if ( ret == 0 ) {
        http_log("ret == 0,Connection was closed by server");
        return HTTP_ECLSD; /* Connection was closed by server */
    } else {
        http_log("Connection error (send returned %d)", ret);
        return HTTP_ECONN;
    }

    return HTTP_SUCCESS;
}

static int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = 0;
    formdata_info_t* data_info;

    if (client_data->post_buf && client_data->post_buf_len) {
        http_log("client_data->post_buf:%s", client_data->post_buf);
#if CONFIG_HTTP_SECURE
        if (client->is_http == false) {
            httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
            if (httpclient_ssl_send_all(&ssl->ssl_ctx, client_data->post_buf, client_data->post_buf_len) != client_data->post_buf_len) {
                http_log("SSL_write failed");
                return HTTP_EUNKOWN;
            }
        } else
#endif
        {
            ret = httpclient_tcp_send_all(client->socket, client_data->post_buf, client_data->post_buf_len);
            if (ret > 0) {
                http_log("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                http_log("ret == 0,Connection was closed by server");
                return HTTP_ECLSD; /* Connection was closed by server */
            } else {
                http_log("Connection error (send returned %d)", ret);
                return HTTP_ECONN;
            }
        }
    }
    else if(((data_info = found_formdata_info(client_data)) != NULL) &&(data_info->is_used == 1)) {
        formdata_node_t* current = data_info->form_data;
        char data[HTTP_DATA_SIZE] = {0};
        FILE* fd;
        while(current != NULL)
        {
            /* set boundary */
            memset(data, 0, sizeof(data));
            snprintf(data, sizeof(data), "\r\n--%s", boundary);
            ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
            if (ret <= 0) {
                return -1;
            }
            ret = httpclient_tcp_send_all(client->socket, current->data, current->data_len);
            if (ret <= 0) {
                return -1;
            }
            if( current->is_file == 1 ) {
                break;
            }
            current = current->next;
        }

        if(current == NULL) {
            return -1;
        }

        fd = fopen(current->file_path, "rb");
        if(fd == NULL)
        {
            http_log("%s: open file(%s) failed errno=%d", __func__, current->file_path, errno);
            return -1;
        }

        while(!feof(fd)) {
            ret = fread(data, 1, sizeof(data), fd);
            if(ret <= 0) {
               http_log("fread failed returned %d errno=%d", ret, errno);
               return -1;
            }

            ret = httpclient_tcp_send_all(client->socket, data, ret);
            if (ret > 0) {
                http_log("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                http_log("ret == 0,Connection was closed by server");
                return HTTP_ECLSD; /* Connection was closed by server */
            } else {
                http_log("Connection error (send returned %d) errno=%d", ret, errno);
                return HTTP_ECONN;
            }

            memset(data, 0, sizeof(data));
            aos_msleep(100);
        }

        fclose(fd);

        current = current->next;
        while(current != NULL)
        {
            memset(data, 0, sizeof(data));
            snprintf(data, sizeof(data), "\r\n--%s", boundary);
            ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
            if (ret <= 0) {
                return -1;
            }

            ret = httpclient_tcp_send_all(client->socket, current->data, current->data_len);
            if (ret <= 0) {
                return -1;
            }
            current = current->next;
        }

        memset(data, 0, sizeof(data));
        snprintf(data, sizeof(data), "\r\n--%s--\r\n", boundary);
        ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
        if (ret <= 0) {
            return -1;
        }
    }

    return HTTP_SUCCESS;
}

#define MAX_RECV_WAIT_TIME_SEC 5
int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len)   /* 0 on success, err code on failure */
{
    int ret = 0, select_ret;
    size_t readLen = 0;
    struct timeval timeout;
    fd_set         sets;
    int err_record;

    timeout.tv_sec = MAX_RECV_WAIT_TIME_SEC;
    timeout.tv_usec = 0;

    while (readLen < max_len) {
        buf[readLen] = '\0';
        if (client->is_http) {
        #if 1
            if (readLen < min_len) {
                FD_ZERO(&sets);
                FD_SET(client->socket, &sets);

                http_log("before recv [blocking] return:%d", ret);
                select_ret = select(client->socket + 1, &sets, NULL, NULL, &timeout);
                err_record = errno;
                if (select_ret > 0) {
                    if (0 == FD_ISSET(client->socket, &sets)) {
                        ret = 0;
                        http_log("select continue");
                        continue;
                    }

                    ret = recv(client->socket, buf + readLen, min_len - readLen, 0);
                    err_record = errno;
                    if (ret <= 0 ) {
                        if ((EINTR == err_record) || (EAGAIN == err_record) || (EWOULDBLOCK == err_record) ||
                           (EPROTOTYPE == err_record) || (EALREADY == err_record) || (EINPROGRESS == err_record)) {
                            http_log("recv continue");
                            continue;
                        }

                        if (ret == 0) {
                            http_log("recv return 0 disconnected");
                            ret = HTTP_ECLSD;
                        }
                    }
                } else if (select_ret == 0) {
                    http_log("select return 0 may disconnected");
                    ret = HTTP_ECLSD;
                } else {
                    http_log("select return %d", select_ret);
                    if (err_record == EINTR)
                        continue;
                    ret = select_ret;
                }
                http_log("after recv [blocking] return:%d", ret);
            } else {
                ret = recv(client->socket, buf + readLen, max_len - readLen, MSG_DONTWAIT);
                err_record = errno;
                http_log("recv [not blocking] return:%d", ret);
                if ((ret == -1) && (err_record == EWOULDBLOCK)) {
                    http_log("recv [not blocking] EWOULDBLOCK");
                    break;
                }
            }
        #else
            ret = recv(client->socket, buf + readLen, max_len - readLen, 0);
        #endif
        }
#if CONFIG_HTTP_SECURE
        else {
            httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
        #if 1
            if (readLen < min_len) {
                http_log("before mbedtls_ssl_read [blocking]");
                mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
                mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, MAX_RECV_WAIT_TIME_SEC * 1000);
                ret = mbedtls_ssl_read(&ssl->ssl_ctx, (unsigned char *)buf + readLen, min_len - readLen);
                http_log("mbedtls_ssl_read [blocking] return:%d", ret);
                if (ret == 0) {
                    ret = HTTP_ECLSD;
                }
            } else {
                mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, httpclient_ssl_nonblock_recv, NULL);
                ret = mbedtls_ssl_read(&ssl->ssl_ctx, (unsigned char *)buf + readLen, max_len - readLen);
                err_record = errno;
                http_log("mbedtls_ssl_read [not blocking] return:%d", ret);
                if ((ret == -1) && (err_record == EWOULDBLOCK)) {
                    http_log("mbedtls_ssl_read [not blocking] EWOULDBLOCK");
                    break;
                }
            }
        #else
            mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);
            ret = mbedtls_ssl_read(&ssl->ssl_ctx, (unsigned char *)buf + readLen, max_len - readLen);
        #endif
            if (ret < 0) {
                http_log("mbedtls_ssl_read, return:%d", ret);
                if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                       || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                       || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                       || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                    /* read already complete */
                    /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
                }
            }
            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {/* read already complete(if call mbedtls_ssl_read again, it will return 0(eof)) */
                //break;
                ret = HTTP_ECLSD;
            }
        }
#endif

        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            break;
        } else if (ret == HTTP_ECLSD) {
            http_log("ret HTTP_ECLSD");
            break;
        } else {
            http_log("Connection error (recv returned %d readLen:%d)", ret,readLen);
            *p_read_len = readLen;
            return HTTP_ECONN;
        }
    }

    http_log("Read %d bytes", readLen);
    *p_read_len = readLen;
    buf[readLen] = '\0';

    if (ret == HTTP_ECLSD) {
        return ret;
    } else {
        return HTTP_SUCCESS;
    }
}

#define HTTPCLIENT_UNSET_RET -100
#define MAX_PRINT_PER_COUNT_LINE 20
int g_httpclient_line =  -1;
int g_httpclient_readlen =  -1;
int g_httpclient_len = -1;
int g_httpclient_ret = HTTPCLIENT_UNSET_RET;
int g_httpclient_print_count = -1;

static void reset_g_httpclient_var(void)
{
    g_httpclient_line =  -1;
    g_httpclient_readlen =  -1;
    g_httpclient_len = -1;
    g_httpclient_ret = HTTPCLIENT_UNSET_RET;
    g_httpclient_print_count = -1;   
}

int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
    /* Receive data */
    http_log("Receiving data:%s", data);
    client_data->is_more = true;
    int print_count = 0;

    if (client_data->response_content_len == -1 && client_data->is_chunked == false) {
        while(true)
        {
            int ret, max_len;
            if (count + len < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, len);
                count += len;
                client_data->response_buf[count] = '\0';
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->content_block_len = client_data->response_buf_len - 1;
                return HTTP_EAGAIN;
            }

            max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
            if (max_len <= 0) {
                http_log("%s %d error max_len %d", __func__, __LINE__, max_len);
                return HTTP_EUNKOWN;
            }
            ret = httpclient_recv(client, data, 1, max_len, &len);

            /* Receive data */
            http_log("data len: %d %d", len, count);

            if (ret == HTTP_ECONN) {
                http_log("ret == HTTP_ECONN");
                client_data->content_block_len = count;
                return ret;
            }

            if (len == 0) {/* read no more data */
                http_log("no more len == 0");
                client_data->is_more = false;
                return HTTP_SUCCESS;
            }
            http_log("in loop %s %d ret %d len %d count %d", __func__, __LINE__, ret, len, count);
        }
    }

    while (true) {
        size_t readLen = 0;

        if ( client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            bool foundCrlf;
            int n;
            do {
                int ret = HTTPCLIENT_UNSET_RET;
                http_log("len: %d", len);
                foundCrlf = false;
                crlf_pos = 0;
                data[len] = 0;
                if (len >= 2) {
                    for (; crlf_pos < len - 2; crlf_pos++) {
                        if ( data[crlf_pos] == '\r' && data[crlf_pos + 1] == '\n' ) {
                            foundCrlf = true;
                            break;
                        }
                    }
                }
                if (!foundCrlf) { /* Try to read more */
                    if ( len < HTTPCLIENT_CHUNK_SIZE ) {
                        int new_trf_len;
                        int max_recv = MIN(client_data->response_buf_len, HTTPCLIENT_CHUNK_SIZE);
                        if (max_recv - len - 1 <= 0) {
                            http_log("%s %d error max_len %d", __func__, __LINE__, max_recv - len - 1);
                            return HTTP_EUNKOWN;
                        }
                        ret = httpclient_recv(client, data + len, 0,  max_recv - len - 1 , &new_trf_len);
                        len += new_trf_len;
                        if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && new_trf_len == 0)) {
                            return ret;
                        } else {
                            http_log("in loop %s %d ret %d len %d", __func__, __LINE__, ret, len);
                            continue;
                        }
                    } else {
                        return HTTP_EUNKOWN;
                    }
                }
                http_log("in loop %s %d len %d ret %d", __func__, __LINE__, len, ret);
            } while (!foundCrlf);
            data[crlf_pos] = '\0';
            n = sscanf(data, "%x", &readLen);/* chunk length */
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (n != 1) {
                http_log("Could not read chunk length");
                return HTTP_EPROTO;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);

            if ( readLen == 0 ) {
               /* Last chunk */
                client_data->is_more = false;
                http_log("no more (last chunk)");
                break;
            }
        } else {
            readLen = client_data->retrieve_len;
        }

        http_log("Retrieving %d bytes, len:%d", readLen, len);
        reset_g_httpclient_var();

        do {
            int ret = HTTPCLIENT_UNSET_RET;
            http_log("readLen %d, len:%d", readLen, len);
            templen = MIN(len, readLen);
            if (count + templen < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, templen);
                count += templen;
                client_data->response_buf[count] = '\0';
                client_data->retrieve_len -= templen;
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->retrieve_len -= (client_data->response_buf_len - 1 - count);
                client_data->content_block_len = client_data->response_buf_len - 1;
                return HTTP_EAGAIN;
            }

            if ( len >= readLen ) {
                http_log("memmove %d %d %d", readLen, len, client_data->retrieve_len);
                memmove(data, &data[readLen], len - readLen); /* chunk case, read between two chunks */
                len -= readLen;
                readLen = 0;
                client_data->retrieve_len = 0;
            } else {
                readLen -= len;
            }

            if (readLen) {
                int max_len = MIN(MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count), readLen);
                if (max_len <= 0) {
                    http_log("%s %d error max_len %d", __func__, __LINE__, max_len);
                    return HTTP_EUNKOWN;
                }

                ret = httpclient_recv(client, data, 1, max_len, &len);
                if (ret == HTTP_ECONN || (ret == HTTP_ECLSD && len == 0)) {
                    return ret;
                }
            }

            if (print_count % MAX_PRINT_PER_COUNT_LINE == 0)
                http_log("in loop %s %d readLen %d len %d ret %d print_count %d", __func__, __LINE__, readLen, len, ret, print_count);

            g_httpclient_line =  __LINE__;
            g_httpclient_readlen =  readLen;
            g_httpclient_len = len;
            g_httpclient_ret = ret;
            g_httpclient_print_count = print_count;

            print_count++;
        } while (readLen);

        if ( client_data->is_chunked ) {
            if (len < 2) {
                int new_trf_len = 0, ret;
                int max_recv = MIN(client_data->response_buf_len - 1 - count + 2, HTTPCLIENT_CHUNK_SIZE - len - 1);
                if (max_recv <= 0) {
                    http_log("%s %d error max_len %d", __func__, __LINE__, max_recv);
                    return HTTP_EUNKOWN;
                }

                /* Read missing chars to find end of chunk */
                ret = httpclient_recv(client, data + len, 2 - len, max_recv, &new_trf_len);
                if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && new_trf_len == 0)) {
                    return ret;
                }
                len += new_trf_len;
            }
            if ( (data[0] != '\r') || (data[1] != '\n') ) {
                http_log("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return HTTP_EPROTO;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            http_log("no more(content-length)");
            client_data->is_more = false;
            break;
        }

    }
    client_data->content_block_len = count;

    return HTTP_SUCCESS;
}

int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int crlf_pos;
    int header_buf_len = client_data->header_buf_len;
    char *header_buf = client_data->header_buf;
    int read_result;
    int print_count = 0;

    // reset the header buffer
    if (header_buf) {
        memset(header_buf, 0, header_buf_len);
    }

    client_data->response_content_len = -1;

    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        http_log("\r\n not found");
        return HTTP_EPROTO;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
    if ( sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1 ) {
        /* Cannot match string, error */
        http_log("Not a correct HTTP answer : %s", data);
        return HTTP_EPROTO;
    }

    if ( (client->response_code < 200) || (client->response_code >= 400) ) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        http_log("Response code %d", client->response_code);

        if (client->response_code == 416) {
            http_log("Requested Range Not Satisfiable");
            return HTTP_EUNKOWN;
        }
    }

    http_log("Reading headers%s", data);

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);

    client_data->is_chunked = false;

    reset_g_httpclient_var();
    /* Now get headers */
    while ( true ) {
        char *colon_ptr, *key_ptr, *value_ptr;
        int key_len, value_len;
        read_result = HTTPCLIENT_UNSET_RET;

        crlf_ptr = strstr(data, "\r\n");
        if (crlf_ptr == NULL) {
            if ( len < HTTPCLIENT_CHUNK_SIZE - 1 ) {
                int new_trf_len = 0;
                if (HTTPCLIENT_CHUNK_SIZE - len - 1 <= 0) {
                    http_log("%s %d error max_len %d", __func__, __LINE__, HTTPCLIENT_CHUNK_SIZE - len - 1);
                    return HTTP_EUNKOWN;
                }
                read_result = httpclient_recv(client, data + len, 1, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                len += new_trf_len;
                data[len] = '\0';
                http_log("Read %d chars; In buf: [%s]", new_trf_len, data);
                if ((read_result == HTTP_ECONN) || (read_result == HTTP_ECLSD && new_trf_len == 0)) {
                    return read_result;
                } else {
                    http_log("in loop %s %d ret %d len %d", __func__, __LINE__, read_result, len);
                    continue;
                }
            } else {
                http_log("header len > chunksize");
                return HTTP_EUNKOWN;
            }
        }

        crlf_pos = crlf_ptr - data;
        if (crlf_pos == 0) { /* End of headers */
            memmove(data, &data[2], len - 2 + 1); /* Be sure to move NULL-terminating char as well */
            len -= 2;
            break;
        }

        colon_ptr = strstr(data, ": ");
        if (colon_ptr) {
            if (header_buf_len >= crlf_pos + 2 && header_buf) {
                /* copy response header to caller buffer */
                memcpy(header_buf, data, crlf_pos + 2);
                header_buf += crlf_pos + 2;
                header_buf_len -= crlf_pos + 2;
            }

            key_len = colon_ptr - data;
            value_len = crlf_ptr - colon_ptr - strlen(": ");
            key_ptr = data;
            value_ptr = colon_ptr + strlen(": ");

            http_log("Read header : %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
            if (0 == strncasecmp(key_ptr, "Content-Length", key_len)) {
                sscanf(value_ptr, "%d[^\r]", &(client_data->response_content_len));
                client_data->retrieve_len = client_data->response_content_len;
            } else if (0 == strncasecmp(key_ptr, "Transfer-Encoding", key_len)) {
                if (0 == strncasecmp(value_ptr, "Chunked", value_len)) {
                    client_data->is_chunked = true;
                    client_data->response_content_len = 0;
                    client_data->retrieve_len = 0;
                }
            } else if ((client->response_code >= 300 && client->response_code < 400) && (0 == strncasecmp(key_ptr, "Location", key_len))) {

                if ( HTTPCLIENT_MAX_URL_LEN < value_len + 1 ) {
                    http_log("url is too large (%d >= %d)", value_len + 1, HTTPCLIENT_MAX_URL_LEN);
                    return HTTP_EUNKOWN;
                }

                if(client_data->redirect_url == NULL) {
                    client_data->redirect_url = (char* )malloc(HTTPCLIENT_MAX_URL_LEN);
                }

                memset(client_data->redirect_url, 0, HTTPCLIENT_MAX_URL_LEN);
                memcpy(client_data->redirect_url, value_ptr, value_len);
                client_data->is_redirected = 1;
           }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
            len -= (crlf_pos + 2);
        } else {
            http_log("Could not parse header");
            return HTTP_EUNKOWN;
        }

        if (print_count % MAX_PRINT_PER_COUNT_LINE == 0){
            http_log("in loop %s %d len %d ret %d", __func__, __LINE__, len, read_result);
        }

        g_httpclient_line =  __LINE__;
        g_httpclient_readlen =  -1;
        g_httpclient_len = len;
        g_httpclient_ret = read_result;
        g_httpclient_print_count = print_count;

        print_count++;
    }

    return httpclient_retrieve_content(client, data, len, client_data);
}


static HTTPC_RESULT httpclient_connect(httpclient_t *client, char *url)
{
    int ret = HTTP_ECONN;
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char scheme[8] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(client->remote_port), path, sizeof(path));
    if (res != HTTP_SUCCESS) {
        http_log("httpclient_parse_url returned %d", res);
        return (HTTPC_RESULT)res;
    }

    // http or https
    if (strcmp(scheme, "https") == 0) {
        client->is_http = false;
    }
    else if (strcmp(scheme, "http") == 0)
    {
        client->is_http = true;
    }

    // default http 80 port, https 443 port
    if (client->remote_port == 0) {
        if (client->is_http) {
            client->remote_port = HTTP_PORT;
        } else {
            client->remote_port = HTTPS_PORT;
        }
    }

    http_log("http?:%d, port:%d, host:%s", client->is_http, client->remote_port, host);

    client->socket = -1;
    if (client->is_http)
        ret = httpclient_conn(client, host);
#if CONFIG_HTTP_SECURE
    else {
        ret = httpclient_ssl_conn(client, host);
        if (0 == ret) {
            httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
            client->socket = ssl->net_ctx.fd;
        }
    }
#endif

    http_log("httpclient_connect() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

static HTTPC_RESULT httpclient_send_request(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    int ret = HTTP_ECONN;

    if (client->socket < 0) {
        return (HTTPC_RESULT)ret;
    }

    ret = httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        return (HTTPC_RESULT)ret;
    }

    if (method == HTTP_POST || method == HTTP_PUT) {
        ret = httpclient_send_userdata(client, client_data);
    }

    http_log("httpclient_send_request() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

static HTTPC_RESULT httpclient_recv_response(httpclient_t *client, httpclient_data_t *client_data)
{
    int reclen = 0;
    int ret = HTTP_ECONN;
    // TODO: header format:  name + value must not bigger than HTTPCLIENT_CHUNK_SIZE.
    char buf[HTTPCLIENT_CHUNK_SIZE] = {0}; // char buf[HTTPCLIENT_CHUNK_SIZE*2] = {0};

    if (client->socket < 0) {
        http_log("Invalid socket fd %d!", client->socket);
        return (HTTPC_RESULT)ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = httpclient_retrieve_content(client, buf, reclen, client_data);
    } else {
        ret = httpclient_recv(client, buf, 1, HTTPCLIENT_CHUNK_SIZE - 1, &reclen);
        if (ret != HTTP_SUCCESS && ret != HTTP_ECLSD) {
            return (HTTPC_RESULT)ret;
        }

        buf[reclen] = '\0';

        if (reclen) {
            http_log("reclen:%d, buf:%s", reclen, buf);
            ret = httpclient_response_parse(client, buf, reclen, client_data);
        }
    }

    http_log("httpclient_recv_response() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

static void httpclient_close(httpclient_t *client)
{
    if (client->is_http) {
        if (client->socket >= 0)
            close(client->socket);
    }
#if CONFIG_HTTP_SECURE
    else
        httpclient_ssl_close(client);
#endif

    client->socket = -1;
    http_log("httpclient_close() client:%p", client);
}

int httpclient_get_response_code(httpclient_t *client)
{
    return client->response_code;
}

static HTTPC_RESULT httpclient_common(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    HTTPC_RESULT ret = HTTP_ECONN;

    /* reset httpclient redirect flag */
    client_data->is_redirected = 0;

    ret = httpclient_connect(client, url);

    if (!ret) {
        ret = httpclient_send_request(client, url, method, client_data);

        if (!ret) {
            ret = httpclient_recv_response(client, client_data);
        }
    }
    /* Don't reset form data when got a redirected response */
    if(client_data->is_redirected == 0) {
        httpclient_clear_form_data(client_data);
    }

    httpclient_close(client);

    return ret;
}

HTTPC_RESULT httpc_get(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_GET, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_GET, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
	}

    return ret;
}

HTTPC_RESULT httpc_post(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_POST, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_POST, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

    return ret;
}

HTTPC_RESULT httpc_put(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, HTTP_PUT, client_data);
}

HTTPC_RESULT httpc_delete(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, HTTP_DELETE, client_data);
}

int httpclient_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len)
{
    char *data = header_buf;
    char *crlf_ptr, *colon_ptr, *key_ptr, *value_ptr;
    int key_len, value_len;

    if (header_buf == NULL || name == NULL || val_pos == NULL  || val_len == NULL )
        return -1;

    while (true) {
        crlf_ptr = strstr(data, "\r\n");
        colon_ptr = strstr(data, ": ");
        if (crlf_ptr && colon_ptr) {
            key_len = colon_ptr - data;
            value_len = crlf_ptr - colon_ptr - strlen(": ");
            key_ptr = data;
            value_ptr = colon_ptr + strlen(": ");

            http_log("Response header: %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
            if (0 == strncasecmp(key_ptr, name, key_len)) {
                *val_pos = value_ptr - header_buf;
                *val_len = value_len;
                return 0;
            } else {
                data = crlf_ptr + 2;
                continue;
            }
        } else
            return -1;

    }
}

HTTPC_RESULT httpc_prepare(httpclient_source_t *source, int header_size, int body_size)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    source->client_data.header_buf   = (char *) malloc (header_size);
    source->client_data.response_buf = (char *) malloc (body_size);

    if (source->client_data.header_buf == NULL || source->client_data.response_buf == NULL){
        http_log("httpc_prepare alloc memory failed");
        if(source->client_data.header_buf){
            free(source->client_data.header_buf);
            source->client_data.header_buf = NULL;
        }

        if(source->client_data.response_buf){
            free(source->client_data.response_buf);
            source->client_data.response_buf = NULL;
        }
        ret = HTTP_EUNKOWN;
        goto finish;
    }

    http_log("httpc_prepare alloc memory");

    source->client_data.header_buf_len = header_size;
    source->client_data.response_buf_len = body_size;
    source->client_data.post_buf_len = 0;

    source->client_data.is_redirected = 0;
    source->client_data.redirect_url = NULL;

finish:
    return ret;
}

void httpc_reset(httpclient_source_t *source)
{
    char *response_buf = source->client_data.response_buf;
    char *header_buf = source->client_data.header_buf;
    int response_buf_len = source->client_data.response_buf_len;
    int header_buf_len = source->client_data.header_buf_len;
    http_log("httpclient_reset");

    memset(&source->client, 0, sizeof (source->client));
    memset(&source->client_data, 0, sizeof (source->client_data));

    source->client_data.response_buf = response_buf;
    source->client_data.header_buf = header_buf;
    source->client_data.response_buf_len = response_buf_len;
    source->client_data.header_buf_len = header_buf_len;
}

HTTPC_RESULT httpc_unprepare(httpclient_source_t* source)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    http_log("httpclient_unprepare relase memory");

    if (source == NULL){
        ret = HTTP_EUNKOWN;
        goto finish;
    }

    if(source->client_data.header_buf){
        free(source->client_data.header_buf);
        source->client_data.header_buf = NULL;
    }

    if(source->client_data.response_buf){
        free(source->client_data.response_buf);
        source->client_data.response_buf = NULL;
    }

    source->client_data.header_buf_len = 0;
    source->client_data.response_buf_len = 0;

    source->client_data.is_redirected = 0;
    source->client_data.redirect_url = NULL;

finish:
    return ret;
}

#if CONFIG_HTTP_SECURE
static int httpclient_ssl_nonblock_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) recv( fd, buf, len, MSG_DONTWAIT );

    if ( ret < 0 ) {
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if ( WSAGetLastError() == WSAECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }
#else
        if ( errno == EPIPE || errno == ECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }

        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }
#endif
        if (ret == -1 && errno == EWOULDBLOCK) {
            return ret;
        }
        return ( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return ( ret );
}

static void httpclient_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    http_log("%s", str);
}

static int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length)
{
    size_t written_len = 0;

    while (written_len < length) {
        int ret = mbedtls_ssl_write(ssl, (unsigned char *)(data + written_len), (length - written_len));
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

static int httpclient_random(void *prng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (uint8_t)rand();
        rngoffset++;
        rnglen--;
    }

    return 0;
}

static int httpclient_ssl_conn(httpclient_t *client, char *host)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
#ifdef MBEDTLS_ENTROPY_C
    const char *pers = "https";
#endif
    int value, ret = 0;
    uint32_t flags;
    char port[10] = {0};
    httpclient_ssl_t *ssl;

    client->ssl = (httpclient_ssl_t *)malloc(sizeof(httpclient_ssl_t));
    if (!client->ssl) {
        http_log("Memory malloc error.");
        ret = -1;
        goto exit;
    }
    ssl = (httpclient_ssl_t *)client->ssl;

    if (client->server_cert)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;

    /*
     * Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
#endif
    mbedtls_pk_init(&ssl->pkey);
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);
    mbedtls_entropy_init(&ssl->entropy);
    if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
                               mbedtls_entropy_func,
                               &ssl->entropy,
                               (const unsigned char*)pers,
                               strlen(pers))) != 0) {
        http_log("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif
    /*
    * Load the Client certificate
    */
    if (client->client_cert && client->client_pk) {
#ifdef MBEDTLS_X509_CRT_PARSE_C
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)client->client_cert, client->client_cert_len);
        if (ret < 0) {
            http_log("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            goto exit;
        }
#endif
        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)client->client_pk, client->client_pk_len, NULL, 0);
        if (ret != 0) {
            http_log("failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            goto exit;
        }
    }

    /*
    * Load the trusted CA
    */
    /* cert_len passed in is gotten from sizeof not strlen */
#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (client->server_cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
                                        (const unsigned char *)client->server_cert,
                                        client->server_cert_len)) < 0)) {
        http_log("mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif

    /*
     * Start the connection
     */
    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        http_log("failed! mbedtls_net_connect returned %d, port:%s.", ret, port);
        goto exit;
    }

    /*
     * Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        http_log("mbedtls_ssl_config_defaults() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    // TODO: add customerization encryption algorithm
#ifdef MBEDTLS_X509_CRT_PARSE_C
    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (client->client_cert && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        http_log(" failed! mbedtls_ssl_conf_own_cert returned %d.", ret );
        goto exit;
    }
#endif

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, httpclient_random, &ssl->ctr_drbg);
    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, httpclient_debug, NULL);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        http_log("mbedtls_ssl_setup() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
    * Handshake
    */
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            http_log("mbedtls_ssl_handshake() failed, ret:-0x%x.", -ret);
            ret = -1;
            goto exit;
        }
    }

    /*
     * Verify the server certificate
     */
    /* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
        * handshake would not succeed if the peer's cert is bad.  Even if we used
        * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
        char vrfy_buf[512];
        http_log("svr_cert varification failed.");
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        http_log("%s", vrfy_buf);
    }
    else {
        http_log("svr_cert varification ok.");
    }

exit:
    http_log("ret=%d.", ret);
    return ret;
}

static int httpclient_ssl_close(httpclient_t *client)
{
    httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
    client->client_cert = NULL;
    client->server_cert = NULL;
    client->client_pk = NULL;

    if (!ssl)
        return -1;

    mbedtls_ssl_close_notify(&ssl->ssl_ctx);
    mbedtls_net_free(&ssl->net_ctx);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
#endif
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->ssl_ctx);
    mbedtls_ssl_config_free(&ssl->ssl_conf);
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);
#endif
    free(ssl);
    return 0;
}

#endif

