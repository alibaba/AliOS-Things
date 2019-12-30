/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _RBTREE_AUGMENTED_H
#define _RBTREE_AUGMENTED_H

#include "rbtree.h"

struct rbt_augment_callbacks {
    void (*propagate)(struct rbt_node *node, struct rbt_node *stop);
    void (*copy)(struct rbt_node *old, struct rbt_node *new);
    void (*rotate)(struct rbt_node *old, struct rbt_node *new);
};

extern void __rbt_insert_augmented(struct rbt_node *node, struct rbt_root *root,
                void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new));

static inline void rbt_insert_augmented(struct rbt_node *node, struct rbt_root *root,
            const struct rbt_augment_callbacks *augment)
{
    __rbt_insert_augmented(node, root, augment->rotate);
}

#define RB_DECLARE_CALLBACKS(rbtstatic, rbtname, rbtstruct, rbtfield,    \
                 rbttype, rbtaugmented, rbtcompute)                      \
static inline void                                                       \
rbtname ## _propagate(struct rbt_node *rbt, struct rbt_node *stop)       \
{                                                                        \
    while (rbt != stop) {                                                \
        rbtstruct *node = rbt_entry(rbt, rbtstruct, rbtfield);           \
        rbttype augmented = rbtcompute(node);                            \
        if (node->rbtaugmented == augmented)                             \
            break;                                                       \
        node->rbtaugmented = augmented;                                  \
        rbt = rbt_parent(&node->rbtfield);                               \
    }                                                                    \
}                                                                        \
static inline void                                                       \
rbtname ## _copy(struct rbt_node *rbt_old, struct rbt_node *rbt_new)     \
{                                                                        \
    rbtstruct *old = rbt_entry(rbt_old, rbtstruct, rbtfield);            \
    rbtstruct *new = rbt_entry(rbt_new, rbtstruct, rbtfield);            \
    new->rbtaugmented = old->rbtaugmented;                               \
}                                                                        \
static void                                                              \
rbtname ## _rotate(struct rbt_node *rbt_old, struct rbt_node *rbt_new)   \
{                                                                        \
    rbtstruct *old = rbt_entry(rbt_old, rbtstruct, rbtfield);            \
    rbtstruct *new = rbt_entry(rbt_new, rbtstruct, rbtfield);            \
    new->rbtaugmented = old->rbtaugmented;                               \
    old->rbtaugmented = rbtcompute(old);                                 \
}                                                                        \
rbtstatic const struct rbt_augment_callbacks rbtname = {                 \
    rbtname ## _propagate, rbtname ## _copy, rbtname ## _rotate          \
};


#define RBT_RED   0
#define RBT_BLACK 1

#define __rbt_parent(pc)    ((struct rbt_node *)(pc & ~3))

#define __rbt_color(pc)     ((pc) & 1)
#define __rbt_is_black(pc)  __rbt_color(pc)
#define __rbt_is_red(pc)    (!__rbt_color(pc))
#define rbt_color(rbt)       __rbt_color((rbt)->__rbt_parent_color)
#define rbt_is_red(rbt)      __rbt_is_red((rbt)->__rbt_parent_color)
#define rbt_is_black(rbt)    __rbt_is_black((rbt)->__rbt_parent_color)

static inline void rbt_set_parent(struct rbt_node *rbt, struct rbt_node *p)
{
    rbt->__rbt_parent_color = rbt_color(rbt) | (unsigned long)p;
}

static inline void rbt_set_parent_color(struct rbt_node *rbt,
                       struct rbt_node *p, int color)
{
    rbt->__rbt_parent_color = (unsigned long)p | color;
}

static inline void __rbt_change_child(struct rbt_node *old, struct rbt_node *new,
          struct rbt_node *parent, struct rbt_root *root)
{
    if (parent) {
        if (parent->rbt_left == old)
            parent->rbt_left = new;
        else
            parent->rbt_right = new;
    } else
        root->rbt_node = new;
}

extern void __rbt_erase_color(struct rbt_node *parent, struct rbt_root *root,
    void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new));

static inline struct rbt_node *__rbt_erase_augmented(struct rbt_node *node, struct rbt_root *root,
             const struct rbt_augment_callbacks *augment)
{
    struct rbt_node *child = node->rbt_right, *tmp = node->rbt_left;
    struct rbt_node *parent, *rebalance;
    unsigned long pc;

    if (!tmp) {
        pc = node->__rbt_parent_color;
        parent = __rbt_parent(pc);
        __rbt_change_child(node, child, parent, root);
        if (child) {
            child->__rbt_parent_color = pc;
            rebalance = NULL;
        } else
            rebalance = __rbt_is_black(pc) ? parent : NULL;
        tmp = parent;
    } else if (!child) {
        tmp->__rbt_parent_color = pc = node->__rbt_parent_color;
        parent = __rbt_parent(pc);
        __rbt_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    } else {
        struct rbt_node *successor = child, *child2;
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
            rbt_set_parent(child, successor);
            augment->copy(node, successor);
            augment->propagate(parent, successor);
        }

        successor->rbt_left = tmp = node->rbt_left;
        rbt_set_parent(tmp, successor);

        pc = node->__rbt_parent_color;
        tmp = __rbt_parent(pc);
        __rbt_change_child(node, successor, tmp, root);
        if (child2) {
            successor->__rbt_parent_color = pc;
            rbt_set_parent_color(child2, parent, RBT_BLACK);
            rebalance = NULL;
        } else {
            unsigned long pc2 = successor->__rbt_parent_color;
            successor->__rbt_parent_color = pc;
            rebalance = __rbt_is_black(pc2) ? parent : NULL;
        }
        tmp = successor;
    }

    augment->propagate(tmp, NULL);
    return rebalance;
}

static inline void rbt_erase_augmented(struct rbt_node *node, struct rbt_root *root,
           const struct rbt_augment_callbacks *augment)
{
    struct rbt_node *rebalance = __rbt_erase_augmented(node, root, augment);
    if (rebalance)
        __rbt_erase_color(rebalance, root, augment->rotate);
}

#endif /* _RBTREE_AUGMENTED_H */

