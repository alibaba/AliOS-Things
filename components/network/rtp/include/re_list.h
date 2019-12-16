/**
 * @file re_list.h  Interface to Linked List
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_LIST_H
#define RE_LIST_H

#include <stdint.h>
#include <re_tmr.h>

/** List Element Initializer */
#define LE_INIT {NULL, NULL, NULL, NULL}


/** Linked list Initializer */
#define LIST_INIT {NULL, NULL}


/**
 * Defines the list apply handler
 *
 * @param le  List element
 * @param arg Handler argument
 *
 * @return true to stop traversing, false to continue
 */
typedef bool (list_apply_h)(struct le *le, void *arg);


/**
 * Defines the list sort handler
 *
 * @param le1  Current list element
 * @param le2  Next list element
 * @param arg  Handler argument
 *
 * @return true if sorted, otherwise false
 */
typedef bool (list_sort_h)(struct le *le1, struct le *le2, void *arg);


void list_init(struct list *list);
void list_flush(struct list *list);
void list_clear(struct list *list);
void list_append(struct list *list, struct le *le, void *data);
void list_prepend(struct list *list, struct le *le, void *data);
void list_insert_before(struct list *list, struct le *le, struct le *ile,
			void *data);
void list_insert_after(struct list *list, struct le *le, struct le *ile,
		       void *data);
void list_unlink(struct le *le);
void list_sort(struct list *list, list_sort_h *sh, void *arg);
struct le *list_apply(const struct list *list, bool fwd, list_apply_h *ah,
		      void *arg);
struct le *list_head(const struct list *list);
struct le *list_tail(const struct list *list);
uint32_t list_count(const struct list *list);


/**
 * Get the user-data from a list element
 *
 * @param le List element
 *
 * @return Pointer to user-data
 */
static inline void *list_ledata(const struct le *le)
{
	return le ? le->data : NULL;
}


static inline bool list_contains(const struct list *list, const struct le *le)
{
	return le ? le->list == list : false;
}


static inline bool list_isempty(const struct list *list)
{
	return list ? list->head == NULL : true;
}


#define LIST_FOREACH(list, le)					\
	for ((le) = list_head((list)); (le); (le) = (le)->next)
#endif /* RE_FMT_H */
