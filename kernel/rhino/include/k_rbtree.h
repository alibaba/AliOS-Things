/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _K_RBTREE_H
#define _K_RBTREE_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

struct k_rbtree_node_t {
    unsigned long  rbt_parent_color;
    struct k_rbtree_node_t *rbt_right;
    struct k_rbtree_node_t *rbt_left;
} __attribute__((aligned(sizeof(long))));

struct k_rbtree_root_t {
    struct k_rbtree_node_t *rbt_node;
};

struct k_rbtree_augment_callbacks {
    void (*propagate)(struct k_rbtree_node_t *node, struct k_rbtree_node_t *stop);
    void (*copy)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new_node);
    void (*rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new_node);
};

#define K_RBTREE_RED   0
#define K_RBTREE_BLACK 1

#define RBT_ROOT (struct k_rbtree_root_t) { NULL, }

#define __rbtree_parent(pcolor)    ((struct k_rbtree_node_t *)(pcolor & ~3))
#define __rbtree_color(pcolor)     ((pcolor) & 1)
#define __rbtree_is_black(pcolor)  __rbtree_color(pcolor)
#define __rbtree_is_red(pcolor)    (!__rbtree_color(pcolor))

#define K_RBTREE_PARENT(rbt)      ((struct k_rbtree_node_t *)((rbt)->rbt_parent_color & ~3))
#define K_RBTREE_COLOR(rbt)       __rbtree_color((rbt)->rbt_parent_color)
#define K_RBTREE_IS_BLACK(rbt)    __rbtree_is_black((rbt)->rbt_parent_color)
#define K_RBTREE_IS_RED(rbt)      __rbtree_is_red((rbt)->rbt_parent_color)

#define K_RBTREE_EMPTY_ROOT(root)  ((root)->rbt_node == NULL)
#define K_RBTREE_EMPTY_NODE(node)  \
    ((node)->rbt_parent_color == (unsigned long)(node))
#define K_RBTREE_CLEAR_NODE(node)  \
    ((node)->rbt_parent_color = (unsigned long)(node))

#define k_rbtree_entry(ptr, type, member) ((type *)((uint8_t *)(ptr) - (size_t)(&((type *)0)->member)))

#define k_rbtree_entry_safe(ptr, type, member) \
    ({ ptr ? k_rbtree_entry(ptr, type, member) : NULL; })

#define k_rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
    for (pos = k_rbtree_entry_safe(k_rbtree_first_postorder(root), typeof(*pos), field); \
         pos && ({ n = k_rbtree_entry_safe(k_rbtree_next_postorder(&pos->field), \
            typeof(*pos), field); 1; }); \
         pos = n)

extern void rbtree_insert_augmented(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
                void (*augment_rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new_node));
extern void rbtree_erase_color(struct k_rbtree_node_t *parent, struct k_rbtree_root_t *root,
    void (*augment_rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new_node));

static inline void rbtree_set_parent(struct k_rbtree_node_t *rbt, struct k_rbtree_node_t *p)
{
    rbt->rbt_parent_color = K_RBTREE_COLOR(rbt) | (unsigned long)p;
}

static inline void rbtree_set_parent_color(struct k_rbtree_node_t *rbt,
                       struct k_rbtree_node_t *p, int color)
{
    rbt->rbt_parent_color = (unsigned long)p | color;
}

static inline void rbtree_change_child(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new_node,
          struct k_rbtree_node_t *parent, struct k_rbtree_root_t *root)
{
    if (parent) {
        if (parent->rbt_left == old)
            parent->rbt_left = new_node;
        else
            parent->rbt_right = new_node;
    } else
        root->rbt_node = new_node;
}

static inline struct k_rbtree_node_t *rbtree_erase_augmented(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
             const struct k_rbtree_augment_callbacks *augment)
{
    struct k_rbtree_node_t *child = node->rbt_right, *tmp = node->rbt_left;
    struct k_rbtree_node_t *parent, *rebalance;
    unsigned long pc;

    if (!tmp) {
        pc = node->rbt_parent_color;
        parent = __rbtree_parent(pc);
        rbtree_change_child(node, child, parent, root);
        if (child) {
            child->rbt_parent_color = pc;
            rebalance = NULL;
        } else
            rebalance = __rbtree_is_black(pc) ? parent : NULL;
        tmp = parent;
    } else if (!child) {
        tmp->rbt_parent_color = pc = node->rbt_parent_color;
        parent = __rbtree_parent(pc);
        rbtree_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    } else {
        struct k_rbtree_node_t *successor = child, *child2;
        tmp = child->rbt_left;
        if (!tmp) {
            parent = successor;
            child2 = successor->rbt_right;
            augment->copy(node, successor);
        } else {
            do {
                parent = successor;
                successor = tmp;
                tmp = tmp->rbt_left;
            } while (tmp);
            parent->rbt_left = child2 = successor->rbt_right;
            successor->rbt_right = child;
            rbtree_set_parent(child, successor);
            augment->copy(node, successor);
            augment->propagate(parent, successor);
        }

        successor->rbt_left = tmp = node->rbt_left;
        rbtree_set_parent(tmp, successor);

        pc = node->rbt_parent_color;
        tmp = __rbtree_parent(pc);
        rbtree_change_child(node, successor, tmp, root);
        if (child2) {
            successor->rbt_parent_color = pc;
            rbtree_set_parent_color(child2, parent, K_RBTREE_BLACK);
            rebalance = NULL;
        } else {
            unsigned long pc2 = successor->rbt_parent_color;
            successor->rbt_parent_color = pc;
            rebalance = __rbtree_is_black(pc2) ? parent : NULL;
        }
        tmp = successor;
    }

    augment->propagate(tmp, NULL);
    return rebalance;
}

static inline void k_rbtree_erase_augmented(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
           const struct k_rbtree_augment_callbacks *augment)
{
    struct k_rbtree_node_t *rebalance = rbtree_erase_augmented(node, root, augment);
    if (rebalance){
        rbtree_erase_color(rebalance, root, augment->rotate);
    }
}

static inline void k_rbtree_insert_augmented(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
            const struct k_rbtree_augment_callbacks *augment)
{
    rbtree_insert_augmented(node, root, augment->rotate);
}

static inline void k_rbtree_link_node(struct k_rbtree_node_t * node, struct k_rbtree_node_t * parent,
                struct k_rbtree_node_t ** rbt_link)
{
    node->rbt_parent_color = (unsigned long)parent;
    node->rbt_left = node->rbt_right = NULL;
    *rbt_link = node;
}

extern void k_rbtree_insert_color(struct k_rbtree_node_t *, struct k_rbtree_root_t *);
extern void k_rbtree_erase(struct k_rbtree_node_t *, struct k_rbtree_root_t *);
extern struct k_rbtree_node_t *k_rbtree_next(const struct k_rbtree_node_t *);
extern struct k_rbtree_node_t *k_rbtree_prev(const struct k_rbtree_node_t *);
extern struct k_rbtree_node_t *k_rbtree_first(const struct k_rbtree_root_t *);
extern struct k_rbtree_node_t *k_rbtree_last(const struct k_rbtree_root_t *);
extern struct k_rbtree_node_t *k_rbtree_first_postorder(const struct k_rbtree_root_t *);
extern struct k_rbtree_node_t *k_rbtree_next_postorder(const struct k_rbtree_node_t *);
extern void k_rbtree_replace_node(struct k_rbtree_node_t *victim, struct k_rbtree_node_t *new_node, struct k_rbtree_root_t *root);

#endif /* _K_RBTREE_H */

