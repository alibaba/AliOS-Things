/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "aos/hal/spi.h"
#include "hal_spi_syscall_arg.h"

int32_t sys_hal_spi_init_stub(void *arg)
{
    hal_spi_init_syscall_arg_t *_arg = arg;

    return hal_spi_init(_arg->spi);
}

int32_t sys_hal_spi_send_stub(void *arg)
{
    hal_spi_send_syscall_arg_t *_arg = arg;

    return hal_spi_send(_arg->spi, _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_spi_recv_stub(void *arg)
{
    hal_spi_recv_syscall_arg_t *_arg = arg;

    return hal_spi_recv(_arg->spi, _arg->data, _arg->size, _arg->timeout);
}

int32_t sys_hal_spi_send_recv_stub(void *arg)
{
    hal_spi_send_recv_syscall_arg_t *_arg = arg;

    return hal_spi_send_recv(_arg->spi, _arg->tx_data, _arg->rx_data,
                             _arg->size, _arg->timeout);
}

int32_t sys_hal_spi_finalize_stub(void *arg)
{
    hal_spi_finalize_syscall_arg_t *_arg = arg;

    return hal_spi_finalize(_arg->spi);
}

