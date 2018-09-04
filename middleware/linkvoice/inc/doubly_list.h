//
//  doubly_list.h
//  pal_sdk
//

#ifndef doubly_list_h
#define doubly_list_h

struct list_entry_t {
    void *elem;
    struct list_entry_t *prev;
    struct list_entry_t *next;
};

void list_init(struct list_entry_t **head);
void list_destroy(struct list_entry_t **head, void (*free_fn)(void *));

struct list_entry_t *list_new_entry(void *elem);
/**
 * return the pointer to the next entry of the target
 */
struct list_entry_t *list_remove_entry(struct list_entry_t *entry);
struct list_entry_t *list_insert_after(struct list_entry_t *target,
                                       struct list_entry_t *entry);
struct list_entry_t *list_insert_before(struct list_entry_t *target,
                                        struct list_entry_t *entry);
struct list_entry_t *list_append(struct list_entry_t *head,
                                 struct list_entry_t *entry);
struct list_entry_t *list_pop(struct list_entry_t *head);
int list_is_empty(struct list_entry_t *head);
struct list_entry_t *list_first_entry_peak(struct list_entry_t *head);

#endif /* doubly_list_h */
