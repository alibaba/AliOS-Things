/**
 * \file            lwesp_http_server.h
 * \brief           HTTP server with callback API
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#ifndef LWESP_HDR_HTTP_SERVER_H
#define LWESP_HDR_HTTP_SERVER_H

#include "lwesp/apps/lwesp_apps.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP_APPS
 * \defgroup        LWESP_APP_HTTP_SERVER HTTP server
 * \brief           HTTP server based on callback API
 * \{
 */

/**
 * \defgroup        LWESP_OPT_APP_HTTP Configuration
 * \brief           Configuration of HTTP server app
 * \{
 */

/**
 * \brief           Server debug default setting
 */
#ifndef LWESP_CFG_DBG_SERVER
#define LWESP_CFG_DBG_SERVER                  LWESP_DBG_OFF
#endif

#ifndef HTTP_SSI_TAG_START
#define HTTP_SSI_TAG_START                  "<!--#" /*!< SSI tag start string */
#define HTTP_SSI_TAG_START_LEN              5   /*!< SSI tag start length */
#endif

#ifndef HTTP_SSI_TAG_END
#define HTTP_SSI_TAG_END                    "-->"   /*!< SSI tag end string */
#define HTTP_SSI_TAG_END_LEN                3   /*!< SSI tag end length */
#endif

/**
 * \brief           Maximal length of tag name excluding start and end parts of tag
 */
#ifndef HTTP_SSI_TAG_MAX_LEN
#define HTTP_SSI_TAG_MAX_LEN                10
#endif

/**
 * \brief           Enables `1` or disables `0` support for POST request
 */
#ifndef HTTP_SUPPORT_POST
#define HTTP_SUPPORT_POST                   1
#endif

/**
 * \brief           Maximal length of allowed uri length including parameters in format `/uri/sub/path?param=value`
 */
#ifndef HTTP_MAX_URI_LEN
#define HTTP_MAX_URI_LEN                    256
#endif

/**
 * \brief           Maximal number of parameters in URI
 */
#ifndef HTTP_MAX_PARAMS
#define HTTP_MAX_PARAMS                     16
#endif

/**
 * \brief           Enables `1` or disables `0` method not allowed response.
 *
 * Response is used in case user makes HTTP request with method
 * which is not on the list of allowed methods. See \ref http_req_method_t
 *
 * \note            When disabled, connection will be closed without response
 */
#ifndef HTTP_USE_METHOD_NOTALLOWED_RESP
#define HTTP_USE_METHOD_NOTALLOWED_RESP     1
#endif

/**
 * \brief           Enables `1` or disables `1` default static files
 *
 * To allow fast startup of server development,
 * several static files are included by default:
 *  - /index.html
 *  - /index.shtml
 *  - /js/style.css
 *  - /js/js.js
 */
#ifndef HTTP_USE_DEFAULT_STATIC_FILES
#define HTTP_USE_DEFAULT_STATIC_FILES       1
#endif

/**
 * \brief           Enables `1` or disables `0` dynamic headers support
 *
 * With dynamic headers enabled, script will try to detect most common
 * file extensions and will try to response with:
 *  - HTTP response code as first line
 *  - Server name as second line
 *  - Content type as third line including end of headers (empty line)
 */
#ifndef HTTP_DYNAMIC_HEADERS
#define HTTP_DYNAMIC_HEADERS                1
#endif

/**
 * \brief           Enables `1` or disables `0` content length header for response
 *
 * If response has fixed length without SSI tags,
 * dynamic headers will try to include "Content-Length" header
 * as part of response message sent to client
 *
 * \note            In order to use this, \ref HTTP_DYNAMIC_HEADERS must be enabled
 */
#ifndef HTTP_DYNAMIC_HEADERS_CONTENT_LEN
#define HTTP_DYNAMIC_HEADERS_CONTENT_LEN    1
#endif

/**
 * \brief           Default server name for `Server: x` response dynamic header
 */
#ifndef HTTP_SERVER_NAME
#define HTTP_SERVER_NAME                    "ESP8266 AT Lib"
#endif

/**
 * \}
 */

/**
 * \brief           Maximal number of headers we can control
 */
#define HTTP_MAX_HEADERS                    4

struct http_state;
struct http_fs_file;

/**
 * \brief           HTTP parameters on http URI in format `?param1=value1&param2=value2&...`
 */
typedef struct {
    const char* name;                           /*!< Name of parameter */
    const char* value;                          /*!< Parameter value */
} http_param_t;

/**
 * \brief           CGI callback function
 * \param[in]       params: Pointer to list of parameteres and their values
 * \param[in]       params_len: Number of parameters
 * \return          Function must return a new URI which is used later
 *                  as response string, such as "/index.html" or similar
 */
typedef char*   (*http_cgi_fn)(http_param_t* params, size_t params_len);

