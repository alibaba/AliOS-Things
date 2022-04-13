#ifndef _CORE_LIST_H_
#define _CORE_LIST_H_

#if defined(__cplusplus)
extern "C" {
#endif

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) || defined(__GNUC__)) && \
    !defined(inline) && !defined(__cplusplus)
    #define inline __inline
#endif

struct core_list_head {
    struct core_list_head *prev;
    struct core_list_head *next;
};

/*
 * Get offset of a member variable.
 *
 * @param[in]   type     the type of the struct this is embedded in.
 * @param[in]   member   the name of the variable within the struct.
 */
#ifdef offsetof
    #undef offsetof
#endif
#define offsetof(type, member)   ((size_t)&(((type *)0)->member))

/*
 * Get the struct for this entry.
 *
 * @param[in]   ptr     the list head to take the element from.
 * @param[in]   type    the type of the struct this is embedded in.
 * @param[in]   member  the name of the variable within the struct.
 */
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))

static inline void CORE_INIT_LIST_HEAD(struct core_list_head *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __core_list_add(struct core_list_head *node,
                              struct core_list_head *prev,
                              struct core_list_head *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

/**
 * core_list_add - add a new entry
 * @node: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void core_list_add(struct core_list_head *node, struct core_list_head *head)
{
    __core_list_add(node, head, head->next);
}

/**
 * core_list_add_tail - add a new entry
 * @node: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void core_list_add_tail(struct core_list_head *node, struct core_list_head *head)
{
    __core_list_add(node, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __core_list_del(struct core_list_head *prev, struct core_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * core_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void __core_list_del_entry(struct core_list_head *entry)
{
    __core_list_del(entry->prev, entry->next);
}

static inline void core_list_del(struct core_list_head *entry)
{
    __core_list_del_entry(entry);
    entry->next = entry;
    entry->prev = entry;
}

/**
 * core_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int core_list_empty(const struct core_list_head *head)
{
    return head->next == head;
}

/**
 * core_list_entry - get the struct for this entry
 * @ptr:    the &struct core_list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the core_list_head within the struct.
 */
#define core_list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * core_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the core_list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define core_list_first_entry(ptr, type, member) \
    core_list_entry((ptr)->next, type, member)

/**
 * core_list_next_entry - get the next element in list
 * @pos:    the type * to cursor
 * @member: the name of the core_list_head within the struct.
 * @type:   the type of the struct this is embedded in
 */
#define core_list_next_entry(pos, member, type)         \
    core_list_entry((pos)->member.next, type, member)

/**
 * core_list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the core_list_head within the struct.
 * @type:   the type of the struct this is embedded in
 */
#define core_list_for_each_entry(pos, head, member, type)       \
    for (pos = core_list_first_entry(head, type, member);       \
         &pos->member != (head);                                \
         pos = core_list_next_entry(pos, member, type))

/**
 * core_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the core_list_head within the struct.
 * @type:   the type of the struct this is embedded in
 */
#define core_list_for_each_entry_safe(pos, n, head, member, type)   \
    for (pos = core_list_first_entry(head, type, member),           \
         n = core_list_next_entry(pos, member, type);               \
         &pos->member != (head);                                    \
         pos = n, n = core_list_next_entry(n, member, type))

#if defined(__cplusplus)
}
#endif

#endif

