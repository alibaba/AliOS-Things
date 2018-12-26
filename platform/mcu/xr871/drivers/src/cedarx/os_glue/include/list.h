#ifndef __MELIS_PTHREAD_LIST_H__
#define __MELIS_PTHREAD_LIST_H__

//#define prefetch(x)				x

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


static __inline void __list_add(struct list_head * new,
	struct list_head * prev,
	struct list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static __inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static __inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static __inline void __list_del(struct list_head * prev,
				  struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static __inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static __inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry); 
}

static __inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

static __inline void list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;

	if (first != list) {
		struct list_head *last = list->prev;
		struct list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

static __inline int list_valid_check(struct list_head *head)
{
    struct list_head *next = head->next;
	if(next == head)
        return 0;
    while(next!=head)
    {
        next = next->next;
        if(next == next->prev)
        {
            return -1;
        }
    }
    return 1;
}

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head) \
	for (pos = (head)->next /*, prefetch(pos->next)*/; pos != (head); \
        	pos = pos->next /*, prefetch(pos->next)*/)
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; /*prefetch(pos->prev), */pos != (head); \
			pos = pos->prev)

        	
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head) \
		for (pos = (head)->prev, n = pos->prev; pos != (head); \
			pos = n, n = pos->prev)

#endif /* __MELIS_PTHREAD_LIST_H__ */

