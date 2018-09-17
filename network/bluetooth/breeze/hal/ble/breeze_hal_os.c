/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <breeze_hal_os.h>

int os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms)
{
    return aos_timer_new_ext(timer, cb, arg, ms, 0, 0);
}

int os_timer_start(os_timer_t *timer)
{
    return aos_timer_start(timer);
}

int os_timer_stop(os_timer_t *timer)
{
    return aos_timer_stop(timer);
}

void os_timer_free(os_timer_t *timer)
{
    aos_timer_free(timer);
}

void os_reboot()
{
    aos_reboot();
}

void os_msleep(int ms)
{
    aos_msleep(ms);
}

long long os_now_ms()
{
    return aos_now_ms();
}

typedef struct event_ctx_s
{
    os_event_cb_t cb;
    void         *priv;
} event_ctx_t;

int os_post_event(os_event_type_t type, os_event_code_t code,
                  unsigned long value)
{
    uint16_t atype, acode;
    unsigned long avalue;

    switch (type) {
        case OS_EV_BLE:
            atype = EV_BLE;
            break;
        case OS_EV_COMBO:
            atype = EV_BZ_COMBO;
            break;
        case OS_EV_AUTH:
            atype = EV_BZ_AUTH;
            break;
        default:
            break;
    }

    /* Since type is already remapped, it's OK to use code and value without
     * remapp */
    acode  = code;
    avalue = value;

    return aos_post_event(atype, acode, avalue);
}

static void event_helper(input_event_t *event, void *priv)
{
    os_event_t    oevt;
    os_event_cb_t cb;
    event_ctx_t  *ctx;

    if (!event || !priv) {
        printf("Error: invalid argument (%s).\r\n", __func__);
        return;
    }

    ctx = (event_ctx_t *)priv;
    cb  = ctx->cb;

    switch (event->type) {
        case EV_BLE:
            oevt.type = OS_EV_BLE;
            break;
        case EV_BZ_COMBO:
            oevt.type = OS_EV_COMBO;
            break;
        case EV_BZ_AUTH:
            oevt.type = OS_EV_AUTH;
            break;
        default:
            printf("Error: invaid event type.\r\n");
            return -1;
    }

    oevt.code  = event->code;
    oevt.value = event->value;

    cb(&oevt, ctx->priv);

    /* Do NOT free the ctx, since one event may want be triggered multiple
     * times. */
    /* ??aos_free(ctx);?? */
}

/* TODO: one type may be registered multiple times, so more than one ctx should
 * be managed. */
int os_register_event_filter(os_event_type_t type, os_event_cb_t cb, void *priv)
{
    uint16_t     atype;
    event_ctx_t *ctx;

    ctx = aos_malloc(sizeof(event_ctx_t));
    if (!ctx) {
        printf("Failed to allocate mm for event (type %d)\r\n", type);
        return -1;
    }

    switch (type) {
        case OS_EV_BLE:
            atype = EV_BLE;
            break;
        case OS_EV_COMBO:
            atype = EV_BZ_COMBO;
            break;
        case OS_EV_AUTH:
            atype = EV_BZ_AUTH;
            break;
        default:
            break;
    }

    ctx->cb   = cb;
    ctx->priv = priv;

    return aos_register_event_filter(atype, event_helper, ctx);
}

void os_post_delayed_action(int ms, void (*action)(void *arg), void *arg)
{
    aos_post_delayed_action(ms, action, arg);
}

void os_start_event_dispatcher()
{
    aos_loop_run();
}

int os_kv_set(const char *key, const void *value, int len, int sync)
{
    return 0;
}

int os_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}

int os_kv_del(const char *key)
{
    return 0;
}
