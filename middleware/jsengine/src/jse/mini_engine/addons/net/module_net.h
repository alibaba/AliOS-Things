/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef MODULE_NET_H
#define MODULE_NET_H

#if defined(__cplusplus)
extern "C" {
#endif

/*inter definiton*/
#ifdef JS_NATIVE_MODULE_NET_C

#define NET_SOCKET_RECV_BUFF_LEN 512
typedef void (*ondata)(int conn, char *data);
typedef void (*onclose)(int conn);
typedef void (*onconnect)(int conn);
typedef void (*onerror)(int conn);

/*options for NET.connect*/
typedef struct {
    char *addr;
    onconnect onconn_cb;
    ondata ondata_cb;
    onclose onclose_cb;
    onerror onerror_cb;
} net_options_t;

typedef struct {
    int sock_id;
    char *p_data;
} send_data_t;

typedef struct {
    int proto;
    char host_ip[128];
    int port;
} net_sckt_param_t;

typedef struct {
    int id;
    int len;
    char *msg;
} Message;
#define NET_QUEUE_MAX_MSG_SIZE (sizeof(Message))
#define NET_QUEUE_MAX_MSG_COUNT (8)
#define NET_QUEUE_SIZE (NET_QUEUE_MAX_MSG_SIZE * NET_QUEUE_MAX_MSG_COUNT)

#endif  /* #ifdef JS_NATIVE_MODULE_NET_C */

void module_net_register(void);

#if defined(__cplusplus)
}
#endif

#endif /* MODULE_NET_H */
