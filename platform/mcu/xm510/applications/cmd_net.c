/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <k_api.h>
#include <lwip/netif.h>
#include <lwip/dns.h>
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>
#include <stdio.h>


void list_if(void)
{
    unsigned int index;
    struct netif * netif;

	krhino_sched_disable();

    netif = netif_list;

    while( netif != NULL )
    {
        printf("network interface: %c%c%s\n",
                   netif->name[0],
                   netif->name[1],
                   (netif == netif_default)?" (Default)":"");
        printf("MTU: %d\n", netif->mtu);
        printf("MAC: ");
        for (index = 0; index < netif->hwaddr_len; index ++)
            printf("%02x ", netif->hwaddr[index]);
        printf("\nFLAGS:");
        if (netif->flags & NETIF_FLAG_UP) printf(" UP");
        else printf(" DOWN");
        if (netif->flags & NETIF_FLAG_LINK_UP) printf(" LINK_UP");
        else printf(" LINK_DOWN");
		//if (netif->flags & NETIF_FLAG_DHCP) printf(" DHCP");
		//if (netif->flags & NETIF_FLAG_POINTTOPOINT) printf(" PPP");
        if (netif->flags & NETIF_FLAG_ETHARP) printf(" ETHARP");
        if (netif->flags & NETIF_FLAG_IGMP) printf(" IGMP");
        printf("\n");
        printf("ip address: %s\n", ipaddr_ntoa(&(netif->ip_addr)));
        printf("gw address: %s\n", ipaddr_ntoa(&(netif->gw)));
        printf("net mask  : %s\n", ipaddr_ntoa(&(netif->netmask)));
        printf("\r\n");

        netif = netif->next;
    }

#if LWIP_DNS
    {
        const ip_addr_t* ip_addr;

        for(index=0; index<DNS_MAX_SERVERS; index++)
        {
            ip_addr = dns_getserver(index);
            printf("dns server #%d: %s\n", index, ipaddr_ntoa(ip_addr));
        }
    }
#endif /**< #if LWIP_DNS */

	krhino_sched_enable();
}
void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
    ip_addr_t *ip;
    ip_addr_t addr;
    struct netif * netif = netif_list;

    if(strlen(netif_name) > sizeof(netif->name))
    {
        printf("network interface name too long!\r\n");
        return;
    }

    while(netif != NULL)
    {
        if(strncmp(netif_name, netif->name, sizeof(netif->name)) == 0)
            break;

        netif = netif->next;
        if( netif == NULL )
        {
            printf("network interface: %s not found!\r\n", netif_name);
            return;
        }
    }

    ip = (ip_addr_t *)&addr;

    /* set ip address */
    if ((ip_addr != NULL) && ipaddr_aton(ip_addr, &addr))
    {
        netif_set_ipaddr(netif, ip);
    }

    /* set gateway address */
    if ((gw_addr != NULL) && ipaddr_aton(gw_addr, &addr))
    {
        netif_set_gw(netif, ip);
    }

    /* set netmask address */
    if ((nm_addr != NULL) && ipaddr_aton(nm_addr, &addr))
    {
        netif_set_netmask(netif, ip);
    }
}
void cmd_ifconfig(char *pwbuf, int blen, int argc, char **argv)
{

    if (argc == 1)
    {
        list_if();
    }
    else if (argc == 5)
    {
        printf("config : %s\n", argv[1]);
        printf("IP addr: %s\n", argv[2]);
        printf("Gateway: %s\n", argv[3]);
        printf("netmask: %s\n", argv[4]);
        set_if(argv[1], argv[2], argv[3], argv[4]);
    }
    else
    {
        printf("bad parameter! e.g: ifconfig e0 192.168.1.30 192.168.1.1 255.255.255.0\n");
    }

    return;
}

static const char * const tcp_state_str[] = {
  "CLOSED",
  "LISTEN",
  "SYN_SENT",
  "SYN_RCVD",
  "ESTABLISHED",
  "FIN_WAIT_1",
  "FIN_WAIT_2",
  "CLOSE_WAIT",
  "CLOSING",
  "LAST_ACK",
  "TIME_WAIT"
};
void cmd_netstat(char *pwbuf, int blen, int argc, char **argv)
{
    unsigned int num = 0;
    struct tcp_pcb *pcb;
    char local_ip_str[16];
    char remote_ip_str[16];

    extern struct tcp_pcb *tcp_active_pcbs;
    extern union tcp_listen_pcbs_t tcp_listen_pcbs;
    extern struct tcp_pcb *tcp_tw_pcbs;

	krhino_sched_disable();
    printf("Active PCB states:\n");
    for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        strcpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)));
        strcpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)));

        printf("#%d %s:%d <==> %s:%d snd_nxt 0x%08lX rcv_nxt 0x%08lX ",
                   num++,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port,
                   pcb->snd_nxt,
                   pcb->rcv_nxt);
        printf("state: %s\n", tcp_state_str[pcb->state]);
    }

    printf("Listen PCB states:\n");
    num = 0;
    for(pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next)
    {
        printf("#%d local port %d ", num++, pcb->local_port);
        printf("state: %s\n", tcp_state_str[pcb->state]);
    }

    printf("TIME-WAIT PCB states:\n");
    num = 0;
    for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
    {
        strcpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)));
        strcpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)));

        printf("#%d %s:%d <==> %s:%d snd_nxt 0x%08lX rcv_nxt 0x%08lX ",
                   num++,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port,
                   pcb->snd_nxt,
                   pcb->rcv_nxt);
        printf("state: %s\n", tcp_state_str[pcb->state]);
    }
	krhino_sched_enable();
}
