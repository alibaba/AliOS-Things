/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "infra_compat.h"
#include "infra_mem_stats.h"
#include "mqtt_api.h"
#include "wrappers.h"

#include "mqtt_instance.h"

#define DPRINT(...)                                         \
    do {                                                    \
        printf("\033[1;31;40m%s.%d: ", __func__, __LINE__); \
        printf(__VA_ARGS__);                                \
        printf("\033[0m");                                  \
    } while (0)

static void *mqtt_client = NULL;

static void *mqtt_rbuf = NULL;
static void *mqtt_wbuf = NULL;

static int abort_request = 0;

typedef struct mqtt_instance_event_s {
    void (*event_cb)(int event, void *ctx);
    void *ctx;

    struct mqtt_instance_event_s *next;
} mqtt_instance_event_t;

static mqtt_instance_event_t *first_event = NULL;

static void event_list_add(mqtt_instance_event_t *ev)
{
    ev->next    = first_event;
    first_event = ev;
}

static void event_list_remove(mqtt_instance_event_t *ev)
{
    mqtt_instance_event_t **ep, *e1;

    /* remove connection from list */
    ep = &first_event;
    while ((*ep) != NULL) {
        e1 = *ep;
        if (e1 == ev)
            *ep = ev->next;
        else
            ep = &e1->next;
    }
}

static void event_handle(void *pcontext, void *pclient,
                         iotx_mqtt_event_msg_pt msg)
{
    int event = 0;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_RECONNECT:
            event = MQTT_INSTANCE_EVENT_CONNECTED;
            break;
        case IOTX_MQTT_EVENT_DISCONNECT:
            event = MQTT_INSTANCE_EVENT_DISCONNECTED;
            break;
        default:
            return;
    }

    mqtt_instance_event_t *ev;
    for (ev = first_event; ev; ev = ev->next) {
        if (ev->event_cb) ev->event_cb(event, ev->ctx);
    }
}

void *mqtt_get_instance()
{
    return mqtt_client;
}

void mqtt_remove_instance()
{
    mqtt_client = NULL;
}

int mqtt_set_instance(void *mqtt_t)
{
    if (mqtt_client || mqtt_t == NULL) return FAIL_RETURN;

    mqtt_client = mqtt_t;
    return SUCCESS_RETURN;
}

/*
 * 1:  mqtt instance have been init
 * 0:  mqtt instance init success
 *     IOT_MQTT_Construct success,  MQTT connected.
 * -1: mqtt instance init fail
 */
int mqtt_init_instance(char *productKey, char *deviceName, char *deviceSecret,
                       int maxMsgSize)
{
    iotx_mqtt_param_t mqtt_params;

    if (mqtt_client) return 1;

    /* IOT_SetLogLevel(IOT_LOG_DEBUG); */
    IOT_SetLogLevel(IOT_LOG_WARNING);
    HAL_SetProductKey(productKey);
    HAL_SetDeviceName(deviceName);
    HAL_SetDeviceSecret(deviceSecret);

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.handle_event.h_fp = event_handle;

    /* Construct a MQTT client with specify parameter */
    mqtt_client = IOT_MQTT_Construct(&mqtt_params);
    if (!mqtt_client) goto fail;

    return 0;

fail:
    mqtt_deinit_instance();
    return -1;
}

int mqtt_deinit_instance(void)
{
    abort_request = 1;

    if (mqtt_rbuf) {
        aos_free(mqtt_rbuf);
        mqtt_rbuf = NULL;
    }

    if (mqtt_wbuf) {
        aos_free(mqtt_wbuf);
        mqtt_wbuf = NULL;
    }

    if (mqtt_client) {
        IOT_MQTT_Destroy(&mqtt_client);
        mqtt_client = NULL;
    }

    mqtt_instance_event_t *ev, *ev_next;
    for (ev = first_event; ev; ev = ev_next) {
        ev_next = ev->next;
        event_list_remove(ev);
        aos_free(ev);
    }

    first_event = NULL;

    abort_request = 0;

    return 0;
}

