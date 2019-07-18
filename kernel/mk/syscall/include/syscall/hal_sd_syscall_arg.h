/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_SD_SYSCALL_ARG_H
#define HAL_SD_SYSCALL_ARG_H

#include "aos/hal/sd.h"

typedef struct {
    sd_dev_t *sd;
} hal_sd_init_syscall_arg_t;

typedef struct {
    sd_dev_t *sd;
    uint8_t  *data;
    uint32_t  blk_addr;
    uint32_t  blks;
    uint32_t  timeout;
} hal_sd_blks_read_syscall_arg_t;

typedef struct {
    sd_dev_t *sd;
    uint8_t  *data;
    uint32_t  blk_addr;
    uint32_t  blks;
    uint32_t  timeout;
} hal_sd_blks_write_syscall_arg_t;

typedef struct {
    sd_dev_t *sd;
    uint32_t  blk_start_addr;
    uint32_t  blk_end_addr;
} hal_sd_erase_syscall_arg_t;

typedef struct {
    sd_dev_t    *sd;
    hal_sd_stat *stat;
} hal_sd_stat_get_syscall_arg_t;

typedef struct {
    sd_dev_t      *sd;
    hal_sd_info_t *info;
} hal_sd_info_get_syscall_arg_t;

typedef struct {
    sd_dev_t *sd;
} hal_sd_finalize_syscall_arg_t;

#endif /* HAL_SD_SYSCALL_ARG_H */

