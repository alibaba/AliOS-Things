/**
 * \file            lwesp_opt.h
 * \brief           ESP-AT options
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
#ifndef LWESP_HDR_DEFAULT_CONFIG_H
#define LWESP_HDR_DEFAULT_CONFIG_H

/* Uncomment to ignore user options (or set macro in compiler flags) */
/* #define LWESP_IGNORE_USER_OPTS */

/* Include application options */
#ifndef LWESP_IGNORE_USER_OPTS
#include "lwesp_opts.h"
#endif /* LWESP_IGNORE_USER_OPTS */

/**
 * \defgroup        LWESP_OPT Configuration
 * \brief           ESP-AT options
 * \{
 *
 */

/**
 * \brief           Enables `1` or disables `0` support for ESP8266 AT commands
 *
 */
#ifndef LWESP_CFG_ESP8266
#define LWESP_CFG_ESP8266                           1
#endif

/**
 * \brief           Enables `1` or disables `0` support for ESP32 AT commands
 *
 */
#ifndef LWESP_CFG_ESP32
#define LWESP_CFG_ESP32                             0
#endif

/**
 * \brief           Enables `1` or disables `0` support for ESP32-C3 AT commands
 *
 */
#ifndef LWESP_CFG_ESP32_C3
#define LWESP_CFG_ESP32_C3                          0
#endif

/**
 * \brief           Enables `1` or disables `0` operating system support for ESP library
 *
 * \note            Value must be set to `1` in the current revision
 *
 * \note            Check \ref LWESP_OPT_OS group for more configuration related to operating system
 *
 */
#ifndef LWESP_CFG_OS
#define LWESP_CFG_OS                                1
#endif

/**
 * \brief           Enables `1` or disables `0` custom memory management functions
 *
 * When set to `1`, \ref LWESP_MEM block must be provided manually.
 * This includes implementation of functions \ref lwesp_mem_malloc,
 * \ref lwesp_mem_calloc, \ref lwesp_mem_realloc and \ref lwesp_mem_free
 *
 * \note            Function declaration follows standard C functions `malloc, calloc, realloc, free`.
 *                  Declaration is available in `lwesp/lwesp_mem.h` file. Include this file to final
 *                  implementation file
 *
 * \note            When implementing custom memory allocation, it is necessary
 *                  to take care of multiple threads accessing same resource for custom allocator
 */
#ifndef LWESP_CFG_MEM_CUSTOM
#define LWESP_CFG_MEM_CUSTOM                        1
#endif

/**
 * \brief           Memory alignment for dynamic memory allocations
 *
 * \note            Some CPUs can work faster if memory is aligned, usually to `4` or `8` bytes.
 *                  To speed up this possibilities, you can set memory alignment and library
 *                  will try to allocate memory on aligned boundaries.
 *
 * \note            Some CPUs such ARM Cortex-M0 dont't support unaligned memory access.
 *
 * \note            This value must be power of `2`
 */
#ifndef LWESP_CFG_MEM_ALIGNMENT
#define LWESP_CFG_MEM_ALIGNMENT                     4
#endif

/**
 * \brief           Enables `1` or disables `0` callback function and custom parameter for API functions
 *
 * When enabled, `2` additional parameters are available in API functions.
 * When command is executed, callback function with its parameter could be called when not set to `NULL`.
 */
#ifndef LWESP_CFG_USE_API_FUNC_EVT
#define LWESP_CFG_USE_API_FUNC_EVT                  1
#endif

/**
 * \brief           Set number of retries for send data command.
 *
 * Sometimes it may happen that `AT+SEND` command fails due to different problems.
 * Trying to send the same data multiple times can raise chances for success.
 */
#ifndef LWESP_CFG_MAX_SEND_RETRIES
#define LWESP_CFG_MAX_SEND_RETRIES                  3
#endif

/**
 * \brief           Default baudrate used for AT port
 *
 * \note            User may call API function to change to desired baudrate if necessary
 */
