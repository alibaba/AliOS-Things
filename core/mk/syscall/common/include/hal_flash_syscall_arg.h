/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_FLASH_SYSCALL_ARG_H
#define HAL_FLASH_SYSCALL_ARG_H

#include "aos/hal/flash.h"

typedef struct {
    hal_partition_t        in_partition;
    hal_logic_partition_t *partition;
} hal_flash_info_get_syscall_arg_t;

typedef struct {
    hal_partition_t in_partition;
    uint32_t        off_set;
    uint32_t        size;
} hal_flash_erase_syscall_arg_t;

typedef struct {
    hal_partition_t  in_partition;
    uint32_t        *off_set;
    const void      *in_buf;
    uint32_t         in_buf_len;
} hal_flash_write_syscall_arg_t;

typedef struct {
    hal_partition_t  in_partition;
    uint32_t        *off_set;
    const void      *in_buf;
    uint32_t         in_buf_len;
} hal_flash_erase_write_syscall_arg_t;

typedef struct {
    hal_partition_t  in_partition;
    uint32_t        *off_set;
    void            *out_buf;
    uint32_t         in_buf_len;
} hal_flash_read_syscall_arg_t;

typedef struct {
    hal_partition_t partition;
    uint32_t        off_set;
    uint32_t        size;
} hal_flash_enable_secure_syscall_arg_t;

typedef struct {
    hal_partition_t in_partition;
    uint32_t        off_set;
    uint32_t        size;
} hal_flash_dis_secure_syscall_arg_t;

#endif /* HAL_FLASH_SYSCALL_ARG_H */

