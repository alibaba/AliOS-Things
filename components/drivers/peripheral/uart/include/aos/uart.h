/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_UART_H
#define AOS_UART_H

#include <aos/tty.h>

/**
 * @defgroup uart_api UART
 * @ingroup driver_api
 * @brief AOS API for UART.
 * @{
 */

typedef aos_tty_ref_t aos_uart_ref_t;

typedef enum {
    AOS_UART_PARITY_NONE,
    AOS_UART_PARITY_ODD,
    AOS_UART_PARITY_EVEN,
} aos_uart_parity_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get a UART device.
 * @param   ref UART ref to operate.
 * @param   id  UART device ID.
 * @return  0: on success; < 0: on failure.
 */
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

/**
 * @brief   Get a UART device and set its baudrate and parity.
 * @param   ref         UART ref to operate.
 * @param   id          UART device ID.
 * @param   baudrate    Baudrate.
 * @param   parity      Parity.
 * @return  0: on success; < 0: on failure.
 */
static inline
aos_status_t aos_uart_get_attributed(aos_uart_ref_t *ref, uint32_t id, uint32_t baudrate, aos_uart_parity_t parity)
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

/**
 * @brief   Get a UART device and set its baudrate and parity.
 * @param   ref         UART ref to operate.
 * @param   id          UART device ID.
 * @param   baudrate    Baudrate.
 * @param   parity      Parity.
 * @return  0: on success; < 0: on failure.
 */
static inline
aos_status_t aos_uart_get_with_attr(aos_uart_ref_t *ref, uint32_t id, uint32_t baudrate, aos_uart_parity_t parity)
{
    return aos_uart_get_attributed(ref, id, baudrate, parity);
}

/**
 * @brief   Release a UART device.
 * @param   ref UART ref to operate.
 * @return  None.
 */
static inline void aos_uart_put(aos_uart_ref_t *ref)
{
    aos_tty_put(ref);
}

/**
 * @brief   Get the parameters associated with a UART device.
 * @param   ref     UART ref to operate.
 * @param   termios POSIX-compliant termios structure.
 * @return  0: on success; < 0: on failure.
 */
static inline aos_status_t aos_uart_get_attr(aos_uart_ref_t *ref, struct termios *termios)
{
    return aos_tty_get_attr(ref, termios);
}

/**
 * @brief   Set the parameters associated with a UART device.
 * @param   ref                 UART ref to operate.
 * @param   optional_actions    Specifie when the changes take effect.
 * @param   termios             POSIX-compliant termios structure.
 * @return  0: on success; < 0: on failure.
 */
static inline aos_status_t aos_uart_set_attr(aos_uart_ref_t *ref, int optional_actions, const struct termios *termios)
{
    return aos_tty_set_attr(ref, optional_actions, termios);
}

/**
 * @brief   Read data from a UART device.
 * @param   ref     UART ref to operate.
 * @param   buf     Data buffer.
 * @param   count   Attempt to read up to count bytes.
 * @param   timeout Timeout in milliseconds.
 * @return  > 0: the number of bytes read; < 0: on failure.
 */
static inline ssize_t aos_uart_read(aos_uart_ref_t *ref, void *buf, size_t count, uint32_t timeout)
{
    return aos_tty_read(ref, buf, count, timeout);
}

/**
 * @brief   Write data to a UART device.
 * @param   ref     UART ref to operate.
 * @param   buf     Data buffer.
 * @param   count   Attempt to write up to count bytes.
 * @param   timeout Timeout in milliseconds.
 * @return  > 0: the number of bytes written; < 0: on failure.
 */
static inline ssize_t aos_uart_write(aos_uart_ref_t *ref, const void *buf, size_t count, uint32_t timeout)
{
    return aos_tty_write(ref, buf, count, timeout);
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* AOS_UART_H */
