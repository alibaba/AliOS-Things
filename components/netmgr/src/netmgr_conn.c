/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "netmgr_wifi.h"
#include "netmgr_conn.h"
#include "ulog/ulog.h"

#define TAG "NETMGR_CONN"
#define NETMGR_CONN_LOGE(level, ...)      LOGE(TAG, level, ##__VA_ARGS__)
#define NETMGR_CONN_LOGI(level, ...)      LOGI(TAG, level, ##__VA_ARGS__)
#define NETMGR_CONN_LOGD(level, ...)      LOGD(TAG, level, ##__VA_ARGS__)

static netmgr_conn_t * g_netmgr_conn_list = NULL;

netmgr_conn_t* netmgr_conn_init(netmgr_type_t type)
{
    netmgr_conn_t* conn = (netmgr_conn_t *)malloc(sizeof(netmgr_conn_t));
    if(conn == NULL) {
        return NULL;
    }
    memset(conn, 0, sizeof(netmgr_conn_t));

    conn->next = NULL;
    conn->state = CONN_STATE_DISCONNECTED;
    conn->type = type;

    if(g_netmgr_conn_list == NULL) {
        g_netmgr_conn_list = conn;
    } else {
        conn->next = g_netmgr_conn_list;
        g_netmgr_conn_list = conn;
    }

    return conn;
}

int netmgr_conn_deinit(netmgr_conn_t* conn)
{
    netmgr_conn_t* current = g_netmgr_conn_list;
    while(current != NULL) {
        if(current == conn) {
            break;
        } else {
            current = current->next;
        }
    }

    if(current != NULL) {
        free(conn);
    }
    else {
        return -1;
    }

    return 0;
}

int conn_disconnecting(netmgr_conn_t* conn)
{
    if((conn->state != CONN_STATE_DISCONNECTED)
        &&(conn->state != CONN_STATE_DISCONNECTING)) {
        NETMGR_CONN_LOGI("%s:%d old state=%d\n", __func__, __LINE__, conn->state);
        conn->state = CONN_STATE_DISCONNECTING;
        return conn->disconnecting_cb(conn);
    }
    return 0;
}

int conn_disconnected(netmgr_conn_t* conn)
{
    NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
    conn->state = CONN_STATE_DISCONNECTED;
    return conn->disconnected_cb(conn);
}

int conn_connecting(netmgr_conn_t* conn)
{
    NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
    if((conn->state != CONN_STATE_DISCONNECTED)
       &&(conn->state != CONN_STATE_CONNECTING)) {
        NETMGR_CONN_LOGE("%s:%d wrong state=%d\n", __func__, __LINE__, conn->state);
        conn->state = CONN_STATE_FAILED;
        return conn->failed_cb(conn);
    } else {
        NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
        conn->state = CONN_STATE_CONNECTING;
        return 0;
    }

}

int conn_connected(netmgr_conn_t* conn)
{
    NETMGR_CONN_LOGI("%s:%d old state=%d\n", __func__, __LINE__, conn->state);
    if(conn->state != CONN_STATE_OBTAINING_IP) {
        conn->state = CONN_STATE_CONNECTED;
    }
    return conn->connected_cb(conn);
}

int conn_obtaining_ip(netmgr_conn_t* conn)
{
    NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
    if((conn->state != CONN_STATE_CONNECTED)
      && (conn->state != CONN_STATE_OBTAINING_IP)
      && (conn->state != CONN_STATE_NETWORK_CONNECTED)) {
        NETMGR_CONN_LOGE("%s:%d wrong state=%d\n", __func__, __LINE__, conn->state);
        conn->state = CONN_STATE_FAILED;
        return conn->failed_cb(conn);
    } else {
        NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
        conn->state = CONN_STATE_OBTAINING_IP;
        return conn->obtaining_ip_cb(conn);
    }

}

int conn_network_connected(netmgr_conn_t* conn)
{
    if((conn->state != CONN_STATE_OBTAINING_IP)
      &&(conn->state != CONN_STATE_NETWORK_CONNECTED)) {
        NETMGR_CONN_LOGE("%s:%d wrong state=%d\n", __func__, __LINE__, conn->state);
        conn->state = CONN_STATE_FAILED;
        return conn->failed_cb(conn);
    } else {
        NETMGR_CONN_LOGI("%s:%d\n", __func__, __LINE__);
        conn->state = CONN_STATE_NETWORK_CONNECTED;
        return conn->network_connected_cb(conn);
    }

}

int conn_failed(netmgr_conn_t* conn)
{
    if(conn == NULL) {
        return -1;
    }

    if(conn->state != CONN_STATE_CONNECTED) {
        NETMGR_CONN_LOGE("%s:%d wrong state=%d\n", __func__, __LINE__, conn->state);
        conn->state = CONN_STATE_FAILED;
        return conn->failed_cb(conn);
    } else {
        conn->state = CONN_STATE_DISCONNECTING;
        return 0;
    }
}

/* Netmgr Connection State Management */
int netmgr_conn_state_change(netmgr_conn_t *conn, netmgr_conn_state_t new_state)
{
    if (conn == NULL) {
        return -1;
    }

    NETMGR_CONN_LOGI("%s:%d new_state=%d old_state=%d\n", __func__, __LINE__, new_state, conn->state);
    switch (new_state) {
        case CONN_STATE_DISCONNECTING:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_disconnecting(conn);
            break;
        case CONN_STATE_DISCONNECTED:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_disconnected(conn);
            break;
        case CONN_STATE_CONNECTING:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_connecting(conn);
            break;
        case CONN_STATE_CONNECTED:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_connected(conn);
            break;
        case CONN_STATE_OBTAINING_IP:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_obtaining_ip(conn);
            break;
        case CONN_STATE_NETWORK_CONNECTED:
            NETMGR_CONN_LOGD("%s:%d\n", __func__, __LINE__);
            conn_network_connected(conn);
            break;
        case CONN_STATE_FAILED:
        default:
            NETMGR_CONN_LOGE("%s:%d old state=%d new state=%d\n", __func__, __LINE__, conn->state, new_state);
            conn_failed(conn);
            break;
    }

    return 0;
}
