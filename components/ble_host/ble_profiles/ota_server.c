/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


//#include <aos/osal_debug.h>
#include <aos/kernel.h>
#include <aos/ble.h>
#include <aos/gatt.h>
//#include <yoc/partition.h>

#include <yoc/ota_server.h>

#define OTA_PACKET_SIZE (8192)

#define OTA_PACKET_START (0xFF)
#define OTA_PACKET_END (0xFE)

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

enum ota_cmd {
    OTA_CMD_START = 0x01,
    OTA_CMD_STOP = 0x02,
    OTA_CMD_REQUEST = 0x03,
    OTA_CMD_COMPLETE = 0x04,
};

typedef struct _ota_start_t {
    uint32_t cmd: 8;
    uint32_t firmware_size: 24;
    uint16_t packet_size;
    uint8_t  timeout;
} ota_start_t;

struct ota_rx_t {
    uint8_t rx_buf[OTA_PACKET_SIZE];
    uint16_t rx_len;
    uint16_t expect_len;
    uint16_t packet_id;
    uint16_t packet_crc;
};

static struct ota_rx_t ota_rx = {0};

static struct ota_ctrl_t {
    ota_state_en st;
    uint32_t firmware_size;
    uint16_t packet_size;
    uint8_t  timeout;
    uint16_t expect_packet_count;
    uint16_t recv_packet_count;
    uint8_t  retry_count;
    aos_timer_t timer;
    ota_event_callback_t cb;
} ota_ctrl;

#define TAG "OTA"

#define YOC_OTA_SERVICE_UUID                                                                      \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd,  \
                    0x87, 0x3d, 0x93)

#define YOC_OTA_RX_UUID                                                                           \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd,  \
                    0x87, 0x3d, 0x94)

#define YOC_OTA_TX_UUID                                                                           \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd,  \
                    0x87, 0x3d, 0x95)

static int16_t g_yoc_ota_handle  = -1;
static int16_t g_conn_hanlde      = -1;
static int16_t g_tx_ccc_value     = 0;

static char rx_char_des[] = "YoC OTA RX";
static char tx_char_des[] = "YoC OTA TX";

enum {
    YOC_OTA_IDX_SVC,

    YOC_OTA_IDX_RX_CHAR,
    YOC_OTA_IDX_RX_VAL,
    YOC_OTA_IDX_RX_DES,

    YOC_OTA_IDX_TX_CHAR,
    YOC_OTA_IDX_TX_VAL,
    YOC_OTA_IDX_TX_CCC,
    YOC_OTA_IDX_TX_DES,

    YOC_OTA_IDX_MAX,
};

//static struct bt_gatt_ccc_cfg_t ccc_data[2] = {};
static gatt_service                    uart_service;

