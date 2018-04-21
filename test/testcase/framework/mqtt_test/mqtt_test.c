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

#include "iot_export_mqtt.h"


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

// These are pre-defined topics
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MSG_LEN_MAX             (2048)

int cnt = 0;
static int is_subscribed = 0;


void *gpclient;

char *msg_buf = NULL, *msg_readbuf = NULL;

iotx_mqtt_topic_info_t topic_msg;
char msg_pub[128];

int mqtt_client_example(void);
static void mqtt_publish(void *pclient);

static void wifi_service_event(input_event_t *event, void *priv_data) {
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
    LOG("wifi_service_event!");
    mqtt_client_example();
}

static void mqtt_service_event(input_event_t *event, void *priv_data) {

    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ) {
        return;
    }
    LOG("mqtt_service_event!");
    mqtt_publish(priv_data);
}

static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    // print topic name and topic message
    LOG("----");
    LOG("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    LOG("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    LOG("----");
}

void release_buff() {
    if (NULL != msg_buf) {
        aos_free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        aos_free(msg_readbuf);
    }
}

/*
 * Subscribe the topic: IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
 * Publish the topic: IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
 */
static void mqtt_publish(void *pclient) {

    int rc = -1;

    if(is_subscribed == 0) {
        /* Subscribe the specific topic */
        rc = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
        if (rc<0) {
            // IOT_MQTT_Destroy(&pclient);
             LOG("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        }
        is_subscribed = 1;
    }
    else{
        /* Initialize topic information */
        memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

        topic_msg.qos = IOTX_MQTT_QOS1;
        topic_msg.retain = 0;
        topic_msg.dup = 0;

        /* Generate topic message */
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            LOG("Error occur! Exit program");
        }

        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = msg_len;

        rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        if (rc < 0) {
            LOG("error occur when publish");
        }
#ifdef MQTT_ID2_CRYPTO
        LOG("packet-id=%u, publish topic msg='0x%02x%02x%02x%02x'...",
                (uint32_t)rc,
                msg_pub[0], msg_pub[1], msg_pub[2], msg_pub[3]);
#else
        LOG("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);
#endif
    }
    cnt++;
    if(cnt < 2) {
        aos_post_delayed_action(3000, mqtt_publish, pclient);
    } else {

        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);

        aos_msleep(200);

        IOT_MQTT_Destroy(&pclient);

        release_buff();
        aos_loop_exit();
        is_subscribed = 0;
        cnt = 0;
    }
}


void event_handle_mqtt(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            LOG("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            LOG("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            LOG("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            LOG("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            LOG("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            LOG("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            LOG("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            LOG("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            LOG("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            LOG("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            LOG("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            LOG("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            LOG("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            LOG("Should NOT arrive here.");
            break;
    }
}

/*
 * initialization parameter: mqtt_params
 */
int mqtt_client_example(void)
{
    int rc = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

    if (msg_buf != NULL) {
        return rc;
    }

    if (NULL == (msg_buf = (char *)aos_malloc(MSG_LEN_MAX))) {
        LOG("not enough memory");
        rc = -1;
        release_buff();
        return rc;
    }

    if (NULL == (msg_readbuf = (char *)aos_malloc(MSG_LEN_MAX))) {
        LOG("not enough memory");
        rc = -1;
        release_buff();
        return rc;
    }

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        LOG("AUTH request failed!");
        rc = -1;
        release_buff();
        return rc;
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

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */

    gpclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == gpclient) {
        LOG("MQTT construct failed");
        rc = -1;
        release_buff();
        //aos_unregister_event_filter(EV_SYS,  mqtt_service_event, gpclient);
    } else{
        aos_register_event_filter(EV_SYS,  mqtt_service_event, gpclient);
    }
    YUNIT_ASSERT(rc == 0);

    return rc;
}

// normal case for mqtt subscribe & publish
static void test_mqtt_case1(void)
{
    int ret = 0;
    ret = mqtt_client_example();
    printf("case1 ret%d\n", ret);
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

static void test_mqtt_case3(void)
{
    void *pclient = NULL;
    /* uninitialized pointer can not be passed as a param otherwise it may cause crash on linux host */
    #if 0
    iotx_mqtt_param_t *mqtt_param;
    pclient = IOT_MQTT_Construct(mqtt_param);
    #endif

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
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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

    mqtt_params.username = pconn_info->username;
    mqtt_params.password = NULL;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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

    mqtt_params.username = pconn_info->username;
    mqtt_params.password = NULL;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = -1;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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

    mqtt_params.username = pconn_info->username;
    mqtt_params.password = NULL;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = NULL;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle_mqtt;
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

    mqtt_client_example();

    sprintf(msg_pub, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = strlen(msg_buf);

    ret = IOT_MQTT_Publish(gpclient, TOPIC_DATA, &topic);
    YUNIT_ASSERT(ret<0);
}

// IOT_MQTT_Publish:payload_len out of size
static void test_mqtt_case13(void)
{
    int  ret = -1;
    void *pclient = NULL;
    iotx_mqtt_topic_info_t topic;

    mqtt_client_example();

    sprintf(msg_pub, "{\"attr_name\":\"temperature\", \"attr_value\":\"199\"}");
    memset(&topic, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic.qos = IOTX_MQTT_QOS1;
    topic.retain = 0;
    topic.dup = 0;
    topic.payload = msg_buf;
    topic.payload_len = MSG_LEN_MAX;

    ret = IOT_MQTT_Publish(gpclient, TOPIC_DATA, &topic);
    IOT_MQTT_Destroy(&gpclient);

    YUNIT_ASSERT(ret < 0);
}

static yunit_test_case_t aos_mqtt_testcases[] = {
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
    { "mqtt", init, cleanup, setup, teardown, aos_mqtt_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_mqtt(void)
{    
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_mqtt);
