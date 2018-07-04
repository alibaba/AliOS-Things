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



#ifndef __IOT_EXPORT_H__
#define __IOT_EXPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef enum _IOT_LogLevel {
    IOT_LOG_EMERG = 0,
    IOT_LOG_CRIT,
    IOT_LOG_ERROR,
    IOT_LOG_WARNING,
    IOT_LOG_INFO,
    IOT_LOG_DEBUG,
} IOT_LogLevel;

/* From device.h */
#define PRODUCT_KEY_LEN     (20)
#define DEVICE_NAME_LEN     (32)
#define DEVICE_ID_LEN       (64)
#define DEVICE_SECRET_LEN   (64)
#define PRODUCT_SECRET_LEN  (64)

#define MODULE_VENDOR_ID    (32)    /* Partner ID */

#define HOST_ADDRESS_LEN    (128)
#define HOST_PORT_LEN       (8)
#define CLIENT_ID_LEN       (256)
#define USER_NAME_LEN       (512)   /* Extend length for ID2 */
#define PASSWORD_LEN        (256)   /* Extend length for ID2 */
#define AESKEY_STR_LEN      (32)
#define AESKEY_HEX_LEN      (128/8)

typedef struct {
    char        product_key[PRODUCT_KEY_LEN + 1];
    char        device_name[DEVICE_NAME_LEN + 1];
    char        device_id[DEVICE_ID_LEN + 1];
    char        device_secret[DEVICE_SECRET_LEN + 1];
    char        module_vendor_id[MODULE_VENDOR_ID + 1];
} iotx_device_info_t, *iotx_device_info_pt;

typedef struct {
    uint16_t        port;
    char            host_name[HOST_ADDRESS_LEN + 1];
    char            client_id[CLIENT_ID_LEN + 1];
    char            username[USER_NAME_LEN + 1];
    char            password[PASSWORD_LEN + 1];
    const char     *pub_key;
#ifdef MQTT_ID2_AUTH
    char            aeskey_str[AESKEY_STR_LEN];
    uint8_t         aeskey_hex[AESKEY_HEX_LEN];
#endif
} iotx_conn_info_t, *iotx_conn_info_pt;
/* From device.h */

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_log log
 *  @{
 */

/**
 * @brief Began to print log information.
 *
 * @param [in] ident: module name.
 *
 * @return None.
 * @see None.
 */
void    IOT_OpenLog(const char *ident);

/**
 * @brief Stop print log information.
 *
 * @return None.
 * @see None.
 */
void    IOT_CloseLog(void);

/**
 * @brief Set the print level.
 *
 * @param [in] level: @n level from 1 to 5, the greater the number, the more detailed the printing.
 *
 * @return None.
 * @see None.
 */
void    IOT_SetLogLevel(IOT_LogLevel level);

/**
 * @brief Print the memory usage statistics.
 *
 * @param [in] level: @n level from 1 to 5, the greater the number, the more detailed the printing.
 *
 * @return None.
 * @see None.
 */
void    IOT_DumpMemoryStats(IOT_LogLevel level);

/** @} */ /* end of api_log */

/** @defgroup group_api_conninfo conninfo
 *  @{
 */


/**
 * @brief Based on the 'product_key' + 'device_name' + 'device_secret' produce an MQTT connection username and password.
 *
 * @param [in] product_key: @n Apply for 'product_key' in the AliYun Console.
 * @param [in] device_name: @n Apply for 'device_name' in the AliYun Console.
 * @param [in] device_secret: @n Apply for 'device_secret' in the AliYun Console.
 * @param [out] info_ptr: @n return MQTT connection parameter.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */
int     IOT_SetupConnInfo(const char *product_key,
                          const char *device_name,
                          const char *device_secret,
                          void **info_ptr);
/**
 * @brief Based on the product_key + device_name + device_secret produce an MQTT connection username and password by ID2_AUTH.
 *
 * @param [in] product_key: @n Apply for 'product_key' in the AliYun Console.
 * @param [in] device_name: @n Apply for 'device_name' in the AliYun Console.
 * @param [in] device_secret: @n Apply for 'device_secret' in the AliYun Console.
 * @param [out] info_ptr: @n return MQTT connection parameter.
 *
 * @retval -1 : Fail.
 * @retval  0 : Success.
 * @see None.
 */

int     IOT_SetupConnInfoSecure(const char *product_key,
                                const char *device_name,
                                const char *device_secret,
                                void **info_ptr);
/** @} */ /* end of api_conninfo */

/** @} */ /* end of api */

#include "exports/iot_export_errno.h"
#include "exports/iot_export_mqtt.h"
#include "exports/iot_export_shadow.h"
#include "exports/iot_export_coap.h"
#include "exports/iot_export_ota.h"
#include "exports/iot_export_http.h"
#ifdef SUBDEVICE_ENABLED
#include "exports/iot_export_subdev.h"
#endif /* SUBDEVICE_ENABLED */
#ifdef CLOUD_CONN_ENABLED
#include "exports/iot_export_cloud_connection.h"
#endif /* CLOUD_CONN_ENABLED */
#ifdef CMP_ENABLED
#include "exports/iot_export_cm.h"
#endif /* CMP_ENABLED */
//#ifdef SERVICE_OTA_ENABLED
#include "exports/iot_export_fota.h"
//#endif /* SERVICE_OTA_ENABLED */


#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_H__ */