static gatt_attr_t uart_attrs[YOC_OTA_IDX_MAX] = {
    [YOC_OTA_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(YOC_OTA_SERVICE_UUID),

    [YOC_OTA_IDX_RX_CHAR] = GATT_CHAR_DEFINE(YOC_OTA_RX_UUID, GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [YOC_OTA_IDX_RX_VAL] =
    GATT_CHAR_VAL_DEFINE(YOC_OTA_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),
    [YOC_OTA_IDX_RX_DES] = GATT_CHAR_CUD_DEFINE(rx_char_des, GATT_PERM_READ),

    [YOC_OTA_IDX_TX_CHAR] =
    GATT_CHAR_DEFINE(YOC_OTA_TX_UUID, GATT_CHRC_PROP_NOTIFY | GATT_CHRC_PROP_READ),
    [YOC_OTA_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(YOC_OTA_TX_UUID, GATT_PERM_READ),
    [YOC_OTA_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
    [YOC_OTA_IDX_TX_DES] = GATT_CHAR_CUD_DEFINE(tx_char_des, GATT_PERM_READ),
};

static partition_t ota_partition_handle = -1;
static partition_info_t *ota_partition_info;

extern uint16_t crc16(uint16_t sedd, const volatile void *p_data, uint32_t size);

static void ota_status_change(int state)
{
    ota_ctrl.st = state;

    if (ota_ctrl.cb) {
        ota_ctrl.cb(state);
    }

    LOGD(TAG, "%d->%d", ota_ctrl.st, state);
}

static void ota_reset()
{
    ota_ctrl.st = 0;
    ota_ctrl.firmware_size = 0;
    ota_ctrl.packet_size = 0;
    ota_ctrl.timeout = 0;
    ota_ctrl.expect_packet_count = 0;
    ota_ctrl.recv_packet_count = 0;
    ota_ctrl.retry_count = 0;
    aos_timer_stop(&ota_ctrl.timer);
    memset(&ota_rx, 0, sizeof(ota_rx));
}

static void ota_reset_rx()
{
    memset(&ota_rx, 0, sizeof(ota_rx));
}

static int ota_partition_erase()
{
    if (partition_erase(ota_partition_handle, 0, (ota_partition_info->length + ota_partition_info->sector_size - 1) / ota_partition_info->sector_size) < 0) {
        LOGE(TAG, "erase addr:%x\n");
        return -1;
    }

    LOGD(TAG, "erase length %d block %d succeed", ota_partition_info->length, (ota_partition_info->length + ota_partition_info->sector_size - 1) / ota_partition_info->sector_size);

    return 0;
}

static int ota_partition_write(uint8_t *buffer, int length, int offset)
{
    if (partition_write(ota_partition_handle, offset + (ota_partition_info->sector_size << 1), (void *)buffer, length) >= 0) {
        LOGD(TAG, "write addr:%x length:%x\n", offset, length);

        return length;
    }

    LOGE(TAG, "write addr fail:%x length:%x\n", offset, length);
    return -1;
}

static inline uint32_t ota_partition_size()
{
    return ota_partition_info->length - (ota_partition_info->sector_size << 1);
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        g_conn_hanlde = e->conn_handle;
    } else {
        g_conn_hanlde = -1;
        ota_reset();
        ota_partition_erase();
        ota_status_change(OTA_ST_IDLE);
    }
}

static inline int ota_recv_data(const uint8_t *data, uint16_t len)
{
    if (!data || !len || !ota_rx.expect_len) {
        return -1;
    }

    uint16_t cp_size = MIN(ota_rx.expect_len - ota_rx.rx_len, len);

    memcpy(ota_rx.rx_buf + ota_rx.rx_len, data, cp_size);

    ota_rx.rx_len += cp_size;

    if (ota_rx.rx_len >= ota_rx.expect_len) {
        aos_timer_stop(&ota_ctrl.timer);
    }

    /* the last data */
    if (ota_rx.rx_len >= ota_rx.expect_len) {
        uint16_t crc = crc16(0, ota_rx.rx_buf, ota_rx.rx_len);

        if (crc == ota_rx.packet_crc) {
            ota_status_change(OTA_ST_FLASH);
        } else {
            LOGE(TAG, "crc err %x %x", crc, ota_rx.packet_crc);
            ota_status_change(OTA_ST_REQUEST);
        }
    }

    if (ota_rx.rx_len > ota_rx.expect_len) {
        LOGE(TAG, "ota packet err len %d,cp size %d, rx len %d expect len %d", len, cp_size , ota_ctrl.expect_packet_count, ota_rx.rx_len, ota_rx.expect_len);
        ota_status_change(OTA_ST_REQUEST);
    }

    return 0;
}

void event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e            = (evt_data_gatt_char_write_t *)event_data;
    int16_t                     hande_offset = 0;

    if (g_conn_hanlde != e->conn_handle) {
        return;
    }

    if (e->char_handle > g_yoc_ota_handle && e->char_handle < g_yoc_ota_handle + YOC_OTA_IDX_MAX) {
        hande_offset = e->char_handle - g_yoc_ota_handle;
    } else {
        return;
    }

    switch (hande_offset) {
        case YOC_OTA_IDX_RX_VAL: {
            switch (ota_ctrl.st) {
                case OTA_ST_IDLE: {
                    /* ota start */
                    if (e->len == 9 && e->data[0] == OTA_PACKET_START && e->data[1] == OTA_CMD_START
                        && e->data[e->len - 1] == OTA_PACKET_END) {
                        uint32_t firmware_size = e->data[2] << 16 | e->data[3] << 8 | e->data[4];
                        uint16_t packet_size = e->data[5] << 8 | e->data[6];
                        uint8_t timeout = e->data[7];

                        if (firmware_size > ota_partition_size() || packet_size > OTA_PACKET_SIZE) {
                            LOGE(TAG, "unsupport ota param %d %d", firmware_size, packet_size);
                            return;
                        }

                        ota_ctrl.firmware_size = firmware_size;
                        ota_ctrl.packet_size = packet_size;
                        ota_ctrl.timeout = timeout;
                        /* ota data formate */
                        /* |head  | packet id | crc16 | firmware    | tail  | */
                        /* |2Bytes| 2bytes    |2bytes |firmware size| 2Bytes| */
                        ota_ctrl.expect_packet_count = (firmware_size + packet_size - 1) / packet_size;
                        LOGI(TAG, "ota start firmware size %d packet size %d timeout %d expect packet count %d",
                             firmware_size, packet_size, timeout, ota_ctrl.expect_packet_count);
                        ota_status_change(OTA_ST_START);
                    } else {
                        LOGE(TAG, "unkonw packet, drop %d", e->len);
                    }

                    break;
                }

                case OTA_ST_DL: {
                    /* ota stop */
                    if (e->len == 3 && e->data[0] == OTA_PACKET_START && e->data[1] == OTA_CMD_STOP
                        && e->data[2] == OTA_PACKET_END) {
                        LOGI(TAG, "ota stop");
                        ota_reset();
                        ota_status_change(OTA_ST_STOP);
                    }
                    /* ota complete */
                    else if (e->len == 3 && e->data[0] == OTA_PACKET_START && e->data[1] == OTA_CMD_COMPLETE
                             && e->data[2] == OTA_PACKET_END) {
                        LOGI(TAG, "ota complelte");
                        ota_status_change(OTA_ST_COMPLETE);
                    }
                    /* ota data */
                    else if (e->len > 6 && e->data[0] == OTA_PACKET_START && e->data[1] == OTA_PACKET_END && ota_rx.expect_len == 0) {
                        uint16_t packet_id = e->data[2] << 8 | e->data[3];
                        uint16_t packet_crc = e->data[4] << 8 | e->data[5];

                        if (packet_id >= ota_ctrl.expect_packet_count) {
                            LOGE(TAG, "packet id overflow, drop %d\n", packet_id);
                            return;
                        }

                        if (packet_id == ota_ctrl.expect_packet_count - 1) {
                            ota_rx.expect_len = ota_ctrl.firmware_size % ota_ctrl.packet_size;
                        } else {
                            ota_rx.expect_len = ota_ctrl.packet_size;
                        }

                        ota_rx.packet_id = packet_id;
                        ota_rx.packet_crc = packet_crc;
                        ota_recv_data(e->data + 6, e->len - 6);
                    } else if (ota_rx.expect_len) {
                        ota_recv_data(e->data, e->len);
                    }

                    break;
                }

                default:
                    LOGE(TAG, "invaild st %d\n", ota_ctrl.st);
                    break;
            }
        }
        break;
    }

    e->len = 0;
    return;
}

void event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e            = (evt_data_gatt_char_read_t *)event_data;
    int16_t                    hande_offset = 0;

    if (g_conn_hanlde == e->conn_handle) {
        if (e->char_handle > g_yoc_ota_handle) {
            hande_offset = e->char_handle - g_yoc_ota_handle;
        } else {
            return;
        }

        switch (hande_offset) {
            case YOC_OTA_IDX_RX_VAL:
                memcpy(e->data, ota_rx.rx_buf, MIN(e->len, sizeof(ota_rx.rx_buf)));
                e->len = MIN(e->len, sizeof(ota_rx.rx_buf));
                return;

            case YOC_OTA_IDX_RX_DES:
                memcpy(e->data, rx_char_des, MIN(e->len, strlen(rx_char_des)));
                e->len = MIN(e->len, strlen(rx_char_des));
                return;

            case YOC_OTA_IDX_TX_DES:
                memcpy(e->data, tx_char_des, MIN(e->len, strlen(tx_char_des)));
                e->len = MIN(e->len, strlen(tx_char_des));
                return;
        }
    }

    e->len = 0;
    return;
}

void event_char_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;

    if (e->char_handle != g_yoc_ota_handle + YOC_OTA_IDX_TX_CCC) {
        return;
    }

    g_tx_ccc_value                     = e->ccc_value;

    LOGD(TAG, "ccc %d %d\n", e->char_handle, e->ccc_value);
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    if (e->err == 0) {
        LOGD(TAG, "mtu get %d", ble_stack_gatt_mtu_get(e->conn_handle));
    } else {
        LOGE(TAG, "mtu exchange fail, %d", e->err);
    }
}

