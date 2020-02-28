/**
 * @file httpclient.h
 * http API header file.
 *
 * @version   V1.0
 * @date      2019-12-24
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include "http.h"

/** @defgroup aos_httpclient_api http
  * @{
  */

/** @brief   http requst type */
typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD
} HTTP_REQUEST_TYPE;


/** @brief   This structure defines the httpclient_t structure   */
typedef struct {
    int socket;                     /**< socket ID                 */
    int remote_port;                /**< hTTP or HTTPS port        */
    int response_code;              /**< response code             */
    char *header;                   /**< request custom header     */
    char *auth_user;                /**< username for basic authentication         */
    char *auth_password;            /**< password for basic authentication         */
    bool is_http;                   /**< http connection? if 1, http; if 0, https  */
#if CONFIG_HTTP_SECURE
    const char *server_cert;        /**< server certification      */
    const char *client_cert;        /**< client certification      */
    const char *client_pk;          /**< client private key        */
    int server_cert_len;            /**< server certification lenght, server_cert buffer size  */
    int client_cert_len;            /**< client certification lenght, client_cert buffer size  */
    int client_pk_len;              /**< client private key lenght, client_pk buffer size      */
    void *ssl;                      /**< ssl content               */
#endif
} httpclient_t;

/** @brief   This structure defines the HTTP data structure.  */
typedef struct {
    bool is_more;                /**< indicates if more data needs to be retrieved. */
    bool is_chunked;             /**< response data is encoded in portions/chunks.*/
    int retrieve_len;            /**< content length to be retrieved. */
    int response_content_len;    /**< response content length. */
    int content_block_len;       /**< the content length of one block. */
    int post_buf_len;            /**< post data length. */
    int response_buf_len;        /**< response body buffer length. */
    int header_buf_len;          /**< response head buffer lehgth. */
    char *post_content_type;     /**< content type of the post data. */
    char *post_buf;              /**< user data to be posted. */
    char *response_buf;          /**< buffer to store the response body data. */
    char *header_buf;            /**< buffer to store the response head data. */
    bool  is_redirected;         /**< redirected URL? if 1, has redirect url; if 0, no redirect url */
    char* redirect_url;          /**< redirect url when got http 3** response code. */
} httpclient_data_t;

/**
 * This function executes a GET request on a given URL. It blocks until completion.
 * @param[in] client             client is a pointer to the #httpclient_t.
 * @param[in] url                url is the URL to run the request.
 * @param[in, out] client_data   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_get(httpclient_t *client, const char *url, httpclient_data_t *client_data);

/**
 * This function executes a HEAD request on a given URL. It blocks until completion.
 * @param[in] client             client is a pointer to the #httpclient_t.
 * @param[in] url                url is the URL to run the request.
 * @param[in, out] client_data   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_head(httpclient_t *client, const char *url, httpclient_data_t *client_data);

/**
 * This function executes a POST request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #httpclient_t.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] client_data    client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be posted.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_post(httpclient_t *client, const char *url, httpclient_data_t *client_data);

/**
 * This function executes a PUT request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #httpclient_t.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] client_data    client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be put.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_put(httpclient_t *client, const char *url, httpclient_data_t *client_data);

/**
 * This function executes a DELETE request on a given URL. It blocks until completion.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @param[in] url                  url is the URL to run the request.
 * @param[in, out] client_data client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_delete(httpclient_t *client, const char *url, httpclient_data_t *client_data);

/**
 * This function allocates buffer for http header/response
 * @param[in] client_data      pointer to the #httpclient_data_t.
 * @param[in] header_size      header buffer size
 * @param[in] resp_size        response buffer size
 * @return  HTTP_SUCCESS       success
 * @return  HTTP_EUNKOWN       fail
 */
HTTPC_RESULT httpclient_prepare(httpclient_data_t *client_data, int header_size, int resp_size);

/**
 * This function deallocates buffer for http header/response.
 * @param[in] client_data      pointer to the #httpclient_data_t.
 * @return  HTTP_SUCCESS       success
 * @return  HTTP_EUNKOWN       fail
 */
HTTPC_RESULT httpclient_unprepare(httpclient_data_t *client_data);

/**
 * This function reset buffer for  http header/response.
 * @param[in] client_data      pointer to the #httpclient_data_t.
 * @return           None.
 */
void httpclient_reset(httpclient_data_t *client_data);

/**
 * This function establish http/https connection.
 * @param[in] client            pointer to the #httpclient_t.
 * @param[in] url               remote URL
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_conn(httpclient_t *client, const char *url);

/**
 * This function sends HTTP request.
 * @param[in] client            a pointer to the #httpclient_t.
 * @param[in] url               remote URL
 * @param[in] method            http request method
 * @param[in] client_data       a pointer to #httpclient_data_t.
 * @return    Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_send(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data);

/**
 * This function receives response from remote
 * @param[in]  client               a pointer to #httpclient_t.
 * @param[out] client_data          a pointer to #httpclient_data_t.
 * @return     Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT httpclient_recv(httpclient_t *client, httpclient_data_t *client_data);

/**
 * This function close http connection.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @return           None.
 */
void httpclient_clse(httpclient_t *client);

/**
 * This function sets a custom header.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @param[in] header               header is a custom header string.
 * @return           None.
 */
void httpclient_set_custom_header(httpclient_t *client, char *header);

/**
 * This function gets the HTTP response code assigned to the last request.
 * @param[in] client               client is a pointer to the #httpclient_t.
 * @return           The HTTP response code of the last request.
 */
int httpclient_get_response_code(httpclient_t *client);

/**
 * This function get specified response header value.
 * @param[in] header_buf header_buf is the response header buffer.
 * @param[in] name                 name is the specified http response header name.
 * @param[in, out] val_pos         val_pos is the position of header value in header_buf.
 * @param[in, out] val_len         val_len is header value length.
 * @return           0, if value is got. Others, if errors occurred.
 */
int httpclient_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len);

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
int httpclient_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len);

/**
 * This function add file formdata information.
 * @param[in] client_data          client_data is a pointer to the #httpclient_data_t.
 * @param[in] content_disposition  content_disposition is a pointer to the content disposition string.
 * @param[in] content_type         content_type is a pointer to the content type string.
 * @param[in] file_path            file_path is a pointer to the file path.
 * @return           The HTTP response code of the last request.
 */
int httpclient_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path);

#endif /* HTTPCLIENT_H */
