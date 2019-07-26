
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "sdk-testsuites_internal.h"
#include "cut.h"
#include "cJSON.h"

#if defined(MQTT_COMM_ENABLED)

#define TEST_TRACE(...)                                         \
    do {                                                        \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

#define INFO_PRODUCT_KEY     "a1qev6AHaVv"
#define INFO_DEVICE_NAME     "aos_test_mqtt_01"
#define INFO_DEVICE_SECRET   "p0oOjwflClumpfQBzvSCUgJmUQhU7p9C"

#define MQTT_MSGLEN             (1024)



/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/get"

/* This topic should be created by user manually in the console of IoT cloud */
#define TOPIC_DATA              "/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/data"

/* These are pre-defined topics format*/
#define TOPIC_UPDATE_FMT            "/%s/%s/update"
#define TOPIC_ERROR_FMT             "/%s/%s/update/error"
#define TOPIC_GET_FMT               "/%s/%s/get"
#define TOPIC_DATA_FMT              "/%s/%s/data"


#define TOPIC_DEVINFO_UPDATE        "/sys/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/thing/deviceinfo/update"
#define TOPIC_DEVINFO_UPDATE_REPLY  "/sys/"INFO_PRODUCT_KEY"/"INFO_DEVICE_NAME"/thing/deviceinfo/update_reply"

#define DEVINFO_UPDATE_ECHO         "{\"id\":\"0\",\"version\":\"1.0\",\"params\":" \
    "[{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"0203000062EB20310102030405060100010203041111111111222222222233333333334444444444\",\"domain\":\"SYSTEM\"},"  \
    "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"2.3.0\",\"domain\":\"SYSTEM\"},"   \
    "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"}," \
    "{\"attrKey\":\"SYS_SDK_IF_INFO\",\"attrValue\":\"WiFi|03ACDEFF0032\",\"domain\":\"SYSTEM\"}]," \
    "\"method\":\"thing.deviceinfo.update\"}"

#define DEVINFO_UPDATE_REPLY        "{\"code\":200,\"data\":{},\"id\":\"0\",\"message\":\"success\",\"method\":\"thing.deviceinfo.update\",\"version\":\"1.0\"}"

#define DEVINFO_AOS_ACT_PATCH       "0102030405060100010203041111111111222222222233333333334444444444"

static uint8_t test_schedule = 0;



void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            TEST_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            TEST_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            TEST_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            TEST_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            TEST_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            TEST_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            TEST_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            TEST_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            TEST_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            TEST_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            TEST_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            TEST_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            TEST_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                       topic_info->topic_len,
                       topic_info->ptopic,
                       topic_info->payload_len,
                       topic_info->payload);
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            TEST_TRACE("buffer overflow, %s", msg->msg);
            break;

        default:
            TEST_TRACE("Should NOT arrive here.");
            break;
    }
}

