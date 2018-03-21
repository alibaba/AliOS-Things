/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "rtl8195a_mii.h"
#include "hal_mii.h"
#include "osdep_api.h"
//#include "hal_gdma.h"



volatile u8 *pTxDataBuf = NULL;
volatile u8 *pRxDataBuf = NULL;
volatile u32 ethernet_unplug = 0;

#if 0 
volatile TX_DESC_FMT TxDesc[MII_TX_DESC_NO];
volatile RX_DESC_FMT RxDesc[MII_RX_DESC_NO];
volatile u32 TxDescIdx = 0;
volatile u32 TxBufIdx = 0;
volatile u32 RxDescIdx = 0;
volatile u32 RxBufIdx = 0;
volatile u8 isTxDone = 0;
volatile u8 isRxDone = 0;
volatile u32 CurrDataLen = 0;
volatile u32 ReadDescIdx = 0;
volatile u32 ReadBufIdx = 0;
volatile u32 CurrSentSize = 0;
volatile u32 CurrReceivedSize = 0;
#endif

// by jimmy 

volatile PTX_DESC_FMT TxDesc;
volatile PRX_DESC_FMT RxDesc;

volatile u8 TxDescWrPtr = 0;
volatile u8 RxDescRdPtr = 0;

volatile u32 CurrDataLen = 0;
#if 0
HAL_GDMA_OBJ gdmaObj;
u8 isGdmaAllocated;
volatile u8 isDmaDone;
#endif
extern HAL_ETHER_ADAPTER HalEtherAdp;



VOID Mii_ISR(
	IN VOID *Data
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER) Data;
    volatile u32 RegValue = HAL_MII_READ32(REG_MII_ISRIMR);

	
	if((RegValue & ISR_RXOK) && ((pEthAdapter->InterruptMask) & IMR_RXOK))
	{	
		/* Disable interrupt and enbale again when the rx descriptor ring is empty */
		pEthAdapter->InterruptMask &= (~IMR_RXOK);

		if(pEthAdapter->CallBack != NULL)
		{
			pEthAdapter->CallBack(ETH_RXDONE, 0);  // receive size is meanless here
		}
	}

	if((RegValue & ISR_RER_OVF) && ((pEthAdapter->InterruptMask) & IMR_RER_OVF))
	{
		DBG_MII_WARN("ISR_RER_OVF 0x%08X \n", RegValue);

		/* Ignore this interrupt, rx thread will clean the OWN bit */
		pEthAdapter->InterruptMask &= (~ISR_RER_OVF);
	}

	if((RegValue & ISR_TXOK) && ((pEthAdapter->InterruptMask) & IMR_TXOK))
	{
		/* Disable TxOK interrupt */
		pEthAdapter->InterruptMask &= (~IMR_TXOK);
		/* User callback function */
		if(pEthAdapter->CallBack != NULL)
		{
			pEthAdapter->CallBack(ETH_TXDONE, 0);
		}
	}

	if((RegValue & ISR_LINKCHG) && ((pEthAdapter->InterruptMask) & IMR_LINKCHG))
	{
		if(pEthAdapter->CallBack != NULL)
		{
			if(HAL_MII_READ32(REG_MII_MS) & MS_LINKB)
			{
				pEthAdapter->CallBack(ETH_LINKDOWN, 0);
			}
			else
			{
				pEthAdapter->CallBack(ETH_LINKUP, 0);
			}
		}
	}

	HAL_MII_WRITE32(REG_MII_ISRIMR, (pEthAdapter->InterruptMask) | ISR_CLR_ALL);
}


u16
HalMiiRwPhyRegRtl8195a(u8 Operation, u8 Address, u16 Data)
{
	u32 tmp;


	tmp = (Operation << 31) | ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((Address << 16) & MIIA_PHY_REG_ADDR_MASK) | Data;
	HAL_MII_WRITE32(REG_MII_MIIA, tmp);
	HalDelayUs(1000);

	if(Operation)
	{
		/* Wait for write operation is completed */
		do {
		}while(HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG);

		return 0;
	}
	else
	{
		/* Wait for read operation is completed */
		do {
		}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);

		return (u16)(HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF);
	}
}


VOID
HalMiiInitIrqRtl8195a(
	IN VOID *Data
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER) Data;
	PIRQ_HANDLE pMiiIrqHandle = &(pEthAdapter->IrqHandle);


	if(pEthAdapter == NULL)
		DBG_MII_ERR("pEthAdapter is NULL !!\n");

	pMiiIrqHandle->Data  = (u32) pEthAdapter;
	pMiiIrqHandle->IrqNum	 = GMAC_IRQ;
	pMiiIrqHandle->IrqFun	 = (IRQ_FUN) Mii_ISR;
	pMiiIrqHandle->Priority = 6;
	InterruptRegister(pMiiIrqHandle);
	InterruptEn(pMiiIrqHandle);
}


