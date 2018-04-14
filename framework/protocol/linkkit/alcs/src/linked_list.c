#include <stdio.h>
#include <string.h>

#include "iot_import.h"
#include "linked_list.h"

#define LINKKED_LIST_PRINTF(...)  \
    do {                                                     \
        printf("\e[0;33m%s@line%d\t:", __FUNCTION__, __LINE__);  \
        printf(__VA_ARGS__);                                 \
        printf("\e[0m");                                   \
    } while (0)

linked_list_t* linked_list_create(char* _name, int synchronized)
{
    size_t name_len = 0;
    linked_list_t* linked_list = NULL;

    linked_list = calloc(1, sizeof(linked_list_t));
    if (!linked_list) return NULL;

    memset(linked_list, 0, sizeof(linked_list_t));

    linked_list->_head = (list_node_t*)calloc(1, sizeof(list_node_t));
    if (!linked_list->_head) goto err_handler;

    linked_list->_head->next = NULL;
    linked_list->_head->data = NULL;
    linked_list->_size = 0;

    if (_name) {
        name_len = strlen(_name);
        linked_list->_name = calloc(1, name_len + 1);
        if (!linked_list->_name) goto err_handler;
        strcpy(linked_list->_name, _name);
    }

    if (synchronized) {
        linked_list->_mutex = HAL_MutexCreate();
    }

    return linked_list;

err_handler:
    if (linked_list->_head) free(linked_list->_head);
    if (linked_list->_name) free(linked_list->_name);
    if (linked_list->_mutex) free(linked_list->_mutex);
    if (linked_list) free(linked_list);

    return NULL;
}

void linked_list_destroy(linked_list_t* _linked_list)
{
    linked_list_t* linked_list = _linked_list;

    list_node_t** p;

    if (linked_list->_mutex) HAL_MutexLock(linked_list->_mutex);

    p = &linked_list->_head;

    linked_list->_size = 0;

    while ((*p) != NULL) {
        list_node_t* node = *p;
        *p = node->next;
        free(node);
    }

    LINKKED_LIST_PRINTF("linked list(%s) destroyed\n", linked_list->_name);

    if (linked_list->_mutex) HAL_MutexUnlock(linked_list->_mutex);

    if (linked_list->_name) free(linked_list->_name);

    if (linked_list->_mutex) {
        HAL_MutexDestroy(linked_list->_mutex);
    }

    if (linked_list) free(linked_list);
}

void linked_list_insert(linked_list_t* _linked_list, void* data)
{
    linked_list_t* linked_list = _linked_list;
    list_node_t* node = (list_node_t*)calloc(1, sizeof(list_node_t));

    if (linked_list->_mutex) HAL_MutexLock(linked_list->_mutex);

    list_node_t** p = (list_node_t**)&linked_list->_head;
    for (; (*p) != NULL; p = &(*p)->next);

    node->data = data;
    node->next = *p;
    *p = node;
    linked_list->_size++;

    if (linked_list->_mutex) HAL_MutexUnlock(linked_list->_mutex);

    LINKKED_LIST_PRINTF("linked list(%s) insert node@%p,size:%d\n", linked_list->_name, node, linked_list->_size);
}

void linked_list_remove(linked_list_t* _linked_list, void* data)
{
    linked_list_t* linked_list = _linked_list;
    list_node_t** p;

    if (linked_list->_mutex) HAL_MutexLock(linked_list->_mutex);

    p = (list_node_t**)&linked_list->_head;

    while ((*p) != NULL) {
        list_node_t* node = *p;
        if (node->data == data) {
            *p = node->next;
            linked_list->_size--;
            LINKKED_LIST_PRINTF("linked list(%s) remove node@%p,size:%d\n", linked_list->_name, node, linked_list->_size);
            free(node);
        } else {
            p = &(*p)->next;
        }
    }

    if (linked_list->_mutex) HAL_MutexUnlock(linked_list->_mutex);
}

void linked_list_clear(linked_list_t* _linked_list)
{
    linked_list_t* linked_list = _linked_list;
    list_node_t** p;

    if (linked_list->_mutex) HAL_MutexLock(linked_list->_mutex);

    p = (list_node_t**)&linked_list->_head->next;

    while ((*p) != NULL) {
        list_node_t* node = *p;
        *p = node->next;
        free(node);
    }
    linked_list->_head->next = NULL;
    linked_list->_size = 0;

    if (linked_list->_mutex) HAL_MutexUnlock(linked_list->_mutex);

    LINKKED_LIST_PRINTF("linked list(%s) cleared\n", linked_list->_name);
}

int  linked_list_empty(const linked_list_t* _linked_list)
{
    const linked_list_t* linked_list = _linked_list;

    return linked_list->_head == NULL;
}

int  linked_list_get_size(const linked_list_t* _linked_list)
{
    const linked_list_t* linked_list = _linked_list;

    return linked_list->_head == NULL;
}

static void linked_list_iterator_routine(const linked_list_t* _linked_list, linked_list_handle_fp_t handle_fp, va_list* params)
{
    const linked_list_t* linked_list = _linked_list;
    list_node_t** p;

    if (linked_list->_mutex) HAL_MutexLock(linked_list->_mutex);

    p = &linked_list->_head->next;

    for (; (*p) != NULL; p = &(*p)->next) {
        va_list args;
        va_copy(args, *params);
        handle_fp((*p)->data, &args);
        va_end(args);
    }

    if (linked_list->_mutex) HAL_MutexUnlock(linked_list->_mutex);
}

void linked_list_iterator(const linked_list_t* _linked_list, linked_list_handle_fp_t handle_fn, ...)
{
    const linked_list_t* linked_list = _linked_list;

    va_list params;
    va_start(params, handle_fn);

    linked_list_iterator_routine(linked_list, handle_fn, &params);

    va_end(params);
}
