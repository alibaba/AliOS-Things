/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <netinet/ether.h>
#include <pthread.h>
#include <signal.h>
#include "aos/kernel.h"

#include <cpu_event.h>
#include <umesh_hal.h>
#include <umesh_80211.h>

#define DEFAULT_MTU_SIZE 1024

typedef struct {
    uint32_t u_mtu;
    uint32_t b_mtu;
    uint8_t channel;
    uint8_t chn_num;
    const uint8_t *channels;
    umesh_handle_received_frame_t rxcb;

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

static void linuxhost_mesh_recv(frame_t *frm,
                                frame_info_t *fino, void *cb_data)
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
#define MESH_SSID_OFF 30
#define MESH_PAYLOAD_OFF 46
#define MESH_SRC_OFF 24
static void *wifi_recv_entry(void *arg)
{
    mesh_hal_priv_t *priv = (mesh_hal_priv_t *)arg;
    int NumTotalPkts;
    int sockfd;
    int sockopt;
    ssize_t numbytes;
    uint8_t buf[2048];
    umesh_extnetid_t extnetid;
    const mac_address_t *mymac;

    memset(buf, 0, sizeof(buf));
    mymac = hal_umesh_get_mac_address(priv->module);

    // create layer2 raw socket
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        printf("listener: create socket error\n");
        return NULL;
    }

#if 0
    struct ifreq ifopts;
    // set interface to promiscuous mode
    strncpy(ifopts.ifr_name, "wlan0", IFNAMSIZ-1);
    ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
#endif

    // allow the socket to be reused - in case connection is closed prematurely
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
        printf("listener: set sockopt error\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // bind to device
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, "wlan0", IFNAMSIZ-1) == -1)  {
        printf("listener: bind device error\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    NumTotalPkts = 0;

    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);
    while(1) {
        numbytes = recvfrom(sockfd, buf, 2048, 0, NULL, NULL);
        if (numbytes < 0) {
            printf("Recvfrom error , failed to get packets\n");
            goto thr_exit;
        }

        // filter for umesh
        // drop if received packet size is too small
        if (numbytes <= MESH_PAYLOAD_OFF) {
            continue;
        }

        // drop if receiving the packet sent by itself
        if (memcmp(buf + MESH_SRC_OFF, mymac->addr, 6) == 0) {
            continue;
        }

        // drop if the ssid doesn't match
        hal_umesh_get_extnetid(priv->module, &extnetid);
        if (memcmp(buf + MESH_SSID_OFF, extnetid.netid, 6)) {
            continue;
        }

#if 0
        printf("process the packet: %d\n", numbytes);
        for (int i = 0; i < numbytes; i++)
            printf("%02x:", buf[i]);
        printf("\n");
#endif

        NumTotalPkts++;
        compound_msg_t *pf;
        pf = cpu_event_malloc(sizeof(*pf) + numbytes - MESH_PAYLOAD_OFF);
        bzero(pf, sizeof(*pf));
        pf->frm.len = numbytes - MESH_PAYLOAD_OFF;
        pf->frm.data = (void *)(pf + 1);
        pf->fino.channel = priv->channel;
        memcpy(pf->fino.peer.addr, buf + MESH_SRC_OFF, 6);
        pf->fino.peer.len = 8;
        pf->priv = priv;
        memcpy(pf->frm.data, buf + MESH_PAYLOAD_OFF, pf->frm.len);
        cpu_call_handler(pass_to_urmesh, pf);
    }

thr_exit:
    close(sockfd);
    return NULL;
}

static int linux_80211_mesh_init(umesh_hal_module_t *module, void *something)
{
    int sockfd;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    // create socket to fetch mac addr
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        printf("fail to create raw socket\n");
        return -1;
    }

    // get interface mac addr
    struct ifreq ifreq_c;
    memset(&ifreq_c, 0, sizeof(ifreq_c));
    strncpy(ifreq_c.ifr_name, "wlan0", IFNAMSIZ-1);
    if ((ioctl(sockfd, SIOCGIFHWADDR, &ifreq_c)) < 0) {
        printf("fail to get interface mac addr\n");
    }
    memcpy(priv->macaddr, ifreq_c.ifr_ifru.ifru_hwaddr.sa_data, 6);

    // set interface to promiscuous mode
    struct ifreq ifopts;
    strncpy(ifopts.ifr_name, "wlan0", IFNAMSIZ-1);
    ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifopts) < 0) {
        printf("fail to set promiscuous mode\n");
    }
    else {
        printf("success to set promiscuous mode\n");
    }

    pthread_t th;
    pthread_create(&th, NULL, wifi_recv_entry, priv);

    close(sockfd);
    return 0;
}

