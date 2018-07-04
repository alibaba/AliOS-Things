#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "queue.h"
#include "iot_import.h"
#include "lite-utils.h"

typedef struct {
    void *data;
} node_t;

struct lk_queue_s {
    int rdx;
    int wdx;

    int max_nodes;
    node_t *nodes;

    void *lock;
    void *sem;
};

lk_queue_t *lk_queue_new(int max_nodes)
{
    lk_queue_t *q = LITE_malloc(sizeof(lk_queue_t));
    if (!q)
        return NULL;
    memset(q, 0, sizeof(lk_queue_t));

    q->nodes = LITE_calloc(max_nodes, sizeof(node_t));
    if (!q->nodes) {
        LITE_free(q);
        return NULL;
    }

    q->max_nodes = max_nodes;

    q->lock = HAL_MutexCreate();
    if (!q->lock) {
        LITE_free(q->nodes);
        LITE_free(q);
        return NULL;
    }

    q->sem = HAL_SemaphoreCreate();
    if (!q->sem) {
        HAL_MutexDestroy(q->lock);
        LITE_free(q->nodes);
        LITE_free(q);
        return NULL;
    }

    return q;
}

int lk_queue_free(lk_queue_t *q)
{
    if (q->nodes)
        LITE_free(q->nodes);

    if (q->lock)
        HAL_MutexDestroy(q->lock);

    if (q->sem)
        HAL_SemaphoreDestroy(q->sem);

    LITE_free(q);

    return 0;
}

inline static int queue_lenght(lk_queue_t *q)
{
    return (unsigned int)(q->wdx - q->rdx);
}

inline static int queue_empty(lk_queue_t *q)
{
    return (queue_lenght(q) == 0);
}

inline static int queue_full(lk_queue_t *q)
{
    return (queue_lenght(q) == q->max_nodes);
}

void *lk_queue_get(lk_queue_t *q, uint32_t timeout_ms)
{
    if (HAL_SemaphoreWait(q->sem, timeout_ms) < 0)
        return NULL;

    HAL_MutexLock(q->lock);
    if (queue_empty(q)) {
        HAL_MutexUnlock(q->lock);
        return NULL;
    }

    void *data = q->nodes[q->rdx % q->max_nodes].data;
    q->rdx++;

    HAL_MutexUnlock(q->lock);

    return data;
}

int lk_queue_put(lk_queue_t *q, void *data)
{
    HAL_MutexLock(q->lock);
    if (queue_full(q)) {
        HAL_MutexUnlock(q->lock);
        return -1;
    }

    q->nodes[q->wdx % q->max_nodes].data = data;
    q->wdx++;

    HAL_MutexUnlock(q->lock);

    HAL_SemaphorePost(q->sem);

    return 0;
}
