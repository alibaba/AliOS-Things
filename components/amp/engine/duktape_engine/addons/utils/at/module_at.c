/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "be_inl.h"
#include "amp_task.h"

#define MOD_STR "AT"
#define MAX_LINE_SIZE 1024

typedef struct {
    int js_cb_ref;
    char *buf;
    int buflen;
    aos_sem_new sem;
} at_notify_param_t;

static duk_ret_t native_atRead(duk_context *ctx)
{
    char *outbuf  = NULL;
    int readsize  = 0;
    int      size = 0;
    int received  = 0;
    int timeout   = 0;
    uint64_t now  = 0;

    if (!(duk_is_number(ctx, 0) && duk_is_number(ctx, 1))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "native_atReadLine parameters must be number");
        return duk_throw(ctx);
    }

    readsize = duk_is_number(ctx, 0);
    timeout = duk_get_number(ctx, 1);
    now = aos_now_ms();

    amp_debug(MOD_STR, "native_atRead readsize %d, timeout %d", readsize, timeout);

    outbuf = (char *)aos_malloc(readsize + 1);
    memset(outbuf, 0, readsize + 1);
    while (received < readsize) {
        if (aos_now_ms() - now > (unsigned long)timeout) {
            received = 0;
            break;
        }
        size = amp_at_read(&outbuf[received], 1);
        if(size == 1) {
            received ++;
        } else {
            aos_msleep(1);
        }
    }

    amp_debug(MOD_STR, "native_atRead received %d", received);

    if(received > 0) {
        duk_push_string(ctx, outbuf);
    } else {
        duk_push_undefined(ctx);
    }

    aos_free(outbuf);

    return 1;
}

static duk_ret_t native_atReadLine(duk_context *ctx)
{
    char *outbuf  = NULL;
    int readsize  = 0;
    int      size = 0;
    int received  = 0;
    int timeout   = 0;
    uint64_t now  = 0;

    if (!(duk_is_number(ctx, 0))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "native_atReadLine parameters must be number");
        return duk_throw(ctx);
    }

    readsize = MAX_LINE_SIZE;

    timeout = duk_get_number(ctx, 0);
    now = aos_now_ms();

    outbuf = (char *)aos_malloc(readsize + 1);
    memset(outbuf, 0, readsize + 1);

    while (received < readsize) {
        if (aos_now_ms() - now > (unsigned long)timeout) {
            received = 0;
            break;
        }
        size = amp_at_read(&outbuf[received], 1);
        if(size == 1) {
            if(outbuf[received] == '\n') {
                outbuf[received + 1] = '\0';
                break;
            } else {
                received ++;
            }
        } else {
            aos_msleep(1);
        }
    }
    if(received > 0) {
        duk_push_string(ctx, outbuf);
    } else {
        duk_push_undefined(ctx);
    }

    aos_free(outbuf);

    return 1;
}

static duk_ret_t native_atSendNoReply(duk_context *ctx)
{
    int ret = 0;
    const char *data = NULL;
    bool delimiter = false;

    if (!(duk_is_string(ctx, 0) && duk_is_number(ctx, 1) )) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "atSendNoReply parameters must be string and function");
        return duk_throw(ctx);
    }

    data      = duk_get_string(ctx, 0);
    delimiter = duk_get_number(ctx, 1);

    ret = amp_at_send_no_reply(data, strlen(data), (delimiter != 0) ? true : false);

    duk_push_int(ctx, ret);
    amp_debug(MOD_STR, "native_atSendNoReply");

    return 0;
}

static void at_notify(void *arg)
{
    at_notify_param_t *param = (at_notify_param_t *)arg;
    duk_context *ctx = be_get_context();

    be_push_ref(ctx, param->js_cb_ref);
    duk_push_pointer(ctx, param->buf);
    duk_push_int(ctx, param->buflen);

    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    aos_sem_signal(&param->sem);
}

