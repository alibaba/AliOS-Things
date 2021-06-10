/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "aos_tcp.h"
#include "be_inl.h"

#define MOD_STR "TCP"
#define MAX_TCP_RECV_LEN 256
#define MAX_TCP_RECV_TIMEOUT 200

typedef struct {
    int sock_id;
    char *msg;
    int msg_len;
    int js_cb_ref;
} tcp_send_param_t;

typedef struct {
    int ret;
    int js_cb_ref;
} tcp_send_notify_param_t;

typedef struct {
    int sock_id;
    int js_cb_ref;
} tcp_recv_param_t;

typedef struct {
    char *host;
    int port;
    int js_cb_ref;
} tcp_create_param_t;

typedef struct {
    int ret;
    int js_cb_ref;
} tcp_create_notify_param_t;

typedef struct {
    char buf[MAX_TCP_RECV_LEN];
    int recv_len;
    int js_cb_ref;
} tcp_recv_notify_param_t;

static char g_tcp_close_flag = 0;
static char g_tcp_recv_flag = 0;
static aos_sem_t g_tcp_close_sem = NULL;

static void tcp_create_notify(void *pdata)
{
    tcp_create_notify_param_t *p = (tcp_create_notify_param_t *)pdata;
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

static int tcp_create_routine(tcp_create_param_t *create_param)
{
    int ret                      = -1;
    tcp_create_notify_param_t *p;

    duk_context *ctx = be_get_context();

    p = aos_calloc(1, sizeof(tcp_create_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        be_unref(ctx, create_param->js_cb_ref);
        goto out;
    }

    ret = aos_tcp_establish(create_param->host, create_param->port);
    if (ret < 0) {
        amp_warn(MOD_STR, "tcp establish failed");
    }

    amp_debug(MOD_STR, "sock_id = %d", ret);

    p->js_cb_ref = create_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(tcp_create_notify, p);

out:

    return ret;
}

/*************************************************************************************
 * Function:    native_udp_create_socket
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static duk_ret_t native_tcp_create_socket(duk_context *ctx)
{
    int err;
    int ret  = -1;
    int port = 0;
    const char *host;
    tcp_create_param_t *create_param = NULL;

    if (!duk_is_object(ctx, 0) || !duk_is_function(ctx, 1)) {
        amp_warn(MOD_STR, "parameter must be (object, function)");
        goto out;
    }

    /* get device certificate */
    duk_get_prop_string(ctx, 0, "host");
    duk_get_prop_string(ctx, 0, "port");

    if (!duk_is_string(ctx, -2) || !duk_is_number(ctx, -1))
    {
        amp_warn(MOD_STR,
            "Parameter 1 must be an object like {host: string, "
            "port: uint}");
        err = -2;
        goto out;
    }

    host = duk_get_string(ctx, -2);
    port = duk_get_number(ctx, -1);

    amp_debug(MOD_STR, "host: %s, port: %d", host, port);

    create_param = (tcp_create_param_t *)aos_malloc(sizeof(*create_param));
    if (!create_param) {
        amp_error(MOD_STR, "allocate memory failed");
        goto out;
    }

    duk_dup(ctx, 1);
    create_param->host = host;
    create_param->port = port;
    create_param->js_cb_ref = be_ref(ctx);

    ret = tcp_create_routine(create_param);
    if (ret < 0) {
        amp_warn(MOD_STR, "tcp create socket failed");
        goto out;
    }

out:
    if(create_param) {
        aos_free(create_param);
    }
    duk_push_int(ctx, ret);
    return 1;

}

static void tcp_send_notify(void *pdata)
{
    tcp_send_notify_param_t *p = (tcp_send_notify_param_t *)pdata;
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
static int tcp_send_routine(tcp_send_param_t *send_param)
{
    int ret                      = -1;
    tcp_send_notify_param_t *p;
    int sock_id;

    duk_context *ctx = be_get_context();

    sock_id = send_param->sock_id;

    ret = aos_tcp_write(sock_id, send_param->msg, send_param->msg_len, 0);
    p   = aos_calloc(1, sizeof(tcp_send_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        be_unref(ctx, send_param->js_cb_ref);
        goto out;
    }

    p->js_cb_ref = send_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(tcp_send_notify, p);
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

static duk_ret_t native_tcp_send(duk_context *ctx)
{
    int ret = -1;
    int sock_id;
    int i;
    int msg_len;
    char *msg;

    if (!duk_is_number(ctx, 0) || !duk_is_array(ctx, 1) ||
        !duk_is_function(ctx, 2)) {
        amp_warn(MOD_STR, "parameter must be (number, string, function)");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }

    msg_len = duk_get_length(ctx, 1);
    msg     = (char *)aos_malloc(msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    for (i = 0; i < msg_len; i++) {
        duk_get_prop_index(ctx, 1, i);
        msg[i] = duk_get_int(ctx, -1);
        duk_pop(ctx);
    }
    msg[msg_len] = 0;

    // const char *send_buf = duk_get_string(ctx, 1);
    // msg_len = strlen(send_buf);
    // msg     = (char *)aos_malloc(msg_len);
    // if (msg == NULL) {
    //     amp_warn(MOD_STR, "allocate memory failed");
    //     goto out;
    // }

    // strncpy(msg, send_buf, msg_len);
    amp_debug(MOD_STR, "msg is:%s, length is: %d", msg, msg_len);
    tcp_send_param_t *send_param =
        (tcp_send_param_t *)aos_malloc(sizeof(*send_param));
    if (!send_param) {
        amp_error(MOD_STR, "allocate memory failed");
        aos_free(msg);
        goto out;
    }
    send_param->sock_id   = sock_id;
    send_param->msg       = msg;
    send_param->msg_len   = msg_len;
    duk_dup(ctx, 2);
    send_param->js_cb_ref = be_ref(ctx);

    tcp_send_routine(send_param);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void tcp_recv_notify(void *pdata)
{
    int i                      = 0;
    tcp_recv_notify_param_t *p = (tcp_recv_notify_param_t *)pdata;
    duk_context *ctx           = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->recv_len);
    int arr_idx = duk_push_array(ctx);
    if(p->recv_len > 0) {
        for (i = 0; i < p->recv_len; i++) {
            duk_push_int(ctx, p->buf[i]);
            duk_put_prop_index(ctx, arr_idx, i);
        }
    }
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
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
static void tcp_recv_routine(void *arg)
{
    tcp_recv_param_t *recv_param = (tcp_recv_param_t *)arg;
    int sock_id;

    g_tcp_recv_flag = 1;
    sock_id  = recv_param->sock_id;
    tcp_recv_notify_param_t *p = aos_calloc(1, sizeof(*p));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        duk_context *ctx = be_get_context();
        be_unref(ctx, recv_param->js_cb_ref);
        goto out;
    }

    while(1) {
        p->recv_len = aos_tcp_read(sock_id, p->buf, sizeof(p->buf), MAX_TCP_RECV_TIMEOUT);
        p->js_cb_ref = recv_param->js_cb_ref;
        if(p->recv_len != 0) {
            amp_task_schedule_call(tcp_recv_notify, p);
        }

        if(p->recv_len < 0) {
            // connection closed
            amp_error(MOD_STR, "connection closed:%d", p->recv_len);
            break;
        }

        if (g_tcp_close_flag) {
            duk_context *ctx = be_get_context();
            be_unref(ctx, recv_param->js_cb_ref);
            aos_free(p);
            break;
        }
    }
    aos_tcp_destroy(sock_id);

out:
    aos_free(recv_param);
    g_tcp_recv_flag = 0;
    aos_sem_signal(&g_tcp_close_sem);
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
static duk_ret_t native_tcp_receive(duk_context *ctx)
{
    int ret     = -1;
    int sock_id = 0;
    aos_task_t tcp_recv_task;
    tcp_recv_param_t *recv_param;

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
    recv_param = (tcp_recv_param_t *)aos_calloc(1, sizeof(*recv_param));
    if (!recv_param) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    recv_param->sock_id = sock_id;
    duk_dup(ctx, 1);
    recv_param->js_cb_ref = be_ref(ctx);

    ret = aos_task_new_ext(&tcp_recv_task, "amp tcp recv task", tcp_recv_routine, recv_param, 1024 * 4, ADDON_TSK_PRIORRITY);
    if (ret != 0) {
        amp_warn(MOD_STR, "tcp recv task error");
        goto out;
    }

out:
    duk_push_int(ctx, ret);
    return 1;
}

/*************************************************************************************
 * Function:    native_tcp_close
 * Description: js native addon for
 *            UDP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when UDP.close call ok
 *            return error number UDP.close call fail
 **************************************************************************************/
static duk_ret_t native_tcp_close(duk_context *ctx)
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
    g_tcp_close_flag = 1;
    aos_sem_wait(&g_tcp_close_sem, MAX_TCP_RECV_TIMEOUT + 50);
    g_tcp_close_flag = 0;
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void module_tcp_source_clean(void)
{
    if (g_tcp_recv_flag) {
        g_tcp_close_flag = 1;
        aos_sem_wait(&g_tcp_close_sem, MAX_TCP_RECV_TIMEOUT + 50);
        g_tcp_close_flag = 0;
    }
}

void module_tcp_register(void)
{
    duk_context *ctx = be_get_context();

    if (!g_tcp_close_sem) {
        if (aos_sem_new(&g_tcp_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create tcp sem fail");
            return;
        }
    }

    amp_module_free_register(module_tcp_source_clean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("createSocket", native_tcp_create_socket, 2);
    AMP_ADD_FUNCTION("send",         native_tcp_send, 3);
    AMP_ADD_FUNCTION("recv",         native_tcp_receive, 2);
    AMP_ADD_FUNCTION("close",        native_tcp_close, 1);

    duk_put_prop_string(ctx, -2, "TCP");
}
