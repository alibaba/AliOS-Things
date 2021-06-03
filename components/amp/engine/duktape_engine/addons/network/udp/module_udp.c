/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "amp_utils.h"
#include "amp_task.h"
#include "amp_defines.h"
#include "aos_udp.h"
#include "be_inl.h"

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#define MOD_STR "UDP"
#define MAX_UDP_RECV_LEN 256
#define MAX_UDP_RECV_TIMEOUT 200

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int port;
    char *msg;
} udp_options_t;

typedef struct {
    int sock_id;
    char *msg;
    int msg_len;
    udp_options_t options;
    int js_cb_ref;
} udp_send_param_t;

typedef struct {
    int ret;
    int js_cb_ref;
} udp_send_notify_param_t;

typedef struct {
    int sock_id;
    udp_options_t options;
    int js_cb_ref;
} udp_recv_param_t;

typedef struct {
    char buf[MAX_UDP_RECV_LEN];
    int recv_len;
    char src_ip[INET_ADDRSTRLEN];
    unsigned short src_port;
    int js_cb_ref;
} udp_recv_notify_param_t;

static char g_udp_close_flag = 0;
static char g_udp_recv_flag = 0;
static aos_sem_t g_udp_close_sem = NULL;

/*************************************************************************************
 * Function:    native_udp_create_socket
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static duk_ret_t native_udp_create_socket(duk_context *ctx)
{
    int sock_id = 0;

    sock_id = aos_udp_socket_create();
    if (sock_id < 0) {
        amp_warn(MOD_STR, "create socket error!");
        goto out;
    }
    amp_debug(MOD_STR, "sock_id = %d", sock_id);

    duk_push_int(ctx, sock_id);

    return 1;

out:
    duk_push_string(ctx, "create socket error!");
    return duk_throw(ctx);
}

/*************************************************************************************
 * Function:    native_udp_bind
 * Description: js native addon for UDP.bind(sock_id,"ip",port)
 * Called by:   js api
 * Input:       UDP.bind(sock_id,"ip",port):
 *            sock_id: is a iterger
 *            ip: is a ip stirng like "192.168.1.1"
 *            port: is a iterger port
 * Output:      return 0 when bind successed
 *            return error number when bind fail
 **************************************************************************************/
static duk_ret_t native_udp_bind(duk_context *ctx)
{
    int ret  = -1;
    int port = 0;
    int sock_id;

    if (!duk_is_number(ctx, 0) || !duk_is_number(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (number, number)");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id < 0) {
        amp_error(MOD_STR, "socket id[%d] error", sock_id);
        goto out;
    }

    port = duk_get_int(ctx, 1);
    if (port < 0) {
        amp_error(MOD_STR, "port[%d] error", port);
        goto out;
    }

    amp_debug(MOD_STR, "udp bind socket id=%d, port=%d", sock_id, port);
    ret = aos_udp_socket_bind(sock_id, port);
    if (ret < 0) {
        amp_error(MOD_STR, "udp bind error");
        goto out;
    }

    // return ret;
out:
    duk_push_int(ctx, ret);
    return 1;
}

static void udp_send_notify(void *pdata)
{
    udp_send_notify_param_t *p = (udp_send_notify_param_t *)pdata;
    duk_context *ctx           = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->ret);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, p->js_cb_ref);
    aos_free(p);
    duk_gc(ctx, 0);
}

/*************************************************************************************
 * Function:    udp_send_routin
 * Description: create a task for blocking sendto call
 * Called by:
 **************************************************************************************/
