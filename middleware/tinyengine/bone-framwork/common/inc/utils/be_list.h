#ifndef _LINUX_LIST_H_
#define _LINUX_LIST_H_

#include <stddef.h>
#include <stdbool.h>

#define	prefetch(x)

#ifndef container_of
#define container_of(ptr, type, member)					\
	({								\
		const typeof(((type *) NULL)->member) *__mptr = (ptr);	\
		(type *) ((char *) __mptr - offsetof(type, member));	\
	})
#endif

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#undef LIST_HEAD
#define LIST_HEAD(name)	struct list_head name = LIST_HEAD_INIT(name)

static inline void
INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list->prev = list;
}

static inline bool
list_empty(const struct list_head *head)
{
	return (head->next == head);
}

static inline bool
list_is_first(const struct list_head *list,
	      const struct list_head *head)
{
	return list->prev == head;
}

static inline bool
list_is_last(const struct list_head *list,
	     const struct list_head *head)
{
	return list->next == head;
}

static inline void
_list_del(struct list_head *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

static inline void
list_del(struct list_head *entry)
{
	_list_del(entry);
	entry->next = entry->prev = NULL;
}

static inline void
_list_add(struct list_head *_new, struct list_head *prev,
    struct list_head *next)
{

	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static inline void
list_del_init(struct list_head *entry)
{
	_list_del(entry);
	INIT_LIST_HEAD(entry);
}


#define	list_entry(ptr, type, field)	container_of(ptr, type, field)
#define	list_first_entry(ptr, type, field)	list_entry((ptr)->next, type, field)
#define	list_last_entry(ptr, type, field)	list_entry((ptr)->prev, type, field)

#define	list_for_each(p, head)						\
	for (p = (head)->next; p != (head); p = p->next)

#define	list_for_each_safe(p, n, head)					\
	for (p = (head)->next, n = p->next; p != (head); p = n, n = p->next)

#define list_for_each_entry(p, h, field)				\
	for (p = list_first_entry(h, typeof(*p), field); &p->field != (h); \
	    p = list_entry(p->field.next, typeof(*p), field))

#define list_for_each_entry_safe(p, n, h, field)			\
	for (p = list_first_entry(h, typeof(*p), field),		\
	    n = list_entry(p->field.next, typeof(*p), field); &p->field != (h);\
	    p = n, n = list_entry(n->field.next, typeof(*n), field))

#define	list_for_each_entry_reverse(p, h, field)			\
	for (p = list_last_entry(h, typeof(*p), field); &p->field != (h); \
	    p = list_entry(p->field.prev, typeof(*p), field))

#define	list_for_each_prev(p, h) for (p = (h)->prev; p != (h); p = p->prev)
#define	list_for_each_prev_safe(p, n, h) for (p = (h)->prev, n = p->prev; p != (h); p = n, n = p->prev)

static inline void
list_add(struct list_head *_new, struct list_head *head)
{
	_list_add(_new, head, head->next);
}

static inline void
list_add_tail(struct list_head *_new, struct list_head *head)
{
	_list_add(_new, head->prev, head);
}

static inline void
list_move(struct list_head *list, struct list_head *head)
{
	_list_del(list);
	list_add(list, head);
}

static inline void
list_move_tail(struct list_head *entry, struct list_head *head)
{
	_list_del(entry);
	list_add_tail(entry, head);
}

static inline void
_list_splice(const struct list_head *list, struct list_head *prev,
    struct list_head *next)
{
	struct list_head *first;
	struct list_head *last;

	if (list_empty(list))
		return;

	first = list->next;
	last = list->prev;
	first->prev = prev;
	prev->next = first;
	last->next = next;
	next->prev = last;
}

static inline void
list_splice(const struct list_head *list, struct list_head *head)
{
	_list_splice(list, head, head->next);
}

static inline void
list_splice_tail(struct list_head *list, struct list_head *head)
{
	_list_splice(list, head->prev, head);
}

static inline void
list_splice_init(struct list_head *list, struct list_head *head)
{
	_list_splice(list, head, head->next);
	INIT_LIST_HEAD(list);
}

static inline void
list_splice_tail_init(struct list_head *list, struct list_head *head)
{
	_list_splice(list, head->prev, head);
	INIT_LIST_HEAD(list);
}

#endif /* _LINUX_LIST_H_ */
