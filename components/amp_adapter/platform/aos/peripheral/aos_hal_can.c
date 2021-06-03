/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/can.h"
#include "aos_hal_can.h"
#include "aos_system.h"

typedef enum {
    PORT_CAN1 = 1,
    PORT_CAN2 = 2,
    PORT_CAN_SIZE,
    PORT_CAN_INVALID = 255,
} PORT_CAN_NUM;

extern can_dev_t g_can_handle[2];
static int can_received = 0;
static uint8_t can1_rx_data[8] = {0};
static uint8_t can2_rx_data[8] = {0};
static can_frameheader_t can1_rx_message = {0};
static can_frameheader_t can2_rx_message = {0};


int32_t aos_hal_can_init(can_dev_t *can)
{
    return hal_can_init(can);
}

int32_t aos_hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config)
{
    return hal_can_filter_init(can, filter_grp_cnt, filter_config);
}

int32_t aos_hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, const uint32_t timeout)
{
    return hal_can_send(can, tx_header, data, timeout);
}

void can1_receive_callback(void)
{
    int i, ret;

    ret = hal_can_recv(&g_can_handle[PORT_CAN1], &can1_rx_message, can1_rx_data, 0xFFFFFFFF);
    if (ret != 0) {
        aos_printf("hal can receive failed!\r\n");
        return;
    }

    can_received = 1;

}

void can2_receive_callback(void)
{
    int i, ret;

    ret = hal_can_recv(&g_can_handle[PORT_CAN2], &can2_rx_message, can2_rx_data, 0xFFFFFFFF);
    if (ret != 0) {
        aos_printf("hal can receive failed!\r\n");
        return;
    }

    can_received = 1;

}

int32_t aos_hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, const uint32_t timeout)
{
    int32_t ret = -1;
    if (can_received) {
        if (can->port == PORT_CAN1) {
            *rx_header = can1_rx_message;
            if (rx_header->rtr == 0) {
                memcpy(data, can1_rx_data, 8);
            }
            can_received = 0;
            return 0;
        }
        if (can->port == PORT_CAN2) {
            *rx_header = can2_rx_message;
            if (rx_header->rtr == 0) {
                memcpy(data, can2_rx_data, 8);
            }
            can_received = 0;
            return 0;
        }
    }
}

int32_t aos_hal_can_finalize(can_dev_t *can)
{
    return hal_can_finalize(can);
}
