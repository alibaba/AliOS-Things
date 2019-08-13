/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"

#include "yunit/yunit.h"

static void CASE_aosapi_kernel_sys_version()
{
	const char *version = aos_version_get();
	YUNIT_ASSERT(strcmp(version, aos_version_get())==0);
}

static void CASE_aosapi_kernel_sys_reboot()
{
	YUNIT_ASSERT(1);
}


void aosapi_kernel_sys_test_entry(yunit_test_suite_t *suite)
{
	yunit_add_test_case(suite, "kernel.sys.reboot", CASE_aosapi_kernel_sys_reboot);
	yunit_add_test_case(suite, "kernel.sys.version", CASE_aosapi_kernel_sys_version);
}

