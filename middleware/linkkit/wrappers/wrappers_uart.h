#ifndef _WRAPPERS_UART_H_
#define _WRAPPERS_UART_H_

#include "infra_types.h"
#include "infra_defs.h"



int32_t HAL_AT_Uart_Init(uart_dev_t *uart);

int32_t HAL_AT_Uart_Deinit(uart_dev_t *uart);

int32_t HAL_AT_Uart_Send(uart_dev_t *uart, const void *data,
                         uint32_t size, uint32_t timeout);

int32_t HAL_AT_Uart_Recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout);


#endif

