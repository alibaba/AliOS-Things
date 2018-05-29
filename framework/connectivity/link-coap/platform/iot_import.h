/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __IOT_IMPORT_H__
#define __IOT_IMPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include "iot_import_config.h"

/** @defgroup group_platform platform
 *  @{
 */

#define _IN_                /* indicate that this is an input parameter */
#define _OU_                /* indicate that this is an output parameter */
#define IOT_TRUE    (1)     /* indicate boolean value true */
#define IOT_FALSE   (0)     /* indicate boolean value false */

#define PID_STRLEN_MAX      (64)
#define MID_STRLEN_MAX      (64)

#ifdef SIM7000C_DAM
#include "qapi_ali_iot.h"
#else

#define NETWORK_ADDR_LEN 16

typedef struct
{
    char addr[NETWORK_ADDR_LEN];
    unsigned short port;
}NetworkAddr;

typedef enum {
    os_thread_priority_idle = -3,        /* priority: idle (lowest) */
    os_thread_priority_low = -2,         /* priority: low */
    os_thread_priority_belowNormal = -1, /* priority: below normal */
    os_thread_priority_normal = 0,       /* priority: normal (default) */
    os_thread_priority_aboveNormal = 1,  /* priority: above normal */
    os_thread_priority_high = 2,         /* priority: high */
    os_thread_priority_realtime = 3,     /* priority: realtime (highest) */
    os_thread_priority_error = 0x84,     /* system cannot determine priority or thread has illegal priority */
} hal_os_thread_priority_t;

typedef struct _hal_os_thread {
    hal_os_thread_priority_t priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    size_t                   stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                      detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} hal_os_thread_param_t;

/*********************************** mutex interface ***********************************/

/** @defgroup group_platform_mutex mutex
 *  @{
 */

/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void *HAL_MutexCreate(void);

/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(_IN_ void *mutex);



/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(_IN_ void *mutex);



/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(_IN_ void *mutex);


/** @} */ /* end of platform_mutex */


/** @defgroup group_platform_memory_manage memory
 *  @{
 */

/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Malloc(_IN_ uint32_t size);


/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(_IN_ void *ptr);


/** @} */ /* end of platform_memory_manage */

/** @defgroup group_platform_other other
 *  @{
 */

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void);


/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(_IN_ uint32_t ms);

/**
 * @brief get time left
 *
 * @param [in] ms time of end. [in] ms time of now
 * @return time left.
 * @see None.
 * @note None.
 */
uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now);

/**
 * @brief Set seed for a sequence of pseudo-random integers, which will be returned by HAL_Random()
 *
 * @param [in] seed @n A start point for the random number sequence
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Srandom(_IN_ uint32_t seed);

/**
 * @brief Get a random integer
 *
 * @param [in] region @n Range of generated random numbers
 * @return Random number
 * @see None.
 * @note None.
 */
uint32_t HAL_Random(_IN_ uint32_t region);

/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, ...);

/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written.
 * @param [in] fmt: @n Format that contains the text to be written.
 * @param [in] ap:  @n the variable argument list.
 * @return bytes of character successfully format into string.
 * @see None.
 * @note None.
 */
int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/**
 * @brief Get vendor ID of hardware module.
 *
 * @param [out] pid_str: @n Get vendor ID of hardware module form HAL_GetParternID
 * @return the strlen of pid_str[] successfully written into.
 */
int HAL_GetPartnerID(_OU_ char pid_str[PID_STRLEN_MAX]);

/**
 * @brief Get Module ID of hardware module.
 *
 * @param [out] mid_str: @n Get Module ID of hardware module form HAL_GetModuleID
 * @return the strlen of mid_str[] successfully written into.
 */
int HAL_GetModuleID(_OU_ char mid_str[MID_STRLEN_MAX]);

int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
            _OU_ int *stack_used);


void HAL_ThreadDetach(_IN_ void *thread_handle);

void HAL_ThreadDelete(_IN_ void *thread_handle);


/** @} */ /* end of group_platform_other */

/** @defgroup group_platform_network network
 *  @{
 */

/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
   @retval   0 : Fail.
   @retval > 0 : Success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_  uint16_t port);


