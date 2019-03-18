/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_EXPORT_H__
#define __IOT_EXPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#undef  being_deprecated
#define being_deprecated

#ifdef _WIN32
#if !defined(CC_IS_MINGW32)
#ifdef DLL_IOT_EXPORTS
#define DLL_IOT_API __declspec(dllexport)
#else
#define DLL_IOT_API __declspec(dllimport)
#endif
#else
#define DLL_IOT_API
#endif
#else
#define DLL_IOT_API
#endif

#include <stdint.h>

/* From device.h */
#define PRODUCT_KEY_LEN     (20)
#define DEVICE_NAME_LEN     (32)
#define DEVICE_ID_LEN       (64)
#define DEVICE_SECRET_LEN   (64)
#define PRODUCT_SECRET_LEN  (64)

#define PRODUCT_KEY_MAXLEN          (20 + 1)
#define DEVICE_NAME_MAXLEN          (32 + 1)
#define DEVICE_ID_MAXLEN            (64 + 1)
#define DEVICE_SECRET_MAXLEN        (64 + 1)
#define PRODUCT_SECRET_MAXLEN       (64 + 1)

#define LINKKIT_VERSION     "2.3.0"
#define MODULE_VENDOR_ID    (32)    /* Partner ID */

#define HOST_ADDRESS_LEN    (128)
#define HOST_PORT_LEN       (8)
#define CLIENT_ID_LEN       (256)
#define USER_NAME_LEN       (512)   /* Extend length for ID2 */
#define PASSWORD_LEN        (256)   /* Extend length for ID2 */
#define AESKEY_STR_LEN      (32)
#define AESKEY_HEX_LEN      (128/8)

typedef enum _IOT_LogLevel {
    IOT_LOG_NONE = 0,
    IOT_LOG_CRIT,
    IOT_LOG_ERROR,
    IOT_LOG_WARNING,
    IOT_LOG_INFO,
    IOT_LOG_DEBUG,
} IOT_LogLevel;

/* region type */
typedef enum IOTX_CLOUD_REGION_TYPES {
    /* Shanghai */
    IOTX_CLOUD_REGION_SHANGHAI,

    /* Singapore */
    IOTX_CLOUD_REGION_SINGAPORE,

    /* Japan */
    IOTX_CLOUD_REGION_JAPAN,

    /* America */
    IOTX_CLOUD_REGION_USA_WEST,

    /* Germany */
    IOTX_CLOUD_REGION_GERMANY,

    /* Custom setting */
    IOTX_CLOUD_REGION_CUSTOM,

    /* Maximum number of domain */
    IOTX_CLOUD_DOMAIN_MAX
} iotx_cloud_region_types_t;

typedef struct {
    char        product_key[PRODUCT_KEY_LEN + 1];
    char        device_name[DEVICE_NAME_LEN + 1];
    char        device_id[DEVICE_ID_LEN + 1];
    char        device_secret[DEVICE_SECRET_LEN + 1];
    char        module_vendor_id[MODULE_VENDOR_ID + 1];
} iotx_device_info_t;

typedef struct {
    uint16_t        port;
    uint8_t         init;
    char            *host_name;
    char            *client_id;
    char            *username;
    char            *password;
    const char      *pub_key;

} iotx_conn_info_t, *iotx_conn_info_pt;

/* data srutct define for IOTX_IOCTL_SET_SUBDEV_SIGN */
typedef struct {
    int         devid;
    const char *sign;
} iotx_ioctl_set_subdev_sign_t;

/* data struct define for IOTX_IOCTL_GET_SUBDEV_LOGIN */
typedef struct {
    int         devid;
    int         status;
} iotx_ioctl_get_subdev_info_t;

typedef enum {
    IOTX_IOCTL_SET_REGION,              /* value(int*): iotx_cloud_region_types_t */
    IOTX_IOCTL_GET_REGION,              /* value(int*) */
    IOTX_IOCTL_SET_MQTT_DOMAIN,         /* value(const char*): point to mqtt domain string */
    IOTX_IOCTL_SET_HTTP_DOMAIN,         /* value(const char*): point to http domain string */
    IOTX_IOCTL_SET_DYNAMIC_REGISTER,    /* value(int*): 0 - Disable Dynamic Register, 1 - Enable Dynamic Register */
    IOTX_IOCTL_GET_DYNAMIC_REGISTER,    /* value(int*) */
    IOTX_IOCTL_RECV_PROP_REPLY,         /* value(int*): 0 - Disable property post reply by cloud; 1 - Enable property post reply by cloud */
    IOTX_IOCTL_RECV_EVENT_REPLY,        /* value(int*): 0 - Disable event post reply by cloud; 1 - Enable event post reply by cloud */
    IOTX_IOCTL_SEND_PROP_SET_REPLY,     /* value(int*): 0 - Disable send post set reply by devid; 1 - Enable property set reply by devid */
    IOTX_IOCTL_SET_SUBDEV_SIGN,         /* value(const char*): only for slave device, set signature of subdevice */
    IOTX_IOCTL_GET_SUBDEV_LOGIN,        /* value(int*): 0 - SubDev is logout; 1 - SubDev is login */
    IOTX_IOCTL_SET_OTA_DEV_ID           /* value(int*):     select the device to do OTA according to devid */
} iotx_ioctl_option_t;

