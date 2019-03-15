/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "sdk-impl_internal.h"

#if defined(DEVICE_MODEL_ENABLED) && !defined(DEPRECATED_LINKKIT)
    #include "iotx_dm.h"
#endif

#define KV_KEY_DEVICE_SECRET            "DyncRegDeviceSecret"

static sdk_impl_ctx_t g_sdk_impl_ctx = {0};

sdk_impl_ctx_t *sdk_impl_get_ctx(void)
{
    return &g_sdk_impl_ctx;
}

void IOT_SetLogLevel(IOT_LogLevel level)
{
    int             lvl = (int)level;

    if (lvl > LOG_DEBUG_LEVEL) {
        sdk_err("Invalid input level: %d out of [%d, %d]", level,
                LOG_NONE_LEVEL,
                LOG_DEBUG_LEVEL);
        return;
    }

    LITE_set_loglevel(lvl);
    HAL_Printf("[prt] log level set as: [ %d ]\r\n", lvl);
}

void IOT_DumpMemoryStats(IOT_LogLevel level)
{
    int             lvl = (int)level;

    if (lvl > LOG_DEBUG_LEVEL) {
        lvl = LOG_DEBUG_LEVEL;
        sdk_warning("Invalid input level, using default: %d => %d", level, lvl);
    }

    LITE_dump_malloc_free_stats(lvl);
}

int IOT_SetupConnInfo(const char *product_key,
                      const char *device_name,
                      const char *device_secret,
                      void **info_ptr)
{
    int                 rc = 0;
    char                device_secret_actual[DEVICE_SECRET_MAXLEN] = {0};
    sdk_impl_ctx_t     *ctx = sdk_impl_get_ctx();

    STRING_PTR_SANITY_CHECK(product_key, -1);
    STRING_PTR_SANITY_CHECK(device_name, -1);

    HAL_SetProductKey((char *)product_key);
    HAL_SetDeviceName((char *)device_name);
    HAL_SetDeviceSecret((char *)device_secret);

    /* Dynamic Register Device If Need */
    if (ctx->dynamic_register == 0) {
#if !defined(SUPPORT_ITLS)
        STRING_PTR_SANITY_CHECK(device_secret, -1);
        memcpy(device_secret_actual, device_secret, strlen(device_secret));
#else
        if (device_secret == NULL || strlen(device_secret) == 0) {
            LITE_get_randstr(device_secret_actual, DEVICE_SECRET_MAXLEN - 1);
        } else {
            memcpy(device_secret_actual, device_secret, strlen(device_secret));
        }
#endif
    } else {
#ifdef DYNAMIC_REGISTER
        int device_secret_len = DEVICE_SECRET_MAXLEN;

        /* Check if Device Secret exit in KV */
        if (HAL_Kv_Get(KV_KEY_DEVICE_SECRET, device_secret_actual, &device_secret_len) == 0) {
            sdk_info("Get DeviceSecret from KV succeed");

            *(device_secret_actual + device_secret_len) = 0;
            HAL_SetDeviceSecret(device_secret_actual);
        } else {
            char product_secret[PRODUCT_SECRET_MAXLEN] = {0};

            /* KV not exit, goto dynamic register */
            sdk_info("DeviceSecret KV not exist, Now We Need Dynamic Register...");

            /* Check If Product Secret Exist */
            HAL_GetProductSecret(product_secret);
            if (strlen(product_secret) == 0) {
                sdk_err("Product Secret Is Not Exist");
                return FAIL_RETURN;
            }
            STRING_PTR_SANITY_CHECK(product_secret, -1);

            rc = perform_dynamic_register((char *)product_key, (char *)product_secret, (char *)device_name, device_secret_actual);
            if (rc != SUCCESS_RETURN) {
                sdk_err("Dynamic Register Failed");
                return FAIL_RETURN;
            }

            device_secret_len = strlen(device_secret_actual);
            if (HAL_Kv_Set(KV_KEY_DEVICE_SECRET, device_secret_actual, device_secret_len, 1) != 0) {
                sdk_err("Save Device Secret to KV Failed");
                return FAIL_RETURN;
            }

            HAL_SetDeviceSecret(device_secret_actual);
        }
#else
        sdk_info("DeviceSecret KV not exist, But Dynamic Register Is Disabled");
        return FAIL_RETURN;
#endif
    }

    /* iotx_device_info_set(product_key, device_name, device_secret_actual); */

#if defined MQTT_COMM_ENABLED
    if (NULL == info_ptr) {
        return SUCCESS_RETURN;
    }
    *info_ptr = iotx_conn_info_reload();
    if (*info_ptr == NULL) {
        return -1;
    }

#endif
    return rc;
}

int IOT_Ioctl(int option, void *data)
{
    int                 res = SUCCESS_RETURN;
    sdk_impl_ctx_t     *ctx = sdk_impl_get_ctx();

    if (option < 0 || data == NULL) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    switch (option) {
        case IOTX_IOCTL_SET_REGION: {
            ctx->domain_type = *(int *)data;
            iotx_guider_set_region(*(int *)data);

            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_GET_REGION: {
            *(int *)data = ctx->domain_type;

            res = SUCCESS_RETURN;
        }
        break;
        case IOTX_IOCTL_SET_MQTT_DOMAIN: {
            ctx->domain_type = GUIDER_REGION_CUSTOM;
            iotx_guider_set_region(GUIDER_REGION_CUSTOM);

            res = iotx_guider_set_custom_domain(GUIDER_DOMAIN_MQTT, (const char *)data);
        }
        break;
        case IOTX_IOCTL_SET_HTTP_DOMAIN: {
            ctx->domain_type = GUIDER_REGION_CUSTOM;
            iotx_guider_set_region(GUIDER_REGION_CUSTOM);

            res = iotx_guider_set_custom_domain(GUIDER_DOMAIN_HTTP, (const char *)data);
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
#ifdef DEVICE_MODEL_GATEWAY
#ifdef DEVICE_MODEL_SUBDEV_OTA
        case IOTX_IOCTL_SET_OTA_DEV_ID: {
            int devid = *(int *)(data);
            res = iotx_dm_ota_switch_device(devid);
        }
        break;
#endif
#endif
#endif
        default: {
            sdk_err("Unknown Ioctl Option");
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}