static VOID
HalMiiDeInitIrqRtl8195a(
	IN VOID *Data
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER) Data;
	IRQ_HANDLE	MiiIrqHandle;


	if(pEthAdapter == NULL)
		DBG_8195A("pEthAdapter is NULL !!\n");

	/* Clear all interrupt status */
	HAL_MII_WRITE32(REG_MII_ISRIMR, ISR_CLR_ALL);

	MiiIrqHandle.Data	 = (u32) pEthAdapter;
	MiiIrqHandle.IrqNum  = GMAC_IRQ;
	MiiIrqHandle.IrqFun  = (IRQ_FUN) Mii_ISR;
	MiiIrqHandle.Priority = 6;
	InterruptDis(&MiiIrqHandle);
	InterruptUnRegister(&MiiIrqHandle);
}

#if 0
void Mii_DmaDone_ISR(u32 id)
{
	isDmaDone = 1;
}
#endif

s32
HalMiiInitRtl8195a(
	IN VOID
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER)&HalEtherAdp;
	u32 regVal = 0;
	u8 macAddr[6];
	u32 i = 0;
	u16 ret;
	

	if((!(pEthAdapter->tx_desc_num)) || (!(pEthAdapter->rx_desc_num)))
	{
		DBG_MII_ERR("Invalid Tx/Rx descriptor number !!\n");
		return -1;
	}
	DBG_MII_INFO("Tx/Rx: %d/%d\n", pEthAdapter->tx_desc_num, pEthAdapter->rx_desc_num);

	regVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PINMUX_CTRL);
	regVal &= ~(BIT_EEPROM_PIN_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PINMUX_CTRL, regVal);

	regVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_PINMUX_CTRL);
	regVal &= ~(BIT_SIC_PIN_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_PINMUX_CTRL, regVal);

	SPI_FLASH_PIN_FCTRL(OFF);

	ACTCK_GPIO_CCTRL(ON);
	SLPCK_GPIO_CCTRL(ON);
	HAL_PERI_ON_WRITE32(REG_GPIO_SHTDN_CTRL, 0x7FF);
	GPIO_FCTRL(ON);

	/* 1. enable MII Pinmux & disable SDIO Host/Device mode Pinmux */
	regVal = HAL_READ32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL);
	regVal |= BIT24;
	regVal &= ~(BIT0 | BIT1); // Important!
	HAL_WRITE32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL, regVal);

	/* 2. enable MII IP block (214, 12) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN);
	regVal |= BIT12;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN, regVal);

	/* 3. Lexra2AHB Function Enable (304, 11) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
	regVal |= BIT11;
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, regVal);

	/* 4. enable MII bus clock (240, 24|25) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0);
	regVal |= (BIT24 | BIT25);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0, regVal);

	/* 5. */
	regVal = HAL_READ32(SYSTEM_CTRL_BASE, 0x74) & 0xFFFFC7FF;
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x74, (regVal | 0x00003000));

	/* 6. AHB mux: select MII (214, 13) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN);
	regVal |= BIT13;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN, regVal);

	/* 7. Vendor Register Clock Enable (230, 6|7) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_CTRL);
	regVal |= (BIT6 | BIT7);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_CLK_CTRL, regVal);

	/* 8. Enable GMAC Lexra Timeout (090, 16|17|18) */
	regVal = HAL_READ32(VENDOR_REG_BASE, 0x0090);
	regVal |= (BIT16 | BIT17 | BIT18);
	HAL_WRITE32(VENDOR_REG_BASE, 0x0090, regVal);

	/* 9. Endian Swap Control (304, 12|13) */
	regVal = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
	regVal |= (BIT12 | BIT13);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, regVal);

	HAL_MII_WRITE32(REG_MII_COM, (HAL_MII_READ32(REG_MII_COM) | COM_RST));

	/* Tx settings */
	HAL_MII_WRITE32(REG_MII_TC, ((TC_IFG_TIME << 10) & TC_IFG_MASK) | ((TC_NORMAL_MODE << 8) & TC_LBK_MASK));  // Normal mode

	/* Rx settings */
	HalMiiGetMacAddressRtl8195a(macAddr);
	HAL_MII_WRITE32(REG_MII_IDR0, (macAddr[0] << 24) | (macAddr[1] << 16) | (macAddr[2] << 8) | macAddr[3]);
	HAL_MII_WRITE32(REG_MII_IDR4, (macAddr[4] << 24) | (macAddr[5] << 16));

	HAL_MII_WRITE32(REG_MII_RC, RC_AER | RC_AR | RC_AB | RC_AM | RC_APM | RC_AAP);	// Accept error/runt/broadcast/multicast, etc.
	HAL_MII_WRITE32(REG_MII_COM, (HAL_MII_READ32(REG_MII_COM) | COM_RXJUMBO));	// Support jumbo packet receiving
	HAL_MII_WRITE32(REG_MII_ETNRXCPU1, 0x01010100);

	TxDesc = (PTX_DESC_FMT)(pEthAdapter->TxDescAddr);
	RxDesc = (PRX_DESC_FMT)(pEthAdapter->RxDescAddr);
	if((TxDesc == NULL) || (RxDesc == NULL))
	{
		DBG_MII_ERR("Invalid Tx/Rx descriptor address !!\n");
		return -1;
	}

	HAL_MII_WRITE32(REG_MII_TXFDP1, (u32)TxDesc);
	HAL_MII_WRITE32(REG_MII_RXFDP1, (u32)RxDesc);
	HAL_MII_WRITE32(REG_MII_IOCMD1, ((IOCMD1_DSC_FMT_EXTRA << 28) & IOCMD1_DSCFMTEXTRA_MASK) |
					IOCMD1_EN_1GB);  // Extra desc. format = 011, support 1GB addressing
	HAL_MII_WRITE32(REG_MII_IOCMD, IOCMD_SHORT_DES_FMT |
					((IOCMD_TXFTH_256 << 19) & IOCMD_TXFTH_MASK) |
					((IOCMD_RXFTH_256 << 11) & IOCMD_RXFTH_MASK) |
					IOCMD_TE);	// short desc. format = 1, Tx & Rx FIFO threshold = 256 bytes, enable MII Tx

	/* Reset PHY */
	ret = HalMiiRwPhyRegRtl8195a(1, 0, PHY_SW_RESET);
	while(1)
	{
		/* Check the reset status */
		ret = HalMiiRwPhyRegRtl8195a(0, 0, 0);
		if(!(ret & PHY_SW_RESET))
			break;
	}
	HalDelayUs(2500000);
	
	HalMiiInitIrqRtl8195a((VOID*)(&HalEtherAdp));
	HAL_WRITE32(0xE000E100, 0x0, 0x00040000);  // enable external interrupt[18] (GMAC)
	/* ISR & IMR */
	HalEtherAdp.InterruptMask = IMR_LINKCHG | IMR_TXOK | IMR_RER_OVF | IMR_RXOK | ISR_CLR_ALL;
	HAL_MII_WRITE32(REG_MII_ISRIMR, HalEtherAdp.InterruptMask);

	if(HalMiiGetLinkStatusRtl8195a()) {
		/* Auto-Negotiation */
		HalMiiForceLinkRtl8195a(-1, -1);
	}
	
	#if 0
	/* Init GDMA */
	gdmaObj.GdmaIrqHandle.IrqFun = (IRQ_FUN)Mii_DmaDone_ISR;
	gdmaObj.GdmaIrqHandle.Data = (u32)(&gdmaObj);
	isGdmaAllocated = HalGdmaMemCpyInit(&gdmaObj);
	#endif

	pTxDataBuf = pEthAdapter->pTxPktBuf;
	pRxDataBuf = pEthAdapter->pRxPktBuf;
	if((pTxDataBuf == NULL) || (pRxDataBuf == NULL))
	{
		DBG_MII_ERR("Invalid Tx/Rx packet buffer address !!\n");
		return -1;
	}

	/* Fill Tx descriptors */
	for(i = 0; i < (pEthAdapter->tx_desc_num); i++)
	{
		if(i == ((pEthAdapter->tx_desc_num) - 1))
			TxDesc[i].dw1 = TX_DESC_EOR;
		TxDesc[i].addr = (u32)(pTxDataBuf + (i * MII_BUF_SIZE));
		TxDesc[i].dw2 = ((TX_DESC_VLAN_REMOVE << 25) & TX_DESC_VLAN_ACT_MASK) | (C_VLAN_HDR & TX_DESC_VLAN_TAG_MASK);
		TxDesc[i].dw3 = 0;
		TxDesc[i].dw4 = 0;
	}
	/* Fill Rx descriptors */
	for(i = 0; i < (pEthAdapter->rx_desc_num); i++)
	{	
		if(i == ((pEthAdapter->rx_desc_num) - 1))
			RxDesc[i].dw1 = RX_DESC_OWN | RX_DESC_EOR | MII_BUF_SIZE;
		else
			RxDesc[i].dw1 = RX_DESC_OWN | MII_BUF_SIZE;
		RxDesc[i].addr = (u32)(pRxDataBuf + (i * MII_BUF_SIZE));
		RxDesc[i].dw2 = 0;
		RxDesc[i].dw3 = 0;
	}

	#if 0
	// Reset flag
	TxDescIdx = 0;
	TxBufIdx = 0;
	isTxDone = 0;
	CurrDataLen = 0;
	CurrSentSize = 0;

	RxDescIdx = 0;
	RxBufIdx = 0;
	isRxDone = 0;

	CurrReceivedSize = 0;
	ReadBufIdx = 0;
	#endif
	HAL_MII_WRITE32(REG_MII_IOCMD, (HAL_MII_READ32(REG_MII_IOCMD) | IOCMD_RE));  // enable MII Rx
	HalDelayUs(1000);

	DBG_8195A("Initialization is finished...\n");

	return 0;
}


