/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <yunit/yunit.h>
extern void button_test_init(void);

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

static yunit_test_case_t aos_button_testcases[] = {
    {"button_test_init", button_test_init}, YUNIT_TEST_CASE_NULL};

static yunit_test_suite_t suites[] = {
    {"button", init, cleanup, setup, teardown, aos_button_testcases},
    YUNIT_TEST_SUITE_NULL};

void aos_button_test(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(aos_button_test);