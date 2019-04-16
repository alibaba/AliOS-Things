/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <breeze_hal_os.h>

int os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms)
{
    return aos_timer_new_ext((aos_timer_t *)timer, cb, arg, ms, 0, 0);
}

int os_timer_start(os_timer_t *timer)
{
    return aos_timer_start((aos_timer_t *)timer);
}

int os_timer_stop(os_timer_t *timer)
{
    return aos_timer_stop((aos_timer_t *)timer);
}

void os_timer_free(os_timer_t *timer)
{
    aos_timer_free((aos_timer_t *)timer);
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

int os_kv_set(const char *key, const void *value, int len, int sync)
{
    return aos_kv_set(key, value, len, sync);
}

int os_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return aos_kv_get(key, buffer, buffer_len);
}

int os_kv_del(const char *key)
{
    return aos_kv_del(key);
}

int os_rand(void)
{
    return 0;
}
