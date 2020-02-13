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

#ifndef K_CONFIG_H
#define K_CONFIG_H

#include <csi_config.h>

/* heap conf */
#define RHINO_CONFIG_MM_BLK                  1
#define RHINO_CONFIG_MM_DEBUG                0

/*
#define RHINO_CONFIG_TICKS_PER_SECOND        100
#define RHINO_CONFIG_SCHED_RR                0
#define RHINO_CONFIG_TIMER                   1
*/

/* kernel task conf */
#define RHINO_CONFIG_TASK_INFO               0
#define RHINO_CONFIG_TASK_STACK_OVF_CHECK    1

#define RHINO_CONFIG_SCHED_RR                1

#define RHINO_CONFIG_TIME_SLICE_DEFAULT      50
#define RHINO_CONFIG_PRI_MAX                 62
#define RHINO_CONFIG_USER_PRI_MAX            (RHINO_CONFIG_PRI_MAX - 2)

/* kernel timer&tick conf */
#define RHINO_CONFIG_TICKS_PER_SECOND        100

#define RHINO_CONFIG_KOBJ_DYN_ALLOC          1
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#define RHINO_CONFIG_K_DYN_TASK_STACK        (64 + CSK_CPU_STACK_EXTRAL)
#define RHINO_CONFIG_K_DYN_MEM_TASK_PRI      RHINO_CONFIG_USER_PRI_MAX
#endif

/* kernel idle conf */
#define RHINO_CONFIG_IDLE_TASK_STACK_SIZE    128

/* kernel hook conf */
#define RHINO_CONFIG_USER_HOOK               1

#endif /* K_CONFIG_H */

