/************************************************************************/
/**	\file	c674emac.c
 *	\brief	LwIP network interface for C674x built-in EMAC
 *
 *	\date	2011/10/07
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/

/* Includes */
#if !NO_SYS
#include <xdc/std.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/family/c64p/hwi.h>
#endif
#include <c6x.h>
#include <ti/pspiom/cslr/cslr_chip.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_mdio.h>
#include <ti/pspiom/cslr/cslr_emac1.h>
#include <ti/pspiom/cslr/cslr_ectl1.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
#include <ti/pspiom/cslr/cslr_cache.h>
#include "c674emac.h"
#include "c674phy.h"
#include "cppi_ram.h"
#include "sys_protect.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "netif/etharp.h"
#include <stdbool.h>

/* Macros */
#define IFNAME0					'e'
#define IFNAME1					't'
#define SIZEOF_STRUCT_PBUF		LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))
#define BUFFER_OFFSET			(SIZEOF_STRUCT_PBUF+ETH_PAD_SIZE)

#define L2CACHE_WB(addr, len) { 												\
		CSL_CacheRegsOvly Cache_Regs = (CSL_CacheRegsOvly) CSL_CACHE_0_REGS;	\
		Cache_Regs->L2WBAR = (Uint32) addr;										\
		Cache_Regs->L2WWC = (Uint32) ((len+3)>>2);								\
		while (Cache_Regs->L2WWC); }

#define L2CACHE_WBINV(addr, len) { 												\
		CSL_CacheRegsOvly Cache_Regs = (CSL_CacheRegsOvly) CSL_CACHE_0_REGS;	\
		Cache_Regs->L2WIBAR = (Uint32) addr;									\
		Cache_Regs->L2WIWC = (Uint32) ((len+3)>>2);								\
		while (Cache_Regs->L2WIWC); }

#define L2CACHE_INV(addr, len) { 												\
		CSL_CacheRegsOvly Cache_Regs = (CSL_CacheRegsOvly) CSL_CACHE_0_REGS;	\
		Cache_Regs->L2IBAR = (Uint32) addr;										\
		Cache_Regs->L2IWC = (Uint32) ((len+3)>>2);								\
		while (Cache_Regs->L2WIWC); }

/* EMAC Packet Flags */
#define EMAC_DSC_FLAG_SOP			0x80000000u
#define EMAC_DSC_FLAG_EOP			0x40000000u
#define EMAC_DSC_FLAG_OWNER			0x20000000u
#define EMAC_DSC_FLAG_EOQ			0x10000000u
#define EMAC_DSC_FLAG_TDOWNCMPLT	0x08000000u
#define EMAC_DSC_FLAG_PASSCRC		0x04000000u
#define EMAC_DSC_FLAG_JABBER 		0x02000000u
#define EMAC_DSC_FLAG_OVERSIZE		0x01000000u
#define EMAC_DSC_FLAG_FRAGMENT		0x00800000u
#define EMAC_DSC_FLAG_UNDERSIZED	0x00400000u
#define EMAC_DSC_FLAG_CONTROL		0x00200000u
#define EMAC_DSC_FLAG_OVERRUN		0x00100000u
#define EMAC_DSC_FLAG_CODEERROR		0x00080000u
#define EMAC_DSC_FLAG_ALIGNERROR	0x00040000u
#define EMAC_DSC_FLAG_CRCERROR		0x00020000u
#define EMAC_DSC_FLAG_NOMATCH		0x00010000u

/* Types */
typedef volatile CSL_EmacRegs   *CSL_EmacRegsOvly;
typedef volatile CSL_EctlRegs	*CSL_EctlRegsOvly;

/* Externals */

/* Variables */
u8_t	c674emac_hash_bin_count[64];	// uninitialized vars are 0

/* Queue Functions */

#pragma CODE_SECTION(c674emac_pop, "emac_code")
static struct EMAC_Desc* c674emac_pop(struct dma_queue_t* q)
{
	struct EMAC_Desc *desc = q->first;
	if (desc)
	{
		q->first = desc->next;
		desc->next = 0;
		if (!q->first)	q->last = 0;
		q->n--;
	}
	return desc;
}

