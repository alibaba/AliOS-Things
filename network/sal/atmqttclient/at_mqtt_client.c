/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include "at_mqtt_client.h"
#include "at_mqtt_ica_client.h"
#include "atparser.h"


#define AT_MQTT_MAX_BUFFER_NUM  10
#define AT_MQTT_MAX_MSG_LEN     512
#define AT_MQTT_MAX_TOPIC_LEN   128


typedef struct at_mqtt_msg_buff_s{
    uint8_t  write_index;
    uint8_t  read_index;
    uint8_t  valid_flag[AT_MQTT_MAX_BUFFER_NUM];
    char     topic[AT_MQTT_MAX_BUFFER_NUM][AT_MQTT_MAX_TOPIC_LEN];
    char     msg_data[AT_MQTT_MAX_BUFFER_NUM][AT_MQTT_MAX_MSG_LEN];
    aos_mutex_t buffer_mutex;
} at_mqtt_msg_buff_t;


static at_mqtt_client_op_t  *g_at_mqtt_ops = NULL;
static at_mqtt_msg_buff_t    g_at_mqtt_buff_mgr;

int at_mqtt_register(at_mqtt_client_op_t *client_ops)
{
    uint32_t        i;

    if (client_ops != NULL) {

        g_at_mqtt_ops = client_ops;

        g_at_mqtt_buff_mgr.read_index  = 0;
        g_at_mqtt_buff_mgr.write_index = 0;

        for (i = 0; i < AT_MQTT_MAX_BUFFER_NUM; i++) {
            g_at_mqtt_buff_mgr.valid_flag[i] = 0;
            memset(g_at_mqtt_buff_mgr.topic[i], 0, AT_MQTT_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, AT_MQTT_MAX_MSG_LEN);
        }

        if (0 != aos_mutex_new(&g_at_mqtt_buff_mgr.buffer_mutex)) {

            printf("create buffer mutex error\r\n");

            return -1;
        }

        return 0;
    }

    return -1;
}

int at_mqtt_unregister(void)
{
    uint32_t        i;

    if (g_at_mqtt_ops != NULL) {

        g_at_mqtt_ops = NULL;

        g_at_mqtt_buff_mgr.read_index  = 0;
        g_at_mqtt_buff_mgr.write_index = 0;

        for (i = 0; i < AT_MQTT_MAX_BUFFER_NUM; i++) {
            g_at_mqtt_buff_mgr.valid_flag[i] = 0;
            memset(g_at_mqtt_buff_mgr.topic[i], 0, AT_MQTT_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, AT_MQTT_MAX_MSG_LEN);
        }

        if (aos_mutex_is_valid(&g_at_mqtt_buff_mgr.buffer_mutex)) {

            aos_mutex_free(&g_at_mqtt_buff_mgr.buffer_mutex);
        }

        return 0;
    }

    return -1;
}

int at_mqtt_connect(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->conn) {
            g_at_mqtt_ops->conn(proKey, devName, devSecret, tlsEnable);

            return 0;
        }
    }

    return -1;
}

int at_mqtt_disconnect(void)
{
    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->disconn) {
            g_at_mqtt_ops->disconn();

            return 0;
        }
    }

    return -1;
}

int at_mqtt_subscribe(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status)
{
    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->subscribe) {
            g_at_mqtt_ops->subscribe(topic, qos, mqtt_packet_id, mqtt_status);

            return 0;
        }
    }

    return -1;
}

int at_mqtt_unsubscribe(char *topic, int *mqtt_packet_id, int *mqtt_status)
{
    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->unsubscribe) {
            g_at_mqtt_ops->unsubscribe(topic, mqtt_packet_id, mqtt_status);

            return 0;
        }
    }

    return -1;
}

int at_mqtt_publish(char *topic, uint8_t qos, char *message)
{
    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->publish) {
            g_at_mqtt_ops->publish(topic, qos, message);

            return 0;
        }
    }

    return -1;
}


int at_mqtt_state(void)
{
    int mqtt_state = 0;

    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->state) {
            mqtt_state = g_at_mqtt_ops->state();
        }
    }

    return mqtt_state;
}

int at_mqtt_settings(void)
{
    int ret = 0;

    if (g_at_mqtt_ops != NULL) {
        if (g_at_mqtt_ops->settings) {
            ret = g_at_mqtt_ops->settings();

            return ret;
        }
    }

    return -1;
}


