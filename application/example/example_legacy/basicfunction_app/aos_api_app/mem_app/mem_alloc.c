/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * The use case allocates the same memory in three different ways.
 */

/* module name used by ulog */
#define MODULE_NAME "mem_alloc"

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
        LOGE(MODULE_NAME, "aos_malloc error");
        return;
    }
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
        LOGE(MODULE_NAME, "aos_zalloc error");
        return;
    }

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
        LOGE(MODULE_NAME, "aos_calloc error");
        return;
    }

    /* free memory when memory is used up */
    aos_free(messages_ptr);
    messages_ptr = NULL;
}