#pragma CODE_SECTION(c674emac_push_desc, "emac_code")
static void c674emac_push_desc(struct dma_queue_t* q, struct EMAC_Desc* desc)
{
	desc->next = 0;

	if (q->first)	q->last->next = desc;
	else			q->first = desc;

	q->last = desc;
	q->n++;
}

#pragma CODE_SECTION(c674emac_push_queue, "emac_code")
static void c674emac_push_queue(struct dma_queue_t* q1, struct dma_queue_t* q2)
{
	if (!q2->first)	return;

	if (q1->first) q1->last->next = q2->first;
	else q1->first = q2->first;

	q1->last = q2->last;
	q1->n += q2->n;

	q2->first = 0;
	q2->last = 0;
	q2->n = 0;
}

static bool	c674emac_alloc_rxdesc(struct EMAC_Desc *rxdesc)
{
	sys_prot_t old_level;
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;

    /* Allocate a new pbuf from the pool */
	struct pbuf* p = pbuf_alloc(PBUF_RAW, 1520, PBUF_POOL);
	rxdesc->pBuffer = (Uint8*) p;

	/* If available, append it to the reception queue */
	if (p)
	{
		rxdesc->BufOffLen = 1536;
		rxdesc->PktFlgLen = EMAC_DSC_FLAG_OWNER | 0;
		L2CACHE_WBINV(p, 1536);

    	old_level = sys_arch_protect();

    	c674emac_push_desc(&cppiram->rxqueued, rxdesc);

		/* If not active, then start the DMA chain here */
		if (!cppiram->rxactive.first)
		{
			c674emac_push_queue(&cppiram->rxactive, &cppiram->rxqueued);
			EMAC_Regs->RX0HDP = (Uint32) cppiram->rxactive.first;
		}

	    sys_arch_unprotect(old_level);
	    return true;
	}

	/* Else, append it to the free queue, to be reallocated later */
	old_level = sys_arch_protect();
	c674emac_push_desc(&cppiram->rxfree, rxdesc);
	sys_arch_unprotect(old_level);
	return false;
}

static void c674emac_alloc_rxfreequeue(void)
{
	sys_prot_t old_level;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;
	struct EMAC_Desc *rxdesc;
	while (1)
	{
		old_level = sys_arch_protect();
		rxdesc = c674emac_pop(&cppiram->rxfree);
		sys_arch_unprotect(old_level);
		if (!rxdesc || !c674emac_alloc_rxdesc(rxdesc))	return;
	}
}

/* DMA Interrupts */

#pragma CODE_SECTION(c674emac_rx_interrupt, "emac_code")
void c674emac_rx_interrupt(void *arg)
{
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;

	/* Move received buffers to complete queue */
	struct EMAC_Desc *rxdesc = cppiram->rxactive.first;
	while (rxdesc && (rxdesc->PktFlgLen & EMAC_DSC_FLAG_OWNER) == 0)
	{
		EMAC_Regs->RX0CP = (Uint32) rxdesc;					//Acknowledge Interrupt
		struct EMAC_Desc *rxnext = rxdesc->next;
		c674emac_push_desc(&cppiram->rxcomplete, rxdesc);
		rxdesc = rxnext;
		cppiram->rxactive.n--;
	}

	/* Update active queue */
	cppiram->rxactive.first = rxdesc;
	if (!cppiram->rxactive.first)
	{
		/* DMA Stopped, restart with queued descriptors */
		if (cppiram->rxqueued.first)
		{
			c674emac_push_queue(&cppiram->rxactive, &cppiram->rxqueued);

			EMAC_Regs->RX0HDP = (Uint32) cppiram->rxactive.first;
		}
		else
		{
			cppiram->rxactive.last = 0;
		}
	}
	else if (cppiram->rxactive.n >= 2)
	{
		c674emac_push_queue(&cppiram->rxactive, &cppiram->rxqueued);
	}

	/* Callback */
	#if !NO_SYS
	if (!cppiram->rxevent)	tcpip_trycallback(cppiram->rxmsg);
	#endif
	cppiram->rxevent = 1;

	/* Confirm handling */
	EMAC_Regs->MACEOIVECTOR = CSL_EMAC_MACEOIVECTOR_INTVECT_C0RX;
}

