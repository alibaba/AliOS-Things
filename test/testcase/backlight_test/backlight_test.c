/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <yunit/yunit.h>
extern void backlight_test_init(void);
extern void backlight_test_onoff(void);
extern void backlight_test_silent_reset(void);
extern void backlight_test_get_status(void);

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

static void setup_pri_save(void)
{
}

static void teardown_pri_restore(void)
{
}

static yunit_test_case_t aos_backlight_testcases[] = {
    {"backlight_test_init", backlight_test_init},
    {"backlight_test_onoff", backlight_test_onoff},
    {"backlight_test_silent_reset", backlight_test_silent_reset},
    {"backlight_test_get_status", backlight_test_get_status},
    YUNIT_TEST_CASE_NULL};

static yunit_test_suite_t suites[] = {
    {"backlight", init, cleanup, setup, teardown, aos_backlight_testcases},
    YUNIT_TEST_SUITE_NULL};

void aos_backlight_test(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(aos_backlight_test);
