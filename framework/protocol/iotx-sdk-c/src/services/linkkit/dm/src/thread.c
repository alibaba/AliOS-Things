#include "lite-utils.h"
#include "iot_import.h"

#include "thread.h"

struct lk_thread_s {
    void *tid;
    void *sem;

    void *stack;
    int stack_size;

    void *(*thread)(void *arg);
    void *arg;
};

static void *lk_detached_thread(void *arg)
{
    lk_thread_t *thr = arg;

    HAL_SemaphoreWait(thr->sem, PLATFORM_WAIT_INFINITE);

    void *ret = thr->thread(thr->arg);

    HAL_SemaphoreDestroy(thr->sem);
    LITE_free(thr);

    return ret;
}

int lk_thread_create_detached(char *name, int stack_size, void *(*thread)(void *arg), void *arg)
{
    if (stack_size <= 0 || !thread)
        return -1;

    lk_thread_t *thr = LITE_malloc(sizeof(lk_thread_t));
    if (!thr)
        return -1;
    memset(thr, 0, sizeof(lk_thread_t));

    thr->thread = thread;
    thr->arg = arg;
    thr->stack_size = stack_size;

    thr->stack = LITE_malloc(stack_size);
    if (!thr->stack) {
        LITE_free(thr);
        return -1;
    }

    thr->sem = HAL_SemaphoreCreate();
    if (!thr->sem) {
        LITE_free(thr->stack);
        LITE_free(thr);
        return -1;
    }

    hal_os_thread_param_t threadParams = {
        .priority = os_thread_priority_normal,
        .stack_addr = thr->stack,
        .stack_size = thr->stack_size,
        .detach_state = 1,
        .name = name,
    };

    int stack_used = 0;

    if (HAL_ThreadCreate(&thr->tid, lk_detached_thread, thr, &threadParams, &stack_used) < 0) {
        HAL_SemaphoreDestroy(thr->sem);
        LITE_free(thr->stack);
        LITE_free(thr);
        return -1;
    }

    HAL_ThreadDetach(thr->tid);

    if (stack_used == 0) {
        LITE_free(thr->stack);
        thr->stack = NULL;
        thr->stack_size = 0;
    }

    HAL_SemaphorePost(thr->sem);    /* wake up lk_detached_thread */

    return 0;
}

static void *lk_thread_wrapper(void *arg)
{
    lk_thread_t *thr = arg;

    void *ret = thr->thread(thr->arg);

    HAL_SemaphorePost(thr->sem);

    return ret;
}

lk_thread_t *lk_thread_create(char *name, int stack_size, void *(*thread)(void *arg), void *arg)
{
    if (stack_size <= 0 || !thread)
        return NULL;

    lk_thread_t *thr = LITE_malloc(sizeof(lk_thread_t));
    if (!thr)
        return NULL;
    memset(thr, 0, sizeof(lk_thread_t));

    thr->thread = thread;
    thr->arg = arg;
    thr->stack_size = stack_size;

    thr->stack = LITE_malloc(stack_size);
    if (!thr->stack) {
        LITE_free(thr);
        return NULL;
    }

    thr->sem = HAL_SemaphoreCreate();
    if (!thr->sem) {
        LITE_free(thr->stack);
        LITE_free(thr);
        return NULL;
    }

    hal_os_thread_param_t threadParams = {
        .priority = os_thread_priority_normal,
        .stack_addr = thr->stack,
        .stack_size = thr->stack_size,
        .detach_state = 1,
        .name = name,
    };

    int stack_used = 0;

    if (HAL_ThreadCreate(&thr->tid, lk_thread_wrapper, thr, &threadParams, &stack_used) < 0) {
        HAL_SemaphoreDestroy(thr->sem);
        LITE_free(thr->stack);
        LITE_free(thr);
        return NULL;
    }

    HAL_ThreadDetach(thr->tid);

    if (stack_used == 0) {
        LITE_free(thr->stack);
        thr->stack = NULL;
        thr->stack_size = 0;
    }

    return thr;
}

int lk_thread_join(lk_thread_t *thr)
{
    /* wait thread to exit */
    if (thr->sem) {
        HAL_SemaphoreWait(thr->sem, PLATFORM_WAIT_INFINITE);
        HAL_SemaphoreDestroy(thr->sem);
    }

    if (thr->stack)
        LITE_free(thr->stack);

    LITE_free(thr);

    return 0;
}
