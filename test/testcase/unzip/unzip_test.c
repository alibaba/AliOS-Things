/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include "yunit/yunit.h"

extern void test_aos_unzip(void);

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

void aos_uz_test(void)
{
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("uzapi", init, cleanup, setup, teardown);

    yunit_add_test_case(suite, "aos_unzip", test_aos_unzip);
}

AOS_TESTCASE(aos_uz_test);
