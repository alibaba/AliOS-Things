// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This is a modified sh2lib, for better integration with LVGL Micropython bindings.
// To prevent including the entire nghttp2 header, only selected enums where copied here.

#ifndef __ESP_EXAMPLE_SH2_LIB_H_
#define __ESP_EXAMPLE_SH2_LIB_H_

#if __has_include("esp_idf_version.h")
#   include "esp_idf_version.h"
#endif
#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 4

typedef struct nghttp2_session nghttp2_session;
typedef struct esp_tls esp_tls;
typedef struct esp_tls_cfg esp_tls_cfg;

/*
 * This is a thin API wrapper over nghttp2 that offers simplified APIs for usage
 * in the application. The intention of this wrapper is to act as a stepping
 * stone to quickly get started with using the HTTP/2 client. Since the focus is
 * on simplicity, not all the features of HTTP/2 are supported through this
 * wrapper. Once you are fairly comfortable with nghttp2, feel free to directly
 * use nghttp2 APIs or make changes to sh2lib.c for realising your objectives.
 *
 * TODO:
 * - Allowing to query response code, content-type etc in the receive callback
 * - A simple function for per-stream header callback
 */


/**
 * @enum
 *
 * Error codes used in this library.  The code range is [-999, -500],
 * inclusive. The following values are defined:
 */
