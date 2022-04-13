#include <stdlib.h>
#include <epoll_inner.h>
#include "k_api.h"
#include <rbtree_augmented.h>


epoll_item_t *rbr_find(struct rbt_root *root, int fd)
{
    struct rbt_node *rbtnode = root->rbt_node;

    while (rbtnode != NULL) {

        epoll_item_t *mynode = rbt_entry(rbtnode, epoll_item_t, tree_node);
        if (mynode == NULL) {
            return NULL;
        }
        if (fd < mynode->fd) {
            rbtnode = rbtnode->rbt_left;
        } else if (fd > mynode->fd) {
            rbtnode = rbtnode->rbt_right;
        } else {
            return mynode;
        }
    }

    return NULL;
}

int rbt_insert(struct rbt_root *root, epoll_item_t *item)
{
    struct rbt_node **tmp = &(root->rbt_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        epoll_item_t *my = rbt_entry(*tmp, epoll_item_t, tree_node);
        if (my == NULL) {
            return -1;
        }
        parent = *tmp;
        if (item->fd <= my->fd) {
            tmp = &((*tmp)->rbt_left);
        } else if (item->fd > my->fd) {
            tmp = &((*tmp)->rbt_right);
        } else {
            return -1;
        }
    }

    /* Add new node and rebalance tree. */
    rbt_link_node(&item->tree_node, parent, tmp);
    rbt_insert_color(&item->tree_node, root);

    return 0;
}

int rbt_delete(struct rbt_root *root, int fd)
{
    epoll_item_t *mynode;

    if ((mynode = rbr_find(root, fd)) == NULL) {
        return -1;
    }

    rbt_erase(&mynode->tree_node, root);
    free(mynode);
    return 0;
}
