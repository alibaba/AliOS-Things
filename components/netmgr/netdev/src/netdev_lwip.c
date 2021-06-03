/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "lwip/opt.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <netdev.h>
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include <lwip/icmp.h>
#include <k_api.h>
#include "lwip/inet.h"
#include <lwip/sockets.h>
#include "aos/cli.h"

#define TAG "TAG_ETHIF"

#if LWIP_IPV6
#include "lwip/ethip6.h"
#endif /* LWIP_IPV6 */

#define netifapi_netif_set_link_up(n)      netifapi_netif_common(n, netif_set_link_up, NULL)
#define netifapi_netif_set_link_down(n)    netifapi_netif_common(n, netif_set_link_down, NULL)

#ifdef AOS_USING_NETDEV

#include "lwip/ip.h"
#include "lwip/init.h"
#include "lwip/netdb.h"
#include <netdev.h>

void dhcpd_start(const char *netif_name);

//add dhcp by lizhihai@anyka.oa
static int lwip_netdev_set_default(struct netdev *netif)
{
    if(netif->user_data)
        netifapi_netif_set_default((struct netif *)netif->user_data);
    return 0;
}
//add end

static int lwip_netdev_set_up(struct netdev *netif)
{
    struct netif *lwip_if = (struct netif *)netif->user_data;

    netifapi_netif_set_up(lwip_if);

#ifdef AOS_LWIP_DHCP
    //add dhcp by lizhihai@anyka.oa
    if(netdev_is_dhcp_enabled(netif))
    {
        netifapi_dhcp_start(lwip_if);
    }
#endif

#ifdef LWIP_USING_DHCPD
    if(netdev_is_dhcpd_enabled(netif))
    {
        dhcpd_start(lwip_if->name);
    }
#endif

    return 0;
}

static int lwip_netdev_set_down(struct netdev *netif)
{
    //add dhcp by lizhihai@anyka.oa
    if(netdev_is_dhcp_enabled(netif))
        netifapi_dhcp_stop((struct netif *)netif->user_data);
    netifapi_netif_set_down((struct netif *)netif->user_data);
    return 0;
}

static int lwip_netdev_set_addr_info(struct netdev *netif, ip_addr_t *ip_addr, ip_addr_t *netmask, ip_addr_t *gw)
{
    if (ip_addr && netmask && gw)
    {
        netif_set_addr((struct netif *)netif->user_data, ip_addr, netmask, gw);
    }
    else
    {
        if (ip_addr)
        {
            netif_set_ipaddr((struct netif *)netif->user_data, ip_addr);
        }

        if (netmask)
        {
            netif_set_netmask((struct netif *)netif->user_data, netmask);
        }

        if (gw)
        {
            netif_set_gw((struct netif *)netif->user_data, gw);
        }
    }

    return 0;
}

#ifdef AOS_LWIP_DNS
static int lwip_netdev_set_dns_server(struct netdev *netif, uint8_t dns_num, ip_addr_t *dns_server)
{
    extern void set_dns(uint8_t dns_num, char* dns_server);
    set_dns(dns_num, ipaddr_ntoa(dns_server));
    return 0;
}
#endif /* AOS_LWIP_DNS */

#ifdef AOS_LWIP_DHCP
static int lwip_netdev_set_dhcp(struct netdev *netif, bool is_enabled)
{
    //add dhcp by lizhihai@anyka.oa
    if(is_enabled)
    {
        ((struct netif *)(netif->user_data))->flags |= NETIF_FLAG_DHCP;
        netifapi_dhcp_start((struct netif *)netif->user_data);
    }
    else
    {
        ((struct netif *)(netif->user_data))->flags &= ~NETIF_FLAG_DHCP;
        netifapi_dhcp_stop((struct netif *)netif->user_data);
    }

    netdev_low_level_set_dhcp_status(netif, is_enabled);
    return 0;
}
#endif /* AOS_LWIP_DHCP */

#ifdef LWIP_USING_DHCPD
static int lwip_netdev_set_dhcpd(struct netdev *netif, bool is_enabled)
{
    //add dhcp by lizhihai@anyka.oa
    if(is_enabled)
    {
        ((struct netif *)(netif->user_data))->flags |= NETIF_FLAG_DHCPD;
        dhcpd_start(((struct netif *)netif->user_data)->name);
    }
    else
    {
        ((struct netif *)(netif->user_data))->flags &= ~NETIF_FLAG_DHCPD;
    }

    //netdev_low_level_set_dhcp_status(netif, is_enabled);
    return 0;
}
#endif /* AOS_LWIP_DHCP */

