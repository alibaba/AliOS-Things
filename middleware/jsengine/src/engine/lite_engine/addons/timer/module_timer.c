/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "jse_common.h"
#include "be_module.h"

#ifdef JSE_CORE_ADDON_TIMER
#define MAGIC 0x55aa55aa
typedef struct {
    int magic;
    be_jse_symbol_t *func; /* timer callback function */
    int repeat;            /* repeat or one shot */
    void *timerid;         /* timer id */
} timer_action_t;

static void timer_call_action(void *arg)
{
    timer_action_t *pdata = (timer_action_t *)arg;

    if (pdata->magic != MAGIC) return;

    be_jse_symbol_t *func = (be_jse_symbol_t *)(pdata->func);
    be_jse_execute_func(jsengine_get_executor(), func, 0, NULL);

    if (pdata->repeat != 1) {
        DEC_SYMBL_REF(func);
        pdata->magic = 0;
        jse_free(arg);
    }
}

static be_jse_symbol_t *setup_timer(int repeat)
{
    be_jse_symbol_t *cbv;
    be_jse_symbol_t *msv;
    void *timer_id = NULL;
    be_jse_handle_function(0, &cbv, &msv, NULL, NULL);

    if (!cbv || !msv || (!symbol_is_function(cbv))) {
        jse_error("process timer parameter error\n");
        goto done;
    }

    long ms = get_symbol_value_int(msv);

    timer_action_t *p_param =
        (timer_action_t *)jse_calloc(1, sizeof(timer_action_t));
    if (!p_param) goto done;
    p_param->magic  = MAGIC;
    p_param->func   = cbv;
    p_param->repeat = repeat;
    INC_SYMBL_REF(p_param->func);
    timer_id = be_jse_task_timer_action(ms, timer_call_action, p_param, repeat);
    p_param->timerid = timer_id;

done:
    symbol_unlock(cbv);
    symbol_unlock(msv);
    return new_int_symbol((be_jse_int_t)p_param);
}

static be_jse_symbol_t *clear_timer()
{
    be_jse_symbol_t *hv = be_jse_handle_single_arg_function();

    if (!hv) {
        return new_symbol(BE_SYM_NULL);
    }

    timer_action_t *p_timer_action = (timer_action_t *)get_symbol_value_int(hv);
    jse_osal_timer_delete((osTimerId)p_timer_action->timerid);
    p_timer_action->magic = 0;
    DEC_SYMBL_REF(p_timer_action->func);
    jse_free(p_timer_action);

    symbol_unlock(hv);
    return new_symbol(BE_SYM_NULL);
}

be_jse_symbol_t *timers_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var, const char *name)
{
    if (!strcmp(name, "setTimeout")) return setup_timer(0);
    if (!strcmp(name, "clearTimeout")) return clear_timer();
    if (!strcmp(name, "setInterval")) return setup_timer(1);
    if (!strcmp(name, "clearInterval")) return clear_timer();

    return BE_JSE_FUNC_UNHANDLED;
}

#endif
