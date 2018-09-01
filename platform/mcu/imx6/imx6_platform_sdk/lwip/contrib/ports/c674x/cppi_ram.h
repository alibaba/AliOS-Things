/************************************************************************/
/**	\file	cppi_ram.h
 *	\brief	C674x built-in CPPI RAM variables
 *
 *	\date	2011/10/07
 *	\author	Pierre Audenard for LwIP
 *
 */
/************************************************************************/

#ifndef CPPI_RAM_H_
#define CPPI_RAM_H_

#include "arch/cc.h"
#include "lwipopts.h"

/* Macros */
#define RX_DESCRIPTORS			(3*PBUF_POOL_SIZE/4)
#define TX_DESCRIPTORS			256

/* Types */

/* EMAC Descriptor */
struct EMAC_Desc
{
	struct EMAC_Desc *next; 	// Pointer to next descriptor in chain
	Uint8 *pBuffer; 			// Pointer to data buffer
	Uint32 BufOffLen; 			// Buffer Offset(MSW) and Length(LSW)
	Uint32 PktFlgLen; 			// Packet Flags(MSW) and Length(LSW)
};

/* Descriptors Queue */
struct dma_queue_t
{
	struct EMAC_Desc*		first;
	struct EMAC_Desc*		last;
	int						n;
};

/* CPPI Ram variables */
struct cppiram_t
{
	struct EMAC_Desc			rxdesc[RX_DESCRIPTORS];
	struct EMAC_Desc			txdesc[TX_DESCRIPTORS];

	volatile u32_t				txevent;
	volatile u32_t				rxevent;

	#if !NO_SYS
	struct tcpip_callback_msg*	txmsg;
	struct tcpip_callback_msg*	rxmsg;
	#endif

	struct dma_queue_t			txfree;			// free descriptors
	struct dma_queue_t			txqueued;		// descriptors ready with pbuf to transmit
	struct dma_queue_t			txactive;		// active DMA chain
	struct dma_queue_t			txcomplete;		// transmitted frame, pbuf can be freed

	struct dma_queue_t			rxfree;			// free descriptors
	struct dma_queue_t			rxqueued;		// descriptors ready with pbuf(pool) pre-allocated for reception
	struct dma_queue_t			rxactive;		// active DMA chain
	struct dma_queue_t			rxcomplete;		// frame received, pbuf ready for input
};

#endif /* CPPI_RAM_H_ */
