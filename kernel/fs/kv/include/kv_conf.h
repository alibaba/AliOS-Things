/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef KV_CONF_H
#define KV_CONF_H

/* The totally storage size for key-value store */
#ifndef CONFIG_KV_TOTAL_SIZE
#define KV_TOTAL_SIZE (8 * 1024)
#else
#define KV_TOTAL_SIZE CONFIG_KV_TOTAL_SIZE
#endif

/* The physical parition for key-value store */
#ifndef CONFIG_KV_PARTITION
#define KV_PARTITION HAL_PARTITION_PARAMETER_2
#else
#define KV_PARTITION CONFIG_KV_PARTITION
#endif

/* The number of bits in block size */
#ifndef CONFIG_KV_BLOCK_SIZE_BITS
/* Default is 4K bytes, should equal or larger than erase sector size */
#define KV_BLOCK_SIZE_BITS 12
#else
#define KV_BLOCK_SIZE_BITS CONFIG_KV_BLOCK_SIZE_BITS
#endif

#ifndef CONFIG_KV_TASK_PRIORITY
#define KV_TASK_PRIORITY 32
#else
#define KV_TASK_PRIORITY CONFIG_KV_TASK_PRIORITY
#endif

#ifndef CONFIG_KV_TASK_STACK_SIZE
#define KV_TASK_STACK_SIZE 1024
#else
#define KV_TASK_STACK_SIZE CONFIG_KV_TASK_STACK_SIZE
#endif

#ifndef CONFIG_KV_MAX_KEY_LEN
#define KV_MAX_KEY_LEN 128
#else
#define KV_MAX_KEY_LEN CONFIG_KV_MAX_KEY_LEN
#endif

#ifndef CONFIG_KV_MAX_VAL_LEN
#define KV_MAX_VAL_LEN 512
#else
#define KV_MAX_VAL_LEN CONFIG_KV_MAX_VAL_LEN
#endif

#endif  /* KV_CONF_H */

