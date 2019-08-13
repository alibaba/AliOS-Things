/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "be_common.h"
#include "be_jse_module.h"
#include "be_jse_task.h"
#include "be_port_osal.h"
#include "hal/system.h"
#include "module_udp.h"

#ifdef DEBUG_UDP_ADDON
#define DEBUG_HEX_DUMP(str, buff, len) hexdump(str, buff, len)
#else
#define DEBUG_HEX_DUMP(str, buff, len)
#endif

typedef struct {
    char ip[32];
    int port;
} udp_options_t;

typedef struct {
    int socketid;
    char *msg;
    int msg_len;
    udp_options_t options;
    be_jse_symbol_t *callback;
} schedule_udp_t;

typedef struct {
    char *msg;
    int msg_len;
    char src_ip[16];
    short src_port;
    be_jse_symbol_t *cb;
} js_cb_udp_recv_t;

typedef struct {
    int ret;
    be_jse_symbol_t *cb;
} js_cb_udp_send_t;

/*************************************************************************************
*Function:    module_udp_create_socket
*Description: js native addon for UDP.createSocket();
*Called by:   js api
*Input:       none
*Output:      return socket fd when create socket success,
              return error number when create socket fail
**************************************************************************************/

static be_jse_symbol_t *module_udp_create_socket()
{
    be_jse_symbol_t *arg0 = NULL;
    int ret               = -1;
    int sock_id           = 0;

    be_jse_handle_function(0, NULL, NULL, NULL, NULL);

    sock_id = socket(AF_INET, SOCK_DGRAM, 0);

    jse_debug("module_udp_create_socket = %d\n\r", sock_id);
    return new_int_symbol(sock_id);
}

/*************************************************************************************
*Function:    module_udp_bind
*Description: js native addon for UDP.bind(socketid,"ip",port)
*Called by:   js api
*Input:       UDP.bind(socketid,"ip",port):
              socketid: is a iterger
              ip: is a ip stirng like "192.168.1.1"
              port: is a iterger port
*Output:      return 0 when bind successed
              return error number when bind fail
**************************************************************************************/

static be_jse_symbol_t *module_udp_bind()
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    int ret               = -1;
    int port              = 0;
    int socketfd          = -1;
    char ip[32]           = {0};

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);

    if ((arg0 == NULL) || !symbol_is_int(arg0)) {
        jse_error("module_udp_bind param 0 invalid");
        goto done;
    }

    if ((arg1 == NULL) || !symbol_is_string(arg1)) {
        jse_error("module_udp_bind param 1 invalid");
        goto done;
    }

    if ((arg2 == NULL) || !symbol_is_int(arg2)) {
        jse_error("module_udp_bind param 2 invalid");
        goto done;
    }

    socketfd = get_symbol_value_int(arg0);
    if (socketfd < 0) {
        jse_error("module_udp_bind socketid[%d] error", port);
        goto done;
    }

    symbol_to_str(arg1, ip, sizeof(ip));

    port = get_symbol_value_int(arg2);
    if (port <= 0) {
        jse_error("module_udp_bind port[%d] error", port);
        goto done;
    }

    struct sockaddr_in addr_in;

    bzero(&addr_in, sizeof(struct sockaddr_in));
    addr_in.sin_family = AF_INET;
    if (strcmp(ip, "127.0.0.1") == 0) {
        addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        addr_in.sin_addr.s_addr = inet_addr(ip);
    }
    addr_in.sin_port = htons(port);

    jse_debug("udp bind socketid=%d,ip=%s,port=%d\n\r", socketfd, ip, port);
    ret = bind(socketfd, (struct sockaddr *)&addr_in, sizeof(addr_in));

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(ret);
}

/*C call JS for UDP.recv*/
static void js_cb_udp_recv(void *pdata)
{
    int i                     = 0;
    js_cb_udp_recv_t *p_param = (js_cb_udp_recv_t *)pdata;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->func        = p_param->cb;
    async->param_count = 4;
    async->params      = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_int_symbol(p_param->msg_len);

    be_jse_symbol_t *array = new_symbol(BE_SYM_ARRAY);
    for (i = 0; i < p_param->msg_len; ++i) {
        be_jse_symbol_t *idx = new_int_symbol((p_param->msg)[i]);
        symbol_array_push(array, idx);
        symbol_unlock(idx);
    }
    async->params[1] = array;
    async->params[2] = new_str_symbol(p_param->src_ip);
    async->params[3] = new_int_symbol(p_param->src_port);

    be_jse_async_event_cb(async); /* async will free automatic */

    jse_free(p_param->msg);
    jse_free(p_param);
}

