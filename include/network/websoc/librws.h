/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

/**
 * @file librws.h
 * websocket API header file.
 *
 * @version   V1.0
 * @date      2019-11-08
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __LIBRWS_H__
#define __LIBRWS_H__

/** @addtogroup aos_websocket websocket
 *  Interfaces of websocket
 *
 *  @{
 */

#include <stdio.h>
#include <stdbool.h>

#define RWS_VERSION_MAJOR 1
#define RWS_VERSION_MINOR 2
#define RWS_VERSION_PATCH 4

/* check windows */
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define RWS_OS_WINDOWS 1
#endif

/* extern */
#if defined(__cplusplus) || defined(_cplusplus)
#define RWS_EXTERN extern "C"
#else
#define RWS_EXTERN extern
#endif

/* attribute */
#if defined(__GNUC__)
#if (__GNUC__ >= 4)
#if defined(__cplusplus) || defined(_cplusplus)
#define RWS_ATTRIB __attribute__((visibility("default")))
#else /* defined(__cplusplus) || defined(_cplusplus) */
#define RWS_ATTRIB __attribute__((visibility("default")))
#endif /* defined(__cplusplus) || defined(_cplusplus) */
#endif /* (__GNUC__ >= 4) */
#endif /* defined(__GNUC__) */


/* check attrib and define empty if not defined */
#if !defined(RWS_ATTRIB)
#define RWS_ATTRIB
#endif

/* dll api */
#if defined(RWS_OS_WINDOWS)
#if defined(RWS_BUILD)
#define RWS_DYLIB_API __declspec(dllexport)
#else /* defined(RWS_BUILD) */
#define RWS_DYLIB_API __declspec(dllimport)
#endif /* defined(RWS_BUILD) */
#endif /* defined(RWS_OS_WINDOWS) */

/* check dll api and define empty if not defined */
#if !defined(RWS_DYLIB_API)
#define RWS_DYLIB_API
#endif

/* combined lib api */
#define RWS_API(return_type) RWS_EXTERN RWS_ATTRIB RWS_DYLIB_API return_type

/* types */

/**
 * Boolean type as unsigned byte type.
 */
typedef unsigned char rws_bool;
#define rws_true 1
#define rws_false 0

/**
 * Macro for Wait forever
 */
#define RWS_WAIT_FOREVER 0xffffffffu

/**
 * Type of all public objects.
 */
typedef void* rws_handle;

/**
 * Socket handle.
 */
typedef struct rws_socket_struct * rws_socket;

/**
 * Error object handle.
 */
typedef struct rws_error_struct * rws_error;

/**
 * Mutex object handle.
 */
typedef rws_handle rws_mutex;

/**
 * Semphore object handle.
 */
typedef rws_handle rws_sem;

/**
 * Thread object handle.
 */
typedef struct rws_thread_struct * rws_thread;


/**
 * Callback type of thread function.
 *
 * @param[in]  user_object  User object provided during thread creation.
 *
 * @return none
 */
typedef void (*rws_thread_funct)(void * user_object);


/**
 * Callback type of socket object.
 *
 * @param[in]  socket  Socket object.
 *
 * @return none
 */
typedef void (*rws_on_socket)(rws_socket socket);


/**
 * Callback type on socket receive text frame.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  text    Pointer to reseived text.
 * @param[in]  length  Received text lenght without null terminated char.
 *
 * @return none
 */
typedef void (*rws_on_socket_recvd_text)(rws_socket socket, const char * text, const unsigned int length, bool is_finished);


/**
 * Callback type on socket receive binary frame.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  data    Received binary data.
 * @param[in]  length  Received binary data lenght.
 *
 * @return none
 */
typedef void (*rws_on_socket_recvd_bin)(rws_socket socket, const void * data, const unsigned int length, bool is_finished);


/**
 * Callback type on socket receive pong.
 *
 * @param[in]  socket  Socket object.
 *
 * @return none
 */
