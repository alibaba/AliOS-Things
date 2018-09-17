/************************************************************************/
/**	\file	bfemac.c
 *	\brief	LwIP network interface for Blackfin built-in EMAC
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#if !NO_SYS
#include "vdk.h"
#endif
#include <blackfin.h>
#include <sys/exception.h>
#include <cplbtab.h> 
#include "bfemac.h"
#include "bfphy.h"
#include "sys_protect.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "netif/etharp.h"

/* Macros */
#define IFNAME0				'e'
#define IFNAME1				't'

#define RX_DESCRIPTORS		(PBUF_POOL_SIZE*3/4)
#define TX_DESCRIPTORS		16

#define DMATX				(FLOW_LARGE | NDSIZE_5 | SYNC | WDSIZE_32 | DMAEN)
#define DMARX				(FLOW_LARGE | NDSIZE_5 | WDSIZE_32 | WNR | DMAEN)
#define DMASTATUS_NEXT		(FLOW_LARGE | NDSIZE_5 | DI_EN | WDSIZE_32 | WNR | DMAEN)
#define DMASTATUS_LAST		(FLOW_STOP | NDSIZE_0 | DI_EN | WDSIZE_32 | WNR | DMAEN)

#define CRC32_POLYNOMIAL	0xEDB88320

/* Types */
struct dma_hwdesc
{
	void*	next;
	void*	startaddr;
	u16_t	config;
};

struct dma_desc
{
	struct pbuf*		packet;
	u32_t				status;
	struct dma_hwdesc	d_packet;
	struct dma_hwdesc	d_status;
	struct dma_desc*	next;
};

struct dma_queue
{
	struct dma_desc*	first;
	struct dma_desc*	last;
	int					n;
};

/* Externals */

/* Variables */
u8_t	bfemac_hash_bin_count[64];	// uninitialized vars are 0
u8_t	bfemac_rmii	= 0;

volatile u8_t bfemac_txevent = 0;
volatile u8_t bfemac_rxevent = 0;
#if !NO_SYS
struct tcpip_callback_msg* bfemac_txmsg;
struct tcpip_callback_msg* bfemac_rxmsg;
#endif

#pragma default_section(ALLDATA, "L1_data")

struct dma_desc		txdescriptor[TX_DESCRIPTORS];
struct dma_desc		rxdescriptor[RX_DESCRIPTORS];

#pragma default_section(ALLDATA, "L1_scratchpad")

struct dma_queue	txfree;			// free descriptors
struct dma_queue	txqueued;		// descriptors ready with pbuf to transmit
struct dma_queue	txactive;		// active DMA chain
struct dma_queue	txcomplete;		// transmitted frame, pbuf can be freed

struct dma_queue	rxfree;			// free descriptors
struct dma_queue	rxqueued;		// descriptors ready with pbuf(pool) pre-allocated for reception
struct dma_queue	rxactive;		// active DMA chain
struct dma_queue	rxcomplete;		// frame received, pbuf ready for input

#pragma default_section(ALLDATA)

/* Queue Functions */

#pragma default_section(CODE, "L1_code")

static struct dma_desc* bfemac_pop(struct dma_queue* q)
{
	struct dma_desc* buf = q->first;
	if (buf)
	{
		q->first = buf->next;
		buf->next = 0;
		if (!q->first)	q->last = 0;
		q->n--;
	}
	return buf;
}

static void bfemac_push_desc(struct dma_queue* q, struct dma_desc* buf)
{
	buf->next = 0;
	buf->d_status.next = 0;
	buf->d_status.config = DMASTATUS_LAST;

	if (q->first)
	{
		q->last->next = buf;
		q->last->d_status.next = &buf->d_packet;
		q->last->d_status.config = DMASTATUS_NEXT;
	}
	else
	{
		q->first = buf;
	}
	q->last = buf;
	q->n++;
}

