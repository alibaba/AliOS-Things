/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "aos/hal/sd.h"
#include "hal_sd_syscall_arg.h"

int32_t sys_hal_sd_init_stub(void *arg)
{
    hal_sd_init_syscall_arg_t *_arg = arg;

    return hal_sd_init(_arg->sd);
}

int32_t sys_hal_sd_blks_read_stub(void *arg)
{
    hal_sd_blks_read_syscall_arg_t *_arg = arg;

    return hal_sd_blks_read(_arg->sd, _arg->data, _arg->blk_addr,
                            _arg->blks, _arg->timeout);
}

int32_t sys_hal_sd_blks_write_stub(void *arg)
{
    hal_sd_blks_write_syscall_arg_t *_arg = arg;

    return hal_sd_blks_write(_arg->sd, _arg->data, _arg->blk_addr,
                             _arg->blks, _arg->timeout);
}

int32_t sys_hal_sd_erase_stub(void *arg)
{
    hal_sd_erase_syscall_arg_t *_arg = arg;

    return hal_sd_erase(_arg->sd, _arg->blk_start_addr,
                        _arg->blk_end_addr);
}

int32_t sys_hal_sd_stat_get_stub(void *arg)
{
    hal_sd_stat_get_syscall_arg_t *_arg = arg;

    return hal_sd_stat_get(_arg->sd, _arg->stat);;
}

int32_t sys_hal_sd_info_get_stub(void *arg)
{
    hal_sd_info_get_syscall_arg_t *_arg = arg;

    return hal_sd_info_get(_arg->sd, _arg->info);
}

int32_t sys_hal_sd_finalize_stub(void *arg)
{
    hal_sd_finalize_syscall_arg_t *_arg = arg;

    return hal_sd_finalize(_arg->sd);
}