#ifdef AOS_LWIP_USING_PING
extern int lwip_ping_recv(int s, int *ttl, int size);
extern err_t lwip_ping_send(int s, ip_addr_t *addr, int size);
extern unsigned long long hal_get_tick_count(void);

int lwip_netdev_ping(struct netdev *netif, const char *host, size_t data_len,
                        uint32_t timeout, struct netdev_ping_resp *ping_resp)
{
    int s, ttl, recv_len, result = 0;
    uint64_t elapsed_time;
    uint64_t recv_start_tick;
#if LWIP_VERSION_MAJOR >= 2U
    struct timeval recv_timeout = { timeout / RHINO_CONFIG_TICKS_PER_SECOND, timeout % RHINO_CONFIG_TICKS_PER_SECOND };
#else
    int recv_timeout = timeout * 1000UL / RHINO_CONFIG_TICKS_PER_SECOND;
#endif
    ip_addr_t target_addr;
    struct addrinfo hint, *res = NULL;
    struct sockaddr_in *h = NULL;
    struct in_addr ina;

    assert(netif);
    assert(host);
    assert(ping_resp);

    memset(&hint, 0x00, sizeof(hint));
    /* convert URL to IP */
    if (lwip_getaddrinfo(host, NULL, &hint, &res) != 0)
    {
        LOGE(TAG, "%s:%d\n", __func__, __LINE__);
        return -1;
    }
    memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(&ina, &h->sin_addr, sizeof(ina));
    lwip_freeaddrinfo(res);
    if (inet_aton(inet_ntoa(ina), &target_addr) == 0)
    {
        LOGE(TAG, "%s:%d\n", __func__, __LINE__);
        return -1;
    }
    memcpy(&(ping_resp->ip_addr), &target_addr, sizeof(ip_addr_t));

    /* new a socket */
    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0)
    {
        LOGE(TAG, "%s:%d\n", __func__, __LINE__);
        return -1;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));


    if (lwip_ping_send(s, &target_addr, data_len) == 0)
    {
        extern uint64_t hal_get_tick_count(void);
        recv_start_tick = hal_get_tick_count();
        if ((recv_len = lwip_ping_recv(s, &ttl, data_len)) >= 0)
        {
            elapsed_time = (hal_get_tick_count() - recv_start_tick);
            ping_resp->data_len = recv_len;
            ping_resp->ttl = ttl;
            ping_resp->ticks = elapsed_time;
        }
        else
        {
            LOGE(TAG, "%s:%d\n", __func__, __LINE__);
            result = -ETIMEDOUT;
            goto __exit;
        }
    }
    else
    {
        LOGE(TAG, "%s:%d\n", __func__, __LINE__);
        result = -ETIMEDOUT;
        goto __exit;
    }

__exit:
    lwip_close(s);

    LOGD(TAG, "%s:%d\n", __func__, __LINE__);
    return result;
}
#endif /* RT_LWIP_USING_PING */

#if defined (AOS_LWIP_TCP) || defined (AOS_LWIP_UDP)
#if LWIP_TCP
void list_tcps(void);
#endif
#if LWIP_UDP
void list_udps(void);
#endif

void lwip_netdev_netstat(struct netdev *netif)
{
#ifdef AOS_LWIP_TCP
    list_tcps();
#endif
#ifdef AOS_LWIP_UDP
    list_udps();
#endif
}
#endif /* AOS_LWIP_TCP || AOS_LWIP_UDP */

const struct netdev_ops lwip_netdev_ops =
{
    lwip_netdev_set_up,
    lwip_netdev_set_down,

    lwip_netdev_set_default,

    lwip_netdev_set_addr_info,
#ifdef AOS_LWIP_DNS
    lwip_netdev_set_dns_server,
#else
    NULL,
#endif /* AOS_LWIP_DNS */

#ifdef AOS_LWIP_DHCP
    lwip_netdev_set_dhcp,
#else
    NULL,
#endif /* AOS_LWIP_DHCP */

#ifdef LWIP_USING_DHCPD
    lwip_netdev_set_dhcpd,
#else
    NULL,
#endif /* LWIP_USING_DHCPD */

#ifdef AOS_LWIP_USING_PING
    lwip_netdev_ping,
#else
    NULL,
#endif /* RT_LWIP_USING_PING */

#if defined (AOS_LWIP_TCP) || defined (AOS_LWIP_UDP)
    lwip_netdev_netstat,
#endif /* AOS_LWIP_TCP || AOS_LWIP_UDP */
};

