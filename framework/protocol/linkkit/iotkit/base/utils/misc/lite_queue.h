#ifndef LITE_QUEUE_H
#define LITE_QUEUE_H

#include <stdlib.h>
#include <stdarg.h>

typedef struct _lite_queue_t
{
    void*  lite_queue_array;
    void*  lite_queue_mutex;
    size_t queue_size;
    size_t queue_array_size;
    size_t front;
    size_t rear;
} lite_queue_t;

typedef void (*handle_queue_item_fp_t)(void* queue_item, va_list* params);

lite_queue_t* lite_queue_create(int queue_size, int synchronized);
void lite_queue_destroy(lite_queue_t* _lite_queue);
int lite_queue_insert(lite_queue_t* _lite_queue, void *data);
void* lite_queue_delete(lite_queue_t* _lite_queue);
void* lite_queue_first(lite_queue_t* _lite_queue);
int lite_queue_is_empty(lite_queue_t* _lite_queue);
int lite_queue_is_full(lite_queue_t* _lite_queue);
void lite_queue_iterator(lite_queue_t* _lite_queue, handle_queue_item_fp_t handle_fn, ...);

#endif /* LITE_QUEUE_H */
