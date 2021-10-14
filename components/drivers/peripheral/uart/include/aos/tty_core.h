/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_TTY_CORE_H
#define AOS_TTY_CORE_H

#include <aos/device_core.h>
#include <aos/tty.h>

#define AOS_TTY_F_UNIQUE_REF    ((uint32_t)1 << 0)

#ifndef AOS_TTY_RX_BUF_SIZE
#define AOS_TTY_RX_BUF_SIZE     1024
#endif
#ifndef AOS_TTY_TX_BUF_SIZE
#define AOS_TTY_TX_BUF_SIZE     1024
#endif

struct aos_tty_ops;

typedef struct {
    aos_dev_t dev;

    /* must be initialized before registration */
    const struct aos_tty_ops *ops;
    uint32_t flags;

    struct termios termios;
    uint32_t status;
    aos_spinlock_t lock;
    aos_event_t event;
    size_t rx_buf_head;
    size_t rx_buf_tail;
    uint8_t rx_buf[AOS_TTY_RX_BUF_SIZE];
    size_t tx_buf_head;
    size_t tx_buf_tail;
    uint8_t tx_buf[AOS_TTY_TX_BUF_SIZE];
} aos_tty_t;

typedef struct aos_tty_ops {
    void (*unregister)(aos_tty_t *);
    aos_status_t (*startup)(aos_tty_t *);
    void (*shutdown)(aos_tty_t *);
    aos_status_t (*set_attr)(aos_tty_t *);
    void (*enable_rx)(aos_tty_t *);
    void (*disable_rx)(aos_tty_t *);
    void (*start_tx)(aos_tty_t *);
    void (*stop_tx)(aos_tty_t *);
} aos_tty_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_tty_register(aos_tty_t *tty);
aos_status_t aos_tty_unregister(uint32_t id);

size_t aos_tty_rx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count);
size_t aos_tty_tx_buffer_consume(aos_tty_t *tty, void *buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* AOS_TTY_CORE_H */
