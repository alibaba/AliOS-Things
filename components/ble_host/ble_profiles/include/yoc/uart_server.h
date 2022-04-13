#ifndef _UART_SERVER_H
#define _UART_SERVER_H

#include <aos/ble.h>
#include <stdio.h>
#include "yoc/uart_profile.h"

typedef struct _uart_server {
    uint8_t adv_def_on;
    adv_param_t *advParam;
    ccc_value_en tx_ccc_value;
} uart_server;

typedef struct _ble_uart_server_t {
    int16_t conn_handle;
    uint16_t uart_svc_handle;
    uart_rx_data_cb uart_recv;
    ble_event_cb uart_event_callback;
    conn_param_t *conn_param;
    uint8_t conn_update_def_on: 1;
    uint8_t update_param_flag: 1;
    uint8_t mtu_exchanged: 1;
    uint16_t mtu;
    uart_server server_data;
} ble_uart_server_t;

uart_handle_t uart_server_init(ble_uart_server_t *service);
int uart_server_send(uart_handle_t handle, const       char *data, int length, bt_uart_send_cb *cb);
int uart_server_disconn(uart_handle_t handle);
int uart_server_adv_control(uint8_t adv_on, adv_param_t *adv_param);
int uart_server_conn_param_update(uart_handle_t handle, conn_param_t *param);


#endif

