/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_LIST_H_
#define _TEE_LIST_H_
#include "tee_types.h"

typedef struct _tee_list_t
{
    struct _tee_list_t *next;
    struct _tee_list_t *prev;
} tee_list_t;

/* initialize list head staticly */
#define TEE_LIST_INIT(list) \
    {                       \
        &(list), &(list)    \
    }

static inline void tee_list_init(tee_list_t *head)
{
    head->prev = head;
    head->next = head;
}

static inline void tee_list_add(tee_list_t *head, tee_list_t *elem)
{
    elem->next       = head->next;
    head->next->prev = elem;
    head->next       = elem;
    elem->prev       = head;
}

static inline void tee_list_add_tail(tee_list_t *head, tee_list_t *elem)
{
    elem->prev       = head->prev;
    head->prev->next = elem;
    head->prev       = elem;
    elem->next       = head;
}

#define tee_list_add_before(entry, new_entry) \
    tee_list_add_tail(entry, new_entry)

static inline void tee_list_del(tee_list_t *elem)
{
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    elem->prev = elem->next = 0;
}

static inline tee_list_t *tee_list_prev(tee_list_t *list, tee_list_t *item)
{
    if (item->prev != list) {
        return item->prev;
    } else {
        return NULL;
    }
}

static inline tee_list_t *tee_list_next(tee_list_t *list, tee_list_t *item)
{
    if (item->next != list) {
        return item->next;
    } else {
        return NULL;
    }
}

#define tee_list_empty(list) ((list)->next == list)

#define tee_list_entry(addr, type, member)            \
    ({                                                \
        type tmp;                                     \
        int  offset = (int)(&tmp.member) - (int)&tmp; \
        (type *)((int)addr - offset);                 \
    })

#define tee_list_iterate(head, entry) \
    for ((entry) = (head)->next; (entry) != (head); (entry) = (entry)->next)

#define tee_list_iterate_safe(head, entry, n)                    \
    for (entry = (head)->next, n = entry->next; entry != (head); \
         entry = n, n = entry->next)

#endif /* _TEE_LIST_H_ */
