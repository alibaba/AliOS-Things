/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_LIST_H
#define BE_LIST_H

#include <stdbool.h>
#include <stddef.h>

#define prefetch(x)

#ifndef container_of
#define container_of(ptr, type, member)                       \
    ({                                                        \
        const typeof(((type *)NULL)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member));    \
    })
#endif

struct be_list_head {
    struct be_list_head *next;
    struct be_list_head *prev;
};

#define BE_LIST_HEAD_INIT(name) \
    {                           \
        &(name), &(name)        \
    }
#undef BE_LIST_HEAD
#define BE_LIST_HEAD(name) struct be_list_head name = BE_LIST_HEAD_INIT(name)

static inline void BE_INIT_LIST_HEAD(struct be_list_head *list)
{
    list->next = list->prev = list;
}

static inline bool be_list_empty(const struct be_list_head *head)
{
    return (head->next == head);
}

static inline bool be_list_is_first(const struct be_list_head *list,
                                    const struct be_list_head *head)
{
    return list->prev == head;
}

static inline bool be_list_is_last(const struct be_list_head *list,
                                   const struct be_list_head *head)
{
    return list->next == head;
}

static inline void _be_list_del(struct be_list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

static inline void be_list_del(struct be_list_head *entry)
{
    _be_list_del(entry);
    entry->next = entry->prev = NULL;
}

static inline void _be_list_add(struct be_list_head *_new,
                                struct be_list_head *prev,
                                struct be_list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static inline void be_list_del_init(struct be_list_head *entry)
{
    _be_list_del(entry);
    BE_INIT_LIST_HEAD(entry);
}

#define be_list_entry(ptr, type, field) container_of(ptr, type, field)
#define be_list_first_entry(ptr, type, field) \
    be_list_entry((ptr)->next, type, field)
#define be_list_last_entry(ptr, type, field) \
    be_list_entry((ptr)->prev, type, field)

#define be_list_for_each(p, head) \
    for (p = (head)->next; p != (head); p = p->next)

#define be_list_for_each_safe(p, n, head) \
    for (p = (head)->next, n = p->next; p != (head); p = n, n = p->next)

#define be_list_for_each_entry(p, h, field)                               \
    for (p = be_list_first_entry(h, typeof(*p), field); &p->field != (h); \
         p = be_list_entry(p->field.next, typeof(*p), field))

#define be_list_for_each_entry_safe(p, n, h, field)           \
    for (p = be_list_first_entry(h, typeof(*p), field),       \
        n  = be_list_entry(p->field.next, typeof(*p), field); \
         &p->field != (h);                                    \
         p = n, n = be_list_entry(n->field.next, typeof(*n), field))

#define be_list_for_each_entry_reverse(p, h, field)                      \
    for (p = be_list_last_entry(h, typeof(*p), field); &p->field != (h); \
         p = be_list_entry(p->field.prev, typeof(*p), field))

#define be_list_for_each_prev(p, h) for (p = (h)->prev; p != (h); p = p->prev)
#define be_list_for_each_prev_safe(p, n, h) \
    for (p = (h)->prev, n = p->prev; p != (h); p = n, n = p->prev)

static inline void be_list_add(struct be_list_head *_new,
                               struct be_list_head *head)
{
    _be_list_add(_new, head, head->next);
}

static inline void be_list_add_tail(struct be_list_head *_new,
                                    struct be_list_head *head)
{
    _be_list_add(_new, head->prev, head);
}

static inline void be_list_move(struct be_list_head *list,
                                struct be_list_head *head)
{
    _be_list_del(list);
    be_list_add(list, head);
}

static inline void be_list_move_tail(struct be_list_head *entry,
                                     struct be_list_head *head)
{
    _be_list_del(entry);
    be_list_add_tail(entry, head);
}

static inline void _be_list_splice(const struct be_list_head *list,
                                   struct be_list_head *prev,
                                   struct be_list_head *next)
{
    struct be_list_head *first;
    struct be_list_head *last;

    if (be_list_empty(list)) return;

    first       = list->next;
    last        = list->prev;
    first->prev = prev;
    prev->next  = first;
    last->next  = next;
    next->prev  = last;
}

static inline void be_list_splice(const struct be_list_head *list,
                                  struct be_list_head *head)
{
    _be_list_splice(list, head, head->next);
}

static inline void be_list_splice_tail(struct be_list_head *list,
                                       struct be_list_head *head)
{
    _be_list_splice(list, head->prev, head);
}

static inline void be_list_splice_init(struct be_list_head *list,
                                       struct be_list_head *head)
{
    _be_list_splice(list, head, head->next);
    BE_INIT_LIST_HEAD(list);
}

static inline void be_list_splice_tail_init(struct be_list_head *list,
                                            struct be_list_head *head)
{
    _be_list_splice(list, head->prev, head);
    BE_INIT_LIST_HEAD(list);
}

#endif /* BE_LIST_H */
