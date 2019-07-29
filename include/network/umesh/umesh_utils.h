/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __UMESH_UTILS_H__
#define __UMESH_UTILS_H__

#ifdef CONFIG_NET_LWIP
#include "arch/cc.h"
#include <lwip/def.h>
#else
#include "utilities/mem/def.h"
#endif

#include "umesh_types.h"
#include "umesh_pal.h"

#include "utilities/configs.h"
#include "utilities/logging.h"
#include "utilities/message.h"
#include "utilities/timer.h"
#include "utilities/memory.h"
#include "utilities/maths.h"
#include "utilities/mac_whitelist.h"

#define umesh_kv_get umesh_pal_kv_get
#define umesh_kv_set umesh_pal_kv_set
#define umesh_malloc umesh_pal_malloc
#define umesh_free umesh_pal_free
#define umesh_now_ms umesh_pal_now_ms
#define umesh_get_timestamp umesh_pal_get_timestamp
#define umesh_set_timestamp umesh_pal_set_timestamp
#define umesh_post_event umesh_pal_post_event
#define umesh_log umesh_pal_log
#define umesh_task_schedule_call umesh_pal_schedule_call

#endif
