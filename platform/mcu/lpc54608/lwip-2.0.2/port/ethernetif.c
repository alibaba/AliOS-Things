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
 * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SDRVL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#if USE_RTOS && defined(FSL_RTOS_AOS)
#include "aos/aos.h"
#endif

#include "ethernetif.h"

#include "fsl_enet.h"
#include "fsl_phy.h"
#include <k_api.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
    #if defined(FSL_FEATURE_L2CACHE_LINESIZE_BYTE) \
        && ((!defined(FSL_SDK_DISBLE_L2CACHE_PRESENT)) || (FSL_SDK_DISBLE_L2CACHE_PRESENT == 0))
        #if defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
            #define FSL_CACHE_LINESIZE_MAX MAX(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE, FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
            #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_CACHE_LINESIZE_MAX)
        #else
            #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_FEATURE_L2CACHE_LINESIZE_BYTE)
        #endif
    #elif defined(FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
        #define FSL_ENET_BUFF_ALIGNMENT MAX(ENET_BUFF_ALIGNMENT, FSL_FEATURE_L1DCACHE_LINESIZE_BYTE)
    #else
        #define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
    #endif
#else
    #define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#endif

#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
#define kENET_RxEvent kENET_RxIntEvent
#define kENET_TxEvent kENET_TxIntEvent
#endif


#define ENET_RING_NUM 1U
#define configMAC_ADDR {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}

