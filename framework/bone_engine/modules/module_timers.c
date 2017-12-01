/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "be_jse_api.h"
#include "module_timers.h"

#ifdef USE_TIMERS

#define MAGIC  0x55ff0055
struct timer_wrap {
    int magic;
    be_jse_symbol_t *cbv;
    long ms;
    int repeat;
};
typedef struct timer_wrap timer_wrap_t;

static void timer_action(void *arg);

static void call_action(void *arg)
{
    be_jse_symbol_t *func = (be_jse_symbol_t*)arg;

    be_jse_execute_func(bone_engine_get_executor(), func, 0, NULL);

    DEC_SYMBL_REF(func);

}


static void timer_action(void *arg)
{
    LOGD("MODULE_TIMERS"," 1 aos_task_name=%s \r\n", aos_task_name());

    timer_wrap_t *t = (timer_wrap_t *)arg;

    LOGD("MODULE_TIMERS","JS handle =%d \n", t);

    if( t->magic == MAGIC ) {

        be_jse_symbol_t *func = get_symbol_value(t->cbv);
        if (symbol_is_function(func)) {
            INC_SYMBL_REF(func);
            // 需要考虑在callback中clearInterval/clearTimeout自身
            aos_schedule_call(call_action, func);
        }
        symbol_unlock(func);
        if (t->repeat) {
            aos_post_delayed_action(t->ms, timer_action, t);
        } else {
            symbol_unlock(t->cbv);
            t->magic = 0;
            aos_free(t);
            LOGI("MODULE_TIMERS","aos_free(%p) \n", t);
        }
    } else {
        LOGW("MODULE_TIMERS","alread free \n");
    }

    LOGD("MODULE_TIMERS"," 2 aos_task_name=%s \r\n", aos_task_name());

}

static be_jse_symbol_t *setup_timer(int repeat)
{
    be_jse_symbol_t *cbv;
    be_jse_symbol_t *msv;
    be_jse_handle_function(0, &cbv, &msv, NULL, NULL);

    be_jse_symbol_t *func = get_symbol_value(cbv);
    if( !func || !msv || (!symbol_is_function(func))) {
        LOGE("MODULE_TIMERS","process timer parameter error\n");
        symbol_unlock(func);
        symbol_unlock(cbv);
        symbol_unlock(msv);
        return new_symbol(BE_SYM_NULL);
    }
    symbol_unlock(func);

    long ms = get_symbol_value_int(msv);
    symbol_unlock(msv);

    timer_wrap_t *t = aos_malloc(sizeof(*t));
    t->magic = MAGIC;
    t->cbv = cbv;
    t->ms = ms;
    t->repeat = repeat;
    aos_post_delayed_action(ms, timer_action, t);

    LOGD("MODULE_TIMERS","JS handle =%d \n", t);

    return new_int_symbol((be_jse_int_t)t);
}

static be_jse_symbol_t* clear_timer()
{
    be_jse_symbol_t *hv = be_jse_handle_single_arg_function();

    if (hv) {
        timer_wrap_t *t = (timer_wrap_t *)get_symbol_value_int(hv);

        LOGD("MODULE_TIMERS","JS handle =%d \n", t);

        if( t->magic == MAGIC ) {
            aos_cancel_delayed_action(t->ms, timer_action, t);
            symbol_unlock(t->cbv);
            t->magic = 0;
            aos_free(t);
            LOGW("MODULE_TIMERS","aos_free(%p) \n", t);
        } else {
            LOGW("MODULE_TIMERS","alread free \n");
        }
    }
    symbol_unlock(hv);
    return new_symbol(BE_SYM_NULL);
}

be_jse_symbol_t *timers_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    if (var)
        return BE_JSE_FUNC_UNHANDLED;

    if (!strcmp(name, "setTimeout"))
        return setup_timer(0);
    if (!strcmp(name, "clearTimeout"))
        return clear_timer();
    if (!strcmp(name, "setInterval"))
        return setup_timer(1);
    if (!strcmp(name, "clearInterval"))
        return clear_timer();

    return BE_JSE_FUNC_UNHANDLED;
}

#endif

