/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include "k_rbtree.h"

static inline void rbtree_set_black(struct k_rbtree_node_t *rbt)
{
    rbt->rbt_parent_color |= K_RBTREE_BLACK;
}

static inline void rbtree_rotate_set_parents(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new,
            struct k_rbtree_root_t *root, int color)
{
    struct k_rbtree_node_t *parent = K_RBTREE_PARENT(old);
    new->rbt_parent_color = old->rbt_parent_color;
    rbtree_set_parent_color(old, new, color);
    rbtree_change_child(old, new, parent, root);
}

static void rbtree_insert(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
        void (*augment_rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new))
{
    struct k_rbtree_node_t *parent = (struct k_rbtree_node_t *)node->rbt_parent_color, *gparent, *tmp;

    while (1) {
        if (!parent) {
            rbtree_set_parent_color(node, NULL, K_RBTREE_BLACK);
            break;
        } else if (K_RBTREE_IS_BLACK(parent)){
            break;
        }

        gparent = (struct k_rbtree_node_t *)parent->rbt_parent_color;

        tmp = gparent->rbt_right;
        if (parent != tmp) {
            if (tmp && K_RBTREE_IS_RED(tmp)) {
                rbtree_set_parent_color(tmp, gparent, K_RBTREE_BLACK);
                rbtree_set_parent_color(parent, gparent, K_RBTREE_BLACK);
                node = gparent;
                parent = K_RBTREE_PARENT(node);
                rbtree_set_parent_color(node, parent, K_RBTREE_RED);
                continue;
            }

            tmp = parent->rbt_right;
            if (node == tmp) {
                parent->rbt_right = tmp = node->rbt_left;
                node->rbt_left = parent;
                if (tmp){
                    rbtree_set_parent_color(tmp, parent,
                                K_RBTREE_BLACK);
                }
                rbtree_set_parent_color(parent, node, K_RBTREE_RED);
                augment_rotate(parent, node);
                parent = node;
                tmp = node->rbt_right;
            }

            gparent->rbt_left = tmp;
            parent->rbt_right = gparent;
            if (tmp){
                rbtree_set_parent_color(tmp, gparent, K_RBTREE_BLACK);
            }
            rbtree_rotate_set_parents(gparent, parent, root, K_RBTREE_RED);
            augment_rotate(gparent, parent);
            break;
        } else {
            tmp = gparent->rbt_left;
            if (tmp && K_RBTREE_IS_RED(tmp)) {
                rbtree_set_parent_color(tmp, gparent, K_RBTREE_BLACK);
                rbtree_set_parent_color(parent, gparent, K_RBTREE_BLACK);
                node = gparent;
                parent = K_RBTREE_PARENT(node);
                rbtree_set_parent_color(node, parent, K_RBTREE_RED);
                continue;
            }

            tmp = parent->rbt_left;
            if (node == tmp) {
                parent->rbt_left = tmp = node->rbt_right;
                node->rbt_right = parent;
                if (tmp){
                    rbtree_set_parent_color(tmp, parent,
                                K_RBTREE_BLACK);
                }
                rbtree_set_parent_color(parent, node, K_RBTREE_RED);
                augment_rotate(parent, node);
                parent = node;
                tmp = node->rbt_left;
            }

            gparent->rbt_right = tmp;
            parent->rbt_left = gparent;
            if (tmp){
                rbtree_set_parent_color(tmp, gparent, K_RBTREE_BLACK);
            }
            rbtree_rotate_set_parents(gparent, parent, root, K_RBTREE_RED);
            augment_rotate(gparent, parent);
            break;
        }
    }
}

