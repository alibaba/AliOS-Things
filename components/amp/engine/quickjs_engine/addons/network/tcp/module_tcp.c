/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_platform.h"
#include "amp_defines.h"
#include "amp_task.h"
#include "amp_memory.h"
#include "aos_system.h"
#include "aos_tcp.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "TCP"
#define MAX_TCP_RECV_LEN 256
#define MAX_TCP_RECV_TIMEOUT 200

typedef struct {
    int sock_id;
    char *msg;
    int msg_len;
    JSValue js_cb_ref;
} tcp_send_param_t;

typedef struct {
    int ret;
    JSValue js_cb_ref;
} tcp_send_notify_param_t;

typedef struct {
    int sock_id;
    JSValue js_cb_ref;
} tcp_recv_param_t;

typedef struct {
    char *host;
    int port;
    JSValue js_cb_ref;
} tcp_create_param_t;

typedef struct {
    int ret;
    JSValue js_cb_ref;
} tcp_create_notify_param_t;

typedef struct {
    char buf[MAX_TCP_RECV_LEN];
    int recv_len;
    JSValue js_cb_ref;
} tcp_recv_notify_param_t;

static char g_tcp_close_flag = 0;
static char g_tcp_recv_flag = 0;
static aos_sem_t g_tcp_close_sem = NULL;
static JSClassID js_tcp_class_id;

static void tcp_create_notify(void *pdata)
{
    tcp_create_notify_param_t *p = (tcp_create_notify_param_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args = JS_NewInt32(ctx, p->ret);
    JSValue val = JS_Call(ctx, p->js_cb_ref, JS_UNDEFINED, 1, &args);
    JS_FreeValue(ctx, args);
    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
    }
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, p->js_cb_ref);
    amp_free(p);
}

static int tcp_create_routine(tcp_create_param_t *create_param)
{
    int ret = -1;

    JSContext *ctx = js_get_context();
    tcp_create_notify_param_t *p = amp_calloc(1, sizeof(tcp_create_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        JS_FreeValue(ctx, create_param->js_cb_ref);
        goto out;
    }

    ret = aos_tcp_establish(create_param->host, create_param->port);
    if (ret < 0) {
        amp_warn(MOD_STR, "tcp establish failed");
        amp_free(p);
        JS_FreeValue(ctx, create_param->js_cb_ref);
        goto out;
    }

    amp_debug(MOD_STR, "sock_id = %d", ret);
    p->js_cb_ref = create_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(tcp_create_notify, p);

out:
    JS_FreeCString(ctx, create_param->host);
    amp_free(create_param);
    return ret;
}

/*************************************************************************************
 * Function:    native_udp_create_socket
 * Description: js native addon for TCP.createSocket();
 * Called by:   js api
 * Input:       none
 * Output:      return socket fd when create socket success,
 *            return error number when create socket fail
 **************************************************************************************/
static JSValue native_tcp_create_socket(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int err;
    int ret  = -1;
    int port = 0;
    char *host = NULL;
    tcp_create_param_t *create_param = NULL;

    amp_debug(MOD_STR, "native_tcp_create_socket");
    if (!JS_IsObject(argv[0]) || !JS_IsFunction(ctx, argv[1])) {
        amp_warn(MOD_STR, "parameter must be (object, function)");
        goto out;
    }

    JSValue j_host = JS_GetPropertyStr(ctx, argv[0], "host");
    host = JS_ToCString(ctx, j_host);
    JS_FreeValue(ctx, j_host);

    JSValue j_port = JS_GetPropertyStr(ctx, argv[0], "port");
    JS_ToInt32(ctx, &port, j_port);
    JS_FreeValue(ctx, j_port);

    amp_debug(MOD_STR, "host: %s, port: %d", host, port);
    create_param = (tcp_create_param_t *)amp_malloc(sizeof(*create_param));
    if (!create_param) {
        amp_error(MOD_STR, "allocate memory failed");
        JS_FreeCString(ctx, host);
        goto out;
    }

    create_param->host = host;
    create_param->port = port;
    create_param->js_cb_ref = JS_DupValue(ctx, argv[1]);

    ret = tcp_create_routine(create_param);
    if (ret < 0) {
        amp_warn(MOD_STR, "tcp create socket failed");
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
}

static void tcp_send_notify(void *pdata)
{
    tcp_send_notify_param_t *p = (tcp_send_notify_param_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args = JS_NewInt32(ctx, p->ret);
    JSValue val = JS_Call(ctx, p->js_cb_ref, JS_UNDEFINED, 1, &args);
    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
    }
    JS_FreeValue(ctx, args);
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, p->js_cb_ref);
    amp_free(p);
}

/*************************************************************************************
 * Function:    udp_send_routin
 * Description: create a task for blocking sendto call
 * Called by:
 **************************************************************************************/
static int tcp_send_routine(tcp_send_param_t *send_param)
{
    JSContext *ctx = js_get_context();
    int sock_id = send_param->sock_id;
    int ret = aos_tcp_write(sock_id, send_param->msg, send_param->msg_len, 0);
    if (ret < 0) {
        JS_FreeValue(ctx, send_param->js_cb_ref);
        goto out;
    }

    tcp_send_notify_param_t *p = amp_calloc(1, sizeof(tcp_send_notify_param_t));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        JS_FreeValue(ctx, send_param->js_cb_ref);
        goto out;
    }

    p->js_cb_ref = send_param->js_cb_ref;
    p->ret       = ret;
    amp_task_schedule_call(tcp_send_notify, p);
    ret = 0;

out:
    JS_FreeCString(ctx, send_param->msg);
    amp_free(send_param);
    return ret;
}

