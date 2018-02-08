/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"

#ifdef CONFIG_MII_EN

#include "hal_mii.h"

HAL_ETHER_ADAPTER HalEtherAdp;



s32
HalMiiInit(
	IN VOID
)
{
	if (FunctionChk(MII, S0) == _FALSE)
		return HAL_ERR_UNKNOWN;
	else
		return HalMiiInitRtl8195a();
}


VOID
HalMiiDeInit(
	IN VOID
)
{
	HalMiiDeInitRtl8195a();
}


s32
HalMiiWriteData(
	IN const char *Data,
	IN u32 Size
)
{
	return HalMiiWriteDataRtl8195a(Data, Size);
}


u32
HalMiiSendPacket(
	IN VOID
)
{
	return HalMiiSendPacketRtl8195a();
}


u32
HalMiiReceivePacket(
	IN VOID
)
{
	return HalMiiReceivePacketRtl8195a();
}


u32
HalMiiReadData(
	IN u8 *Data,
	IN u32 Size
)
{
	return HalMiiReadDataRtl8195a(Data, Size);
}


VOID
HalMiiGetMacAddress(
	IN u8 *Addr
)
{
	HalMiiGetMacAddressRtl8195a(Addr);
}


u32
HalMiiGetLinkStatus(
	IN VOID
)
{
	return HalMiiGetLinkStatusRtl8195a();
}


VOID
HalMiiForceLink(
	IN s32 Speed,
	IN s32 Duplex
)
{
	HalMiiForceLinkRtl8195a(Speed, Duplex);
}


#ifdef CONFIG_MII_VERIFY
VOID
HalMiiOpInit(
    IN VOID *Data
)
{
    PHAL_MII_OP pHalMiiOp = (PHAL_MII_OP) Data;


    pHalMiiOp->HalMiiGmacInit          = HalMiiGmacInitRtl8195a;
    pHalMiiOp->HalMiiGmacReset         = HalMiiGmacResetRtl8195a;
    pHalMiiOp->HalMiiGmacEnablePhyMode = HalMiiGmacEnablePhyModeRtl8195a;
    pHalMiiOp->HalMiiGmacXmit          = HalMiiGmacXmitRtl8195a;
    pHalMiiOp->HalMiiGmacCleanTxRing   = HalMiiGmacCleanTxRingRtl8195a;
    pHalMiiOp->HalMiiGmacFillTxInfo    = HalMiiGmacFillTxInfoRtl8195a;
    pHalMiiOp->HalMiiGmacFillRxInfo    = HalMiiGmacFillRxInfoRtl8195a;
    pHalMiiOp->HalMiiGmacTx            = HalMiiGmacTxRtl8195a;
    pHalMiiOp->HalMiiGmacRx            = HalMiiGmacRxRtl8195a;
    pHalMiiOp->HalMiiGmacSetDefaultEthIoCmd   = HalMiiGmacSetDefaultEthIoCmdRtl8195a;
    pHalMiiOp->HalMiiGmacInitIrq       = HalMiiGmacInitIrqRtl8195a;
    pHalMiiOp->HalMiiGmacGetInterruptStatus   = HalMiiGmacGetInterruptStatusRtl8195a;
    pHalMiiOp->HalMiiGmacClearInterruptStatus = HalMiiGmacClearInterruptStatusRtl8195a;
}
#endif  // #ifdef CONFIG_MII_VERIFY

#endif  // #ifdef CONFIG_MII_EN


