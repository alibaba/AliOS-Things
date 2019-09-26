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

extern int aos_components_init(kinit_t *kinit);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

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

struct m_app_info_t {
     void (*app_entry)(void *ksyscall_tbl, int argc, char *argv[]);
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
     /* reserve for other */
     unsigned int reserve0;
     unsigned int reserve1;
     /* this bin_type must be here, 0x28, uniform with single bin & kernel bin,
        arm cortex not used */
     unsigned int bin_type;
     unsigned int reserve2;
     unsigned int reserve3;
     unsigned int reserve4;
     unsigned int reserve5;
     unsigned int reserve6;
};

#endif


#endif /* AOS_INIT_H */

