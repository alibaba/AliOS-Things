/**
 * @file    wm_http_client.h
 *
 * @brief   Http client APIs
 *
 * @author  wanghf
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_HTTP_CLIENT_H
#define WM_HTTP_CLIENT_H

#include "wm_config.h"
#include "wm_type_def.h"


/* HTTP Status, API Return codes */
/** HTTP Success status */
#define HTTP_CLIENT_SUCCESS                 0
/** Unknown error */
#define HTTP_CLIENT_UNKNOWN_ERROR           1
/** an Invalid handle or possible bad pointer was passed to a function */
#define HTTP_CLIENT_ERROR_INVALID_HANDLE    2
/** Buffer too small or a failure while in memory allocation */
#define HTTP_CLIENT_ERROR_NO_MEMORY         3
/** an attempt to use an invalid socket handle was made */
#define HTTP_CLIENT_ERROR_SOCKET_INVALID    4
/** Can't send socket parameters */
#define HTTP_CLIENT_ERROR_SOCKET_CANT_SET   5
/** Error while resolving host name */
#define HTTP_CLIENT_ERROR_SOCKET_RESOLVE    6
/** Error while connecting to the remote server */
#define HTTP_CLIENT_ERROR_SOCKET_CONNECT    7
/** socket time out error */
#define HTTP_CLIENT_ERROR_SOCKET_TIME_OUT   8
/** Error while receiving data */
#define HTTP_CLIENT_ERROR_SOCKET_RECV       9
/** Error while sending data */
#define HTTP_CLIENT_ERROR_SOCKET_SEND       10
/** Error while receiving the remote HTTP headers */
#define HTTP_CLIENT_ERROR_HEADER_RECV       11
/** Could not find element within header */
#define HTTP_CLIENT_ERROR_HEADER_NOT_FOUND  12
/** The headers search clue was too large for the internal API buffer */
#define HTTP_CLIENT_ERROR_HEADER_BIG_CLUE   13
/** No content length was specified for the outgoing data. the caller should
    specify chunking mode in the session creation */
#define HTTP_CLIENT_ERROR_HEADER_NO_LENGTH  14
/** The HTTP chunk token that was received from the server was too big and possibly wrong */
#define HTTP_CLIENT_ERROR_CHUNK_TOO_BIG     15
/** Could not authenticate with the remote host */
#define HTTP_CLIENT_ERROR_AUTH_HOST         16
/** Could not authenticate with the remote proxy */
#define HTTP_CLIENT_ERROR_AUTH_PROXY        17
/** Bad or not supported HTTP verb was passed to a function */
#define HTTP_CLIENT_ERROR_BAD_VERB          18
/** a function received a parameter that was too large */
#define HTTP_CLIENT_ERROR_LONG_INPUT        19
/** The session state prevents the current function from proceeding */
#define HTTP_CLIENT_ERROR_BAD_STATE         20
/** Could not parse the chunk length while in chunked transfer */
#define HTTP_CLIENT_ERROR_CHUNK             21
/** Could not parse curtail elements from the URL (such as the host name, HTTP prefix act') */
#define HTTP_CLIENT_ERROR_BAD_URL           22
/** Could not detect key elements in the received headers */
#define HTTP_CLIENT_ERROR_BAD_HEADER        23
/** Error while attempting to resize a buffer */
#define HTTP_CLIENT_ERROR_BUFFER_RSIZE      24
/** Authentication schema is not supported */
#define HTTP_CLIENT_ERROR_BAD_AUTH          25
/** The selected authentication schema does not match the server response */
#define HTTP_CLIENT_ERROR_AUTH_MISMATCH     26
/** an element was missing while parsing the digest authentication challenge */
#define HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN   27
/** Digest algorithem could be MD5 or MD5-sess other types are not supported */
#define HTTP_CLIENT_ERROR_NO_DIGEST_ALG     28
/** Binding error */
#define HTTP_CLIENT_ERROR_SOCKET_BIND		29
/** Tls negotiation error */
#define HTTP_CLIENT_ERROR_TLS_NEGO			30
/** Feature is not (yet) implemented */
#define HTTP_CLIENT_ERROR_NOT_IMPLEMENTED   64
/** HTTP end of stream message */
#define HTTP_CLIENT_EOS                     1000

