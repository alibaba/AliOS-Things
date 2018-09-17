/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_DEFAULT_CONFIG_H
#define K_DEFAULT_CONFIG_H

#ifndef RHINO_CONFIG_USER_SPACE
#define RHINO_CONFIG_USER_SPACE              0
#endif

#ifndef RHINO_CONFIG_CPU_PWR_MGMT
#define RHINO_CONFIG_CPU_PWR_MGMT            0
#endif

#ifndef RHINO_SCHED_NONE_PREEMPT
#define RHINO_SCHED_NONE_PREEMPT             0
#endif

#ifndef RHINO_CONFIG_STK_CHK_WORDS
#define RHINO_CONFIG_STK_CHK_WORDS           1u
#endif

#ifndef RHINO_CONFIG_CPU_STACK_DOWN
#define RHINO_CONFIG_CPU_STACK_DOWN          1
#endif

#ifndef RHINO_CONFIG_BITMAP_HW
#define RHINO_CONFIG_BITMAP_HW               0
#endif

#ifndef RHINO_CONFIG_GCC_RETADDR
#define RHINO_CONFIG_GCC_RETADDR             0
#endif

/* kernel feature conf */
#ifndef RHINO_CONFIG_SEM
#define RHINO_CONFIG_SEM                     0
#endif

#ifndef RHINO_CONFIG_TASK_SEM
#define RHINO_CONFIG_TASK_SEM                0
#endif

#ifndef RHINO_CONFIG_QUEUE
#define RHINO_CONFIG_QUEUE                   0
#endif

#ifndef RHINO_CONFIG_BUF_QUEUE
#define RHINO_CONFIG_BUF_QUEUE               0
#endif

#ifndef RHINO_CONFIG_WORKQUEUE
#define RHINO_CONFIG_WORKQUEUE               0
#endif

#if (RHINO_CONFIG_WORKQUEUE > 0)

#ifndef RHINO_CONFIG_WORKQUEUE_STACK_SIZE
#define RHINO_CONFIG_WORKQUEUE_STACK_SIZE    512
#endif

#ifndef RHINO_CONFIG_WORKQUEUE_TASK_PRIO
#define RHINO_CONFIG_WORKQUEUE_TASK_PRIO     20
#endif

#endif /* RHINO_CONFIG_WORKQUEUE */

#ifndef RHINO_CONFIG_EVENT_FLAG
#define RHINO_CONFIG_EVENT_FLAG              0
#endif

#ifndef RHINO_CONFIG_TIMER
#define RHINO_CONFIG_TIMER                   0
#endif

#ifndef RHINO_CONFIG_MM_BLK
#define RHINO_CONFIG_MM_BLK                  1
#endif

#ifndef RHINO_CONFIG_MM_BLK_SIZE
#define RHINO_CONFIG_MM_BLK_SIZE             32
#endif

/* heap conf */
#ifndef RHINO_CONFIG_MM_TLF
#define RHINO_CONFIG_MM_TLF                  1
#endif

#if (RHINO_CONFIG_MM_TLF > 0)

#ifndef RHINO_CONFIG_MM_MINISIZEBIT
#define RHINO_CONFIG_MM_MINISIZEBIT          6
#endif

#ifndef RHINO_CONFIG_MM_MAXMSIZEBIT
#define RHINO_CONFIG_MM_MAXMSIZEBIT          20
#endif

#ifndef RHINO_CONFIG_MM_TLF_BLK_SIZE
#define RHINO_CONFIG_MM_TLF_BLK_SIZE         8192
#endif

#ifndef RHINO_CONFIG_MM_QUICK
#define RHINO_CONFIG_MM_QUICK                1
#endif

#ifndef RHINO_CONFIG_MM_DEBUG
#define RHINO_CONFIG_MM_DEBUG                0
#endif

#ifndef RHINO_CONFIG_MM_LEAKCHECK
#define RHINO_CONFIG_MM_LEAKCHECK            0
#endif

/* kernel mm_region conf */
#ifndef RHINO_CONFIG_MM_REGION_MUTEX
#define RHINO_CONFIG_MM_REGION_MUTEX         1
#endif

#endif /* RHINO_CONFIG_MM_TLF */

/* kernel task conf */
#ifndef RHINO_CONFIG_TASK_PRI_CHG
#define RHINO_CONFIG_TASK_PRI_CHG            1
#endif

#ifndef RHINO_CONFIG_TASK_INFO
#define RHINO_CONFIG_TASK_INFO               0
#endif

#ifndef RHINO_CONFIG_TASK_INFO_NUM
#define RHINO_CONFIG_TASK_INFO_NUM           2
#endif

#ifndef RHINO_CONFIG_TASK_DEL
#define RHINO_CONFIG_TASK_DEL                0
#endif

#ifndef RHINO_CONFIG_TASK_STACK_CUR_CHECK
#define RHINO_CONFIG_TASK_STACK_CUR_CHECK    0
#endif

#ifndef RHINO_CONFIG_TASK_WAIT_ABORT
#define RHINO_CONFIG_TASK_WAIT_ABORT         0
#endif

#ifndef RHINO_CONFIG_SCHED_RR
#define RHINO_CONFIG_SCHED_RR                0
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

#ifndef RHINO_CONFIG_RINGBUF_VENDOR
#define RHINO_CONFIG_RINGBUF_VENDOR          0
#endif

/* kernel timer&tick conf */
#ifndef RHINO_CONFIG_HW_COUNT
#define RHINO_CONFIG_HW_COUNT                0
#endif

#ifndef RHINO_CONFIG_TICKS_PER_SECOND
#define RHINO_CONFIG_TICKS_PER_SECOND        100
#endif