typedef enum {
  /**
   * Invalid argument passed.
   */
  SH2LIB_ERR_INVALID_ARGUMENT = -501,
  /**
   * Out of buffer space.
   */
  SH2LIB_ERR_BUFFER_ERROR = -502,
  /**
   * The specified protocol version is not supported.
   */
  SH2LIB_ERR_UNSUPPORTED_VERSION = -503,
  /**
   * Used as a return value from :type:`nghttp2_send_callback`,
   * :type:`nghttp2_recv_callback` and
   * :type:`nghttp2_send_data_callback` to indicate that the operation
   * would block.
   */
  SH2LIB_ERR_WOULDBLOCK = -504,
  /**
   * General protocol error
   */
  SH2LIB_ERR_PROTO = -505,
  /**
   * The frame is invalid.
   */
  SH2LIB_ERR_INVALID_FRAME = -506,
  /**
   * The peer performed a shutdown on the connection.
   */
  SH2LIB_ERR_EOF = -507,
  /**
   * Used as a return value from
   * :func:`nghttp2_data_source_read_callback` to indicate that data
   * transfer is postponed.  See
   * :func:`nghttp2_data_source_read_callback` for details.
   */
  SH2LIB_ERR_DEFERRED = -508,
  /**
   * Stream ID has reached the maximum value.  Therefore no stream ID
   * is available.
   */
  SH2LIB_ERR_STREAM_ID_NOT_AVAILABLE = -509,
  /**
   * The stream is already closed; or the stream ID is invalid.
   */
  SH2LIB_ERR_STREAM_CLOSED = -510,
  /**
   * RST_STREAM has been added to the outbound queue.  The stream is
   * in closing state.
   */
  SH2LIB_ERR_STREAM_CLOSING = -511,
  /**
   * The transmission is not allowed for this stream (e.g., a frame
   * with END_STREAM flag set has already sent).
   */
  SH2LIB_ERR_STREAM_SHUT_WR = -512,
  /**
   * The stream ID is invalid.
   */
  SH2LIB_ERR_INVALID_STREAM_ID = -513,
  /**
   * The state of the stream is not valid (e.g., DATA cannot be sent
   * to the stream if response HEADERS has not been sent).
   */
  SH2LIB_ERR_INVALID_STREAM_STATE = -514,
  /**
   * Another DATA frame has already been deferred.
   */
  SH2LIB_ERR_DEFERRED_DATA_EXIST = -515,
  /**
   * Starting new stream is not allowed (e.g., GOAWAY has been sent
   * and/or received).
   */
  SH2LIB_ERR_START_STREAM_NOT_ALLOWED = -516,
  /**
   * GOAWAY has already been sent.
   */
  SH2LIB_ERR_GOAWAY_ALREADY_SENT = -517,
  /**
   * The received frame contains the invalid header block (e.g., There
   * are duplicate header names; or the header names are not encoded
   * in US-ASCII character set and not lower cased; or the header name
   * is zero-length string; or the header value contains multiple
   * in-sequence NUL bytes).
   */
  SH2LIB_ERR_INVALID_HEADER_BLOCK = -518,
  /**
   * Indicates that the context is not suitable to perform the
   * requested operation.
   */
  SH2LIB_ERR_INVALID_STATE = -519,
  /**
   * The user callback function failed due to the temporal error.
   */
  SH2LIB_ERR_TEMPORAL_CALLBACK_FAILURE = -521,
  /**
   * The length of the frame is invalid, either too large or too small.
   */
  SH2LIB_ERR_FRAME_SIZE_ERROR = -522,
  /**
   * Header block inflate/deflate error.
   */
  SH2LIB_ERR_HEADER_COMP = -523,
  /**
   * Flow control error
   */
  SH2LIB_ERR_FLOW_CONTROL = -524,
  /**
   * Insufficient buffer size given to function.
   */
  SH2LIB_ERR_INSUFF_BUFSIZE = -525,
  /**
   * Callback was paused by the application
   */
  SH2LIB_ERR_PAUSE = -526,
  /**
   * There are too many in-flight SETTING frame and no more
   * transmission of SETTINGS is allowed.
   */
  SH2LIB_ERR_TOO_MANY_INFLIGHT_SETTINGS = -527,
  /**
   * The server push is disabled.
   */
  SH2LIB_ERR_PUSH_DISABLED = -528,
  /**
   * DATA or HEADERS frame for a given stream has been already
   * submitted and has not been fully processed yet.  Application
   * should wait for the transmission of the previously submitted
   * frame before submitting another.
   */
  SH2LIB_ERR_DATA_EXIST = -529,
  /**
   * The current session is closing due to a connection error or
   * `nghttp2_session_terminate_session()` is called.
   */
  SH2LIB_ERR_SESSION_CLOSING = -530,
  /**
   * Invalid HTTP header field was received and stream is going to be
   * closed.
   */
  SH2LIB_ERR_HTTP_HEADER = -531,
  /**
   * Violation in HTTP messaging rule.
   */
  SH2LIB_ERR_HTTP_MESSAGING = -532,
  /**
   * Stream was refused.
   */
  SH2LIB_ERR_REFUSED_STREAM = -533,
  /**
   * Unexpected internal error, but recovered.
   */
  SH2LIB_ERR_INTERNAL = -534,
  /**
   * Indicates that a processing was canceled.
   */
  SH2LIB_ERR_CANCEL = -535,
  /**
   * The errors < :enum:`SH2LIB_ERR_FATAL` mean that the library is
   * under unexpected condition and processing was terminated (e.g.,
   * out of memory).  If application receives this error code, it must
   * stop using that :type:`nghttp2_session` object and only allowed
   * operation for that object is deallocate it using
   * `nghttp2_session_del()`.
   */
  SH2LIB_ERR_FATAL = -900,
  /**
   * Out of memory.  This is a fatal error.
   */
  SH2LIB_ERR_NOMEM = -901,
  /**
   * The user callback function failed.  This is a fatal error.
   */
  SH2LIB_ERR_CALLBACK_FAILURE = -902,
  /**
   * Invalid client magic (see :macro:`SH2LIB_CLIENT_MAGIC`) was
   * received and further processing is not possible.
   */
  SH2LIB_ERR_BAD_CLIENT_MAGIC = -903,
  /**
   * Possible flooding by peer was detected in this HTTP/2 session.
   * Flooding is measured by how many PING and SETTINGS frames with
   * ACK flag set are queued for transmission.  These frames are
   * response for the peer initiated frames, and peer can cause memory
   * exhaustion on server side to send these frames forever and does
   * not read network.
   */
  SH2LIB_ERR_FLOODED = -904
} sh2lib_error;

