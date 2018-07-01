/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/types.h>
#include <yunit.h>
#include <yts.h>
#include "alink_export.h"

static void callback_cloud_disconnected(void)
{
    printf("alink wsf server disconnected.\n");
}

static void callback_cloud_connected(void)
{
    int ret = 0;

    printf("alink wsf server connected.\n");

    ret = alink_end();
    YUNIT_ASSERT(0 == ret); 
}

static void test_alink_sandbox(void)
{
    int ret = 0;

    ret = alink_enable_sandbox_mode();
    YUNIT_ASSERT(0 == ret); 
    
    alink_register_callback(ALINK_CLOUD_CONNECTED,callback_cloud_connected);
    alink_register_callback(ALINK_CLOUD_DISCONNECTED,callback_cloud_disconnected);

    ret = alink_start();
    YUNIT_ASSERT(0 == ret); 
}

static void test_alink_daily(void)
{
    int ret = 0;

    ret = alink_enable_daily_mode(NULL,0);
    YUNIT_ASSERT(0 == ret); 
    
    ret = alink_start();
    YUNIT_ASSERT(0 == ret); 
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

static yunit_test_case_t aos_alink_testcases[] = {
    { "daily", test_alink_daily},
    { "sandbox",test_alink_sandbox},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "alink", init, cleanup, setup, teardown, aos_alink_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_alink(void)
{    
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_alink);