VOID
HalMiiDeInitRtl8195a(
	IN VOID
)
{
	#if 0
	if (isGdmaAllocated)
		HalGdmaMemCpyDeInit(&gdmaObj);
	#endif
	HalMiiDeInitIrqRtl8195a((VOID*)(&HalEtherAdp));
	HAL_WRITE32(0xE000E100, 0x0, (HAL_READ32(0xE000E100, 0x0) & (~BIT18)));  // disable external interrupt[18] (GMAC)

	ACTCK_MII_MPHY_CCTRL(OFF);
	SLPCK_MII_MPHY_CCTRL(OFF);
	PinCtrl(MII, 0, OFF);
	MII_FCTRL(OFF);
}


s32
HalMiiWriteDataRtl8195a(
	IN const char *Data,
	IN u32 Size
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER)&HalEtherAdp ;


	if((Data == NULL) || (Size == 0) || (Size > MAX_FRAME_SIZE))
	{
		DBG_MII_ERR("Invalid parameter !!\n");
		return -1;
	}

	/* Check if current Tx descriptor is available */
	u8 tx_serach_idx = TxDescWrPtr;
	if((((volatile u32)TxDesc[tx_serach_idx].dw1) & TX_DESC_OWN) == 0)
	{
		#if 1
		_memcpy((void*)(TxDesc[tx_serach_idx].addr), Data, Size);
		#else
		isDmaDone = 0;
		HalGdmaMemCpy(&gdmaObj, (void*)(TxDesc[tx_serach_idx].addr), (void*)Data, Size);		
		while (isDmaDone == 0);
		#endif

		TxDesc[tx_serach_idx].dw1 &= TX_DESC_EOR;  // keep the EOR bit
		TxDesc[tx_serach_idx].dw1 |= TX_DESC_OWN | TX_DESC_FS | TX_DESC_LS | TX_DESC_CRC | Size;
		TxDesc[tx_serach_idx].dw2 = ((TX_DESC_VLAN_REMOVE << 25) & TX_DESC_VLAN_ACT_MASK) | (C_VLAN_HDR & TX_DESC_VLAN_TAG_MASK);
		CurrDataLen += Size;
	}
	else
	{
		/* Enable TxOK interrupt */
		HalEtherAdp.InterruptMask |= IMR_TXOK;
		HAL_MII_WRITE32(REG_MII_ISRIMR, HalEtherAdp.InterruptMask);
		/* Enable Tx ring1 */
		HAL_MII_WRITE32(REG_MII_IOCMD, (HAL_MII_READ32(REG_MII_IOCMD) | IOCMD_TXFN1ST));
		DBG_MII_WARN("Tx ring full\n");

		/* Upper layer will re-transmit this packet */
		return 0;
	}

	/* Check if reach to the end of the ring */
	if(tx_serach_idx == ((pEthAdapter->tx_desc_num) - 1))
		TxDescWrPtr = 0;
	else
		TxDescWrPtr++;


	return Size;
}


