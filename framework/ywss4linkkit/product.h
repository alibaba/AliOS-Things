/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#ifndef __HV_PLATFORM_H__
#define __HV_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "platform/platform.h"
//#include "platform/iot_import_product.h"

/** @defgroup group_product product
 *  @{
 */

#define PRODUCT_MODEL_LEN       (MID_STR_MAXLEN)
#define PRODUCT_KEY_LEN         (PRODUCT_KEY_MAXLEN)
#define DEVICE_KEY_LEN          (DEVICE_NAME_MAXLEN)
#define MANUFACTURE_KEY_LEN     (PRODUCT_KEY_MAXLEN)
#define PRODUCT_SECRET_LEN      (DEVICE_SECRET_MAXLEN)
#define MANUFACTURE_SECRET_LEN  (DEVICE_SECRET_MAXLEN)
#define DEVICE_SECRET_LEN       (DEVICE_SECRET_MAXLEN)
#define PRODUCT_UUID_LEN        (32 + 1)
#define PRODUCT_VERSION_LEN     (FIRMWARE_VERSION_MAXLEN)
#define PRODUCT_NAME_LEN        (DEVICE_NAME_MAXLEN)

/**
 * @brief Get the product version string.
 *
 * @param[in] version_str @n Buffer for using to store version string.
 * @return The version string.
 * @see None.
 * @note
 */
#define product_get_version HAL_GetFirmwareVesion

/**
 * @brief Get product name string.
 *
 * @param[out] name_str @n Buffer for using to store name string.
 * @return A pointer to the start address of name_str.
 * @see None.
 * @note None.
 */
#define product_get_name HAL_GetDeviceName

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
 * @brief Get device key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
#define product_get_device_key HAL_GetProductKey

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
#define product_get_device_secret(sec) HAL_GetDeviceSecret(sec)

/**
 * @brief Get manufacture key string.
 *
 * @param[out] key_str @n Buffer for using to store manufacture key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
#define product_get_manufacture_key product_get_key

/**
 * @brief Get manufacture secret string.
 *
 * @param[out] secret_str @n Buffer for using to store manufacture secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
#define product_get_manufacture_secret product_get_secret

	/** @} */// end of group_product

#ifdef __cplusplus
}
#endif
#endif
