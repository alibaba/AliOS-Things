/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef CONFIG_AOS_MESH

#include <string.h>
#include <stdio.h>

#include "hci.h"
#include "adv.h"
#include "bluetooth.h"
#include "hci_core.h"
#include "umesh_hal.h"
#include "net/buf.h"
#include "mesh/access.h"

enum {
    BLE_MAC_ADDR_SIZE = 8,
    BLE_DEFAULT_MTU_SIZE = 29,
    BT_MESH_XMIT_COUNT = 2, // 3 transmissions
    BT_MESH_XMIT_INT = 20, // 20ms interval
    BT_MESH_SCAN_INTERVAL = 0x10,
    BT_MESH_SCAN_WINDOW = 0x10,
};

typedef struct send_ext_s {
    void *context;
    void *sent;
    frame_t *frame;
} send_cxt_t;

typedef struct {
    uint32_t mtu;
    uint8_t channel;
    uint8_t chn_num;
    const uint8_t *channels;
    umesh_handle_received_frame_t rxcb;
    
    void *context;
    umesh_hal_module_t *module;
    unsigned char macaddr[BLE_MAC_ADDR_SIZE];

    frame_stats_t stats;
} mesh_hal_priv_t;

static send_cxt_t g_send_ucast_cxt;
static send_cxt_t g_send_bcast_cxt;

static mesh_hal_priv_t *g_hal_priv;
static umesh_hal_module_t esp_bt_mesh_module;
static const uint8_t g_ble_channels[] = {0, 12, 39}; //advertising channels
static mesh_hal_priv_t g_ble_priv = {
    .mtu = BLE_DEFAULT_MTU_SIZE,
    .channel = 0,
    .chn_num = sizeof(g_ble_channels),
    .channels = g_ble_channels,
    .module = &esp_bt_mesh_module,
};

static void adv_frame_start(u16_t duration, int err, void *user_data)
{
    (void)duration;
    (void)user_data;
    printk("adv frame start error = %d\n", err);
}

static void adv_frame_sent(int err, void *user_data)
{
    mesh_hal_priv_t *priv = g_hal_priv;
    send_cxt_t *cxt = (send_cxt_t *)user_data;
    umesh_handle_sent_ucast_t sent;

    if (err == 0) {
        priv->stats.out_frames++;
    }
    
    printk("adv frame sent error = %d\n", err);

    if (cxt) {
        sent = cxt->sent;
        (*sent)(cxt->context, cxt->frame, ((err == 0) ? 0 : 1));
    }
}

static const struct bt_mesh_send_cb send_cb = {
     //.start = adv_frame_start,
     .end = adv_frame_sent,
};

static void pass_to_umesh(struct net_buf_simple *buf, const bt_addr_le_t *addr,
                          const uint8_t rssi)
{
    uint8_t *payload;
    frame_t frame;
    frame_info_t fino;
    mesh_hal_priv_t *priv = g_hal_priv;
    umesh_hal_module_t *module = priv->module;

    printk("len %u: %s\n", buf->len, bt_hex(buf->data, buf->len));

    payload = aos_malloc(buf->len);
    memset(payload, 0, buf->len);
    frame.len = buf->len;
    frame.data = payload;
    memcpy(fino.peer.addr, addr->a.val, BLE_MAC_ADDR_SIZE);
    fino.peer.len = BLE_MAC_ADDR_SIZE;

    module = hal_umesh_get_default_module();
    fino.channel = hal_umesh_get_channel(module);
    fino.rssi = rssi;

    memcpy(frame.data, buf->data, frame.len);

    priv->stats.in_frames++;
    if (priv->rxcb) {
        priv->rxcb(priv->context, &frame, &fino, 0);
    }

    aos_free(payload);
}

static int send_frame(umesh_hal_module_t *module, frame_t *frame,
                      send_cxt_t *cxt)
{
    struct net_buf *buf;

    buf = bt_mesh_adv_create(BT_MESH_ADV_DATA, BT_MESH_XMIT_COUNT,
                             BT_MESH_XMIT_INT, 0);
    if (!buf) {
        printk("no available buf\n");
        return -1;
    }

    net_buf_add_mem(buf, frame->data, frame->len);
    bt_mesh_adv_send(buf, &send_cb, (void *)cxt);
    net_buf_unref(buf);
    return 0;
}

