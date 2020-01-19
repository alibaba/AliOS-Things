/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <network/network.h>
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/ip6_addr.h"
#include "lwip/opt.h"
#include "adapter_lwip.h"
#include "define.h"
#include "hal_wifi.h"


/*
output means lwip to umesh

input means umesh to lwip
*/

#if LWIP_IPV6 
extern const uint8_t BCAST_ADDR[];
extern const uint8_t UMESH_BSSID[];
#ifdef CONFIG_AOS_MESH_TAPIF
static bool is_router;
int umesh_tapif_init(const char *ifname);
void umesh_tapif_deinit(void);
void umesh_tapif_send_pbuf(struct pbuf *pbuf);
#endif

typedef struct lwip_adapter_state_s {
    struct netif adpif;
    //ur_adapter_callback_t adapter_cb;
    const char interface_name[3];
    umesh_state_t *umesh_state;
} lwip_adapter_state_t;

static lwip_adapter_state_t g_la_state = {.interface_name = "um"};

/* Receive IP frame from umesh and pass up to LwIP */
static int umesh_adapter_input(struct pbuf *buf)
{

    if (g_la_state.adpif.input) {
        pbuf_ref(buf);
        g_la_state.adpif.input(buf, &g_la_state.adpif);
        return 0;
    } 
    return UMESH_ERR_NOT_INIT;
}

/*send ip data to lwip, schedule in umesh main task*/
int umesh_adapter_input_buf(void *buf, int len)
{
    int ret = 0;
    struct pbuf *pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (!pbuf) {
        return UMESH_ERR_MALLOC_FAILED;
    }

    pbuf_take(pbuf, buf, len);
    ret = umesh_adapter_input(pbuf);
    // if(ret < 0) {
    //     buf_free(buf);
    // }
    buf_free(buf);
    return ret;
}

static const void *_adapter_get_mcast_ipaddr(void)
{
    struct netif *netif = &g_la_state.adpif;

    static ip6_addr_t mcast;
    uint8_t index = 2;
    const ip6_addr_t *mcast_addr = netif_ip6_addr(netif, index);

    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, index))) {
        IP6_ADDR(&mcast, mcast_addr->addr[0], mcast_addr->addr[1],
                 0, htonl(0xfc));
        return &mcast;
    }

    return NULL;
}

bool umesh_is_mcast_subscribed(const ur_ip6_addr_t *addr)
{
     uint8_t ip6_addr[16] = {0};
     umesh_get_ipv6_mcast(ip6_addr);

    return !memcmp(addr, ip6_addr, sizeof(ur_ip6_addr_t));
}

static int umesh_adapter_ipv6_output(struct netif *netif, struct pbuf *p,
                                    const ip6_addr_t *dest)
{
    int ret = UMESH_ERR_IP_ADDR;


    if (ip6_addr_ismulticast(dest) && umesh_is_mcast_subscribed((ur_ip6_addr_t *)dest)) {

        ret = umesh_recv_ip_data(g_la_state.umesh_state, (const uint8_t *)p->payload, p->len, BCAST_ADDR);
    } else if (ip6_addr_isuniquelocal(dest)) {
        uint8_t dst_addr[IEEE80211_MAC_ADDR_LEN] = {0};
   
        ur_ip6_addr_t *ip6_addr = (ur_ip6_addr_t *)dest;
        
        ret = umesh_resolve_mac_addr(ip6_addr->m8, dst_addr);
        if(ret < 0) {
            pbuf_free(p);
            return ret;
        }

        ret = umesh_recv_ip_data(g_la_state.umesh_state, (const uint8_t *)p->payload, p->len, dst_addr);
    } 

    pbuf_free(p);
    return ret;
 
}

static int umesh_adapter_if_init(struct netif *netif)
{
    netif->name[0] = g_la_state.interface_name[0];
    netif->name[1] = g_la_state.interface_name[1];
    netif->num = g_la_state.interface_name[2] - '0';

    netif->output_ip6 = umesh_adapter_ipv6_output;

   /* netif->output = ur_adapter_ipv4_output;*/
    netif->mtu = 1280;
    netif->flags = NETIF_FLAG_LINK_UP | NETIF_FLAG_UP | NETIF_FLAG_BROADCAST ;
    return UMESH_OK;
}


static void update_interface_ipaddr(void)
{
    int ret;
    ur_ip6_addr_t ip6_addr;
    ip6_addr_t addr6;
    uint8_t index;
    uint8_t mac_addr[IEEE80211_MAC_ADDR_LEN] = {0};

    ret = umesh_wifi_get_mac(mac_addr);
    if(ret < 0) {
        return ret;
    }

    for (index = 0; index < 3; index++) {
        memset(ip6_addr.m8, 0, sizeof(ip6_addr.m8));
        if (index == 0) {
            umesh_get_ipv6(mac_addr, ip6_addr.m8);
        } else if (index == 1) {
            umesh_get_ipv6(mac_addr, ip6_addr.m8);
        } else {  // mcast addr
            umesh_get_ipv6_mcast(ip6_addr.m8);
        }
        netif_ip6_addr_set_state(&g_la_state.adpif, index, IP6_ADDR_INVALID);
        IP6_ADDR(&addr6, ip6_addr.m32[0], ip6_addr.m32[1],
                 ip6_addr.m32[2], ip6_addr.m32[3]);
        ip6_addr_copy(*(ip_2_ip6(&g_la_state.adpif.ip6_addr[index])), addr6);
        netif_ip6_addr_set_state(&g_la_state.adpif, index, IP6_ADDR_VALID);
    }
    g_la_state.adpif.ip6_autoconfig_enabled = 1;
}

