/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _KV_CONF_H
#define _KV_CONF_H

/* The totally storage size for key-value store */
#ifndef CONFIG_AOS_KV_BUFFER_SIZE
#define KV_TOTAL_SIZE (8 * 1024)
#else
#define KV_TOTAL_SIZE CONFIG_AOS_KV_BUFFER_SIZE
#endif

/* The physical parition for key-value store */
#ifndef CONFIG_AOS_KV_PTN
#define KV_PTN HAL_PARTITION_PARAMETER_2
#else
#define KV_PTN CONFIG_AOS_KV_PTN
#endif

/* The number of bits in block size */
#ifndef CONFIG_AOS_KV_BLK_BITS
/* Default is 4K bytes, should equal or larger than erase sector size */
#define BLK_BITS 12
#else
#define BLK_BITS CONFIG_AOS_KV_BLK_BITS
#endif

#ifndef CONFIG_AOS_KV_TASK_PRI
#define KV_TASK_PRI 32
#else
#define KV_TASK_PRI CONFIG_AOS_KV_TASK_PRI
#endif

#ifndef CONFIG_AOS_KV_TASK_SIZE
#define KV_GC_TASK_SIZE 1024
#else
#define KV_GC_TASK_SIZE CONFIG_AOS_KV_TASK_SIZE
#endif

#endif
