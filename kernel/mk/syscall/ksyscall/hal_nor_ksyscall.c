/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_NOR_SYSCALL > 0)

#include <stdint.h>
#include "aos/hal/nor.h"
#include "hal_nor_syscall_arg.h"

int32_t sys_hal_nor_init_stub(void *arg)
{
    hal_nor_init_syscall_arg_t *_arg = arg;

    return hal_nor_init(_arg->nor);
}

int32_t sys_hal_nor_read_stub(void *arg)
{
    hal_nor_read_syscall_arg_t *_arg = arg;

    return hal_nor_read(_arg->nor, _arg->addr, _arg->data, _arg->len);
}
int32_t sys_hal_nor_write_stub(void *arg)
{
    hal_nor_write_syscall_arg_t *_arg = arg;

    return hal_nor_write(_arg->nor, _arg->addr, _arg->data, _arg->len);
}
int32_t sys_hal_nor_erase_block_stub(void *arg)
{
    hal_nor_erase_block_syscall_arg_t *_arg = arg;

    return hal_nor_erase_block(_arg->nor, _arg->addr, _arg->block_count);
}
int32_t sys_hal_nor_erase_chip_stub(void *arg)
{
    hal_nor_erase_chip_syscall_arg_t *_arg = arg;

    return hal_nor_erase_chip(_arg->nor, _arg->addr);
}

int32_t sys_hal_nor_finalize_stub(void *arg)
{
    hal_nor_finalize_syscall_arg_t *_arg = arg;

    return hal_nor_finalize(_arg->nor);
}

#endif /* RHINO_CONFIG_HAL_NOR_SYSCALL */