typedef uint8_t rx_buffer_t[SDK_SIZEALIGN(ENET_RXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];
typedef uint8_t tx_buffer_t[SDK_SIZEALIGN(ENET_TXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 */
struct ethernetif
{
    ENET_Type *base;
#if (defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)) || \
    (USE_RTOS && defined(FSL_RTOS_AOS))
    enet_handle_t handle;
#endif
#if USE_RTOS && defined(FSL_RTOS_AOS)
    kevent_t *enetTransmitAccessEvent;
    unsigned int txFlag;
#endif
    enet_rx_bd_struct_t *RxBuffDescrip;
    enet_tx_bd_struct_t *TxBuffDescrip;
    rx_buffer_t *RxDataBuff;
    tx_buffer_t *TxDataBuff;
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
    uint8_t txIdx;
#if !(USE_RTOS && defined(FSL_RTOS_AOS))
    uint8_t rxIdx;
#endif
#endif
};

struct netif fsl_netif0;

/*******************************************************************************
 * Code
 ******************************************************************************/
#if USE_RTOS && defined(FSL_RTOS_AOS)
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
static void ethernet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event, void *param)
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static void ethernet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event, uint8_t channel, void *param)
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
{
    struct netif *netif = (struct netif *)param;
    struct ethernetif *ethernetif = netif->state;
    int ret;
    

    switch (event)
    {
        case kENET_RxEvent:
            ethernetif_input(netif);
            break;
        case kENET_TxEvent:
        {
            krhino_event_set(&ethernetif->enetTransmitAccessEvent, ethernetif->txFlag, 0x02);
        }
        break;
        default:
            break;
    }
}
#endif

#if LWIP_IPV4 && LWIP_IGMP
static err_t ethernetif_igmp_mac_filter(struct netif *netif, const ip4_addr_t *group, u8_t action)
{
    struct ethernetif *ethernetif = netif->state;
    uint8_t multicastMacAddr[6];
    err_t result;

    multicastMacAddr[0] = 0x01U;
    multicastMacAddr[1] = 0x00U;
    multicastMacAddr[2] = 0x5EU;
    multicastMacAddr[3] = (group->addr >> 8) & 0x7FU;
    multicastMacAddr[4] = (group->addr >> 16) & 0xFFU;
    multicastMacAddr[5] = (group->addr >> 24) & 0xFFU;

    switch (action)
    {
        case IGMP_ADD_MAC_FILTER:
            /* Adds the ENET device to a multicast group.*/
            ENET_AddMulticastGroup(ethernetif->base, multicastMacAddr);
            result = ERR_OK;
            break;
        case IGMP_DEL_MAC_FILTER:
/* Moves the ENET device from a multicast group.*/
#if 0
      ENET_LeaveMulticastGroup(ethernetif->base, multicastMacAddr);
#endif
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#endif

#if LWIP_IPV6 && LWIP_IPV6_MLD
static err_t ethernetif_mld_mac_filter(struct netif *netif, const ip6_addr_t *group, enum netif_mac_filter_action action)
{
    struct ethernetif *ethernetif = netif->state;
    uint8_t multicastMacAddr[6];
    err_t result;

    multicastMacAddr[0] = 0x33U;
    multicastMacAddr[1] = 0x33U;
    multicastMacAddr[2] = (group->addr[3]) & 0xFFU;
    multicastMacAddr[3] = (group->addr[3] >> 8) & 0xFFU;
    multicastMacAddr[4] = (group->addr[3] >> 16) & 0xFFU;
    multicastMacAddr[5] = (group->addr[3] >> 24) & 0xFFU;

    switch (action)
    {
        case NETIF_ADD_MAC_FILTER:
            /* Adds the ENET device to a multicast group.*/
            ENET_AddMulticastGroup(ethernetif->base, multicastMacAddr);
            result = ERR_OK;
            break;
        case NETIF_DEL_MAC_FILTER:
/* Moves the ENET device from a multicast group.*/
#if 0
      ENET_LeaveMulticastGroup(ethernetif->base, multicastMacAddr);
#endif
            result = ERR_OK;
            break;
        default:
            result = ERR_IF;
            break;
    }

    return result;
}
#endif

#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static inline enet_rx_bd_struct_t *get_rx_desc(struct ethernetif *ethernetif, uint32_t index)
{
    return &(ethernetif->RxBuffDescrip[index]);
}

static inline enet_tx_bd_struct_t *get_tx_desc(struct ethernetif *ethernetif, uint32_t index)
{
    return &(ethernetif->TxBuffDescrip[index]);
}
#endif

/**
 * Initializes ENET driver.
 */
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
static void enet_init(struct netif *netif, struct ethernetif *ethernetif,
                      const ethernetif_config_t *ethernetifConfig)
{
    enet_config_t config;
    uint32_t sysClock;
    status_t status;
    bool link = false;
    phy_speed_t speed;
    phy_duplex_t duplex;
    uint32_t count = 0;
    enet_buffer_config_t buffCfg[ENET_RING_NUM];

    /* prepare the buffer configuration. */
    buffCfg[0].rxBdNumber = ENET_RXBD_NUM;                      /* Receive buffer descriptor number. */
    buffCfg[0].txBdNumber = ENET_TXBD_NUM;                      /* Transmit buffer descriptor number. */
    buffCfg[0].rxBuffSizeAlign = sizeof(rx_buffer_t);           /* Aligned receive data buffer size. */
    buffCfg[0].txBuffSizeAlign = sizeof(tx_buffer_t);           /* Aligned transmit data buffer size. */
    buffCfg[0].rxBdStartAddrAlign = &(ethernetif->RxBuffDescrip[0]); /* Aligned receive buffer descriptor start address. */
    buffCfg[0].txBdStartAddrAlign = &(ethernetif->TxBuffDescrip[0]); /* Aligned transmit buffer descriptor start address. */
    buffCfg[0].rxBufferAlign = &(ethernetif->RxDataBuff[0][0]); /* Receive data buffer start address. */
    buffCfg[0].txBufferAlign = &(ethernetif->TxDataBuff[0][0]); /* Transmit data buffer start address. */

    sysClock = CLOCK_GetFreq(ethernetifConfig->clockName);

    ENET_GetDefaultConfig(&config);
    config.ringNum = ENET_RING_NUM;

    status = PHY_Init(ethernetif->base, ethernetifConfig->phyAddress, sysClock);
    if (kStatus_Success != status)
    {
        LWIP_ASSERT("\r\nCannot initialize PHY.\r\n", 0);
    }

    while ((count < ENET_ATONEGOTIATION_TIMEOUT) && (!link))
    {
        PHY_GetLinkStatus(ethernetif->base, ethernetifConfig->phyAddress, &link);

        if (link)
        {
            /* Get the actual PHY link speed. */
            PHY_GetLinkSpeedDuplex(ethernetif->base, ethernetifConfig->phyAddress, &speed, &duplex);
            /* Change the MII speed and duplex for actual link status. */
            config.miiSpeed = (enet_mii_speed_t)speed;
            config.miiDuplex = (enet_mii_duplex_t)duplex;
        }

        count++;
    }

#if 0 /* Disable assert. If initial auto-negation is timeout, \ \
        the ENET set to default 100Mbs and full-duplex.*/
    if (count == ENET_ATONEGOTIATION_TIMEOUT)
    {
        LWIP_ASSERT("\r\nPHY Link down, please check the cable connection.\r\n", 0);
    }
#endif

#if USE_RTOS && defined(FSL_RTOS_AOS)
    uint32_t instance;
    static ENET_Type *const enetBases[] = ENET_BASE_PTRS;
    static const IRQn_Type enetTxIrqId[] = ENET_Transmit_IRQS;
    /*! @brief Pointers to enet receive IRQ number for each instance. */
    static const IRQn_Type enetRxIrqId[] = ENET_Receive_IRQS;
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
    /*! @brief Pointers to enet timestamp IRQ number for each instance. */
    static const IRQn_Type enetTsIrqId[] = ENET_1588_Timer_IRQS;
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */

    /* Create the Event for transmit busy release trigger. */
    ethernetif->enetTransmitAccessEvent = xEventGroupCreate();
    ethernetif->txFlag = 0x1;

    config.interrupt |= kENET_RxFrameInterrupt | kENET_TxFrameInterrupt | kENET_TxBufferInterrupt;

    for (instance = 0; instance < ARRAY_SIZE(enetBases); instance++)
    {
        if (enetBases[instance] == ethernetif->base)
        {
#ifdef __CA7_REV
            GIC_SetPriority(enetRxIrqId[instance], ENET_PRIORITY);
            GIC_SetPriority(enetTxIrqId[instance], ENET_PRIORITY);
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            GIC_SetPriority(enetTsIrqId[instance], ENET_1588_PRIORITY);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
#else
            NVIC_SetPriority(enetRxIrqId[instance], ENET_PRIORITY);
            NVIC_SetPriority(enetTxIrqId[instance], ENET_PRIORITY);
#if defined(ENET_ENHANCEDBUFFERDESCRIPTOR_MODE) && ENET_ENHANCEDBUFFERDESCRIPTOR_MODE
            NVIC_SetPriority(enetTsIrqId[instance], ENET_1588_PRIORITY);
#endif /* ENET_ENHANCEDBUFFERDESCRIPTOR_MODE */
#endif /* __CA7_REV */
            break;
        }
    }

    LWIP_ASSERT("Input Ethernet base error!", (instance != ARRAY_SIZE(enetBases)));
#endif /* USE_RTOS */

    /* Initialize the ENET module.*/
    ENET_Init(ethernetif->base, &ethernetif->handle, &config, &buffCfg[0], netif->hwaddr, sysClock);

#if USE_RTOS && defined(FSL_RTOS_AOS)
    ENET_SetCallback(&ethernetif->handle, ethernet_callback, netif);
#endif

    ENET_ActiveRead(ethernetif->base);
}
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static void enet_init(struct netif *netif, struct ethernetif *ethernetif,
                      const ethernetif_config_t *ethernetifConfig)
{
    enet_config_t config;
    status_t status;
    uint32_t sysClock;
    bool link = false;
    phy_speed_t speed;
    phy_duplex_t duplex;
    uint32_t count = 0;
    enet_buffer_config_t buffCfg[ENET_RING_NUM];
    uint32_t rxBufferStartAddr[ENET_RXBD_NUM];
    uint32_t i;

    /* calculate start addresses of all rx buffers */
    for (i = 0; i < ENET_RXBD_NUM; i++)
    {
        rxBufferStartAddr[i] = (uint32_t)&(ethernetif->RxDataBuff[i][0]);
    }
    /* prepare the buffer configuration. */
    buffCfg[0].rxRingLen = ENET_RXBD_NUM;                          /* The length of receive buffer descriptor ring. */
    buffCfg[0].txRingLen = ENET_TXBD_NUM;                          /* The length of transmit buffer descriptor ring. */
    buffCfg[0].txDescStartAddrAlign = get_tx_desc(ethernetif, 0U); /* Aligned transmit descriptor start address. */
    buffCfg[0].txDescTailAddrAlign = get_tx_desc(ethernetif, 0U);  /* Aligned transmit descriptor tail address. */
    buffCfg[0].rxDescStartAddrAlign = get_rx_desc(ethernetif, 0U); /* Aligned receive descriptor start address. */
    buffCfg[0].rxDescTailAddrAlign = get_rx_desc(ethernetif, ENET_RXBD_NUM); /* Aligned receive descriptor tail address. */
    buffCfg[0].rxBufferStartAddr = rxBufferStartAddr;              /* Start addresses of the rx buffers. */
    buffCfg[0].rxBuffSizeAlign = sizeof(rx_buffer_t);              /* Aligned receive data buffer size. */

    sysClock = CLOCK_GetFreq(ethernetifConfig->clockName);

    ENET_GetDefaultConfig(&config);
    config.multiqueueCfg = NULL;
    status = PHY_Init(ethernetif->base, ethernetifConfig->phyAddress, sysClock);
    if (kStatus_Success != status)
    {
        LWIP_ASSERT("\r\nCannot initialize PHY.\r\n", 0);
    }
	
    while ((count < ENET_ATONEGOTIATION_TIMEOUT) && (!link))
    {
        PHY_GetLinkStatus(ethernetif->base, ethernetifConfig->phyAddress, &link);
        if (link)
        {
            /* Get the actual PHY link speed. */
            PHY_GetLinkSpeedDuplex(ethernetif->base, ethernetifConfig->phyAddress, &speed, &duplex);
            /* Change the MII speed and duplex for actual link status. */
            config.miiSpeed = (enet_mii_speed_t)speed;
            config.miiDuplex = (enet_mii_duplex_t)duplex;
        }

        count++;
    }
	
#if 0 /* Disable assert. If initial auto-negation is timeout, \ \
        the ENET set to default 100Mbs and full-duplex.*/
    if (count == ENET_ATONEGOTIATION_TIMEOUT)
    {
        LWIP_ASSERT("\r\nPHY Link down, please check the cable connection.\r\n", 0);
    }
#endif

#if USE_RTOS && defined(FSL_RTOS_AOS)
    /* Create the Event for transmit busy release trigger. */
    krhino_event_dyn_create(&ethernetif->enetTransmitAccessEvent, "AOS", 0);
    ethernetif->txFlag = 0x1;
    NVIC_SetPriority(ETHERNET_IRQn, ENET_PRIORITY);
#else
    ethernetif->rxIdx = 0U;
#endif /* USE_RTOS */

    ethernetif->txIdx = 0U;

    ENET_Init(ethernetif->base, &config, netif->hwaddr, sysClock);

/* Create the handler. */
#if USE_RTOS && defined(FSL_RTOS_AOS)
    ENET_EnableInterrupts(ethernetif->base, kENET_DmaTx | kENET_DmaRx);
    ENET_CreateHandler(ethernetif->base, &ethernetif->handle, &config, &buffCfg[0], ethernet_callback, netif);
#endif

    ENET_DescriptorInit(ethernetif->base, &config, &buffCfg[0]);

    /* Active TX/RX. */
    ENET_StartRxTx(ethernetif->base, 1, 1);
}
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif, const uint8_t enetIdx,
                           const ethernetif_config_t *ethernetifConfig)
{
    struct ethernetif *ethernetif = netif->state;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, ethernetifConfig->macAddress, NETIF_MAX_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500; /* TODO: define a config */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* ENET driver initialization.*/
    enet_init(netif, ethernetif, ethernetifConfig);
#if LWIP_IPV6 && LWIP_IPV6_MLD
    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL)
    {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */
}

/**
 * Returns next buffer for TX.
 * Can wait if no buffer available.
 */
static unsigned char *enet_get_tx_buffer(struct ethernetif *ethernetif)
{
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
    {
        static unsigned char ucBuffer[ENET_FRAME_MAX_FRAMELEN];
        return ucBuffer;
    }
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
    {
        enet_tx_bd_struct_t *txBuffDesc = get_tx_desc(ethernetif, ethernetif->txIdx);
#if USE_RTOS && defined(FSL_RTOS_AOS)
        while (1)
        {
            if (ENET_IsTxDescriptorDmaOwn(txBuffDesc))
            {
                unsigned int actl_flags;   
                krhino_event_get(&ethernetif->enetTransmitAccessEvent, ethernetif->txFlag, 0x02, &actl_flags, RHINO_WAIT_FOREVER);
            }
            else
            {
                break;
            }
        }
#else
        {
            uint32_t counter;

            for (counter = ENET_TIMEOUT; counter != 0U; counter--)
            {
                if (!ENET_IsTxDescriptorDmaOwn(txBuffDesc))
                {
                    break;
                }
            }

            if (counter == 0U)
            {
                return (unsigned char *)NULL;
            }
        }
#endif
        return (unsigned char *)&(ethernetif->TxDataBuff[ethernetif->txIdx][0]);
    }
#endif
}

/**
 * Sends frame via ENET.
 */
static err_t enet_send_frame(struct ethernetif *ethernetif, unsigned char *data, const uint32_t length)
{
#if USE_RTOS && defined(FSL_RTOS_AOS)
    {
        status_t result;

        do
        {
            result = ENET_SendFrame(ethernetif->base, &ethernetif->handle, data, length);

            if (result == kStatus_ENET_TxFrameBusy)
            {
                unsigned int actl_flags; 
                krhino_event_get(&ethernetif->enetTransmitAccessEvent, ethernetif->txFlag, 0x02, &actl_flags, RHINO_WAIT_FOREVER);
            }

        } while (result == kStatus_ENET_TxFrameBusy);
        return ERR_OK;
    }
#elif defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
    {
        uint32_t counter;

        for (counter = ENET_TIMEOUT; counter != 0U; counter--)
        {
            if (ENET_SendFrame(ethernetif->base, &ethernetif->handle, data, length) != kStatus_ENET_TxFrameBusy)
            {
                return ERR_OK;
            }
        }

        return ERR_TIMEOUT;
    }
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
    {
        uint32_t tail;
        enet_tx_bd_struct_t *txBuffDesc = get_tx_desc(ethernetif, ethernetif->txIdx);

        ENET_SetupTxDescriptor(txBuffDesc, data, length, NULL, 0U, length, false, false, kENET_FirstLastFlag, 0U);
        ethernetif->txIdx = (ethernetif->txIdx + 1U) % ENET_TXBD_NUM;

        /* Update the transmit tail address. */
        if (ethernetif->txIdx == 0U)
        {
            tail = (uint32_t)get_tx_desc(ethernetif, ENET_TXBD_NUM);
        }
        else
        {
            tail = (uint32_t)get_tx_desc(ethernetif, ethernetif->txIdx);
        }
        ENET_UpdateTxDescriptorTail(ethernetif->base, 0U, tail);

        return ERR_OK;
    }
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

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t result;
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *q;
    unsigned char *pucBuffer;
    unsigned char *pucChar;

    LWIP_ASSERT("Output packet buffer empty", p);

    pucBuffer = enet_get_tx_buffer(ethernetif);
    if (pucBuffer == NULL)
    {
        return ERR_BUF;
    }

/* Initiate transfer. */

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    if (p->len == p->tot_len)
    {
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
        /* No pbuf chain, don't have to copy -> faster. */
        pucBuffer = (unsigned char *)p->payload;
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
        /* No pbuf chain, still have to copy as pbuf could be reclaimed early. */
        memcpy(pucBuffer, p->payload, p->len);
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
    }
    else
    {
        /* pbuf chain, copy into contiguous ucBuffer. */
        if (p->tot_len >= ENET_FRAME_MAX_FRAMELEN)
        {
            return ERR_BUF;
        }
        else
        {
            pucChar = pucBuffer;

            for (q = p; q != NULL; q = q->next)
            {
                /* Send the data from the pbuf to the interface, one pbuf at a
                time. The size of the data in each pbuf is kept in the ->len
                variable. */
                /* send data from(q->payload, q->len); */
                memcpy(pucChar, q->payload, q->len);
                pucChar += q->len;
            }
        }
    }

    /* Send frame. */
    result = enet_send_frame(ethernetif, pucBuffer, p->tot_len);

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
    if (((u8_t *)p->payload)[0] & 1)
    {
        /* broadcast or multicast packet*/
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    }
    else
    {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
/* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return result;
}

/**
 * Gets the length of received frame (if any).
 */
static status_t enet_get_rx_frame_size(struct ethernetif *ethernetif, uint32_t *length)
{
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
    {
        return ENET_GetRxFrameSize(&ethernetif->handle, length);
    }
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
#if USE_RTOS && defined(FSL_RTOS_AOS)
    {
        return ENET_GetRxFrameSize(ethernetif->base, &ethernetif->handle, length, 0U);
    }
#else
    {
        uint8_t index = ethernetif->rxIdx;
        enet_rx_bd_struct_t *rxDesc = get_rx_desc(ethernetif, index);
        uint32_t rxControl = ENET_GetRxDescriptor(rxDesc);

        /* Reset the length to zero. */
        *length = 0;

        if (rxControl & ENET_RXDESCRIP_WR_OWN_MASK)
        {
            return kStatus_ENET_RxFrameEmpty;
        }
        else
        {
            do
            {
                /* Application owns the buffer descriptor, get the length. */
                if (rxControl & ENET_RXDESCRIP_WR_LD_MASK)
                {
                    if (rxControl & ENET_RXDESCRIP_WR_ERRSUM_MASK)
                    {
                        return kStatus_ENET_RxFrameError;
                    }
                    else
                    {
                        *length = rxControl & ENET_RXDESCRIP_WR_PACKETLEN_MASK;
                        return kStatus_Success;
                    }
                }

                index = (index + 1U) % ENET_RXBD_NUM;
                rxDesc = get_rx_desc(ethernetif, index);
                rxControl = ENET_GetRxDescriptor(rxDesc);
            } while (index != ethernetif->rxIdx);

            return kStatus_ENET_RxFrameError;
        }
    }
#endif
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
}

/**
 * Reads frame from ENET.
 */
static void enet_read_frame(struct ethernetif *ethernetif, uint8_t *data, uint32_t length)
{
#if defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0)
    {
        ENET_ReadFrame(ethernetif->base, &ethernetif->handle, data, length);
    }
#elif defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
#if USE_RTOS && defined(FSL_RTOS_AOS)
    {
        ENET_ReadFrame(ethernetif->base, &ethernetif->handle, data, length, 0U);
    }
#else
    {
        enet_rx_bd_struct_t *rxDesc;
        uint8_t index = ethernetif->rxIdx;
        uint32_t rxControl;
        bool isLastBuff = false;
        uint32_t len = 0;
        uint32_t offset = 0;

        if (!data)
        {
            do
            {
                rxDesc = get_rx_desc(ethernetif, ethernetif->rxIdx);
                ethernetif->rxIdx = (ethernetif->rxIdx + 1U) % ENET_RXBD_NUM;
                rxControl = ENET_GetRxDescriptor(rxDesc);

                /* Update the receive buffer descriptor. */
                ENET_UpdateRxDescriptor(rxDesc, NULL, NULL, false, false);

                /* Find the last buffer descriptor for the frame. */
                if (rxControl & ENET_RXDESCRIP_WR_LD_MASK)
                {
                    break;
                }
            } while (ethernetif->rxIdx != index);
        }
        else
        {
            while (!isLastBuff)
            {
                rxDesc = get_rx_desc(ethernetif, ethernetif->rxIdx);
                ethernetif->rxIdx = (ethernetif->rxIdx + 1U) % ENET_RXBD_NUM;
                rxControl = ENET_GetRxDescriptor(rxDesc);

                if (rxControl & ENET_RXDESCRIP_WR_LD_MASK)
                {
                    /* This is a valid frame. */
                    isLastBuff = true;
                    if (length == (rxControl & ENET_RXDESCRIP_WR_PACKETLEN_MASK))
                    {
                        /* Copy the frame to user's buffer. */
                        len = (rxControl & ENET_RXDESCRIP_WR_PACKETLEN_MASK) - offset;
                        if (len > sizeof(rx_buffer_t))
                        {
                            memcpy(data + offset, (void *)rxDesc->buff1Addr, sizeof(rx_buffer_t));
                            offset += sizeof(rx_buffer_t);
                            memcpy(data + offset, (void *)rxDesc->buff2Addr, len - sizeof(rx_buffer_t));
                        }
                        else
                        {
                            memcpy(data + offset, (void *)rxDesc->buff1Addr, len);
                        }
                    }

                    /* Update the receive buffer descriptor. */
                    ENET_UpdateRxDescriptor(rxDesc, NULL, NULL, false, false);
                }
                else
                {
                    /* Store a frame on several buffer descriptors. */
                    isLastBuff = false;
                    /* Length check. */
                    if (offset >= length)
                    {
                        /* Updates the receive buffer descriptors. */
                        ENET_UpdateRxDescriptor(rxDesc, NULL, NULL, false, false);
                        break;
                    }

                    memcpy(data + offset, (void *)rxDesc->buff1Addr, sizeof(rx_buffer_t));
                    offset += sizeof(rx_buffer_t);

                    /* Update the receive buffer descriptor. */
                    ENET_UpdateRxDescriptor(rxDesc, NULL, NULL, false, false);
                }
            }
        }

        ENET_UpdateRxDescriptorTail(ethernetif->base, 0U, (uint32_t)get_rx_desc(ethernetif, ENET_RXBD_NUM));
    }
#endif /* USE_RTOS */
#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */
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
    struct ethernetif *ethernetif = netif->state;
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;
    status_t status;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    status = enet_get_rx_frame_size(ethernetif, &len);

    if (kStatus_ENET_RxFrameEmpty != status)
    {
        /* Call enet_read_frame when there is a received frame. */
        if (len != 0)
        {
#if ETH_PAD_SIZE
            len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

            /* We allocate a pbuf chain of pbufs from the pool. */
            p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

            if (p != NULL)
            {
#if ETH_PAD_SIZE
                pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
                if (p->next == 0) /* One-chain buffer.*/
                {
                    enet_read_frame(ethernetif, p->payload, p->len);
                }
                else    /* Multi-chain buffer.*/
                {
                    uint8_t data_tmp[ENET_FRAME_MAX_FRAMELEN];
                    uint32_t data_tmp_len = 0;

                    enet_read_frame(ethernetif, data_tmp, p->tot_len);

                    /* We iterate over the pbuf chain until we have read the entire
                    * packet into the pbuf. */
                    for (q = p; (q != NULL) && ((data_tmp_len + q->len) <= sizeof(data_tmp)); q = q->next)
                    {
                        /* Read enough bytes to fill this pbuf in the chain. The
                        * available data in the pbuf is given by the q->len
                        * variable. */
                        memcpy(q->payload,  &data_tmp[data_tmp_len], q->len);
                        data_tmp_len += q->len;
                    }
                }

                MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
                if (((u8_t *)p->payload)[0] & 1)
                {
                    /* broadcast or multicast packet*/
                    MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
                }
                else
                {
                    /* unicast packet*/
                    MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
                }
#if ETH_PAD_SIZE
                pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

                LINK_STATS_INC(link.recv);
            }
            else
            {
                /* drop packet*/
                enet_read_frame(ethernetif, NULL, 0U);

                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: Fail to allocate new memory space\n"));

                LINK_STATS_INC(link.memerr);
                LINK_STATS_INC(link.drop);
                MIB2_STATS_NETIF_INC(netif, ifindiscards);
            }
        }
        else
        {
            /* Update the received buffer when error happened. */
            if (status == kStatus_ENET_RxFrameError)
            {
#if 0 && defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 0) /* Error statisctics */
        enet_data_error_stats_t eErrStatic;
        /* Get the error information of the received g_frame. */
        ENET_GetRxErrBeforeReadFrame(&ethernetif->handle, &eErrStatic);
#endif

                /* Update the receive buffer. */
                enet_read_frame(ethernetif, NULL, 0U);

                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: RxFrameError\n"));

                LINK_STATS_INC(link.drop);
                MIB2_STATS_NETIF_INC(netif, ifindiscards);
            }
        }
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
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /* move received packet into a new pbuf */
    while ((p = low_level_input(netif)) != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

static ENET_Type *get_enet_base(const uint8_t enetIdx)
{
    ENET_Type* enets[] = ENET_BASE_PTRS;
    int arrayIdx;
    int enetCount;

    for (arrayIdx = 0, enetCount = 0; arrayIdx < ARRAY_SIZE(enets); arrayIdx++)
    {
        if (enets[arrayIdx] != 0U)    /* process only defined positions */
        {                             /* (some SOC headers count ENETs from 1 instead of 0) */
            if (enetCount == enetIdx)
            {
                return enets[arrayIdx];
            }
            enetCount++;
        }
    }

    return NULL;
}

static err_t ethernetif_init(struct netif *netif, struct ethernetif *ethernetif,
                             const uint8_t enetIdx,
                             const ethernetif_config_t *ethernetifConfig)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_ASSERT("ethernetifConfig != NULL", (ethernetifConfig != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
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

#if LWIP_IPV4 && LWIP_IGMP
    netif_set_igmp_mac_filter(netif, ethernetif_igmp_mac_filter);
    netif->flags |= NETIF_FLAG_IGMP;
#endif
#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif_set_mld_mac_filter(netif, ethernetif_mld_mac_filter);
    netif->flags |= NETIF_FLAG_MLD6;
#endif

    /* Init ethernetif parameters.*/
    ethernetif->base = get_enet_base(enetIdx);
    LWIP_ASSERT("ethernetif->base != NULL", (ethernetif->base != NULL));

    /* initialize the hardware */
    low_level_init(netif, enetIdx, ethernetifConfig);

    return ERR_OK;
}

/**
 * Should be called at the beginning of the program to set up the
 * first network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif0_init(struct netif *netif)
{
    static struct ethernetif ethernetif_0;
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t rxBuffDescrip_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t txBuffDescrip_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static rx_buffer_t rxDataBuff_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static tx_buffer_t txDataBuff_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);

    ethernetif_0.RxBuffDescrip = &(rxBuffDescrip_0[0]);
    ethernetif_0.TxBuffDescrip = &(txBuffDescrip_0[0]);
    ethernetif_0.RxDataBuff = &(rxDataBuff_0[0]);
    ethernetif_0.TxDataBuff = &(txDataBuff_0[0]);
    return ethernetif_init(netif, &ethernetif_0, 0U, (ethernetif_config_t *)netif->state);
}


#if (defined(FSL_FEATURE_SOC_ENET_COUNT) && (FSL_FEATURE_SOC_ENET_COUNT > 1)) \
 || (defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 1))
/**
 * Should be called at the beginning of the program to set up the
 * second network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif1_init(struct netif *netif)
{
    static struct ethernetif ethernetif_1;
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t rxBuffDescrip_1[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t txBuffDescrip_1[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static rx_buffer_t rxDataBuff_1[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
    SDK_ALIGN(static tx_buffer_t txDataBuff_1[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);

    ethernetif_1.RxBuffDescrip = &(rxBuffDescrip_1[0]);
    ethernetif_1.TxBuffDescrip = &(txBuffDescrip_1[0]);
    ethernetif_1.RxDataBuff = &(rxDataBuff_1[0]);
    ethernetif_1.TxDataBuff = &(txDataBuff_1[0]);

    return ethernetif_init(netif, &ethernetif_1, 1U, (ethernetif_config_t *)netif->state);
}

#endif /* FSL_FEATURE_SOC_*_ENET_COUNT */

static void tcpip_dhcpc_cb(struct netif *pstnetif) {
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(pstnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    if (dhcp->state == DHCP_STATE_BOUND)
    {		
#if LWIP_IPV4 && LWIP_IPV6
        printf("\r\n IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&pstnetif->ip_addr.u_addr.ip4)[0],
                ((u8_t *)&pstnetif->ip_addr.u_addr.ip4)[1], ((u8_t *)&pstnetif->ip_addr.u_addr.ip4)[2],
                ((u8_t *)&pstnetif->ip_addr.u_addr.ip4)[3]);
        printf(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&pstnetif->netmask.u_addr.ip4)[0],
                ((u8_t *)&pstnetif->netmask.u_addr.ip4)[1], ((u8_t *)&pstnetif->netmask.u_addr.ip4)[2],
                ((u8_t *)&pstnetif->netmask.u_addr.ip4)[3]);
        printf(" IPv4 Gateway     : %u.%u.%u.%u\r\n\r\n", ((u8_t *)&pstnetif->gw.u_addr.ip4)[0],
                ((u8_t *)&pstnetif->gw.u_addr.ip4)[1], ((u8_t *)&pstnetif->gw.u_addr.ip4)[2],
                ((u8_t *)&pstnetif->gw.u_addr.ip4)[3]);
#else
#if LWIP_IPV4
        printf("\r\n IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&pstnetif->ip_addr.addr)[0],
               ((u8_t *)&pstnetif->ip_addr.addr)[1], ((u8_t *)&pstnetif->ip_addr.addr)[2],
               ((u8_t *)&pstnetif->ip_addr.addr)[3]);
        printf(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&pstnetif->netmask.addr)[0],
               ((u8_t *)&pstnetif->netmask.addr)[1], ((u8_t *)&pstnetif->netmask.addr)[2],
               ((u8_t *)&pstnetif->netmask.addr)[3]);
        printf(" IPv4 Gateway     : %u.%u.%u.%u\r\n\r\n", ((u8_t *)&pstnetif->gw.addr)[0],
               ((u8_t *)&pstnetif->gw.addr)[1], ((u8_t *)&pstnetif->gw.addr)[2], 
               ((u8_t *)&pstnetif->gw.addr)[3]);
#endif //LWIP_IPV4
#endif //LWIP_IPV4 && LWIP_IPV6              
        aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0xdeaddead);
    }
    return;
}



err_t tcpip_dhcpc_start(struct netif *pstnetif)
{
    if (NULL == pstnetif){
        LOG("%s input netif is NULL \r\n");
        return -1;
    }

    if (netif_is_up(pstnetif)) {
        if (dhcp_start(pstnetif) != ERR_OK) {
            LOG("dhcp client start failed");
            return -1;
        }
    }

    netif_set_status_callback(pstnetif, tcpip_dhcpc_cb);
}

static void tcpip_init_done(void *arg) {
    ethernetif_config_t fsl_enet_config0 = {
        .phyAddress = 0x00, .clockName = kCLOCK_CoreSysClk, .macAddress = configMAC_ADDR,
    };
#if LWIP_IPV4
    ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    IP4_ADDR(&fsl_netif0_ipaddr, 0U, 0U, 0U, 0U);
    IP4_ADDR(&fsl_netif0_netmask, 0U, 0U, 0U, 0U);
    IP4_ADDR(&fsl_netif0_gw, 0U, 0U, 0U, 0U);
    netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, &fsl_enet_config0, ethernetif0_init,
              ethernet_input);
#endif

#if LWIP_IPV6
#if !LWIP_IPV4
    netif_add(&fsl_netif0, &fsl_enet_config0, ethernetif_init, tcpip_input);
#endif
    netif_create_ip6_linklocal_address(&fsl_netif0, 1);
    fsl_netif0.ip6_autoconfig_enabled = 1;
#endif
    netif_set_default(&fsl_netif0);
    netif_set_up(&fsl_netif0);
	tcpip_dhcpc_start(&fsl_netif0);
}
  
int lwip_tcpip_init(void)
{
    tcpip_init(NULL, NULL);
    tcpip_init_done(NULL);
    LOG("TCP/IP initialized.");

    return 0;
} 