//
//  blocking_queue.h
//  pal_sdk
//

#ifndef blocking_queue_h
#define blocking_queue_h

#include "doubly_list.h"

struct blocking_queue_t {
    void *mutex;
    struct list_entry_t *list;
    int length;
};

int blocking_queue_init (struct blocking_queue_t **q);
void blocking_queue_destroy (struct blocking_queue_t **q, void (*free_fn)(void *));
int blocking_queue_add (struct blocking_queue_t *q, void *elem, int raw);
int blocking_queue_get (struct blocking_queue_t *q, int timeout_ms, void **elem, int raw);
int blocking_queue_peek(struct blocking_queue_t *q, void **elem, int tail);

#endif /* blocking_queue_h */
