/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_SPI_KSYSCALL_H
#define HAL_SPI_KSYSCALL_H

int32_t sys_hal_spi_init_stub(void *arg);

int32_t sys_hal_spi_send_stub(void *arg);

int32_t sys_hal_spi_recv_stub(void *arg);

int32_t sys_hal_spi_send_recv_stub(void *arg);

int32_t sys_hal_spi_finalize_stub(void *arg);

#endif /* HAL_SPI_KSYSCALL_H */

