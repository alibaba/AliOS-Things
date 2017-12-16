///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_device_cfg.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "app_config.h"
#include "gpio.h"
#include "uart.h"

#ifdef FUNC_BT_EN
//
//配置蓝牙芯片的GPIO复用关系
//
void BTDevicePinCFG(void)
{
uint8_t BtBbType = (uint8_t)BT_DEVICE_TYPE;
    
	GpioClk32kIoConfig(1);
#ifdef FUNC_BT_HF_EN
	GpioPcmSyncIoConfig(1);
#endif
	GpioBuartRxIoConfig(3);
	GpioBuartTxIoConfig(3);

    if(BtBbType == (uint8_t)(BTUartDeviceMTK662X))
    {
        //no rts pin is used. DO NOT modify this if you DO NOT understand what you are doing. @ yzhou 
    }
    else
    {
		GpioBuartRtsIoConfig(BT_UART_RTS_PORT);
    }

	BuartIOctl(BUART_IOCTL_RXRTS_FLOWCTL_SET, 2);
	BuartExFifoInit(BUART_RX_TX_FIFO_ADDR - PMEM_ADDR, BUART_RX_FIFO_SIZE, BUART_TX_FIFO_SIZE, 1);

}
#endif

//
//蓝牙芯片的Reset PIN/LDOEN PIN,拉高拉低，用于蓝牙设备POR或RESET
//
void BtLDOEn(bool Flag) // BtLDOEn Flag  Do not delete.
{
	uint8_t	GpioPort;

	switch(BT_LDOEN_RST_GPIO_PORT)
	{
		case GPIO_PORT_A:
			GpioPort = GPIO_A_IN;
			break;
		case GPIO_PORT_B:
			GpioPort = GPIO_B_IN;
			break;
		case GPIO_PORT_C:
			GpioPort = GPIO_C_IN;
			break;
		default:
			return;
	}

	if(Flag)
	{
		GpioClrRegOneBit(GpioPort + 2, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
		GpioSetRegOneBit(GpioPort + 3, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
		GpioSetRegOneBit(GpioPort + 1, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	}
	else
	{
		GpioClrRegOneBit(GpioPort + 2, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
		GpioSetRegOneBit(GpioPort + 3, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
		GpioClrRegOneBit(GpioPort + 1, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	}
}

//
//brief  Power off bt device
//return None
//
void BTDevicePowerOff(void)
{
	uint8_t	GpioPort;

	switch(BT_LDOEN_RST_GPIO_PORT)
	{
		case GPIO_PORT_A:
			GpioPort = GPIO_A_IN;
			break;
		case GPIO_PORT_B:
			GpioPort = GPIO_B_IN;
			break;
		case GPIO_PORT_C:
			GpioPort = GPIO_C_IN;
			break;
		default:
			return;
	}

	GpioClrRegOneBit(GpioPort + 2, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	GpioSetRegOneBit(GpioPort + 3, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	GpioClrRegOneBit(GpioPort + 1, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
}

//
// @brief  Power on bt device
// @return None
//
void BTDevicePowerOn(void)
{
	uint8_t	GpioPort;

	switch(BT_LDOEN_RST_GPIO_PORT)
	{
		case GPIO_PORT_A:
			GpioPort = GPIO_A_IN;
			break;
		case GPIO_PORT_B:
			GpioPort = GPIO_B_IN;
			break;
		case GPIO_PORT_C:
			GpioPort = GPIO_C_IN;
			break;
		default:
			return;
	}

	GpioClrRegOneBit(GpioPort + 2, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	GpioSetRegOneBit(GpioPort + 3, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
	GpioSetRegOneBit(GpioPort + 1, ((uint32_t)1 << BT_LDOEN_RST_GPIO_PIN));
}

/////////////////for MTK ONLY////////////////////////////////////////
#ifdef AP80_DEV_BOARD
void BtMtkLDOEn(bool EnFlag) // BtMtkLDOEn Flag, for mtk usage ONLY  Do not delete.
{
	GpioClrRegOneBit(GPIO_B_IE, MTK_BT_PWR_EN);
	GpioSetRegOneBit(GPIO_B_OE, MTK_BT_PWR_EN); //tx low
	GpioSetRegOneBit(GPIO_B_PD, MTK_BT_PWR_EN); //pull down  
	GpioClrRegOneBit(GPIO_B_PU, MTK_BT_PWR_EN); //clear pu
	 
	if(EnFlag)
	{
		GpioSetRegOneBit(GPIO_B_OUT, MTK_BT_PWR_EN);
	}
	else
	{
		GpioClrRegOneBit(GPIO_B_OUT, MTK_BT_PWR_EN);
	}
}

void BtMtkRestEn(bool EnFlag) // BtMtkRestEn Flag, for mtk usage ONLY  Do not delete.
{
	GpioClrRegOneBit(GPIO_B_IE, MTK_BT_RESET);
	GpioSetRegOneBit(GPIO_B_OE, MTK_BT_RESET); //tx low
	GpioSetRegOneBit(GPIO_B_PD, MTK_BT_RESET); //pull down  
	GpioClrRegOneBit(GPIO_B_PU, MTK_BT_RESET); //clear pu

	if(EnFlag)
	{
		GpioSetRegOneBit(GPIO_B_OUT, MTK_BT_RESET);
	}
	else
	{
		GpioClrRegOneBit(GPIO_B_OUT, MTK_BT_RESET);
	}        
}
#else
void BtMtkLDOEn(bool EnFlag) // BtMtkLDOEn Flag, for mtk usage ONLY  Do not delete.
{
	GpioClrRegOneBit(GPIO_C_IE, MTK_BT_PWR_EN);
	GpioSetRegOneBit(GPIO_C_OE, MTK_BT_PWR_EN); //tx low
	GpioSetRegOneBit(GPIO_C_PD, MTK_BT_PWR_EN); //pull down  
	GpioClrRegOneBit(GPIO_C_PU, MTK_BT_PWR_EN); //clear pu
	 
	if(EnFlag)
	{
		GpioSetRegOneBit(GPIO_C_OUT, MTK_BT_PWR_EN);
	}
	else
	{
		GpioClrRegOneBit(GPIO_C_OUT, MTK_BT_PWR_EN);
	}
}

void BtMtkRestEn(bool EnFlag) // BtMtkRestEn Flag, for mtk usage ONLY  Do not delete.
{
	GpioClrRegOneBit(GPIO_B_IE, MTK_BT_RESET);
	GpioSetRegOneBit(GPIO_B_OE, MTK_BT_RESET); //tx low
	GpioSetRegOneBit(GPIO_B_PD, MTK_BT_RESET); //pull down  
	GpioClrRegOneBit(GPIO_B_PU, MTK_BT_RESET); //clear pu

	if(EnFlag)
	{
		GpioSetRegOneBit(GPIO_B_OUT, MTK_BT_RESET);
	}
	else
	{
		GpioClrRegOneBit(GPIO_B_OUT, MTK_BT_RESET);
	}        
}

#endif

void BTMtkDevicePowerOff(void)
{
	BtMtkLDOEn(0);
}

