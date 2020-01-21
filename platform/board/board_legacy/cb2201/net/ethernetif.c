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
#include <csi_config.h>
#ifdef CONFIG_NETIF_ETH

//#include <csi_syslog.h>
#include "aos/kernel.h"
#include <string.h>
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
#ifdef CONFIG_TEE_CA
#include <drv_tee.h>
#endif
#include <aos/list.h>

#include "lwip/sockets.h"


/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

enum nic_event {
    NIC_EVENT_INT,
    NIC_EVENT_API_RESET
};

static const char *TAG = "netif";

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

static gpio_pin_handle_t pin_int = NULL;
static eth_phy_handle_t eth_phy_handle;
static eth_mac_handle_t eth_mac_handle;

static k_timer_handle_t timer_send_handle;
static int nic_link_stat = 0;
static int g_bTimerStopped = 1;
static uint8_t g_continue_send_error = 0;
static uint8_t g_continue_rx_error = 0;

static void eth_gpio_config_int(bool en);
static err_t eth_handle_event(struct netif *netif, uint32_t event);
static void  ethernetif_input(struct netif *netif);

/* Forward declarations. */
extern int32_t csi_eth_mac_ex_send_frame_begin(eth_mac_handle_t handle, uint32_t len);
extern int32_t csi_eth_mac_ex_send_frame(eth_mac_handle_t handle, const uint8_t *frame, uint32_t len, uint32_t flags);
extern int32_t csi_eth_mac_ex_send_frame_end(eth_mac_handle_t handle);
extern int32_t csi_eth_mac_ex_read_frame(eth_mac_handle_t handle, uint8_t *frame, uint32_t len);
extern int32_t csi_eth_mac_ex_read_frame_begin(eth_mac_handle_t handle);
extern int32_t csi_eth_mac_ex_read_frame_end(eth_mac_handle_t handle);
extern uint32_t eth_get_event(int32_t idx);
int yoc_nic_reset(void);

#ifdef GPIO_PIN_FOR_DEBUG
static gpio_pin_handle_t *pin = NULL;

void pin_set_level(int level)
{
    if (level) {
        csi_gpio_pin_write(pin, 1);
    } else {
        csi_gpio_pin_write(pin, 0);
    }
}

void pin_toggle(void)
{
    static int level = 0;

    level ^= 0x01;
    csi_gpio_pin_write(pin, level);
}

void pin_gpio_init(void)
{
    drv_pinmux_config(PA18, PIN_FUNC_GPIO);

    pin = csi_gpio_pin_initialize(PA18, NULL);
    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);
}
#else
void pin_toggle(void)
{
    // do nothing;
}
#endif

/**
 int line:
 -----------            ----------------------
            |___________|   |_________________
            1           2 3 4
 1. a nic event arised, enc28j60 pulls int line down
    run ISR to tell tcpip thread to handle the event
 2. switch to tcpip thread, clear INTIE, save and clear all pending event
 3. handle the event one by one
 4. set INTIE, if new event is valid while handling event, enc28j60 pulls line down
    otherwise, keep the line high as usually

 If a falling edge of GPIO is missing, the line is always kept low,
 no gpio interrupt will be triggered any more.
 As consider of reliability, trigger interrupt by low level
 */
static void eth_int_handle(int32_t idx)
{
    pin_toggle();

    eth_gpio_config_int(0);

    tcpip_signal_netif_event(&lwip_netif, NIC_EVENT_INT, eth_handle_event);
}

#if 0
static bool eth_check_card_is_existed(void)
{
    bool val;

    csi_gpio_pin_read(pin_int, &val);

    if (val == 1) {
        return true;
    } else {
        return false;
    }
}
#endif

static void eth_gpio_config_int(bool en)
{
    if (en) {
        csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 1);
    } else {
        csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 0);
    }
}

bool eth_gpio_get_level(void)
{
    bool pin_level;
    csi_gpio_pin_read(pin_int, &pin_level);

    return pin_level;
}

static int32_t eth_gpio_init(int32_t pin, gpio_event_cb_t event_cb)
{
    uint32_t ret;

    pin_int = csi_gpio_pin_initialize(pin, event_cb);
    ret = csi_gpio_pin_config_mode(pin_int, GPIO_MODE_PULLUP);
    ret = csi_gpio_pin_config_direction(pin_int, GPIO_DIRECTION_INPUT);

    ret = csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 0);

    return ret;
}