static void bfemac_push_queue(struct dma_queue* q1, struct dma_queue* q2)
{
	if (!q2->first)	return;

	if (q1->first)
	{
		q1->last->next = q2->first;
		q1->last->d_status.next = &q2->first->d_packet;
		q1->last->d_status.config = DMASTATUS_NEXT;
	}
	else
	{
		q1->first = q2->first;
	}
	q1->last = q2->last;
	q1->n += q2->n;
	q2->first = 0;
	q2->last = 0;
	q2->n=0;
}

static bool	bfemac_alloc_rxdesc(struct dma_desc *rxdesc)
{
	sys_prot_t old_level;

    // Allocate a new pbuf from the pool
	rxdesc->status = 0;
	rxdesc->packet = pbuf_alloc(PBUF_RAW, 1520, PBUF_POOL);

	// If available, append it to the reception queue
	if (rxdesc->packet)
	{
		rxdesc->d_packet.startaddr = rxdesc->packet->payload;
		flush_data_buffer(rxdesc->packet->payload, (u8_t*)rxdesc->packet->payload+1520-1, 1);

    	old_level = sys_arch_protect();

   		bfemac_push_desc(&rxqueued, rxdesc);

		// If not active, then start the DMA chain here
		if (!rxactive.first)
		{
			bfemac_push_queue(&rxactive, &rxqueued);

			*pDMA1_CONFIG = 0;
			*pDMA1_NEXT_DESC_PTR = &rxactive.first->d_packet;
			*pDMA1_CONFIG = DMARX;
			*pEMAC_OPMODE |= RE;
		}

	    sys_arch_unprotect(old_level);
	    return true;
	}

	// Else, append it to the free queue, to be realloced later
	rxdesc->d_packet.startaddr = 0;

	old_level = sys_arch_protect();
	bfemac_push_desc(&rxfree, rxdesc);
	sys_arch_unprotect(old_level);
	return false;
}

static void bfemac_alloc_rxfreequeue(void)
{
	sys_prot_t old_level;
	struct dma_desc *rxdesc;
	while (1)
	{
		old_level = sys_arch_protect();
		rxdesc = bfemac_pop(&rxfree);
		sys_arch_unprotect(old_level);
		if (!rxdesc || !bfemac_alloc_rxdesc(rxdesc))	return;
	}
}


/* DMA Interrupts */

#pragma default_section(CODE, "L1_code")

EX_REENTRANT_HANDLER(bfemac_dma_interrupts) 
{
	int isr = *pSIC_ISR;
	// RX
	if (isr & IRQ_DMA1)
	{
		// Confirm handling
		*pDMA1_IRQ_STATUS = DMA_DONE;

		// Move received descriptors to complete queue
		struct dma_desc *rxdesc = rxactive.first;
		while (rxdesc && (rxdesc->status & RX_COMP))
		{
			struct dma_desc *rxnext = rxdesc->next;
			bfemac_push_desc(&rxcomplete, rxdesc);
			rxdesc = rxnext;
			rxactive.n--;
		}

		// Update active queue
		rxactive.first = rxdesc;
		if (!rxactive.first)
		{
			if (rxqueued.first)
			{
				bfemac_push_queue(&rxactive, &rxqueued);
				
				*pDMA1_CONFIG = 0;
				*pDMA1_NEXT_DESC_PTR = &rxactive.first->d_packet;
				*pDMA1_CONFIG = DMARX;
				*pEMAC_OPMODE |= RE;
			}
			else
			{
				*pEMAC_OPMODE &= ~RE;
				rxactive.last = 0;
			}
		}
		else if (rxactive.n >= 2)
		{
			bfemac_push_queue(&rxactive, &rxqueued);
		}

		// Callback
		#if !NO_SYS
		if (!bfemac_rxevent)	tcpip_trycallback(bfemac_rxmsg);
		#endif
		bfemac_rxevent = 1;
	}
	// TX
	if (isr & IRQ_DMA2)
	{
		// Confirm handling
		*pDMA2_IRQ_STATUS = DMA_DONE;

		// Move received descriptors to complete queue
		struct dma_desc *txdesc = txactive.first;
		while (txdesc && (txdesc->status & TX_COMP))
		{
			struct dma_desc *txnext = txdesc->next;
			bfemac_push_desc(&txcomplete, txdesc);
			txdesc = txnext;
			txactive.n--;
		}

		// Update active queue
		txactive.first = txdesc;
		if (!txactive.first)
		{
			if (txqueued.first)
			{
				bfemac_push_queue(&txactive, &txqueued);
				
				*pDMA2_CONFIG = 0;
				*pDMA2_NEXT_DESC_PTR = &txactive.first->d_packet;
				*pDMA2_CONFIG = DMATX;
				*pEMAC_OPMODE |= TE;
			}
			else
			{
				*pEMAC_OPMODE &= ~TE;
				txactive.last = 0;
			}
		}
		else if (txactive.n >= 2)
		{
			bfemac_push_queue(&txactive, &txqueued);
		}

		// Callback
		#if !NO_SYS
		if (!bfemac_txevent)	tcpip_trycallback(bfemac_txmsg);
		#endif
		bfemac_txevent = 1;
	}
}

