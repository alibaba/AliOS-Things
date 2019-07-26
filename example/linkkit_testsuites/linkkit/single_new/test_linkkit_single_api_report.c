#ifdef DEVICE_MODEL_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(DEPRECATED_LINKKIT)

#include "iot_import.h"
#include "iot_export.h"
#include "cut.h"
#include "single_config_new.h"

DATA(test_linkkit_single_api_report)
{
    user_example_ctx_t *user_example_ctx;
    iotx_linkkit_dev_meta_info_t master_meta_info;
};

SETUP(test_linkkit_single_api_report)
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
    ASSERT_GE(data->user_example_ctx->master_devid, 0);

    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    IOT_Linkkit_Connect(data->user_example_ctx->master_devid);
    wait_for_connected();
}

TEARDOWN(test_linkkit_single_api_report)
{
    IOT_Linkkit_Close(data->user_example_ctx->master_devid);
}

// 正常上报属性
CASE2(test_linkkit_single_api_report, case_01)
{
    char *payload = "{\"LightSwitch\":1}";
    int ret = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload,
                             strlen(payload));
    ASSERT_GE(ret, 0);


}

// 上报属性, payload=NULL
CASE2(test_linkkit_single_api_report, case_02)
{
    int ret = 0;

    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, NULL, 10);
    ASSERT_EQ(ret, -1);
}

// 上报属性, payload_len=0
CASE2(test_linkkit_single_api_report, case_03)
{
    char *payload = "{\"LightSwitch\":1}";
    int ret = 0;

    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, 0);
    ASSERT_EQ(ret, -1);
}

// 上报属性, payload_len = -1
CASE2(test_linkkit_single_api_report, case_04)
{
    char *payload = "{\"LightSwitch\":1}";
    int ret = 0;

    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, -1);
    ASSERT_EQ(ret, -1);
}

// 上报属性, payload_len = 102400
CASE2(test_linkkit_single_api_report, case_05)
{
    char *payload = "{\"LightSwitch\":1}";
    int len = 102400;
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, len);
    wait_seconds(2);
    ASSERT_GE(ret, 0);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

/*同时上报多个属性*/
CASE2(test_linkkit_single_api_report, case_06)
{
    char *payload = "{\"LightSwitch\":1,\"PowerSwitch\":1}";
    int len = strlen(payload);
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, len);
    wait_seconds(2);
    ASSERT_GE(ret, 0);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

/*同时上报多个属性，其中一个不存在*/
CASE2(test_linkkit_single_api_report, case_07)
{
    char *payload = "{\"LightSwitch\":1,\"NotExistPropertyIdentifer\":1}";
    int len = strlen(payload);
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, len);
    wait_seconds(2);
    ASSERT_GE(ret, 0);
    ASSERT_EQ(data->user_example_ctx->flag, 0);
}

// 正常上报事件
CASE2(test_linkkit_single_api_report, case_11)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER),
                                   payload, strlen(payload));
    ASSERT_GE(ret, 0);
}

// 上报事件:identifer = NULL
CASE2(test_linkkit_single_api_report, case_12)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, NULL, strlen(EVENT_IDENTIFIER), payload,
                                   strlen(payload));
    ASSERT_EQ(ret, -1);
}

// 上报事件:identifer_len = 0
CASE2(test_linkkit_single_api_report, case_13)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, 0, payload, strlen(payload));
    ASSERT_EQ(ret, -1);
}

// 上报事件:identifer_len = -1
CASE2(test_linkkit_single_api_report, case_14)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, -1, payload,  strlen(payload));
    ASSERT_EQ(ret, -1);
}

// 上报事件:payload = NULL
CASE2(test_linkkit_single_api_report, case_15)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER), NULL,
                                   strlen(payload));
    ASSERT_EQ(ret, -1);
}

// 上报事件:payload_len = 0
CASE2(test_linkkit_single_api_report, case_16)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER),
                                   payload, 0);
    ASSERT_EQ(ret, -1);
}