typedef enum {
    ITE_AWSS_STATUS,
    ITE_CONNECT_SUCC,
    ITE_CONNECT_FAIL,
    ITE_DISCONNECTED,
    ITE_RAWDATA_ARRIVED,
    ITE_SERVICE_REQUST,
    ITE_PROPERTY_SET,
    ITE_PROPERTY_GET,
#ifdef DEVICE_MODEL_SHADOW
    ITE_PROPERTY_DESIRED_GET_REPLY,
#endif
    ITE_REPORT_REPLY,
    ITE_TRIGGER_EVENT_REPLY,
    ITE_TIMESTAMP_REPLY,
    ITE_TOPOLIST_REPLY,
    ITE_PERMIT_JOIN,
    ITE_INITIALIZE_COMPLETED,
    ITE_FOTA,
    ITE_COTA,
    ITE_MQTT_CONNECT_SUCC
} iotx_ioctl_event_t;

#define IOT_RegisterCallback(evt, cb)           iotx_register_for_##evt(cb);
#define DECLARE_EVENT_CALLBACK(evt, cb)         DLL_IOT_API int iotx_register_for_##evt(cb);
#define DEFINE_EVENT_CALLBACK(evt, cb)          DLL_IOT_API int iotx_register_for_##evt(cb) { \
        if (evt < 0 || evt >= sizeof(g_impl_event_map)/sizeof(impl_event_map_t)) {return -1;} \
        g_impl_event_map[evt].callback = (void *)callback;return 0;}

DECLARE_EVENT_CALLBACK(ITE_AWSS_STATUS,          int (*cb)(int))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_SUCC,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_FAIL,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_DISCONNECTED,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_RAWDATA_ARRIVED,      int (*cb)(const int, const unsigned char *, const int))
DECLARE_EVENT_CALLBACK(ITE_SERVICE_REQUST,       int (*cb)(const int, const char *, const int, const char *, const int,
                       char **, int *))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_SET,         int (*cb)(const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_DESIRED_GET_REPLY,         int (*cb)(const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_GET,         int (*cb)(const int, const char *, const int, char **, int *))
DECLARE_EVENT_CALLBACK(ITE_REPORT_REPLY,         int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TRIGGER_EVENT_REPLY,  int (*cb)(const int, const int, const int, const char *, const int,
                       const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TIMESTAMP_REPLY,      int (*cb)(const char *))
DECLARE_EVENT_CALLBACK(ITE_TOPOLIST_REPLY,       int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PERMIT_JOIN,          int (*cb)(const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_INITIALIZE_COMPLETED, int (*cb)(const int))
DECLARE_EVENT_CALLBACK(ITE_FOTA,                 int (*cb)(const int, const char *))
DECLARE_EVENT_CALLBACK(ITE_COTA,                 int (*cb)(const int, const char *, int, const char *, const char *,
                       const char *, const char *))
DECLARE_EVENT_CALLBACK(ITE_MQTT_CONNECT_SUCC,    int (*cb)(void))

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_log log
 *  @{
 */

/**
 * @brief Set the print level.
 *
 * @param [in] level: @n level from 1 to 5, the greater the number, the more detailed the printing.
 *
 * @return None.
 * @see None.
 */
DLL_IOT_API void IOT_SetLogLevel(IOT_LogLevel level);

/**
 * @brief Print the memory usage statistics.
 *
 * @param [in] level: @n level from 1 to 5, the greater the number, the more detailed the printing.
 *
 * @return None.
 * @see None.
 */
DLL_IOT_API void IOT_DumpMemoryStats(IOT_LogLevel level);

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
DLL_IOT_API int IOT_SetupConnInfo(const char *product_key,
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
DLL_IOT_API int IOT_Ioctl(int option, void *data);

/** @} */ /* end of api_conninfo */

/** @} */ /* end of api */

/* #include "iot_export_compat.h" */
#include "iot_export_errno.h"

#if defined(DEV_BIND_ENABLED)
#include "iot_export_awss.h"
#endif

#if defined(HTTP_COMM_ENABLED)
#include "iot_export_http.h"
#endif

#if defined(UTILS_EVENT)
#include "iot_export_event.h"
#endif

#if defined(HTTP2_COMM_ENABLED)
#include "iot_export_http2.h"
#include "iot_export_http2_stream.h"
#endif

#if defined(MQTT_COMM_ENABLED) || defined(MAL_ENABLED)
#include "iot_export_mqtt.h"
#endif

#if defined(MQTT_SHADOW)
#include "iot_export_shadow.h"
#endif

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
#include "iot_export_ota.h"
#endif

#if defined(COAP_COMM_ENABLED)
#include "iot_export_coap.h"
#endif

#if defined(DEVICE_MODEL_ENABLED) && !defined(DEPRECATED_LINKKIT)
#include "iot_export_linkkit.h"
#endif

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_H__ */
