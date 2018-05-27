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

#ifndef TYPES_H
#define TYPES_H

#define RHINO_NO_WAIT                0u
//#define RHINO_WAIT_FOREVER           0xffffffffu     /* 32 bit value, if tick type is 64 bit, you need change it to 64 bit */
#define RHINO_TASK_STACK_OVF_MAGIC   0xdeadbeafu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_INTRPT_STACK_OVF_MAGIC 0xdeaddeadu     /* 32 bit or 64 bit stack overflow magic value */
#define RHINO_MM_FRAG_ALLOCATED      0xabcddcabu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_MM_FRAG_FREE           0xfefdecdbu     /* 32 bit value, if 64 bit system, you need change it to 64 bit */
#define RHINO_INLINE                 static __inline /* inline keyword, it may change under different compilers */

#define RHINO_MM_CORRUPT_DYE        0xFEFEFEFE
#define RHINO_MM_FREE_DYE           0xABABABAB

typedef char     name_t;
typedef uint32_t sem_count_t;
typedef uint32_t cpu_stack_t;

/* you may change here depend on your hardware timer */
typedef uint32_t hr_timer_t;       /* 32 bit or 64 bit unsigned value */
typedef uint32_t lr_timer_t;       /* 32 bit or 64 bit unsigned value */

//typedef uint64_t tick_t;           /* 32 bit or 64 bit unsigned value */
//typedef uint64_t idle_count_t;     /* 64 bit unsigned value */
//typedef uint64_t sys_time_t;       /* 64 bit unsigned value */
typedef uint32_t mutex_nested_t;   /* 8 bit or 16bit or 32bit unsigned value */
typedef uint8_t  suspend_nested_t; /* 8 bit normally */

typedef uint64_t ctx_switch_t;     /* 32 bit or 64 bit unsigned value */

//typedef int32_t  ssize_t;

#endif /* TYPES_H */

