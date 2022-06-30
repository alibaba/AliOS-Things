/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-08-24     chinesebear  first version
 */

#include <string.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <aos/debug.h>
#include <drivers/u_ld.h>
#include <lwip/err.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/ip_addr.h>
#include <lwip/dns.h>
#include <lwip/netifapi.h>
#include <netif/etharp.h>
#include <ls1c_irq.h>
#include <ls1c_pin.h>
#include "synopGMAC.h"
#include "synopGMAC_debug.h"
#include "mii.c"

#define RMII

#define Gmac_base           0xbfe10000
#define Buffer_Size         2048
#define MAX_ADDR_LEN        6
#define NAMESIZE            16
#define BUS_SIZE_ALIGN(x)   ((x+15)&~15)
#define DEFAULT_MAC_ADDRESS {0x00, 0x55, 0x7B, 0xB5, 0x7D, 0xF7}

u32 regbase = 0xbfe10000;
static u32 GMAC_Power_down;
extern void *plat_alloc_consistent_dmaable_memory(synopGMACdevice *pcidev, u32 size, u32 *addr) ;
static s32 synopGMAC_check_phy_init(synopGMACPciNetworkAdapter *adapter) ;
static int init_phy(synopGMACdevice *gmacdev);
extern dma_addr_t plat_dma_map_single(void *hwdev, void *ptr, u32 size);

static void eth_rx_irq(int irqno, void *param);
static err_t low_level_output(struct netif *netif, struct pbuf *p);
static void rx_task_func(void *arg);
// static char Rx_Buffer[Buffer_Size];
// static char Tx_Buffer[Buffer_Size];

struct eth_dev
{
    struct netif netif;
    struct synopGMACNetworkAdapter gmac_adapter;
    synopGMACdevice gmac_dev;
    aos_timer_t link_timer;
    aos_task_t rx_task;
    aos_sem_t sem_ack;
    aos_sem_t sem_lock;
};

static struct eth_dev eth_dev;

