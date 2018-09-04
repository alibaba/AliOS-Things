/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
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

char g_product_key[PRODUCT_KEY_LEN + 1];
char g_product_secret[PRODUCT_SECRET_LEN + 1];
char g_device_name[DEVICE_NAME_LEN + 1];
char g_device_secret[DEVICE_SECRET_LEN + 1];

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
            HAL_Printf("undefined event occur.\n");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            HAL_Printf("MQTT disconnect.\n");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            HAL_Printf("MQTT reconnect.\n");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            HAL_Printf("subscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            HAL_Printf("subscribe wait ack timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            HAL_Printf("subscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            HAL_Printf("unsubscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            HAL_Printf("unsubscribe timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            HAL_Printf("unsubscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            HAL_Printf("publish success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            HAL_Printf("publish timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            HAL_Printf("publish nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            HAL_Printf("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s\n",
                       topic_info->topic_len,
                       topic_info->ptopic,
                       topic_info->payload_len,
                       topic_info->payload);
            break;

        default:
            HAL_Printf("Should NOT arrive here.\n");
            break;
    }
}


void mqtt_rrpc_msg_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt     ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
    iotx_mqtt_topic_info_t      topic_msg;
    char                        msg_pub[RRPC_MQTT_MSGLEN] = {0};
    char                        topic[TOPIC_LEN_MAX] = {0};
    char                        msg_id[MSG_ID_LEN_MAX] = {0};

    /* print topic name and topic message */
    HAL_Printf("----\n");
    HAL_Printf("Topic: '%.*s' (Length: %d)\n",
               ptopic_info->topic_len,
               ptopic_info->ptopic,
               ptopic_info->topic_len);
    HAL_Printf("Payload: '%.*s' (Length: %d)\n",
               ptopic_info->payload_len,
               ptopic_info->payload,
               ptopic_info->payload_len);
    HAL_Printf("----\n");

    if (snprintf(msg_id,
                 ptopic_info->topic_len - strlen(TOPIC_RRPC_REQ) + 1,
                 "%s",
                 ptopic_info->ptopic + strlen(TOPIC_RRPC_REQ))
        > sizeof(msg_id)) {
        HAL_Printf("snprintf error!\n");
        return;
    }

    HAL_Printf("response msg_id = %s\n", msg_id);
    if (snprintf(topic, sizeof(topic), "%s%s", TOPIC_RRPC_RSP, msg_id) > sizeof(topic)) {
        HAL_Printf("snprintf error!\n");
        return;
    }
    HAL_Printf("response topic = %s\n", topic);

    sprintf(msg_pub, "rrpc client has received message!\n");
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    if (IOT_MQTT_Publish(pclient, topic, &topic_msg) < 0) {
        HAL_Printf("error occur when publish!\n");
    }
}


int mqtt_rrpc_client(void)
{
    int rc = 0;
    void *pclient;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    char *msg_buf = NULL, *msg_readbuf = NULL;


    if (NULL == (msg_buf = (char *)HAL_Malloc(RRPC_MQTT_MSGLEN))) {
        HAL_Printf("not enough memory!\n");
        rc = -1;
        goto do_exit;
    }

    if (NULL == (msg_readbuf = (char *)HAL_Malloc(RRPC_MQTT_MSGLEN))) {
        HAL_Printf("not enough memory!\n");
        rc = -1;
        goto do_exit;
    }

    /**< get device info */
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&pconn_info)) {
        HAL_Printf("AUTH request failed!\n");
        rc = -1;
        goto do_exit;
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
    mqtt_params.read_buf_size = RRPC_MQTT_MSGLEN;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = RRPC_MQTT_MSGLEN;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        HAL_Printf("MQTT construct failed\n");
        rc = -1;
        goto do_exit;
    }

    /* Subscribe the specific topic */
    rc = IOT_MQTT_Subscribe(pclient, TOPIC_RRPC_REQ "+", IOTX_MQTT_QOS0, mqtt_rrpc_msg_arrive, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&pclient);
        HAL_Printf("IOT_MQTT_Subscribe failed, rc = %d\n", rc);
        rc = -1;
        goto do_exit;
    }

    HAL_SleepMs(1000);
    do {
        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);
        HAL_SleepMs(1000);
        HAL_Printf("Waiting RRPC from Cloud ...\n");

        if (running_unittest) {
            HAL_Printf("Break waiting since in unittest mode\n");
            break;
        }
    } while (1);


    IOT_MQTT_Unsubscribe(pclient, TOPIC_RRPC_REQ"+");

    HAL_SleepMs(200);

    IOT_MQTT_Destroy(&pclient);


do_exit:
    if (NULL != msg_buf) {
        HAL_Free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        HAL_Free(msg_readbuf);
    }

    return rc;
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

int linkkit_main(void *paras)
{
    int      argc = 0;
    char   **argv = NULL;

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        argc = p->argc;
        argv = p->argv;
    }

    IOT_OpenLog("mqtt");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    if (argc == 2 && !strcmp(argv[1], "unittest")) {
        HAL_Printf("***********unittest start*****************\n");
        test_mqtt_rrpc_msg_arrive();
        HAL_Printf("***********unittest end*****************\n");
        running_unittest = 1;
    }
    /**< set device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    /**< end*/
    mqtt_rrpc_client();

    HAL_Printf("out of sample!\n");

    return 0;
}
