/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#pragma once

#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>
#include <stdlib.h>
#include "heap_api.h"

#ifdef __cplusplus
extern "C" {
#endif

extern heap_handle_t g_nv_mempool;

#define pool_malloc(size)   heap_malloc(g_nv_mempool,size)
#define pool_free(ptr)      heap_free(g_nv_mempool,ptr)
#define pool_free_space()   heap_free_size(g_nv_mempool)

typedef void (*list_free_cb)(void *data);
typedef bool (*list_iter_cb)(void *data);

typedef struct list_node_t {
  struct list_node_t *next;
  void *data;
} list_node_t;

typedef struct list_t {
  list_node_t *head;
  list_node_t *tail;
  size_t length;
  list_free_cb free_cb;
} list_t;

//struct list_t;
typedef struct list_t list_t;
void *zmalloc_ext (size_t size);
// Lifecycle.
list_t *list_new_ext(list_free_cb callback);
void list_free_ext(list_t *list);

// Accessors.
bool list_is_empty_ext(const list_t *list);
size_t list_length_ext(const list_t *list);
void *list_front_ext(const list_t *list);
void *list_back_ext(const list_t *list);

// Mutators.
bool list_insert_after_ext(list_t *list, list_node_t *prev_node, void *data);
bool list_prepend_ext(list_t *list, void *data);
bool list_append_ext(list_t *list, void *data);
bool list_remove_ext(list_t *list, void *data);
void list_clear_ext(list_t *list);

// Iteration.
void list_foreach_ext(const list_t *list, list_iter_cb callback);

list_node_t *list_begin_ext(const list_t *list);
list_node_t *list_end_ext(const list_t *list);
list_node_t *list_next_ext(const list_node_t *node);
void *list_node_ext(const list_node_t *node);

#ifdef __cplusplus
    }
#endif

#endif//__FMDEC_H__

