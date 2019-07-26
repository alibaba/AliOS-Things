#include "infra_config.h"

#ifdef INFRA_COMPAT
#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers.h"

#if !defined(INFRA_LOG)
void IOT_SetLogLevel(IOT_LogLevel level) {}
#endif

#if defined(WIFI_PROVISION_ENABLED)
    extern void awss_set_press_timeout_ms(unsigned int timeout_ms);
    extern void awss_set_channel_scan_interval_ms(uint32_t timeout_ms);
#endif

#ifdef MQTT_COMM_ENABLED
#include "linkkit/dev_sign_api.h"
#include "linkkit/mqtt_api.h"

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define sdk_err(...)       log_err("infra_compat", __VA_ARGS__)
    #define sdk_info(...)      log_info("infra_compat", __VA_ARGS__)
#else
    #define sdk_err(...)
    #define sdk_info(...)
#endif

sdk_impl_ctx_t g_sdk_impl_ctx = {0};
/* global variable for mqtt construction */
static iotx_conn_info_t g_iotx_conn_info = {0};
static char g_empty_string[1] = "";

int IOT_SetupConnInfo(const char *product_key,
                      const char *device_name,
                      const char *device_secret,
                      void **info_ptr)
{
    if (product_key == NULL || device_name == NULL || device_secret == NULL ||
        strlen(product_key) > IOTX_PRODUCT_KEY_LEN ||
        strlen(device_name) > IOTX_DEVICE_NAME_LEN ||
        strlen(device_secret) > IOTX_DEVICE_SECRET_LEN) {
        return NULL_VALUE_ERROR;
    }

    if (info_ptr) {
        memset(&g_iotx_conn_info, 0, sizeof(iotx_conn_info_t));
        g_iotx_conn_info.host_name = g_empty_string;
        g_iotx_conn_info.client_id = g_empty_string;
        g_iotx_conn_info.username = g_empty_string;
        g_iotx_conn_info.password = g_empty_string;
        g_iotx_conn_info.pub_key = g_empty_string;

        *info_ptr = &g_iotx_conn_info;
    }
    return SUCCESS_RETURN;
}
#endif /* #ifdef MQTT_COMM_ENABLED */

#if defined(DEVICE_MODEL_ENABLED)
    #include "iotx_dm.h"
    #include "dm_opt.h"
#endif

#if defined(DEVICE_MODEL_GATEWAY)
extern int iot_linkkit_subdev_query_id(char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                       char device_name[IOTX_DEVICE_NAME_LEN + 1]);
#endif

