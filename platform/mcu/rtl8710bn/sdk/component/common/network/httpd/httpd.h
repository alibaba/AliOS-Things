/**
  ******************************************************************************
  * @file    httpd.h
  * @author
  * @version
  * @brief   This file provides user interface for HTTP/HTTPS server.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#ifndef _HTTPD_H_
#define _HTTPD_H_

/** @addtogroup httpd       HTTPD
 *  @ingroup    network
 *  @brief      HTTP/HTTPS server functions
 *  @{
 */

#include "platform_stdlib.h"
#include "platform_opts.h"

#define HTTPD_SECURE_NONE        0   /*!< Running with HTTP server */
#define HTTPD_SECURE_TLS         1   /*!< Running with HTTPS server */
#define HTTPD_SECURE_TLS_VERIFY  2   /*!< Running with HTTPS server and verify client */

#define HTTPD_THREAD_SINGLE      0   /*!< Multi-thread mode for request handling */
#define HTTPD_THREAD_MULTIPLE    1   /*!< Single-thread mode for request handling */

#define HTTPD_DEBUG_OFF          0   /*!< Disable httpd debug log */
#define HTTPD_DEBUG_ON           1   /*!< Enable httpd debug log */
#define HTTPD_DEBUG_VERBOSE      2   /*!< Enable httpd verbose debug log */

#define HTTPD_TLS_POLARSSL       0    /*!< Use PolarSSL for TLS when HTTPS */
#define HTTPD_TLS_MBEDTLS        1    /*!< Use mbedTLS for TLS when HTTPS */

#if CONFIG_USE_POLARSSL
#define HTTPD_USE_TLS            HTTPD_TLS_POLARSSL
#elif CONFIG_USE_MBEDTLS
#define HTTPD_USE_TLS            HTTPD_TLS_MBEDTLS
#endif

/**
  * @brief  The structure is the context used for HTTP request header parsing.
  * @note   Only header string includes string terminator.
  */
struct http_request {
	uint8_t *header;                 /*!< HTTP header string parsed in HTTP request */
	size_t header_len;               /*!< HTTP header string length */
	uint8_t *method;                 /*!< Pointer to HTTP method in the parsed HTTP header string */
	size_t method_len;               /*!< HTTP method data length */
	uint8_t *path;                   /*!< Pointer to resource path in the parsed HTTP header string */
	size_t path_len;                 /*!< Resource path data length */
	uint8_t *query;                  /*!< Pointer to query string in the parsed HTTP header string */
	size_t query_len;                /*!< Query string data length */
	uint8_t *version;                /*!< Pointer to HTTP version in the parsed HTTP header string */
	size_t version_len;              /*!< HTTP version data length */
	uint8_t *host;                   /*!< Pointer to Host header field in the parsed HTTP header string */
	size_t host_len;                 /*!< Host header field data length */
	uint8_t *content_type;           /*!< Pointer to Content-Type header field in the parsed HTTP header string */
	size_t content_type_len;         /*!< Content-Type header field data length */
	size_t content_len;              /*!< Value of Content-Length header field parsed in HTTP header string */
};

/**
  * @brief  The structure is the context used for client connection.
  */
struct httpd_conn {
	int sock;                        /*!< Client socket descriptor for connection */
	struct http_request request;     /*!< Context for HTTP request */
	void *tls;                       /*!< Context for TLS connection */
	uint8_t *response_header;        /*!< Pointer to transmission buffer of HTTP response header */
};

/**
 * @brief     This function is used to start an HTTP or HTTPS server.
 * @param[in] port: service port
 * @param[in] max_conn: max client connections allowed
 * @param[in] stack_bytes: thread stack size in bytes
 * @param[in] thread_mode: server running thread mode. Must be HTTPD_THREAD_SINGLE, HTTPD_THREAD_MULTIPLE.
 * @param[in] secure: security mode for HTTP or HTTPS. Must be HTTPD_SECURE_NONE, HTTPD_SECURE_TLS, HTTPD_SECURE_TLS_VERIFY.
 * @return    0 : if successful
 * @return    -1 : if error occurred
 */
int httpd_start(uint16_t port, uint8_t max_conn, uint32_t stack_bytes, uint8_t thread_mode, uint8_t secure);

/**
 * @brief     This function is used to stop a running server
 * @return    None
 */
void httpd_stop(void);

/**
 * @brief     This function is used to register a callback function for a Web page request handling.
 * @param[in] path: resource path for a page
 * @param[in] callback: callback function to handle the request to page
 * @return    0 : if successful
 * @return    -1 : if error occurred
 */
int httpd_reg_page_callback(char *path, void (*callback)(struct httpd_conn *conn));

/**
 * @brief     This function is used to clear all registered page callback functions.
 * @return    None
 * @note      All page callback will be cleared automatically if httpd_stop().
 */
void httpd_clear_page_callbacks(void);

/**
 * @brief     This function is used to setup httpd debug.
 * @param[in] debug: flag to enable/disable httpd debug. Must be HTTPD_DEBUG_OFF, HTTPD_DEBUG_ON, HTTPD_DEBUG_VERBOSE.
 * @return    None
 */
void httpd_setup_debug(uint8_t debug);

/**
 * @brief     This function is used to setup certificate and key for server before starting with HTTPS.
 * @param[in] server_cert: string of server certificate
 * @param[in] server_key: string of server private key
 * @param[in] ca_certs: string including certificates in CA trusted chain
 * @return    0 : if successful
 * @return    -1 : if error occurred
 * @note      Must be used before httpd_start() if staring HTTPS server
 */
int httpd_setup_cert(const char *server_cert, const char *server_key, const char *ca_certs);

