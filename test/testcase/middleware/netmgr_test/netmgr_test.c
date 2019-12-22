/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/yloop.h"

#include "network/hal/base.h"
#include <hal/wifi.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include "netmgr.h"

enum {
    SMART_CONFIG_FLAG = 1 << 0,
    SAVED_CONFIG_FLAG = 1 << 1,
    DONE_FLAGS = SMART_CONFIG_FLAG | SAVED_CONFIG_FLAG,
};

static uint8_t done_flag;
static void smart_config_test(void *arg);
static void saved_config_test(void *arg);

static int events_executor(input_event_t *eventinfo, void *cb_para, int flag)
{
    char ips[16];

    if (eventinfo->type != EV_WIFI) {
        return -1;
    }

    switch (eventinfo->code) {
        case CODE_WIFI_ON_GOT_IP:
            done_flag |= flag;
            netmgr_deinit();
            netmgr_wifi_get_ip(ips);
            return 0;
    }
    return -1;
}

static void saved_config_event_executor(input_event_t *eventinfo, void *cb_para)
{
    events_executor(eventinfo, cb_para, SAVED_CONFIG_FLAG);
}

static void smart_config_event_executor(input_event_t *eventinfo, void *cb_para)
{
    if (events_executor(eventinfo, cb_para, SMART_CONFIG_FLAG) == 0) {
        aos_schedule_call(saved_config_test, NULL);
    }
}

static void smart_config_test(void *arg)
{
    int ret;
    aos_unregister_event_filter(EV_WIFI, saved_config_event_executor, NULL);
    aos_register_event_filter(EV_WIFI, smart_config_event_executor, NULL);
    netmgr_init();
    ret = netmgr_start(true);
#ifdef CONFIG_AOS_NETMGRYTS_NOSMARTCONFIG
    /* do not wait for WIFI event if no valid AP. */
    if (ret != 0) done_flag = DONE_FLAGS;
#else
    (void)ret;
#endif
}

static void saved_config_test(void *arg)
{
    int ret;
    aos_unregister_event_filter(EV_WIFI, smart_config_event_executor, NULL);
    aos_register_event_filter(EV_WIFI, saved_config_event_executor, NULL);
    netmgr_init();
    ret = netmgr_start(true);
#ifdef CONFIG_AOS_NETMGRYTS_NOSMARTCONFIG
    /* do not wait for WIFI event if no valid AP. */
    if (ret != 0) done_flag = DONE_FLAGS;
#else
    (void)ret;
#endif
}

static void test_netmgr_cases(void *arg)
{
    aos_schedule_call(smart_config_test, NULL);
}

static void netmgr_test_entry(void *arg)
{
    aos_post_delayed_action(1000, test_netmgr_cases, NULL);
    aos_loop_run();
}

static void netmgr_test_exit(void *arg)
{
    aos_loop_exit();
}

static void test_netmgr_connect_case(void)
{
    aos_task_new("netmgr_test_main", netmgr_test_entry, NULL, 8192);

    /* Set max 60s wait here, since wifi connect needs time to finish. */
    check_cond_wait(done_flag == DONE_FLAGS, 60);
    aos_schedule_call(netmgr_test_exit, NULL);
    aos_unregister_event_filter(EV_WIFI, smart_config_event_executor, NULL);
    aos_unregister_event_filter(EV_WIFI, saved_config_event_executor, NULL);
}

static int init(void)
{
    done_flag = 0;
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

static yunit_test_case_t aos_netmgr_testcases[] = {
    { "netmgr_connect", test_netmgr_connect_case },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "netmgr", init, cleanup, setup, teardown, aos_netmgr_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_netmgr(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_netmgr);
