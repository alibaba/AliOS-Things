/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_I2C_KSYSCALL_H
#define HAL_I2C_KSYSCALL_H

int32_t sys_hal_i2c_init_stub(void *arg);

int32_t sys_hal_i2c_master_send_stub(void *arg);

int32_t sys_hal_i2c_master_recv_stub(void *arg);

int32_t sys_hal_i2c_slave_send_stub(void *arg);

int32_t sys_hal_i2c_slave_recv_stub(void *arg);

int32_t sys_hal_i2c_mem_write_stub(void *arg);

int32_t sys_hal_i2c_mem_read_stub(void *arg);

int32_t sys_hal_i2c_finalize_stub(void *arg);

#endif /* HAL_I2C_KSYSCALL_H */

