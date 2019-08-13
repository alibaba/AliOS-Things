/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "be_jse_api.h"
#include "be_jse_module.h"
#include "be_jse_task.h"
#include "module_net.h"

#include "be_port_osal.h"
#include "hal/system.h"

typedef enum { MUTEX_UNLOCKED = 0, MUTEX_LOCKED = 1 } net_mutex_t;

typedef struct {
    be_jse_symbol_t *onconnect_cb;
    be_jse_symbol_t *onerror_cb;
    be_jse_symbol_t *ondata_cb;
    be_jse_symbol_t *onclose_cb;
    char *data_recv;
    int socketid;
} js_cb_net_param_t;

#define NET_SOCKET_RECV_BUFF_LEN 512
typedef void (*ondata)(int conn, char *data);
typedef void (*onclose)(int conn);
typedef void (*onconnect)(int conn);
typedef void (*onerror)(int conn);

/* options for NET.connect */
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

static int g_net_recv_mutex = MUTEX_UNLOCKED;

/* for example addr: tcp://www.baidu.com:80 */
static int parse_net_option_addr(const char *addr,
                                 net_sckt_param_t *p_sckt_param)
{
    char *begin;
    char *colon;

    if (!addr || !p_sckt_param) return -1;

    if (strncmp(addr, "tcp", strlen("tcp")) == 0) {
        p_sckt_param->proto = SOCK_STREAM;
    } else if (strncmp(addr, "udp", strlen("udp")) == 0) {
        p_sckt_param->proto = SOCK_DGRAM;
    } else if (strncmp(addr, "raw", strlen("raw")) == 0) {
        p_sckt_param->proto = SOCK_RAW;
    } else {
        jse_error("invalid net connection protocol type");
        return -1;
    }

    begin = strstr(addr, "//");
    if (!begin) return -1;

    begin = begin + 2;
    colon = strrchr(addr, ':');

    if (!colon) return -1;
    if (colon - begin <= 0) return -1;

    strncpy(p_sckt_param->host_ip, begin, colon - begin);
    p_sckt_param->host_ip[colon - begin] = '\0';

    colon              = colon + 1;
    p_sckt_param->port = atoi(colon);
    if (p_sckt_param->port <= 0) return -1;

    return 0;
}

static int net_socket_connect(net_sckt_param_t *p_sckt_param)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    int sockfd           = 0;
    int ret              = -1;
    char s_port[10]      = {0};

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = p_sckt_param->proto;

    itoa(p_sckt_param->port, s_port, 10);
    ret    = getaddrinfo(p_sckt_param->host_ip, s_port, &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        return -1;
    }

    ret = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (ret != 0) {
        return -1;
    }

    return sockfd;
}

/* C call JS for NET.onconnect */
void js_cb_net_onconnect(void *param)
{
    js_cb_net_param_t *pdata = (js_cb_net_param_t *)param;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    if (pdata->socketid > 0) {
        async->func      = (be_jse_symbol_t *)pdata->onconnect_cb;
        async->params[0] = new_int_symbol(pdata->socketid);
        jse_debug("NET.connect ok ,socketid = %d", pdata->socketid);
    } else {
        async->func      = (be_jse_symbol_t *)pdata->onerror_cb;
        async->params[0] = new_str_symbol("connect error");
        jse_debug("NET.connect error");
    }

    INC_SYMBL_REF(async->func); /* decrease symble reference when task exit */
    be_jse_async_event_cb(async);
}

/* C call JS for NET.ondata */
void js_cb_net_ondata(void *param)
{
    js_cb_net_param_t *pdata = (js_cb_net_param_t *)param;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->param_count = 2;
    async->params      = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->func      = (be_jse_symbol_t *)pdata->ondata_cb;
    async->params[0] = new_int_symbol(pdata->socketid);
    jse_debug("js_cb_net_ondata get data=%s\n\r", pdata->data_recv);
    async->params[1] = new_str_symbol(pdata->data_recv);

    INC_SYMBL_REF(async->func); /* decrease symble reference when task exit */
    be_jse_async_event_cb(async);
    memset(pdata->data_recv, 0, NET_SOCKET_RECV_BUFF_LEN);
    g_net_recv_mutex = MUTEX_UNLOCKED;
}

/* C call JS for NET.onerror */
void js_cb_net_onerror(void *param)
{
    js_cb_net_param_t *pdata = (js_cb_net_param_t *)param;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);

    async->func      = (be_jse_symbol_t *)pdata->onerror_cb;
    async->params[0] = new_str_symbol("recv error");

    INC_SYMBL_REF(async->func); /* decrease symble reference when task exit */
    be_jse_async_event_cb(async);
}

