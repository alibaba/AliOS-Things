/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <yunit/yunit.h>
extern void lcd_test_init(void);
extern void lcd_test_onoff(void);
extern void lcd_test_image(void);
extern void lcd_test_finalize(void);

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

static yunit_test_case_t aos_lcd_testcases[] = {
    { "lcd_test_init", lcd_test_init},
    { "lcd_test_onoff", lcd_test_onoff},
    { "lcd_test_image", lcd_test_image},
    { "lcd_test_finalize", lcd_test_finalize},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "lcd", init, cleanup, setup, teardown, aos_lcd_testcases },
    YUNIT_TEST_SUITE_NULL
};

void aos_lcd_test(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(aos_lcd_test);