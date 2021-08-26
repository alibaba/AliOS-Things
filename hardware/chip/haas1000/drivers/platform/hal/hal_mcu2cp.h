/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_MCU2CP_H__
#define __HAL_MCU2CP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

enum HAL_MCU2CP_ID_T {
    HAL_MCU2CP_ID_0,
    HAL_MCU2CP_ID_1,

    HAL_MCU2CP_ID_QTY
};

enum HAL_MCU2CP_MSG_TYPE_T {
    HAL_MCU2CP_MSG_TYPE_0,
    HAL_MCU2CP_MSG_TYPE_1,

    HAL_MCU2CP_MSG_TYPE_QTY
};

typedef unsigned int (*HAL_MCU2CP_RX_IRQ_HANDLER)(const unsigned char *data, unsigned int len);
typedef void (*HAL_MCU2CP_TX_IRQ_HANDLER)(const unsigned char *data, unsigned int len);

int hal_mcu2cp_open_mcu(enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_MSG_TYPE_T type,
                        HAL_MCU2CP_RX_IRQ_HANDLER rxhandler, HAL_MCU2CP_TX_IRQ_HANDLER txhandler, bool rx_flowctrl);
int hal_mcu2cp_open_cp (enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_MSG_TYPE_T type,
                        HAL_MCU2CP_RX_IRQ_HANDLER rxhandler, HAL_MCU2CP_TX_IRQ_HANDLER txhandler, bool rx_flowctrl);

int hal_mcu2cp_close_mcu(enum HAL_MCU2CP_ID_T id,enum HAL_MCU2CP_MSG_TYPE_T type);
int hal_mcu2cp_close_cp (enum HAL_MCU2CP_ID_T id,enum HAL_MCU2CP_MSG_TYPE_T type);

int hal_mcu2cp_start_recv_mcu(enum HAL_MCU2CP_ID_T id);
int hal_mcu2cp_start_recv_cp (enum HAL_MCU2CP_ID_T id);

int hal_mcu2cp_stop_recv_mcu(enum HAL_MCU2CP_ID_T id);
int hal_mcu2cp_stop_recv_cp (enum HAL_MCU2CP_ID_T id);

int hal_mcu2cp_send_mcu(enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_MSG_TYPE_T type,
                    const unsigned char *data, unsigned int len);
int hal_mcu2cp_send_cp (enum HAL_MCU2CP_ID_T id, enum HAL_MCU2CP_MSG_TYPE_T type,
                    const unsigned char *data, unsigned int len);

void hal_mcu2cp_rx_done_mcu(enum HAL_MCU2CP_ID_T id);
void hal_mcu2cp_rx_done_cp (enum HAL_MCU2CP_ID_T id);

int hal_mcu2cp_opened_mcu(enum HAL_MCU2CP_ID_T id);
int hal_mcu2cp_opened_cp(enum HAL_MCU2CP_ID_T id);

int hal_mcu2cp_local_irq_pending_mcu(enum HAL_MCU2CP_ID_T id);
int hal_mcu2cp_local_irq_pending_cp(enum HAL_MCU2CP_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