/* avoid stdout in irq function */
static void native_atCallback(void *arg, char *buf, int buflen)
{
    at_notify_param_t *at_notify_param = NULL;

    at_notify_param = (at_notify_param_t *) aos_malloc(sizeof(at_notify_param_t));

    at_notify_param->js_cb_ref = (int)(arg);
    at_notify_param->buf       = (char *)malloc(buflen);
    memcpy(at_notify_param->buf, buf, buflen);
    at_notify_param->buflen    = buflen;

    aos_sem_new(&at_notify_param->sem, 0);

    amp_debug(MOD_STR, "native_atCallback buflen %d\n", at_notify_param->buflen);

    amp_task_schedule_call(at_notify, at_notify_param);

    aos_sem_wait(&at_notify_param->sem, AOS_WAIT_FOREVER);
    aos_sem_free(&at_notify_param->sem);
    aos_free(at_notify_param->buf);
    aos_free(at_notify_param);

    amp_debug(MOD_STR, "native_atCallback end\n");
}

static duk_ret_t native_atRegisterCmdCallback(duk_context *ctx)
{
    char *prefix   = NULL;
    void *js_cb_ref = NULL;
    int prompt         = 0;
    int ret            = 0;

    if (!(duk_is_string(ctx, 0) && duk_is_function(ctx, 1) )) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "atRegisterCmdCallback parameters must be string and function");
        return duk_throw(ctx);
    }

    prefix = (char *)duk_get_string(ctx, 0);

    duk_dup(ctx, 1);
    js_cb_ref = (void *)be_ref(ctx);

    ret = amp_at_register_callback(prefix, native_atCallback, js_cb_ref);

    duk_push_int(ctx, ret);
    amp_debug(MOD_STR, "native_atRegisterCmdCallback");

    return 1;
}

static duk_ret_t native_atInit(duk_context *ctx)
{
    int at_uart_port     = 0;
    uint32_t baud_rate   = 0;
    char *send_delimiter = NULL;
    int ret              = 0;

    if (!(duk_is_number(ctx, 0) && duk_is_number(ctx, 1) && duk_is_string(ctx, 2))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "atInit parameters must be number");
        return duk_throw(ctx);
    }

    at_uart_port   = duk_get_number(ctx, 0);
    baud_rate      = duk_get_number(ctx, 1);
    send_delimiter = duk_get_string(ctx, 2);

    ret = amp_at_init(at_uart_port, baud_rate, send_delimiter);

    duk_push_int(ctx, ret);
    amp_debug(MOD_STR, "native_atInit");

    return 1;
}

static duk_ret_t native_atDeInit(duk_context *ctx)
{
    amp_at_deinit();

    duk_push_int(ctx, 0);
    return 1;
}

static duk_ret_t native_atSetEcho(duk_context *ctx)
{
    int flag = 0;
    if (!(duk_is_number(ctx, 0))) {
        amp_warn(MOD_STR, "invalid parameters");
        duk_push_string(ctx,
                        "atInit parameters must be number");
        return duk_throw(ctx);
    }

    flag   = duk_get_number(ctx, 0);
    amp_at_set_echo(flag);

    duk_push_int(ctx, 0);
    return 1;
}

static void module_atClean(void)
{
    amp_at_deinit();
}

void module_at_register(void)
{
    amp_debug(MOD_STR, "module_at_register");
    duk_context *ctx = be_get_context();

    amp_module_free_register(module_atClean);

    duk_push_object(ctx);

    AMP_ADD_FUNCTION("atInit",    native_atInit, 3);
    AMP_ADD_FUNCTION("atDeInit",    native_atDeInit, 0);
    AMP_ADD_FUNCTION("atSetEcho",    native_atSetEcho, 1);
    AMP_ADD_FUNCTION("atRegisterCmdCallback",  native_atRegisterCmdCallback, 2);
    AMP_ADD_FUNCTION("atSendNoReply", native_atSendNoReply, 2);
    AMP_ADD_FUNCTION("atRead", native_atRead, 2);
    AMP_ADD_FUNCTION("atReadLine", native_atReadLine, 1);

    duk_put_prop_string(ctx, -2, "AT");
}
