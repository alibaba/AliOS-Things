/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "app_entry.h"
#include "cJSON.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "dev_model_api.h"
#include "wrappers.h"

#define USE_CUSTOME_DOMAIN 0

#define USER_EXAMPLE_YIELD_TIMEOUT_MS 2000

#if USE_CUSTOME_DOMAIN
#define CUSTOME_DOMAIN_MQTT "iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define CUSTOME_DOMAIN_HTTP "iot-auth.cn-shanghai.aliyuncs.com"
#endif

#define EXAMPLE_TRACE(...)                                      \
    do {                                                        \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

static user_example_ctx_t g_user_example_ctx;

static int g_linkkit_con_flag = 0;

user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Connected");
    user_example_ctx->cloud_connected = 1;

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Disconnected");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

static int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
                                                                     const int payload_len)
{
    EXAMPLE_TRACE("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);

    return 0;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                                          const char *request, const int request_len, char **response, int *response_len)
{
    int contrastratio = 0;
    int to_cloud      = 0;

    cJSON *root              = NULL;
    cJSON *item_transparency = NULL;
    cJSON *item_from_cloud   = NULL;

    const char *response_fmt = "{\"Contrastratio\":%d}";
    const char *response_tc  = "{\"ToCloud\":%d}";

    EXAMPLE_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s", devid, serviceid_len,serviceid, request);

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

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)aos_malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, contrastratio);
        *response_len = strlen(*response);
    } else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
        /* Parse Item */
        item_from_cloud = cJSON_GetObjectItem(root, "FromCloud");
        if (item_from_cloud == NULL || !cJSON_IsNumber(item_from_cloud)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("FromCloud: %d", item_from_cloud->valueint);
        to_cloud = item_from_cloud->valueint + 1;

        /* Send Service Response To Cloud */
        *response_len = strlen(response_tc) + 10 + 1;
        *response = (char *)aos_malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_tc, to_cloud);
        *response_len = strlen(*response);
    }
    cJSON_Delete(root);

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

static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
                                                       int *response_len)
{
    int index = 0;

    cJSON *request_root = NULL;
    cJSON *item_propertyid = NULL;
    cJSON *response_root = NULL;

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

        if (strcmp("WIFI_Tx_Rate", item_propertyid->valuestring) == 0) {
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
        } else if (strcmp("LocalTimer", item_propertyid->valuestring) == 0) {
            cJSON *array_localtimer = cJSON_CreateArray();
            if (array_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }

            cJSON *item_localtimer = cJSON_CreateObject();
            if (item_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                cJSON_Delete(array_localtimer);
                return -1;
            }
            cJSON_AddStringToObject(item_localtimer, "Timer", "10 11 * * * 1 2 3 4 5");
            cJSON_AddNumberToObject(item_localtimer, "Enable", 1);
            cJSON_AddNumberToObject(item_localtimer, "IsValid", 1);
            cJSON_AddItemToArray(array_localtimer, item_localtimer);
            cJSON_AddItemToObject(response_root, "LocalTimer", array_localtimer);
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

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
                                                      const int reply_len)
{
    const char *reply_value     = (reply == NULL) ? ("NULL") : (reply);
    const int   reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    EXAMPLE_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
                  reply_value_len, reply_value);
    return 0;
}

static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
                                                               const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Devid: %d, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s", devid,
                  msgid, code, eventid_len, eventid, message_len, message);

    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128]   = {0};
    int  buffer_length = 128;

    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/** type:
  *
  * 0 - new config exist
  *
  */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                            const char *sign, const char *sign_method, const char *url)
{
    char buffer[128]   = {0};
    int  buffer_length = 128;

    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(user_example_ctx->master_devid, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
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

int linkkit_init(void)
{
    int res              = 0;
    int dynamic_register = 0;
    int post_event_reply = 0;
    int domain_type      = IOTX_CLOUD_REGION_SHANGHAI;

    iotx_linkkit_dev_meta_info_t    master_meta_info;
    user_example_ctx_t             *user_example_ctx = user_example_get_ctx();

#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
#if USE_CUSTOME_DOMAIN
    IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)CUSTOME_DOMAIN_MQTT);
    IOT_Ioctl(IOTX_IOCTL_SET_HTTP_DOMAIN, (void *)CUSTOME_DOMAIN_HTTP);
#else
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
#endif

    /* Choose Login Method */
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    return 0;
}