static int event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ:
            event_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            event_char_write(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE:
            event_char_ccc_change(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}


static ble_event_cb_t ble_ota_cb = {
    .callback = event_callback,
};

static inline int ota_request(uint32_t offset)
{
    LOGI(TAG, "request %d", offset);

    ota_reset_rx();

    aos_timer_change(&ota_ctrl.timer, ota_ctrl.timeout * 1000);
    aos_timer_start(&ota_ctrl.timer);
    static uint8_t send_buf[6] = {0xff, 0x03, 0x00, 0x00, 0x00, 0xfe};
    send_buf[2] = offset >> 16 & 0xFF;
    send_buf[3] = offset >> 8 & 0xFF;
    send_buf[4] = offset >> 0 & 0xFF;
    return ble_stack_gatt_notificate(g_conn_hanlde, g_yoc_ota_handle + YOC_OTA_IDX_TX_VAL, send_buf, sizeof(send_buf));
}

void ble_ota_process()
{
    if (g_conn_hanlde == -1) {
        return;
    }

    switch (ota_ctrl.st) {
        case OTA_ST_START: {
            /* ota start response */
            if (g_tx_ccc_value != CCC_VALUE_NOTIFY) {
                aos_msleep(10);
                ota_status_change(OTA_ST_START);
            } else {
                conn_param_t param = {
                    0x06,
                    0x06,
                    0,
                    100,
                };
                ble_stack_connect_param_update(g_conn_hanlde, &param);

                uint8_t send_buf[4]  = {0xff, 0x01, 0x01, 0xfe};
                ble_stack_gatt_notificate(g_conn_hanlde, g_yoc_ota_handle + YOC_OTA_IDX_TX_VAL, send_buf, sizeof(send_buf));
                ota_status_change(OTA_ST_REQUEST);
            }
        }
        break;

        case OTA_ST_REQUEST: {
            uint32_t offset;
            offset = ota_ctrl.packet_size * ota_ctrl.recv_packet_count;
            ota_ctrl.retry_count++;

            if (ota_ctrl.retry_count > 10) {
                LOGE(TAG, "out of max try count %d", ota_ctrl.retry_count);
                ota_reset();
                ota_status_change(OTA_ST_IDLE);
                ble_stack_disconnect(g_conn_hanlde);
                return;
            }

            ota_request(offset);

            ota_status_change(OTA_ST_DL);

        }
        break;

        case OTA_ST_FLASH: {
            ota_ctrl.retry_count = 0;

            ota_partition_write(ota_rx.rx_buf, ota_rx.rx_len, ota_ctrl.recv_packet_count * ota_ctrl.packet_size);

            ota_ctrl.recv_packet_count++;

            if (ota_ctrl.recv_packet_count < ota_ctrl.expect_packet_count) {
                ota_status_change(OTA_ST_REQUEST);
            } else {
                LOGI(TAG, "all packet recv, %d", ota_ctrl.recv_packet_count);
                ota_status_change(OTA_ST_COMPLETE);
            }
        }
        break;

        case OTA_ST_STOP: {
            LOGI(TAG, "ota stop");
            /* may disconnect */

            ota_status_change(OTA_ST_IDLE);
        }
        break;

        case OTA_ST_COMPLETE: {
            LOGI(TAG, "ota reboot");
            /* may disconnect */
            /* ota start response */
            uint8_t send_buf[4]  = {0xff, 0x04, 0x00, 0xfe};
            ble_stack_gatt_notificate(g_conn_hanlde, g_yoc_ota_handle + YOC_OTA_IDX_TX_VAL, send_buf, sizeof(send_buf));
            extern void mdelay(uint32_t ms);
            mdelay(100);

            aos_reboot();
        }
        break;

        default:
            break;
    }
}

static void ota_timeout(void *arg1, void *arg2)
{
    ota_reset_rx();
    ota_status_change(OTA_ST_REQUEST);
    LOGW(TAG, "timeout");
}

int ble_ota_init(ota_event_callback_t cb)
{
    int ret;

    init_param_t init = {
        .dev_name     = NULL,
        .dev_addr     = NULL,
        .conn_num_max = 1,
    };

    ble_stack_init(&init);

    ble_stack_event_register(&ble_ota_cb);

    ret = ble_stack_gatt_registe_service(&uart_service, uart_attrs, BLE_ARRAY_NUM(uart_attrs));

    if (ret < 0) {
        return 0;
    }

    g_yoc_ota_handle = ret;

    if (ota_partition_handle < 0) {
        ota_partition_handle = partition_open("misc");

        if (ota_partition_handle < 0) {
            LOGD(TAG, "ota partition open fail %d", ret);
        }

        ota_partition_info = hal_flash_get_info(ota_partition_handle);
    }

    aos_timer_new_ext(&ota_ctrl.timer, ota_timeout, NULL, 1000, 0, 0);

    ota_reset();

    ota_partition_erase();

    ota_ctrl.cb = cb;

    return 0;
}

