/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CONFIG_H
#define CONFIG_H

#define RHINO_CONFIG_STD_MALLOC              1
#define K_MM_STATISTIC                       1

/* chip level conf */
#define RHINO_CONFIG_LITTLE_ENDIAN           1
#define RHINO_CONFIG_CPU_STACK_DOWN          1

/* kernel feature conf */
#define RHINO_CONFIG_SEM                     1
#define RHINO_CONFIG_QUEUE                   1
#define RHINO_CONFIG_TASK_SEM                1
#define RHINO_CONFIG_EVENT_FLAG              1
#define RHINO_CONFIG_TIMER                   1
#define RHINO_CONFIG_BUF_QUEUE               1
#define RHINO_CONFIG_MM_BLK                  1
#define RHINO_CONFIG_MM_FIRSTFIT             1
#define RHINO_CONFIG_MM_BESTFIT              1

#define RHINO_CONFIG_MM_DEBUG                1
#define RHINO_CONFIG_KOBJ_SET                1
#define RHINO_CONFIG_RINGBUF_VENDOR          0

/* kernel dynamic tick conf */
#ifdef CONFIG_LPM_TICKLESS_SLEEP
#define RHINO_CONFIG_DYNTICKLESS             1
#else
#define RHINO_CONFIG_DYNTICKLESS             0
#endif

/*
#define RHINO_CONFIG_TICKS_PER_SECOND        100
#define RHINO_CONFIG_TICK_HEAD_ARRAY         1
#define RHINO_CONFIG_SCHED_RR                0
#define RHINO_CONFIG_TIMER_RATE              1
#define RHINO_CONFIG_TIMER                   1
*/

/* kernel task conf */
#define RHINO_CONFIG_TASK_SUSPEND            1
#define RHINO_CONFIG_TASK_INFO               0
#define RHINO_CONFIG_TASK_DEL                1
#define RHINO_CONFIG_TASK_WAIT_ABORT         0
#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    1

#if (RHINO_CONFIG_DYNTICKLESS > 0)
#define RHINO_CONFIG_SCHED_RR                0
#else
#define RHINO_CONFIG_SCHED_RR                1
#endif

#define RHINO_CONFIG_TIME_SLICE_DEFAULT      50
#define RHINO_CONFIG_PRI_MAX                 62
#define RHINO_CONFIG_USER_PRI_MAX            (RHINO_CONFIG_PRI_MAX - 2)

/* kernel workqueue conf */
#define RHINO_CONFIG_WORKQUEUE               0

/* kernel timer&tick conf */
#define RHINO_CONFIG_HW_COUNT                0
#define RHINO_CONFIG_TICK_TASK               1
#if (RHINO_CONFIG_TICK_TASK > 0)
#ifdef CONFIG_LPM_TICKLESS_SLEEP
#define RHINO_CONFIG_TICK_TASK_STACK_SIZE    250
#else
#define RHINO_CONFIG_TICK_TASK_STACK_SIZE    50
#endif
#define RHINO_CONFIG_TICK_TASK_PRI           1
#endif

#define RHINO_CONFIG_TICKS_PER_SECOND        100
/* must be 2^n size!, such as 1, 2, 4, 8, 16,32, etc....... */
#define RHINO_CONFIG_TICK_HEAD_ARRAY         1

#define RHINO_CONFIG_TIMER_TASK_STACK_SIZE   200
#ifdef CONFIG_LPM_TICKLESS_SLEEP
#define RHINO_CONFIG_TIMER_RATE              50
#else
#define RHINO_CONFIG_TIMER_RATE              1
#endif
#define RHINO_CONFIG_TIMER_TASK_PRI          5

/* kernel intrpt conf */
#define RHINO_CONFIG_INTRPT_STACK_REMAIN_GET 1
#define RHINO_CONFIG_INTRPT_STACK_OVF_CHECK  0
#define RHINO_CONFIG_INTRPT_MAX_NESTED_LEVEL 188u
#define RHINO_CONFIG_INTRPT_GUARD            0
#define RHINO_CONFIG_STACK_OVF_CHECK_HW      0

/* kernel dyn alloc conf */
#define RHINO_CONFIG_KOBJ_DYN_ALLOC          1
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#define RHINO_CONFIG_K_DYN_QUEUE_MSG         30
#define RHINO_CONFIG_K_DYN_TASK_STACK        64
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      RHINO_CONFIG_USER_PRI_MAX
#endif

/* kernel idle conf */
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    128

/* kernel hook conf */
#define RHINO_CONFIG_USER_HOOK               1

/* kernel stats conf */
#define RHINO_CONFIG_SYSTEM_STATS            1
#define RHINO_CONFIG_DISABLE_SCHED_STATS     0
#define RHINO_CONFIG_DISABLE_INTRPT_STATS    0
#define RHINO_CONFIG_CPU_USAGE_STATS         0
#define RHINO_CONFIG_CPU_USAGE_TASK_PRI      (RHINO_CONFIG_PRI_MAX - 2)
#define RHINO_CONFIG_TASK_SCHED_STATS        0
#define RHINO_CONFIG_CPU_USAGE_TASK_STACK    50

#endif /* CONFIG_H */