/**
 * This sets up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures for ring mode or chain mode.
 * This function depends on the pcidev structure for allocation consistent dma-able memory in case
 * of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 * - Allocates the memory for the descriptors.
 * - Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 * - Initialize the Busy and Next descriptors to first descriptor address.
 * - Initialize the last descriptor with the endof ring in case of ring mode.
 * - Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode,
 * but in chain mode
 * function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from
 * this function
 * user should for gmacdev->TxDescCount to see how many descriptors are there in the chain. Should
 * continue further
 * only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_tx_desc_queue(synopGMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode)
{
    s32 i;
    DmaDesc *bf1;

    DmaDesc *first_desc = NULL;

    dma_addr_t dma_addr;
    gmacdev->TxDescCount = 0;

    first_desc = (DmaDesc *)plat_alloc_consistent_dmaable_memory(gmacdev, sizeof(DmaDesc) * no_of_desc, &dma_addr);
    if (first_desc == NULL)
    {
        aos_debug_printf("Error in Tx Descriptors memory allocation\n");
        return -ESYNOPGMACNOMEM;
    }

    DEBUG_MES("tx_first_desc_addr = %p\n", first_desc);
    DEBUG_MES("dmaadr = %p\n", dma_addr);
    gmacdev->TxDescCount = no_of_desc;
    gmacdev->TxDesc      = first_desc;
    gmacdev->TxDescDma  = dma_addr;

    for (i = 0; i < gmacdev->TxDescCount; i++)
    {
        synopGMAC_tx_desc_init_ring(gmacdev->TxDesc + i, i == gmacdev->TxDescCount - 1);
#if SYNOP_TOP_DEBUG
        aos_debug_printf("\n%02d %08x \n", i, (unsigned int)(gmacdev->TxDesc + i));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i))->status);
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->length));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->buffer1));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->buffer2));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->data1));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->data2));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->dummy1));
        aos_debug_printf("%08x ", (unsigned int)((gmacdev->TxDesc + i)->dummy2));
#endif
    }

    gmacdev->TxNext = 0;
    gmacdev->TxBusy = 0;
    gmacdev->TxNextDesc = gmacdev->TxDesc;
    gmacdev->TxBusyDesc = gmacdev->TxDesc;
    gmacdev->BusyTxDesc  = 0;

    return -ESYNOPGMACNOERR;
}

/**
 * This sets up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures in ring mode or chain mode.
 * This function depends on the pcidev structure for allocation of consistent dma-able memory in
 * case of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 * - Allocates the memory for the descriptors.
 * - Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 * - Initialize the Busy and Next descriptors to first descriptor address.
 * - Initialize the last descriptor with the endof ring in case of ring mode.
 * - Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to pci_device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode,
 * but in chain mode
 * function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from
 * this function
 * user should for gmacdev->RxDescCount to see how many descriptors are there in the chain. Should
 * continue further
 * only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_rx_desc_queue(synopGMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode)
{
    s32 i;
    DmaDesc *bf1;
    DmaDesc *first_desc = NULL;

    dma_addr_t dma_addr;

    gmacdev->RxDescCount = 0;
    first_desc = (DmaDesc *)plat_alloc_consistent_dmaable_memory(gmacdev, sizeof(DmaDesc) * no_of_desc, &dma_addr);
    if (first_desc == NULL)
    {
        aos_debug_printf("Error in Rx Descriptor Memory allocation in Ring mode\n");
        return -ESYNOPGMACNOMEM;
    }

    DEBUG_MES("rx_first_desc_addr = %p\n", first_desc);
    DEBUG_MES("dmaadr = %p\n", dma_addr);
    gmacdev->RxDescCount = no_of_desc;
    gmacdev->RxDesc      = (DmaDesc *)first_desc;
    gmacdev->RxDescDma   = dma_addr;

    for (i = 0; i < gmacdev->RxDescCount; i++)
    {
        synopGMAC_rx_desc_init_ring(gmacdev->RxDesc + i, i == gmacdev->RxDescCount - 1);

    }

    gmacdev->RxNext = 0;
    gmacdev->RxBusy = 0;
    gmacdev->RxNextDesc = gmacdev->RxDesc;
    gmacdev->RxBusyDesc = gmacdev->RxDesc;

    gmacdev->BusyRxDesc   = 0;

    return -ESYNOPGMACNOERR;
}

static void synopGMAC_linux_cable_unplug_function(void *timer, void *adaptr)
{
    s32 data;
    synopGMACPciNetworkAdapter *adapter = (synopGMACPciNetworkAdapter *)adaptr;
    synopGMACdevice            *gmacdev = adapter->synopGMACdev;
    struct eth_dev *dev = aos_container_of(adapter, struct eth_dev, gmac_adapter);
    struct ethtool_cmd cmd;

    //aos_debug_printf("%s\n",__func__);
    if (!mii_link_ok(&adapter->mii))
    {
        if (gmacdev->LinkState)
            aos_debug_printf("\r\nNo Link\r\n");
        gmacdev->DuplexMode = 0;
        gmacdev->Speed = 0;
        gmacdev->LoopBackMode = 0;
        gmacdev->LinkState = 0;
        netifapi_netif_set_link_down(&dev->netif);
    }
    else
    {
        data = synopGMAC_check_phy_init(adapter);

        if (gmacdev->LinkState != data)
        {
            gmacdev->LinkState = data;
            synopGMAC_mac_init(gmacdev);
            aos_debug_printf("Link is up in %s mode\n", (gmacdev->DuplexMode == FULLDUPLEX) ? "FULL DUPLEX" : "HALF DUPLEX");
            if (gmacdev->Speed == SPEED1000)
                aos_debug_printf("Link is with 1000M Speed \r\n");
            if (gmacdev->Speed == SPEED100)
                aos_debug_printf("Link is with 100M Speed \n");
            if (gmacdev->Speed == SPEED10)
                aos_debug_printf("Link is with 10M Speed \n");

            netifapi_netif_set_link_up(&dev->netif);
        }
    }
}

static s32 synopGMAC_check_phy_init(synopGMACPciNetworkAdapter *adapter)
{
    struct ethtool_cmd cmd;
    synopGMACdevice            *gmacdev = adapter->synopGMACdev;

    if (!mii_link_ok(&adapter->mii))
    {
        gmacdev->DuplexMode = FULLDUPLEX;
        gmacdev->Speed      =   SPEED100;

        return 0;
    }
    else
    {
        mii_ethtool_gset(&adapter->mii, &cmd);

        gmacdev->DuplexMode = (cmd.duplex == DUPLEX_FULL)  ? FULLDUPLEX : HALFDUPLEX ;
        if (cmd.speed == SPEED_1000)
            gmacdev->Speed      =   SPEED1000;
        else if (cmd.speed == SPEED_100)
            gmacdev->Speed      =   SPEED100;
        else
            gmacdev->Speed      =   SPEED10;
    }

    return gmacdev->Speed | (gmacdev->DuplexMode << 4);
}


static int Mac_change_check(u8 *macaddr0, u8 *macaddr1)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if (macaddr0[i] != macaddr1[i])
            return 1;
    }
    return 0;
}

static err_t ethernetif_init(struct netif *netif)
{
    struct eth_dev *dev = aos_container_of(netif, struct eth_dev, netif);
    s32 status = 0;
    u64 dma_addr;
    u32 Mac_changed = 0;
    struct pbuf *pbuf;
    u8 macaddr[6] = DEFAULT_MAC_ADDRESS;
    struct synopGMACNetworkAdapter *adapter = &dev->gmac_adapter;
    synopGMACdevice *gmacdev = (synopGMACdevice *)adapter->synopGMACdev;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "haas_aos";
#endif /* LWIP_NETIF_HOSTNAME */
    netif->name[0] = 'e';
    netif->name[1] = 't';
    netif->num = 0;
    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;
    /* set netif MAC hardware address */
    netif->hwaddr[0] = macaddr[0];
    netif->hwaddr[1] = macaddr[1];
    netif->hwaddr[2] = macaddr[2];
    netif->hwaddr[3] = macaddr[3];
    netif->hwaddr[4] = macaddr[4];
    netif->hwaddr[5] = macaddr[5];
    /* set netif maximum transfer unit */
    netif->mtu = 1500;
    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    synopGMAC_reset(gmacdev);
    synopGMAC_attach(gmacdev, (regbase + MACBASE), (regbase + DMABASE), DEFAULT_PHY_BASE, macaddr);

    synopGMAC_read_version(gmacdev);

    synopGMAC_set_mdc_clk_div(gmacdev, GmiiCsrClk3);
    gmacdev->ClockDivMdc = synopGMAC_get_mdc_clk_div(gmacdev);

    init_phy(adapter->synopGMACdev);

    DEBUG_MES("tx desc_queue\n");
    synopGMAC_setup_tx_desc_queue(gmacdev, TRANSMIT_DESC_SIZE, RINGMODE);
    synopGMAC_init_tx_desc_base(gmacdev);

    DEBUG_MES("rx desc_queue\n");
    synopGMAC_setup_rx_desc_queue(gmacdev, RECEIVE_DESC_SIZE, RINGMODE);
    synopGMAC_init_rx_desc_base(gmacdev);
    DEBUG_MES("DmaRxBaseAddr = %08x\n", synopGMACReadReg(gmacdev->DmaBase, DmaRxBaseAddr));

