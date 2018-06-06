//
//  be_debuger.h
//  app
//
//  Created by howie on 2018/4/19.
//  Copyright © 2018 alibaba. All rights reserved.
//

#ifndef be_debuger_h
#define be_debuger_h

#include <utils/be_list.h>
#include <http-parser/http_parser.h>

#define BE_DEBUGER_SSDP_ST             "ST: urn:aos-be-debugers"
#define BE_DEBUGER_SSDP_PORT           1900
#define BE_DEBUGER_HTTP_PORT           1901
#define BE_DEBUGER_HTTP_HEADER_FIELD   128
#define BE_DEBUGER_HTTP_RECV_BUFFER    1024
#define BE_DEBUGER_UAER_RECV_BUFFER    1024

typedef struct be_debug_http_header_s  be_debug_http_header_t;
typedef struct be_debug_http_req_s     be_debug_http_req_t;
typedef struct be_debug_http_resp_s    be_debug_http_resp_t;
typedef struct be_debug_http_client_s  be_debug_http_client_t;

struct be_debug_http_header_s {
    char                 field[BE_DEBUGER_HTTP_HEADER_FIELD];
    char                 value[BE_DEBUGER_HTTP_HEADER_FIELD];
    struct list_head     list;
};

struct be_debug_http_req_s {
    http_parser          parser;
    struct list_head     headers;
    char                 url[BE_DEBUGER_HTTP_HEADER_FIELD];
    char                 *method;
    size_t               content_nread;
    size_t               content_length;
};

struct be_debug_http_resp_s {
    http_parser          parser;
    struct list_head     headers;
};

struct be_debug_http_client_s {
    void                 *arg;
    int                  sock;
    struct list_head     list;
    be_debug_http_req_t  req;
    be_debug_http_resp_t resp;
};

typedef struct be_debug_http_s           be_debug_http_t;
typedef void (*on_debuger_recv_begin)    (size_t content_length);
typedef void (*on_debuger_recv_update)   (unsigned char* data, size_t len);
typedef void (*on_debuger_recv_complete) (unsigned char* data, size_t len);

struct be_debug_http_s
{
    int  sock;
};

int be_debuger_init(on_debuger_recv_begin on_begin,
                    on_debuger_recv_update on_update,
                    on_debuger_recv_complete on_complete);

int be_debuger_ssdp_start(void);

int be_debuger_http_start(void);

int be_debuger_uart_start(void);

#endif /* be_debuger_h */
