/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_I2C_SYSCALL_ARG_H
#define HAL_I2C_SYSCALL_ARG_H

#include "aos/hal/i2c.h"

typedef struct {
    i2c_dev_t *i2c;
} hal_i2c_init_syscall_arg_t;

typedef struct {
    i2c_dev_t     *i2c;
    uint16_t       dev_addr;
    const uint8_t *data;
    uint16_t       size;
    uint32_t       timeout;
} hal_i2c_master_send_syscall_arg_t;

typedef struct {
    i2c_dev_t *i2c;
    uint16_t   dev_addr;
    uint8_t   *data;
    uint16_t   size;
    uint32_t   timeout;
} hal_i2c_master_recv_syscall_arg_t;

typedef struct {
    i2c_dev_t     *i2c;
    const uint8_t *data;
    uint16_t       size;
    uint32_t       timeout;
} hal_i2c_slave_send_syscall_arg_t;

typedef struct {
    i2c_dev_t *i2c;
    uint8_t   *data;
    uint16_t   size;
    uint32_t   timeout;
} hal_i2c_slave_recv_syscall_arg_t;

typedef struct {
    i2c_dev_t     *i2c;
    uint16_t       dev_addr;
    uint16_t       mem_addr;
    uint16_t       mem_addr_size;
    const uint8_t *data;
    uint16_t       size;
    uint32_t       timeout;
} hal_i2c_mem_write_syscall_arg_t;

typedef struct {
    i2c_dev_t *i2c;
    uint16_t  dev_addr;
    uint16_t  mem_addr;
    uint16_t  mem_addr_size;
    uint8_t  *data;
    uint16_t  size;
    uint32_t  timeout;
} hal_i2c_mem_read_syscall_arg_t;

typedef struct {
    i2c_dev_t *i2c;
} hal_i2c_finalize_syscall_arg_t;

#endif /* HAL_I2C_SYSCALL_ARG_H */

