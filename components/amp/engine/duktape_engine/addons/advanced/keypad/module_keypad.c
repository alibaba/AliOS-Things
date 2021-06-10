/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "amp_hal_keypad.h"
#include "aos_system.h"
#include "amp_task.h"
#include "board_mgr.h"
#include "be_inl.h"

#define MOD_STR "Keypad"


static int keypad_js_cb_ref;

typedef struct {
	keypad_code_t code;
	int value;
} keypad_notify_t;

static int keypad_notify_callback(void *pdata)
{
	duk_context *ctx;
	keypad_notify_t *param = (keypad_notify_t *)pdata;
	if (!param)
		return -1;
    ctx = be_get_context();
    be_push_ref(ctx, keypad_js_cb_ref);
    duk_push_int(ctx, param->code);
	duk_push_int(ctx, param->value);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    duk_gc(ctx, 0);
	aos_free(param);
	return 0;
}

static int keypad_event_callback(keypad_code_t code, int value)
{
	keypad_notify_t *param = aos_malloc(sizeof(keypad_notify_t));
	if (!param)
		return -1;
	param->code = code;
	param->value = value;
	amp_task_schedule_call(keypad_notify_callback, param);
	return 0;
}

static duk_ret_t native_keypad_on(duk_context *ctx)
{
    int ret = -1;

    if (!duk_is_function(ctx, 0)) {
        amp_warn(MOD_STR, "parameter must be (function)");
        goto out;
    }
    duk_dup(ctx, 0);
	keypad_js_cb_ref = be_ref(ctx);

    ret = amp_hal_keypad_event_register(keypad_event_callback);
    if (ret < 0) {
        amp_error(MOD_STR, "amp_hal_keypad_event_register fail!");
        goto out;
    }
    ret = 0;

out:
    duk_push_int(ctx, ret);
    return 1;
}


static duk_ret_t native_keypad_open(duk_context *ctx)
{
	//amp_hal_keypad_init();
    duk_push_int(ctx, 0);
    return 1;
}

static duk_ret_t native_keypad_close(duk_context *ctx)
{
	//amp_hal_keypad_finalize();
    duk_push_int(ctx, 0);
    return 1;
}

void module_keypad_register(void)
{
    amp_debug(MOD_STR, "module_keypad_register");
    duk_context *ctx = be_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_keypad_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_keypad_on, 1);
    duk_put_prop_string(ctx, -2, "on");

    duk_push_c_function(ctx, native_keypad_close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_prop_string(ctx, -2, "Keypad");
}

