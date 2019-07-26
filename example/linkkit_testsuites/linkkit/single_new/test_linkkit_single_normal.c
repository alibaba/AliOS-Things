#ifdef DEVICE_MODEL_ENABLED
#include <stdio.h>
#include <stdlib.h>
#if !defined(DEPRECATED_LINKKIT)

#include "iot_import.h"
#include "iot_export.h"
#include "cJSON.h"
#include "cut.h"
#include "single_config_new.h"

static int finished = 0;
static int success = 0;

static void request_callback(int test_type, char *resp_str, int size)
{
    cJSON *resp_json = cJSON_Parse(resp_str);
    cJSON *item = NULL;

    HAL_Printf("type=%d, size=%d: %s\n", test_type, size, resp_str);
    switch (test_type) {
        case TEST_TYPE_MODEL:
            success = 1;
            item = cJSON_GetObjectItem(resp_json, "data");
            cJSON *child_item = item->child;
            while (child_item) {
                if (child_item->type == cJSON_Array) {
                    int size = cJSON_GetArraySize(child_item);
                    if (size <= 0) {
                        success = 0;
                        HAL_Printf("%s 设置失败\n", child_item->string);
                    }
                    child_item = child_item->next;
                }
            }
            finished = 1;
            break;
        case TEST_TYPE_PRESSURE:
            item = cJSON_GetObjectItem(resp_json, "method");
            if (strcmp(item->valuestring, "progress") == 0) {
                item = cJSON_GetObjectItem(resp_json, "status");
                if (strcmp(item->valuestring, "FINISHED") == 0) {
                    finished = 1;
                    item = cJSON_GetObjectItem(resp_json, "downStream");
                    int down = (int)item->valuedouble;
                    item = cJSON_GetObjectItem(resp_json, "upStream");
                    int up = (int)item->valuedouble;
                    if (up >= down) {
                        success = 1;
                    }
                }
            }
            break;
        case TEST_TYPE_FOTA:
            break;
    }

    cJSON_Delete(resp_json);
}

static void *request_thread(void *args)
{
    int flag = *(int *)(args);

    if (flag == TEST_TYPE_MODEL) {
        /* blocked */
        set_property_and_call_service_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, (void *)request_callback);
        while (!finished) {
            HAL_SleepMs(500);
        }
    } else if (flag == TEST_TYPE_PRESSURE) {
        char *identifier = "LightSwitch";
        int value = 1;
        int duration = 10; // unit in sec
        double frequency = 5; // unit int Hz

        /* non-blocked: stop pressure */
        pressure_stop_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, request_callback);

        /* non-blokced: start pressure */
        pressure_start_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, identifier, value, frequency, duration,
                                  (void *)request_callback);
        while (!finished) {
            /* non-blocked: query pressure progress */
            pressure_progress_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, (void *)request_callback);
            HAL_SleepMs(1000);
        }
        HAL_SleepMs(10000);
        pressure_progress_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, (void *)request_callback);
        pressure_stop_from_cloud(TEST_PRODUCT_KEY, TEST_DEVICE_NAME, request_callback);
    } else {
        HAL_Printf("not support method\n");
        finished = 1;
    }
    return NULL;
}

DATA(test_linkkit_single_normal)
{
    user_example_ctx_t *user_example_ctx;
};