/**
 * @enum
 *
 * The flags for header field name/value pair.
 */
typedef enum {
  /**
   * No flag set.
   */
  SH2LIB_NV_FLAG_NONE = 0,
  /**
   * Indicates that this name/value pair must not be indexed ("Literal
   * Header Field never Indexed" representation must be used in HPACK
   * encoding).  Other implementation calls this bit as "sensitive".
   */
  SH2LIB_NV_FLAG_NO_INDEX = 0x01,
  /**
   * This flag is set solely by application.  If this flag is set, the
   * library does not make a copy of header field name.  This could
   * improve performance.
   */
  SH2LIB_NV_FLAG_NO_COPY_NAME = 0x02,
  /**
   * This flag is set solely by application.  If this flag is set, the
   * library does not make a copy of header field value.  This could
   * improve performance.
   */
  SH2LIB_NV_FLAG_NO_COPY_VALUE = 0x04
} sh2lib_nv_flag;

/**
 * @enum
 *
 * The flags used to set in |data_flags| output parameter in
 * :type:`nghttp2_data_source_read_callback`.
 */
typedef enum {
  /**
   * No flag set.
   */
  SH2LIB_DATA_FLAG_NONE = 0,
  /**
   * Indicates EOF was sensed.
   */
  SH2LIB_DATA_FLAG_EOF = 0x01,
  /**
   * Indicates that END_STREAM flag must not be set even if
   * SH2LIB_DATA_FLAG_EOF is set.  Usually this flag is used to send
   * trailer fields with `nghttp2_submit_request()` or
   * `nghttp2_submit_response()`.
   */
  SH2LIB_DATA_FLAG_NO_END_STREAM = 0x02,
  /**
   * Indicates that application will send complete DATA frame in
   * :type:`nghttp2_send_data_callback`.
   */
  SH2LIB_DATA_FLAG_NO_COPY = 0x04
} sh2lib_data_flag;

/**
 * @brief Flag returned by data_recv_cb to indicate recieve status
 */
typedef enum {
    SH2LIB_DATA_RECV_NONE,              
    SH2LIB_DATA_RECV_RST_STREAM,        /*!< Flag indicating receive stream is reset */
    SH2LIB_DATA_RECV_FRAME_COMPLETE     /*!< Flag indicating frame is completely received */
} sh2lib_data_recv_flag;

/**
 * @brief Handle for working with sh2lib APIs
 */
struct sh2lib_handle {
    nghttp2_session     *http2_sess;                /*!< Pointer to the HTTP2 session handle */
    char                *hostname;                  /*!< The hostname we are connected to */
    struct esp_tls      *http2_tls;                 /*!< Pointer to the TLS session handle */
    struct esp_tls_cfg  *http2_tls_cfg;             /*!< Pointer to the TLS session configuration */
    void                *user_data;                 /*!< Needed for Micropython binding */
    TaskHandle_t        connect_task_handle;        /*!< Connection task handle */
    int                 connect_result;             /*!< Current result of the connection task */
};

/**
 * @brief Wrapper for nghttp2_nv
 */
struct sh2lib_nv {
    const char *name;
    const char *value;
    uint8_t flags;
};



/**
 * @brief Function Prototype for data receive callback
 *
 * This function gets called whenever data is received on any stream. The
 * function is also called for indicating events like frame receive complete, or
 * end of stream. The function may get called multiple times as long as data is
 * received on the stream.
 *
 * @param[in] handle     Pointer to the sh2lib handle.
 * @param[in] data       Pointer to a buffer that contains the data received.
 * @param[in] len        The length of valid data stored at the 'data' pointer.
 * @param[in] flags      Flags indicating whether the stream is reset (DATA_RECV_RST_STREAM) or
 *                       this particularly frame is completely received
 *                       DATA_RECV_FRAME_COMPLETE).
 *
 * @return The function should return 0
 */
