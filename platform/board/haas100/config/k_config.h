/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_CONFIG_H
#define K_CONFIG_H


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
#define RHINO_CONFIG_MM_BLK                  0
#endif
#ifndef RHINO_CONFIG_MM_DEBUG
#define RHINO_CONFIG_MM_DEBUG                0
#endif
#ifndef RHINO_CONFIG_MM_TLF
#define RHINO_CONFIG_MM_TLF                  1
#endif

#ifndef RHINO_CONFIG_MM_BLK_SIZE
#define RHINO_CONFIG_MM_BLK_SIZE             256
#endif

#ifndef RHINO_CONFIG_MM_MAXMSIZEBIT
#define RHINO_CONFIG_MM_MAXMSIZEBIT          28
#endif

#ifndef RHINO_CONFIG_MM_TLF_BLK_SIZE
#define RHINO_CONFIG_MM_TLF_BLK_SIZE         0x100000
#endif

#ifndef RHINO_CONFIG_ERR_DUMP
#define RHINO_CONFIG_ERR_DUMP                1
#endif

/* kernel task conf */
#ifndef RHINO_CONFIG_TASK_INFO
#define RHINO_CONFIG_TASK_INFO               1
#endif
#ifndef RHINO_CONFIG_TASK_DEL
#define RHINO_CONFIG_TASK_DEL                1
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

/* kernel workqueue conf */
//#ifndef RHINO_CONFIG_WORKQUEUE
#define RHINO_CONFIG_WORKQUEUE               1
//#endif
#ifndef RHINO_CONFIG_WORKQUEUE_STACK_SIZE
#define RHINO_CONFIG_WORKQUEUE_STACK_SIZE    768
#endif

/* kernel mm_region conf */
#ifndef RHINO_CONFIG_MM_REGION_MUTEX
#define RHINO_CONFIG_MM_REGION_MUTEX         0
#endif

/* kernel timer&tick conf */
#ifndef RHINO_CONFIG_TICKS_PER_SECOND
#define RHINO_CONFIG_TICKS_PER_SECOND        1000
#endif

/*must reserve enough stack size for timer cb will consume*/
#ifndef RHINO_CONFIG_TIMER_TASK_STACK_SIZE
#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   2048
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
#define RHINO_CONFIG_K_DYN_TASK_STACK        256
#endif
#ifndef RHINO_CONFIG_K_DYN_MEM_TASK_PRI
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      6
#endif
#endif

/* kernel idle conf */
#ifndef RHINO_CONFIG_IDLE_TASK_STACK_SIZE
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    1024
#endif

/* kernel hook conf */
#ifndef RHINO_CONFIG_USER_HOOK
#define RHINO_CONFIG_USER_HOOK               1
#endif

#ifndef RHINO_CONFIG_CPU_NUM
#define RHINO_CONFIG_CPU_NUM                 1
#endif


/*task user info index start*/
#ifndef RHINO_CONFIG_TASK_INFO_NUM
#define RHINO_CONFIG_TASK_INFO_NUM           5
#endif

#ifndef PTHREAD_CONFIG_USER_INFO_POS
#define PTHREAD_CONFIG_USER_INFO_POS         0
#endif

#ifndef RHINO_TASK_HOOK_USER_INFO_POS
#define RHINO_TASK_HOOK_USER_INFO_POS        1
#endif

#ifndef RHINO_CLI_CONSOLE_USER_INFO_POS
#define RHINO_CLI_CONSOLE_USER_INFO_POS      2
#endif

#ifndef RHINO_ERRNO_USER_INFO_POS
#define RHINO_ERRNO_USER_INFO_POS            3
#endif
/*task user info index end*/

#ifndef RHINO_CONFIG_SYS_STATS
#define RHINO_CONFIG_SYS_STATS               1
#endif

#ifndef RHINO_CONFIG_HW_COUNT
#define RHINO_CONFIG_HW_COUNT                1
#endif

#ifndef DEBUG_CONFIG_PANIC_CLI
#define DEBUG_CONFIG_PANIC_CLI               0
#endif

/* cli panic dump */
#ifndef DEBUG_CONFIG_LAST_WORD
#define DEBUG_CONFIG_LAST_WORD            1
#endif

#ifndef DEBUG_CONFIG_LASTWORD_RAM_ADDR
#define DEBUG_CONFIG_LASTWORD_RAM_ADDR       LASTWORD_RAM_ADDR
#endif

#ifndef DEBUG_CONFIG_LASTWORD_REGION_LEN
#define DEBUG_CONFIG_LASTWORD_REGION_LEN     0x4000
#endif

#ifndef RHINO_CONFIG_MM_TRACE_LVL
#define RHINO_CONFIG_MM_TRACE_LVL            0
#endif

#ifndef DEBUG_CONFIG_AOS_TRACE
#define DEBUG_CONFIG_AOS_TRACE               0
#endif

#ifndef RHINO_CONFIG_CLI_AS_NMI
#define RHINO_CONFIG_CLI_AS_NMI              0
#endif

#if (RHINO_CONFIG_CLI_AS_NMI > 0)
#ifndef RHINO_CONFIG_NMI_OFFSET
#define RHINO_CONFIG_NMI_OFFSET              48
#endif
#endif

#ifndef CLI_CONFIG_MAX_COMMANDS
#define CLI_CONFIG_MAX_COMMANDS              256
#endif

#endif /* K_CONFIG_H */
