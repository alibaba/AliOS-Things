/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_NOR_SYSCALL > 0)

#include <stdint.h>
#include "syscall.h"
#include "hal_nor_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_nor_init(nor_dev_t *nor)
{
    hal_nor_init_syscall_arg_t _arg;

    _arg.nor = nor;

    return (int32_t)SYSCALL(SYS_HAL_NOR_INIT, (void*)&_arg);
}

int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    hal_nor_read_syscall_arg_t _arg;

    _arg.nor  = nor;
    _arg.addr = addr;
    _arg.data = data;
    _arg.len  = len;

    return (int32_t)SYSCALL(SYS_HAL_NOR_READ, (void*)&_arg);

}

int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    hal_nor_write_syscall_arg_t _arg;

    _arg.nor  = nor;
    _arg.addr = addr;
    _arg.data = data;
    _arg.len  = len;

    return (int32_t)SYSCALL(SYS_HAL_NOR_WRITE, (void*)&_arg);
}

int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count)
{
    hal_nor_erase_block_syscall_arg_t _arg;

    _arg.nor         = nor;
    _arg.addr        = addr;
    _arg.block_count = block_count;

    return (int32_t)SYSCALL(SYS_HAL_NOR_ERASE_BLOCK, (void*)&_arg);
}

int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr)
{
    hal_nor_erase_chip_syscall_arg_t _arg;

    _arg.nor  = nor;
    _arg.addr = addr;

    return (int32_t)SYSCALL(SYS_HAL_NOR_ERASE_CHIP, (void*)&_arg);
}

int32_t hal_nor_finalize(nor_dev_t *nor)
{
    hal_nor_finalize_syscall_arg_t _arg;

    _arg.nor = nor;

    return (int32_t)SYSCALL(SYS_HAL_NOR_FINALIZE, (void*)&_arg);
}

#endif /* RHINO_CONFIG_HAL_NOR_SYSCALL */

