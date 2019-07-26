#if defined(MQTT_COMM_ENABLED)
#include "sdk-testsuites_internal.h"
#include "cut.h"

#include "utest_mqtt.h"

/*************************************************************************************/
/* iot_mqtt_construct*/
/*************************************************************************************/

DATA(MQTT_CONS)
{
    void *mqtt;
};

SETUP(MQTT_CONS)
{
    data->mqtt = NULL;
    memset(msg_buf, 0, MSG_LEN_MAX);
    memset(msg_readbuf, 0, MSG_LEN_MAX);
    memset(msg_pub, 0, MSG_LEN_MAX);

    TEST_REPLACE_DEVCERT(&UTEST_MQTT_PRODUCT_KEY, &UTEST_MQTT_PRODUCT_SECRET, &UTEST_MQTT_DEVICE_NAME,
                         &UTEST_MQTT_DEVICE_SECRET);
    utest_mqtt_update_topic();
}

TEARDOWN(MQTT_CONS)
{
    if (data->mqtt != NULL) {
        IOT_MQTT_Destroy(&(data->mqtt));
        data->mqtt = NULL;
    }
}

/*mqtt_params 入参为空*/
CASEs(MQTT_CONS, case_01)
{
    HAL_SetProductKey(UTEST_MQTT_PRODUCT_KEY);
    HAL_SetDeviceName(UTEST_MQTT_DEVICE_NAME);
    HAL_SetDeviceSecret(UTEST_MQTT_DEVICE_SECRET);
    HAL_SetProductSecret(UTEST_MQTT_PRODUCT_SECRET);

    data->mqtt = IOT_MQTT_Construct((iotx_mqtt_param_pt)NULL);
    ASSERT_NOT_NULL(data->mqtt);
}

/* IOT_MQTT_Construct:param.port=0, make it faile */
CASEs(MQTT_CONS, case_02)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_TRUE(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    mqtt_params.port = 0; /* pconn_info->port; */
    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    if (data->mqtt) {
        ASSERT_FAIL();
    }
}

/* IOT_MQTT_Construct:param.client_id = NULL */
CASEs(MQTT_CONS, case_03)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_GE(ret, 0);
    ASSERT_TRUE(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;

    /* mqtt_params.client_id = pconn_info->client_id; */
    mqtt_params.client_id = NULL;

    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != data->mqtt) {
        ASSERT_FAIL();
    }
}
/* IOT_MQTT_Construct:param.user_name = NULL */
CASEs(MQTT_CONS, case_04)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_GE(ret, 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;

    /* mqtt_params.user_name = pconn_info->username; */
    mqtt_params.username = NULL;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != data->mqtt) {
        ASSERT_FAIL();
    }
}

/* IOT_MQTT_Construct:param.passwd= NULL */
CASEs(MQTT_CONS, case_05)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_GE(ret, 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;

    /* mqtt_params.password = pconn_info->password; */
    mqtt_params.password = NULL;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != data->mqtt) {
        ASSERT_FAIL();
    }
}

/* IOT_MQTT_Construct:param.pub_key = NULL */
CASEs(MQTT_CONS, case_06)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_TRUE(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;

    /* mqtt_params.pub_key = pconn_info->pub_key; */
    mqtt_params.pub_key = NULL;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 6000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;
    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
#ifdef SUPPORT_TLS
    ASSERT_NULL(data->mqtt);
#else
    ASSERT_NOT_NULL(data->mqtt);
#endif
}

// normal case, should pass
CASEs(MQTT_CONS, case_07)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_TRUE(ret >= 0);

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
    mqtt_params.keepalive_interval_ms = 6000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    ASSERT_NOT_NULL(data->mqtt);
}

// nested construct + simple publish, should pass
CASEs(MQTT_CONS, case_08)
{
    int ret = -1;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(UTEST_MQTT_PRODUCT_KEY, UTEST_MQTT_DEVICE_NAME, UTEST_MQTT_DEVICE_SECRET, (void **)&pconn_info);
    ASSERT_TRUE(ret >= 0);

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
    mqtt_params.keepalive_interval_ms = 30000;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = utest_mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    printf("data->mqtt = IOT_MQTT_Construct() = %p\n", data->mqtt);
    ASSERT_NOT_NULL(data->mqtt);

    data->mqtt = IOT_MQTT_Construct(&mqtt_params);
    printf("data->mqtt = IOT_MQTT_Construct() = %p\n", data->mqtt);
    ASSERT_NOT_NULL(data->mqtt);

    ret = IOT_MQTT_Publish_Simple(data->mqtt, UTEST_TOPIC_UPDATE, 1, "abcd", 4);
    printf("ret = IOT_MQTT_Publish_Simple('abcd') = %d\n", ret);
    ASSERT_GT(ret, 1);

    ret = IOT_MQTT_Publish_Simple(data->mqtt, UTEST_TOPIC_UPDATE, 0, "xyz", 3);
    printf("ret = IOT_MQTT_Publish_Simple('xyz') = %d\n", ret);
    ASSERT_EQ(ret, 0);
}

SUITE(MQTT_CONS) = {
    ADD_CASE(MQTT_CONS, case_01),
    ADD_CASE(MQTT_CONS, case_02),
    ADD_CASE(MQTT_CONS, case_03),
    ADD_CASE(MQTT_CONS, case_04),
    ADD_CASE(MQTT_CONS, case_05),
    ADD_CASE(MQTT_CONS, case_06),
    ADD_CASE(MQTT_CONS, case_07),
    ADD_CASE(MQTT_CONS, case_08),
    ADD_CASE_NULL
};
#endif