#pragma CODE_SECTION(c674emac_tx_interrupt, "emac_code")
void c674emac_tx_interrupt(void *arg)
{
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;

	/* Move transmitted buffers to complete queue */
	struct EMAC_Desc *txdesc = cppiram->txactive.first;
	while (txdesc && (txdesc->PktFlgLen & EMAC_DSC_FLAG_OWNER) == 0)
	{
		EMAC_Regs->TX0CP = (Uint32) txdesc;
		struct EMAC_Desc *txnext = txdesc->next;
		c674emac_push_desc(&cppiram->txcomplete, txdesc);
		txdesc = txnext;
		cppiram->txactive.n--;
	}

	/* Update active queue */
	cppiram->txactive.first = txdesc;
	if (cppiram->txactive.first == 0)
	{
		/* DMA Stopped, restart with queued descriptors */
		if (cppiram->txqueued.first)
		{
			c674emac_push_queue(&cppiram->txactive, &cppiram->txqueued);

			EMAC_Regs->TX0HDP = (Uint32) cppiram->txactive.first;
		}
		else
		{
			cppiram->txactive.last = 0;
		}
	}
	else if (cppiram->txactive.n >= 2)
	{
		c674emac_push_queue(&cppiram->txactive, &cppiram->txqueued);
	}

	/* Callback */
	#if !NO_SYS
	if (!cppiram->txevent)	tcpip_trycallback(cppiram->txmsg);
	#endif
	cppiram->txevent = 1;

	/* Confirm handling */
	EMAC_Regs->MACEOIVECTOR = CSL_EMAC_MACEOIVECTOR_INTVECT_C0TX;
}

/* Callbacks */

static void c674emac_tx_callback(void* ctx)
{
	sys_prot_t old_level;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;
	struct EMAC_Desc* txdesc;

	while (1)
	{
		/* Extract buffer from tx completed queue */
		old_level = sys_arch_protect();
		txdesc = c674emac_pop(&cppiram->txcomplete);
		if (!txdesc)	cppiram->txevent = 0;
		sys_arch_unprotect(old_level);
		if (!txdesc)	break;

		LINK_STATS_INC(link.xmit);
		snmp_add_ifoutoctets(netif, txdesc->PktFlgLen & 0xFFFF);

		struct pbuf* p = (struct pbuf*) txdesc->pBuffer;
		struct eth_hdr* eh = (struct eth_hdr*) p->payload;
		if ( eh->dest.addr[0] & 0x01 )	snmp_inc_ifoutnucastpkts(netif);
		else							snmp_inc_ifoutucastpkts(netif);
		
		/* Free the pbuf */
		pbuf_free(p);
		txdesc->pBuffer = 0;

		/* Move back to free queue */
		old_level = sys_arch_protect();
		c674emac_push_desc(&cppiram->txfree, txdesc);
		sys_arch_unprotect(old_level);
	}

	c674emac_alloc_rxfreequeue();

	//switch_printstats();
}

