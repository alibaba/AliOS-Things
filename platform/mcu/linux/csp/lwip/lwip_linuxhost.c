/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"

#include "netif/tapif.h"
#include "netif/etharp.h"

static ip4_addr_t ipaddr, netmask, gw;
static struct netif netif;
#define IP_MAX_LEN  17
static void tcpip_init_done_callback(void *arg)
{
    char ip_str[IP_MAX_LEN] = {0}, nm_str[IP_MAX_LEN] = {0}, gw_str[IP_MAX_LEN] = {0};
    /* startup defaults (may be overridden by one or more opts) */
    IP4_ADDR(&gw, 192,168,0,1);
    IP4_ADDR(&ipaddr, 192,168,0,2);
    IP4_ADDR(&netmask, 255,255,255,0);
#if 0
    ip4_addr_t dnssrv;
    IP4_ADDR(&dnssrv,10,65,1,2);
    dns_setserver(0, (ip_addr_t *)&dnssrv);
#endif
    strncpy(ip_str, ip4addr_ntoa(&ipaddr), sizeof(ip_str));
    ip_str[IP_MAX_LEN-1] = 0;
    strncpy(nm_str, ip4addr_ntoa(&netmask), sizeof(nm_str));
    nm_str[IP_MAX_LEN-1] = 0;
    strncpy(gw_str, ip4addr_ntoa(&gw), sizeof(gw_str));
    gw_str[IP_MAX_LEN-1] = 0;
    printf("Host at %s mask %s gateway %s\n", ip_str, nm_str, gw_str);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, tapif_init, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);
}

void krhino_lwip_init(int enable_tapif)
{
    tcpip_init(NULL, NULL);

    if (enable_tapif)
        tcpip_init_done_callback(NULL);
    printf("TCP/IP initialized.\n");
}
