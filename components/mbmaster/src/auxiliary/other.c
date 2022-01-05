/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <mbmaster.h>

static mb_handler_t mb_handler[MBMASTER_CONFIG_HANDLER_MAX];

mb_handler_t *mb_alloc_handler(void)
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();
    for (uint8_t i = 0; i < MBMASTER_CONFIG_HANDLER_MAX; i++) {
        if (mb_handler[i].used == 0) {
            mb_handler[i].used = 1;
            MB_CRITICAL_EXIT();
            return &mb_handler[i];
        }
    }
    MB_CRITICAL_EXIT();
    return NULL;
}

void mb_free_handler(mb_handler_t *handler)
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();
    if (handler->used == 1) {
        handler->used = 0;
    }
    MB_CRITICAL_EXIT();
}

mb_status_t mb_mutex_create(MB_MUTEX_T *mutex)
{
    return aos_mutex_create(mutex, 0);
}

mb_status_t mb_mutex_lock(MB_MUTEX_T *mutex)
{
    return aos_mutex_lock(mutex, AOS_WAIT_FOREVER);
}

mb_status_t mb_mutex_unlock(MB_MUTEX_T *mutex)
{
    return aos_mutex_unlock(mutex);
}

mb_status_t mb_mutex_del(MB_MUTEX_T *mutex)
{
    aos_mutex_free(mutex);
    return 0;
}
