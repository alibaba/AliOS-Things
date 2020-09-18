
#include "bt_common.h"
#include "osi/allocator.h"
#include "osi/list.h"
#include "osi/osi.h"

struct list_node_t {
    struct list_node_t *next;
    void *data;
};

typedef struct list_t {
    list_node_t *head;
    list_node_t *tail;
    size_t length;
    list_free_cb free_cb;
} list_t;

int32_t osi_check_list(list_t *list)
{
    int32_t cnt = 0;
    list_node_t *node;

    node = list->head;
    while (node) {
        cnt++;
        node = node->next;
    }

    if (cnt != list->length) {
        OSI_TRACE_ERROR("List unmatch: cnt = %d, list = %x, head = %x, tail = %x, length = %d, list->callback = %x", cnt, list, list->head, list->tail, list->length, list->free_cb);

        node = list->head;
        while (node) {
            uint8_t *pNode;
            int32_t i;

            OSI_TRACE_ERROR("Node = %x", node);
            pNode = (uint8_t *)node;
            pNode -= 32;
            for (i = 0; i < 64; i += 8) {
                OSI_TRACE_ERROR("Data[%02x]:%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x", i, pNode[i+0], pNode[i+1], pNode[i+2], pNode[i+3], pNode[i+4], pNode[i+5],pNode[i+6],pNode[i+7]);
            }
            node = node->next;
        }
    }

    assert(cnt == list->length);

    return 0;
}

//static list_node_t *list_free_node_(list_t *list, list_node_t *node);

// Hidden constructor, only to be used by the hash map for the allocation tracker.
// Behaves the same as |list_new|, except you get to specify the allocator.
list_t *osi_list_new_internal(list_free_cb callback)
{
    list_t *list = (list_t *) osi_calloc(sizeof(list_t));
    if (!list) {
        return NULL;
    }

    list->head = list->tail = NULL;
    list->length = 0;
    list->free_cb = callback;
    return list;
}

list_t *osi_list_new(list_free_cb callback)
{
    return osi_list_new_internal(callback);
}

void osi_list_free(list_t *list)
{
    if (!list) {
        return;
    }

    osi_list_clear(list);
    osi_free(list);
}

bool osi_list_is_empty(const list_t *list)
{
    assert(list != NULL);
    return (list->length == 0);
}

bool osi_list_contains(const list_t *list, const void *data)
{
    assert(list != NULL);
    assert(data != NULL);

    for (const list_node_t *node = osi_list_begin(list); node != osi_list_end(list); node = osi_list_next(node)) {
        if (osi_list_node(node) == data) {
            return true;
        }
    }

    return false;
}

size_t osi_list_length(const list_t *list)
{
    assert(list != NULL);
    return list->length;
}

void *osi_list_front(const list_t *list)
{
    assert(list != NULL);
    assert(!osi_list_is_empty(list));

    if (list->head == NULL) {
        OSI_TRACE_ERROR("list = %x,list->head = %x, list->tail = %x, list->length = %x", 
                    list, list->head, list->tail, list->length);
    }
    assert(list->head != NULL);

    return list->head->data;
}

void *osi_list_back(const list_t *list)
{
    assert(list != NULL);
    assert(!osi_list_is_empty(list));

    return list->tail->data;
}

list_node_t *osi_list_back_node(const list_t *list)
{
    assert(list != NULL);
    assert(!osi_list_is_empty(list));

    return list->tail;
}

bool osi_list_insert_after(list_t *list, list_node_t *prev_node, void *data)
{
    assert(list != NULL);
    assert(prev_node != NULL);
    assert(data != NULL);

    osi_check_list(list);
    list_node_t *node = (list_node_t *)osi_calloc(sizeof(list_node_t));
    if (!node) {
        return false;
    }
    node->next = prev_node->next;
    node->data = data;
    prev_node->next = node;
    if (list->tail == prev_node) {
        list->tail = node;
    }
    ++list->length;

    osi_check_list(list);
    return true;
}

bool osi_list_prepend(list_t *list, void *data)
{
    assert(list != NULL);
    assert(data != NULL);

    osi_check_list(list);
    list_node_t *node = (list_node_t *)osi_calloc(sizeof(list_node_t));
    if (!node) {
        return false;
    }
    node->next = list->head;
    node->data = data;
    list->head = node;
    if (list->tail == NULL) {
        list->tail = list->head;
    }
    ++list->length;

    osi_check_list(list);
    return true;
}

bool osi_list_append(list_t *list, void *data)
{
    assert(list != NULL);
    assert(data != NULL);

    osi_check_list(list);
    list_node_t *node = (list_node_t *)osi_calloc(sizeof(list_node_t));
    if (!node) {
        return false;
    }
    node->next = NULL;
    node->data = data;
    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    ++list->length;

    osi_check_list(list);
    return true;
}

bool osi_list_remove(list_t *list, void *data)
{
    assert(list != NULL);
    assert(data != NULL);

    osi_check_list(list);
    if (osi_list_is_empty(list)) {
        return false;
    }

    if (list->head->data == data) {
        list_node_t *next = osi_list_free_node(list, list->head);
        if (list->tail == list->head) {
            list->tail = next;
        }
        list->head = next;

        osi_check_list(list);
        return true;
    }

    for (list_node_t *prev = list->head, *node = list->head->next; node; prev = node, node = node->next) {
        if (node->data == data) {
            prev->next = osi_list_free_node(list, node);
            if (list->tail == node) {
                list->tail = prev;
            }

            osi_check_list(list);
            return true;
        }
    }
    return false;
}

void osi_list_clear(list_t *list)
{
    assert(list != NULL);
    for (list_node_t *node = list->head; node; ) {
        node = osi_list_free_node(list, node);
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

list_node_t *osi_list_foreach(const list_t *list, list_iter_cb callback, void *context)
{
    assert(list != NULL);
    assert(callback != NULL);

    for (list_node_t *node = list->head; node;) {
        list_node_t *next = node->next;

        if (!callback(node->data, context)) {
            return node;
        }

        node = next;
    }

    return NULL;
}

list_node_t *osi_list_begin(const list_t *list)
{
    assert(list != NULL);
    return list->head;
}

list_node_t *osi_list_end(UNUSED_ATTR const list_t *list)
{
    assert(list != NULL);
    return NULL;
}

list_node_t *osi_list_next(const list_node_t *node)
{
    assert(node != NULL);
    return node->next;
}

void *osi_list_node(const list_node_t *node)
{
    assert(node != NULL);
    return node->data;
}

list_node_t *osi_list_free_node(list_t *list, list_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);

    list_node_t *next = node->next;

    if (list->free_cb) {
        list->free_cb(node->data);
    }
    osi_free(node);
    --list->length;

    return next;
}
