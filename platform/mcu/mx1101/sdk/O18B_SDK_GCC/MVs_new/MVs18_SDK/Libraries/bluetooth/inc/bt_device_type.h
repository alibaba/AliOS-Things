/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			bt_device_type.h
* Description:		bt_device_type function h file.
* maintainer: Halley
******************************************************************************/
#ifndef __BT_DEVICE_TYPE_H__
#define __BT_DEVICE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
enum BtDeviceType
{
    BtNoneDevice = 0,
    BTUsbDevice,
    BTUartDeviceRda5875,
    BTUartDeviceCsrBC6,
    BTUartDeviceBcm20702,
    BTUartDeviceMTK662X,
    BTUartDeviceRTL8761,
};

typedef enum
{
    GPIO_PORT_A = 1, /**< Use GPIOA for LDOEN/RST */
    GPIO_PORT_B,	 /**< Use GPIOB for LDOEN/RST */
    GPIO_PORT_C	     /**< Use GPIOC for LDOEN/RST */
} GPIO_PORT_SEL;

//#define  BUART_RX_FIFO_SIZE   (1024*6)
//#define  BUART_TX_FIFO_SIZE   (1024)


//
//配置蓝牙芯片的GPIO复用关系
//
void BTDevicePinCFG(void);

//
//初始化蓝牙设备，BtDeviceType 为蓝牙芯片类型枚举值
//
bool BTDeviceInit(uint8_t BTDeviceType, int8_t* BdAdd/*6Bytes, LE*/);

//
//关闭蓝牙硬件模块
//
void BTDevicePowerOff(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