typedef int (*sh2lib_frame_data_recv_cb_t)(struct sh2lib_handle *handle, const void *data, size_t len, int flags);

/**
 * @brief Function Prototype for callback to send data in PUT/POST
 *
 * This function gets called whenever nghttp2 wishes to send data, like for
 * PUT/POST requests to the server. The function keeps getting called until this
 * function sets the flag SH2LIB_DATA_FLAG_EOF to indicate end of data.
 *
 * @param[in] handle       Pointer to the sh2lib handle.
 * @param[out] data        Pointer to a buffer that should contain the data to send.
 * @param[in] len          The maximum length of data that can be sent out by this function.
 * @param[out] data_flags  Pointer to the data flags. The SH2LIB_DATA_FLAG_EOF
 *                         should be set in the data flags to indicate end of new data.
 *
 * @return The function should return the number of valid bytes stored in the
 * data pointer
 */
typedef int (*sh2lib_putpost_data_cb_t)(struct sh2lib_handle *handle, void *data, size_t len, uint32_t *data_flags);

/**
 * @brief Connect to a URI using HTTP/2
 *
 * This API opens an HTTP/2 connection with the provided URI. If successful, the
 * hd pointer is populated with a valid handle for subsequent communication.
 *
 * Only 'https' URIs are supported.
 *
 * @param[out] hd       Pointer to a variable of the type 'struct sh2lib_handle'.
 *                      This struct is expected to be zeroed before passed as an argument!
 * @param[in]  uri      Pointer to the URI that should be connected to.
 *
 * @return
 *             - ESP_OK if the connection was successful
 *             - ESP_FAIL if the connection fails
 */
int sh2lib_connect(struct sh2lib_handle *hd, const char *uri);

/**
 * @brief Connect to a URI using HTTP/2 concurrently on a different task
 *
 * This API opens an HTTP/2 connection with the provided URI. If successful, the
 * hd pointer is populated with a valid handle for subsequent communication.
 *
 * The connection task is created by this function, and deletes itself when done.
 * The connection task updates hd->connect_result to indicate connection status.
 * The caller should poll hd->connect_result
 * hd->connect_result indicates: 0 in progress, 1 succeeded, -1 failed.
 *
 * Only 'https' URIs are supported.
 *
 * @param[out] hd       Pointer to a variable of the type 'struct sh2lib_handle'.
 *                      This struct is expected to be zeroed before passed as an argument!
 * @param[in]  uri      Pointer to the URI that should be connected to.
 * @param[in]  priority Connection task priority
 * @param[in]  core_id  Connection task core id
 *
 * @return
 *             - ESP_OK if the connection was successful
 *             - ESP_FAIL if the connection fails
 */
int sh2lib_connect_task(struct sh2lib_handle *hd, const char *uri, int priority, int core_id);

/**
 * @brief Async connect to a URI using HTTP/2
 *
 * This API opens a non blocking HTTP/2 connection with the provided URI. If successful, the
 * hd pointer is populated with a valid handle for subsequent communication.
 * The user is expected to call this function repeatedly until it does not return 0.
 *
 * Only 'https' URIs are supported.
 *
 * @param[out] hd      Pointer to a variable of the type 'struct sh2lib_handle'.
 *                     This struct is expected to be zeroed before passed as an argument for the first time! 
 *                     When this function is called repeatedly, pass the same struct without change.
 * @param[in]  uri     Pointer to the URI that should be connected to.
 *
 * @return
 *             - -1     If connection establishment fails.
 *             -  0     If connection establishment is in progress.
 *             -  1     If connection establishment is successful.
 */
