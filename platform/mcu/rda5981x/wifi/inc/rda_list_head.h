#ifndef _WLNAD_LIST_HEAD_H_
#define _WLNAD_LIST_HEAD_H_
#include <stddef.h>

#define _2offsetof(type, member) ((long) &((type *) 0)->member)
#if !defined(__ICCARM__)
#define container_of(ptr, type, member) ({        \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - _2offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each_entry_safe(pos, n, head, member)        \
    for (pos = list_entry((head)->next, typeof(*pos), member),    \
    n = list_entry(pos->member.next, typeof(*pos), member);    \
         &pos->member != (head);             \
         pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry(pos, head, member)        \
        for (pos = list_entry((head)->next, typeof(*pos), member);  \
             &pos->member != (head);    \
             pos = list_entry(pos->member.next, typeof(*pos), member))
#else
#define container_of(ptr, type, member) (        \
    (type *)( (char *)(ptr) - _2offsetof(type,member) ))

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each_entry_safe(pos, n, head, member)        \
    for (pos = list_entry((head)->next, wland_scan_result, member),    \
    n = list_entry(pos->member.next, wland_scan_result, member);    \
         &pos->member != (head);             \
         pos = n, n = list_entry(n->member.next, wland_scan_result, member))

#define list_for_each_entry(pos, head, member)        \
        for (pos = list_entry((head)->next, wland_scan_result, member);  \
             &pos->member != (head);    \
             pos = list_entry(pos->member.next, wland_scan_result, member))
#endif

struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *new,
              struct list_head *prev,
              struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}


#endif /*_WLNAD_LIST_HEAD_H_*/
