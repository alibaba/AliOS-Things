/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_AVL_H
#define LEXBOR_AVL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "base.h"
#include "dobject.h"


typedef struct lexbor_avl_node lexbor_avl_node_t;

typedef void (*lexbor_avl_node_f)(lexbor_avl_node_t *avl_node, void *ctx);

struct lexbor_avl_node {
    size_t            type;
    short             height;
    void              *value;

    lexbor_avl_node_t *left;
    lexbor_avl_node_t *right;
    lexbor_avl_node_t *parent;
};

typedef struct {
    lexbor_dobject_t *nodes;
}
lexbor_avl_t;


LXB_API lexbor_avl_t *
lexbor_avl_create(void);

LXB_API lxb_status_t
lexbor_avl_init(lexbor_avl_t *avl, size_t chunk_len);

LXB_API void
lexbor_avl_clean(lexbor_avl_t *avl);

LXB_API lexbor_avl_t *
lexbor_avl_destroy(lexbor_avl_t *avl, bool self_destroy);


LXB_API lexbor_avl_node_t *
lexbor_avl_node_make(lexbor_avl_t *avl, size_t type, void *value);

LXB_API void
lexbor_avl_node_clean(lexbor_avl_node_t *node);

LXB_API lexbor_avl_node_t *
lexbor_avl_node_destroy(lexbor_avl_t *avl, lexbor_avl_node_t *node,
                        bool self_destroy);


LXB_API lexbor_avl_node_t *
lexbor_avl_insert(lexbor_avl_t *avl, lexbor_avl_node_t **scope,
                  size_t type, void *value);

LXB_API lexbor_avl_node_t *
lexbor_avl_search(lexbor_avl_t *avl, lexbor_avl_node_t *scope, size_t type);

LXB_API void *
lexbor_avl_remove(lexbor_avl_t *avl, lexbor_avl_node_t **scope, size_t type);


LXB_API void
lexbor_avl_foreach_recursion(lexbor_avl_t *avl, lexbor_avl_node_t *scope,
                             lexbor_avl_node_f callback, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_AVL_H */
