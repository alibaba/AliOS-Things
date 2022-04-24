/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_API_H
#define K_API_H

/** @addtogroup rhino API
 *  All rhino header files.
 *
 *  @{
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "k_config.h"
#include "k_default_config.h"

#include "k_types.h"
#include "k_err.h"
#include "k_sys.h"
#include "k_critical.h"
#include "k_spin_lock.h"
#include "k_list.h"
#if (RHINO_CONFIG_SCHED_CFS > 0)
#include "k_cfs.h"
#endif
#include "k_obj.h"
#include "k_sched.h"
#include "k_task.h"
#include "k_ringbuf.h"
#include "k_queue.h"
#include "k_buf_queue.h"
#include "k_sem.h"
#include "k_task_sem.h"
#include "k_mutex.h"
#include "k_timer.h"
#include "k_time.h"
#include "k_event.h"
#include "k_stats.h"
#if RHINO_CONFIG_MM_DEBUG
#include "k_mm_debug.h"
#endif
#include "k_mm_blk.h"
#include "k_mm_region.h"
#include "k_mm.h"
#include "k_workqueue.h"

#include "k_internal.h"
#include "k_trace.h"
#include "k_soc.h"
#include "k_hook.h"
#include "k_bitmap.h"

#include "port.h"

/** @} */

#endif /* K_API_H */

