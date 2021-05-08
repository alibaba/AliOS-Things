/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file       drv/list.h
 * @brief      Header File for LIST Driver
 * @version    V1.0
 * @date       10. Oct 2020
 * @model      list
 ******************************************************************************/

#ifndef AOS_LIST_H
#define AOS_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 \brief       Get offset of a member variable
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the variable within the struct
 \return      None
*/
#define aos_offsetof(type, member)   ((size_t)&(((type *)0)->member))

/**
 \brief       Get the struct for this entry
 \param[in]   ptr       The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the variable within the struct
 \return      None
*/
#define aos_container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - aos_offsetof(type, member)))

/* For double link list */
typedef struct dlist_s {
    struct dlist_s *prev;
    struct dlist_s *next;
} dlist_t;

static inline void __dlist_add(dlist_t *node, dlist_t *prev, dlist_t *next)
{
    node->next = next;
    node->prev = prev;

    prev->next = node;
    next->prev = node;
}

/**
 \brief       Get the struct for this entry
 \param[in]   addr      The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the dlist_t within the struct
 \return      None
*/
#define dlist_entry(addr, type, member) \
    ((type *)((long)addr - aos_offsetof(type, member)))


static inline void dlist_add(dlist_t *node, dlist_t *queue)
{
    __dlist_add(node, queue, queue->next);
}

static inline void dlist_add_tail(dlist_t *node, dlist_t *queue)
{
    __dlist_add(node, queue->prev, queue);
}

static inline void dlist_del(dlist_t *node)
{
    dlist_t *prev = node->prev;
    dlist_t *next = node->next;

    prev->next = next;
    next->prev = prev;
}

static inline void dlist_init(dlist_t *node)
{
    node->next = (node->prev = node);
}

static inline void INIT_AOS_DLIST_HEAD(dlist_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline int dlist_empty(const dlist_t *head)
{
    return head->next == head;
}

/**
 \brief       Initialise the list
 \param[in]   list    The list to be inited
 \return      None
*/
#define AOS_DLIST_INIT(list)  {&(list), &(list)}

/**
 \brief       Get the first element from a list
 \param[in]   ptr       The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the dlist_t within the struct
 \return      None
*/
#define dlist_first_entry(ptr, type, member) \
    dlist_entry((ptr)->next, type, member)

/**
 \brief       Iterate over a list
 \param[in]   pos     The &struct dlist_t to use as a loop cursor
 \param[in]   head    The head for your list
 \return      None
*/
#define dlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 \brief       Iterate over a list safe against removal of list entry
 \param[in]   pos     The &struct dlist_t to use as a loop cursor
 \param[in]   n       Another &struct dlist_t to use as temporary storage
 \param[in]   head    The head for your list
 \return      None
*/
#define dlist_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

/**
 \brief       Iterate over list of given type
 \param[in]   queue     The head for your list
 \param[in]   node      The &struct dlist_t to use as a loop cursor
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the dlist_t within the struct
 \return      None
*/
#define dlist_for_each_entry(queue, node, type, member) \
    for (node = aos_container_of((queue)->next, type, member); \
         &node->member != (queue); \
         node = aos_container_of(node->member.next, type, member))

/**
 \brief       Iterate over list of given type safe against removal of list entry
 \param[in]   queue     The head for your list
 \param[in]   n         The type * to use as a temp
 \param[in]   node      The type * to use as a loop cursor
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the dlist_t within the struct
 \return      None
*/
#define dlist_for_each_entry_safe(queue, n, node, type, member) \
    for (node = aos_container_of((queue)->next, type, member),  \
         n = (queue)->next ? (queue)->next->next : NULL;        \
         &node->member != (queue);                              \
         node = aos_container_of(n, type, member), n = n ? n->next : NULL)

/**
 \brief       Get the struct for this entry
 \param[in]   ptr       The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the variable within the struct
 \return      None
 */
#define list_entry(ptr, type, member) \
    aos_container_of(ptr, type, member)


/**
 \brief       Iterate backwards over list of given type
 \param[in]   pos       The type * to use as a loop cursor
 \param[in]   head      The head for your list
 \param[in]   member    The name of the dlist_t within the struct
 \param[in]   type      The type of the struct this is embedded in
 \return      None
*/
#define dlist_for_each_entry_reverse(pos, head, member, type) \
    for (pos = list_entry((head)->prev, type, member);        \
         &pos->member != (head);                              \
         pos = list_entry(pos->member.prev, type, member))

/**
 \brief       Get the list length
 \param[in]   queue     The head for your list
 \return      None
*/
int dlist_entry_number(dlist_t *queue);

/**
 \brief       Initialise the list
 \param[in]   name    The list to be initialized
 \return      None
*/
#define AOS_DLIST_HEAD_INIT(name) { &(name), &(name) }

/**
 \brief       Initialise the list
 \param[in]   name    The list to be initialized
 \return      None
*/
#define AOS_DLIST_HEAD(name) \
    dlist_t name = AOS_DLIST_HEAD_INIT(name)

/* For single link list */
typedef struct slist_s {
    struct slist_s *next;
} slist_t;

static inline void slist_add(slist_t *node, slist_t *head)
{
    node->next = head->next;
    head->next = node;
}

void slist_add_tail(slist_t *node, slist_t *head);

static inline void slist_del(slist_t *node, slist_t *head)
{
    while (head->next) {
        if (head->next == node) {
            head->next = node->next;
            break;
        }

        head = head->next;
    }
}

static inline int slist_empty(const slist_t *head)
{
    return !head->next;
}

static inline void slist_init(slist_t *head)
{
    head->next = 0;
}

static inline slist_t *slist_remove(slist_t *l, slist_t *n)
{
    /* Remove slist head */
    struct slist_s *node = l;

    while (node->next && (node->next != n)) {
        node = node->next;
    }

    /* Remove node */
    if (node->next != (slist_t *)0) {
        node->next = node->next->next;
    }

    return l;
}

static inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}