static void test_devinfo_update_echo(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    const char *payload = ptopic_info->payload;
    
    cJSON *root = NULL, *params = NULL, *itemElem = NULL, *attrKey = NULL, *attrValue;
    int index = 0;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root)) {
        /* error */
        return;
    }

    params = cJSON_GetObjectItem(root, "params");
    if (params == NULL || !cJSON_IsArray(params)) {
        cJSON_Delete(root);
        return;
    }

    for (index = 0; index < cJSON_GetArraySize(params); index++) {
        itemElem = cJSON_GetArrayItem(params, index);
        if (itemElem == NULL || !cJSON_IsObject(itemElem)) {
            cJSON_Delete(root);
        }

        attrKey = cJSON_GetObjectItem(itemElem, "attrKey");
        if (attrKey == NULL || !cJSON_IsString(attrKey)) {
            cJSON_Delete(root);
            return;
        }
        attrValue = cJSON_GetObjectItem(itemElem, "attrValue");
        if (attrKey == NULL || !cJSON_IsString(attrValue)) {
            cJSON_Delete(root);
            return;
        }

        if (!memcmp(attrKey->valuestring, "SYS_ALIOS_ACTIVATION", strlen("SYS_ALIOS_ACTIVATION"))) {
            test_schedule |= 0x01;
            TEST_TRACE("step 1: %s", attrValue->valuestring);
        } else if (!memcmp(attrKey->valuestring, "SYS_LP_SDK_VERSION", strlen("SYS_LP_SDK_VERSION"))) {
            if (strlen(attrValue->valuestring) == strlen(LINKKIT_VERSION)
                && !memcmp(attrValue->valuestring, LINKKIT_VERSION, strlen(LINKKIT_VERSION))) {
                test_schedule |= 0x02;
                TEST_TRACE("step 2: %s", attrValue->valuestring);
            }
        } else if (!memcmp(attrKey->valuestring, "SYS_SDK_LANGUAGE", strlen("SYS_SDK_LANGUAGE"))) {
            if (strlen(attrValue->valuestring) == strlen("C") && !memcmp(attrValue->valuestring, "C", strlen("C"))) {
                test_schedule |= 0x04;
                TEST_TRACE("step 3: %s", attrValue->valuestring);
            }
        } else if (!memcmp(attrKey->valuestring, "SYS_SDK_IF_INFO", strlen("SYS_SDK_IF_INFO"))) {
            if (strlen(attrValue->valuestring) == 17 && !memcmp(attrValue->valuestring, "WiFi|", strlen("WiFi|"))) {
                test_schedule |= 0x08;
                TEST_TRACE("step 4: %s", attrValue->valuestring);
            }
        }
    }

    cJSON_Delete(root);
}

static void test_devinfo_update_reply(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    const char *payload = ptopic_info->payload;

    cJSON *root = NULL, *itemCode = NULL;

    root = cJSON_Parse(payload);
    if (root == NULL || !cJSON_IsObject(root)) {
        /* error */
        return;
    }

    itemCode = cJSON_GetObjectItem(root, "code");
    if (itemCode == NULL || !cJSON_IsNumber(itemCode)) {
        cJSON_Delete(root);
        /* error */
        return;
    }

    if (itemCode->valueint == 200) {
        /* success */
        test_schedule |= 0x10;
        TEST_TRACE("step 5: %d", itemCode->valueint);
    }

    cJSON_Delete(root);
}


CASE(INFO_REPORT, case_01_mqtt_devinfo_report)
{
    HAL_SetProductKey(INFO_PRODUCT_KEY);
    HAL_SetDeviceName(INFO_DEVICE_NAME);
    HAL_SetDeviceSecret(INFO_DEVICE_SECRET);

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    int rc = 0;
    void *pclient;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

    /* Init cJSON Hooks */
    cJSON_Hooks cjson_hooks;
    cjson_hooks.malloc_fn = (void *(*)(size_t sz))HAL_Malloc;
    cjson_hooks.free_fn = HAL_Free;
    cJSON_InitHooks(&cjson_hooks);

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(INFO_PRODUCT_KEY, INFO_DEVICE_NAME, INFO_DEVICE_SECRET, (void **)&pconn_info)) {
        TEST_TRACE("AUTH request failed!");
        return;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;
    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    ASSERT_NE(pclient, NULL);

    /* Sub /thing/deviceinfo/update topic and thing/deviceinfo/update_reply topic first */
    rc = IOT_MQTT_Subscribe(pclient, TOPIC_DEVINFO_UPDATE, IOTX_MQTT_QOS1, test_devinfo_update_echo, NULL);
    ASSERT_NE(rc, FAIL_RETURN);

    rc = IOT_MQTT_Subscribe(pclient, TOPIC_DEVINFO_UPDATE_REPLY, IOTX_MQTT_QOS1, test_devinfo_update_reply, NULL);
    ASSERT_NE(rc, FAIL_RETURN);

    int     i = 0;
    for (i = 0; i < 5; i++) {
        IOT_MQTT_Yield(pclient, 1000);
        TEST_TRACE("wait test complete, %0X", test_schedule);

        if (test_schedule == 0x1F) {
            IOT_MQTT_Destroy(&pclient);
            return;
        }
    }

    IOT_MQTT_Destroy(&pclient);
    ASSERT_FAIL();
}

SUITE(INFO_REPORT) = {
    ADD_CASE(INFO_REPORT, case_01_mqtt_devinfo_report),
    ADD_CASE_NULL
};

#endif



