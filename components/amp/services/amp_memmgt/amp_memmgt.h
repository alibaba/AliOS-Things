/*!
 * @file amp_memmgt.h
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AMP_MEMMGT_H_
#define _AMP_MEMMGT_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

//#define MEMMGT_MEM_SIZE_CHECK
//#define MEMMGT_MEM_DBG_STATIC
//#define MEMMGT_MEM_TRACE
#define MEMMGT_TOTAL_PT_NUM 2

#define MEMMGT_OFFSET 4

typedef void * (*amp_memmgt_malloc_fn)(unsigned int size);
typedef void * (*amp_memmgt_realloc_fn)(void *ptr, unsigned int size);
typedef void (*amp_memmgt_free_fn)(void *ptr);

typedef struct {
    amp_memmgt_malloc_fn malloc_fn;
    amp_memmgt_realloc_fn realloc_fn;
    amp_memmgt_free_fn free_fn;

    int pt_num;
    int trace_pt;
    unsigned int mem_limit[MEMMGT_TOTAL_PT_NUM];
} amp_memmgt_config_t;

extern int amp_memmgt_init(amp_memmgt_config_t *config);

//////////////////////////////////////////////////////////////////////////////

#ifdef __i386__
#define GET_LR(tmp_lr) tmp_lr = 0
#else
#ifdef __GNUC__
#define GET_LR(tmp_lr) __asm__ __volatile__("mov %0, lr \n":"=r"(tmp_lr):)
#else
#define GET_LR(tmp_lr) __asm("MOV tmp_lr, lr\n")
#endif
#endif

extern void *amp_memmgt_malloc(unsigned int size, unsigned int lr, int ptno);

#define AMP_MEMMGT_MALLOC(ptr, size, ptno)               \
do {                                                     \
    unsigned int tmp_lr = 0;                             \
    GET_LR(tmp_lr);                                      \
    ptr = amp_memmgt_malloc(size, tmp_lr, ptno);         \
} while(0)

/////////////////////////////////////////////////////////////////////////////////////////
extern void *amp_memmgt_realloc(void *ptr, unsigned int size, unsigned int lr, int ptno);
#define AMP_MEMMGT_REALLOC(ptr_new, ptr, size, ptno)       \
do {                                                       \
    unsigned int tmp_lr = 0;                               \
    GET_LR(tmp_lr);                                        \
    ptr_new = amp_memmgt_realloc(ptr, size, tmp_lr, ptno); \
} while(0)


/////////////////////////////////////////////////////////////////////////////
extern void amp_memmgt_free(void *ptr, unsigned int lr, int ptno);

#define AMP_MEMMGT_FREE(ptr, ptno)            \
do {                                          \
    unsigned int tmp_lr = 0;                  \
    GET_LR(tmp_lr);                           \
    amp_memmgt_free(ptr, tmp_lr, ptno);       \
} while(0)

#endif
