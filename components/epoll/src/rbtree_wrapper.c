#include <private/epoll_inner.h>
#include "k_api.h"
#include <k_rbtree.h>


epoll_item_t *rbr_find(struct k_rbtree_root_t *root, int fd)
{
    struct k_rbtree_node_t *rbtnode = root->rbt_node;

    while (rbtnode != NULL) {

        epoll_item_t *mynode = k_rbtree_entry(rbtnode, epoll_item_t, tree_node);
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

int rbt_insert(struct k_rbtree_root_t *root, epoll_item_t *item)
{
    struct k_rbtree_node_t **tmp = &(root->rbt_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        epoll_item_t *my = k_rbtree_entry(*tmp, epoll_item_t, tree_node);
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
    k_rbtree_link_node(&item->tree_node, parent, tmp);
    k_rbtree_insert_color(&item->tree_node, root);

    return 0;
}

int rbt_delete(struct k_rbtree_root_t *root, int fd)
{
    epoll_item_t *mynode;

    if ((mynode = rbr_find(root, fd)) == NULL) {
        return -1;
    }

    k_rbtree_erase(&mynode->tree_node, root);
    free(mynode);
    return 0;
}
