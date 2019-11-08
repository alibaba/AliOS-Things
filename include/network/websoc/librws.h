/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __LIBRWS_H__
#define __LIBRWS_H__ 1

#include <stdio.h>
#include <stdbool.h>

#define RWS_VERSION_MAJOR 1
#define RWS_VERSION_MINOR 2
#define RWS_VERSION_PATCH 4

#if defined(WEBSOCKET_DEBUG)
#define TAG "websocket"
#define DBG(format, ...) LOGD(TAG, format, ##__VA_ARGS__)
#else
#define DBG(format, ...)
#endif

// check windows
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define RWS_OS_WINDOWS 1
#endif


// extern
#if defined(__cplusplus) || defined(_cplusplus)
#define RWS_EXTERN extern "C"
#else
#define RWS_EXTERN extern
#endif


// attribute
#if defined(__GNUC__)
#if (__GNUC__ >= 4)
#if defined(__cplusplus) || defined(_cplusplus)
#define RWS_ATTRIB __attribute__((visibility("default")))
#else
#define RWS_ATTRIB __attribute__((visibility("default")))
#endif
#endif
#endif


// check attrib and define empty if not defined
#if !defined(RWS_ATTRIB)
#define RWS_ATTRIB
#endif


// dll api
#if defined(RWS_OS_WINDOWS)
#if defined(RWS_BUILD)
#define RWS_DYLIB_API __declspec(dllexport)
#else
#define RWS_DYLIB_API __declspec(dllimport)
#endif
#endif


// check dll api and define empty if not defined
#if !defined(RWS_DYLIB_API)
#define RWS_DYLIB_API
#endif


// combined lib api
#define RWS_API(return_type) RWS_EXTERN RWS_ATTRIB RWS_DYLIB_API return_type


// types

/**
 @brief Boolean type as unsigned byte type.
 */
typedef unsigned char rws_bool;
#define rws_true 1
#define rws_false 0


/**
 @brief Type of all public objects.
 */
typedef void* rws_handle;


/**
 @brief Socket handle.
 */
typedef struct rws_socket_struct * rws_socket;


/**
 @brief Error object handle.
 */
typedef struct rws_error_struct * rws_error;


/**
 @brief Mutex object handle.
 */
typedef rws_handle rws_mutex;


/**
 @brief Thread object handle.
 */
typedef struct rws_thread_struct * rws_thread;


/**
 @brief Callback type of thread function.
 @param user_object User object provided during thread creation.
 */
typedef void (*rws_thread_funct)(void * user_object);


/**
 @brief Callback type of socket object.
 @param socket Socket object.
 */
typedef void (*rws_on_socket)(rws_socket socket);


/**
 @brief Callback type on socket receive text frame.
 @param socket Socket object.
 @param text Pointer to reseived text.
 @param length Received text lenght without null terminated char.
 */
typedef void (*rws_on_socket_recvd_text)(rws_socket socket, const char * text, const unsigned int length, bool is_finished);


/**
 @brief Callback type on socket receive binary frame.
 @param socket Socket object.
 @param data Received binary data.
 @param length Received binary data lenght.
 */
typedef void (*rws_on_socket_recvd_bin)(rws_socket socket, const void * data, const unsigned int length, bool is_finished);


/**
 @brief Callback type on socket receive pong.
 @param socket Socket object.
 */
typedef void (*rws_on_socket_recvd_pong)(rws_socket socket);

// socket

/**
 @brief Create new socket.
 @return Socket handler or NULL on error.
 */
RWS_API(rws_socket) rws_socket_create(void);


/**
 @brief Set socket connect URL.
 @param socket Socket object.
 @param scheme Connect URL scheme, "http" or "ws"
 @param scheme Connect URL host, "echo.websocket.org"
 @param scheme Connect URL port.
 @param scheme Connect URL path started with '/' character, "/" - for empty, "/path"
 @code
 rws_socket_set_url(socket, "http", "echo.websocket.org", 80, "/");
 rws_socket_set_url(socket, "ws", "echo.websocket.org", 80, "/");
 @endcode
 */
