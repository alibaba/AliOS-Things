/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_INTERSYS_H__
#define __HAL_INTERSYS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

enum HAL_INTERSYS_ID_T {
    HAL_INTERSYS_ID_0,
    HAL_INTERSYS_ID_1,

    HAL_INTERSYS_ID_QTY
};

enum HAL_INTERSYS_MSG_TYPE_T {
    HAL_INTERSYS_MSG_HCI,
    HAL_INTERSYS_MSG_SYS,

    HAL_INTERSYS_MSG_TYPE_QTY
};

typedef struct debug_intersys
{
    unsigned short irq_happen;
    unsigned short cmd_opcode;
}debug_intersys_type;

typedef unsigned int (*HAL_INTERSYS_RX_IRQ_HANDLER)(const unsigned char *data, unsigned int len);
typedef void (*HAL_INTERSYS_TX_IRQ_HANDLER)(const unsigned char *data, unsigned int len);

int hal_intersys_open(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_MSG_TYPE_T type,
                      HAL_INTERSYS_RX_IRQ_HANDLER rxhandler, HAL_INTERSYS_TX_IRQ_HANDLER txhandler, bool rx_flowctrl);

int hal_intersys_close(enum HAL_INTERSYS_ID_T id,enum HAL_INTERSYS_MSG_TYPE_T type);

int hal_intersys_start_recv(enum HAL_INTERSYS_ID_T id);

int hal_intersys_stop_recv(enum HAL_INTERSYS_ID_T id);

int hal_intersys_send(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_MSG_TYPE_T type,
                      const unsigned char *data, unsigned int len);

void hal_intersys_rx_done(enum HAL_INTERSYS_ID_T id);

void hal_intersys_peer_irq_auto_clear(bool enable);

#ifdef TX_RX_PCM_MASK
typedef void (*FRAME2BUFF_HANDLER)();

int hal_intersys_mic_open(enum HAL_INTERSYS_ID_T id, FRAME2BUFF_HANDLER dch);

void hal_intersys_mic_rx_irq();
#endif

#ifdef __cplusplus
}
#endif

#endif

