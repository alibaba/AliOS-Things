/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_LIST_H
#define K_LIST_H

typedef struct klist_s {
    struct klist_s *next;
    struct klist_s *prev;
} klist_t;

#define krhino_list_entry(node, type, member) ((type *)((uint8_t *)(node) - (size_t)(&((type *)0)->member)))

RHINO_INLINE void klist_init(klist_t *list_head)
{
    list_head->next = list_head;
    list_head->prev = list_head;
}

RHINO_INLINE uint8_t is_klist_empty(klist_t *list)
{
    return (list->next == list);
}

RHINO_INLINE void klist_insert(klist_t *head, klist_t *element)
{
    element->prev = head->prev;
    element->next = head;

    head->prev->next = element;
    head->prev       = element;
}

RHINO_INLINE void klist_add(klist_t *head, klist_t *element)
{
    element->prev = head;
    element->next = head->next;

    head->next->prev = element;
    head->next = element;
}

RHINO_INLINE void klist_rm(klist_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
}

RHINO_INLINE void klist_rm_init(klist_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;

    element->next = element->prev = element;
}


#endif /* K_LIST_H */

