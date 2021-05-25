#include "FreeRTOS.h"
#include "ameba_soc.h" 
#include "spdio_api.h"
#include "rtl8721d_inic.h"

/** @addtogroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup MBED_SPDIO
 *  @brief      MBED_SPDIO driver modules.
 *  @{
 */

/** @defgroup MBED_SPDIO_Exported_Types MBED_SPDIO Exported Types
  * @{
  */

#define SPDIO_IRQ_PRIORITY							5
#define SPDIO_TX_BUF_SZ_UNIT		64
#define RX_BD_FREE_TH		5
#define MIN_RX_BD_SEND_PKT	2
#define MAX_RX_BD_BUF_SIZE			               16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

typedef struct {
	u32	Address;    /* The TX buffer physical address, it must be 4-bytes aligned */
}SPDIO_TX_BD;

/* The RX Buffer Descriptor format */
typedef struct {
	u32 BuffSize:14;		/* bit[13:0], RX Buffer Size, Maximum 16384-1 */
	u32 LS:1;					/* bit[14], is the Last Segment ? */
	u32 FS:1;					/* bit[15], is the First Segment ? */
	u32 Seq:16;				/* bit[31:16], The sequence number, it's no use for now */
	u32 PhyAddr;			/* The RX buffer physical address, it must be 4-bytes aligned */
} SPDIO_RX_BD;

/* the data structer to bind a TX_BD with a TX Packet */
typedef struct {
	SPDIO_TX_BD *pTXBD;		// Point to the TX_BD buffer
	VOID *priv;
	u8 isPktEnd;		// For a packet over 1 BD , this flag to indicate is this BD contains a packet end
	u8 isFree;		// is this TX BD free 
} SPDIO_TX_BD_HANDLE;

/* the data structer to bind a RX_BD with a RX Packet */
typedef struct {
	VOID	*priv;
	SPDIO_RX_BD *pRXBD;		// Point to the RX_BD buffer
	INIC_RX_DESC *pRXDESC;	// point to the Rx Packet
	u8 isPktEnd;		// For a packet over 1 BD , this flag to indicate is this BD contains a packet end
	u8 isFree;		// is this RX BD free (DMA done and its RX packet has been freed)
} SPDIO_RX_BD_HANDLE;

typedef struct {
	VOID 			*spdio_priv;		/*Data from User*/	
	u8				*pTXBDAddr;			/* The TX_BD start address */
	SPDIO_TX_BD		*pTXBDAddrAligned;	/* The TX_BD start address, it must be 4-bytes aligned */

	SPDIO_TX_BD_HANDLE*	pTXBDHdl;		/* point to the allocated memory for TX_BD Handle array */
	u16				TXBDWPtr;		    /* The SDIO TX(Host->Device) BD local write index, different with HW maintained write Index. */
	u16				TXBDRPtr;		    /* The SDIO TX(Host->Device) BD read index */
	u16				TXBDRPtrReg;		/* The SDIO TX(Host->Device) BD read index has been write to HW register */
	u8				TxOverFlow;

	u8				*pRXBDAddr;			/* The RX_BD start address */
	SPDIO_RX_BD		*pRXBDAddrAligned;	/* The RX_BD start address, it must be 8-bytes aligned */

	u8				*pRXDESCAddr;
	INIC_RX_DESC	*pRXDESCAddrAligned;
	SPDIO_RX_BD_HANDLE*	pRXBDHdl;		/* point to the allocated memory for RX_BD Handle array */
	u16				RXBDWPtr;		    /* The SDIO RX(Device->Host) BD write index */
	u16				RXBDRPtr;		    /* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */

	_sema		IrqSema;             /* Semaphore for SDIO RX, use to wakeup the SDIO RX task */
	xTaskHandle	xSDIOIrqTaskHandle;  /* The handle of the SDIO Task speical for RX, can be used to delte the task */
} HAL_SPDIO_ADAPTER, *PHAL_SPDIO_ADAPTER;

struct spdio_t *g_spdio_priv = NULL;
HAL_SPDIO_ADAPTER gSPDIODev;
PHAL_SPDIO_ADAPTER pgSPDIODev = NULL;
/** 
  * @}
  */