int sh2lib_connect_async(struct sh2lib_handle *hd, const char *uri);

/**
 * @brief Free a sh2lib handle
 *
 * This API frees-up an sh2lib handle, thus closing any open connections that
 * may be associated with this handle, and freeing up any resources.
 *
 * @param[in] hd      Pointer to a variable of the type 'struct sh2lib_handle'.
 *
 */
void sh2lib_free(struct sh2lib_handle *hd);

/**
 * @brief Setup an HTTP GET request stream
 *
 * This API sets up an HTTP GET request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP GET operation on (for example, /users).
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_get(struct sh2lib_handle *hd, const char *path, sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP POST request stream
 *
 * This API sets up an HTTP POST request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP POST operation on (for example, /users).
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_post(struct sh2lib_handle *hd, const char *path,
                   sh2lib_putpost_data_cb_t send_cb,
                   sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP PUT request stream
 *
 * This API sets up an HTTP PUT request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP PUT operation on (for example, /users).
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_put(struct sh2lib_handle *hd, const char *path,
                  sh2lib_putpost_data_cb_t send_cb,
                  sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Execute send/receive on an HTTP/2 connection
 *
 * While the API sh2lib_do_get(), sh2lib_do_post() setup the requests to be
 * initiated with the server, this API performs the actual data send/receive
 * operations on the HTTP/2 connection. The callback functions are accordingly
 * called during the processing of these requests.
 *
 * @param[in] hd      Pointer to a variable of the type 'struct sh2lib_handle'
 *
 * @return
 *             - ESP_OK if the connection was successful
 *             - ESP_FAIL if the connection fails
 */
int sh2lib_execute(struct sh2lib_handle *hd);

#define SH2LIB_MAKE_NV(NAME, VALUE)                                    \
  {                                                                    \
    NAME, VALUE, SH2LIB_NV_FLAG_NONE                                  \
  }

/**
 * @brief Setup an HTTP GET request stream with custom name-value pairs
 *
 * For a simpler version of the API, please refer to sh2lib_do_get().
 *
 * This API sets up an HTTP GET request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * Please note that the following name value pairs MUST be a part of the request
 *     -  name:value
 *     -  ":method":"GET"
 *     -  ":scheme":"https"
 *     -  ":path":<the-path-for-the-GET-operation>  (for example, /users)
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] nva       An array of name-value pairs that should be part of the request.
 * @param[in] nvlen     The number of elements in the array pointed to by 'nva'.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_get_with_nv(struct sh2lib_handle *hd, const struct sh2lib_nv nva[], size_t nvlen, sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP PUT/POST request stream with custom name-value pairs
 *
 * For a simpler version of the API, please refer to sh2lib_do_put() or
 * sh2lib_do_post().
 *
 * This API sets up an HTTP PUT/POST request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * Please note that the following name value pairs MUST be a part of the request
 *     -  name:value
 *     -  ":method":"PUT" (or POST)
 *     -  ":scheme":"https"
 *     -  ":path":<the-path-for-the-PUT-operation>  (for example, /users)
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] nva       An array of name-value pairs that should be part of the request.
 * @param[in] nvlen     The number of elements in the array pointed to by 'nva'.
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_putpost_with_nv(struct sh2lib_handle *hd, const struct sh2lib_nv nva[], size_t nvlen,
                              sh2lib_putpost_data_cb_t send_cb,
                              sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @function
 *
 * Puts back previously deferred DATA frame in the stream |stream_id|
 * to the outbound queue.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * :enum:`SH2LIB_ERR_INVALID_ARGUMENT`
 *     The stream does not exist; or no deferred data exist.
 * :enum:`SH2LIB_ERR_NOMEM`
 *     Out of memory.
 */
int sh2lib_session_resume_data(struct sh2lib_handle *hd, int32_t stream_id);


#endif // defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 4
#endif /* ! __ESP_EXAMPLE_SH2_LIB_H_ */

