/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_I2C_SYSCALL > 0)

#include <stdint.h>
#include "syscall.h"
#include "hal_i2c_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    hal_i2c_init_syscall_arg_t _arg;

    _arg.i2c = i2c;

    return (int32_t)SYSCALL(SYS_HAL_I2C_INIT, (void*)&_arg);
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    hal_i2c_master_send_syscall_arg_t _arg;

    _arg.i2c      = i2c;
    _arg.dev_addr = dev_addr;
    _arg.data     = data;
    _arg.size     = size;
    _arg.timeout  = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_MASTER_SEND, (void*)&_arg);
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    hal_i2c_master_recv_syscall_arg_t _arg;

    _arg.i2c      = i2c;
    _arg.dev_addr = dev_addr;
    _arg.data     = data;
    _arg.size     = size;
    _arg.timeout  = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_MASTER_RECV, (void*)&_arg);
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    hal_i2c_slave_send_syscall_arg_t _arg;

    _arg.i2c     = i2c;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_SLAVE_SEND, (void*)&_arg);
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    hal_i2c_slave_recv_syscall_arg_t _arg;

    _arg.i2c     = i2c;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_SLAVE_RECV, (void*)&_arg);
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    hal_i2c_mem_write_syscall_arg_t _arg;

    _arg.i2c           = i2c;
    _arg.dev_addr      = dev_addr;
    _arg.mem_addr      = mem_addr;
    _arg.mem_addr_size = mem_addr_size;
    _arg.data          = data;
    _arg.size          = size;
    _arg.timeout       = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_MEM_WRITE, (void*)&_arg);
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    hal_i2c_mem_read_syscall_arg_t _arg;

    _arg.i2c           = i2c;
    _arg.dev_addr      = dev_addr;
    _arg.mem_addr      = mem_addr;
    _arg.mem_addr_size = mem_addr_size;
    _arg.data          = data;
    _arg.size          = size;
    _arg.timeout       = timeout;

    return (int32_t)SYSCALL(SYS_HAL_I2C_MEM_READ, (void*)&_arg);
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    hal_i2c_finalize_syscall_arg_t _arg;

    _arg.i2c = i2c;

    return (int32_t)SYSCALL(SYS_HAL_I2C_FINALIZE, (void*)&_arg);
}

#endif /* RHINO_CONFIG_HAL_I2C_SYSCALL */

