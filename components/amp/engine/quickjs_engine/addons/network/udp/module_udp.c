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
#include "quickjs.h"
#include "quickjs_addon_common.h"

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
    JSValue js_cb_ref;
} udp_send_param_t;

typedef struct {
    int ret;
    JSValue js_cb_ref;
} udp_send_notify_param_t;

typedef struct {
    int sock_id;
    udp_options_t options;
    JSValue js_cb_ref;
} udp_recv_param_t;

typedef struct {
    char buf[MAX_UDP_RECV_LEN];
    int recv_len;
    char src_ip[INET_ADDRSTRLEN];
    unsigned short src_port;
    JSValue js_cb_ref;
} udp_recv_notify_param_t;

/* udp socket close request flag*/
static char volatile g_udp_close_flag = 0;
static char g_udp_recv_flag = 0;

static aos_sem_t g_udp_close_sem = NULL;

static JSClassID js_udp_class_id;

/*************************************************************************************
 * Function:    native_udp_create_socket
 * Description: js native addon for UDP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static JSValue native_udp_create_socket(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int sock_id = 0;

    sock_id = aos_udp_socket_create();
    if (sock_id < 0) {
        amp_warn(MOD_STR, "create socket error!");
        goto out;
    }
    amp_debug(MOD_STR, "sock_id = %d", sock_id);

out:
    return JS_NewInt32(ctx, sock_id);
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
static JSValue native_udp_bind(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret  = -1;
    int port = 0;
    int sock_id;

    if (!JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1])) {
        amp_warn(MOD_STR, "parameter must be (number, number)");
        goto out;
    }

    JS_ToInt32(ctx, &sock_id, argv[0]);
    if (sock_id < 0) {
        amp_error(MOD_STR, "socket id[%d] error", sock_id);
        goto out;
    }

    JS_ToInt32(ctx, &port, argv[1]);
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

out:
    return JS_NewInt32(ctx, ret);
}

static void udp_send_notify(void *pdata)
{
    udp_send_notify_param_t *notify = (udp_send_notify_param_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args;

    args = JS_NewInt32(ctx, notify->ret);

    JSValue val = JS_Call(ctx, notify->js_cb_ref, JS_UNDEFINED, 1, &args);

    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
        amp_error(MOD_STR, "udp_send_notify, call error");
    }

    JS_FreeValue(ctx, args);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, notify->js_cb_ref);
    amp_free(notify);
}

/*************************************************************************************
 * Function:    udp_send_routin
 * Description: create a task for blocking sendto call
 * Called by:
 **************************************************************************************/
