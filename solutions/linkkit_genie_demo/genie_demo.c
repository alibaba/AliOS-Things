/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "netmgr.h"
#include "linkkit/wifi_provision_api.h"
#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/dev_model_api.h"
#include "linkkit/infra/infra_config.h"
#include "linkkit/wrappers/wrappers.h"
#include "cJSON.h"

#include "aos/gpioc.h"

/**
 * These PRODUCT_KEY|PRODUCT_SECRET|DEVICE_NAME|DEVICE_SECRET are listed for demo only
 *
 * When you created your own devices on iot.console.com, you SHOULD replace them with what you got from console
 *
 */
#define PRODUCT_KEY      "xx"
#define PRODUCT_SECRET   "xx"
#define DEVICE_NAME      "xx"
#define DEVICE_SECRET    "xx"

// kv keys of Wi-Fi SSID/Password
#define WIFI_SSID_KV_KEY "wifi_ssid"
#define WIFI_PASSWORD_KV_KEY "wifi_password"

#define EXAMPLE_TRACE(...)                                          \
    do {                                                            \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);     \
        HAL_Printf(__VA_ARGS__);                                    \
        HAL_Printf("\033[0m\r\n");                                  \
    } while (0)

#define EXAMPLE_MASTER_DEVID            (0)
#define EXAMPLE_YIELD_TIMEOUT_MS        (200)

// HaaS200's GPIO named with PA_28 is connected to the Red LED
#define LED_PIN 28

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;
static aos_gpioc_ref_t gpioc;
static led_state = 0;



// control LED's on/off
int led_ctrl(unsigned int on)
{
    // output 0 to turn on the LED, while output 1 to turn off the LED
    return aos_gpioc_set_value(&gpioc, LED_PIN, on ? 0 : 1);
}

// initialize the GPIO, which is connected with LED
int led_init(void)
{
    int ret = -1;
    unsigned int mode;

    memset(&gpioc, 0, sizeof(gpioc));

    // get GPIO controller's reference
    ret = aos_gpioc_get(&gpioc, 0);
    if (ret) {
        EXAMPLE_TRACE("GPIO_HAL", "get gpioc failed,  gpioc_index = %d, ret = %d;\n", LED_PIN, ret);
        return ret;
    }

    // set LED_PIN into output mode
    mode = AOS_GPIO_DIR_OUTPUT;
    mode |= AOS_GPIO_OUTPUT_CFG_PP;
    mode |= AOS_GPIO_OUTPUT_INIT_HIGH;
    ret = aos_gpioc_set_mode(&gpioc, LED_PIN, mode);
    if (ret) {
        EXAMPLE_TRACE("GPIO_HAL", "%s: set gpio mode failed, %d\n", __func__, ret);
        aos_gpioc_put(&gpioc);
        return ret;
    }
    EXAMPLE_TRACE("GPIO_HAL", "%s: set gpio mode succeed\n", __func__);

    return ret;
}

/** cloud connected event callback */
static int user_connected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Connected");
    g_user_example_ctx.cloud_connected = 1;

    return 0;
}

/** cloud disconnected event callback */
static int user_disconnected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Disconnected");
    g_user_example_ctx.cloud_connected = 0;

    return 0;
}

/* device initialized event callback */
static int user_initialized(const int devid)
{
    EXAMPLE_TRACE("Device Initialized");
    g_user_example_ctx.master_initialized = 1;

    return 0;
}

/** recv property post response message from cloud **/
static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    EXAMPLE_TRACE("Message Post Reply Received, Message ID: %d, Code: %d, Reply: %.*s", msgid, code,
                  reply_len,
                  (reply == NULL) ? ("NULL") : (reply));
    return 0;
}

/** recv event post response message from cloud **/
static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s",
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

/** recv property set command from cloud **/
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    cJSON *root = NULL, *power_state = NULL;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);

    /* parse json string */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    /* check whether powerstate control command exist or not */
    power_state = cJSON_GetObjectItem(root, "powerstate");
    if (power_state == NULL)
        goto out;

    EXAMPLE_TRACE("powerstate = %d", power_state->valueint);
    if (power_state->valueint)
        led_state = 1;
    else
        led_state = 0;

    // turn on/off the LED according to the command from cloud
    EXAMPLE_TRACE("turn %s the LED", led_state ? "on" : "off");
    led_ctrl(led_state);

    // post device's property back to the cloud
    user_post_property();
out:
    cJSON_Delete(root);

    return 0;
}

