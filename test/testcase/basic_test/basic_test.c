/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "aos/kernel.h"
#include "aos/list.h"

#include "yunit.h"

struct test_node {
    int v;
    slist_t snext;
    dlist_t dnext;
};

static int list_num(slist_t *head)
{
    int i = 0, j = 0;
    slist_t *tmp;
    struct test_node *node;
    slist_for_each_entry(head, node, struct test_node, snext) {
        i ++;
    }

    slist_for_each_entry_safe(head, tmp, node, struct test_node, snext) {
        j ++;
    }

    YUNIT_ASSERT(i == j);
    return i;
}

static void test_slist_case(void)
{
    AOS_SLIST_HEAD(head);
    struct test_node *node;
    int i;

    YUNIT_ASSERT(slist_entry(NULL, struct test_node, snext) == NULL);
    YUNIT_ASSERT(list_num(&head) == 0);
    YUNIT_ASSERT(slist_entry_number(&head) == 0);
    YUNIT_ASSERT(slist_empty(&head));

    for (i=0;i<10;i++) {
        node = malloc(sizeof(*node));
        node->v = i;
        slist_add_tail(&node->snext, &head);
    }
    YUNIT_ASSERT(slist_entry_number(&head) == 10);

    i = 0;
    slist_for_each_entry(&head, node, struct test_node, snext) {
        YUNIT_ASSERT (node->v == i);

        i ++;
    }
    YUNIT_ASSERT(i == 10);

    i = 0;
    slist_t *tmp;
    slist_for_each_entry_safe(&head, tmp, node, struct test_node, snext) {
        YUNIT_ASSERT (node->v == i);

        i ++;
    }
    YUNIT_ASSERT(i == 10);

    slist_for_each_entry_safe(&head, tmp, node, struct test_node, snext) {
        slist_del(&node->snext, &head);
        free(node);
    }
    YUNIT_ASSERT(list_num(&head) == 0);

    for (i=0;i<10;i++) {
        node = malloc(sizeof(*node));
        node->v = i;
        slist_add(&node->snext, &head);
    }

    i = 9;
    slist_for_each_entry(&head, node, struct test_node, snext) {
        YUNIT_ASSERT (node->v == i);

        i --;
    }
    YUNIT_ASSERT(i == -1);

    slist_for_each_entry_safe(&head, tmp, node, struct test_node, snext) {
        slist_del(&node->snext, &head);
        free(node);
    }
    YUNIT_ASSERT(list_num(&head) == 0);
}

static int dlist_num(dlist_t *head)
{
    int n = dlist_entry_number(head);
    struct test_node *node;
    dlist_t *tmp;

    int i = 0;
    dlist_for_each_entry(head, node, struct test_node, dnext) {
        i ++;
    }

    int j = 0;
    dlist_for_each_entry_safe(head, tmp, node, struct test_node, dnext) {
        j ++;
    }

    YUNIT_ASSERT(n == i);
    YUNIT_ASSERT(n == j);
    return n;
}

static void test_dlist_case(void)
{
    AOS_DLIST_HEAD(dhead);
    YUNIT_ASSERT(dlist_num(&dhead) == 0);

    struct test_node *node;
    int i;

    YUNIT_ASSERT(dlist_empty(&dhead));

    for (i=0;i<10;i++) {
        node = malloc(sizeof(*node));
        node->v = i;
        dlist_add_tail(&node->dnext, &dhead);
    }
    YUNIT_ASSERT(dlist_num(&dhead) == 10);

    i = 0;
    dlist_for_each_entry(&dhead, node, struct test_node, dnext) {
        YUNIT_ASSERT (node->v == i);

        i ++;
    }
    YUNIT_ASSERT(i == 10);

    i = 0;
    dlist_t *tmp;
    dlist_for_each_entry_safe(&dhead, tmp, node, struct test_node, dnext) {
        YUNIT_ASSERT (node->v == i);

        i ++;
    }
    YUNIT_ASSERT(i == 10);

    dlist_for_each_entry_safe(&dhead, tmp, node, struct test_node, dnext) {
        dlist_del(&node->dnext);
        free(node);
    }
    YUNIT_ASSERT(dlist_num(&dhead) == 0);
}

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

static yunit_test_case_t aos_basic_testcases[] = {
    { "slist", test_slist_case },
    { "dlist", test_dlist_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "basic", init, cleanup, setup, teardown, aos_basic_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_basic(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_basic);
