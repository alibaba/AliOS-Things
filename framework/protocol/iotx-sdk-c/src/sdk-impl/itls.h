/*
 * Copyright (c) 2014-2018 Alibaba Group. All rights reserved.
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

#ifndef __ITLS_H__
#define __ITLS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

#define _IN_                /* indicate that this is an input parameter */
#define _OU_                /* indicate that this is an output parameter */

/**
 * @brief Establish a iTLS connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the iTLS server
 * @param [in] port: @n Specify the iTLS port of iTLS server
 * @param [in] product_key @n Specify the product key.
 * @return iTLS handle.
 * @see None.
 * @note None.
 */
uintptr_t HAL_iTLS_Establish(
            _IN_ const char *host,
            _IN_ uint16_t port,
            _IN_ const char *product_key);


/**
 * @brief Destroy the specific iTLS connection.
 *
 * @param[in] handle: @n Handle of the specific connection.
 *
 * @return The result of destroy iTLS
 *
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t HAL_iTLS_Destroy(_IN_ uintptr_t handle);


/**
 * @brief Write data into the specific iTLS connection.
 *        The API will return immediately if 'len' be written into the specific iTLS connection.
 *
 * @param [in] handle @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 * @retval      < 0 : iTLS connection error occur..
 * @retval        0 : No any data be write into the iTLS connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_iTLS_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len, _IN_ int timeout_ms);


/**
 * @brief Read data from the specific iTLS connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific SSL connection.
 *
 * @param [in] handle @n A descriptor identifying a iTLS connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out]  len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : iTLS connection error occur.
 * @retval       -1 : iTLS connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_iTLS_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len, _IN_ int timeout_ms);

#if defined(__cplusplus)
}
#endif

#endif  /* __ITLS_H__ */


