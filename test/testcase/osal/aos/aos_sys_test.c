/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "aos/kernel.h"

#include "yunit/yunit.h"

extern const char *aos_version_get(void);
extern int aos_printf(const char *fmt, ...);
extern int aos_snprintf(char *str, const int len, const char *fmt, ...);
extern int aos_vsnprintf(char *str, const int len, const char *format, va_list ap);
static void CASE_aosapi_kernel_sys_version()
{
	const char *version = aos_version_get();
	YUNIT_ASSERT(strcmp(version, aos_version_get())==0);
}

static void CASE_aosapi_kernel_sys_reboot()
{
	YUNIT_ASSERT(1);
}

static void test_vsnprintf(const char *fmt, ...)
{
    char str[100];
    va_list args;
    va_start(args, fmt);
    aos_vsnprintf(str, 10, fmt, args);
    va_end(args);

    // 输出字符串
    aos_printf("%s", str);
}
static void CASE_aosapi_aos_etc()
{
    int32_t r_value1, r_value2, r_value3;
    unsigned int seed = (unsigned int)aos_now();
    char str[100];
    int a = 10;
    float b = 3.14;

    aos_srand(seed);
    r_value1 = aos_rand();
    r_value2 = aos_rand();

    seed = (unsigned int)aos_now_ms();
    aos_srand(seed);
    r_value3 = aos_rand();
	YUNIT_ASSERT(r_value1 != r_value2);
	YUNIT_ASSERT(r_value1 != r_value3);

    // 使用snprintf将格式化字符串设置为最多10个字符
    aos_snprintf(str, 10, "a=%d, b=%.2f", a, b);
    // 输出字符串
    aos_printf("%s", str);
 
    test_vsnprintf("a=%d, b=%.2f", a, b);

    aos_now_time_str(str, 15);
    aos_printf("%s", str);
}

void aosapi_kernel_sys_test_entry(yunit_test_suite_t *suite)
{
	yunit_add_test_case(suite, "kernel.sys.reboot", CASE_aosapi_kernel_sys_reboot);
	yunit_add_test_case(suite, "kernel.sys.version", CASE_aosapi_kernel_sys_version);
	yunit_add_test_case(suite, "kernel.aos_etc", CASE_aosapi_aos_etc);
}

