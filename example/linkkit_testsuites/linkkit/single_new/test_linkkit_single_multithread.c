#ifdef DEVICE_MODEL_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(DEPRECATED_LINKKIT)

#include "iot_import.h"
#include "iot_export.h"
#include "cut.h"
#include "single_config_new.h"

DATA(test_linkkit_single_multithread)
{
    user_example_ctx_t *user_example_ctx;
    iotx_linkkit_dev_meta_info_t master_meta_info;
};

SETUP(test_linkkit_single_multithread)
{
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

    memset(&data->master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(data->master_meta_info.product_key, TEST_PRODUCT_KEY, strlen(TEST_PRODUCT_KEY));
    memcpy(data->master_meta_info.product_secret, TEST_PRODUCT_SECRET, strlen(TEST_PRODUCT_SECRET));
    memcpy(data->master_meta_info.device_name, TEST_DEVICE_NAME, strlen(TEST_DEVICE_NAME));
    memcpy(data->master_meta_info.device_secret, TEST_DEVICE_SECRET, strlen(TEST_DEVICE_SECRET));

    data->user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &data->master_meta_info);

    int domain_type = IOTX_CLOUD_DOMAIN_SH;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    int post_property_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_PROP_REPLY, (void *)&post_property_reply);

    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    IOT_Linkkit_Connect(data->user_example_ctx->master_devid);

    wait_for_connected();
}

TEARDOWN(test_linkkit_single_multithread)
{
    IOT_Linkkit_Close(data->user_example_ctx->master_devid);
}

void *thread_post_property(void *arg)
{
    int count = 10;
    int ret = 0;
    unsigned char *payload = (unsigned char *)"{\"LightSwitch\":1}";
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    while (count--) {
        ret = IOT_Linkkit_Report(ctx->master_devid, ITM_MSG_POST_PROPERTY, payload, strlen((const char *)payload));
        if (ret == -1) {
            EXAMPLE_TRACE("IOT_Linkkit_Report property fail\n");
            return NULL;
        }
        HAL_SleepMs(500);
    }
    ctx->flag++;
    return NULL;
}

void *thread_trigger_event(void *arg)
{
    int count = 10;
    int ret = 0;
    char *payload = "{\"ErrorCode\":0}";
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    while (count--) {
        ret = IOT_Linkkit_TriggerEvent(ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER), payload,
                                       strlen((const char *)payload));
        if (ret == -1) {
            EXAMPLE_TRACE("IOT_Linkkit_TriggerEvent fail\n");
            return NULL;
        }
        HAL_SleepMs(500);
    }
    ctx->flag++;
    return NULL;
}

void *thread_linkkit_start(void *arg)
{
    int ret = 0;
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    ret = IOT_Linkkit_Connect(ctx->master_devid);
    if (ret == -1) {
        EXAMPLE_TRACE("\n\n\nlinkkit start fail\n\n\n");
        ctx->flag = 1;
        return NULL;
    }
    ctx->flag = 0;
    return NULL;
}

void *thread_linkkit_end(void *arg)
{
    int ret = 0;
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    ret = IOT_Linkkit_Close(ctx->master_devid);
    if (ret == -1) {
        printf("\n\n\nlinkkit end fail\n\n\n");
        ctx->flag = 0;
        return NULL;
    }
    ctx->flag = 1;
    return NULL;
}

void *thread_linkkit_dispatch(void *arg)
{
    int max_count = 20;
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    do {
        IOT_Linkkit_Yield(100);
        EXAMPLE_TRACE("%d multithread yield...\n", max_count);
    } while (max_count--);

    ctx->flag = 1;
    return NULL;
}

void *thread_trigger_extendinfo(void *arg)
{
    char *payload = "{\"attrKey\":\"key\",\"attrValue\":\"value\"}";
    int count = 10;
    int ret = 0;
    user_example_ctx_t *ctx = (user_example_ctx_t *)arg;

    while (count--) {
        EXAMPLE_TRACE("IOT_Linkkit_Report Update Device Info %d\n", count);
        ret = IOT_Linkkit_Report(ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                                 strlen((const char *)payload));
        if (ret == -1) {
            EXAMPLE_TRACE("IOT_Linkkit_Report Update Device Info fail\n");
            return NULL;
        }
        HAL_SleepMs(500);
    }
    ctx->flag++;
    return NULL;
}

