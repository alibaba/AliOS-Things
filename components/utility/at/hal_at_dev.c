/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "at_internal.h"
#include "aos/hal/uart.h"

/*-----------------------------UART operates------------------------------------------*/
static int at_init_uart(void *dev)
{
    int ret = 0;

    ret = hal_uart_init((uart_dev_t *)dev);

	return ret;
}

static int at_recvfrom_uart(void *dev, void *data, uint32_t expect_size,
                            uint32_t *recv_size, uint32_t timeout)
{
    int ret = 0;

    ret = hal_uart_recv_II((uart_dev_t *)dev, data, expect_size, recv_size, timeout);

    return ret;
}

static int at_sendto_uart(void *dev, void *data, uint32_t size,
                          uint32_t timeout)
{
    int ret = 0;

    ret = hal_uart_send((uart_dev_t *)dev, data, size, timeout);

    return ret;
}

static int at_deinit_uart(void *dev)
{
    int ret = 0;

    ret = hal_uart_finalize((uart_dev_t *)dev);

    return ret;
}

at_dev_ops_t at_uart_ops = {
	.type   = AT_DEV_UART,
    .init   = at_init_uart,
    .recv   = at_recvfrom_uart,
    .send   = at_sendto_uart,
    .deinit = at_deinit_uart,
};

/*-----------------------------Add other devices' operates below-----------------------------------------*/