static int udp_send_routine(udp_send_param_t *send_param)
{
    int ret = -1;
    int sock_id;
    udp_options_t udp_options;
    udp_send_notify_param_t *p;

    JSContext *ctx = js_get_context();

    sock_id = send_param->sock_id;

    ret = aos_udp_sendto(sock_id, &(send_param->options), send_param->msg, send_param->msg_len, 0);

    p   = amp_calloc(1, sizeof(udp_send_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    p->js_cb_ref = send_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(udp_send_notify, p);
    ret = 0;

out:
    amp_free(send_param->msg);
    amp_free(send_param);
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

static JSValue native_udp_sendto(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    udp_options_t options;
    int sock_id;
    int i;
    int msg_len;
    char *msg;
    const char *buf;
    char *str;

    if (!JS_IsNumber(argv[0]) || !JS_IsObject(argv[1]) ||
            !JS_IsFunction(ctx, argv[2])) {
        amp_warn(MOD_STR, "parameter must be (number, string, function)");
        goto out;
    }

    JS_ToInt32(ctx, &sock_id, argv[0]);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }
    memset(&options, 0, sizeof(udp_options_t));

    /* options.port */
    JSValue j_port = JS_GetPropertyStr(ctx, argv[1], "port");
    if (JS_IsException(j_port) || JS_IsUndefined(j_port)) {
        amp_warn(MOD_STR, "port not specify");
        goto out;
    }

    JS_ToInt32(ctx, &options.port, j_port);
    JS_FreeValue(ctx, j_port);

    /* options.address */
    JSValue j_host = JS_GetPropertyStr(ctx, argv[1], "address");
    if (JS_IsException(j_host) || JS_IsUndefined(j_host)) {
        amp_warn(MOD_STR, "ip not specify");
        goto out;
    }

    str = JS_ToCString(ctx, j_host);
    memset(options.ip, 0, INET_ADDRSTRLEN);
    memcpy(options.ip, str, strlen(str));

    JS_FreeValue(ctx, j_host);

    /* options.message */

    JSValue j_message = JS_GetPropertyStr(ctx, argv[1], "message");
    if (JS_IsException(j_message) || JS_IsUndefined(j_message)) {
        amp_warn(MOD_STR, "message not specify");
        goto out;
    }

    if (JS_IsString(j_message)) {
        buf = JS_ToCString(ctx, j_message);
        msg_len = strlen(buf);
    } else {
        buf = JS_GetArrayBuffer(ctx, &msg_len, j_message);
        if (!buf) {
            amp_warn(MOD_STR, "parameter buffer is invalid, size: %d", msg_len);
            goto out;
        }
    }

    msg = (char *)amp_malloc(msg_len + 1);
    if (!msg) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }
    memcpy(msg, buf, msg_len);
    msg[msg_len] = 0;

    if (JS_IsString(j_message)) {
        JS_FreeCString(ctx, buf);
    } else {
        JS_FreeValue(ctx, buf);
    }

    amp_debug(MOD_STR, "msg is:%s, length is: %d", msg, msg_len);
    udp_send_param_t *send_param = (udp_send_param_t *)amp_malloc(sizeof(*send_param));
    if (!send_param) {
        amp_error(MOD_STR, "allocate memory failed");
        amp_free(msg);
        goto out;
    }

    send_param->sock_id = sock_id;
    send_param->js_cb_ref = JS_DupValue(ctx, argv[2]);
    send_param->msg       = msg;
    send_param->msg_len   = msg_len;
    memcpy(&(send_param->options), &options, sizeof(udp_options_t));

    amp_debug(MOD_STR, "sockid:%d ip:%s port:%d msg:%s msg_len:%d", sock_id, options.ip, options.port, msg, msg_len);

    udp_send_routine(send_param);

out:
    return JS_NewInt32(ctx, ret);
}

static void udp_recv_notify(void *pdata)
{
    int i = 0;
    udp_recv_notify_param_t *notify = (udp_recv_notify_param_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args[3];
    JSValue host, port;

    if (notify->recv_len > 0) {
        args[0] = JS_NewArrayBufferCopy(ctx, notify->buf, notify->recv_len);
        args[1] = JS_NewObject(ctx);

        host = JS_NewStringLen(ctx, notify->src_ip, INET_ADDRSTRLEN);
        port = JS_NewInt32(ctx, notify->src_port);

        JS_SetPropertyStr(ctx, args[1], "host", host);
        JS_SetPropertyStr(ctx, args[1], "port", port);

    }
    args[2] = JS_NewInt32(ctx, notify->recv_len);

    JSValue val = JS_Call(ctx, notify->js_cb_ref, JS_UNDEFINED, 3, args);

    JS_FreeValue(ctx, args[2]);
    if (notify->recv_len > 0) {
        JS_FreeValue(ctx, port);
        JS_FreeCString(ctx, host);
        JS_FreeValue(ctx, args[0]);
        JS_FreeValue(ctx, args[1]);
    }

    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
        amp_error(MOD_STR, "udp_send_notify, call error");
    }
    JS_FreeValue(ctx, val);
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
    aos_networkAddr addr_info;
    udp_recv_notify_param_t *p;
    JSContext *ctx = js_get_context();

    sock_id = recv_param->sock_id;
    p = amp_calloc(1, sizeof(udp_recv_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    g_udp_recv_flag = 1;
    while (1) {
        p->recv_len = aos_udp_recvfrom(sock_id, &addr_info, p->buf, sizeof(p->buf), MAX_UDP_RECV_TIMEOUT);
        if (p->recv_len > 0) {
            memcpy(p->src_ip, addr_info.addr, INET_ADDRSTRLEN);
            p->src_port  = addr_info.port;
            p->js_cb_ref = recv_param->js_cb_ref;
            amp_task_schedule_call(udp_recv_notify, p);
        }

        if (p->recv_len < 0) {
            // connection closed
            amp_error(MOD_STR, "connection closed:%d", p->recv_len);
            break;
        }

        if (g_udp_close_flag) {
            amp_warn(MOD_STR, "close udp connect");
            break;
        }
    }

    aos_udp_close_without_connect(sock_id);

out:

    JS_FreeValue(ctx, recv_param->js_cb_ref);
    amp_free(recv_param);
    if (p) {
        amp_free(p);
    }

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
static JSValue native_udp_recvfrom(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    int sock_id = 0;
    aos_task_t udp_recv_task;
    udp_recv_param_t *recv_param;

    if (!JS_IsNumber(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be number and function");
        goto out;
    }

    JS_ToInt32(ctx, &sock_id, argv[0]);
    if (sock_id < 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }

    amp_debug(MOD_STR, "sock_id: %d", sock_id);
    recv_param = (udp_recv_param_t *)amp_calloc(1, sizeof(*recv_param));
    if (!recv_param) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    recv_param->sock_id = sock_id;
    recv_param->js_cb_ref = JS_DupValue(ctx, argv[1]);

    ret = aos_task_new_ext(&udp_recv_task, "amp udp recv task", udp_recv_routine, recv_param, 1024 * 4, ADDON_TSK_PRIORRITY);
    if (ret != 0) {
        amp_debug(MOD_STR, "udp recv task create faild");
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
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
static JSValue native_udp_close_socket(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    int sock_id = 0;

    if ((argc < 1) || (!JS_IsNumber(argv[0]))) {
        amp_warn(MOD_STR, "parameter must be number");
        goto out;
    }

    JS_ToInt32(ctx, &sock_id, argv[0]);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }

    g_udp_close_flag = 1;

    aos_sem_wait(&g_udp_close_sem, MAX_UDP_RECV_TIMEOUT + 50);

    g_udp_close_flag = 0;
    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static void module_udp_source_clean(void)
{
    if (g_udp_recv_flag) {
        g_udp_close_flag = 1;
        aos_sem_wait(&g_udp_close_sem, MAX_UDP_RECV_TIMEOUT + 50);
        g_udp_close_flag = 0;
    }
}

static JSClassDef js_udp_class = {
    "UDP",
};

static const JSCFunctionListEntry js_udp_funcs[] = {
    JS_CFUNC_DEF("createSocket", 0, native_udp_create_socket),
    JS_CFUNC_DEF("bind", 2, native_udp_bind),
    JS_CFUNC_DEF("sendto", 3, native_udp_sendto),
    JS_CFUNC_DEF("recvfrom", 2, native_udp_recvfrom),
    JS_CFUNC_DEF("close", 1, native_udp_close_socket)
};

static int js_udp_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_udp_class);

    JS_NewClass(JS_GetRuntime(ctx), js_udp_class_id, &js_udp_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_udp_funcs, countof(js_udp_funcs));
    JS_SetClassProto(ctx, js_udp_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_udp_funcs, countof(js_udp_funcs));
}

JSModuleDef *js_init_module_udp(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_udp_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_udp_funcs, countof(js_udp_funcs));
    return m;
}


void module_udp_register(void)
{
    amp_debug(MOD_STR, "module_udp_register");
    JSContext *ctx = js_get_context();

    if (!g_udp_close_sem) {
        if (aos_sem_new(&g_udp_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create udp sem fail");
            return;
        }
    }

    amp_module_free_register(module_udp_source_clean);

    js_init_module_udp(ctx, "UDP");
}
