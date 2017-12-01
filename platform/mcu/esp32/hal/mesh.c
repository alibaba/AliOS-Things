/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <aos/network.h>

#include "hal/wifi.h"
#include "umesh_hal.h"
#include "umesh_80211.h"

#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_wifi_internal.h"
#include "esp_event_loop.h"

enum {
    WIFI_MESH_OFFSET = 32,
    WIFI_DST_OFFSET = 4,
    WIFI_SRC_OFFSET = 10,
    WIFI_BSSID_OFFSET = 16,
    WIFI_MAC_ADDR_SIZE = 6,
    WIFI_FCS_SIZE = 4,

    DEFAULT_MTU_SIZE = 512,
};

typedef struct {
    uint32_t u_mtu;
    uint32_t b_mtu;
    uint8_t channel;
    uint8_t chn_num;
    const uint8_t *channels;
    umesh_handle_received_frame_t rxcb;

    void *context;
    umesh_hal_module_t *module;
    mesh_key_t keys[2];
    unsigned char bssid[WIFI_MAC_ADDR_SIZE];
    unsigned char macaddr[WIFI_MAC_ADDR_SIZE];

    frame_stats_t stats;
} mesh_hal_priv_t;

umesh_hal_module_t esp32_wifi_mesh_module;
static mesh_hal_priv_t *g_hal_priv;

typedef struct send_ext_s {
    void *context;
    void *sent;
    frame_t *frame;
} send_cxt_t;

static send_cxt_t g_send_ucast_cxt;
static send_cxt_t g_send_bcast_cxt;

typedef struct {
    frame_t frm;
    frame_info_t fino;
    mesh_hal_priv_t *priv;
} compound_msg_t;

/* WiFi HAL */
static int esp32_wifi_mesh_init(umesh_hal_module_t *module, void *config)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t esp_err;
    bool enable;

    g_hal_priv = priv;
    esp_err = esp_wifi_get_promiscuous(&enable);
    if (esp_err != ESP_OK) {
        ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    }
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, priv->macaddr));
    return 0;
}

static void pass_to_umesh(void* arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;
    frame_t *frm = &cmsg->frm;
    frame_info_t *fino = &cmsg->fino;
    mesh_hal_priv_t *priv = cmsg->priv;

    if (priv->rxcb) {
        priv->rxcb(priv->context, frm, fino, 0);
    }

    aos_free(cmsg);
}

bool esp32_is_mesh_pkt(void *buf, wifi_promiscuous_pkt_type_t type)
{
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    mesh_hal_priv_t *priv = g_hal_priv;
    umesh_hal_module_t *module = priv->module;

    if (type != WIFI_PKT_DATA)
        return false;

    if (pkt->rx_ctrl.sig_len < 40) {
        return false;
    }

    if (umesh_80211_filter_frame(module, pkt->payload, pkt->rx_ctrl.sig_len)) {
        return false;
    }
    return true;
}

void mesh_promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{
    compound_msg_t *pf;
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    mesh_hal_priv_t *priv = g_hal_priv;
    umesh_hal_module_t *module = priv->module;

    if (type != WIFI_PKT_DATA)
        return;

    if (umesh_80211_filter_frame(module, pkt->payload, pkt->rx_ctrl.sig_len)) {
        return;
    }

    pf = aos_malloc(sizeof(*pf) + pkt->rx_ctrl.sig_len - WIFI_MESH_OFFSET - WIFI_FCS_SIZE);
    bzero(pf, sizeof(*pf));
    pf->frm.len = pkt->rx_ctrl.sig_len - WIFI_MESH_OFFSET - WIFI_FCS_SIZE;
    pf->frm.data = (void *)(pf + 1);
    memcpy(pf->fino.peer.addr, pkt->payload + WIFI_SRC_OFFSET, WIFI_MAC_ADDR_SIZE);
    pf->fino.peer.len = 8;

    module = hal_umesh_get_default_module();
    pf->fino.channel = hal_umesh_get_channel(module);
    pf->fino.rssi = pkt->rx_ctrl.rssi;

    pf->priv = priv;
    memcpy(pf->frm.data, pkt->payload + WIFI_MESH_OFFSET, pf->frm.len);

    priv->stats.in_frames++;
    aos_schedule_call(pass_to_umesh, pf);
}

static int esp32_wifi_mesh_enable(umesh_hal_module_t *module)
{
    bool enable;
    esp_err_t esp_err;
    uint8_t mac[6];

    esp_err = esp_wifi_get_promiscuous(&enable);
    if (enable == false) {
        ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscous_autoack(true, mac));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(1));
        ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(mesh_promiscuous_rx_cb));
    }
    return 0;
}

static int esp32_wifi_mesh_disable(umesh_hal_module_t *module)
{
    return 0;
}


static int send_frame(umesh_hal_module_t *module, frame_t *frame,
                      mac_address_t *dest, send_cxt_t *cxt)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    uint8_t *pkt;
    int len = frame->len + WIFI_MESH_OFFSET;
    umesh_handle_sent_ucast_t sent;
    int result = 0;
    esp_err_t esp_err;

    pkt = aos_malloc(len);
    if (pkt == NULL) {
        result = 1;
        goto tx_exit;
    }
    umesh_80211_make_frame(module, frame, dest, pkt);

    extern esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len);
    esp_err = esp_wifi_80211_tx(ESP_IF_WIFI_STA, pkt, len);

    if (esp_err != ESP_OK) {
        result = 1;
    } else {
        priv->stats.out_frames++;
    }
    aos_free(pkt);

