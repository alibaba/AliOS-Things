/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include "connectivity_manager.h"
#include "aos/aos.h"

#define cm_init_connect(conn) _##conn##_init()
#define MODULE "conn_mgr"

dlist_t g_conn_list;

CONNECTIVITY_DELCARE(wsf);

int cm_init()
{
    INIT_AOS_DLIST_HEAD(&g_conn_list);
    cm_init_connect(wsf);
    cm_add_conn(&wsf);
    return CONNECT_RESULT_OK;
}

int cm_add_conn(connectivity_t *conn)
{
    dlist_add(&conn->list_head, &g_conn_list);
    return CONNECT_RESULT_OK;
}

int cm_del_conn(connectivity_t *conn)
{
    dlist_del(&conn->list_head);
    return CONNECT_RESULT_OK;
}

int cm_get_conn_state(connectivity_t *conn)
{
    return conn->state;
}

int cm_query_conn()
{
    connectivity_t *pos;
    if (dlist_empty(&g_conn_list)) {
        LOGI(MODULE, "No service!");
    } else {
        dlist_for_each_entry_reverse(pos, &g_conn_list, list_head, connectivity_t) {
            LOGI(MODULE, "%s is found!", ((connectivity_t *)pos)->name);
        }
    }
    return CONNECT_RESULT_OK;
}

connectivity_t *cm_get_conn(char *conn_name)
{
    connectivity_t *pos;

    dlist_for_each_entry_reverse(pos, &g_conn_list, list_head, connectivity_t) {
        connectivity_t *conn = (connectivity_t *)pos;
        if (!strcmp(conn->name, conn_name)) {
            return conn;
        }
    }
    return 0;
}

int cm_bind_conn(char *conn_name, connectivity_cb listen)
{
    connectivity_t *conn = cm_get_conn(conn_name);
    if (conn) {
        return conn->add_listener(listen);
    }
    return CONNECT_RESULT_ERR;
}

int cm_release_conn(char *conn_name, connectivity_cb listen)
{
    connectivity_t *conn = cm_get_conn(conn_name);
    if (conn) {
        return conn->del_listener(listen);
    }
    return CONNECT_RESULT_ERR;
}

char *cm_code2string(int state)
{
    if (state == CONNECT_RESULT_OK) {
        return "CONNECT_RESULT_OK";
    } else if (state == CONNECT_RESULT_ERR) {
        return "CONNECT_RESULT_ERR";
    } else if (state == CONNECT_EVENT) {
        return "CONNECT_EVENT";
    } else if (state == CONNECT_DATA) {
        return "CONNECT_DATA";
    } else if (state == CONNECT_BIND) {
        return "CONNECT_BIND";
    } else if (state == CONNECT_RELEASE) {
        return "CONNECT_RELEASE";
    } else if (state == CONNECT_STATE_OPEN) {
        return "CONNECT_STATE_OPEN";
    } else if (state == CONNECT_STATE_READY) {
        return "CONNECT_STATE_READY";
    } else if (state == CONNECT_STATE_CLOSE) {
        return "CONNECT_STATE_CLOSE";
    }
    return "CONNECT_UNKNOWN";
}
