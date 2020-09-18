/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef NETMGR_CONN_H
#define NETGMR_CONN_H
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum netmgr_conn_state{
    CONN_STATE_DISCONNECTING,
    CONN_STATE_DISCONNECTED,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_OBTAINING_IP,
    CONN_STATE_NETWORK_CONNECTED,
    CONN_STATE_FAILED,
    CONN_STATE_UNKNOWN
} netmgr_conn_state_t;

typedef struct netmgr_conn netmgr_conn_t;
struct netmgr_conn
{
    netmgr_conn_t* next;
    void* saved_config;
    netmgr_conn_state_t state;   /* connection state */
    int type;                    /* link type eg:Wi-Fi, Cellular */
    int auto_reconnect;
    int  reconnect_task_running;  /* reconnect task is running:true(running), false(stop) */
    int  (*connected_cb)(netmgr_conn_t* conn);         /* link connected callback */
    int  (*failed_cb)(netmgr_conn_t* conn);            /* failed callback */
    int  (*obtaining_ip_cb)(netmgr_conn_t* conn);      /* obtaining_ip callback */
    int  (*network_connected_cb)(netmgr_conn_t* conn); /* network connected callback */
    int  (*disconnecting_cb)(netmgr_conn_t* conn);     /* disconnecting callback */
    int  (*disconnected_cb)(netmgr_conn_t* conn);      /* disconnected callback */
    int  (*msg_cb)(void* data);                        /* msg callback */
};

netmgr_conn_t* netmgr_conn_init(int type);
int netmgr_conn_deinit(netmgr_conn_t* conn);
int netmgr_conn_state_change(netmgr_conn_t *conn, netmgr_conn_state_t new_state);

#endif /* NETMGR_CONN_H */
