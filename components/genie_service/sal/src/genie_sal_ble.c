#include <stdint.h>
#include <hal/hal.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
//#include <aos/aos.h>
#include <aos/kv.h>
#include "adv.h"
#include "common/log.h"
#include "genie_service.h"
#include "genie_sal_ble.h"

static aos_sem_t scan_sem;
static genie_sal_ble_get_rssi_cb get_rssi_cb = NULL;
static uint8_t test_dev_mac[6] = {0};

extern int hci_driver_init(void);

void mesh_scan_cb(const bt_addr_le_t *addr, int8_t rssi,
                  uint8_t adv_type, struct net_buf_simple *buf)
{
    uint8_t mac_addr[6] = {0};

    if (buf == NULL || addr == NULL)
    {
        return;
    }

    //printf("addr->a.val: %s, rssi: %d\n", bt_hex(addr->a.val, sizeof(addr->a.val)), rssi);
    sys_memcpy_swap(mac_addr, addr->a.val, 6);
    if (memcmp(test_dev_mac, mac_addr, sizeof(test_dev_mac)) == 0)
    {
        bt_mesh_scan_disable();
        bt_mesh_scan_enable();

        memset(test_dev_mac, 0, sizeof(test_dev_mac));
        if (get_rssi_cb)
        {
            get_rssi_cb(mac_addr, rssi);
        }
        aos_sem_signal_all(&scan_sem);
    }
}

static void scan_timeout(void)
{
    printf("scan timeout\r\n");
    bt_mesh_scan_disable();
    bt_mesh_scan_enable();

    memset(test_dev_mac, 0, sizeof(test_dev_mac));
    if (get_rssi_cb)
    {
        get_rssi_cb(test_dev_mac, 0);
    }
}

int genie_sal_ble_get_rssi(uint8_t mac[6], genie_sal_ble_get_rssi_cb get_rssi_callback, uint32_t timeout)
{
    struct bt_le_scan_param scan_param = {
        .type = BT_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval = 0x10,
        .window = 0x10};

    if (get_rssi_callback == NULL)
    {
        return -1;
    }

    if (0 != aos_sem_new(&scan_sem, 0))
    {
        printf("new sem fail\r\n");
        return -1;
    }

    memcpy(test_dev_mac, mac, sizeof(test_dev_mac));
    printf("scan mac:%s\n", bt_hex(test_dev_mac, sizeof(test_dev_mac)));
    get_rssi_cb = get_rssi_callback;

    bt_mesh_scan_disable();
    int ret = bt_le_scan_start(&scan_param, mesh_scan_cb);
    if (ret)
    {
        printf("scan err:%d\n", ret);
        memset(test_dev_mac, 0, sizeof(test_dev_mac));
        if (get_rssi_cb)
        {
            get_rssi_cb(test_dev_mac, 0);
        }
        aos_sem_free(&scan_sem);
        return -1;
    }

    if (-ETIMEDOUT == aos_sem_wait(&scan_sem, timeout))
    {
        scan_timeout();
    }
    aos_sem_free(&scan_sem);

    return 0;
}

int genie_sal_ble_send_msg(uint8_t element_id, uint8_t *p_data, uint8_t len)
{
    int ret = -1;
    uint8_t i = 0;
    uint8_t seg_count = 0;
    struct bt_mesh_model *p_model = NULL;
    struct net_buf_simple *msg = NULL;
    struct bt_mesh_msg_ctx ctx;
    struct bt_mesh_elem *p_element = NULL;
    genie_transport_model_param_t request_msg;

    if (len > GENIE_HAL_BLE_SEND_MAX_DATA_LEN)
    {
        return -1;
    }

    p_element = bt_mesh_elem_find_by_id(element_id);
    if (p_element == NULL)
    {
        printf("not find element by id:%d\r\n", element_id);
        return -1;
    }

    if ((len >= 4) && (p_data[0] == VENDOR_OP_ATTR_INDICATE || p_data[0] == VENDOR_OP_ATTR_INDICATE_TG))
    {
        seg_count = genie_transport_get_seg_count(len - 4 + 4);
        memset(&request_msg, 0, sizeof(genie_transport_model_param_t));
        request_msg.opid = p_data[0];
        request_msg.tid = p_data[3];
        request_msg.data = (u8_t *)(p_data + 4);
        request_msg.len = len - 4;
        request_msg.p_elem = p_element;
        request_msg.retry_period = GENIE_TRANSPORT_EACH_PDU_TIMEOUT * seg_count;
        request_msg.retry = VENDOR_MODEL_MSG_MAX_RETRY_TIMES;

        ret = genie_transport_send_model(&request_msg);
        if (ret != 0)
        {
            printf("vendor model msg send fail\n");
            return -1;
        }
        return 0;
    }
    else
    {
        ctx.app_idx = 0;
        ctx.net_idx = 0;
        ctx.addr = genie_transport_src_addr_get();
        ctx.send_ttl = GENIE_TRANSPORT_DEFAULT_TTL;
        ctx.send_rel = 0;

        msg = NET_BUF_SIMPLE(GENIE_HAL_BLE_SEND_MAX_DATA_LEN + 4);
        if (msg == NULL)
        {
            printf("no mem\n");
            return -1;
        }

        net_buf_simple_init(msg, 0);
        while (i < len)
        {
            net_buf_simple_add_u8(msg, p_data[i]);
            i++;
        }

        if ((p_data[0] & 0xC0) == 0xC0)
        {
            p_model = p_element->vnd_models;
        }
        else
        {
            p_model = p_element->models;
        }

        if (p_model == NULL)
        {
            printf("no model\n");
            return -1;
        }

        if (bt_mesh_model_send(p_model, &ctx, msg, NULL, NULL))
        {
            printf("mesh send fail\n");
            return -1;
        }

        return 0;
    }
}

int genie_sal_ble_set_factory_flag(void)
{
    int ret = -1;
    uint16_t flag = 0xAA55;

    ret = aos_kv_set("FTST", &flag, sizeof(uint16_t), 0);
    if (ret != 0)
    {
        printf("write FTST flag error\n");
        return ret;
    }

    return 0;
}

int genie_sal_ble_init(void)
{
    return hci_driver_init();
}