/**
 * @brief     This function is used to setup authorization for server.
 * @param[in] user: string of user name for authorization
 * @param[in] password: string of password for authorization
 * @return    0 : if successful
 * @return    -1 : if error occurred
 * @note      Must be used before httpd_start() if basic authorization is used
 */
int httpd_setup_user_password(char *user, char *password);

/**
 * @brief     This function is used to free memory allocated by httpd API, such as httpd_request_get_header_field() and httpd_request_get_query_key().
 * @param[in] ptr: pointer to memory to be deallocated
 * @return    None
 */
void httpd_free(void *ptr);

/**
 * @brief     This function is used to close a client connection and release context resource.
 * @param[in] conn: pointer to connection context
 * @return    None
 * @note      Multiple requests/response can be handled in a connection before connection closed. All connections will be closed automatically if httpd_stop().
 */
void httpd_conn_close(struct httpd_conn *conn);

/**
 * @brief     This function is used to dump the parsed HTTP header of request in connection context.
 * @param[in] conn: pointer to connection context
 * @return    None
 */
void httpd_conn_dump_header(struct httpd_conn *conn);

/**
 * @brief     This function is used to check HTTP method of request in connection context.
 * @param[in] conn: pointer to connection context
 * @param[in] method: HTTP method string to compare with
 * @return    0 : if different
 * @return    1 : if matched
 */
int httpd_request_is_method(struct httpd_conn *conn, char *method);

/**
 * @brief     This function is used to read HTTP header from client socket descriptor and parse content to connection context.
 * @param[in] conn: pointer to connection context
 * @return    0 : if successful
 * @return    -1 : if error occurred
 * @note      httpd_request_read_header() is automatically invoked by httpd server to parse request before executing page callback
 */
int httpd_request_read_header(struct httpd_conn *conn);

/**
 * @brief      This function is used to read data from HTTP/HTTPS connection.
 * @param[in]  conn: pointer to connection context
 * @param[out] data: buffer for data read
 * @param[in]  data_len: buffer length
 * @return     return value of lwip socket read() for HTTP and PolarSSL ssl_read() for HTTPS
 */
int httpd_request_read_data(struct httpd_conn *conn, uint8_t *data, size_t data_len);

/**
 * @brief      This function is used to get a header field from HTTP header of connection context.
 * @param[in]  conn: pointer to connection context
 * @param[in]  field: header field string to search
 * @param[out] value: search result stored in memory allocated
 * @return    0 : if found
 * @return    -1 : if not found
 * @note      The search result memory should be free by httpd_free().
 */
int httpd_request_get_header_field(struct httpd_conn *conn, char *field, char **value);

/**
 * @brief      This function is used to get a key value from query string in HTTP header of connection context.
 * @param[in]  conn: pointer to connection context
 * @param[in]  key: key name string to search
 * @param[out] value: search result stored in memory allocated
 * @return    0 : if found
 * @return    -1 : if not found
 * @note      The search result memory should be free by httpd_free().
 */
int httpd_request_get_query_key(struct httpd_conn *conn, char *key, char **value);

/**
 * @brief      This function is used to start a HTTP response in connection.
 * @param[in]  conn: pointer to connection context
 * @param[in]  status: string of status code in HTTP response
 * @param[in]  content_type: string of Content-Type header field written to HTTP response. No Content-Type in HTTP response if NULL.
 * @param[in]  content_len: value of Content-Length header field written to HTTP response. No Content-Length in HTTP response if NULL.
 * @return     0 : if successful
 * @return     -1 : if error occurred
 */
int httpd_response_write_header_start(struct httpd_conn *conn, char *status, char *content_type, size_t content_len);

/**
 * @brief      This function is used to add an HTTP header field to HTTP response.
 * @param[in]  conn: pointer to connection context
 * @param[in]  name: HTTP header field name string
 * @param[in]  value: HTTP header field value
 * @return     0 : if successful
 * @return     -1 : if error occurred
 */
int httpd_response_write_header(struct httpd_conn *conn, char *name, char *value);

/**
 * @brief      This function is used to write HTTP response header data to connection.
 * @param[in]  conn: pointer to connection context
 * @return     return value of lwip socket write() for HTTP and PolarSSL ssl_write() for HTTPS
 */
int httpd_response_write_header_finish(struct httpd_conn *conn);

/**
 * @brief      This function is used to write HTTP response body data to connection.
 * @param[in]  conn: pointer to connection context
 * @param[in]  data: data to be written
 * @param[in]  data_len: data length
 * @return     return value of lwip socket write() for HTTP and PolarSSL ssl_write() for HTTPS
 */
int httpd_response_write_data(struct httpd_conn *conn, uint8_t *data, size_t data_len);

/**
 * @brief      This function is used to write a default HTTP response for error of 400 Bad Request.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_bad_request(struct httpd_conn *conn, char *msg);

/**
 * @brief      This function is used to write a default HTTP response for error of 401 Unauthorized.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_unauthorized(struct httpd_conn *conn, char *msg);

/**
 * @brief      This function is used to write a default HTTP response for error of 404 Not Found.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_not_found(struct httpd_conn *conn, char *msg);

/**
 * @brief      This function is used to write a default HTTP response for error of 405 Method Not Allowed.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_method_not_allowed(struct httpd_conn *conn, char *msg);

/**
 * @brief      This function is used to write a default HTTP response for error of 429 Too Many Requests.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_too_many_requests(struct httpd_conn *conn, char *msg);

/**
 * @brief      This function is used to write a default HTTP response for error of 500 Internal Server Error.
 * @param[in]  conn: pointer to connection context
 * @param[in]  msg: message write to HTTP response body. A default message will be used if NULL.
 * @return     None
 */
void httpd_response_internal_server_error(struct httpd_conn *conn, char *msg);

/*\@}*/

#endif /* _HTTPD_H_ */