static int udp_send_routine(udp_send_param_t *send_param)
{
    int ret                      = -1;
    int sock_id;
    udp_options_t udp_options;
    udp_send_notify_param_t *p;
    duk_context *ctx = be_get_context();
    sock_id = send_param->sock_id;
    memcpy(&udp_options, &(send_param->options), sizeof(udp_options_t));

    ret = aos_udp_sendto(sock_id, &udp_options, send_param->msg, send_param->msg_len, 0);
    p   = aos_calloc(1, sizeof(udp_send_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        be_unref(ctx, send_param->js_cb_ref);
        goto out;
    }

    p->js_cb_ref = send_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(udp_send_notify, p);
    ret = 0;

out:
    aos_free(send_param->msg);
    aos_free(send_param);
    return ret;
}

/*************************************************************************************
 * Function:    native_udp_sendto
 * Description: js native addon for
 *UDP.send(sock_id,option,buffer_array,function(ret){}) Called by:   js api
 * Input:       sock_id: interger
 *            options: is a object include options.ip and options.port
 *            buffer_array: is a array which include message to send
 *            function(ret): is the callback function which has a ret input
 *param Output:      return send msg length when send success return error
 *number when send fail
 **************************************************************************************/

static duk_ret_t native_udp_sendto(duk_context *ctx)
{
    int ret = -1;
    udp_options_t options;
    int sock_id;
    int i;
    int msg_len;
    char *msg;

    if (!duk_is_number(ctx, 0) || !duk_is_object(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (number, object, array, function)");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }
    memset(&options, 0, sizeof(udp_options_t));
    /* options.port */
    duk_get_prop_string(ctx, 1, "port");
    if (!duk_is_number(ctx, -1)) {
        amp_warn(MOD_STR, "port not specify");
        duk_pop(ctx);
        goto out;
    }
    options.port = duk_get_int(ctx, -1);
    duk_pop(ctx);

    /* options.address */
    duk_get_prop_string(ctx, 1, "address");
    if (!duk_is_string(ctx, -1)) {
        amp_warn(MOD_STR, "ip not specify");
        duk_pop(ctx);
        goto out;
    }
    strncpy(options.ip, duk_get_string(ctx, -1), sizeof(options.ip) - 1);
    duk_pop(ctx);

    /* options.message */
    duk_get_prop_string(ctx, 1, "message");
    if (!duk_is_array(ctx, -1)) {
        amp_warn(MOD_STR, "message not specify");
        duk_pop(ctx);
        goto out;
    }

    msg_len = duk_get_length(ctx, -1);
    msg     = (char *)aos_calloc(1, msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < msg_len; i++) {
        duk_get_prop_index(ctx, -1, i);
        msg[i] = duk_get_int(ctx, -1);
        duk_pop(ctx);
    }
    msg[msg_len] = 0;

    // options.msg = (char *)duk_get_string(ctx, -1);
    // msg_len = strlen(options.msg);
    // duk_pop(ctx);

    // msg     = (char *)aos_calloc(1, msg_len + 1);
    // if (!msg) {
    //     amp_warn(MOD_STR, "allocate memory failed");
    //     goto out;
    // }
    // strncpy(msg, options.msg, msg_len);
    amp_debug(MOD_STR, "msg is:%s, length is: %d", msg, msg_len);
    udp_send_param_t *send_param =
        (udp_send_param_t *)aos_malloc(sizeof(*send_param));
    if (!send_param) {
        amp_error(MOD_STR, "allocate memory failed");
        aos_free(msg);
        goto out;
    }
    send_param->sock_id = sock_id;
    duk_dup(ctx, 2);
    send_param->js_cb_ref = be_ref(ctx);
    send_param->msg       = msg;
    send_param->msg_len   = msg_len;
    memcpy(&(send_param->options), &options, sizeof(udp_options_t));

    amp_debug(MOD_STR, "sockid:%d ip:%s port:%d msg:%s msg_len:%d", sock_id, options.ip, options.port, msg, msg_len);

    udp_send_routine(send_param);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void udp_recv_notify(void *pdata)
{
    int i                      = 0;
    udp_recv_notify_param_t *p = (udp_recv_notify_param_t *)pdata;
    duk_context *ctx           = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    int arr_idx = duk_push_array(ctx);
    if(p->recv_len > 0) {
        for (i = 0; i < p->recv_len; i++) {
            duk_push_int(ctx, p->buf[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
    }
    duk_push_object(ctx);
    duk_push_string(ctx, p->src_ip);
    duk_put_prop_string(ctx, -2, "host");
    duk_push_uint(ctx, p->src_port);
    duk_put_prop_string(ctx, -2, "port");
    duk_push_int(ctx, p->recv_len);
    if (duk_pcall(ctx, 3) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
    if(p->recv_len < 0) {
        be_unref(ctx, p->js_cb_ref);
        aos_free(p);
    }
}

/*************************************************************************************
 * Function:    udp_recv_routine
 * Description: create a task for blocking recvfrom call
 * Called by:
 **************************************************************************************/
static void udp_recv_routine(void *arg)
{
    udp_recv_param_t *recv_param = (udp_recv_param_t *)arg;
    int sock_id;
    udp_options_t udp_options;
    aos_networkAddr addr_info;
    udp_recv_notify_param_t *p;
    duk_context *ctx = be_get_context();
    sock_id = recv_param->sock_id;
    memcpy(&udp_options, &(recv_param->options), sizeof(udp_options_t));
    p = aos_calloc(1, sizeof(udp_recv_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        duk_context *ctx = be_get_context();
        be_unref(ctx, recv_param->js_cb_ref);
        goto out;
    }

    g_udp_recv_flag = 1;
    while(1) {
        p->recv_len = aos_udp_recvfrom(sock_id, &addr_info, p->buf, sizeof(p->buf), MAX_UDP_RECV_TIMEOUT);
        strcpy(p->src_ip, addr_info.addr);
        p->src_port  = addr_info.port;
        p->js_cb_ref = recv_param->js_cb_ref;

        if (p->recv_len > 0) {
            amp_task_schedule_call(udp_recv_notify, p);
        }

        if (p->recv_len < 0) {
            // connection closed
            amp_error(MOD_STR, "connection closed:%d", p->recv_len);
            break;
        }

        if (g_udp_close_flag) {
            duk_context *ctx = be_get_context();
            be_unref(ctx, recv_param->js_cb_ref);
            if (p) {
                aos_free(p);
            }
            break;
        }
    }
    aos_udp_close_without_connect(sock_id);

out:
    aos_free(recv_param);
    g_udp_recv_flag = 0;
    aos_sem_signal(&g_udp_close_sem);
    aos_task_exit(0);

    return;
}

/*************************************************************************************
 * Function:    native_udp_recvfrom
 * Description: js native addon for
 *            UDP.recv(sock_id,function(length, buffer_array, src_ip,
 *src_port){}) Called by:   js api Input:       sock_id: interger
 *            function(length, buffer_array, src_ip, src_port): the callback
 *function length: the recv msg length buffer_array: the recv msg buffer src_ip:
 *the peer ip string src_port: the peer port which is a interger
 *
 * Output:      return 0 when UDP.recv call ok
 *            return error number UDP.recv call fail
 **************************************************************************************/
static duk_ret_t native_udp_recvfrom(duk_context *ctx)
{
    int ret     = -1;
    int sock_id = 0;
    aos_task_t udp_recv_task;
    udp_recv_param_t *recv_param;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be number and function");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id < 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }

    amp_debug(MOD_STR, "sock_id: %d", sock_id);
    recv_param = (udp_recv_param_t *)aos_calloc(1, sizeof(*recv_param));
    if (!recv_param) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    recv_param->sock_id = sock_id;
    duk_dup(ctx, 1);
    recv_param->js_cb_ref = be_ref(ctx);

    ret = aos_task_new_ext(&udp_recv_task, "amp udp recv task", udp_recv_routine, recv_param, 1024 * 4, ADDON_TSK_PRIORRITY);
    if (ret != 0) {
        amp_debug(MOD_STR, "udp recv task create faild");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_udp_close_socket
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static duk_ret_t native_udp_close_socket(duk_context *ctx)
{
    int ret     = -1;
    int sock_id = 0;

    if (!duk_is_number(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }
    g_udp_close_flag = 1;
    aos_sem_wait(&g_udp_close_sem, MAX_UDP_RECV_TIMEOUT + 50);
    g_udp_close_flag = 0;
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void module_udp_source_clean(void)
{
    if (g_udp_recv_flag) {
        g_udp_close_flag = 1;
        aos_sem_wait(&g_udp_close_sem, MAX_UDP_RECV_TIMEOUT + 50);
        g_udp_close_flag = 0;
    }
}

void module_udp_register(void)
{
    duk_context *ctx = be_get_context();

    if (!g_udp_close_sem) {
        if (aos_sem_new(&g_udp_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create udp sem fail");
            return;
        }
    }

    amp_module_free_register(module_udp_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("createSocket", native_udp_create_socket, 0);
    AMP_ADD_FUNCTION("bind",         native_udp_bind, 2);
    AMP_ADD_FUNCTION("sendto",       native_udp_sendto, 3);
    AMP_ADD_FUNCTION("recvfrom",     native_udp_recvfrom, 2);
    AMP_ADD_FUNCTION("close",        native_udp_close_socket, 1);

    duk_put_prop_string(ctx, -2, "UDP");
}