typedef void (*rws_on_socket_recvd_pong)(rws_socket socket);


/**
 * Callback type on socket send ping.
 *
 * @param[in]  socket  Socket object.
 *
 * @return none
 */
typedef void (*rws_on_socket_send_ping)(rws_socket socket);

/* socket */

/**
 * @brief Create new socket.
 *
 * @return Socket handler or NULL on error.
 */
RWS_API(rws_socket) rws_socket_create(void);


/**
 * Set socket connect URL.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  scheme  Connect URL scheme, "http" or "ws"
 * @param[in]  host    Connect URL host, "echo.websocket.org"
 * @param[in]  port    Connect URL port.
 * @param[in]  path    Connect URL path started with '/' character, "/" - for empty, "/path"
 *
 * @return none
 *
 * @code
 * rws_socket_set_url(socket, "http", "echo.websocket.org", 80, "/");
 * rws_socket_set_url(socket, "ws", "echo.websocket.org", 80, "/");
 * @endcode
 */
RWS_API(void) rws_socket_set_url(rws_socket socket, const char * scheme,
                                 const char * host, const int port,
                                 const char * path);

/**
 * Set socket connect URL scheme string.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  scheme  Connect URL scheme, "http" or "ws"
 *
 * @return none
 *
 * @code
 * rws_socket_set_scheme(socket, "http");
 * rws_socket_set_scheme(socket, "ws");
 * @endcode
 */
RWS_API(void) rws_socket_set_scheme(rws_socket socket, const char * scheme);


/**
 * Get socket connect URL scheme string.
 *
 * @param[in]  socket  Socket object.
 *
 * @return Connect URL cheme or null.
 */
RWS_API(const char *) rws_socket_get_scheme(rws_socket socket);


/**
 * Set socket connect URL scheme string.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  scheme  Connect URL host, "echo.websocket.org"
 *
 * @return none
 *
 * @code
 * rws_socket_set_host(socket, "echo.websocket.org");
 * @endcode
 */
RWS_API(void) rws_socket_set_host(rws_socket socket, const char * host);


/**
 * Get socket connect URL host string.
 *
 * @param[in]  socket  Socket object.
 *
 * @return Connect URL host or null.
 */
RWS_API(const char *) rws_socket_get_host(rws_socket socket);


/**
 * Set socket connect URL port.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  scheme  Connect URL port.
 *
 * @return none
 *
 * @code
 * rws_socket_set_port(socket, 80);
 * @endcode
 */
RWS_API(void) rws_socket_set_port(rws_socket socket, const int port);


/**
 * Get socket connect URL port.
 *
 * @param[in]  socket  Socket object.
 *
 * @return Connect URL port or 0.
 */
RWS_API(int) rws_socket_get_port(rws_socket socket);


/**
 * Set socket connect URL path string.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  scheme  Connect URL path started with '/' character, "/" - for empty, "/path"
 *
 * @return none
 *
 * @code
 * rws_socket_set_path(socket, "/"); // empty path
 * rws_socket_set_path(socket, "/path"); // some path
 * @endcode
 */
RWS_API(void) rws_socket_set_path(rws_socket socket, const char * path);


/**
 * Get socket connect URL path string.
 *
 * @param[in]  socket  Socket object.
 *
 * @return Connect URL path or null.
 */
RWS_API(const char *) rws_socket_get_path(rws_socket socket);


/**
 * Set socket connect Sec-WebSocket-Protocol field.
 * @param socket Socket object.
 * @param Sec-WebSocket-Protocol.
 *
 * @code
 * rws_socket_set_protocol(socket, "echo-protocol")
 * rws_socket_set_protocol(socket, "chat, superchat")
 * @endcode
 *
 * @return none
 */
RWS_API(void) rws_socket_set_protocol(rws_socket socket, const char * protocol);


