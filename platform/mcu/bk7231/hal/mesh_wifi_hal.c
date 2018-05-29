/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef CONFIG_AOS_MESH

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "umesh_hal.h"
#include <umesh_80211.h>
#include <hal/wifi.h>

static inline uint8_t nxmac_dsss_max_pwr_level_getf(void)
{
    uint32_t localVal = bk_wlan_max_power_level_get();
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

struct tx_policy_tbl
{
    uint32_t upatterntx;
    uint32_t phycntrlinfo1;
    uint32_t phycntrlinfo2;
    uint32_t maccntrlinfo1;
    uint32_t maccntrlinfo2;
    uint32_t ratecntrlinfo[4];
    uint32_t powercntrlinfo[4];
};

struct txl_buffer_control
{
    struct tx_policy_tbl policy_tbl;
    uint32_t mac_control_info;
    uint32_t phy_control_info;
    uint32_t status;
};

extern struct txl_buffer_control txl_buffer_control_24G;

enum {
    WIFI_MESH_OFFSET = 32,
    WIFI_DST_OFFSET = 4,
    WIFI_SRC_OFFSET = 10,
    WIFI_BSSID_OFFSET = 16,

    WIFI_MAC_ADDR_SIZE = 6,
};

enum {
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
    unsigned char bssid[WIFI_MAC_ADDR_SIZE];
    unsigned char macaddr[WIFI_MAC_ADDR_SIZE];

    frame_stats_t stats;
} mesh_hal_priv_t;

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

typedef struct mac_entry_s {
    uint64_t mactime;
    uint16_t last_seq;
    uint8_t  macaddr[6];
} mac_entry_t;

enum {
    ENT_NUM = 32,
};

static mac_entry_t entries[ENT_NUM];
static mesh_hal_priv_t *g_hal_priv;
static bool g_radio_wakeup = true;

extern int bk_wlan_monitor_enabled(void);
static void pass_to_umesh(const void* arg)
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

static void wifi_monitor_cb(uint8_t *data, int len,
                            hal_wifi_link_info_t *info)
{
    compound_msg_t *pf;
    mesh_hal_priv_t *priv = g_hal_priv;
    umesh_hal_module_t *module = priv->module;

    if (umesh_80211_filter_frame(module, data, len)) {
        return;
    }

#ifdef AOS_DEBUG_MESH
    dump_packet(true, data, len);
#endif

    pf = aos_malloc(sizeof(*pf) + len - WIFI_MESH_OFFSET);
    bzero(pf, sizeof(*pf));
    pf->frm.len = len - WIFI_MESH_OFFSET;
    pf->frm.data = (void *)(pf + 1);
    memcpy(pf->fino.peer.addr, data + WIFI_SRC_OFFSET, WIFI_MAC_ADDR_SIZE);
    pf->fino.peer.len = 8;

    module = hal_umesh_get_default_module();
    pf->fino.channel = hal_umesh_get_channel(module);
    pf->fino.rssi = info->rssi;

    pf->priv = priv;
    memcpy(pf->frm.data, data + WIFI_MESH_OFFSET, pf->frm.len);

    priv->stats.in_frames++;
    pass_to_umesh(pf);
}

static int beken_wifi_mesh_init(umesh_hal_module_t *module, void *config)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    g_hal_priv = priv;
    bk_wifi_get_mac_address(priv->macaddr);
    return 0;
}

static int beken_wifi_mesh_enable(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    struct tx_policy_tbl *pol;

    if (wlan_get_mesh_condition() == 0 && bk_wlan_is_monitor_mode() == 0) {
        bk_wlan_start_monitor();
        hal_machw_exit_monitor_mode();

        tpc_update_tx_power(10);
        pol = &txl_buffer_control_24G.policy_tbl;
        pol->powercntrlinfo[0] = nxmac_dsss_max_pwr_level_getf();
    }

    wlan_register_mesh_monitor_cb(wifi_monitor_cb);
    wlan_set_mesh_bssid(priv->bssid);
    return 0;
}

static int beken_wifi_mesh_disable(umesh_hal_module_t *module)
{
    wlan_register_mesh_monitor_cb(NULL);
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

    if (g_radio_wakeup == false) {
        return -2;
    }

    pkt = aos_malloc(len);
    if (pkt == NULL) {
        result = 1;
        goto tx_exit;
    }
    umesh_80211_make_frame(module, frame, dest, pkt);

    if (bmsg_tx_raw_sender(pkt, len) != 0) {
        result = 1;
    } else {
        priv->stats.out_frames++;
    }

tx_exit:
    if (cxt) {
        sent = cxt->sent;
        (*sent)(cxt->context, cxt->frame, result);
    }

    return 0;
}