//  u32 dmaRx_Base_addr = synopGMACReadReg(gmacdev->DmaBase,DmaRxBaseAddr);
//  aos_debug_printf("first_desc_addr = 0x%x\n", dmaRx_Base_addr);

#ifdef ENH_DESC_8W
    synopGMAC_dma_bus_mode_init(gmacdev, DmaBurstLength32 | DmaDescriptorSkip2 | DmaDescriptor8Words);
#else
    //synopGMAC_dma_bus_mode_init(gmacdev, DmaBurstLength4 | DmaDescriptorSkip1);
    synopGMAC_dma_bus_mode_init(gmacdev, DmaBurstLength4 | DmaDescriptorSkip2);
#endif
    synopGMAC_dma_control_init(gmacdev, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);

    status = synopGMAC_check_phy_init(adapter);
    synopGMAC_mac_init(gmacdev);

    synopGMAC_pause_control(gmacdev);

#ifdef IPC_OFFLOAD
    synopGMAC_enable_rx_chksum_offload(gmacdev);
    synopGMAC_rx_tcpip_chksum_drop_enable(gmacdev);
#endif

    u32 skb;
    do {
        skb = (u32)plat_alloc_memory(RX_BUF_SIZE);      //should skb aligned here?
        if (skb == 0)
        {
            aos_debug_printf("ERROR in skb buffer allocation\n");
            break;
        }

        dma_addr = plat_dma_map_single(gmacdev, (void *)skb, RX_BUF_SIZE);  //获取 skb 的 dma 地址

        status = synopGMAC_set_rx_qptr(gmacdev, dma_addr, RX_BUF_SIZE, (u32)skb, 0, 0, 0);
        if (status < 0)
        {
            aos_debug_printf("status < 0!!\n");
            plat_free_memory((void *)skb);
        }
    } while (status >= 0 && (status < (RECEIVE_DESC_SIZE - 1)));

    synopGMAC_clear_interrupt(gmacdev);

    synopGMAC_disable_mmc_tx_interrupt(gmacdev, 0xFFFFFFFF);
    synopGMAC_disable_mmc_rx_interrupt(gmacdev, 0xFFFFFFFF);
    synopGMAC_disable_mmc_ipc_rx_interrupt(gmacdev, 0xFFFFFFFF);

