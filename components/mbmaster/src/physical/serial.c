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

#if ((MBMASTER_CONFIG_RTU_ENABLED > 0) || (MBMASTER_CONFIG_ASCII_ENABLED > 0))
static int uart_mb = -1;

mb_status_t mb_serial_init(mb_handler_t *handler, uint8_t port, uint32_t baud_rate, uint8_t data_width, mb_parity_t parity)
{
    const char *fmt = "/dev/ttyUART%u";
    char dev_name[32];
    unsigned long flags = 0;

    snprintf(dev_name, sizeof(dev_name) - 1, fmt, (unsigned)port);
    dev_name[sizeof(dev_name) - 1] = '\0';
    uart_mb = open(dev_name, 0);
    if (uart_mb < 0)
        return MB_SERIAL_INIT_FAILED;

    switch (baud_rate) {
        case 50:
            flags |= B50;
            break;
        case 75:
            flags |= B75;
            break;
        case 110:
            flags |= B110;
            break;
        case 134:
            flags |= B134;
            break;
        case 150:
            flags |= B150;
            break;
        case 200:
            flags |= B200;
            break;
        case 300:
            flags |= B300;
            break;
        case 600:
            flags |= B600;
            break;
        case 1200:
            flags |= B1200;
            break;
        case 1800:
            flags |= B1800;
            break;
        case 2400:
            flags |= B2400;
            break;
        case 4800:
            flags |= B4800;
            break;
        case 9600:
            flags |= B9600;
            break;
        case 19200:
            flags |= B19200;
            break;
        case 38400:
            flags |= B38400;
            break;
        case 57600:
            flags |= B57600;
            break;
        case 115200:
            flags |= B115200;
            break;
        case 230400:
            flags |= B230400;
            break;
        case 460800:
            flags |= B460800;
            break;
        case 500000:
            flags |= B500000;
            break;
        case 576000:
            flags |= B576000;
            break;
        case 921600:
            flags |= B921600;
            break;
        case 1000000:
            flags |= B1000000;
            break;
        case 1152000:
            flags |= B1152000;
            break;
        case 1500000:
            flags |= B1500000;
            break;
        case 2000000:
            flags |= B2000000;
            break;
        case 2500000:
            flags |= B2500000;
            break;
        case 3000000:
            flags |= B3000000;
            break;
        case 3500000:
            flags |= B3500000;
            break;
        case 4000000:
            flags |= B4000000;
            break;
        default:
            flags |= B9600;
            break;
    }

    switch (data_width) {
        case 5:
            flags |= CS5;
            break;
        case 6:
            flags |= CS6;
            break;
        case 7:
            flags |= CS7;
            break;
        case 8:
            flags |= CS8;
            break;
        default:
            flags |= CS8;
            break;
    }

    switch (parity) {
        case MB_PAR_NONE:
            break;
        case MB_PAR_ODD:
            flags |= PARODD;
            break;
        case MB_PAR_EVEN:
            flags |= PARENB;
            break;
        default:
            break;
    }

    if (ioctl(uart_mb, IOC_UART_SET_CFLAG, flags) < 0)
        return MB_SERIAL_INIT_FAILED;

    handler->private = &uart_mb;

    return MB_SUCCESS;
}

mb_status_t mb_serial_finalize(mb_handler_t *handler)
{
    close(*(int *)handler->private);
    *(int *)handler->private = -1;

    return MB_SUCCESS;
}

mb_status_t mb_serial_frame_send(mb_handler_t *handler, uint32_t timeout)
{
    int fd = *(int *)handler->private;

    if (write(fd, handler->mb_frame_buff, handler->mb_frame_length) < 0)
        return MB_FRAME_SEND_ERR;

    return MB_SUCCESS;
}

mb_status_t mb_serial_frame_recv(mb_handler_t *handler)
{
    int fd = *(int *)handler->private;
    mb_status_t ret = MB_SUCCESS;

    LOGD(MODBUS_MOUDLE, "waiting %ld ms for rev frame", handler->respond_timeout);

    while (1) {
        struct pollfd poll_fds[1];
        int timeout;
        ssize_t r;

        r = read(fd, handler->mb_frame_buff, ADU_BUF_MAX_LENGTH);

        if (r < 0) {
            handler->mb_frame_length = 0;
            ret = MB_SLAVE_NO_RESPOND;
            break;
        }

        if (r > 0) {
            handler->mb_frame_length = (uint32_t)r;
            ret = MB_SUCCESS;
            break;
        }

        memset(poll_fds, 0, sizeof(poll_fds));
        poll_fds[0].fd = fd;
        poll_fds[0].events = POLLIN;

        if (handler->respond_timeout == AOS_WAIT_FOREVER)
            timeout = -1;
        else if (handler->respond_timeout > INT_MAX)
            timeout = INT_MAX;
        else
            timeout = (int)handler->respond_timeout;

        if (poll(poll_fds, 1, timeout) <= 0) {
            handler->mb_frame_length = 0;
            ret = MB_SLAVE_NO_RESPOND;
            break;
        }

        if ((poll_fds[0].revents & POLLERR) ||
            !(poll_fds[0].revents & POLLIN)) {
            handler->mb_frame_length = 0;
            ret = MB_SLAVE_NO_RESPOND;
            break;
        }
    }

    return ret;
}

#endif /* (MBM_RTU_ENABLED > 0) || (MBM_ASCII_ENABLED > 0) */
