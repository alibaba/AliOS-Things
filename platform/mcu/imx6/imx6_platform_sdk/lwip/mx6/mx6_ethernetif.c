/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include <assert.h>

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

#include "sdk.h"
#include "iomux_config.h"
#include "registers/regsiomuxc.h"

#if CHIP_MX6DQ || CHIP_MX6SDL
#include "enet/enet.h"
#elif CHIP_MX6SL
#include "fec/fec.h"
#endif

#if CHIP_MX6DQ || CHIP_MX6SDL
//! @todo Fix me to make me board-independant!! Put me in board lib.
#if defined(BOARD_SMART_DEVICE) || defined(BOARD_SABRE_AI)
#define ENET_PHY_ADDR 1
#elif defined(BOARD_EVB)
#define ENET_PHY_ADDR 0
#else
#error Unknown ENET_PHY_ADDR
#endif
#endif // CHIP_MX6DQ || CHIP_MX6SDL

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
// struct enet {
//     struct eth_addr *ethaddr;
//     /* Add whatever per-interface state that is needed here. */
// };

#if CHIP_MX6DQ || CHIP_MX6SDL

static imx_enet_priv_t enet0;
imx_enet_priv_t *g_en0 = &enet0;

extern int imx_enet_mii_type(imx_enet_priv_t * dev, enum imx_mii_type mii_type);
extern void imx_enet_iomux(void);
extern void imx_enet_phy_reset(void);

#elif CHIP_MX6SL

static imx_fec_priv_t fec0;
imx_fec_priv_t *g_en0 = &fec0;

extern int imx_fec_mii_type(imx_fec_priv_t * dev, enum imx_mii_type mii_type);
extern void imx_feciomux(void);
extern void imx_fec_phy_reset(void);

#endif

static unsigned char s_pkt_send[2048];
static unsigned char s_pkt_recv[2048];
static unsigned char mac_addr0[6] = { 0x00, 0x04, 0x9f, 0x00, 0x00, 0x01 };

/* Forward declarations. */
static void  enet_input(struct netif *netif);


void enet_set_mac(const uint8_t * macAddress)
{
    memcpy(mac_addr0, macAddress, sizeof(mac_addr0));
}

#if CHIP_MX6DQ || CHIP_MX6SDL

void init_enet(void)
{
    // setup iomux for ENET
    imx_enet_iomux();
    imx_enet_phy_reset();

    // init enet0
    imx_enet_init(g_en0, ENET_BASE_ADDR, ENET_PHY_ADDR);
    imx_enet_mii_type(g_en0, RGMII);
    
    // init phy0.
    imx_enet_phy_init(g_en0);

    // Check PHY link status.
    if (!(g_en0->status & ENET_STATUS_LINK_ON))
    {
            printf("ENET link status check fail\n");
    }

    imx_enet_start(g_en0, mac_addr0);
}

#elif CHIP_MX6SL

int imx_fec_setup(void)
{
	/* config iomux */
	fec_iomux_config();
	
	/* gpio4_21 controls the power of FEC PHY */
	gpio_set_direction(4, 21, 1);
	gpio_set_level(4, 21, 1);
	hal_delay_us(100000);
	
	/* get enet tx reference clk from internal clock from anatop
	 * GPR1[14] = 0, GPR1[18:17] = 00
	 */
	HW_IOMUXC_GPR1_CLR(
	        BM_IOMUXC_GPR1_ENET_CLK_SEL_FROM_ANALOG_LOOPBACK |
	        BM_IOMUXC_GPR1_ENET_CLK_SEL);
	
	/* Enable ENET PLLs */
	/* already done in ccm_init() */

	return 0;
}

void init_fec(void)
{
    // setup iomux for ENET
    imx_fec_setup();

    // init fec0
    imx_fec_init(g_en0, REGS_FEC_BASE, 0);
    imx_fec_mii_type(g_en0, RMII);
    
    // init phy0.
    imx_fec_phy_init(g_en0);

    // Check PHY link status.
    if (!(g_en0->status & FEC_STATUS_LINK_ON))
    {
            printf("FEC link status check fail\n");
    }

    imx_fec_start(g_en0, mac_addr0);
}