//  synopGMAC_disable_interrupt_all(gmacdev);
    synopGMAC_enable_interrupt(gmacdev, DmaIntEnable);
    synopGMAC_enable_dma_rx(gmacdev);
    synopGMAC_enable_dma_tx(gmacdev);

    plat_delay(DEFAULT_LOOP_VARIABLE);
    synopGMAC_check_phy_init(adapter);
    synopGMAC_mac_init(gmacdev);

    if (aos_sem_new(&dev->sem_ack, 0)) {
        aos_debug_printf("gmac sem ack init failed!\n");
        return ERR_IF;
    }

    if (aos_sem_new(&dev->sem_lock, 1)) {
        aos_debug_printf("gmac sem lock init failed!\n");
        return ERR_IF;
    }

    if (aos_timer_new_ext(&dev->link_timer, synopGMAC_linux_cable_unplug_function, adapter, 1000, true, true)) {
        aos_debug_printf("gmac link timer init failed!\n");
        return ERR_IF;
    }

    if (aos_task_new_ext(&dev->rx_task, "gmac_rx", rx_task_func, dev, 8192, 20)) {
        aos_debug_printf("gmac rx task init failed!\n");
        return ERR_IF;
    }

#ifdef AOS_USING_GMAC_INT_MODE
    /* installl isr */
    DEBUG_MES("%s\n", __func__);
    irq_install(LS1C_MAC_IRQ, eth_rx_irq, dev);
    irq_enable(LS1C_MAC_IRQ);
#else
    /* TODO */
#endif  /*AOS_USING_GMAC_INT_MODE*/

    aos_debug_printf("gmac ethernetif inited!\n");

    return ERR_OK;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    s32 status;
    u32 pbuf;
    u64 dma_addr;
    u32 offload_needed = 0;
    u32 index;
    DmaDesc *dpr;
    struct eth_dev *dev = aos_container_of(netif, struct eth_dev, netif);
    struct synopGMACNetworkAdapter *adapter = &dev->gmac_adapter;
    synopGMACdevice *gmacdev = adapter->synopGMACdev;

    DEBUG_MES("in %s\n", __func__);

    /* lock eth device */
    aos_sem_wait(&dev->sem_lock, AOS_WAIT_FOREVER);

    if (!synopGMAC_is_desc_owned_by_dma(gmacdev->TxNextDesc))
    {

        pbuf = (u32)plat_alloc_memory(p->tot_len);
        //pbuf = (u32)pbuf_alloc(PBUF_LINK, p->len, PBUF_RAM);
        if (pbuf == 0)
        {
            aos_debug_printf("===error in alloc bf1\n");
            return ERR_BUF;
        }

        DEBUG_MES("p->len = %d\n", p->len);
        pbuf_copy_partial(p, (void *)pbuf, p->tot_len, 0);
        dma_addr = plat_dma_map_single(gmacdev, (void *)pbuf, p->tot_len);

        status = synopGMAC_set_tx_qptr(gmacdev, dma_addr, p->tot_len, pbuf, 0, 0, 0, offload_needed, &index, dpr);
        if (status < 0)
        {
            aos_debug_printf("%s No More Free Tx Descriptors\n", __func__);

            plat_free_memory((void *)pbuf);
            return ERR_BUF;
        }
    }

    synopGMAC_resume_dma_tx(gmacdev);

    s32 desc_index;
    u32 data1, data2;
    u32 dma_addr1, dma_addr2;
    u32 length1, length2;
#ifdef ENH_DESC_8W
    u32 ext_status;
    u16 time_stamp_higher;
    u32 time_stamp_high;
    u32 time_stamp_low;
#endif
    do
    {
#ifdef ENH_DESC_8W
        desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2, &ext_status, &time_stamp_high, &time_stamp_low);
        synopGMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);
#else
        desc_index = synopGMAC_get_tx_qptr(gmacdev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2);