/*C call JS for UDP.send*/
void js_cb_udp_send(void *pdata)
{
    js_cb_udp_send_t *p_param = (js_cb_udp_send_t *)pdata;
    BE_ASYNC_S *async         = (BE_ASYNC_S *)jse_calloc(1, sizeof(BE_ASYNC_S));
    async->func               = p_param->cb;
    async->param_count        = 1;
    async->params             = (be_jse_symbol_t **)jse_calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_int_symbol(p_param->ret);

    be_jse_async_event_cb(async);
    jse_free(pdata);
}

/*************************************************************************************
 *Function:    task_udp_recv_fun
 *Description: create a task for blocking recvfrom call
 *Called by:
 **************************************************************************************/

static void task_udp_recv_fun(void *arg)
{
    schedule_udp_t *msg    = (schedule_udp_t *)arg;
    be_jse_symbol_t *array = NULL;
    int socketid;
    int ret = -1;
    int i   = 0;
    struct sockaddr_in srcaddr;
    socklen_t addr_len;
    int max_udp_recv_len = 64;

    if (msg == NULL) {
        jse_error("error:task_udp_recv_fun arg is null\n\r");
        be_osal_delete_task(NULL);
        return;
    }

    socketid  = msg->socketid;
    addr_len  = sizeof(struct sockaddr_in);
    char *buf = msg->msg;

    bzero(&srcaddr, sizeof(struct sockaddr_in));
    int recv_len = recvfrom(socketid, buf, max_udp_recv_len, 0,
                            (struct sockaddr *)&srcaddr, &addr_len);

    DEBUG_HEX_DUMP("udp recv:", buf, recv_len);

    js_cb_udp_recv_t *p_param = jse_calloc(1, sizeof(js_cb_udp_recv_t));
    if (!p_param) {
        goto done;
    }

    p_param->msg_len = recv_len;
    p_param->msg     = buf;
    snprintf(p_param->src_ip, sizeof(p_param->src_ip), "%s",
             inet_ntoa(srcaddr.sin_addr));
    p_param->src_port = ntohs(srcaddr.sin_port);
    p_param->cb       = msg->callback;

    be_jse_task_schedule_call(js_cb_udp_recv, p_param);

done:
    jse_free(arg);
    be_osal_delete_task(NULL);
}

/*************************************************************************************
 *Function:    task_udp_send_fun
 *Description: create a task for blocking sendto call
 *Called by:
 **************************************************************************************/

static void task_udp_send_fun(void *arg)
{
    int ret             = -1;
    schedule_udp_t *msg = (schedule_udp_t *)arg;
    int socketid;
    udp_options_t udp_options = {0};

    if (msg == NULL) {
        jse_error("error:task_udp_send_fun arg is NULL\n\r");
        be_osal_delete_task(NULL);
        return;
    }

    socketid = msg->socketid;
    memcpy(&udp_options, &(msg->options), sizeof(udp_options_t));
    be_osal_delay(50); /* need do things after state changed in main task */

    struct sockaddr_in addr_in;

    bzero(&addr_in, sizeof(struct sockaddr_in));
    addr_in.sin_family      = AF_INET;
    addr_in.sin_addr.s_addr = inet_addr(udp_options.ip);
    addr_in.sin_port        = htons(udp_options.port);
    int broadcast           = 1;

    if (strcmp(udp_options.ip, "255.255.255.255") == 0) {
        if (setsockopt(socketid, SOL_SOCKET, SO_BROADCAST, &broadcast,
                       sizeof(broadcast)) < 0) {
            jse_error("ERROR: setsockopt failed\n");
            goto done;
        }
    }

    ret                       = sendto(socketid, msg->msg, msg->msg_len, 0,
                 (struct sockaddr *)&addr_in, sizeof(addr_in));
    js_cb_udp_send_t *p_param = jse_calloc(1, sizeof(js_cb_udp_send_t));
    if (!p_param) {
        goto done;
    }

    p_param->cb  = msg->callback;
    p_param->ret = ret;
    be_jse_task_schedule_call(js_cb_udp_send, p_param);

done:
    jse_free(msg->msg);
    jse_free(msg);
    be_osal_delete_task(NULL);
}

/*************************************************************************************
*Function:    module_udp_sendto
*Description: js native addon for
UDP.send(socketid,option,buffer_array,function(ret){}) *Called by:   js api
*Input:       socketid: interger
              options: is a object include options.ip and options.port
              buffer_array: is a array which include message to send
              function(ret): is the callback function which has a ret input
param *Output:      return send msg length when send success return error number
when send fail
**************************************************************************************/