#pragma default_section(CODE)

/* Callbacks */

static void bfemac_tx_callback(void* ctx)
{
	sys_prot_t old_level;
	struct netif *netif = (struct netif *)ctx;
	struct dma_desc *txdesc;

	while (1)
	{
		// Extract descriptor from tx completed queue
		old_level = sys_arch_protect();
		txdesc = bfemac_pop(&txcomplete);
		if (!txdesc)	bfemac_txevent = 0;
		sys_arch_unprotect(old_level);
		if (!txdesc)	break;

		if (txdesc->status & TX_OK)
		{
			LINK_STATS_INC(link.xmit);
			snmp_add_ifoutoctets(netif, txdesc->status>>16);
			if (txdesc->status & (TX_BROAD|TX_MULTI))	snmp_inc_ifoutnucastpkts(netif);
			else										snmp_inc_ifoutucastpkts(netif);
		}
		else
		{
			LINK_STATS_INC(link.drop);
			snmp_inc_ifoutdiscards(netif);
		}
		
		// Free the pbuf
		pbuf_free(txdesc->packet);
		txdesc->packet = 0;
		txdesc->d_packet.startaddr = 0;

		// Move back to free queue
		old_level = sys_arch_protect();
		bfemac_push_desc(&txfree, txdesc);
		sys_arch_unprotect(old_level);
	}

	bfemac_alloc_rxfreequeue();
}

static void bfemac_rx_callback(void* ctx)
{
	sys_prot_t old_level;
	struct netif *netif = (struct netif *)ctx;
	struct dma_desc *rxdesc;

	while (1)
	{
		// Extract descriptor from rx completed queue
		old_level = sys_arch_protect();
		rxdesc = bfemac_pop(&rxcomplete);
		if (!rxdesc)	bfemac_rxevent = 0;
		sys_arch_unprotect(old_level);
		if (!rxdesc)	break;

		LINK_STATS_INC(link.recv);

		u16_t l = (rxdesc->status & 0x7FF);
		snmp_add_ifinoctets(netif, l);

		if (rxdesc->status & (RX_BROAD|RX_MULTI))	snmp_inc_ifinnucastpkts(netif);
		else										snmp_inc_ifinucastpkts(netif);

		// Pass the packet to the stack
		struct pbuf* p = rxdesc->packet;
		p->tot_len = p->len = l + 2;
		ethernet_input(p, netif);

		// Try to reuse the descriptor
		bfemac_alloc_rxdesc(rxdesc);
	}

	bfemac_alloc_rxfreequeue();
}

/* Driver Functions */