#endif
        if (desc_index >= 0 && data1 != 0)
        {
#ifdef  IPC_OFFLOAD
            if (synopGMAC_is_tx_ipv4header_checksum_error(gmacdev, status))
            {
                aos_debug_printf("Harware Failed to Insert IPV4 Header Checksum\n");
            }
            if (synopGMAC_is_tx_payload_checksum_error(gmacdev, status))
            {
                aos_debug_printf("Harware Failed to Insert Payload Checksum\n");
            }
#endif

            plat_free_memory((void *)(data1));  //sw:   data1 = buffer1

            if (synopGMAC_is_desc_valid(status))
            {
                adapter->synopGMACNetStats.tx_bytes += length1;
                adapter->synopGMACNetStats.tx_packets++;
            }
            else
            {
                adapter->synopGMACNetStats.tx_errors++;
                adapter->synopGMACNetStats.tx_aborted_errors += synopGMAC_is_tx_aborted(status);
                adapter->synopGMACNetStats.tx_carrier_errors += synopGMAC_is_tx_carrier_error(status);
            }
        }
        adapter->synopGMACNetStats.collisions += synopGMAC_get_tx_collision_count(status);
    } while (desc_index >= 0);

    /* unlock eth device */
    aos_sem_signal(&dev->sem_lock);
    // aos_debug_printf("output %d bytes\n", p->len);
    u32 test_data;
    test_data = synopGMACReadReg(gmacdev->DmaBase, DmaStatus);

    return ERR_OK;
}

static struct pbuf *eth_rx(struct eth_dev *dev)
{
    DEBUG_MES("%s : \n", __func__);
    struct synopGMACNetworkAdapter *adapter;
    synopGMACdevice *gmacdev;
//  struct PmonInet * pinetdev;
    s32 desc_index;
    int i;
    char *ptr;
    u32 bf1;
    u32 data1;
    u32 data2;
    u32 len;
    u32 status;
    u32 dma_addr1;
    u32 dma_addr2;
    struct pbuf *pbuf = NULL;

    aos_sem_wait(&dev->sem_lock, AOS_WAIT_FOREVER);

    adapter = (struct synopGMACNetworkAdapter *)&dev->gmac_adapter;
    gmacdev = (synopGMACdevice *)adapter->synopGMACdev;

    /*Handle the Receive Descriptors*/
    desc_index = synopGMAC_get_rx_qptr(gmacdev, &status, &dma_addr1, NULL, &data1, &dma_addr2, NULL, &data2);
    if (desc_index >= 0 && data1 != 0)
    {
        DEBUG_MES("Received Data at Rx Descriptor %d for skb 0x%08x whose status is %08x\n", desc_index, dma_addr1, status);
        if (synopGMAC_is_rx_desc_valid(status) || SYNOP_PHY_LOOPBACK)
        {
            dma_addr1 = plat_dma_map_single(gmacdev, (void *)data1, RX_BUF_SIZE);
            len = synopGMAC_get_rx_desc_frame_length(status) - 4; //Not interested in Ethernet CRC bytes
            pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
            if (pbuf == NULL) {
                aos_debug_printf("===error in pbuf_alloc\n");
            } else {
                struct pbuf *q = NULL;
                const uint8_t *buffer = (const uint8_t *)data1;
                uint32_t bufferoffset = 0;
                uint32_t copylen = 0;
                uint32_t leftlen = len;
                uint32_t pbuf_len = 0;
                void *dest_buf = NULL;

                for (q = pbuf; q != NULL && leftlen > 0; q = q->next) {
                    dest_buf = q->payload;
                    pbuf_len = q->len;

                    if (pbuf_len > leftlen) {
                        copylen = leftlen;
                    } else {
                        copylen = pbuf_len;
                    }

                    memcpy(dest_buf, &buffer[bufferoffset], copylen);
                    bufferoffset = bufferoffset + copylen;
                    leftlen = leftlen - copylen;
                }

                DEBUG_MES("==get pkg len: %d\n", len);
            }
        }
        else
        {
            aos_debug_printf("s: %08x\n", status);
            adapter->synopGMACNetStats.rx_errors++;
            adapter->synopGMACNetStats.collisions       += synopGMAC_is_rx_frame_collision(status);
            adapter->synopGMACNetStats.rx_crc_errors    += synopGMAC_is_rx_crc(status);
            adapter->synopGMACNetStats.rx_frame_errors  += synopGMAC_is_frame_dribbling_errors(status);
            adapter->synopGMACNetStats.rx_length_errors += synopGMAC_is_rx_frame_length_errors(status);
        }
        desc_index = synopGMAC_set_rx_qptr(gmacdev, dma_addr1, RX_BUF_SIZE, (u32)data1, 0, 0, 0);
        if (desc_index < 0)
        {
#if SYNOP_RX_DEBUG
            aos_debug_printf("Cannot set Rx Descriptor for data1 %08x\n", (u32)data1);
#endif
            plat_free_memory((void *)data1);
        }
    }

