/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "aos/hal/i2c.h"
#include "hal_i2c_syscall_arg.h"

int32_t sys_hal_i2c_init_stub(void *arg)
{
    hal_i2c_init_syscall_arg_t *_arg = arg;

    return hal_i2c_init(_arg->i2c);
}

int32_t sys_hal_i2c_master_send_stub(void *arg)
{
    hal_i2c_master_send_syscall_arg_t *_arg = arg;

    return hal_i2c_master_send(_arg->i2c, _arg->dev_addr, _arg->data,
                               _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_master_recv_stub(void *arg)
{
    hal_i2c_master_recv_syscall_arg_t *_arg = arg;

    return hal_i2c_master_recv(_arg->i2c, _arg->dev_addr, _arg->data,
                               _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_slave_send_stub(void *arg)
{
    hal_i2c_slave_send_syscall_arg_t *_arg = arg;

    return hal_i2c_slave_send(_arg->i2c, _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_slave_recv_stub(void *arg)
{
    hal_i2c_slave_recv_syscall_arg_t *_arg = arg;

    return hal_i2c_slave_recv(_arg->i2c, _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_mem_write_stub(void *arg)
{
hal_i2c_mem_write_syscall_arg_t *_arg = arg;

    return hal_i2c_mem_write(_arg->i2c, _arg->dev_addr, _arg->mem_addr, _arg->mem_addr_size,
                             _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_mem_read_stub(void *arg)
{
    hal_i2c_mem_read_syscall_arg_t *_arg = arg;

    return hal_i2c_mem_read(_arg->i2c, _arg->dev_addr, _arg->mem_addr, _arg->mem_addr_size,
                            _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_i2c_finalize_stub(void *arg)
{
    hal_i2c_finalize_syscall_arg_t *_arg = arg;

    return hal_i2c_finalize(_arg->i2c);
}

