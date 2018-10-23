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

typedef enum {
    BLE_AIS_EVT_RX_DATA,
    BLE_AIS_EVT_TX_DONE,
    BLE_AIS_EVT_SVC_ENABLED,
} ble_ais_evt_type_t;

typedef struct {
    uint8_t *p_data;
    uint8_t length;
} ble_ais_rx_data_evt_t;

typedef struct {
    uint8_t pkt_sent;
} ble_ais_tx_done_evt_t;

typedef struct {
    ble_ais_evt_type_t type;
    union {
        ble_ais_rx_data_evt_t rx_data;
        ble_ais_tx_done_evt_t tx_done;
    } data;
} ble_ais_event_t;

typedef void (*ble_ais_event_handler_t)(ble_ais_event_t * p_event);

typedef struct {
    ble_ais_event_handler_t event_handler;
    void *p_context;
    uint16_t mtu;
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
    ble_ais_event_handler_t event_handler;
    void *p_context;
    uint16_t max_pkt_size;
} ble_ais_t;

uint32_t ble_ais_init(ble_ais_t * p_ais, const ble_ais_init_t * p_ais_init);
uint32_t ble_ais_send_notification(uint8_t * p_data, uint16_t length);
uint32_t ble_ais_send_indication(uint8_t * p_data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif // BZ_AIS_H
