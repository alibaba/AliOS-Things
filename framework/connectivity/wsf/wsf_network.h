/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_NETWORK_H
#define WSF_NETWORK_H

#include "wsf_config.h"

#define WSF_MAX_UNACK_HEARTBEART    3

typedef enum {
    CONN_CLOSED = 0,
    CONN_OPEN,
    CONN_READY,
} CONN_STATE;

typedef struct wsf_connection_t {
    void *tcp;
    void *ssl;
    char *session_id;
    char *device_id;
    char *recv_buf;
    int recv_buf_len;
    int recv_buf_pos;
    int heartbeat_unack_counter;
    volatile CONN_STATE conn_state;
    void *mutex;
} wsf_connection_t;

wsf_code wsf_open_curator_connection(wsf_config_t *config);
wsf_code wsf_open_connection(wsf_config_t *config);
wsf_code wsf_send_msg(wsf_connection_t *conn, const char *data, int length);
wsf_code wsf_destroy_connections();
wsf_code wsf_reset_connection(wsf_connection_t *conn, int clear_session);
wsf_code wsf_recvbuff_append(wsf_connection_t *conn, const char *buf,
                             size_t length);
wsf_code wsf_recvbuff_clear();
char *wsf_session_id_get();
void wsf_session_id_set(const char *session_id);
char *wsf_device_id_get();
void wsf_device_id_set(const char *session_id);
void wsf_send_addr_req();
int adjust_heartbeat_interval(int hb);
wsf_code wsf_recvbuff_check(wsf_connection_t *conn);

static inline void wsf_reset_heartbeat_counter(wsf_connection_t *conn)
{
    if (conn) {
        conn->heartbeat_unack_counter = WSF_MAX_UNACK_HEARTBEART;
    }
}

static inline int wsf_is_heartbeat_timeout(wsf_connection_t *conn)
{
    if (conn) {
        return conn->heartbeat_unack_counter < 0;
    } else {
        return 0;
    }
}

static inline void wsf_dec_heartbeat_counter(wsf_connection_t *conn)
{
    if (conn) {
        conn->heartbeat_unack_counter--;
    }
}

#endif
