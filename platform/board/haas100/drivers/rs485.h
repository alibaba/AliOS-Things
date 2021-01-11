/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef RS485_H
#define RS485_H


/* RS485 Macro definition */
#define HAAS_RS485_UART_IDX  1
#define HAAS_RS485_BD_DFT    19200
#define HAAS_RS485_DFT_CFG   NULL

int32_t haas_rs485_init(uart_dev_t *uart_dev);
int32_t haas_rs485_send(const void *data, uint32_t size, uint32_t timeout);
int32_t haas_rs485_recv(void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout);

int32_t haas_rs485_deinit(uart_dev_t *uart_dev);
#endif
