/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "aos/log.h"
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>
#include <netmgr.h>
#include <aos/cli.h>
#include <aos/cloud.h>
#include "iot_export_mqtt.h"
#include "mqtt_instance.h"

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
#if 0
    #define PRODUCT_KEY             "yfTuLfBJTiL"
    #define DEVICE_NAME             "TestDeviceForDemo"
    #define DEVICE_SECRET           "fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c"
#endif
#endif

typedef struct {
    char productKey[16];
    char deviceName[32];
    char deviceSecret[48];

    int max_msg_size;
    int max_msgq_size;
    int connected;
    int (*event_handler)(int event_type, void *ctx);
    int (*delete_subdev)(char *productKey, char *deviceName, void *ctx);
    void *ctx;
} MqttContext;

// These are pre-defined topics
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"

#define MSG_LEN_MAX             (2048)

int cnt = 0;
static int is_subscribed = 0;

#ifdef MQTT_PRESS_TEST 
static int sub_counter = 0;
static int pub_counter = 0;
#endif
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

static void mqtt_sub_callback(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
    LOG("----");
    LOG("Topic: '%s' (Length: %d)",
                  topic,
                  topic_len);
    LOG("Payload: '%s' (Length: %d)",
                  (char*)payload,
                  payload_len);
    LOG("----");

#ifdef MQTT_PRESS_TEST 
    sub_counter++;
    int rc = mqtt_publish(TOPIC_UPDATE, IOTX_MQTT_QOS1, payload, payload_len);
    if(rc < 0) {
        LOG("IOT_MQTT_Publish fail, ret=%d", rc);
    }
    else {
        pub_counter++; 
    }
    LOG("RECV=%d, SEND=%d", sub_counter, pub_counter);
#endif MQTT_PRESS_TEST 
}



/*
 * Subscribe the topic: IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, _demo_message_arrive, NULL);
 * Publish the topic: IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
 */
static void mqtt_work(void *parms) {

    int rc = -1;

    if(is_subscribed == 0) {
        /* Subscribe the specific topic */
        rc = mqtt_subscribe(TOPIC_GET, mqtt_sub_callback, NULL);
        if (rc<0) {
            // IOT_MQTT_Destroy(&pclient);
             LOG("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        }
        is_subscribed = 1;
        aos_schedule_call(ota_init, NULL);
    }
#ifndef MQTT_PRESS_TEST    
    else{
        /* Generate topic message */
        int msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\"%d\"}", cnt);
        if (msg_len < 0) {
            LOG("Error occur! Exit program");
        }
        rc = mqtt_publish(TOPIC_UPDATE, IOTX_MQTT_QOS1, msg_pub, msg_len);
        if (rc < 0) {
            LOG("error occur when publish");
        }

        LOG("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);
    }
    cnt++;
    if(cnt < 200) {
        aos_post_delayed_action(3000, mqtt_work, NULL);
    } else {
        aos_cancel_delayed_action(3000, mqtt_work, NULL);
        mqtt_unsubscribe(TOPIC_GET);
        aos_msleep(200);
        mqtt_deinit_instance();
        is_subscribed = 0;
        cnt = 0;
    }
#endif    
}



static void mqtt_service_event(input_event_t *event, void *priv_data) {

    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ) {
        return;
    }
    LOG("mqtt_service_event!");
    mqtt_work(NULL);
}

static int smartled_event_handler(int event_type, void *ctx)
{
    LOG("event_type %d\n", event_type);
    switch (event_type) {
    default:
        break;
    }

    return 0;
}

static MqttContext mqtt;

int mqtt_client_example(void)
{
     memset(&mqtt, 0, sizeof(MqttContext));

    strncpy(mqtt.productKey,   PRODUCT_KEY,   sizeof(mqtt.productKey)   - 1);
    strncpy(mqtt.deviceName,   DEVICE_NAME,   sizeof(mqtt.deviceName)   - 1);
    strncpy(mqtt.deviceSecret, DEVICE_SECRET, sizeof(mqtt.deviceSecret) - 1);

    mqtt.max_msg_size = MSG_LEN_MAX;
    mqtt.max_msgq_size = 8;

    mqtt.event_handler = smartled_event_handler;
    mqtt.delete_subdev = NULL;
    if (mqtt_init_instance(mqtt.productKey, mqtt.deviceName, mqtt.deviceSecret, mqtt.max_msg_size) < 0) {
        LOG("mqtt_init_instance failed\n");
        return -1;
    }
    aos_register_event_filter(EV_SYS,  mqtt_service_event, NULL);

    return 0;

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

#ifdef AOS_ATCMD
static void at_uart_configure(uart_dev_t *u)
{
    u->port                = AT_UART_PORT;
    u->config.baud_rate    = AT_UART_BAUDRATE;
    u->config.data_width   = AT_UART_DATA_WIDTH;
    u->config.parity       = AT_UART_PARITY;
    u->config.stop_bits    = AT_UART_STOP_BITS;
    u->config.flow_control = AT_UART_FLOW_CONTROL;
}
#endif

int application_start(int argc, char *argv[])
{
#if AOS_ATCMD
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

    aos_loop_run();
    return 0;
}

static void ota_init(void *P)
{
    aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, 0u);
}
