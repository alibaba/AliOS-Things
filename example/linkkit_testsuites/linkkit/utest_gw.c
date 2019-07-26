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
#include "cut.h"

#if (defined(DEVICE_MODEL_GATEWAY) && !defined(DEPRECATED_LINKKIT))

/* master device info */
#define UT_GW_PRODUCT_KEY       "a1xnvMLMpr3"
#define UT_GW_PRODUCT_SECRET    "jBkS12XffoIRxsiw"
#define UT_GW_DEVICE_NAME       "aos_test_gw_01"
#define UT_GW_DEVICE_SECRET     "4UgWeUNbUQLw6RpeB5GDtRdBuduocHnS"

#define USER_EXAMPLE_YIELD_TIMEOUT_MS (200)

#define TEST_TRACE(...)                                         \
    do {                                                        \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

/* slave device info */
#define SUBDEV_ID_1                     1
#define SUBDEV_ID_2                     2
#define SUBDEV_ID_3                     3
#define EXAMPLE_SUBDEV_MAX_NUM          3
const iotx_linkkit_dev_meta_info_t subdev_array[EXAMPLE_SUBDEV_MAX_NUM] = {
    {
        "a1S5pn9fohk",
        "0q6x5FURue8i91wq",
        "aos_ut_sub_01",
        "mmV0YYLiHnJlNALhNMY8LHXBbxnvOytp"
    },
    {
        "a1S5pn9fohk",
        "0q6x5FURue8i91wq",
        "aos_ut_sub_02",
        "DGlZ3LJ2KjItoPTc3xYOvT7GeT8FTLJq"
    },
    {
        "a1S5pn9fohk",
        "tpYifDRIJjro1tLd",
        "aos_ut_sub_03",
        "",                 /* empty string used */
    },
};

/* all type of properties define */
#define TEST_PROPERTY_MAX_NUM           9
char *property_data_array[TEST_PROPERTY_MAX_NUM] = {
    "{\"id_int32\":1111}",
    "{\"id_float\":11.11}",
    "{\"id_double\":11.0123456789}",
    "{\"id_enum\":3}",
    "{\"id_bool\":1}",
    "{\"id_text\":\"hello world!\"}",
    "{\"id_date\":\"1540460575000\"}",
    "{\"id_struct\":{\"id_item\":12345}}",
    "{\"id_array\":[1, 2, 3, 4, 5]}"
};

/* all type of events define */
#define TEST_EVENT_MAX_NUM              1
char *event_data_array[1][2] = {
    {"Error", "{\"ErrorCode\":0}"},
};

#define DOWN_RAW_DATA           "\x06\x07\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E"

/* test context */
typedef struct {
    int master_devid;
    int subdev_devid;
    int cloud_connected;
    int master_initialized;
    int slave_initialized;
    int post_reply_msgid;
    int post_reply_code;
    char raw_data_down[17];
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;
static void *g_user_dispatch_thread;

static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    TEST_TRACE("Cloud Connected");

    user_example_ctx->cloud_connected = 1;

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    TEST_TRACE("Cloud Disconnected");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

/* case: set all properties, post all properties */
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;

    TEST_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)request, request_len);
    TEST_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    TEST_TRACE("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s", devid, msgid, code,
               reply_value_len,
               reply_value);

    g_user_example_ctx.post_reply_msgid = msgid;
    g_user_example_ctx.post_reply_code = code;

    return 0;
}

static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    TEST_TRACE("Trigger Event Reply Received, Devid: %d, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s", devid,
               msgid, code,
               eventid_len,
               eventid, message_len, message);

    g_user_example_ctx.post_reply_msgid = msgid;
    g_user_example_ctx.post_reply_code = code;

    return 0;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
        const char *request, const int request_len, char **response, int *response_len)
{
    TEST_TRACE("Service Request Received, Devid: %d, Service ID: %.*s, input: %s", devid, serviceid_len, serviceid,
               request);

    if (strlen("id_service_sync") == serviceid_len && memcmp("id_service_sync", serviceid, serviceid_len) == 0) {
        const char *responseTemp = "{\"id_service_output\":\"This is a sync service output\"}";

        /* Send Service Response To Cloud */
        *response_len = strlen(responseTemp) + 1;
        *response = HAL_Malloc(*response_len);
        if (*response == NULL) {
            TEST_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        memcpy(*response, responseTemp, *response_len);
    } else if (strlen("id_service_async") == serviceid_len && memcmp("id_service_async", serviceid, serviceid_len) == 0) {
        const char *responseTemp = "{\"id_service_output_a\":\"This is a async service output\"}";

        /* Send Service Response To Cloud */
        *response_len = strlen(responseTemp) + 1;
        *response = HAL_Malloc(*response_len);
        if (*response == NULL) {
            TEST_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        memcpy(*response, responseTemp, *response_len);
    }

    return 0;
}

static int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
        const int payload_len)
{
    TEST_TRACE("Down Raw Message, Devid: %d, Payload Length: %d", devid, payload_len);

    if (payload_len <= 16) {
        memcpy(g_user_example_ctx.raw_data_down, payload, payload_len);
    }

    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    TEST_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    TEST_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    } else if (user_example_ctx->subdev_devid == devid) {
        user_example_ctx->slave_initialized = 1;
    }

    return 0;
}

