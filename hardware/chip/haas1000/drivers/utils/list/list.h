/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#pragma once

#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*list_mempool_zmalloc)(size_t);
typedef void (*list_mempool_free)(void *);

typedef struct {    
    list_mempool_zmalloc zmalloc;
    list_mempool_free free;
} list_mempool_functions_t;

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
  list_mempool_functions_t mempool_functions;
} list_t;

struct list_node_t;
typedef struct list_node_t list_node_t;
//struct list_t;
typedef struct list_t list_t;

int list_init(void);

// Lifecycle.
list_t *list_new(list_free_cb callback, list_mempool_zmalloc zmalloc, list_mempool_free free);
void list_free(list_t *list);

// Accessors.
bool list_is_empty(const list_t *list);
size_t list_length(const list_t *list);
void *list_front(const list_t *list);
void *list_back(const list_t *list);

// Mutators.
bool list_insert_after(list_t *list, list_node_t *prev_node, void *data);
bool list_prepend(list_t *list, void *data);
bool list_append(list_t *list, void *data);
bool list_remove(list_t *list, void *data);
void list_clear(list_t *list);

// Iteration.
void list_foreach(const list_t *list, list_iter_cb callback);

list_node_t *list_begin(const list_t *list);
list_node_t *list_end(const list_t *list);
list_node_t *list_next(const list_node_t *node);
void *list_node(const list_node_t *node);

#ifdef __cplusplus
	}
#endif

#endif//__FMDEC_H__