static err_t bfemac_low_level_output(struct netif *netif, struct pbuf *p)
{
	sys_prot_t old_level;

	struct dma_desc *txdesc;
	struct pbuf *q = p;
	
	// DMA supports only one contiguous packet buffer
	if (p->next)
	{
		q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
		if (!q)
		{
		    LINK_STATS_INC(link.memerr);
		    LINK_STATS_INC(link.drop);
		    snmp_inc_ifoutdiscards(netif);
			return ERR_MEM;
		}
		pbuf_copy(q, p);
	}
	else
	{
		// Increment reference count because we queue the packet to the DMA chain and return immedialety
		// After transmission complete, the interrupt callback will free the pbuf
		pbuf_ref(q);
	}

	// Get a TX DMA descriptor from the free queue
	old_level = sys_arch_protect();
	txdesc = bfemac_pop(&txfree);
	sys_arch_unprotect(old_level);
	if (!txdesc)
	{
		pbuf_free(q);
	    LINK_STATS_INC(link.memerr);
	    LINK_STATS_INC(link.drop);
	    snmp_inc_ifoutdiscards(netif);
		return ERR_MEM;
	}

	// Setup packet
	txdesc->status = 0;
	txdesc->packet = q;
	txdesc->d_packet.startaddr = (u8_t*)q->payload;
	*(u16_t*)q->payload = q->len - 2;
	flush_data_buffer(q->payload, (u8_t*)q->payload+q->len, 0);

	old_level = sys_arch_protect();

	// Append to the queued descriptors
	bfemac_push_desc(&txqueued, txdesc);

	// If not active, then start the DMA chain here
	if (!txactive.first)
	{
		bfemac_push_queue(&txactive, &txqueued);

		*pDMA2_CONFIG = 0;
		*pDMA2_NEXT_DESC_PTR = &txactive.first->d_packet;
		*pDMA2_CONFIG = DMATX;
		*pEMAC_OPMODE |= TE;
	}

	sys_arch_unprotect(old_level);

	return ERR_OK;
}

static int bfemac_get_mac_hash_bin(const u8_t *macaddr)
{
	int		bin_index;
	int 	i, j;
	u32_t	crc32 = 0xFFFFFFFF;
	u8_t	data;
	bool	bit;

	for (i=0; i<6; i++)
	{     
		data = macaddr[i];
		for (j=0; j<8; j++)
		{
			bit = (crc32 ^ data) & 1;
			crc32 >>= 1;
			if (bit)	crc32 ^= CRC32_POLYNOMIAL;
			data >>= 1;
		}           
	}

	bin_index = ((crc32&0x01)<<5) | ((crc32&0x02)<<3) | ((crc32&0x04)<<1) | ((crc32&0x08)>>1) | ((crc32&0x10)>>3) | ((crc32&0x20)>>5);
	return bin_index;
}

static err_t bfemac_igmp_mac_filter(struct netif *netif, ip_addr_t *group, u8_t action)
{
	u8_t multicast_mac[6];
	multicast_mac[0] = 0x01;
	multicast_mac[1] = 0x00;
	multicast_mac[2] = 0x5E;
	multicast_mac[3] = ip4_addr2(group) & 0x7F;
	multicast_mac[4] = ip4_addr3(group);
	multicast_mac[5] = ip4_addr4(group);

	int bin_index = bfemac_get_mac_hash_bin(multicast_mac);
	
	if (action == IGMP_ADD_MAC_FILTER)
	{
		if (bfemac_hash_bin_count[bin_index]++ == 0)
		{
			if (bin_index < 32)	*pEMAC_HASHLO |= 1 << bin_index;
			else				*pEMAC_HASHHI |= 1 << (bin_index-32);
		}
		return ERR_OK;
	}
	else if (action == IGMP_DEL_MAC_FILTER)
	{
		if (--bfemac_hash_bin_count[bin_index] == 0)
		{
			if (bin_index < 32)	*pEMAC_HASHLO &= ~(1 << bin_index);
			else				*pEMAC_HASHHI &= ~(1 << (bin_index-32));
		}
		return ERR_OK;
	}
	
	return ERR_IF;
}

