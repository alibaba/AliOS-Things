/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "be_jse_api.h"
#include "be_jse_module.h"
#include "be_jse_task.h"
#include "be_log.h"
#include "be_port_osal.h"
#include "hal/system.h"
#include "miio/miio-device.h"
#include "miio/miio-discover.h"

/* params: host:string, token:string */
static be_jse_symbol_t *module_miio_create_device() {
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    char host[17]         = {0};
    char token[33]        = {0};

    be_jse_handle_function(PARSE_NOSKIP_ALL, &arg0, &arg1, NULL, NULL);

    symbol_to_str(arg0, host, sizeof(host));
    symbol_to_str(arg1, token, sizeof(token));
    debug("host: %s, token: %s\n", host, token);

    symbol_unlock(arg0);
    symbol_unlock(arg1);

    miio_device_t *device = miio_device_create(host, token);
    if (device) return new_int_symbol((be_jse_int_t)device);
    return new_symbol(BE_SYM_NULL);
}

typedef struct async_event_param {
    be_jse_symbol_t *func;
    char *event;
} async_event_param_t;

static void event_cb(void *arg) {
    async_event_param_t *p = (async_event_param_t *)arg;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
    async->func        = p->func;
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_str_symbol(p->event);
    int ret          = be_jse_task_schedule_call(be_jse_async_event_cb,
                                        async);  /* async will free automatic */
    if (ret >= 0) {
        INC_SYMBL_REF(async->func);
    } else {
        error("fatal error for be_osal_schedule_call\n\r");
        DEC_SYMBL_REF(async->func);
    }
    free(p->event);
    free(p);
}

static void on_event(void *priv, const char *event) {
    debug("priv: %p, event: %s\n", priv, event);
    if (strstr(event, "heartbeat")) {
        debug("ignore heartbeet event\n");
        return;
    }

    async_event_param_t *p = (async_event_param_t *)malloc(sizeof(*p));
    p->func                = (be_jse_symbol_t *)priv;
    p->event               = strdup(event);
    be_jse_task_schedule_call(event_cb, p);
}

/* params: device:int, cb:function */
static be_jse_symbol_t *module_miio_device_on_event() {
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    debug("in\n");
    be_jse_handle_function(PARSE_NOSKIP_ALL, &arg0, &arg1, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0) || (arg1 == NULL) ||
        !symbol_is_function(arg1)) {
        error("invalid parameter");
        goto out;
    }

    debug("arg1: %p\n", arg1);
    miio_device_t *device = (miio_device_t *)get_symbol_value_int(arg0);
    miio_device_set_event_cb(device, on_event, (void *)arg1);
    INC_SYMBL_REF(arg1);

out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);

    return new_symbol(BE_SYM_NULL);
}

/* params: device:int, method:string, args:string, sid:string */
static be_jse_symbol_t *module_miio_device_control() {
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;
    be_jse_symbol_t *arg2 = NULL;
    be_jse_symbol_t *arg3 = NULL;
    miio_device_t *device = NULL;
    char method[64]       = {0};
    char *params          = NULL;
    char *sid             = NULL;
    const char *res       = NULL;

    be_jse_handle_function(PARSE_NOSKIP_ALL, &arg0, &arg1, &arg2, &arg3);

    symbol_to_str(arg1, method, sizeof(method));

    int params_len = symbol_str_len(arg2);
    params         = (char *)malloc(params_len + 1);
    if (!params) goto cleanup;
    symbol_to_str(arg2, params, params_len);
    params[params_len] = 0;

    if (arg3) {
        int sid_len = symbol_str_len(arg3);
        sid         = (char *)malloc(sid_len + 1);
        if (!sid) goto cleanup;
        symbol_to_str(arg3, sid, sid_len);
        sid[sid_len] = 0;
    }

    device = (miio_device_t *)get_symbol_value_int(arg0);
    res    = miio_device_control(device, method, params, sid);

cleanup:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    symbol_unlock(arg3);

    if (params) free(params);
    if (sid) free(sid);

    if (res) return new_str_symbol(res);

    return new_symbol(BE_SYM_NULL);
}

typedef struct async_discover_param {
    be_jse_symbol_t *func;
    long device_id;
} async_discover_param_t;

static void discover_cb(void *arg) {
    async_discover_param_t *p = (async_discover_param_t *)arg;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
    async->func        = p->func;
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_int_symbol(p->device_id);
    int ret          = be_jse_task_schedule_call(be_jse_async_event_cb,
                                        async);  /* async will free automatic */
    if (ret >= 0) {
        INC_SYMBL_REF(async->func);
    } else {
        error("fatal error for be_osal_schedule_call\n\r");
        DEC_SYMBL_REF(async->func);
    }
    free(p);
}

static void on_discover(void *priv, long device_id) {
    debug("discover device %ld\n", device_id);
    async_discover_param_t *p = (async_discover_param_t *)malloc(sizeof(*p));
    p->func                   = (be_jse_symbol_t *)priv;
    p->device_id              = device_id;
    be_jse_task_schedule_call(discover_cb, p);
}

/* params: timeout:int, cb:function */
static be_jse_symbol_t *module_miio_discover() {
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    be_jse_handle_function(PARSE_NOSKIP_ALL, &arg0, &arg1, NULL, NULL);
    if ((arg0 == NULL) || !symbol_is_int(arg0) || (arg1 == NULL) ||
        !symbol_is_function(arg1)) {
        error("invalid parameter");
        goto out;
    }

    int timeout = get_symbol_value_int(arg0);
    debug("timeout: %d\n", timeout);
    miio_device_discover(timeout, (void *)arg1, on_discover);
    INC_SYMBL_REF(arg1);

out:
    symbol_unlock(arg0);
    symbol_unlock(arg1);

    return new_symbol(BE_SYM_NULL);
}

static be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var,
                                         const char *name) {
    if (strcmp(name, "createDevice") == 0) return module_miio_create_device();
    if (strcmp(name, "deviceOnEvent") == 0)
        return module_miio_device_on_event();
    if (strcmp(name, "deviceControl") == 0) return module_miio_device_control();
    if (strcmp(name, "discover") == 0) return module_miio_discover();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_miio_register(void) {
    be_jse_module_load("miio", module_handle_cb);
}
