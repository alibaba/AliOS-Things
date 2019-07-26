#ifdef DEVICE_MODEL_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if !defined(DEPRECATED_LINKKIT)

#include "iot_import.h"
#include "iot_export.h"
#include "cut.h"
#include "single_config_new.h"

DATA(test_linkkit_single_api_connect)
{
    user_example_ctx_t *user_example_ctx;
    iotx_linkkit_dev_meta_info_t master_meta_info;
};

SETUP(test_linkkit_single_api_connect)
{
    data->user_example_ctx = user_example_get_ctx();
    memset(data->user_example_ctx, 0, sizeof(user_example_ctx_t));

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
}

TEARDOWN(test_linkkit_single_api_connect)
{
    IOT_Linkkit_Close(data->user_example_ctx->master_devid);
}

// 正常调用
CASE2(test_linkkit_single_api_connect, case_01)
{
    int ret = 0;

    ret = IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
    ASSERT_EQ(ret, 0);

    wait_for_connected();
}

// 重复调用
CASE2(test_linkkit_single_api_connect, case_02)
{
    int ret = 0;

    ret = IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
    ASSERT_EQ(ret, 0);

    wait_for_connected();

    ret = IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
    ASSERT_EQ(ret, -1);

    wait_for_connected();
}

// master_devid = -1;
CASE2(test_linkkit_single_api_connect, case_03)
{
    int ret = 0;

    ret = IOT_Linkkit_Connect(-1);
    ASSERT_EQ(ret, -1);
}

// master_devid = 1;
CASE2(test_linkkit_single_api_connect, case_04)
{
    int ret = 0;

    ret = IOT_Linkkit_Connect(1);
    ASSERT_EQ(ret, -1);
}

#if 0
// bad network
static void timer_to_stop_bad_network(void *args)
{
    HAL_Printf("timeout...\n");
    stop_bad_network();
}

CASE2(test_linkkit_single_api_connect, case_05)
{
    int ret = 0;
    int success_count = 0;
    int fail_count = 0;
    void *timer = NULL;

    /* loss_rate=100%, delay=5s */
    start_bad_network(1883, 1.0, 5000);

    timer = HAL_Timer_Create("timer_to_stop_bad_network", timer_to_stop_bad_network, NULL);
    HAL_Timer_Start(timer, 25000);

    while (success_count <= 0) {
        /* connect successful until timeout */
        ret = IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
        if (ret < 0) {
            fail_count++;
        } else {
            success_count++;
            IOT_Linkkit_Close(data->user_example_ctx->master_devid);
        }
    }
    ASSERT_GE(fail_count, 1);
}
#endif

SUITE(test_linkkit_single_api_connect) = {
    ADD_CASE(test_linkkit_single_api_connect, case_01),
    ADD_CASE(test_linkkit_single_api_connect, case_02),
    ADD_CASE(test_linkkit_single_api_connect, case_03),
    ADD_CASE(test_linkkit_single_api_connect, case_04),
#if 0
    ADD_CASE(test_linkkit_single_api_connect, case_05),
#endif
    ADD_CASE_NULL
};
#endif  /* !defined(DEPRECATED_LINKKIT) */
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