/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t HAL_TCP_Destroy(_IN_ uintptr_t fd);


/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Write(_IN_ uintptr_t fd, _IN_ const char *buf, _IN_ uint32_t len, _IN_ uint32_t timeout_ms);


/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Read(_IN_ uintptr_t fd, _OU_ char *buf, _OU_ uint32_t len, _IN_ uint32_t timeout_ms);

/**
 * @brief Establish a SSL connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the SSL server
 * @param [in] port: @n Specify the SSL port of SSL server
 * @param [in] ca_crt @n Specify the root certificate which is PEM format.
 * @param [in] ca_crt_len @n Length of root certificate, in bytes.
 * @return SSL handle.
 * @see None.
 * @note None.
 */
uintptr_t HAL_SSL_Establish(
            _IN_ const char *host,
            _IN_ uint16_t port,
            _IN_ const char *ca_crt,
            _IN_ size_t ca_crt_len);


/**
 * @brief Destroy the specific SSL connection.
 *
 * @param[in] handle: @n Handle of the specific connection.
 *
 * @return The result of destroy ssl
 *
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t HAL_SSL_Destroy(_IN_ uintptr_t handle);


/**
 * @brief Write data into the specific SSL connection.
 *        The API will return immediately if 'len' be written into the specific SSL connection.
 *
 * @param [in] handle @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 * @retval      < 0 : SSL connection error occur..
 * @retval        0 : No any data be write into the SSL connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_SSL_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len, _IN_ int timeout_ms);


/**
 * @brief Read data from the specific SSL connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific SSL connection.
 *
 * @param [in] handle @n A descriptor identifying a SSL connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : SSL connection error occur.
 * @retval       -1 : SSL connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_SSL_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len, _IN_ int timeout_ms);

/**
 * @brief Establish a UDP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the UDP server
 * @param [in] port: @n Specify the UDP port of UDP server
 *
 * @retval  < 0 : Fail.
 * @retval >= 0 : Success, the value is handle of this UDP connection.
 * @see None.
 */
intptr_t HAL_UDP_create(_IN_ const char *host, _IN_ unsigned short port);

/**
 * @brief Destroy the specific UDP connection.
 *
 * @param [in] p_socket: @n Specify the UDP connection by handle.
 * @return None.
 * @see None .
 */
void HAL_UDP_close(_IN_ intptr_t p_socket);

/**
 * @brief Write data into the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a connection.
 * @param [in] p_data @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.

 * @retval          < 0 : UDP connection error occur.
 * @retval [0,datalen ] : The number of bytes sent.
 * @see None.
 */
int HAL_UDP_write(
            _IN_ intptr_t p_socket,
            _IN_ const unsigned char *p_data,
            _IN_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection by blocked
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [in] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @return
 *
 * @retval < 0 : UDP connect error occur.
 * @retval = 0 : End of file.
 * @retval > 0 : The number of byte read.
 * @see None.
 */
int HAL_UDP_read(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection with timeout parameter.
 *        The API will return immediately if 'datalen' be received from the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [out] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block timeout_ms millisecond maximumly.
 *
 * @retval          -4 : UDP connect error occur.
 * @retval          -3 : The  call  was interrupted by a signal before any data was read.
 * @retval          -2 : No any data be received in 'timeout_ms' timeout period.
 * @retval          -1 : Invalid parameter.
 * @retval           0 : End of file.
 * @retval (0,datalen] : The number of byte read.
 * @see None.
 */
int HAL_UDP_readTimeout(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen,
            _IN_ unsigned int timeout_ms);


int HAL_UDP_recvfrom(intptr_t          sockfd,
                NetworkAddr     *p_remote,
                unsigned char   *p_data,
                unsigned int     datalen,
                unsigned int     timeout_ms);

int HAL_UDP_sendto(intptr_t            sockfd,
                const NetworkAddr   *p_remote,
                const unsigned char *p_data,
                unsigned int         datalen,
                unsigned int         timeout_ms);



int HAL_UDP_joinmulticast(intptr_t           sockfd,
                           const char        *p_group);


/** @} */ /* end of platform_network */
/** @} */ /* end of platform */

#endif  /* SIM7000C_DAM */

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_IMPORT_H__ */
