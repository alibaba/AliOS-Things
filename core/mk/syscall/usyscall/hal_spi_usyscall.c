/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_spi_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_spi_init(spi_dev_t *spi)
{
     hal_spi_init_syscall_arg_t _arg;

    _arg.spi = spi;

    return (int32_t)SYSCALL(SYS_HAL_SPI_INIT, (void*)&_arg);
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
     hal_spi_send_syscall_arg_t _arg;

    _arg.spi     = spi;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_SPI_SEND, (void*)&_arg);
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
     hal_spi_recv_syscall_arg_t _arg;

    _arg.spi     = spi;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_SPI_RECV, (void*)&_arg);
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                             uint16_t size, uint32_t timeout)
{
     hal_spi_send_recv_syscall_arg_t _arg;

    _arg.spi     = spi;
    _arg.tx_data = tx_data;
    _arg.rx_data = rx_data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return (int32_t)SYSCALL(SYS_HAL_SPI_SEND_RECV, (void*)&_arg);
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
     hal_spi_finalize_syscall_arg_t _arg;

    _arg.spi = spi;

    return (int32_t)SYSCALL(SYS_HAL_SPI_FINALIZE, (void*)&_arg);
}