/** @defgroup MBED_SPDIO_Exported_Functions MBED_SPDIO Exported Functions
  * @{
  */

s8 spdio_rx_done_cb(void *padapter, void *data, u16 offset, u16 pktsize, u8 type)
{
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	struct spdio_t *obj = (struct spdio_t *)padapter;
	if(obj)
		return obj->rx_done_cb(obj, buf, (u8 *)(buf->buf_addr+offset), pktsize, type);
	else
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "spdio rx done callback function is null!");
	return SUCCESS;
}

s8 spdio_tx_done_cb(void *padapter, IN u8 *data)
{
	struct spdio_t *obj = (struct spdio_t *)padapter;
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	if(obj)
		return obj->tx_done_cb(obj, buf);
	else
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "spdio tx done callback function is null!");
	return SUCCESS;		
}

s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf)
{
	PHAL_SPDIO_ADAPTER pgSDIODev = obj->priv;
	INIC_RX_DESC *pRxDesc;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 Offset=0;
	u16 RxBdWrite=0;	// to count how much RX_BD used in a Transaction
	u16 RxBdRdPtr= pgSDIODev->RXBDRPtr;	// RX_BD read pointer
	u32 pkt_size;
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	u8 needed_rxbd_num;
#endif
	/* check if RX_BD available */
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	needed_rxbd_num = ((pbuf->buf_size - 1)/MAX_RX_BD_BUF_SIZE) + MIN_RX_BD_SEND_PKT;
#endif
	if (RxBdRdPtr != pgSDIODev->RXBDWPtr) {
		if (pgSDIODev->RXBDWPtr > RxBdRdPtr) {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->tx_bd_num - needed_rxbd_num)) 
#else
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->tx_bd_num - (u32)MIN_RX_BD_SEND_PKT)) 
#endif					
			{
				DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", \
				RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return _FALSE;
			}
		} else {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)needed_rxbd_num)
#else
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)MIN_RX_BD_SEND_PKT)
#endif
			{
				DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return _FALSE;
			}
		}
	}

	// TODO: Add RX_DESC before the packet

	/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc, 
	   other RX_BDs are for packet payload */
	/* Use a RX_BD to transmit RX_Desc */		
	pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr;	// get the RX_BD head
	pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;

	pRxDesc = pRxBdHdl->pRXDESC;
	pRxDesc->type = pbuf->type;
	pRxDesc->pkt_len = pbuf->buf_size;
	pRxDesc->offset = sizeof(INIC_RX_DESC);

	if (!pRxBdHdl->isFree) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Return_Rx_Data: Allocated a non-free RX_BD\n");
	}
	pRxBdHdl->isFree = 0;
	pRXBD->FS = 1;
	pRXBD->LS = 0;
	pRXBD->PhyAddr = (u32)((u8 *)pRxBdHdl->pRXDESC);
	pRXBD->BuffSize = sizeof(INIC_RX_DESC);
	pRxBdHdl->isPktEnd = 0;
	
	pgSDIODev->RXBDWPtr += 1;
	if (pgSDIODev->RXBDWPtr >= obj->tx_bd_num) {
		pgSDIODev->RXBDWPtr -= obj->tx_bd_num;
	}

	RxBdWrite++;

	/* Take RX_BD to transmit packet payload */
	pkt_size = pbuf->buf_size;
	Offset = 0;
	do {
		pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr;	// get the RX_BD head
		pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;
		pRxBdHdl->isFree = 0;
		pRXBD->FS = 0;
		pRXBD->PhyAddr = (u32)((u8 *)pbuf->buf_addr + Offset);
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		if ((pkt_size - Offset) <= MAX_RX_BD_BUF_SIZE)	{
			pRXBD->BuffSize = pkt_size - Offset;
			pRxBdHdl->isPktEnd = 1;
		}else {
			pRXBD->BuffSize = MAX_RX_BD_BUF_SIZE;
			pRxBdHdl->isPktEnd = 0;
			DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO_Return_Rx_Data: Split RX_BD, Offset=%d PktSize=%d\n", \
				Offset, pkt_size);
		}
#else
		if (pkt_size > MAX_RX_BD_BUF_SIZE)	{
			// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
			DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Return_Rx_Data: The Packet Size bigger than 16K\n");
			pkt_size = MAX_RX_BD_BUF_SIZE;
		}
		pRXBD->BuffSize = pkt_size;
		pRxBdHdl->priv = (void*)pbuf;
		pRxBdHdl->isPktEnd = 1;
#endif
		Offset += pRXBD->BuffSize;
		// Move the RX_BD Write pointer forward
		RxBdWrite++;
		pgSDIODev->RXBDWPtr += 1;
		if (pgSDIODev->RXBDWPtr >= obj->tx_bd_num) {
			pgSDIODev->RXBDWPtr -= obj->tx_bd_num;
		}

		if (Offset >= pkt_size) {
			pRXBD->LS = 1;
		}
	} while (Offset < pkt_size);


	if (RxBdWrite > 0) {
		SDIO_RXBD_WPTR_Set(pgSDIODev->RXBDWPtr);
		HAL_SDIO_WRITE8(REG_SPDIO_HCI_RX_REQ, BIT_HCI_RX_REQ);
	}

	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO_Return_Rx_Data(%d)<==\n", RxBdWrite);
	return _TRUE;
}

