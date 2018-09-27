/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include "at_mqtt_client.h"


#define AT_MQTT_MAX_BUFFER_NUM  10
#define AT_MQTT_MAX_MSG_LEN     1500


typedef struct at_mqtt_msg_buff_s{
    uint8_t  write_index;
    uint8_t  read_index;
    uint32_t msg_len[AT_MQTT_MAX_BUFFER_NUM];
    char     msg_buffer[AT_MQTT_MAX_BUFFER_NUM][AT_MQTT_MAX_MSG_LEN];
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
            g_at_mqtt_buff_mgr.msg_len[i] = 0;
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
            g_at_mqtt_buff_mgr.msg_len[i] = 0;
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

int at_mqtt_publish(char *topic, uint8_t qos, int *message)
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


int at_mqtt_write_buffer(char *msg, uint32_t msg_len)
{
    uint32_t        i;
    char           *data_buff;
    uint8_t         write_index;

    write_index     = g_at_mqtt_buff_mgr.write_index;
    data_buff       = g_at_mqtt_buff_mgr.msg_buffer[write_index];

    for (i = 0; i < msg_len; i++) {
        data_buff[i] = msg[i];
    }

    // TODO: mutex lock
    g_at_mqtt_buff_mgr.msg_len[write_index] = msg_len;

    write_index++;

    if (write_index >= AT_MQTT_MAX_BUFFER_NUM) {
        write_index = 0;
    }

    g_at_mqtt_buff_mgr.write_index  = write_index;

    // TODO: mutex unlock

    return msg_len;
}

int at_mqtt_read_buffer(char *buffer)
{
    uint32_t        i;
    uint32_t        msg_len;
    uint8_t         read_index;
    char           *msg_data;

    read_index = g_at_mqtt_buff_mgr.read_index;

    msg_len = g_at_mqtt_buff_mgr.msg_len[read_index];

    if (msg_len == 0) {
        return 0;
    }

    msg_data = g_at_mqtt_buff_mgr.msg_buffer[read_index];

    for (i = 0; i < msg_len; i++) {
        buffer[i] = msg_data[i];
    }

    // TODO: mutex lock
    g_at_mqtt_buff_mgr.msg_len[read_index] = 0;

    read_index++;

    if (read_index >= AT_MQTT_MAX_BUFFER_NUM) {
        read_index = 0;
    }

    g_at_mqtt_buff_mgr.read_index = read_index;

    // TODO: mutex unlock
}


