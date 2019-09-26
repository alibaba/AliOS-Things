/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_flash_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_flash_info_get(hal_partition_t in_partition,
                           hal_logic_partition_t *partition)
{
    hal_flash_info_get_syscall_arg_t _arg;

    _arg.in_partition = in_partition;
    _arg.partition    = partition;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_INFO_GET, (void*)&_arg);
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    hal_flash_erase_syscall_arg_t _arg;

    _arg.in_partition = in_partition;
    _arg.off_set      = off_set;
    _arg.size         = size;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_ERASE, (void*)&_arg);
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    hal_flash_write_syscall_arg_t _arg;

    _arg.in_partition = in_partition;
    _arg.off_set      = off_set;
    _arg.in_buf       = in_buf;
    _arg.in_buf_len   = in_buf_len;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_WRITE, (void*)&_arg);
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    hal_flash_erase_write_syscall_arg_t _arg;

    _arg.in_partition = in_partition;
    _arg.off_set      = off_set;
    _arg.in_buf       = in_buf;
    _arg.in_buf_len   = in_buf_len;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_ERASE_WRITE, (void*)&_arg);
}
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    hal_flash_read_syscall_arg_t _arg;

    _arg.in_partition = in_partition;
    _arg.off_set      = off_set;
    _arg.out_buf      = out_buf;
    _arg.in_buf_len   = in_buf_len;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_READ, (void*)&_arg);

}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    hal_flash_enable_secure_syscall_arg_t _arg;

    _arg.partition = partition;
    _arg.off_set   = off_set;
    _arg.size      = size;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_ENABLE_SECURE, (void*)&_arg);
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    hal_flash_enable_secure_syscall_arg_t _arg;

    _arg.partition = partition;
    _arg.off_set   = off_set;
    _arg.size      = size;

    return (int32_t)SYSCALL(SYS_HAL_FLASH_DIS_SECURE, (void*)&_arg);
}

