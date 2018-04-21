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



#ifndef __IOT_SUBDEV_EXPORT_H__
#define __IOT_SUBDEV_EXPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "iot_export_gw.h"

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_sub_device_callback callback
 */

/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when get sub device property.
 *          The response data post by IOT_PropertyReport API
 * @param [in] product_key:  @n Sub device product key.
 * @param[in] device_name:   @n Unique device identifier,maximum length 32 bytes.
 * @param[in] property_id[]: @n NULL-terminated array which hold the property name.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*sub_device_cb_property_get_fpt)(_IN_ const char *product_key,
                _IN_ const char *device_name,
                _IN_ const char *property_id[]);

/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when set gateway property.
 * @param [in] product_key:     @n Sub device product key.
 * @param[in] device_name:      @n Unique device identifier,maximum length 32 bytes.
 * @param[in] property_kv[]:    @n The array of property key-value struct.
 * @param[in] array_sz:         @n The property kv array size.

 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*sub_device_cb_property_set_fpt)(_IN_ const char *product_key,
                _IN_ const char *device_name,
                _IN_ iot_kv_info_t property_kv[],
                _IN_ int array_sz);


/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when invode sub device service.
 *          The ouput data post by IOT_SubDevServiceReplyPost API
 * @param[in] product_key:      @n Sub device product key.
 * @param[in] device_name:      @n Unique device identifier,maximum length 32 bytes.
 * @param[in] invoke_id:        @n The invoke service id,it is used to post reply data if data non-null.
 * @param[in] service_name:     @n Service identifier,maximum length 32 bytes.
 * @param[in] service_params:   @n Json object format service params.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*sub_device_cb_service_invoke_fpt)(_IN_ const char *product_key,
                _IN_ const char *device_name,
                _IN_ unsigned int invoke_id,
                _IN_ const char *service_id,
                _IN_ const char *service_params);


/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when remove sub device.
 * @param [in] product_key: @n Sub device product key.
 * @param[in] device_name:  @n Unique device identifier,maximum length 32 bytes.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*sub_device_cb_remove_fpt)(_IN_ const char *product_key,
                          _IN_ const char *device_name);


/* sub device callback types */
typedef enum {
    /* get sub device propertys */
    SUB_DEVICE_CB_PROPERTY_GET = 0,

    /* set sub device propertys */
    SUB_DEVICE_CB_PROPERTY_SET,

    /* invode sub device service */
    SUB_DEVICE_CB_SERVICE_INVOKE,

    /* remove sub device */
    SUB_DEVICE_CB_REMOVE,

    /* Maximum number of callback */
    SUB_DEVICE_CB_MAX
}iot_sub_device_cb_types_t;

/** @} */ //end of sub_device_callback


/** @defgroup subdev_api subdev
 *  @{
 */


/**
 * @brief Register sub device service callback to IOT SDK.
 *
 * @param [in] service_callback: @n The array of struct pointer to sub device service callback.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevSrvCbRegister(
                    _IN_ iot_service_callback_pt service_callback[SUB_DEVICE_CB_MAX]);


/**
 * @brief Static register an subdevice to AliYun cloud, based on the 'product_key' + 'device_name' + 'device_secret' tuple.
 *
 * @param [in] product_key:     @n Apply for 'product_key' in the AliYun Console.
 * @param [in] device_name:     @n Apply for 'device_name' in the AliYun Console.
 * @param [in] device_secret:   @n Apply for 'device_secret' in the AliYun Console.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevStaticRegister(_IN_ const char *product_key,
                          _IN_ const char *device_name,
                          _IN_ const char *device_secret);

/**
 * @brief Dynamic register an subdevice to AliYun cloud, Based on the 'product_key' + 'device_name'.
 *
 * @param [in] product_key: @n Apply for 'product_key' in the AliYun Console.
 * @param [in] device_name: @n Sub device Unique device identifier,maximum length 32 bytes.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevDynamicRegister(_IN_ const char *product_key,
                          _IN_ const char *device_name);

/**
 * @brief Unregister an subdevice to AliYun cloud.
 *
 * @param [in] product_key: @n Sub device product key.
 * @param [in] device_name: @n Sub device Unique device identifier,maximum length 32 bytes.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevUnegister(_IN_ const char *product_key,
                          _IN_ const char *device_name);

/**
 * @brief Login subdevice to AliYun cloud.
 *
 * @param [in] product_key: @n Sub device product key.
 * @param [in] device_name: @n Sub device Unique device identifier,maximum length 32 bytes.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevLogin(_IN_ const char *product_key,
                        _IN_ const char *device_name);


/**
 * @brief Logout subdevice to AliYun cloud.
 *
 * @param [in] product_key: @n Sub device product key.
 * @param [in] device_name: @n Sub device Unique device identifier,maximum length 32 bytes.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevLogout(_IN_ const char *product_key,
                        _IN_ const char *device_name);

/**
 * @brief Post Service reply data to AliYun cloud.
 *
 * @param [in] invoke_id:   @n The Service invoke id ,read in SUB_DEVICE_CB_SERVICE_INVOKE callback.
 * @param [in] product_key: @n The Sub device product key,maximum length 20 bytes.
 * @param [in] device_name: @n The Sub device nique device identifier,maximum length 32 bytes.
 * @param [in] service_id:  @n The service id,maximum length 32 bytes.
 * @param [in] ouput_data:  @n The service invoke output data.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SubDevServiceReplyToCloud(_IN_ unsigned int invoke_id,
                        _IN_ const char *product_key,
                        _IN_ const char *device_name,
                        _IN_ const char *service_id,
                        _IN_ const char *ouput_data);

/** @} */ /* end of subdev_api */

/** @} */ /* end of group_api */


#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_SUBDEV_EXPORT_H__ */
