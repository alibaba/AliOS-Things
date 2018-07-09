/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TYPES_H
#define TYPES_H
#include <inttypes.h>
#ifndef FILE
#include <stdio.h>
#endif

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE
#define RHINO_MM_FREE_DYE            0xABABABAB
#define RHINO_INLINE                 static __inline /* inline keyword, it may change under different compilers */

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;
typedef uint32_t hr_timer_t;
typedef uint32_t lr_timer_t;
typedef uint32_t mutex_nested_t;
typedef uint8_t  suspend_nested_t;
typedef uint64_t ctx_switch_t;
typedef uint8_t  cpu_cpsr_t;

                                                        /* ---------------------- CPU WORD SIZE ----------------------- */
#define  CPU_WORD_SIZE_08                          1u   /*  8-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_16                          2u   /* 16-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_32                          4u   /* 32-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_64                          8u   /* 64-bit word size (in octets).                                */


#define CPU_CFG_DATA_SIZE  CPU_WORD_SIZE_32    /*sizeof(uint32_t)*/

#define CPU_CFG_DATA_SIZE_MAX  CPU_WORD_SIZE_64  /*sizeof(uint64_t)*/
#endif /* TYPES_H */

