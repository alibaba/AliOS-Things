/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "aos_network.h"
#include "amp_memory.h"
#include "amp_defines.h"
#include "module_mqtt.h"
#include "aiot_mqtt_api.h"
#include "amp_task.h"
#include "amp_list.h"
#include "quickjs_addon_common.h"

#define MOD_STR "MQTT"
#define MQTT_TASK_YIELD_TIMEOUT 200
static char g_mqtt_close_flag = 0;
static char g_mqtt_conn_flag = 0;
static aos_sem_t g_mqtt_close_sem = NULL;

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static JSClassID js_mqtt_class_id;
static JSClassDef js_mqtt_class = {
    "MQTT",
};

typedef struct {
    aos_mqtt_handle_t *aos_mqtt_handle;
    char *topic;
    char *payload;
    char *service_id;
    char *params;
    JSValue js_cb_ref;
    int ret_code;
    int topic_len;
    int payload_len;
    int params_len;
    uint64_t msg_id;
    aiot_mqtt_option_t option;
    aiot_mqtt_event_type_t event_type;
    aiot_mqtt_recv_type_t recv_type;
} aos_mqtt_notify_param_t;

static char *__amp_strdup(char *src)
{
    char   *dst;
    size_t  len = 0;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src);

    dst = amp_malloc(len + 1);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

static void aos_mqtt_notify(void *pdata)
{
    aos_mqtt_notify_param_t *param = (aos_mqtt_notify_param_t *)pdata;

    JSContext *ctx = js_get_context();

    if (param->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        switch (param->event_type) {
        case AIOT_MQTTEVT_CONNECT:
        case AIOT_MQTTEVT_RECONNECT:
        case AIOT_MQTTEVT_DISCONNECT:
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "code", JS_NewInt32(ctx, param->ret_code));
            JSValue mqtt = JS_NewObjectClass(ctx, js_mqtt_class_id);
            JS_SetOpaque(mqtt, param->aos_mqtt_handle);
            JS_SetPropertyStr(ctx, obj, "handle", mqtt);

            JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &obj);
            JS_FreeValue(ctx, val);
            JS_FreeValue(ctx, obj);
        }
            break;
        default:
            amp_free(param);
            return;
        }
    } else if (param->option == AIOT_MQTTOPT_RECV_HANDLER) {
        switch (param->recv_type) {
        case AIOT_MQTTRECV_PUB:
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "code", JS_NewInt32(ctx, param->ret_code));
            JSValue mqtt = JS_NewObjectClass(ctx, js_mqtt_class_id);
            JS_SetOpaque(mqtt, param->aos_mqtt_handle);
            JS_SetPropertyStr(ctx, obj, "handle", mqtt);
            JS_SetPropertyStr(ctx, obj, "topic", JS_NewString(ctx, param->topic));
            JS_SetPropertyStr(ctx, obj, "payload", JS_NewString(ctx, param->payload));

            JSValue val = JS_Call(ctx, param->js_cb_ref, JS_UNDEFINED, 1, &obj);
            JS_FreeValue(ctx, val);
            JS_FreeValue(ctx, obj);

            amp_free(param->topic);
            amp_free(param->payload);
        }
            break;
        default:
            amp_free(param);
            return;
        }
    }

    amp_free(param);
}

static void aos_mqtt_message_cb(aos_mqtt_message_t *message, void *userdata)
{
    aos_mqtt_userdata_t *udata = (aos_mqtt_userdata_t *)userdata;
    aos_mqtt_handle_t *aos_mqtt_handle;
    aos_mqtt_notify_param_t *param;

    if (!message || !udata)
        return;

    param = amp_malloc(sizeof(aos_mqtt_notify_param_t));
    if (!param) {
        amp_error(MOD_STR, "alloc device notify param fail");
        return;
    }
    memset(param, 0, sizeof(aos_mqtt_notify_param_t));
    param->aos_mqtt_handle = (aos_mqtt_handle_t *)udata->handle;
    param->option = message->option;
    param->js_cb_ref = param->aos_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF];

    if (message->option == AIOT_MQTTOPT_EVENT_HANDLER) {
        switch (message->event.type) {
        case AIOT_MQTTEVT_CONNECT:
        case AIOT_MQTTEVT_RECONNECT:
        case AIOT_MQTTEVT_DISCONNECT:
            param->ret_code = message->event.code;
            param->event_type = message->event.type;
            break;
        default:
            amp_free(param);
            return;
        }
    } else if (message->option == AIOT_MQTTOPT_RECV_HANDLER) {
        switch (message->recv.type) {
        case AIOT_MQTTRECV_PUB:
            param->ret_code = message->recv.code;
            param->topic_len = message->recv.topic_len;
            param->payload_len = message->recv.payload_len;
            param->topic = __amp_strdup(message->recv.topic);
            param->payload = __amp_strdup(message->recv.payload);
            param->recv_type = message->recv.type;
            break;
        default:
            amp_free(param);
            return;
        }
    } else {
        amp_free(param);
        return;
    }

    amp_task_schedule_call(aos_mqtt_notify, param);
}

