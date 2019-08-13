/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <aos/kernel.h>

#include "yunit/yunit.h"

#include <errno.h>

static aos_timer_t g_timer;
static aos_sem_t   sync_sem;
static int         timer_trigger_count = 0;

static void TIMER_aosapi_kernel_timer_param() {}
static void CASE_aosapi_kernel_timer_param()
{
    int ret;
#if 0
	ret = aos_timer_new(NULL, TIMER_aosapi_kernel_timer_param, NULL, 1000, 0);
	YUNIT_ASSERT(ret==RHINO_NULL_PTR);
#endif

    ret = aos_timer_new(&g_timer, NULL, NULL, 1000, 0);
    YUNIT_ASSERT(ret != RHINO_SUCCESS);

    ret = aos_timer_new(&g_timer, TIMER_aosapi_kernel_timer_param, NULL, 0, 0);
    YUNIT_ASSERT(ret != RHINO_SUCCESS);

#if 0
	ret = aos_timer_start(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	ret = aos_timer_stop(NULL);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif

#if 0
	aos_timer_free(NULL);
#endif

#if 0
	ret = aos_timer_change(NULL, 0);
	YUNIT_ASSERT(ret == RHINO_NULL_PTR);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void TIMER_aosapi_kernel_timer_norepeat()
{
    timer_trigger_count++;
    aos_timer_stop(&g_timer);
}
static void CASE_aosapi_kernel_timer_norepeat()
{
    int ret;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT(ret == RHINO_SUCCESS);

    timer_trigger_count = 0;
    ret =
      aos_timer_new(&g_timer, TIMER_aosapi_kernel_timer_norepeat, NULL, 100, 0);
    YUNIT_ASSERT(ret == RHINO_SUCCESS);

    aos_msleep(1000);
    YUNIT_ASSERT(timer_trigger_count == 1);
    aos_timer_free(&g_timer);
}

///////////////////////////////////////////////////////////////////////////////////////////////
static void TIMER_aosapi_kernel_timer_repeat()
{
    timer_trigger_count++;
    if (timer_trigger_count == 10) {
        aos_timer_stop(&g_timer);
    }
}
static void CASE_aosapi_kernel_timer_repeat()
{
    int ret;

    ret = aos_sem_new(&sync_sem, 0);
    YUNIT_ASSERT(ret == RHINO_SUCCESS);

    timer_trigger_count = 0;
    ret =
      aos_timer_new(&g_timer, TIMER_aosapi_kernel_timer_repeat, NULL, 100, 1);
    YUNIT_ASSERT(ret == RHINO_SUCCESS);
    aos_msleep(1500);
    YUNIT_ASSERT(timer_trigger_count == 10);
    aos_timer_free(&g_timer);
}

void aosapi_kernel_timer_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "kernel.timer.param",
                        CASE_aosapi_kernel_timer_param);
    yunit_add_test_case(suite, "kernel.timer.norepeat",
                        CASE_aosapi_kernel_timer_norepeat);
    yunit_add_test_case(suite, "kernel.timer.repeat",
                        CASE_aosapi_kernel_timer_repeat);
}