RWS_API(void) rws_socket_set_url(rws_socket socket,
								 const char * scheme,
								 const char * host,
								 const int port,
								 const char * path);

/**
 @brief Set socket connect URL scheme string.
 @param socket Socket object.
 @param scheme Connect URL scheme, "http" or "ws"
 @code
 rws_socket_set_scheme(socket, "http");
 rws_socket_set_scheme(socket, "ws");
 @endcode
 */
RWS_API(void) rws_socket_set_scheme(rws_socket socket, const char * scheme);


/**
 @brief Get socket connect URL scheme string.
 @param socket Socket object.
 @return Connect URL cheme or null.
 */
RWS_API(const char *) rws_socket_get_scheme(rws_socket socket);


/**
 @brief Set socket connect URL scheme string.
 @param socket Socket object.
 @param scheme Connect URL host, "echo.websocket.org"
 @code
 rws_socket_set_host(socket, "echo.websocket.org");
 @endcode
 */
RWS_API(void) rws_socket_set_host(rws_socket socket, const char * host);


/**
 @brief Get socket connect URL host string.
 @param socket Socket object.
 @return Connect URL host or null.
 */
RWS_API(const char *) rws_socket_get_host(rws_socket socket);


/**
 @brief Set socket connect URL port.
 @param socket Socket object.
 @param scheme Connect URL port.
 @code
 rws_socket_set_port(socket, 80);
 @endcode
 */
RWS_API(void) rws_socket_set_port(rws_socket socket, const int port);


/**
 @brief Get socket connect URL port.
 @param socket Socket object.
 @return Connect URL port or 0.
 */
RWS_API(int) rws_socket_get_port(rws_socket socket);


/**
 @brief Set socket connect URL path string.
 @param socket Socket object.
 @param scheme Connect URL path started with '/' character, "/" - for empty, "/path"
 @code
 rws_socket_set_path(socket, "/"); // empty path
 rws_socket_set_path(socket, "/path"); // some path
 @endcode
 */
RWS_API(void) rws_socket_set_path(rws_socket socket, const char * path);


/**
 @brief Get socket connect URL path string.
 @param socket Socket object.
 @return Connect URL path or null.
 */
RWS_API(const char *) rws_socket_get_path(rws_socket socket);

#ifdef WEBSOCKET_SSL_ENABLE
/**
 @brief Set server certificate for ssl connection.
 @param socket Socket object.
 @param server_cert Server certificate.
 @param server_cert_len The length of the server certificate caculated by sizeof
*/
RWS_API(void) rws_socket_set_server_cert(rws_socket socket, const char *server_cert, int server_cert_len);
#endif


/**
 @brief Get socket last error object handle.
 @param socket Socket object.
 @return Last error object handle or null if no error.
 */
RWS_API(rws_error) rws_socket_get_error(rws_socket socket);


/**
 @brief Start connection.
 @detailed This method can generate error object.
 @param socket Socket object.
 @return rws_true - all params exists and connection started, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_connect(rws_socket socket);


/**
 @brief Disconnect socket.
 @detailed Cleanup prev. send messages and start disconnection sequence.
 SHOULD forget about this socket handle and don't use it anymore.
 @warning Don't use this socket object handler after this command.
 @param socket Socket object.
 */
RWS_API(void) rws_socket_disconnect_and_release(rws_socket socket);


/**
 @brief Check is socket has connection to host and handshake(sucessfully done).
 @detailed Thread safe getter.
 @param socket Socket object.
 @return trw_true - connected to host and handshacked, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_is_connected(rws_socket socket);


/**
 @brief Send text to connect socket.
 @detailed Thread safe method.
 @param socket Socket object.
 @param text Text string for sending.
 @return rws_true - socket and text exists and placed to send queue, otherwice rws_false.
 */
RWS_API(rws_bool) rws_socket_send_text(rws_socket socket, const char * text);