#ifdef WEBSOCKET_SSL_ENABLE
/**
 * Set server certificate for ssl connection.
 *
 * @param[in]  socket           Socket object.
 * @param[in]  server_cert      Server certificate.
 * @param[in]  server_cert_len  The length of the server certificate caculated by sizeof
 *
 * @return none
 */
RWS_API(void) rws_socket_set_server_cert(rws_socket socket, const char *server_cert, int server_cert_len);
#endif /* WEBSOCKET_SSL_ENABLE */

/**
 * Start connection.
 *
 * @note: This method can generate error object.
 *
 * @param[in]  socket  Socket object.
 *
 * @return rws_true - all params exists and connection started, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_connect(rws_socket socket);


/**
 * Disconnect socket.
 *
 * @note: Cleanup prev. send messages and start disconnection sequence.
 *        SHOULD forget about this socket handle and don't use it anymore.
 *        Don't use this socket object handler after this command.
 *
 * @param[in]  socket  Socket object.
 */
RWS_API(void) rws_socket_disconnect_and_release(rws_socket socket);


/**
 * Check is socket has connection to host and handshake(sucessfully done).
 *
 * @note: Thread safe getter.
 *
 * @param[in]  socket  Socket object.
 *
 * @return rws_true - connected to host and handshacked, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_is_connected(rws_socket socket);


/**
 * Set socket user defined object pointer for identificating socket object.
 *
 * @param[in]  socket       Socket object.
 * @param[in]  user_object  Void pointer to user object.
 *
 * @return none.
 */
RWS_API(void) rws_socket_set_user_object(rws_socket socket, void * user_object);


/**
 * Get socket user defined object.
 *
 * @param[in]  socket  Socket object.
 *
 * @return User defined object pointer or null.
 */
RWS_API(void *) rws_socket_get_user_object(rws_socket socket);


/**
 * Set the callback handler for the connection event.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_connected(rws_socket socket, rws_on_socket callback);


/**
 * Set the callback handler for the disconnection event.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_disconnected(rws_socket socket, rws_on_socket callback);


/**
 * Set the callback handler when text received.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_received_text(rws_socket socket, rws_on_socket_recvd_text callback);


/**
 * Set the callback handler when bin data received.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_received_bin(rws_socket socket, rws_on_socket_recvd_bin callback);


/**
 * Set the callback handler when pong data received.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_received_pong(rws_socket socket, rws_on_socket_recvd_pong callback);


/**
 * Set the callback handler when ping sent.
 *
 * @param[in]  socket    The socket object.
 * @param[in]  callback  The handler function.
 *
 * @return None.
 */
RWS_API(void) rws_socket_set_on_send_ping(rws_socket socket, rws_on_socket_send_ping callback);

/**
 * Send text to connect socket.
 *
 * @note: Thread safe method.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  text    Text string for sending.
 *
 * @return rws_true - socket and text exists and placed to send queue, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_send_text(rws_socket socket, const char * text);


/**
 * Send bin header to connect socket.
 *
 * @detailed Thread safe method.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  bin     header string for sending.
 * @param[in]  len     length of header, 8 bytes.
 *
 * @return rws_true - socket and bin header exists and placed to send queue, otherwice rws_false
 */
RWS_API(rws_bool) rws_socket_send_bin_start(rws_socket socket, const char *bin, size_t len);


/**
 * Send bin content to connect socket.
 *
 * @detailed Thread safe method.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  binary  content for sending.
 * @param[in]  len     length of binary content.
 *
 * @return rws_true - socket and bin content exists and placed to send queue, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_send_bin_continue(rws_socket socket, const char *bin, size_t len);


/**
 * Send final bin to connect socket.
 *
 * @detailed Thread safe method.
 *
 * @param[in]  socket  Socket object.
 * @param[in]  final   bin string for sending.
 * @param[in]  len     length of final, 6 bytes.
 *
 * @return rws_true - socket and final bin exists and placed to send queue, otherwice rws_false
 */
RWS_API(rws_bool) rws_socket_send_bin_finish(rws_socket socket, const char * bin, size_t len);

