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

#include "mx6_lwip.h"
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

typedef struct mx6_lwip_timers {
    uint32_t etharp;
    uint32_t tcp;
    uint32_t dhcp_coarse;
    uint32_t dhcp_fine;
    uint32_t autoip;
    uint32_t igmp;
    uint32_t dns;
} mx6_lwip_timers_t;

static mx6_lwip_timers_t s_timers = { 0 };

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void mx6_init_lwip(netif_init_mode_t mode, ip_addr_t *addr, ip_addr_t * mask, ip_addr_t * gw)
{
// Set CTRL_3 high to power on the PHY.
//     max7310_set_gpio_output(0, 2, 0);
//     max7310_set_gpio_output(1, 3, 0);
//     hal_delay_us(100000);
//     max7310_set_gpio_output(1, 3, 1);
//     hal_delay_us(100000);
//     max7310_set_gpio_output(0, 2, 1);
//     
//     lwip_init();
// 
// #if LWIP_NETIF_HOSTNAME
//     g_netif.hostname = "lwip";
// #endif
// 
//     netif_add(&g_netif, &addr, &netmask, &gw, NULL, enet_init, ethernet_input);
//     netif_set_status_callback(&g_netif, netif_status_callback);
//     netif_set_link_callback(&g_netif, netif_link_status_callback);
//     netif_set_default(&g_netif);
// 
//     dns_init();
// 
// Wait for link to come up.
//     printf("Waiting for link...\n");
//     while (true) 
//     {
//         uint32_t status = imx_enet_get_phy_status(dev0);
//         if (status & ENET_STATUS_LINK_ON)
//         {
//             printf("Ethernet link is up!\n");
//             break;
//         }
//         
//         hal_delay_us(100000); // 100 ms
//     }
//     
//     switch (mode)
//     {
//         // DHCP
//         case kDHCP:
//             dhcp_start(&g_netif);
//             break;
//             
//         // Auto IP
//         case kAutoIP:
//             autoip_start(&g_netif);
//             break;
//     
//         // Static IP address
//         case kStaticIP:
//             netif_set_up(&g_netif);
//             break;
//     }
// 
//     printf("TCP/IP initialized.\n");
}

bool check_and_update_ms_timer(uint32_t * lastTime, uint32_t interval_ms)
{
    uint32_t now = sys_now();
    uint32_t elapsed = now - *lastTime;
    bool fire = (elapsed >= interval_ms);
    if (fire)
    {
        *lastTime = sys_now();
    }
    return fire;
}

void mx6_run_lwip(struct netif *netif)
{
    // Poll for incoming Ethernet packet.
    enet_poll_for_packet(netif);

    // ARP timer.
    if (check_and_update_ms_timer(&s_timers.etharp, ARP_TMR_INTERVAL))
    {
        etharp_tmr();
    }

    // TCP timer.
    if (check_and_update_ms_timer(&s_timers.tcp, TCP_TMR_INTERVAL))
    {
        tcp_tmr();
    }

    // DHCP coarse timer.
    if (check_and_update_ms_timer(&s_timers.dhcp_coarse, DHCP_COARSE_TIMER_MSECS))
    {
        dhcp_coarse_tmr();
    }

    // DHCP fine timer.
    if (check_and_update_ms_timer(&s_timers.dhcp_fine, DHCP_FINE_TIMER_MSECS))
    {
        dhcp_fine_tmr();
    }

    // AutoIP fine timer.
    if (check_and_update_ms_timer(&s_timers.autoip, AUTOIP_TMR_INTERVAL))
    {
        autoip_tmr();
    }

    // IGMP fine timer.
//     if (check_and_update_ms_timer(&s_timers.igmp, IGMP_TMR_INTERVAL))
//     {
//         igmp_tmr();
//     }

    // DNS fine timer.
    if (check_and_update_ms_timer(&s_timers.dns, DNS_TMR_INTERVAL))
    {
        dns_tmr();
    }
}


////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