SETUP(test_linkkit_single_normal)
{
    int res = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;

    data->user_example_ctx = user_example_get_ctx();
    memset(data->user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Init cJSON Hooks */
    cJSON_Hooks cjson_hooks = {(void *)test_malloc, (void *)test_free};
    cJSON_InitHooks(&cjson_hooks);

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

    TEST_REPLACE_DEVCERT(&TEST_PRODUCT_KEY, &TEST_PRODUCT_SECRET, &TEST_DEVICE_NAME, &TEST_DEVICE_SECRET);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, TEST_PRODUCT_KEY, strlen(TEST_PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, TEST_PRODUCT_SECRET, strlen(TEST_PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, TEST_DEVICE_NAME, strlen(TEST_DEVICE_NAME));
    memcpy(master_meta_info.device_secret, TEST_DEVICE_SECRET, strlen(TEST_DEVICE_SECRET));

    data->user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    ASSERT_GE(data->user_example_ctx->master_devid, 0);

    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    ASSERT_EQ(res, 0);

    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
    ASSERT_EQ(res, 0);

    int post_property_reply = 1;
    res = IOT_Ioctl(IOTX_IOCTL_RECV_PROP_REPLY, (void *)&post_property_reply);
    ASSERT_EQ(res, 0);

    int post_event_reply = 1;
    res = IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);
    ASSERT_EQ(res, 0);

    res = IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
    ASSERT_GE(res, 0);

    wait_for_connected();
    curl_init();
    success = 0;
    finished = 0;
}

TEARDOWN(test_linkkit_single_normal)
{
    curl_deinit();
    IOT_Linkkit_Close(data->user_example_ctx->master_devid);
    success = 0;
    finished = 0;
}

/* 从服务端设置所有属性、调用服务 */
/* TODO:数组数据类型还未支持 */
CASE2(test_linkkit_single_normal, case_01)
{
    void *thread = NULL;
    int timeout = 600; // 1min
    int test_type = TEST_TYPE_MODEL;

    HAL_ThreadCreate(&thread, request_thread, &test_type, NULL, NULL);

    while (!finished && timeout > 0) {
        IOT_Linkkit_Yield(100);
        timeout--;
    }

    if (!finished) {
        HAL_Printf("not finished, maybe timeout!\n");
    }
    HAL_ThreadDelete(thread);
    // TODO: not relaiable yet
    // ASSERT_EQ(success, 1);
}

/* 从服务端压测 */
CASE2(test_linkkit_single_normal, case_02)
{
    void *thread = NULL;
    int timeout = 600; // 1min
    int test_type = TEST_TYPE_PRESSURE;

    HAL_ThreadCreate(&thread, request_thread, &test_type, NULL, NULL);

    while (!finished && timeout > 0) {
        IOT_Linkkit_Yield(100);
        timeout--;
    }

    if (!finished) {
        HAL_Printf("not finished, maybe timeout!\n");
    }
    HAL_ThreadDelete(thread);
    // TODO: not relaiable yet
    // ASSERT_EQ(success, 1);
}
#if 0
/* 动态注册 */
CASE(test_linkkit_single_normal, case_02)
{
    int res = 0;
    int count = 10;
    char *payload = NULL;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    iotx_linkkit_dev_meta_info_t master_meta_info;
    char *pk = "a1f6DltNDUr";
    char *ps = "5yHH3AFv9na6Gb34";
    char *dn = "test_dev_advance_02";
    char *ds = "";

    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    cJSON_Hooks cjson_hooks = {test_malloc, test_free};
    cJSON_InitHooks(&cjson_hooks);

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, pk, strlen(pk));
    memcpy(master_meta_info.product_secret, ps, strlen(ps));
    memcpy(master_meta_info.device_name, dn, strlen(dn));
    memcpy(master_meta_info.device_secret, ds, strlen(ds));

    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    ASSERT_GE(user_example_ctx->master_devid, 0);

    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    ASSERT_EQ(res, 0);

    int dynamic_register = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
    ASSERT_EQ(res, 0);

    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    ASSERT_GE(res, 0);

    wait_for_connected();

    while (count--) {
        IOT_Linkkit_Yield(100);

        payload = "{\"LightSwitch\":0}";
        res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload,
                                 strlen(payload));
        ASSERT_GE(res, 0);

        payload = "{\"ErrorCode\":0}";
        res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER), payload,
                                       strlen(payload));
        ASSERT_GE(res, 0);

        wait_seconds(1);
    }

    res = IOT_Linkkit_Close(user_example_ctx->master_devid);
    ASSERT_EQ(res, 0);
}
#endif

SUITE(test_linkkit_single_normal) = {
    ADD_CASE(test_linkkit_single_normal, case_01),
    ADD_CASE(test_linkkit_single_normal, case_02),
    ADD_CASE_NULL
};
#endif  /* !defined(DEPRECATED_LINKKIT) */
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