static void c674emac_rx_callback(void* ctx)
{
	sys_prot_t old_level;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;
	struct netif* netif = (struct netif *)ctx;
	struct EMAC_Desc* rxdesc;

	while (1)
	{
		/* Extract buffer from rx completed queue */
		old_level = sys_arch_protect();
		rxdesc = c674emac_pop(&cppiram->rxcomplete);
		if (!rxdesc)	cppiram->rxevent = 0;
		sys_arch_unprotect(old_level);
		if (!rxdesc)	break;

		LINK_STATS_INC(link.recv);

		u16_t l = rxdesc->PktFlgLen & 0xFFFF;
		snmp_add_ifinoctets(netif, l);

		struct pbuf* p = (struct pbuf*) rxdesc->pBuffer;
		struct eth_hdr* eh = (struct eth_hdr*) p->payload;
		if ( eh->dest.addr[0] & 0x01 )	snmp_inc_ifinnucastpkts(netif);
		else							snmp_inc_ifinucastpkts(netif);

		/* Pass the packet to the stack */
		p->tot_len = p->len = l + 2;

		ethernet_input(p, netif);

		/* Try to reuse the buffer */
		c674emac_alloc_rxdesc(rxdesc);
	}

	c674emac_alloc_rxfreequeue();
}

/* Driver Functions */
static err_t c674emac_low_level_output(struct netif *netif, struct pbuf *p)
{
	sys_prot_t old_level;
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;

	struct EMAC_Desc *txdesc;
	struct pbuf *q = p;
	
	/* DMA supports only one contiguous packet buffer */
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
		/* Increment reference count because we queue the packet to the DMA chain and return immediately
		 * After transmission complete, the interrupt callback will free the buffer
		 */
		pbuf_ref(q);
	}

	/* Get a TX DMA buffer from the free queue */
	old_level = sys_arch_protect();
	txdesc = c674emac_pop(&cppiram->txfree);
	sys_arch_unprotect(old_level);
	if (!txdesc)
	{
		pbuf_free(q);
	    LINK_STATS_INC(link.memerr);
	    LINK_STATS_INC(link.drop);
	    snmp_inc_ifoutdiscards(netif);
		return ERR_MEM;
	}

	/* Setup descriptor */
	Uint16 l = q->len-2;
	if (l<0x3C) l = 0x3C;
	Uint16 off = ((u8_t*)q->payload - (u8_t*)q) + 2;
	txdesc->pBuffer = (Uint8*) q;
	txdesc->BufOffLen = (off<<16) | l;
	txdesc->PktFlgLen = EMAC_DSC_FLAG_EOP | EMAC_DSC_FLAG_SOP | EMAC_DSC_FLAG_OWNER | l;
	L2CACHE_WB(q, l+off);

	old_level = sys_arch_protect();

	/* Append to the queued buffers */
	c674emac_push_desc(&cppiram->txqueued, txdesc);

	/* If not active, then start the DMA chain here */
	if (!cppiram->txactive.first)
	{
		c674emac_push_queue(&cppiram->txactive, &cppiram->txqueued);
		EMAC_Regs->TX0HDP = (Uint32) cppiram->txactive.first;
	}
	/* Else, let the interrupt manage appending or restart */

	sys_arch_unprotect(old_level);

	return ERR_OK;
}

static int c674emac_get_mac_hash_bin(const u8_t *da)
{
	int		bin_index;
	int 	i, j;

/*
 * Hash_fun(0)=DA(0) XOR DA(6) XOR DA(12) XOR DA(18) XOR DA(24) XOR DA(30) XOR DA(36) XOR DA(42);
 * Hash_fun(1)=DA(1) XOR DA(7) XOR DA(13) XOR DA(19) XOR DA(25) XOR DA(31) XOR DA(37) XOR DA(43);
 * Hash_fun(2)=DA(2) XOR DA(8) XOR DA(14) XOR DA(20) XOR DA(26) XOR DA(32) XOR DA(38) XOR DA(44);
 * Hash_fun(3)=DA(3) XOR DA(9) XOR DA(15) XOR DA(21) XOR DA(27) XOR DA(33) XOR DA(39) XOR DA(45);
 * Hash_fun(4)=DA(4) XOR DA(10) XOR DA(16) XOR DA(22) XOR DA(28) XOR DA(34) XOR DA(40) XOR DA(46);
 * Hash_fun(5)=DA(5) XOR DA(11) XOR DA(17) XOR DA(23) XOR DA(29) XOR DA(35) XOR DA(41) XOR DA(47);
*/
	bin_index = 0;
	for (i=0;i<6;i++)
	{
		int bit = 0;
		for (j=0;j<8;j++)
		{
			int bitnum = i+j*6;
			bit ^= ( da[5-bitnum/8] >> (bitnum%8) ) & 1;
		}
		bin_index |= (bit<<i);
	}

	return bin_index;
}

