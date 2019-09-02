/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "aos/kernel.h"
#include "yunit/yunit.h"

#include "rbtree_augmented.h"

#define CHECK_INSERT 1
#define CHECK_DELETE 1
#define LENGTH(a) ( (sizeof(a)) / (sizeof(a[0])) )

struct my_node {
    struct rbt_node rbt_node;  /* rbttree node */
    int             key;       /* key */
};

/*
 * Search rbtree for the node with key. If the node exist,
 * return the node pointer, else return NULL
 */
static struct my_node *my_search(struct rbt_root *root, int key)
{
    struct rbt_node *rbtnode = root->rbt_node;

    while (rbtnode!=NULL)
    {
        struct my_node *mynode = rbt_entry(rbtnode, struct my_node, rbt_node);

        if (key < mynode->key)
            rbtnode = rbtnode->rbt_left;
        else if (key > mynode->key)
            rbtnode = rbtnode->rbt_right;
        else
            return mynode;
    }

    return NULL;
}

/*
 * Insert @key into rbtree, On success, return 0, else return -1
 */
static int my_insert(struct rbt_root *root, int key)
{
    struct my_node *mynode; // create new node
    struct rbt_node **tmp = &(root->rbt_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp)
    {
        struct my_node *my = rbt_entry(*tmp, struct my_node, rbt_node);

        parent = *tmp;
        if (key < my->key)
            tmp = &((*tmp)->rbt_left);
        else if (key > my->key)
            tmp = &((*tmp)->rbt_right);
        else
            return -1;
    }

    if ((mynode = malloc(sizeof(struct my_node))) == NULL)
        return -1;

    mynode->key = key;

    /* Add new node and rebalance tree. */
    rbt_link_node(&mynode->rbt_node, parent, tmp);
    rbt_insert_color(&mynode->rbt_node, root);

    return 0;
}

/*
 * Delete the node whose key value equals @key
 */
static void my_delete(struct rbt_root *root, int key)
{
    struct my_node *mynode;

    if ((mynode = my_search(root, key)) == NULL)
        return ;

    rbt_erase(&mynode->rbt_node, root);
    free(mynode);
}

/*
 * Print the rbtree
 */
static void print_rbttree(struct rbt_node *tree, int key, int direction)
{
    if(tree != NULL)
    {
        if(direction==0)    // tree is the root node
            printf("%2d(B) is root\n", key);
        else                // tree is child node
            printf("%2d(%s) is %2d's %6s child\n", key, rbt_is_black(tree)?"B":"R", key, direction==1?"right" : "left");

        if (tree->rbt_left)
            print_rbttree(tree->rbt_left, rbt_entry(tree->rbt_left, struct my_node, rbt_node)->key, -1);
        if (tree->rbt_right)
            print_rbttree(tree->rbt_right,rbt_entry(tree->rbt_right, struct my_node, rbt_node)->key,  1);
    }
}

static void my_print(struct rbt_root *root)
{
    if (root!=NULL && root->rbt_node!=NULL)
        print_rbttree(root->rbt_node, rbt_entry(root->rbt_node, struct my_node, rbt_node)->key,  0);
}


static void test_rbtree_case(void)
{
    int i;
    int ilen;
    int a[] = {10, 40, 30, 60, 90, 70, 20, 50, 80};
    int b[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    struct rbt_node *tmp;
    struct my_node  *mynode;
    struct rbt_root  mytree = RBT_ROOT;

    ilen = LENGTH(a);

    printf("== origin value: ");
    for(i=0; i<ilen; i++) {
        printf("%d ", a[i]);
    }
    printf("\r\n");

    YUNIT_ASSERT(1);

    for (i=0; i < ilen; i++)
    {
        my_insert(&mytree, a[i]);
#if CHECK_INSERT
        printf("== intsert node: %d\r\n", a[i]);
        printf("== Details of the tree: \r\n");
        my_print(&mytree);
        printf("r\\n");
#endif
    }

    for (tmp = rbt_first(&mytree), i = 0; tmp; tmp = rbt_next(tmp), i++) {
        mynode = rbt_entry(tmp, struct my_node, rbt_node);
        printf("%d ", mynode->key);
        YUNIT_ASSERT(mynode->key == b[i]);
    }

    printf("\r\n");

    for (i=0; i<ilen; i++) {
        my_delete(&mytree, a[i]);
#if CHECK_DELETE
        printf("== Delete node: %d\r\n", a[i]);
        printf("== Details of the tree: \r\n");
        my_print(&mytree);
        printf("\r\n");
#endif
    }
}

static yunit_test_case_t rbtree_testcases[] = {
    { "rbtree_test", test_rbtree_case},
    YUNIT_TEST_CASE_NULL
};

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static yunit_test_suite_t suites[] = {
    { "rbtree", init, cleanup, setup, teardown, rbtree_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_rbtree(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(test_rbtree);

