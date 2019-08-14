/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/* #define LOG_NDEBUG 0 */
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "jse_port.h"
#include "jse_task.h"
#include "be_utils.h"
#include "bone_engine_inl.h"

/* #define DEBUG_UDP_ADDON */
#ifdef DEBUG_UDP_ADDON
#define DEBUG_HEX_DUMP(str, buff, len) hexdump(str, buff, len)
#else
#define DEBUG_HEX_DUMP(str, buff, len)
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#define MAX_UDP_RECV_LEN 256

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int port;
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
    int js_cb_ref;
} udp_recv_param_t;

typedef struct {
    char buf[MAX_UDP_RECV_LEN];
    int recv_len;
    char src_ip[INET_ADDRSTRLEN];
    unsigned short src_port;
    int js_cb_ref;
} udp_recv_notify_param_t;

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

    sock_id = socket(AF_INET, SOCK_DGRAM, 0);
    jse_debug("sock_id = %d\n", sock_id);
    duk_push_int(ctx, sock_id);
    return 1;
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
    const char *ip;

    if (!duk_is_number(ctx, 0) || !duk_is_string(ctx, 1) ||
        !duk_is_number(ctx, 2)) {
        jse_warn("parameter must be (number, string, number)\n");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id < 0) {
        jse_error("socket id[%d] error\n", sock_id);
        goto out;
    }

    ip = duk_get_string(ctx, 1);

    port = duk_get_int(ctx, 2);
    if (port <= 0) {
        jse_error("port[%d] error\n", port);
        goto out;
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

    jse_debug("udp bind socket id=%d, ip=%s, port=%d\n", sock_id, ip, port);
    ret = bind(sock_id, (struct sockaddr *)&addr_in, sizeof(addr_in));

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void udp_send_notify(void *pdata)
{
    udp_send_notify_param_t *p = (udp_send_notify_param_t *)pdata;
    duk_context *ctx           = bone_engine_get_context();
    bone_engine_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->ret);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    bone_engine_unref(ctx, p->js_cb_ref);
    jse_free(p);
}

/*************************************************************************************
 * Function:    udp_send_routin
 * Description: create a task for blocking sendto call
 * Called by:
 **************************************************************************************/
static void udp_send_routine(void *arg)
{
    int ret                      = -1;
    udp_send_param_t *send_param = (udp_send_param_t *)arg;
    int sock_id;
    udp_options_t udp_options;
    struct sockaddr_in addr_in;
    udp_send_notify_param_t *p;
    duk_context *ctx = bone_engine_get_context();

    sock_id = send_param->sock_id;
    memcpy(&udp_options, &(send_param->options), sizeof(udp_options_t));

    bzero(&addr_in, sizeof(struct sockaddr_in));
    addr_in.sin_family      = AF_INET;
    addr_in.sin_addr.s_addr = inet_addr(udp_options.ip);
    addr_in.sin_port        = htons(udp_options.port);

    if (strcmp(udp_options.ip, "255.255.255.255") == 0) {
        int broadcast = 1;
        if (setsockopt(sock_id, SOL_SOCKET, SO_BROADCAST, &broadcast,
                       sizeof(broadcast)) < 0) {
            jse_error("setsockopt failed\n");
            bone_engine_unref(ctx, send_param->js_cb_ref);
            goto out;
        }
    }

    ret = sendto(sock_id, send_param->msg, send_param->msg_len, 0,
                 (struct sockaddr *)&addr_in, sizeof(addr_in));
    p   = jse_calloc(1, sizeof(udp_send_notify_param_t));
    if (!p) {
        jse_warn("allocate memory failed\n");
        bone_engine_unref(ctx, send_param->js_cb_ref);
        goto out;
    }

    p->js_cb_ref = send_param->js_cb_ref;
    p->ret       = ret;
    be_jse_task_schedule_call(udp_send_notify, p);

out:
    jse_free(send_param->msg);
    jse_free(send_param);
    jse_osal_delete_task(NULL);
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
        !duk_is_array(ctx, 2) || !duk_is_function(ctx, 3)) {
        jse_warn("parameter must be (number, object, array, function)\n");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id <= 0) {
        jse_warn("socket id[%d] is invalid\n", sock_id);
        goto out;
    }

    memset(&options, 0, sizeof(udp_options_t));
    duk_get_prop_string(ctx, 1, "port");
    if (!duk_is_number(ctx, -1)) {
        jse_warn("port not specify\n");
        duk_pop(ctx);
        goto out;
    }
    options.port = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 1, "ip");
    if (!duk_is_string(ctx, -1)) {
        jse_warn("ip not specify\n");
        duk_pop(ctx);
        goto out;
    }
    strncpy(options.ip, duk_get_string(ctx, -1), sizeof(options.ip) - 1);
    duk_pop(ctx);

    msg_len = duk_get_length(ctx, 2);
    msg     = (char *)jse_malloc(msg_len);
    if (!msg) {
        jse_warn("allocate memory failed\n");
        goto out;
    }
    for (i = 0; i < msg_len; i++) {
        duk_get_prop_index(ctx, 2, i);
        msg[i] = (char)duk_get_int(ctx, -1);
        duk_pop(ctx);
    }
    DEBUG_HEX_DUMP("send msg:", msg, msg_len);

    udp_send_param_t *send_param =
        (udp_send_param_t *)jse_malloc(sizeof(*send_param));
    if (!send_param) {
        jse_error("allocate memory failed\n");
        jse_free(msg);
        goto out;
    }
    send_param->sock_id = sock_id;
    duk_dup(ctx, 3);
    send_param->js_cb_ref = bone_engine_ref(ctx);
    send_param->msg       = msg;
    send_param->msg_len   = msg_len;
    memcpy(&(send_param->options), &options, sizeof(udp_options_t));

    ret = jse_osal_create_task("udp_send", udp_send_routine, send_param, 4096,
                              ADDON_TSK_PRIORRITY, NULL);

