/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <Windows.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>

#include <signal.h>
#include "hal/soc/soc.h"



int32_t hal_uart_init(uart_dev_t *uart)
{
    
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
#ifdef _WIN32 //Not Support UART now
	printf("###error, UART Send %d may fail in Windows\n",uart->port);
#endif
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
#ifdef _WIN32 //Not Support UART now

	while (1==1)
	{		
		Sleep(5000);
	}
#endif
    return 0;
}