/*init tcp ip*/
int umesh_adapter_interface_up(void)
{
    //const mac_address_t *mac_addr;
    uint8_t mac_addr[IEEE80211_MAC_ADDR_LEN] = {0};
    struct netif *interface;
    int ret = 0;

    interface = netif_find(g_la_state.interface_name);

    if (interface == NULL) {
        // mac_addr = umesh_get_mac_address(MEDIA_TYPE_DFL);
        // assert(mac_addr);
        ret = umesh_wifi_get_mac(mac_addr);
        if(ret < 0) {
            return ret;
        }
        g_la_state.adpif.hwaddr_len = IEEE80211_MAC_ADDR_LEN;
        memcpy(g_la_state.adpif.hwaddr, mac_addr, IEEE80211_MAC_ADDR_LEN);

        ret = netifapi_netif_add(&g_la_state.adpif, NULL, NULL, NULL, NULL,
                                   umesh_adapter_if_init, tcpip_input);
        if (ret < 0) {
            return UMESH_ERR_IPSTACK_INIT;
        }
        interface = &g_la_state.adpif;
    }
    if(interface != &g_la_state.adpif) {
        return UMESH_ERR_IPSTACK_INIT;
    }
    //assert(interface == &g_la_state.adpif);

    if (netif_is_up(interface) == 0) {
        netifapi_netif_set_up(&g_la_state.adpif);
    }
    update_interface_ipaddr();

// #ifdef CONFIG_AOS_MESH_TAPIF
//     if (umesh_mm_get_device_state() == DEVICE_STATE_LEADER) {
//         is_router = true;
//         umesh_tapif_init("tun0");
//     } else {
//         is_router = false;
//         umesh_tapif_deinit();
//     }

//     netifapi_netif_set_default(&g_la_state.adpif);
// #else
    /*
     * if we are LEADER MODE, means WiFi is connected
     * then default if should be WiFi IF
     */
    if (umesh_wifi_get_connect_state() == 0) {
        netifapi_netif_set_default(&g_la_state.adpif);
    }
//#endif

    return ret;
}

int umesh_adapter_interface_down(void)
{
    if (netif_is_up(&g_la_state.adpif)) {
        netifapi_netif_set_down(&g_la_state.adpif);
    }

    return 0;
}

int umesh_adapter_interface_update(void)
{
    update_interface_ipaddr();
    return 0;
}

struct netif *umesh_adapter_get_netif(void)
{
    return &g_la_state.adpif;
}

const void *umesh_adapter_get_default_ipaddr(void)
{
    struct netif *netif = &g_la_state.adpif;
    uint8_t index;

    for (index = 0; index < LWIP_IPV6_NUM_ADDRESSES; index++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, index))) {
            return netif_ip_addr6(netif, index);
        }
    }
    return NULL;

}

const void *umesh_adapter_get_mcast_ipaddr(void)
{
    struct netif *netif = &g_la_state.adpif;

    static ip6_addr_t mcast;
    uint8_t index = 2;
    const ip6_addr_t *mcast_addr = netif_ip6_addr(netif, index);

    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, index))) {
        IP6_ADDR(&mcast, mcast_addr->addr[0], mcast_addr->addr[1],
                 0, htonl(0xfc));
        return &mcast;
    }

    return NULL;
}

struct netif *umesh_adapter_ip6_route(const ip6_addr_t *src,
                                   const ip6_addr_t *dest)
{
    return &g_la_state.adpif;
}

bool umesh_adapter_is_mcast_subscribed(const ip6_addr_t *addr)
{

    return umesh_is_mcast_subscribed((const ur_ip6_addr_t *)addr);
}

struct netif *lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    return NULL;

}

int umesh_adapter_interface_init(umesh_state_t *state)
{
    g_la_state.umesh_state = state;
    state->data_to_ip_cb = umesh_adapter_input_buf;
    umesh_adapter_interface_update();
    umesh_adapter_interface_up();
    // g_la_state.adapter_cb.input = umesh_adapter_input;
    // g_la_state.adapter_cb.interface_up = umesh_adapter_interface_up;
    // g_la_state.adapter_cb.interface_down = umesh_adapter_interface_down;
    // g_la_state.adapter_cb.interface_update = umesh_adapter_interface_update;
    //umesh_register_callback(&g_la_state.adapter_cb);
    return 0;
}

int umesh_adapter_interface_deinit(umesh_state_t *state)
{
    umesh_adapter_interface_down();
    return 0;
}

#endif