/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

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

#if defined(OTA_ENABLED)
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
        HAL_Printf("\033[1;31;40m%s.%d: ", __func__, __LINE__);  \
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
#if defined(OTA_ENABLED)
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

static int user_down_raw_event_handler(const int devid, const unsigned char *payload, const int payload_len)
{
    EXAMPLE_TRACE("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);
    return 0;
}

static int user_up_raw_reply_event_handler(const int devid, const unsigned char *payload, const int payload_len)
{
    EXAMPLE_TRACE("Up Raw Reply Message, Devid: %d, Payload Length: %d", devid, payload_len);
    return 0;
}

static int user_async_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    int contrastratio = 0;
    cJSON *root = NULL, *item_transparency = NULL;
    const char *response_fmt = "{\"Contrastratio\":%d}";
    EXAMPLE_TRACE("Async Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,
                  serviceid,
                  request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }


    if (strlen("Custom") == serviceid_len && memcmp("Custom", serviceid, serviceid_len) == 0) {
        /* Parse Item */
        item_transparency = cJSON_GetObjectItem(root, "transparency");
        if (item_transparency == NULL || !cJSON_IsNumber(item_transparency)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("transparency: %d", item_transparency->valueint);
        contrastratio = item_transparency->valueint + 1;
    }
    cJSON_Delete(root);

    /* Send Service Response To Cloud */
    *response_len = strlen(response_fmt) + 10 + 1;
    *response = HAL_Malloc(*response_len);
    if (*response == NULL) {
        EXAMPLE_TRACE("Memory Not Enough");
        return -1;
    }
    memset(*response, 0, *response_len);
    HAL_Snprintf(*response, *response_len, response_fmt, contrastratio);
    *response_len = strlen(*response);

    return 0;
}

static int user_sync_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request,
        const int request_len,
        char **response, int *response_len)
{
    int to_cloud = 0;
    cJSON *root = NULL, *item_from_cloud = NULL;
    const char *response_fmt = "{\"ToCloud\":%d}";
    EXAMPLE_TRACE("Sync Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,
                  serviceid,
                  request);

    /* Parse Request */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }


    if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
        /* Parse Item */
        item_from_cloud = cJSON_GetObjectItem(root, "FromCloud");
        if (item_from_cloud == NULL || !cJSON_IsNumber(item_from_cloud)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("FromCloud: %d", item_from_cloud->valueint);
        to_cloud = item_from_cloud->valueint + 1;
    }

    cJSON_Delete(root);

    /* Send Service Response To Cloud */
    *response_len = strlen(response_fmt) + 10 + 1;
    *response = HAL_Malloc(*response_len);
    if (*response == NULL) {
        EXAMPLE_TRACE("Memory Not Enough");
        return -1;
    }
    memset(*response, 0, *response_len);
    HAL_Snprintf(*response, *response_len, response_fmt, to_cloud);
    *response_len = strlen(*response);

    return 0;
}

