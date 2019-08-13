/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <aos/kernel.h>

#include "yunit/yunit.h"
#include <k_api.h>
#include "test_fw.h"


#define RHINO_TEST_TASK_PRI 9

extern void test_rhino_port(void);

static int init(void)
{
    test_case_init();
    return 0;
}

static int cleanup(void)
{
    test_case_cleanup();
    return 0;
}

static void setup(void)
{
    return;
}

static void teardown(void)
{
    return;
}

static int item = 0;

void rhino_ytest_fn(void)
{
    YUNIT_ASSERT(test_case_fail ==  0);
}

void test_rhino(void)
{
    uint8_t old_pri;
    void *suite;

    krhino_task_pri_change(krhino_cur_task_get(), RHINO_TEST_TASK_PRI, &old_pri);

    suite = yunit_add_test_suite("rhino", init, cleanup, setup, teardown);

    for (item = 0;; item++) {
        if ((test_fw_map[item].name == NULL) || (test_fw_map[item].fn == NULL)) {
            break;
        }

        yunit_add_test_case(suite, test_fw_map[item].name, test_fw_map[item].fn);
    }

    yunit_add_test_case(suite, "rhino test stats", rhino_ytest_fn);

#ifdef YTS_LINUX
    test_rhino_port();
#endif
}
AOS_TESTCASE(test_rhino);
