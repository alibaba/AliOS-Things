/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <yunit.h>
#include <yts.h>
#include <aos/types.h>
#include <k_api.h>

#define MSG_SIZE 50


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

static void test_osal_case(void)
{
    printf("osal test\n");
}

static yunit_test_case_t aos_osal_testcases[] = {
    { "osal", test_osal_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "osal", init, cleanup, setup, teardown, aos_osal_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_osal(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_osal);
