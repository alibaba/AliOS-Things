/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
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
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
//#include <yoc_syslog.h>
#include <string.h>

#include "aos/list.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"

#include "csi_core.h"
#include "csi_kernel.h"
#include "netif/ethernet.h"
#include "pin.h"
#include "drv_spi.h"
#include "drv_gpio.h"
#include "drv_eth.h"
#include "drv_eth_phy.h"
#include "drv_eth_mac.h"
#include <errno.h>
#ifdef CONFIG_MODULES_SECURITY_TEE_CA
#include <drv_tee.h>
#endif
//#include <link_list.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define NIC_MBOX_SIZE  32

#define NETIF_TASK_STACKSIZE         1024 /* unit 1 byte */
#define NETIF_TASK_PRIO              KPRIO_NORMAL_BELOW0
#define THIS_MODULE MODULE_DEV_ETH

#ifndef CONFIG_NET_INIT_OPEN
static int nic_link_stat = 0;
#else
int nic_link_stat = 0;
uint8_t g_init_flag = 0;
#endif

eth_phy_handle_t eth_phy_handle;
eth_mac_handle_t eth_mac_handle;
k_timer_handle_t timer_send_handle;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

struct netif lwip_netif;

static sys_mbox_t mbox_nic_Int;

struct nic_msg {
    dlist_t node;
    struct netif *netif;
    uint32_t event;
};

dlist_t nic_msg_list;
struct nic_msg nic_msg_node[NIC_MBOX_SIZE];

/* Forward declarations. */
static void  ethernetif_input(struct netif *netif);
static int nic_net_restart(const uint8_t *macaddrs);
void nic_task(void *arg);

int32_t g_fn_phy_read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{

    return 0;
}

int32_t g_fn_phy_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t  data)
{

    return 0;
}

/**
 * netif_set_rs_count.
 *
 * @param NULL
 *
 */

void netif_set_rs_count(struct netif *netif, uint8_t value)
{
#if LWIP_IPV6
    netif->rs_count = value;
#endif
}

/**
 * get network link status.
 *
 * @param NULL
 *
 */
int yoc_net_get_link_status(void)
{
    return nic_link_stat;
}

/**
 * In this function, send a sem for net reset.
 * csios_net_hwreset.
 *
 * @param NULL
 *
 */
int yoc_net_hwreset(void)
{

    uint32_t lpsr = csi_irq_save();
    nic_net_restart(lwip_netif.hwaddr);
    csi_irq_restore(lpsr);
    return 0;
}

static void eth_mac_signal_event(eth_mac_handle_t handle, uint32_t event)
{
    int i = 0;

    if (nic_msg_list.next == nic_msg_list.prev)
    {
        dlist_init(&nic_msg_list);

        for (i = 0; i < NIC_MBOX_SIZE; i++) {
            dlist_add_tail(&nic_msg_node[i].node, &nic_msg_list);
        }

        return;
    }

    /* not need to modify msg, it's like a semaphore */
    struct nic_msg *evt = (struct nic_msg *)nic_msg_list.next;
    dlist_del(&evt->node);

    evt->event = event;

    sys_mbox_trypost(&mbox_nic_Int, evt);
}

/**
 * enc28j60_net_restart.
 *
 * @param macaddr for net
 *
 */
static int nic_net_restart(const uint8_t *macaddrs)
{
    int ret = -1;

    /* restart init */
    eth_mac_handle = csi_eth_mac_initialize(eth_mac_signal_event);
    /* enable irq */
    ret = csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    return ret;
}

void nic_send_timeout_cb(void *arg)
{
    uint32_t lpsr = csi_irq_save();
    csi_eth_mac_initialize(eth_mac_signal_event);
    csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    csi_irq_restore(lpsr);
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
#ifndef CONFIG_DRIVER_ETH_MACADDR
#ifdef CONFIG_MODULES_SECURITY_TEE_CA
    int random;

    csi_tee_rand_generate((uint8_t*)&random, 4);
#else
    int random = rand();
#endif
#endif
    int i;
    /* set MAC hardware address length */
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN; //ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
#ifdef CONFIG_DRIVER_ETH_MACADDR
    netif->hwaddr[0] = (CONFIG_DRIVER_ETH_MACADDR >> 40) & 0xff;
    netif->hwaddr[1] = (CONFIG_DRIVER_ETH_MACADDR >> 32) & 0xff;
    netif->hwaddr[2] = (CONFIG_DRIVER_ETH_MACADDR >> 24) & 0xff;
    netif->hwaddr[3] = (CONFIG_DRIVER_ETH_MACADDR >> 16) & 0xff;
    netif->hwaddr[4] = (CONFIG_DRIVER_ETH_MACADDR >> 8) & 0xff;
    netif->hwaddr[5] = (CONFIG_DRIVER_ETH_MACADDR) & 0xff;
#else
    netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = 0x11;
    netif->hwaddr[2] = 0x22;
    netif->hwaddr[3] = (random >> 16) & 0xFF;
    netif->hwaddr[4] = (random >> 8) & 0xFF;
    netif->hwaddr[5] = (random) & 0xFF;
#endif
    /* maximum transfer unit */
    netif->mtu = TCP_MSS + 54 + 10;  /* net head + redundance */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IPV6 && LWIP_IPV6_MLD

    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL) {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }

