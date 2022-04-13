#include <stdio.h>
#include <errno.h>
#include <ble_os.h>
//#include <aos/osal_debug.h>
#include <aos/kernel.h>
#include <aos/ble.h>
//#include <aos/aos.h>
#include "yoc/uart_client.h"



#define UUID_COMPARE_LENGTH 10
#define MAX_DEV_SURVIVE_TIME 10000 //ms


enum {
    DISC_IDLE = 0,
    DISC_SRV,
    DISC_CHAR,
    DISC_DES,
};

typedef struct _report_dev {
    dev_addr_t addr;
    long long time_found;
} report_dev;

typedef struct {
    uint8_t front;
    uint8_t rear;
    uint8_t length;
    report_dev dev[10];
} dev_data_queue;

typedef struct _bt_uuid {
    uint8_t type;
} bt_uuid;

struct bt_uuid_128 {
    bt_uuid uuid;
    uint16_t val[16];
};

static ble_uart_client_t *g_uart_dev = NULL;
static dev_data_queue g_dev_list;
static client_config *g_cli_config = NULL;
static uint8_t g_disconn_flag = 0;
static dev_addr_t g_conn_last;
static uint8_t g_conn_same_flag  = 0;

static void conn_change(ble_event_en event, void *event_data)
{

    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    ble_uart_client_t *node = g_uart_dev;

    if (!node) {
        return;
    }

    if (e->connected == CONNECTED) {
        ble_stack_gatt_mtu_exchange(e->conn_handle);
        node->conn_handle = e->conn_handle;
        connect_info_t info = {0};
        ble_stack_connect_info_get(e->conn_handle, &info);

        if (!memcmp(&info.peer_addr, &g_conn_last, sizeof(dev_addr_t))) {
            g_conn_same_flag  = 1;
        } else {
            memcpy(&g_conn_last, &info.peer_addr, sizeof(dev_addr_t));
        }
    } else {
        node->conn_handle = -1;
    }

    node->uart_event_callback(event, event_data);

}



static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    ble_uart_client_t *node = g_uart_dev;

    if (!node) {
        return;
    }

    if (e->err == 0) {
        if (node->conn_handle == e->conn_handle) {
            node->mtu_exchanged = 1;
            node->mtu = ble_stack_gatt_mtu_get(node->conn_handle);

            if (!g_conn_same_flag) {
                ble_stack_gatt_discovery_primary(node->conn_handle, YOC_UART_SERVICE_UUID, 0x0001, 0xFFFF);
            } else {
                uint8_t ccc_enable = CCC_VALUE_NOTIFY;
                int ret = ble_stack_gatt_write_response(e->conn_handle, node->client_data.uart_profile.uart_ccc_handle, &ccc_enable, sizeof(ccc_enable), 0);

                if (ret < 0) {
                    return;
                }

                node->client_data.uart_profile.notify_enabled = 1;

                if ((node->update_param_flag || node->conn_update_def_on) && node->conn_param != NULL) {
                    ble_stack_connect_param_update(node->conn_handle, (conn_param_t *)node->conn_param);
                }
            }
        }

        node->uart_event_callback(event, event_data);
    }
}


int find_uart_uuid_by_ad(uint8_t *data, uint16_t len)
{
    uint8_t *pdata = data;
    ad_data_t ad = {0};
    uint8_t uuid_temp[16] = {0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93};

    while (len) {
        if (pdata[0] == 0) {
            return -1;
        }

        if (len < pdata[0] + 1) {
            return -1;
        };

        ad.len = pdata[0] - 1;

        ad.type = pdata[1];

        ad.data = &pdata[2];

        if (ad.type == AD_DATA_TYPE_UUID128_ALL && ad.len == 16) {
            if (!memcmp(ad.data, uuid_temp, 10)) {
                return 0;
            }
        }

        len -= (pdata[0] + 1);
        pdata += (pdata[0] + 1);
    }

    return -1;
}


static bool is_dev_data_queue_empty(dev_data_queue *queue)
{
    return queue->front == queue->rear;
}

static bool is_dev_data_queue_full(dev_data_queue *queue)
{
    if ((queue->rear + 1) % 10 == queue->front) {
        return true;
    } else {
        return false;
    }
}

static report_dev *get_dev_data(dev_data_queue *queue)
{
    if (is_dev_data_queue_empty(queue)) {
        return NULL;
    } else {
        report_dev *data = &queue->dev[queue->front];
        queue->front = (queue->front + 1) % 10;
        return data;
    }
}