#ifndef LWESP_CFG_AT_PORT_BAUDRATE
#define LWESP_CFG_AT_PORT_BAUDRATE                  115200
#endif

/**
 * \brief           Enables `1` or disables `0` ESP acting as station
 *
 * \note            When device is in station mode, it can connect to other access points
 */
#ifndef LWESP_CFG_MODE_STATION
#define LWESP_CFG_MODE_STATION                      1
#endif

/**
 * \brief           Enables `1` or disables `0` ESP acting as access point
 *
 * \note            When device is in access point mode, it can accept connections from other stations
 */
#ifndef LWESP_CFG_MODE_ACCESS_POINT
#define LWESP_CFG_MODE_ACCESS_POINT                 1
#endif

/**
 * \brief           Enables `1` or disables `0` full data info in \ref lwesp_ap_t structure.
 *
 * When enabled, advanced information is stored, and as a consequence, structure size is increased.
 * Information such as scan type, min scan time, max scan time, frequency offset, frequency calibration are added
 */
#ifndef LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS
#define LWESP_CFG_ACCESS_POINT_STRUCT_FULL_FIELDS   0
#endif

/**
 * \brief           Enables `1` or disables `0` periodic keep-alive events to registered callbacks
 *
 */
#ifndef LWESP_CFG_KEEP_ALIVE
#define LWESP_CFG_KEEP_ALIVE                        1
#endif

/**
 * \brief           Timeout periodic time to trigger keep alive events to registered callbacks
 *
 * Feature must be enabled with \ref LWESP_CFG_KEEP_ALIVE
 */
#ifndef LWESP_CFG_KEEP_ALIVE_TIMEOUT
#define LWESP_CFG_KEEP_ALIVE_TIMEOUT                1000
#endif

/**
 * \defgroup        LWESP_OPT_CONN Connection settings
 * \brief           Connection settings
 * \{
 */

/**
 * \brief           Enables `1` or disables `0` support for IPv6
 *
 */
#ifndef LWESP_CFG_IPV6
#define LWESP_CFG_IPV6                              0
#endif

/**
 * \brief           Maximum single buffer size for network receive data on active connection
 *
 * \note            When ESP sends buffer bigger than maximal, multiple buffers are created.
 *                  Exception is UDP connection type, which can be controlled,
 *                  with option \ref LWESP_CFG_CONN_ALLOW_FRAGMENTED_UDP_SEND
 */
#ifndef LWESP_CFG_CONN_MAX_RECV_BUFF_SIZE
#define LWESP_CFG_CONN_MAX_RECV_BUFF_SIZE           1460
#endif

/**
 * \brief           Enables `1` or disables `0` support for fragmented send of UDP packets.
 *
 * When connection type is UDP and packet length longer than maximal transmission unit,
 * it can be split into multiple packets and sent over the network.
 *
 * When this feature is disabled, max length of UDP packet is defined with
 * \ref LWESP_CFG_CONN_MAX_DATA_LEN option
 */
#ifndef LWESP_CFG_CONN_ALLOW_FRAGMENTED_UDP_SEND
#define LWESP_CFG_CONN_ALLOW_FRAGMENTED_UDP_SEND    0
#endif

/**
 * \brief           Maximal number of connections AT software can support on ESP device
 *
 * \note            In case of official ESP-AT software, leave this on default value (`5`)
 */
#ifndef LWESP_CFG_MAX_CONNS
#define LWESP_CFG_MAX_CONNS                         5
#endif

/**
 * \brief           Maximal number of bytes we can send at single command to ESP
 * \note            Value can not exceed `2048` bytes or no data will be send at all (ESP8266 AT SW limitation)
 *
 * When manual TCP read mode is enabled, this parameter defines number of bytes to be read at a time
 *
 * \note            This is limitation of ESP AT commands and on systems where RAM
 *                  is not an issue, it should be set to maximal value (`2048`)
 *                  to optimize data transfer speed performance
 */
