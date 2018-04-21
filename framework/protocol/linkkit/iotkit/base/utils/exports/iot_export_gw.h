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



#ifndef __IOT_GW_EXPORT_H__
#define __IOT_GW_EXPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _PLATFORM_IS_LINUX_
#define HAS_STDINT_H    1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAS_STDINT_H
#include <stdint.h>
#else
typedef signed char             int8_t;
typedef signed short            int16_t;
typedef signed int              int32_t;
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;
#endif

#define _IN_                /* 参数前缀,    表示后面跟着的参数是一个输入参数 */
#define _OU_                /* 参数前缀,    表示后面跟着的参数是一个输出参数 */
#define _IN_OPT_            /* 参数前缀,    表示后面跟着的参数是一个可选的输入参数 */
#define _OU_OPT_            /* 参数前缀,    表示后面跟着的参数是一个可选的输出参数 */
#define IOT_TRUE    (1)     /* 逻辑真 */
#define IOT_FALSE   (0)     /* 逻辑假 */
#define IOT_SUCCESS (0)     /* 返回成功 */
#define IOT_FAILURE (-1)    /* 返回失败 */

#define DEVICE_THING_ID_LEN  (32)


/** @defgroup group_gw_api gw_api
 *  @{
 */

/** @defgroup group_typedef typedef
 */

/* IOT SDK log level */
typedef enum {
    IOT_LOG_EMERG = 0,
    IOT_LOG_CRIT,
    IOT_LOG_ERROR,
    IOT_LOG_WARNING,
    IOT_LOG_INFO,
    IOT_LOG_DEBUG,
} iot_log_level;


/* key-value data type */
typedef enum {
    IOT_DATA_TYPE_INT = 0,
    IOT_DATA_TYPE_FLOAT,
    IOT_DATA_TYPE_BOOL,
    IOT_DATA_TYPE_ENUM,
    IOT_DATA_TYPE_STRING,
    IOT_DATA_TYPE_JSON_STRUCT,
    IOT_DATA_TYPE_JSON_ARRAY
} iot_data_types_t;


/* The structure of key-value*/
typedef struct {
    char *key;
    iot_data_types_t data_type;
    union {
        int val_int;
        float val_float;
        bool val_bool;
        char *val_enum;
        char *val_string;
        char *val_json_struct;
        char *val_json_array;
    } value;
} iot_kv_info_t, *iot_kv_info_pt;


/* gateway callback type */
typedef enum {
    /* get gateway propertys */
    GW_CB_PROPERTY_GET = 0,

    /* set gateway propertys */
    GW_CB_PROPERTY_SET,

    /* invode gateway service */
    GW_CB_SERVICE_INVOKE,

    /* connected AliYun cloud */
    GW_CB_CLOUD_CONNECTED,

    /* disconnected AliYun cloud */
    GW_CB_CLOUD_DISCONNECTED,

    /* Maximum number of callback */
    GW_CB_CALLBACK_MAX
} iot_gw_cb_types_t;


/* The structure of service callback */
typedef struct {
    unsigned int cb_type;
    void *cb_func;
} iot_service_callback_t, *iot_service_callback_pt;


/** @} */ //end of group_typedef

/** @defgroup group_gateway_callback callback
 */

/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when get gateway property.
 * @param[in] property_id[]:    @n NULL-terminated array which hold the property name.
 * @param[out] output_params:   @n Json format property key-value.
 *                  The caller must release this buffer.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*gateway_cb_property_get_fpt)(_IN_ const char *property_id[],
                        _OU_ char **output_params);


/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when set gateway property.
 * @param[in] property_kv[]:@n The array of property key-value struct.
 * @param[in] array_sz:     @n The property array size.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*gateway_cb_property_set_fpt)(_IN_ iot_kv_info_t property_kv[],
                        _IN_ int array_sz);


/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when invode gateway service.
 * @param[in] service_id:       @n The service identifier,maximum length 32 bytes.
 * @param[in] input_params:     @n The service input params,json object format string.
 * @param[out] output_params:   @n The service output params,json object format string.
 *                  The caller must release this buffer.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*gateway_cb_service_invoke_fpt)(_IN_ const char *service_id,
                        _IN_ const char *input_params,
                        _OU_ char **output_params);



