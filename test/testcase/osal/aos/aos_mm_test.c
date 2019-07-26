/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>

#include "yts/yunit.h"


static void CASE_aosapi_kernel_mm_param()
{
	/* dumpsys_mm_info_func here */
	aos_malloc(0);

	/* coredump here */
#if 0
	aos_free(NULL);
#endif
}

static void CASE_aosapi_kernel_mm_allocfree()
{
	const int COUNT = 1024;
	int *ptr = aos_malloc(sizeof(int)*COUNT);

	memset(ptr, 0, COUNT);
	int i = 0;
	for(; i<COUNT; i++) {
		*(ptr+i) = i;
	}
	i = 0;
	for(; i<COUNT; i++) {
		YUNIT_ASSERT_MSG((int)*(ptr+i)==i, "*(ptr+i)=%d", i);
	}
	aos_free(ptr);
	ptr = NULL;
}

void aosapi_kernel_mm_test_entry(yunit_test_suite_t *suite)
{
	yunit_add_test_case(suite, "kernel.mm.param", CASE_aosapi_kernel_mm_param);
	yunit_add_test_case(suite, "kernel.mm.allocfree", CASE_aosapi_kernel_mm_allocfree);
}