#ifndef LWESP_CFG_CONN_MAX_DATA_LEN
#define LWESP_CFG_CONN_MAX_DATA_LEN                 2048
#endif

/**
 * \brief           Poll interval for connections in units of milliseconds
 *
 * Value indicates interval time to call poll event on active connections.
 *
 * \note            Single poll interval applies for all connections
 */
#ifndef LWESP_CFG_CONN_POLL_INTERVAL
#define LWESP_CFG_CONN_POLL_INTERVAL                500
#endif

/**
 * \brief           Enables `1` or disables `0` manual `TCP` data receive from ESP device
 *
 * Normally ESP automatically sends received TCP data to host device
 * in async mode. When host device is slow or if there is memory constrain,
 * it may happen that processing cannot handle all received data.
 *
 * When feature is enabled, ESP will notify host device about new data
 * available for read and then user may start read process
 *
 * \note            This feature is only available for `TCP` connections.
 */
#ifndef LWESP_CFG_CONN_MANUAL_TCP_RECEIVE
#define LWESP_CFG_CONN_MANUAL_TCP_RECEIVE           0
#endif

/**
 * \}
 */

/**
 * \brief           Buffer size for received data waiting to be processed
 * \note            When server mode is active and a lot of connections are in queue
 *                  this should be set high otherwise your buffer may overflow
 *
 * \note            Buffer size also depends on TX user driver if it uses DMA or blocking mode.
 *                  In case of DMA (CPU can work other tasks), buffer may be smaller as CPU
 *                  will have more time to process all the incoming bytes
 *
 * \note            This parameter has no meaning when \ref LWESP_CFG_INPUT_USE_PROCESS is enabled
 */
#ifndef LWESP_CFG_RCV_BUFF_SIZE
#define LWESP_CFG_RCV_BUFF_SIZE                     0x400
#endif

/**
 * \brief           Enables `1` or disables `0` reset sequence after \ref lwesp_init call
 *
 * \note            When this functionality is disabled, user must manually call \ref lwesp_reset to send
 *                  reset sequence to ESP device.
 */
#ifndef LWESP_CFG_RESET_ON_INIT
#define LWESP_CFG_RESET_ON_INIT                     1
#endif

/**
 * \brief           Enables `1` or disables `0` device restore after \ref lwesp_init call
 *
 * \note            When this feature is enabled, it will automatically
 *                  restore and clear any settings stored as \em default in ESP device
 */
#ifndef LWESP_CFG_RESTORE_ON_INIT
#define LWESP_CFG_RESTORE_ON_INIT                   1
#endif

/**
 * \brief           Enables `1` or disables `0` reset sequence after \ref lwesp_device_set_present call
 *
 * \note            When this functionality is disabled, user must manually call \ref lwesp_reset to send
 *                  reset sequence to ESP device.
 */
#ifndef LWESP_CFG_RESET_ON_DEVICE_PRESENT
#define LWESP_CFG_RESET_ON_DEVICE_PRESENT           1
#endif

/**
 * \brief           Default delay (milliseconds unit) before sending first AT command on reset sequence
 *
 */
#ifndef LWESP_CFG_RESET_DELAY_DEFAULT
#define LWESP_CFG_RESET_DELAY_DEFAULT               1000
#endif

/**
 * \brief           Maximum length of SSID for access point scan
 *
 * \note            This parameter must include trailling zero
 */
#ifndef LWESP_CFG_MAX_SSID_LENGTH
#define LWESP_CFG_MAX_SSID_LENGTH                   21
#endif

/**
 * \brief           Maximum length of PWD for access point
 *
 * \note            This parameter must include trailling zero
 */
#ifndef LWESP_CFG_MAX_PWD_LENGTH
#define LWESP_CFG_MAX_PWD_LENGTH                    65
#endif

/**
 * \brief           Enables `1` or disables `0` listing all available CMDs during reset/restore operation
 *
 */
