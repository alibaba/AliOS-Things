/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef KV_CONF_H
#define KV_CONF_H

/* The physical parition for key-value store */
#ifndef KV_CONFIG_PARTITION
    #define KV_PARTITION HAL_PARTITION_ENV
#else
    #define KV_PARTITION KV_CONFIG_PARTITION
#endif

/* The page size, in bytes, of the flash used for KV storage. */
#ifndef KV_CONFIG_PAGE_SIZE
    #define KV_PAGE_SIZE (4 * 1024)
#else
    #define KV_PAGE_SIZE KV_CONFIG_PAGE_SIZE
#endif

/* blocks in kv partition */
#ifndef KV_CONFIG_BLOCK_NUMS
    #define KV_BLOCK_NUMS 4
#else
    #define KV_BLOCK_NUMS KV_CONFIG_BLOCK_NUMS
#endif

/* The block size, in bytes, of the flash used for KV storage. */
#ifndef KV_CONFIG_BLOCK_SIZE
    /* Default is 64 pages */
    #define KV_BLOCK_SIZE (64 * KV_PAGE_SIZE)
#else
    #define KV_BLOCK_SIZE KV_CONFIG_BLOCK_SIZE
#endif

/* The totally storage size, in bytes, for key-value store, must be multiple of block size. */
#ifndef KV_CONFIG_TOTAL_SIZE
    #define KV_TOTAL_SIZE (KV_BLOCK_NUMS * KV_BLOCK_SIZE)
#else
    #define KV_TOTAL_SIZE KV_CONFIG_TOTAL_SIZE
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

