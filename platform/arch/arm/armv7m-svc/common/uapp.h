/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UAPP_H
#define UAPP_H

#include <stdbool.h>
#include <k_api.h>

#define MAX_APP_BINS   (3)

/* PID 0 is reserved for kernel */
#define PID_TO_APP_ID(pid) (pid-1)

#define APP_INFO_MAGIC (0xFEE16666)

typedef struct uapp_info {
     unsigned int   magic;
     ktask_t      **task;
     void           (*main_entry)(int argc, char *argv[]);
     cpu_stack_t   *ustack;
     unsigned int   ustack_size;
     unsigned int   kstack_size;
     unsigned int   priority;
     unsigned int   text_flash_begin;
     unsigned int   text_flash_end;
     unsigned int   data_ram_start;
     unsigned int   data_ram_end;
     unsigned int   data_flash_begin;
     unsigned int   bss_start;
     unsigned int   bss_end;
     unsigned int   heap_start;
     unsigned int   heap_end;
     unsigned int   reserve[4];
} uapp_info_t;

#endif /* UAPP_H */