out:
    duk_push_int(ctx, ret);
    return 1;
}

static void udp_recv_notify(void *pdata)
{
    int i                      = 0;
    udp_recv_notify_param_t *p = (udp_recv_notify_param_t *)pdata;
    duk_context *ctx           = bone_engine_get_context();
    bone_engine_push_ref(ctx, p->js_cb_ref);
    duk_push_int(ctx, p->recv_len);
    int arr_idx = duk_push_array(ctx);
    duk_push_string(ctx, p->src_ip);
    duk_push_uint(ctx, p->src_port);
    for (i = 0; i < p->recv_len; i++) {
        duk_push_int(ctx, p->buf[i]);
        duk_put_prop_index(ctx, arr_idx, i);
    }
    duk_pcall(ctx, 4);
    duk_pop(ctx);
    bone_engine_unref(ctx, p->js_cb_ref);
    jse_free(p);
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
    struct sockaddr_in srcaddr;
    socklen_t addr_len;

    sock_id  = recv_param->sock_id;
    addr_len = sizeof(struct sockaddr_in);

    udp_recv_notify_param_t *p = jse_calloc(1, sizeof(*p));
    if (!p) {
        jse_warn("allocate memory failed\n");
        duk_context *ctx = bone_engine_get_context();
        bone_engine_unref(ctx, recv_param->js_cb_ref);
        goto out;
    }

    bzero(&srcaddr, sizeof(struct sockaddr_in));
    p->recv_len = recvfrom(sock_id, p->buf, sizeof(p->buf), 0,
                           (struct sockaddr *)&srcaddr, &addr_len);
    DEBUG_HEX_DUMP("udp recv:", p->buf, p->recv_len);

    strcpy(p->src_ip, inet_ntoa(srcaddr.sin_addr));
    p->src_port  = ntohs(srcaddr.sin_port);
    p->js_cb_ref = recv_param->js_cb_ref;

    be_jse_task_schedule_call(udp_recv_notify, p);

out:
    jse_free(recv_param);
    jse_osal_delete_task(NULL);
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
    udp_recv_param_t *recv_param;

    if (!duk_is_number(ctx, 0) || !duk_is_function(ctx, 1)) {
        jse_warn("parameter must be number and function\n");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id < 0) {
        jse_warn("socket id[%d] is invalid\n", sock_id);
        goto out;
    }

    jse_debug("sock_id: %d\n", sock_id);
    recv_param = (udp_recv_param_t *)jse_calloc(1, sizeof(*recv_param));
    if (!recv_param) {
        jse_warn("allocate memory failed\n");
        goto out;
    }

    recv_param->sock_id = sock_id;
    duk_dup(ctx, 1);
    recv_param->js_cb_ref = bone_engine_ref(ctx);

    ret = jse_osal_create_task("udp_recv", udp_recv_routine, recv_param, 4096,
                              ADDON_TSK_PRIORRITY, NULL);

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
        jse_warn("parameter must be number\n");
        goto out;
    }

    sock_id = duk_get_int(ctx, 0);
    if (sock_id <= 0) {
        jse_warn("socket id[%d] is invalid", sock_id);
        goto out;
    }
    ret = close(sock_id);

out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_udp_register(void)
{
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_udp_create_socket, 0);
    duk_put_prop_string(ctx, -2, "createSocket");

    duk_push_c_function(ctx, native_udp_bind, 3);
    duk_put_prop_string(ctx, -2, "bind");

    duk_push_c_function(ctx, native_udp_sendto, 4);
    duk_put_prop_string(ctx, -2, "send");

    duk_push_c_function(ctx, native_udp_recvfrom, 2);
    duk_put_prop_string(ctx, -2, "recv");

    duk_push_c_function(ctx, native_udp_close_socket, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "UDP");
}
