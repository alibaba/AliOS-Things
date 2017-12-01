/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_INIT_H
#define AOS_INIT_H

#include <stdbool.h>


typedef struct {
    int argc;
    char **argv;

    bool cli_enable;
} kinit_t;

extern int aos_kernel_init(kinit_t *kinit);

#ifdef AOS_BINS
#include <k_api.h>
struct app_info_t {
     void (*app_entry)(void *ksyscall_tbl, void *fsyscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

struct framework_info_t {
     void (*framework_entry)(void *syscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
};

#endif


#endif /* AOS_INIT_H */

