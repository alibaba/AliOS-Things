/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _LIST_H_
#define _LIST_H_
#include "tee_types.h"

typedef struct _list_t
{
    struct _list_t *next;
    struct _list_t *prev;
} list_t;

/* initialize list head staticly */
#define LIST_INIT(list)  \
    {                    \
        &(list), &(list) \
    }

static inline void list_init(list_t *head)
{
    head->prev = head;
    head->next = head;
}

static inline void list_add(list_t *head, list_t *elem)
{
    elem->next       = head->next;
    head->next->prev = elem;
    head->next       = elem;
    elem->prev       = head;
}

static inline void list_add_tail(list_t *head, list_t *elem)
{
    elem->prev       = head->prev;
    head->prev->next = elem;
    head->prev       = elem;
    elem->next       = head;
}

#define list_add_before(entry, new_entry) list_add_tail(entry, new_entry)

static inline void list_del(list_t *elem)
{
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    elem->prev = elem->next = 0;
}

static inline list_t *list_prev(list_t *list, list_t *item)
{
    if (item->prev != list) {
        return item->prev;
    } else {
        return NULL;
    }
}

static inline list_t *list_next(list_t *list, list_t *item)
{
    if (item->next != list) {
        return item->next;
    } else {
        return NULL;
    }
}

#define list_empty(list) ((list)->next == list)

#define list_entry(addr, type, member)                \
    ({                                                \
        type tmp;                                     \
        int  offset = (int)(&tmp.member) - (int)&tmp; \
        (type *)((int)addr - offset);                 \
    })

#define list_iterate(head, entry) \
    for ((entry) = (head)->next; (entry) != (head); (entry) = (entry)->next)

#define list_iterate_safe(head, entry, n)                        \
    for (entry = (head)->next, n = entry->next; entry != (head); \
         entry = n, n = entry->next)

#endif /* _LIST_H_ */
