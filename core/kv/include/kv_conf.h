/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef KV_CONF_H
#define KV_CONF_H

/* The totally storage size for key-value store */
#ifndef KV_CONFIG_TOTAL_SIZE
#define KV_TOTAL_SIZE (8 * 1024)
#else
#define KV_TOTAL_SIZE KV_CONFIG_TOTAL_SIZE
#endif

/* The physical parition for key-value store */
#ifndef KV_CONFIG_PARTITION
#define KV_PARTITION HAL_PARTITION_PARAMETER_2
#else
#define KV_PARTITION KV_CONFIG_PARTITION
#endif

/* The number of bits in block size */
#ifndef KV_CONFIG_BLOCK_SIZE_BITS
/* Default is 4K bytes, should equal or larger than erase sector size */
#define KV_BLOCK_SIZE_BITS 12
#else
#define KV_BLOCK_SIZE_BITS KV_CONFIG_BLOCK_SIZE_BITS
#endif

#ifndef KV_CONFIG_TASK_PRIORITY
#define KV_TASK_PRIORITY 32
#else
#define KV_TASK_PRIORITY KV_CONFIG_TASK_PRIORITY
#endif

#ifndef KV_CONFIG_TASK_STACK_SIZE
#define KV_TASK_STACK_SIZE 1024
#else
#define KV_TASK_STACK_SIZE KV_CONFIG_TASK_STACK_SIZE
#endif

#ifndef KV_CONFIG_MAX_KEY_LEN
#define KV_MAX_KEY_LEN 128
#else
#define KV_MAX_KEY_LEN KV_CONFIG_MAX_KEY_LEN
#endif

#ifndef KV_CONFIG_MAX_VAL_LEN
#define KV_MAX_VAL_LEN 512
#else
#define KV_MAX_VAL_LEN KV_CONFIG_MAX_VAL_LEN
#endif

#ifndef KV_CONFIG_SECURE_SUPPORT
#define KV_SECURE_SUPPORT 0
#else
#define KV_SECURE_SUPPORT KV_CONFIG_SECURE_SUPPORT
#endif

#if (KV_SECURE_SUPPORT > 0)

#ifndef KV_CONFIG_SECURE_LEVEL
#define KV_SECURE_LEVEL 1
#else
#define KV_SECURE_LEVEL KV_CONFIG_SECURE_LEVEL
#endif

#ifndef KV_CONFIG_SECURE_CRYPT_IMPL
#define KV_SECURE_CRYPT_IMPL 1
#else
#define KV_SECURE_CRYPT_IMPL KV_CONFIG_SECURE_CRYPT_IMPL
#endif

#endif

#endif  /* KV_CONF_H */

