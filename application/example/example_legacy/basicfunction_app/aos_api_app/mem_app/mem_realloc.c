/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case demonstrates that the dynamically allocated memory is out of use
 * and aos_realloc is called to expand the memory.
 */

/* module name used by ulog */
#define MODULE_NAME "mem_alloc"

/* This structure is used to show how to allocate memory */
struct message {
    uint8_t type;
    uint16_t length;
    uint8_t data[10];
};

void mem_realloc(void)
{
    struct message *old_ptr = NULL, *new_ptr = NULL;

    /* allocate four message structures using aos_malloc */
    old_ptr = (struct message *)aos_malloc(sizeof(struct message) * 4);
    /**
     * after allocating memory, you need to determine whether the return value is NULL.
     * NULL means the allocation failed.
     */
    if (old_ptr == NULL) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
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
        old_ptr= NULL;
        LOGE(MODULE_NAME, "realloc task1 error");
        return;
    }


    /**
     * When aos_realloc executes successfully, the contents of old_ptr are automatically copied
     * to new_ptr, and old_ptr is freed, after which the user can use new_ptr's new memory.
     */


    /* free memory when memory is used up */
    aos_free(new_ptr);
    new_ptr = NULL;
}