int at_mqtt_save_msg(char *topic, char *message)
{
    uint8_t       write_index;
    char         *copy_ptr;

    if ((topic == NULL)||(message == NULL)) {

        printf("write buffer is NULL\r\n");

        return -1;
    }

    if ((strlen(topic) >= AT_MQTT_MAX_TOPIC_LEN)||
        (strlen(message) >= AT_MQTT_MAX_MSG_LEN)) {

        printf("topic or message too large\r\n");

        return -1;
    }

    aos_mutex_lock(&g_at_mqtt_buff_mgr.buffer_mutex, AOS_WAIT_FOREVER);

    write_index     = g_at_mqtt_buff_mgr.write_index;

    if (g_at_mqtt_buff_mgr.valid_flag[write_index]) {

        printf("buffer is full\r\n");

        aos_mutex_unlock(&g_at_mqtt_buff_mgr.buffer_mutex);

        return -1;
    }

    copy_ptr = g_at_mqtt_buff_mgr.topic[write_index];

    while (*topic) {
        *copy_ptr++ = *topic++;
    }

    copy_ptr = g_at_mqtt_buff_mgr.msg_data[write_index];

    while (*message) {
        *copy_ptr++ = *message++;
    }

    g_at_mqtt_buff_mgr.valid_flag[write_index] = 1;

    write_index++;

    if (write_index >= AT_MQTT_MAX_BUFFER_NUM) {
        write_index = 0;
    }

    g_at_mqtt_buff_mgr.write_index  = write_index;

    aos_mutex_unlock(&g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}

int at_mqtt_read_msg(char *topic, char *message)
{
    uint8_t         read_index;
    char           *copy_ptr;

    if ((topic == NULL)||(message == NULL)) {
        printf("read buffer is NULL\r\n");

        return -1;
    }

    aos_mutex_lock(&g_at_mqtt_buff_mgr.buffer_mutex, AOS_WAIT_FOREVER);

    read_index = g_at_mqtt_buff_mgr.read_index;

    if (g_at_mqtt_buff_mgr.valid_flag[read_index] == 0) {

        aos_mutex_unlock(&g_at_mqtt_buff_mgr.buffer_mutex);

        return -1;
    }

    copy_ptr = g_at_mqtt_buff_mgr.topic[read_index];

    while (*copy_ptr) {
        *topic++ = *copy_ptr++;
    }

    copy_ptr = g_at_mqtt_buff_mgr.msg_data[read_index];

    while (*copy_ptr) {
        *message++ = *copy_ptr++;
    }

    memset(g_at_mqtt_buff_mgr.topic[read_index], 0, AT_MQTT_MAX_TOPIC_LEN);
    memset(g_at_mqtt_buff_mgr.msg_data[read_index], 0, AT_MQTT_MAX_MSG_LEN);

    g_at_mqtt_buff_mgr.valid_flag[read_index] = 0;

    read_index++;

    if (read_index >= AT_MQTT_MAX_BUFFER_NUM) {
        read_index = 0;
    }

    g_at_mqtt_buff_mgr.read_index = read_index;

    aos_mutex_unlock(&g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}
#if 0
volatile uint8_t          g_mqtt_gotip = 0;

static void at_mqtt_gotip_callback(void *arg, char *rspinfo, int rsplen)
{
    printf("got ip callback\r\n");
    g_mqtt_gotip = 1;
}
#endif
int at_mqtt_connect_wifi(char *at_conn_wifi)
{
    char  at_cmd[64];
    uint8_t wifi_timeout = 0;

    if (g_at_mqtt_ops != NULL) {
        // disconnect before connect to the network
        if (g_at_mqtt_ops->disconn) {
            if (0 != g_at_mqtt_ops->disconn()) {
                printf("disconnect error\r\n");

                return -1;
            }
        }

        if (g_at_mqtt_ops->sendat) {
            memcpy(at_cmd, at_conn_wifi, 64);
#if 0
            at.oob(get_ip_keywords,
                   AT_MQTT_ICA_POSTFIX,
                   512,
                   at_mqtt_gotip_callback,
                   NULL);
#endif
            // connect to the network
            if (0 != g_at_mqtt_ops->sendat(at_cmd)) {
                printf("connect wifi at fail\r\n");

                return -1;
            }

            aos_msleep(4000);

#if 0
            while (g_mqtt_gotip != 1) {
                wifi_timeout++;

                // 5 second timeout return
                if (wifi_timeout >= 40) {
                    printf("get ip address timeout\r\n");

                    return -1;
                }

                // must be 1000ms, or some error may appear, need to resolve later
                aos_msleep(2000);
                printf("delay\r\n");
            }
#endif
        }

        return 0;
    }

    return -1;
}


