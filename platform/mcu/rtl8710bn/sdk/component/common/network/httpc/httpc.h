 /**
  ******************************************************************************
  * @file    httpc.h
  * @author
  * @version
  * @brief   This file provides user interface for HTTP/HTTPS client.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#ifndef _HTTPC_H_
#define _HTTPC_H_

/** @addtogroup httpc       HTTPC
 *  @ingroup    network
 *  @brief      HTTP/HTTPS client functions
 *  @{
 */

#include "platform_stdlib.h"
#include "platform_opts.h"

#define HTTPC_SECURE_NONE        0    /*!< Running with HTTP client */
#define HTTPC_SECURE_TLS         1    /*!< Running with HTTPS client */

#define HTTPC_DEBUG_OFF          0    /*!< Disable httpc debug log */
#define HTTPC_DEBUG_ON           1    /*!< Enable httpc debug log */
#define HTTPC_DEBUG_VERBOSE      2    /*!< Enable httpc verbose debug log */

#define HTTPC_TLS_POLARSSL       0    /*!< Use PolarSSL for TLS when HTTPS */
#define HTTPC_TLS_MBEDTLS        1    /*!< Use mbedTLS for TLS when HTTPS */

#if CONFIG_USE_POLARSSL
#define HTTPC_USE_TLS            HTTPC_TLS_POLARSSL
#elif CONFIG_USE_MBEDTLS
#define HTTPC_USE_TLS            HTTPC_TLS_MBEDTLS
#endif

/**
  * @brief  The structure is the context used for HTTP response header parsing.
  * @note   Only header string includes string terminator.
  */
struct http_response {
	uint8_t *header;                 /*!< HTTP header string parsed in HTTP response */
	size_t header_len;               /*!< HTTP header string length */
	uint8_t *version;                /*!< Pointer to HTTP version in the parsed HTTP header string */
	size_t version_len;              /*!< HTTP version data length */
	uint8_t *status;                 /*!< Pointer to HTTP status code in the parsed HTTP header string */
	size_t status_len;               /*!< HTTP status code data length */
	uint8_t *content_type;           /*!< Pointer to Content-Type header field in the parsed HTTP header string */
	size_t content_type_len;         /*!< Content-Type header field data length */
	size_t content_len;              /*!< Value of Content-Length header field parsed in HTTP header string */
};

/**
  * @brief  The structure is the context used for connection.
  */
struct httpc_conn {
	int sock;                        /*!< Client socket descriptor for connection */
	struct http_response response;   /*!< Context for HTTP response */
	void *tls;                       /*!< Context for TLS connection */
	uint8_t *request_header;         /*!< Pointer to transmission buffer of HTTP request header */
	char *host;                      /*!< String of server host name or IP */
	uint16_t port;                   /*!< Service port */
	char *user_password;             /*!< Base64 string for HTTP basic authorization */
};

/**
 * @brief     This function is used to generate connection context for an HTTP/HTTPS client.
 * @param[in] secure: security mode for HTTP or HTTPS. Must be HTTPD_SECURE_NONE, HTTPD_SECURE_TLS.
 * @param[in] client_cert: string of client certificate if required to be verified by server.
 * @param[in] client_key: string of client private key if required to be verified by server.
 * @param[in] ca_certs: string including certificates in CA trusted chain if want to verify server certificate.
 * @return    pointer to the generated connection context
 */
struct httpc_conn *httpc_conn_new(uint8_t secure, char *client_cert, char *client_key, char *ca_certs);

/**
 * @brief      This function is used to deallocate a connection context.
 * @param[in]  conn: pointer to connection context
 * @return     None
 */
void httpc_conn_free(struct httpc_conn *conn);

/**
 * @brief      This function is used to connect to a server.
 * @param[in]  conn: pointer to connection context
 * @param[in]  host: string of server host name or IP
 * @param[in]  port: service port
 * @param[in]  timeout: connection timeout in seconds
 * @return     0 : if successful
 * @return     -1 : if error occurred
 */
int httpc_conn_connect(struct httpc_conn *conn, char *host, uint16_t port, uint32_t timeout);

/**
 * @brief      This function is used to close connection from a server.
 * @param[in]  conn: pointer to connection context
 * @return     None
 */
void httpc_conn_close(struct httpc_conn *conn);