u32
HalMiiSendPacketRtl8195a(
	IN VOID
)
{
	u32 size = CurrDataLen;


	/* Enable TxOK interrupt */
	HalEtherAdp.InterruptMask |= IMR_TXOK;
	HAL_MII_WRITE32(REG_MII_ISRIMR, HalEtherAdp.InterruptMask);
	/* Enable Tx ring1 */
	HAL_MII_WRITE32(REG_MII_IOCMD, (HAL_MII_READ32(REG_MII_IOCMD) | IOCMD_TXFN1ST));
	CurrDataLen = 0;
	
	
	return size;
}


u32
HalMiiReceivePacketRtl8195a(
	IN VOID
)
{
	u8 rx_serach_idx = RxDescRdPtr;
	u32 rx_len = 0;


	/* Check if Rx descriptor is available */
	if((((volatile u32)RxDesc[rx_serach_idx].dw1) & RX_DESC_OWN) == 0)
	{
		rx_len = (RxDesc[rx_serach_idx].dw1) & RX_DESC_DATA_LEN_MASK;
	}
	else
	{
		/* Enable interrupt again */
		DBG_MII_INFO("RX ring empty\n\r");
		HalEtherAdp.InterruptMask |= (IMR_RXOK | ISR_RER_OVF);
		HAL_MII_WRITE32(REG_MII_ISRIMR, HalEtherAdp.InterruptMask);
		return 0;
	}

	return rx_len;
}


