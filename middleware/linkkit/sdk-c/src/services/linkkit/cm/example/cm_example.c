/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"

#define IOTX_PRODUCT_KEY        "a1AzoSi5TMc"
#define IOTX_DEVICE_NAME        "test_light_for_dm_cmp_2"
#define IOTX_DEVICE_SECRET      "XjdICFKdWrm8pDughFg2cAImO6O0aDhE"
#define IOTX_DEVICE_ID          "IoTxHttpTestDev_001"


/* These are pre-defined topics */
#define TOPIC_DATA              "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/data"
#define TOPIC_SEND              "/"IOTX_PRODUCT_KEY"/test_light_for_dm_cmp_444/data"
#define TOPIC_UPDATE            "/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME"/update"
#define TOPIC_ERROR             "/ext/error/"IOTX_PRODUCT_KEY"/"IOTX_DEVICE_NAME

#if 0
#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


static void _register_func(iotx_cm_send_peer_t *source, iotx_cm_message_info_t *msg, void *user_data)
{
    EXAMPLE_TRACE("source %s:%s\n", source->product_key, source->device_name);
    EXAMPLE_TRACE("URI %.*s\n", msg->URI_length, msg->URI);
    EXAMPLE_TRACE("parameter %.*s\n", msg->payload_length, (char *)msg->payload);
}


static void _event_handle(void *pcontext, iotx_cm_event_msg_t *msg, void *user_data)
{
    EXAMPLE_TRACE("event %d\n", msg->event_id);

    if (IOTX_CM_EVENT_REGISTER_RESULT == msg->event_id) {
        iotx_cm_event_result_pt result = (iotx_cm_event_result_pt)msg->msg;

        EXAMPLE_TRACE("register result\n");
        EXAMPLE_TRACE("result %d\n", result->result);
        EXAMPLE_TRACE("URI %s\n", result->URI);
    } else if (IOTX_CM_EVENT_UNREGISTER_RESULT == msg->event_id) {
        iotx_cm_event_result_pt result = (iotx_cm_event_result_pt)msg->msg;

        EXAMPLE_TRACE("unregister result\n");
        EXAMPLE_TRACE("result %d\n", result->result);
        EXAMPLE_TRACE("URI %s\n", result->URI);
    } else if (IOTX_CM_EVENT_NEW_DATA_RECEIVED == msg->event_id) {
        iotx_cm_new_data_t *new_data = (iotx_cm_new_data_t *)msg->msg;
        _register_func(new_data->peer, new_data->message_info, user_data);
    }
}


