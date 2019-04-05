/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#include <stdint.h>
#include <signal.h>
#include "k_compiler.h"


/* Be very careful here, you can modyfy the following code, if only you understand what yor are doing! */

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE
#define RHINO_MM_FREE_DYE            0xABABABAB
#define RHINO_INLINE                 static __inline /* inline keyword, it may change under different compilers */

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;
/* you may change here depend on your hardware timer */
typedef uint32_t hr_timer_t;       /* 32 bit or 64 bit unsigned value */
typedef uint32_t lr_timer_t;       /* 32 bit or 64 bit unsigned value */
typedef uint32_t mutex_nested_t;   /* 8 bit or 16bit or 32bit unsigned value */
typedef uint8_t  suspend_nested_t; /* 8 bit normally */
typedef uint64_t ctx_switch_t;     /* 32 bit or 64 bit unsigned value */
typedef sigset_t cpu_cpsr_t;

#endif /* K_TYPES_H */

