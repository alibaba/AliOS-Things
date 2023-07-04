/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <yunit/yunit.h>
extern void pcm_test_init(void);
extern void pcm_test_open(void);
extern void pcm_test_read(void);
extern void pcm_test_write(void);
extern void pcm_test_setup(void);
extern void pcm_test_mute(void);
extern void pcm_test_silence(void);
extern void pcm_test_flush(void);
extern void pcm_test_close(void);

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

static yunit_test_case_t aos_uvoice_testcases[] = {
    { "pcm_test_init",    pcm_test_init},
    { "pcm_test_open",    pcm_test_open},
    { "pcm_test_read",    pcm_test_read},
    { "pcm_test_write",   pcm_test_write},
    { "pcm_test_setup",   pcm_test_setup},
    { "pcm_test_mute",    pcm_test_mute},
    { "pcm_test_silence", pcm_test_silence},
    { "pcm_test_flush",   pcm_test_flush},
    { "pcm_test_close",   pcm_test_close},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "uvoice", init, cleanup, setup, teardown, aos_uvoice_testcases },
    YUNIT_TEST_SUITE_NULL
};

void aos_uvoice_test(void)
{
    yunit_add_test_suites(suites);
}

AOS_TESTCASE(aos_uvoice_test);