// HTTP Session flags (Public flags)
#define HTTP_CLIENT_FLAG_KEEP_ALIVE         0x00000001 // Set the keep alive header
#define HTTP_CLIENT_FLAG_SEND_CHUNKED       0x00000002 // The outgoing should chunked
#define HTTP_CLIENT_FLAG_NO_CACHE           0x00000004 // Set the no cache header
#define HTTP_CLIENT_FLAG_ASYNC              0x00000008 // Currently not implemented 
#define HTTP_CLIENT_FLAG_MULTIPART_FORM       0x00000010 // The outgoing should multipart/form-data

// HTTP Type Definitions 
typedef u32          HTTP_SESSION_HANDLE;
typedef u32          HTTP_CLIENT_SESSION_FLAGS;
/******************************************************************************
*
*  Section      : HTTP API structures
*
******************************************************************************/

/* HTTP Type Definitions */
/** http seesion handle */
typedef u32 tls_http_session_handle_t;
/** http seesion flags */
typedef u32 tls_http_session_flags_t;

/** HTTP Supported authentication methods */
typedef enum tls_http_auth_achema
{
    WM_AUTH_SCHEMA_NONE      = 0,
    WM_AUTH_SCHEMA_BASIC,
    WM_AUTH_SCHEMA_DIGEST,
    WM_AUTH_SCHEMA_KERBEROS,
    WM_AUTH_NOT_SUPPORTED
} tls_http_auth_achema_t;

/** HTTP supported verbs */
typedef enum tls_http_verb
{
    WM_VERB_GET              = 0,
    WM_VERB_HEAD,
    WM_VERB_POST,
    WM_VERB_PUT,
    WM_VERB_NOT_SUPPORTED,
    WM_VERB_FWUP
} tls_http_verb_t;

/** Data structure that the caller can request at any time that will include
    some information regarding the session */
typedef struct tls_http_client
{
    u32  http_status_code;                 /**< HTTP Status code (200 OK) */
    u32  request_body_length_sent;         /**< Total bytes sent (body only) */
    u32  response_body_length_received;    /**< Total bytes received (body only) */
    u32  total_response_body_length;       /**< as extracted from the “content-length" header */
    u32  http_state;                       /**< HTTP client state */
} tls_http_client_t;

/** HTTP parameters */
typedef struct tls_http_parameters
{
    char*                  url;
    char*                  proxy_host;
    u32                    use_proxy;
    u32                    proxy_port;
    u32                    verbose;
    char*                  user_name;
    char*                  password;
    tls_http_auth_achema_t auth_type;
} tls_http_param_t;

#if TLS_CONFIG_HTTP_CLIENT_TASK
/** the callback function of http clent for received */
typedef void  (*tls_http_client_recv_callback_fn)(tls_http_session_handle_t session, char *data, u32 total_len, u32 data_len);
/** the callback function of http clent for err */
typedef void  (*tls_http_client_err_callback_fn)(tls_http_session_handle_t session, int err);

/** message of the http client */
typedef struct _http_client_msg
{
    tls_http_session_handle_t        session;
    tls_http_param_t                 param;
    tls_http_verb_t                  method;
    char*                            send_data;
    u32                              data_len;
    tls_http_client_recv_callback_fn recv_fn;
    tls_http_client_err_callback_fn  err_fn;
} tls_http_client_msg;
#endif

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup HTTPC_APIs HTTPC APIs
 * @brief HTTP client APIs
 */

/**
 * @addtogroup HTTPC_APIs
 * @{
 */


/******************************************************************************
*
*  Section      : HTTP API public interface
*
******************************************************************************/

/**
 * @brief          Allocate memory for a new HTTP Session
 *
 * @param[in]      flags    HTTP Session internal API flags, 0 should be passed here
 *
 * @retval         0        failed
 * @retval         other    HTTP Session handle
 *
 * @note           None
 */
tls_http_session_handle_t tls_http_client_open_request(tls_http_session_flags_t flags);

