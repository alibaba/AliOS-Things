/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <http.h>
#include <http_parser.h>
#include "http_string.h"

#if CONFIG_HTTP_SECURE
#include "mbedtls/x509_crt.h"
#include "mbedtls/ssl.h"
#endif

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#define HTTP_CLIENT_FLAG_KEEP_ALIVE 0x00000001
#define HTTP_CLIENT_FLAG_NO_CACHE 0x00000004

#define HTTP_CLIENT_FLAG_SECURE 0x00000010
#define HTTP_CLIENT_FLAG_PORT 0x00000020

#define INT32T_MAX 0x7fffffff
#define INT16T_MAX 0x7fff
#define INT32T_MIN (-INT32T_MAX - 1)
#define INT16T_MIN (-INT16T_MAX - 1)

#define HTTP_CONTENT_LEN_SIZE 6

/* http client connection handle */
typedef struct httpc_s {
    int socket;
    uint8_t index;
    uint32_t flags;
    char server_name[CONFIG_HTTPC_SERVER_NAME_SIZE + 2];
    uint16_t rx_state;
    uint16_t port;
    bool connection;

    struct {
        int method;
        uint8_t *buf;
        uint32_t buf_size;
        uint32_t data_len;
    } req;

    struct {
        uint8_t *body_start;
        int32_t processed;

        int32_t content_len;
        char http_status[CONFIG_HTTP_STATUS_SIZE];

        uint8_t content_len_present:1;
        uint8_t body_present:1;
        uint8_t message_complete:1;
        uint8_t headers_complete:1;
    } rsp;

    struct http_parser parser;
    struct http_parser_settings parser_settings;
#if CONFIG_HTTP_SECURE
    struct {
        bool is_inited;
        struct {
            mbedtls_ssl_context context;
            mbedtls_ssl_config conf;
            mbedtls_x509_crt ca_cert;
            const char *ca_cert_c;
        } ssl;
    } https;
#endif
} httpc_t;

/** @brief   This structure defines the httpclient and HTTP data structure.  */
#define HTTPC_VERSION "HTTP/1.1"

#define HTTPC_CRLF "\r\n"

typedef struct httpc_param_s {
    char *param;
    uint16_t len;
} httpc_param_t;


void http_log(const char *fmt, ...);

#endif