/**
 @brief Set socket user defined object pointer for identificating socket object.
 @param socket Socket object.
 @param user_object Void pointer to user object.
 */
RWS_API(void) rws_socket_set_user_object(rws_socket socket, void * user_object);


/**
 @brief Get socket user defined object.
 @param socket Socket object.
 @return User defined object pointer or null.
 */
RWS_API(void *) rws_socket_get_user_object(rws_socket socket);


RWS_API(void) rws_socket_set_on_connected(rws_socket socket, rws_on_socket callback);


RWS_API(void) rws_socket_set_on_disconnected(rws_socket socket, rws_on_socket callback);


RWS_API(void) rws_socket_set_on_received_text(rws_socket socket, rws_on_socket_recvd_text callback);


RWS_API(void) rws_socket_set_on_received_bin(rws_socket socket, rws_on_socket_recvd_bin callback);

/**
 @brief Get socket user defined object.
 @param socket Socket object.
*/
RWS_API(void) rws_socket_set_on_received_pong(rws_socket socket, rws_on_socket_recvd_pong callback);

/**
 @brief Send bin header to connect socket.
 @detailed Thread safe method.
 @param socket Socket object.
 @param bin header string for sending.
 @param len length of header, 8 bytes.
 @return rws_true - socket and bin header exists and placed to send queue, otherwice rws_false
 */
RWS_API(rws_bool) rws_socket_send_bin_start(rws_socket socket, const char *bin, size_t len);

/**
 @brief Send bin content to connect socket.
 @detailed Thread safe method.
 @param socket Socket object.
 @param binary content for sending.
 @param len length of binary content.
 @return rws_true - socket and bin content exists and placed to send queue, otherwice rws_false.
*/
RWS_API(rws_bool) rws_socket_send_bin_continue(rws_socket socket, const char *bin, size_t len);


/**
 @brief Send final bin to connect socket.
 @detailed Thread safe method.
 @param socket Socket object.
 @param final bin string for sending.
 @param len length of final, 6 bytes.
 @return rws_true - socket and final bin exists and placed to send queue, otherwice rws_false
*/
RWS_API(rws_bool) rws_socket_send_bin_finish(rws_socket socket, const char * bin, size_t len);


// error

typedef enum _rws_error_code {
	rws_error_code_none = 0,
	
	rws_error_code_missed_parameter,
	
	rws_error_code_send_handshake,
	rws_error_code_parse_handshake,
	rws_error_code_read_write_socket,
	rws_error_code_connect_to_host,
	
	/**
	 @brief Connection was closed by endpoint.
	 Reasons: an endpoint shutting down, an endpoint having received a frame too large, or an
	 endpoint having received a frame that does not conform to the format expected by the endpoint.
	 */
	rws_error_code_connection_closed,
	
	
} rws_error_code;


/**
 @return 0 - if error is empty or no error, otherwice error code.
 */
RWS_API(int) rws_error_get_code(rws_error error);


/**
 @return 0 - if error is empty or no error, otherwice HTTP error.
 */
RWS_API(int) rws_error_get_http_error(rws_error error);


/**
 @brief Get description of the error object.
 */
RWS_API(const char *) rws_error_get_description(rws_error error);


// mutex

/**
 @brief Creates recursive mutex object.
 */
RWS_API(rws_mutex) rws_mutex_create_recursive(void);


/**
 @brief Lock mutex object.
 */
RWS_API(void) rws_mutex_lock(rws_mutex mutex);


/**
 @brief Unlock mutex object.
 */
RWS_API(void) rws_mutex_unlock(rws_mutex mutex);



/**
 @brief Unlock mutex object.
 */
RWS_API(void) rws_mutex_delete(rws_mutex mutex);


// thread

/**
 @brief Create thread object that start immidiatelly.
 */
RWS_API(rws_thread) rws_thread_create(rws_thread_funct thread_function, void * user_object);


/**
 @brief Pause current thread for a number of milliseconds.
 */
RWS_API(void) rws_thread_sleep(const unsigned int millisec);

#endif
