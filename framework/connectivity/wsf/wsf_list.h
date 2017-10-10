/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_LIST_H
#define WSF_LIST_H
#include "wsf_defines.h"
#include <stdlib.h>

typedef struct wsf_list_node_t {
    void       *data;
    size_t        length;
    struct wsf_list_node_t *next;
} wsf_list_node_t;

typedef struct wsf_list_t {
    wsf_list_node_t *head;
    wsf_list_node_t *tail;
} wsf_list_t;

void wsf_list_init(wsf_list_t *list);
/**
 * append the data to the list, the data size is 'length'
 *
 */
wsf_code wsf_list_push_back(wsf_list_t *list, void *data, size_t length);

int wsf_list_is_empty(wsf_list_t *list);

void wsf_list_free(wsf_list_t *list);

#endif
