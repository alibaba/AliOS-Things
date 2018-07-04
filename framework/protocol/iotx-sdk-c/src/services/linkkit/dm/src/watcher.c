#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"

#include "watcher.h"
#include "mempool.h"
#include "lite-log.h"
#include "iotx_dm_common.h"

struct watcher_s {
    int id;
    void *sem;

    int code;
    void *data;
    int data_len;

    struct watcher_s *next;
};

static void *mutex = NULL;
static watcher_t *first_watcher = NULL;        /* watcher list head */
static AMemPool *watcher_pool = NULL;

static int init_watcher(void *ptr)
{
    watcher_t *w = (watcher_t *)ptr;
    if (!w)
        return -1;
    memset(w, 0, sizeof(watcher_t));

    w->sem = HAL_SemaphoreCreate();
    if (!w->sem)
        return -1;

    return 0;
}

static int free_watcher(void *ptr)
{
    watcher_t *w = (watcher_t *)ptr;
    if (!w)
        return -1;

    if (w->sem) {
        HAL_SemaphoreDestroy(w->sem);
        w->sem = NULL;
    }

    return 0;
}

int watcher_init(void)
{
    watcher_pool = AMemPool_New(sizeof(watcher_t), 4, init_watcher, free_watcher);
    if (!watcher_pool)
        return -1;

    mutex = HAL_MutexCreate();
    if (!mutex) {
        AMemPool_Free(watcher_pool);
        watcher_pool = NULL;
        return -1;
    }

    return 0;
}

int watcher_exit(void)
{
    if (watcher_pool) {
        AMemPool_Free(watcher_pool);
        watcher_pool = NULL;
    }

    if (mutex) {
        HAL_MutexDestroy(mutex);
        mutex = NULL;
    }

    return 0;
}

watcher_t *watcher_new(int id)
{
    watcher_t *w = AMemPool_Get(watcher_pool);
    if (!w)
        return NULL;
    /* do not memset watcher instance */

    w->id = id;
    w->code = 0;
    w->data = NULL;
    w->data_len = 0;

    HAL_MutexLock(mutex);
    w->next = first_watcher;
    first_watcher = w;
    HAL_MutexUnlock(mutex);

    return w;
}

int watcher_free(watcher_t *w)
{
    watcher_t **wp, *w1;

    HAL_MutexLock(mutex);
    wp = &first_watcher;
    while ((*wp) != NULL) {
        w1 = *wp;
        if (w1 == w)
            *wp = w->next;
        else
            wp = &w1->next;
    }
    HAL_MutexUnlock(mutex);

    /* do not memset watcher instance */

    AMemPool_Put(watcher_pool, w);

    return 0;
}

int watcher_wait(watcher_t *w, int *code, char **data, int *data_len, int timeout_ms)
{
    if (HAL_SemaphoreWait(w->sem, timeout_ms) < 0) {
        dm_log_info("sem wait timeout\n");
        return -1;
    }

    if (code)
        *code = w->code;

    if (data)
        *data = w->data;

    if (data_len)
        *data_len = w->data_len;

    return 0;
}

int watcher_signal(int id, int code, void *data, int data_len)
{
    watcher_t *w, *w_next;

    HAL_MutexLock(mutex);
    for (w = first_watcher; w; w = w_next) {
        w_next = w->next;
        if (w->id == id) {
            w->code = code;
            w->data = data;
            w->data_len = data_len;
            HAL_SemaphorePost(w->sem);
            break;
        }
    }
    HAL_MutexUnlock(mutex);

    return 0;
}