tx_exit:
    if (cxt) {
        sent = cxt->sent;
        (*sent)(cxt->context, cxt->frame, result);
    }

    return 0;
}

static int esp32_wifi_mesh_send_ucast(umesh_hal_module_t *module,
                                    frame_t *frame, mac_address_t *dest,
                                    umesh_handle_sent_ucast_t sent,
                                    void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if(frame == NULL) {
        return -1;
    }

    if(frame->len > priv->u_mtu) {
        return -2;
    }

    g_send_ucast_cxt.context = context;
    g_send_ucast_cxt.sent = sent;
    g_send_ucast_cxt.frame = frame;
    error = send_frame(module, frame, dest, &g_send_ucast_cxt);
    return error;
}

static int esp32_wifi_mesh_send_bcast(umesh_hal_module_t *module,
                                      frame_t *frame,
                                      umesh_handle_sent_bcast_t sent,
                                      void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    mac_address_t dest;

    if(frame == NULL) {
        return -1;
    }

    if(frame->len > priv->b_mtu) {
        return -2;
    }

    g_send_bcast_cxt.context = context;
    g_send_bcast_cxt.sent = sent;
    g_send_bcast_cxt.frame = frame;

    dest.len = 8;
    memset(dest.addr, 0xff, sizeof(dest.addr));
    error = send_frame(module, frame, &dest, &g_send_bcast_cxt);
    return error;
}

static int esp32_wifi_mesh_register_receiver(umesh_hal_module_t *module,
                                    umesh_handle_received_frame_t received,
                                    void *context)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if(received == NULL) {
        return -1;
    }

    priv->rxcb = received;
    priv->context = context;
    return 0;
}

static int esp32_wifi_mesh_get_mtu(umesh_hal_module_t *module)
{
    return DEFAULT_MTU_SIZE;
}

static const mac_address_t *esp32_wifi_mesh_get_mac_address(umesh_hal_module_t *module)
{
    static mac_address_t addr;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, priv->macaddr));
    memcpy(addr.addr, priv->macaddr, WIFI_MAC_ADDR_SIZE);
    addr.len = 8;
    return &addr;
}

static int esp32_wifi_mesh_get_channel(umesh_hal_module_t *module)
{
    uint8_t primary;
    wifi_second_chan_t second;

    esp_wifi_get_channel(&primary, &second);
    return (int)primary;
}

static int esp32_wifi_mesh_set_channel(umesh_hal_module_t *module,
                                  uint8_t channel)
{
    if (channel < 1 || channel > 13) {
        return -1;
    }

    ESP_ERROR_CHECK(esp_wifi_set_channel(channel, 0));
    return 0;
}

static int esp32_wifi_mesh_get_channel_list(umesh_hal_module_t *module,
                                            const uint8_t **chnlist)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    if (chnlist == NULL) {
        return -1;
    }

    *chnlist = priv->channels;
    return priv->chn_num;
}

static int esp32_wifi_mesh_set_extnetid(umesh_hal_module_t *module,
                                   const umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (extnetid == NULL) {
        return -1;
    }
    memcpy(priv->bssid, extnetid->netid, WIFI_MAC_ADDR_SIZE);
    return 0;
}

void esp32_wifi_mesh_get_extnetid(umesh_hal_module_t *module,
                                  umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (extnetid == NULL) {
        return;
    }
    extnetid->len = WIFI_MAC_ADDR_SIZE;
    memcpy(extnetid->netid, priv->bssid, extnetid->len);
}

static const frame_stats_t *esp32_wifi_mesh_get_stats(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return &priv->stats;
}

static const uint8_t g_wifi_channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static mesh_hal_priv_t g_wifi_priv = {
    .u_mtu = DEFAULT_MTU_SIZE,
    .b_mtu = DEFAULT_MTU_SIZE,
    .channel = 0,
    .chn_num = sizeof(g_wifi_channels),
    .channels = g_wifi_channels,
    .module = &esp32_wifi_mesh_module,
    .bssid = {0xb0, 0xf8, 0x93, 0x00, 0x00, 0x07},
};

umesh_hal_module_t esp32_wifi_mesh_module = {
    .base.name = "esp32_mesh_wifi_module",
    .base.priv_dev = &g_wifi_priv,
    .type = MEDIA_TYPE_WIFI,
    .umesh_hal_init = esp32_wifi_mesh_init,
    .umesh_hal_enable = esp32_wifi_mesh_enable,
    .umesh_hal_disable = esp32_wifi_mesh_disable,
    .umesh_hal_send_ucast_request = esp32_wifi_mesh_send_ucast,
    .umesh_hal_send_bcast_request = esp32_wifi_mesh_send_bcast,
    .umesh_hal_register_receiver = esp32_wifi_mesh_register_receiver,
    .umesh_hal_get_bcast_mtu = esp32_wifi_mesh_get_mtu,
    .umesh_hal_get_ucast_mtu = esp32_wifi_mesh_get_mtu,
    .umesh_hal_get_mac_address = esp32_wifi_mesh_get_mac_address,
    .umesh_hal_get_channel = esp32_wifi_mesh_get_channel,
    .umesh_hal_set_channel = esp32_wifi_mesh_set_channel,
    .umesh_hal_set_extnetid = esp32_wifi_mesh_set_extnetid,
    .umesh_hal_get_extnetid = esp32_wifi_mesh_get_extnetid,
    .umesh_hal_get_stats = esp32_wifi_mesh_get_stats,
    .umesh_hal_get_chnlist = esp32_wifi_mesh_get_channel_list,
};

void esp32_wifi_mesh_register(void)
{
    hal_umesh_register_module(&esp32_wifi_mesh_module);
}
