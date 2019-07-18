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

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** @defgroup group_platform platform
 *  @{
 */

#define _IN_ /**< indicate that this is an input parameter. */
#define _OU_ /**< indicate that this is an output parameter. */

/*********************************** mutex interface
 * ***********************************/

/** @defgroup group_platform_mutex mutex
 *  @{
 */

/**
 * @brief Create a mutex.
 *
 * @return NULL, initialize mutex failed; not NULL, the mutex handle.
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
 * @brief Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.
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
 * @param[in] ptr @n Pointer to a memory block previously allocated with
 * platform_malloc.
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
 * @brief Retrieves the number of milliseconds that have elapsed since the
 * system was boot.
 *
 * @param None.
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint32_t HAL_UptimeMs(void);

/**
 * @brief sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended,
 * in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(_IN_ uint32_t ms);

/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can
 optionally contain embedded format specifiers that specifies how subsequent
 arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in
 the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief Writes formatted data to string.
 *
 * @param [in] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can
 optionally contain embedded format specifiers that specifies how subsequent
 arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in
 the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...);

/**
 * @brief Get vendor ID of hardware module.
 *
 * @return NULL, Have NOT PID; NOT NULL, point to pid_str.
 */
char *HAL_GetPartnerID(char pid_str[]);

/** @} */ /* end of group_platform_other */

/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return 0, fail; > 0, success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);

/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return < 0, fail; 0, success.
 */
int32_t HAL_TCP_Destroy(uintptr_t fd);

/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if @len be written into the specific
 TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be
 transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the @buf
 parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other
 words, the API block @timeout_ms millisecond maximumly.
 * @return
   @verbatim
        < 0 : TCP connection error occur..
          0 : No any data be write into the TCP connection in @timeout_ms
 timeout period. (0, len] : The total number of bytes be written in @timeout_ms
 timeout period.
   @endverbatim
 * @see None.
 */
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms);

/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if @len be received from the specific
 TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [in] buf @n A pointer to a buffer to receive incoming data.
 * @param [in] len @n The length, in bytes, of the data pointed to by the @buf
 parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other
 words, the API block @timeout_ms millisecond maximumly.
 * @return
   @verbatim
         -2 : TCP connection error occur.
         -1 : TCP connection be closed by remote server.
          0 : No any data be received in @timeout_ms timeout period.
   (0, len] : The total number of bytes be received in @timeout_ms timeout
 period.
   @endverbatim
 * @see None.
 */
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len,
                     uint32_t timeout_ms);

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
uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt,
                            size_t ca_crt_len);

/**
 * @brief Destroy the specific SSL connection.
 *
 * @param[in] handle: @n Handle of the specific connection.
 *
 * @return < 0, fail; 0, success.
 */
int32_t HAL_SSL_Destroy(uintptr_t handle);

/**
 * @brief Write data into the specific SSL connection.
 *        The API will return immediately if @len be written into the specific
 SSL connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be
 transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the @buf
 parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other
 words, the API block @timeout_ms millisecond maximumly.
 * @return
   @verbatim
        < 0 : SSL connection error occur..
          0 : No any data be write into the SSL connection in @timeout_ms
 timeout period. (0, len] : The total number of bytes be written in @timeout_ms
 timeout period.
   @endverbatim
 * @see None.
 */
int32_t HAL_SSL_Write(uintptr_t handle, const char *buf, int len,
                      int timeout_ms);

/**
 * @brief Read data from the specific SSL connection with timeout parameter.
 *        The API will return immediately if @len be received from the specific
 SSL connection.
 *
 * @param [in] fd @n A descriptor identifying a SSL connection.
 * @param [in] buf @n A pointer to a buffer to receive incoming data.
 * @param [in] len @n The length, in bytes, of the data pointed to by the @buf
 parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other
 words, the API block @timeout_ms millisecond maximumly.
 * @return
   @verbatim
         -2 : SSL connection error occur.
         -1 : SSL connection be closed by remote server.
          0 : No any data be received in @timeout_ms timeout period.
   (0, len] : The total number of bytes be received in @timeout_ms timeout
 period.
   @endverbatim
 * @see None.
 */
int32_t HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms);

#if defined(__cplusplus)
}
#endif
#endif /* __IOT_IMPORT_H__ */
