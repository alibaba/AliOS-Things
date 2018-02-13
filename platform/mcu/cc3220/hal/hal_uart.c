/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include <hal/soc/uart.h>

#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_types.h>

#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/uart.h>

uart_dev_t uart_0 = {
  .port=0,
};


void message(const char *str){
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(UARTA0_BASE,*str++);
        }
    }
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	if(uart->port == 0)
		message(data);

    	return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int ttl_len = 0;
    char *buf = data;
    while (1) {
        char c;
        int ret = 0;
        if(uart->port == 0) {
            c = MAP_UARTCharGet(UARTA0_BASE);
            if(c)
                ret = 1;
        }
        if (ret <= 0)
            break;

        *buf++ = c;
        ttl_len ++;
        if (ttl_len >= expect_size)
            break;
    }

    if (recv_size)
        *recv_size = ttl_len;

    return ttl_len > 0 ? 0 : -1;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    //uart_driver_install(uart->port, 256, 0, 0, NULL, 0);
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}


