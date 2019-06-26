/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers.h"
#include "infra_compat.h"
#include "app_entry.h"

#define PRODUCT_KEY             "W9LchU2zAAK"
#define DEVICE_NAME             "subdevice_2"
#define DEVICE_SECRET           "Y8QN9QFGvbCVpJ23F2ZFuwhR4785NO5C"


/* These are the pre-defined topics */
#define TOPIC_RRPC_REQ       "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/rrpc/request/"
#define TOPIC_RRPC_RSP       "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/rrpc/response/"

#define TEST_TOPIC            "/sys/lLeATwv18gi/test1/rrpc/request/890192612580343808"
#define TEST_PAYLOAD          "hello world"
#define TEST_TOPIC_PAYLOAD    "/sys/lLeATwv18gi/test1/rrpc/request/890192612580343808hello world"


#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

#define RRPC_MQTT_MSGLEN    (1024)
#define MSG_ID_LEN_MAX      (64)
#define TOPIC_LEN_MAX       (1024)

static int running_unittest = 0;

void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.\n");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.\n");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.\n");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s\n",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.\n");
            break;
    }
}


void mqtt_rrpc_msg_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt     ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    uintptr_t packet_id = (uintptr_t)msg->msg;
    switch (msg->event_type) {


        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED: {
            iotx_mqtt_topic_info_t      topic_msg;
            char                        msg_pub[RRPC_MQTT_MSGLEN] = {0};
            char                        topic[TOPIC_LEN_MAX] = {0};
            char                        msg_id[MSG_ID_LEN_MAX] = {0};

            /* print topic name and topic message */
            EXAMPLE_TRACE("----\n");
            EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)\n",
                          ptopic_info->topic_len,
                          ptopic_info->ptopic,
                          ptopic_info->topic_len);
            EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)\n",
                          ptopic_info->payload_len,
                          ptopic_info->payload,
                          ptopic_info->payload_len);
            EXAMPLE_TRACE("----\n");

            if (snprintf(msg_id,
                         ptopic_info->topic_len - strlen(TOPIC_RRPC_REQ) + 1,
                         "%s",
                         ptopic_info->ptopic + strlen(TOPIC_RRPC_REQ))
                > sizeof(msg_id)) {
                EXAMPLE_TRACE("snprintf error!\n");
                return;
            }

            EXAMPLE_TRACE("response msg_id = %s\n", msg_id);
            if (snprintf(topic, sizeof(topic), "%s%s", TOPIC_RRPC_RSP, msg_id) > sizeof(topic)) {
                EXAMPLE_TRACE("snprintf error!\n");
                return;
            }
            EXAMPLE_TRACE("response topic = %s\n", topic);

            sprintf(msg_pub, "rrpc client has received message!\n");
            topic_msg.qos = IOTX_MQTT_QOS0;
            topic_msg.retain = 0;
            topic_msg.dup = 0;
            topic_msg.payload = (void *)msg_pub;
            topic_msg.payload_len = strlen(msg_pub);

            if (IOT_MQTT_Publish(pclient, topic, &topic_msg) < 0) {
                EXAMPLE_TRACE("error occur when publish!\n");
            }
        }
        break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            EXAMPLE_TRACE("buffer overflow, %s", msg->msg);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }



}


