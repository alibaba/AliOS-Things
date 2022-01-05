
#ifndef _HAAS_HTTP_CLIENT_H
#define _HAAS_HTTP_CLIENT_H

#include <stdint.h>

#include "py/obj.h"

#define MAX_HTTP_RECV_BUFFER     512
#define MAX_HTTP_RESP_BUFFER     2048
#define HTTP_HEADER_SIZE         1024
#define HTTP_HEADER_COUNT        16
#define HTTP_DEFAULT_HEADER_NAME "content-type"
#define HTTP_DEFAULT_HEADER_DATA "application/json"
#define HTTP_CRLF                "\r\n"

#define ADDON_TSK_PRIORRITY      (20)

typedef struct {
    const char *name;
    const char *data;
} http_header_t;

typedef struct {
    const char *url;
    const char *filepath;
    int32_t method;
    uint32_t timeout;
    const char *params;
    mp_obj_t cb;
    http_header_t http_header[HTTP_HEADER_COUNT];
    int32_t header_size;
} hapy_http_param_t;

typedef enum {
    HTTP_DL_STATUS_OK = 0,
    HTTP_DL_STATUS_FAIL = -1,
    HTTP_DL_STATUS_PATH_NULL = -2,
} http_dl_status;

#endif