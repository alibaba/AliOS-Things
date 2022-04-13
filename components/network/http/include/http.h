/**
 * @file http.h
 * http API header file.
 *
 * @version   V1.0
 * @date      2019-12-24
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _HTTP__H
#define _HTTP__H

#include <stdbool.h>
#include <stdint.h>
#include "network.h"

/** @defgroup aos_http_api http
  * @{
  */
/** @brief   http client connection settings */
typedef struct httpc_connection_s
{
    int socket;         /**< one socket per http session,
                               not support multiple http
                               sessions on one socket            */
    bool keep_alive;    /**< keep alive http connection or not */
    /*boot use_proxy;*/ /**< not support in this version       */
    char *server_name;  /**< pointer to server name            */
#if CONFIG_HTTP_SECURE
    const char *ca_cert; /**< pointer to ca certificate         */
#endif
    int port; /**< http server port                  */

    uint8_t *req_buf;     /**< pointer to request buffer         */
    int32_t req_buf_size; /**< request buffer size               */

#if CONFIG_HTTP_SECURE
    uint8_t flags; /**< flags to http client              */
#endif
} httpc_connection_t;

/** @brief   http error code */
typedef enum
{
    HTTP_EAGAIN = 1,     /**< more data to retrieved */
    HTTP_SUCCESS = 0,    /**< operation success      */
    HTTP_ENOBUFS = -1,   /**< buffer error           */
    HTTP_EARG = -2,      /**< illegal argument       */
    HTTP_ENOTSUPP = -3,  /**< not support            */
    HTTP_EDNS = -4,      /**< DNS fail               */
    HTTP_ECONN = -5,     /**< connect fail           */
    HTTP_ESEND = -6,     /**< send packet fail       */
    HTTP_ECLSD = -7,     /**< connect closed         */
    HTTP_ERECV = -8,     /**< recv packet fail       */
    HTTP_EPARSE = -9,    /**< url parse error        */
    HTTP_EPROTO = -10,   /**< protocol error         */
    HTTP_EUNKOWN = -11,  /**< unknown error          */
    HTTP_ETIMEOUT = -12, /**< timeout                */
} HTTPC_RESULT;

/** @brief   This structure defines the http_rsp_info_t structure   */
typedef struct http_rsp_info_s
{
    uint32_t rsp_len;                /**< response length            */
    uint8_t *body_start;             /**< body start position        */
    uint8_t content_len_present : 1; /**< content length length flag */
    uint8_t body_present : 1;        /**< body present flag          */
    uint8_t message_complete : 1;    /**< message complete flag      */
    uint8_t headers_complete : 1;    /**< headers complete flag      */
} http_rsp_info_t;

typedef uint32_t httpc_handle_t;

#define HTTP_ALWAYS_HTTP_FLAG 0x80

/** @} */
#endif /* HTTP_API_H */
