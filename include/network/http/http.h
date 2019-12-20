/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HTTP_API_H
#define HTTP_API_H

#include <stdbool.h>
#include <stdint.h>
#include "http_def_config.h"
#include "http_parser.h"
#include "network/network.h"
#if CONFIG_HTTP_SECURE
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#endif

/** @defgroup aos_http_api http
  * @{
  */
/** @brief   http client connection settings */
typedef struct httpc_connection_s {
    int socket;           /* one socket per http session, 
                             not support multiple http 
                             sessions on one socket            */
    bool keep_alive;      /* keep alive http connection or not */
    /*boot use_proxy;*/   /* not support in this version       */
    char *server_name;    /* pointer to server name            */
#if CONFIG_HTTP_SECURE
    const char *ca_cert;  /* pointer to ca certificate         */
#endif
    int port;

    uint8_t *req_buf;     /* pointer to request buffer         */
    int32_t  req_buf_size;/* request buffer size               */

#if CONFIG_HTTP_SECURE
    uint8_t flags;        /* flags to http client              */
#endif
} httpc_connection_t;

/** @brief   This structure defines the httpclient_t structure   */
typedef struct {
    int socket;                     /* socket ID                 */
    int remote_port;                /* hTTP or HTTPS port        */
    int response_code;              /* response code             */
    char *header;                   /* request custom header     */
    char *auth_user;                /* username for basic authentication         */
    char *auth_password;            /* password for basic authentication         */
    bool is_http;                   /* http connection? if 1, http; if 0, https  */
#if CONFIG_HTTP_SECURE
    const char *server_cert;        /* server certification      */
    const char *client_cert;        /* client certification      */
    const char *client_pk;          /* client private key        */
    int server_cert_len;            /* server certification lenght, server_cert buffer size  */
    int client_cert_len;            /* client certification lenght, client_cert buffer size  */
    int client_pk_len;              /* client private key lenght, client_pk buffer size      */
    void *ssl;                      /* ssl content               */
#endif
} httpclient_t;

/** @brief   This structure defines the http_rsp_info_t structure   */
typedef struct http_rsp_info_s {
    uint32_t  rsp_len;
    uint8_t   *body_start;
    uint8_t   content_len_present:1;
    uint8_t   body_present:1;
    uint8_t   message_complete:1;
    uint8_t   headers_complete:1;
} http_rsp_info_t;

/** @brief   This structure defines the HTTP data structure.  */
typedef struct {
    bool is_more;                /* indicates if more data needs to be retrieved. */
    bool is_chunked;             /* response data is encoded in portions/chunks.*/
    int retrieve_len;            /* content length to be retrieved. */
    int response_content_len;    /* response content length. */
    int content_block_len;       /* the content length of one block. */
    int post_buf_len;            /* post data length. */
    int response_buf_len;        /* response body buffer length. */
    int header_buf_len;          /* response head buffer lehgth. */
    char *post_content_type;     /* content type of the post data. */
    char *post_buf;              /* user data to be posted. */
    char *response_buf;          /* buffer to store the response body data. */
    char *header_buf;            /* buffer to store the response head data. */
    bool  is_redirected;         /* redirected URL? if 1, has redirect url; if 0, no redirect url */
    char* redirect_url;          /* redirect url when got http 3** response code. */
} httpclient_data_t;

#if CONFIG_HTTP_SECURE
/** @brief   This structure defines the httpclient ssl structure.  */
typedef struct {
    mbedtls_ssl_context ssl_ctx;        /* mbedtls ssl context */
    mbedtls_net_context net_ctx;        /* fill in socket id   */
    mbedtls_ssl_config ssl_conf;        /* ssl configuration   */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt_profile profile;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
} httpclient_ssl_t;
#endif

/** @brief   http error code */
typedef enum {
    HTTP_EAGAIN   =  1,  /* more data to retrieved */
    HTTP_SUCCESS  =  0,  /* operation success      */
    HTTP_ENOBUFS  = -1,  /* buffer error           */
    HTTP_EARG     = -2,  /* illegal argument       */
    HTTP_ENOTSUPP = -3,  /* not support            */
    HTTP_EDNS     = -4,  /* DNS fail               */
    HTTP_ECONN    = -5,  /* connect fail           */
    HTTP_ESEND    = -6,  /* send packet fail       */
    HTTP_ECLSD    = -7,  /* connect closed         */
    HTTP_ERECV    = -8,  /* recv packet fail       */
    HTTP_EPARSE   = -9,  /* url parse error        */
    HTTP_EPROTO   = -10, /* protocol error         */
    HTTP_EUNKOWN  = -11, /* unknown error          */
    HTTP_ETIMEOUT = -12, /* timeout                */
} HTTPC_RESULT;

typedef uint32_t httpc_handle_t;

/* The following content is used in the HttpClient module. */
#define HTTPCLIENT_DEBUG 0

#define HTTP_PORT   80   /* HTTP default port */
#define HTTPS_PORT  443  /* HTTPS default port */

#define HTTP_ALWAYS_HTTP_FLAG 0x80

/**
 * http client module initialize
 *
 * @return HTTP_SUCCESS http client initialize success
 *
 */
int8_t http_client_initialize(void);