#ifndef LWESP_CFG_LIST_CMD
#define LWESP_CFG_LIST_CMD                          0
#endif

/**
 * \defgroup        LWESP_OPT_DBG Debugging
 * \brief           Debugging configurations
 * \{
 */

/**
 * \brief           Set global debug support
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 *
 * \note            Set to \ref LWESP_DBG_OFF to globally disable all debugs
 */
#ifndef LWESP_CFG_DBG
#define LWESP_CFG_DBG                               LWESP_DBG_ON
#endif

/**
 * \brief           Debugging output function
 *
 * Called with format and optional parameters for printf-like debug
 */
#ifndef LWESP_CFG_DBG_OUT
#define LWESP_CFG_DBG_OUT(fmt, ...)                 do { extern int printf( const char * format, ... ); printf(fmt, ## __VA_ARGS__); } while (0)
#endif

/**
 * \brief           Minimal debug level
 *
 * Check \ref LWESP_DBG_LVL for possible values
 */
#ifndef LWESP_CFG_DBG_LVL_MIN
#define LWESP_CFG_DBG_LVL_MIN                       LWESP_DBG_LVL_ALL
#endif

/**
 * \brief           Enabled debug types
 *
 * When debug is globally enabled with \ref LWESP_CFG_DBG parameter,
 * user must enable debug types such as TRACE or STATE messages.
 *
 * Check \ref LWESP_DBG_TYPE for possible options. Separate values with `bitwise OR` operator
 */
#ifndef LWESP_CFG_DBG_TYPES_ON
#define LWESP_CFG_DBG_TYPES_ON                      LWESP_DBG_TYPE_ALL
#endif

/**
 * \brief           Set debug level for init function
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_INIT
#define LWESP_CFG_DBG_INIT                          LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for memory manager
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_MEM
#define LWESP_CFG_DBG_MEM                           LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for input module
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_INPUT
#define LWESP_CFG_DBG_INPUT                         LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for ESP threads
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_THREAD
#define LWESP_CFG_DBG_THREAD                        LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for asserting of input variables
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_ASSERT
#define LWESP_CFG_DBG_ASSERT                        LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for incoming data received from device
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_IPD
#define LWESP_CFG_DBG_IPD                           LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for netconn sequential API
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_NETCONN
#define LWESP_CFG_DBG_NETCONN                       LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for packet buffer manager
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_PBUF
#define LWESP_CFG_DBG_PBUF                          LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for connections
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_CONN
#define LWESP_CFG_DBG_CONN                          LWESP_DBG_ON
#endif

/**
 * \brief           Set debug level for dynamic variable allocations
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_VAR
#define LWESP_CFG_DBG_VAR                           LWESP_DBG_ON
#endif

/**
 * \brief           Enables `1` or disables `0` echo mode on AT commands
 *                  sent to ESP device.
 *
 * \note            This mode is useful when debugging ESP communication
 */
