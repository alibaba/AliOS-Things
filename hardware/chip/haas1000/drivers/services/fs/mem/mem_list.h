/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef _LIST_H
#define _LIST_H

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
list_node_t *list_node_new_du(void *val);

/* list_t prototypes. */
list_t *list_new_du(void);

list_node_t *list_rpush_du(list_t *self, list_node_t *node);

list_node_t *list_lpush_du(list_t *self, list_node_t *node);

list_node_t *list_find_du(list_t *self, void *val);

list_node_t *list_at_du(list_t *self, int index);

list_node_t *list_rpop_du(list_t *self);

list_node_t *list_lpop_du(list_t *self);

void list_remove_du(list_t *self, list_node_t *node);

void list_destroy_du(list_t *self);

/* list_t iterator prototypes. */
list_iterator_t *list_iterator_new_du(list_t *list, list_direction_t direction);

list_iterator_t *list_iterator_new_from_node_du(list_node_t *node, list_direction_t direction);

list_node_t *list_iterator_next_du(list_iterator_t *self);

void list_iterator_destroy_du(list_iterator_t *self);

#endif

