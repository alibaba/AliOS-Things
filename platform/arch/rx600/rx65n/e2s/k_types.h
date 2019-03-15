/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#include "k_compiler.h"

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE
#define RHINO_MM_FREE_DYE            0xABABABAB

typedef uint32_t cpu_stack_t;
typedef uint32_t hr_timer_t;
typedef uint32_t lr_timer_t;
typedef uint8_t  cpu_cpsr_t;

/* ---------------------- CPU WORD SIZE ----------------------- */
#define CPU_WORD_SIZE_08             1u   /*  8-bit word size (in octets). */
#define CPU_WORD_SIZE_16             2u   /* 16-bit word size (in octets). */
#define CPU_WORD_SIZE_32             4u   /* 32-bit word size (in octets). */
#define CPU_WORD_SIZE_64             8u   /* 64-bit word size (in octets). */

#define CPU_CFG_DATA_SIZE            CPU_WORD_SIZE_32  /*sizeof(uint32_t)*/
#define CPU_CFG_DATA_SIZE_MAX        CPU_WORD_SIZE_64  /*sizeof(uint64_t)*/

#endif /* TYPES_H */