int test_post_property(int idx)
{
    int res = 0;
    char *property_payload = property_data_array[idx];

    res = IOT_Linkkit_Report(1, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));
    TEST_TRACE("Post Property Message ID: %d", res);

    return res;
}

int test_post_event(int idx)
{
    int res = 0;
    char *event_id = event_data_array[idx][0];
    char *event_payload = event_data_array[idx][1];

    res = IOT_Linkkit_TriggerEvent(1, event_id, strlen(event_id), event_payload, strlen(event_payload));
    TEST_TRACE("Post Event Message ID: %d", res);

    return res;
}

int ut_add_subdev(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0, devid = -1;
    devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_SLAVE, meta_info);
    if (devid == FAIL_RETURN) {
        TEST_TRACE("subdev open Failed\n");
        return FAIL_RETURN;
    }
    g_user_example_ctx.subdev_devid = devid;

    TEST_TRACE("subdev open susseed, devid = %d\n", devid);

    res = IOT_Linkkit_Connect(devid);
    if (res == FAIL_RETURN) {
        TEST_TRACE("subdev connect Failed\n");
        return res;
    }
    TEST_TRACE("subdev connect success: devid = %d\n", devid);

    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGIN, NULL, 0);
    if (res == FAIL_RETURN) {
        TEST_TRACE("subdev login Failed\n");
        return res;
    }
    TEST_TRACE("subdev login success: devid = %d\n", devid);
    return devid;
}

int example_logout_subdev(int devid)
{
    int res = 0;

    res = IOT_Linkkit_Report(devid, ITM_MSG_LOGOUT, NULL, 0);
    if (res == FAIL_RETURN) {
        TEST_TRACE("subdev logout Failed\n");
        return res;
    }
    TEST_TRACE("subdev logout success: devid = %d\n", devid);
    return res;
}

int running = 1;
static void *user_dispatch_yield(void *args)
{

    while (running) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
    }

    return NULL;
}

CASE(GW_TEST, case_01_setup_master_device_connection)
{
    int res = 0;
    int cnt = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;

    HAL_SetProductKey(UT_GW_PRODUCT_KEY);
    HAL_SetProductSecret(UT_GW_PRODUCT_SECRET);
    HAL_SetDeviceName(UT_GW_DEVICE_NAME);
    HAL_SetDeviceSecret(UT_GW_DEVICE_SECRET);

    /* Open log */
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, UT_GW_PRODUCT_KEY, strlen(UT_GW_PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, UT_GW_PRODUCT_SECRET, strlen(UT_GW_PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, UT_GW_DEVICE_NAME, strlen(UT_GW_DEVICE_NAME));
    memcpy(master_meta_info.device_secret, UT_GW_DEVICE_SECRET, strlen(UT_GW_DEVICE_SECRET));

    /* reset test ctx */
    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    res = IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);
    ASSERT_EQ(res, SUCCESS_RETURN);

    /* Choose Login Method */
    int dynamic_register = 0;
    res = IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);
    ASSERT_EQ(res, SUCCESS_RETURN);

    g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    ASSERT_EQ(g_user_example_ctx.master_devid, 0);

    /* Choose Whether You Need Post Property/Event Reply, SHOULD be invoked after IOT_Linkkit_Open */
    int post_event_reply = 1;
    res = IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);
    ASSERT_EQ(res, SUCCESS_RETURN);

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
    ASSERT_EQ(res, SUCCESS_RETURN);

    res = HAL_ThreadCreate(&g_user_dispatch_thread, user_dispatch_yield, NULL, NULL, NULL);
    ASSERT_EQ(res, SUCCESS_RETURN);

    while (!g_user_example_ctx.master_initialized) {
        HAL_SleepMs(100);
        HAL_Printf("Wait master inited\n");

        if (++cnt > 100) {
            ASSERT_FAIL();
        }
    }

}

CASE(GW_TEST, case_02_add_first_subdev)
{
    int cnt = 0;

    g_user_example_ctx.subdev_devid = ut_add_subdev((iotx_linkkit_dev_meta_info_t *)&subdev_array[0]);
    ASSERT_EQ(g_user_example_ctx.subdev_devid, 1);

    while (!g_user_example_ctx.slave_initialized) {
        HAL_SleepMs(100);
        HAL_Printf("Wait subdev inited\n");

        if (++cnt > 50) {
            ASSERT_FAIL();
        }
    }
}

CASE(GW_TEST, case_03_post_all_type_of_properties)
{
    int res = 0;
    int cnt = 0;
    int idx = 0;

    for (idx = 0; idx < TEST_PROPERTY_MAX_NUM; idx++) {
        res = test_post_property(idx);
        ASSERT_NE(res, FAIL_RETURN);

        cnt = 0;
        while (g_user_example_ctx.post_reply_msgid != res) {
            HAL_SleepMs(100);
            HAL_Printf("Wait post reply\n");

            if (++cnt > 50) {
                ASSERT_FAIL();
            }
        }

        ASSERT_EQ(g_user_example_ctx.post_reply_code, 200);
    }
}

