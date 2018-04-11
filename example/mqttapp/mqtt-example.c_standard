/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "aos/log.h"
//#include "json_parser.h"
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>
//#include <k_err.h>
//#include "aliot_platform.h"
//#include "aliot_log.h"
//#include "aliot_mqtt_client.h"
//#include "aliot_auth.h"
//#include "aliot_device.h"
#include <netmgr.h>
#include <aos/cli.h>
#include <aos/cloud.h>

#ifdef AOS_ATCMD
#include <atparser.h>
#endif

#if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
/*
    #define PRODUCT_KEY             "OvNmiEYRDSY"
    #define DEVICE_NAME             "sh_online_sample_mqtt"
    #define DEVICE_SECRET           "v9mqGzepKEphLhXmAoiaUIR2HZ7XwTky"
*/
#else
/*
    #define PRODUCT_KEY             "yfTuLfBJTiL"
    #define DEVICE_NAME             "TestDeviceForDemo"
    #define DEVICE_SECRET           "fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c"
*/
    #define PRODUCT_KEY             "ZK0xhqzNJF6"
    #define DEVICE_NAME             "Rx65N_Envision_Kit"
    #define DEVICE_SECRET           "ehBIeVbimtvAnD9971QYH1GHoHDv1gds"
#endif

// These are pre-defined topics
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MSG_LEN_MAX             (2048)

/* 重复连接测试使能 */
#define TEST_CONFIG_CONNECT_REPEAT_ENABLED   (1)
/* 重复连接次数 */
#define TEST_CONFIG_CONNECT_REPEAT_TIMES     (10000)
/* 重复连接时间间隔 ms */
#define TEST_CONFIG_CONNECT_REPEAR_INTERCAL  (2000)

static int cnt = 0;
static int is_subscribed = 0;
static int sub_counter = 0;
static int pub_counter = 0;

typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    void *pclient;
} OTA_device_info_t;

OTA_device_info_t ota_device_info;

void *gpclient;

char *msg_buf = NULL, *msg_readbuf = NULL;

iotx_mqtt_topic_info_t topic_msg;
char msg_pub[128];

static void ota_init(void *pclient);
int mqtt_client_example(void);
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

static void _demo_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    iotx_mqtt_topic_info_t pub_topic;
    int ret = -1;

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
    sub_counter++;

    pub_topic.qos = IOTX_MQTT_QOS1;
    pub_topic.retain = 0;
    pub_topic.dup = 0;
    pub_topic.payload = (void*)ptopic_info->payload;
    pub_topic.payload_len = ptopic_info->payload_len;
    ret = IOT_MQTT_Publish(pclient, TOPIC_UPDATE, &pub_topic);
    if(ret < 0) {
        LOG("IOT_MQTT_Publish fail, ret=%d", ret);
    }
    else {
        pub_counter++;
    }
    LOG("RECV=%d, SEND=%d", sub_counter, pub_counter);
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
        rc = IOT_MQTT_Subscribe(pclient, TOPIC_GET, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
        if (rc<0) {
            // IOT_MQTT_Destroy(&pclient);
             LOG("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        }
        is_subscribed = 1;
        aos_schedule_call(ota_init, gpclient);
    }
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
#if (TEST_CONFIG_CONNECT_REPEAT_ENABLED == 1)
    int repeat_count = 0;
    int repeat_success_count = 0;
#endif

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

#if (TEST_CONFIG_CONNECT_REPEAT_ENABLED == 1)
    LOG("重复连接测试开始");
    while (++repeat_count < TEST_CONFIG_CONNECT_REPEAT_TIMES)
    {
        LOG("==================================================");
        LOG("尝试第 %d 次连接", repeat_count);
#endif
        /* Initialize MQTT parameter */
        memset(&mqtt_params, 0x0, sizeof(mqtt_params));

        mqtt_params.port = pconn_info->port;
        mqtt_params.host = pconn_info->host_name;
        mqtt_params.client_id = pconn_info->client_id;
        mqtt_params.username = pconn_info->username;
        mqtt_params.password = pconn_info->password;
        mqtt_params.pub_key = pconn_info->pub_key;

        mqtt_params.request_timeout_ms = 2000;
        mqtt_params.clean_session = 1;
        mqtt_params.keepalive_interval_ms = 60000;
        mqtt_params.pread_buf = msg_readbuf;
        mqtt_params.read_buf_size = MSG_LEN_MAX;
        mqtt_params.pwrite_buf = msg_buf;
        mqtt_params.write_buf_size = MSG_LEN_MAX;

        mqtt_params.handle_event.h_fp = event_handle_mqtt;
        mqtt_params.handle_event.pcontext = NULL;

#if (TEST_CONFIG_CONNECT_REPEAT_ENABLED == 1)
        gpclient = IOT_MQTT_Construct(&mqtt_params);
        if (NULL == gpclient)
        {
            LOG("第 %d 次连接失败", repeat_count);
        }
        else
        {
            repeat_success_count++;
            IOT_MQTT_Destroy(&gpclient);
            LOG("第 %d 次连接成功(%d/%d)", repeat_count, repeat_success_count, TEST_CONFIG_CONNECT_REPEAT_TIMES);
        }
        LOG("==================================================");
        aos_msleep(TEST_CONFIG_CONNECT_REPEAR_INTERCAL);
    }
    LOG("重复连接测试结束！");
    return -1;
#endif


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

    return rc;
}

static void handle_mqtt(char *pwbuf, int blen, int argc, char **argv)
{
    mqtt_client_example();
}

static struct cli_command mqttcmd = {
    .name = "mqtt",
    .help = "factory mqtt",
    .function = handle_mqtt
};

int application_start(int argc, char *argv[])
{
#ifdef AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
#endif
    aos_set_log_level(AOS_LL_DEBUG);

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_init();
    netmgr_start(false);

    aos_cli_register_command(&mqttcmd);
#ifdef CSP_LINUXHOST
    mqtt_client_example();
#endif
    aos_loop_run();
    return 0;
}

static void ota_init(void *pclient)
{
    ota_device_info.product_key=PRODUCT_KEY;
    ota_device_info.device_name=DEVICE_NAME;
    ota_device_info.pclient=pclient;
    aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, (long unsigned int)&ota_device_info);
}