int netdev_add(struct netif *lwip_netif)
{
#define LWIP_NETIF_NAME_LEN 5
    int result = 0;
    struct netdev *netdev = NULL;
    char name[LWIP_NETIF_NAME_LEN + 1] = {0};

    assert(lwip_netif);

    netdev = (struct netdev *)calloc(1, sizeof(struct netdev));
    if (netdev == NULL)
    {
        return -ERR_IF;
    }

    netdev->flags = lwip_netif->flags;
    netdev->ops = &lwip_netdev_ops;
    netdev->hwaddr_len =  lwip_netif->hwaddr_len;
    memcpy(netdev->hwaddr, lwip_netif->hwaddr, lwip_netif->hwaddr_len);

#ifdef SAL_USING_LWIP
    extern int sal_lwip_netdev_set_pf_info(struct netdev *netdev);
    /* set the lwIP network interface device protocol family information */
    sal_lwip_netdev_set_pf_info(netdev);
#endif /* SAL_USING_LWIP */

    strncpy(name, lwip_netif->name, LWIP_NETIF_NAME_LEN);
    result = netdev_register(netdev, name, (void *)lwip_netif);

#ifdef AOS_LWIP_DHCP
    netdev_low_level_set_dhcp_status(netdev, true);
#endif

    return result;
}

/* synchronize lwIP network interface device and network interface device flags */
static int netdev_flags_sync(struct netif *lwip_netif)
{
    struct netdev *netdev = NULL;

    assert(lwip_netif);

    netdev = netdev_get_by_name(lwip_netif->name);
    if (netdev == NULL)
    {
        return -ERR_IF;
    }

    netdev->mtu = lwip_netif->mtu;
    netdev->flags |= lwip_netif->flags;

    return 0;
}
#endif /* _USING_NETDEV */

void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
    ip4_addr_t *ip;
    ip4_addr_t addr;
    struct netif * netif = netif_list;

    if(strlen(netif_name) > sizeof(netif->name))
    {
        LOGE(TAG, "network interface name too long!\r\n");
        return;
    }

    while(netif != NULL)
    {
        if(strncmp(netif_name, netif->name, sizeof(netif->name)) == 0)
            break;

        netif = netif->next;
        if( netif == NULL )
        {
            LOGE(TAG, "network interface: %s not found!\r\n", netif_name);
            return;
        }
    }

    ip = (ip4_addr_t *)&addr;

    /* set ip address */
    if ((ip_addr != NULL) && ip4addr_aton(ip_addr, &addr))
    {
        netif_set_ipaddr(netif, ip);
    }

    /* set gateway address */
    if ((gw_addr != NULL) && ip4addr_aton(gw_addr, &addr))
    {
        netif_set_gw(netif, ip);
    }

    /* set netmask address */
    if ((nm_addr != NULL) && ip4addr_aton(nm_addr, &addr))
    {
        netif_set_netmask(netif, ip);
    }
}

#if LWIP_DNS
#include <lwip/dns.h>
void set_dns(uint8_t dns_num, char* dns_server)
{
    ip_addr_t addr;

    if ((dns_server != NULL) && ipaddr_aton(dns_server, &addr))
    {
        dns_setserver(dns_num, &addr);
    }
}
#endif

#include "aos/cli.h"
#define CLI_PRINT aos_cli_printf

