/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __HV_PLATFORM_H__
#define __HV_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"

/** @defgroup group_product product
 *  @{
 */
#ifndef PRODUCT_KEY_LEN
#define PRODUCT_KEY_LEN         (PRODUCT_KEY_MAXLEN)
#define DEVICE_NAME_LEN         (DEVICE_NAME_MAXLEN)
#define PRODUCT_SECRET_LEN      (DEVICE_SECRET_MAXLEN)
#define DEVICE_SECRET_LEN       (DEVICE_SECRET_MAXLEN)
#endif

/**
 * @brief Get device name string.
 *
 * @param[out] name_str @n Buffer for using to store name string.
 * @return A pointer to the start address of name_str.
 * @see None.
 * @note None.
 */
#define device_get_name HAL_GetDeviceName

/**
 * @brief Get product key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
#define product_get_key(key) HAL_GetProductKey(key)

/**
 * @brief Get product secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
#define product_get_secret HAL_GetProductSecret

/**
 * @brief Get deivce secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
#define device_get_secret(sec) HAL_GetDeviceSecret(sec)

	/** @} */// end of group_product

#ifdef __cplusplus
}
#endif
#endif
