/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ESPOS_TYPES_H_
#define _ESPOS_TYPES_H_

#include <stdint.h>
#include <stddef.h>

#include "arch/espos_arch.h"

#define ESPOS_OBJ_NONE  0

#ifdef ARCH64
typedef uint64_t        espos_obj_t;
#else
typedef uint32_t        espos_obj_t;
#endif

typedef espos_obj_t     espos_task_t;
typedef espos_obj_t     espos_queue_t;
typedef espos_obj_t     espos_mutex_t;
typedef espos_obj_t     espos_sem_t;
typedef espos_obj_t     espos_timer_t;
typedef espos_obj_t     espos_critical_t;

typedef size_t          espos_size_t;
typedef size_t          espos_pos_t;
typedef uint8_t         espos_prio_t;
typedef uint32_t        espos_opt_t;
typedef int32_t         espos_cpu_t;

typedef espos_size_t    espos_tick_t;
typedef espos_size_t    espos_time_t;
typedef espos_size_t    espos_sem_count_t;

typedef void (*espos_task_entry_t)(void *p);
typedef void (*espos_timer_cb_t)(espos_timer_t timer, void *arg);

#endif

