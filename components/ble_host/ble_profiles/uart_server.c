
#include <aos/kernel.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <errno.h>
#include "yoc/uart_server.h"



enum {
    YOC_UART_IDX_SVC,

    YOC_UART_IDX_RX_CHAR,
    YOC_UART_IDX_RX_VAL,
    YOC_UART_IDX_RX_DES,

    YOC_UART_IDX_TX_CHAR,
    YOC_UART_IDX_TX_VAL,
    YOC_UART_IDX_TX_CCC,
    YOC_UART_IDX_TX_DES,

    YOC_UART_IDX_MAX,
};


//static struct bt_gatt_ccc_cfg_t ccc_data[2] = {};
static  gatt_service g_uart_profile;

extern uint8_t llState;

static ble_uart_server_t *g_uart_dev = NULL;

static char rx_char_des[] = "YoC UART RX";
static char tx_char_des[] = "YoC UART TX";

gatt_attr_t uart_attrs[YOC_UART_IDX_MAX] = {
    [YOC_UART_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(YOC_UART_SERVICE_UUID),

    [YOC_UART_IDX_RX_CHAR] = GATT_CHAR_DEFINE(YOC_UART_RX_UUID,  GATT_CHRC_PROP_WRITE),
    [YOC_UART_IDX_RX_VAL] = GATT_CHAR_VAL_DEFINE(YOC_UART_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),
    [YOC_UART_IDX_RX_DES] = GATT_CHAR_CUD_DEFINE(rx_char_des, GATT_PERM_READ),

    [YOC_UART_IDX_TX_CHAR] = GATT_CHAR_DEFINE(YOC_UART_TX_UUID,  GATT_CHRC_PROP_NOTIFY | GATT_CHRC_PROP_READ),
    [YOC_UART_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(YOC_UART_TX_UUID, GATT_PERM_READ),
    [YOC_UART_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
    [YOC_UART_IDX_TX_DES] = GATT_CHAR_CUD_DEFINE(tx_char_des, GATT_PERM_READ),
};


static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    ble_uart_server_t *node = g_uart_dev;

    if (!node) {
        return;
    }

    if (e->connected == CONNECTED) {
        node->conn_handle = e->conn_handle;
    } else {
        node->conn_handle = -1;
    }

    node->uart_event_callback(event, event_data);

}


static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    ble_uart_server_t *node = g_uart_dev;

    if (!node) {
        return;
    }

    if (e->err == 0) {
        if (node->conn_handle == e->conn_handle) {
            node->mtu_exchanged = 1;
            node->mtu = ble_stack_gatt_mtu_get(node->conn_handle);
        }

        conn_param_t param = {
            0x08,  //interval min 20ms
            0x08,  //interval max 20ms
            0,
            100,   //supervision timeout 1s
        };
        ble_stack_connect_param_update(e->conn_handle, &param);

        node->uart_event_callback(event, event_data);
    }
}

static void event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;

    ble_uart_server_t *uart = g_uart_dev;

    if (!uart) {
        return;
    }

    uart->server_data.tx_ccc_value = e->ccc_value;

    if ((uart->update_param_flag || uart->conn_update_def_on) && uart->conn_param != NULL) {
        ble_stack_connect_param_update(uart->conn_handle, (conn_param_t *)uart->conn_param);
    }
}

static void event_char_write(ble_event_en event, void *event_data)
{

    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    ble_uart_server_t *uart = g_uart_dev;

    if (!uart) {
        return;
    }

    e->len = e->len < RX_MAX_LEN ?  e->len : RX_MAX_LEN;
    uart->uart_recv(e->data, e->len);
    e->len = 0;
    return;
}

int uart_server_disconn(uart_handle_t handle)
{
    ble_uart_server_t *uart = (ble_uart_server_t *)handle;

    if (!uart) {
        return -1;
    }

    if (ble_stack_disconnect(uart->conn_handle)) {
        return -1;
    }

    return 0;
}


static int uart_event_callback(ble_event_en event, void *event_data)
{

    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE:
            event_char_ccc_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            event_char_write(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = uart_event_callback,
};


int uart_server_send(uart_handle_t handle, const       char *data, int length, bt_uart_send_cb *cb)
{

    uint32_t count = length;
    uint16_t wait_timer = 0;
    int ret = 0;
    ble_uart_server_t *uart = (ble_uart_server_t *)handle;

    if (!data || !length || !uart || uart->conn_handle < 0 || uart->server_data.tx_ccc_value != CCC_VALUE_NOTIFY) {
        return -1;
    }

    if (cb != NULL && cb->start != NULL) {
        cb->start(0, NULL);
    }

    while (count) {
        uint16_t send_count = (uart->mtu - 3) < count ? (uart->mtu - 3) : count;
        ret = ble_stack_gatt_notificate(uart->conn_handle, uart->uart_svc_handle + YOC_UART_IDX_TX_VAL, (uint8_t *)data, send_count);

        if (ret == -ENOMEM) {
            wait_timer++;

            if (wait_timer >= 100) {
                cb->end(-1, NULL);
                return -1;
            }

            aos_msleep(1);

            continue;
        }

        if (ret) {
            if (cb != NULL && cb->end != NULL) {
                cb->end(ret, NULL);
            }

            return ret;
        }

        data += send_count;
        count -= send_count;
    }

    if (cb != NULL && cb->end != NULL) {
        cb->end(0, NULL);
    }

    return 0;

}


uart_handle_t uart_server_init(ble_uart_server_t *service)
{
    int ret = 0;
    int g_yoc_uart_handle = -1;

    if (service == NULL) {
        return NULL;
    }

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        return NULL;
    }

    g_yoc_uart_handle = ble_stack_gatt_registe_service(&g_uart_profile, uart_attrs,  BLE_ARRAY_NUM(uart_attrs));

    if (g_yoc_uart_handle < 0) {
        return NULL;
    }

    service->uart_svc_handle = g_yoc_uart_handle;
    service->conn_handle = -1;

    g_uart_dev = service;
    return service;
}


int uart_server_adv_control(uint8_t adv_on, adv_param_t *adv_param)
{
    int ret = 0;

    ble_stack_adv_stop();

    if (adv_on) {
        if (adv_param == NULL) {
            return -1;
        } else {
            ret = ble_stack_adv_start(adv_param);

            if (ret) {
                return ret;
            }
        }
    }

    return 0;
}



int uart_server_conn_param_update(uart_handle_t handle, conn_param_t *param)
{
    ble_uart_server_t *uart = (ble_uart_server_t *)handle;

    if (!uart) {
        return -1;
    }

    uart->conn_param->interval_min = param->interval_min;
    uart->conn_param->interval_max = param->interval_max;
    uart->conn_param->latency = param->latency;
    uart->conn_param->timeout = param->timeout;

    if (uart->conn_handle < 0) {
        uart->update_param_flag = 1;

    } else {
        return ble_stack_connect_param_update(uart->conn_handle, param);
    }

    return 0;
}