static int beken_wifi_mesh_send_ucast(umesh_hal_module_t *module,
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

static int beken_wifi_mesh_send_bcast(umesh_hal_module_t *module,
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

static int beken_wifi_mesh_register_receiver(umesh_hal_module_t *module,
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

static int beken_wifi_mesh_get_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    return priv->u_mtu;
}

static const mac_address_t *beken_wifi_mesh_get_mac_address(
                                         umesh_hal_module_t *module)
{
    static mac_address_t addr;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(addr.addr, priv->macaddr, WIFI_MAC_ADDR_SIZE);
    addr.len = 8;
    return &addr;
}

static int beken_wifi_mesh_get_channel(umesh_hal_module_t *module)
{
    int channel;

    channel = bk_wlan_get_channel();
    return channel;
}

extern uint8_t umesh_get_mode(void);
static int beken_wifi_mesh_set_channel(umesh_hal_module_t *module,
                                       uint8_t channel)
{
    uint8_t chan;

    if (channel < 1 || channel > 14) {
        return -1;
    }
	if (umesh_get_mode() & MODE_LEADER) {
        return 0;
	}

	chan = bk_wlan_get_channel();
	if(chan != channel)
	{
		bk_wlan_set_channel(channel);
	}
    return 0;
}

static int beken_wifi_mesh_get_channel_list(umesh_hal_module_t *module,
                                            const uint8_t **chnlist)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    if (chnlist == NULL) {
        return -1;
    }

    *chnlist = priv->channels;
    return priv->chn_num;
}

int beken_wifi_mesh_set_extnetid(umesh_hal_module_t *module,
                                 const umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(priv->bssid, extnetid->netid, WIFI_MAC_ADDR_SIZE);
    wlan_set_mesh_bssid(priv->bssid);
    return 0;
}

void beken_wifi_mesh_get_extnetid(umesh_hal_module_t *module,
                                  umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (extnetid == NULL) {
        return;
    }
    extnetid->len = WIFI_MAC_ADDR_SIZE;
    memcpy(extnetid->netid, priv->bssid, extnetid->len);
}

int beken_wifi_mesh_radio_wakeup(struct umesh_hal_module_s *module)
{
#ifdef CONFIG_AOS_MESH_LOWPOWER
    g_radio_wakeup = true;
    if (bk_wlan_is_ap() == 0 && bk_wlan_is_sta() == 0 && bk_wlan_monitor_enabled() == 0) {
        bk_wlan_start_monitor();
        hal_machw_exit_monitor_mode();
    }
#endif
    return 0;
}

int beken_wifi_mesh_radio_sleep(struct umesh_hal_module_s *module)
{
#ifdef CONFIG_AOS_MESH_LOWPOWER
    g_radio_wakeup = false;
    bk_wlan_suspend();
#endif
    return 0;
}

static const frame_stats_t *beken_wifi_mesh_get_stats(
                                         umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return &priv->stats;
}

static umesh_hal_module_t beken_wifi_mesh_module;
static const uint8_t g_wifi_channels[] = {1,2,3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static mesh_hal_priv_t g_wifi_priv = {
    .u_mtu = DEFAULT_MTU_SIZE,
    .b_mtu = DEFAULT_MTU_SIZE,
    .channel = 0,
    .chn_num = sizeof(g_wifi_channels),
    .channels = g_wifi_channels,
    .module = &beken_wifi_mesh_module,
    .bssid = {0xb0, 0xf8, 0x93, 0x00, 0x00, 0x07},
};

static umesh_hal_module_t beken_wifi_mesh_module = {
    .base.name = "beken7231_mesh_wifi_module",
    .base.priv_dev = &g_wifi_priv,
    .type = MEDIA_TYPE_WIFI,
    .umesh_hal_init = beken_wifi_mesh_init,
    .umesh_hal_enable = beken_wifi_mesh_enable,
    .umesh_hal_disable = beken_wifi_mesh_disable,
    .umesh_hal_send_ucast_request = beken_wifi_mesh_send_ucast,
    .umesh_hal_send_bcast_request = beken_wifi_mesh_send_bcast,
    .umesh_hal_register_receiver = beken_wifi_mesh_register_receiver,
    .umesh_hal_get_bcast_mtu = beken_wifi_mesh_get_mtu,
    .umesh_hal_get_ucast_mtu = beken_wifi_mesh_get_mtu,
    .umesh_hal_get_mac_address = beken_wifi_mesh_get_mac_address,
    .umesh_hal_get_channel = beken_wifi_mesh_get_channel,
    .umesh_hal_set_channel = beken_wifi_mesh_set_channel,
    .umesh_hal_set_extnetid = beken_wifi_mesh_set_extnetid,
    .umesh_hal_get_extnetid = beken_wifi_mesh_get_extnetid,
    .umesh_hal_get_stats = beken_wifi_mesh_get_stats,
    .umesh_hal_get_chnlist = beken_wifi_mesh_get_channel_list,
    .umesh_hal_radio_wakeup = beken_wifi_mesh_radio_wakeup,
    .umesh_hal_radio_sleep = beken_wifi_mesh_radio_sleep,
};
#endif

void beken_wifi_mesh_register(void)
{
#ifdef CONFIG_AOS_MESH
    hal_umesh_register_module(&beken_wifi_mesh_module);
#endif
}
