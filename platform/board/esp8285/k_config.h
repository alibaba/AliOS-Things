/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CONFIG_H
#define CONFIG_H

/* chip level conf */
#ifndef RHINO_CONFIG_CPU_STACK_DOWN
#define RHINO_CONFIG_CPU_STACK_DOWN          1
#endif

/* kernel feature conf */
#ifndef RHINO_CONFIG_SEM
#define RHINO_CONFIG_SEM                     1
#endif
#ifndef RHINO_CONFIG_QUEUE
#define RHINO_CONFIG_QUEUE                   1
#endif
#ifndef RHINO_CONFIG_TASK_SEM
#define RHINO_CONFIG_TASK_SEM                1
#endif
#ifndef RHINO_CONFIG_EVENT_FLAG
#define RHINO_CONFIG_EVENT_FLAG              1
#endif
#ifndef RHINO_CONFIG_TIMER
#define RHINO_CONFIG_TIMER                   1
#endif
#ifndef RHINO_CONFIG_BUF_QUEUE
#define RHINO_CONFIG_BUF_QUEUE               1
#endif
#ifndef RHINO_CONFIG_MM_BLK
#define RHINO_CONFIG_MM_BLK                  1
#endif
#ifndef RHINO_CONFIG_MM_BLK_SIZE
#define RHINO_CONFIG_MM_BLK_SIZE             16
#endif
#ifndef RHINO_CONFIG_MM_TLF
#define RHINO_CONFIG_MM_TLF                  1
#endif
#ifndef RHINO_CONFIG_MM_MINISIZEBIT
#define RHINO_CONFIG_MM_MINISIZEBIT          5
#endif
#ifndef RHINO_CONFIG_MM_TLF_BLK_SIZE
#define RHINO_CONFIG_MM_TLF_BLK_SIZE         3072
#endif
#ifndef RHINO_CONFIG_MM_DEBUG
#define RHINO_CONFIG_MM_DEBUG                0
#endif
#define K_MM_STATISTIC                       1

/* kernel task conf */
#ifndef RHINO_CONFIG_TASK_INFO
#define RHINO_CONFIG_TASK_INFO               1
#endif

#ifndef RHINO_CONFIG_TASK_INFO_NUM
#define RHINO_CONFIG_TASK_INFO_NUM           5
#endif

#ifndef RHINO_CONFIG_TASK_DEL
#define RHINO_CONFIG_TASK_DEL                1
#endif
#ifndef RHINO_CONFIG_TASK_WAIT_ABORT
#define RHINO_CONFIG_TASK_WAIT_ABORT         1
#endif
#ifndef RHINO_CONFIG_TASK_STACK_OVF_CHECK
#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    1
#endif
#ifndef RHINO_CONFIG_SCHED_RR
#define RHINO_CONFIG_SCHED_RR                1
#endif
#ifndef RHINO_CONFIG_TIME_SLICE_DEFAULT
#define RHINO_CONFIG_TIME_SLICE_DEFAULT      50
#endif
#ifndef RHINO_CONFIG_PRI_MAX
#define RHINO_CONFIG_PRI_MAX                 62
#endif
#ifndef RHINO_CONFIG_USER_PRI_MAX
#define RHINO_CONFIG_USER_PRI_MAX            (RHINO_CONFIG_PRI_MAX - 2)
#endif

/* kernel mm_region conf */
#ifndef RHINO_CONFIG_MM_REGION_MUTEX
#define RHINO_CONFIG_MM_REGION_MUTEX         0
#endif

/* kernel timer&tick conf */
#ifndef RHINO_CONFIG_HW_COUNT
#define RHINO_CONFIG_HW_COUNT                0
#endif

#ifndef RHINO_CONFIG_TICKS_PER_SECOND
#define RHINO_CONFIG_TICKS_PER_SECOND        100
#endif
#ifndef RHINO_CONFIG_TIMER_TASK_STACK_SIZE
#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   128
#endif
#ifndef RHINO_CONFIG_TIMER_TASK_PRI
#define RHINO_CONFIG_TIMER_TASK_PRI          5
#endif

/* kernel intrpt conf */
#ifndef RHINO_CONFIG_INTRPT_STACK_OVF_CHECK
#define RHINO_CONFIG_INTRPT_STACK_OVF_CHECK  0
#endif

/* kernel dyn alloc conf */
#ifndef RHINO_CONFIG_KOBJ_DYN_ALLOC
#define RHINO_CONFIG_KOBJ_DYN_ALLOC          1
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#ifndef RHINO_CONFIG_K_DYN_TASK_STACK
#define RHINO_CONFIG_K_DYN_TASK_STACK        128
#endif
#ifndef RHINO_CONFIG_K_DYN_MEM_TASK_PRI
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      9
#endif
#endif

/* kernel idle conf */
#ifndef RHINO_CONFIG_IDLE_TASK_STACK_SIZE
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    256
#endif

/* kernel hook conf */
#ifndef RHINO_CONFIG_USER_HOOK
#define RHINO_CONFIG_USER_HOOK               1
#endif

/* kernel stats conf */
#ifndef RHINO_CONFIG_SYSTEM_STATS
#define RHINO_CONFIG_SYSTEM_STATS            1
#endif
#ifndef RHINO_CONFIG_DISABLE_SCHED_STATS
#define RHINO_CONFIG_DISABLE_SCHED_STATS     0
#endif
#ifndef RHINO_CONFIG_DISABLE_INTRPT_STATS
#define RHINO_CONFIG_DISABLE_INTRPT_STATS    0
#endif

#ifndef RHINO_CONFIG_TASK_SCHED_STATS
#define RHINO_CONFIG_TASK_SCHED_STATS        0
#endif
#ifndef RHINO_CONFIG_TASK_STACK_CUR_CHECK
#define RHINO_CONFIG_TASK_STACK_CUR_CHECK    1
#endif

#ifndef RHINO_CONFIG_TIMER_MSG_NUM
#define RHINO_CONFIG_TIMER_MSG_NUM           10
#endif

/* kernel workqueue conf */
#ifndef RHINO_CONFIG_WORKQUEUE
#define RHINO_CONFIG_WORKQUEUE               0
#endif
#ifndef RHINO_CONFIG_WORKQUEUE_STACK_SIZE
#define RHINO_CONFIG_WORKQUEUE_STACK_SIZE    512
#endif
#ifndef RHINO_CONFIG_WORKQUEUE_TASK_PRIO
#define RHINO_CONFIG_WORKQUEUE_TASK_PRIO     20
#endif

#ifndef RHINO_CONFIG_CPU_NUM
#define RHINO_CONFIG_CPU_NUM                 1
#endif

#endif /* CONFIG_H */

