/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_COMMON_LIST_H
#define _IOTX_COMMON_LIST_H

#include <stdlib.h>

/*
 * list_t iterator direction.
 */
typedef enum {
    LIST_HEAD,
    LIST_TAIL
} list_direction_t;

/*
 * list_t node struct.
 */
typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
    void *val;
} list_node_t;

/*
 * list_t struct.
 */
typedef struct {
    list_node_t *head;
    list_node_t *tail;
    unsigned int len;
    void (*free)(void *val);
    int (*match)(void *a, void *b);
} list_t;

/*
 * list_t iterator struct.
 */
typedef struct {
    list_node_t *next;
    list_direction_t direction;
} list_iterator_t;

/* Node prototypes. */
list_node_t *list_node_new(void *val);

/* list_t prototypes. */
list_t *list_new(void);

list_node_t *list_rpush(list_t *self, list_node_t *node);

list_node_t *list_lpush(list_t *self, list_node_t *node);

list_node_t *list_find(list_t *self, void *val);

list_node_t *list_at(list_t *self, int index);

list_node_t *list_rpop(list_t *self);

list_node_t *list_lpop(list_t *self);

void list_remove(list_t *self, list_node_t *node);

void list_destroy(list_t *self);

/* list_t iterator prototypes. */
list_iterator_t *list_iterator_new(list_t *list, list_direction_t direction);

list_iterator_t *list_iterator_new_from_node(list_node_t *node, list_direction_t direction);

list_node_t *list_iterator_next(list_iterator_t *self);

void list_iterator_destroy(list_iterator_t *self);

#endif

