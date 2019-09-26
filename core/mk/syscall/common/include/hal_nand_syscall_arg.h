/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_NAND_SYSCALL_ARG_H
#define HAL_NAND_SYSCALL_ARG_H

#include "aos/hal/nand.h"

typedef struct {
    nand_dev_t *nand;
} hal_nand_init_syscall_arg_t;

typedef struct {
    nand_dev_t  *nand;
    nand_addr_t *addr;
    uint8_t     *data;
    uint32_t     page_count;
} hal_nand_read_page_syscall_arg_t;

typedef struct {
    nand_dev_t  *nand;
    nand_addr_t *addr;
    uint8_t     *data;
    uint32_t     page_count;
} hal_nand_write_page_syscall_arg_t;

typedef struct {
    nand_dev_t  *nand;
    nand_addr_t *addr;
    uint8_t     *data;
    uint32_t     data_len;
} hal_nand_read_spare_syscall_arg_t;

typedef struct {
    nand_dev_t  *nand;
    nand_addr_t *addr;
    uint8_t     *data;
    uint32_t     data_len;
} hal_nand_write_spare_syscall_arg_t;

typedef struct {
    nand_dev_t  *nand;
    nand_addr_t *addr;
} hal_nand_erase_block_syscall_arg_t;

typedef struct {
    nand_dev_t *nand;
} hal_nand_finalize_syscall_arg_t;

#endif /* HAL_NAND_SYSCALL_ARG_H */

