/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#include "k_compiler.h"

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFE
#define RHINO_MM_FREE_DYE            0xABAB

typedef uint16_t cpu_stack_t;
typedef uint32_t hr_timer_t;
typedef uint32_t lr_timer_t;
typedef uint8_t  cpu_cpsr_t;

#endif /* K_TYPES_H */