/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when connected Aliyun cloud.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*gateway_cb_cloud_connnected_fpt)(void);


/**
 *
 * @brief It define a datatype of function pointer.
 *          This type of function will be called when disconnected Aliyun cloud.
 * @retval  0 on success, otherwise -1 will return.
 */
typedef int (*gateway_cb_cloud_disconnnected_fpt)(void);

/** @} */ //end of group_gateway_callback


/**
 * @brief Register gateway service callback to IOT SDK.
 *
 * @param [in] service_callback: @n The array of struct pointer to gateway service callback.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_GatewaySrvCbRegister(
                            _IN_ iot_service_callback_pt service_callback[GW_CB_CALLBACK_MAX]);
/**
 * @brief Began to print log information.
 *
 * @param [in] ident: module name.
 *
 * @return None.
 * @see None.
 */
void    IOT_LogOpen(_IN_ const char *ident);


/**
 * @brief Stop print log information.
 *
 * @return None.
 * @see None.
 */
void    IOT_LogClose(void);


/**
 * @brief Set printer level.
 *
 * @param [in] log_level: @n level from 1 to 5, the greater the number, the more detailed the printing.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_LogLevelSet(_IN_ iot_log_level log_level);


/**
 * @brief IOT sdk init.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_ConnInit(void);


/**
 * @brief IOT SDK deinit.
 *
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_ConnDeinit(void);


int     IOT_ThingIdGet(_IN_ const char *product_key,
                       _IN_ const char *device_name,
                       _OU_ char *thing_id[DEVICE_THING_ID_LEN + 1])


/**
 * @brief Report gateway/sub device propertys to AliYun cloud.
 *
 * @param [in] product_key:     @n The product key,maximum length 20 bytes.
 * @param [in] device_name:     @n The unique device identifier,maximum length 32 bytes.
 * @param [in] property_kv[]:   @n The array of property key-value struct.
 * @param [in] array_sz:        @n The property kv array size.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_PropertyReport(_IN_ const char *product_key,
                        _IN_ const char *device_name,
                        _IN_ iot_kv_info_t property_kv[],
                        _IN_ int array_sz);

/**
 * @brief Report gateway/sub device event to AliYun cloud.
 *
 * @param [in] product_key: @n The product key,maximum length 20 bytes.
 * @param [in] device_name: @n The unique device identifier,maximum length 32 bytes.
 * @param [in] event_id:    @n The event name string,maximum length 32 bytes.
 * @param [in] event_params:@n The event params ,json object format string.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_EventReport(_IN_ const char *product_key,
                        _IN_ const char *device_name,
                        _IN_ const char *event_id,
                        _IN_ const char *event_params);

/**
 * @brief Update gateway/sub device extension key-value infomation to AliYun cloud.
 *
 * @param [in] product_key: @n The product key,maximum length 20 bytes.
 * @param [in] device_name: @n The unique device identifier,maximum length 32 bytes.
 * @param [in] attr_kv[]:   @n The array of extend attribute key-value struct.
 * @param [in] array_sz:    @n The extend attribute kv array size.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_ExtendInfoUpdate(_IN_ const char *product_key,
                        _IN_ const char *device_name,
                        _IN_ iot_kv_info_t attr_kv[],
                        _IN_ int array_sz);

/**
 * @brief Delete gateway/sub device extension infomation from AliYun cloud.
 *
 * @param [in] product_key: @n The product key,maximum length 20 bytes.
 * @param [in] device_name: @n The unique device identifier,maximum length 32 bytes.
 * @param [in] attr_key:    @n The ptr array of attribute key,
 *                  NULL-terminated array which hold the 'attr_key'.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_ExtendInfoDelete(_IN_ const char *product_key,
                        _IN_ const char *device_name,
                        _IN_ const char *attr_key[]);


/** @} */ /* end of group_gw_api */

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_GW_H__ */