static int put_dev_data(dev_data_queue *queue, dev_addr_t *dev)
{
    if (!queue || !dev) {
        return -1;
    }

    if (is_dev_data_queue_full(queue)) {
        return -1;
    }

    for (int i = queue->front; i < queue->rear ; i++) {
        if (!memcmp(&queue->dev[i].addr, dev, sizeof(dev_addr_t))) {
            return 0;
        }
    }

    memcpy(&queue->dev[queue->rear].addr, dev, sizeof(dev_addr_t));
    queue->dev[queue->rear].time_found = aos_now_ms();
    queue->rear = (queue->rear + 1) % 10;
    return 0;
}


dev_addr_t *found_dev_get()
{
    report_dev *dev = NULL;
    long long time_now = 0;
    long long survive_time = 0;

    do {
        dev = get_dev_data(&g_dev_list);

        if (dev) {
            time_now = aos_now_ms();
            survive_time = time_now - dev->time_found;
        }

    } while (dev && survive_time > MAX_DEV_SURVIVE_TIME);

    if (!dev) {
        return NULL;
    } else {
        return &dev->addr;
    }
}

static void device_find(ble_event_en event, void *event_data)
{
    int ret = -1;
    int uuid_peer = -1;
    evt_data_gap_dev_find_t ee;
    evt_data_gap_dev_find_t *e;
    uint8_t find_dev = 0;
    memcpy(&ee, event_data, sizeof(evt_data_gap_dev_find_t));////

    e = &ee;
    conn_param_t param = {
        0x06,  //interval min 7.5ms
        0x06,  //interval max 7.5ms
        0,
        100,   //supervision timeout 1s
    };

    if (!g_cli_config) {
        return;
    }

    if (e->adv_type != ADV_IND) {
        return;
    }

    if (e->adv_len > 31) {
        return;
    }

    uuid_peer = find_uart_uuid_by_ad(e->adv_data, e->adv_len);

    if (uuid_peer < 0) {
        return;
    }

    put_dev_data(&g_dev_list, &e->dev_addr);

    if (g_disconn_flag) {
        return;
    }

    if (!g_cli_config->conn_def_on) {
        if (!memcmp(&g_cli_config->temp_conn_dev_set, &e->dev_addr, sizeof(g_cli_config->temp_conn_dev_set))) {
            find_dev = 1;
        }
    } else {
        if (g_cli_config->auto_conn_mac_size > 0) {
            for (int i = 0; i < g_cli_config->auto_conn_mac_size; i++) {
                if (!memcmp((uint8_t *)&g_cli_config->auto_conn_mac[i], (uint8_t *)&e->dev_addr, sizeof(dev_addr_t))) {
                    find_dev = 1;
                    break;
                }
            }
        } else if (!uuid_peer) {
            find_dev = 1;
        }
    }

    if (find_dev) {
        ble_stack_scan_stop();
        ret = ble_stack_connect(&e->dev_addr, &param, 0);

        if (ret < 0) {
            uart_client_scan_start();
            return;
        }
    }
}


static uint8_t UUID_EQUAL_LEN(uuid_t *uuid1, uuid_t *uuid2, uint8_t comp_len)
{
    if (UUID_TYPE(uuid1) != UUID_TYPE(uuid2)) {
        return false;
    }

    comp_len = comp_len < UUID_LEN(uuid1) ? comp_len : UUID_LEN(uuid1);
    return (0 == memcmp(&((struct bt_uuid_128 *)uuid1)->val, &((struct bt_uuid_128 *)uuid2)->val, comp_len));
}


static void service_discovery(ble_event_en event, void *event_data)
{
    int ret;

    static uint8_t discovery_state = 0;
    ble_uart_client_t *uart = g_uart_dev;

    if (!uart) {
        return;
    }


    if (event == EVENT_GATT_DISCOVERY_COMPLETE) {
        evt_data_gatt_discovery_complete_t *e = event_data;

        if (discovery_state == DISC_SRV) {
            ble_stack_gatt_discovery_char(e->conn_handle, YOC_UART_RX_UUID, uart->client_data.uart_profile.uart_handle + 1, uart->client_data.uart_profile.uart_end_handle);
        } else if (discovery_state == DISC_CHAR) {
            ble_stack_gatt_discovery_descriptor(e->conn_handle, UUID_GATT_CCC, uart->client_data.uart_profile.uart_char_handle + 1, 0xffff);
        } else if (discovery_state == DISC_DES) {
            uint8_t ccc_enable = CCC_VALUE_NOTIFY;
            ret = ble_stack_gatt_write_response(e->conn_handle, uart->client_data.uart_profile.uart_ccc_handle, &ccc_enable, sizeof(ccc_enable), 0);

            if (ret < 0) {
                return;
            }

            uart->client_data.uart_profile.notify_enabled = 1;

            if ((uart->update_param_flag || uart->conn_update_def_on) && uart->conn_param != NULL) {
                ble_stack_connect_param_update(uart->conn_handle, (conn_param_t *)uart->conn_param);
            }

            return;
        }
    }

    if (event == EVENT_GATT_DISCOVERY_SVC) {
        evt_data_gatt_discovery_svc_t *e = event_data;
        discovery_state = DISC_SRV;

        if (UUID_EQUAL_LEN(&e->uuid, YOC_UART_SERVICE_UUID, UUID_COMPARE_LENGTH)) {
            uart->client_data.uart_profile.uart_handle = e->start_handle;
            uart->client_data.uart_profile.uart_end_handle = e->end_handle;
        }
    }

    if (event == EVENT_GATT_DISCOVERY_CHAR) {
        evt_data_gatt_discovery_char_t *e = event_data;
        uart->client_data.uart_profile.uart_char_handle = e->val_handle;
        discovery_state = DISC_CHAR;
    }

    if (event == EVENT_GATT_DISCOVERY_CHAR_DES) {
        evt_data_gatt_discovery_char_des_t *e = event_data;
        uart->client_data.uart_profile.uart_ccc_handle = e->attr_handle;
        discovery_state = DISC_DES;

    }
}

