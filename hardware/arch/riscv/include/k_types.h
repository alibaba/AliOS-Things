/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#include "k_compiler.h"

#if __riscv_xlen == 64
#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafdeadbeafull     /* stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeadbeafdeadbeafull     /* stack overflow magic value */
#define RHINO_MM_FRAG_ALLOCATED      0xabcddcababcddcabull     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_FRAG_FREE           0xfefdecdbfefdecdbull     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFEFEFEFEFEULL
#define RHINO_MM_FREE_DYE            0xABABABABABABABABULL
#define AOS_ARCH_LP64                1

typedef uint64_t cpu_stack_t;
typedef uint64_t cpu_cpsr_t;

#elif __riscv_xlen == 32
#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeadbeafu     /* stack overflow magic value */
#define RHINO_MM_FRAG_ALLOCATED      0xabcddcabu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_FRAG_FREE           0xfefdecdbu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFEU
#define RHINO_MM_FREE_DYE            0xABABABABU

typedef uint32_t cpu_stack_t;
typedef uint32_t cpu_cpsr_t;

#else
	#error Assembler did not define __riscv_xlen
#endif

typedef uint64_t hr_timer_t;
typedef uint64_t lr_timer_t;

#endif /* K_TYPES_H */