/* C call JS for NET.onclose */
void js_cb_net_onclose(void *param)
{
    js_cb_net_param_t *pdata = (js_cb_net_param_t *)param;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);

    async->func      = (be_jse_symbol_t *)pdata->onclose_cb;
    async->params[0] = new_int_symbol(pdata->socketid);

    INC_SYMBL_REF(async->func); /* decrease symble reference when task exit */
    be_jse_async_event_cb(async);
}

/* create task for socket receiving */
static void task_net_connect_fun(void *arg)
{
    static int socketid;
    int ret                     = -1;
    char *buffer                = NULL;
    net_sckt_param_t sckt_param = {0};
    net_options_t *msg          = (net_options_t *)arg;

    memset(&sckt_param, 0, sizeof(net_sckt_param_t));
    js_cb_net_param_t *pdata = jse_calloc(1, sizeof(js_cb_net_param_t));
    if (!pdata) {
        goto done;
    }

    pdata->onconnect_cb = msg->onconn_cb;
    pdata->onerror_cb   = msg->onerror_cb;
    pdata->ondata_cb    = msg->ondata_cb;
    pdata->onclose_cb   = msg->onclose_cb;

    ret = parse_net_option_addr(msg->addr, &sckt_param);
    if (ret < 0) {
        be_jse_task_schedule_call(js_cb_net_onerror, pdata);
        goto done;
    }

    socketid        = net_socket_connect(&sckt_param);
    pdata->socketid = socketid;
    be_jse_task_schedule_call(js_cb_net_onconnect, pdata);

    if (socketid > 0) {
        buffer             = jse_calloc(1, NET_SOCKET_RECV_BUFF_LEN);
        int bytes_received = 0;
        while (1) {
            if (g_net_recv_mutex == MUTEX_LOCKED) {
                /* waitting for NET.onrecv return */
                be_osal_delay(30);
                continue;
            }
            jse_debug("ready to recv...socketid=%d", socketid);
            bytes_received =
                recv(socketid, buffer, NET_SOCKET_RECV_BUFF_LEN, 0);
            if (bytes_received > 0) { /* data received */
                jse_debug("recved...=%s", buffer);
                pdata->data_recv = buffer;
                g_net_recv_mutex = MUTEX_LOCKED;
                be_jse_task_schedule_call(js_cb_net_ondata, pdata);
            } else if (bytes_received < 0) { /* socket error */
                if (errno == EINTR) {
                    continue;
                }
                jse_warn("recv...error=%d", bytes_received);
                break;
            } else {
                jse_warn("server closed=%d", bytes_received);
                break;
            }
        }

        /* callback when close or error */
        if (bytes_received == 0) {
            be_jse_task_schedule_call(js_cb_net_onclose, pdata);
        } else {
            be_jse_task_schedule_call(js_cb_net_onerror, pdata);
        }
    }

done:
    /*onxxx_cb callback is optional,need check if vaild*/
    if (msg->onclose_cb) symbol_unlock((be_jse_symbol_t *)msg->onclose_cb);
    if (msg->ondata_cb) symbol_unlock((be_jse_symbol_t *)msg->ondata_cb);
    if (msg->onerror_cb) symbol_unlock((be_jse_symbol_t *)msg->onerror_cb);
    if (msg->onconn_cb) symbol_unlock((be_jse_symbol_t *)msg->onconn_cb);
    jse_free(msg->addr);
    jse_free(msg);
    jse_free(pdata);
    jse_free(buffer);

    be_osal_delete_task(NULL);
}

static void task_net_send_fun(void *arg)
{
    int ret          = -1;
    int bytes_sent   = 0;
    send_data_t *msg = (send_data_t *)arg;

    if (!msg) return;

    int size = strlen(msg->p_data) + 1; /* TODO,use size */

    char *buffer = msg->p_data;
    while (size > 0) {
        bytes_sent = send(msg->sock_id, buffer, size, 0);
        if (bytes_sent <= 0) break;
        size   = size - bytes_sent;
        buffer = buffer + bytes_sent;
        jse_debug("task_net_send_fun sent: %d", bytes_sent);
    }

    jse_free(msg->p_data);
    jse_free(msg);
    be_osal_delete_task(NULL);
}

