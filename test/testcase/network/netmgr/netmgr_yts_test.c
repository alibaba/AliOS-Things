/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <netmgr.h>

#define TAG "netmgr_yts"

#define SSID_KEY             "ssid"
#define PWD_KEY              "pwd"
#define MAX_SSID_LEN          50
#define MAX_PWD_LEN           50
#define CONNECT_TIMEOUT_MS    10000

static char test_ssid[MAX_SSID_LEN] = { 0 };
static char test_pwd[MAX_PWD_LEN]   = { 0 };

static void test_netmgr_connect(void)
{
    int ret_i, len;
    netmgr_stats_t stats;

    ret_i = netmgr_init();
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = netmgr_start(false);
    YUNIT_ASSERT( 0 >= ret_i );

    ret_i = aos_kv_get(SSID_KEY, test_ssid, &len);
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = aos_kv_get(PWD_KEY, test_pwd, &len);
    YUNIT_ASSERT( 0 == ret_i );

    ret_i = netmgr_connect(test_ssid, test_pwd, CONNECT_TIMEOUT_MS);
    YUNIT_ASSERT( 0 == ret_i );

    netmgr_stats(0, &stats);
    YUNIT_ASSERT( true == stats.ip_available );
    if (stats.ip_available)
        LOGI(TAG, "netmgr test got ip: %s", stats.ip);
}

static int init(void)
{
    aos_set_log_level(AOS_LL_INFO);
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
    { "basic", test_netmgr_connect },
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
