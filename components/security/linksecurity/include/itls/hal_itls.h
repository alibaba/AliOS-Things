/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __HAL_ITLS_H__
#define __HAL_ITLS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
#endif

/**
 * @brief Get itls library version.
 *
 * @param [out] version: @n A pointer to a buffer to receive version string.
 * @see None.
 * @note None.
 */
void hal_itls_get_version(char version[16]);

/**
 * @brief Set itls debug levels.
 *
 * @param [in] debug_level: @n Specify itls debug levels:
 *                              - 0 No debug
 *                              - 1 Error
 *                              - 2 State change
 *                              - 3 Informational
 *                              - 4 Verbose
 * @see None.
 * @note None.
 */
void hal_itls_set_debug_level(uint32_t debug_level);

/**
 * @brief Establish a itls connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the itls server
 * @param [in] port: @n Specify the port of itls server
 * @param [in] product_key @n Specify the product key, terminated with '\0'.
 * @param [in] product_secret @n Specify the product secret, terminated with '\0'.
 * @return itls handle.
 * @see None.
 * @note None.
 */
uintptr_t hal_itls_establish(
                   const char *host,
                   uint32_t port,
                   const char *product_key,
                   const char *product_secret);

/**
 * @brief Establish a itls connection in timeout seconds.
 *
 * @param [in] host: @n Specify the hostname(IP) of the itls server
 * @param [in] port: @n Specify the port of itls server
 * @param [in] product_key @n Specify the product key, terminated with '\0'.
 * @param [in] product_secret @n Specify the product secret, terminated with '\0'.
 * @param [in] timeout @n Specify the maximum number of seconds to wait.
 * @return itls handle.
 * @see None.
 * @note None.
 */
uintptr_t hal_itls_establish_timeout(
                   const char *host,
                   uint32_t port,
                   const char *product_key,
                   const char *product_secret,
                   uint32_t timeout);

/**
 * @brief Get message alert type if hal_itls_establish or hal_itls_establish_timeout fail.
 *
 * @return message alert type.
 * @see extended alert type for id2:
 *      160 : id2 generic error
 *      161 : id2 no quota
 *      162 : id2 is not exist
 *      163 : id2 authcode is invalid
 *      164 : id2 has not been activated
 *      165 : the timestamp used in authcode is expired
 *      166 : id2 challenge is invalid
 *      167 : not support this operation
 *      168 : id2 has been suspended
 *      169 : id2 has been discarded
 *      170 : permission denied, id2 has been binded to other product key
 *      171 : product key is invalid
 *      172 : Product key is not exist
 *      173 : id2 server is busy
 *      174 : the device fingerprint is invalid
 *      175 : the device fingerprint is duplicated
 *      176 : id2 server random is invalid
 *      177 : hash type used in authcode generated is invalid
 *      178 : id2 key type is invalid
 */
uint32_t hal_itls_get_alert_type(void);

/**
 * @brief Destroy the specific itls connection.
 *
 * @param[in] handle: @n Handle of the specific connection.
 *
 * @return The result of destroy ssl
 *
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t hal_itls_destroy(uintptr_t handle);

/**
 * @brief Write data into the specific itls connection.
 *
 * @param [in] handle: @n Handle of a itls connection.
 * @param [in] buf: @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len: @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms: @n Specify the timeout value in millisecond.
 *
 * @retval      < 0 : itls connection error occur..
 * @retval        0 : No any data be write into the SSL connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t hal_itls_write(uintptr_t handle, const char *buf, int len, int timeout_ms);

/**
 * @brief Read data from the specific SSL connection with timeout parameter.
 *
 * @param [in] handle: @n Handle of a itls connection.
 * @param [out] buf: @n A pointer to a buffer to receive incoming data.
 * @param [out] len: @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms: @n Specify the timeout value in millisecond.
 *
 * @retval       -2 : itls connection error occur.
 * @retval       -1 : itls connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t hal_itls_read(uintptr_t handle, char *buf, int len, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ITLS_H__ */
