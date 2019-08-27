/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "yunit/yunit.h"
#include "yunit/yts.h"

#include "aos/kernel.h"

#include "ulocation/ulocation.h"

extern int32_t ulocation_ggainfo_parse(uint8_t *gga_data, ulocation_gga_info_t *location);
extern int32_t ulocation_qxwz_getgga(uint8_t *gga_raw_data);

static unsigned char gga[100] = "nmea:$GNGGA,063823.00,3120.497601,N,12129.853177,E,4,25,0.6,66.110,M,11.034,M,1.0,*77\r\n";
static ulocation_gga_info_t location;

#define ULOCATION_QXWZ_APPKEY          "770302"
#define ULOCATION_QXWZ_APPSECRET       "99094b805092d2e7e08f63bcde97a2a7a70bded81a1e0b0f6728dede7fc01902"
#define ULOCATION_QXWZ_DEVICE_ID       "AliOS-Things2.1.2"
#define ULOCATION_QXWZ_DEVICE_TYPE     "uLocation_qianxun"

static ulocation_qxwz_usr_config_t config = {
    ULOCATION_QXWZ_APPKEY,     /* appKey */
    ULOCATION_QXWZ_APPSECRET,  /* appSecret */
    ULOCATION_QXWZ_DEVICE_ID,  /* device_ID */
    ULOCATION_QXWZ_DEVICE_TYPE /* device_Type */
};

static void test_ulocation_qianxun_service_normal(void)
{
    const int ret = ulocation_qianxun_service(&config, &location);
    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_qianxun_service_error_input(void)
{
    const int ret = ulocation_qianxun_service(NULL, NULL);
    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_qxwz_getgga_normal(void)
{
    const int ret = ulocation_qxwz_getgga(gga);

    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_qxwz_getgga_error_input_1(void)
{
    const int ret = ulocation_qxwz_getgga(NULL);

    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_ggainfo_parse_normal(void)
{
    const int ret = ulocation_ggainfo_parse(gga, &location);

    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_ggainfo_parse_error_input_1(void)
{
    const int ret = ulocation_ggainfo_parse(NULL, &location);

    YUNIT_ASSERT(0 == ret);
}

static void test_ulocation_ggainfo_parse_error_input_2(void)
{
    const int ret = ulocation_ggainfo_parse(gga, NULL);

    YUNIT_ASSERT(0 == ret);
}

static int init(void)
{
    int ret = 0;

    ret = ulocation_qxwz_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static int cleanup(void)
{
    int ret = 0;

    ret = ulocation_qxwz_init();
    YUNIT_ASSERT(ret == 0);
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_ulocation_testcases[] = {
    { "ulocation_qianxun_service", test_ulocation_qianxun_service_normal },
    { "ulocation_qianxun_service", test_ulocation_qianxun_service_error_input },
    { "ulocation_qxwz_getgga", test_ulocation_qxwz_getgga_normal},
    { "ulocation_qxwz_getgga", test_ulocation_qxwz_getgga_error_input_1},
    { "ulocation_ggainfo_parse", test_ulocation_ggainfo_parse_normal},
    { "ulocation_ggainfo_parse", test_ulocation_ggainfo_parse_error_input_1},
    { "ulocation_ggainfo_parse", test_ulocation_ggainfo_parse_error_input_2},

    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "ulocation", init, cleanup, setup, teardown, aos_ulocation_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_ulocation(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_ulocation);

