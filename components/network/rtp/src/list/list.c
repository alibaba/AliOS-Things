/**
 * @file list.c  Linked List implementation
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re_types.h>
#include <re_list.h>
#include <re_mem.h>


#define DEBUG_MODULE "list"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


/**
 * Initialise a linked list
 *
 * @param list Linked list
 */
void list_init(struct list *list)
{
	if (!list)
		return;

	list->head = NULL;
	list->tail = NULL;
}


/**
 * Flush a linked list and free all elements
 *
 * @param list Linked list
 */
void list_flush(struct list *list)
{
	struct le *le;

	if (!list)
		return;

	le = list->head;
	while (le) {
		struct le *next = le->next;
		void *data = le->data;
		le->list = NULL;
		le->prev = le->next = NULL;
		le->data = NULL;
		le = next;
		mem_deref(data);
	}

	list_init(list);
}


/**
 * Clear a linked list without dereferencing the elements
 *
 * @param list Linked list
 */
void list_clear(struct list *list)
{
	struct le *le;

	if (!list)
		return;

	le = list->head;
	while (le) {
		struct le *next = le->next;
		le->list = NULL;
		le->prev = le->next = NULL;
		le->data = NULL;
		le = next;
	}

	list_init(list);
}


/**
 * Append a list element to a linked list
 *
 * @param list  Linked list
 * @param le    List element
 * @param data  Element data
 */
void list_append(struct list *list, struct le *le, void *data)
{
	if (!list || !le)
		return;

	if (le->list) {
		DEBUG_WARNING("append: le linked to %p\n", le->list);
		return;
	}

	le->prev = list->tail;
	le->next = NULL;
	le->list = list;
	le->data = data;

	if (!list->head)
		list->head = le;

	if (list->tail)
		list->tail->next = le;

	list->tail = le;
}


/**
 * Prepend a list element to a linked list
 *
 * @param list  Linked list
 * @param le    List element
 * @param data  Element data
 */
void list_prepend(struct list *list, struct le *le, void *data)
{
	if (!list || !le)
		return;

	if (le->list) {
		DEBUG_WARNING("prepend: le linked to %p\n", le->list);
		return;
	}

	le->prev = NULL;
	le->next = list->head;
	le->list = list;
	le->data = data;

	if (list->head)
		list->head->prev = le;

	if (!list->tail)
		list->tail = le;

	list->head = le;
}


/**
 * Insert a list element before a given list element
 *
 * @param list  Linked list
 * @param le    Given list element
 * @param ile   List element to insert
 * @param data  Element data
 */
void list_insert_before(struct list *list, struct le *le, struct le *ile,
		       void *data)
{
	if (!list || !le || !ile)
		return;

	if (ile->list) {
		DEBUG_WARNING("insert_before: le linked to %p\n", le->list);
		return;
	}

	if (le->prev)
		le->prev->next = ile;
	else if (list->head == le)
		list->head = ile;

	ile->prev = le->prev;
	ile->next = le;
	ile->list = list;
	ile->data = data;

	le->prev = ile;
}


/**
 * Insert a list element after a given list element
 *
 * @param list  Linked list
 * @param le    Given list element
 * @param ile   List element to insert
 * @param data  Element data
 */
void list_insert_after(struct list *list, struct le *le, struct le *ile,
		       void *data)
{
	if (!list || !le || !ile)
		return;

	if (ile->list) {
		DEBUG_WARNING("insert_after: le linked to %p\n", le->list);
		return;
	}

	if (le->next)
		le->next->prev = ile;
	else if (list->tail == le)
		list->tail = ile;

	ile->prev = le;
	ile->next = le->next;
	ile->list = list;
	ile->data = data;

	le->next = ile;
}


/**
 * Remove a list element from a linked list
 *
 * @param le    List element to remove
 */
void list_unlink(struct le *le)
{
	struct list *list;

	if (!le || !le->list)
		return;

	list = le->list;

	if (le->prev)
		le->prev->next = le->next;
	else
		list->head = le->next;

	if (le->next)
		le->next->prev = le->prev;
	else
		list->tail = le->prev;

	le->next = NULL;
	le->prev = NULL;
	le->list = NULL;
}


/**
 * Sort a linked list in an order defined by the sort handler
 *
 * @param list  Linked list
 * @param sh    Sort handler
 * @param arg   Handler argument
 */
void list_sort(struct list *list, list_sort_h *sh, void *arg)
{
	struct le *le;
	bool sort;

	if (!list || !sh)
		return;

 retry:
	le = list->head;
	sort = false;

	while (le && le->next) {

		if (sh(le, le->next, arg)) {

			le = le->next;
		}
		else {
			struct le *tle = le->next;

			list_unlink(le);
			list_insert_after(list, tle, le, le->data);
			sort = true;
		}
	}

	if (sort) {
		goto retry;
	}
}


/**
 * Call the apply handler for each element in a linked list
 *
 * @param list  Linked list
 * @param fwd   true to traverse from head to tail, false for reverse
 * @param ah    Apply handler
 * @param arg   Handler argument
 *
 * @return Current list element if handler returned true
 */
struct le *list_apply(const struct list *list, bool fwd,
		      list_apply_h *ah, void *arg)
{
	struct le *le;

	if (!list || !ah)
		return NULL;

	le = fwd ? list->head : list->tail;

	while (le) {
		struct le *cur = le;

		le = fwd ? le->next : le->prev;

		if (ah(cur, arg))
			return cur;
	}

	return NULL;
}


/**
 * Get the first element in a linked list
 *
 * @param list  Linked list
 *
 * @return First list element (NULL if empty)
 */
struct le *list_head(const struct list *list)
{
	return list ? list->head : NULL;
}


/**
 * Get the last element in a linked list
 *
 * @param list  Linked list
 *
 * @return Last list element (NULL if empty)
 */
struct le *list_tail(const struct list *list)
{
	return list ? list->tail : NULL;
}


/**
 * Get the number of elements in a linked list
 *
 * @param list  Linked list
 *
 * @return Number of list elements
 */
uint32_t list_count(const struct list *list)
{
	uint32_t n = 0;
	struct le *le;

	if (!list)
		return 0;

	for (le = list->head; le; le = le->next)
		++n;

	return n;
}