/*************************************************************************************
 * Function:    native_udp_sendto
 * Description: js native addon for
 *TCP.send(sock_id,option,buffer_array,function(ret){}) Called by:   js api
 * Input:       sock_id: interger
 *            options: is a object include options.ip and options.port
 *            buffer_array: is a array which include message to send
 *            function(ret): is the callback function which has a ret input
 *param Output:      return send msg length when send success return error
 *number when send fail
 **************************************************************************************/

static JSValue native_tcp_send(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
    int sock_id;
    int i;
    int msg_len;
    char *msg;

    if (!JS_IsNumber(argv[0]) || !JS_IsString(argv[1]) ||
            !JS_IsFunction(ctx, argv[2])) {
        amp_warn(MOD_STR, "parameter must be (number, string, function)");
        goto out;
    }

    JS_ToInt32(ctx, &sock_id, argv[0]);
    if (sock_id <= 0) {
        amp_warn(MOD_STR, "socket id[%d] is invalid", sock_id);
        goto out;
    }

    msg = JS_ToCString(ctx, argv[1]);
    msg_len = strlen(msg);
    amp_debug(MOD_STR, "msg is:%s, length is: %d", msg, msg_len);

    tcp_send_param_t *send_param = (tcp_send_param_t *)amp_malloc(sizeof(*send_param));
    if (!send_param) {
        amp_error(MOD_STR, "allocate memory failed");
        JS_FreeCString(ctx, msg);
        goto out;
    }
    send_param->sock_id   = sock_id;
    send_param->msg       = msg;
    send_param->msg_len   = msg_len;
    send_param->js_cb_ref = JS_DupValue(ctx, argv[2]);
    tcp_send_routine(send_param);

out:
    return JS_NewInt32(ctx, ret);
}

static void tcp_recv_notify(void *pdata)
{
    int i = 0;
    tcp_recv_notify_param_t *p = (tcp_recv_notify_param_t *)pdata;
    JSContext *ctx = js_get_context();

    JSValue args[2];
    args[0] = JS_NewInt32(ctx, p->recv_len);
    args[1] = JS_NewStringLen(ctx, p->buf, p->recv_len);

    JSValue val = JS_Call(ctx, p->js_cb_ref, JS_UNDEFINED, 2, args);
    JS_FreeValue(ctx, args[0]);
    JS_FreeValue(ctx, args[1]);
    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
    }
    JS_FreeValue(ctx, val);
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
    JSContext *ctx = js_get_context();

    g_tcp_recv_flag = 1;
    sock_id  = recv_param->sock_id;
    tcp_recv_notify_param_t *p = amp_calloc(1, sizeof(*p));
    if (!p) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    p->js_cb_ref = recv_param->js_cb_ref;

    while (1) {
        p->recv_len = aos_tcp_read(sock_id, p->buf, sizeof(p->buf), MAX_TCP_RECV_TIMEOUT);
        if (p->recv_len > 0) {
            amp_task_schedule_call(tcp_recv_notify, p);
        }

        if (p->recv_len < 0) {
            amp_warn(MOD_STR, "connection closed: %d", p->recv_len);
            break;
        }

        if (g_tcp_close_flag) {
            amp_warn(MOD_STR, "close tcp connect");
            break;
        }
    }
    aos_tcp_destroy(sock_id);