    aos_sem_signal(&dev->sem_lock);
    DEBUG_MES("%s : before return \n", __func__);

    return pbuf;
}

static void rx_task_func(void *arg)
{
    struct eth_dev *dev = (struct eth_dev *)arg;

    while (1) {
        aos_sem_wait(&dev->sem_ack, AOS_WAIT_FOREVER);

        while (1) {
            struct pbuf *pbuf;

            pbuf = eth_rx(dev);
            if (pbuf) {
                if (dev->netif.input(pbuf, &dev->netif) != ERR_OK)
                    pbuf_free(pbuf);
            } else {
                break;
            }
        }
    }
}

static int rtl88e1111_config_init(synopGMACdevice *gmacdev)
{
    int retval, err;
    u16 data;

    DEBUG_MES("in %s\n", __func__);
    synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x14, &data);
    data = data | 0x82;
    err = synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x14, data);
    synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x00, &data);
    data = data | 0x8000;
    err = synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x00, data);
#if SYNOP_PHY_LOOPBACK
    synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x14, &data);
    data = data | 0x70;
    data = data & 0xffdf;
    err = synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x14, data);
    data = 0x8000;
    err = synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x00, data);
    data = 0x5140;
    err = synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x00, data);
#endif
    if (err < 0)
        return err;
    return 0;
}

static int init_phy(synopGMACdevice *gmacdev)
{
    u16 data;

    synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 2, &data);
    /*set 88e1111 clock phase delay*/
    if (data == 0x141)
        rtl88e1111_config_init(gmacdev);
#if defined (RMII)
    else if (data == 0x8201)
    {
        //RTL8201
        data = 0x400;    // set RMII mode
        synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x19, data);
        synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x19, &data);
        TR("phy reg25 is %0x \n", data);

        data = 0x3100;    //set  100M speed
        synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x0, data);
    }
    else if (data == 0x0180 || data == 0x0181)
    {
        //DM9161
        synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x10, &data);
        data |= (1 << 8);  //set RMII mode
        synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x10, data); //set RMII mode
        synopGMAC_read_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x10, &data);
        TR("phy reg16 is 0x%0x \n", data);

        //  synopGMAC_read_phy_reg(gmacdev->MacBase,gmacdev->PhyBase,0x0,&data);
        //  data &= ~(1<<10);
        data = 0x3100;  //set auto-
        //data = 0x0100;    //set  10M speed
        synopGMAC_write_phy_reg(gmacdev->MacBase, gmacdev->PhyBase, 0x0, data);
    }
#endif

    return 0;
}

u32 synopGMAC_wakeup_filter_config3[] =
{
    0x00000000,
    0x000000FF,
    0x00000000,
    0x00000000,
    0x00000100,
    0x00003200,
    0x7eED0000,
    0x00000000
};

static void synopGMAC_linux_powerdown_mac(synopGMACdevice *gmacdev)
{
    aos_debug_printf("Put the GMAC to power down mode..\n");

    GMAC_Power_down = 1;

    synopGMAC_disable_dma_tx(gmacdev);
    plat_delay(10000);

    synopGMAC_tx_disable(gmacdev);
    synopGMAC_rx_disable(gmacdev);
    plat_delay(10000);

    synopGMAC_disable_dma_rx(gmacdev);

    synopGMAC_magic_packet_enable(gmacdev);
    synopGMAC_write_wakeup_frame_register(gmacdev, synopGMAC_wakeup_filter_config3);

    synopGMAC_wakeup_frame_enable(gmacdev);

    synopGMAC_rx_enable(gmacdev);

    synopGMAC_pmt_int_enable(gmacdev);

    synopGMAC_power_down_enable(gmacdev);
    return;
}

static void synopGMAC_linux_powerup_mac(synopGMACdevice *gmacdev)
{
    GMAC_Power_down = 0;
    if (synopGMAC_is_magic_packet_received(gmacdev))
        aos_debug_printf("GMAC wokeup due to Magic Pkt Received\n");
    if (synopGMAC_is_wakeup_frame_received(gmacdev))
        aos_debug_printf("GMAC wokeup due to Wakeup Frame Received\n");

    synopGMAC_pmt_int_disable(gmacdev);

    synopGMAC_rx_enable(gmacdev);
    synopGMAC_enable_dma_rx(gmacdev);

    synopGMAC_tx_enable(gmacdev);
    synopGMAC_enable_dma_tx(gmacdev);
    return;
}