/**
 * \brief           CGI structure to register handlers on URI paths
 */
typedef struct {
    const char* uri;                            /*!< URI path for CGI handler */
    http_cgi_fn fn;                             /*!< Callback function to call when we have a CGI match */
} http_cgi_t;

/**
 * \brief           Post request started with non-zero content length function prototype
 * \param[in]       hs: HTTP state
 * \param[in]       uri: POST request URI
 * \param[in]       content_length: Total content length (Content-Length HTTP parameter) in units of bytes
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
typedef lwespr_t  (*http_post_start_fn)(struct http_state* hs, const char* uri, uint32_t content_length);

/**
 * \brief           Post data received on request function prototype
 * \note            This function may be called multiple time until content_length from \ref http_post_start_fn callback is not reached
 * \param[in]       hs: HTTP state
 * \param[in]       pbuf: Packet buffer wit reciveed data
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
typedef lwespr_t  (*http_post_data_fn)(struct http_state* hs, lwesp_pbuf_p pbuf);

/**
 * \brief           End of POST data request function prototype
 * \param[in]       hs: HTTP state
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
typedef lwespr_t  (*http_post_end_fn)(struct http_state* hs);

/**
 * \brief           SSI (Server Side Includes) callback function prototype
 * \note            User can use server write functions to directly write to connection output
 * \param[in]       hs: HTTP state
 * \param[in]       tag_name: Name of TAG to replace with user content
 * \param[in]       tag_len: Length of TAG
 * \retval          1: Everything was written on this tag
 * \retval          0: There are still data to write to output which means
 *                     callback will be called again for user to process all the data
 */
typedef size_t  (*http_ssi_fn)(struct http_state* hs, const char* tag_name, size_t tag_len);

/**
 * \brief           File system open file function
 *                  Function is called when user file system (FAT or similar) should be invoked to open a file from specific path
 * \param[in]       file: Pointer to file where user has to set length of file if opening was successful
 * \param[in]       path: Path of file to open
 * \return          1 if file is opened, 0 otherwise
 */
typedef uint8_t (*http_fs_open_fn)(struct http_fs_file* file, const char* path);

/**
 * \brief           File system read file function
 *                  Function may be called for `2` purposes. First is to read data and second to get remaining length of file to read
 * \param[in]       file: File pointer to read content
 * \param[in]       buff: Buffer to read data to. When parameter is set to NULL, number of remaining bytes available to read should be returned
 * \param[in]       btr: Number of bytes to read from file. This parameter has no meaning when buff is NULL
 * \return          Number of bytes read or number of bytes available to read
 */
typedef uint32_t    (*http_fs_read_fn)(struct http_fs_file* file, void* buff, size_t btr);

/**
 * \brief           Close file callback function
 * \param[in]       file: File to close
 * \return          `1` on success, `0` otherwise
 */
typedef uint8_t (*http_fs_close_fn)(struct http_fs_file* file);

/**
 * \brief           HTTP server initialization structure
 */
typedef struct {
    /* POST request related functions */
#if HTTP_SUPPORT_POST || __DOXYGEN__
    http_post_start_fn post_start_fn;           /*!< Callback function for post start */
    http_post_data_fn post_data_fn;             /*!< Callback functon for post data */
    http_post_end_fn post_end_fn;               /*!< Callback functon for post end */
#endif /* HTTP_SUPPORT_POST || __DOXYGEN__ */

    /* CGI related */
    const http_cgi_t* cgi;                      /*!< Pointer to array of CGI entries. Set to NULL if not used */
    size_t cgi_count;                           /*!< Length of CGI array. Set to 0 if not used */

    /* SSI related */
    http_ssi_fn ssi_fn;                         /*!< SSI callback function */

    /* File system related */
    http_fs_open_fn fs_open;                    /*!< Open file function callback */
    http_fs_read_fn fs_read;                    /*!< Read file function callback */
    http_fs_close_fn fs_close;                  /*!< Close file function callback */
} http_init_t;

/**
 * \brief           Request method type
 */
typedef enum {
    HTTP_METHOD_NOTALLOWED,                     /*!< HTTP method is not allowed */
    HTTP_METHOD_GET,                            /*!< HTTP request method GET */
#if HTTP_SUPPORT_POST || __DOXYGEN__
    HTTP_METHOD_POST,                           /*!< HTTP request method POST */
#endif /* HTTP_SUPPORT_POST || __DOXYGEN__ */
} http_req_method_t;

/**
 * \brief           List of SSI TAG parsing states
 */
typedef enum {
    HTTP_SSI_STATE_WAIT_BEGIN = 0x00,           /*!< Waiting beginning of tag */
    HTTP_SSI_STATE_BEGIN = 0x01,                /*!< Beginning detected, parsing it */
    HTTP_SSI_STATE_TAG = 0x02,                  /*!< Parsing TAG value */
    HTTP_SSI_STATE_END = 0x03,                  /*!< Parsing end of TAG */
} http_ssi_state_t;

