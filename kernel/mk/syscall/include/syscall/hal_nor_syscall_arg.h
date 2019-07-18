/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_NOR_SYSCALL_ARG_H
#define HAL_NOR_SYSCALL_ARG_H

#include "aos/hal/nor.h"

typedef struct {
    nor_dev_t *nor;
} hal_nor_init_syscall_arg_t;

typedef struct {
    nor_dev_t *nor;
    uint32_t  *addr;
    uint8_t   *data;
    uint32_t   len;
} hal_nor_read_syscall_arg_t;

typedef struct {
    nor_dev_t *nor;
    uint32_t  *addr;
    uint8_t   *data;
    uint32_t   len;
} hal_nor_write_syscall_arg_t;

typedef struct {
    nor_dev_t *nor;
    uint32_t  *addr;
    uint32_t   block_count;
} hal_nor_erase_block_syscall_arg_t;

typedef struct {
    nor_dev_t *nor;
    uint32_t  *addr;
} hal_nor_erase_chip_syscall_arg_t;

typedef struct {
    nor_dev_t *nor;
} hal_nor_finalize_syscall_arg_t;

#endif /* HAL_NOR_SYSCALL_ARG_H */

