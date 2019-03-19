/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef K_PROC_H
#define K_PROC_H

#include "k_phymm.h"
#include "preamble.h"

#define K_CONFIG_RAM_LOAD           0

#define ARM_NUM_L1_PGTABLE_ENTRIES 4096

#define ARM_MAX_ASIDS 16

/* segment type */
typedef enum {
    /* text, rodata... */
    SEG_EXE,
    /* data, bss... */
    SEG_DATA,
    /* for all type */
    SEG_EXE_DATA
} seg_type_t;

/* physical memory pool */
typedef struct
{
    uintptr_t paddr_start;
    size_t    len;
    kphymm_info_t info;
} kproc_pmm_t;

/* proc infomation */
typedef struct
{
    int32_t     pid;  /* same as pid */
    const char *name;
    void       *entry;
    preamble_t  preamble;
} kproc_app_t;

/* load infomation */
typedef struct
{
    kproc_pmm_t phymem;
    kproc_app_t procs[ARM_MAX_ASIDS];
} kproc_info_t;

int32_t k_proc_init(void);
int32_t k_proc_load(const char *path, const char *name);
int32_t k_proc_unload(int32_t pid);
int32_t k_proc_switch(int32_t pid);
void k_proc_show(void);

kproc_app_t *proc_info_get(int pid);
int proc_address_access(int pid, unsigned long addr, unsigned long size);

#endif /* K_PROC_H */
