/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mdal_at_mqtt_client.h"
#include "mdal_at_ica_mqtt_client.h"
#include "iotx_log.h"

#define AT_MQTT_MAX_BUFFER_NUM  10
#define AT_MQTT_MAX_MSG_LEN     512
#define AT_MQTT_MAX_TOPIC_LEN   128
#define AT_MQTT_WAIT_FOREVER 0xffffffffu

#define mdal_err(...)               log_err("MAL", __VA_ARGS__)

typedef struct at_mqtt_msg_buff_s{
    uint8_t  write_index;
    uint8_t  read_index;
    uint8_t  valid_flag[AT_MQTT_MAX_BUFFER_NUM];
    char     topic[AT_MQTT_MAX_BUFFER_NUM][AT_MQTT_MAX_TOPIC_LEN];
    char     msg_data[AT_MQTT_MAX_BUFFER_NUM][AT_MQTT_MAX_MSG_LEN];
    void*    buffer_mutex;
} at_mqtt_msg_buff_t;
static at_mqtt_msg_buff_t    g_at_mqtt_buff_mgr;

int HAL_AT_MQTT_Init()
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;

    for (int i = 0; i < AT_MQTT_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        memset(g_at_mqtt_buff_mgr.topic[i], 0, AT_MQTT_MAX_TOPIC_LEN);
        memset(g_at_mqtt_buff_mgr.msg_data[i], 0, AT_MQTT_MAX_MSG_LEN);
    }

    if (NULL == (g_at_mqtt_buff_mgr.buffer_mutex = HAL_MutexCreate())) {
        mdal_err("create buffer mutex error");
        return -1;
    }

    return at_ica_mqtt_client_init();
#endif
    return -1;
}


int HAL_AT_MQTT_Deinit()
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;

    for (int i = 0; i < AT_MQTT_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        memset(g_at_mqtt_buff_mgr.topic[i], 0, AT_MQTT_MAX_TOPIC_LEN);
        memset(g_at_mqtt_buff_mgr.msg_data[i], 0, AT_MQTT_MAX_MSG_LEN);
    }

    HAL_MutexDestroy(g_at_mqtt_buff_mgr.buffer_mutex);
    return at_ica_mqtt_client_deinit();
#endif
    return -1;
}

int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_conn(proKey, devName, devSecret, tlsEnable);
#endif
    return -1;
}

int HAL_AT_MQTT_Disconnect(void)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_disconn();
#endif
    return -1;
}

int HAL_AT_MQTT_Subscribe(char *topic, uint8_t qos, int *mqtt_packet_id, int *mqtt_status, int timeout_ms)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_subscribe(topic, qos, mqtt_packet_id, mqtt_status, timeout_ms);
#endif
    return -1;
}

int HAL_AT_MQTT_Unsubscribe(char *topic, int *mqtt_packet_id, int *mqtt_status)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_unsubscribe(topic, mqtt_packet_id, mqtt_status);
#endif
    return -1;
}

int HAL_AT_MQTT_Publish(char *topic, uint8_t qos, char *message)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_publish(topic, qos, message);
#endif
    return -1;
}


int HAL_AT_MQTT_State(void)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    return at_ica_mqtt_client_state();
#endif
    return -1;
}

int HAL_AT_MQTT_Savemsg(char *topic, char *message)
{
    uint8_t       write_index;
    char         *copy_ptr;

    if ((topic == NULL)||(message == NULL)) {

        mdal_err("write buffer is NULL");

        return -1;
    }

    if ((strlen(topic) >= AT_MQTT_MAX_TOPIC_LEN)||
        (strlen(message) >= AT_MQTT_MAX_MSG_LEN)) {

        mdal_err("topic or message too large");

        return -1;
    }

    HAL_MutexLock(g_at_mqtt_buff_mgr.buffer_mutex);

    write_index     = g_at_mqtt_buff_mgr.write_index;

    if (g_at_mqtt_buff_mgr.valid_flag[write_index]) {

        mdal_err("buffer is full");

        HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

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

    HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}

int HAL_AT_MQTT_Readmsg(char *topic, char *message)
{
    uint8_t         read_index;
    char           *copy_ptr;

    if ((topic == NULL)||(message == NULL)) {
        mdal_err("read buffer is NULL");

        return -1;
    }

    HAL_MutexLock(&g_at_mqtt_buff_mgr.buffer_mutex);

    read_index = g_at_mqtt_buff_mgr.read_index;

    if (g_at_mqtt_buff_mgr.valid_flag[read_index] == 0) {

        HAL_MutexUnlock(&g_at_mqtt_buff_mgr.buffer_mutex);

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

    HAL_MutexUnlock(&g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}

int HAL_AT_MQTT_Connectwifi(char *at_conn_wifi)
{
#ifdef AT_ICA_MQTT_CLIENT_SUPPORT
    char  at_cmd[64];
    int   ret;
    // disconnect before connect to the network
    if(at_ica_mqtt_client_disconn() != 0)
    {
        return -1;
    }

    memcpy(at_cmd, at_conn_wifi, 64);
    // connect to the network
    if(at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_FOREVER) != 0)
    {
        return -1;
    }

    return 0;
#endif
    return -1;
}


