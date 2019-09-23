/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * mesh hal impl.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>

#include "aos/kernel.h"

#include <cpu_event.h>
#include <umesh_hal.h>
#include <umesh_80211.h>

#include "osdep.h"

#define MYTAG "DDA_HAL"
#define DEFAULT_MTU_SIZE 1024

#ifndef LINUX_WIFI_MESH_IF_NAME
#define LINUX_WIFI_MESH_IF_NAME "mon0"
#endif

typedef struct {
    uint32_t u_mtu;
    uint32_t b_mtu;
    uint8_t channel;
    uint8_t chn_num;
    const uint8_t *channels;
    umesh_handle_received_frame_t rxcb;

    struct wif *wif;

    void *context;
    umesh_hal_module_t *module;
    unsigned char bssid[6];
    unsigned char macaddr[6];

    frame_stats_t stats;
} mesh_hal_priv_t;

typedef struct {
    frame_t frm;
    frame_info_t fino;
    mesh_hal_priv_t *priv;
} compound_msg_t;

static void linuxhost_mesh_recv(frame_t *frm, frame_info_t *fino, void *cb_data)
{
    mesh_hal_priv_t *priv = cb_data;
    uint8_t control;

    if (!priv->rxcb)
        return;

    priv->rxcb(priv->context, frm, fino, 0);
}

static void pass_to_urmesh(const void* arg)
{
    compound_msg_t *cmsg = (compound_msg_t *)arg;

    linuxhost_mesh_recv(&cmsg->frm, &cmsg->fino, cmsg->priv);

    cpu_event_free(cmsg);
}

#define MESH_DATA_OFF 32
static void *wifi_recv_entry(void *arg)
{
    mesh_hal_priv_t *priv = arg;
    fd_set rfds;
    int fd = wi_fd(priv->wif);
    unsigned char pkt[2048];

    while (1) {
        int ret;
        int count = sizeof pkt;
        struct rx_info ri;

        FD_ZERO( &rfds );
        FD_SET( fd, &rfds );

        ret = select(fd + 1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
            continue;

        if (!FD_ISSET(fd, &rfds))
            continue;

        count = wi_read(priv->wif, pkt, count, &ri);
        if (count < 25)
            continue;
        if (umesh_80211_filter_frame(priv->module, pkt, count))
            continue;

        priv->stats.in_frames ++;

        compound_msg_t *pf;
        pf = cpu_event_malloc(sizeof(*pf) + count - MESH_DATA_OFF);
        bzero(pf, sizeof(*pf));
        pf->frm.len = count - MESH_DATA_OFF;
        pf->frm.data = (void *)(pf + 1);
        pf->fino.channel = ri.ri_channel;
        memcpy(pf->fino.peer.addr, pkt + OFF_SRC, 6);
        pf->fino.peer.len = 8;
        pf->priv = priv;
        memcpy(pf->frm.data, pkt + MESH_DATA_OFF, pf->frm.len);
        cpu_call_handler(pass_to_urmesh, pf);
    }

    return NULL;
}

static int linux_80211_mesh_init(umesh_hal_module_t *module, void *something)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    priv->wif = wi_open(LINUX_WIFI_MESH_IF_NAME);
    if (priv->wif == NULL)
        return -1;

    wi_get_mac(priv->wif, priv->macaddr);
    pthread_t th;
    pthread_create(&th, NULL, wifi_recv_entry, priv);
    return 0;
}

static int send_frame(umesh_hal_module_t *module, frame_t *frame, mac_address_t *dest)
{
    void *pkt = NULL;
    int count = frame->len + MESH_DATA_OFF;
    int ret;

    pkt = aos_malloc(count);
    if (pkt == NULL)
        return -1;

    ret = umesh_80211_make_frame(module, frame, dest, pkt);
    if (ret < 0)
        goto out;

    mesh_hal_priv_t *priv = module->base.priv_dev;
    wi_write(priv->wif, pkt, count, NULL);
    priv->stats.out_frames ++;

out:
    aos_free(pkt);
    return ret;
}

static int linux_80211_mesh_send_ucast(umesh_hal_module_t *module, frame_t *frame,
                                   mac_address_t *dest,
                                   umesh_handle_sent_ucast_t sent,
                                   void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if(frame == NULL) {
        LOGE(MYTAG, "frame is NULL, cannot proceed");
        return -1;
    }

    if(frame->len > priv->u_mtu) {
        LOGE(MYTAG, "frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed", frame->len, priv->u_mtu);
        return -2;
    }

    frame->key_index = -1;
    error = send_frame(module, frame, dest);
    if(sent) {
        (*sent)(context, frame, error);
    }
    return error;
}