static int send_frame(umesh_hal_module_t *module, frame_t *frame, mac_address_t *dest)
{
    void *pkt = NULL;
    int ret;
    int sockfd;
    int tx_len = 0;
    int count = frame->len + MESH_DATA_OFF;
    char datagram[2048];
    struct ifreq if_idx;
    struct ether_header *eh;
    struct sockaddr_ll socket_address;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    memset(datagram, 0, 2048);
    eh = (struct ether_header *)datagram;

    // create layer2 raw socket
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        printf("sender: create raw socket error\n");
        ret = -1;
        goto out;
    }

    pkt = aos_malloc(count);
    if (pkt == NULL) {
        ret = -1;
        goto out;
    }

    // make up 80211 mesh packet
    ret = umesh_80211_make_frame(module, frame, dest, pkt);
    if (ret < 0) {
        goto out;
    }

    // get the index of the interface to send on
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, "wlan0", IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        printf("sender: get index of interface error\n");
        ret = -1;
        goto out;
    }

    // construct the ethernet header
    eh->ether_shost[0] = priv->macaddr[0];
    eh->ether_shost[1] = priv->macaddr[1];
    eh->ether_shost[2] = priv->macaddr[2];
    eh->ether_shost[3] = priv->macaddr[3];
    eh->ether_shost[4] = priv->macaddr[4];
    eh->ether_shost[5] = priv->macaddr[5];
    eh->ether_dhost[0] = dest->addr[0];
    eh->ether_dhost[1] = dest->addr[1];
    eh->ether_dhost[2] = dest->addr[2];
    eh->ether_dhost[3] = dest->addr[3];
    eh->ether_dhost[4] = dest->addr[4];
    eh->ether_dhost[5] = dest->addr[5];

    // ethertype field
    eh->ether_type = htons(ETH_P_ALL);
    tx_len += sizeof(struct ether_header);

    // umesh packet
    memcpy(datagram + tx_len, pkt, count);
    tx_len += count;

    // index of the network device
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    // address length
    socket_address.sll_halen = ETH_ALEN;
    // destination MAC
    socket_address.sll_addr[0] = dest->addr[0];
    socket_address.sll_addr[1] = dest->addr[0];
    socket_address.sll_addr[2] = dest->addr[0];
    socket_address.sll_addr[3] = dest->addr[0];
    socket_address.sll_addr[4] = dest->addr[0];
    socket_address.sll_addr[5] = dest->addr[0];

    // allow broadcast
    //int broadcast = 1;
    //if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
    //    printf("setsockopt (SO_BROADCAST)\n");
    //    ret = -1;
    //    goto out;
    //}

    // send packet
    if (sendto(sockfd, datagram, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
        printf("Send failed\n");
        ret = -1;
        goto out;
    }
    //Data send successfully
#if 0
    else
    {
        printf("Packet Send Length : %d\n" , tx_len);
        for (int i = 0; i < tx_len; i++) {
            printf("%02x:", datagram[i]);
        }
        printf("\n");
    }
#endif
    priv->stats.out_frames++;

out:
    aos_free(pkt);
    close(sockfd);
    return ret;
}

static int linux_80211_mesh_send_ucast(umesh_hal_module_t *module,
                                       frame_t *frame, mac_address_t *dest,
                                       umesh_handle_sent_ucast_t sent,
                                       void *context)
{
    int error;
    mesh_hal_priv_t *priv = module->base.priv_dev;

    if (frame == NULL) {
        printf("frame is NULL, cannot proceed\n");
        return -1;
    }

    if (frame->len > priv->u_mtu) {
        printf("frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed\n", frame->len, priv->u_mtu);
        return -2;
    }

    frame->key_index = -1;
    error = send_frame(module, frame, dest);
    if (sent) {
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

    if (frame == NULL) {
        printf("frame is NULL, cannot proceed\n");
        return -1;
    }

    if (frame->len > priv->b_mtu) {
        printf("frame->len(%d) > MAX_FRAME_SIZE(%d), will not proceed\n", frame->len, priv->u_mtu);
        return -2;
    }

    dest.len = 8;
    memset(dest.addr, 0xff, sizeof(dest.addr));
    error = send_frame(module, frame, &dest);
    if (sent) {
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

    if (received == NULL)
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
    return priv->channel;
}

static int linux_80211_mesh_hal_set_channel(umesh_hal_module_t *module, uint8_t channel)
{
    mesh_hal_priv_t *priv = module->base.priv_dev;

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
//static const uint8_t g_wifi_channels[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13};
static const uint8_t g_wifi_channels[] = {1, 6, 11}; // send 3 discovery requests
static mesh_hal_priv_t wifi_priv = {
    .u_mtu = DEFAULT_MTU_SIZE,
    .b_mtu = DEFAULT_MTU_SIZE,
    .channel = 1,
    .chn_num = sizeof(g_wifi_channels),
    .channels = g_wifi_channels,
    .module = &linux_80211_mesh_wifi_module,
    .bssid = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5},
};

static umesh_hal_module_t linux_80211_mesh_wifi_module = {
    .base.name = "rtl_80211_mesh_wifi_module",
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
