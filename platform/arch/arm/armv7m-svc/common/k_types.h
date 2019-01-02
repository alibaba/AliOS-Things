/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_TYPES_H
#define K_TYPES_H

#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* stack overflow magic value */
#define RHINO_MM_CORRUPT_DYE         0xFEFEFEFE
#define RHINO_MM_FREE_DYE            0xABABABAB

#if defined(__CC_ARM)
#define RHINO_INLINE                 static __inline
#define RHINO_RETURN_ADDRESS()       __return_address()
#elif defined(__ICCARM__)
#define RHINO_INLINE                 static inline
#define RHINO_RETURN_ADDRESS()       NULL
#elif defined(__GNUC__)
#define RHINO_INLINE                 static inline
#define RHINO_RETURN_ADDRESS()       __builtin_return_address(0)
#else
#error "Unsupported compiler"
#endif

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;
typedef uint32_t hr_timer_t;
typedef uint32_t lr_timer_t;
typedef uint32_t mutex_nested_t;
typedef uint8_t  suspend_nested_t;
typedef uint64_t ctx_switch_t;
typedef uint32_t cpu_cpsr_t;

#endif /* K_TYPES_H */