int mqtt_set_event_cb(void (*event_cb)(int event, void *ctx), void *ctx)
{
    if (!event_cb) return -1;

    if (!mqtt_client) return -1;

    mqtt_instance_event_t *ev = LITE_malloc(sizeof(mqtt_instance_event_t));
    if (!ev) return -1;
    memset(ev, 0, sizeof(mqtt_instance_event_t));

    ev->event_cb = event_cb;
    ev->ctx      = ctx;

    event_list_add(ev);

    return 0;
}

typedef struct mqtt_instance_topic_s {
    char *topic;

    void (*cb)(char *topic, int topic_len, void *payload, int payload_len,
               void *ctx);
    void *ctx;

    struct mqtt_instance_topic_s *next;
} mqtt_instance_topic_t;

static mqtt_instance_topic_t *first_topic = NULL;

static void topic_list_add(mqtt_instance_topic_t *t)
{
    t->next     = first_topic;
    first_topic = t;
}

static void topic_list_remove(mqtt_instance_topic_t *t)
{
    mqtt_instance_topic_t **tp, *t1;

    /* remove connection from list */
    tp = &first_topic;
    while ((*tp) != NULL) {
        t1 = *tp;
        if (t1 == t)
            *tp = t->next;
        else
            tp = &t1->next;
    }
}

static void subscriber_cb(void *ctx, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    mqtt_instance_topic_t *t            = ctx;
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    if (t && t->cb) {
        t->cb((char *)ptopic_info->ptopic, ptopic_info->topic_len,
              (void *)ptopic_info->payload, ptopic_info->payload_len, t->ctx);
    }
}

int mqtt_subscribe(char *topic,
                   void (*cb)(char *topic, int topic_len, void *payload,
                              int len, void *ctx),
                   void *ctx)
{
    if (!topic || !cb) return -1;

    if (!mqtt_client) return -1;

    mqtt_instance_topic_t *t = aos_malloc(sizeof(mqtt_instance_topic_t));
    if (!t) return -1;
    memset(t, 0, sizeof(mqtt_instance_topic_t));

    t->topic = aos_malloc(strlen(topic) + 1);
    if (!t->topic) {
        aos_free(t);
        return -1;
    }
    strcpy(t->topic, topic);

    t->cb  = cb;
    t->ctx = ctx;

    int ret = IOT_MQTT_Subscribe(mqtt_client, t->topic, IOTX_MQTT_QOS1,
                                 subscriber_cb, t);
    if (ret < 0) {
        aos_free(t->topic);
        aos_free(t);
        return -1;
    }

    topic_list_add(t);

    return 0;
}

int mqtt_unsubscribe(char *topic)
{
    mqtt_instance_topic_t *t, *t_next;

    if (!mqtt_client) return -1;

    for (t = first_topic; t; t = t_next) {
        t_next = t->next;

        if (strcmp(t->topic, topic) == 0) {
            IOT_MQTT_Unsubscribe(mqtt_client, topic);
            topic_list_remove(t);
            aos_free(t->topic);
            aos_free(t);
            t = NULL;
        }
    }

    return 0;
}

int mqtt_publish(char *topic, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;

    if (!mqtt_client) return -1;
    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    if (IOT_MQTT_Publish(mqtt_client, topic, &mqtt_msg) < 0) {
        DPRINT("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return 0;
}

static char mqtt_domain[64] = {0};
static int mqtt_port        = 1883;
void mqtt_set_domain(char *domain, int port)
{
    DPRINT("IOT MQTT set url=%s:%d \n", domain, port);
    if (domain) {
        strncpy(mqtt_domain, domain, 63);
        mqtt_port = port;
    } else {
        mqtt_domain[0] = 0;
    }
}

const char *mqtt_get_domain(void)
{
    if (mqtt_domain[0]) return mqtt_domain;
    return NULL;
}

int mqtt_get_port(void)
{
    return mqtt_port;
}