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
     void (*app_entry)(int pid ,int argc, char *argv[]);
     unsigned int text_flash_begin;
     unsigned int text_flash_end;
     unsigned int data_ram_start;
     unsigned int data_ram_end;
     unsigned int data_flash_begin;
     unsigned int bss_start;
     unsigned int bss_end;
     unsigned int heap_start;
     unsigned int heap_end;
     /* reserve for other */
     unsigned int magic;
     unsigned int reserve1;
     /* this bin_type must be here, 0x28, uniform with single bin & kernel bin,
        arm cortex not used */
     unsigned int bin_type;
     unsigned int reserve2;
     unsigned int reserve3;
     unsigned int reserve4;
     unsigned int reserve5;
     unsigned int reserve6;
} uapp_info_t;

#endif /* UAPP_H */