#endif // CHIP_MX6SL

/**
 * In this function, the hardware should be initialized.
 * Called from enet_init().
 *
 * @param netif the already initialized lwip network interface structure
 *              for this enet
 */
static void
low_level_init(struct netif *netif)
{
//     struct enet *enet = netif->state;
    
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    assert(sizeof(mac_addr0) == ETHARP_HWADDR_LEN);
    memcpy(netif->hwaddr, mac_addr0, ETHARP_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500;
    
    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 
    /* Do whatever else is needed to initialize interface. */    
#if CHIP_MX6DQ || CHIP_MX6SDL
    init_enet();
#elif CHIP_MX6SL
    init_fec();
#endif
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this enet
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *               an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *           strange results. You might consider waiting for space in the DMA queue
 *           to become availale since the stack doesn't retry to send a packet
 *           dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
//     struct enet *enet = netif->state;
    struct pbuf *q;
    u32_t l = 0;

//   initiate transfer();
    
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for(q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
             time. The size of the data in each pbuf is kept in the ->len
             variable. */
        memcpy(&s_pkt_send[l], (u8_t*) q->payload, q->len);
        l += q->len;
    }

//     printf("enetif: sending %d bytes\n", l);
#if CHIP_MX6DQ || CHIP_MX6SDL
    imx_enet_send(g_en0, s_pkt_send, l, 1);
#elif CHIP_MX6SL
    imx_fec_send(g_en0, s_pkt_send, l, 1);
#endif

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    
    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this enet
 * @return a pbuf filled with the received packet (including MAC header)
 *               NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
//     struct enet *enet = netif->state;
    struct pbuf *p, *q;
    int len;
	u16_t l = 0;

    /* Obtain the size of the packet and put it into the "len"
         variable. */
#if CHIP_MX6DQ || CHIP_MX6SDL
    imx_enet_recv(g_en0, s_pkt_recv, &len);
#elif CHIP_MX6SL
    imx_fec_recv(g_en0, s_pkt_recv, &len);
#endif
//     printf("enetif: received %d bytes\n", len);

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    
    if (p != NULL) {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            memcpy((u8_t*) q->payload, &s_pkt_recv[l], q->len);
            l = l + q->len;
        }
//         acknowledge that packet has been read();

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;    
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this enet
 */
static void
enet_input(struct netif *netif)
{
    struct enet *enet;
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    enet = netif->state;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* no packet could be read, silently ignore this */
    if (p == NULL) return;
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type)) {
    /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_IPV6:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
    /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
        /* full packet send to tcpip_thread to process */
        if (netif->input(p, netif)!=ERR_OK)
         { LWIP_DEBUGF(NETIF_DEBUG, ("enet_input: IP input error\n"));
             pbuf_free(p);
             p = NULL;
         }
        break;

    default:
        pbuf_free(p);
        p = NULL;
        break;
    }
}

void enet_poll_for_packet(struct netif * netif)
{
    unsigned long enet_events;
    
#if CHIP_MX6DQ || CHIP_MX6SDL
    enet_events = imx_enet_poll(g_en0);
//     if (enet_events)
//     {
//         printf("enet_events = 0x%x\n", enet_events);
//     }
    if (enet_events & ENET_EVENT_RX)
    {
        enet_input(netif);
    }
#elif CHIP_MX6SL
    enet_events = imx_fec_poll(g_en0);
    
    if (enet_events & FEC_EVENT_RX)
    {
        enet_input(netif);
    }
#endif
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this enet
 * @return ERR_OK if the loopif is initialized
 *               ERR_MEM if private data couldn't be allocated
 *               any other err_t on error
 */
err_t
enet_init(struct netif *netif)
{
//     struct enet *enet;

    LWIP_ASSERT("netif != NULL", (netif != NULL));
        
//     enet = mem_malloc(sizeof(struct enet));
//     if (enet == NULL) {
//         LWIP_DEBUGF(NETIF_DEBUG, ("enet_init: out of memory\n"));
//         return ERR_MEM;
//     }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 1000000);

//     netif->state = enet;
    netif->state = NULL;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;
    
//     enet->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
    
    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

