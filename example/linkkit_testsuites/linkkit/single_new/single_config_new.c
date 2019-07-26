#ifdef DEVICE_MODEL_ENABLED
#include "iot_import.h"
#include "iot_export.h"
#include "linkkit_export.h"
#include "iot_export_linkkit.h"
#include "cJSON.h"
#include "single_config_new.h"
#include <string.h>
#include <stdio.h>
#if !defined(DEPRECATED_LINKKIT)


static user_example_ctx_t g_user_example_ctx;

user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Connected");

    user_example_ctx->cloud_connected = 1;

    return 0;
}

int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Disconnected");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload, const int payload_len)
{
    EXAMPLE_TRACE("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);
    return 0;
}

int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                       const char *request, const int request_len,
                                       char **response, int *response_len)
{
    int contrastratio = 0, to_cloud = 0;
    cJSON *root = NULL, *item_transparency = NULL, *item_from_cloud = NULL;
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
        const char *response_fmt = "{\"Contrastratio\":%d}";
        item_transparency = cJSON_GetObjectItem(root, "transparency");
        if (item_transparency == NULL || !cJSON_IsNumber(item_transparency)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("transparency: %d", item_transparency->valueint);
        contrastratio = item_transparency->valueint + 1;

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
    } else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
        /* Parse Item */
        const char *response_fmt = "{\"ToCloud\":%d}";
        item_from_cloud = cJSON_GetObjectItem(root, "FromCloud");
        if (item_from_cloud == NULL || !cJSON_IsNumber(item_from_cloud)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("FromCloud: %d", item_from_cloud->valueint);
        to_cloud = item_from_cloud->valueint + 1;

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
    }
    cJSON_Delete(root);

    return 0;
}

int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
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

int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
                                    const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
                  reply_value_len,
                  reply_value);
    user_example_ctx->flag = 0;
    if (strcmp(reply_value, "{}") == 0 || strcmp(reply_value, "NULL") == 0) {
        user_example_ctx->flag = 1;
    }
    return 0;
}

int user_ntp_response_event_handler(const char *utc)
{
    EXAMPLE_TRACE("Current UTC: %s", utc);

    return 0;
}

int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

    return 0;
}

void wait_for_connected()
{
    user_example_ctx_t *ctx = user_example_get_ctx();
    int max_count = 100;

    while (!ctx->cloud_connected && max_count--) {
        IOT_Linkkit_Yield(100);
    }
}

void wait_seconds(int sec)
{
    int count = sec * 10;
    while (count--) {
        IOT_Linkkit_Yield(100);
    }
}

void *test_malloc(size_t size)
{
    return HAL_Malloc(size);
}

void test_free(void *ptr)
{
    HAL_Free(ptr);
}


// iotnext.console.aliyun.com
#if 1
    char *TEST_PRODUCT_KEY = "a1f6DltNDUr";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "test_dev_advance_02";
    char *TEST_DEVICE_SECRET = "NuiExe3LqJkWhrFNKbcpql9tMuqMbvLd";
#endif

#if 0
    char *TEST_PRODUCT_KEY = "a1f6DltNDUr";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "test_dev_advance_03";
    char *TEST_DEVICE_SECRET = "3gvr1HxNsQf9EyceebfY7VDRPwpkb7Gi";
#endif

#if 0
    char *TEST_PRODUCT_KEY = "a1f6DltNDUr";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "test_dev_advance_04";
    char *TEST_DEVICE_SECRET = "f3KOHlbgTRpMFTFZ5Q16Qe0dyp8LP5Gu";
#endif

#if 0
    char *TEST_PRODUCT_KEY = "a1f6DltNDUr";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "test_dev_advance_05";
    char *TEST_DEVICE_SECRET = "aeoXaVVCCCN3uzDhCX0HpR1T6dpMBYUR";
#endif

#if 0
    // living.aliyun.com
    char *TEST_PRODUCT_KEY = "a1YzpyeAnaj";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "example1";
    char *TEST_DEVICE_SECRET = "pNoJqERMEms0qxZLaDzy03UzrGfkaUF2";
#endif

#if 0
    // living.aliyun.com
    char *TEST_PRODUCT_KEY = "a1YzpyeAnaj";
    char *TEST_PRODUCT_SECRET = "5yHH3AFv9na6Gb34";
    char *TEST_DEVICE_NAME = "example1";
    char *TEST_DEVICE_SECRET = "pNoJqERMEms0qxZLaDzy03UzrGfkaUF2";
#endif

#endif  /* !defined(DEPRECATED_LINKKIT) */
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
