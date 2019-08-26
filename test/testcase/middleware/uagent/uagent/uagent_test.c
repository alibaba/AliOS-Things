/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include "aos/kernel.h"

#include "uagent/uagent.h"
#include "aos/errno.h"

#define PRODUCT_KEY             "a1MZxOdcBnO"
#define DEVICE_NAME             "test_01"
#define DEVICE_SECRET           "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56"

static void test_uagent_register_normal(void)
{
    const int ret = uagent_register(UAGENT_MOD_ULOG, "ulog",
            "", UAGENT_FUNC_USER_BASE, "dummy",
            NULL, NULL);
    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_register_error_input(void)
{
    const int ret = uagent_register(UAGENT_MOD_ULOG, NULL,
            NULL, UAGENT_FUNC_USER_BASE, NULL,
            NULL, NULL);
    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_unregister_normal(void)
{
    const int ret = uagent_unregister(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE);

    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_request_service_normal(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("ulog level=5")+1, "ulog level=5");

    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_request_service_error_input_1(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, 12, NULL);

    YUNIT_ASSERT(-EINVAL == ret);
}

static void test_uagent_request_service_error_input_2(void)
{
    const int ret = uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, 65535, "ulog level=5");

    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_send_normal(void)
{
    const int ret = uagent_send(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("helloworld")+1, "helloworld", 0);
    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_send_error_input(void)
{
    const int ret = uagent_send(UAGENT_MOD_ULOG, UAGENT_FUNC_USER_BASE, strlen("helloworld")+1, NULL, 0);
    YUNIT_ASSERT(-EINVAL == ret);
}

static void test_uagent_ext_comm_start_normal(void)
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    const int ret = uagent_ext_comm_start(PRODUCT_KEY, DEVICE_NAME);
    YUNIT_ASSERT(0 == ret);
}

static void test_uagent_ext_comm_start_error_input(void)
{
    const int ret = uagent_ext_comm_start(NULL, NULL);
    YUNIT_ASSERT(-EINVAL == ret);
}

static int init(void)
{
    int ret = 0;

    ret = uagent_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static int cleanup(void)
{
    int ret = uagent_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_uagent_testcases[] = {
    { "uagent_register",             test_uagent_register_normal },
    { "uagent_register",             test_uagent_register_error_input },
    { "test_uagent_unregister",      test_uagent_unregister_normal },
    { "test_uagent_request_service", test_uagent_request_service_normal },
    { "test_uagent_request_service", test_uagent_request_service_error_input_1 },
    { "test_uagent_request_service", test_uagent_request_service_error_input_2 },

    { "uagent_ext_comm_start",       test_uagent_ext_comm_start_normal },
    { "uagent_ext_comm_start",       test_uagent_ext_comm_start_error_input },
    { "test_uagent_send",            test_uagent_send_normal   },
    { "test_uagent_send",            test_uagent_send_error_input   },

    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "uagent", init, cleanup, setup, teardown, aos_uagent_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_uagent(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_uagent);
