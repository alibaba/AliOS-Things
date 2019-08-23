/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef K_PROC_H
#define K_PROC_H

#include "k_compiler.h"
#include "k_phymm.h"
#include "k_asid.h"
#include "preamble.h"

#define ARM_NUM_L1_PGTABLE_ENTRIES 4096

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

/**
 * @brief init process manager
 *
 * @param[in] phyaddr   The physical memory pool start address.
 *                      physical memory pool is memory region reserved
 *                      for process manager who allocates physial memory
 *                      for process.
 * @param[in] size      Physical memory pool size
 *
 * @note phyaddr and size should be aligne to K_MMU_PAGE_ALIGN.
 *
 * @return On success, return 0, else return negative value.
 */
int32_t k_proc_init(void *phyaddr, size_t size);

int32_t k_proc_load(const char *file, const char *name);

int32_t k_proc_ramload(const void *ramaddr, const char *name);

int32_t k_proc_unload(int32_t pid);

/**
 * @brief switch to new process if task p_new and p_old belongs to
 *        different process, else do nothing.
 *
 * @param[in] p_new     new task pointer
 * @param[in] p_old     old task pointer
 */
void k_proc_switch(void *p_new, void *p_old);

void k_proc_show(void);

kproc_app_t *k_proc_app_get(int32_t pid);

int k_proc_addr_check(int32_t pid, void *addr, size_t size);

__WEAK void k_proc_create_pre_hook(kproc_app_t *proc);
__WEAK void k_proc_create_post_hook(kproc_app_t *proc);

#endif /* K_PROC_H */