err_t bfemac_init(struct netif *netif)
{
	int i;
	u32_t u;

	// LwIP network interface level
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = etharp_output;
	netif->linkoutput = bfemac_low_level_output;
	netif->igmp_mac_filter = bfemac_igmp_mac_filter;
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->mtu = 1500;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

	// PHY
	bfphy_init(netif);

	// MAC Address
	*pEMAC_ADDRLO = (netif->hwaddr[3]<<24) | (netif->hwaddr[2]<<16) | (netif->hwaddr[1]<<8) | netif->hwaddr[0];
	*pEMAC_ADDRHI = (netif->hwaddr[5]<<8) | netif->hwaddr[4];

	// Operating Mode: Late Collision TX Retry, Hash Multicast, Automatic Pad Stripping
	u = LCTRE | HM | ASTP;
	if (bfemac_rmii)	u |= RMII;
	*pEMAC_OPMODE = u;

	*pEMAC_SYSCTL |= RXDWA;		// Receive frame DMA word alignment

	// DMA Interrupts
	*pEVT11 = (void*) bfemac_dma_interrupts;
	#ifdef _DEF_BF52X_H
	*pSIC_IMASK0 |= IRQ_DMA1 | IRQ_DMA2;
	#else
	*pSIC_IMASK |= IRQ_DMA1 | IRQ_DMA2;
	#endif
	*pIMASK |= EVT_IVG11;

	// Static messages for interrupt callbacks
	#if !NO_SYS
	bfemac_txmsg = tcpip_callbackmsg_new(bfemac_tx_callback, netif);
	bfemac_rxmsg = tcpip_callbackmsg_new(bfemac_rx_callback, netif);
	#endif

	// Init all queues
	memset(&txfree, 0, 8*sizeof(struct dma_queue));

	// Prepare TX Descriptors
	for (i=0; i<TX_DESCRIPTORS; i++)
	{
		txdescriptor[i].d_packet.next = &txdescriptor[i].d_status;
		txdescriptor[i].d_packet.config = DMATX;
		txdescriptor[i].d_status.startaddr = &txdescriptor[i].status;

		txdescriptor[i].status = 0;
		txdescriptor[i].packet = 0;
		txdescriptor[i].d_packet.startaddr = 0;
		bfemac_push_desc(&txfree, &txdescriptor[i]);
	}

	// Initialize TX DMA
	*pDMA2_X_COUNT = 0;
	*pDMA2_X_MODIFY = 4;

	// Prepare RX Descriptors
	for (i=0; i<RX_DESCRIPTORS; i++)
	{
		rxdescriptor[i].d_packet.next = &rxdescriptor[i].d_status;
		rxdescriptor[i].d_packet.config = DMARX;
		rxdescriptor[i].d_status.startaddr = &rxdescriptor[i].status;

		rxdescriptor[i].status = 0;
		rxdescriptor[i].packet = pbuf_alloc(PBUF_RAW, 1520, PBUF_POOL);
		rxdescriptor[i].d_packet.startaddr = rxdescriptor[i].packet->payload;

		flush_data_buffer(rxdescriptor[i].packet->payload, (u8_t*)rxdescriptor[i].packet->payload+1520-1, 1);
		
		bfemac_push_desc(&rxactive, &rxdescriptor[i]);
	}

	// Initialize RX DMA
	*pDMA1_X_COUNT = 0;
	*pDMA1_X_MODIFY = 4;
	*pDMA1_NEXT_DESC_PTR = &rxactive.first->d_packet;
	*pDMA1_CONFIG = DMARX;

	// Enable MAC RX
	*pEMAC_OPMODE |= RE;

	return ERR_OK;
}

#if NO_SYS
void bfemac_poll(struct netif* netif)
{
	if (bfemac_rxevent)		bfemac_rx_callback(netif);
	if (bfemac_txevent)		bfemac_tx_callback(netif);
}
#endif
