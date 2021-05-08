/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * The use case allocates the same memory in three different ways.
 */

/* module name used by ulog */
#define MODULE_NAME "aos_mem_example"

/* four of these types of structures are allocated in the use case */
struct message {
    uint8_t type;
    uint16_t length;
    uint8_t data[10];
};

void mem_alloc(void)
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
        return;
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
        return;
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
        return;
    }

    printf("[%s]aos_calloc success!\n", MODULE_NAME);

    /* free memory when memory is used up */
    aos_free(messages_ptr);
    messages_ptr = NULL;
}

/**
 * This use case demonstrates that the dynamically allocated memory is out of use
 * and aos_realloc is called to expand the memory.
 */

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
        printf("[%s]aos_malloc error\n", MODULE_NAME);
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
        printf("[%s]aos_realloc task1 error\n", MODULE_NAME);
        return;
    }

    printf("[%s]aos_realloc success!\n", MODULE_NAME);
    /**
     * When aos_realloc executes successfully, the contents of old_ptr are automatically copied
     * to new_ptr, and old_ptr is freed, after which the user can use new_ptr's new memory.
     */


    /* free memory when memory is used up */
    aos_free(new_ptr);
    new_ptr = NULL;
}

static void aos_mem_example(int argc, char **argv)
{
    mem_alloc();

    mem_realloc();
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_mem_example, mem_example, aos mem example)
#endif