static err_t c674emac_igmp_mac_filter(struct netif *netif, ip_addr_t *group, u8_t action)
{
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;

	u8_t multicast_mac[6];
	multicast_mac[0] = 0x01;
	multicast_mac[1] = 0x00;
	multicast_mac[2] = 0x5E;
	multicast_mac[3] = ip4_addr2(group) & 0x7F;
	multicast_mac[4] = ip4_addr3(group);
	multicast_mac[5] = ip4_addr4(group);

	int bin_index = c674emac_get_mac_hash_bin(multicast_mac);
	
	if (action == IGMP_ADD_MAC_FILTER)
	{
		if (c674emac_hash_bin_count[bin_index]++ == 0)
		{
			if (bin_index < 32)	EMAC_Regs->MACHASH1 |= 1 << bin_index;
			else				EMAC_Regs->MACHASH2 |= 1 << (bin_index-32);
		}
		return ERR_OK;
	}
	else if (action == IGMP_DEL_MAC_FILTER)
	{
		if (--c674emac_hash_bin_count[bin_index] == 0)
		{
			if (bin_index < 32)	EMAC_Regs->MACHASH1 &= ~(1 << bin_index);
			else				EMAC_Regs->MACHASH2 &= ~(1 << (bin_index-32));
		}
		return ERR_OK;
	}
	
	return ERR_IF;
}