void spdio_structinit(struct spdio_t *obj){
	obj->rx_bd_bufsz = SPDIO_RX_BUFSZ_ALIGN(2048+24); //extra 24 bytes for sdio header
	obj->rx_bd_num = 24;
	obj->tx_bd_num = 24;
	obj->priv = NULL;
	obj->rx_buf = NULL;
	obj->rx_done_cb = NULL;
	obj->tx_done_cb = NULL;
}


/******************************************************************************
 * Function: SDIO_TX_FIFO_DataReady
 * Desc: Handle the SDIO FIFO data ready interrupt.
 *		1. Send those data to the target driver via callback fun., like WLan.
 *		2. Allocate a buffer for the TX BD
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SPDIO_TX_FIFO_DataReady(IN PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SPDIO_TX_BD_HANDLE* pTxBdHdl;
	PINIC_TX_DESC pTxDesc;
	volatile u16 TxBDWPtr=0;
	u8 isForceBreak=0;
	s8 ret=FAIL;
	u32 reg;
	SPDIO_TX_BD *pTXBD = NULL;	
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;

	TxBDWPtr = SDIO_TXBD_WPTR_Get();
	if (TxBDWPtr == pSPDIODev->TXBDRPtr) {
        if (unlikely(pSPDIODev->TxOverFlow != 0)) {
			pSPDIODev->TxOverFlow = 0;
			reg = HAL_SDIO_READ32(REG_SPDIO_AHB_DMA_CTRL);
			DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO TX Overflow Case: Reg DMA_CTRL==0x%x %x %x %x\n", (reg>> 24)&0xff , (reg>>16)&0xff, (reg>>8)&0xff, (reg)&0xff);	
        } else {
        	DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO TX Data Read False Triggered!!, TXBDWPtr=0x%x\n", TxBDWPtr);
			return;
		}
	}
	
	do {
		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO_TX_DataReady: TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
		pTXBD= (SPDIO_TX_BD*)(pSPDIODev->pTXBDAddrAligned + pSPDIODev->TXBDRPtr);
		pTxBdHdl = pSPDIODev->pTXBDHdl + pSPDIODev->TXBDRPtr;
		pTxDesc = (PINIC_TX_DESC)(pTXBD->Address);

		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO_TX_DataReady: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);
		
		if ((pTxDesc->txpktsize + pTxDesc->offset) <= obj->rx_bd_bufsz) {
			// use the callback function to fordward this packet to target(WLan) driver
			ret = spdio_rx_done_cb(obj, (u8*)pTxBdHdl->priv, pTxDesc->offset, pTxDesc->txpktsize, pTxDesc->type);
			if(ret == FAIL)
				DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO TX_Callback is Null!\n");
			pTXBD->Address =  obj->rx_buf[pSPDIODev->TXBDRPtr].buf_addr;
		} else {
			// Invalid packet, Just drop it
			ret = SUCCESS;  // pretend we call the TX callback OK
		}
		
		if (SUCCESS != ret) {
			// may be is caused by TX queue is full, so we skip it and try again later
			isForceBreak = 1;
			break;  // break the while loop
		} else {  
			pSPDIODev->TXBDRPtr++;
			if (pSPDIODev->TXBDRPtr >= obj->rx_bd_num) {
				pSPDIODev->TXBDRPtr = 0;
			}
			pSPDIODev->TXBDRPtrReg = pSPDIODev->TXBDRPtr;
			SDIO_TXBD_RPTR_Set(pSPDIODev->TXBDRPtrReg);
		}

		TxBDWPtr = SDIO_TXBD_WPTR_Get();
		if (isForceBreak) {
			break;	// break the TX FIFO DMA Done processing
		}
	} while (pSPDIODev->TXBDRPtr != TxBDWPtr);

	// if not all TX data were processed, set an event to trigger SDIO_Task to process them later
	if (isForceBreak) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSPDIODev->TXBDRPtr);	
	}
}

VOID SPDIO_Recycle_Rx_BD (IN PHAL_SPDIO_ADAPTER pgSPDIODev)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 FreeCnt=0;		// for debugging
	struct spdio_t *obj = (struct spdio_t *)pgSPDIODev->spdio_priv;

	SDIO_INTConfig(BIT_C2H_DMA_OK, DISABLE);
	while (SDIO_RXBD_RPTR_Get() != pgSPDIODev->RXBDRPtr) {
		pRxBdHdl = pgSPDIODev->pRXBDHdl + pgSPDIODev->RXBDRPtr;
		pRXBD = pRxBdHdl->pRXBD;
		if (!pRxBdHdl->isFree) {
			if(pRxBdHdl->isPktEnd){
				spdio_tx_done_cb(obj, (u8*)(pRxBdHdl->priv));
				
			}
			pRxBdHdl->isPktEnd = 0;
			_memset((void *)(pRxBdHdl->pRXDESC), 0, sizeof(INIC_RX_DESC));
			_memset((void *)pRXBD , 0, sizeof(SPDIO_RX_BD));	// clean this RX_BD
			pRxBdHdl->isFree = 1;
		}
		else {
			DBG_PRINTF(MODULE_SDIO, LEVEL_WARN, "SDIO_Recycle_Rx_BD: Warring, Recycle a Free RX_BD,RXBDRPtr=%d\n",pgSPDIODev->RXBDRPtr);
		}
		pgSPDIODev->RXBDRPtr++;

		if (pgSPDIODev->RXBDRPtr >= obj->tx_bd_num) {
			pgSPDIODev->RXBDRPtr -= obj->tx_bd_num;
		}
	}
	SDIO_INTConfig(BIT_C2H_DMA_OK, ENABLE);
	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "<==SDIO_Recycle_Rx_BD(%d)\n", FreeCnt);

}


/******************************************************************************
 * Function: SPDIO_IRQ_Handler
 * Desc: SPDIO device interrupt service routine
 *		1. Read & clean the interrupt status
 *		2. Wake up the SDIO task to handle the IRQ event
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
VOID SPDIO_IRQ_Handler(VOID *pData)
{
	PHAL_SPDIO_ADAPTER pSPDIODev = pData;
	InterruptDis(SDIO_DEVICE_IRQ);
	rtw_up_sema_from_isr(&pSPDIODev->IrqSema);
}

VOID SPDIO_IRQ_Handler_BH(VOID *pData)
{
	PHAL_SPDIO_ADAPTER pgSPDIODev = pData;
	u16 IntStatus;

	for (;;)		
	{
			/* Task blocked and wait the semaphore(events) here */
		rtw_down_sema(&pgSPDIODev->IrqSema);
		IntStatus = HAL_SDIO_READ16(REG_SPDIO_CPU_INT_STAS);
		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "%s:ISRStatus=0x%x\n", __FUNCTION__, IntStatus);
		HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, IntStatus);	// clean the ISR
		InterruptEn(SDIO_DEVICE_IRQ, SPDIO_IRQ_PRIORITY);
		if (IntStatus & BIT_C2H_DMA_OK) {
			SPDIO_Recycle_Rx_BD(pgSPDIODev);
		}

		if (IntStatus & BIT_H2C_MSG_INT) {
			HAL_SDIO_READ32(REG_SPDIO_CPU_H2C_MSG);
		}
		
		if (IntStatus & BIT_H2C_DMA_OK) {
			SDIO_INTConfig(BIT_H2C_DMA_OK, DISABLE);
			SPDIO_TX_FIFO_DataReady(pgSPDIODev);
			SDIO_INTConfig(BIT_H2C_DMA_OK, ENABLE);
		}
		if (IntStatus & BIT_TXFIFO_H2C_OVF) {
			pgSPDIODev->TxOverFlow = 1;
		}

		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "%s @2 IntStatus=0x%x\n", __FUNCTION__, IntStatus);
	}