static int linux_80211_mesh_send_bcast(umesh_hal_module_t *module, frame_t *frame,
                                   umesh_handle_sent_bcast_t sent,
                                   void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;
    mac_address_t dest;

    if(frame == NULL) {
        LOGE(MYTAG, "frame is NULL, cannot proceed");
        return -1;
    }

    if(frame->len > priv->b_mtu) {
        LOGE(MYTAG, "frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed", frame->len, priv->u_mtu);
        return -2;
    }

    dest.len = 8;
    memset(dest.addr, 0xff, sizeof(dest.addr));
    error = send_frame(module, frame, &dest);
    if(sent) {
        (*sent)(context, frame, error);
    }
    return error;
}

static int linux_80211_mesh_get_u_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return priv->u_mtu;
}

static int linux_80211_mesh_get_b_mtu(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return priv->b_mtu;
}

static int linux_80211_mesh_set_rxcb(umesh_hal_module_t *module,
                          umesh_handle_received_frame_t received, void *context)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if(received == NULL)
        return -1;

    priv->rxcb = received;
    priv->context = context;
    return 0;
}

static const mac_address_t *linux_80211_mesh_get_mac_address(
                                        umesh_hal_module_t *module)
{
    static mac_address_t addr;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(addr.addr, priv->macaddr, 6);
    addr.len = 8;
    return &addr;
}

static int linux_80211_mesh_hal_get_channel(umesh_hal_module_t *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    int chan = wi_get_channel(priv->wif);
    return chan < 0 ? priv->channel : chan;
}

static int linux_80211_mesh_hal_set_channel(umesh_hal_module_t *module, uint8_t channel)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    printf("setting channel to %d\n", channel);
    wi_set_channel(priv->wif, channel);
    /* channel will appended to each data packet sent */
    priv->channel = channel;
    return 0;
}

static int linux_80211_mesh_get_channel_list(umesh_hal_module_t *module, const uint8_t **chnlist)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    if (chnlist == NULL) {
        return -1;
    }

    *chnlist = priv->channels;

    return priv->chn_num;
}

static const frame_stats_t *linux_80211_mesh_get_stats(struct umesh_hal_module_s *module)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;
    return &priv->stats;
}

static int linux_80211_wifi_mesh_set_extnetid(umesh_hal_module_t *module,
                                 const umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memcpy(priv->bssid, extnetid->netid, 6);
    return 0;
}

static void linux_80211_wifi_mesh_get_extnetid(umesh_hal_module_t *module,
                                  umesh_extnetid_t *extnetid)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (extnetid == NULL) {
        return;
    }
    extnetid->len = 6;
    memcpy(extnetid->netid, priv->bssid, extnetid->len);
}

static umesh_hal_module_t linux_80211_mesh_wifi_module;
static const uint8_t g_wifi_channels[] = {1, 4, 6, 9, 11};
static mesh_hal_priv_t wifi_priv = {
    .u_mtu = DEFAULT_MTU_SIZE,
    .b_mtu = DEFAULT_MTU_SIZE,
    .channel = 0,
    .chn_num = sizeof(g_wifi_channels),
    .channels = g_wifi_channels,
    .module = &linux_80211_mesh_wifi_module,
    .bssid = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5},
};

static umesh_hal_module_t linux_80211_mesh_wifi_module = {
    .base.name = "linux_80211_mesh_wifi_module",
    .base.priv_dev = &wifi_priv,
    .type = MEDIA_TYPE_WIFI,
    .umesh_hal_init = linux_80211_mesh_init,
    .umesh_hal_send_ucast_request = linux_80211_mesh_send_ucast,
    .umesh_hal_send_bcast_request = linux_80211_mesh_send_bcast,
    .umesh_hal_register_receiver = linux_80211_mesh_set_rxcb,
    .umesh_hal_get_bcast_mtu = linux_80211_mesh_get_b_mtu,
    .umesh_hal_get_ucast_mtu = linux_80211_mesh_get_u_mtu,
    .umesh_hal_get_mac_address = linux_80211_mesh_get_mac_address,
    .umesh_hal_get_channel = linux_80211_mesh_hal_get_channel,
    .umesh_hal_set_channel = linux_80211_mesh_hal_set_channel,
    .umesh_hal_get_chnlist = linux_80211_mesh_get_channel_list,
    .umesh_hal_get_stats = linux_80211_mesh_get_stats,
    .umesh_hal_set_extnetid = linux_80211_wifi_mesh_set_extnetid,
    .umesh_hal_get_extnetid = linux_80211_wifi_mesh_get_extnetid,
};

void linux_wifi_register(void)
{
    hal_umesh_register_module(&linux_80211_mesh_wifi_module);
}
