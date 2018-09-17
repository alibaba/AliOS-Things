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
#include "ping.h"

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

#define PING_INTERVAL_MS (1000) //!< 1 second

enum _fsm_states
{
    kInitingState,
    kAskForTargetState,
    kSendPingState,
    kWaitState,
    kPauseBetweenPingsState
};

struct netif g_netif;
char g_buf[64];
char * g_ping_target_hostname = NULL;
ip_addr_t g_ping_target_addr;
int g_state = kInitingState;
bool g_isNetifUp = false;

const uint8_t kMACAddress[] = { 0x00, 0x04, 0x9f, 0x00, 0x00, 0x01 };

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void netif_status_callback(struct netif *netif)
{
    g_isNetifUp = (g_netif.flags & NETIF_FLAG_UP);
    printf("netif: %s (ip=%s)\n",
        g_isNetifUp ? "up" : "down",
        ipaddr_ntoa_r(&g_netif.ip_addr, g_buf, sizeof(g_buf)));
    
    g_state = kAskForTargetState;
}

void netif_link_status_callback(struct netif *netif)
{
    bool isLinkUp = g_netif.flags & NETIF_FLAG_LINK_UP;
    printf("netif: link %s\n", isLinkUp ? "up" : "down");
}

void init_lwip(void)
{
#if BOARD_EVB
    // Set CTRL_3 high to power on the PHY.
    max7310_set_gpio_output(0, 2, 0);
    max7310_set_gpio_output(1, 3, 0);
    hal_delay_us(100000);
    max7310_set_gpio_output(1, 3, 1);
    hal_delay_us(100000);
    max7310_set_gpio_output(0, 2, 1);
#endif
    
    lwip_init();

    ip_addr_t addr;
    ip_addr_t netmask;
    ip_addr_t gw;
    IP4_ADDR(&addr, 192, 168, 10, 159);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 10, 200);

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
    
    ping_init();

    printf("TCP/IP initialized.\n");
}

void dns_found(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    if (ipaddr)
    {
        printf("Resolved %s to %s\n", name,
            ipaddr_ntoa_r(ipaddr, g_buf, sizeof(g_buf)));
        
        // Setting this flag will start pings.
        g_state = kSendPingState;
    }
    else
    {
        printf("Could not resolve hostname %s\n", name);
        g_state = kAskForTargetState;
    }
}

void get_target_address(void)
{
    printf("\n== Ping test ==\n");
    printf("Enter the target IP address or host name:\n");
    printf("> ");
    fflush(stdout);
    
    g_ping_target_hostname = read_input_string(NULL);
    
    // Convert name to IP.
    dns_gethostbyname(g_ping_target_hostname, &g_ping_target_addr, dns_found, NULL);
}

void ping_received(const struct icmp_echo_hdr * echo, const ip_addr_t *addr, int elapsed)
{
    if (echo)
    {
        printf("ping: recv seq=%d %d ms\n", ntohs(echo->seqno), elapsed);
    }
    else
    {
        printf("ping: timed out\n");
    }
    
    g_state = kPauseBetweenPingsState;
}

void run(void)
{
    uint32_t last_ping_sent = 0;
    
    while (true)
    {
        mx6_run_lwip(&g_netif);
        
        switch (g_state)
        {
            // Do nothing while waiting for netif to come up.
            case kInitingState:
                break;
                
            case kAskForTargetState:
                get_target_address();
                g_state = kWaitState;
                break;
            
            // Wait for either DNS response or ping reply.
            case kWaitState:
                break;
            
            case kSendPingState:
                IP4_ADDR(&g_ping_target_addr, 10, 81, 4, 140);
                ping_send_to(&g_ping_target_addr, ping_received);

                last_ping_sent = sys_now();
                g_state = kWaitState;
                break;
            
            case kPauseBetweenPingsState:
                if (check_and_update_ms_timer(&last_ping_sent, PING_INTERVAL_MS))
                {
                    g_state = kSendPingState;
                }
                break;
        }
    }
}

void main(void)
{
    platform_init();
    
    init_lwip();

    run();
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
