/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <limits.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <aos/kernel.h>
#include <vfsdev/uart_dev.h>
#include <mbmaster.h>
#include "serial.h"
#include "aos_hal_uart.h"

#if ((MBMASTER_CONFIG_RTU_ENABLED > 0) || (MBMASTER_CONFIG_ASCII_ENABLED > 0))
static uart_dev_t uart_dev = { 0 };

mb_status_t mb_serial_init(mb_handler_t *handler, uint8_t port, uint32_t baud_rate,
                            uint8_t data_width, mb_parity_t parity)
{
    hal_uart_data_width_t enum_data_with = DATA_WIDTH_8BIT;
    switch (data_width) {
        case 5:
            enum_data_with = DATA_WIDTH_5BIT;
            break;
        case 6:
            enum_data_with = DATA_WIDTH_6BIT;
            break;
        case 7:
            enum_data_with = DATA_WIDTH_7BIT;
            break;
        case 8:
        default:
            enum_data_with = DATA_WIDTH_8BIT;
            break;
    }

    uart_dev.port = port;
    uart_dev.config.baud_rate = baud_rate;
    uart_dev.config.data_width = enum_data_with;
    uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_dev.config.mode = MODE_TX_RX;
    uart_dev.config.parity = parity;
    uart_dev.config.stop_bits = STOP_BITS_1;
    int ret = aos_hal_uart_init(&uart_dev);
    if (ret != 0) {
        return MB_SERIAL_INIT_FAILED;
    } else {
        handler->private = (void *)&uart_dev;
        return MB_SUCCESS;
    }
}

mb_status_t mb_serial_finalize(mb_handler_t *handler)
{
    aos_hal_uart_finalize((uart_dev_t *)handler->private);
    handler->private = NULL;

    return MB_SUCCESS;
}

mb_status_t mb_serial_frame_send(mb_handler_t *handler, uint32_t timeout)
{
    uart_dev_t *dev = (uart_dev_t *)(handler->private);

    if (aos_hal_uart_send(dev, handler->mb_frame_buff,
                          handler->mb_frame_length, timeout) != 0)
        return MB_FRAME_SEND_ERR;

    return MB_SUCCESS;
}

mb_status_t mb_serial_frame_recv(mb_handler_t *handler)
{
    uart_dev_t *dev = (uart_dev_t *)(handler->private);

    mb_status_t ret = MB_SUCCESS;

    uint32_t recv_size = 0;
    int32_t r = aos_hal_uart_recv_II(dev, handler->mb_frame_buff,
                ADU_BUF_MAX_LENGTH, &recv_size, handler->respond_timeout);
    if (r < 0) {
        handler->mb_frame_length = 0;
        ret = MB_SLAVE_NO_RESPOND;
        return ret;
    }

    if (recv_size == 0) {
        handler->mb_frame_length = 0;
        ret = MB_RESPOND_TIMEOUT;
        return ret;
    }

    handler->mb_frame_length = recv_size;
    return ret;
}

#endif /* (MBM_RTU_ENABLED > 0) || (MBM_ASCII_ENABLED > 0) */