u32
HalMiiReadDataRtl8195a(
	IN u8 *Data,
	IN u32 Size
)
{
	PHAL_ETHER_ADAPTER pEthAdapter = (PHAL_ETHER_ADAPTER)&HalEtherAdp;
	u32 read_idx = RxDescRdPtr;


	if((Data == NULL) || (Size == 0) || (Size > 1536))
	{
		DBG_MII_ERR("Wrong params\n");
		return 0;
	}

	#if 1
	_memcpy((void*)Data, (void*)(RxDesc[read_idx].addr + 2), Size);
	#else
	isDmaDone = 0;
	HalGdmaMemCpy(&gdmaObj, (void*)Data, (void*)(RxDesc[read_idx].addr + 2), Size); 
	while (isDmaDone == 0);
	#endif
	
	RxDesc[read_idx].dw1 &= RX_DESC_EOR;  // keep the EOR bit
	RxDesc[read_idx].dw1 |= (RX_DESC_OWN | MII_BUF_SIZE);
	//RxDesc[read_idx].addr = (u32)(pRxDataBuf + (read_idx * MII_BUF_SIZE));
	RxDesc[read_idx].dw2 = 0;
	RxDesc[read_idx].dw3 = 0;

	/* Update the read pointer */
	if(read_idx == ((pEthAdapter->rx_desc_num) - 1))
		RxDescRdPtr = 0;
	else
		RxDescRdPtr++;

	
	return Size;
}


VOID
HalMiiGetMacAddressRtl8195a(
	IN u8 *Addr
)
{
	u8 *ptr;

// To do: Get info. from Flash or Efuse
	ptr = Addr;
	*(ptr)	 = 0x12;
	*(ptr+1) = 0x34;
	*(ptr+2) = 0x56;
	*(ptr+3) = 0x78;
	*(ptr+4) = 0x9A;
	*(ptr+5) = 0xBC;
}


u32
HalMiiGetLinkStatusRtl8195a(
	IN VOID
)
{
	u32 ret;


	if(HAL_MII_READ32(REG_MII_MS) & MS_LINKB)
		ret = 0;  // Link down
	else
		ret = 1;  // Link up
	
	return ret;
}


