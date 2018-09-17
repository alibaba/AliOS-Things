/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef __IOT_EXPORT_H__
#define __IOT_EXPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#undef  deprecated
#define deprecated

#include <stdint.h>

typedef enum _IOT_LogLevel {
    IOT_LOG_EMERG = 0,
    IOT_LOG_CRIT,
    IOT_LOG_ERROR,
    IOT_LOG_WARNING,
    IOT_LOG_INFO,
    IOT_LOG_DEBUG,
} IOT_LogLevel;

/* domain type */
typedef enum IOTX_CLOUD_DOMAIN_TYPES {
    /* Shanghai */
    IOTX_CLOUD_DOMAIN_SH,

    /* Singapore */
    IOTX_CLOUD_DOMAIN_SG,

    /* Japan */
    IOTX_CLOUD_DOMAIN_JP,

    /* America */
    IOTX_CLOUD_DOMAIN_US,

    /* Germany */
    IOTX_CLOUD_DOMAIN_GER,

    /* Maximum number of domain */
    IOTX_CLOUD_DOMAIN_MAX
} iotx_cloud_domain_types_t;


/* From device.h */
#define PRODUCT_KEY_LEN     (20)
#define DEVICE_NAME_LEN     (32)
#define DEVICE_ID_LEN       (64)
#define DEVICE_SECRET_LEN   (64)
#define PRODUCT_SECRET_LEN  (64)

#define LINKKIT_VERSION     "2.2.1"
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
} iotx_conn_info_t, *iotx_conn_info_pt;

typedef enum {
    IOTX_IOCTL_SET_DOMAIN,              /* value(int*): iotx_cloud_domain_types_t */
    IOTX_IOCTL_GET_DOMAIN,              /* value(int*) */
    IOTX_IOCTL_SET_DYNAMIC_REGISTER,    /* value(int*): 0 - Disable Dynamic Register, 1 - Enable Dynamic Register */
    IOTX_IOCTL_GET_DYNAMIC_REGISTER     /* value(int*) */
} iotx_ioctl_option_t;
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
 * @brief Setup Demain type, should be called before MQTT connection.
 *
 * @param [in] option: see iotx_ioctl_option_t.
 *
 * @return None.
 * @see None.
 */
int     IOT_Ioctl(int option, void *data);

/** @} */ /* end of api_conninfo */

/** @} */ /* end of api */

#include "exports/iot_export_errno.h"
#include "exports/iot_export_mqtt.h"
#include "exports/iot_export_shadow.h"
#include "exports/iot_export_coap.h"
#include "exports/iot_export_ota.h"
#include "exports/iot_export_http.h"
#include "exports/iot_export_event.h"
#include "exports/iot_export_subdev.h"

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_H__ */
