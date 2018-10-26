/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UAPP_H
#define UAPP_H

#include <stdbool.h>
#include <k_api.h>

#define MAX_APP_BINS   (3)

#define APP_INFO_MAGIC (0xA7FC429E)

typedef struct uapp_info {
     unsigned int magic;
     unsigned int bin_type;
     void (*app_entry)(int argc, char *argv[]);
     unsigned int text_flash_begin;
     unsigned int text_flash_end;
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
     unsigned int app_task_struct;
     unsigned int app_stack;
     unsigned int app_stack_size;
     unsigned int priority;
     unsigned int reserve[4];
} uapp_info_t;

#endif /* UAPP_H */