err_t c674emac_init(struct netif *netif)
{
	int i;

	/* LwIP network interface level */
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->output = etharp_output;
	netif->linkoutput = c674emac_low_level_output;
	netif->igmp_mac_filter = c674emac_igmp_mac_filter;
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->mtu = 1500;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

	/* Initialize EMAC Control Module */
	CSL_EctlRegsOvly EMAC_Ctrl = (CSL_EctlRegsOvly) CSL_ECTL_0_REGS;
	EMAC_Ctrl->SOFTRESET = 1;
	EMAC_Ctrl->SOFTRESET = 0;

	/* Initialize EMAC Module */
	CSL_EmacRegsOvly EMAC_Regs = (CSL_EmacRegsOvly) CSL_EMAC_DSC_CONTROL_REG;
	EMAC_Regs->SOFTRESET = 1;
	while (EMAC_Regs->SOFTRESET & 1);

	/* PHY */
	c674phy_init(netif);

	/* MAC */
	EMAC_Regs->MACCONTROL =	CSL_FMKT(EMAC_MACCONTROL_FULLDUPLEX, FULLDUPLEX)
						|	CSL_FMKT(EMAC_MACCONTROL_RMIISPEED, 100MBIT)
						|	CSL_FMKT(EMAC_MACCONTROL_TXFLOWEN, DISABLE)
						|	CSL_FMKT(EMAC_MACCONTROL_RXBUFFERFLOWEN, DISABLE);


	/* Receive Configuration */
	EMAC_Regs->RXMBPENABLE = CSL_FMKT(EMAC_RXMBPENABLE_RXMULTEN, ENABLE)	// Multicast Frames Accepted
						|	 CSL_FMK(EMAC_RXMBPENABLE_RXMULTCH, 0)			// Channel 0 Receive Multicast Frames
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXBROADEN, ENABLE)	// Broadcast Frames Accepted
						|	 CSL_FMK(EMAC_RXMBPENABLE_RXBROADCH, 0)			// Channel 0 Receive Broadcast Frames
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXCAFEN, DISABLE)	// Unknown Address Frames Dropped
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXCEFEN, DISABLE)	// Erroneous Frames Dropped
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXCSFEN, DISABLE)	// Short Frames Dropped
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXCMFEN, DISABLE)	// Mac Control Frames Dropped
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXNOCHAIN, DISABLE)	// RX frames can span multiple buffers
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXQOSEN, DISABLE)	// QOS Disabled
						|	 CSL_FMKT(EMAC_RXMBPENABLE_RXPASSCRC, DISCARD);	// Frame's CRC Dropped

	/* Start without Multicast */
	EMAC_Regs->MACHASH1 = 0;
	EMAC_Regs->MACHASH2 = 0;

	/* Unicast Configuration */
	EMAC_Regs->RXUNICASTCLEAR = 0xff;			// Clear All Unicast Channel
	EMAC_Regs->RXUNICASTSET = 1;				// Enable Unicast Channel 0

	EMAC_Regs->RXMAXLEN = 1518;					// 1500 bytes + 18 bytes ethernet encapsulation
	EMAC_Regs->RXBUFFEROFFSET = BUFFER_OFFSET;	// Only for SOP Frame

	/* Enable Rx & Tx Interrupts for Channel 1 */
	EMAC_Regs->TXINTMASKCLEAR = 0xffff;
	EMAC_Regs->TXINTMASKSET = CSL_FMK(EMAC_TXINTMASKSET_TX0MASK, 1);

	EMAC_Regs->RXINTMASKCLEAR = 0xffff;
	EMAC_Regs->RXINTMASKSET = CSL_FMK(EMAC_RXINTMASKSET_RX0MASK, 1);

	/* No Statistic or Host Error Interrupts */
	EMAC_Regs->MACINTMASKCLEAR = CSL_FMK(EMAC_MACINTMASKCLEAR_HOSTMASK, 1)
							|	 CSL_FMK(EMAC_MACINTMASKCLEAR_STATMASK, 1);

	/* MAC Address */
	EMAC_Regs->MACADDRHI =	CSL_FMK(EMAC_MACADDRHI_MACADDR2, netif->hwaddr[3])
						|	CSL_FMK(EMAC_MACADDRHI_MACADDR3, netif->hwaddr[2])
						|	CSL_FMK(EMAC_MACADDRHI_MACADDR4, netif->hwaddr[1])
						|	CSL_FMK(EMAC_MACADDRHI_MACADDR5, netif->hwaddr[0]);

	for (i=0; i<8; i++)
	{
		EMAC_Regs->MACINDEX = i;
		EMAC_Regs->MACADDRLO =	CSL_FMKT(EMAC_MACADDRLO_VALID, VALID)
							|	CSL_FMKT(EMAC_MACADDRLO_MATCHFILT, MATCH)
							|	CSL_FMK(EMAC_MACADDRLO_CHANNEL, i)
							|	CSL_FMK(EMAC_MACADDRLO_MACADDR0, netif->hwaddr[5])
							|	CSL_FMK(EMAC_MACADDRLO_MACADDR1, netif->hwaddr[4]);
	}

	/* Head Descriptor Pointers */
	EMAC_Regs->TX0HDP = 0;
	EMAC_Regs->TX1HDP = 0;
	EMAC_Regs->TX2HDP = 0;
	EMAC_Regs->TX3HDP = 0;
	EMAC_Regs->TX4HDP = 0;
	EMAC_Regs->TX5HDP = 0;
	EMAC_Regs->TX6HDP = 0;
	EMAC_Regs->TX7HDP = 0;

	EMAC_Regs->RX0HDP = 0;
	EMAC_Regs->RX1HDP = 0;
	EMAC_Regs->RX2HDP = 0;
	EMAC_Regs->RX3HDP = 0;
	EMAC_Regs->RX4HDP = 0;
	EMAC_Regs->RX5HDP = 0;
	EMAC_Regs->RX6HDP = 0;
	EMAC_Regs->RX7HDP = 0;

	/* Emulation Mode */
	EMAC_Regs->EMCONTROL = 0;

	/* Clear CPPI RAM */
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;
	memset(cppiram, 0, sizeof(struct cppiram_t));

	/* Static messages for interrupt callbacks */
	#if !NO_SYS
	cppiram->txmsg = tcpip_callbackmsg_new(c674emac_tx_callback, netif);
	cppiram->rxmsg = tcpip_callbackmsg_new(c674emac_rx_callback, netif);
	#endif

	/* Prepare Descriptors */
	for (i=0; i<TX_DESCRIPTORS; i++)
	{
		c674emac_push_desc(&cppiram->txfree, &cppiram->txdesc[i]);
	}

	for (i=0; i<RX_DESCRIPTORS; i++)
	{
		struct pbuf* p = pbuf_alloc(PBUF_RAW, 1520, PBUF_POOL);
		cppiram->rxdesc[i].pBuffer = (Uint8*) p;
		cppiram->rxdesc[i].BufOffLen = 1536;
		cppiram->rxdesc[i].PktFlgLen = EMAC_DSC_FLAG_OWNER | 0;
		L2CACHE_WBINV(p, 1536);

		c674emac_push_desc(&cppiram->rxactive, &cppiram->rxdesc[i]);
	}

	/* Start RX DMA */
	EMAC_Regs->RX0HDP = (Uint32) cppiram->rxactive.first;

	/* EMAC Interrupts */
	EMAC_Ctrl->INTCONTROL = 0;
	EMAC_Ctrl->C0RXTHRESHEN = 0;
	EMAC_Ctrl->C1RXTHRESHEN = 0;
	EMAC_Ctrl->C0RXEN = CSL_FMKT(ECTL_C0RXEN_RXCH0EN, ENABLE);
	EMAC_Ctrl->C1RXEN = 0;
	EMAC_Ctrl->C0TXEN = CSL_FMKT(ECTL_C0TXEN_TXCH0EN, ENABLE);
	EMAC_Ctrl->C1TXEN = 0;
	EMAC_Ctrl->C0RXTHRESHSTAT = 0;
	EMAC_Ctrl->C1RXTHRESHSTAT = 0;

