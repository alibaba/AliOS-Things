/**
 * @file k_list.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_LIST_H
#define K_LIST_H

/** @addtogroup aos_rhino list
 *  List operations
 *
 *  @{
 */

/**
 * As a member of other structures, 'klist_t' can form a doubly linked list.
 */
typedef struct klist_s {
    struct klist_s *next;
    struct klist_s *prev;
} klist_t;

/**
 * Get the struct for this entry.
 *
 * @param[in]  node    the list head to take the element from.
 * @param[in]  type    the type of the struct this is embedded in.
 * @param[in]  member  the name of the klist_t within the struct.
 *
 * @return  the pointer to struct
 */
#define krhino_list_entry(node, type, member) ((type *)((uint8_t *)(node) - (size_t)(&((type *)0)->member)))

/**
 * Double linked list initialization: a single Node forms a ring
 *
 * @param[in]  list_head    the list_head node to be initialized.
 *
 * @return  none
 */
RHINO_INLINE void klist_init(klist_t *list_head)
{
    list_head->next = list_head;
    list_head->prev = list_head;
}

/**
 * Judge whether 'klist' is empty.
 *
 * @param[in]  list    the list node.
 *
 * @return 1 on empty, 0 FALSE.
 */
RHINO_INLINE uint8_t is_klist_empty(klist_t *list)
{
    return (list->next == list);
}

/**
 * Double linked list insertion: 'element' before 'head'
 * When 'head' is the head of the linked list, insert 'element' in the list end
 *
 * @param[in]  head       the specified head node.
 * @param[in]  element    the data node to be inserted.
 *
 * @return  none
 */
RHINO_INLINE void klist_insert(klist_t *head, klist_t *element)
{
    element->prev = head->prev;
    element->next = head;

    head->prev->next = element;
    head->prev       = element;
}

/**
 * Double linked list insertion: 'element' after 'head'
 * When 'head' is the head of the linked list, insert 'element' in the list front
 *
 * @param[in]  head       the specified head node.
 * @param[in]  element    the data node to be inserted.
 */
RHINO_INLINE void klist_add(klist_t *head, klist_t *element)
{
    element->prev = head;
    element->next = head->next;

    head->next->prev = element;
    head->next = element;
}

/**
 * Double linked list deletion: 'element' is deleted from the linked list
 *
 * @param[in]  element    the list node to be deleted.
 */
RHINO_INLINE void klist_rm(klist_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
}

/**
 * Double linked list deletion: 'element' is deleted from the linked list and 'element' is initialized
 *
 * @param[in]  element    the list node to be deleted and reinited.
 */
RHINO_INLINE void klist_rm_init(klist_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;

    element->next = element->prev = element;
}

/** @} */

#endif /* K_LIST_H */