static void mqtt_connect_task(void *pdata)
{
    int ret;
    JSContext *ctx = js_get_context();
    aos_mqtt_handle_t *aos_mqtt_handle = (aos_mqtt_handle_t *)pdata;
    aos_mqtt_userdata_t *userdata = amp_malloc(sizeof(aos_mqtt_userdata_t));
    if (userdata == NULL) {
        amp_error(MOD_STR, "amp mqtt handle malloc failed");
        amp_free(aos_mqtt_handle);
        return;
    }

    userdata->callback = aos_mqtt_message_cb;
    userdata->handle = aos_mqtt_handle;

    ret = mqtt_client_start(&aos_mqtt_handle->mqtt_handle, userdata);
    if (ret < 0) {
        amp_error(MOD_STR, "mqtt client init failed");
        goto out;
    }

    g_mqtt_conn_flag = 1;
    aos_mqtt_handle->res = ret;

    while (!g_mqtt_close_flag) {
        aos_msleep(1000);
    }

out:
    JS_FreeValue(ctx, aos_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF]);
    JS_FreeCString(ctx, aos_mqtt_handle->host);
    JS_FreeCString(ctx, aos_mqtt_handle->clientid);
    JS_FreeCString(ctx, aos_mqtt_handle->username);
    JS_FreeCString(ctx, aos_mqtt_handle->password);

    aos_free(userdata);
    aos_free(aos_mqtt_handle);
    aos_sem_signal(&g_mqtt_close_sem);
    aos_task_exit(0);
}

static JSValue native_mqtt_start(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int ret;
    uint32_t val = 0;
    aos_mqtt_handle_t *aos_mqtt_handle = NULL;
    aos_task_t mqtt_task;
    JSValue js_cb_ref;

    /* check paramters */
    JSValue options = argv[0];
    JSValue cb = argv[1];

    if (!JS_IsObject(options) || !JS_IsFunction(ctx, cb)) {
        amp_warn(MOD_STR, "parameter must be object and function\n");
        ret = -1;
        goto out;
    }

    /* get device certificate */
    JSValue j_host = JS_GetPropertyStr(ctx, argv[0], "host");
    JSValue j_port = JS_GetPropertyStr(ctx, argv[0], "port");
    JSValue j_client_id = JS_GetPropertyStr(ctx, argv[0], "client_id");
    JSValue j_username = JS_GetPropertyStr(ctx, argv[0], "username");
    JSValue j_password = JS_GetPropertyStr(ctx, argv[0], "password");
    JSValue j_keepaliveSec = JS_GetPropertyStr(ctx, argv[0], "keepalive_interval");

    if (!JS_IsString(j_host) || !JS_IsNumber(j_port) || !JS_IsString(j_client_id) ||
        !JS_IsString(j_username) || !JS_IsString(j_password) || !JS_IsNumber(j_keepaliveSec))
    {
        amp_warn(MOD_STR,
                 "Parameter 1 must be an object like {host: string, "
                 "port: uint, client_id: string, username: string, "
                 "password: string, keepalive_interval: uint}\n");
        ret = -2;
        goto out;
    }

    aos_mqtt_handle = (aos_mqtt_handle_t *)amp_malloc(sizeof(aos_mqtt_handle_t));
    if (!aos_mqtt_handle) {
        amp_error(MOD_STR, "allocate memory failed\n");
        goto out;
    }

    aos_mqtt_handle->host = JS_ToCString(ctx, j_host);
    JS_ToUint32(ctx, &val, j_port);
    aos_mqtt_handle->port = (uint16_t)val;

    aos_mqtt_handle->clientid = JS_ToCString(ctx, j_client_id);
    aos_mqtt_handle->username = JS_ToCString(ctx, j_username);
    aos_mqtt_handle->password = JS_ToCString(ctx, j_password);

    JS_ToUint32(ctx, &val, j_keepaliveSec);
    aos_mqtt_handle->keepaliveSec = (uint16_t)val;

    amp_debug(MOD_STR, "host: %s, port: %d\n", aos_mqtt_handle->host, aos_mqtt_handle->port);
    amp_debug(MOD_STR, "client_id: %s, username: %s, password: %s\n", aos_mqtt_handle->clientid, aos_mqtt_handle->username, aos_mqtt_handle->password);

    js_cb_ref = JS_DupValue(ctx, cb);
    aos_mqtt_handle->js_cb_ref[MQTT_JSCALLBACK_START_CLIENT_REF] = js_cb_ref;

    /* create task to IOT_MQTT_Yield() */
    ret = aos_task_new_ext(&mqtt_task, "amp mqtt task", mqtt_connect_task, aos_mqtt_handle, 1024 * 4, MQTT_TSK_PRIORITY);
    if (ret < 0) {
        amp_warn(MOD_STR, "jse_osal_create_task failed\n");
        JS_FreeValue(ctx, js_cb_ref);
        aos_free(aos_mqtt_handle);
        ret = -4;
    }

out:
    return JS_NewInt32(ctx, ret);
}