#if NO_SYS
	CSL_DspintcRegsOvly DspInt_Regs = (CSL_DspintcRegsOvly) CSL_INTC_0_REGS;
	CSL_FINS(DspInt_Regs->INTMUX2, DSPINTC_INTMUX2_INTSEL9, CSL_INTC_EVENTID_EMAC_RXC0);
	CSL_FINS(DspInt_Regs->INTMUX2, DSPINTC_INTMUX2_INTSEL10, CSL_INTC_EVENTID_EMAC_TXC0);
	CSL_FINST(IER, CHIP_IER_IE09, ENABLE);
	CSL_FINST(IER, CHIP_IER_IE10, ENABLE);
#else
	Hwi_eventMap(9, CSL_INTC_EVENTID_EMAC_RXC0);
 	Hwi_Handle rxisr = Hwi_create(9, (Hwi_FuncPtr) c674emac_rx_interrupt, NULL, NULL);
 	Hwi_eventMap(10, CSL_INTC_EVENTID_EMAC_TXC0);
 	Hwi_Handle txisr = Hwi_create(10, (Hwi_FuncPtr) c674emac_tx_interrupt, NULL, NULL);
#endif

	/* Enable MAC */
	EMAC_Regs->TXCONTROL = CSL_FMKT(EMAC_TXCONTROL_TXEN, ENABLE);
	EMAC_Regs->RXCONTROL = CSL_FMKT(EMAC_RXCONTROL_RXEN, ENABLE);
	CSL_FINST(EMAC_Regs->MACCONTROL, EMAC_MACCONTROL_GMIIEN, ENABLE);

	return ERR_OK;
}

#if NO_SYS
void c674emac_poll(struct netif* netif)
{
	struct cppiram_t* cppiram = (struct cppiram_t*) CSL_EMAC_DSC_CTRL_MOD_RAM;
	if (cppiram->rxevent)		c674emac_rx_callback(netif);
	if (cppiram->txevent)		c674emac_tx_callback(netif);
}
#endif