#if 0
	SDIO_SetEvent(pgSPDIODev, (u32)SDIO_EVENT_IRQ_STOPPED);
	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO irq Task Stopped!\n");
#if ( INCLUDE_vTaskDelete == 1 )
	vTaskDelete(NULL);
#endif
#endif
}

/******************************************************************************
 * Function: SPDIO_Device_Init
 * Desc: SDIO mbed device driver initialization.
 *		1. Allocate SDIO TX BD and RX BD adn RX Desc.
 *		2. Allocate SDIO RX Buffer Descriptor and RX Buffer. Initial RX related
 *			register.
 *		3. Register the Interrupt function.
 *
 ******************************************************************************/
BOOL SPDIO_Device_Init(struct spdio_t * obj)
{
	u32 i;
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	int ret;
	//SPDIO_TX_BD *pTXBD = NULL;
	SDIO_InitTypeDef SDIO_InitStruct;
	u32 Temp = 0;

	if(obj == NULL){
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "struct spdio_t must be inited\n");
		return FAIL;	
	}
	
	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "SDIO_Device_Init==>\n");

	pgSPDIODev = &gSPDIODev;
	pgSPDIODev->spdio_priv = (void*)obj;	
	obj->priv = (void *)pgSPDIODev;
	
// initial TX BD and RX BD
	pgSPDIODev->pTXBDAddr = _rtw_malloc((obj->rx_bd_num* sizeof(SPDIO_TX_BD))+3);
	if (NULL == pgSPDIODev->pTXBDAddr) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc for TX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pTXBDAddrAligned = (SPDIO_TX_BD*)(((((u32)pgSPDIODev->pTXBDAddr - 1) >> 2) + 1) << 2);	// Make it 4-bytes aligned

	pgSPDIODev->pRXBDAddr = _rtw_malloc((obj->tx_bd_num * sizeof(SPDIO_RX_BD))+7);
	if (NULL == pgSPDIODev->pRXBDAddr) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc for RX_BD Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXBDAddrAligned = (SPDIO_RX_BD*)(((((u32)pgSPDIODev->pRXBDAddr - 1) >> 3) + 1) << 3);	// Make it 8-bytes aligned

	pgSPDIODev->pRXDESCAddr = _rtw_zmalloc((obj->tx_bd_num * sizeof(INIC_RX_DESC)) + 3);
	if (NULL == pgSPDIODev->pRXDESCAddr) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc for RX_DESC Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXDESCAddrAligned = (INIC_RX_DESC*)(((((u32)pgSPDIODev->pRXDESCAddr - 1) >> 2) + 1) << 2); //Make it 4-bytes aligned

	// Clean boot from wakeup bit