CASE(GW_TEST, case_04_post_all_type_of_events)
{
    int res = 0;
    int cnt = 0;
    int idx = 0;

    for (idx = 0; idx < TEST_EVENT_MAX_NUM; idx++) {
        res = test_post_event(idx);
        ASSERT_NE(res, FAIL_RETURN);

        cnt = 0;
        while (g_user_example_ctx.post_reply_msgid != res) {
            HAL_SleepMs(100);
            HAL_Printf("Wait post reply\n");

            if (++cnt > 50) {
                ASSERT_FAIL();
            }
        }

        ASSERT_EQ(g_user_example_ctx.post_reply_code, 200);
    }
}

CASE(GW_TEST, case_05_subdev_logout)
{
    int res = 0;

    res = IOT_Linkkit_Report(g_user_example_ctx.subdev_devid, ITM_MSG_LOGOUT, NULL, 0);

    ASSERT_EQ(res, SUCCESS_RETURN);
}

CASE(GW_TEST, case_06_add_second_subdev)
{
    int cnt = 0;
    g_user_example_ctx.slave_initialized = 0;

    g_user_example_ctx.subdev_devid = ut_add_subdev((iotx_linkkit_dev_meta_info_t *)&subdev_array[1]);
    ASSERT_EQ(g_user_example_ctx.subdev_devid, 2);

    while (!g_user_example_ctx.slave_initialized) {
        HAL_SleepMs(100);
        HAL_Printf("Wait subdev inited\n");

        if (++cnt > 50) {
            ASSERT_FAIL();
        }
    }
}

CASE(GW_TEST, case_07_post_raw_data_and_wait_raw_data_reply)
{
    int res = 0;
    int cnt = 0;
    char *raw_data = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E";

    res = IOT_Linkkit_Report(g_user_example_ctx.subdev_devid, ITM_MSG_POST_RAW_DATA,
                             (unsigned char *)raw_data, strlen(raw_data));
    ASSERT_EQ(res, SUCCESS_RETURN);

    while (!g_user_example_ctx.raw_data_down[0]) {
        HAL_SleepMs(100);
        HAL_Printf("Wait down raw data arrived\n");

        if (++cnt > 50) {
            ASSERT_FAIL();
        }
    }

    ASSERT_STR_EQ(g_user_example_ctx.raw_data_down, DOWN_RAW_DATA);
}

CASE(GW_TEST, case_08_post_prop_when_subdev_logout)
{
    int res = 0;
    int cnt = 0;
    char *property_payload = property_data_array[0];

    res = IOT_Linkkit_Report(1, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));
    TEST_TRACE("Post Property Message ID: %d", res);
    ASSERT_NE(res, FAIL_RETURN);

    /* wait get post timeout code100000 after 10second */
    while (g_user_example_ctx.post_reply_msgid != res) {
        HAL_SleepMs(200);
        HAL_Printf("Wait post reply\n");

        if (++cnt > 60) {
            ASSERT_FAIL();
        }
    }

    ASSERT_NE(g_user_example_ctx.post_reply_code, 200);
}

CASE(GW_TEST, case_09_subdev_dynamic_register)
{
    int cnt = 0;
    g_user_example_ctx.slave_initialized = 0;

    g_user_example_ctx.subdev_devid = ut_add_subdev((iotx_linkkit_dev_meta_info_t *)&subdev_array[2]);
    ASSERT_EQ(g_user_example_ctx.subdev_devid, SUBDEV_ID_3);

    while (!g_user_example_ctx.slave_initialized) {
        HAL_SleepMs(100);
        HAL_Printf("Wait subdev inited\n");

        if (++cnt > 50) {
            ASSERT_FAIL();
        }
    }
}

CASE(GW_TEST, case_10_close_connection)
{
    int res;

    res = IOT_Linkkit_Close(g_user_example_ctx.master_devid);
    running = 0;
    HAL_ThreadDelete(g_user_dispatch_thread);

    ASSERT_EQ(res, SUCCESS_RETURN);
}

SUITE(GW_TEST) = {
    ADD_CASE(GW_TEST, case_01_setup_master_device_connection),
    ADD_CASE(GW_TEST, case_02_add_first_subdev),
    ADD_CASE(GW_TEST, case_03_post_all_type_of_properties),
    ADD_CASE(GW_TEST, case_04_post_all_type_of_events),
    ADD_CASE(GW_TEST, case_05_subdev_logout),
    ADD_CASE(GW_TEST, case_06_add_second_subdev),
    /* ADD_CASE(GW_TEST, case_07_post_raw_data_and_wait_raw_data_reply),*/
    ADD_CASE(GW_TEST, case_08_post_prop_when_subdev_logout),
    /* ADD_CASE(GW_TEST, case_09_subdev_dynamic_register),*/
    ADD_CASE(GW_TEST, case_10_close_connection),

    ADD_CASE_NULL
};

#endif  /* defined(DEVICE_MODEL_GATEWAY) */