out:
    JS_FreeValue(ctx, recv_param->js_cb_ref);
    amp_free(recv_param);
    if (p)
        amp_free(p);

    g_tcp_recv_flag = 0;
    aos_sem_signal(&g_tcp_close_sem);
    aos_task_exit(0);
    return;
}

/*************************************************************************************
 * Function:    native_udp_recvfrom
 * Description: js native addon for
 *            TCP.recv(sock_id,function(length, buffer_array, src_ip,
 *src_port){}) Called by:   js api Input:       sock_id: interger
 *            function(length, buffer_array, src_ip, src_port): the callback
 *function length: the recv msg length buffer_array: the recv msg buffer src_ip:
 *the peer ip string src_port: the peer port which is a interger
 *
 * Output:      return 0 when TCP.recv call ok
 *            return error number TCP.recv call fail
 **************************************************************************************/
static JSValue native_tcp_receive(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret     = -1;
    int sock_id = 0;
    aos_task_t tcp_recv_task;
    tcp_recv_param_t *recv_param;

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
    recv_param = (tcp_recv_param_t *)amp_calloc(1, sizeof(*recv_param));
    if (!recv_param) {
        amp_warn(MOD_STR, "allocate memory failed");
        goto out;
    }

    recv_param->sock_id = sock_id;
    recv_param->js_cb_ref = JS_DupValue(ctx, argv[1]);

    ret = aos_task_new_ext(&tcp_recv_task, "amp_tcp_recv_task", tcp_recv_routine, recv_param, 1024 * 4, ADDON_TSK_PRIORRITY);
    if (ret != 0) {
        amp_warn(MOD_STR, "tcp recv task error");
        goto out;
    }

out:
    return JS_NewInt32(ctx, ret);
}

/*************************************************************************************
 * Function:    native_tcp_close
 * Description: js native addon for
 *            TCP.close(sock_id)
 * Called by:   js api
 * Input:       sock_id: interger
 *
 * Output:      return 0 when TCP.close call ok
 *            return error number TCP.close call fail
 **************************************************************************************/
static JSValue native_tcp_close(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret = -1;
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

    g_tcp_close_flag = 1;
    aos_sem_wait(&g_tcp_close_sem, MAX_TCP_RECV_TIMEOUT + 50);
    g_tcp_close_flag = 0;
    ret = 0;

out:
    return JS_NewInt32(ctx, ret);
}

static void module_tcp_source_clean(void)
{
    if (g_tcp_recv_flag) {
        g_tcp_close_flag = 1;
        aos_sem_wait(&g_tcp_close_sem, MAX_TCP_RECV_TIMEOUT + 50);
        g_tcp_close_flag = 0;
    }
}


static JSClassDef js_tcp_class = {
    "TCP",
};

static const JSCFunctionListEntry js_tcp_funcs[] = {
    JS_CFUNC_DEF("createSocket", 2, native_tcp_create_socket),
    JS_CFUNC_DEF("send", 3, native_tcp_send),
    JS_CFUNC_DEF("recv", 2, native_tcp_receive),
    JS_CFUNC_DEF("close", 1, native_tcp_close)
};

static int js_tcp_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_tcp_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_tcp_class_id, &js_tcp_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_tcp_funcs, countof(js_tcp_funcs));
    JS_SetClassProto(ctx, js_tcp_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_tcp_funcs, countof(js_tcp_funcs));
}

JSModuleDef *js_init_module_tcp(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_tcp_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_tcp_funcs, countof(js_tcp_funcs));
    return m;
}

void module_tcp_register(void)
{
    amp_debug(MOD_STR, "module_tcp_register");
    JSContext *ctx = js_get_context();

    if (!g_tcp_close_sem) {
        if (aos_sem_new(&g_tcp_close_sem, 0) != 0) {
            amp_error(MOD_STR, "create tcp sem fail");
            return;
        }
    }

    amp_module_free_register(module_tcp_source_clean);
    js_init_module_tcp(ctx, "TCP");
}
