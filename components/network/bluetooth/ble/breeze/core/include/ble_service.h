/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_AIS_H
#define BZ_AIS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_UUID_AIS_SERVICE 0xFEB3

typedef void (* init_done_t)(uint8_t res);

typedef struct {
    uint16_t mtu;
    init_done_t init_done;
} ble_ais_init_t;

typedef struct ble_gatts_char_handles_s {
    uint16_t chrc_handle;
    uint16_t value_handle;
    uint16_t user_desc_handle;
    uint16_t cccd_handle;
    uint16_t sccd_handle;
} ble_gatts_char_handles_t;

typedef struct ble_ais_s {
    uint16_t service_handle;
    ble_gatts_char_handles_t rc_handles;  // Handles related to Read Characteristics
    ble_gatts_char_handles_t wc_handles;  // Handles related to Write Characteristics
    ble_gatts_char_handles_t ic_handles;  // Handles related to Indicate Characteristics
    ble_gatts_char_handles_t wwnrc_handles;  // Handles related to Write WithNoRsp Characteristics
    ble_gatts_char_handles_t nc_handles;  // Handles related to Notify Characteristics
    uint16_t conn_handle;  // Handle of the current connection
    bool is_indication_enabled;
    bool is_notification_enabled;
    void *p_context;
    uint16_t max_pkt_size;
} ble_ais_t;

uint32_t ble_ais_init(const ble_ais_init_t * p_ais_init);
uint32_t ble_ais_send_notification(uint8_t * p_data, uint16_t length);
uint32_t ble_ais_send_indication(uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif // BZ_AIS_H