/** recv service request from cloud **/
static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len,
        char **response, int *response_len)
{
    int add_result = 0;
    int ret = -1;
    cJSON *root = NULL, *item_number_a = NULL, *item_number_b = NULL;
    const char *response_fmt = "{\"Result\": %d}";
    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }
    do {
        if (strlen("Operation_Service") == serviceid_len && memcmp("Operation_Service", serviceid, serviceid_len) == 0) {
            /* Parse NumberA */
            item_number_a = cJSON_GetObjectItem(root, "NumberA");
            if (item_number_a == NULL || !cJSON_IsNumber(item_number_a))
                break;

            EXAMPLE_TRACE("NumberA = %d", item_number_a->valueint);

            /* Parse NumberB */
            item_number_b = cJSON_GetObjectItem(root, "NumberB");
            if (item_number_b == NULL || !cJSON_IsNumber(item_number_b))
                break;

            EXAMPLE_TRACE("NumberB = %d", item_number_b->valueint);
            add_result = item_number_a->valueint + item_number_b->valueint;
            ret = 0;
            /* Send Service Response To Cloud */
        }
    } while (0);

    *response_len = strlen(response_fmt) + 10 + 1;
    *response = (char *)HAL_Malloc(*response_len);
    if (*response != NULL) {
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, add_result);
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);
    return ret;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

/** fota event handler **/
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/* cota event handler */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* type = 0, new config exist, query the new config */
    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

// report device's property to the cloud
void user_post_property(void)
{
    int res = 0;

    char property_payload[30] = {0};
    HAL_Snprintf(property_payload, sizeof(property_payload), "{\"powerstate\": %d}", led_state);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));

    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_UPDATE,
                             (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_DELETE,
                             (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

static int user_cloud_error_handler(const int code, const char *data, const char *detail)
{
    EXAMPLE_TRACE("code =%d ,data=%s, detail=%s", code, data, detail);
    return 0;
}

// make sure device's signature is correct
static void set_iot_device_info(void)
{
    char _device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetDeviceSecret(_device_secret);
    // if device secret does not exist, store pk/ps/dn/ds into kv system
    if (!strlen(_device_secret)) {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
    }

    return;
}

int linkkit_main(void)
{
    int res = 0;
    int cnt = 0;
    int auto_quit = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0, fota_timeout = 30;

    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(master_meta_info.product_key);
    HAL_GetDeviceName(master_meta_info.device_name);
    HAL_GetProductSecret(master_meta_info.product_secret);
    HAL_GetDeviceSecret(master_meta_info.device_secret);

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
    IOT_RegisterCallback(ITE_CLOUD_ERROR, user_cloud_error_handler);


    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* post reply doesn't need */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    IOT_Ioctl(IOTX_IOCTL_FOTA_TIMEOUT_MS, (void *)&fota_timeout);
#if defined(USE_ITLS)
    {
        char url[128] = {0};
        int port = 1883;
        snprintf(url, 128, "%s.itls.cn-shanghai.aliyuncs.com", master_meta_info.product_key);
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)url);
        IOT_Ioctl(IOTX_IOCTL_SET_CUSTOMIZE_INFO, (void *)"authtype=id2");
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, &port);
    }
#endif
    /* Create Master Device Resources */
    do {
        g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (g_user_example_ctx.master_devid >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        HAL_SleepMs(2000);
    } while (1);
    /* Start Connect Aliyun Server */
    do {
        res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
        if (res >= 0) {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        HAL_SleepMs(5000);
    } while (1);

    while (1) {
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);

        /* Post Proprety Example */

        if ((cnt % 20) == 0) {
            user_post_property();
        }

        cnt++;

        if (auto_quit == 1 && cnt > 3600) {
            break;
        }
    }

    IOT_Linkkit_Close(g_user_example_ctx.master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    return 0;
}

int application_start(int argc, char *argv[])
{
    int len = 0;
    int ret = 0;
    int count = 0;
    char ssid[32 + 1] = {0};
    char password[64 + 1] = {0};

    EXAMPLE_TRACE("nano entry here!\r\n");

    /* LED GPIO initialization */
    ret = led_init();
    if (ret) {
        EXAMPLE_TRACE("led init done, ret:%d\r\n", ret);
    }

    set_iot_device_info();

    /*
        check whether SSID/Password are configured or not
     */
    len = sizeof(ssid);
    ret = aos_kv_get(WIFI_SSID_KV_KEY, ssid, &len);
    if (ret) {
        EXAMPLE_TRACE("aos_kv_get %s ret:%d\r\n", WIFI_SSID_KV_KEY, ret);
        goto zero_config;
    }
    len = sizeof(password);
    ret = aos_kv_get(WIFI_PASSWORD_KV_KEY, password, &len);
    if (ret) {
        EXAMPLE_TRACE("aos_kv_get %s ret:%d\r\n", WIFI_PASSWORD_KV_KEY, ret);
        goto zero_config;
    }

    /*
        connect to Wi-Fi
     */
    netmgr_comp_enable(ssid, password, 0);
    goto linkkit_start;

zero_config:
    awss_config_press();
    awss_start();

linkkit_start:
    linkkit_main();

    while (1) {
        EXAMPLE_TRACE("hello genie intelligent light! count %d \r\n", count++);
        aos_msleep(10000);
    };
}