void rbtree_erase_color(struct k_rbtree_node_t *parent, struct k_rbtree_root_t *root,
                 void (*augment_rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new))
{
    struct k_rbtree_node_t *node = NULL, *sibling, *tmp1, *tmp2;

    while (1) {
        sibling = parent->rbt_right;
        if (node != sibling) {
            if (K_RBTREE_IS_RED(sibling)) {
                parent->rbt_right = tmp1 = sibling->rbt_left;
                sibling->rbt_left = parent;
                rbtree_set_parent_color(tmp1, parent, K_RBTREE_BLACK);
                rbtree_rotate_set_parents(parent, sibling, root,
                            K_RBTREE_RED);
                augment_rotate(parent, sibling);
                sibling = tmp1;
            }
            tmp1 = sibling->rbt_right;
            if (!tmp1 || K_RBTREE_IS_BLACK(tmp1)) {
                tmp2 = sibling->rbt_left;
                if (!tmp2 || K_RBTREE_IS_BLACK(tmp2)) {
                    rbtree_set_parent_color(sibling, parent,
                                K_RBTREE_RED);
                    if (K_RBTREE_IS_RED(parent)){
                        rbtree_set_black(parent);
                    }
                    else {
                        node = parent;
                        parent = K_RBTREE_PARENT(node);
                        if (parent){
                            continue;
                        }
                    }
                    break;
                }
                sibling->rbt_left = tmp1 = tmp2->rbt_right;
                tmp2->rbt_right = sibling;
                parent->rbt_right = tmp2;
                if (tmp1){
                    rbtree_set_parent_color(tmp1, sibling,
                                K_RBTREE_BLACK);
                }
                augment_rotate(sibling, tmp2);
                tmp1 = sibling;
                sibling = tmp2;
            }
            parent->rbt_right = tmp2 = sibling->rbt_left;
            sibling->rbt_left = parent;
            rbtree_set_parent_color(tmp1, sibling, K_RBTREE_BLACK);
            if (tmp2){
                rbtree_set_parent(tmp2, parent);
            }
            rbtree_rotate_set_parents(parent, sibling, root,
                        K_RBTREE_BLACK);
            augment_rotate(parent, sibling);
            break;
        } else {
            sibling = parent->rbt_left;
            if (K_RBTREE_IS_RED(sibling)) {
                parent->rbt_left = tmp1 = sibling->rbt_right;
                sibling->rbt_right = parent;
                rbtree_set_parent_color(tmp1, parent, K_RBTREE_BLACK);
                rbtree_rotate_set_parents(parent, sibling, root,
                            K_RBTREE_RED);
                augment_rotate(parent, sibling);
                sibling = tmp1;
            }
            tmp1 = sibling->rbt_left;
            if (!tmp1 || K_RBTREE_IS_BLACK(tmp1)) {
                tmp2 = sibling->rbt_right;
                if (!tmp2 || K_RBTREE_IS_BLACK(tmp2)) {
                    rbtree_set_parent_color(sibling, parent,
                                K_RBTREE_RED);
                    if (K_RBTREE_IS_RED(parent)){
                        rbtree_set_black(parent);
                    }
                    else {
                        node = parent;
                        parent = K_RBTREE_PARENT(node);
                        if (parent){
                            continue;                            
                        }
                    }
                    break;
                }
                sibling->rbt_right = tmp1 = tmp2->rbt_left;
                tmp2->rbt_left = sibling;
                parent->rbt_left = tmp2;
                if (tmp1){
                    rbtree_set_parent_color(tmp1, sibling,
                                K_RBTREE_BLACK);
                }
                augment_rotate(sibling, tmp2);
                tmp1 = sibling;
                sibling = tmp2;
            }
            parent->rbt_left = tmp2 = sibling->rbt_right;
            sibling->rbt_right = parent;
            rbtree_set_parent_color(tmp1, sibling, K_RBTREE_BLACK);
            if (tmp2){
                rbtree_set_parent(tmp2, parent);
            }
            rbtree_rotate_set_parents(parent, sibling, root,
                        K_RBTREE_BLACK);
            augment_rotate(parent, sibling);
            break;
        }
    }
}

static inline void rbtree_dummy_propagate(struct k_rbtree_node_t *node, struct k_rbtree_node_t *stop) {}
static inline void rbtree_dummy_copy(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new) {}
static inline void rbtree_dummy_rotate(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new) {}

static const struct k_rbtree_augment_callbacks dummy_callbacks = {
    rbtree_dummy_propagate, rbtree_dummy_copy, rbtree_dummy_rotate
};