#if 0
VOID
HalMiiForceLinkRtl8195a(
	IN s32 Speed,
	IN s32 Duplex
)
{
	u32 regVal = 0;
	u16 ret;

	#if 0
	/* Reset PHY */
	HAL_MII_WRITE32(REG_MII_MIIA, MIIA_FLAG |
								((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) |
								((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK) |
								PHY_SW_RESET);
	HalDelayUs(1000);
	do {
	}while(HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG);

	while(1)
	{
		/* Check reset status */
		HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));
		HalDelayUs(1000);
		do {
		}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);

		regVal = HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF;
		if(!(regVal & PHY_SW_RESET))
			break;
	}
	DBG_MII_INFO("PHY reset is completed...\n");
	#endif
	/* Switch to Page 0 */
	ret = HalMiiRwPhyRegRtl8195a(1, 31, 0);

	if(((Speed == 1) && (Duplex == 1)) || ((Speed == 1) && (Duplex == 0)) || ((Speed == 0) && (Duplex == 1)) || ((Speed == 0) && (Duplex == 0)))
	{
		/* Disable Auto-negotiation */
		HAL_MII_WRITE32(REG_MII_MIIA, MIIA_FLAG |
									((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) |
									((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK) |
									0x0000);
		HalDelayUs(1000);
		do {
		}while(HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG);

		/* Manual settings */
		HAL_MII_WRITE32(REG_MII_MIIA, MIIA_FLAG |
									((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) |
									((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK) |
									(((Speed << 13) & PHY_SPEED_LSB) | ((Duplex << 8) & PHY_DUPLEX_MODE)));
		HalDelayUs(1000);
		do {
		}while(HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG);
	}
	else
	{		
		#if 1
		/* Reset PHY */
		HAL_MII_WRITE32(REG_MII_MIIA, MIIA_FLAG |
									((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) |
									((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK) |
									PHY_SW_RESET);
		HalDelayUs(1000);
		do {
		}while(HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG);

		while(1)
		{
			/* Check reset status */
			HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));
			HalDelayUs(1000);
			do {
			}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);

			regVal = HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF;
			if(!(regVal & PHY_SW_RESET))
				break;
		}
		DBG_MII_INFO("PHY reset is completed...\n");
		#endif
		while(1)
		{
			/* Check Auto-negotiation process status */
			HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG1_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));
			HalDelayUs(1000);
			do {
			}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);
		
			regVal = HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF;
			if(regVal & PHY_NWAY_COMPLETE)
				break;
		}
		DBG_MII_INFO("Auto-negotiation is completed...\n");
	}

	/* Wait until link is up */
	while(1)
	{
		HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG1_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));
		HalDelayUs(1000);
		do {
		}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);

		HalDelayUs(1000);

		HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG1_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));
		HalDelayUs(1000);
		do {
		}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);

		regVal = HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF;
		if(regVal & PHY_LINK_STATUS)
			break;
	}

	/* Get link info. */
	HAL_MII_WRITE32(REG_MII_MIIA, ((PHY_ADDRESS << 26) & MIIA_PHY_ADDR_MASK) | ((PHY_REG0_ADDR << 16) & MIIA_PHY_REG_ADDR_MASK));	
	HalDelayUs(1000);
	do {
	}while((HAL_MII_READ32(REG_MII_MIIA) & MIIA_FLAG) == 0);
	
	regVal = HAL_MII_READ32(REG_MII_MIIA) & 0xFFFF;
	
	if((!(regVal & PHY_SPEED_MSB)) && (regVal & PHY_SPEED_LSB))
	{
		if(regVal & PHY_DUPLEX_MODE)
			DBG_8195A("Link status: 100 Mb/s, Full duplex\n");
		else
			DBG_8195A("Link status: 100 Mb/s, Half duplex\n");
	}
	else if((!(regVal & PHY_SPEED_MSB)) && (!(regVal & PHY_SPEED_LSB)))
	{
		if(regVal & PHY_DUPLEX_MODE)
			DBG_8195A("Link status: 10 Mb/s, Full duplex\n");
		else
			DBG_8195A("Link status: 10 Mb/s, Half duplex\n");
	}

	HalDelayUs(4000000);
}
#else
VOID
HalMiiForceLinkRtl8195a(
	IN s32 Speed,
	IN s32 Duplex
)
{
	u16 ret;
	u16 i = 0;


	/* Switch to Page 0 */
	ret = HalMiiRwPhyRegRtl8195a(1, 31, 0);

	if(((Speed == 1) && (Duplex == 1)) || ((Speed == 1) && (Duplex == 0)) || ((Speed == 0) && (Duplex == 1)) || ((Speed == 0) && (Duplex == 0)))
	{
		/* Disable Auto-negotiation */
		ret = HalMiiRwPhyRegRtl8195a(1, 0, 0x0);
		/* Manual settings */
		ret = HalMiiRwPhyRegRtl8195a(1, 0, (((Speed << 13) & PHY_SPEED_LSB) | ((Duplex << 8) & PHY_DUPLEX_MODE)));
	}
	else
	{		
		/* Restart Auto-negotiation */
		ret = HalMiiRwPhyRegRtl8195a(1, 0, PHY_NWAY_EN | PHY_RESTART_NWAY);
		while(1)
		{
			/* Check the reset status */
			ret = HalMiiRwPhyRegRtl8195a(0, 0, 0);
			if(!(ret & PHY_RESTART_NWAY))
				break;
		}

		/* Check Auto-negotiation process status */
		while(1)
		{
			ret = HalMiiRwPhyRegRtl8195a(0, 1, 0);
			if(ret & PHY_NWAY_COMPLETE){
				ethernet_unplug = 0;
				break;
			}
			else
				RtlMsleepOS(100);
			if((i++ >= 20) && (ethernet_unplug == 0))
				ethernet_unplug = 1;
		}
		DBG_MII_INFO("Auto-negotiation is completed...\n");
	}	

	/* Wait until link is up */
	while(1)
	{
		ret = HalMiiRwPhyRegRtl8195a(0, 1, 0);
		HalDelayUs(1000);
		ret = HalMiiRwPhyRegRtl8195a(0, 1, 0);
		if(ret & PHY_LINK_STATUS)
			break;
	}

	/* Get link info. */
	ret = HalMiiRwPhyRegRtl8195a(0, 0, 0);
	if((!(ret & PHY_SPEED_MSB)) && (ret & PHY_SPEED_LSB))
	{
		if(ret & PHY_DUPLEX_MODE)
			DBG_8195A("Link status: 100 Mb/s, Full duplex\n");
		else
			DBG_8195A("Link status: 100 Mb/s, Half duplex\n");
	}
	else if((!(ret & PHY_SPEED_MSB)) && (!(ret & PHY_SPEED_LSB)))
	{
		if(ret & PHY_DUPLEX_MODE)
			DBG_8195A("Link status: 10 Mb/s, Full duplex\n");
		else
			DBG_8195A("Link status: 10 Mb/s, Half duplex\n");
	}

	HalDelayUs(4000000);
}
#endif