//	SOCPS_BootFromPS(DISABLE);

	/* SDIO Function & CLock Enable */
	RCC_PeriphClockCmd(APBPeriph_SDIOD, APBPeriph_SDIOD_CLOCK, ENABLE);

	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_SDIO_CTRL);
	Temp |= BIT_HS_SDIOD_PIN_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HS_SDIO_CTRL, Temp);

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_EFUSE_CTRL1);
	Temp |= BIT_SYS_EF_ALDEN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_EFUSE_CTRL1, Temp);
	// SDIO_SCLK / SPI_CLK pin pull-low
	//PAD_PullCtrl(_PA_3, GPIO_PuPd_DOWN);

	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)pgSPDIODev->pTXBDAddrAligned;
	SDIO_InitStruct.TXBD_RING_SIZE = obj->rx_bd_num; //SDIO_TX_BD_NUM;
	SDIO_InitStruct.TX_BUFFER_SIZE = ((((obj->rx_bd_bufsz-1)/SPDIO_TX_BUF_SZ_UNIT)+1)&0xff);
	SDIO_InitStruct.RXBD_BAR = (u32)pgSPDIODev->pRXBDAddrAligned;
	SDIO_InitStruct.RXBD_RING_SIZE = obj->tx_bd_num;
	SDIO_InitStruct.RXBD_FREE_TH = RX_BD_FREE_TH;

	SDIO_Init((&SDIO_InitStruct));

	pgSPDIODev->TXBDWPtr = SDIO_TXBD_WPTR_Get();
	pgSPDIODev->TXBDRPtr = pgSPDIODev->TXBDWPtr;
	pgSPDIODev->TXBDRPtrReg = pgSPDIODev->TXBDWPtr;

	pgSPDIODev->RXBDWPtr = pgSPDIODev->RXBDRPtr = SDIO_RXBD_RPTR_Get();
	
	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pgSPDIODev->TXBDWPtr, pgSPDIODev->TXBDRPtr);	
    
	pgSPDIODev->pTXBDHdl = (SPDIO_TX_BD_HANDLE*)_rtw_zmalloc(obj->rx_bd_num * sizeof(SPDIO_TX_BD_HANDLE));
	if (NULL == pgSPDIODev->pTXBDHdl) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc for TX_BD Handle Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	for(i=0;i<obj->rx_bd_num;i++){
		pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
		pTxBdHdl->pTXBD= pgSPDIODev->pTXBDAddrAligned + i;
		// Pre-allocate buffer by User
		pTxBdHdl->priv = (void *)&obj->rx_buf[i];
		pTxBdHdl->pTXBD->Address = (u32)obj->rx_buf[i].buf_addr;	
		if(pTxBdHdl->pTXBD->Address%4){
			DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "buffer address must be aligned to 4!!\n");	
			goto SDIO_INIT_ERR;			
		}

	        if (NULL == (u32*)(pTxBdHdl->pTXBD->Address)) {
			    DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc buffer for TX_BD Err!!\n");
			    goto SDIO_INIT_ERR;            
	        }
		pTxBdHdl->isFree = 1;
		DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "TX_BD%d @ 0x%x 0x%x\n", i, pTxBdHdl, pTxBdHdl->pTXBD);	
	}

	pgSPDIODev->pRXBDHdl = (SPDIO_RX_BD_HANDLE*)_rtw_zmalloc(obj->tx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
	if (NULL == pgSPDIODev->pRXBDHdl) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Malloc for RX_BD Handle Err!!\n");	
		goto SDIO_INIT_ERR;
	}

	for (i=0; i<obj->tx_bd_num; i++) {
		pRxBdHdl = pgSPDIODev->pRXBDHdl + i;
		pRxBdHdl->pRXBD = pgSPDIODev->pRXBDAddrAligned + i;
		pRxBdHdl->pRXDESC = pgSPDIODev->pRXDESCAddrAligned + i;
		pRxBdHdl->isFree = 1;	
	}

	rtw_init_sema(&(pgSPDIODev->IrqSema), 0);
	if (NULL == pgSPDIODev->IrqSema){
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init Create IRQ Semaphore Err!!\n");	
		goto SDIO_INIT_ERR;
	}
	
	ret = xTaskCreate( SPDIO_IRQ_Handler_BH, "SPDIO_IRQ_TASK", ((1024*1)/sizeof(portBASE_TYPE)), (void *)pgSPDIODev, 1 + PRIORITIE_OFFSET, &pgSPDIODev->xSDIOIrqTaskHandle);
	if (pdTRUE != ret )
	{
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "SDIO_Device_Init: Create IRQ Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}

	//pgSPDIODev->CRPWM = SDIO_RPWM1_Get();
	//pgSPDIODev->CRPWM2 = SDIO_RPWM2_Get();

	// Indicate Host this is a iNIC FW
	//SDIO_CPWM2_Set(CPWM2_INIC_FW_RDY_BIT, ENABLE);

		/* enable the interrupt */
	InterruptRegister((IRQ_FUN) SPDIO_IRQ_Handler, SDIO_DEVICE_IRQ, (u32) pgSPDIODev, SPDIO_IRQ_PRIORITY);
	InterruptEn(SDIO_DEVICE_IRQ, SPDIO_IRQ_PRIORITY);
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, SDIO_INIT_INT_MASK);	// Clean pending interrupt first
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_MASK, SDIO_INIT_INT_MASK);
	
	// Update the power state indication
	SDIO_CPWM2_Set(CPWM2_ACT_BIT, ENABLE);
	
	/* Indicate the Host system that the TX/RX is ready */
	HAL_SDIO_WRITE8(REG_SPDIO_CPU_IND, \
		HAL_SDIO_READ8(REG_SPDIO_CPU_IND)|BIT_SYSTEM_TRX_RDY_IND);

	DBG_PRINTF(MODULE_SDIO, LEVEL_INFO, "<==SDIO_Device_Init\n");

	return SUCCESS;
	