/**
 * Send ping async to connect socket.
 *
 * @detailed Thread safe method.
 *
 * @param[in]  socket  Socket object.
 *
 * @return rws_true - send ping success, otherwice rws_false
 */
RWS_API(rws_bool) rws_socket_send_ping(rws_socket socket);


/* error code type */
typedef enum _rws_error_code {
	rws_error_code_none = 0,
	rws_error_code_missed_parameter,
	rws_error_code_send_handshake,
	rws_error_code_parse_handshake,
	rws_error_code_read_write_socket,
	rws_error_code_connect_to_host,
	/**
	 * @brief Connection was closed by endpoint.
	 * Reasons: an endpoint shutting down, an endpoint having received a frame too large, or an
	 * endpoint having received a frame that does not conform to the format expected by the endpoint.
	 */
	rws_error_code_connection_closed,
} rws_error_code;


/**
 * Get socket last error object handle.
 *
 * @param[in]  socket  Socket object.
 *
 * @return Last error object handle or null if no error.
 */
RWS_API(rws_error) rws_socket_get_error(rws_socket socket);

/**
 * Get the code of the error object.
 *
 * @param[in]  error  The error.
 *
 * @return 0 - if error is empty or no error, otherwice error code.
 */
RWS_API(int) rws_error_get_code(rws_error error);


/**
 * Get the code of the http error object.
 *
 * @param[in]  error  The error.
 *
 * @return 0 - if error is empty or no error, otherwice HTTP error.
 */
RWS_API(int) rws_error_get_http_error(rws_error error);


/**
 * Get the description of the error object.
 *
 * @param[in]  error  The error.
 *
 * @return The pointer of the description string.
 */
RWS_API(const char *) rws_error_get_description(rws_error error);


/* mutex */

/**
 * Creates recursive mutex object.
 *
 * @return The mutex object.
 */
RWS_API(rws_mutex) rws_mutex_create_recursive(void);


/**
 * Lock mutex object.
 *
 * @param[in]  mutex  The mutex object.
 *
 * @return None.
 */
RWS_API(void) rws_mutex_lock(rws_mutex mutex);


/**
 * Unlock mutex object.
 *
 * @param[in]  mutex  The mutex object.
 *
 * @return None.
 */
RWS_API(void) rws_mutex_unlock(rws_mutex mutex);


/**
 * Delete mutex object.
 *
 * @param[in]  mutex  The mutex object.
 *
 * @return None.
 */
RWS_API(void) rws_mutex_delete(rws_mutex mutex);


/* thread */

/**
 * Create thread object that start immidiatelly.
 *
 * @param[in]  thread_function  The function to be executed in the thread.
 * @param[in]  user_object      The argument of the thread function.
 *
 * @return The thrad object.
 */
RWS_API(rws_thread) rws_thread_create(rws_thread_funct thread_function, void * user_object);


/* semphore */

/**
 * Creates sem object.
 *
 * @return sem object.
 */
RWS_API(rws_sem) rws_sem_create(void);

/**
 * Release a semhpore
 *
 * @param[in] sem sem object
 *
 * @return none
 */
RWS_API(void) rws_sem_signal(rws_sem sem);

/**
 * Wait a semhpore
 *
 * @param[in] sem     sem object
 * @param[in] timeout timeout in millisecond
 *
 * @return 0 on success, other on failure
 */
RWS_API(int) rws_sem_wait(rws_sem sem, unsigned int timeout);

/**
 * Release a semhpore
 *
 * @param[in] sem sem object
 *
 * @return none
 */
RWS_API(void) rws_sem_delete(rws_sem sem);

/**
 * Pause current thread for a number of milliseconds.
 *
 * param[in]  millisec  The number of milliseconds.
 *
 * @return None.
 */
RWS_API(void) rws_thread_sleep(const unsigned int millisec);

/** @} */

#endif /* __LIBRWS_H__ */
