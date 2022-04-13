/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_LIST_H__
#define __UVOICE_LIST_H__


typedef struct uvoice_list {
    struct uvoice_list *prev;
    struct uvoice_list *next;
} uvoice_list_t;

#define UVOICE_LIST_INIT(list)	\
	{&(list), &(list)}

#define uvoice_list_for_each_entry(head, node, type, member)			\
	for (node = os_container_of((head)->next, type, member);				\
		&node->member != (head);										\
		node = os_container_of(node->member.next, type, member))

#define uvoice_list_for_each_entry_safe(head, temp, node, type, member)	\
	for (node = os_container_of((head)->next, type, member),				\
		temp = (head)->next ? (head)->next->next : NULL;				\
		&node->member != (head);										\
		node = os_container_of(temp, type, member), temp = temp ? temp->next : NULL)

static inline void uvoice_list_add(uvoice_list_t *node, uvoice_list_t *head)
{
	uvoice_list_t *next = head->next;
    node->next = next;
    node->prev = head;

    head->next = node;
    next->prev = node;
}

static inline void uvoice_list_add_tail(uvoice_list_t *node, uvoice_list_t *head)
{
	uvoice_list_t *prev = head->prev;
    node->next = head;
    node->prev = prev;

    prev->next = node;
    head->prev = node;
}

static inline int uvoice_list_entry_count(uvoice_list_t *head)
{
	uvoice_list_t *pos = head;
	int count = 0;
	while (pos->next != head) {
		pos = pos->next;
		count++;
	}
	return count;
}

static inline int uvoice_list_empty(uvoice_list_t *head)
{
	return head->next == head;
}

static inline void uvoice_list_del(uvoice_list_t *node)
{
    uvoice_list_t *prev = node->prev;
    uvoice_list_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

static inline void uvoice_list_init(uvoice_list_t *head)
{
	head->next = head->prev = head;
}


#endif /* __UVOICE_LIST_H__ */

