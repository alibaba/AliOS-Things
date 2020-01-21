/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CONFIG_H
#define K_CONFIG_H

#define RHINO_CONFIG_SEM                     1

#define RHINO_CONFIG_QUEUE                   1

#define RHINO_CONFIG_TASK_SEM                1

#define RHINO_CONFIG_WORKQUEUE               0

#define RHINO_CONFIG_WORKQUEUE_STACK_SIZE    512

#define RHINO_CONFIG_WORKQUEUE_TASK_PRIO     20

#define RHINO_CONFIG_EVENT_FLAG              0

#define RHINO_CONFIG_TIMER                   0

#define RHINO_CONFIG_BUF_QUEUE               0

#define RHINO_CONFIG_MM_BLK                  1

#define RHINO_CONFIG_MM_BLK_SIZE             32

#define RHINO_CONFIG_MM_TLF                  1

#define RHINO_CONFIG_MM_MAXMSIZEBIT          20

#define RHINO_CONFIG_MM_TLF_BLK_SIZE         512

#define RHINO_CONFIG_MM_DEBUG                0

/* kernel task conf */
#define RHINO_CONFIG_TASK_INFO               1

#define RHINO_CONFIG_TASK_INFO_NUM           2

#define RHINO_CONFIG_TASK_DEL                1

#define RHINO_CONFIG_SCHED_RR                0

#define RHINO_CONFIG_TIME_SLICE_DEFAULT      50

#define RHINO_CONFIG_PRI_MAX                 62

#define RHINO_CONFIG_USER_PRI_MAX            (RHINO_CONFIG_PRI_MAX - 2)

/* kernel mm_region conf */
#define RHINO_CONFIG_MM_REGION_MUTEX         1

/* kernel timer&tick conf */
#define RHINO_CONFIG_TICKS_PER_SECOND        100

#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   200

#define RHINO_CONFIG_TIMER_TASK_PRI          5

#define RHINO_CONFIG_TIMER_MSG_NUM           20

#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    1

/* kernel dyn alloc conf */
#define RHINO_CONFIG_KOBJ_DYN_ALLOC          1

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#define RHINO_CONFIG_K_DYN_TASK_STACK        256

#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      RHINO_CONFIG_USER_PRI_MAX

#endif /* RHINO_CONFIG_KOBJ_DYN_ALLOC */

/* kernel idle conf */
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    64

/* kernel hook conf */
#define RHINO_CONFIG_USER_HOOK               0

#define RHINO_CONFIG_CPU_NUM                 1

#endif /* K_CONFIG_H */