/**
 * @brief          Closes any active connection and free any used memory
 *
 * @param[in]      *session                HTTP Session handle
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_close_request(tls_http_session_handle_t *session);

#if TLS_CONFIG_HTTP_CLIENT_AUTH
/**
 * @brief          Sets the HTTP authentication schema
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      auth_schema             HTTP Supported authentication methods
 * @param[in]      *reserved               Reserved parameter
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_set_auth(tls_http_session_handle_t session,
                             tls_http_auth_achema_t auth_schema,
                             void *reserved);

/**
 * @brief          Sets credentials for the target host
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      *user_name              User name
 * @param[in]      *password               Password
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_set_credentials(tls_http_session_handle_t session,
                                    char *user_name, char *password);
#endif

#if TLS_CONFIG_HTTP_CLIENT_PROXY
/**
 * @brief          Sets all the proxy related parameters
 *
 * @param[in]      session        HTTP Session handle
 * @param[in]      *proxy_name    The host name
 * @param[in]      port           The proxy port number
 * @param[in]      *user_name     User name for proxy authentication (can be null)
 * @param[in]      *password      User password for proxy authentication (can be null)
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_set_proxy(tls_http_session_handle_t session, char *proxy_name,
                              u16 port, char *user_name, char *password);
#endif

/**
 * @brief          Sets the HTTP verb for the outgoing request
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      http_verb               HTTP supported verbs
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_set_verb(tls_http_session_handle_t session, tls_http_verb_t http_verb);

/**
 * @brief          Add headers to the outgoing request
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      *header_name            The Headers name
 * @param[in]      *header_data            The headers data
 * @param[in]      insert                  Reserved could be any
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_add_request_headers(tls_http_session_handle_t session, char *header_name,
                                        char *header_data, bool insert);

/**
 * @brief          This function builds the request headers, performs a DNS resolution,
 *                 opens the connection (if it was not opened yet by a previous request
 *                 or if it has closed) and sends the request headers
 *
 * @param[in]      session      HTTP Session handle
 * @param[in]      *url         The requested URL
 * @param[in]      *data        Data to post to the server
 * @param[in]      data_len     Length of posted data
 * @param[in]      total_len    Valid only when http method is post
 *                              TRUE:  Post data to http server.
 *                              FALSE: In a post request without knowing the total
 *                                     length in advance so return error or use chunking.
 * @param[in]      time_out     Operation timeout
 * @param[in]      client_port  Client side port 0 for none
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_send_request(tls_http_session_handle_t session, char *url, void *data,
                                 u32 data_len, bool total_len, u32 time_out, u32 client_port);

/**
 * @brief          Write data to the remote server
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      *buffer                 Data to write to the server
 * @param[in]      buffer_len              Length of wtitten data
 * @param[in]      time_out                Timeout for the operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_write_data(tls_http_session_handle_t session, void *buffer,
                               u32 buffer_len, u32 time_out);

/**
 * @brief          Receives the response header on the connection and parses it.
 *                 Performs any required authentication.
 *
 * @param[in]      session                 HTTP Session handle
 * @param[in]      time_out                Timeout for the operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_recv_response(tls_http_session_handle_t session, u32 time_out);

/**
 * @brief          Read data from the server. Parse out the chunks data
 *
 * @param[in]      session           HTTP Session handle
 * @param[out]     *buffer           A pointer to a buffer that will be filled with the servers response
 * @param[in]      bytes_to_read     The size of the buffer (numbers of bytes to read)
 * @param[in]      time_out          Operation timeout in seconds
 * @param[out]     *bytes_recived    Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_read_data(tls_http_session_handle_t session, void *buffer,
                              u32 bytes_to_read, u32 time_out, u32 *bytes_recived);

/**
 * @brief          Fill the users structure with the session information
 *
 * @param[in]      session                 HTTP Session handle
 * @param[out]     *http_client            The session information
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_get_info(tls_http_session_handle_t session, tls_http_client_t *http_client);

/**
 * @brief          Initiate the headr searching functions and find the first header
 *
 * @param[in]      session           HTTP Session handle
 * @param[in]      *search_clue      Search clue
 * @param[out]     *header_buffer    A pointer to a buffer that will be filled with the header name and value
 * @param[out]     *length           Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_find_first_header(tls_http_session_handle_t session, char *search_clue,
                                      char *header_buffer, u32 *length);

/**
 * @brief          Find the next header.
 *
 * @param[in]      session           HTTP Session handle
 * @param[out]     *header_buffer    A pointer to a buffer that will be filled with the header name and value
 * @param[out]     *length           Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_get_next_header(tls_http_session_handle_t session,
                                    char *header_buffer, u32 *length);

/**
 * @brief          Terminate a headers search session
 *
 * @param[in]      session                 HTTP Session handle
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
int tls_http_client_find_close_header(tls_http_session_handle_t session);

#if TLS_CONFIG_HTTP_CLIENT_TASK
/**
 * @brief          initialize task of the http client
 *
 * @param          None
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED      failed
 *
 * @note           None
 */
int tls_http_client_task_init(void);

/**
 * @brief          post message to the task of http client
 *
 * @param[in]      msg      pointer to the message
 *
 * @retval         ERR_OK     success
 * @retval         other      failed
 *
 * @note           None
 */
int tls_http_client_post(tls_http_client_msg * msg);
#endif /* TLS_CONFIG_HTTP_CLIENT_TASK */

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_HTTP_CLIENT_H */

