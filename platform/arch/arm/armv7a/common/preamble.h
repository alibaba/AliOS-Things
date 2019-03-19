/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef PREAMBLE_H
#define PREAMBLE_H

#define MAX_APP_BINS   (3)

#define PREAMBLE_MAGIC 0xfee16666

typedef struct {
    uint32_t     magicword;
    const char  *app_name;
    uintptr_t   *text_start;
    uintptr_t   *text_end;
    uintptr_t   *data_start;
    uintptr_t   *data_end;
    uintptr_t   *zero_start;
    uintptr_t   *zero_end;
    ktask_t    **task_struct;
    void         (*app_entry)(int argc, char *argv[]);
    cpu_stack_t *ustack;
    unsigned int ustack_size;
    unsigned int kstack_size;
    unsigned int priority;
} preamble_t;

#endif /* PREAMBLE */
