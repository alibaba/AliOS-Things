/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "platform_init.h"
#include "iomux_config.h"
#include "timer/timer.h"
#include "httpd.h"
#include "fs.h"
#include "fsdata.h"
#include "httpd_structs.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/raw.h"
#include "lwip/debug.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/ip.h"
#include "lwip/ip_frag.h"
#include "lwip/udp.h"
#include "lwip/snmp_msg.h"
#include "lwip/tcp_impl.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/autoip.h"

#include "mx6_lwip.h"

//! The network interface.
struct netif g_netif;

const uint8_t kMACAddress[] = { 0x00, 0x04, 0x9f, 0x00, 0x00, 0x01 };

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void netif_status_callback(struct netif *netif)
{
    char buf[64];
    bool isUp = (g_netif.flags & NETIF_FLAG_UP);
    printf("netif: %s (ip=%s)\n",
        isUp ? "up" : "down",
        ipaddr_ntoa_r(&g_netif.ip_addr, buf, sizeof(buf)));
}

void netif_link_status_callback(struct netif *netif)
{
    bool isUp = (g_netif.flags & NETIF_FLAG_LINK_UP);
    printf("netif: link %s\n",
        isUp ? "up" : "down");
}

void init_lwip(void)
{
    lwip_init();

    ip_addr_t addr;
    ip_addr_t netmask;
    ip_addr_t gw;
    IP4_ADDR(&addr, 10, 81, 4, 142);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 10, 81, 7, 254);

#if LWIP_NETIF_HOSTNAME
    g_netif.hostname = "lwip";
#endif

    // Set the MAC address.
    enet_set_mac(kMACAddress);

    // Create the netif.
    netif_add(&g_netif, &addr, &netmask, &gw, NULL, enet_init, ethernet_input);
    netif_set_status_callback(&g_netif, netif_status_callback);
    netif_set_link_callback(&g_netif, netif_link_status_callback);
    netif_set_default(&g_netif);

    dns_init();

    // Wait for link to come up.
    printf("Waiting for link...\n");
    while (true) 
    {
#if CHIP_MX6DQ || CHIP_MX6SDL
        uint32_t status = imx_enet_get_phy_status(g_en0);
        if (status & ENET_STATUS_LINK_ON)
#elif CHIP_MX6SL
        uint32_t status = imx_fec_get_phy_status(g_en0);
        if (status & FEC_STATUS_LINK_ON)
#endif
        {
            printf("Ethernet link is up!\n");
            break;
        }
        
        hal_delay_us(100000); // 100 ms
    }

    // DHCP
    if (1)
    {
        dhcp_start(&g_netif);
    }
    // Auto IP
    else if (0)
    {
        autoip_start(&g_netif);
    }
    // Static IP address
    else
    {
        netif_set_up(&g_netif);
    }
    
    httpd_init();

    printf("TCP/IP initialized.\n");
}

void main(void)
{
    platform_init();
    init_lwip();

    while (true)
    {
        mx6_run_lwip(&g_netif);
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