static void esp_bt_mesh_scan_cb(const bt_addr_le_t *addr, signed char rssi,
                                uint8_t adv_type, struct net_buf_simple *buf)
{
    if (adv_type != BT_LE_ADV_NONCONN_IND) {
        return;
    }

    while (buf->len > 1) {
        struct net_buf_simple_state state;
        uint8_t len, type;
        
        len = net_buf_simple_pull_u8(buf);
        /* Check for early termination */
        if (len == 0) {
            return;
        }
        
        if (len > buf->len || buf->len < 1) {
            return;
        }
        
        net_buf_simple_save(buf, &state);
        type = net_buf_simple_pull_u8(buf);
        buf->len = len - 1;
        
        switch (type) {
            case BT_DATA_MESH_MESSAGE:
                pass_to_umesh(buf, addr, rssi);
                break;
#if defined(CONFIG_BT_MESH_PB_ADV)
            case BT_DATA_MESH_PROV:
                bt_mesh_pb_adv_recv(buf);
                break;
#endif
            case BT_DATA_MESH_BEACON:
                bt_mesh_beacon_recv(buf);
                break;
            default:
                break;
        }

        net_buf_simple_restore(buf, &state);
        net_buf_simple_pull(buf, len);
    }
}

static int esp_bt_mesh_scan_enable()
{
    struct bt_le_scan_param scan_param =
    {
        .type       = BT_HCI_LE_SCAN_PASSIVE,
        .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = BT_MESH_SCAN_INTERVAL,
        .window     = BT_MESH_SCAN_WINDOW,
    };

    return bt_le_scan_start(&scan_param, esp_bt_mesh_scan_cb);
}

static int esp_bt_mesh_scan_disable()
{
    return bt_le_scan_stop();
}

static void bt_ready(int err)
{
    (void)err;
    printk("bt enable ready.\n");
}

extern int hci_driver_init(void);
static int esp_bt_mesh_init(umesh_hal_module_t *module, void *config)
{
    int error;
    struct net_buf *rsp;
    struct bt_hci_rp_read_bd_addr *rp;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    g_hal_priv = priv;

    // register hci drv
    hci_driver_init();

    // enable bt stack
    bt_enable(bt_ready);

    // init adv context
    bt_mesh_adv_init();

    // read public device address from controller
    error = bt_hci_cmd_send_sync(BT_HCI_OP_READ_BD_ADDR, NULL, &rsp);
    if (error) {
        return error;
    }
    
    rp = (void *)rsp->data;
    memcpy(priv->macaddr, &rp->bdaddr, BLE_MAC_ADDR_SIZE);
    net_buf_unref(rsp);
    return 0;
}

static int esp_bt_mesh_enable(umesh_hal_module_t *module)
{
    esp_bt_mesh_scan_enable();
    return 0;
}

static int esp_bt_mesh_disable(umesh_hal_module_t *module)
{
    esp_bt_mesh_scan_disable();
    return 0;
}

static int esp_bt_mesh_send_ucast(umesh_hal_module_t *module,
                                  frame_t *frame, mac_address_t *dest,
                                  umesh_handle_sent_ucast_t sent,
                                  void *context)
{
    (void)dest;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    
    printk("umesh sends ucast\n");

    if (frame == NULL) {
        return -1;
    }
    
    if (frame->len > priv->mtu) {
        return -2;
    }
    
    g_send_ucast_cxt.context = context;
    g_send_ucast_cxt.sent = sent;
    g_send_ucast_cxt.frame = frame;

    return send_frame(module, frame, &g_send_ucast_cxt);
}

static int esp_bt_mesh_send_bcast(umesh_hal_module_t *module,
                                  frame_t *frame,
                                  umesh_handle_sent_bcast_t sent,
                                  void *context)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    
    printk("umesh sends bcast\n");

    if (frame == NULL) {
        return -1;
    }
    
    if (frame->len > priv->mtu) {
        return -2;
    }

    g_send_bcast_cxt.context = context;
    g_send_bcast_cxt.sent = sent;
    g_send_bcast_cxt.frame = frame;
    
    return send_frame(module, frame, &g_send_bcast_cxt);
}

