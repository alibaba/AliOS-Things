/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_CAN_STM32F4_H
#define HAL_CAN_STM32F4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aos/kernel.h"
#include "aos/hal/can.h"
#include "board.h"

#define can_baud_1M   1000000
#define can_baud_500K 500000
#define can_baud_250K 250000
#define can_baud_125K 125000

#define STM32F4_BANK_NUM_HALF 14

typedef struct {
    PORT_CAN_TYPE  logical_func;
    void          *physical_port;
    void           (*isr_rx_handler)(CAN_RxHeaderTypeDef *raw, void *para);
    void          *isr_rx_para;
    int32_t        (*customer_recv)(can_frameheader_t *rxheader, void *data, const uint32_t timeout);
} CAN_MAPPING;

#define FORMAT_CAN(SRC, DST_HEAD, DST_DATA) \
    do { \
        DST_HEAD.id  = SRC->IDE == CAN_ID_STD ? SRC->StdId : SRC->ExtId; \
        DST_HEAD.rtr = SRC->RTR == CAN_RTR_DATA ? 0 : 1;                 \
        DST_HEAD.dlc = SRC->DLC < 8 ? SRC->DLC : 8;                      \
    } while(0)

/* Mapping Table is defined in soc_init.c */
extern CAN_MAPPING CAN_MAPPING_TABLE[PORT_CAN_SIZE];

extern CAN_MAPPING *get_can_list_logical(const PORT_CAN_TYPE port);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_CAN_STM32F4_H */