/**
 * \brief           HTTP file system table structure of static files in device memory
 */
typedef struct {
    const char* path;                           /*!< File path, ex. "/index.html" */
    const void* data;                           /*!< Pointer to file data */
    uint32_t size;                              /*!< Size of file in units of bytes */
} http_fs_file_table_t;

/**
 * \brief           HTTP response file structure
 */
typedef struct http_fs_file {
    const uint8_t* data;                        /*!< Pointer to data array in case file is static */
    uint8_t is_static;                          /*!< Flag indicating file is static and no dynamic read is required */

    uint32_t size;                              /*!< Total length of file */
    uint32_t fptr;                              /*!< File pointer to indicate next read position */

    const uint16_t* rem_open_files;             /*!< Pointer to number of remaining open files.
                                                        User can use value on this pointer to get number of other opened files */
    void* arg;                                  /*!< User custom argument, may be used for user specific file system object */
} http_fs_file_t;

/**
 * \brief           HTTP state structure
 */
typedef struct http_state {
    lwesp_conn_p conn;                          /*!< Connection handle */
    lwesp_pbuf_p p;                             /*!< Header received pbuf chain */

    size_t conn_mem_available;                  /*!< Available memory in connection send queue */
    uint32_t written_total;                     /*!< Total number of bytes written into send buffer */
    uint32_t sent_total;                        /*!< Number of bytes we already sent */

    http_req_method_t req_method;               /*!< Used request method */
    uint8_t headers_received;                   /*!< Did we fully received a headers? */
    uint8_t process_resp;                       /*!< Process with response flag */

#if HTTP_SUPPORT_POST || __DOXYGEN__
    uint32_t content_length;                    /*!< Total expected content length for request (on POST) (without headers) */
    uint32_t content_received;                  /*!< Content length received so far (POST request, without headers) */
#endif /* HTTP_SUPPORT_POST || __DOXYGEN__ */

    http_fs_file_t rlwesp_file;                 /*!< Response file structure */
    uint8_t rlwesp_file_opened;                 /*!< Status if response file is opened and ready */
    const uint8_t* buff;                        /*!< Buffer pointer with data */
    uint32_t buff_len;                          /*!< Total length of buffer */
    uint32_t buff_ptr;                          /*!< Current buffer pointer */

    void* arg;                                  /*!< User optional argument */

#if HTTP_DYNAMIC_HEADERS || __DOXYGEN__
    const char* dyn_hdr_strs[HTTP_MAX_HEADERS]; /*!< Pointer to constant strings for dynamic header outputs */
    size_t dyn_hdr_idx;                         /*!< Current header for processing on output */
    size_t dyn_hdr_pos;                         /*!< Current position in current index for output */
#if HTTP_DYNAMIC_HEADERS_CONTENT_LEN || __DOXYGEN__
    char dyn_hdr_cnt_len[30];                   /*!< Content length header response: "Content-Length: 0123456789\r\n" */
#endif /* HTTP_DYNAMIC_HEADERS_CONTENT_LEN || __DOXYGEN__ */
#endif /* HTTP_DYNAMIC_HEADERS || __DOXYGEN__ */

    /* SSI tag parsing */
    uint8_t is_ssi;                             /*!< Flag if current request is SSI enabled */
    http_ssi_state_t ssi_state;                 /*!< Current SSI state when parsing SSI tags */
    char ssi_tag_buff[HTTP_SSI_TAG_START_LEN + HTTP_SSI_TAG_END_LEN + HTTP_SSI_TAG_MAX_LEN + 1];/*!< Temporary buffer for SSI tag storing */
    size_t ssi_tag_buff_ptr;                    /*!< Current write pointer */
    size_t ssi_tag_buff_written;                /*!< Number of bytes written so far to output buffer in case tag is not valid */
    size_t ssi_tag_len;                         /*!< Length of SSI tag */
    size_t ssi_tag_process_more;                /*!< Set to `1` when we have to process tag multiple times */
} http_state_t;

/**
 * \brief           Write string to HTTP server output
 * \note            May only be called from SSI callback function
 * \param[in]       hs: HTTP handle
 * \param[in]       str: String to write
 * \return          Number of bytes written to output
 * \sa              lwesp_http_server_write
 */
#define     lwesp_http_server_write_string(hs, str)   lwesp_http_server_write(hs, str, strlen(str))

lwespr_t    lwesp_http_server_init(const http_init_t* init, lwesp_port_t port);
size_t      lwesp_http_server_write(http_state_t* hs, const void* data, size_t len);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_HTTP_SERVER_H */