/* 多线程linkkit_post_property */
CASE2(test_linkkit_single_multithread, case_01)
{
    void*  pid1;
    void*  pid2;
    int ret = 0;

    EXAMPLE_TRACE("多线程linkkit_post_property\n\n\n");
    //ret = pthread_create(&pid1, NULL, thread_post_property, data->user_example_ctx);
    ret = HAL_ThreadCreate(&pid1, thread_post_property, data->user_example_ctx, NULL, NULL);

    if (ret != 0) {
        EXAMPLE_TRACE("pthread_post_property 1 failed!\n");
    }

    ret = HAL_ThreadCreate(&pid2, thread_post_property, data->user_example_ctx, NULL, NULL);
    if (ret != 0) {
        EXAMPLE_TRACE("pthread_post_property 2 failed!\n");
    }

    wait_seconds(15);

    ASSERT_EQ(data->user_example_ctx->flag, 3);
}

/* 多线程linkkit_trigger_event */
CASE2(test_linkkit_single_multithread, case_02)
{
    void*  pid1;
    void*  pid2;
    int ret = 0;

    EXAMPLE_TRACE("多线程linkkit_trigger_event\n\n\n");
    ret = HAL_ThreadCreate(&pid1, thread_trigger_event, data->user_example_ctx, NULL, NULL);
    //ret = pthread_create(&pid1, NULL, thread_trigger_event, data->user_example_ctx);
    if (ret != 0) {
        EXAMPLE_TRACE("pthread_post_property 1 failed!\n");
    }

    ret = HAL_ThreadCreate(&pid2, thread_trigger_event, data->user_example_ctx, NULL, NULL);
    //ret = pthread_create(&pid2, NULL, thread_trigger_event, data->user_example_ctx);
    if (ret != 0) {
        EXAMPLE_TRACE("pthread_post_property 2 failed!\n");
    }

    wait_seconds(15);
    ASSERT_EQ(data->user_example_ctx->flag, 2);
}

/* 多线程上报同一扩展信息 */
CASE2(test_linkkit_single_multithread, case_03)
{
    void*  pid1;
    void*  pid2;
    int ret = 0;

    wait_for_connected();
        ret = HAL_ThreadCreate(&pid1, thread_trigger_extendinfo, data->user_example_ctx, NULL, NULL);
    if (ret != 0) {
        EXAMPLE_TRACE("thread_trigger_extendinfo failed!\n");
    }
        ret = HAL_ThreadCreate(&pid2, thread_trigger_extendinfo, data->user_example_ctx, NULL, NULL);
    if (ret != 0) {
        EXAMPLE_TRACE("thread_trigger_extendinfo failed!\n");
    }

    wait_seconds(15);
    ASSERT_EQ(data->user_example_ctx->flag, 3);
}

/* linkkit_start在线程1，yeild和dispatch在线程2 */
CASE2(test_linkkit_single_multithread, case_04)
{
    void*  pid;
    int ret = 0;
    data->user_example_ctx->flag = 0;
    ret = HAL_ThreadCreate(&pid, thread_linkkit_dispatch, data->user_example_ctx, NULL, NULL);
    //ret = pthread_create(&pid, NULL, thread_linkkit_dispatch, data->user_example_ctx);
    if (ret != 0) {
        EXAMPLE_TRACE("pthread_linkkit_dispatch failed!\n");
    }
    //pthread_join(pid, NULL);
    wait_seconds(5);

    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

/* 多线程linkkit_start */
CASE2(test_linkkit_single_multithread, case_05)
{
    void*  pid;
    int ret = 0;

    wait_seconds(3);
    ret = HAL_ThreadCreate(&pid, thread_linkkit_start, data->user_example_ctx, NULL, NULL);
    //ret = pthread_create(&pid, NULL, thread_linkkit_start, data->user_example_ctx);
    if (ret != 0) {
        EXAMPLE_TRACE("pthread_linkkit_start 1 failed!\n");
    }
    //pthread_join(pid, NULL);
    wait_seconds(15);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

/* 线程1 linkkit_start,线程2 linkkit_end */
CASE2(test_linkkit_single_multithread, case_06)
{
    void*  pid;
    int ret = 0;
    ret = HAL_ThreadCreate(&pid, thread_linkkit_end, data->user_example_ctx, NULL, NULL);
    //ret = pthread_create(&pid, NULL, thread_linkkit_end, data->user_example_ctx);
    if (ret != 0) {
        EXAMPLE_TRACE("thread_linkkit_end failed!\n");
    }
    //pthread_join(pid, NULL);
    wait_seconds(5);
    HAL_SleepMs(5000);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

SUITE(test_linkkit_single_multithread) = {
    // ADD_CASE(test_linkkit_single_multithread, case_01),
    // ADD_CASE(test_linkkit_single_multithread, case_02),
    // ADD_CASE(test_linkkit_single_multithread, case_03),
    // ADD_CASE(test_linkkit_single_multithread, case_04),
    // ADD_CASE(test_linkkit_single_multithread, case_05),
    ADD_CASE(test_linkkit_single_multithread, case_06),
    ADD_CASE_NULL
};
#endif  /* !defined(DEPRECATED_LINKKIT) */
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
