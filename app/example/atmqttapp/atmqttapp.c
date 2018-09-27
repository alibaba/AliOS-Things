/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "at_mqtt_ica_client.h"

#define PRODUCT_KEY     "a1kecMbd2yb"
#define DEVICE_NAME     "mqtttest"
#define DEVICE_SECRET   "aWp2N4ObAk8SZfJ81SSpfArZz16jVG45"
#define MQTT_SUB_TOPIC  "\"\/a1kecMbd2yb\/mqtttest\/get\""
#define MQTT_PUB_TOPIC  "\"\/a1kecMbd2yb\/mqtttest\/update\""
#define MQTT_PUB_MSG    "abcdefg"

static void app_delayed_action(void *arg)
{
    LOG("helloworld %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    char mqtt_topic[128];
    char mqtt_pub_msg[128];
    uint8_t mqtt_qos;
    int  mqtt_packet_id;
    int  mqtt_status;
    int  ret;

    sal_init();

    LOG("application started.");

    // init the ica client module
    ret = at_mqtt_ica_client_init();

    if (ret != 0) {
        printf("init ica client module error\r\n");
    }

    // connect to network test
    printf("connect to network\r\n");

    ret = at_mqtt_connect(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, 0);

    if (ret != 0) {
        printf("connect to network error\r\n");
    }

    // set topic info and qos
    memset(mqtt_topic, 0, 128);
    memcpy(mqtt_topic, MQTT_SUB_TOPIC, 128);
    mqtt_qos = 2;

    aos_msleep(1000);

    // subscribe test
    printf("subscribe\r\n");

    ret = at_mqtt_subscribe(mqtt_topic, mqtt_qos, &mqtt_packet_id, &mqtt_status);

    if (ret != 0) {
        printf("subscribe error\r\n");
    }

    aos_msleep(1000);
#if 0
    // unsubscribe test
    printf("unsubscribe\r\n");

    ret = at_mqtt_unsubscribe(mqtt_topic, &mqtt_packet_id, &mqtt_status);

    if (ret != 0) {
        printf("unsubscribe error\r\n");
    }
#endif
    memset(mqtt_topic, 0, 128);
    memset(mqtt_pub_msg, 0, 128);
    memcpy(mqtt_topic, MQTT_PUB_TOPIC, 128);
    memcpy(mqtt_pub_msg, MQTT_PUB_MSG, 128);
    mqtt_qos = 1;

    // publish test
    printf("publish\r\n");

    ret = at_mqtt_publish(mqtt_topic, mqtt_qos, mqtt_pub_msg);

    if (ret != 0) {
        printf("publish error\r\n");
    }

    // test finished
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