#ifndef LWESP_CFG_AT_ECHO
#define LWESP_CFG_AT_ECHO                           1
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_OS OS configuration
 * \brief           Operating system dependant configuration
 * \{
 */

/**
 * \brief           Set number of message queue entries for procuder thread
 *
 * Message queue is used for storing memory address to command data
 */
#ifndef LWESP_CFG_THREAD_PRODUCER_MBOX_SIZE
#define LWESP_CFG_THREAD_PRODUCER_MBOX_SIZE         16
#endif

/**
 * \brief           Set number of message queue entries for processing thread
 *
 * Message queue is used to notify processing thread about new received data on AT port
 */
#ifndef LWESP_CFG_THREAD_PROCESS_MBOX_SIZE
#define LWESP_CFG_THREAD_PROCESS_MBOX_SIZE          16
#endif

/**
 * \brief           Enables `1` or disables `0` direct support for processing input data
 *
 * When this mode is enabled, no overhead is included for copying data
 * to receive buffer because bytes are processed directly by \ref lwesp_input_process function
 *
 * If this mode is not enabled, then user have to send every received byte via \ref lwesp_input
 * function to the internal buffer for future processing. This may introduce additional overhead
 * with data copy and may decrease library performance
 *
 * \note            This mode can only be used when \ref LWESP_CFG_OS is enabled
 *
 * \note            When using this mode, separate thread must be dedicated only
 *                  for reading data on AT port. It is usually implemented in LL driver
 *
 * \note            Best case for using this mode is if DMA receive is supported by host device
 */
#ifndef LWESP_CFG_INPUT_USE_PROCESS
#define LWESP_CFG_INPUT_USE_PROCESS                 0
#endif

/**
 * \brief           Producer thread hook, called each time thread wakes-up and does the processing.
 *
 * It can be used to check if thread is alive.
 */
#ifndef LWESP_THREAD_PRODUCER_HOOK
#define LWESP_THREAD_PRODUCER_HOOK()
#endif

/**
 * \brief           Process thread hook, called each time thread wakes-up and does the processing.
 *
 * It can be used to check if thread is alive.
 */
#ifndef LWESP_THREAD_PROCESS_HOOK
#define LWESP_THREAD_PROCESS_HOOK()
#endif

/**
 * \brief           Enables `1` or disables `0` custom memory byte pool extension for ThreadX port
 *
 * When enabled, user must manually set byte pool at run-time, before \ref lwesp_init is called
 */
#ifndef LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL
#define LWESP_CFG_THREADX_CUSTOM_MEM_BYTE_POOL      0
#endif

/**
 * \brief           Enables `1` or disables `0` idle thread extensions feature of ThreadX
 *
 * When enabled, user must manually configure idle thread and setup additional thread handle extension fields.
 * By default ThreadX doesn't support self-thread cleanup when thread memory is dynamically allocated & thread terminated,
 * hence another thread is mandatory to do the cleanup process instead.
 *
 * This configuration does not create idle-thread, rather only sets additional TX_THREAD fields,
 * indicating thread handle and thread stack are dynamically allocated.
 *
 * Have a look at System-ThreadX port for implementation
 */
#ifndef LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION
#define LWESP_CFG_THREADX_IDLE_THREAD_EXTENSION     0
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_MODULES Modules
 * \brief           Configuration of specific modules
 * \{
 */

/**
 * \defgroup        LWESP_OPT_MODULES_NETCONN Netconn module
 * \brief           Configuration of netconn API module
 * \{
 */

/**
 * \brief           Enables `1` or disables `0` NETCONN sequential API support for OS systems
 *
 * \note            To use this feature, OS support is mandatory.
 * \sa              LWESP_CFG_OS
 */
#ifndef LWESP_CFG_NETCONN
#define LWESP_CFG_NETCONN                           0
#endif

/**
 * \brief           Enables `1` or disables `0` receive timeout feature
 *
 * When this option is enabled, user will get an option
 * to set timeout value for receive data on netconn,
 * before function returns timeout error.
 *
 * \note            Even if this option is enabled, user must still manually set timeout,
 *                  by default time will be set to 0 which means no timeout.
 */
#ifndef LWESP_CFG_NETCONN_RECEIVE_TIMEOUT
#define LWESP_CFG_NETCONN_RECEIVE_TIMEOUT           1
#endif

/**
 * \brief           Accept queue length for new client when netconn server is used
 *
 * Defines number of maximal clients waiting in accept queue of server connection
 */
#ifndef LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN
#define LWESP_CFG_NETCONN_ACCEPT_QUEUE_LEN          5
#endif

/**
 * \brief           Receive queue length for pbuf entries
 *
 * Defines maximal number of pbuf data packet references for receive
 */
#ifndef LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN
#define LWESP_CFG_NETCONN_RECEIVE_QUEUE_LEN         8
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_MODULES_MQTT MQTT client module
 * \brief           Configuration of MQTT and MQTT API client modules
 * \{
 */

/**
 * \brief           Maximal number of open MQTT requests at a time
 *
 */
#ifndef LWESP_CFG_MQTT_MAX_REQUESTS
#define LWESP_CFG_MQTT_MAX_REQUESTS                 8
#endif

/**
 * \brief           Size of MQTT API message queue for received messages
 *
 */
#ifndef LWESP_CFG_MQTT_API_MBOX_SIZE
#define LWESP_CFG_MQTT_API_MBOX_SIZE                8
#endif

/**
 * \brief           Set debug level for MQTT client module
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_MQTT
#define LWESP_CFG_DBG_MQTT                          LWESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for MQTT API client module
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_MQTT_API
#define LWESP_CFG_DBG_MQTT_API                      LWESP_DBG_OFF
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_MODULES_CAYENNE Cayenne MQTT client
 * \brief           Configuration of Cayenne MQTT client
 * \{
 */

/**
 * \brief           Size of Cayenne messages size TX buffer in units of bytes
 *
 */
#ifndef LWESP_CFG_CAYENNE_TX_BUFF_SIZE
#define LWESP_CFG_CAYENNE_TX_BUFF_SIZE              256
#endif

/**
 * \brief           Size of Cayenne RX buffer size in units of bytes
 *
 */
#ifndef LWESP_CFG_CAYENNE_RX_BUFF_SIZE
#define LWESP_CFG_CAYENNE_RX_BUFF_SIZE              256
#endif

/**
 * \brief           Set debug level for Cayenne MQTT client module
 *
 * Possible values are \ref LWESP_DBG_ON or \ref LWESP_DBG_OFF
 */
#ifndef LWESP_CFG_DBG_CAYENNE
#define LWESP_CFG_DBG_CAYENNE                       LWESP_DBG_OFF
#endif

/**
 * \}
 */

/**
 * \brief           Enables `1` or disables `0` support for DNS functions
 *
 */
#ifndef LWESP_CFG_DNS
#define LWESP_CFG_DNS                               1
#endif

/**
 * \brief           Enables `1` or disables `0` support for WPS functions
 *
 */
#ifndef LWESP_CFG_WPS
#define LWESP_CFG_WPS                               0
#endif

/**
 * \brief           Enables `1` or disables `0` support for SNTP protocol with AT commands
 *
 */
#ifndef LWESP_CFG_SNTP
#define LWESP_CFG_SNTP                              0
#endif

/**
 * \brief           Enables `1` or disables `0` support for hostname with AT commands
 *
 */
#ifndef LWESP_CFG_HOSTNAME
#define LWESP_CFG_HOSTNAME                          0
#endif

/**
 * \brief           Enables `1` or disables `0` support for ping functions
 *
 */
#ifndef LWESP_CFG_PING
#define LWESP_CFG_PING                              0
#endif

/**
 * \brief           Enables `1` or disables `0` support for mDNS
 *
 */
#ifndef LWESP_CFG_MDNS
#define LWESP_CFG_MDNS                              0
#endif

/**
 * \brief           Enables `1` or disables `0` support for SMART config
 *
 */
#ifndef LWESP_CFG_SMART
#define LWESP_CFG_SMART                             0
#endif

/**
 * \brief           Enables `1` or disables `0` support for Web Server feature
 *
 */
#ifndef LWESP_CFG_WEBSERVER
#define LWESP_CFG_WEBSERVER                         0
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_STD_LIB Standard library
 * \brief           Standard C library configuration
 * \{
 *
 * Configuration allows you to overwrite default C language function
 * in case of better implementation with hardware (for example DMA for data copy).
 */

/**
 * \brief           Memory copy function declaration
 *
 * User is able to change the memory function, in case
 * hardware supports copy operation, it may implement its own
 *
 * Function prototype must be similar to:
 *
 * \code{c}
void *  my_memcpy(void* dst, const void* src, size_t len);
\endcode
 *
 * \param[in]       dst: Destination memory start address
 * \param[in]       src: Source memory start address
 * \param[in]       len: Number of bytes to copy
 * \return          Destination memory start address
 */
#ifndef LWESP_MEMCPY
#define LWESP_MEMCPY(dst, src, len)                 memcpy(dst, src, len)
#endif

/**
 * \brief           Memory set function declaration
 *
 * Function prototype must be similar to:
 *
 * \code{c}
void *  my_memset(void* dst, int b, size_t len);
\endcode
 *
 * \param[in]       dst: Destination memory start address
 * \param[in]       b: Value (byte) to set in memory
 * \param[in]       len: Number of bytes to set
 * \return          Destination memory start address
 */
#ifndef LWESP_MEMSET
#define LWESP_MEMSET(dst, b, len)                   memset(dst, b, len)
#endif

/**
 * \}
 */

/**
 * \defgroup        LWESP_OPT_MIN_AT_VERSIONS Minimum AT versions
 * \brief           Minimum AT versions needed for Espressif devices to run properly with LwESP
 * \{
 */

#define LWESP_MIN_AT_VERSION_MAJOR_ESP8266          2   /*!< Minimal major version for ESP8266 */
#define LWESP_MIN_AT_VERSION_MINOR_ESP8266          2   /*!< Minimal minor version for ESP8266 */
#define LWESP_MIN_AT_VERSION_PATCH_ESP8266          1   /*!< Minimal patch version for ESP8266 */
#define LWESP_MIN_AT_VERSION_MAJOR_ESP32            2   /*!< Minimal major version for ESP32 */
#define LWESP_MIN_AT_VERSION_MINOR_ESP32            2   /*!< Minimal minor version for ESP32 */
#define LWESP_MIN_AT_VERSION_PATCH_ESP32            0   /*!< Minimal patch version for ESP32 */
#define LWESP_MIN_AT_VERSION_MAJOR_ESP32_C3         2   /*!< Minimal major version for ESP32-C3 */
#define LWESP_MIN_AT_VERSION_MINOR_ESP32_C3         3   /*!< Minimal minor version for ESP32-C3 */
#define LWESP_MIN_AT_VERSION_PATCH_ESP32_C3         0   /*!< Minimal patch version for ESP32-C3 */

/**
 * \}
 */

/**
 * \}
 */

#if !__DOXYGEN__

/* Define group mode value */
#define LWESP_CFG_MODE_STATION_ACCESS_POINT   (LWESP_CFG_MODE_STATION && LWESP_CFG_MODE_ACCESS_POINT)

/* At least one of them must be enabled */
#if !LWESP_CFG_MODE_STATION && !LWESP_CFG_MODE_ACCESS_POINT
#error "Invalid ESP configuration. LWESP_CFG_MODE_STATION and LWESP_CFG_MODE_ACCESS_POINT cannot be disabled at the same time!"
#endif

/* Operating system config */
#if !LWESP_CFG_OS
#if LWESP_CFG_INPUT_USE_PROCESS
#error "LWESP_CFG_INPUT_USE_PROCESS may only be enabled when OS is used!"
#endif /* LWESP_CFG_INPUT_USE_PROCESS */
#endif /* !LWESP_CFG_OS */

/* Device config */
#if !LWESP_CFG_ESP8266 && !LWESP_CFG_ESP32 && !LWESP_CFG_ESP32_C3
#error "At least one of LWESP_CFG_ESP8266 or LWESP_CFG_ESP32 or LWESP_CFG_ESP32_C3 must be set to 1!"
#endif /* !LWESP_CFG_ESP8266 && !LWESP_CFG_ESP32 */

/* WPS config */
#if LWESP_CFG_WPS && !LWESP_CFG_MODE_STATION
#error "WPS function may only be used when station mode is enabled!"
#endif /* LWESP_CFG_WPS && !LWESP_CFG_MODE_STATION */

#endif /* !__DOXYGEN__ */

#include "lwesp/lwesp_debug.h"

#endif /* LWESP_HDR_DEFAULT_CONFIG_H */
