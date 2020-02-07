/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "serial.h"

#if ((MBMASTER_CONFIG_RTU_ENABLED > 0) || (MBMASTER_CONFIG_ASCII_ENABLED > 0))
static uart_dev_t uart_mb;

mb_status_t mb_serial_init(mb_handler_t *handler, uint8_t port, uint32_t baud_rate, uint8_t data_width, mb_parity_t parity)
{
    int32_t ret;

    switch(data_width) {
        case 7:
            uart_mb.config.data_width = DATA_WIDTH_7BIT;
            break;
        case 8:
            uart_mb.config.data_width = DATA_WIDTH_8BIT;
            break;
        case 9:
            uart_mb.config.data_width = DATA_WIDTH_9BIT;
            break;
    }

    switch(parity) {
        case MB_PAR_NONE:
            uart_mb.config.parity = NO_PARITY;
            break;
        case MB_PAR_ODD:
            uart_mb.config.parity = ODD_PARITY;
            break;
        case MB_PAR_EVEN:
            uart_mb.config.parity = EVEN_PARITY;
            break;
    }

    uart_mb.port                = port;
    uart_mb.config.baud_rate    = baud_rate;
    uart_mb.config.mode         = MODE_TX_RX;
    uart_mb.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_mb.config.stop_bits    = STOP_BITS_1;

    ret = hal_uart_init(&uart_mb);
    if(ret != 0) {
        return MB_SERIAL_INIT_FAILED;
    }

    handler->private = &uart_mb;

    return MB_SUCCESS;
}

mb_status_t mb_serial_finalize(mb_handler_t *handler)
{
    int32_t     ret;
    uart_dev_t *uart = (uart_dev_t *)handler->private;
    ret = hal_uart_finalize(uart);
    if (ret == 0) {
        return MB_SUCCESS;
    }
    return MB_SERIAL_UNINIT_FAILED;
}

mb_status_t mb_serial_frame_send(mb_handler_t *handler, uint32_t timeout)
{
    int32_t     ret;
    uart_dev_t *uart = (uart_dev_t *)handler->private;

    ret = hal_uart_send(uart, handler->mb_frame_buff, handler->mb_frame_length, timeout);
    if (ret == 0) {
        return MB_SUCCESS;
    }

    return MB_FRAME_SEND_ERR;
}

mb_status_t mb_serial_frame_recv(mb_handler_t *handler)
{
    int32_t ret;

    uart_dev_t *uart = (uart_dev_t *)handler->private;
    LOGD(MODBUS_MOUDLE, "waiting %ld ms for rev frame", handler->respond_timeout);
    ret = hal_uart_recv_II(uart, handler->mb_frame_buff, ADU_BUF_MAX_LENGTH, &handler->mb_frame_length, handler->respond_timeout);
    if (ret == 0) {
        return MB_SUCCESS;
    }

    return MB_SLAVE_NO_RESPOND;
}

#endif /* (MBM_RTU_ENABLED > 0) || (MBM_ASCII_ENABLED > 0) */
