/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORAWAN_PORT_H__
#define __LORAWAN_PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "hal/lorawan.h"
#include "utilities.h"

// rhino header files
#include <k_config.h>
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_critical.h>
#include <k_spin_lock.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_obj.h>
#include <k_sched.h>
#include <k_task.h>
#include <k_ringbuf.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_sem.h>
#include <k_task_sem.h>
#include <k_mutex.h>
#include <k_event.h>
#include <k_stats.h>
#include <k_mm_debug.h>
#include <k_mm_blk.h>
#include <k_mm_region.h>
#include <k_mm.h>
#include <k_workqueue.h>
#include <k_internal.h>
#include <k_trace.h>
#include <k_soc.h>
#include <k_hook.h>
#include <port.h>
#include <k_bitmap.h>

    typedef uint32_t time_ms_t;
    typedef uint32_t time_tick_t;

    extern hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode;
    extern hal_lrwan_time_itf_t     aos_lrwan_time_itf;
    extern hal_lrwan_radio_ctrl_t   aos_lrwan_radio_ctrl;
    extern hal_lrwan_sys_t          aos_lrwan_sys;
    extern hal_manufactory_itf_t    aos_mft_itf;

#ifdef __cplusplus
}
#endif

#endif /* lorawan_port.h */