static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_POST_PROPERTY,
                           (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len)
{
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;
    int index = 0;
    EXAMPLE_TRACE("Property Get Received, Devid: %d, Request: %s", devid, request);

    /* Parse Request */
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++) {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid)) {
            EXAMPLE_TRACE("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        EXAMPLE_TRACE("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);

        if (strcmp("WIFI_Band", item_propertyid->valuestring) == 0) {
            cJSON_AddStringToObject(response_root, "WIFI_Band", "2.4G");
        } else if (strcmp("WIFI_Channel", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Channel", 3);
        } else if (strcmp("WiFI_RSSI", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WiFI_RSSI", -30);
        } else if (strcmp("WiFI_SNR", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WiFI_SNR", 100);
        } else if (strcmp("WIFI_AP_BSSID", item_propertyid->valuestring) == 0) {
            cJSON_AddStringToObject(response_root, "WIFI_AP_BSSID", "testap");
        } else if (strcmp("WIFI_Tx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Tx_Rate", 1111);
        } else if (strcmp("WIFI_Rx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Rx_Rate", 2222);
        } else if (strcmp("RGBColor", item_propertyid->valuestring) == 0) {
            cJSON *item_rgbcolor = cJSON_CreateObject();
            if (item_rgbcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_rgbcolor, "Red", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Green", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Blue", 100);
            cJSON_AddItemToObject(response_root, "RGBColor", item_rgbcolor);
        } else if (strcmp("HSVColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hsvcolor = cJSON_CreateObject();
            if (item_hsvcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hsvcolor, "Hue", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Saturation", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Value", 50);
            cJSON_AddItemToObject(response_root, "HSVColor", item_hsvcolor);
        } else if (strcmp("HSLColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hslcolor = cJSON_CreateObject();
            if (item_hslcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hslcolor, "Hue", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Saturation", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Lightness", 70);
            cJSON_AddItemToObject(response_root, "HSLColor", item_hslcolor);
        } else if (strcmp("WorkMode", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WorkMode", 4);
        } else if (strcmp("NightLightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "NightLightSwitch", 1);
        } else if (strcmp("Brightness", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Brightness", 30);
        } else if (strcmp("LightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "LightSwitch", 1);
        } else if (strcmp("ColorTemperature", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "ColorTemperature", 2800);
        } else if (strcmp("PropertyCharacter", item_propertyid->valuestring) == 0) {
            cJSON_AddStringToObject(response_root, "PropertyCharacter", "testprop");
        } else if (strcmp("Propertypoint", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Propertypoint", 50);
        }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    EXAMPLE_TRACE("Property Get Response: %s", *response);

    return SUCCESS_RETURN;
}

static int user_post_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    EXAMPLE_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
                  reply_value_len,
                  reply_value);
    return 0;
}

static int user_ntp_response_event_handler(const char *utc)
{
    EXAMPLE_TRACE("Current UTC: %s", utc);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

	#if defined(BUILD_AOS)
	print_heap();
	#endif
    user_example_ctx->subdev_index++;

    return 0;
}

static iotx_linkkit_event_handler_t user_event_handler = {
    .connected             = user_connected_event_handler,
    .disconnected          = user_disconnected_event_handler,
    .down_raw              = user_down_raw_event_handler,
    .up_raw_reply          = user_up_raw_reply_event_handler,
    .async_service_request = user_async_service_request_event_handler,
    .sync_service_request  = user_sync_service_request_event_handler,
    .property_set          = user_property_set_event_handler,
    .property_get          = user_property_get_event_handler,
    .post_reply            = user_post_reply_event_handler,
    .query_ntp_response    = user_ntp_response_event_handler,
    .initialized           = user_initialized
};

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

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_POST_PROPERTY,
                           (unsigned char *)property_payload, strlen(property_payload));
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_post_event(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *event_id = "Error";
    char *event_payload = "{\"ErrorCode\":0}";

    res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_DEVICEINFO_UPDATE,
                           (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_DEVICEINFO_DELETE,
                           (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

void user_post_raw_data(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    unsigned char raw_data[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

    res = IOT_Linkkit_Post(user_example_ctx->master_devid, IOTX_LINKKIT_MSG_POST_RAW_DATA,
                           raw_data, 7);
    EXAMPLE_TRACE("Post Raw Data Message ID: %d", res);
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
    
    res = IOT_Linkkit_Connect(devid, &user_event_handler);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev connect Failed\n");
        return res;
    }

    res = IOT_Linkkit_Apply(devid, IOTX_LINKKIT_MSG_LOGIN, NULL, 0);
    if (res == FAIL_RETURN) {
        EXAMPLE_TRACE("subdev login Failed\n");
        return res;
    }

    return res;
}

int linkkit_main(void *paras)
{
#ifndef WIFI_AWSS_ENABLED
    set_iotx_info();
#endif
    int res = 0;
    uint64_t time_prev_sec = 0, time_now_sec = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int subdevCurrentIndx = -1;
	
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    user_example_ctx->subdev_index = -1;

    /* Init cJSON Hooks */
    cJSON_Hooks cjson_hooks = {HAL_Malloc, HAL_Free};
    cJSON_InitHooks(&cjson_hooks);

    IOT_OpenLog("iot_linkkit");
    IOT_SetLogLevel(IOT_LOG_ERROR);

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
    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property Reply */
    int post_property_reply = 0;
    IOT_Linkkit_Ioctl(user_example_ctx->master_devid, IOTX_LINKKIT_CMD_OPTION_PROPERTY_POST_REPLY,
                      (void *)&post_property_reply);

    /* Choose Whether You Need Post Event Reply */
    int post_event_reply = 0;
    IOT_Linkkit_Ioctl(user_example_ctx->master_devid, IOTX_LINKKIT_CMD_OPTION_EVENT_POST_REPLY, (void *)&post_event_reply);

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid, &user_event_handler);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }

    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }

		/* Add subdev */
        if (user_example_ctx->master_initialized) {
            if ( (subdevCurrentIndx != user_example_ctx->subdev_index) && (user_example_ctx->subdev_index < EXAMPLE_SUBDEV_MAX_NUM) ) {
                /* Add next subdev */
                if (example_add_subdev(&subdevArr[user_example_ctx->subdev_index]) == SUCCESS_RETURN) {
                    EXAMPLE_TRACE("subdev %s add succeed", subdevArr[user_example_ctx->subdev_index].device_name);
                } 
                else {
                    EXAMPLE_TRACE("subdev %s add failed", subdevArr[user_example_ctx->subdev_index].device_name);
                }
                subdevCurrentIndx = user_example_ctx->subdev_index;
            }
        }

        /* Post Proprety Example */
        if (time_now_sec % 11 == 0 && user_master_dev_available()) {
            user_post_property();
        }
        /* Post Event Example */
        if (time_now_sec % 17 == 0 && user_master_dev_available()) {
            user_post_event();
        }

        /* Device Info Update Example */
        if (time_now_sec % 23 == 0 && user_master_dev_available()) {
            user_deviceinfo_update();
        }

        /* Device Info Delete Example */
        if (time_now_sec % 29 == 0 && user_master_dev_available()) {
            user_deviceinfo_delete();
        }

        /* Post Raw Example */
        if (time_now_sec % 37 == 0 && user_master_dev_available()) {
            user_post_raw_data();
        }

        time_prev_sec = time_now_sec;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);
    return 0;
}