#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

    csi_eth_mac_set_macaddr(eth_mac_handle, (const eth_mac_addr_t *)netif->hwaddr);

    eth_phy_handle = csi_eth_phy_initialize(g_fn_phy_read, g_fn_phy_write);

    eth_mac_handle = csi_eth_mac_initialize(eth_mac_signal_event);

    nic_link_stat = csi_eth_phy_get_linkstate(eth_phy_handle);

    /* Do whatever else is needed to initialize interface. */
    sys_mbox_new(&mbox_nic_Int, NIC_MBOX_SIZE);
    timer_send_handle = csi_kernel_timer_new(nic_send_timeout_cb, KTIMER_TYPE_ONCE, NULL);

    dlist_init(&nic_msg_list);

    for (i = 0; i < NIC_MBOX_SIZE; i++) {

        dlist_add_tail(&nic_msg_node[i].node, &nic_msg_list);
    }

    //LOG_I("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);

    sys_thread_new("nic_task", nic_task, &lwip_netif, NETIF_TASK_STACKSIZE, NETIF_TASK_PRIO);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
#ifdef CONFIG_ETH_ENC28J60_USE_PBUF
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    int l = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    csi_kernel_timer_stop(timer_send_handle);
    csi_kernel_timer_start(timer_send_handle, csi_kernel_ms2tick(10000));

    uint32_t lpsr = csi_irq_save();

    csi_eth_mac_ex_send_frame_begin(eth_mac_handle, p->tot_len);

    for (q = p; q != NULL; q = q->next) {
        csi_eth_mac_ex_send_frame(eth_mac_handle, q->payload, q->len, 0);
        l = l + q->len;
    }
    csi_eth_mac_ex_send_frame_end(eth_mac_handle);

    csi_irq_restore(lpsr);

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);

    if (((u8_t *)p->payload)[0] & 1) {
        /* broadcast or multicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    } else {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }

    /* increase ifoutdiscards or ifouterrors on error */

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
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    int32_t len;
    int l = 0;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = csi_eth_mac_ex_read_frame_begin(eth_mac_handle);

    if (len < 0) {
        /* errors in rx buffer, reset enc28j60 */
        //LOG_E("err rxb, rst");

        eth_mac_handle = csi_eth_mac_initialize(eth_mac_signal_event);

        csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
        return NULL;
    } else if (len == 0) {
        return NULL;
    }

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
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            /* read data into(q->payload, q->len); */
            csi_eth_mac_ex_read_frame(eth_mac_handle, (u8_t *)q->payload, q->len);
            l = l + q->len;
        }

        csi_eth_mac_ex_read_frame_end(eth_mac_handle);

        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);

        if (((u8_t *)p->payload)[0] & 1) {
            /* broadcast or multicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {
            /* unicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* drop packet(); */
        /* error to allocate a pbuf chain of pbufs from the pool */
        //LOG_E("err alloc pbuf");
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    return p;
}
#else

