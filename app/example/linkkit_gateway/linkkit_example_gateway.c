/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef DEPRECATED_LINKKIT
#include "deprecated/gateway.c"
#else
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "cJSON.h"
#include "iot_import.h"
#include "iot_export_linkkit.h"
#include "app_entry.h"

#ifdef LINKKIT_GATEWAY_TEST_CMD
    #include "simulate_subdev/testcmd.h"
#endif

#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    #include "ota_service.h"
#endif

// for demo only
#define PRODUCT_KEY      "a1RIsMLz2BJ"
#define PRODUCT_SECRET   "fSAF0hle6xL0oRWd"
#define DEVICE_NAME      "example1"
#define DEVICE_SECRET    "RDXf67itLqZCwdMCRrw0N5FHbv5D7jrE"

#define USER_EXAMPLE_YIELD_TIMEOUT_MS (200)

#define EXAMPLE_TRACE(...)                               \
    do {                                                     \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\033[0m\r\n");                                   \
    } while (0)

#define EXAMPLE_SUBDEV_MAX_NUM          20
const iotx_linkkit_dev_meta_info_t subdevArr[EXAMPLE_SUBDEV_MAX_NUM] = {
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_01",
        "qHLwZxOH5hwm0ApWVRXZbSxFzRUUddFc"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_02",
        "jFsErM3uA7UfbS6J0hm0QaEXsQbmO6Pa"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_03",
        "MjWMvCLBcuZyqUswryBbgypN8uOgJGVD"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_04",
        "DXbcbpxepIyYm3BiS0ICdBou4uWPfP6L"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_05",
        "VKuVZfcz3umcoR3WhOp4cu1p2dyTQGq1"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_06",
        "QTobiz1BdGW5XNgLGIgNSylH0btVvvGS"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_07",
        "IX7ol50rRS2uP8V74jt0DKfmYn8iC6h1"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_08",
        "sbFxx62evXVoVgJ5gL2oCLcz1pX9d6K2"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_09",
        "S0CpOl54GZxEO7Gz5DWQa5YxgUMfT4xA"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_10",
        "MowJJjiNIkTdUcX5fCNUDu39Yz02KADL"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_11",
        "BrJNdAihVznMWTpdRfe8HIiI95ubSYdN"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_12",
        "Yd3ZHK8D6cAKKRQb9rUevCfwPf7atoQ4"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_13",
        "d3HKvu2eBR5ytcgDaBEt0gpvJZlu9W0g"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_14",
        "qAJYUpQ1tGmAINQBzMiZwwbyjY6YXDGc"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_15",
        "GxgVknnAmUmwjjdHJf9dbEBDoqyDaUfp"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_16",
        "9d17Sv05j1XeTYOs80UBpBU1OYTTJ58X"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_17",
        "FTUm4HAfhZ5wH2u0pPn7PWcCLGDrgEfn"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_18",
        "mF7a2ptc3PRi7jWLE92t0GElhGdPnAe3"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_19",
        "Vrh8HuNvu3jtTEwSzulAjTqgOseAsVgz"
    },
    {
        "a1YRfb9bepk",
        "PKbZL7baK8pBso94",
        "test_20",
        "8Wxrxnjch6SW0s2HR5JkIBtgjt3BOUo7"
    }
};

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int subdev_index;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;
static void *g_user_dispatch_thread;

void *example_malloc(size_t size)
{
    return HAL_Malloc(size);
}

void example_free(void *ptr)
{
    HAL_Free(ptr);
}

int example_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info);

static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Connected");

    user_example_ctx->cloud_connected = 1;
#if defined(OTA_ENABLED) && defined(BUILD_AOS)
    ota_service_init(NULL);
#endif
    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Disconnected");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                           (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    EXAMPLE_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
                  reply_value_len,
                  reply_value);
    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
        user_example_ctx->subdev_index++;
    }

    return 0;
}

static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

void user_post_property(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "{\"LightSwitch\":1}";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                           (unsigned char *)property_payload, strlen(property_payload));
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE,
                           (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_DELETE,
                           (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

static int user_master_dev_available(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (user_example_ctx->cloud_connected && user_example_ctx->master_initialized) {
        return 1;
    }

    return 0;
}

void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

int example_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0, devid = -1;
    devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_SLAVE, meta_info);
    if (devid == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev open Failed\n");
        return FAIL_RETURN;
    }
    EXAMPLE_TRACE("subdev open susseed, devid = %d\n", devid);

    res = IOT_Linkkit_Connect(devid);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev connect Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev connect success: devid = %d\n",devid);

    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGIN, NULL, 0);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev login Failed\n");
        return res;
    }
    EXAMPLE_TRACE("subdev login success: devid = %d\n",devid);
    return res;
}

void *user_dispatch_yield(void *args)
{
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
    }

    return NULL;
}

int linkkit_main(void *paras)
{
#ifndef WIFI_PROVISION_ENABLED
    set_iotx_info();
#endif
    int res = 0;
    uint64_t time_prev_sec = 0, time_now_sec = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    user_example_ctx->subdev_index = -1;

    /* Init cJSON Hooks */
    cJSON_Hooks cjson_hooks = {example_malloc, example_free};
    cJSON_InitHooks(&cjson_hooks);

    IOT_OpenLog("iot_linkkit");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property Reply */
    int post_property_reply = 0;
    IOT_Ioctl(IOTX_IOCTL_RECV_PROP_REPLY, (void *)&post_property_reply);

    /* Choose Whether You Need Post Event Reply */
    int post_event_reply = 0;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }

    res = HAL_ThreadCreate(&g_user_dispatch_thread, user_dispatch_yield, NULL, NULL, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("HAL_ThreadCreate Failed\n");
        IOT_Linkkit_Close(user_example_ctx->master_devid);
        return -1;
    }

    while (1) {
        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }

        /* Add subdev */
        if (user_example_ctx->master_initialized && user_example_ctx->subdev_index >= 0) {
            if (user_example_ctx->subdev_index < EXAMPLE_SUBDEV_MAX_NUM) {
                /* Add next subdev */
                if (example_add_subdev((iotx_linkkit_dev_meta_info_t *)&subdevArr[user_example_ctx->subdev_index]) == SUCCESS_RETURN) {
                    EXAMPLE_TRACE("subdev %s add succeed", subdevArr[user_example_ctx->subdev_index].device_name);
                } else {
                    EXAMPLE_TRACE("subdev %s add failed", subdevArr[user_example_ctx->subdev_index].device_name);
                }
                user_example_ctx->subdev_index++;
            }
        }

        /* Post Proprety Example */
        if (time_now_sec % 11 == 0 && user_master_dev_available()) {
            user_post_property();
        }

        /* Device Info Update Example */
        if (time_now_sec % 23 == 0 && user_master_dev_available()) {
            user_deviceinfo_update();
        }

        /* Device Info Delete Example */
        if (time_now_sec % 29 == 0 && user_master_dev_available()) {
            user_deviceinfo_delete();
        }

        time_prev_sec = time_now_sec;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);
    return 0;
}
#endif