/* subscribe */
static JSValue native_mqtt_subscribe(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int res = -1;
    aos_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic = NULL;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    amp_mqtt_handle = JS_GetOpaque2(ctx, argv[0], js_mqtt_class_id);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = JS_ToCString(ctx, argv[1]);
    JS_ToInt32(ctx, &qos, argv[2]);

    res = aiot_mqtt_sub(amp_mqtt_handle->mqtt_handle, topic, NULL, qos, NULL);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt subscribe failed, ret = -0x%04X", res);
    }

    JS_FreeCString(ctx, topic);

out:
    return JS_NewInt32(ctx, res);
}

/* unsubscribe */
static JSValue native_mqtt_unsubscribe(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int res = -1;
    aos_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    amp_mqtt_handle = JS_GetOpaque2(ctx, argv[0], js_mqtt_class_id);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = JS_ToCString(ctx, argv[1]);

    amp_debug(MOD_STR, "unsubscribe topic: %s", topic);

    res = aiot_mqtt_unsub(amp_mqtt_handle->mqtt_handle, topic);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt unsubscribe failed\n");
    }
    JS_FreeCString(ctx, topic);

out:
    return JS_NewInt32(ctx, res);
}

/* publish */
static JSValue native_mqtt_publish(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int res = -1;
    aos_mqtt_handle_t *amp_mqtt_handle = NULL;
    const char *topic;
    const char *payload;
    uint16_t payload_len = 0;
    uint8_t qos = 0;
    int js_cb_ref = 0;

    amp_mqtt_handle = JS_GetOpaque2(ctx, argv[0], js_mqtt_class_id);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    topic = JS_ToCString(ctx, argv[1]);
    payload = JS_ToCString(ctx, argv[2]);
    JS_ToInt32(ctx, &qos, argv[1]);
    payload_len = strlen(payload);

    amp_debug(MOD_STR, "publish topic: %s, payload: %s, qos is: %d", topic, payload, qos);

    res = aiot_mqtt_pub(amp_mqtt_handle->mqtt_handle, topic, payload, payload_len, qos);
    if (res < 0) {
        amp_error(MOD_STR, "aiot app mqtt publish failed");
    }
    JS_FreeCString(ctx, topic);
    JS_FreeCString(ctx, payload);

out:
    return JS_NewInt32(ctx, res);
}

static JSValue native_mqtt_close(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int res = -1;
    int js_cb_ref = 0;
    aos_mqtt_handle_t *amp_mqtt_handle = NULL;

    amp_mqtt_handle = JS_GetOpaque2(ctx, argv[0], js_mqtt_class_id);
    if (amp_mqtt_handle == NULL) {
        amp_warn(MOD_STR, "mqtt handle is null");
        goto out;
    }

    res = mqtt_client_stop(&amp_mqtt_handle->mqtt_handle);
    if (res < 0) {
        amp_debug(MOD_STR, "mqtt client stop failed");
    }

out:
    /* release mqtt in mqtt_yield_task() */
    g_mqtt_close_flag = 1;
    aos_sem_wait(&g_mqtt_close_sem, MQTT_TASK_YIELD_TIMEOUT + 50);
    g_mqtt_close_flag = 0;
    return JS_NewInt32(ctx, 1);
}

static void module_mqtt_source_clean(void)
{
    if (g_mqtt_close_flag) {
        aos_sem_wait(&g_mqtt_close_sem, MQTT_TASK_YIELD_TIMEOUT + 50);
        g_mqtt_close_flag = 0;
    }
}

static const JSCFunctionListEntry js_mqtt_funcs[] = {
    JS_CFUNC_DEF("start", 2, native_mqtt_start),
    JS_CFUNC_DEF("subscribe", 3, native_mqtt_subscribe),
    JS_CFUNC_DEF("unsubscribe", 3, native_mqtt_unsubscribe),
    JS_CFUNC_DEF("publish", 5, native_mqtt_publish),
    JS_CFUNC_DEF("close", 2, native_mqtt_close),
};

static int js_mqtt_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_mqtt_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_mqtt_class_id, &js_mqtt_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_mqtt_funcs, countof(js_mqtt_funcs));
    JS_SetClassProto(ctx, js_mqtt_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_mqtt_funcs, countof(js_mqtt_funcs));
}

JSModuleDef *js_init_module_mqtt(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_mqtt_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_mqtt_funcs, countof(js_mqtt_funcs));
    return m;
}

void module_mqtt_register(void)
{
    amp_debug(MOD_STR, "module_mqtt_register");
    JSContext *ctx = js_get_context();
    js_init_module_mqtt(ctx, "MQTT");
}
