/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_UART_H
#define AOS_UART_H

#include <aos/tty.h>

typedef aos_tty_ref_t aos_uart_ref_t;

typedef enum {
    AOS_UART_PARITY_NONE,
    AOS_UART_PARITY_ODD,
    AOS_UART_PARITY_EVEN,
} aos_uart_parity_t;

#define aos_uart_put            aos_tty_put
#define aos_uart_get_attr       aos_tty_get_attr
#define aos_uart_set_attr       aos_tty_set_attr
#define aos_uart_read           aos_tty_read
#define aos_uart_write          aos_tty_write

#ifdef __cplusplus
extern "C" {
#endif

static inline aos_status_t aos_uart_get(aos_uart_ref_t *ref, uint32_t id)
{
    struct termios termios;
    aos_status_t ret;

    ret = aos_tty_get(ref, id);
    if (ret)
        return ret;

    ret = aos_tty_get_attr(ref, &termios);
    if (ret) {
        aos_tty_put(ref);
        return ret;
    }

    cfmakeraw(&termios);
    termios.c_cflag |= CREAD;
    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 0;

    ret = aos_tty_set_attr(ref, TCSANOW, &termios);
    if (ret) {
        aos_tty_put(ref);
        return ret;
    }

    return 0;
}

static inline aos_status_t
aos_uart_get_with_attr(aos_uart_ref_t *ref, uint32_t id, uint32_t baudrate, aos_uart_parity_t parity)
{
    struct termios termios;
    aos_status_t ret;

    ret = aos_tty_get(ref, id);
    if (ret)
        return ret;

    ret = aos_tty_get_attr(ref, &termios);
    if (ret) {
        aos_tty_put(ref);
        return ret;
    }

    cfmakeraw(&termios);
    termios.c_cflag |= CREAD;
    termios.c_cflag &= ~(PARENB | PARODD);
    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 0;

    if (cfsetspeed(&termios, baudrate)) {
        aos_tty_put(ref);
        return -EINVAL;
    }

    switch (parity) {
    case AOS_UART_PARITY_NONE:
        break;
    case AOS_UART_PARITY_ODD:
        termios.c_cflag |= PARENB | PARODD;
        break;
    case AOS_UART_PARITY_EVEN:
        termios.c_cflag |= PARENB;
        break;
    default:
        aos_tty_put(ref);
        return -EINVAL;
    }

    ret = aos_tty_set_attr(ref, TCSANOW, &termios);
    if (ret) {
        aos_tty_put(ref);
        return ret;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_UART_H */