SDIO_INIT_ERR:

	if (pgSPDIODev->pRXBDHdl) {
		_rtw_mfree((u8 *)pgSPDIODev->pRXBDHdl, obj->tx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
		pgSPDIODev->pRXBDHdl = NULL;
	}

    if ((pgSPDIODev->pTXBDHdl)) {
        for (i=0;i<obj->rx_bd_num;i++){
            pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
            if (pTxBdHdl->pTXBD->Address) {
                pTxBdHdl->pTXBD->Address = (u32)NULL;
            }
        }
		_rtw_mfree((u8 *)pgSPDIODev->pTXBDHdl, (obj->rx_bd_num * sizeof(SPDIO_TX_BD_HANDLE)));
        pgSPDIODev->pTXBDHdl = NULL;
    }
    
	if (pgSPDIODev->pRXBDAddr) {
		_rtw_mfree((u8 *)pgSPDIODev->pRXBDAddr, (obj->tx_bd_num * sizeof(SPDIO_RX_BD))+7);
		pgSPDIODev->pRXBDAddr = NULL;
	}
	
    if (pgSPDIODev->pTXBDAddr) {
        _rtw_mfree(pgSPDIODev->pTXBDAddr, ((obj->rx_bd_num * sizeof(SPDIO_TX_BD))+3));
        pgSPDIODev->pTXBDAddr = NULL;
        pgSPDIODev->pTXBDAddrAligned = NULL;
    }    

	if (pgSPDIODev->pRXDESCAddr) {
		_rtw_mfree(pgSPDIODev->pRXDESCAddr, (((obj->rx_bd_num)* sizeof(INIC_RX_DESC)) + 3));
		pgSPDIODev->pRXDESCAddr = NULL;
		pgSPDIODev->pRXDESCAddrAligned = NULL;
	}
	return FAIL;
}