/* local conn */
int _ut_test_3()
{
    int ret = -1;
    void *local_connectivity = NULL;
    void *cloud_connectivity = NULL;
    iotx_cm_init_param_t param = {0};
    iotx_cm_connectivity_param_t create_param = {0};
    iotx_cm_connectivity_cloud_param_t cloud_param = {0};
    iotx_cm_connectivity_alcs_param_t alcs_param = {0};
    iotx_cm_register_param_t register_param = {0};
    iotx_cm_add_service_param_t service_param = {0};
    iotx_cm_message_info_t message_info = {0};

    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;
    param.event_func = _event_handle;
    param.user_data = NULL;
    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);
    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = &cloud_param;
    cloud_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity) {
        EXAMPLE_TRACE("cloud connectivity create fail first");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    alcs_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_LOCAL;
    create_param.alcs_param = &alcs_param;
    local_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == local_connectivity) {
        EXAMPLE_TRACE("local connectivity create fail first");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(local_connectivity, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("local connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    register_param.mail_box = NULL;
    register_param.register_func = _register_func;
    register_param.URI = TOPIC_DATA;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(NULL, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("register fail");
        return FAIL_RETURN;
    }

    service_param.auth_type = IOTX_CM_MESSAGE_NO_AUTH;
    service_param.mail_box = NULL;
    service_param.service_func = _register_func;
    service_param.URI = TOPIC_DATA;
    service_param.user_data = NULL;
    ret = iotx_cm_serv_add(NULL, &service_param, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("add service fail");
        return FAIL_RETURN;
    }

    message_info.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    message_info.conn_ctx = NULL;
    message_info.payload = "data!!!!!\n\n";
    message_info.payload_length = strlen("data!!!!!\n\n");
    message_info.URI = TOPIC_DATA;
    message_info.URI_length = strlen(TOPIC_DATA);
    ret = iotx_cm_send(NULL, NULL, &message_info, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("send fail");
        return FAIL_RETURN;
    }

    HAL_SleepMs(2000);

    iotx_cm_close(&local_connectivity, NULL);

    iotx_cm_deinit(NULL);

    return SUCCESS_RETURN;
}

/* one connectivity create and destory more time */
int _ut_test_2()
{
    int ret = -1;
    void *cloud_connectivity = NULL;
    iotx_cm_init_param_t param = {0};
    iotx_cm_connectivity_param_t create_param = {0};
    iotx_cm_connectivity_cloud_param_t cloud_param = {0};
    iotx_cm_register_param_t register_param = {0};
    iotx_cm_message_info_t message_info = {0};
    iotx_cm_unregister_param_t unregister_param = {0};

    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;
    param.event_func = _event_handle;
    param.user_data = NULL;
    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);
    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity) {
        EXAMPLE_TRACE("Cloud connectivity create fail first");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    iotx_cm_close(&cloud_connectivity, NULL);

    HAL_SleepMs(5000);

    cloud_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity) {
        EXAMPLE_TRACE("Cloud connectivity create fail first");
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register\n");
    register_param.URI = TOPIC_DATA;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        /*iotx_cm_deinit(NULL);
        return FAIL_RETURN;*/
    }

    register_param.URI = TOPIC_ERROR;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        /*iotx_cm_deinit(NULL);
        return FAIL_RETURN;*/
    }

    iotx_cm_close(&cloud_connectivity, NULL);

    HAL_SleepMs(5000);

    cloud_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity) {
        EXAMPLE_TRACE("Cloud connectivity create fail first");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register\n");
    register_param.URI = TOPIC_DATA;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    register_param.URI = TOPIC_ERROR;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register success \n");

    iotx_cm_close(&cloud_connectivity, NULL);

    HAL_SleepMs(5000);

    cloud_connectivity = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity) {
        EXAMPLE_TRACE("Cloud connectivity create fail first");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("send\n");

    message_info.URI = HAL_Malloc(strlen(TOPIC_SEND) + 1);
    memset(message_info.URI, 0x0, strlen(TOPIC_SEND) + 1);
    strncpy(message_info.URI, TOPIC_SEND, strlen(TOPIC_SEND));
    message_info.payload = HAL_Malloc(strlen("{hello world!}"));
    memset(message_info.payload, 0x0, strlen("{hello world!}") + 1);
    strncpy(message_info.payload, "{hello world!}", strlen("{hello world!}"));
    message_info.payload_length = strlen(message_info.payload);

    ret = iotx_cm_send(cloud_connectivity, NULL, &message_info, NULL);

    HAL_Free(message_info.URI);
    HAL_Free(message_info.payload);

    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("send fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(2000);
#endif

    HAL_SleepMs(5000);

    EXAMPLE_TRACE("unregister\n");
    unregister_param.URI = TOPIC_DATA;
    ret = iotx_cm_serv_unreg(cloud_connectivity, &unregister_param, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("unregister fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(200);
#endif

    iotx_cm_close(&cloud_connectivity, NULL);

    iotx_cm_deinit(NULL);

    return SUCCESS_RETURN;
}

/* mqtt & coap & http */
int _ut_test_1()
{
    int ret = -1;
    void *cloud_connectivity_mqtt = NULL;
    void *cloud_connectivity_http = NULL;
    void *cloud_connectivity_coap = NULL;
    iotx_cm_init_param_t param = {0};
    iotx_cm_connectivity_param_t create_param = {0};
    iotx_cm_connectivity_cloud_param_t cloud_param = {0};
    iotx_cm_register_param_t register_param = {0};
    iotx_cm_message_info_t message_info = {0};
    iotx_cm_unregister_param_t unregister_param = {0};

    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;
    param.event_func = _event_handle;
    param.user_data = NULL;

    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);
    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity_mqtt = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity_mqtt) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT create fail");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity_mqtt, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_COAP;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity_coap = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity_coap) {
        EXAMPLE_TRACE("Cloud connectivity via COAP create fail");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity_coap, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via COAP connectivity fail");
        return FAIL_RETURN;
    }

    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity_http = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity_http) {
        EXAMPLE_TRACE("Cloud connectivity via HTTP create fail");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity_http, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via HTTP connectivity fail");
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register\n");
    register_param.URI = TOPIC_DATA;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity_mqtt, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    register_param.URI = TOPIC_ERROR;
    register_param.register_func = _register_func;
    register_param.user_data = NULL;
    ret = iotx_cm_serv_reg(cloud_connectivity_mqtt, &register_param, 1, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register success \n");

    EXAMPLE_TRACE("send\n");

    message_info.URI = HAL_Malloc(strlen(TOPIC_SEND) + 1);
    memset(message_info.URI, 0x0, strlen(TOPIC_SEND) + 1);
    strncpy(message_info.URI, TOPIC_SEND, strlen(TOPIC_SEND));
    message_info.payload = HAL_Malloc(strlen("{hello world!}"));
    memset(message_info.payload, 0x0, strlen("{hello world!}") + 1);
    strncpy(message_info.payload, "{hello world!}", strlen("{hello world!}"));
    message_info.payload_length = strlen(message_info.payload);

    ret = iotx_cm_send(NULL, NULL, &message_info, NULL);

    HAL_Free(message_info.URI);
    HAL_Free(message_info.payload);

    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("send fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(2000);
#endif

    HAL_SleepMs(5000);

    EXAMPLE_TRACE("unregister\n");
    unregister_param.URI = TOPIC_DATA;
    ret = iotx_cm_serv_unreg(cloud_connectivity_mqtt, &unregister_param, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("unregister fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(200);
#endif

    iotx_cm_close(&cloud_connectivity_mqtt, NULL);

    iotx_cm_deinit(NULL);

    return SUCCESS_RETURN;
}


/* base
 * mutli-subscribe
 */
int _ut_test_base()
{
    int ret = -1;
    void *cloud_connectivity_mqtt = NULL;
    iotx_cm_init_param_t param = {0};
    iotx_cm_connectivity_param_t create_param = {0};
    iotx_cm_connectivity_cloud_param_t cloud_param = {0};
    iotx_cm_register_param_t register_param[2];
    iotx_cm_message_info_t message_info = {0};
    iotx_cm_unregister_param_t unregister_param = {0};

    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;
    param.event_func = _event_handle;
    param.user_data = NULL;

    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);
    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    cloud_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity_mqtt = iotx_cm_open(&create_param, NULL);
    if (NULL == cloud_connectivity_mqtt) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT create fail");
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity_mqtt, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("Cloud connectivity via MQTT connectivity fail");
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register\n");
    register_param[0].URI = TOPIC_DATA;
    register_param[0].register_func = _register_func;
    register_param[0].user_data = NULL;
    register_param[1].URI = TOPIC_ERROR;
    register_param[1].register_func = _register_func;
    register_param[1].user_data = NULL;
    ret = iotx_cm_serv_reg(NULL, register_param, 2, NULL);
    if (FAIL_RETURN == ret) {
        printf("mqtt register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register success \n");

    EXAMPLE_TRACE("send\n");

    message_info.URI = HAL_Malloc(strlen(TOPIC_SEND) + 1);
    memset(message_info.URI, 0x0, strlen(TOPIC_SEND) + 1);
    strncpy(message_info.URI, TOPIC_SEND, strlen(TOPIC_SEND));
    message_info.payload = HAL_Malloc(strlen("{hello world!}"));
    memset(message_info.payload, 0x0, strlen("{hello world!}") + 1);
    strncpy(message_info.payload, "{hello world!}", strlen("{hello world!}"));
    message_info.payload_length = strlen(message_info.payload);

    ret = iotx_cm_send(NULL, NULL, &message_info, NULL);

    HAL_Free(message_info.URI);
    HAL_Free(message_info.payload);

    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("send fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(2000);
#endif

    HAL_SleepMs(5000);

    EXAMPLE_TRACE("unregister\n");
    unregister_param.URI = TOPIC_DATA;
    ret = iotx_cm_serv_unreg(cloud_connectivity_mqtt, &unregister_param, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("unregister fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(200);
#endif

    iotx_cm_close(&cloud_connectivity_mqtt, NULL);

    iotx_cm_deinit(NULL);

    return SUCCESS_RETURN;
}


int cm_client()
{
    _ut_test_base();
#if 0
#if 1
    int ret = -1;
    int user_data = 10;
    void *cloud_connectivity = NULL;
    void *alcs_connectivity = NULL;
    iotx_cm_init_param_t param = {0};
    iotx_cm_register_param_t register_param = {0};
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    iotx_cm_add_service_param_t add_service = {0};
#endif
    iotx_cm_send_peer_t cloud_peer;
    iotx_cm_message_info_t message_info = {0};
    iotx_cm_unregister_param_t unregister_param = {0};
    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;

    param.event_func = _event_handle;
    param.user_data = &user_data;

    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);

    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    iotx_cm_connectivity_param_t create_param = {0};
    iotx_cm_connectivity_cloud_param_t cloud_param = {0};
    cloud_param.protocol_type =
                IOTX_CM_CONNECTION_PROTOCOL_TYPE_MQTT;//IOTX_CM_CONNECTION_PROTOCOL_TYPE_COAP; //IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity = iotx_cm_open(&create_param, NULL);

    if (NULL == cloud_connectivity) {
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);

    if (FAIL_RETURN == ret) {
        printf("connect fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    //#if 0
    iotx_cm_connectivity_alcs_param_t alcs_param = {0};
    alcs_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_ALCS;
    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_LOCAL;
    create_param.alcs_param = &alcs_param;
    alcs_connectivity = iotx_cm_open(&create_param, NULL);

    if (NULL == alcs_connectivity) {
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(alcs_connectivity, NULL);
#endif
#if 1
    iotx_cm_close(&cloud_connectivity, NULL);
    //IOTX_CM_Connectivity_Destory(&alcs_connectivity);
    HAL_SleepMs(5000);

    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity = iotx_cm_open(&create_param, NULL);

    if (NULL == cloud_connectivity) {
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);

    iotx_cm_close(&cloud_connectivity, NULL);

    HAL_SleepMs(5000);

    create_param.connectivity_type = IOTX_CM_CONNECTIVITY_TYPE_CLOUD;
    create_param.cloud_param = & cloud_param;
    cloud_connectivity = iotx_cm_open(&create_param, NULL);

    if (NULL == cloud_connectivity) {
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(cloud_connectivity, NULL);
#endif
    EXAMPLE_TRACE("register\n");
    register_param.URI = TOPIC_DATA;
    register_param.message_type = IOTX_CM_MESSAGE_REQUEST;
    register_param.register_func = _register_func;
    register_param.user_data = &user_data;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, NULL);
    if (FAIL_RETURN == ret) {
        printf("register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register success \n");

    EXAMPLE_TRACE("register error\n");
    register_param.URI = TOPIC_ERROR;
    register_param.message_type = IOTX_CM_MESSAGE_REQUEST;
    register_param.register_func = _register_func;
    register_param.user_data = &user_data;
    ret = iotx_cm_serv_reg(cloud_connectivity, &register_param, NULL);
    if (FAIL_RETURN == ret) {
        printf("register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    EXAMPLE_TRACE("register success \n");

#ifdef CONFIG_CM_SUPPORT_LOCAL_CONN
    //#if 0
    add_service.auth_type = IOTX_CM_MESSAGE_NO_AUTH;
    add_service.message_type = IOTX_CM_MESSAGE_REQUEST;
    add_service.service_func = NULL;
    add_service.URI = TOPIC_DATA;
    add_service.user_data = NULL;
    ret = iotx_cm_serv_add(alcs_connectivity, &add_service, NULL);
#endif /* CONFIG_CM_SUPPORT_LOCAL_CONN */

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("yield fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(2000);
#endif
    iotx_cm_close(&alcs_connectivity, NULL);

    EXAMPLE_TRACE("send\n");
    memset(&cloud_peer, 0x0, sizeof(iotx_cm_send_peer_t));

    HAL_GetProductKey(cloud_peer.product_key);
    HAL_GetDeviceName(cloud_peer.device_name);

    //    message_info.id = 0;
    //    message_info.message_type = IOTX_CM_MESSAGE_REQUEST;
    message_info.URI = LITE_malloc(strlen(TOPIC_SEND) + 1, MEM_MAGIC, "CM");
    memset(message_info.URI, 0x0, strlen(TOPIC_SEND) + 1);
    strncpy(message_info.URI, TOPIC_SEND, strlen(TOPIC_SEND));
    //    message_info.method = LITE_malloc(strlen("thing.data") + 1, MEM_MAGIC, "CM");
    //    memset(message_info.method, 0x0, strlen("thing.data") + 1);
    //    strncpy(message_info.method, "thing.data", strlen("thing.data"));
    message_info.payload = LITE_malloc(strlen("{hello world!}"), MEM_MAGIC, "CM");
    memset(message_info.payload, 0x0, strlen("{hello world!}") + 1);
    strncpy(message_info.payload, "{hello world!}", strlen("{hello world!}"));
    message_info.payload_length = strlen(message_info.payload);

    //    ret = iotx_cm_send(alcs_connectivity, NULL, &message_info, NULL);
    ret = iotx_cm_send(NULL, NULL, &message_info, NULL);

    LITE_free(message_info.URI);
    //    LITE_free(message_info.method);
    LITE_free(message_info.payload);

    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("send fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(2000);
#endif

    HAL_SleepMs(5000);

    EXAMPLE_TRACE("unregister\n");
    unregister_param.URI = TOPIC_DATA;
    ret = iotx_cm_serv_unreg(cloud_connectivity, &unregister_param, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("unregister fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 0)
    ret = iotx_cm_yield(200, NULL);
    if (FAIL_RETURN == ret) {
        EXAMPLE_TRACE("register fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#else
    HAL_SleepMs(200);
#endif
    //    IOTX_CM_Connectivity_Destory(&cloud_connectivity, NULL);

    iotx_cm_close(&alcs_connectivity, NULL);

    iotx_cm_deinit(NULL);
#else

    iotx_cm_init_param_t param = {0};
    cm_connectivity_param_t create_param = {0};
    cm_connectivity_ota_param_t ota_param = {0};
    void *ota_connectivity = NULL;
    int ret = SUCCESS_RETURN;

    param.domain_type = IOTX_CM_CLOUD_DOMAIN_ShangHai;

    param.event_func = _event_handle;
    param.user_data = NULL;

    EXAMPLE_TRACE("init\n");
    ret = iotx_cm_init(&param, NULL);

    if (FAIL_RETURN == ret) {
        printf("init fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }

    ota_param.protocol_type = IOTX_CM_CONNECTION_PROTOCOL_TYPE_HTTP;
    ota_param.url = "http://baidu.com";
    ota_param.port = 80;
    create_param.connectivity_type = CM_CONNECTIVIT_TYPE_OTA;
    create_param.ota_param = &ota_param;
    ota_connectivity = iotx_cm_open(&create_param, NULL);

    if (NULL == ota_connectivity) {
        return FAIL_RETURN;
    }

    ret = iotx_cm_connect(ota_connectivity);

    if (FAIL_RETURN == ret) {
        printf("connect fail\n");
        iotx_cm_deinit(NULL);
        return FAIL_RETURN;
    }
#endif
#endif

    return SUCCESS_RETURN;
}
#endif

int main(int argc, char **argv)
{
#if 0
    IOT_OpenLog("cm");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    EXAMPLE_TRACE("start!\n");

    HAL_SetProductKey(IOTX_PRODUCT_KEY);
    HAL_SetDeviceName(IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret(IOTX_DEVICE_SECRET);

    cm_client();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!\n");
#endif
    return 0;
}
