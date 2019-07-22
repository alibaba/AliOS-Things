/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_UART_SYSCALL_ARG_H
#define HAL_UART_SYSCALL_ARG_H

#include "aos/hal/spi.h"

typedef struct {
    spi_dev_t *spi;
} hal_spi_init_syscall_arg_t;

typedef struct {
    spi_dev_t     *spi;
    const uint8_t *data;
    uint16_t       size;
    uint32_t       timeout;
} hal_spi_send_syscall_arg_t;

typedef struct {
    spi_dev_t *spi;
    uint8_t   *data;
    uint16_t   size;
    uint32_t   timeout;
} hal_spi_recv_syscall_arg_t;

typedef struct {
    spi_dev_t *spi;
    uint8_t   *tx_data;
    uint8_t   *rx_data;
    uint16_t   size;
    uint32_t   timeout;
} hal_spi_send_recv_syscall_arg_t;

typedef struct {
    spi_dev_t *spi;
} hal_spi_finalize_syscall_arg_t;

#endif /* HAL_SPI_SYSCALL_ARG_H */