// 上报事件:payload_len = -1
CASE2(test_linkkit_single_api_report, case_17)
{
    char *payload = "{\"ErrorCode\":0}";
    int ret = 0;

    ret = IOT_Linkkit_TriggerEvent(data->user_example_ctx->master_devid, EVENT_IDENTIFIER, strlen(EVENT_IDENTIFIER),
                                   payload, -1);
    ASSERT_EQ(ret, -1);
}

// 正常上报扩展信息
CASE2(test_linkkit_single_api_report, case_21)
{
    char *payload = "[{\"domain\":\"SYSTEM\",\"attrKey\":\"key\",\"attrValue\":\"value\"}]";
    int len = strlen(payload);
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                             len);
    wait_seconds(2);
    ASSERT_GE(ret, 0);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

// 上报扩展信息:paylay = NULL
CASE2(test_linkkit_single_api_report, case_22)
{
    char *payload = NULL;
    int len = 10;
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                             len);
    wait_seconds(2);
    ASSERT_EQ(ret, -1);
    ASSERT_EQ(data->user_example_ctx->flag, 0);
}

// 上报扩展信息:paylay_len = 0
CASE2(test_linkkit_single_api_report, case_23)
{
    char *payload = "{\"attrKey\":\"key\",\"attrValue\":\"value\"}";
    int len = 0;
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                             len);
    wait_seconds(2);
    ASSERT_EQ(ret, -1);
    ASSERT_EQ(data->user_example_ctx->flag, 0);
}

// 上报扩展信息:paylay_len = -1
CASE2(test_linkkit_single_api_report, case_24)
{
    char *payload = "{\"attrKey\":\"key\",\"attrValue\":\"value\"}";
    int len = -1;
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                             len);
    wait_seconds(2);
    ASSERT_EQ(ret, -1);
    ASSERT_EQ(data->user_example_ctx->flag, 0);
}

/*上报扩展信息：同时上报多个key-value对*/
CASE2(test_linkkit_single_api_report, case_25)
{
    char *payload = "[{\"attrKey\":\"key1\",\"attrValue\":\"value1\"},{\"attrKey\":\"key2\",\"attrValue\":\"value2\"}]";
    int len = strlen(payload);
    int ret = 0;

    data->user_example_ctx->flag = 0;
    ret = IOT_Linkkit_Report(data->user_example_ctx->master_devid, ITM_MSG_DEVICEINFO_UPDATE, (unsigned char *)payload,
                             len);
    wait_seconds(3);
    ASSERT_GE(ret, 0);
    ASSERT_EQ(data->user_example_ctx->flag, 1);
}

SUITE(test_linkkit_single_api_report) = {
    ADD_CASE(test_linkkit_single_api_report, case_01),
    ADD_CASE(test_linkkit_single_api_report, case_02),
    ADD_CASE(test_linkkit_single_api_report, case_03),
    ADD_CASE(test_linkkit_single_api_report, case_04),
    ADD_CASE(test_linkkit_single_api_report, case_05),
    ADD_CASE(test_linkkit_single_api_report, case_06),
    ADD_CASE(test_linkkit_single_api_report, case_07),
    ADD_CASE(test_linkkit_single_api_report, case_11),
    ADD_CASE(test_linkkit_single_api_report, case_12),
    ADD_CASE(test_linkkit_single_api_report, case_13),
    ADD_CASE(test_linkkit_single_api_report, case_14),
    ADD_CASE(test_linkkit_single_api_report, case_15),
    ADD_CASE(test_linkkit_single_api_report, case_16),
    ADD_CASE(test_linkkit_single_api_report, case_17),
    ADD_CASE(test_linkkit_single_api_report, case_21),
    ADD_CASE(test_linkkit_single_api_report, case_22),
    ADD_CASE(test_linkkit_single_api_report, case_23),
    ADD_CASE(test_linkkit_single_api_report, case_24),
    ADD_CASE(test_linkkit_single_api_report, case_25),
    ADD_CASE_NULL
};
#endif  /* !defined(DEPRECATED_LINKKIT) */
#endif  /* #ifdef DEVICE_MODEL_ENABLED */
