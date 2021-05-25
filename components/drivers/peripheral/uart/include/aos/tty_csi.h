/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_TTY_CSI_H
#define AOS_TTY_CSI_H

#include <aos/tty.h>
#include <drv/uart.h>

typedef struct {
    aos_tty_t tty;
    csi_uart_t csi_uart;
    tcflag_t mode;
    aos_sem_t rx_sem;
    aos_sem_t tx_sem;
    aos_task_t rx_task;
    aos_task_t tx_task;
} aos_tty_csi_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_tty_csi_register(aos_tty_csi_t *tty_csi);
aos_status_t aos_tty_csi_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* AOS_TTY_CSI_H */
