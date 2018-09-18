/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __ALI_LIST_H__
#define __ALI_LIST_H__

#include "os.h"
#include "lite-list.h"


/**
 * list_init_head - initialize a list head
 * @head: list head to initialize
 */
#define list_init_head(head) INIT_LIST_HEAD(head)

/**
* list_for_each_entry_t  -  iterate over list of given type
* @pos:        the type * to use as a loop counter.
* @head:       the head for your list.
* @member:     the name of the list_struct within the struct.
* @type:       the type of the struct this is embedded in.
*/
#define list_for_each_entry_t(pos, head, member, type)	\
    for (pos = list_entry((head)->next, type, member);  \
         &pos->member != (head);    \
         pos = list_entry(pos->member.next, type, member))

/**
* list_for_each_entry_reverse_t - iterate backwards over list of given type.
* @pos:        the type * to use as a loop cursor.
* @head:       the head for your list.
* @member:     the name of the list_head within the struct.
* @type:       the type of the struct this is embedded in.
*/
#define list_for_each_entry_reverse_t(pos, head, member, type) \
    for (pos = list_entry((head)->prev, type, member);  \
         &pos->member != (head);    \
         pos = list_entry(pos->member.prev, type, member))

/**
* list_prepare_entry_t - prepare a pos entry for use in list_for_each_entry_continue()
* @pos:        the type * to use as a start point
* @head:       the head of the list
* @member:     the name of the list_head within the struct.
* @type:       the type of the struct this is embedded in.
*
* Prepares a pos entry for use as a start point in list_for_each_entry_continue().
*/
#define list_prepare_entry_t(pos, head, member, type) \
            ((pos) ? : list_entry(head, type, member))

/**
* list_for_each_entry_continue_t - iterate over list of given type
*                      continuing after existing point
* @pos:        the type * to use as a loop counter.
* @head:       the head for your list.
* @member:     the name of the list_struct within the struct.
* @type:       the type of the struct this is embedded in.
*/
#define list_for_each_entry_continue_t(pos, head, member, type) \
    for (pos = list_entry(pos->member.next, type, member);  \
         &pos->member != (head);    \
         pos = list_entry(pos->member.next, type, member))

/**
* list_for_each_entry_safe_t - iterate over list of given type safe against removal of list entry
* @pos:        the type * to use as a loop counter.
* @n:          another type * to use as temporary storage
* @head:       the head for your list.
* @member:     the name of the list_struct within the struct.
* @type:       the type of the struct this is embedded in.
*/
#define list_for_each_entry_safe_t(pos, n, head, member, type)	\
    for (pos = list_entry((head)->next, type, member),  \
        n = list_entry(pos->member.next, type, member); \
         &pos->member != (head);                    \
         pos = n, n = list_entry(n->member.next, type, member))
#endif
