/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "rbtree_augmented.h"

static inline void rbt_set_black(struct rbt_node *rbt)
{
    rbt->__rbt_parent_color |= RBT_BLACK;
}

static inline struct rbt_node *rbt_red_parent(struct rbt_node *red)
{
    return (struct rbt_node *)red->__rbt_parent_color;
}

static inline void __rbt_rotate_set_parents(struct rbt_node *old, struct rbt_node *new,
            struct rbt_root *root, int color)
{
    struct rbt_node *parent = rbt_parent(old);
    new->__rbt_parent_color = old->__rbt_parent_color;
    rbt_set_parent_color(old, new, color);
    __rbt_change_child(old, new, parent, root);
}

static void __rbt_insert(struct rbt_node *node, struct rbt_root *root,
        void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new))
{
    struct rbt_node *parent = rbt_red_parent(node), *gparent, *tmp;

    while (1) {
        if (!parent) {
            rbt_set_parent_color(node, NULL, RBT_BLACK);
            break;
        } else if (rbt_is_black(parent))
            break;

        gparent = rbt_red_parent(parent);

        tmp = gparent->rbt_right;
        if (parent != tmp) {
            if (tmp && rbt_is_red(tmp)) {
                rbt_set_parent_color(tmp, gparent, RBT_BLACK);
                rbt_set_parent_color(parent, gparent, RBT_BLACK);
                node = gparent;
                parent = rbt_parent(node);
                rbt_set_parent_color(node, parent, RBT_RED);
                continue;
            }

            tmp = parent->rbt_right;
            if (node == tmp) {
                parent->rbt_right = tmp = node->rbt_left;
                node->rbt_left = parent;
                if (tmp)
                    rbt_set_parent_color(tmp, parent,
                                RBT_BLACK);
                rbt_set_parent_color(parent, node, RBT_RED);
                augment_rotate(parent, node);
                parent = node;
                tmp = node->rbt_right;
            }

            gparent->rbt_left = tmp;
            parent->rbt_right = gparent;
            if (tmp)
                rbt_set_parent_color(tmp, gparent, RBT_BLACK);
            __rbt_rotate_set_parents(gparent, parent, root, RBT_RED);
            augment_rotate(gparent, parent);
            break;
        } else {
            tmp = gparent->rbt_left;
            if (tmp && rbt_is_red(tmp)) {
                rbt_set_parent_color(tmp, gparent, RBT_BLACK);
                rbt_set_parent_color(parent, gparent, RBT_BLACK);
                node = gparent;
                parent = rbt_parent(node);
                rbt_set_parent_color(node, parent, RBT_RED);
                continue;
            }

            tmp = parent->rbt_left;
            if (node == tmp) {
                parent->rbt_left = tmp = node->rbt_right;
                node->rbt_right = parent;
                if (tmp)
                    rbt_set_parent_color(tmp, parent,
                                RBT_BLACK);
                rbt_set_parent_color(parent, node, RBT_RED);
                augment_rotate(parent, node);
                parent = node;
                tmp = node->rbt_left;
            }

            gparent->rbt_right = tmp;
            parent->rbt_left = gparent;
            if (tmp)
                rbt_set_parent_color(tmp, gparent, RBT_BLACK);
            __rbt_rotate_set_parents(gparent, parent, root, RBT_RED);
            augment_rotate(gparent, parent);
            break;
        }
    }
}