#ifdef CONFIG_MII_VERIFY
VOID MiiIrqHandle (IN VOID *Data);

VOID MiiIrqHandle (IN VOID *Data) {
	u32 RegValue = HalMiiGmacGetInterruptStatusRtl8195a();
	extern volatile u8 isRxOK;
	extern volatile u8 isTxOK;
	extern volatile u8 RxIntCnt;
	

//	DBG_8195A("ISR = 0x%08X\n", RegValue);
	if(RegValue & GMAC_ISR_ROK) {
		HalMiiGmacClearInterruptStatusRtl8195a(0x00410001);
		isRxOK = 1;
		RxIntCnt++;
	}

	if(RegValue & GMAC_ISR_TOK_TI) {
		HalMiiGmacClearInterruptStatusRtl8195a(0x00410040);
		isTxOK = 1;
	}
}


/**
 * MII Initialize.
 *
 * MII Initialize.
 *
 * Initialization Steps:
 *	 I. Rtl8195A Board Configurations:
 *	   1. MII Function Enable & AHB mux
 *
 * @return runtime status value.
 */
BOOL
HalMiiGmacInitRtl8195a(
		IN VOID *Data
		)
{
	u32 RegValue;


	/* 1. enable MII Pinmux & disable SDIO Host/Device mode Pinmux */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL);
	RegValue |= BIT24;
	RegValue &= ~(BIT0 | BIT1); // Important!
	HAL_WRITE32(PERI_ON_BASE, REG_HCI_PINMUX_CTRL, RegValue);

	/* 2. enable MII IP block (214, 12) */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN);
	RegValue |= BIT12;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN, RegValue);

	/* 3. Lexra2AHB Function Enable (304, 11) */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
	RegValue |= BIT11;
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, RegValue);

	/* 4. enable MII bus clock (240, 24|25) */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0);
	RegValue |= (BIT24 | BIT25);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_HCI_CLK_CTRL0, RegValue);

	/* 5. */
	RegValue = HAL_READ32(SYSTEM_CTRL_BASE, 0x74) & 0xFFFFC7FF;
	HAL_WRITE32(SYSTEM_CTRL_BASE, 0x74, (RegValue | 0x00003000));

	/* 6. AHB mux: select MII (214, 13) */
		RegValue = HAL_READ32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN);
	RegValue |= BIT13;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_HCI_COM_FUNC_EN, RegValue);

	/* 7. Vendor Register Clock Enable (230, 6|7) */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_CLK_CTRL);
	RegValue |= (BIT6 | BIT7);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_CLK_CTRL, RegValue);

	/* 8. Enable GMAC Lexra Timeout (090, 16|17|18) */
	RegValue = HAL_READ32(VENDOR_REG_BASE, 0x0090);
	RegValue |= (BIT16 | BIT17 | BIT18);
	HAL_WRITE32(VENDOR_REG_BASE, 0x0090, RegValue);

	/* 9. Endian Swap Control (304, 12|13) */
	RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
	RegValue |= (BIT12 | BIT13);
	HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, RegValue);

	return _TRUE;
}


BOOL
HalMiiGmacResetRtl8195a(
		IN VOID *Data
		)
{
	HAL_MII_WRITE32(REG_RTL_MII_CR, (HAL_MII_READ32(REG_RTL_MII_CR) | BIT0));

	return _TRUE;
}


BOOL
HalMiiGmacEnablePhyModeRtl8195a(
		IN VOID *Data
		)
{
	return _TRUE;
}


u32
HalMiiGmacXmitRtl8195a(
		IN VOID *Data
		)
{
	return 0;
}


VOID
HalMiiGmacCleanTxRingRtl8195a(
		IN VOID *Data
		)
{
}


