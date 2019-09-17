/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <network/network.h>

static void test_lwip_socket(void)
{
    int fd = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    YUNIT_ASSERT(fd >= 0);
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

static yunit_test_case_t aos_lwip_testcases[] = {
    { "socket", test_lwip_socket },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "lwip", init, cleanup, setup, teardown, aos_lwip_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_lwip(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_lwip);