static int esp_bt_mesh_register_receiver(umesh_hal_module_t *module,
                                         umesh_handle_received_frame_t received,
                                         void *context)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    
    if (received == NULL) {
        return -1;
    }
    
    priv->rxcb = received;
    priv->context = context;
    return 0;
}

static int esp_bt_mesh_get_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    return priv->mtu;
}

static const mac_address_t *esp_bt_mesh_get_mac_address(umesh_hal_module_t *module)
{
    static mac_address_t addr;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    
    memcpy(addr.addr, priv->macaddr, BLE_MAC_ADDR_SIZE);
    addr.len = BLE_MAC_ADDR_SIZE;
    return &addr;
}

static int esp_bt_mesh_get_channel(umesh_hal_module_t *module)
{
    (void)module;
    return 0;
}

static int esp_bt_mesh_set_channel(umesh_hal_module_t *module,
                                    uint8_t channel)
{
    (void)module;
    (void)channel;
    return 0;
}

static int esp_bt_mesh_get_channel_list(umesh_hal_module_t *module,
                                        const uint8_t **chnlist)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    if (chnlist == NULL) {
        return -1;
    }
    
    *chnlist = priv->channels;
    return priv->chn_num;
}

static int esp_bt_mesh_set_extnetid(umesh_hal_module_t *module,
                                    const umesh_extnetid_t *extnetid)
{
    (void)module;
    (void)extnetid;
    return 0;
}

static void esp_bt_mesh_get_extnetid(umesh_hal_module_t *module,
                                     umesh_extnetid_t *extnetid)
{
    (void)module;
    (void)extnetid;
    return;
}

static int esp_bt_mesh_radio_wakeup(struct umesh_hal_module_s *module)
{
    (void)module;
    return 0;
}

static int esp_bt_mesh_radio_sleep(struct umesh_hal_module_s *module)
{
    (void)module;
    return 0;
}

static const frame_stats_t *esp_bt_mesh_get_stats(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return &priv->stats;
}

static umesh_hal_module_t esp_bt_mesh_module = {
    .base.name = "esp32_mesh_bt_module",
    .base.priv_dev = &g_ble_priv,
    .type = MEDIA_TYPE_BLE,
    .umesh_hal_init = esp_bt_mesh_init,
    .umesh_hal_enable = esp_bt_mesh_enable,
    .umesh_hal_disable = esp_bt_mesh_disable,
    .umesh_hal_send_ucast_request = esp_bt_mesh_send_ucast,
    .umesh_hal_send_bcast_request = esp_bt_mesh_send_bcast,
    .umesh_hal_register_receiver = esp_bt_mesh_register_receiver,
    .umesh_hal_get_bcast_mtu = esp_bt_mesh_get_mtu,
    .umesh_hal_get_ucast_mtu = esp_bt_mesh_get_mtu,
    .umesh_hal_get_mac_address = esp_bt_mesh_get_mac_address,
    .umesh_hal_get_channel = esp_bt_mesh_get_channel,
    .umesh_hal_set_channel = esp_bt_mesh_set_channel,
    .umesh_hal_set_extnetid = esp_bt_mesh_set_extnetid,
    .umesh_hal_get_extnetid = esp_bt_mesh_get_extnetid,
    .umesh_hal_get_stats = esp_bt_mesh_get_stats,
    .umesh_hal_get_chnlist = esp_bt_mesh_get_channel_list,
    .umesh_hal_radio_wakeup = esp_bt_mesh_radio_wakeup,
    .umesh_hal_radio_sleep = esp_bt_mesh_radio_sleep,
};

void esp_bt_mesh_register(void)
{
#ifdef CONFIG_AOS_MESH
    hal_umesh_register_module(&esp_bt_mesh_module);
#endif
}

#endif // #ifdef CONFIG_AOS_MESH