VOID
HalMiiGmacFillTxInfoRtl8195a(
		IN VOID *Data
		)
{
	PMII_ADAPTER pMiiAdapter = (PMII_ADAPTER) Data;
	PTX_INFO pTx_Info = pMiiAdapter->pTx_Info;
	VOID* TxBuffer	= pMiiAdapter->TxBuffer;


	pTx_Info->opts1.dw = 0xBC8001FE;
	/* pTx_Info->opts1.dw = 0xBC800080; // size: 128 */

	pTx_Info->addr	   = (u32)TxBuffer;
	pTx_Info->opts2.dw = 0x0400279F;
	pTx_Info->opts3.dw = 0x00000000;
	/* pTx_Info->opts4.dw = 0x57800000; */
	pTx_Info->opts4.dw = 0x1FE00000;

	HAL_MII_WRITE32(REG_RTL_MII_TXFDP1, pTx_Info);
}


VOID
HalMiiGmacFillRxInfoRtl8195a(
		IN VOID *Data
		)
{
	PMII_ADAPTER pMiiAdapter = (PMII_ADAPTER) Data;
	PRX_INFO pRx_Info = pMiiAdapter->pRx_Info;
	VOID* RxBuffer	  = pMiiAdapter->RxBuffer;


	/* pRx_Info->opts1.dw = 0x80000200; //Data Length: 4095(FFF), 512(200) */
	pRx_Info->opts1.dw = 0x800001FC; //Data Length: 4095(FFF), 512(200)
	/* pRx_Info->opts1.dw = 0x8000007F; */

	pRx_Info->addr	   = (u32)RxBuffer;
	pRx_Info->opts2.dw = 0x00000000;
	pRx_Info->opts3.dw = 0x00000000;

	HAL_MII_WRITE32(REG_RTL_MII_RXFDP1, pRx_Info);
}


VOID
HalMiiGmacTxRtl8195a(
		IN VOID *Data
		)
{
	u32 RegValue;


	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD);
	RegValue |= BIT_IOCMD_TXENABLE(1);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD, RegValue);

	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD);
	RegValue |= BIT_IOCMD_FIRST_DMATX_ENABLE(1);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD, RegValue);
}


VOID
HalMiiGmacRxRtl8195a(
		IN VOID *Data
		)
{
	u32 RegValue;


	RegValue = HAL_MII_READ32(REG_RTL_MII_TCR);

	HAL_MII_WRITE32(REG_RTL_MII_TCR, 0x00000D00);  // loopback R2T mode

	RegValue = HAL_MII_READ32(REG_RTL_MII_RCR);
	HAL_MII_WRITE32(REG_RTL_MII_RCR, 0x0000007F);

	RegValue = HAL_MII_READ32(REG_RTL_MII_ETNRXCPU1);
	HAL_MII_WRITE32(REG_RTL_MII_ETNRXCPU1, 0x1F0A0F00);

	RegValue = HAL_MII_READ32(REG_RTL_MII_RX_PSE1);
	HAL_MII_WRITE32(REG_RTL_MII_RX_PSE1, 0x00000022);

	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD1);
	RegValue |= BIT_IOCMD1_FIRST_DMARX_ENABLE(1);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD1, RegValue);

	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD);
	RegValue |= BIT_IOCMD_RXENABLE(1);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD, RegValue);
}


VOID
HalMiiGmacSetDefaultEthIoCmdRtl8195a(
		IN VOID *Data
		)
{
	u32 RegValue;


	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD, CMD_CONFIG);

	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD1);
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD1, CMD1_CONFIG);

	//2014-04-29 yclin (disable 0x40051438[27] r_en_precise_dma) {
	RegValue = HAL_MII_READ32(REG_RTL_MII_IOCMD1);
	RegValue = RegValue & 0xF7FFFFFF;
	HAL_MII_WRITE32(REG_RTL_MII_IOCMD1, RegValue);
	// }
}


VOID
HalMiiGmacInitIrqRtl8195a(
		IN VOID *Data
		)
{
	IRQ_HANDLE	MiiIrqHandle_Master;
	PMII_ADAPTER pMiiAdapter = (PMII_ADAPTER) Data;


	MiiIrqHandle_Master.Data	 = (u32) (pMiiAdapter);
	MiiIrqHandle_Master.IrqNum	 = GMAC_IRQ;
	MiiIrqHandle_Master.IrqFun	 = (IRQ_FUN) MiiIrqHandle;
	MiiIrqHandle_Master.Priority = 10;
	InterruptRegister(&MiiIrqHandle_Master);
	InterruptEn(&MiiIrqHandle_Master);
}


u32
HalMiiGmacGetInterruptStatusRtl8195a(
		VOID
		)
{
	u32 RegValue;


	RegValue = HAL_MII_READ32(REG_RTL_MII_IMRISR);

	return RegValue;
}


VOID
HalMiiGmacClearInterruptStatusRtl8195a(
		u32 IsrStatus
		)
{
	HAL_MII_WRITE32(REG_RTL_MII_IMRISR, IsrStatus);
}
#endif	// #ifdef CONFIG_MII_VERIFY


