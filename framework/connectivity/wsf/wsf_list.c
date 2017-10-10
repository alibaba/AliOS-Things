/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

#include "os.h"
#include "wsf_list.h"

wsf_code wsf_list_push_back(wsf_list_t *list, void *data, size_t length)
{
    if (list == NULL) {
        return WSF_FAIL;
    }

    size_t len = sizeof(wsf_list_node_t);
    wsf_list_node_t *node = (wsf_list_node_t *)os_malloc(len);
    if (!node) {
        return WSF_FAIL;
    }

    node->next = NULL;
    node->data = data;
    node->length = length;

    if (!list->head) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }

    return WSF_SUCCESS;
}

wsf_list_node_t *wsf_list_pop_front(wsf_list_t *list)
{
    wsf_list_node_t *node = NULL;
    if (list) {
        if (list->head) {
            node = list->head;
            list->head = list->head->next;
            node->next = NULL;
        }
        if (!list->head) {
            list->tail = NULL;
        }
    }
    return node;
}


int wsf_list_is_empty(wsf_list_t *list)
{
    return !(list && list->head);
}

void wsf_list_init(wsf_list_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}
void wsf_list_os_free(wsf_list_t *list)
{
    wsf_list_node_t *node = NULL;
    while ((node = wsf_list_pop_front(list)) != NULL) {
        os_free(node);
    }

}
