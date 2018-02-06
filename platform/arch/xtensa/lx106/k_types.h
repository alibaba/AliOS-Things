/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TYPES_H
#define TYPES_H

#define RHINO_NO_WAIT                0u
#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_FRAG_ALLOCATED      0xabcddcabu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_FRAG_FREE           0xfefdecdbu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_INLINE                 static __inline /* inline keyword, it may change under different compilers */
#define RHINO_MM_FREE_DYE            0xABABABAB
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;

typedef uint32_t hr_timer_t;
typedef uint32_t lr_timer_t;

typedef uint64_t idle_count_t;
typedef uint32_t mutex_nested_t;
typedef uint8_t  suspend_nested_t;

typedef uint64_t ctx_switch_t;
typedef uint32_t cpu_cpsr_t;

#endif /* TYPES_H */