static be_jse_symbol_t *module_udp_sendto()
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    be_jse_symbol_t *arg3 = NULL;
    int ret               = -1;
    udp_options_t options;
    int sock_id = 0;
    int msg_len = 0;
    char *msg   = NULL;

    be_jse_handle_function(0, &arg0, &arg1, &arg2, &arg3);
    if ((arg0 == NULL) || !symbol_is_int(arg0) || (arg1 == NULL) ||
        !symbol_is_object(arg1) || (arg2 == NULL) || !symbol_is_array(arg2) ||
        (arg3 == NULL) || !symbol_is_function(arg3)) {
        jse_error("module_udp_sendto param invalid");
        goto done;
    }

    memset(&options, 0, sizeof(udp_options_t));
    sock_id = get_symbol_value_int(arg0);

    be_jse_symbol_t *ipSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "ip", false));
    be_jse_symbol_t *portSymbol = unlock_symbol_value(
        lookup_named_child_symbol(get_symbol_node_id(arg1), "port", false));

    options.port = get_symbol_value_int(portSymbol);
    symbol_to_str(ipSymbol, options.ip, sizeof(options.ip));

    msg     = be_array_symbol_to_buffer(arg2);
    msg_len = get_symbol_array_length(arg2);

    DEBUG_HEX_DUMP("msg:", msg, msg_len);

    schedule_udp_t *schedule_msg =
        (schedule_udp_t *)jse_calloc(1, sizeof(schedule_udp_t));
    if (!schedule_msg) {
        goto done;
    }

    schedule_msg->socketid = sock_id;
    schedule_msg->callback = arg3;
    schedule_msg->msg      = msg;
    schedule_msg->msg_len  = msg_len;
    memcpy(&(schedule_msg->options), &options, sizeof(udp_options_t));

    INC_SYMBL_REF(schedule_msg->callback);

    ret = be_osal_create_task("udp_send", task_udp_send_fun, schedule_msg, 4096,
                              ADDON_TSK_PRIORRITY, NULL);

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);
    symbol_unlock(ipSymbol);
    symbol_unlock(portSymbol);
    return new_int_symbol(ret);
}

/*************************************************************************************
*Function:    module_udp_recvfrom
*Description: js native addon for
              UDP.recv(socketid,function(length, buffer_array, src_ip,
src_port){}) *Called by:   js api *Input:       socketid: interger
              function(length, buffer_array, src_ip, src_port): the callback
function length: the recv msg length buffer_array: the recv msg buffer src_ip:
the peer ip string src_port: the peer port which is a interger

*Output:      return 0 when UDP.recv call ok
              return error number UDP.recv call fail
**************************************************************************************/

static be_jse_symbol_t *module_udp_recvfrom()
{
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    int ret               = -1;
    int sock_id           = 0;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0) || (arg1 == NULL) ||
        !symbol_is_function(arg1)) {
        jse_error("module_udp_recvfrom param invalid");
        goto done;
    }

    sock_id = get_symbol_value_int(arg0);
    if (sock_id < 0) {
        jse_warn("socket id[%d] is invalid", sock_id);
    }

    jse_debug("module_udp_recvfrom id=%d\n\r", sock_id);
    schedule_udp_t *schedule_msg =
        (schedule_udp_t *)jse_calloc(1, sizeof(schedule_udp_t));
    if (!schedule_msg) {
        goto done;
    }

    char *msg = jse_calloc(1, MAX_UDP_RECV_LEN + 1);
    if (!msg) {
        jse_free(schedule_msg);
        goto done;
    }

    schedule_msg->callback = arg1;
    schedule_msg->msg      = msg;
    schedule_msg->msg_len  = 0;
    schedule_msg->socketid = sock_id;

    INC_SYMBL_REF(schedule_msg->callback);

    ret = be_osal_create_task("udp_recv", task_udp_recv_fun, schedule_msg, 4096,
                              ADDON_TSK_PRIORRITY, NULL);

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(ret);
}

/*************************************************************************************
*Function:    module_udp_close_socket
*Description: js native addon for
              UDP.close(socketid)
*Called by:   js api
*Input:       socketid: interger

*Output:      return 0 when UDP.close call ok
              return error number UDP.close call fail
**************************************************************************************/

static be_jse_symbol_t *module_udp_close_socket()
{
    be_jse_symbol_t *arg0 = NULL;
    int ret               = -1;
    int sock_id           = 0;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0)) {
        jse_error("module_udp_recvfrom param invalid");
        goto done;
    }

    sock_id = get_symbol_value_int(arg0);
    if (sock_id <= 0) {
        jse_warn("socket id[%d] is invalid", sock_id);
    }

    ret = close(sock_id);

done:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *udp_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name)
{
    jse_debug("%s Enter: name=%s", __FUNCTION__, name);

    if (strcmp(name, "createSocket") == 0) return module_udp_create_socket();
    if (strcmp(name, "bind") == 0) return module_udp_bind();
    if (strcmp(name, "send") == 0) return module_udp_sendto();
    if (strcmp(name, "recv") == 0) return module_udp_recvfrom();
    if (strcmp(name, "close") == 0) return module_udp_close_socket();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_udp_register(void)
{
    be_jse_module_load("UDP", udp_module_handle_cb);
}
