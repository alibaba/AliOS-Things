#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdarg.h>

typedef void (*linked_list_handle_fp_t)(void* list_node, va_list* params);

typedef struct _list_node {
    void*              data;
    struct _list_node* next;
} list_node_t;

typedef struct {
    list_node_t* _head;
    size_t       _size;
    char*        _name;
    void*        _mutex;
} linked_list_t;

extern linked_list_t* linked_list_create(char *_name, int synchronized);
extern void linked_list_destroy(linked_list_t* _linked_list);
extern void linked_list_insert(linked_list_t *_linked_list, void* data);
extern void linked_list_remove(linked_list_t *_linked_list, void* data);
extern void linked_list_clear(linked_list_t *_linked_list);
extern int  linked_list_empty(const linked_list_t *_linked_list);
extern int  linked_list_get_size(const linked_list_t* _linked_list);
extern void linked_list_iterator(const linked_list_t *_linked_list, linked_list_handle_fp_t handle_fn, ...);

#endif /* LINKED_LIST_H */