#ifndef RHINO_CONFIG_TIMER_TASK_STACK_SIZE
#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   200
#endif

#ifndef RHINO_CONFIG_TIMER_RATE
#define RHINO_CONFIG_TIMER_RATE              1
#endif

#ifndef RHINO_CONFIG_TIMER_TASK_PRI
#define RHINO_CONFIG_TIMER_TASK_PRI          5
#endif

#ifndef RHINO_CONFIG_TIMER_MSG_NUM
#define RHINO_CONFIG_TIMER_MSG_NUM           20
#endif

/* kernel intrpt conf */
#ifndef RHINO_CONFIG_INTRPT_STACK_REMAIN_GET
#define RHINO_CONFIG_INTRPT_STACK_REMAIN_GET 0
#endif

#ifndef RHINO_CONFIG_INTRPT_MAX_NESTED_LEVEL
#define RHINO_CONFIG_INTRPT_MAX_NESTED_LEVEL 188u
#endif

#ifndef RHINO_CONFIG_INTRPT_GUARD
#define RHINO_CONFIG_INTRPT_GUARD            0
#endif

/* kernel stack ovf check */
#ifndef RHINO_CONFIG_INTRPT_STACK_OVF_CHECK
#define RHINO_CONFIG_INTRPT_STACK_OVF_CHECK  0
#endif

#ifndef RHINO_CONFIG_TASK_STACK_OVF_CHECK
#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    0
#endif

/* kernel dyn alloc conf */
#ifndef RHINO_CONFIG_KOBJ_DYN_ALLOC
#define RHINO_CONFIG_KOBJ_DYN_ALLOC          0
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

#ifndef RHINO_CONFIG_K_DYN_QUEUE_MSG
#define RHINO_CONFIG_K_DYN_QUEUE_MSG         30
#endif

#ifndef RHINO_CONFIG_K_DYN_TASK_STACK
#define RHINO_CONFIG_K_DYN_TASK_STACK        256
#endif

#ifndef RHINO_CONFIG_K_DYN_MEM_TASK_PRI
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      RHINO_CONFIG_USER_PRI_MAX
#endif

#endif /* RHINO_CONFIG_KOBJ_DYN_ALLOC */

/* kernel idle conf */
#ifndef RHINO_CONFIG_IDLE_TASK_STACK_SIZE
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    100
#endif

/* kernel hook conf */
#ifndef RHINO_CONFIG_USER_HOOK
#define RHINO_CONFIG_USER_HOOK               1
#endif

/* kernel stats conf */
#ifndef RHINO_CONFIG_SYSTEM_STATS
#define RHINO_CONFIG_SYSTEM_STATS            0
#endif

#ifndef RHINO_CONFIG_SCHED_STATS
#define RHINO_CONFIG_SCHED_STATS             0
#endif

#ifndef RHINO_CONFIG_INTRPT_STATS
#define RHINO_CONFIG_INTRPT_STATS            0
#endif

#ifndef RHINO_CONFIG_TASK_SCHED_STATS
#define RHINO_CONFIG_TASK_SCHED_STATS        0
#endif

#ifndef RHINO_CONFIG_CPU_USAGE_PERIOD
#define RHINO_CONFIG_CPU_USAGE_PERIOD        0
#endif

/* kernel trace conf */
#ifndef RHINO_CONFIG_TRACE
#define RHINO_CONFIG_TRACE                   0
#endif

#ifndef RHINO_CONFIG_CPU_NUM
#define RHINO_CONFIG_CPU_NUM                 1
#endif

#if ((RHINO_CONFIG_TIMER >= 1) && (RHINO_CONFIG_BUF_QUEUE == 0))
#error "RHINO_CONFIG_BUF_QUEUE should be 1 when RHINO_CONFIG_TIMER is enabled."
#endif

#if ((RHINO_CONFIG_MM_TLF >= 1) && (RHINO_CONFIG_MM_BLK == 0))
#error "RHINO_CONFIG_MM_BLK should be 1 when RHINO_CONFIG_MM_TLF is enabled."
#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC >= 1) && (RHINO_CONFIG_MM_TLF == 0))
#error \
  "RHINO_CONFIG_MM_TLF should be 1 when RHINO_CONFIG_KOBJ_DYN_ALLOC is enabled."
#endif

#if (RHINO_CONFIG_PRI_MAX >= 256)
#error "RHINO_CONFIG_PRI_MAX must be <= 255."
#endif

#if ((RHINO_CONFIG_SEM == 0) && (RHINO_CONFIG_TASK_SEM >= 1))
#error "you need enable RHINO_CONFIG_SEM as well."
#endif

#if ((RHINO_CONFIG_HW_COUNT == 0) && (RHINO_CONFIG_TASK_SCHED_STATS >= 1))
#error "you need enable RHINO_CONFIG_HW_COUNT as well."
#endif

#if ((RHINO_CONFIG_HW_COUNT == 0) && (RHINO_CONFIG_SCHED_STATS >= 1))
#error "you need enable RHINO_CONFIG_HW_COUNT as well."
#endif

#if ((RHINO_CONFIG_HW_COUNT == 0) && (RHINO_CONFIG_INTRPT_STATS >= 1))
#error "you need enable RHINO_CONFIG_HW_COUNT as well."
#endif

#if ((RHINO_CONFIG_TIMER == 0) && (RHINO_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_TIMER as well."
#endif

#if ((RHINO_CONFIG_TASK_SCHED_STATS == 0) && (RHINO_CONFIG_CPU_USAGE_PERIOD >= 1))
#error "you need enable RHINO_CONFIG_TASK_SCHED_STATS as well."
#endif

#endif /* K_DEFAULT_CONFIG_H */
