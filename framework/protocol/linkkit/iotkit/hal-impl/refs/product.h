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

/** @defgroup group_product product
 *  @{
 */

#define PRODUCT_SN_LEN			(64 + 1)
#define PRODUCT_MODEL_LEN		(80 + 1)
#define PRODUCT_KEY_LEN			(20 + 1)
#define DEVICE_KEY_LEN			(20 + 1)
#define PRODUCT_SECRET_LEN		(40 + 1)
#define DEVICE_SECRET_LEN		(32 + 1)
#define PRODUCT_UUID_LEN		(32 + 1)
#define PRODUCT_VERSION_LEN     (16 + 1)
#define PRODUCT_NAME_LEN        (32 + 1)

/**
 * @brief Get the product version string.
 *
 * @param[in] version_str @n Buffer for using to store version string.
 * @return The version string.
 * @see None.
 * @note
 */
	char *product_get_version(char version_str[PRODUCT_VERSION_LEN]);

/**
 * @brief Get product name string.
 *
 * @param[out] name_str @n Buffer for using to store name string.
 * @return A pointer to the start address of name_str.
 * @see None.
 * @note None.
 */
	char *product_get_name(char name_str[PRODUCT_NAME_LEN]);

/**
 * @brief Get product SN string.
 *
 * @param[out] sn_str @n Buffer for using to store SN string.
 * @return A pointer to the start address of sn_str.
 * @see None.
 * @note None.
 */
	char *product_get_sn(char sn_str[PRODUCT_SN_LEN]);

/**
 * @brief Get product model string.
 *
 * @param[out] model_str @n Buffer for using to store model string.
 * @return A pointer to the start address of model_str.
 * @see None.
 * @note None.
 */
	char *product_get_model(char model_str[PRODUCT_MODEL_LEN]);

/**
 * @brief Get product key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
	char *product_get_key(char key_str[PRODUCT_KEY_LEN]);

/**
 * @brief Get device key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
	char *product_get_device_key(char key_str[DEVICE_KEY_LEN]);

/**
 * @brief Get product secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
	char *product_get_secret(char secret_str[PRODUCT_SECRET_LEN]);

/**
 * @brief Get deivce secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
	char *product_get_device_secret(char secret_str[DEVICE_SECRET_LEN]);

/**
 * @brief Get product debug key string.
 *
 * @param[out] key_str @n Buffer for using to store debug key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
	char *product_get_debug_key(char key_str[PRODUCT_KEY_LEN]);

/**
 * @brief Get product debug secret string.
 *
 * @param[out] secret_str @n Buffer for using to store debug secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
	char *product_get_debug_secret(char secret_str[PRODUCT_SECRET_LEN]);

	/** @} */// end of group_product

#ifdef __cplusplus
}
#endif
#endif
