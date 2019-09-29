/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "linkkit/dev_sign_api.h"
#include "linkkit/mqtt_api.h"
#include "linkkit/wrappers/wrappers.h"
#include "linkkit/infra/infra_compat.h"
#include "app_entry.h"

#include "das.h"

#if defined(ON_DAILY)
#define PRODUCT_KEY             "a1JGT7wyRyA"
#define PRODUCT_SECRET          "h4I4dneEFp7EImTv"
#define DEVICE_NAME             "DAS_121001_test"
#define DEVICE_SECRET           "M8TeXygMVtq49XVdC2BB9qGXOaC6DVHC"
#else
#define PRODUCT_KEY             "a1Tvl7rVOIe"
#define PRODUCT_SECRET          "h4I4dneEFp7EImTv"
#define DEVICE_NAME             "sf74UYEFGqRSmeX9aGgk"
#define DEVICE_SECRET           "S4rHc5zoQxw3cxeh6dJ1PqRtni4ZhLap"
#endif

/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA               "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MQTT_MSGLEN             (1024)

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static int      user_argc;
static char   **user_argv;

static void       *session = NULL;
static const char *sub_topic = NULL;

void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            EXAMPLE_TRACE("buffer overflow, %s", (char *)msg->msg);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

static int on_publish(const char *topic, const uint8_t *msg, size_t size, void *channel)
{
    int ret = 0;
    int retry = 0;
    iotx_mqtt_topic_info_t topic_msg;

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg;
    topic_msg.payload_len = size;

    do {
        ret = IOT_MQTT_Publish(channel, topic, &topic_msg);
        if (ret < 0) {
            if (ret == MQTT_STATE_ERROR || ret == MQTT_NETWORK_ERROR) {
                HAL_SleepMs(200);
            } else {
                EXAMPLE_TRACE("error occur when publish, %d", ret);
                break;
            }
        }
    } while(ret < 0 && retry++ < 10);

    EXAMPLE_TRACE("\n publish message: \n topic: %s\n payload len: \%d\n ret = %d\n",
                  topic, topic_msg.payload_len, ret);

    if (ret < 0) {
        ret = DAS_ERROR_GENERIC;
    } else {
        ret = DAS_SUCCESS;
    }

    return ret;
}

static void on_message(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt     ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("----");
            EXAMPLE_TRACE("PacketId: %d", ptopic_info->packet_id);
            EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)",
                          ptopic_info->topic_len,
                          ptopic_info->ptopic,
                          ptopic_info->topic_len);
            EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",
                          ptopic_info->payload_len,
                          ptopic_info->payload,
                          ptopic_info->payload_len);
            EXAMPLE_TRACE("----");
            break;
        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }

    if (!memcmp(sub_topic, ptopic_info->ptopic, ptopic_info->topic_len)) {
        das_on_message(pcontext,
                       (uint8_t *)ptopic_info->payload,
                       (size_t)ptopic_info->payload_len);
    }
}

int mqtt_client(void)
{
    int rc, cnt = 0;
    void *pclient;
    iotx_mqtt_param_t mqtt_params;
    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.handle_event.h_fp = event_handle;

    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    session = das_init(PRODUCT_KEY, DEVICE_NAME);
    if (session == NULL) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("das_init() failed");
        return -1;
    }

    das_connection(session, on_publish, pclient);
    das_on_connected(session);

    sub_topic = das_sub_topic(session, NULL);
    if (sub_topic == NULL) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("das_get_sub_topic() failed");
        rc = -1;
        goto do_exit;
    }

    rc = IOT_MQTT_Subscribe(pclient,
              sub_topic, IOTX_MQTT_QOS1, on_message, session);
    if (rc < 0) {
        EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, %d\n", rc);
        goto do_exit;
    }

    do {
        uint64_t time_ms = HAL_UptimeMs();

        rc = das_stepping(session, time_ms);
        if (rc != DAS_SUCCESS) {
            EXAMPLE_TRACE("das_stepping() failed, %d", rc);
            goto do_exit;
        }

        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);

        HAL_SleepMs(2000);

        /* infinite loop if running with 'loop' argument */
        if (user_argc >= 2 && !strcmp("loop", user_argv[1])) {
            HAL_SleepMs(2000);
            cnt = 0;
        }
    } while (cnt < 1);

    IOT_MQTT_Yield(pclient, 200);

do_exit:
    IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);

    IOT_MQTT_Yield(pclient, 200);

    IOT_MQTT_Destroy(&pclient);

    das_on_disconnected(session);

    das_final(session);

    return 0;
}

int linkkit_main(void *paras)
{
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    user_argc = 0;
    user_argv = NULL;

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        user_argc = p->argc;
        user_argv = p->argv;
    }

    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    HAL_SetProductSecret(PRODUCT_SECRET);
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    mqtt_client();
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    EXAMPLE_TRACE("out of sample!");

    return 0;
}