void rbtree_insert_augmented(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root,
         void (*augment_rotate)(struct k_rbtree_node_t *old, struct k_rbtree_node_t *new))
{
    rbtree_insert(node, root, augment_rotate);
}

void k_rbtree_insert_color(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root)
{
    rbtree_insert(node, root, rbtree_dummy_rotate);
}

void k_rbtree_erase(struct k_rbtree_node_t *node, struct k_rbtree_root_t *root)
{
    struct k_rbtree_node_t *rebalance;
    rebalance = rbtree_erase_augmented(node, root, &dummy_callbacks);
    if (rebalance){
        rbtree_erase_color(rebalance, root, rbtree_dummy_rotate);
    }
}

struct k_rbtree_node_t *k_rbtree_first(const struct k_rbtree_root_t *root)
{
    struct k_rbtree_node_t *n;

    n = root->rbt_node;
    if (!n){
        return NULL;
    }
    while (n->rbt_left){
        n = n->rbt_left;
    }
    return n;
}

struct k_rbtree_node_t *k_rbtree_last(const struct k_rbtree_root_t *root)
{
    struct k_rbtree_node_t    *n;

    n = root->rbt_node;
    if (!n){
        return NULL;
    }
    while (n->rbt_right){
        n = n->rbt_right;
    }
    return n;
}

struct k_rbtree_node_t *k_rbtree_next(const struct k_rbtree_node_t *node)
{
    struct k_rbtree_node_t *parent;

    if (K_RBTREE_EMPTY_NODE(node)){
        return NULL;
    }

    if (node->rbt_right) {
        node = node->rbt_right;
        while (node->rbt_left){
            node=node->rbt_left;
        }
        return (struct k_rbtree_node_t *)node;
    }

    while ((parent = K_RBTREE_PARENT(node)) && node == parent->rbt_right){
        node = parent;
    }

    return parent;
}

struct k_rbtree_node_t *k_rbtree_prev(const struct k_rbtree_node_t *node)
{
    struct k_rbtree_node_t *parent;

    if (K_RBTREE_EMPTY_NODE(node)){
        return NULL;
    }

    if (node->rbt_left) {
        node = node->rbt_left;
        while (node->rbt_right){
            node=node->rbt_right;
        }
        return (struct k_rbtree_node_t *)node;
    }

    while ((parent = K_RBTREE_PARENT(node)) && node == parent->rbt_left){
        node = parent;
    }

    return parent;
}

void k_rbtree_replace_node(struct k_rbtree_node_t *victim, struct k_rbtree_node_t *new,
             struct k_rbtree_root_t *root)
{
    struct k_rbtree_node_t *parent = K_RBTREE_PARENT(victim);

    rbtree_change_child(victim, new, parent, root);
    if (victim->rbt_left){
        rbtree_set_parent(victim->rbt_left, new);
    }
    if (victim->rbt_right){
        rbtree_set_parent(victim->rbt_right, new);
    }

    *new = *victim;
}

static struct k_rbtree_node_t *k_rbtree_left_deepest_node(const struct k_rbtree_node_t *node)
{
    for (;;) {
        if (node->rbt_left){
            node = node->rbt_left;
        }
        else if (node->rbt_right){
            node = node->rbt_right;
        }
        else{
            return (struct k_rbtree_node_t *)node;
        }
    }
}

struct k_rbtree_node_t *k_rbtree_next_postorder(const struct k_rbtree_node_t *node)
{
    const struct k_rbtree_node_t *parent;
    if (!node){
        return NULL;
    }

    parent = K_RBTREE_PARENT(node);

    if (parent && node == parent->rbt_left && parent->rbt_right) {
        return k_rbtree_left_deepest_node(parent->rbt_right);
    } else{
        return (struct k_rbtree_node_t *)parent;
    }
}

struct k_rbtree_node_t *k_rbtree_first_postorder(const struct k_rbtree_root_t *root)
{
    if (!root->rbt_node){
        return NULL;
    }

    return k_rbtree_left_deepest_node(root->rbt_node);
}

