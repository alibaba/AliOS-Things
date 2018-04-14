#include <stdio.h>

#include "iot_import.h"
#include "lite_queue.h"

lite_queue_t* lite_queue_create(int queue_size, int synchronized)
{
    lite_queue_t* lite_queue;

    if (queue_size <= 0) return NULL;

    lite_queue = calloc(1, sizeof(lite_queue_t));

    if (lite_queue == NULL) goto err_handler;

    lite_queue->queue_size = queue_size;
    lite_queue->queue_array_size = queue_size + 1;

    lite_queue->lite_queue_array = calloc(lite_queue->queue_array_size, sizeof(void*));

    if (lite_queue->lite_queue_array == NULL) goto err_handler;

    if (synchronized) {
        lite_queue->lite_queue_mutex = HAL_MutexCreate();

        if (lite_queue->lite_queue_mutex == NULL) {
            goto err_handler;
        }
    }

    lite_queue->front = 1;
    lite_queue->rear = 0;

    return lite_queue;

err_handler:
    if (lite_queue && lite_queue->lite_queue_array) free(lite_queue->lite_queue_array);

    if (lite_queue && lite_queue->lite_queue_mutex) {
        HAL_MutexDestroy(lite_queue->lite_queue_mutex);
        lite_queue->lite_queue_mutex = NULL;
    }

    if (lite_queue) free(lite_queue);

    return lite_queue;
}

void lite_queue_destroy(lite_queue_t* _lite_queue)
{
    lite_queue_t* lite_queue = _lite_queue;

    if(!_lite_queue) return;

    if (lite_queue && lite_queue->lite_queue_array) {
        free(lite_queue->lite_queue_array);
    }

    if (lite_queue->lite_queue_mutex) {
        HAL_MutexDestroy(lite_queue->lite_queue_mutex);
        lite_queue->lite_queue_mutex = NULL;
    }

    if (lite_queue) free(lite_queue);
}

int lite_queue_insert(lite_queue_t* _lite_queue, void* data)
{
    lite_queue_t* lite_queue = _lite_queue;

    if(!_lite_queue) return -1;

    if (lite_queue_is_full(lite_queue)) return -1;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);

    lite_queue->rear = (lite_queue->rear + 1) % lite_queue->queue_array_size;

    *(void**)((char*)lite_queue->lite_queue_array + lite_queue->rear *  sizeof(void*)) = data;

    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);

    return 0;
}

void* lite_queue_delete(lite_queue_t* _lite_queue)
{
    lite_queue_t* lite_queue = _lite_queue;
    void* data;

    if (!_lite_queue || lite_queue_is_empty(lite_queue)) return NULL;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);

    data = *(void**)((char*)lite_queue->lite_queue_array + lite_queue->front * (sizeof(void*)));

    lite_queue->front = (lite_queue->front + 1) % lite_queue->queue_array_size;

    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);

    return data;
}

void* lite_queue_first(lite_queue_t* _lite_queue)
{
    lite_queue_t* lite_queue = _lite_queue;
    void* data;

    if(!_lite_queue) return NULL;

    if (lite_queue_is_empty(lite_queue)) return NULL;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);

    data = *(void**)((char*)lite_queue->lite_queue_array + lite_queue->front * (sizeof(void*)));

    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);

    return data;
}

int lite_queue_is_empty(lite_queue_t* _lite_queue)
{
    lite_queue_t* lite_queue = _lite_queue;
    int is_empty;

    if(!_lite_queue) return -1;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);
    is_empty = (lite_queue->rear + 1) % lite_queue->queue_array_size == lite_queue->front;
    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);

    return is_empty;
}

int lite_queue_is_full(lite_queue_t* _lite_queue)
{
    lite_queue_t* lite_queue = _lite_queue;
    int is_full;

    if(!_lite_queue) return -1;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);
    is_full = (lite_queue->rear + 2) % lite_queue->queue_array_size == lite_queue->front;
    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);

    return is_full;
}

static void _lite_queue_iterator(lite_queue_t* _lite_queue, handle_queue_item_fp_t handle_fn, va_list* params)
{
    lite_queue_t* lite_queue = _lite_queue;
    void* data;
    size_t index;

    if(!_lite_queue) return;

    if (lite_queue->lite_queue_mutex) HAL_MutexLock(lite_queue->lite_queue_mutex);

    for (index = lite_queue->front; index <= lite_queue->rear + (lite_queue->front <= lite_queue->rear ? 0 : lite_queue->queue_array_size); ++index) {
        data = *(void**)((char*)lite_queue->lite_queue_array + index % lite_queue->queue_array_size * (sizeof(void*)));

        va_list args;
        va_copy(args, *params);

        handle_fn(data, &args);

        va_end(args);
    }

    if (lite_queue->lite_queue_mutex) HAL_MutexUnlock(lite_queue->lite_queue_mutex);
}

void lite_queue_iterator(lite_queue_t* _lite_queue, handle_queue_item_fp_t handle_fn, ...)
{
    lite_queue_t* lite_queue = _lite_queue;

    if(!lite_queue || !handle_fn || lite_queue_is_empty(lite_queue)) return;

    va_list params;

    va_start(params, handle_fn);

    _lite_queue_iterator(lite_queue, handle_fn, &params);

    va_end(params);
}