static int32_t g_fn_phy_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    return 0;
}

static int32_t g_fn_phy_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t  data)
{
    return 0;
}

/**
 *  Hard reset enc28j60
 * @param void
 *
 */
static void eth_hard_reset(void)
{
    gpio_pin_handle_t pin = NULL;
    pin = csi_gpio_pin_initialize(ENC28J60_ETH_PIN_RST, NULL);
    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pin, 0);  /* LOW */
    csi_kernel_delay_ms(10);
    csi_gpio_pin_write(pin, 1);
    csi_kernel_delay_ms(50);
}

static void eth_send_timeout_cb(void *arg)
{
    LOGI(TAG, "eth_send_timeout_cb");
    g_continue_send_error++;

    if (g_continue_send_error >= 3) {
#if defined(CONFIG_SELF_RECOVER)
        //yoc_recover_send_event(YOC_EXP_ETH(YOC_RCV_TXERR), YOC_RCV_ACT_REBOOT, YOC_UNRCV_ACT_SLEEP);
#endif
    }

    yoc_nic_reset();
}

static err_t eth_handle_event(struct netif *netif, uint32_t event)
{
    int link_status;
    uint32_t eth_event;

    pin_toggle();

    if (event == NIC_EVENT_API_RESET) {
        LOGI(TAG, "reset ethernet card.");
        /* restart init */
        eth_hard_reset();
        eth_mac_handle = csi_eth_mac_initialize(0, NULL);
        /* enable irq */
        csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
        eth_gpio_config_int(1);
        g_bTimerStopped = 1;
    } else {
        eth_event = eth_get_event(0);

        if (eth_event) {
            csi_kernel_timer_stop(timer_send_handle);
            g_bTimerStopped = 1;
            g_continue_send_error = 0;
        } else {
            //printf("invalid interrupt\n");
        }

        if (eth_event & 0x80) {
            LOGI(TAG, "reset ethernet card.");
            /* restart init */
            eth_hard_reset();
            eth_mac_handle = csi_eth_mac_initialize(0, NULL);
            /* enable irq */
        } else {
            if (eth_event & CSI_ETH_MAC_EVENT_LINK_CHANGE) {
                link_status = csi_eth_phy_get_linkstate(eth_phy_handle);

                if ((link_status != -1)) {
                    nic_link_stat = link_status;
                    LOGI(TAG, "Net:link %s", (link_status == 1) ? "up" : "down");

                    if (link_status == 1) {
                        //aos_post_event(EV_NET, CODE_NET_ON_CONNECTED, VALUE_NET_ETH);
                    } else {
                        //aos_post_event(EV_NET, CODE_NET_ON_DISCONNECT, VALUE_NET_ETH);
                    }
                }
            }

            if (eth_event == CSI_ETH_MAC_EVENT_RX_FRAME) {

                ethernetif_input(netif);

                /*if there are packets in rx buffer, interrupt will be triggered again*/
            }

            if (eth_event == CSI_ETH_MAC_EVENT_TX_FRAME) {

            }
            pin_toggle();
        }
        //Enable interrupt
        csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
        eth_gpio_config_int(1);
    }
    return 0;
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
#ifndef CONFIG_ETH_MACADDR
#ifdef CONFIG_TEE_CA
    int random;

    csi_tee_rand_generate((uint8_t *)&random, 4);
#else
    int random = rand();
#endif
#endif
    //int i;
    /* set MAC hardware address length */
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN; //ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
#ifdef CONFIG_ETH_MACADDR
    netif->hwaddr[0] = (CONFIG_ETH_MACADDR >> 40) & 0xff;
    netif->hwaddr[1] = (CONFIG_ETH_MACADDR >> 32) & 0xff;
    netif->hwaddr[2] = (CONFIG_ETH_MACADDR >> 24) & 0xff;
    netif->hwaddr[3] = (CONFIG_ETH_MACADDR >> 16) & 0xff;
    netif->hwaddr[4] = (CONFIG_ETH_MACADDR >> 8) & 0xff;
    netif->hwaddr[5] = (CONFIG_ETH_MACADDR) & 0xff;
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

    /* Do whatever else is needed to initialize interface. */
    timer_send_handle = csi_kernel_timer_new(eth_send_timeout_cb, KTIMER_TYPE_ONCE, NULL);

    eth_gpio_init(ENC28J60_ETH_PIN_INT, eth_int_handle);
#if 0

    // first, check if network card is existed
    if (eth_check_card_is_existed() == false) {
        LOGE(TAG, "ethernet card is not existed");
#if defined(CONFIG_SELF_RECOVER)
        yoc_recover_send_event(YOC_EXP_ETH(YOC_RCV_CARD_REMOVED), YOC_RCV_ACT_REBOOT, YOC_UNRCV_ACT_SLEEP);
#endif
        return;
    }

#endif

    eth_hard_reset();

    csi_eth_mac_set_macaddr(eth_mac_handle, (const eth_mac_addr_t *)netif->hwaddr);

    int retry = 0;

    do {
        eth_phy_handle = csi_eth_phy_initialize(g_fn_phy_read, g_fn_phy_write);
        eth_mac_handle = csi_eth_mac_initialize(0, NULL);
        retry++;
        // TODO: power off and power on ethernet module
    } while ((eth_mac_handle == NULL) && (retry < 3));

    if (eth_mac_handle != NULL) {
        LOGD(TAG, "csi_eth init OK");
    } else {
        LOGE(TAG, "failed to init ethernet chip");
#if defined(CONFIG_SELF_RECOVER)
        yoc_recover_send_event(YOC_EXP_ETH(YOC_RCV_INITERR), YOC_RCV_ACT_REBOOT, YOC_UNRCV_ACT_SLEEP);
#endif
    }

    LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    eth_gpio_config_int(1);

#ifdef GPIO_PIN_FOR_DEBUG
    pin_gpio_init();
#endif
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
    pin_toggle();

    if (g_bTimerStopped) {
        csi_kernel_timer_stop(timer_send_handle);
        csi_kernel_timer_start(timer_send_handle, csi_kernel_ms2tick(10000));
        g_bTimerStopped = 0;
    }

    csi_eth_mac_ex_send_frame_begin(eth_mac_handle, p->tot_len);

    for (q = p; q != NULL; q = q->next) {
        csi_eth_mac_ex_send_frame(eth_mac_handle, q->payload, q->len, 0);
        l = l + q->len;
    }

    csi_eth_mac_ex_send_frame_end(eth_mac_handle);
    pin_toggle();

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
        LOGE(TAG, "recv frame error");
        g_continue_rx_error++;

        if (g_continue_rx_error >= 3) {
#if defined(CONFIG_SELF_RECOVER)
            //yoc_recover_send_event(YOC_EXP_ETH(YOC_RCV_RXERR), YOC_RCV_ACT_REBOOT, YOC_UNRCV_ACT_SLEEP);
#endif
        }

        // TODO: power off/on ethernet card
        yoc_nic_reset();

        return NULL;
    } else if (len == 0) {
        return NULL;
    }

    g_continue_rx_error = 0;

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
        LOGI(TAG, "err alloc pbuf");
        /* drop packet(); */
        csi_eth_mac_ex_read_frame_end(eth_mac_handle);
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

    for (q = p; q != NULL; q = q->next) {
        memcpy((u8_t *)&Tx_Data_Buf[l], q->payload, q->len);
        l = l + q->len;
    }

    csi_eth_mac_send_frame(eth_mac_handle, Tx_Data_Buf, p->tot_len, 0);

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
        LOGE(TAG, "err rxb, rst");

        yoc_nic_reset();

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
            memcpy((u8_t *)q->payload, (u8_t *)&Data_Buf[l], q->len);
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
        LOGE(TAG, "err alloc pbuf");
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
    p = low_level_input(netif);

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
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));
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
    LOGI(TAG, "TCP/IP initialized.");

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
int yoc_nic_reset(void)
{
    tcpip_signal_netif_event(&lwip_netif, 0x80, eth_handle_event);
    return 0;
}


#endif /* CONFIG_NETIF_ETH */
