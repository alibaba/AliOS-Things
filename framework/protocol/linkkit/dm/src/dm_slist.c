#include <stdlib.h>

#include "interface/list_abstract.h"
#include "dm_slist.h"

#include "dm_import.h"

static const char string_slist_class_name[] __DM_READ_ONLY__ = "slist_cls";
static const char string_slist_log_pattern[] __DM_READ_ONLY__ = "slist(%s) %s node,size:%d\n";
static const char string_slist_clear_pattern[] __DM_READ_ONLY__ = "slist(%s) cleared\n";
static const char string_slist_insert[] __DM_READ_ONLY__ = "insert";
static const char string_slist_remove[] __DM_READ_ONLY__ = "remove";

static void slist_insert(void* _self, void* data);
static void slist_clear(void* _self);

static void* slist_ctor(void* _self, va_list* params)
{
    slist_t* self = _self;

    self->_head = (slist_node_t*)dm_lite_calloc(1, sizeof(slist_node_t));

    if (self->_head == NULL) return NULL;

    self->_head->next = NULL;
    self->_head->data = NULL;
    self->_size = 0;

    self->_name = va_arg(*params, char*);

    if (self->_name == NULL) goto err_handler;

    return self;

err_handler:
    if (self->_head) dm_lite_free(self->_head);
    if (self->_name) dm_lite_free(self->_name);

    return NULL;
}

static void* slist_dtor(void* _self)
{
    slist_t* self = _self;
    slist_node_t** p = &self->_head;

    self->_size = 0;

    while ((*p) != NULL) {
        slist_node_t* node = *p;
        *p = node->next;
        dm_lite_free(node);
    }

    return self;
}

static void slist_insert(void* _self, void* data)
{
    slist_t* self = _self;
    slist_node_t* node = (slist_node_t*)dm_lite_calloc(1, sizeof(slist_node_t));

    slist_node_t** p = (slist_node_t**)&self->_head;
    for (; (*p) != NULL; p = &(*p)->next)
        ;

    node->data = data;
    node->next = *p;
    *p = node;
    self->_size++;

    printf(string_slist_log_pattern, self->_name, string_slist_insert, self->_size);
}

static void slist_remove(void* _self, void* data)
{
    slist_t* self = _self;
    slist_node_t** p = (slist_node_t**)&self->_head;

    while ((*p) != NULL) {
        slist_node_t* node = *p;
        if (node->data == data) {
            *p = node->next;
            self->_size--;
            printf(string_slist_log_pattern, self->_name, string_slist_remove, self->_size);
            dm_lite_free(node);
        } else {
            p = &(*p)->next;
        }
    }
}

static void slist_clear(void* _self)
{
    slist_t* self = _self;
    slist_node_t** p = (slist_node_t**)&self->_head->next;

    while ((*p) != NULL) {
        slist_node_t* node = *p;
        *p = node->next;
        dm_lite_free(node);
    }
    self->_head->next = NULL;
    self->_size = 0;
    printf(string_slist_clear_pattern, self->_name);
}

static int slist_empty(const void* _self)
{
    const slist_t* self = _self;

    return self->_head == NULL;
}

static int slist_get_size(const void* _self)
{
    const slist_t* self = _self;

    return self->_size;
}

static void slist_iterator(const void* _self, handle_fp_t handle_fn, va_list* params)
{
    const slist_t* self = _self;
    slist_node_t** p = &self->_head->next;

    for (; (*p) != NULL; p = &(*p)->next) {
        va_list args;
        va_copy(args, *params);
        handle_fn((*p)->data, &args);
        va_end(args);
    }
}

void list_iterator(const void* _list, handle_fp_t handle_fp, ...)
{
    const list_t** list = (const list_t**)_list;
    va_list params;
    va_start(params, handle_fp);

    (*list)->iterator(list, handle_fp, &params);
    va_end(params);
}

static void slist_print(const void* _self, print_fp_t print_fn)
{
    const slist_t* self = _self;
    slist_node_t** p = &self->_head->next;

    while ((*p) != NULL) {
        print_fn((*p)->data);
        p = &(*p)->next;
    }
}

static const list_t _slist_class = {
    sizeof(slist_t),
    string_slist_class_name,
    slist_ctor,
    slist_dtor,
    slist_insert,
    slist_remove,
    slist_clear,
    slist_empty,
    slist_get_size,
    slist_iterator,
    slist_print,
};

const void* get_slist_class()
{
    return &_slist_class;
}
