/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "ethernet_api.h"
#include "ethernet_ex_api.h"
#include "hal_mii.h"

#if DEVICE_ETHERNET

#if CONFIG_MII_EN

extern HAL_ETHER_ADAPTER HalEtherAdp;



void ethernet_irq_hook(ethernet_callback callback)
{
	HalEtherAdp.CallBack = callback;
}


void ethernet_set_descnum(uint8_t txdescCnt, uint8_t rxdescCnt)
{
	HalEtherAdp.tx_desc_num = txdescCnt;
	HalEtherAdp.rx_desc_num = rxdescCnt;
}


void ethernet_trx_pre_setting(uint8_t *TxDescAddr, uint8_t *RxDescAddr, uint8_t *pTxPktBuf, uint8_t *pRxPktBuf)
{
	HalEtherAdp.TxDescAddr = TxDescAddr;
	HalEtherAdp.RxDescAddr = RxDescAddr;
	HalEtherAdp.pTxPktBuf = pTxPktBuf;
	HalEtherAdp.pRxPktBuf = pRxPktBuf;
}


int ethernet_init(void)
{
	return HalMiiInit();
}


void ethernet_free(void)
{
	HalMiiDeInit();
}


int ethernet_write(const char *data, int size)
{
	return HalMiiWriteData(data, size);
}


int ethernet_send(void)
{
	return HalMiiSendPacket();
}


int ethernet_receive(void)
{
	return HalMiiReceivePacket();
}


int ethernet_read(char *data, int size)
{
	return HalMiiReadData((u8*)data, size);
}


void ethernet_address(char *mac)
{
	HalMiiGetMacAddress((u8*)mac);
}


int ethernet_link(void)
{
	int ret;


	ret = HalMiiGetLinkStatus();

	return ret;
}


void ethernet_set_link(int speed, int duplex)
{
	HalMiiForceLink(speed, duplex);
}

#endif  // #if CONFIG_MII_EN
#endif  // #if DEVICE_ETHERNET