int linkkit_main(void *paras)
{
    int      argc = 0;
    char   **argv = NULL;

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        argc = p->argc;
        argv = p->argv;
    }

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    if (argc == 2 && !strcmp(argv[1], "unittest")) {
        EXAMPLE_TRACE("***********unittest start*****************\n");
        test_mqtt_rrpc_msg_arrive();
        EXAMPLE_TRACE("***********unittest end*****************\n");
        running_unittest = 1;
    }
    /**< set device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    int rc = 0;
    void *pclient;
    iotx_mqtt_param_t mqtt_params;

    /* Initialize MQTT parameter */
    /*
     * Note:
     *
     * If you did NOT set value for members of mqtt_params, SDK will use their default values
     * If you wish to customize some parameter, just un-comment value assigning expressions below
     *
     **/
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    /**
     *
     *  MQTT connect hostname string
     *
     *  MQTT server's hostname can be customized here
     *
     *  default value is ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com
     */
    /* mqtt_params.host = "something.iot-as-mqtt.cn-shanghai.aliyuncs.com"; */

    /**
     *
     *  MQTT connect port number
     *
     *  TCP/TLS port which can be 443 or 1883 or 80 or etc, you can customize it here
     *
     *  default value is 1883 in TCP case, and 443 in TLS case
     */
    /* mqtt_params.port = 1883; */

    /**
     *
     * MQTT request timeout interval
     *
     * MQTT message request timeout for waiting ACK in MQTT Protocol
     *
     * default value is 2000ms.
     */
    /* mqtt_params.request_timeout_ms = 2000; */

    /**
     *
     * MQTT clean session flag
     *
     * If CleanSession is set to 0, the Server MUST resume communications with the Client based on state from
     * the current Session (as identified by the Client identifier).
     *
     * If CleanSession is set to 1, the Client and Server MUST discard any previous Session and Start a new one.
     *
     * default value is 0.
     */
    /* mqtt_params.clean_session = 0; */

    /**
     *
     * MQTT keepAlive interval
     *
     * KeepAlive is the maximum time interval that is permitted to elapse between the point at which
     * the Client finishes transmitting one Control Packet and the point it starts sending the next.
     *
     * default value is 60000.
     */
    /* mqtt_params.keepalive_interval_ms = 60000; */

    /**
     *
     * MQTT write buffer size
     *
     * Write buffer is allocated to place upstream MQTT messages, MQTT client will be limitted
     * to send packet no longer than this to Cloud
     *
     * default value is 1024.
     *
     */
    /* mqtt_params.write_buf_size = 1024; */

    /**
     *
     * MQTT read buffer size
     *
     * Write buffer is allocated to place downstream MQTT messages, MQTT client will be limitted
     * to recv packet no longer than this from Cloud
     *
     * default value is 1024.
     *
     */
    /* mqtt_params.read_buf_size = 1024; */

    /**
     *
     * MQTT event callback function
     *
     * Event callback function will be called by SDK when it want to notify user what is happening inside itself
     *
     * default value is NULL, which means PUB/SUB event won't be exposed.
     *
     */
    mqtt_params.handle_event.h_fp = event_handle;
    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed\n");
        return -1;
    }

    /* Subscribe the specific topic */
    rc = IOT_MQTT_Subscribe(pclient, TOPIC_RRPC_REQ "+", IOTX_MQTT_QOS0, mqtt_rrpc_msg_arrive, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("IOT_MQTT_Subscribe failed, rc = %d\n", rc);
        return -1;
    }

    HAL_SleepMs(1000);
    do {
        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);
        HAL_SleepMs(1000);
        EXAMPLE_TRACE("Waiting RRPC from Cloud ...\n");

        if (running_unittest) {
            EXAMPLE_TRACE("Break waiting since in unittest mode\n");
            break;
        }
    } while (1);


    IOT_MQTT_Unsubscribe(pclient, TOPIC_RRPC_REQ"+");

    HAL_SleepMs(200);

    IOT_MQTT_Destroy(&pclient);

    return 0;
}


void test_mqtt_rrpc_msg_arrive(void)
{
    iotx_mqtt_topic_info_t topic_info;
    iotx_mqtt_event_msg_t msg;

    topic_info.packet_id = 0;
    topic_info.qos = 0;
    topic_info.dup  = 0;
    topic_info.retain = 0;
    topic_info.topic_len = strlen(TEST_TOPIC);
    topic_info.payload_len = strlen(TEST_PAYLOAD);
    topic_info.ptopic = TEST_TOPIC_PAYLOAD;
    topic_info.payload = TEST_TOPIC_PAYLOAD + strlen(TEST_TOPIC);

    msg.event_type = 0;
    msg.msg = &topic_info;
    mqtt_rrpc_msg_arrive(NULL, NULL, &msg);
}

