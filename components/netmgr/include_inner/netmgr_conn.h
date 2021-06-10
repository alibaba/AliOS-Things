/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef NETMGR_CONN_H
#define NETGMR_CONN_H
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <netmgr.h>
#include "sys/socket.h"

typedef struct netmgr_conn netmgr_conn_t;
struct netmgr_conn
{
    netmgr_conn_t* next;
    netmgr_hdl_t hdl;
    void* netif;
    void* saved_config;
    netmgr_conn_state_t state;   /* connection state */
    int type;                    /* link type eg:Wi-Fi, Cellular */
    bool  auto_reconnect;         /* Auto reconnect setting flag */
    int   use_ip_mode;            /* Ip mode: Auto, Static */
    ip_addr_t static_ip;
    ip_addr_t static_mask;
    ip_addr_t static_gw;
    ip_addr_t static_dns;
    int  reconnect_task_running;  /* reconnect task is running:true(running), false(stop) */
    int  (*connected_cb)(netmgr_conn_t* conn);         /* link connected callback */
    int  (*failed_cb)(netmgr_conn_t* conn);            /* failed callback */
    int  (*obtaining_ip_cb)(netmgr_conn_t* conn);      /* obtaining_ip callback */
    int  (*network_connected_cb)(netmgr_conn_t* conn); /* network connected callback */
    int  (*disconnecting_cb)(netmgr_conn_t* conn);     /* disconnecting callback */
    int  (*disconnected_cb)(netmgr_conn_t* conn);      /* disconnected callback */
    void*  msg_cb_list;                                /* msg callback list */
};

netmgr_conn_t* netmgr_conn_init(netmgr_type_t type);
int netmgr_conn_deinit(netmgr_conn_t* conn);
int netmgr_conn_state_change(netmgr_conn_t *conn, netmgr_conn_state_t new_state);

#endif /* NETMGR_CONN_H */