static int mdio_read(synopGMACPciNetworkAdapter *adapter, int addr, int reg)
{
    synopGMACdevice *gmacdev;
    u16 data;
    gmacdev = adapter->synopGMACdev;

    synopGMAC_read_phy_reg(gmacdev->MacBase, addr, reg, &data);
    return data;
}

static void mdio_write(synopGMACPciNetworkAdapter *adapter, int addr, int reg, int data)
{
    synopGMACdevice *gmacdev;
    gmacdev = adapter->synopGMACdev;
    synopGMAC_write_phy_reg(gmacdev->MacBase, addr, reg, data);
}

static void eth_rx_irq(int irqno, void *param)
{
    struct eth_dev *dev = (struct eth_dev *)param;
    struct synopGMACNetworkAdapter *adapter = &dev->gmac_adapter;
    synopGMACdevice *gmacdev = (synopGMACdevice *)adapter->synopGMACdev;
    u32 interrupt, dma_status_reg;
    s32 status;
    u32 dma_addr;

    DEBUG_MES("in irq!!\n");
#ifdef AOS_USING_GMAC_INT_MODE
    plat_delay(7200);
#endif /*AOS_USING_GMAC_INT_MODE*/

    //aos_debug_printf("irq i = %d\n", i++);
    dma_status_reg = synopGMACReadReg(gmacdev->DmaBase, DmaStatus);
    if (dma_status_reg == 0)
    {
        aos_debug_printf("dma_status ==0 \n");
        return;
    }

    //aos_debug_printf("dma_status_reg is 0x%x\n", dma_status_reg);
    u32 gmacstatus;
    synopGMAC_disable_interrupt_all(gmacdev);
    gmacstatus = synopGMACReadReg(gmacdev->MacBase, GmacStatus);

    if (dma_status_reg & GmacPmtIntr)
    {
        aos_debug_printf("%s:: Interrupt due to PMT module\n", __func__);
        //synopGMAC_linux_powerup_mac(gmacdev);
    }
    if (dma_status_reg & GmacMmcIntr)
    {
        aos_debug_printf("%s:: Interrupt due to MMC module\n", __func__);
        DEBUG_MES("%s:: synopGMAC_rx_int_status = %08x\n", __func__, synopGMAC_read_mmc_rx_int_status(gmacdev));
        DEBUG_MES("%s:: synopGMAC_tx_int_status = %08x\n", __func__, synopGMAC_read_mmc_tx_int_status(gmacdev));
    }

    if (dma_status_reg & GmacLineIntfIntr)
    {
        aos_debug_printf("%s:: Interrupt due to GMAC LINE module\n", __func__);
    }

    interrupt = synopGMAC_get_interrupt_type(gmacdev);
    //aos_debug_printf("%s:Interrupts to be handled: 0x%08x\n",__func__,interrupt);
    if (interrupt & synopGMACDmaError)
    {
        u8 mac_addr0[6];

        aos_debug_printf("%s::Fatal Bus Error Inetrrupt Seen\n", __func__);

        synopGMAC_disable_dma_tx(gmacdev);
        synopGMAC_disable_dma_rx(gmacdev);

        synopGMAC_take_desc_ownership_tx(gmacdev);
        synopGMAC_take_desc_ownership_rx(gmacdev);

        synopGMAC_init_tx_rx_desc_queue(gmacdev);

        synopGMAC_reset(gmacdev);

        mac_addr0[0]= dev->netif.hwaddr[0];
        mac_addr0[1]= dev->netif.hwaddr[1];
        mac_addr0[2]= dev->netif.hwaddr[2];
        mac_addr0[3]= dev->netif.hwaddr[3];
        mac_addr0[4]= dev->netif.hwaddr[4];
        mac_addr0[5]= dev->netif.hwaddr[5];
        synopGMAC_set_mac_addr(gmacdev, GmacAddr0High, GmacAddr0Low, mac_addr0);
        synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength8 | DmaDescriptorSkip2);
        synopGMAC_dma_control_init(gmacdev, DmaStoreAndForward);
        synopGMAC_init_rx_desc_base(gmacdev);
        synopGMAC_init_tx_desc_base(gmacdev);
        synopGMAC_mac_init(gmacdev);
        synopGMAC_enable_dma_rx(gmacdev);
        synopGMAC_enable_dma_tx(gmacdev);
    }

    if (interrupt & synopGMACDmaRxNormal)
    {
        //DEBUG_MES("%s:: Rx Normal \n", __func__);
        //synop_handle_received_data(netdev);
        aos_sem_signal(&dev->sem_ack);
    }
    if (interrupt & synopGMACDmaRxAbnormal)
    {
        //aos_debug_printf("%s::Abnormal Rx Interrupt Seen\n",__func__);
        if (GMAC_Power_down == 0)
        {
            adapter->synopGMACNetStats.rx_over_errors++;
            synopGMACWriteReg(gmacdev->DmaBase, DmaStatus, 0x80);
            synopGMAC_resume_dma_rx(gmacdev);
        }
    }
    if (interrupt & synopGMACDmaRxStopped)
    {
        aos_debug_printf("%s::Receiver stopped seeing Rx interrupts\n", __func__); //Receiver gone in to stopped state
    }

    if (interrupt & synopGMACDmaTxNormal)
    {
        DEBUG_MES("%s::Finished Normal Transmission \n", __func__);
        //          synop_handle_transmit_over(netdev);
    }

    if (interrupt & synopGMACDmaTxAbnormal)
    {
        aos_debug_printf("%s::Abnormal Tx Interrupt Seen\n", __func__);
    }
    if (interrupt & synopGMACDmaTxStopped)
    {
        TR("%s::Transmitter stopped sending the packets\n", __func__);
        if (GMAC_Power_down == 0)    // If Mac is not in powerdown
        {
            synopGMAC_disable_dma_tx(gmacdev);
            synopGMAC_take_desc_ownership_tx(gmacdev);

            synopGMAC_enable_dma_tx(gmacdev);
            //      netif_wake_queue(netdev);
            TR("%s::Transmission Resumed\n", __func__);
        }
    }
    /* Enable the interrrupt before returning from ISR*/
    synopGMAC_enable_interrupt(gmacdev, DmaIntEnable);

    return;
}

