/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE
#define RHINO_MM_FREE_DYE            0xABABABAB

/* inline keyword, it may change under different compilers */
#if defined(__CC_ARM)
#define RHINO_INLINE                 static __inline
#elif defined(__GNUC__)
#define RHINO_INLINE                 static inline
#else
#error "Unknown Compiler"
#endif

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;
typedef uint64_t hr_timer_t;
typedef uint64_t lr_timer_t;
typedef uint32_t mutex_nested_t;
typedef uint8_t  suspend_nested_t;
typedef uint64_t ctx_switch_t;
typedef uint32_t cpu_cpsr_t;

#endif /* TYPES_H */

