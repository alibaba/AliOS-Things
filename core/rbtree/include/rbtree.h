/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _RBTREE_H
#define _RBTREE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

struct rbt_node {
    unsigned long  __rbt_parent_color;
    struct rbt_node *rbt_right;
    struct rbt_node *rbt_left;
} __attribute__((aligned(sizeof(long))));

struct rbt_root {
    struct rbt_node *rbt_node;
};

#define rbt_parent(r) ((struct rbt_node *)((r)->__rbt_parent_color & ~3))

#define RBT_ROOT (struct rbt_root) { NULL, }
#define rbt_entry(ptr, type, member) krhino_list_entry(ptr, type, member)

#define RBT_EMPTY_ROOT(root)  ((root)->rbt_node == NULL)

#define RBT_EMPTY_NODE(node)  \
    ((node)->__rbt_parent_color == (unsigned long)(node))
#define RBT_CLEAR_NODE(node)  \
    ((node)->__rbt_parent_color = (unsigned long)(node))

extern void rbt_insert_color(struct rbt_node *, struct rbt_root *);
extern void rbt_erase(struct rbt_node *, struct rbt_root *);

extern struct rbt_node *rbt_next(const struct rbt_node *);
extern struct rbt_node *rbt_prev(const struct rbt_node *);
extern struct rbt_node *rbt_first(const struct rbt_root *);
extern struct rbt_node *rbt_last(const struct rbt_root *);

extern struct rbt_node *rbt_first_postorder(const struct rbt_root *);
extern struct rbt_node *rbt_next_postorder(const struct rbt_node *);

extern void rbt_replace_node(struct rbt_node *victim, struct rbt_node *new,
                struct rbt_root *root);

static inline void rbt_link_node(struct rbt_node * node, struct rbt_node * parent,
                struct rbt_node ** rbt_link)
{
    node->__rbt_parent_color = (unsigned long)parent;
    node->rbt_left = node->rbt_right = NULL;

    *rbt_link = node;
}

#define rbt_entry_safe(ptr, type, member) \
    ({ ptr ? rbt_entry(ptr, type, member) : NULL; })

#define rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
    for (pos = rbt_entry_safe(rbt_first_postorder(root), typeof(*pos), field); \
         pos && ({ n = rbt_entry_safe(rbt_next_postorder(&pos->field), \
            typeof(*pos), field); 1; }); \
         pos = n)

#endif /* _RBTREE_H */

