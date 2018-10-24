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

#include <stdint.h>

/* From device.h */
#define PRODUCT_KEY_LEN     (20)
#define DEVICE_NAME_LEN     (32)
#define DEVICE_ID_LEN       (64)
#define DEVICE_SECRET_LEN   (64)
#define PRODUCT_SECRET_LEN  (64)

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
} iotx_device_info_t, *iotx_device_info_pt;

typedef struct {
    uint16_t        port;
    char            host_name[HOST_ADDRESS_LEN + 1];
    char            client_id[CLIENT_ID_LEN + 1];
    char            username[USER_NAME_LEN + 1];
    char            password[PASSWORD_LEN + 1];
    const char     *pub_key;
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
    IOTX_IOCTL_SEND_PROP_REPLY,         /* value(int*): 0 - Disable send post set reply by devid; 1 - Enable property set reply by devid */
    IOTX_IOCTL_SET_SUBDEV_SIGN,         /* value(const char*): only for slave device, set signature of subdevice */
    IOTX_IOCTL_GET_SUBDEV_LOGIN         /* value(int*): 0 - SubDev is logout; 1 - SubDev is login */
} iotx_ioctl_option_t;

#define ITE_AWSS_STATUS            (0x0000)
#define ITE_CONNECT_SUCC           (0x0001)
#define ITE_CONNECT_FAIL           (0x0002)
#define ITE_DISCONNECTED           (0x0003)
#define ITE_RAWDATA_ARRIVED        (0x0004)
#define ITE_SERVICE_REQUST         (0x0005)
#define ITE_PROPERTY_SET           (0x0006)
#define ITE_PROPERTY_GET           (0x0007)
#define ITE_REPORT_REPLY           (0x0008)
#define ITE_TRIGGER_EVENT_REPLY    (0x0009)
#define ITE_TIMESTAMP_REPLY        (0x000A)
#define ITE_TOPOLIST_REPLY         (0x000B)
#define ITE_PERMIT_JOIN            (0x000C)
#define ITE_INITIALIZE_COMPLETED   (0x000D)
#define ITE_FOTA                   (0x000E)
#define ITE_COTA                   (0x000F)

#define IOT_RegisterCallback(evt, cb)           iotx_register_for_##evt(cb);
#define DECLARE_EVENT_CALLBACK(evt, cb)     int iotx_register_for_##evt(cb);
#define DEFINE_EVENT_CALLBACK(evt, cb)      int iotx_register_for_##evt(cb) { \
        if (evt < 0 || evt >= sizeof(g_impl_event_map)/sizeof(impl_event_map_t)) {return -1;} \
        g_impl_event_map[evt].callback = callback;return 0;}

DECLARE_EVENT_CALLBACK(ITE_AWSS_STATUS,          int (*cb)(int))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_SUCC,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_FAIL,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_DISCONNECTED,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_RAWDATA_ARRIVED,      int (*cb)(const int, const unsigned char *, const int))
DECLARE_EVENT_CALLBACK(ITE_SERVICE_REQUST,       int (*cb)(const int, const char *, const int, const char *, const int, char **, int *))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_SET,         int (*cb)(const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_GET,         int (*cb)(const int, const char *, const int, char **, int *))
DECLARE_EVENT_CALLBACK(ITE_REPORT_REPLY,         int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TRIGGER_EVENT_REPLY,  int (*cb)(const int, const int, const int, const char *, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TIMESTAMP_REPLY,      int (*cb)(const char *))
DECLARE_EVENT_CALLBACK(ITE_TOPOLIST_REPLY,       int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PERMIT_JOIN,          int (*cb)(const char *, int))
DECLARE_EVENT_CALLBACK(ITE_INITIALIZE_COMPLETED, int (*cb)(const int))
DECLARE_EVENT_CALLBACK(ITE_FOTA,                 int (*cb)(const int, const char *))
DECLARE_EVENT_CALLBACK(ITE_COTA,                 int (*cb)(const int, const char *, int, const char *, const char *, const char *, const char *))

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

#include "exports/iot_export_compat.h"
#include "exports/iot_export_errno.h"
#include "exports/iot_export_awss.h"
#include "exports/iot_export_mqtt.h"
#include "exports/iot_export_shadow.h"
#include "exports/iot_export_coap.h"
#include "exports/iot_export_ota.h"
#include "exports/iot_export_http.h"
#include "exports/iot_export_event.h"
#include "exports/iot_export_http2.h"
#include "exports/iot_export_http2_stream.h"

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_EXPORT_H__ */