/**
 * @brief      This function is used to setup authorization for connection.
 * @param[in]  conn: pointer to connection context
 * @param[in]  user: string of user name for authorization
 * @param[in]  password: string of password for authorization
 * @return     0 : if successful
 * @return     -1 : if error occurred
 * @note       Must be used before httpc_conn_connect() if basic authorization is used
 */
int httpc_conn_setup_user_password(struct httpc_conn *conn, char *user, char *password);

/**
 * @brief     This function is used to setup httpc debug.
 * @param[in] debug: flag to enable/disable httpc debug. Must be HTTPC_DEBUG_OFF, HTTPC_DEBUG_ON, HTTPC_DEBUG_VERBOSE.
 * @return    None
 */
void httpc_setup_debug(uint8_t debug);

/**
 * @brief     This function is used to free memory allocated by httpc API, such as httpc_response_get_header_field().
 * @param[in] ptr: pointer to memory to be deallocated
 * @return    None
 */
void httpc_free(void *ptr);

/**
 * @brief     This function is used to dump the parsed HTTP header of response in connection context.
 * @param[in] conn: pointer to connection context
 * @return    None
 */
void httpc_conn_dump_header(struct httpc_conn *conn);

/**
 * @brief      This function is used to start a HTTP request in connection.
 * @param[in]  conn: pointer to connection context
 * @param[in]  method: string of HTTP method in HTTP request
 * @param[in]  resource: string including path and query string to identify a resource
 * @param[in]  content_type: string of Content-Type header field written to HTTP request. No Content-Type in HTTP request if NULL.
 * @param[in]  content_len: value of Content-Length header field written to HTTP request. No Content-Length in HTTP request if NULL.
 * @return     0 : if successful
 * @return     -1 : if error occurred
 */
int httpc_request_write_header_start(struct httpc_conn *conn, char *method, char *resource, char *content_type, size_t content_len);

/**
 * @brief      This function is used to add an HTTP header field to HTTP request.
 * @param[in]  conn: pointer to connection context
 * @param[in]  name: HTTP header field name string
 * @param[in]  value: HTTP header field value
 * @return     0 : if successful
 * @return     -1 : if error occurred
 */
int httpc_request_write_header(struct httpc_conn *conn, char *name, char *value);

/**
 * @brief      This function is used to write HTTP request header data to connection.
 * @param[in]  conn: pointer to connection context
 * @return     return value of lwip socket write() for HTTP and PolarSSL ssl_write() for HTTPS
 */
int httpc_request_write_header_finish(struct httpc_conn *conn);

/**
 * @brief      This function is used to write HTTP request body data to connection.
 * @param[in]  conn: pointer to connection context
 * @param[in]  data: data to be written
 * @param[in]  data_len: data length
 * @return     return value of lwip socket write() for HTTP and PolarSSL ssl_write() for HTTPS
 */
int httpc_request_write_data(struct httpc_conn *conn, uint8_t *data, size_t data_len);

/**
 * @brief     This function is used to check HTTP status of response in connection context.
 * @param[in] conn: pointer to connection context
 * @param[in] status: HTTP status string to compare with
 * @return    0 : if different
 * @return    1 : if matched
 */
int httpc_response_is_status(struct httpc_conn *conn, char *status);

/**
 * @brief     This function is used to read HTTP header from client socket descriptor and parse content to connection context.
 * @param[in] conn: pointer to connection context
 * @return    0 : if successful
 * @return    -1 : if error occurred
 */
int httpc_response_read_header(struct httpc_conn *conn);

/**
 * @brief      This function is used to read data from HTTP/HTTPS connection.
 * @param[in]  conn: pointer to connection context
 * @param[out] data: buffer for data read
 * @param[in]  data_len: buffer length
 * @return     return value of lwip socket read() for HTTP and PolarSSL ssl_read() for HTTPS
 */
int httpc_response_read_data(struct httpc_conn *conn, uint8_t *data, size_t data_len);

/**
 * @brief      This function is used to get a header field from HTTP header of connection context.
 * @param[in]  conn: pointer to connection context
 * @param[in]  field: header field string to search
 * @param[out] value: search result stored in memory allocated
 * @return    0 : if found
 * @return    -1 : if not found
 * @note      The search result memory should be free by httpc_free().
 */
int httpc_response_get_header_field(struct httpc_conn *conn, char *field, char **value);

/*\@}*/

#endif /* _HTTPC_H_ */