static inline slist_t *slist_tail(slist_t *l)
{
    while (l->next) {
        l = l->next;
    }

    return l;
}

static inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

/**
  \brief       Iterate over list of given type
  \param[in]   node      The type * to use as a loop cursor
  \param[in]   type      The type of the struct this is embedded in
  \param[in]   member    The name of the slist_t within the struct
  \param[in]   queue     The head for your list
  \return      None
*/
#define slist_for_each_entry(queue, node, type, member)        \
    for (node = aos_container_of((queue)->next, type, member); \
         &node->member;                                        \
         node = aos_container_of(node->member.next, type, member))

/**
 \brief       Iterate over list of given type safe against removal of list entry
 \param[in]   queue     The head for your list
 \param[in]   tmp       The type * to use as a temp
 \param[in]   node      The type * to use as a loop cursor
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the slist_t within the struct
 \return      None
*/
#define slist_for_each_entry_safe(queue, tmp, node, type, member) \
    for (node = aos_container_of((queue)->next, type, member),    \
         tmp = (queue)->next ? (queue)->next->next : NULL;        \
         &node->member;                                           \
         node = aos_container_of(tmp, type, member), tmp = tmp ? tmp->next : tmp)

/**
 \brief       Initialise the list
 \param[in]   name    The list to be initialized
 \return      None
*/
#define AOS_SLIST_HEAD_INIT(name) {0}

/**
 \brief       Initialise the list
 \param[in]   name    The list to be initialized
 \return      None
*/
#define AOS_SLIST_HEAD(name) \
    slist_t name = AOS_SLIST_HEAD_INIT(name)

/**
 \brief       Get the struct for this entry
 \param[in]   addr      The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the slist_t within the struct
 \return      None
*/
#define slist_entry(addr, type, member) (                                   \
        addr ? (type *)((long)addr - aos_offsetof(type, member)) : (type *)addr \
                                        )

/**
 \brief       Get the first element from a list
 \param[in]   ptr       The list head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the slist_t within the struct
 \return      None
*/
#define slist_first_entry(ptr, type, member) \
    slist_entry((ptr)->next, type, member)

/**
 \brief       Slist_tail_entry - get the tail element from a slist
 \param[in]   ptr       The slist head to take the element from
 \param[in]   type      The type of the struct this is embedded in
 \param[in]   member    The name of the slist_struct within the struct
 \return      None
 \note        That slist is expected to be not empty
*/
#define slist_tail_entry(ptr, type, member) \
    slist_entry(slist_tail(ptr), type, member)

/**
 \brief       Get the list length
 \param[in]   queue    The head for your list
 \return      None
*/
int slist_entry_number(slist_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* AOS_LIST_H */