static int gmac_eth_init(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    ip_addr_t dns;
    ip_addr_t old_dns;
    u64 base_addr = Gmac_base;
    struct synopGMACNetworkAdapter *synopGMACadapter;
    static u8 mac_addr0[6] = DEFAULT_MAC_ADDRESS;
    int index;

    IP4_ADDR(&ipaddr, 10, 0, 2, 15);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 10, 0, 2, 2);
    IP4_ADDR(&dns, 10, 0, 2, 3);

    for (index = 21; index <= 30; index++)
    {
        pin_set_purpose(index, PIN_PURPOSE_OTHER);
        pin_set_remap(index, PIN_REMAP_FIRST);
    }

    pin_set_purpose(35, PIN_PURPOSE_OTHER);
    pin_set_remap(35, PIN_REMAP_FIRST);
    *((volatile unsigned int *)0xbfd00424) &= ~(7 << 28);
    *((volatile unsigned int *)0xbfd00424) |= (1 << 30); //wl rmii

    synopGMACadapter = &eth_dev.gmac_adapter;
    synopGMACadapter->synopGMACdev = &eth_dev.gmac_dev;
    /*
     * Attach the device to MAC struct This will configure all the required base addresses
     * such as Mac base, configuration base, phy base address(out of 32 possible phys)
     * */
    synopGMAC_attach(synopGMACadapter->synopGMACdev, (regbase + MACBASE), regbase + DMABASE, DEFAULT_PHY_BASE, mac_addr0);

    init_phy(synopGMACadapter->synopGMACdev);
    synopGMAC_reset(synopGMACadapter->synopGMACdev);

    /* MII setup */
    synopGMACadapter->mii.phy_id_mask = 0x1F;
    synopGMACadapter->mii.reg_num_mask = 0x1F;
    synopGMACadapter->mii.dev = synopGMACadapter;
    synopGMACadapter->mii.mdio_read = mdio_read;
    synopGMACadapter->mii.mdio_write = mdio_write;
    synopGMACadapter->mii.phy_id = synopGMACadapter->synopGMACdev->PhyBase;
    synopGMACadapter->mii.supports_gmii = mii_check_gmii_support(&synopGMACadapter->mii);

    netif_add(&eth_dev.netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
    netif_set_default(&eth_dev.netif);
    netif_set_up(&eth_dev.netif);
    old_dns = *dns_getserver(0);
    dns_setserver(0, &dns);
    dns_setserver(1, &old_dns);

    return 0;
}

LEVEL1_DRIVER_ENTRY(gmac_eth_init);