static void event_notify(ble_event_en event, void *event_data)
{
    evt_data_gatt_notify_t *e = (evt_data_gatt_notify_t *)event_data;
    ble_uart_client_t *uart = g_uart_dev;

    if (!uart) {
        return;
    }

    if (uart->conn_handle == e->conn_handle) {
        e->len = e->len < RX_MAX_LEN ? e->len : RX_MAX_LEN;
        uart->uart_recv(e->data, e->len);
    }
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

        case EVENT_GAP_DEV_FIND:
            device_find(event, event_data);
            break;

        case EVENT_GATT_DISCOVERY_SVC:
        case EVENT_GATT_DISCOVERY_CHAR:
        case EVENT_GATT_DISCOVERY_CHAR_DES:
        case EVENT_GATT_DISCOVERY_COMPLETE:
            service_discovery(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE: {
            break;
        }

        case EVENT_GATT_NOTIFY: {
            event_notify(event, event_data);
            break;
        }

        default:
            break;
    }

    return 0;
}



int uart_client_scan_start()
{
    int ret;
    scan_param_t param = {
        SCAN_PASSIVE,
        SCAN_FILTER_DUP_ENABLE,
        SCAN_FAST_INTERVAL,
        SCAN_FAST_WINDOW,
    };

    ret = ble_stack_scan_start(&param);

    if (ret) {
        return ret;
    }

    return 0;
}


static ble_event_cb_t ble_cb = {
    .callback = uart_event_callback,
};

int uart_client_send(uart_handle_t handle, const       char *data, int length, bt_uart_send_cb *cb)
{
    uint32_t count = length;
    int ret = 0;
    uint16_t wait_timer = 0;

    ble_uart_client_t *uart = handle;

    if (data == NULL || length <= 0 || !uart || uart->conn_handle < 0 || !uart->client_data.uart_profile.notify_enabled) {
        return -1;
    }

    if (cb != NULL && cb->start != NULL) {
        cb->start(0, NULL);
    }

    while (count) {
        uint16_t send_count = (uart->mtu - 3) < count ? (uart->mtu - 3) : count;
        ret = ble_stack_gatt_write_no_response(uart->conn_handle, uart->client_data.uart_profile.uart_handle + 2, (uint8_t *)data, send_count, 0);

        if (ret == -ENOMEM) {
            wait_timer++;

            if (wait_timer >= 500) {
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

int uart_client_conn(dev_addr_t *conn_mac, conn_param_t *conn_param)
{
    if (!g_cli_config) {
        return -1;
    }

    memcpy(g_cli_config->temp_conn_dev_set.val, conn_mac->val, sizeof(g_cli_config->temp_conn_dev_set.val));
    g_cli_config->temp_conn_dev_set.type = conn_mac->type;

    if (conn_param) {
        g_cli_config->temp_conn_param = conn_param;
    }

    g_disconn_flag = 0;
    return 0;
}


int uart_client_disconn(uart_handle_t handle)
{
    ble_uart_client_t *uart = handle;

    if (!uart) {
        return -1;
    }

    if (ble_stack_disconnect(uart->conn_handle)) {
        return -1;
    }

    uart->client_data.client_conf.conn_def_on = 0;
    g_disconn_flag = 1;

    return 0;
}


uart_handle_t uart_client_init(ble_uart_client_t *service)
{
    int ret = 0;

    if (service == NULL) {
        return NULL;
    }

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        return NULL;
    }

    service->conn_handle = -1;
    g_cli_config = &service->client_data.client_conf;
    g_uart_dev = service;
    g_dev_list.front = 0;
    g_dev_list.rear = 0;
    g_dev_list.length = 10;

    return service;
}

int uart_client_conn_param_update(uart_handle_t handle, conn_param_t *param)
{
    ble_uart_client_t *uart = (ble_uart_client_t *)handle;

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