/*********************************************************
*Function:    module_net_connect
*Description: js native addon for NET.connect
*Called by:   js api
*Input:       NET.connect(options),options is a object like this
                                { addr: 'tcp://127.0.0.1:1234',
                                  onconnect: function(conn) {},
                                  ondata: function(conn, data){},
                                  onclose: function(conn) {},
                                  onerror: function(err) {},
                                }
*Output: 0 try connect ok ,other try connect fail
*********************************************************/
static be_jse_symbol_t *module_net_connect()
{
    be_jse_symbol_t *arg0 = NULL;
    int ret               = -1;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_object(arg0)) {
        jse_error("module_net_connect param invalid");
        goto done;
    }

    be_jse_symbol_t *addrNetSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg0), "addr", false));
    be_jse_symbol_t *onConnNetSymbol =
        unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0),
                                                      "onconnect", false));
    be_jse_symbol_t *onDataNetSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg0), "ondata", false));
    be_jse_symbol_t *onCloseNetSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg0), "onclose", false));
    be_jse_symbol_t *onErrorNetSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg0), "onerror", false));

    if (addrNetSymbol == NULL || onConnNetSymbol == NULL) {
        jse_error("module_net_connect param invalid");
        goto done;
    }

    net_options_t *p_net_options = jse_calloc(1, sizeof(net_options_t));
    p_net_options->addr          = jse_calloc(1, symbol_str_len(addrNetSymbol) + 1);

    if (!p_net_options || p_net_options->addr == NULL) goto done;

    /* increase the count of call function */
    symbol_to_str(addrNetSymbol, p_net_options->addr,
                  symbol_str_len(addrNetSymbol));
    symbol_unlock(addrNetSymbol);
    if (onConnNetSymbol && symbol_is_function(onConnNetSymbol)) {
        p_net_options->onconn_cb = (onconnect)onConnNetSymbol;
        INC_SYMBL_REF(onConnNetSymbol);
    }
    if (onDataNetSymbol && symbol_is_function(onDataNetSymbol)) {
        p_net_options->ondata_cb = (ondata)onDataNetSymbol;
        INC_SYMBL_REF(onDataNetSymbol);
    }
    if (onCloseNetSymbol && symbol_is_function(onCloseNetSymbol)) {
        p_net_options->onclose_cb = (onclose)onCloseNetSymbol;
        INC_SYMBL_REF(onCloseNetSymbol);
    }
    if (onErrorNetSymbol && symbol_is_function(onErrorNetSymbol)) {
        p_net_options->onerror_cb = (onerror)onErrorNetSymbol;
        INC_SYMBL_REF(onErrorNetSymbol);
    }

    ret = be_osal_create_task("net_connect", task_net_connect_fun,
                              p_net_options, 4096, ADDON_TSK_PRIORRITY, NULL);
done:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

/*************************************************************************************
 *Function:    module_net_send
 *Description: js native addon for NET.send
 *Called by:   js api
 *Input:       NET.send(conn，data), conn is the socket handle, data is which to
 *be sent Output: 0 js call native ok ,other js call native fail
 **************************************************************************************/

static be_jse_symbol_t *module_net_send()
{
    int ret                  = -1;
    int sock_id              = 0;
    char *data               = NULL;
    send_data_t *p_send_data = NULL;
    be_jse_symbol_t *arg0    = NULL;
    be_jse_symbol_t *arg1    = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0) || (arg1 == NULL) ||
        !symbol_is_string(arg1)) {
        jse_error("module_net_connect param invalid");
        goto done;
    }

    sock_id = get_symbol_value_int(arg0);
    data    = jse_calloc(1, symbol_str_len(arg1) + 1);
    if (!data) goto done;

    symbol_to_str(arg1, data, symbol_str_len(arg1));
    jse_debug("NET.send(%d,%s)", sock_id, data);

    p_send_data          = jse_calloc(1, sizeof(send_data_t));
    p_send_data->p_data  = data;
    p_send_data->sock_id = sock_id;

    ret = be_osal_create_task("net_connect", task_net_send_fun, p_send_data,
                              4096, ADDON_TSK_PRIORRITY, NULL);
    if (ret < 0) {
        jse_free(p_send_data);
        jse_free(p_send_data->p_data);
    }

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(ret);
}

/*****************************************************************************
*Function:    module_net_close
*Description: js native addon for NET.close
*Called by:   js api
*Input:       NET.closet(conn) ; conn is the handle which NET.connect's
                          callback return ,for example onconnect: function(conn)
{} *Output:      0 close ok ,other close fail
*****************************************************************************/

static be_jse_symbol_t *module_net_close()
{
    be_jse_symbol_t *arg0 = NULL;
    int ret               = -1;
    int sock_id           = 0;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0)) {
        jse_error("module_net_close param invalid");
        goto done;
    }

    sock_id = get_symbol_value_int(arg0);
    if (sock_id > 0) {
        jse_warn("socket id[%d] has been closed", sock_id);
        ret = close(sock_id);
    }

done:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var, const char *name)
{
    jse_debug("%s Enter: name=%s", __FUNCTION__, name);

    if (strcmp(name, "connect") == 0) return module_net_connect();
    if (strcmp(name, "send") == 0) return module_net_send();
    if (strcmp(name, "close") == 0) return module_net_close();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_net_register(void)
{
    be_jse_module_load("NET", module_handle_cb);
}