static void ____rbt_erase_color(struct rbt_node *parent, struct rbt_root *root,
                 void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new))
{
    struct rbt_node *node = NULL, *sibling, *tmp1, *tmp2;

    while (1) {
        sibling = parent->rbt_right;
        if (node != sibling) {
            if (rbt_is_red(sibling)) {
                parent->rbt_right = tmp1 = sibling->rbt_left;
                sibling->rbt_left = parent;
                rbt_set_parent_color(tmp1, parent, RBT_BLACK);
                __rbt_rotate_set_parents(parent, sibling, root,
                            RBT_RED);
                augment_rotate(parent, sibling);
                sibling = tmp1;
            }
            tmp1 = sibling->rbt_right;
            if (!tmp1 || rbt_is_black(tmp1)) {
                tmp2 = sibling->rbt_left;
                if (!tmp2 || rbt_is_black(tmp2)) {
                    rbt_set_parent_color(sibling, parent,
                                RBT_RED);
                    if (rbt_is_red(parent))
                        rbt_set_black(parent);
                    else {
                        node = parent;
                        parent = rbt_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                sibling->rbt_left = tmp1 = tmp2->rbt_right;
                tmp2->rbt_right = sibling;
                parent->rbt_right = tmp2;
                if (tmp1)
                    rbt_set_parent_color(tmp1, sibling,
                                RBT_BLACK);
                augment_rotate(sibling, tmp2);
                tmp1 = sibling;
                sibling = tmp2;
            }
            parent->rbt_right = tmp2 = sibling->rbt_left;
            sibling->rbt_left = parent;
            rbt_set_parent_color(tmp1, sibling, RBT_BLACK);
            if (tmp2)
                rbt_set_parent(tmp2, parent);
            __rbt_rotate_set_parents(parent, sibling, root,
                        RBT_BLACK);
            augment_rotate(parent, sibling);
            break;
        } else {
            sibling = parent->rbt_left;
            if (rbt_is_red(sibling)) {
                parent->rbt_left = tmp1 = sibling->rbt_right;
                sibling->rbt_right = parent;
                rbt_set_parent_color(tmp1, parent, RBT_BLACK);
                __rbt_rotate_set_parents(parent, sibling, root,
                            RBT_RED);
                augment_rotate(parent, sibling);
                sibling = tmp1;
            }
            tmp1 = sibling->rbt_left;
            if (!tmp1 || rbt_is_black(tmp1)) {
                tmp2 = sibling->rbt_right;
                if (!tmp2 || rbt_is_black(tmp2)) {
                    rbt_set_parent_color(sibling, parent,
                                RBT_RED);
                    if (rbt_is_red(parent))
                        rbt_set_black(parent);
                    else {
                        node = parent;
                        parent = rbt_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                sibling->rbt_right = tmp1 = tmp2->rbt_left;
                tmp2->rbt_left = sibling;
                parent->rbt_left = tmp2;
                if (tmp1)
                    rbt_set_parent_color(tmp1, sibling,
                                RBT_BLACK);
                augment_rotate(sibling, tmp2);
                tmp1 = sibling;
                sibling = tmp2;
            }
            parent->rbt_left = tmp2 = sibling->rbt_right;
            sibling->rbt_right = parent;
            rbt_set_parent_color(tmp1, sibling, RBT_BLACK);
            if (tmp2)
                rbt_set_parent(tmp2, parent);
            __rbt_rotate_set_parents(parent, sibling, root,
                        RBT_BLACK);
            augment_rotate(parent, sibling);
            break;
        }
    }
}

void __rbt_erase_color(struct rbt_node *parent, struct rbt_root *root,
    void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new))
{
    ____rbt_erase_color(parent, root, augment_rotate);
}


static inline void dummy_propagate(struct rbt_node *node, struct rbt_node *stop) {}
static inline void dummy_copy(struct rbt_node *old, struct rbt_node *new) {}
static inline void dummy_rotate(struct rbt_node *old, struct rbt_node *new) {}

static const struct rbt_augment_callbacks dummy_callbacks = {
    dummy_propagate, dummy_copy, dummy_rotate
};

void rbt_insert_color(struct rbt_node *node, struct rbt_root *root)
{
    __rbt_insert(node, root, dummy_rotate);
}

void rbt_erase(struct rbt_node *node, struct rbt_root *root)
{
    struct rbt_node *rebalance;
    rebalance = __rbt_erase_augmented(node, root, &dummy_callbacks);
    if (rebalance)
        __rbt_erase_color(rebalance, root, dummy_rotate);
}

void __rbt_insert_augmented(struct rbt_node *node, struct rbt_root *root,
         void (*augment_rotate)(struct rbt_node *old, struct rbt_node *new))
{
    __rbt_insert(node, root, augment_rotate);
}

struct rbt_node *rbt_first(const struct rbt_root *root)
{
    struct rbt_node *n;

    n = root->rbt_node;
    if (!n)
        return NULL;
    while (n->rbt_left)
        n = n->rbt_left;
    return n;
}

struct rbt_node *rbt_last(const struct rbt_root *root)
{
    struct rbt_node    *n;

    n = root->rbt_node;
    if (!n)
        return NULL;
    while (n->rbt_right)
        n = n->rbt_right;
    return n;
}

struct rbt_node *rbt_next(const struct rbt_node *node)
{
    struct rbt_node *parent;

    if (RBT_EMPTY_NODE(node))
        return NULL;

    if (node->rbt_right) {
        node = node->rbt_right;
        while (node->rbt_left)
            node=node->rbt_left;
        return (struct rbt_node *)node;
    }

    while ((parent = rbt_parent(node)) && node == parent->rbt_right)
        node = parent;

    return parent;
}

struct rbt_node *rbt_prev(const struct rbt_node *node)
{
    struct rbt_node *parent;

    if (RBT_EMPTY_NODE(node))
        return NULL;

    if (node->rbt_left) {
        node = node->rbt_left;
        while (node->rbt_right)
            node=node->rbt_right;
        return (struct rbt_node *)node;
    }

    while ((parent = rbt_parent(node)) && node == parent->rbt_left)
        node = parent;

    return parent;
}

void rbt_replace_node(struct rbt_node *victim, struct rbt_node *new,
             struct rbt_root *root)
{
    struct rbt_node *parent = rbt_parent(victim);

    __rbt_change_child(victim, new, parent, root);
    if (victim->rbt_left)
        rbt_set_parent(victim->rbt_left, new);
    if (victim->rbt_right)
        rbt_set_parent(victim->rbt_right, new);

    *new = *victim;
}

static struct rbt_node *rbt_left_deepest_node(const struct rbt_node *node)
{
    for (;;) {
        if (node->rbt_left)
            node = node->rbt_left;
        else if (node->rbt_right)
            node = node->rbt_right;
        else
            return (struct rbt_node *)node;
    }
}

struct rbt_node *rbt_next_postorder(const struct rbt_node *node)
{
    const struct rbt_node *parent;
    if (!node)
        return NULL;
    parent = rbt_parent(node);

    if (parent && node == parent->rbt_left && parent->rbt_right) {
        return rbt_left_deepest_node(parent->rbt_right);
    } else
        return (struct rbt_node *)parent;
}

struct rbt_node *rbt_first_postorder(const struct rbt_root *root)
{
    if (!root->rbt_node)
        return NULL;

    return rbt_left_deepest_node(root->rbt_node);
}

