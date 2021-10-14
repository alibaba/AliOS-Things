#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include "netmgr.h"
#include "netmgr_conn.h"
#include "netmgr_ethernet.h"
#include "uservice/uservice.h"
#include "uservice/eventid.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dns.h"
#include "lwip/netifapi.h"
#include "sntp/sntp.h"
#include "ulog/ulog.h"


#define TAG "ETH"
#define NETMGR_ETH_LOGE(level, ...)      LOGE(TAG, level, ##__VA_ARGS__)
#define NETMGR_ETH_LOGW(level, ...)      LOGW(TAG, level, ##__VA_ARGS__)
#define NETMGR_ETH_LOGI(level, ...)      LOGI(TAG, level, ##__VA_ARGS__)
#define NETMGR_ETH_LOGD(level, ...)      LOGD(TAG, level, ##__VA_ARGS__)


static netmgr_conn_t *g_eth_conn_info = NULL;

/* ethernet link up cb from netmgr_conn state machine */
static int eth_link_up(netmgr_conn_t *conn)
{
    struct netif *net_if;

    if (conn == NULL) {
        NETMGR_ETH_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    net_if = conn->netif;

    /* bring up netif */
    if (!netif_is_up(net_if)) {
        netifapi_netif_set_up(net_if);
        netifapi_netif_set_default(net_if);
    }

    /* bring up link */
    if (!netif_is_link_up(net_if)) {
        NETMGR_ETH_LOGD("%s:%d, net_if 0x%x, name %s\n", __func__, __LINE__, (unsigned int)net_if, net_if->name);
        netifapi_netif_set_link_up(net_if);
    }

    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    return netmgr_conn_state_change(conn, CONN_STATE_OBTAINING_IP);
}

static void tcpip_dhcpc_cb(struct netif *net_if)
{
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(net_if, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

    if (netif_is_up(net_if) && (dhcp->state == DHCP_STATE_BOUND) && !ip_addr_isany(&net_if->ip_addr)) {
        int ret;
        netmgr_ip_stat_t in;
        netmgr_conn_t *conn = g_eth_conn_info;

        NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
        memset(&in, 0, sizeof(in));
        snprintf(in.ip, sizeof(in.ip), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);

        snprintf(in.mask, sizeof(in.mask), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);

        snprintf(in.gate, sizeof(in.gate), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);

        NETMGR_ETH_LOGI("\r\n IPv4 Address     : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);
        NETMGR_ETH_LOGI(" IPv4 Subnet mask : %hhu.%hhu.%hhu.%hhu\r\n",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);
        NETMGR_ETH_LOGI(" IPv4 Gateway     : %hhu.%hhu.%hhu.%hhu\r\n\r\n",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);
        event_publish(EVENT_ETHERNET_DHCP_SUCCESS, NULL);
    }
    NETMGR_ETH_LOGI("%s: dhcp state =%d", __func__, dhcp->state);
}

static void eth_dhcp_start(void)
{
    struct netif *net_if = NULL;
    netmgr_conn_t *conn;
    // int event = EVENT_NETMGR_DHCP_START_FAILED;

    conn = g_eth_conn_info;
    net_if = conn->netif;
    if (!net_if) {
        NETMGR_ETH_LOGE("get net interface failed\n");
        goto end;
    }
    netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    netif_set_status_callback(net_if, tcpip_dhcpc_cb);

    if (netif_is_up(net_if)) {
        NETMGR_ETH_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
        if (netifapi_dhcp_start(net_if) != 0) {
            NETMGR_ETH_LOGE("%s:%d start dhcp failed.\n", __func__, __LINE__);
            goto end;
        }
    }
    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    return ;
end:
    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    return ;
}

/* ethernet obtaining ip cb from netmgr_conn state machine */
static int eth_obtaining_ip(netmgr_conn_t *conn)
{
    if (conn == NULL) {
        NETMGR_ETH_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    if (conn->use_ip_mode == NETMGR_ETH_IP_MODE_STATIC) {
        struct netif *net_if = conn->netif;

        if (net_if != NULL) {
            netmgr_ip_stat_t in;
            const ip_addr_t *dns;
            int ret;

            netifapi_netif_set_down(net_if);
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
            dns = dns_getserver(0);
            dns_setserver(0, &conn->static_dns);
            dns_setserver(1, dns);

            event_publish(EVENT_ETHERNET_DHCP_SUCCESS, NULL);

            memset(&in, 0, sizeof(in));
            snprintf(in.ip, sizeof(in.ip), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->ip_addr.addr)[0],
               ((u8_t *)&net_if->ip_addr.addr)[1],
               ((u8_t *)&net_if->ip_addr.addr)[2],
               ((u8_t *)&net_if->ip_addr.addr)[3]);

            snprintf(in.mask, sizeof(in.mask), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->netmask.addr)[0],
               ((u8_t *)&net_if->netmask.addr)[1],
               ((u8_t *)&net_if->netmask.addr)[2],
               ((u8_t *)&net_if->netmask.addr)[3]);

            snprintf(in.gate, sizeof(in.gate), "%hhu.%hhu.%hhu.%hhu",
               ((u8_t *)&net_if->gw.addr)[0],
               ((u8_t *)&net_if->gw.addr)[1],
               ((u8_t *)&net_if->gw.addr)[2],
               ((u8_t *)&net_if->gw.addr)[3]);
        }
    } else {
        NETMGR_ETH_LOGI("%s:%d start dhcp\n", __func__, __LINE__);
        eth_dhcp_start();
    }

    return 0;
}

/* dummy event cb from ethernet driver */
static int eth_network_connected(netmgr_conn_t *conn)
{
    if (conn == NULL) {
        NETMGR_ETH_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    event_publish(EVENT_NETMGR_DHCP_SUCCESS, NULL);

    return 0;
}

/* ethernet link up cb from netmgr_conn state machine */
static int eth_failed(netmgr_conn_t *conn)
{
    if (conn == NULL) {
        NETMGR_ETH_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    return netmgr_conn_state_change(conn, CONN_STATE_DISCONNECTED);
}

/* ethernet link up cb from netmgr_conn state machine */
static int eth_disconnected(netmgr_conn_t *conn)
{
    struct netif *net_if;
    if (conn == NULL) {
        NETMGR_ETH_LOGE("%s:%d error: connection is invalid\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_ETH_LOGI("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if (net_if != NULL) {
        if (netif_is_link_up(net_if))
            netifapi_netif_set_link_down(net_if);

        if (netif_is_up(net_if)) {
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
        }
        conn->netif = NULL;
    }
    return 0;
}

/* event cb from ethernet driver */
static void eth_event_cb(uint32_t event_id, const void *param, void *context)
{
    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
    if (NULL == g_eth_conn_info) {
        NETMGR_ETH_LOGD("%s:%d, ethernet service not initialized, event_id 0x%x\n", __func__, __LINE__, event_id);
        return;
    }

    switch (event_id) {
    case EVENT_ETHERNET_LINK_UP:
        NETMGR_ETH_LOGD("%s:%d, eth link up --> CONN_STATE_CONNECTED\n", __func__, __LINE__);
        g_eth_conn_info->netif = (struct netif *)param;
        netmgr_conn_state_change(g_eth_conn_info, CONN_STATE_CONNECTED);
        break;
    case EVENT_ETHERNET_DHCP_SUCCESS:
        NETMGR_ETH_LOGD("%s:%d, eth dhcp success --> CONN_STATE_NETWORK_CONNECTED\n", __func__, __LINE__);
        netmgr_conn_state_change(g_eth_conn_info, CONN_STATE_NETWORK_CONNECTED);
        break;
    case EVENT_ETHERNET_LINK_DOWN:
        NETMGR_ETH_LOGD("%s:%d, eth link down --> CONN_STATE_DISCONNECTED\n", __func__, __LINE__);
        netmgr_conn_state_change(g_eth_conn_info, CONN_STATE_DISCONNECTED);
        break;
    default:
        NETMGR_ETH_LOGD("%s:%d, unexpected event_id 0x%x\n", __func__, __LINE__, event_id);
        break;
    }
}

/* dummy event cb from ethernet driver */
static int eth_dummy_cb(uint32_t event_id, const void *param, void *context)
{
    NETMGR_ETH_LOGD("%s:%d, unexpected event id 0x%x\n", __func__, __LINE__, event_id);
    return 0;
}

static int netmgr_eth_set_ip_mode(netmgr_eth_ip_mode_t mode)
{
    struct netif *net_if;
    netmgr_conn_t *conn = g_eth_conn_info;

    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
    if (conn->use_ip_mode != mode) {
        const ip_addr_t *dns = NULL;
        conn->use_ip_mode = mode;
        net_if = conn->netif;
        if (!net_if) {
            NETMGR_ETH_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
            return -1;
        }
        NETMGR_ETH_LOGD("%s:%d set ip mode %d\n", __func__, __LINE__, mode);
        if (mode == NETMGR_ETH_IP_MODE_AUTO) {
            NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
            netifapi_netif_set_addr(net_if, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
            dns_setserver(0, IP4_ADDR_ANY);
            dns_setserver(1, IP4_ADDR_ANY);
        } else {
            NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
            netifapi_dhcp_stop(net_if);
            netifapi_netif_set_down(net_if);
            netifapi_netif_set_up(net_if);
            netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
            dns = dns_getserver(0);
            dns_setserver(0, &conn->static_dns);
            dns_setserver(1, dns);
        }
    }
    return 0;
}

static int netmgr_eth_set_static_ip_stat(const char *ip_addr, const char *mask, const char *gw, const char *dns_server)
{
    struct netif *net_if;
    const ip_addr_t *dns;
    netmgr_conn_t *conn = g_eth_conn_info;

    if ((ip_addr == NULL) || (mask == NULL) || (gw == NULL) || (dns_server == NULL)) {
        NETMGR_ETH_LOGE("%s:%d invalid params\n", __func__, __LINE__);
        return -1;
    }

    NETMGR_ETH_LOGD("%s:%d ip_addr=%s mask=%s gw=%s dns_server=%s\n",
                       __func__, __LINE__, ip_addr, mask, gw, dns_server);

    sscanf(ip_addr, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_ip.addr)[0],
                     &((u8_t *)&conn->static_ip.addr)[1],
                     &((u8_t *)&conn->static_ip.addr)[2],
                     &((u8_t *)&conn->static_ip.addr)[3]);
    NETMGR_ETH_LOGD("%s:%d set ip address %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_ip.addr)[0],
                     ((u8_t *)&conn->static_ip.addr)[1],
                     ((u8_t *)&conn->static_ip.addr)[2],
                     ((u8_t *)&conn->static_ip.addr)[3]);

    sscanf(mask, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_mask.addr)[0],
                     &((u8_t *)&conn->static_mask.addr)[1],
                     &((u8_t *)&conn->static_mask.addr)[2],
                     &((u8_t *)&conn->static_mask.addr)[3]);
    NETMGR_ETH_LOGD("%s:%d set mask %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_mask.addr)[0],
                     ((u8_t *)&conn->static_mask.addr)[1],
                     ((u8_t *)&conn->static_mask.addr)[2],
                     ((u8_t *)&conn->static_mask.addr)[3]);

    sscanf(gw, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_gw.addr)[0],
                     &((u8_t *)&conn->static_gw.addr)[1],
                     &((u8_t *)&conn->static_gw.addr)[2],
                     &((u8_t *)&conn->static_gw.addr)[3]);
    NETMGR_ETH_LOGD("%s:%d set gw %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_gw.addr)[0],
                     ((u8_t *)&conn->static_gw.addr)[1],
                     ((u8_t *)&conn->static_gw.addr)[2],
                     ((u8_t *)&conn->static_gw.addr)[3]);

    sscanf(dns_server, "%hhu.%hhu.%hhu.%hhu",
                     &((u8_t *)&conn->static_dns.addr)[0],
                     &((u8_t *)&conn->static_dns.addr)[1],
                     &((u8_t *)&conn->static_dns.addr)[2],
                     &((u8_t *)&conn->static_dns.addr)[3]);
    NETMGR_ETH_LOGD("%s:%d set dns server %hhu.%hhu.%hhu.%hhu\n", __func__, __LINE__,
                     ((u8_t *)&conn->static_dns.addr)[0],
                     ((u8_t *)&conn->static_dns.addr)[1],
                     ((u8_t *)&conn->static_dns.addr)[2],
                     ((u8_t *)&conn->static_dns.addr)[3]);
    net_if = conn->netif;
    if (net_if) {
        netifapi_dhcp_stop(net_if);
        netifapi_netif_set_down(net_if);
        netifapi_netif_set_up(net_if);
        netifapi_netif_set_addr(net_if, &conn->static_ip, &conn->static_mask, &conn->static_gw);
        dns = dns_getserver(0);
        dns_setserver(0, &conn->static_dns);
        dns_setserver(1, dns);
    }
    return 0;
}

int netmgr_eth_init(netmgr_hdl_t hdl)
{
    int err;
    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);

    if (g_eth_conn_info != NULL) {
        NETMGR_ETH_LOGE("ethernet service has already initialized.");
        return -1;
    }

    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);

    /* init connection info */
    g_eth_conn_info = netmgr_conn_init(NETMGR_TYPE_ETH);
    if (g_eth_conn_info == NULL) {
        NETMGR_ETH_LOGE("ethernet service init failed.");
        return -1;
    }

    g_eth_conn_info->hdl = hdl;
    g_eth_conn_info->auto_reconnect = false;
    g_eth_conn_info->use_ip_mode = NETMGR_ETH_IP_MODE_AUTO;

    g_eth_conn_info->connected_cb = eth_link_up;
    g_eth_conn_info->failed_cb = eth_failed;
    g_eth_conn_info->obtaining_ip_cb = eth_obtaining_ip;
    g_eth_conn_info->network_connected_cb = eth_network_connected;
    g_eth_conn_info->disconnecting_cb = eth_dummy_cb;
    g_eth_conn_info->disconnected_cb = eth_disconnected;

    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);

    event_subscribe(EVENT_ETHERNET_LINK_UP, eth_event_cb, NULL);
    event_subscribe(EVENT_ETHERNET_LINK_DOWN, eth_event_cb, NULL);
    event_subscribe(EVENT_ETHERNET_DHCP_SUCCESS, eth_event_cb, NULL);

    return 0;
}

int netmgr_eth_set_ifconfig(netmgr_ifconfig_info_t *info)
{
    if (info != NULL) {
        if (info->dhcp_en == true) {
            NETMGR_ETH_LOGD("%s:%d, auto ip mode\n", __func__, __LINE__);
            netmgr_eth_set_ip_mode(NETMGR_ETH_IP_MODE_AUTO);
            return 0;
        } else {
            NETMGR_ETH_LOGD("%s:%d, static ip mode\n", __func__, __LINE__);
            netmgr_eth_set_ip_mode(NETMGR_ETH_IP_MODE_STATIC);
            return netmgr_eth_set_static_ip_stat(info->ip_addr, info->mask, info->gw, info->dns_server);
        }
    }
    return -1;
}

int netmgr_eth_get_ip_stat(char *ip_addr, char *mask, char *gw, char *dns_server, bool dhcp_en)
{
    struct netif *net_if;
    const ip_addr_t *dns;
    static netmgr_conn_t *conn;

    conn = g_eth_conn_info;
    if (!conn) {
        NETMGR_ETH_LOGE("%s:%d, g_eth_conn_info is null\n", __func__, __LINE__);
        return -1;
    }
    NETMGR_ETH_LOGD("%s:%d\n", __func__, __LINE__);
    net_if = conn->netif;
    if (!net_if) {
        NETMGR_ETH_LOGE("%s:%d get netif failed\n", __func__, __LINE__);
        return -1;
    }

    sprintf(ip_addr, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->ip_addr.addr)[0],
                     ((u8_t *)&net_if->ip_addr.addr)[1],
                     ((u8_t *)&net_if->ip_addr.addr)[2],
                     ((u8_t *)&net_if->ip_addr.addr)[3]);

    sprintf(mask, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->netmask.addr)[0],
                     ((u8_t *)&net_if->netmask.addr)[1],
                     ((u8_t *)&net_if->netmask.addr)[2],
                     ((u8_t *)&net_if->netmask.addr)[3]);

    sprintf(gw, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&net_if->gw.addr)[0],
                     ((u8_t *)&net_if->gw.addr)[1],
                     ((u8_t *)&net_if->gw.addr)[2],
                     ((u8_t *)&net_if->gw.addr)[3]);

    dns = dns_getserver(0);
    sprintf(dns_server, "%hhu.%hhu.%hhu.%hhu",
                     ((u8_t *)&dns->addr)[0],
                     ((u8_t *)&dns->addr)[1],
                     ((u8_t *)&dns->addr)[2],
                     ((u8_t *)&dns->addr)[3]);

    dhcp_en = (conn->use_ip_mode == NETMGR_ETH_IP_MODE_AUTO) ? true : false;

    NETMGR_ETH_LOGD("%s:%d ip_addr=%s mask=%s gw=%s dns=%s dhcp_en=%d\n", __func__, __LINE__,
                        ip_addr, mask, gw, dns_server, dhcp_en);
    return 0;

}

netmgr_conn_state_t netmgr_eth_get_stat(void)
{
    int ret = -1;
    netmgr_ip_stat_t eth_stat = {0};

    ret = netmgr_eth_get_ip_stat(eth_stat.ip, eth_stat.mask, eth_stat.gate, eth_stat.dns, &eth_stat.dhcp);
    if (ret != 0) {
        NETMGR_ETH_LOGE("%s:%d, netmgr_eth_get_stat error\n", __func__, __LINE__);
        return CONN_STATE_DISCONNECTED;
    }

    if ((strcmp(eth_stat.ip, "0.0.0.0") != 0)) {
        return CONN_STATE_NETWORK_CONNECTED;
    } else {
        return CONN_STATE_DISCONNECTED;
    }
}
