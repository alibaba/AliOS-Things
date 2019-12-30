/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "jse_common.h"
#include "be_inl.h"
#include "miio_common.h"
#include "miio_device.h"
#include "miio_discover.h"

/* params: host:string, token:string */
static duk_ret_t native_createDevice(duk_context *ctx)
{
    if (!(duk_is_string(ctx, 0) && duk_is_string(ctx, 1))) {
        jse_warn("invalid parameters\n");
        duk_push_string(
            ctx,
            "createDevice parameters must be host:string and token:string ");
        return duk_throw(ctx);
    }
    const char *host  = duk_get_string(ctx, 0);
    const char *token = duk_get_string(ctx, 1);
    jse_debug("host: %s, token: %s\n", host, token);
    miio_device_t *device = miio_device_create(host, token);
    duk_push_pointer(ctx, device);
    return 1;
}

typedef struct async_event_param {
    int js_cb_ref;
    char *event;
} async_event_param_t;

static void event_cb(void *arg)
{
    async_event_param_t *p = (async_event_param_t *)arg;
    duk_context *ctx       = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_string(ctx, p->event);
    duk_pcall(ctx, 1);
    duk_pop(ctx);
    jse_free(p->event);
    jse_free(p);
}

static void on_event(void *priv, const char *event)
{
    jse_debug("priv: %p, event: %s\n", priv, event);
    if (strstr(event, "heartbeat")) {
        return;
    }
    async_event_param_t *p = (async_event_param_t *)jse_malloc(sizeof(*p));
    p->js_cb_ref           = (int)priv;
    p->event               = strdup(event);
    if (jse_task_schedule_call(event_cb, p) < 0) {
        jse_warn("jse_task_schedule_call failed\n");
        jse_free(p->event);
        jse_free(p);
    }
}

/* deviceOnEvent(device:pointer, function (event) {}) */
static duk_ret_t native_deviceOnEvent(duk_context *ctx)
{
    if (!(duk_is_pointer(ctx, 0) && duk_is_function(ctx, 1))) {
        jse_warn("invalid parameters\n");
        duk_push_string(
            ctx,
            "deviceOnEvent parameters must be (device:pointer, cb:function)");
        return duk_throw(ctx);
    }
    miio_device_t *device = (miio_device_t *)duk_get_pointer(ctx, 0);
    duk_dup(ctx, 1);
    /* consider not to clear ref */
    int js_cb_ref = be_ref(ctx);
    miio_device_set_event_cb(device, on_event, (void *)js_cb_ref);
    return 0;
}

/* params: device:pointer, method:string, args:string, sid:string */
static duk_ret_t native_deviceControl(duk_context *ctx)
{
    if (!(duk_is_pointer(ctx, 0) && duk_is_string(ctx, 1) &&
          duk_is_string(ctx, 2) &&
          (duk_is_undefined(ctx, 3) || duk_is_string(ctx, 3)))) {
        jse_warn("invalid parameters\n");
        duk_push_string(
            ctx,
            "deviceControl parameters must be "
            " (device:pointer, method:string, args:string) or "
            " (device:pointer, method:string, args:string, sid:string)");
        return duk_throw(ctx);
    }

    miio_device_t *device = duk_get_pointer(ctx, 0);
    const char *method    = duk_get_string(ctx, 1);
    const char *params    = duk_get_string(ctx, 2);
    ;
    const char *sid = duk_get_string(ctx, 3);
    const char *res = NULL;

    res = miio_device_control(device, method, params, sid);
    if (res)
        duk_push_string(ctx, res);
    else
        duk_push_null(ctx);

    return 1;
}

typedef struct async_discover_param {
    int js_cb_ref;
    char host[INET_ADDRSTRLEN];
    long device_id;
} async_discover_param_t;

static void discover_cb(void *arg)
{
    async_discover_param_t *p = (async_discover_param_t *)arg;
    duk_context *ctx          = be_get_context();
    be_push_ref(ctx, p->js_cb_ref);
    duk_push_string(ctx, p->host);
    duk_push_number(ctx, p->device_id);
    duk_pcall(ctx, 2);
    duk_pop(ctx);
    jse_free(p);
}

static void on_discover(void *priv, char *host, long device_id)
{
    jse_debug("discover host: %s, device_id: %ld\n", host, device_id);
    async_discover_param_t *p = (async_discover_param_t *)jse_malloc(sizeof(*p));
    p->js_cb_ref              = (int)priv;
    strcpy(p->host, host);
    p->device_id = device_id;
    if (jse_task_schedule_call(discover_cb, p) < 0) {
        jse_warn("jse_task_schedule_call failed\n");
        jse_free(p);
    }
}

/* discover(timeout, function (ip, deviceId) {}) */
static duk_ret_t native_discover(duk_context *ctx)
{
    if (!(duk_is_number(ctx, 0) && duk_is_function(ctx, 1))) {
        jse_warn("invalid parameters\n");
        duk_push_string(
            ctx, "discover parameters must be (timeout:number, cb:function)");
        return duk_throw(ctx);
    }
    long timeout = (long)duk_get_number(ctx, 0);
    jse_debug("timeout: %ld\n", timeout);
    duk_dup(ctx, 1);
    /* TODO:consider to clear ref */
    int js_cb_ref = be_ref(ctx);
    miio_device_discover(timeout, (void *)js_cb_ref, on_discover);
    return 0;
}

void module_miio_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    /* createDevice(host:string, token:string) */
    duk_push_c_function(ctx, native_createDevice, 2);
    duk_put_prop_string(ctx, -2, "createDevice");

    /* deviceOnEvent(device:pointer, function (event) {}) */
    duk_push_c_function(ctx, native_deviceOnEvent, 2);
    duk_put_prop_string(ctx, -2, "deviceOnEvent");

    /* deviceControl(device:pointer, method:string, args:string, sid:string) */
    duk_push_c_function(ctx, native_deviceControl, 4);
    duk_put_prop_string(ctx, -2, "deviceControl");

    /* discover(timeout, function (ip, deviceId) {}) */
    duk_push_c_function(ctx, native_discover, 2);
    duk_put_prop_string(ctx, -2, "discover");

    duk_put_global_string(ctx, "miio");
}