int IOT_Ioctl(int option, void *data)
{
    int                 res = SUCCESS_RETURN;
    sdk_impl_ctx_t     *ctx = NULL;

    ctx = &g_sdk_impl_ctx;

    if (option < 0 || data == NULL) {
        return FAIL_RETURN;
    }

    switch (option) {
        case IOTX_IOCTL_SET_REGION: {
            ctx->domain_type = *(int *)data;
            /* iotx_guider_set_region(*(int *)data); */

            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_GET_REGION: {
            *(int *)data = ctx->domain_type;

            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_SET_MQTT_DOMAIN: {
            ctx->domain_type = IOTX_CLOUD_REGION_CUSTOM;

            if (ctx->cloud_custom_domain) {
                HAL_Free(ctx->cloud_custom_domain);
                ctx->cloud_custom_domain = NULL;
                g_infra_mqtt_domain[IOTX_CLOUD_REGION_CUSTOM] = NULL;
            }
            ctx->cloud_custom_domain = HAL_Malloc(strlen((char *)data) + 1);
            if (ctx->cloud_custom_domain == NULL) {
                return FAIL_RETURN;
            }
            memset(ctx->cloud_custom_domain, 0, strlen((char *)data) + 1);
            memcpy(ctx->cloud_custom_domain, data, strlen((char *)data));
            g_infra_mqtt_domain[IOTX_CLOUD_REGION_CUSTOM] = (const char *)ctx->cloud_custom_domain;
            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_SET_HTTP_DOMAIN: {
            ctx->domain_type = IOTX_HTTP_REGION_CUSTOM;

            if (ctx->http_custom_domain) {
                HAL_Free(ctx->http_custom_domain);
                ctx->http_custom_domain = NULL;
                g_infra_http_domain[IOTX_CLOUD_REGION_CUSTOM] = NULL;
            }
            ctx->http_custom_domain = HAL_Malloc(strlen((char *)data) + 1);
            if (ctx->http_custom_domain == NULL) {
                return FAIL_RETURN;
            }
            memset(ctx->http_custom_domain, 0, strlen((char *)data) + 1);
            memcpy(ctx->http_custom_domain, data, strlen((char *)data));
            g_infra_http_domain[IOTX_CLOUD_REGION_CUSTOM] = (const char *)ctx->http_custom_domain;
            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_SET_DYNAMIC_REGISTER: {
            ctx->dynamic_register = *(int *)data;

            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_GET_DYNAMIC_REGISTER: {
            *(int *)data = ctx->dynamic_register;

            res = SUCCESS_RETURN;
        }
        break;
#if defined(DEVICE_MODEL_ENABLED) && !defined(DEPRECATED_LINKKIT)
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case IOTX_IOCTL_RECV_EVENT_REPLY:
        case IOTX_IOCTL_RECV_PROP_REPLY: {
            res = iotx_dm_set_opt(IMPL_LINKKIT_IOCTL_SWITCH_EVENT_POST_REPLY, data);
        }
        break;
        case IOTX_IOCTL_SEND_PROP_SET_REPLY : {
            res = iotx_dm_set_opt(IMPL_LINKKIT_IOCTL_SWITCH_PROPERTY_SET_REPLY, data);
        }
        break;
#endif
        case IOTX_IOCTL_SET_SUBDEV_SIGN: {
            /* todo */
        }
        break;
        case IOTX_IOCTL_GET_SUBDEV_LOGIN: {
            /* todo */
        }
        break;
#if defined(DEVICE_MODEL_GATEWAY)
#ifdef DEVICE_MODEL_SUBDEV_OTA
        case IOTX_IOCTL_SET_OTA_DEV_ID: {
            int devid = *(int *)(data);
            res = iotx_dm_ota_switch_device(devid);
        }
        break;
#endif
#endif
#else
        case IOTX_IOCTL_RECV_EVENT_REPLY:
        case IOTX_IOCTL_RECV_PROP_REPLY:
        case IOTX_IOCTL_SEND_PROP_SET_REPLY:
        case IOTX_IOCTL_GET_SUBDEV_LOGIN: {
            res = SUCCESS_RETURN;
        }
        break;
#endif
#if defined(DEVICE_MODEL_ENABLED)
        case IOTX_IOCTL_FOTA_TIMEOUT_MS: {
            res = iotx_dm_set_opt(DM_OPT_FOTA_RETRY_TIMEOUT_MS, data);
        }
        break;
#endif
        case IOTX_IOCTL_SET_CUSTOMIZE_INFO: {
            if (ctx->mqtt_customzie_info) {
                HAL_Free(ctx->mqtt_customzie_info);
                ctx->mqtt_customzie_info = NULL;
            }
            ctx->mqtt_customzie_info = HAL_Malloc(strlen((char *)data) + 1);
            if (ctx->mqtt_customzie_info == NULL) {
                return FAIL_RETURN;
            }
            memset(ctx->mqtt_customzie_info, 0, strlen((char *)data) + 1);
            memcpy(ctx->mqtt_customzie_info, data, strlen((char *)data));
            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_SET_MQTT_PORT: {
            ctx->mqtt_port_num = *(uint16_t *)data;
            res = SUCCESS_RETURN;
        }
        break;
#if defined(DEVICE_MODEL_GATEWAY) && !defined(DEPRECATED_LINKKIT)
        case IOTX_IOCTL_SET_PROXY_REGISTER: {
            res = iotx_dm_set_opt(DM_OPT_PROXY_PRODUCT_REGISTER, data);
        }
        break;
        case IOTX_IOCTL_QUERY_DEVID: {
            iotx_dev_meta_info_t *dev_info = (iotx_dev_meta_info_t *)data;

            res = iot_linkkit_subdev_query_id(dev_info->product_key, dev_info->device_name);
        }
        break;
#endif
#if defined(WIFI_PROVISION_ENABLED)
        case IOTX_IOCTL_SET_AWSS_ENABLE_INTERVAL: {
            uint32_t timeout = *(uint32_t *) data;
            awss_set_press_timeout_ms(timeout);
        }
        break;
        case IOTX_IOCTL_SET_AWSS_CHANNEL_SCAN_INTERVAL: {
            uint32_t timeout = *(uint32_t *) data;
            awss_set_channel_scan_interval_ms(timeout);
        }
        break;
#endif
#if defined(DEVICE_MODEL_ENABLED)
        case IOTX_IOCTL_SUB_USER_TOPIC: {
            iotx_user_subscribe_context *context = (iotx_user_subscribe_context *) data;
            iotx_dm_subscribe_user_topic((char *)context->topic, (void *)context->callback);
        }
        break;
#endif
        default: {
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}

/* release memory allocated in ioctl */
void iotx_ioctl_clean(void)
{
    sdk_impl_ctx_t *ctx = &g_sdk_impl_ctx;

    if (ctx->cloud_custom_domain) {
        HAL_Free(ctx->cloud_custom_domain);
        ctx->cloud_custom_domain = NULL;
    }

    if (ctx->http_custom_domain) {
        HAL_Free(ctx->http_custom_domain);
        ctx->http_custom_domain = NULL;
    }

    if (ctx->mqtt_customzie_info) {
        HAL_Free(ctx->mqtt_customzie_info);
        ctx->mqtt_customzie_info = NULL;
    }
}

void IOT_DumpMemoryStats(IOT_LogLevel level)
{
#ifdef INFRA_MEM_STATS
    int             lvl = (int)level;

    if (lvl > LOG_DEBUG_LEVEL) {
        lvl = LOG_DEBUG_LEVEL;
        HAL_Printf("Invalid input level, using default: %d => %d", level, lvl);
    }

    LITE_dump_malloc_free_stats(lvl);
#endif
}

static void *g_event_monitor = NULL;

int iotx_event_regist_cb(void (*monitor_cb)(int event))
{
    g_event_monitor = (void *)monitor_cb;
    return 0;
}

int iotx_event_post(int event)
{
    if (g_event_monitor == NULL) {
        return -1;
    }
    ((void (*)(int))g_event_monitor)(event);
    return 0;
}

typedef struct {
    int eventid;
    void *callback;
} impl_event_map_t;

static impl_event_map_t g_impl_event_map[] = {
    {ITE_AWSS_STATUS,          NULL},
    {ITE_CONNECT_SUCC,         NULL},
    {ITE_CONNECT_FAIL,         NULL},
    {ITE_DISCONNECTED,         NULL},
    {ITE_RAWDATA_ARRIVED,      NULL},
    {ITE_SERVICE_REQUEST,      NULL},
    {ITE_SERVICE_REQUEST_EXT,  NULL},
    {ITE_PROPERTY_SET,         NULL},
    {ITE_PROPERTY_GET,         NULL},
#ifdef DEVICE_MODEL_SHADOW
    {ITE_PROPERTY_DESIRED_GET_REPLY,         NULL},
#endif
    {ITE_REPORT_REPLY,         NULL},
    {ITE_TRIGGER_EVENT_REPLY,  NULL},
    {ITE_TIMESTAMP_REPLY,      NULL},
    {ITE_TOPOLIST_REPLY,       NULL},
    {ITE_PERMIT_JOIN,          NULL},
    {ITE_INITIALIZE_COMPLETED, NULL},
    {ITE_FOTA,                 NULL},
    {ITE_COTA,                 NULL},
    {ITE_MQTT_CONNECT_SUCC,    NULL},
    {ITE_CLOUD_ERROR,          NULL},
};

void *iotx_event_callback(int evt)
{
    if (evt < 0 || evt >= sizeof(g_impl_event_map) / sizeof(impl_event_map_t)) {
        return NULL;
    }
    return g_impl_event_map[evt].callback;
}

DEFINE_EVENT_CALLBACK(ITE_AWSS_STATUS,          int (*callback)(int))
DEFINE_EVENT_CALLBACK(ITE_CONNECT_SUCC,         int (*callback)(void))
DEFINE_EVENT_CALLBACK(ITE_CONNECT_FAIL,         int (*callback)(void))
DEFINE_EVENT_CALLBACK(ITE_DISCONNECTED,         int (*callback)(void))
DEFINE_EVENT_CALLBACK(ITE_RAWDATA_ARRIVED,      int (*callback)(const int, const unsigned char *, const int))
DEFINE_EVENT_CALLBACK(ITE_SERVICE_REQUEST,      int (*callback)(const int, const char *, const int, const char *,
                      const int, char **, int *))
DEFINE_EVENT_CALLBACK(ITE_SERVICE_REQUEST_EXT,  int (*callback)(int, const char *, int, const char *, int, const char *,
                      int, void *))
DEFINE_EVENT_CALLBACK(ITE_PROPERTY_SET,         int (*callback)(const int, const char *, const int))
#ifdef DEVICE_MODEL_SHADOW
    DEFINE_EVENT_CALLBACK(ITE_PROPERTY_DESIRED_GET_REPLY,         int (*callback)(const char *, const int))
#endif
DEFINE_EVENT_CALLBACK(ITE_PROPERTY_GET,         int (*callback)(const int, const char *, const int, char **, int *))
DEFINE_EVENT_CALLBACK(ITE_REPORT_REPLY,         int (*callback)(const int, const int, const int, const char *,
                      const int))
DEFINE_EVENT_CALLBACK(ITE_TRIGGER_EVENT_REPLY,  int (*callback)(const int, const int, const int, const char *,
                      const int, const char *, const int))
DEFINE_EVENT_CALLBACK(ITE_TIMESTAMP_REPLY,      int (*callback)(const char *))
DEFINE_EVENT_CALLBACK(ITE_TOPOLIST_REPLY,       int (*callback)(const int, const int, const int, const char *,
                      const int))
DEFINE_EVENT_CALLBACK(ITE_PERMIT_JOIN,          int (*callback)(const char *, int))
DEFINE_EVENT_CALLBACK(ITE_INITIALIZE_COMPLETED, int (*callback)(const int))
DEFINE_EVENT_CALLBACK(ITE_FOTA,                 int (*callback)(const int, const char *))
DEFINE_EVENT_CALLBACK(ITE_COTA,                 int (*callback)(const int, const char *, int, const char *,
                      const char *, const char *, const char *))
DEFINE_EVENT_CALLBACK(ITE_MQTT_CONNECT_SUCC,    int (*callback)(void))
DEFINE_EVENT_CALLBACK(ITE_CLOUD_ERROR,          int (*callback)(const int, const char *, const char *))

#endif
