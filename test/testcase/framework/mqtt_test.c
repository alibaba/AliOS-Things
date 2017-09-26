/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <aos/aos.h>

#include <yunit.h>
#include <yts.h>

#include "iot_import.h"
#include "iot_export.h"

#define PRODUCT_KEY             "edWuENTjOf4"
#define DEVICE_NAME             "zmmqtttest-device"
#define DEVICE_SECRET           "AU6hOT7CCKitFKb3d9thMjaEszHzCfTA"

// These are pre-defined topics
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MSG_LEN_MAX             (1024)

static int cnt = 0;

static int is_subscribed = 0;

static void *pclient;

static char *msg_buf = NULL, *msg_readbuf = NULL;

static iotx_mqtt_topic_info_t topic_msg;
static char msg_pub[128];

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

void release_buff() {
    if (NULL != msg_buf) {
        HAL_Free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        HAL_Free(msg_readbuf);
    }
}

static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    // print topic name and topic message
    printf("----");
    printf("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    printf("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    printf("----");
}

static void mqtt_test() {

    int rc = -1;

    if(is_subscribed == 0) {
        /* Subscribe the specific topic */
        rc = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
        if (rc < 0) {
            IOT_MQTT_Destroy(&pclient);
            printf("IOT_MQTT_Subscribe() failed, rc = %d", rc);
            rc = -1; 
            YUNIT_ASSERT(0 == rc);
            return;
        }
        is_subscribed = 1;

        HAL_SleepMs(1000);
        YUNIT_ASSERT(0 < rc);
    }



    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    //strcpy(msg_pub, "message: hello! start!");

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;

        /* Generate topic message */
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            printf("Error occur! Exit program");
            rc = -1;
        }

        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = msg_len;

        rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        if (rc < 0) {
            printf("error occur when publish");
            rc = -1;
        }
        YUNIT_ASSERT(0 < rc); 
#ifdef MQTT_ID2_CRYPTO
        printf("packet-id=%u, publish topic msg='0x%02x%02x%02x%02x'...",
                      (uint32_t)rc,
                      msg_pub[0], msg_pub[1], msg_pub[2], msg_pub[3]
                     );
#else
        printf("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);
#endif

    cnt++;

    if(cnt < 2) {
        aos_post_delayed_action(200, mqtt_test, NULL);
    } else {

        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
    
        HAL_SleepMs(200);

        IOT_MQTT_Destroy(&pclient);

        release_buff();

        is_subscribed = 0;
        cnt = 0;
        aos_loop_exit();
    }
}


static void mqtt_service_event(input_event_t *event, void *priv_data) {
    LOG("wifi_service_event!");
    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ) {
        return;
    }

    mqtt_test();
}

void mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            printf("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            printf("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            printf("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            printf("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            printf("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            printf("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            printf("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            printf("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            printf("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            printf("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            printf("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            printf("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            printf("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            printf("Should NOT arrive here.");
            break;
    }
}

void* mqtt_init()
{
    int               ret = -1;
    void*             pclient = NULL;
    iotx_conn_info_pt pconn_info = NULL;
    iotx_mqtt_param_t mqtt_params;
    int start_time = 0;

    char *tmp_ptr = HAL_Malloc(1024);

    start_time = HAL_UptimeMs();
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void**)&pconn_info)) {
        return NULL;
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
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    start_time = HAL_UptimeMs();
    pclient = IOT_MQTT_Construct(&mqtt_params);
    printf("construct mqtt client...%d ms\n", HAL_UptimeMs()-start_time);

    HAL_Free(tmp_ptr);
    return pclient;
}

int mqtt_client_example(void)
{
    int rc = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

    if (NULL == (msg_buf = (char *)HAL_Malloc(MSG_LEN_MAX))) {
        printf("not enough memory");
        rc = -1;
        release_buff();
        YUNIT_ASSERT(0 == rc); 
        return rc;
    }

    if (NULL == (msg_readbuf = (char *)HAL_Malloc(MSG_LEN_MAX))) {
        printf("not enough memory");
        rc = -1;
        release_buff();
        YUNIT_ASSERT(0 == rc); 
        return rc;
    }

    aos_register_event_filter(EV_SYS,  mqtt_service_event, NULL);

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        printf("AUTH request failed!");
        rc = -1;
        release_buff();
        YUNIT_ASSERT(0 == rc); 
        return rc;
    }
    YUNIT_ASSERT(0 == rc);

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
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        printf("MQTT construct failed");
        rc = -1;
        release_buff();
        YUNIT_ASSERT(0 == rc); 
    }
    YUNIT_ASSERT(0 == rc);

    return rc;
}

// normal case for mqtt subscribe & publish
static void test_mqtt_case1(void)
{
    int ret = 0;
    ret = mqtt_client_example();
    YUNIT_ASSERT(0 == ret); 
    aos_loop_run();
}

// IOT_MQTT_Construct:param=NULL
static void test_mqtt_case2(void)
{
    void *pclient = NULL;
    pclient = IOT_MQTT_Construct((iotx_mqtt_param_pt)NULL);
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param=uninitialized pointer
static void test_mqtt_case3(void)
{
    void *pclient = NULL;
    // iotx_mqtt_param_pt mqtt_param;
    iotx_mqtt_param_t *mqtt_param;
    pclient = IOT_MQTT_Construct(mqtt_param);
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param = 0
static void test_mqtt_case4(void)
{
    void *pclient = NULL;
    iotx_mqtt_param_t mqtt_param;
    memset(&mqtt_param, 0, sizeof(iotx_mqtt_param_t));
    pclient = IOT_MQTT_Construct(&mqtt_param);
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.host = NULL
static void test_mqtt_case5(void)
{
    int               ret = -1;
    void              *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;

    //mqtt_params.host = pconn->host_name;
    mqtt_params.host = NULL;

    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.client_id = NULL
static void test_mqtt_case6(void)
{
    int               ret = -1;
    void              *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;

    // mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.client_id = NULL;

    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.user_name = NULL
static void test_mqtt_case7(void)
{
    int               ret = -1;
    void              *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;

    // mqtt_params.user_name = pconn_info->username;
    mqtt_params.username = NULL;

    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.passwd= NULL
static void test_mqtt_case8(void)
{
    int               ret = -1;
    void              *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;

    // mqtt_params.password = pconn_info->password;
    mqtt_params.password = NULL;

    mqtt_params.pub_key = pconn_info->pub_key;
    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 1000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.request_timeout_ms invalid
static void test_mqtt_case9(void)
{
    int  ret = -1;
    void *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    // mqtt_params.request_timeout_ms = 2000;
    mqtt_params.request_timeout_ms = -1;

    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 6000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    YUNIT_ASSERT(NULL != pclient);

    IOT_MQTT_Destroy(&pclient);
}

// IOT_MQTT_Construct:param.pread_buf = NULL
static void test_mqtt_case10(void)
{
    int  ret = -1;
    void *pclient = NULL;
    iotx_mqtt_param_t mqtt_params;
    iotx_conn_info_pt pconn_info;

    ret = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    YUNIT_ASSERT(ret >= 0);

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

    // mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.pread_buf = NULL;

    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp = mqtt_event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL != pclient) {
        IOT_MQTT_Destroy(&pclient);
    }
    YUNIT_ASSERT(NULL == pclient);
}

// IOT_MQTT_Construct:param.pread_buf = NULL
static void test_mqtt_case11(void)
{
    YUNIT_ASSERT(IOT_MQTT_Destroy((void**)NULL) < 0);
}

// IOT_MQTT_Publish:handle=NULL
static void test_mqtt_case12(void)
{
    int ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    pclient = mqtt_init();

    sprintf(msg_pub, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = strlen(msg_buf);

    ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic);
    YUNIT_ASSERT(ret<0);
}

// IOT_MQTT_Publish:payload_len out of size
static void test_mqtt_case13(void)
{
    int  ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    pclient = mqtt_init();

    sprintf(msg_pub, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = MSG_LEN_MAX;

    ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic);
    IOT_MQTT_Destroy(&pclient);

    YUNIT_ASSERT(ret < 0);
}

static yunit_test_case_t aos_basic_testcases[] = {
    { "case1", test_mqtt_case1},
    { "case2", test_mqtt_case2},
    { "case3", test_mqtt_case3},
    { "case4", test_mqtt_case4},
    { "case5", test_mqtt_case5},
    { "case6", test_mqtt_case6},
    { "case7", test_mqtt_case7},
    { "case8", test_mqtt_case8},
//    { "case9", test_mqtt_case9},
    { "case10", test_mqtt_case10},
    { "case11", test_mqtt_case11},
/*    { "case12", test_mqtt_case12},
    { "case13", test_mqtt_case13},
    { "case14", test_mqtt_case14},
    { "case1", test_mqtt_case1},*/
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "mqtt", init, cleanup, setup, teardown, aos_basic_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_mqtt(void)
{    
    yunit_add_test_suites(suites);
}