/******************************************************************************
 * Function: SPDIO_Device_DeInit
 * Desc: SDIO device driver free resource. This function should be called in 
 *			a task.
 *		1. Free TX FIFO buffer
 *
 * Para:
 *		pSDIODev: The SDIO device data structor.
 ******************************************************************************/
//TODO: Call this function in a task
VOID SPDIO_Device_DeInit(VOID)
{
	u32 i=0;
	SPDIO_TX_BD_HANDLE *pTxBdHdl; 
	SPDIO_TX_BD *pTXBD = NULL;
	struct spdio_t * obj;
		
	if (NULL == pgSPDIODev)
		return;

	obj = pgSPDIODev->spdio_priv;
	// Indicate the Host that Ameba is InActived
	SDIO_CPWM2_Set(CPWM2_ACT_BIT, DISABLE);
		
	if (pgSPDIODev->pRXBDHdl) {
		_rtw_mfree((u8 *)pgSPDIODev->pRXBDHdl, obj->tx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
		pgSPDIODev->pRXBDHdl = NULL;
	}
	
	/* Free TX FIFO Buffer */
	for (i=0;i<obj->rx_bd_num;i++)
	{
		pTXBD = (SPDIO_TX_BD*)(pgSPDIODev->pTXBDAddrAligned + i);
		pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
		if (pTXBD->Address) {
			pTXBD->Address = (u32)NULL;
		}
	}

    if ((pgSPDIODev->pTXBDHdl)) {
        for (i=0;i<obj->rx_bd_num;i++){
            pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
            if (pTxBdHdl->pTXBD->Address) {
                pTxBdHdl->pTXBD->Address = (u32)NULL;
            }
        }
		_rtw_mfree((u8 *)pgSPDIODev->pTXBDHdl, (obj->rx_bd_num * sizeof(SPDIO_TX_BD_HANDLE)));
        pgSPDIODev->pTXBDHdl = NULL;
    }
	
	if (pgSPDIODev->pRXBDAddr) {
		_rtw_mfree((u8 *)pgSPDIODev->pRXBDAddr, (obj->tx_bd_num * sizeof(SPDIO_RX_BD))+7);
		pgSPDIODev->pRXBDAddr = NULL;
	}
	
    if (pgSPDIODev->pTXBDAddr) {
        _rtw_mfree(pgSPDIODev->pTXBDAddr, ((obj->rx_bd_num * sizeof(SPDIO_TX_BD))+3));
        pgSPDIODev->pTXBDAddr = NULL;
        pgSPDIODev->pTXBDAddrAligned = NULL;
    }    

	if (pgSPDIODev->pRXDESCAddr) {
		_rtw_mfree(pgSPDIODev->pRXDESCAddr, (((obj->rx_bd_num)* sizeof(INIC_RX_DESC)) + 3));
		pgSPDIODev->pRXDESCAddr = NULL;
		pgSPDIODev->pRXDESCAddrAligned = NULL;
	}

	SDIO_INTConfig(0xffff, DISABLE);
	HAL_SDIO_WRITE16(REG_SPDIO_CPU_INT_STAS, 0xffff);	// Clean pending interrupt first
	InterruptDis(SDIO_DEVICE_IRQ);
	InterruptUnRegister(SDIO_DEVICE_IRQ);

	if (pgSPDIODev->IrqSema) {
		rtw_free_sema(&pgSPDIODev->IrqSema);
		pgSPDIODev->IrqSema = NULL;
	}

	// Reset SDIO DMA
	SDIO_DMA_Reset();

#ifdef AMEBAD_TODO
	/* SDIO_OFF Disable, SDIO will lost if SDIO_ON OFF */
	SDIOD_OFF_FCTRL(OFF);
#endif
}

void spdio_init(struct spdio_t *obj)
{
	if(obj == NULL){
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "spdio obj is NULL, spdio init failed!\n");
		return;
	}
	
	if((obj->rx_bd_num == 0) ||(obj->rx_bd_bufsz == 0) ||  (obj->rx_bd_bufsz%64)
		||(obj->tx_bd_num == 0) ||(obj->tx_bd_num%2)||(obj->rx_buf == NULL)) {
		DBG_PRINTF(MODULE_SDIO, LEVEL_ERROR, "spdio obj resource isn't correctly inited, spdio init failed!\n");
		return;
	}
	
	g_spdio_priv = obj;
	SPDIO_Device_Init(obj);
}

void spdio_deinit(struct spdio_t *obj)
{	
	if(obj == NULL){
		SPDIO_API_PRINTK("spdio obj is NULL, spdio deinit failed");
		return;
	}
	SPDIO_Device_DeInit();
	g_spdio_priv = NULL;
}
/** 
  * @}
  */
  
/** 
  * @}
  */
  
/** 
  * @}
  */