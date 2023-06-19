/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>

#include "yunit/yunit.h"


//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>

//#include <aos/kernel.h>
//
/**
 * The use case allocates the same memory in three different ways.
 */

/* module name used by ulog */
#define MODULE_NAME "aos_mem_test"

/* four of these types of structures are allocated in the use case */
struct message {
    uint8_t type;
    uint16_t length;
    uint8_t data[10];
};

int mem_alloc(void)
{
    struct message *messages_ptr = NULL;

    /* way-1 */
    /* allocate four message structures using aos_malloc */
    messages_ptr = (struct message *)aos_malloc(sizeof(struct message) * 4);
    /**
     * after allocating memory, you need to determine whether the return value is NULL.
     * NULL means the allocation failed.
     */
    if (messages_ptr == NULL) {
        printf("[%s]aos_malloc error\n", MODULE_NAME);
        return -1;
    }

    printf("[%s]aos_malloc success!\n", MODULE_NAME);

    /* all newly allocated memory areas are set to 0 */
    memset(messages_ptr, 0, sizeof(struct message) * 4);

    /* free memory when memory is used up */
    aos_free(messages_ptr);
    messages_ptr = NULL;


    /* way-2 */
    /* allocate four message structures using aos_zalloc. This function clears memory automatically. */
    messages_ptr = (struct message *)aos_zalloc(sizeof(struct message) * 4);
    /**
     * after allocating memory, you need to determine whether the return value is NULL.
     * NULL means the allocation failed.
     */
    if (messages_ptr == NULL) {
        printf("[%s]aos_zalloc error\n", MODULE_NAME);
        return -1;
    }

    printf("[%s]aos_zalloc success!\n", MODULE_NAME);

    /* free memory when memory is used up */
    aos_free(messages_ptr);
    messages_ptr = NULL;


    /* way-3 */
    /* allocate four message structures using aos_calloc. This function clears memory automatically.*/
    messages_ptr = (struct message *)aos_calloc(4, sizeof(struct message));
    /**
     * after allocating memory, you need to determine whether the return value is NULL.
     * NULL means the allocation failed.
     */
    if (messages_ptr == NULL) {
        printf("[%s]aos_calloc error\n", MODULE_NAME);
        return -1;
    }

    printf("[%s]aos_calloc success!\n", MODULE_NAME);

    /* free memory when memory is used up */
    aos_free(messages_ptr);
    messages_ptr = NULL;

    return 0;
}

/**
 * This use case demonstrates that the dynamically allocated memory is out of use
 * and aos_realloc is called to expand the memory.
 */

int mem_realloc(void)
{
    struct message *old_ptr = NULL, *new_ptr = NULL;

    /* allocate four message structures using aos_malloc */
    old_ptr = (struct message *)aos_malloc(sizeof(struct message) * 4);
    /**
     * after allocating memory, you need to determine whether the return value is NULL.
     * NULL means the allocation failed.
     */
    if (old_ptr == NULL) {
        printf("[%s]aos_malloc error\n", MODULE_NAME);
        return -1;
    }
    /* all newly allocated memory areas are set to 0 */
    memset(old_ptr, 0, sizeof(struct message) * 4);

    /**
     * in the process of use, it was found that 4 structures were not enough,
     * so we need to expand to 6
     */
    new_ptr = aos_realloc(old_ptr, sizeof(struct message) * 6);
    if (new_ptr == NULL) {
        /* The memory pointed to by old_ptr is not automatically freed when aos_realloc fails */
        aos_free(old_ptr);
        old_ptr = NULL;
        printf("[%s]aos_realloc task1 error\n", MODULE_NAME);
        return -1;
    }

    printf("[%s]aos_realloc success!\n", MODULE_NAME);
    /**
     * When aos_realloc executes successfully, the contents of old_ptr are automatically copied
     * to new_ptr, and old_ptr is freed, after which the user can use new_ptr's new memory.
     */


    /* free memory when memory is used up */
    aos_free(new_ptr);
    new_ptr = NULL;
    return 0;
}

void kernel_test_mem_1(void)
{
    YUNIT_ASSERT(mem_alloc() == 0);
}

void kernel_test_mem_2(void)
{
    YUNIT_ASSERT(mem_realloc() == 0);
}

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
	yunit_add_test_case(suite, "kernel.mm.test1", kernel_test_mem_1);
	yunit_add_test_case(suite, "kernel.mm.test2", kernel_test_mem_2);
}

