/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_sd_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_sd_init(sd_dev_t *sd)
{
    hal_sd_init_syscall_arg_t _arg;

    _arg.sd = sd;

    return (int32_t)SYSCALL(SYS_HAL_SD_INIT, (void*)&_arg);
}
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                            uint32_t blks, uint32_t timeout)
{
    hal_sd_blks_read_syscall_arg_t _arg;

    _arg.sd       = sd;
    _arg.data     = data;
    _arg.blk_addr = blk_addr;
    _arg.blks     = blks;
    _arg.timeout  = timeout;

    return (int32_t)SYSCALL(SYS_HAL_SD_BLKS_READ, (void*)&_arg);
}
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                            uint32_t blks, uint32_t timeout)
{
    hal_sd_blks_write_syscall_arg_t _arg;

    _arg.sd       = sd;
    _arg.data     = data;
    _arg.blk_addr = blk_addr;
    _arg.blks     = blks;
    _arg.timeout  = timeout;

    return (int32_t)SYSCALL(SYS_HAL_SD_BLKS_WRITE, (void*)&_arg);
}
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    hal_sd_erase_syscall_arg_t _arg;

    _arg.sd             = sd;
    _arg.blk_start_addr = blk_start_addr;
    _arg.blk_end_addr   = blk_end_addr;

    return (int32_t)SYSCALL(SYS_HAL_SD_ERASE, (void*)&_arg);
}
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
    hal_sd_stat_get_syscall_arg_t _arg;

    _arg.sd   = sd;
    _arg.stat = stat;

    return (int32_t)SYSCALL(SYS_HAL_SD_STAT_GET, (void*)&_arg);
}
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
    hal_sd_info_get_syscall_arg_t _arg;

    _arg.sd   = sd;
    _arg.info = info;

    return (int32_t)SYSCALL(SYS_HAL_SD_INFO_GET, (void*)&_arg);
}
int32_t hal_sd_finalize(sd_dev_t *sd)
{
    hal_sd_finalize_syscall_arg_t _arg;

    _arg.sd = sd;

    return (int32_t)SYSCALL(SYS_HAL_SD_FINALIZE, (void*)&_arg);
}

