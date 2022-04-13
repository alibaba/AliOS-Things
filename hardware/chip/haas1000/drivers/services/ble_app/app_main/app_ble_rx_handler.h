/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_BLE_RX_HANDLER_H__
#define __APP_BLE_RX_HANDLER_H__

#include "rwapp_config.h"

#ifdef __cplusplus
extern "C" {
#endif  // #ifdef __cplusplus

enum ble_rx_data_handler_num {
    BLE_RX_DATA_GSOUND_CONTROL = 0,
#if (BLE_APP_OTA)
    BLE_RX_DATA_SELF_OTA,
#endif
#if (BLE_APP_TOTA)
    BLE_RX_DATA_SELF_TOTA,
#endif
};

typedef struct {
    uint8_t  flag;
    uint8_t  conidx;
    uint16_t len;
    uint8_t *ptr;
} BLE_RX_EVENT_T;

void app_ble_rx_handler_init(void);

void app_ble_push_rx_data(uint8_t flag, uint8_t conidx, uint8_t *ptr, uint16_t len);

uint8_t *app_voicepath_get_common_ota_databuf(void);

#ifdef __cplusplus
}
#endif  // #ifdef __cplusplus

#endif  // #ifndef __APP_BLE_RX_HANDLER_H__
