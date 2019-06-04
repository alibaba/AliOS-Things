/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __PREAMBLE_H
#define __PREAMBLE_H

#include "k_api.h"

#define USER_PRIV_INFO_CNT  (4)

#define PREAMBLE_MAGIC 0xfee16666

typedef void (*main_entry_t)(int argc, char *argv[]);

typedef struct {
    /**
     * the magic number of app
     */
    uint32_t magic;

    /**
     * it's used as app process name, as well as the first
     * the first task name
     */
    const char *app_name;

    /**
     * text section
     */
    void *text_start;
    void *text_end;

    /**
     * data section
     */
    void *data_start;
    void *data_end;

    /**
     * zero section
     */
    void *zero_start;
    void *zero_end;

    /**
     * points to the first task's pointer
     */
    ktask_t **task_struct;

    /**
     * the entry point of app, when the first task start to run,
     * it will jump to the entry point.
     */
    main_entry_t main_entry;

    /**
     * the first task's stack pointer
     */
    cpu_stack_t *ustack;

    /**
     * the first task's user space stack size, the unit is cpu_stack_t,
     * rather than byte.
     */
    uint32_t ustack_size;

    /**
     * the first task's kernel space stack size, the unit is cpu_stack_t,
     * rather than byte.
     */
    uint32_t kstack_size;

    /**
     * the first task's task priority
     */
    uint8_t priority;

    /**
     * app version number
     */
    uint32_t version;

    /**
     * member left to user's private utility
     */
    void *user_info[USER_PRIV_INFO_CNT];
} preamble_t;

#endif /* __PREAMBLE_H */