void list_if(void)
{
    unsigned long index;
    struct netif * netif;

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();

    netif = netif_list;

    while( netif != NULL )
    {
        CLI_PRINT("network interface: %c%c%s\n",
                   netif->name[0],
                   netif->name[1],
                   (netif == netif_default)?" (Default)":"");
        CLI_PRINT("MTU: %d\n", netif->mtu);
        CLI_PRINT("MAC: ");
        for (index = 0; index < netif->hwaddr_len; index ++)
            CLI_PRINT("%02x ", netif->hwaddr[index]);
        CLI_PRINT("\nFLAGS:");
        if (netif->flags & NETIF_FLAG_UP) CLI_PRINT(" UP");
        else CLI_PRINT(" DOWN");
        if (netif->flags & NETIF_FLAG_LINK_UP) CLI_PRINT(" LINK_UP");
        else CLI_PRINT(" LINK_DOWN");
        if (netif->flags & NETIF_FLAG_ETHARP) CLI_PRINT(" ETHARP");
        if (netif->flags & NETIF_FLAG_BROADCAST) CLI_PRINT(" BROADCAST");
        if (netif->flags & NETIF_FLAG_IGMP) CLI_PRINT(" IGMP");
        CLI_PRINT("\n");
        CLI_PRINT("ip address: %s\n", ipaddr_ntoa(&(netif->ip_addr)));
        CLI_PRINT("gw address: %s\n", ipaddr_ntoa(&(netif->gw)));
        CLI_PRINT("net mask  : %s\n", ipaddr_ntoa(&(netif->netmask)));
#if LWIP_IPV6
        {
            ip6_addr_t *addr;
            int addr_state;
            int i;

            addr = (ip6_addr_t *)&netif->ip6_addr[0];
            addr_state = netif->ip6_addr_state[0];

            CLI_PRINT("\nipv6 link-local: %s state:%02X %s\n", ip6addr_ntoa(addr),
            addr_state, ip6_addr_isvalid(addr_state)?"VALID":"INVALID");

            for(i=1; i<LWIP_IPV6_NUM_ADDRESSES; i++)
            {
                addr = (ip6_addr_t *)&netif->ip6_addr[i];
                addr_state = netif->ip6_addr_state[i];

                CLI_PRINT("ipv6[%d] address: %s state:%02X %s\n", i, ip6addr_ntoa(addr),
                addr_state, ip6_addr_isvalid(addr_state)?"VALID":"INVALID");
            }

        }
        CLI_PRINT("\r\n");
#endif /* LWIP_IPV6 */
        netif = netif->next;
    }

#if LWIP_DNS
    {
        const ip_addr_t *ip_addr;

        for(index=0; index<DNS_MAX_SERVERS; index++)
        {
            ip_addr = dns_getserver(index);
            CLI_PRINT("dns server #%d: %s\n", index, ipaddr_ntoa(ip_addr));
        }
    }
#endif /**< #if LWIP_DNS */

    RHINO_CRITICAL_EXIT();
}

#if LWIP_TCP
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>

void list_tcps(void)
{
    uint32_t num = 0;
    struct tcp_pcb *pcb;
    char local_ip_str[16];
    char remote_ip_str[16];

    extern struct tcp_pcb *tcp_active_pcbs;
    extern union tcp_listen_pcbs_t tcp_listen_pcbs;
    extern struct tcp_pcb *tcp_tw_pcbs;

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
    CLI_PRINT("Active PCB states:\n");
    for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        memset(local_ip_str, 0, sizeof(local_ip_str));
        strncpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)), sizeof(local_ip_str)-1);
        memset(remote_ip_str, 0, sizeof(remote_ip_str));
        strncpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)), sizeof(remote_ip_str)-1);

        CLI_PRINT("#%d %s:%d <==> %s:%d snd_nxt 0x%08X rcv_nxt 0x%08X ",
                   num++,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port,
                   pcb->snd_nxt,
                   pcb->rcv_nxt);
        CLI_PRINT("state: %s\n", tcp_debug_state_str(pcb->state));
    }

    CLI_PRINT("Listen PCB states:\n");
    num = 0;
    for(pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next)
    {
        CLI_PRINT("#%d local port %d ", num++, pcb->local_port);
        CLI_PRINT("state: %s\n", tcp_debug_state_str(pcb->state));
    }

    CLI_PRINT("TIME-WAIT PCB states:\n");
    num = 0;
    for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
    {
        memset(local_ip_str, 0, sizeof(local_ip_str));
        strncpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)), sizeof(local_ip_str)-1);
        memset(remote_ip_str, 0, sizeof(remote_ip_str));
        strncpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)), sizeof(remote_ip_str)-1);

        CLI_PRINT("#%d %s:%d <==> %s:%d snd_nxt 0x%08X rcv_nxt 0x%08X ",
                   num++,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port,
                   pcb->snd_nxt,
                   pcb->rcv_nxt);
        CLI_PRINT("state: %s\n", tcp_debug_state_str(pcb->state));
    }
    RHINO_CRITICAL_EXIT();
}
#endif /* LWIP_TCP */

#if LWIP_UDP
#include "lwip/udp.h"
void list_udps(void)
{
    struct udp_pcb *pcb;
    uint32_t num = 0;
    char local_ip_str[16];
    char remote_ip_str[16];

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
    CLI_PRINT("Active UDP PCB states:\n");
    for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next)
    {
        memset(local_ip_str, 0, sizeof(local_ip_str));
        memset(remote_ip_str, 0, sizeof(remote_ip_str));
        strncpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)), sizeof(local_ip_str)-1);
        strncpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)), sizeof(remote_ip_str)-1);

        CLI_PRINT("#%d %d %s:%d <==> %s:%d \n",
                   num, (int)pcb->flags,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port);

        num++;
    }
    RHINO_CRITICAL_EXIT();
}
#endif /* LWIP_UDP */
