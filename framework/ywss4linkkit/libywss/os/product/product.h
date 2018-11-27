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

#include "platform.h"

/** @defgroup group_product product
 *  @{
 */
#if 0
#define PRODUCT_KEY_LEN         (PRODUCT_KEY_MAXLEN)
#define DEVICE_NAME_LEN         (DEVICE_NAME_MAXLEN)
#define PRODUCT_SECRET_LEN      (DEVICE_SECRET_MAXLEN)
#define DEVICE_SECRET_LEN       (DEVICE_SECRET_MAXLEN)
#endif

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
 * @brief Get device name string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
#define product_get_device_name HAL_GetDeviceName

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

	/** @} */// end of group_product

#ifdef __cplusplus
}
#endif
#endif
