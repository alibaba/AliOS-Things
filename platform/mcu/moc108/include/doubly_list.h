#ifndef _DOUBLY_LIST_H
#define _DOUBLY_LIST_H

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#define list_entry(ptr, type, member) ((type *)((char *)ptr - offsetof(type,member)))

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
typedef struct list_head
{
	struct list_head *next, *prev;
}LIST_HEADER_T;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD_DEFINE(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new_node entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
__INLINE void __list_add(struct list_head *new_node, struct list_head *prev, struct list_head *next)
{
	next->prev = new_node;
	new_node->next = next;
	new_node->prev = prev;
	prev->next = new_node;
}

/**
 * list_add - add a new_node entry
 * @new_node: new_node entry to be added
 * @head: list head to add it after
 *
 * Insert a new_node entry after the specified head.
 * This is good for implementing stacks.
 */
__INLINE void list_add_head(struct list_head *new_node, struct list_head *head)
{
	__list_add(new_node, head, head->next);
}

/**
 * list_add_tail - add a new_node entry
 * @new_node: new_node entry to be added
 * @head: list head to add it before
 *
 * Insert a new_node entry before the specified head.
 * This is useful for implementing queues.
 */
__INLINE void list_add_tail(struct list_head *new_node, struct list_head *head)
{
	__list_add(new_node, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
__INLINE void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
__INLINE void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
__INLINE void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
__INLINE void list_move(struct list_head *list, struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_head(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
__INLINE void list_move_tail(struct list_head *list,
				  struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
__INLINE unsigned int list_empty(const struct list_head *head)
{
	return head->next == head;
}

__INLINE void __list_splice(struct list_head *list,
				 struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 */
__INLINE void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new_node list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
__INLINE void list_splice_init(struct list_head *list,
				    struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
	}
}

__INLINE void list_switch(struct list_head **list1,
				    struct list_head ** list2)
{
	struct list_head * temp;

	temp = *list1;
	*list1 = *list2;
    *list2 = temp;
}



/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


/**
 * list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop counter.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_size
 * @head:	the head for your list.
 */
__INLINE unsigned int list_size(struct list_head *list) {
	unsigned int num_of_list=0;
	struct list_head *n, *pos;
        
	list_for_each_safe(pos, n, list)
		num_of_list++;

    return num_of_list;
}

#endif /* _DOUBLY_LIST_H */