/**
 * initialize a http client
 *
 * @param[in] settings pointer to http client settings
 *
 * @return http client handle
 * @return NULL fail to initialize a http client
 *
 */
httpc_handle_t httpc_init(httpc_connection_t *settings);

/**
 * deinitialize a http client
 *
 * @param[in] httpc http client context handle
 *
 * @return HTTP_SUCCESS deinitialize http client success
 * @return < 0          deinitialize http client failed
 *
 */
int8_t httpc_deinit(httpc_handle_t httpc);

/**
 * http client send request
 *
 * @param[in] httpc        http client context handle
 * @param[in] method       http method
 * @param[in] uri          pointer to URI
 * @param[in] hdr          pointer to http hdr
 * @param[in] content_type pointer to content type
 * @param[in] param        pointer to http request's param
 * @param[in] param_len    param buffer length
 *
 * @return HTTP_SUCCESS  http client send request
 * @return HTTP_EARG     input illegal arguments
 * @return HTTP_EDNS     DNS fail to get IP address
 * @return HTTP_ENOTSUPP operation not support
 * @return HTTP_ECONN    establish connection fail
 * @return HTTP_ESEND    send request fail
 *
 */
int8_t httpc_send_request(httpc_handle_t httpc, int8_t method, char *uri,
                          const char *hdr, const char *content_type,
                          const char *param, uint16_t param_len);

/**
 * http client receive response
 *
 * @param[in] httpc     http client context handle
 * @param[in] rsp       pointer to buffer space for storing response
 * @param[in] rsp_size  response buffer size
 * @param[in] info      pointer to response information
 * @param[in] timeout   time waiting for response
 * @param[in] param     pointer to http request's param
 * @param[in] param_len param buffer length
 *
 * @return HTTP_SUCCESS  http client send request
 * @return HTTP_EARG     input illegal arguments
 * @return HTTP_EDNS     DNS fail to get IP address
 * @return HTTP_ENOTSUPP operation not support
 * @return HTTP_ECONN    establish connection fail
 * @return HTTP_ESEND    send request fail
 *
 */
int32_t httpc_recv_response(httpc_handle_t httpc, uint8_t *rsp, uint32_t rsp_size,
                            http_rsp_info_t *info, uint32_t timeout);

/**
 * http client construct header
 *
 * @param[in] buf   pointer to buffer space
 * @param[in] buf_size buffer space size
 * @param[in] name  pointer to http request header field name
 * @param[in] data  pointer to http request header field data
 *
 * @return (0, buf_size] construct header size
 * @return HTTP_EARG     input illegal arguments
 * @return HTTP_ENOBUFS  DNS fail to get IP address
 *
 */
int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data);

/**
 * This function executes a GET request on a given URL. It blocks until completion.
 * @param[in] client             client is a pointer to the #httpclient_t.
 * @param[in] url                url is the URL to run the request.
 * @param[in, out] client_data   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpc_get(httpclient_t *client, char *url, httpclient_data_t *client_data);


/**
 * This function executes a POST request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #httpclient_t.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] client_data    client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be posted.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpc_post(httpclient_t *client, char *url, httpclient_data_t *client_data);

/**
 * This function executes a PUT request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #httpclient_t.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] client_data    client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be put.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpc_put(httpclient_t *client, char *url, httpclient_data_t *client_data);

/**
 * This function executes a DELETE request on a given URL. It blocks until completion.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @param[in] url                  url is the URL to run the request.
 * @param[in, out] client_data client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpc_delete(httpclient_t *client, char *url, httpclient_data_t *client_data);

/**
 * This function gets the HTTP response code assigned to the last request.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @return           The HTTP response code of the last request.
 */
int httpc_get_response_code(httpclient_t *client);

/**
 * This function sets a custom header.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @param[in] header               header is a custom header string.
 * @return           None.
 */
void httpc_set_custom_header(httpclient_t *client, char *header);

/**
 * This function get specified response header value.
 * @param[in] header_buf header_buf is the response header buffer.
 * @param[in] name                 name is the specified http response header name.
 * @param[in, out] val_pos         val_pos is the position of header value in header_buf.
 * @param[in, out] val_len         val_len is header value length.
 * @return           0, if value is got. Others, if errors occurred.
 */
int httpc_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len);

/**
 * This function add text formdata information.
 * @param[in] client_data          client_data is a pointer to the #httpclient_data_t.
 * @param[in] content_disposition  content_disposition is a pointer to the content disposition string.
 * @param[in] content_type         content_type is a pointer to the content type string.
 * @param[in] name                 name is a pointer to the name string.
 * @param[in] data                 data is a pointer to the data.
 * @param[in] data_len             data_len is the data length.
 * @return           The HTTP response code of the last request.
 */
int httpc_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len);

/**
 * This function add file formdata information.
 * @param[in] client_data          client_data is a pointer to the #httpclient_data_t.
 * @param[in] content_disposition  content_disposition is a pointer to the content disposition string.
 * @param[in] content_type         content_type is a pointer to the content type string.
 * @param[in] file_path            file_path is a pointer to the file path.
 * @return           The HTTP response code of the last request.
 */
int httpc_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path);

/** @} */
#endif /* HTTP_API_H */