uint8_t Data_Buf[1528];
uint8_t Tx_Data_Buf[1528];

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    int l = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    csi_kernel_timer_stop(timer_send_handle);
    csi_kernel_timer_start(timer_send_handle, csi_kernel_ms2tick(10000));

    uint32_t lpsr = csi_irq_save();


    for (q = p; q != NULL; q = q->next) {
        memcpy((u8_t*)&Tx_Data_Buf[l], q->payload, q->len);
        l = l + q->len;
    }

    csi_eth_mac_send_frame(eth_mac_handle, Tx_Data_Buf, p->tot_len, 0);

    csi_irq_restore(lpsr);

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);

    if (((u8_t *)p->payload)[0] & 1) {
        /* broadcast or multicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    } else {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }

    /* increase ifoutdiscards or ifouterrors on error */

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
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    int32_t len;
    int l = 0;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = csi_eth_mac_read_frame(eth_mac_handle, Data_Buf, MAX_FRAMELEN);

    if (len < 0) {
        /* errors in rx buffer, reset enc28j60 */
        //LOG_E("err rxb, rst");

        eth_mac_handle = csi_eth_mac_initialize(eth_mac_signal_event);

        csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
        return NULL;
    } else if (len == 0) {
        return NULL;
    }

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
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            /* read data into(q->payload, q->len); */
            memcpy((u8_t*)q->payload, (u8_t*)&Data_Buf[l], q->len);
            l = l + q->len;
        }

        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);

        if (((u8_t *)p->payload)[0] & 1) {
            /* broadcast or multicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {
            /* unicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* drop packet(); */
        /* error to allocate a pbuf chain of pbufs from the pool */
        //LOG_E("err alloc pbuf");
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    return p;
}
#endif
/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void ethernetif_input(struct netif *netif)
{
    //struct ethernetif *ethernetif;
    struct pbuf *p;

    //ethernetif = netif->state;

    /* move received packet into a new pbuf */
    uint32_t lpsr = csi_irq_save();
    p = low_level_input(netif);
    csi_irq_restore(lpsr);

    /* if no packet could be read, silently ignore this */
    if (p != NULL) {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\r\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));

    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\r\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "cksy-lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

void nic_task(void *arg)
{
    struct netif *netif = arg;
    struct nic_msg *msg;
    int link_status;

    while (1) {

        sys_arch_mbox_fetch(&mbox_nic_Int, (void **)&msg, 0);

        do {
            csi_kernel_timer_stop(timer_send_handle);

            if (msg->event == CSI_ETH_MAC_EVENT_LINK_CHANGE) {
                uint32_t lpsr = csi_irq_save();
                /* don't disable interrupt that may lost interrupt */
                link_status = csi_eth_phy_get_linkstate(eth_phy_handle);
                csi_irq_restore(lpsr);

                if ((link_status != -1)) {
                    nic_link_stat = link_status;
                    //LOG_E("Ethernet:link %s", (nic_link_stat == 1) ? "up" : "down");
                }
            } else if (msg->event == CSI_ETH_MAC_EVENT_RX_FRAME) {

                ethernetif_input(netif);

                /*if there are packets in rx buffer, interrupt will be triggered again*/
            } else if (msg->event == CSI_ETH_MAC_EVENT_TX_FRAME) {

            }

            dlist_add_tail(&msg->node, &nic_msg_list);
        } while (sys_arch_mbox_tryfetch(&mbox_nic_Int, (void **)&msg) == ERR_OK);

        csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);

    }
}

/********************************************************************************
 * Name:
 *          netif_proto_init
 *
 * Description:
 *          the network card init call low_level_init()
 *
 * Parameter:
 *          netif:struct for network card
 *
 * Return:
 *          the status of init
 ********************************************************************************/
static void tcpip_init_done(void *arg)
{
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
#endif

#if LWIP_IPV4
#ifdef CONFIG_CLI_NET_IPADDR
    ipaddr.addr = CONFIG_CLI_NET_IPADDR;
    netmask.addr = 0x00ffffff;
    gw.addr = ((CONFIG_CLI_NET_IPADDR & 0x00ffffff) | 0x01000000);
#else /* SET default IP */
    IP4_ADDR(&ipaddr, 192, 168, 103, 150);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 103, 1);
#endif
    //LOG_I("ip addr: %d.%d.%d.%d\n", ((ipaddr.addr) & 0xff), ((ipaddr.addr >> 8) & 0xff),
    //      ((ipaddr.addr >> 16) & 0xff), ((ipaddr.addr >> 24) & 0xff));
    netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
#endif

#if LWIP_IPV6
#if !LWIP_IPV4
    netif_add(&lwip_netif, NULL, ethernetif_init, tcpip_input);
#endif
    netif_create_ip6_linklocal_address(&lwip_netif, 1);
    lwip_netif.ip6_autoconfig_enabled = 1;
#endif

    netif_set_default(&lwip_netif);
    netif_set_up(&lwip_netif);
}


/********************************************************************************
 * Name:
 *          netif_proto_init
 *
 * Description:
 *          the network card init call low_level_init()
 *
 * Parameter:
 *          netif:struct for network card
 *
 * Return:
 *          the status of init
 ********************************************************************************/
int lwip_tcpip_init(void)
{
#ifdef PERF
    perf_init("/tmp/minimal.perf");
#endif /* PERF */

    tcpip_init(tcpip_init_done, NULL);
    //LOG_I("TCP/IP initialized.");

    return 0;
}

