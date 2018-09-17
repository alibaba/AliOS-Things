/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include <string.h>
#include "iotx_utils_internal.h"
#include "iot_import.h"
#include "linked_list.h"

linked_list_t *linked_list_create(char *_name, int synchronized)
{
    size_t name_len = 0;
    linked_list_t *linked_list = NULL;

    linked_list = LITE_calloc(1, sizeof(linked_list_t));
    if (!linked_list) {
        return NULL;
    }

    memset(linked_list, 0, sizeof(linked_list_t));

    linked_list->_head = (list_unit_t *)LITE_calloc(1, sizeof(list_unit_t));
    if (!linked_list->_head) {
        goto err_handler;
    }

    linked_list->_head->next = NULL;
    linked_list->_head->data = NULL;
    linked_list->_size = 0;

    if (_name) {
        name_len = strlen(_name);
        linked_list->_name = LITE_calloc(1, name_len + 1);
        if (!linked_list->_name) {
            goto err_handler;
        }
        strcpy(linked_list->_name, _name);
    }

    if (synchronized) {
        linked_list->_mutex = HAL_MutexCreate();
    }

    return linked_list;

err_handler:
    if (linked_list->_head) {
        LITE_free(linked_list->_head);
    }
    if (linked_list->_name) {
        LITE_free(linked_list->_name);
    }
    if (linked_list->_mutex) {
        LITE_free(linked_list->_mutex);
    }
    if (linked_list) {
        LITE_free(linked_list);
    }

    return NULL;
}

void linked_list_destroy(linked_list_t *_linked_list)
{
    linked_list_t *linked_list = _linked_list;

    list_unit_t **p;

    if (linked_list == NULL) {
        return;
    }

    if (linked_list->_mutex) {
        HAL_MutexLock(linked_list->_mutex);
    }

    p = &linked_list->_head;

    linked_list->_size = 0;

    while ((*p) != NULL) {
        list_unit_t *node = *p;
        *p = node->next;
        LITE_free(node);
    }

    utils_info("linked list(%s) destroyed\n", linked_list->_name);

    if (linked_list->_mutex) {
        HAL_MutexUnlock(linked_list->_mutex);
    }

    if (linked_list->_name) {
        LITE_free(linked_list->_name);
    }

    if (linked_list->_mutex) {
        HAL_MutexDestroy(linked_list->_mutex);
    }

    if (linked_list) {
        LITE_free(linked_list);
    }
}

void linked_list_insert(linked_list_t *_linked_list, void *data)
{
    linked_list_t *linked_list = _linked_list;
    list_unit_t *node = (list_unit_t *)LITE_calloc(1, sizeof(list_unit_t));

    if (linked_list->_mutex) {
        HAL_MutexLock(linked_list->_mutex);
    }

    list_unit_t **p = (list_unit_t **)&linked_list->_head;
    for (; (*p) != NULL; p = &(*p)->next);

    node->data = data;
    node->next = *p;
    *p = node;
    linked_list->_size++;

    if (linked_list->_mutex) {
        HAL_MutexUnlock(linked_list->_mutex);
    }

    utils_info("linked list(%s) insert node@%p,size:%lu\n", linked_list->_name, node, linked_list->_size);
}

void linked_list_remove(linked_list_t *_linked_list, void *data)
{
    linked_list_t *linked_list = _linked_list;
    list_unit_t **p;

    if (linked_list->_mutex) {
        HAL_MutexLock(linked_list->_mutex);
    }

    p = (list_unit_t **)&linked_list->_head;

    while ((*p) != NULL) {
        list_unit_t *node = *p;
        if (node->data == data) {
            *p = node->next;
            linked_list->_size--;
            utils_info("linked list(%s) remove node@%p,size:%lu\n", linked_list->_name, node, linked_list->_size);
            LITE_free(node);
        } else {
            p = &(*p)->next;
        }
    }

    if (linked_list->_mutex) {
        HAL_MutexUnlock(linked_list->_mutex);
    }
}

void linked_list_clear(linked_list_t *_linked_list)
{
    linked_list_t *linked_list = _linked_list;
    list_unit_t **p;

    if (linked_list->_mutex) {
        HAL_MutexLock(linked_list->_mutex);
    }

    p = (list_unit_t **)&linked_list->_head->next;

    while ((*p) != NULL) {
        list_unit_t *node = *p;
        *p = node->next;
        LITE_free(node);
    }
    linked_list->_head->next = NULL;
    linked_list->_size = 0;

    if (linked_list->_mutex) {
        HAL_MutexUnlock(linked_list->_mutex);
    }

    utils_info("linked list(%s) cleared\n", linked_list->_name);
}

int  linked_list_empty(const linked_list_t *_linked_list)
{
    const linked_list_t *linked_list = _linked_list;

    return linked_list->_head == NULL;
}

int  linked_list_get_size(const linked_list_t *_linked_list)
{
    const linked_list_t *linked_list = _linked_list;

    return linked_list->_size;
}

static void linked_list_iterator_routine(const linked_list_t *_linked_list, linked_list_handle_fp_t handle_fp,
        va_list *params)
{
    const linked_list_t *linked_list = _linked_list;
    list_unit_t **p;

    if (linked_list->_mutex) {
        HAL_MutexLock(linked_list->_mutex);
    }

    p = &linked_list->_head->next;

    for (; (*p) != NULL; p = &(*p)->next) {
        va_list args;
        va_copy(args, *params);
        handle_fp((*p)->data, &args);
        va_end(args);
    }

    if (linked_list->_mutex) {
        HAL_MutexUnlock(linked_list->_mutex);
    }
}

void linked_list_iterator(const linked_list_t *_linked_list, linked_list_handle_fp_t handle_fn, ...)
{
    const linked_list_t *linked_list = _linked_list;

    va_list params;
    va_start(params, handle_fn);

    linked_list_iterator_routine(linked_list, handle_fn, &params);

    va_end(params);
}
